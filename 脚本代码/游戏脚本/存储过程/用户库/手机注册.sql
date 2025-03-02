
/***************************************************************************************************
** 名称：帐号注册 (游戏)
***************************************************************************************************/

----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO


IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_RegisterMobilePhone]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_RegisterMobilePhone]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------


-- 帐号注册
CREATE PROC [dbo].[GSP_MB_RegisterMobilePhone]
	@wGateID INT,								-- 网关标识	
	@wLogonID INT,								-- 登录标识
	@dwSocketID INT,							-- 网络标识
	@dwSpreaderID INT,							-- 代理ID
	@strAuthCode NCHAR(32),						-- 验证码
	@strMobilePhone NVARCHAR(21),				-- 手机号码		
	@strNickName NVARCHAR(32),					-- 用户昵称
	@strPassword NCHAR(32),						-- 用户密码	
	@wFaceID SMALLINT,							-- 头像标识
	@cbGender TINYINT,							-- 用户性别
	@wMarketID SMALLINT,						-- 渠道标识
	@dwDeviceKind INT,							-- 设备类型
	@dwPlazaVersion INT,						-- 大厅版本
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NCHAR(32),					-- 机器标识	
	@strMachineOSID NVARCHAR(64),				-- 系统标识
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
DECLARE @UserRight		INT
DECLARE @CustomID		INT
DECLARE @InsurePass		NVARCHAR(32)

-- 扩展信息
DECLARE @GameID INT
DECLARE @ProtectID INT
DECLARE @MasterOrder INT
DECLARE @LoveLiness INT
DECLARE @Experience INT
DECLARE @LikedNumber INT
DECLARE @InfoPublic TINYINT
DECLARE @MemberPoint INT
DECLARE @MemberOverTime INT
DECLARE @RegisterTime INT
DECLARE @RegisterMode TINYINT

-- 用户信息
DECLARE @Gender TINYINT

-- 行为掩码
DECLARE @ActionMaskEver INT
DECLARE @ActionMaskPerDay INT
DECLARE @ActionMaskPerWeek INT

-- 辅助变量
DECLARE @EnjoinLogon AS INT
DECLARE @EnjoinRegister AS INT

-- 财富信息
DECLARE @UserGold BIGINT
DECLARE @UserCard BIGINT
DECLARE @InsureGold BIGINT
DECLARE @Commission BIGINT

-- 变量定义
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

	-- 注册暂停
	SELECT @EnjoinRegister=CONVERT(INT,StatusValue),@StatusString=StatusString FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'EnjoinRegister'
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN		
		IF @StatusString IS NULL SET @StatusString=N''
		SET @strErrorDescribe=N'{"Idx":1001,"Msg":"'+@StatusString+N'"}'
		RETURN 1
	END

	-- 登陆暂停
	SELECT @EnjoinLogon=CONVERT(INT,StatusValue),@StatusString=StatusString FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		IF dbo.LHF_IsAccountExcept(@strClientIP, @strMachineID)<>0
		BEGIN
			IF @StatusString IS NULL SET @StatusString=N''
			SET @strErrorDescribe=N'{"Idx":1002,"Msg":"'+@StatusString+N'"}'

			RETURN 1
		END

		SET @EnjoinLogon=0	
	END

	-- 验证信息
	DECLARE @BindUserID INT
	DECLARE @AuthKind TINYINT
	DECLARE @AuthCodeMD5 NVARCHAR(32)
	DECLARE @AcquireMachine NVARCHAR(32)	
	DECLARE @LastAcquireDate DATETIME
	SELECT @BindUserID=UserID, @AuthKind=AuthKind, @AuthCodeMD5=AuthCodeMD5, @AcquireMachine=AcquireMachine, @LastAcquireDate=LastAcquireDate FROM RecordAuthCode(NOLOCK) WHERE CodeKind=1 AND MobileEmail=@strMobilePhone

	-- 验证码不存在
	IF @BindUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1012,"Msg":"验证码错误，注册失败1！"}'
		RETURN 6
	END

	-- 用户不匹配
	IF @BindUserID<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1012,"Msg":"验证码错误，注册失败2！"}'
		RETURN 6
	END

	-- 校验类型
	IF @AuthKind<>7
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1012,"Msg":"验证码错误，注册失败3！"}'
		RETURN 6		
	END
	
	-- 校验验证码
	IF @AuthCodeMD5<>@strAuthCode
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1012,"Msg":"验证码错误，注册失败4！"}'
		RETURN 7				
	END

	-- 校验机器码
	IF @AcquireMachine<>@strMachineID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1013,"Msg":"您的机器码发生异常，注册失败！"}'
		RETURN 8
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
		SET @strErrorDescribe=N'{"Idx":1014,"Msg":"验证码已过期，请重新获取验证码！"}'
		RETURN 8
	END

	-- 校验昵称
	--IF (SELECT COUNT(1) FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strNickName)>0)>0
	--BEGIN
	--	SET @strErrorDescribe=N'{"Idx":1004,"Msg":"抱歉地通知您，您所输入的昵称含有限制字符串，请更换昵后再次申请昵称！"}'
	--	RETURN 4
	--END

	--IF @strNickName NOT LIKE '%[^0-9]%' AND LEN(@strNickName)=16
	--BEGIN
	--	SET @strErrorDescribe=N'{"Idx":1004,"Msg":"抱歉地通知您，您所输入的昵称含有限制字符串，请更换昵后再次申请昵称！"}'
	--	RETURN 116
	--END

	-- 效验地址
	SELECT @EnjoinRegister=EnjoinRegister FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1005,"Msg":"抱歉地通知您，系统禁止了您所在的 IP 地址的注册功能，请联系客户服务中心了解详细情况！"}'
		RETURN 5
	END
	
	-- 效验机器
	SELECT @EnjoinRegister=EnjoinRegister FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1006,"Msg":"抱歉地通知您，系统禁止了您的机器的注册功能，请联系客户服务中心了解详细情况！"}'
		RETURN 6
	END
 
	-- 查询用户
	IF EXISTS (SELECT 1 FROM AccountsInfo(NOLOCK) WHERE Accounts=@strMobilePhone)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1015,"Msg":"此手机号码已被注册，请换另一手机号码尝试再次注册！"}'
		RETURN 7
	END

	-- 查询用户
	--IF EXISTS (SELECT 1 FROM AccountsInfo(NOLOCK) WHERE NickName=@strNickName)
	--BEGIN
	--	SET @strErrorDescribe=N'{"Idx":1008,"Msg":"此昵称已被注册，请换另一昵称尝试再次注册！"}'
	--	RETURN 7
	--END
	
	-- 查询手机
	IF EXISTS (SELECT 1 FROM AccountsInfo(NOLOCK) WHERE MobilePhone=@strMobilePhone)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1016,"Msg":"此手机号码已被其他游戏账号绑定，请换另一手机号码尝试再次注册！"}'
		RETURN 7
	END	

	-- 生成账号
	SET @Accounts=CONCAT(N'Mobile',SUBSTRING(@strMobilePhone,LEN(@strMobilePhone)-3,4))
	SET @strNickName=CONCAT(SUBSTRING(@strMobilePhone,LEN(@strMobilePhone)-3,4),N':',N'Mobile')	

	-- 调整账号
	EXEC dbo.LHP_AdjustmentAccounts @Accounts OUTPUT,@strNickName OUTPUT
	
	-- 登陆令牌
	DECLARE @SecretKey NVARCHAR(128)
	SET @SecretKey=REPLACE(NEWID(), N'-', N'')

	-- 注册用户
	INSERT AccountsInfo (Accounts,NickName,RegAccounts,LogonPass,InsurePass,Gender,FaceID,LogonMachineID, SecretKey, RegisterMode,RegisterMachineID,RegisterDeviceKind,RegisterPackageName,GameLogonTimes,
		  RegisterIP,LogonIP,LogonMode,LogonDeviceKind,PlazaVersion,MarketID,MobilePhone)
	VALUES (@strMobilePhone,@strNickName,@strMobilePhone,@strPassword,N'',@cbGender,@wFaceID,@strMachineID,@SecretKey,4,@strMachineID,@dwDeviceKind,@strPackageName,1,
	     @strClientIP,@strClientIP,4,@dwDeviceKind,@dwPlazaVersion,@wMarketID,@strMobilePhone)

	-- 错误判断
	IF @@ERROR<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1018,"Msg":"手机号已存在，请换另一手机号码尝试再次注册！"}'
		RETURN 8
	END	
	
	-- 查询用户
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts,@NickName=NickName,@FaceID=FaceID, @CustomID=CustomID, @ProtectID=ProtectID, @MasterOrder=MasterOrder, @Gender=Gender, @Experience=Experience,
		@LoveLiness=LoveLiness, @UnderWrite=UnderWrite,@UserRight=UserRight,@MemberPoint=MemberPoint, @MemberOverTime=MemberOverTime, @LikedNumber=LikedNumber,@InfoPublic=InfoPublic,@ActionMaskEver=ActionMaskEver,
		@ActionMaskPerDay=ActionMaskPerDay, @ActionMaskPerWeek=ActionMaskPerWeek,@RegisterMode=RegisterMode,@RegisterTime=dbo.LHF_ConvertTimestamp(RegisterDate)
	FROM AccountsInfo(NOLOCK) WHERE MobilePhone=@strMobilePhone

	-- 分配标识
	SELECT @GameID=GameID FROM GameIdentifier(NOLOCK) WHERE UserID=@UserID
	IF @GameID IS NULL 
	BEGIN
		SET @GameID=0
		SET @strErrorDescribe=N'{"Idx":1010,"Msg":"用户注册成功，但未成功获取游戏 ID 号码，系统稍后将给您分配！"}'
	END
	ELSE UPDATE AccountsInfo SET GameID=@GameID WHERE UserID=@UserID

	-- 日期标识
	DECLARE @DateID	INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	
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

	-- 登录日志
	INSERT INTO RecordLogonInfo(UserID,DateID,DeviceKind,MachineOSID,PlazaVersion,ClientIP,MachineID,CollectDate)
	VALUES (@UserID,@DateID,@dwDeviceKind,@strMachineOSID,@dwPlazaVersion,@strClientIP,@strMachineID,GETDATE())

	-- 登录统计	
	UPDATE SystemStreamLogon SET LogonCount=LogonCount+1,LastDate=GETDATE() WHERE DateID=@DateID AND UserID=@UserID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO SystemStreamLogon(DateID,UserID,LogonCount) VALUES(@DateID,@UserID,1)
	END

	-- 注册统计	
	UPDATE SystemStreamRegister SET TotalCount=TotalCount+1,LastDate=GETDATE()
	WHERE DateID=@DateID AND DeviceKind=@dwDeviceKind AND ExternalID=0
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO SystemStreamRegister(DateID,DeviceKind,ExternalID,TotalCount) VALUES(@DateID,@dwDeviceKind,0,1)
	END

	-- 记录日志
	UPDATE SystemStreamInfo SET GameRegisterSuccess=GameRegisterSuccess+1 WHERE DateID=@DateID 
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID,GameRegisterSuccess) VALUES (@DateID, 1)
	   
	-- 金币查询
	SELECT @UserGold=Score,@InsureGold=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@UserID
	IF @UserGold IS NULL OR @InsureGold IS NULL
	BEGIN
		SELECT @UserGold=0, @InsureGold = 0

		INSERT LHGameGoldDB.dbo.GameScoreInfo (UserID,Score,InsureScore)  VALUES (@UserID, @UserGold, 0) 
	END

	----------------------------------------------------------------------------------------------------

	-- 签到信息 
	DECLARE @CheckInDays SMALLINT
	DECLARE @CheckInState TINYINT
	
	-- 查询签到信息
	SELECT @CheckInDays=CheckInDays,@CheckInState=CheckInState FROM dbo.LHF_GetCheckInData(@UserID)	


	--------------------------------------注册赠送-----------------------------------------------------

	EXEC GSP_MB_RegisterPresent @UserID,4,@strClientIP,@strMachineID,@szPresentGoodsList OUTPUT

	--------------------------------------注册赠送-----------------------------------------------------

	-- 钻石信息
	SELECT @UserCard=Score FROM LHGameBattleDB.dbo.GameCardInfo(NOLOCK) WHERE UserID=@UserID

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
	SELECT @LockGateID=GateID,@LockLogonID=LogonID,@LockSocketID=SocketID,@LockServerID=ServerID,@LockKindID=KindID,@LockServerKind=ServerKind FROM AccountsOnlineInfo(NOLOCK) WHERE UserID=@UserID

	-- 调整变量
	IF @LockGateID IS NOT NULL
	BEGIN		
		-- 更新数据
		UPDATE AccountsOnlineInfo SET GateID=@wGateID,LogonID=@wLogonID,SocketID=@dwSocketID,InsertTime=GETDATE(),ModifyTime=GETDATE() WHERE UserID=@UserID	
	END ELSE
	BEGIN
		-- 调整变量
		SELECT @LockGateID=0,@LockLogonID=0,@LockSocketID=0,@LockServerID=0,@LockKindID=0,@LockServerKind=0

		-- 插入数据
		INSERT INTO AccountsOnlineInfo(UserID,GateID,LogonID,SocketID,InsertTime,ModifyTime) VALUES (@UserID,@wGateID,@wLogonID,@dwSocketID,GETDATE(),GETDATE())
	END

	--版本统计
	UPDATE PlazaVersionCollect SET LogonCount=LogonCount+1 WHERE PlazaVersion=@dwPlazaVersion AND DateID=@DateID
	IF @@ROWCOUNT=0 INSERT INTO PlazaVersionCollect (DateID,PlazaVersion,LogonCount,CollectDate) VALUES(@DateID,@dwPlazaVersion,1,GETDATE())

	-- 更新信息
	DECLARE @ATAuthTimes SMALLINT
	DECLARE @ATLastAcquireDate DATETIME
	SELECT @ATAuthTimes=AuthTimes,@ATLastAcquireDate=LastAcquireDate FROM AccountsAuthInfo(NOLOCK) WHERE UserID=@UserID

	-- 插入数据
	IF @ATAuthTimes IS NULL
	BEGIN
		INSERT INTO AccountsAuthInfo(UserID,CodeKind,AuthTimes,AcquireTimes,LastAcquireDate,LastMobileEmail) VALUES (@UserID,1,7,0,GETDATE(),N'')		
	END ELSE
	BEGIN
		-- 时间间隔
		DECLARE @DateDiff INT
		SET @DateDiff=DateDiff(day,@ATLastAcquireDate,GETDATE())

		-- 调整次数
		IF @DateDiff<>0 SET @ATAuthTimes=0

		-- 更新数据
		UPDATE AccountsAuthInfo SET AuthTimes=@ATAuthTimes+1,AcquireTimes=0,LastAcquireDate=GETDATE(),LastMobileEmail=N'' WHERE UserID=@UserID AND CodeKind=1
	END

	-- 更新次数
	UPDATE RecordAuthCode SET AuthCode=N'',AuthCodeMD5=N'',AuthTimes=AuthTimes+1,AcquireTimes=0 WHERE CodeKind=1 AND MobileEmail=@strMobilePhone

	-- 记录验证次数
	UPDATE RecordAuthTimes SET AuthTimes=AuthTimes+1 WHERE DateID=@DateID AND UserID=@UserID AND CodeKind=1 AND AuthKind=@AuthKind
	IF @@ROWCOUNT=0
	BEGIN
		-- 插入记录
		INSERT INTO RecordAuthTimes(DateID,UserID,CodeKind,AuthKind,AuthTimes) VALUES (@DateID,@UserID,1,@AuthKind,1)
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

	--昵称修改查询
	DECLARE @wNickNameModifyTimes INT
	DECLARE @wNickNameDailyModifyTimes INT
	SELECT @wNickNameDailyModifyTimes=DailyModify,@wNickNameModifyTimes=TotalModify FROM RecordModifyUserIndividual WHERE DateID=@DateID AND UserID=@UserID
	IF @wNickNameModifyTimes IS NULL SELECT @wNickNameModifyTimes=COUNT(1) FROM RecordModifyUserIndividual WHERE UserID=@UserID
	IF @wNickNameModifyTimes IS NULL SET @wNickNameModifyTimes = 0
	IF @wNickNameDailyModifyTimes IS NULL SET @wNickNameDailyModifyTimes = 0

	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName, @strPassword AS Password, @InsurePass AS InsurePass, @FaceID AS FaceID, @CustomID AS CustomID, @ProtectID AS ProtectID,
		   @MasterOrder AS MasterOrder, @SecretKey AS SecretKey, @UserGold AS Gold, @InsureGold AS InsureGold, @UserCard AS UserCard, @UserGold AS UserGold, @Commission AS Commission, @Gender AS Gender, @LockGateID AS LockGateID,
		   @LockLogonID AS LockLogonID, @LockSocketID AS LockSocketID, @LockServerID AS LockServerID, @LockKindID AS LockKindID, @LockServerKind AS LockServerKind, @Experience AS Experience, @LoveLiness AS LoveLiness, 
		   @MemberPoint AS MemberPoint, @MemberOverTime AS MemberOverTime, @UnderWrite AS UnderWrite, @strMobilePhone AS MobilePhone, N'' AS Email, N'' AS UniqueID, @LikedNumber AS LikedNumber, @InfoPublic AS InfoPublic, 
		   @ActionMaskEver AS ActionMaskEver, @ActionMaskPerDay AS ActionMaskPerDay, @ActionMaskPerWeek AS ActionMaskPerWeek, @RegisterTime AS RegisterTime, 1 AS GameLogonTimes, @ParentGameID AS ParentGameID,@ParentFaceID AS ParentFaceID,
		   @ParentNickName AS ParentNickName, 0 AS ShutUpStatus, 0 AS ShutUpOverTime, @RegisterMode AS RegisterMode, @UIShieldMask AS UIShieldMask, @ShieldKindList AS ShieldKindList, @szCompellation AS Compellation, @szPassPortID AS PassPortID,
		   @szPresentGoodsList AS PresentGoodsList,@CheckInDays AS CheckInDays,@CheckInState AS CheckInState,@wNickNameDailyModifyTimes AS NickNameDailyModifyTimes,@wNickNameModifyTimes AS NickNameModifyTimes,@UserRight AS UserRight
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
