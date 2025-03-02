/***************************************************************************************************
** 名称：帐号登录 (游戏)
***************************************************************************************************/
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_EfficacyMobilePhone]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_EfficacyMobilePhone]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_EfficacyMobileCode]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_EfficacyMobileCode]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 手机登陆
CREATE PROC GSP_MB_EfficacyMobilePhone
	@wGateID INT,								-- 网关标识	
	@wLogonID INT,								-- 登录标识
	@dwSocketID INT,							-- 网络标识
	@strMobilePhone NVARCHAR(21),				-- 手机号码
	@strPassword NCHAR(32),						-- 用户密码
	@wMarketID SMALLINT,						-- 渠道标识
	@dwDeviceKind INT,							-- 设备类型
	@dwPlazaVersion INT,						-- 大厅版本
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NCHAR(32),					-- 机器标识
	@strMachineOSID NVARCHAR(64),				-- 系统标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID			INT
DECLARE @FaceID			SMALLINT
DECLARE @Accounts		NVARCHAR(31)
DECLARE @NickName		NVARCHAR(31)
DECLARE @UniqueID		NVARCHAR(32)
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
DECLARE @InfoPublic	 TINYINT
DECLARE @MemberPoint INT
DECLARE @MemberOverTime INT

-- 用户信息
DECLARE @Gender TINYINT

-- 金币信息
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
 
	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @ShutUpStatus BIT
	DECLARE @ShutDownStatus BIT
	DECLARE @GameLogonTimes INT
	DECLARE @LogonPass NCHAR(32)
	DECLARE @InsurePass NCHAR(32)
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

	-- 查询用户
	SELECT @UserID=a.UserID, @GameID=a.GameID, @Accounts=a.Accounts, @NickName=a.NickName, @UnderWrite=a.UnderWrite, @LogonPass=a.LogonPass, @InsurePass=a.InsurePass, @FaceID=a.FaceID,@CustomID=a.CustomID,@ProtectID=a.ProtectID,
		@MasterOrder=a.MasterOrder,@Gender=a.Gender, @Nullity=a.Nullity, @Experience=a.Experience, @LoveLiness=a.LoveLiness, @GameLogonTimes=a.GameLogonTimes, @MemberPoint=a.MemberPoint, @MemberOverTime=a.MemberOverTime, 
		@MachineID=a.LogonMachineID, @PlazaVersion=a.PlazaVersion,@LikedNumber=a.LikedNumber, @MobilePhone=MobilePhone, @Email=a.EMail, @RegisterMode=a.RegisterMode, @InfoPublic=a.InfoPublic,@RegisterDeviceKind=a.RegisterDeviceKind,
		@ActionMaskEver=a.ActionMaskEver,@ActionMaskPerDay=a.ActionMaskPerDay, @ActionMaskPerWeek=a.ActionMaskPerWeek, @ParentUserID=a.ParentID,@ShutUpStatus=b.ShutupStatus,@ShutDownStatus=b.ShutDownStatus,@ShutupOverTime=b.ShutupOverTime,
		@UserRight=a.UserRight,@ShutDownOverTime=b.ShutDownOverTime,@LogonDateTime=a.LogonDate,@ResetDateTime=a.ResetDateTime,@RegisterTime=dbo.LHF_ConvertTimestamp(a.RegisterDate)
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.MobilePhone=@strMobilePhone

	-- 调整变量
	IF @Email IS NULL SET @Email=N''
	IF @ShutUpStatus IS NULL SET @ShutUpStatus=0
	IF @ShutDownStatus IS NULL SET @ShutDownStatus=0
	IF @ShutUpOverTime IS NULL SET @ShutUpOverTime=0
	IF @ShutDownOverTime IS NULL SET @ShutDownOverTime=0

	-- 校验标识
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试登录！"}'
		RETURN 200
	END	

	-- 密码判断
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试登录！"}'
		RETURN 3
	END

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
	
	-- 他方注册
	SELECT @UniqueID=UniqueID FROM AccountsExternal(NOLOCK) WHERE UserID=@UserID	

	-- 调整变量
	IF @UniqueID IS NULL SET @UniqueID=N''	

	-- 登陆令牌
	DECLARE @SecretKey NVARCHAR(128)
	SET @SecretKey=REPLACE(NEWID(), N'-', N'')

	-- 转换日期
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 登录次数
	SET @GameLogonTimes+=1

	-- 更新信息
	UPDATE AccountsInfo SET SecretKey=@SecretKey,GameLogonTimes=@GameLogonTimes,LogonDate=GETDATE(), LogonIP=@strClientIP,LogonMode=4,LogonDeviceKind=@dwDeviceKind,
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
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)	
	
	-- 金币查询
	SELECT @UserGold=Score,@InsureGold=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@UserID
	IF @UserGold IS NULL OR @InsureGold IS NULL
	BEGIN
		SET @UserGold=0
		SET @InsureGold = 0
	END
	
	-- 财富信息
	SELECT @UserCard=Score FROM LHGameBattleDB.dbo.GameCardInfo(NOLOCK) WHERE UserID=@UserID
	SELECT @Commission=Commission FROM AccountsCommission(NOLOCK) WHERE UserID=@UserID

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
		@ProtectID AS ProtectID,@MasterOrder AS MasterOrder, @UserGold AS UserGold, @InsureGold AS InsureGold, @InfoPublic AS InfoPublic, @SecretKey AS SecretKey, @UserCard AS UserCard, @Commission AS Commission, 
		@Gender AS Gender, @Experience AS Experience, @LoveLiness AS LoveLiness, @MemberPoint AS MemberPoint, @MemberOverTime AS MemberOverTime, @LockGateID AS LockGateID, @LockKindID AS LockKindID, @LockLogonID AS LockLogonID,
		@LockSocketID AS LockSocketID,@LockServerID AS LockServerID, @MobilePhone AS MobilePhone, @Email AS Email, @UniqueID AS UniqueID, @RegisterTime AS RegisterTime, @LikedNumber AS LikedNumber, @ActionMaskEver AS ActionMaskEver,
		@ActionMaskPerDay AS ActionMaskPerDay, @ActionMaskPerWeek AS ActionMaskPerWeek, @GameLogonTimes AS GameLogonTimes, @LockServerKind AS LockServerKind, @ParentGameID AS ParentGameID,@ParentFaceID AS ParentFaceID,
		@ParentNickName AS ParentNickName,@ShutUpStatus AS ShutUpStatus, @ShutUpOverTime AS ShutUpOverTime, @RegisterMode AS RegisterMode,@UIShieldMask AS UIShieldMask,@ShieldKindList AS ShieldKindList, @LogonDateTime AS LastLogonDate,
		@UserRight AS UserRight, @szCompellation AS Compellation,@szPassPortID AS PassPortID,@CheckInDays AS CheckInDays,@CheckInState AS CheckInState,@wNickNameDailyModifyTimes AS NickNameDailyModifyTimes,@wNickNameModifyTimes AS NickNameModifyTimes
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 验证码登陆
CREATE PROC GSP_MB_EfficacyMobileCode
	@wGateID INT,								-- 网关标识	
	@wLogonID INT,								-- 登录标识
	@dwSocketID INT,							-- 网络标识	
	@strAuthCode NCHAR(32),						-- 验证码
	@strMobilePhone NVARCHAR(21),				-- 手机号码
	@wMarketID SMALLINT,						-- 渠道标识
	@dwSpreaderID INT,							-- 推广员ID
	@dwDeviceKind INT,							-- 设备类型
	@dwPlazaVersion INT,						-- 大厅版本
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NCHAR(32),					-- 机器标识	
	@strMachineOSID NVARCHAR(64),				-- 系统标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID			INT
DECLARE @FaceID			SMALLINT
DECLARE @Accounts		NVARCHAR(31)
DECLARE @NickName		NVARCHAR(31)
DECLARE @UniqueID		NVARCHAR(32)
DECLARE @UnderWrite		NVARCHAR(63)
DECLARE @CustomID		INT
DECLARE @PasswordID		INT
DECLARE @UserRight		INT
DECLARE @ServiceRight	INT

-- 扩展信息
DECLARE @GameID			INT
DECLARE @ProtectID		INT
DECLARE @MasterOrder	INT
DECLARE @LoveLiness		INT
DECLARE @Experience		INT
DECLARE @LikedNumber	INT
DECLARE @InfoPublic		TINYINT
DECLARE @MemberPoint	INT
DECLARE @MemberOverTime INT

-- 用户信息
DECLARE @Gender TINYINT

-- 行为掩码
DECLARE @ActionMaskEver INT
DECLARE @ActionMaskPerDay INT
DECLARE @ActionMaskPerWeek INT

-- 辅助变量
DECLARE @EnjoinLogon AS INT
DECLARE @StatusString NVARCHAR(128)

-- 财富信息
DECLARE @UserGold BIGINT
DECLARE @UserCard BIGINT
DECLARE @InsureGold BIGINT	
DECLARE @Commission BIGINT

-- 代理信息
DECLARE @ParentUserID INT
DECLARE @ParentGameID INT
DECLARE @ParentFaceID INT
DECLARE @ParentNickName NVARCHAR(31)

--实名信息
DECLARE @szCompellation NVARCHAR(32)
DECLARE @szPassPortID NVARCHAR(32)

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
		SET @strErrorDescribe=N'{"Idx":1027,"Msg":"验证码错误，登陆失败！"}'
		RETURN 6
	END

	-- 用户不匹配
	IF @BindUserID<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1027,"Msg":"验证码错误，登陆失败！"}'
		RETURN 6
	END

	-- 校验类型
	IF @AuthKind<>6
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1027,"Msg":"验证码错误，登陆失败！"}'
		RETURN 6		
	END
	
	-- 校验验证码
	IF @AuthCodeMD5<>@strAuthCode
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1027,"Msg":"验证码错误，登陆失败！"}'
		RETURN 7				
	END

	-- 校验机器码
	IF @AcquireMachine<>@strMachineID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1028,"Msg":"您的机器码发生异常，登陆失败！"}'
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
		SET @strErrorDescribe=N'{"Idx":1029,"Msg":"验证码已过期，继续登陆请重新获取验证码！"}'
		RETURN 8
	END

	-- 转换日期
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)	
 
	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @ShutDownStatus BIT
	DECLARE @ShutUpStatus BIT	
	DECLARE @GameLogonTimes INT
	DECLARE @LogonPass NCHAR(32)
	DECLARE @InsurePass NCHAR(32)	
	DECLARE	@MachineID NCHAR(32)
	DECLARE	@Email NVARCHAR(64)
	DECLARE @PlazaVersion INT
	DECLARE @ShutupOverTime INT
	DECLARE @ShutDownOverTime INT
	DECLARE @RegisterTime INT
	DECLARE @RegisterMode SMALLINT	
	DECLARE	@LogonDateTime DATETIME
	DECLARE	@ResetDateTime DATETIME
	DECLARE @RegisterDeviceKind SMALLINT	

	SELECT @UserID=a.UserID, @GameID=a.GameID, @Accounts=a.Accounts, @NickName=a.NickName, @UnderWrite=a.UnderWrite, @LogonPass=a.LogonPass, @InsurePass=a.InsurePass, @FaceID=a.FaceID,@CustomID=a.CustomID,@ProtectID=a.ProtectID,
		@MasterOrder=a.MasterOrder,@Gender=a.Gender, @Nullity=a.Nullity, @Experience=a.Experience, @LoveLiness=a.LoveLiness, @GameLogonTimes=a.GameLogonTimes, @MemberPoint=a.MemberPoint, @MemberOverTime=a.MemberOverTime, 
		@MachineID=a.LogonMachineID, @PlazaVersion=a.PlazaVersion,@LikedNumber=a.LikedNumber, @RegisterMode=a.RegisterMode, @InfoPublic=a.InfoPublic,@RegisterDeviceKind=a.RegisterDeviceKind,@ActionMaskEver=a.ActionMaskEver,
		@ActionMaskPerDay=a.ActionMaskPerDay,@ActionMaskPerWeek=a.ActionMaskPerWeek, @LogonDateTime=a.LogonDate,@ResetDateTime=a.ResetDateTime,@ParentUserID=a.ParentID,@ShutUpStatus=b.ShutupStatus,@ShutDownStatus=b.ShutDownStatus,
		@UserRight=a.UserRight,@ShutupOverTime=b.ShutupOverTime,@ShutDownOverTime=b.ShutDownOverTime,@Email=Email,@RegisterTime=dbo.LHF_ConvertTimestamp(a.RegisterDate)
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID  WHERE a.MobilePhone=@strMobilePhone
	
	-- 调整变量
	IF @Email IS NULL SET @Email=N''
	IF @ShutUpStatus IS NULL SET @ShutUpStatus=0
	IF @ShutDownStatus IS NULL SET @ShutDownStatus=0
	IF @ShutUpOverTime IS NULL SET @ShutUpOverTime=0
	IF @ShutDownOverTime IS NULL SET @ShutDownOverTime=0

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
			SET @strErrorDescribe=N'{"Idx":1136,"Msg":"当前帐号已被临时封禁至%s（解封时间）, 暂无法使用。如有其他疑问，请联系客服。","Ops":["'+CONVERT(NVARCHAR(16),DATEADD(s,@ShutDownOverTime,'1970-1-1'))+'"]}'	
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
	
	-- 他方注册
	SELECT @UniqueID=UniqueID FROM AccountsExternal(NOLOCK) WHERE UserID=@UserID		
	IF @UniqueID IS NULL SET @UniqueID=N''

	-- 登陆令牌
	DECLARE @SecretKey NVARCHAR(128)
	SET @SecretKey=REPLACE(NEWID(), N'-', N'')

	-- 更新信息
	UPDATE AccountsInfo SET SecretKey=@SecretKey,GameLogonTimes=GameLogonTimes+1,LogonDate=GETDATE(), LogonIP=@strClientIP,
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
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)		

	-- 查询金币
	SELECT @UserGold=Score,@InsureGold=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@UserID
	IF @UserGold IS NULL OR @InsureGold IS NULL
	BEGIN
		SET @UserGold=0
		SET @InsureGold = 0
	END
	
	-- 查询财富
	SELECT @UserCard=Score FROM LHGameBattleDB.dbo.GameCardInfo(NOLOCK) WHERE UserID=@UserID
	SELECT @Commission=Commission FROM AccountsCommission(NOLOCK) WHERE UserID=@UserID

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
		SELECT @LockGateID=0,@LockLogonID=0,@LockSocketID=0,@LockKindID=0,@LockServerID=0,@LockServerKind=0

		-- 插入数据
		INSERT INTO AccountsOnlineInfo(UserID,GateID,LogonID,SocketID,InsertTime,ModifyTime) VALUES (@UserID,@wGateID,@wLogonID,@dwSocketID,GETDATE(),GETDATE())
	END

	--版本统计
	UPDATE PlazaVersionCollect SET LogonCount=LogonCount+1 WHERE PlazaVersion=@dwPlazaVersion AND DateID=@DateID
	IF @@ROWCOUNT=0 INSERT INTO PlazaVersionCollect (DateID,PlazaVersion,LogonCount,CollectDate) VALUES(@DateID,@dwPlazaVersion,1,GETDATE())

	-- 更新信息
	DECLARE @ATAuthTimes SMALLINT
	DECLARE @ATLastAcquireDate DATETIME
	SELECT @ATAuthTimes=AuthTimes,@ATLastAcquireDate=LastAcquireDate FROM AccountsAuthInfo(NOLOCK) WHERE UserID=@UserID AND CodeKind=1

	-- 插入数据
	IF @ATAuthTimes IS NULL
	BEGIN
		INSERT INTO AccountsAuthInfo(UserID,CodeKind,AuthTimes,AcquireTimes,LastAcquireDate,LastMobileEmail) VALUES (@UserID,1,1,0,GETDATE(),N'')		
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
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName, @LogonPass AS Password, @InsurePass AS InsurePass, @UnderWrite AS UnderWrite,@SecretKey AS SecretKey, @FaceID AS FaceID, 
		   @CustomID AS CustomID,@ProtectID AS ProtectID,@MasterOrder AS MasterOrder, @UserGold AS UserGold, @InsureGold AS InsureGold, @UserCard AS UserCard,@Commission AS Commission, @Gender AS Gender, 
		   @Experience AS Experience, @LoveLiness AS LoveLiness, @MemberPoint AS MemberPoint, @MemberOverTime AS MemberOverTime, @LockGateID AS LockGateID, @InfoPublic AS InfoPublic,@LockLogonID AS LockLogonID, 
		   @LockSocketID AS LockSocketID, @LockKindID AS LockKindID, @LockServerID AS LockServerID, @strMobilePhone AS MobilePhone,@Email AS Email, @UniqueID AS UniqueID, @RegisterTime AS RegisterTime, 
		   @ActionMaskEver AS ActionMaskEver, @ActionMaskPerDay AS ActionMaskPerDay, @ActionMaskPerWeek AS ActionMaskPerWeek, @LikedNumber AS LikedNumber, @GameLogonTimes AS GameLogonTimes,@LockServerKind AS LockServerKind,
		   @ParentGameID AS ParentGameID,@ParentFaceID AS ParentFaceID,@ParentNickName AS ParentNickName, @ShutUpStatus AS ShutUpStatus, @ShutUpOverTime AS ShutUpOverTime,@RegisterMode AS RegisterMode,@UIShieldMask AS UIShieldMask,
		   @UserRight AS UserRight, @ShieldKindList AS ShieldKindList, @LogonDateTime AS LastLogonDate,@szCompellation AS Compellation,@szPassPortID AS PassPortID,@CheckInDays AS CheckInDays,@CheckInState AS CheckInState,
		   @wNickNameDailyModifyTimes AS NickNameDailyModifyTimes,@wNickNameModifyTimes AS NickNameModifyTimes
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------