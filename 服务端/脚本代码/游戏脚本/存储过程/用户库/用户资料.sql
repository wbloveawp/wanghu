
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ModifyAccountsInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ModifyAccountsInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryUserIndividual]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryUserIndividual]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ModifyUserIndividual]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ModifyUserIndividual]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_ModifyUserIndividual]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_ModifyUserIndividual]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 修改帐号
CREATE PROC GSP_GP_ModifyAccountsInfo
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@strAccounts NVARCHAR(32),					-- 用户帐号
	@strUnderWrite NVARCHAR(63),				-- 个性签名
	@wFaceID SMALLINT,							-- 头像标识
	@cbAge TINYINT,								-- 用户年龄
	@cbGender TINYINT,							-- 用户性别
	@strProvince NVARCHAR(15),					-- 用户省份
	@strCity NVARCHAR(15),						-- 用户城市
	@strArea NVARCHAR(15),						-- 用户区域
	@bOldVersion TINYINT,						-- 是否旧版
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 变量定义
	DECLARE @UserID INT
	DECLARE @Accounts NVARCHAR(31)
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @LogonPass AS NCHAR(32)

	-- 查询用户
	SELECT @UserID=a.UserID,@Accounts=a.Accounts,@LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
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
		RETURN 2
	END	
	
	-- 密码判断
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 3
	END
	
	-- 效验名字
	IF (SELECT COUNT(*) FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strAccounts)>0)>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1032,"Msg":"抱歉地通知您，您所输入的昵称含有限制字符串，请更换昵称后尝试再次修改！"}'
		RETURN 4
	END

	-- 校验名字
	IF @strAccounts NOT LIKE '%[^0-9]%' AND LEN(@strAccounts)=16
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1032,"Msg":"抱歉地通知您，您所输入的昵称含有限制字符串，请更换昵称后尝试再次修改！"}'
		RETURN 116
	END

	-- 存在判断
	IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE NickName=@strAccounts AND UserID<>@dwUserID)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1033,"Msg":"此昵称已被其他玩家注册了，请换另一昵称尝试再次修改！"}'
		RETURN 4
	END

	-- 更新资料
	UPDATE AccountsInfo SET UnderWrite=@strUnderWrite, FaceID=@wFaceID,  Gender=@cbGender WHERE UserID=@dwUserID

	-- 更新资料
	UPDATE AccountsDatum SET Age=@cbAge,Province=@strProvince, City=@strCity, Area=@strArea WHERE UserID=@dwUserID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO AccountsDatum(UserID,Age,Area,City,Province) VALUES(@dwUserID,@cbAge,@strArea,@strCity,@strProvince)
	END

	SET @strErrorDescribe=N'{"Idx":1034,"Msg":"服务器已经接受了您的新资料！"}'	

	RETURN 0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 查询资料
CREATE PROC GSP_GP_QueryUserIndividual
	@dwUserID INT,								-- 用户 I D
	@strClientIP NVARCHAR(15)					-- 连接地址
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 变量定义
	DECLARE @UserID INT
	DECLARE @QQ NVARCHAR(16)
	DECLARE @EMail NVARCHAR(33)
	DECLARE @UserNote NVARCHAR(256)
	DECLARE @SeatPhone NVARCHAR(32)
	DECLARE @MobilePhone NVARCHAR(16)
	DECLARE @Compellation NVARCHAR(16)
	DECLARE @DwellingPlace NVARCHAR(128)

	-- 查询用户
	SELECT @UserID=UserID, @QQ=QQ, @EMail=EMail, @UserNote=UserNote, @SeatPhone=SeatPhone,
		@MobilePhone=MobilePhone, @Compellation=Compellation, @DwellingPlace=DwellingPlace
	FROM AccountsDatum(NOLOCK) WHERE UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @QQ=N''	
		SET @EMail=N''	
		SET @UserNote=N''	
		SET @SeatPhone=N''	
		SET @MobilePhone=N''	
		SET @Compellation=N''	
		SET @DwellingPlace=N''	
	END

	-- 输出信息
	SELECT @dwUserID AS UserID, @Compellation AS Compellation, @QQ AS QQ, @EMail AS EMail, @SeatPhone AS SeatPhone,
		@MobilePhone AS MobilePhone, @DwellingPlace AS DwellingPlace, @UserNote AS UserNote

	RETURN 0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 更新资料
CREATE PROC GSP_GP_ModifyUserIndividual
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@strCompellation NVARCHAR(16),				-- 真实名字
	@strQQ NVARCHAR(16),						-- Q Q 号码
	@strEMail NVARCHAR(33),						-- 电子邮电
	@strSeatPhone NVARCHAR(32),					-- 固定电话
	@strMobilePhone NVARCHAR(16),				-- 移动电话
	@strDwellingPlace NVARCHAR(128),			-- 详细地址
	@strUserNote NVARCHAR(256),					-- 用户备注				
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 变量定义
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @LogonPass AS NCHAR(32)

	-- 查询用户
	SELECT @UserID=a.UserID, @LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
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
		RETURN 2
	END	
	
	-- 密码判断
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 3
	END

	-- 修改资料
	UPDATE AccountsDatum SET Compellation=@strCompellation, QQ=@strQQ, EMail=@strEMail, SeatPhone=@strSeatPhone,
		MobilePhone=@strMobilePhone, DwellingPlace=@strDwellingPlace, UserNote=@strUserNote WHERE UserID=@dwUserID
		
	-- 修改资料
	IF @@ROWCOUNT=0
	INSERT AccountsDatum (UserID, Compellation, QQ, EMail, SeatPhone, MobilePhone, DwellingPlace, UserNote)
	VALUES (@dwUserID, @strCompellation, @strQQ, @strEMail, @strSeatPhone, @strMobilePhone, @strDwellingPlace, @strUserNote)

	-- 设置信息
	IF @@ERROR=0 SET @strErrorDescribe=N'{"Idx":1034,"Msg":"服务器已经接受了您的新资料！"}'	

	RETURN 0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 修改资料
CREATE PROC GSP_MB_ModifyUserIndividual
	@dwUserID INT,								-- 用户 I D
	@cbGender TINYINT,							-- 用户性别	
	@wInfoMask INT,								-- 信息掩码
	@strNickName NVARCHAR(32),					-- 用户昵称
	@strUnderWrite NVARCHAR(63),				-- 个性签名	
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 变量定义
	DECLARE @UserID INT
	DECLARE @Gender TINYINT
	DECLARE @NickName NVARCHAR(31)
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @ActionMaskEver INT
	DECLARE @ActionMaskPerDay INT
	DECLARE @ActionMaskPerWeek INT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @UnderWrite NVARCHAR(63)	

	-- 查询用户
	SELECT @UserID=a.UserID,@Gender=a.Gender,@NickName=a.NickName,@UnderWrite=a.UnderWrite,@LogonPass=a.LogonPass, @ActionMaskEver=a.ActionMaskEver,@ActionMaskPerDay=a.ActionMaskPerDay,
		@ActionMaskPerWeek=a.ActionMaskPerWeek,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
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
		RETURN 2
	END	

	-- 性别修改
	IF @Gender<>@cbGender
	BEGIN
		-- 校验掩码
		--IF (@ActionMaskEver&0x01)<>0
		--BEGIN
		--	SET @strErrorDescribe=N'{"Idx":1277,"Msg":"抱歉，性别信息系统暂时只允许每个玩家修改一次，您已经修改过了！"}'
		--	RETURN 3
		--END

		-- 设置掩码
		SET @ActionMaskEver|=0x01
	END
	
	-- 修改昵称
	IF (@wInfoMask&0x0004)<>0
	BEGIN
		DECLARE @DateID INT
		SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

		-- 读取配置
		DECLARE @szModifyNickNameOption NVARCHAR(MAX)
		SELECT @szModifyNickNameOption=StatusValue FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ModifyNickNameOption'
		--
		DECLARE @DailyModify INT
		DECLARE @TotalModify INT
		SELECT @DailyModify=DailyModify,@TotalModify=TotalModify FROM RecordModifyUserIndividual WHERE DateID=@DateID AND UserID=@dwUserID AND ActionMask=0x0002
		IF @DailyModify IS NULL
		BEGIN
			SET @DailyModify = 0
			SELECT @TotalModify = COUNT(1) FROM RecordModifyUserIndividual WHERE UserID=@dwUserID AND ActionMask=0x0002
			IF @TotalModify IS NULL SET @TotalModify=0
			INSERT INTO RecordModifyUserIndividual (DateID,UserID,ActionMask,DailyModify,TotalModify,CollectDate) VALUES(@DateID,@dwUserID,0x0002,0,@TotalModify,GETDATE())
				
		END

		DECLARE @MoneyID INT
		DECLARE @MoneyAmount INT
		DECLARE @FreeTimesPerAccounts INT
		DECLARE @MaxTimesPerDay INT
		SET @MoneyID = JSON_VALUE(@szModifyNickNameOption,'$.MoneyID')
		SET @MoneyAmount = JSON_VALUE(@szModifyNickNameOption,'$.MoneyAmount')
		SET @FreeTimesPerAccounts = JSON_VALUE(@szModifyNickNameOption,'$.FreeTimesPerAccounts')
		SET @MaxTimesPerDay = JSON_VALUE(@szModifyNickNameOption,'$.MaxTimesPerDay')
		IF @DailyModify >= @MaxTimesPerDay
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1329,"Msg":"修改次数已达上限"}'
			RETURN 4
		END
		IF @MoneyAmount > 0 AND @TotalModify >= @FreeTimesPerAccounts
			BEGIN
			SET @MoneyAmount = -@MoneyAmount
			-- 写入财富
			DECLARE @ResultCode INT
			EXEC @ResultCode=GSP_GP_WriteGameWealth @dwUserID,@MoneyID,17,@MoneyAmount

			-- 校验结果
			IF @ResultCode<>0
			BEGIN
				SET @strErrorDescribe=N'{"Idx":1321,"Msg":"您身上的黄金余额不足'+CONVERT(NVARCHAR(12),-@MoneyAmount)+'，修改昵称失败！","Ops":["'+CONVERT(NVARCHAR(12),-@MoneyAmount)+'"]}'
				RETURN 4
			END
		END

		-- 存在判断
		IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE NickName=@strNickName AND UserID<>@dwUserID)
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1033,"Msg":"此昵称已被其他玩家注册了，请换另一昵称尝试再次修改！"}'
			RETURN 4
		END

		-- 设置昵称
		SET @NickName = @strNickName

		-- 设置掩码
		SET @ActionMaskEver|=0x0002

		-- 更新记录
		UPDATE RecordModifyUserIndividual SET DateID=@DateID,DailyModify=DailyModify+1,TotalModify=TotalModify+1,CollectDate=GETDATE() WHERE DateID=@DateID AND UserID=@dwUserID AND ActionMask=0x0002
	END

	-- 修改签名
	IF (@wInfoMask&0x0080)<>0
	BEGIN
		SET @UnderWrite=@strUnderWrite
	END

	-- 修改资料
	UPDATE AccountsInfo SET Gender=@cbGender,NickName=@NickName,UnderWrite=@UnderWrite,ActionMaskEver=@ActionMaskEver WHERE UserID=@dwUserID
	
	-- 设置信息
	IF @@ERROR=0 
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1034,"Msg":"服务器已经接受了您的新资料！"}'	
	END

	-- 读取黄金
	DECLARE @UserGold BIGINT
	SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 调整变量
	IF @UserGold IS NULL SET @UserGold=0

	-- 抛出数据
	SELECT @ActionMaskEver AS ActionMaskEver, @ActionMaskPerDay AS ActionMaskPerDay, @ActionMaskPerWeek AS ActionMaskPerWeek,@UserGold AS UserGold

	RETURN 0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
