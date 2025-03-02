
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_WealthTransfer]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_WealthTransfer]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 货币转账
CREATE PROC GSP_MB_WealthTransfer
	@dwUserID INT,								-- 用户 I D
	@dwTargetGameID INT,						-- 游戏 I D
	@cbCodeKind TINYINT,						-- 验证码类型
	@strAuthCode NCHAR(32),						-- 验证码	
	@lTransferAmount INT,						-- 转账金额
	@wReason INT,								-- 变更原因
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器序列
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
	DECLARE @NickName NVARCHAR(32)
	DECLARE @Email NVARCHAR(64)
	DECLARE @MobilePhone NVARCHAR(21)
	SELECT @UserID=a.UserID,@GameID=a.GameID,@NickName=a.NickName,@Email=a.Email,@MobilePhone=a.MobilePhone, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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

	-- 查询用户
	DECLARE @TargetUserID INT
	DECLARE @TargetGameID INT
	DECLARE @TargetNickName NVARCHAR(32)	
	SELECT @TargetUserID=a.UserID, @TargetNickName=a.NickName, @TargetGameID=a.GameID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.GameID=@dwTargetGameID

	-- 校验用户
	IF @TargetUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1036,"Msg":"抱歉,目标帐号不存在。"}'
		RETURN 4
	END	

	-- 校验用户
	IF @TargetUserID = @dwUserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1037,"Msg":"抱歉,同一帐号不允许进行金币转账。"}'
		RETURN 5
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1038,"Msg":"抱歉！目标帐号暂时处于冻结状态，无法进行金币转账。"}'
		RETURN 6
	END	

	-- 帐号关闭
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1039,"Msg":"抱歉！目标帐号使用了安全关闭功能，无法进行金币转账。"}'
		RETURN 7
	END	

	-- 变量定义
	DECLARE @MobileEmail NVARCHAR(64)

	-- 设置变量
	IF @cbCodeKind=2 SET @MobileEmail=@Email
	IF @cbCodeKind=1 SET @MobileEmail=@MobilePhone	
	
	-- 验证信息
	DECLARE @BindUserID INT
	DECLARE @AuthKind TINYINT
	DECLARE @AuthCodeMD5 NVARCHAR(32)
	DECLARE @AcquireMachine NVARCHAR(32)	
	DECLARE @LastAcquireDate DATETIME
	SELECT @BindUserID=UserID, @AuthKind=AuthKind, @AuthCodeMD5=AuthCodeMD5, @AcquireMachine=AcquireMachine, @LastAcquireDate=LastAcquireDate FROM RecordAuthCode(NOLOCK) WHERE CodeKind=@cbCodeKind AND MobileEmail=@MobileEmail

	-- 验证码不存在
	IF @BindUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1027,"Msg":"验证码错误！"}'
		RETURN 8
	END

	-- 用户不匹配
	IF @BindUserID<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1027,"Msg":"验证码错误！"}'
		RETURN 9
	END

	-- 校验类型
	IF @AuthKind<>10
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1027,"Msg":"验证码错误！"}'
		RETURN 10		
	END
	
	-- 校验验证码
	IF @AuthCodeMD5<>@strAuthCode
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1027,"Msg":"验证码错误！"}'
		RETURN 11				
	END

	-- 校验机器码
	IF @AcquireMachine<>@strMachineID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1028,"Msg":"您的机器码发生异常！"}'
		RETURN 12
	END

	-- 校验状态
	IF EXISTS(SELECT 1 FROM AccountsOnlineInfo(NOLOCK) WHERE UserID=@dwUserID AND ServerID<>0)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1052,"Msg":"系统检测到您的账号正在游戏房间中，请先退出房间再进行转账！"}'
		RETURN 13
	END

	-- 有效时长
	DECLARE @ACValidSeconds INT
	SELECT @ACValidSeconds=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ACValidSecond'

	-- 调整参数
	IF @ACValidSeconds IS NULL SET @ACValidSeconds=600

	-- 时间间隔
	DECLARE @SecondDiff INT
	SET @SecondDiff=DateDiff(s,@LastAcquireDate,GETDATE())

	-- 时间限制
	IF @SecondDiff>@ACValidSeconds	
	BEGIN			
		SET @strErrorDescribe=N'{"Idx":1029,"Msg":"验证码已过期，请重新获取验证码！"}'
		RETURN 14
	END
	
	-- 转账配置
	DECLARE @TransferEnabled TINYINT	
	DECLARE @ServiceFeeRate SMALLINT	
	DECLARE @ReservedAmount INT
	DECLARE @MinTransferAmount INT
	DECLARE @MaxTransferAmount INT

	-- 查询配置
	SELECT @TransferEnabled=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TransferOption' AND StatusName=N'TransferEnabled'
	SELECT @ServiceFeeRate=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TransferOption' AND StatusName=N'ServiceFeeRate'
	SELECT @ReservedAmount=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TransferOption' AND StatusName=N'ReservedAmount'
	SELECT @MinTransferAmount=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TransferOption' AND StatusName=N'MinTransferAmount'
	SELECT @MaxTransferAmount=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TransferOption' AND StatusName=N'MaxTransferAmount'
	
	-- 调整配置
	IF @ReservedAmount IS NULL SET @ReservedAmount=10000
	IF @TransferEnabled IS NULL SET @TransferEnabled=1
	IF @ServiceFeeRate IS NULL SET @ServiceFeeRate=30
	IF @MinTransferAmount IS NULL SET @MinTransferAmount=10000
	IF @MaxTransferAmount IS NULL SET @MaxTransferAmount=1000000000

	-- 校验开关
	IF @TransferEnabled=0
	BEGIN
		-- 错误信息
		SET @strErrorDescribe=N'{"Idx":1035,"Msg":"抱歉，转账功能已关闭。"}'	
		RETURN 15
	END

	-- 校验金额
	IF @lTransferAmount<@MinTransferAmount
	BEGIN
		-- 错误信息
		SET @strErrorDescribe=N'{"Idx":1040,"Msg":"抱歉, 每笔转账金额不能小于%s。","Ops":["'+CONVERT(NVARCHAR(16),@MinTransferAmount/1000)+'"]}'	
		RETURN 16
	END

	-- 校验金额
	IF @lTransferAmount>@MaxTransferAmount
	BEGIN
		-- 错误信息
		SET @strErrorDescribe=N'{"Idx":1041,"Msg":"抱歉, 每笔转账金额不能大于%s。","Ops":["'+CONVERT(NVARCHAR(16),@MaxTransferAmount/1000)+'"]}'	
		RETURN 17
	END

	-- 变量定义
	DECLARE @MailID INT
	DECLARE @MailTitle NVARCHAR(128)
	DECLARE @MailContent NVARCHAR(512)

	-- 邮件内容
	SET @MailTitle =N'Coin transfer notice'
	SET @MailContent=N'{"Ops":["'+@TargetNickName+'","'+@NickName+'","'+CONVERT(NVARCHAR(18),@GameID)+'","'+CONVERT(NVARCHAR(16),@lTransferAmount/1000)+'","'+ CONVERT(VARCHAR(100),GETDATE(),109)+'"]}'

	-- 插入邮件信息
	INSERT INTO AccountsMailInfo(MailKind,MailType,MailTitle,MailContent,SendTime,ExpireTime,CollectTime)
	VALUES (2,2,@MailTitle,@MailContent,dbo.LHF_GetTimestamp(),0,GetDate())

	-- 邮件标识
	SET @MailID=SCOPE_IDENTITY()	

	-- 插入附件
	INSERT INTO AccountsMailAttach(MailID,GoodsID,GoodsCount,GoodsIndate) VALUES (@MailID,2,@lTransferAmount,0)			

	-- 服务费用
	DECLARE @ServiceFee INT
	DECLARE @DeductAmount INT

	-- 计算费用
	SET @ServiceFee = @lTransferAmount*@ServiceFeeRate/1000
	SET @DeductAmount = -(@lTransferAmount+@ServiceFee)

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 查询金币
	DECLARE @Gold BIGINT
	SELECT @Gold=Score FROM LHGameGoldDB.dbo.GameScoreInfo WITH(UPDLOCK) WHERE UserID=@dwUserID

	-- 校验余额
	IF @Gold<@ReservedAmount+@lTransferAmount+@ServiceFee
	BEGIN
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 错误信息
		SET @strErrorDescribe=N'{"Idx":1043,"Msg":"抱歉！您身上的金币余额不足，转账失败。"}'	
		RETURN 18
	END	

	-- 写入财富
	EXEC GSP_GP_WriteGameWealth @dwUserID,2,@wReason,@DeductAmount

	-- 插入用户邮箱
	INSERT INTO AccountsMailBox(UserID,MailID,MailState,ExpireTime,UpdateTime) VALUES (@TargetUserID,@MailID,1,0,GetDate())
	
	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 转账记录
	INSERT INTO LHGameRecordDB.dbo.RecordWealthTransfer(UserID,NickName,GameID,TargetUserID,TargetNickName,TargetGameID,ServiceFee,TransferAmount,CollectClientIP,CollectMachineID,CollectDateTime)
	VALUES (@UserID,@NickName,@GameID,@TargetUserID,@TargetNickName,@TargetGameID,@ServiceFee,@lTransferAmount,@strClientIP,@strMachineID,GETDATE())

	-- 更新信息
	DECLARE @ATAuthTimes SMALLINT
	DECLARE @ATLastAcquireDate DATETIME
	SELECT @ATAuthTimes=AuthTimes,@ATLastAcquireDate=LastAcquireDate FROM AccountsAuthInfo(NOLOCK) WHERE UserID=@UserID AND CodeKind=@cbCodeKind

	-- 插入数据
	IF @ATAuthTimes IS NULL
	BEGIN
		INSERT INTO AccountsAuthInfo(UserID,CodeKind,AuthTimes,AcquireTimes,LastAcquireDate,LastMobileEmail)	VALUES (@UserID,@cbCodeKind,1,0,GETDATE(),N'')		
	END ELSE
	BEGIN
		-- 时间间隔
		DECLARE @DateDiff INT
		SET @DateDiff=DateDiff(day,@ATLastAcquireDate,GETDATE())

		-- 调整次数
		IF @DateDiff<>0 SET @ATAuthTimes=0

		-- 更新数据
		UPDATE AccountsAuthInfo SET AuthTimes=@ATAuthTimes+1,AcquireTimes=0,LastAcquireDate=GETDATE(),LastMobileEmail=N'' WHERE UserID=@UserID AND CodeKind=@cbCodeKind
	END

	-- 更新次数
	UPDATE RecordAuthCode SET AuthCode=N'',AuthCodeMD5=N'',AuthTimes=AuthTimes+1,AcquireTimes=0 WHERE CodeKind=@cbCodeKind AND MobileEmail=@MobileEmail

	-- 转换日期
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)	

	-- 记录验证次数
	UPDATE RecordAuthTimes SET AuthTimes=AuthTimes+1 WHERE DateID=@DateID AND UserID=@UserID AND CodeKind=@cbCodeKind AND AuthKind=@AuthKind
	IF @@ROWCOUNT=0
	BEGIN
		-- 插入记录
		INSERT INTO RecordAuthTimes(DateID,UserID,CodeKind,AuthKind,AuthTimes) VALUES (@DateID,@UserID,@cbCodeKind,@AuthKind,1)
	END
		
	-- 转账成功
	SET @strErrorDescribe=N'{"Idx":1051,"Msg":"转账成功！"}'
	   
	-- 抛出数据
	SELECT @MailID AS MailID, @TargetUserID AS TargetUserID
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

