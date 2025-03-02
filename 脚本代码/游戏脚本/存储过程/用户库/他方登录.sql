/***************************************************************************************************
** 名称：帐号登录 (游戏)
***************************************************************************************************/
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_EfficacyThirdParty]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_EfficacyThirdParty]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 第三方登陆
CREATE PROC GSP_MB_EfficacyThirdParty
	@wGateID INT,								-- 网关标识	
	@wLogonID INT,								-- 登录标识
	@dwSocketID INT,							-- 网络标识
	@dwSpreaderID INT,							-- 代理ID
	@dwExternalID INT,							-- 他方标识
	@strUniqueID NVARCHAR(32),			        -- 用户Uin
	@strNickName NVARCHAR(31),					-- 用户昵称
	@strUserDatum NVARCHAR(256),				-- 用户资料
	@strCompellation NVARCHAR(16),				-- 真实名字
	@cbGender TINYINT,							-- 用户性别	
	@wMarketID SMALLINT,						-- 渠道标识
	@dwDeviceKind INT,							-- 设备类型
	@dwPlazaVersion INT,						-- 大厅版本
	@strClientIP NVARCHAR(15),					-- 连接地址	
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strMachineOSID NVARCHAR(64),				-- 系统版本
	@strMobilePhone NVARCHAR(21),				-- 手机号码	
	@strPackageName NVARCHAR(64),				-- 应用包名
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID			INT
DECLARE @FaceID			SMALLINT
DECLARE @Accounts		NVARCHAR(31)
DECLARE @NickName		NVARCHAR(31)
DECLARE @UnderWrite		NVARCHAR(63)
DECLARE @CustomID		INT
DECLARE @PasswordID		INT
DECLARE @UserRight		INT
DECLARE @ServiceRight	INT

-- 扩展信息
DECLARE @GameID INT
DECLARE @ProtectID INT
DECLARE @MasterOrder INT
DECLARE @LoveLiness INT
DECLARE @Experience INT
DECLARE @LikedNumber INT
DECLARE @InfoPublic  TINYINT
DECLARE @MemberPoint INT
DECLARE @MemberOverTime INT

-- 用户信息
DECLARE @Gender TINYINT

-- 分数信息
DECLARE @UserGold BIGINT
DECLARE @UserCard BIGINT
DECLARE @InsureGold BIGINT
DECLARE @Commission BIGINT

-- 行为掩码
DECLARE @ActionMaskEver INT
DECLARE @ActionMaskPerDay INT
DECLARE @ActionMaskPerWeek INT

-- 辅助变量
DECLARE @EnjoinLogon AS INT
DECLARE @StatusString NVARCHAR(128)

-- 代理信息
DECLARE @ParentUserID INT
DECLARE @ParentGameID INT
DECLARE @ParentFaceID INT
DECLARE @ParentNickName NVARCHAR(31)

--实名信息
DECLARE @szCompellation NVARCHAR(32)
DECLARE @szPassPortID NVARCHAR(32)

--注册赠送
DECLARE @szPresentGoodsList NVARCHAR(MAX)

-- 执行逻辑
BEGIN
	-- 系统暂停
	SELECT @EnjoinLogon=CONVERT(INT,StatusValue),@StatusString=StatusString FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		IF dbo.LHF_IsAccountExcept(@strClientIP, @strMachineID)<>0
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1002,"Msg":"'+@StatusString+N'"}'
			RETURN 1
		END

		SET @EnjoinLogon=0	
	END

	-- 效验地址
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1019,"Msg":"抱歉地通知您，系统禁止了您所在的 IP 地址的登录功能，请联系客户服务中心了解详细情况！"}'
		RETURN 4
	END
	
	-- 效验机器
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	
	-- 校验机器
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN					
		SET @strErrorDescribe=N'{"Idx":1020,"Msg":"抱歉地通知您，系统禁止了您的机器的登录功能，请联系客户服务中心了解详细情况！"}'
		RETURN 7
	END	
 
	-- 用户信息
	DECLARE @Nullity BIT
	DECLARE @ShutUpStatus BIT
	DECLARE @ShutDownStatus BIT
	DECLARE @GameLogonTimes INT 
	DECLARE @LogonPass NCHAR(32)
	DECLARE @InsurePass NVARCHAR(32)
	DECLARE	@MachineID NCHAR(32)
	DECLARE	@Email NVARCHAR(64)
	DECLARE	@MobilePhone NVARCHAR(21)
	DECLARE @PlazaVersion INT	
	DECLARE @ShutupOverTime INT
	DECLARE @ShutDownOverTime INT
	DECLARE @RegisterTime INT
	DECLARE @RegisterMode SMALLINT
	DECLARE	@LogonDateTime DATETIME
	DECLARE	@ResetDateTime DATETIME
	DECLARE @RegisterDeviceKind SMALLINT

	-- 日期标识
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 注册赠送
	DECLARE @PresentGold BIGINT
	DECLARE @PresentCard BIGINT

	-- 用户查询
	SELECT @UserID=UserID FROM AccountsExternal(NOLOCK) WHERE UniqueID=@strUniqueID AND ExternalID=@dwExternalID	

	IF @UserID IS NULL
	BEGIN
		
		-- 设置前缀
		IF LEN(@strNickName)=0
		BEGIN
			-- IOS、IPAD
			IF (@dwDeviceKind&0x60)<>0 SET @strNickName=N'IPhone'

			-- ANDROID
			IF (@dwDeviceKind&0x80)<>0 SET @strNickName=N'Android'
		END
		
		-- 设置账号
		SET @Accounts=@strNickName
	
		-- 调整账号
		EXEC dbo.LHP_AdjustmentAccounts @Accounts OUTPUT,@strNickName OUTPUT		
		
		-- 随机生成密码
		SET @LogonPass=CONVERT(nvarchar(32),REPLACE(newid(),'-',''))	

		-- 注册用户
		INSERT AccountsInfo (Accounts,NickName,RegAccounts,LogonPass,InsurePass,Gender,FaceID,LogonMachineID, RegisterMode,RegisterMachineID,RegisterDeviceKind,RegisterPackageName,GameLogonTimes,
				RegisterIP,LogonIP,PlazaVersion,MarketID,ParentID)
		VALUES (@Accounts,@strNickName,@Accounts,@LogonPass,N'',@cbGender,CAST(RAND()*10 AS INT)+1,@strMachineID,5,@strMachineID,@dwDeviceKind,@strPackageName,0,
				@strClientIP,@strClientIP,@dwPlazaVersion,@wMarketID,0)

		-- 错误判断
		IF @@ERROR<>0
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1009,"Msg":"帐号已存在，请换另一帐号名字尝试再次注册！"}'
			RETURN 8
		END

		-- 设置用户
		SET @UserID=SCOPE_IDENTITY()
		INSERT AccountsExternal (UserID, UniqueID, ExternalID, UserDatum, UserFacePath, NickName, Gender, LogonClientIP) 
		VALUES (@UserID, @strUniqueID, @dwExternalID, @strUserDatum, N'', @strNickName, @cbGender, @strClientIP)

		-- 错误判断
		IF @@ERROR<>0
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1009,"Msg":"帐号已存在，请换另一帐号名字尝试再次注册！"}'
			RETURN 1
		END

		-- 查询用户
		SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts,@NickName=NickName,@LogonPass=LogonPass,@InsurePass=InsurePass,@FaceID=FaceID,@CustomID=CustomID,@ProtectID=ProtectID, @MasterOrder=MasterOrder,
			   @Gender=Gender, @Experience=Experience, @LoveLiness=LoveLiness, @UnderWrite=UnderWrite, @MemberPoint=MemberPoint,@GameLogonTimes=GameLogonTimes, @MemberOverTime=MemberOverTime, @InfoPublic=InfoPublic,
			   @RegisterMode=RegisterMode,@RegisterDeviceKind=RegisterDeviceKind,@ActionMaskEver=ActionMaskEver,@ActionMaskPerDay=ActionMaskPerDay,@LogonDateTime=LogonDate, @ResetDateTime=ResetDateTime, @ActionMaskPerWeek=ActionMaskPerWeek,
			   @UserRight=UserRight,@LikedNumber=LikedNumber,@MobilePhone=MobilePhone,@Email=EMail,@RegisterMode=@RegisterMode,@RegisterTime=dbo.LHF_ConvertTimestamp(RegisterDate)
		FROM AccountsInfo(NOLOCK) WHERE UserID=@UserID
		
		-- 分配标识
		SELECT @GameID=GameID FROM GameIdentifier(NOLOCK) WHERE UserID=@UserID
		IF @GameID IS NULL 
		BEGIN
			SET @GameID=0
			SET @strErrorDescribe=N'{"Idx":1010,"Msg":"用户注册成功，但未成功获取游戏 ID 号码，系统稍后将给您分配！"}'
		END ELSE 
		BEGIN
			UPDATE AccountsInfo SET GameID=@GameID WHERE UserID=@UserID
		END	
		
		-- 查询用户
		DECLARE @ParentParentID INT
		SELECT @ParentUserID=UserID,@ParentParentID=ParentID FROM AccountsInfo(NOLOCK) WHERE GameID=@dwSpreaderID
		--分享绑定
		IF @ParentUserID IS NULL OR @ParentUserID = 0
		BEGIN
			SELECT @ParentUserID=GameID FROM BindIP WHERE UserIP=@strClientIP
			SELECT @ParentUserID=UserID FROM AccountsInfo(NOLOCK) WHERE GameID=@ParentUserID
			DELETE FROM BindIP WHERE UserIP=@strClientIP
		END
		-- 调整标识
		IF @ParentUserID IS NULL OR @ParentParentID=@UserID SET @ParentUserID=0

		-- 绑定上级
		IF @ParentUserID<>0
		BEGIN		
			DECLARE @MailID BIGINT
			EXEC GSP_MB_BindParent @UserID,@ParentUserID,@strClientIP,@MailID OUTPUT
		END

		-- 代理配置		
		DECLARE @AgentLevel NVARCHAR(256)	
		SELECT @AgentLevel=StatusValue FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'AgentOption' AND StatusName=N'AgentLevel_0'

		-- 校验配置
		IF ISJSON(@AgentLevel)>0
		BEGIN
			INSERT INTO AccountsAgentInfo(UserID,AgentLevel,LowerBetAwardRate,LowerPayAwardAmount,LLowerBetAwardRate,LLowerPayAwardAmount)
			SELECT @UserID,0,ISNULL(JSON_VALUE(@AgentLevel,N'$.LowerBetAwardRate'),0),ISNULL(JSON_VALUE(@AgentLevel,N'$.LowerPayAwardAmount'),0),ISNULL(JSON_VALUE(@AgentLevel,N'$.LLowerBetAwardRate'),0),ISNULL(JSON_VALUE(@AgentLevel,N'$.LLowerPayAwardAmount'),0)			
		END

		-- 注册统计
		UPDATE SystemStreamRegister SET TotalCount+=1,LastDate=GETDATE()
		WHERE DateID=@DateID AND DeviceKind=@dwDeviceKind AND ExternalID=@dwExternalID
		IF @@ROWCOUNT=0
		BEGIN
			INSERT INTO SystemStreamRegister(DateID,DeviceKind,ExternalID,TotalCount) VALUES(@DateID,@dwDeviceKind,@dwExternalID,1)
		END
		
		-- 记录日志
		UPDATE SystemStreamInfo SET GameRegisterSuccess=GameRegisterSuccess+1 WHERE DateID=@DateID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID,GameRegisterSuccess) VALUES (@DateID, 1)		

		-- 金币查询
		SELECT @UserGold=Score,@InsureGold=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@UserID
		IF @UserGold IS NULL OR @InsureGold IS NULL
		BEGIN
			SELECT @UserGold=0,@InsureGold = 0

			INSERT LHGameGoldDB.dbo.GameScoreInfo (UserID, Score, InsureScore) VALUES (@UserID, @UserGold, @InsureGold) 
		END

		--------------------------------------注册赠送-----------------------------------------------------
		EXEC GSP_MB_RegisterPresent @UserID,5,@strClientIP,@strMachineID,@szPresentGoodsList OUTPUT

		--------------------------------------注册赠送-----------------------------------------------------

	END	ELSE
	BEGIN
		
		-- 查询用户
		SELECT @UserID=a.UserID, @GameID=a.GameID, @Accounts=a.Accounts, @NickName=a.NickName, @UnderWrite=a.UnderWrite, @LogonPass=a.LogonPass, @InsurePass=a.InsurePass, @FaceID=a.FaceID,@CustomID=a.CustomID,
			@ProtectID=a.ProtectID,@MasterOrder=a.MasterOrder,@Gender=a.Gender, @Nullity=a.Nullity, @Experience=a.Experience, @LoveLiness=a.LoveLiness, @GameLogonTimes=a.GameLogonTimes, @MemberPoint=a.MemberPoint,
			@MemberOverTime=a.MemberOverTime, @MachineID=a.LogonMachineID, @PlazaVersion=a.PlazaVersion,@LikedNumber=a.LikedNumber, @MobilePhone=MobilePhone, @Email=a.EMail, @RegisterMode=a.RegisterMode, 
			@InfoPublic=a.InfoPublic,@RegisterDeviceKind=a.RegisterDeviceKind,@ActionMaskEver=a.ActionMaskEver,@ActionMaskPerDay=a.ActionMaskPerDay, @ActionMaskPerWeek=a.ActionMaskPerWeek,@ParentUserID=a.ParentID,
			@ShutUpStatus=b.ShutupStatus,@ShutDownStatus=b.ShutDownStatus,@ShutupOverTime=b.ShutupOverTime,@ShutDownOverTime=b.ShutDownOverTime,@RegisterMode=a.RegisterMode, @LogonDateTime=a.LogonDate,@ResetDateTime=a.ResetDateTime,
			@UserRight=a.UserRight,@RegisterTime=dbo.LHF_ConvertTimestamp(a.RegisterDate)
		FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@UserID
	END

	-- 调整变量
	IF @Email IS NULL SET @Email=N''
	IF @MobilePhone IS NULL SET @MobilePhone=N''
	IF @PresentGold IS NULL SET @PresentGold=0
	IF @PresentCard IS NULL SET @PresentCard=0
	IF @ShutUpStatus IS NULL SET @ShutUpStatus=0
	IF @ShutDownStatus IS NULL SET @ShutDownStatus=0
	IF @ShutUpOverTime IS NULL SET @ShutUpOverTime=0
	IF @ShutDownOverTime IS NULL SET @ShutDownOverTime=0
	IF @InsurePass IS NULL SET @InsurePass=N''
	
	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
		RETURN 2
	END	

	-- 帐号关闭
	IF @ShutDownStatus<>0
	BEGIN
		-- 更新状态
		IF dbo.LHF_GetTimestamp()<@ShutDownOverTime
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1136,"Msg":"当前帐号已被临时封禁至%s（解封时间）, 暂无法使用。如有其他疑问，请联系客服。","Ops":["'+CONVERT(NVARCHAR(100),LHAccountsDB.dbo.LHF_ConvertTimestampToLocalTime(@ShutDownOverTime),120)+'"]}'	
			RETURN 2
		END			

		-- 更新状态		
		UPDATE AccountsStatus SET ShutDownStatus=0,ShutDownOverTime=0 WHERE UserID=@UserID
	END	

	----------------------------------------------------------------------------------------------------

	-- 签到信息 
	DECLARE @CheckInDays SMALLINT
	DECLARE @CheckInState TINYINT
	
	-- 查询签到信息
	SELECT @CheckInDays=CheckInDays,@CheckInState=CheckInState FROM dbo.LHF_GetCheckInData(@UserID)	

	----------------------------------------------------------------------------------------------------

	-- 时间定义
	DECLARE @CurrDateTime DATETIME
	DECLARE @ReferDateTime DATETIME	
	
	--设置时间
	SET @CurrDateTime=GETDATE()
	SET @ReferDateTime=@LogonDateTime

	-- 调整时间
	IF @ResetDateTime>@LogonDateTime SET @ReferDateTime=@ResetDateTime

	-- 获取分量
	DECLARE @CurrDayID INT = CAST(CAST(@CurrDateTime AS FLOAT) AS INT)
	DECLARE @ReferDayID INT = CAST(CAST(@ReferDateTime AS FLOAT) AS INT)

	-- 重置数据
	IF @CurrDayID<>@ReferDayID
	BEGIN
		
		-- 重置日更新数据
		SELECT @ActionMaskPerDay=0

		-- 更新信息
		UPDATE AccountsInfo SET ActionMaskPerDay=@ActionMaskPerDay WHERE UserID=@UserID	
	END

	----------------------------------------------------------------------------------------------------

	-- 登陆令牌
	DECLARE @SecretKey NVARCHAR(128)
	SET @SecretKey=REPLACE(NEWID(), N'-', N'')

	-- 登录次数
	SET @GameLogonTimes+=1
	
	-- 更新信息
	UPDATE AccountsInfo SET SecretKey=@SecretKey,GameLogonTimes=@GameLogonTimes,LogonDate=GETDATE(), LogonIP=@strClientIP, LogonMode=5,LogonDeviceKind=@dwDeviceKind,
	PlazaVersion=@dwPlazaVersion,LogonMachineID=@strMachineID,LogonMobile=@strMobilePhone WHERE UserID=@UserID		

	-- 登录日志
	INSERT INTO RecordLogonInfo(UserID,DateID,DeviceKind,MachineOSID,PlazaVersion,ClientIP,MachineID,CollectDate)
	VALUES (@UserID,@DateID,@dwDeviceKind,@strMachineOSID,@dwPlazaVersion,@strClientIP,@strMachineID,GETDATE())

	-- 登录统计	
	UPDATE SystemStreamLogon SET LogonCount=LogonCount+1,LastDate=GETDATE() WHERE DateID=@DateID AND UserID=@UserID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO SystemStreamLogon(DateID,UserID,LogonCount) VALUES(@DateID,@UserID,1)
	END	

	-- 记录日志
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID,GameLogonSuccess) VALUES (@DateID, 1)	

	-- 金币查询
	SELECT @UserGold=Score,@InsureGold=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@UserID
	IF @UserGold IS NULL OR @InsureGold IS NULL
	BEGIN
		SET @UserGold=0
		SET @InsureGold = 0
	END

	-- 钻石信息
	SELECT @UserCard=Score FROM LHGameBattleDB.dbo.GameCardInfo(NOLOCK) WHERE UserID=@UserID

	-- 查询佣金
	SELECT @Commission=Commission FROM AccountsCommission WHERE UserID=@UserID

	-- 调整财富
	IF @UserCard IS NULL SET @UserCard=0
	IF @Commission IS NULL SET @Commission=0
	
	-- 在线信息
	DECLARE @LockGateID INT
	DECLARE @LockLogonID INT
	DECLARE @LockSocketID INT
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT
	DECLARE @LockServerKind INT

	-- 查询锁定
	SELECT @LockGateID=GateID,@LockLogonID=LogonID,@LockSocketID=SocketID,@LockKindID=KindID,@LockServerID=ServerID,@LockServerKind=ServerKind FROM AccountsOnlineInfo(NOLOCK) WHERE UserID=@UserID

	-- 调整变量
	IF @LockGateID IS NOT NULL
	BEGIN		
		-- 更新数据
		UPDATE AccountsOnlineInfo SET GateID=@wGateID,LogonID=@wLogonID,SocketID=@dwSocketID,InsertTime=GETDATE(),ModifyTime=GETDATE() WHERE UserID=@UserID	
	END ELSE
	BEGIN
		-- 调整变量
		SELECT @LockGateID=0,@LockLogonID=0,@LockSocketID=0,@LockKindID=0,@LockServerID=0,@LockServerKind=0

		-- 插入数据
		INSERT INTO AccountsOnlineInfo(UserID,GateID,LogonID,SocketID,InsertTime,ModifyTime) VALUES (@UserID,@wGateID,@wLogonID,@dwSocketID,GETDATE(),GETDATE())
	END
	
	-- 查询代理
	SELECT @ParentGameID=GameID,@ParentFaceID=FaceID,@ParentNickName=NickName FROM AccountsInfo(NOLOCK) WHERE UserID=@ParentUserID
	IF @ParentGameID IS NULL
	BEGIN
		SET @ParentGameID=0
		SET @ParentFaceID=0
		SET @ParentNickName=N''
	END

	--查询实名
	SELECT @szCompellation=Compellation,@szPassPortID=PassPortID FROM AccountsAuthentPresent WHERE UserID=@UserID
	IF @szCompellation IS NULL SET @szCompellation=N''
	IF @szPassPortID IS NULL SET @szPassPortID=N''

	-- 获取屏蔽掩码
	DECLARE @UIShieldMask INT
	SET @UIShieldMask=dbo.LHF_GetUserUIShieldMask(@UserID,@strClientIP,@strMachineID)

	-- 获取类型屏蔽列表
	DECLARE @ShieldKindList NVARCHAR(256)
	SET @ShieldKindList=dbo.LHF_GetUserShieldKindList(@UserID)
	
	--版本统计
	UPDATE PlazaVersionCollect SET LogonCount=LogonCount+1 WHERE PlazaVersion=@dwPlazaVersion AND DateID=@DateID
	IF @@ROWCOUNT=0 INSERT INTO PlazaVersionCollect (DateID,PlazaVersion,LogonCount,CollectDate) VALUES(@DateID,@dwPlazaVersion,1,GETDATE())

	--昵称修改查询
	DECLARE @wNickNameModifyTimes INT
	DECLARE @wNickNameDailyModifyTimes INT
	SELECT @wNickNameDailyModifyTimes=DailyModify,@wNickNameModifyTimes=TotalModify FROM RecordModifyUserIndividual WHERE DateID=@DateID AND UserID=@UserID
	IF @wNickNameModifyTimes IS NULL SELECT @wNickNameModifyTimes=COUNT(1) FROM RecordModifyUserIndividual WHERE UserID=@UserID
	IF @wNickNameModifyTimes IS NULL SET @wNickNameModifyTimes = 0
	IF @wNickNameDailyModifyTimes IS NULL SET @wNickNameDailyModifyTimes = 0

	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName, @LogonPass AS Password, @InsurePass AS InsurePass, @UnderWrite AS UnderWrite, @FaceID AS FaceID, @CustomID AS CustomID,
		@ProtectID AS ProtectID,@MasterOrder AS MasterOrder, @UserGold AS UserGold, @InsureGold AS InsureGold,@Commission AS Commission, @SecretKey AS SecretKey, @UserCard AS UserCard, @Gender AS Gender, @Experience AS Experience,
		@LoveLiness AS LoveLiness, @MemberPoint AS MemberPoint, @MemberOverTime AS MemberOverTime, @LockGateID AS LockGateID, @LockKindID As LockKindID, @LockLogonID AS LockLogonID, @LockSocketID AS LockSocketID,
		@LockServerID AS LockServerID, @MobilePhone AS MobilePhone,@Email AS Email, @StrUniqueID AS UniqueID, @RegisterTime AS RegisterTime, @LikedNumber AS LikedNumber,@InfoPublic AS InfoPublic, @ActionMaskEver AS ActionMaskEver,
		@ActionMaskPerDay AS ActionMaskPerDay, @ActionMaskPerWeek AS ActionMaskPerWeek, @GameLogonTimes AS GameLogonTimes, @LockServerKind AS LockServerKind,@ParentGameID AS ParentGameID,@ParentFaceID AS ParentFaceID,
		@ParentNickName AS ParentNickName,@ShutUpStatus AS ShutUpStatus, @ShutUpOverTime AS ShutUpOverTime, @RegisterMode AS RegisterMode, @UIShieldMask AS UIShieldMask, @ShieldKindList AS ShieldKindList, @LogonDateTime AS LastLogonDate,
		@UserRight AS UserRight, @szCompellation AS Compellation,@szPassPortID AS PassPortID,@szPresentGoodsList AS PresentGoodsList,@CheckInDays AS CheckInDays,@CheckInState AS CheckInState,@wNickNameDailyModifyTimes AS NickNameDailyModifyTimes,
		@wNickNameModifyTimes AS NickNameModifyTimes
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
