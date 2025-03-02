/***************************************************************************************************
** 名称：帐号登录 (游戏)
***************************************************************************************************/
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_EfficacyAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_EfficacyAccounts]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 帐号登陆
CREATE PROC GSP_GP_EfficacyAccounts
	@wGateID INT,								-- 网关标识	
	@wLogonID INT,								-- 登录标识
	@dwSocketID INT,							-- 网络标识
	@strAccounts NVARCHAR(31),					-- 用户帐号
	@strPassword NCHAR(32),						-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址	
	@dwPlazaVersion INT,						-- 大厅版本
	@strMachineID NCHAR(32),					-- 机器标识
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
DECLARE @ServiceRight	INT

-- 扩展信息
DECLARE @GameID			INT
DECLARE @ProtectID		INT
DECLARE @MasterOrder	INT
DECLARE @Experience		INT
DECLARE @LikedNumber	INT
DECLARE @InfoPublic		TINYINT
DECLARE @UserRight		INT
DECLARE @MemberPoint	INT
DECLARE @MemberOverTime INT

-- 用户信息
DECLARE @Age TINYINT
DECLARE @Gender TINYINT
DECLARE @Area NVARCHAR(15)
DECLARE @City NVARCHAR(15)
DECLARE @Province NVARCHAR(15)

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
	DECLARE @ShutDownStatus BIT
	DECLARE @GameLogonTimes INT
	DECLARE @LogonPass NCHAR(32)
	DECLARE	@MachineID NCHAR(32)
	DECLARE @RegisterDate DATETIME	
	DECLARE @RegisterDeviceKind SMALLINT

	SELECT @UserID=a.UserID, @GameID=a.GameID, @Accounts=a.Accounts, @NickName=a.NickName, @UnderWrite=a.UnderWrite, @LogonPass=a.LogonPass, @FaceID=a.FaceID, @CustomID=a.CustomID, @ProtectID=a.ProtectID, 
		@MasterOrder=a.MasterOrder, @GameLogonTimes=a.GameLogonTimes,@RegisterDate=a.RegisterDate, @RegisterDeviceKind=a.RegisterDeviceKind, @Gender=a.Gender, @InfoPublic=a.InfoPublic,@Nullity=a.Nullity, 
		@ShutDownStatus=b.ShutDownStatus, @Experience=a.Experience, @MemberPoint=a.MemberPoint, @MemberOverTime=a.MemberOverTime, @MachineID=a.LogonMachineID,@ActionMaskEver=a.ActionMaskEver,
		@ActionMaskPerDay=a.ActionMaskPerDay,@ActionMaskPerWeek=a.ActionMaskPerWeek,@LikedNumber=a.LikedNumber,@ParentUserID=a.ParentID
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.Accounts=@strAccounts	

	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 200
	END	

	-- 密码判断
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
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
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
		RETURN 2
	END	

	----------------------------------------------------------------------------------------------------
	-- 查询资料
	SELECT @Age=Age, @Area=Area, @City=City, @Province=Province FROM AccountsDatum(NOLOCK) WHERE UserID=@UserID

	-- 调整资料
	IF @Age IS NULL SET @Age=0
	IF @Area IS NULL SET @Area=N''
	IF @City IS NULL SET @City=N''
	IF @Province IS NULL SET @Province=N''
	
	-----------------------------------------------------------------------------------------------------
	
	-- 更新信息
	UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1,LogonDate=GETDATE(), LogonIP=@strClientIP,PlazaVersion=@dwPlazaVersion, LogonMachineID=@strMachineID 
	WHERE UserID=@UserID			

	-- 记录日志
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)

	-- 登录日志
	INSERT INTO RecordLogonInfo(UserID,DateID,DeviceKind,MachineOSID,PlazaVersion,ClientIP,MachineID,CollectDate)
	VALUES (@UserID,@DateID,0,0,@dwPlazaVersion,@strClientIP,@strMachineID,GETDATE())

	-- 登录统计	
	UPDATE SystemStreamLogon SET LogonCount=LogonCount+1,LastDate=GETDATE()
	WHERE DateID=@DateID AND UserID=@UserID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO SystemStreamLogon(DateID,UserID,LogonCount) VALUES(@DateID,@UserID,1)
	END

	-- 财富信息
	DECLARE @UserGold BIGINT
	DECLARE @InsureGold BIGINT
	
	-- 查询财富
	SELECT @UserGold=Score,@InsureGold=InsureScore FROM LHGameGoldDB..GameScoreInfo(NOLOCK) WHERE UserID=@UserID
	IF @UserGold IS NULL OR @InsureGold IS NULL
	BEGIN
		SET @UserGold=0
		SET @InsureGold = 0
	END

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

	--版本统计
	UPDATE PlazaVersionCollect SET LogonCount=LogonCount+1 WHERE PlazaVersion=@dwPlazaVersion AND DateID=@DateID
	IF @@ROWCOUNT=0 INSERT INTO PlazaVersionCollect (DateID,PlazaVersion,LogonCount,CollectDate) VALUES(@DateID,@dwPlazaVersion,1,GETDATE())

	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName, @LogonPass AS Password, @UnderWrite AS UnderWrite, @FaceID AS FaceID, @CustomID AS CustomID,@Age AS Age, 
			@ProtectID AS ProtectID, @MasterOrder AS MasterOrder, @UserGold AS Gold, @InsureGold AS InsureGold, 0 AS UserCard, 0 AS MoorMachine, @Gender AS Gender, @Province AS Province, @City AS City, @Area AS Area,
			@Experience AS Experience, @MemberPoint AS MemberPoint, @InfoPublic AS InfoPublic, @MemberOverTime AS MemberOverTime,@ActionMaskEver AS ActionMaskEver,@ActionMaskPerDay AS ActionMaskPerDay,
			@ActionMaskPerWeek AS ActionMaskPerWeek, @LikedNumber AS LikedNumber, @GameLogonTimes AS GameLogonTimes,@ParentGameID AS ParentGameID,@ParentFaceID AS ParentFaceID,@ParentNickName AS ParentNickName,
			@LockGateID AS LockGateID, @LockLogonID AS LockLogonID, @LockSocketID AS LockSocketID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
