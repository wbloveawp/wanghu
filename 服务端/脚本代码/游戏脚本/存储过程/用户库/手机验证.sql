
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_AcquireAuthCode]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_AcquireAuthCode]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_BindEMail]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_BindEMail]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_UnBindEMail]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_UnBindEMail]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_BindMobilePhone]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_BindMobilePhone]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_UnBindMobilePhone]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_UnBindMobilePhone]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_ResetLogonPasswd]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_ResetLogonPasswd]
GO

--IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_ReceiptAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
--DROP PROCEDURE [dbo].[GSP_MB_ReceiptAccounts]
--GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 获取验证码
CREATE PROC GSP_MB_AcquireAuthCode
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@strMachineID NVARCHAR(32),					-- 机器序列
	@strClientIP NVARCHAR(15),					-- 连接地址
	@cbCodeKind	 TINYINT,						-- 验证码类型
	@cbAuthKind TINYINT,						-- 验证类型		
	@dwBindUserID INT,							-- 绑定标识	
	@strMobileEmail NVARCHAR(64),				-- 手机邮箱
	@strAuthCode NCHAR(6),						-- 验证码
	@strAuthCodeMD5 NCHAR(32),					-- 验证码	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 时间间隔
DECLARE @DateDiff INT
DECLARE @SecondDiff INT

-- 标识定义
DECLARE @BindUserID INT
DECLARE @BindAccounts NVARCHAR(31)

-- 执行逻辑
BEGIN	
	
	-- 校验标识
	IF @dwUserID<>0
	BEGIN
		-- 查询用户
		DECLARE @UserID INT
		DECLARE @Nullity BIT
		DECLARE @ShutDown BIT	
		DECLARE @RegisterMode INT
		DECLARE @LogonPass NCHAR(32)
		DECLARE @EMail NVARCHAR(64)
		DECLARE @MobilePhone NVARCHAR(21)				
		SELECT @UserID=a.UserID, @LogonPass=a.LogonPass, @RegisterMode=a.RegisterMode, @Nullity=a.Nullity, @EMail=a.EMail, @MobilePhone=a.MobilePhone, @ShutDown=b.ShutDownStatus
		FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

		-- 查询用户
		IF @UserID IS NULL
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
			RETURN 10
		END	

		-- 帐号禁止
		IF @Nullity<>0
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
			RETURN 11
		END	

		-- 帐号关闭
		IF @ShutDown<>0
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
			RETURN 12
		END	
	
		-- 密码判断	
		IF @LogonPass<>@strPassword
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
			RETURN 13
		END	

		-- 变量定义
		DECLARE @strSubString NVARCHAR(31)

		-- 绑定手机号
		IF @cbAuthKind=1
		BEGIN
			-- 校验手机号
			--IF @MobilePhone<>N''
			--BEGIN
			--	-- 设置提示
			--	SET @strErrorDescribe=N'{"Idx":1055,"Msg":"您的账号已绑定尾号为：'+SUBSTRING(@MobilePhone,LEN(@MobilePhone)-3,4)+'的手机号，如需更换手机号,请先解除绑定！","Ops":["'+SUBSTRING(@MobilePhone,LEN(@MobilePhone)-3,4)+'"]}'
			--	RETURN 14
			--END

			-- 查询账号			
			SELECT @BindUserID=UserID,@BindAccounts=NickName FROM AccountsInfo(NOLOCK) WHERE MobilePhone=@strMobileEmail
			IF @BindUserID IS NOT NULL AND @BindUserID<>@dwBindUserID
			--IF @BindUserID IS NOT NULL
			BEGIN
				SET @strErrorDescribe=N'{"Idx":1067,"Msg":"抱歉，该手机号已经和其他游戏账号绑定，请更换新的手机号绑定！"}'
				RETURN 14		

				-- 账号保护
				--IF LEN(@BindAccounts)>3
				--BEGIN
				--	SET @strSubString=SUBSTRING(@BindAccounts,2,LEN(@BindAccounts)-3)

				--	 -- 替换字符
				--	SET @BindAccounts=REPLACE(@BindAccounts,@strSubString,REPLICATE(N'*',LEN(@strSubString)))			
				--END

				---- 设置提示字符
				--SET @strErrorDescribe=N'"Idx":1057,"Msg":"'+@strMobileEmail+N'已经跟游戏账号：'+@BindAccounts+'绑定，是否重新将手机绑定到当前帐号上。","Ops":["'+@strMobileEmail+'","'+@BindAccounts+'"]}'

				---- 抛出记录
				--SELECT @BindUserID AS BindUserID

				--RETURN 2											
			END
		END

		-- 解绑手机号
		IF @cbAuthKind=2 AND @MobilePhone<>@strMobileEmail
		BEGIN
			-- 设置提示
			SET @strErrorDescribe=N'{"Idx":1056,"Msg":"解除绑定需使用原手机号码，您输入的手机号码不匹配，请仔细核对后再次尝试！"}'
			RETURN 15
		END	

		-- 绑定邮箱
		IF @cbAuthKind=3
		BEGIN
			IF @Email<>N''
			BEGIN
				-- 设置提示
				SET @strErrorDescribe=N'{"Idx":1045,"Msg":"您的账号已绑定邮箱：'+@Email+'，如需更换请先解除绑定！","Ops":["'+@Email+'"]}'
				RETURN 14
			END

			-- 查询邮箱
			SELECT @BindUserID=UserID,@BindAccounts=NickName FROM AccountsInfo(NOLOCK) WHERE EMail=@strMobileEmail
			IF @BindUserID IS NOT NULL AND @BindUserID<>@dwBindUserID
			BEGIN
				-- 账号保护
				IF LEN(@BindAccounts)>3
				BEGIN					
					SET @strSubString=SUBSTRING(@BindAccounts,2,LEN(@BindAccounts)-3)

					 -- 替换字符
					SET @BindAccounts=REPLACE(@BindAccounts,@strSubString,REPLICATE(N'*',LEN(@strSubString)))			
				END

				-- 设置提示字符
				SET @strErrorDescribe=N'{"Idx":1048,"Msg":"'+@strMobileEmail+N'已经跟游戏账号：'+@BindAccounts+'绑定，是否重新将邮箱绑定到当前帐号上。","Ops":["'+@strMobileEmail+'","'+@BindAccounts+'"]}'

				-- 抛出记录
				SELECT @BindUserID AS BindUserID

				RETURN 2											
			END
		END

		-- 解绑邮箱
		IF @cbAuthKind=4 AND @Email<>@strMobileEmail
		BEGIN
			-- 设置提示
			SET @strErrorDescribe=N'{"Idx":1046,"Msg":"解除绑定需使用原绑定邮箱，您输入的邮箱不匹配，请仔细核对后再次尝试！"}'
			RETURN 15
		END	

		-- 货币转账
		IF @cbAuthKind=10
		BEGIN
			-- 校验手机号
			IF @cbCodeKind=1 AND @MobilePhone<>@strMobileEmail
			BEGIN
				-- 设置提示
				SET @strErrorDescribe=N'{"Idx":1053,"Msg":"转账需使用游戏账号绑定的手机号，您输入的手机号码不匹配，请仔细核对后再次尝试！"}'
				RETURN 16
			END

			-- 校验邮箱
			IF @cbCodeKind=2 AND @Email<>@strMobileEmail
			BEGIN
				-- 设置提示
				SET @strErrorDescribe=N'{"Idx":1047,"Msg":"转账需使用游戏账号绑定的邮箱，您的邮箱不匹配，请仔细核对后再次尝试！"}'
				RETURN 16
			END
		END	

		-- 绑定银行账号
		IF @cbAuthKind=11
		BEGIN
			-- 校验手机号
			IF @cbCodeKind=1 AND @MobilePhone<>@strMobileEmail
			BEGIN
				-- 设置提示
				SET @strErrorDescribe=N'{"Idx":1083,"Msg":"绑定银行账号需使用游戏账号绑定的手机号，您输入的手机号码不匹配，请仔细核对后再次尝试！"}'
				RETURN 17
			END

			-- 校验邮箱
			IF @cbCodeKind=2 AND @Email<>@strMobileEmail
			BEGIN
				-- 设置提示
				SET @strErrorDescribe=N'{"Idx":1084,"Msg":"绑定银行账号需使用游戏账号绑定的邮箱，您的邮箱不匹配，请仔细核对后再次尝试！"}'
				RETURN 17
			END
		END
		
	END ELSE
	BEGIN
		-- 变量定义
		DECLARE @DateID INT
		DECLARE @AuthTimes SMALLINT		

		-- 计算日期			
		SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

		-- 重置密码
		IF @cbAuthKind=5
		BEGIN		
			-- 手机验证码
			IF @cbCodeKind=1
			BEGIN
				-- 查询账号
				SELECT @BindUserID=UserID FROM AccountsInfo(NOLOCK) WHERE MobilePhone=@strMobileEmail
				IF @BindUserID IS NULL
				BEGIN
					-- 设置提示
					SET @strErrorDescribe=N'{"Idx":1058,"Msg":"该手机号没有绑定的游戏账号，请仔细核对手机号再次尝试！"}'

					RETURN 16
				END	
			END

			-- 邮箱验证码
			IF @cbCodeKind=2
			BEGIN
				-- 查询账号
				SELECT @BindUserID=UserID FROM AccountsInfo(NOLOCK) WHERE EMail=@strMobileEmail
				IF @BindUserID IS NULL
				BEGIN
					-- 设置提示
					SET @strErrorDescribe=N'{"Idx":1044,"Msg":"该邮箱没有绑定的游戏账号，请仔细核对邮箱后再次尝试！"}'

					RETURN 16
				END	
			END

			-- 读取记录
			SELECT @AuthTimes=AuthTimes FROM RecordAuthTimes(NOLOCK) WHERE DateID=@DateID AND UserID=@BindUserID AND CodeKind=@cbCodeKind AND AuthKind=@cbAuthKind

			-- 调整次数
			IF @AuthTimes IS NULL SET @AuthTimes=0

			-- 读取配置			
			DECLARE @ResetLogonPassTimes SMALLINT			
			SELECT @ResetLogonPassTimes=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ResetLogonPassTimes'

			-- 调整配置
			IF @ResetLogonPassTimes IS NULL SET @ResetLogonPassTimes=3		

			-- 校验次数
			IF @AuthTimes>=@ResetLogonPassTimes
			BEGIN
				-- 设置提示
				SET @strErrorDescribe=N'{"Idx":1059,"Msg":"您的游戏账号今天重置密码的次数已达到'+CONVERT(NVARCHAR(3),@ResetLogonPassTimes)+'次，为了您的帐号安全，已锁定验证。请明天再试！","Ops":["'+CONVERT(NVARCHAR(3),@ResetLogonPassTimes)+'"]}'

				RETURN 17
			END
		END	
		
		-- 验证码登陆
		IF @cbAuthKind=6
		BEGIN
			-- 读取记录
			SELECT @AuthTimes=AuthTimes FROM RecordAuthTimes(NOLOCK) WHERE DateID=@DateID AND UserID=@BindUserID AND CodeKind=@cbCodeKind AND  AuthKind=@cbAuthKind

			-- 调整次数
			IF @AuthTimes IS NULL SET @AuthTimes=0

			-- 读取配置			
			DECLARE @AuthCodeLogonTimes SMALLINT			
			SELECT @AuthCodeLogonTimes=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND  StatusName=N'AuthCodeLogonTimes'

			-- 调整配置
			IF @AuthCodeLogonTimes IS NULL SET @AuthCodeLogonTimes=10

			-- 校验次数
			IF @AuthTimes>=@AuthCodeLogonTimes
			BEGIN
				-- 设置提示
				IF @cbCodeKind=1 SET @strErrorDescribe=N'{"Idx":1061,"Msg":"您的手机号今天使用验证码登录的次数已达到'+CONVERT(NVARCHAR(3),@AuthCodeLogonTimes)+'次，为了您的帐号安全，已锁定验证。请明天再试！","Ops":["'+CONVERT(NVARCHAR(3),@AuthCodeLogonTimes)+'"]}'
				IF @cbCodeKind=2 SET @strErrorDescribe=N'{"Idx":1082,"Msg":"您的邮箱今天使用验证码登录的次数已达到'+CONVERT(NVARCHAR(3),@AuthCodeLogonTimes)+'次，为了您的帐号安全，已锁定验证。请明天再试！","Ops":["'+CONVERT(NVARCHAR(3),@AuthCodeLogonTimes)+'"]}'

				RETURN 17
			END
		END		
	END

	-- 调整变量
	IF @BindUserID IS NULL SET @BindUserID=0

	-- 系统参数
	DECLARE @ACValidSeconds INT
	DECLARE @MaxMPAuthTimes SMALLINT 
	DECLARE @MaxMPAcquireTimes SMALLINT
	DECLARE @MaxATAuthTimes SMALLINT 
	DECLARE @MaxATAcquireTimes SMALLINT

	-- 读取参数
	SELECT @ACValidSeconds=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ACValidSecond'
	SELECT @MaxMPAuthTimes=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND  StatusName=N'MaxMPAuthTimes'
	SELECT @MaxMPAcquireTimes=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'MaxMPAcquireTimes'
	SELECT @MaxATAuthTimes=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND  StatusName=N'MaxATAuthTimes'
	SELECT @MaxATAcquireTimes=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'MaxATAcquireTimes'

	-- 调整参数
	IF @ACValidSeconds IS NULL SET @ACValidSeconds=600
	IF @MaxMPAuthTimes IS NULL SET @MaxMPAuthTimes=10
	IF @MaxMPAcquireTimes IS NULL SET @MaxMPAuthTimes=5
	IF @MaxATAuthTimes IS NULL SET @MaxMPAuthTimes=10
	IF @MaxATAcquireTimes IS NULL SET @MaxMPAuthTimes=5

	-- 账号验证记录
	DECLARE @ATAuthDate DATETIME
	DECLARE @ATAuthTimes SMALLINT		
	DECLARE @ATAcquireTimes SMALLINT		

	-- 手机号验证记录
	DECLARE @MPAuthDate DATETIME
	DECLARE @MPAuthKind SMALLINT	
	DECLARE @MPAuthTimes SMALLINT	
	DECLARE @MPAuthCode NVARCHAR(32)				
	DECLARE @MPAcquireTimes SMALLINT	

	-- 查询信息
	SELECT @ATAuthTimes=AuthTimes,@ATAcquireTimes=AcquireTimes,@ATAuthDate=LastAcquireDate FROM AccountsAuthInfo(NOLOCK) WHERE UserID=@dwUserID	AND CodeKind=@cbCodeKind
	
	-- 记录不存在
	IF @ATAuthTimes IS NULL
	BEGIN
		-- 验证码记录				
		SELECT @MPAuthTimes=AuthTimes,@MPAuthKind=AuthKind,@MPAuthCode=AuthCode,@MPAcquireTimes=AcquireTimes,@MPAuthDate=LastAcquireDate FROM RecordAuthCode(NOLOCK) WHERE CodeKind=@cbCodeKind AND MobileEmail=@strMobileEmail

		-- 记录存在
		IF @MPAuthTimes IS NULL
		BEGIN
			-- 插入记录
			INSERT INTO RecordAuthCode(CodeKind,MobileEmail,UserID,AuthKind,AuthCode,AuthCodeMD5,AuthTimes,AcquireTimes,AcquireClientIP,AcquireMachine,LastAcquireDate)										
			VALUES (@cbCodeKind,@strMobileEmail,@dwUserID,@cbAuthKind,@strAuthCode,@strAuthCodeMD5,0,1,@strClientIP,@strMachineID,GetDate())

		END ELSE
		BEGIN
			-- 计算间隔
			SET @DateDiff=DateDiff(day,@MPAuthDate,GetDate())
			SET @SecondDiff=DateDiff(s,@MPAuthDate,GetDate())			

			-- 次数限制	
			IF @DateDiff=0 AND @MPAuthTimes>=@MaxMPAuthTimes
			BEGIN		
				IF @cbCodeKind=1 SET @strErrorDescribe=N'{"Idx":1062,"Msg":"该手机号今天的验证次数已超过'+CONVERT(NVARCHAR(3),@MaxMPAuthTimes)+'次，为了您的帐号安全，已锁定验证。请明天再试！","Ops":["'+CONVERT(NVARCHAR(3),@MaxMPAuthTimes)+'"]}'
				IF @cbCodeKind=2 SET @strErrorDescribe=N'{"Idx":1077,"Msg":"该邮箱今天的验证次数已超过'+CONVERT(NVARCHAR(3),@MaxMPAuthTimes)+'次，为了您的帐号安全，已锁定验证。请明天再试！","Ops":["'+CONVERT(NVARCHAR(3),@MaxMPAuthTimes)+'"]}'
				RETURN 20						
			END

			-- 次数限制	
			IF @DateDiff=0 AND @MPAcquireTimes>=@MaxMPAcquireTimes
			BEGIN	
				IF @cbCodeKind=1 SET @strErrorDescribe=N'{"Idx":1063,"Msg":"该手机号今天的验证请求次数已超过'+CONVERT(NVARCHAR(3),@MaxMPAcquireTimes)+'次，为了您的帐号安全，已锁定验证。请更换手机号码或明天再试！","Ops":["'+CONVERT(NVARCHAR(3),@MaxMPAcquireTimes)+'"]}'
				IF @cbCodeKind=2 SET @strErrorDescribe=N'{"Idx":1078,"Msg":"该邮箱今天的验证请求次数已超过'+CONVERT(NVARCHAR(3),@MaxMPAcquireTimes)+'次，为了您的帐号安全，已锁定验证。请更换邮箱或明天再试！","Ops":["'+CONVERT(NVARCHAR(3),@MaxMPAcquireTimes)+'"]}'
				RETURN 21						
			END

			-- 时间限制
			IF @MPAuthCode<>N'' AND @SecondDiff<@ACValidSeconds	
			BEGIN			
				-- 更新类型
				IF @MPAuthKind<>@cbAuthKind
				BEGIN
					UPDATE RecordAuthCode SET AuthKind=@cbAuthKind WHERE CodeKind=@cbCodeKind AND MobileEmail=@strMobileEmail
				END

				-- 提示信息
				IF @cbCodeKind=1 SET @strErrorDescribe=N'{"Idx":1064,"Msg":"手机验证码已成功发送至尾号为：'+SUBSTRING(@strMobileEmail,LEN(@strMobileEmail)-3,4)+'的手机。请留意短信通知。","Ops":["'+SUBSTRING(@strMobileEmail,LEN(@strMobileEmail)-3,4)+'"]}'
				IF @cbCodeKind=2 SET @strErrorDescribe=N'{"Idx":1079,"Msg":"验证码已成功发送至邮箱：'+@strMobileEmail+'。请留意邮件通知。","Ops":["'+@strMobileEmail+'"]}'

				-- 抛出标识
				SELECT @BindUserID AS BindUserID

				RETURN 3
			END			

			-- 调整次数
			IF @DateDiff<>0 SELECT @MPAuthTimes=0,@MPAcquireTimes=0

			-- 更新记录
			UPDATE RecordAuthCode SET UserID=@dwUserID,AuthKind=@cbAuthKind,AuthCode=@strAuthCode,AuthCodeMD5=@strAuthCodeMD5,AuthTimes=@MPAuthTimes,AcquireTimes=@MPAcquireTimes+1,
			AcquireClientIP=@strClientIP,AcquireMachine=@strMachineID,LastAcquireDate=GetDate() WHERE CodeKind=@cbCodeKind AND MobileEmail=@strMobileEmail
		END			
				
		-- 插入记录
		IF @dwUserID<>0
		BEGIN
			INSERT INTO AccountsAuthInfo(UserID,CodeKind,AuthTimes,AcquireTimes,LastAcquireDate,LastMobileEmail) VALUES (@dwUserID,@cbCodeKind,0,1,GetDate(),@strMobileEmail)
		END

	END ELSE
	BEGIN	
		-- 计算间隔
		SET @DateDiff=DateDiff(day,@ATAuthDate,GetDate())		

		-- 次数限制	
		IF @DateDiff=0 AND @ATAuthTimes>=@MaxATAuthTimes
		BEGIN		
			IF @cbCodeKind=1 SET @strErrorDescribe=N'{"Idx":1065,"Msg":"该游戏账户今天的手机验证次数已超过'+CONVERT(NVARCHAR(3),@MaxATAuthTimes)+'次，为了您的帐号安全，已锁定验证。请明天再试！","Ops":["'+CONVERT(NVARCHAR(3),@MaxATAuthTimes)+'"]}'
			IF @cbCodeKind=2 SET @strErrorDescribe=N'{"Idx":1080,"Msg":"该游戏账户今天的邮箱验证次数已超过'+CONVERT(NVARCHAR(3),@MaxATAuthTimes)+'次，为了您的帐号安全，已锁定验证。请明天再试！","Ops":["'+CONVERT(NVARCHAR(3),@MaxATAuthTimes)+'"]}'
			RETURN 23						
		END	
		
		-- 次数限制	
		IF @DateDiff=0 AND @ATAcquireTimes>=@MaxATAcquireTimes
		BEGIN		
			IF @cbCodeKind=1 SET @strErrorDescribe=N'{"Idx":1066,"Msg":"该游戏账号今天的手机验证请求次数已超过'+CONVERT(NVARCHAR(3),@MaxATAcquireTimes)+'次，为了您的帐号安全，已锁定验证。请明天再试！","Ops":["'+CONVERT(NVARCHAR(3),@MaxATAcquireTimes)+'"]}'
			IF @cbCodeKind=2 SET @strErrorDescribe=N'{"Idx":1081,"Msg":"该游戏账号今天的邮箱验证请求次数已超过'+CONVERT(NVARCHAR(3),@MaxATAcquireTimes)+'次，为了您的帐号安全，已锁定验证。请明天再试！","Ops":["'+CONVERT(NVARCHAR(3),@MaxATAcquireTimes)+'"]}'
			RETURN 24						
		END	

		-- 验证码记录
		SELECT @MPAuthTimes=AuthTimes,@MPAuthCode=AuthCode,@MPAcquireTimes=AcquireTimes,@MPAuthDate=LastAcquireDate FROM RecordAuthCode(NOLOCK) WHERE CodeKind=@cbCodeKind AND MobileEmail=@strMobileEmail

		-- 记录存在
		IF @MPAuthTimes IS NULL
		BEGIN
			-- 插入记录
			INSERT INTO RecordAuthCode(CodeKind, MobileEmail,UserID,AuthKind,AuthCode,AuthCodeMD5,AuthTimes,AcquireTimes,AcquireClientIP,AcquireMachine,LastAcquireDate)										
			VALUES (@cbCodeKind,@strMobileEmail,@UserID,@cbAuthKind,@strAuthCode,@strAuthCodeMD5,0,1,@strClientIP,@strMachineID,GetDate())

		END ELSE
		BEGIN
			-- 计算间隔
			SET @DateDiff=DateDiff(day,@MPAuthDate,GetDate())
			SET @SecondDiff=DateDiff(s,@MPAuthDate,GetDate())			

			-- 次数限制	
			IF @DateDiff=0 AND @MPAuthTimes>=@MaxMPAuthTimes
			BEGIN		
				IF @cbCodeKind=1 SET @strErrorDescribe=N'{"Idx":1062,"Msg":"该手机号今天的验证次数已超过'+CONVERT(NVARCHAR(3),@MaxMPAuthTimes)+'次，为了您的帐号安全，已锁定验证。请明天再试！","Ops":["'+CONVERT(NVARCHAR(3),@MaxMPAuthTimes)+'"]}'
				IF @cbCodeKind=2 SET @strErrorDescribe=N'{"Idx":1077,"Msg":"该邮箱今天的验证次数已超过'+CONVERT(NVARCHAR(3),@MaxMPAuthTimes)+'次，为了您的帐号安全，已锁定验证。请明天再试！","Ops":["'+CONVERT(NVARCHAR(3),@MaxMPAuthTimes)+'"]}'
				RETURN 25						
			END

			-- 次数限制	
			IF @DateDiff=0 AND @MPAcquireTimes>=@MaxMPAcquireTimes
			BEGIN		
				IF @cbCodeKind=1 SET @strErrorDescribe=N'{"Idx":1063,"Msg":"该手机号今天的验证请求次数已超过'+CONVERT(NVARCHAR(3),@MaxMPAcquireTimes)+'次，为了您的帐号安全，已锁定验证。请更换手机号码或明天再试！","Ops":["'+CONVERT(NVARCHAR(3),@MaxMPAcquireTimes)+'"]}'
				IF @cbCodeKind=2 SET @strErrorDescribe=N'{"Idx":1078,"Msg":"该邮箱今天的验证请求次数已超过'+CONVERT(NVARCHAR(3),@MaxMPAcquireTimes)+'次，为了您的帐号安全，已锁定验证。请更换邮箱或明天再试！","Ops":["'+CONVERT(NVARCHAR(3),@MaxMPAcquireTimes)+'"]}'
				RETURN 26						
			END

			-- 时间限制			
			IF @MPAuthCode<>N'' AND @SecondDiff<@ACValidSeconds	
			BEGIN	
				-- 更新类型
				IF @MPAuthKind<>@cbAuthKind
				BEGIN
					UPDATE RecordAuthCode SET AuthKind=@cbAuthKind WHERE CodeKind=@cbCodeKind AND MobileEmail=@strMobileEmail
				END			
				
				-- 提示信息
				IF @cbCodeKind=1 SET @strErrorDescribe=N'{"Idx":1064,"Msg":"手机验证码已成功发送至尾号为：'+SUBSTRING(@strMobileEmail,LEN(@strMobileEmail)-3,4)+'的手机。请留意短信通知。","Ops":["'+SUBSTRING(@strMobileEmail,LEN(@strMobileEmail)-3,4)+'"]}'
				IF @cbCodeKind=2 SET @strErrorDescribe=N'{"Idx":1079,"Msg":"验证码已成功发送至邮箱：'+@strMobileEmail+'。请留意邮件通知。","Ops":["'+@strMobileEmail+'"]}'

				-- 抛出标识
				SELECT @BindUserID AS BindUserID

				RETURN 3
			END		

			-- 调整次数
			IF @DateDiff<>0 SELECT @MPAuthTimes=0,@MPAcquireTimes=0
			
			-- 更新记录
			UPDATE RecordAuthCode SET UserID=@dwUserID,AuthKind=@cbAuthKind,AuthCode=@strAuthCode,AuthCodeMD5=@strAuthCodeMD5,AuthTimes=@MPAuthTimes,AcquireTimes=@MPAcquireTimes+1,
			AcquireClientIP=@strClientIP,AcquireMachine=@strMachineID,LastAcquireDate=GetDate() WHERE CodeKind=@cbCodeKind AND MobileEmail=@strMobileEmail
		END	
		
		-- 计算日期
		SET @DateDiff=DateDiff(day,@ATAuthDate,GetDate())

		-- 调整次数
		IF @DateDiff<>0 SELECT @ATAuthTimes=0,@ATAcquireTimes=0				

		-- 更新记录
		UPDATE AccountsAuthInfo SET AcquireTimes=@ATAcquireTimes+1,LastAcquireDate=GetDate(),LastMobileEmail=@strMobileEmail WHERE UserID=@dwUserID AND CodeKind=@cbCodeKind
	END	

	-- 提示信息
	IF @cbCodeKind=1 SET @strErrorDescribe=N'{"Idx":1064,"Msg":"手机验证码已成功发送至尾号为：'+SUBSTRING(@strMobileEmail,LEN(@strMobileEmail)-3,4)+'的手机。请留意短信通知。","Ops":["'+SUBSTRING(@strMobileEmail,LEN(@strMobileEmail)-3,4)+'"]}'
	IF @cbCodeKind=2 SET @strErrorDescribe=N'{"Idx":1079,"Msg":"验证码已成功发送至邮箱：'+@strMobileEmail+'。请留意邮件通知。","Ops":["'+@strMobileEmail+'"]}'

	-- 抛出标识
	SELECT @BindUserID AS BindUserID
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 绑定邮箱
CREATE PROC GSP_MB_BindEMail
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码	
	@strNewPassword NCHAR(32),					-- 用户密码
	@strMachineID NVARCHAR(32),					-- 机器序列
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strEmail NVARCHAR(64),						-- 邮    箱
	@strAuthCode NCHAR(32),						-- 验 证 码
	@dwUnBindUserID INT,						-- 解绑标识	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	DECLARE @LogonPass NCHAR(32)	
	SELECT @UserID=a.UserID,@LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
		RETURN 2
	END	

	-- 帐号关闭
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
		RETURN 3
	END	
	
	-- 密码判断	
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 4
	END	

	-- 查询用户
	DECLARE @BindUserID INT
	SELECT @BindUserID=UserID FROM AccountsInfo(NOLOCK) WHERE EMail=@strEmail

	-- 校验标识
	IF @BindUserID IS NOT NULL AND @BindUserID<>@dwUnBindUserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1085,"Msg":"抱歉，该邮箱已经和其他游戏账号绑定，邮箱绑定失败！"}'
		RETURN 5		
	END

	-- 查询邮箱
	DECLARE @ATLastAcquireDate DATETIME
	DECLARE @LastEmail NVARCHAR(32)		
	SELECT @LastEmail=LastMobileEmail,@ATLastAcquireDate=LastAcquireDate FROM AccountsAuthInfo(NOLOCK) WHERE UserID=@dwUserID AND CodeKind=2
	
	-- 校验记录
	IF @LastEmail IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1086,"Msg":"验证码错误，邮箱绑定失败！"}'
		RETURN 51
	END

	-- 校验邮箱
	IF @LastEmail<>@strEmail
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1086,"Msg":"验证码错误，邮箱绑定失败！"}'
		RETURN 52
	END

	-- 验证信息
	DECLARE @AuthKind TINYINT	
	DECLARE @AuthCodeMD5 NVARCHAR(32)
	DECLARE @AcquireMachine NVARCHAR(32)
	DECLARE @MBLastAcquireDate DATETIME
	SELECT @BindUserID=UserID,@AuthKind=AuthKind, @AuthCodeMD5=AuthCodeMD5, @AcquireMachine=AcquireMachine, @MBLastAcquireDate=LastAcquireDate FROM RecordAuthCode(NOLOCK) WHERE CodeKind=2 AND MobileEmail=@strEmail

	-- 验证码不存在
	IF @BindUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1086,"Msg":"验证码错误，邮箱绑定失败！"}'
		RETURN 61
	END

	-- 用户不匹配
	IF @BindUserID<>@UserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1086,"Msg":"验证码错误，邮箱绑定失败！"}'
		RETURN 62
	END

	-- 校验类型
	IF @AuthKind<>3
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1086,"Msg":"验证码错误，邮箱绑定失败！"}'
		RETURN 63		
	END

	-- 校验验证码
	IF @AuthCodeMD5<>@strAuthCode
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1086,"Msg":"验证码错误，邮箱绑定失败！"}'
		RETURN 7				
	END

	-- 校验机器码
	IF @AcquireMachine<>@strMachineID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1069,"Msg":"您的机器码发生异常，操作失败！"}'
		RETURN 8
	END	

	-- 有效时长
	DECLARE @ACValidSeconds INT
	SELECT @ACValidSeconds=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ACValidSecond'

	-- 调整参数
	IF @ACValidSeconds IS NULL SET @ACValidSeconds=600

	-- 时间间隔
	DECLARE @SecondDiff INT
	SET @SecondDiff=DateDiff(s,@MBLastAcquireDate,GetDate())

	-- 时间限制
	IF @SecondDiff>@ACValidSeconds	
	BEGIN			
		SET @strErrorDescribe=N'{"Idx":1029,"Msg":"验证码已过期，请重新获取验证码！"}'
		RETURN 8
	END

	-- 他方标识
	DECLARE @ExternalID INT	
	SELECT @ExternalID=ExternalID FROM AccountsExternal(NOLOCK) WHERE UserID=@dwUserID

	-- 调整变量	
	IF @ExternalID IS NULL SET @ExternalID=0

	-- 用户财富
	DECLARE @UserCard INT	

	-- 绑定记录
	IF NOT EXISTS(SELECT 1 FROM RecordBindEMail(NOLOCK) WHERE UserID=@dwUserID)
	BEGIN		
		-- 插入记录
		INSERT INTO RecordBindEMail(UserID,EMail,CollectDate) VALUES (@dwUserID,@strEmail,GetDate())
	END	
	
	-- 更新信息
	UPDATE AccountsAuthInfo SET AuthTimes=AuthTimes+1,AcquireTimes=0, LastAcquireDate=GetDate(),LastMobileEmail=N'' WHERE UserID=@dwUserID AND CodeKind=2

	-- 更新记录
	UPDATE RecordAuthCode SET AuthCode=N'',AuthCodeMD5=N'', AuthTimes=AuthTimes+1,AcquireTimes=0 WHERE CodeKind=2 AND MobileEmail=@strEmail			

	-- 设置密码
	IF @strNewPassword<>N'' SET @LogonPass=@strNewPassword

	-- 解绑邮箱
	IF @dwUnBindUserID<>0 UPDATE AccountsInfo SET EMail=N'' WHERE UserID=@dwUnBindUserID

	-- 更新信息
	UPDATE AccountsInfo SET EMail=@strEmail,LogonPass=@LogonPass WHERE UserID=@dwUserID	

	-- 计算日期
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 记录验证次数
	UPDATE RecordAuthTimes SET AuthTimes=AuthTimes+1 WHERE DateID=@DateID AND UserID=@dwUserID AND CodeKind=2 AND AuthKind=@AuthKind
	IF @@ROWCOUNT=0
	BEGIN
		-- 插入记录
		INSERT INTO RecordAuthTimes(DateID,UserID,CodeKind,AuthKind,AuthTimes) VALUES (@DateID,@dwUserID,2,@AuthKind,1)
	END
	
	-- 查询财富
	SELECT @UserCard =Score FROM LHGameBattleDB.dbo.GameCardInfo(NOLOCK) WHERE UserID=@dwUserID	

	-- 调整变量
	IF @UserCard IS NULL SET @UserCard=0

	-- 绑定成功
	SET @strErrorDescribe=N'{"Idx":1070,"Msg":"邮箱绑定成功！"}'	

	-- 抛出财富
	SELECT @UserCard AS UserCard
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 解绑邮箱
CREATE PROC GSP_MB_UnBindEMail
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@strMachineID NVARCHAR(32),					-- 机器序列
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strEmail NVARCHAR(32),						-- 邮	 箱
	@strAuthCode NCHAR(32),						-- 验 证 码	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @LogonPass NCHAR(32)
	DECLARE @BindEMail NVARCHAR(32)
	SELECT @UserID=a.UserID, @LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus, @BindEMail=a.EMail 
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
		RETURN 2
	END	

	-- 帐号关闭
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
		RETURN 3
	END	
	
	-- 密码判断	
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 4
	END	

	-- 查询邮箱
	DECLARE @LastEmail NVARCHAR(32)	
	SELECT @LastEmail=LastMobileEmail FROM AccountsAuthInfo(NOLOCK) WHERE UserID=@dwUserID AND CodeKind=2
	
	-- 校验记录
	IF @LastEmail IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"验证码错误[1]，邮箱解绑失败！"}'
		RETURN 5
	END

	-- 校验邮箱
	IF @LastEmail<>@strEmail
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"验证码错误[2]，邮箱解绑失败！"}'
		RETURN 5
	END

	-- 验证信息
	DECLARE @BindUserID INT
	DECLARE @AuthKind TINYINT
	DECLARE @AuthCodeMD5 NVARCHAR(32)
	DECLARE @AcquireMachine NVARCHAR(32)	
	DECLARE @LastAcquireDate DATETIME
	SELECT @BindUserID=UserID, @AuthKind=AuthKind, @AuthCodeMD5=AuthCodeMD5, @AcquireMachine=AcquireMachine, @LastAcquireDate=LastAcquireDate FROM RecordAuthCode(NOLOCK) WHERE CodeKind=2 AND MobileEmail=@strEmail

	-- 验证码不存在
	IF @BindUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"验证码错误[3]，邮箱解绑失败！"}'
		RETURN 6
	END

	-- 用户不匹配
	IF @BindUserID<>@UserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"验证码错误[4]，邮箱解绑失败！"}'
		RETURN 6
	END

	-- 校验类型
	IF @AuthKind<>4
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"验证码错误[5]，邮箱解绑失败！"}'
		RETURN 6		
	END

	-- 校验验证码
	IF @AuthCodeMD5<>@strAuthCode
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"验证码错误[6]，邮箱解绑失败！"}'
		RETURN 7				
	END

	-- 校验机器码
	IF @AcquireMachine<>@strMachineID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1069,"Msg":"您的机器码发生异常，邮箱解绑失败！"}'
		RETURN 8
	END

	-- 有效时长
	DECLARE @ACValidSeconds INT
	SELECT @ACValidSeconds=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ACValidSecond'

	-- 调整参数
	IF @ACValidSeconds IS NULL SET @ACValidSeconds=600

	-- 时间间隔
	DECLARE @SecondDiff INT
	SET @SecondDiff=DateDiff(s,@LastAcquireDate,GetDate())

	-- 时间限制
	IF @SecondDiff>@ACValidSeconds	
	BEGIN			
		SET @strErrorDescribe=N'{"Idx":1029,"Msg":"验证码已过期，请重新获取验证码！"}'
		RETURN 8
	END

	-- 变量定义
	--DECLARE @BindMPDeductCard INT
	
	---- 读取配置
	--SELECT @BindMPDeductCard=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'BindMPDeductCard'				
			
	---- 调整配置
	--IF @BindMPDeductCard IS NULL SET @BindMPDeductCard=10			
	
	---- 用户财富
	DECLARE @UserCard BIGINT
			
	---- 扣除房卡
	--IF @BindMPDeductCard>0
	--BEGIN
	--	-- 查询房卡				
	--	SELECT @UserCard=Card FROM LHGameBattleDB.dbo.GameCardInfo WHERE UserID=@dwUserID	

	--	-- 房卡调整
	--	IF @UserCard IS NULL SET @UserCard=0

	--	-- 余额校验
	--	IF @BindMPDeductCard>@UserCard
	--	BEGIN
	--		SET @strErrorDescribe=N'{"Idx":1072,"Msg":"你的房卡余额不足，请充值后再次尝试！"}'
	--		RETURN 9	
	--	END

	--	-- 更新房卡
	--	UPDATE LHGameBattleDB.dbo.GameCardInfo SET Card=Card-@BindMPDeductCard WHERE UserID=@dwUserID		

	---- 变更记录
	--INSERT INTO LHGameRecordDB.dbo.RecordChangeCard(UserID, GameID, TypeID, BeforeCard, ChangeCard,CollectClientIP, CollectDateTime)
	--VALUES (@dwUserID, @GameID, 6, @UserCard, -@BindMPDeductCard,  N'-----------', GetDate())

	--END		

	-- 更新信息
	UPDATE AccountsAuthInfo SET AuthTimes=AuthTimes+1,AcquireTimes=0,LastMobileEmail=N'' WHERE UserID=@dwUserID AND CodeKind=2

	-- 更新记录
	UPDATE RecordAuthCode SET AuthCode=N'',AuthCodeMD5=N'',AuthTimes=AuthTimes+1,AcquireTimes=0 WHERE CodeKind=2 AND MobileEmail=@strEmail

	-- 更新邮箱
	UPDATE AccountsInfo SET EMail=N'' WHERE UserID=@dwUserID

	-- 计算日期
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 记录验证次数
	UPDATE RecordAuthTimes SET AuthTimes=AuthTimes+1 WHERE DateID=@DateID AND UserID=@dwUserID AND CodeKind=2 AND AuthKind=@AuthKind
	IF @@ROWCOUNT=0
	BEGIN
		-- 插入记录
		INSERT INTO RecordAuthTimes(DateID,UserID,CodeKind,AuthKind,AuthTimes) VALUES (@DateID,@dwUserID,2,@AuthKind,1)
	END

	-- 查询财富
	SELECT @UserCard=Score FROM LHGameBattleDB.dbo.GameCardInfo WHERE UserID=@dwUserID	
	
	-- 调整变量
	IF @UserCard IS NULL SET @UserCard=0
	
	-- 绑定成功
	SET @strErrorDescribe=N'{"Idx":1070,"Msg":"邮箱解绑成功！"}'	

	-- 抛出财富
	SELECT @UserCard AS UserCard
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 绑定手机号码
CREATE PROC GSP_MB_BindMobilePhone
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码	
	@strNewPassword NCHAR(32),					-- 用户密码
	@strMachineID NVARCHAR(32),					-- 机器序列
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMobilePhone NVARCHAR(21),				-- 手机号码
	@strAuthCode NCHAR(32),						-- 验 证 码
	@dwUnBindUserID INT,						-- 解绑标识	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @LogonPass NCHAR(32)		
	SELECT @UserID=a.UserID, @LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
		RETURN 2
	END	

	-- 帐号关闭
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
		RETURN 3
	END	
	
	-- 密码判断	
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 4
	END	

	-- 查询用户
	DECLARE @BindUserID INT
	SELECT @BindUserID=UserID FROM AccountsInfo(NOLOCK) WHERE MobilePhone=@strMobilePhone

	-- 校验标识
	IF @BindUserID IS NOT NULL AND @BindUserID<>@dwUnBindUserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1067,"Msg":"抱歉，该手机号已经和其他游戏账号绑定，请更换新的手机号绑定！"}'
		RETURN 5		
	END

	-- 查询手机号
	DECLARE @ATLastAcquireDate DATETIME
	DECLARE @LastMobilePhone NVARCHAR(21)		
	SELECT @LastMobilePhone=LastMobileEmail,@ATLastAcquireDate=LastAcquireDate FROM AccountsAuthInfo(NOLOCK) WHERE UserID=@dwUserID AND CodeKind=1
	
	-- 校验记录
	IF @LastMobilePhone IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1068,"Msg":"验证码错误，手机号绑定失败！"}'
		RETURN 51
	END

	-- 校验手机号
	IF @LastMobilePhone<>@strMobilePhone
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1068,"Msg":"验证码错误，手机号绑定失败！"}'
		RETURN 52
	END

	-- 验证信息
	DECLARE @AuthKind TINYINT	
	DECLARE @AuthCodeMD5 NVARCHAR(32)
	DECLARE @AcquireMachine NVARCHAR(32)
	DECLARE @MBLastAcquireDate DATETIME
	SELECT @BindUserID=UserID,@AuthKind=AuthKind, @AuthCodeMD5=AuthCodeMD5, @AcquireMachine=AcquireMachine, @MBLastAcquireDate=LastAcquireDate FROM RecordAuthCode(NOLOCK) WHERE CodeKind=1 AND MobileEmail=@strMobilePhone

	-- 验证码不存在
	IF @BindUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1068,"Msg":"验证码错误，手机号绑定失败！"}'
		RETURN 61
	END

	-- 用户不匹配
	IF @BindUserID<>@UserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1068,"Msg":"验证码错误，手机号绑定失败！"}'
		RETURN 62
	END

	-- 校验类型
	IF @AuthKind<>1
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1068,"Msg":"验证码错误，手机号绑定失败！"}'
		RETURN 63		
	END

	-- 校验验证码
	IF @AuthCodeMD5<>@strAuthCode
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1068,"Msg":"验证码错误，手机号绑定失败！"}'
		RETURN 7				
	END

	-- 校验机器码
	IF @AcquireMachine<>@strMachineID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1069,"Msg":"您的机器码发生异常，手机号绑定失败！"}'
		RETURN 8
	END	

	-- 有效时长
	DECLARE @ACValidSeconds INT
	SELECT @ACValidSeconds=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ACValidSecond'

	-- 调整参数
	IF @ACValidSeconds IS NULL SET @ACValidSeconds=600

	-- 时间间隔
	DECLARE @SecondDiff INT
	SET @SecondDiff=DateDiff(s,@MBLastAcquireDate,GetDate())

	-- 时间限制
	IF @SecondDiff>@ACValidSeconds	
	BEGIN			
		SET @strErrorDescribe=N'{"Idx":1029,"Msg":"验证码已过期，请重新获取验证码！"}'
		RETURN 8
	END

	-- 他方标识
	DECLARE @ExternalID INT	
	SELECT @ExternalID=ExternalID FROM AccountsExternal(NOLOCK) WHERE UserID=@dwUserID

	-- 调整变量	
	IF @ExternalID IS NULL SET @ExternalID=0

	-- 用户财富
	DECLARE @UserCard INT	

	-- 绑定记录
	IF NOT EXISTS(SELECT 1 FROM RecordBindMobilePhone(NOLOCK) WHERE UserID=@dwUserID)
	BEGIN			
		-- 插入记录
		INSERT INTO RecordBindMobilePhone(UserID,MobilePhone,CollectDate) VALUES (@dwUserID,@strMobilePhone,GetDate())
	END	

	-- 更新信息
	UPDATE AccountsAuthInfo SET AuthTimes=AuthTimes+1,AcquireTimes=0, LastAcquireDate=GetDate(),LastMobileEmail=N'' WHERE UserID=@dwUserID AND CodeKind=1

	-- 更新记录
	UPDATE RecordAuthCode SET AuthCode=N'',AuthCodeMD5=N'', AuthTimes=AuthTimes+1,AcquireTimes=0 WHERE CodeKind=1 AND MobileEmail=@strMobilePhone			
	UPDATE RecordBindMobilePhone SET MobilePhone=@strMobilePhone,CollectDate=GetDate() WHERE UserID=@dwUserID

	-- 设置密码
	IF @strNewPassword<>N'' SET @LogonPass=@strNewPassword

	-- 解绑手机
	IF @dwUnBindUserID<>0 
		UPDATE AccountsInfo SET MobilePhone=N'' WHERE UserID=@dwUnBindUserID
	ELSE
		-- 更新信息
		UPDATE AccountsInfo SET MobilePhone=@strMobilePhone,LogonPass=@LogonPass WHERE UserID=@dwUserID	
	
	-- 游客更新为正式账号,调整账号名
	IF @dwUnBindUserID = 0 AND EXISTS(SELECT 1 FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID AND RegisterMode=1)
	BEGIN
		DECLARE @Accounts NVARCHAR(32)
		DECLARE @strNickName NVARCHAR(32)
		SET @Accounts=CONCAT(N'Mobile',SUBSTRING(@strMobilePhone,LEN(@strMobilePhone)-3,4))
		SET @strNickName=CONCAT(SUBSTRING(@strMobilePhone,LEN(@strMobilePhone)-3,4),N':',N'Mobile')	

		EXEC dbo.LHP_AdjustmentAccounts @Accounts OUTPUT,@strNickName OUTPUT

		UPDATE AccountsInfo SET RegisterMode=2,NickName=@strNickName,Accounts=@Accounts,RegAccounts=@Accounts WHERE UserID=@dwUserID AND RegisterMode=1
	END
	
	-- 计算日期
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 记录验证次数
	UPDATE RecordAuthTimes SET AuthTimes=AuthTimes+1 WHERE DateID=@DateID AND UserID=@dwUserID AND CodeKind=1 AND AuthKind=@AuthKind
	IF @@ROWCOUNT=0
	BEGIN
		-- 插入记录
		INSERT INTO RecordAuthTimes(DateID,UserID,CodeKind, AuthKind,AuthTimes) VALUES (@DateID,@dwUserID,1,@AuthKind,1)
	END
	
	-- 查询财富
	SELECT @UserCard =Score FROM LHGameBattleDB.dbo.GameCardInfo(NOLOCK) WHERE UserID=@dwUserID	

	-- 调整变量
	IF @UserCard IS NULL SET @UserCard=0

	-- 绑定成功
	SET @strErrorDescribe=N'{"Idx":1070,"Msg":"手机号绑定成功！"}'	

	-- 抛出财富
	SELECT @UserCard AS UserCard
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 解绑手机号
CREATE PROC GSP_MB_UnBindMobilePhone
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@strMachineID NVARCHAR(32),					-- 机器序列
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMobilePhone NVARCHAR(21),				-- 手机号码
	@strAuthCode NCHAR(32),						-- 验证码	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @UserID INT
	DECLARE @GameID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @LogonPass NCHAR(32)
	DECLARE @BindMobilePhone NVARCHAR(21)
	SELECT @UserID=a.UserID, @GameID=a.GameID, @LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus, @BindMobilePhone=a.MobilePhone 
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
		RETURN 2
	END	

	-- 帐号关闭
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
		RETURN 3
	END	
	
	-- 密码判断	
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 4
	END	

	-- 查询手机号
	DECLARE @LastMobilePhone NVARCHAR(21)	
	SELECT @LastMobilePhone=LastMobileEmail FROM AccountsAuthInfo(NOLOCK) WHERE UserID=@dwUserID
	
	-- 校验记录
	IF @LastMobilePhone IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"验证码错误[1]，手机号解绑失败！"}'
		RETURN 5
	END

	-- 校验手机号
	IF @LastMobilePhone<>@strMobilePhone
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"验证码错误[2]，手机号解绑失败！"}'
		RETURN 5
	END

	-- 验证信息
	DECLARE @BindUserID INT
	DECLARE @AuthKind TINYINT
	DECLARE @AuthCodeMD5 NVARCHAR(32)
	DECLARE @AcquireMachine NVARCHAR(32)	
	DECLARE @LastAcquireDate DATETIME
	SELECT @BindUserID=UserID, @AuthKind=AuthKind, @AuthCodeMD5=AuthCodeMD5, @AcquireMachine=AcquireMachine, @LastAcquireDate=LastAcquireDate FROM RecordAuthCode(NOLOCK) WHERE  CodeKind=1 AND MobileEmail=@strMobilePhone

	-- 验证码不存在
	IF @BindUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"验证码错误[3]，手机号解绑失败！"}'
		RETURN 6
	END

	-- 用户不匹配
	IF @BindUserID<>@UserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"验证码错误[4]，手机号解绑失败！"}'
		RETURN 6
	END

	-- 校验类型
	IF @AuthKind<>2
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"验证码错误[5]，手机号解绑失败！"}'
		RETURN 6		
	END

	-- 校验验证码
	IF @AuthCodeMD5<>@strAuthCode
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"验证码错误[6]，手机号解绑失败！"}'
		RETURN 7				
	END

	-- 校验机器码
	IF @AcquireMachine<>@strMachineID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1069,"Msg":"您的机器码发生异常，手机号解绑失败！"}'
		RETURN 8
	END

	-- 有效时长
	DECLARE @ACValidSeconds INT
	SELECT @ACValidSeconds=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ACValidSecond'

	-- 调整参数
	IF @ACValidSeconds IS NULL SET @ACValidSeconds=600

	-- 时间间隔
	DECLARE @SecondDiff INT
	SET @SecondDiff=DateDiff(s,@LastAcquireDate,GetDate())

	-- 时间限制
	IF @SecondDiff>@ACValidSeconds	
	BEGIN			
		SET @strErrorDescribe=N'{"Idx":1029,"Msg":"验证码已过期，请重新获取验证码！"}'
		RETURN 8
	END

	-- 变量定义
	--DECLARE @BindMPDeductCard INT
	
	---- 读取配置
	--SELECT @BindMPDeductCard=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'BindMPDeductCard'				
			
	---- 调整配置
	--IF @BindMPDeductCard IS NULL SET @BindMPDeductCard=10			
	
	---- 用户财富
	DECLARE @UserCard INT
			
	---- 扣除房卡
	--IF @BindMPDeductCard>0
	--BEGIN
	--	-- 查询房卡				
	--	SELECT @UserCard=Score FROM LHGameBattleDB.dbo.GameCardInfo WHERE UserID=@dwUserID	

	--	-- 房卡调整
	--	IF @UserCard IS NULL SET @UserCard=0

	--	-- 余额校验
	--	IF @BindMPDeductCard>@UserCard
	--	BEGIN
	--		SET @strErrorDescribe=N'{"Idx":1072,"Msg":"你的房卡余额不足，请充值后再次尝试！"}'
	--		RETURN 9	
	--	END

	--	-- 更新房卡
	--	UPDATE LHGameBattleDB.dbo.GameCardInfo SET Score-=@BindMPDeductCard WHERE UserID=@dwUserID		

	---- 变更记录
	--INSERT INTO LHGameRecordDB.dbo.RecordChangeCard(UserID, GameID, TypeID, BeforeCard, ChangeCard,CollectClientIP, CollectDateTime)
	--VALUES (@dwUserID, @GameID, 5, @UserCard, -@BindMPDeductCard,  N'-----------', GetDate())
	--END	
	
	-- 更新信息
	UPDATE AccountsAuthInfo SET AuthTimes=AuthTimes+1,AcquireTimes=0,LastMobileEmail=N'' WHERE UserID=@dwUserID

	-- 更新记录
	UPDATE RecordAuthCode SET AuthCode=N'',AuthCodeMD5=N'',AuthTimes=AuthTimes+1,AcquireTimes=0 WHERE CodeKind=1 AND MobileEmail=@strMobilePhone

	-- 更新手机号
	UPDATE AccountsInfo SET MobilePhone=N'' WHERE UserID=@dwUserID

	-- 常量定义
	DECLARE @ShutDownOverTime INT
	SET @ShutDownOverTime=dbo.LHF_GetTimestamp()+24*60*60*365*3

	-- 注销账号
	UPDATE AccountsStatus SET ShutDownStatus=1,ShutDownOverTime=@ShutDownOverTime WHERE UserID=@dwUserID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO AccountsStatus(UserID,ShutupStatus,ShutupOverTime,ShutDownStatus,ShutDownOverTime,LastShutupDateTime,LastShutDownDateTime)
		VALUES (@dwUserID,0,0,1,@ShutDownOverTime,GETDATE(),GETDATE())
	END

	-- 计算日期
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 记录验证次数
	UPDATE RecordAuthTimes SET AuthTimes=AuthTimes+1 WHERE DateID=@DateID AND UserID=@dwUserID AND CodeKind=1 AND AuthKind=@AuthKind
	IF @@ROWCOUNT=0
	BEGIN
		-- 插入记录
		INSERT INTO RecordAuthTimes(DateID,UserID,CodeKind, AuthKind,AuthTimes) VALUES (@DateID,@dwUserID,1,@AuthKind,1)
	END

	-- 查询财富
	SELECT @UserCard=Score FROM LHGameBattleDB.dbo.GameCardInfo WHERE UserID=@dwUserID

	-- 调整变量
	IF @UserCard IS NULL SET @UserCard=0
	
	-- 绑定成功
	SET @strErrorDescribe=N'{"Idx":1070,"Msg":"手机号解绑成功！"}'	

	-- 抛出财富
	SELECT @UserCard AS UserCard
	
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 重置密码
CREATE PROC GSP_MB_ResetLogonPasswd
	@dwBindUserID INT,							-- 绑定标识
	@strPassword NCHAR(32),						-- 用户密码
	@strMachineID NVARCHAR(32),					-- 机器序列
	@cbCodeKind TINYINT,						-- 验证码类型
	@strMobileEmail NVARCHAR(64),				-- 手机邮箱
	@strAuthCode NCHAR(32),						-- 验证码	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	DECLARE @LogonPass NCHAR(32)	

	-- 手机验证码
	IF @cbCodeKind=1
	BEGIN
		SELECT @UserID=a.UserID, @LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
		FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.MobilePhone=@strMobileEmail
	END

	-- 邮箱验证码
	IF @cbCodeKind=2
	BEGIN
		SELECT @UserID=a.UserID, @LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
		FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.Email=@strMobileEmail
	END

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
		RETURN 2
	END	

	-- 帐号关闭
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
		RETURN 3
	END	

	-- 校验标识
	IF @UserID<>@dwBindUserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1075,"Msg":"验证码异常[1]，密码重置失败！"}'
		RETURN 4
	END	

	-- 验证信息
	DECLARE @BindUserID INT
	DECLARE @AuthKind TINYINT
	DECLARE @AuthCodeMD5 NVARCHAR(32)
	DECLARE @AcquireMachine NVARCHAR(32)	
	DECLARE @LastAcquireDate DATETIME
	SELECT @BindUserID=UserID, @AuthKind=AuthKind, @AuthCodeMD5=AuthCodeMD5, @AcquireMachine=AcquireMachine, @LastAcquireDate=LastAcquireDate FROM RecordAuthCode(NOLOCK) WHERE CodeKind=@cbCodeKind AND MobileEmail=@strMobileEmail

	-- 验证码不存在
	IF @BindUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1075,"Msg":"验证码异常[2]，密码重置失败！"}'
		RETURN 6
	END

	-- 用户不匹配
	IF @BindUserID<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1075,"Msg":"验证码异常[3]，密码重置失败！"}'
		RETURN 7
	END

	-- 校验类型
	IF @AuthKind<>5
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1075,"Msg":"验证码异常[4]，密码重置失败！"}'
		RETURN 8
	END
	
	-- 校验验证码
	IF @AuthCodeMD5<>@strAuthCode
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1075,"Msg":"验证码异常[5]，密码重置失败！"}'
		RETURN 9
	END

	-- 校验机器码
	IF @AcquireMachine<>@strMachineID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1069,"Msg":"您的机器码发生异常，密码重置失败！"}'
		RETURN 10
	END

	-- 有效时长
	DECLARE @ACValidSeconds INT
	SELECT @ACValidSeconds=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ACValidSecond'

	-- 调整参数
	IF @ACValidSeconds IS NULL SET @ACValidSeconds=600

	-- 时间间隔
	DECLARE @SecondDiff INT
	SET @SecondDiff=DateDiff(s,@LastAcquireDate,GetDate())

	-- 时间限制
	IF @SecondDiff>@ACValidSeconds	
	BEGIN			
		SET @strErrorDescribe=N'{"Idx":1029,"Msg":"验证码已过期，请重新获取验证码！"}'
		RETURN 11
	END

	-- 更新信息
	DECLARE @ATAuthTimes SMALLINT
	DECLARE @ATLastAcquireDate DATETIME
	SELECT @ATAuthTimes=AuthTimes,@ATLastAcquireDate=LastAcquireDate FROM AccountsAuthInfo(NOLOCK) WHERE UserID=@UserID AND CodeKind=@cbCodeKind

	-- 插入数据
	IF @ATAuthTimes IS NULL
	BEGIN
		INSERT INTO AccountsAuthInfo(UserID,CodeKind,AuthTimes,AcquireTimes,LastAcquireDate,LastMobileEmail) VALUES (@UserID,@cbCodeKind,1,0,GetDate(),N'')		
	END ELSE
	BEGIN
		-- 时间间隔
		DECLARE @DateDiff INT
		SET @DateDiff=DateDiff(day,@ATLastAcquireDate,GetDate())

		-- 调整次数
		IF @DateDiff<>0 SET @ATAuthTimes=0

		-- 更新数据
		UPDATE AccountsAuthInfo SET AuthTimes=@ATAuthTimes+1,AcquireTimes=0,LastAcquireDate=GetDate(),LastMobileEmail=N'' WHERE UserID=@UserID AND CodeKind=@cbCodeKind
	END

	-- 更新次数
	UPDATE RecordAuthCode SET AuthCode=N'',AuthCodeMD5=N'',AuthTimes=AuthTimes+1,AcquireTimes=0 WHERE CodeKind=@cbCodeKind AND MobileEmail=@strMobileEmail

	-- 更新密码
	UPDATE AccountsInfo SET LogonPass=@strPassword WHERE UserID=@UserID	

	-- 计算日期
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 记录验证次数
	UPDATE RecordAuthTimes SET AuthTimes=AuthTimes+1 WHERE DateID=@DateID AND UserID=@UserID AND CodeKind=@cbCodeKind AND AuthKind=@AuthKind
	IF @@ROWCOUNT=0
	BEGIN
		-- 插入记录
		INSERT INTO RecordAuthTimes(DateID,UserID,CodeKind,AuthKind,AuthTimes) VALUES (@DateID,@UserID,@cbCodeKind,@AuthKind,1)
	END
	
	-- 绑定成功
	SET @strErrorDescribe=N'{"Idx":1076,"Msg":"密码重置成功！"}'	
END

RETURN 0

GO


------------------------------------------------------------------------------------------------------

---- 收款账号
--CREATE PROC GSP_MB_ReceiptAccounts
--	@dwUserID INT,								-- 用户标识
--	@strPassword NCHAR(32),						-- 用户密码
--	@strAccountsName NVARCHAR(32),				-- 账号名称
--	@strMachineID NVARCHAR(32),					-- 机器序列
--	@strClientIP NVARCHAR(15),					-- 终端地址
--	@strMobilePhone NVARCHAR(21),				-- 手机号码
--	@strAuthCode NCHAR(32),					-- 验证码		
--	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
--WITH ENCRYPTION AS

---- 属性设置
--SET NOCOUNT ON

---- 执行逻辑
--BEGIN

--	-- 查询用户
--	DECLARE @UserID INT
--	DECLARE @Nullity BIT
--	DECLARE @ShutDown BIT	
--	DECLARE @LogonPass NCHAR(32)	
--	DECLARe @MobilePhone NVARCHAR(21)
--	SELECT @UserID=a.UserID,@MobilePhone=a.MobilePhone, @LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
--	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

--	-- 查询用户
--	IF @MobilePhone IS NULL
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
--		RETURN 1
--	END	

--	-- 帐号禁止
--	IF @Nullity<>0
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
--		RETURN 2
--	END	

--	-- 帐号关闭
--	IF @ShutDown<>0
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
--		RETURN 3
--	END	

--	-- 校验标识
--	IF @MobilePhone<>@strMobilePhone
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1276,"Msg":"验证码异常[1]，收款账号设置失败！"}'
--		RETURN 4
--	END	

--	-- 验证信息
--	DECLARE @BindUserID INT
--	DECLARE @AuthKind TINYINT
--	DECLARE @AuthCodeMD5 NVARCHAR(32)
--	DECLARE @AcquireMachine NVARCHAR(32)	
--	DECLARE @LastAcquireDate DATETIME
--	SELECT @BindUserID=UserID, @AuthKind=AuthKind, @AuthCodeMD5=AuthCodeMD5, @AcquireMachine=AcquireMachine, @LastAcquireDate=LastAcquireDate FROM RecordAuthCode(NOLOCK) WHERE MobilePhone=@strMobilePhone

--	-- 验证码不存在
--	IF @BindUserID IS NULL
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1276,"Msg":"验证码异常[2]，收款账号设置失败！"}'
--		RETURN 6
--	END

--	-- 用户不匹配
--	IF @BindUserID<>0
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1276,"Msg":"验证码异常[3]，收款账号设置失败！"}'
--		RETURN 6
--	END

--	-- 校验类型
--	IF @AuthKind<>8
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1276,"Msg":"验证码异常[4]，收款账号设置失败！"}'
--		RETURN 6		
--	END
	
--	-- 校验验证码
--	IF @AuthCodeMD5<>@strAuthCode
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1276,"Msg":"验证码异常[5]，收款账号设置失败！"}'
--		RETURN 7				
--	END

--	-- 校验机器码
--	IF @AcquireMachine<>@strMachineID
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1276,"Msg":"您的机器码发生异常，收款账号设置失败！"}'
--		RETURN 8
--	END

--	-- 有效时长
--	DECLARE @ACValidSeconds INT
--	SELECT @ACValidSeconds=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ACValidSecond'

--	-- 调整参数
--	IF @ACValidSeconds IS NULL SET @ACValidSeconds=600

--	-- 时间间隔
--	DECLARE @SecondDiff INT
--	SET @SecondDiff=DateDiff(s,@LastAcquireDate,GetDate())

--	-- 时间限制
--	IF @SecondDiff>@ACValidSeconds	
--	BEGIN			
--		SET @strErrorDescribe=N'{"Idx":1029,"Msg":"验证码已过期，请重新获取验证码！"}'
--		RETURN 8
--	END

--	-- 更新信息
--	DECLARE @ATAuthTimes SMALLINT
--	DECLARE @ATLastAcquireDate DATETIME
--	SELECT @ATAuthTimes=AuthTimes,@ATLastAcquireDate=LastAcquireDate FROM AccountsAuthInfo(NOLOCK) WHERE UserID=@UserID

--	-- 插入数据
--	IF @ATAuthTimes IS NULL
--	BEGIN
--		INSERT INTO AccountsAuthInfo(UserID,AuthTimes,AcquireTimes,LastAcquireDate,LastMobilePhone)	VALUES (@UserID,1,0,GetDate(),N'')		
--	END ELSE
--	BEGIN
--		-- 时间间隔
--		DECLARE @DateDiff INT
--		SET @DateDiff=DateDiff(day,@ATLastAcquireDate,GetDate())

--		-- 调整次数
--		IF @DateDiff<>0 SET @ATAuthTimes=0

--		-- 更新数据
--		UPDATE AccountsAuthInfo SET AuthTimes=@ATAuthTimes+1,AcquireTimes=0,LastAcquireDate=GetDate(),LastMobilePhone=N'' WHERE UserID=@UserID
--	END

--	-- 更新次数
--	UPDATE RecordAuthCode SET AuthCode=N'',AuthCodeMD5=N'',AuthTimes=AuthTimes+1,AcquireTimes=0 WHERE MobilePhone=@strMobilePhone

--	-- 更新账号
--	--UPDATE LHInsureDB.dbo.AgentReceiptInfo SET AccountName=@strAccountsName,PhoneNum=@strMobilePhone,CollectDate=GetDate() WHERE UserID=@UserID	
--	--IF @@ROWCOUNT=0
--	--BEGIN
--	--	INSERT INTO LHInsureDB.dbo.AgentReceiptInfo(UserID,AccountName,PhoneNum,CollectDate)
--	--	VALUES (@dwUserID,@strAccountsName,@strMobilePhone,GetDate())
--	--END

--	-- 计算日期
--	DECLARE @DateID INT
--	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

--	-- 记录验证次数
--	UPDATE RecordAuthTimes SET AuthTimes=AuthTimes+1 WHERE DateID=@DateID AND UserID=@dwUserID AND AuthKind=@AuthKind
--	IF @@ROWCOUNT=0
--	BEGIN
--		-- 插入记录
--		INSERT INTO RecordAuthTimes(DateID,UserID,AuthKind,AuthTimes) VALUES (@DateID,@dwUserID,@AuthKind,1)
--	END
	
--	-- 设置成功
--	SET @strErrorDescribe=N'{"Idx":1277,"Msg":"收款账号设置成功！"}'	
--END

--RETURN 0

--GO

----------------------------------------------------------------------------------------------------

