
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_EfficacyComputer]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_EfficacyComputer]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_EfficacyMobile]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_EfficacyMobile]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 电脑登录
CREATE PROC GSP_GR_EfficacyComputer
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@wKindID SMALLINT,							-- 游戏 I D
	@wServerID SMALLINT,						-- 房间 I D
	@dwRequestScore BIGINT,						-- 请求积分	
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NCHAR(32),					-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT
DECLARE @FaceID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @CustomID INT

-- 扩展信息
DECLARE @GameID INT
DECLARE @UserRight INT
DECLARE @MasterRight INT
DECLARE @MemberPoint INT
DECLARE @MasterOrder SMALLINT
DECLARE @MemberOrder SMALLINT

-- 用户信息
DECLARE @Age TINYINT
DECLARE @Gender TINYINT
DECLARE @Area NVARCHAR(15)
DECLARE @City NVARCHAR(15)
DECLARE @Province NVARCHAR(15)

-- 积分变量
DECLARE @Score BIGINT
DECLARE @UserGold BIGINT
DECLARE @UserCard BIGINT
DECLARE @InsureScore BIGINT
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @Experience INT
DECLARE @WinStreak INT
DECLARE @MaxWinStreak INT

-- 辅助变量
DECLARE @LockerScore BIGINT
DECLARE @EnjoinLogon BIGINT

-- 执行逻辑
BEGIN
	-- 系统暂停
	SELECT @EnjoinLogon=StatusValue FROM LHPlatformDB..SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		-- 提示信息
		DECLARE @StatusString NVARCHAR(128)
		SELECT @StatusString=StatusString FROM LHPlatformDB..SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		
		-- 调整提示
		IF @StatusString IS NULL SET @StatusString=N''

		SET @strErrorDescribe=N'{"Idx":1002,"Msg":"'+@StatusString+N'"}'
		RETURN 2
	END
 
	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @ShutUpStatus BIT
	DECLARE @ShutDownStatus BIT
	DECLARE @ShutUpOverTime INT
	DECLARE @LogonPass NCHAR(32)	
	
	-- 查询用户
	SELECT @UserID=a.UserID, @GameID=a.GameID, @Accounts=a.Accounts, @NickName=a.NickName, @UnderWrite=a.UnderWrite, @LogonPass=a.LogonPass, @FaceID=a.FaceID,  
		@CustomID=a.CustomID, @Gender=a.Gender, @Nullity=a.Nullity, @Experience=a.Experience, @UserRight=a.UserRight, @MasterRight=a.MasterRight, @MasterOrder=a.MasterOrder,
		@MemberPoint=a.MemberPoint, @ShutDownStatus=b.ShutDownStatus, @ShutUpStatus=b.ShutupStatus, @ShutUpOverTime=b.ShutupOverTime		
	FROM LHAccountsDB.dbo.AccountsInfo a LEFT JOIN LHAccountsDB.dbo.AccountsStatus b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

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
	IF @ShutDownStatus<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
		RETURN 2
	END

	-- 查询资料
	SELECT @Age=Age, @Area=Area, @City=City, @Province=Province FROM LHAccountsDB.dbo.AccountsDatum(NOLOCK) WHERE UserID=@dwUserID

	-- 调整资料
	IF @Age IS NULL SET @Age=N''
	IF @Area IS NULL SET @Area=N''
	IF @City IS NULL SET @City=N''
	IF @Province IS NULL SET @Province=N''

	-- 调整状态
	IF @ShutUpStatus IS NULL SET @ShutUpStatus=0
	IF @ShutUpOverTime IS NULL SET @ShutUpOverTime=0

	-- 在线判断
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT	
	DECLARE @LockServerKind INT	

	-- 查询数据
	SELECT @LockKindID=KindID,@LockServerID=ServerID,@LockServerKind=ServerKind FROM LHAccountsDB.dbo.AccountsOnlineInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 锁定判断
	IF @LockServerID<>0 AND @LockServerID<>@wServerID
	BEGIN	
		-- 锁定信息
		SELECT @LockKindID AS LockKindID,@LockServerID AS LockServerID,@LockServerKind AS LockServerKind

		-- 提示信息
		SET @strErrorDescribe='{"Idx":1102,"Msg":"您已经在游戏房间了，不能同时再进入其他游戏房间！"}'  
		RETURN 100
	END

	-- 设置分数
	SELECT @Score=0,@InsureScore=0,@WinCount=0, @LostCount=0, @DrawCount=0, @DrawCount=0, @FleeCount=0,@WinStreak=0, @MaxWinStreak=0

	-- 查询财富
	SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID
	SELECT @UserCard=Score FROM LHGameBattleDB.dbo.GameCardInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 调整变量
	IF @UserCard IS NULL SET @UserCard=0
	IF @UserGold IS NULL SET @UserGold=0

	-- 更新信息
	UPDATE GameScoreInfo SET AllLogonTimes=AllLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,LastLogonMachine=@strMachineID WHERE UserID=@dwUserID

	-- 进入记录
	INSERT RecordUserInout(UserID,KindID,ServerID,EnterTime,EnterScore,EnterMachine,EnterClientIP,ClubID,ScoreKind,BattleNum)
	VALUES(@dwUserID,@wKindID,@wServerID,GetDate(),@Score,@strMachineID,@strClientIP,0,0,0)
	
	-- 记录标识
	DECLARE @InoutIndex BIGINT
	SET @InoutIndex=SCOPE_IDENTITY()

	-- 日期标识
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 登录统计	
	UPDATE SystemStreamInfo SET LogonCount=LogonCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, LogonCount) VALUES (@DateID, @wKindID, @wServerID, 1)

	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @LogonPass AS LogonPass, @NickName AS Accounts, @UnderWrite AS UnderWrite, @FaceID AS FaceID, 
		@CustomID AS CustomID, @Age AS Age, @Gender AS Gender, @Province AS Province, @City AS City, @Area AS Area, 0 AS ClubID, N'' AS ClubName, @MasterOrder AS MemberOrder, 
		@UserRight AS UserRight, @MasterRight AS MasterRight, @MasterOrder AS MasterOrder, @MemberPoint AS MemberPoint, @Score AS Score, 3 AS ScoreKind,
		@UserGold AS Gold, @UserCard AS UserCard, @WinCount AS WinCount, @LostCount AS LostCount, @DrawCount AS DrawCount, @FleeCount AS FleeCount, @InsureScore AS InsureScore, @Score AS FreeScore, 
		@Experience AS Experience,@InoutIndex AS InoutIndex,@WinStreak AS WinStreak, @MaxWinStreak AS MaxWinStreak,@ShutUpStatus AS ShutUpStatus, @ShutUpOverTime AS ShutUpOverTime

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 手机登录
CREATE PROC GSP_GR_EfficacyMobile
	-- 用户信息
	@dwUserID INT,								-- 用户 I D
	@wKindID SMALLINT,							-- 游戏 I D
	@wServerID SMALLINT,						-- 房间 I D
	@dwRequestScore BIGINT,						-- 请求积分	
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NCHAR(32),					-- 机器标识
	@dwDeviceKind SMALLINT,						-- 设备类型

	-- 约战信息
	@dwClubID INT,								-- 茶馆标识	
	@dwBattleNum INT,							-- 约战房号
	@cbScoreKind TINYINT,						-- 分数类型
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT
DECLARE @FaceID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @CustomID INT

-- 扩展信息
DECLARE @GameID INT
DECLARE @UserRight INT
DECLARE @MasterRight INT
DECLARE @MasterOrder SMALLINT
DECLARE @MemberPoint INT

-- 用户信息
DECLARE @Age TINYINT
DECLARE @Gender TINYINT
DECLARE @Area NVARCHAR(15)
DECLARE @City NVARCHAR(15)
DECLARE @Province NVARCHAR(15)

-- 积分变量
DECLARE @Score BIGINT
DECLARE @UserGold BIGINT
DECLARE @UserCard BIGINT
DECLARE @InsureScore BIGINT
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @Experience INT
DECLARE @WinStreak INT
DECLARE @MaxWinStreak INT

-- 群组信息
DECLARE @ClubMemberType TINYINT

-- 辅助变量
DECLARE @LockerScore BIGINT
DECLARE @EnjoinLogon BIGINT

-- 执行逻辑
BEGIN
	-- 系统暂停
	SELECT @EnjoinLogon=StatusValue FROM LHPlatformDB..SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		-- 提示信息
		DECLARE @StatusString NVARCHAR(128)
		SELECT @StatusString=StatusString FROM LHPlatformDB..SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		
		-- 调整提示
		IF @StatusString IS NULL SET @StatusString=N''

		SET @strErrorDescribe=N'{"Idx":1002,"Msg":"'+@StatusString+N'"}'
		RETURN 2
	END
 
	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @ShutUpStatus BIT
	DECLARE @ShutDownStatus BIT
	DECLARE @ShutUpOverTime INT
	DECLARE @LogonPass NCHAR(32)	
	
	-- 查询用户
	SELECT @UserID=a.UserID, @GameID=a.GameID, @Accounts=a.Accounts, @NickName=a.NickName, @UnderWrite=a.UnderWrite, @LogonPass=a.LogonPass, @FaceID=a.FaceID,  
		@CustomID=a.CustomID, @Gender=a.Gender, @Nullity=a.Nullity, @Experience=a.Experience, @UserRight=a.UserRight, @MasterRight=a.MasterRight, @MasterOrder=a.MasterOrder,
		@MemberPoint=a.MemberPoint, @ShutDownStatus=b.ShutDownStatus, @ShutUpStatus=b.ShutupStatus, @ShutUpOverTime=b.ShutupOverTime		
	FROM LHAccountsDB.dbo.AccountsInfo a LEFT JOIN LHAccountsDB.dbo.AccountsStatus b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

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
	IF @ShutDownStatus<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
		RETURN 2
	END

	-- 查询资料
	SELECT @Age=Age, @Area=Area, @City=City, @Province=Province FROM LHAccountsDB.dbo.AccountsDatum(NOLOCK) WHERE UserID=@dwUserID

	-- 调整资料
	IF @Age IS NULL SET @Age=N''
	IF @Area IS NULL SET @Area=N''
	IF @City IS NULL SET @City=N''
	IF @Province IS NULL SET @Province=N''

	-- 调整状态
	IF @ShutUpStatus IS NULL SET @ShutUpStatus=0
	IF @ShutUpOverTime IS NULL SET @ShutUpOverTime=0

	-- 在线判断
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT	
	DECLARE @LockServerKind INT	

	-- 查询数据
	SELECT @LockKindID=KindID,@LockServerID=ServerID,@LockServerKind=ServerKind FROM LHAccountsDB.dbo.AccountsOnlineInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 锁定判断
	IF @LockServerID<>0 AND @LockServerID<>@wServerID
	BEGIN	
		-- 锁定信息
		SELECT @LockKindID AS LockKindID,@LockServerID AS LockServerID,@LockServerKind AS LockServerKind

		-- 提示信息
		SET @strErrorDescribe='{"Idx":1102,"Msg":"您已经在游戏房间了，不能同时再进入其他游戏房间！"}'  
		RETURN 100
	END

	-- 约战锁定	
	DECLARE @BattleNum INT
	DECLARE @BattleStatus TINYINT
	DECLARE @OnlineStatus TINYINT
	SELECT @BattleNum=BattleNum,@BattleStatus=BattleStatus,@OnlineStatus=OnlineStatus FROM BattleUserLocker(NOLOCK) WHERE UserID=@dwUserID

	-- 调整状态
	IF @BattleStatus IS NULL SET @BattleStatus=0

	-- 约战状态
	IF @BattleStatus=1
	BEGIN	
		-- 校验房号
		IF @BattleNum<>@dwBattleNum
		BEGIN
			-- 提示信息
			SET @strErrorDescribe=N'{"Idx":1177,"Msg":"由于您在约战房间['+CONVERT(NVARCHAR(12),@BattleNum)+']的游戏尚未完成，暂时不能加入其它约战房间！","Ops":["'+CONVERT(NVARCHAR(12),@BattleNum)+'"]}'

			RETURN 5
		END
	END
	
	-- 在线状态
	IF @OnlineStatus=1
	BEGIN
		-- 提示信息
		SET @strErrorDescribe=N'{"Idx":1177,"Msg":"由于您在约战房间['+CONVERT(NVARCHAR(12),@BattleNum)+']的游戏尚未完成，暂时不能加入其它约战房间！","Ops":["'+CONVERT(NVARCHAR(12),@BattleNum)+'"]}'
		RETURN 5		
	END		

	-- 游戏信息
	DECLARE @PlayTimeCount INT
	DECLARE @GameUserRight INT
	DECLARE @GameMasterRight INT
	DECLARE @GameMasterOrder SMALLINT	

	-- 设置分数
	SELECT @InsureScore=0

	-- 查询游戏信息
	SELECT  @Score=Score, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount, @FleeCount=FleeCount, @GameUserRight=UserRight, @GameMasterRight=MasterRight, 
			@WinStreak=WinStreak, @MaxWinStreak=MaxWinStreak, @GameMasterOrder=MasterOrder, @PlayTimeCount=PlayTimeCount
	FROM GameScoreInfo WHERE UserID=@dwUserID

	-- 插入数据
	IF @WinCount IS NULL
	BEGIN
		INSERT INTO GameScoreInfo(UserID,Score,AllLogonTimes,LastLogonIP,LastLogonDate,LastLogonMachine,RegisterIP,RegisterDate,RegisterMachine,WinStreak,MaxWinStreak)
		VALUES (@dwUserID,0,1,@strClientIP,GetDate(),@strMachineID,@strClientIP,GetDate(),@strMachineID,0,0)	

		SELECT @Score=0, @WinCount=0, @LostCount=0, @DrawCount=0, @DrawCount=0, @FleeCount=0,@WinStreak=0,@MaxWinStreak=0, @GameUserRight=0, @GameMasterRight=0, @GameMasterOrder=0,@PlayTimeCount=0		
	END

	-- 馆内约战
	IF  @dwClubID<>0  
	BEGIN
		-- 变量定义
		DECLARE	@TableID	INT											
		DECLARE	@TableName	NVARCHAR(128)		
		DECLARE @SqlString	NVARCHAR(512)								
		DECLARE @ClubUserScore BIGINT
		DECLARE @ClubMemberStatus TINYINT

		-- 分表标识				
		SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

		-- 群组成员表
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

		-- 查询数据表
		IF (EXISTS(SELECT 1 FROM LHGameBattleDB.dbo.sysobjects WHERE xtype='u' AND name=@TableName))
		BEGIN
			-- 查询脚本		
			SET	@SqlString = CONCAT( ' SELECT @MemberStatus=MemberStatus,@MemberType=MemberType,@ClubScore=ClubScore FROM  ',
								  @TableName,'(NOLOCK) WHERE MemberID=',CONVERT(NVARCHAR(18), @dwUserID),' AND ',' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

			-- 执行脚本
			EXEC SP_EXECUTESQL @stmt=@SqlString,@params=N'@MemberStatus TINYINT OUT,@MemberType TINYINT OUT, @ClubScore INT OUT', @MemberStatus=@ClubMemberStatus OUTPUT,@MemberType=@ClubMemberType OUTPUT,@ClubScore=@ClubUserScore OUTPUT
		END	

		-- 调整变量
		IF @ClubUserScore IS NULL SET @ClubUserScore=0
		IF @ClubMemberStatus IS NULL SET @ClubMemberStatus=0

		-- 冻结校验
		IF @ClubMemberStatus=1
		BEGIN
			-- 提示信息
			SET @strErrorDescribe='{"Idx":1178,"Msg":"您已被俱乐部老板冻结游戏权限，请联系老板解除！"}'  
			RETURN 6
		END			
		
		-- 茶社积分		
		IF @cbScoreKind=2 SET @Score=@ClubUserScore
	END	

	-- 调整变量	
	IF @ClubMemberType IS NULL SET @ClubMemberType=0	

	-- 重置约战积分
	IF @cbScoreKind=3 AND(@BattleNum IS NULL OR @BattleNum=0)
	BEGIN
		SET @Score=0		
	END

	-- 更新记录
	UPDATE GameScoreInfo SET Score=@Score,AllLogonTimes=AllLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP, LastLogonMachine=@strMachineID WHERE UserID=@dwUserID 	

	-- 约战锁定
	IF @BattleNum IS NULL
	BEGIN			
		INSERT INTO BattleUserLocker(UserID,KindID,ServerID,ScoreKind,BattleNum,OnlineStatus) VALUES(@dwUserID,@wKindID,@wServerID,@cbScoreKind,@dwBattleNum,1)
	END	ELSE
	BEGIN
		UPDATE BattleUserLocker SET KindID=@wKindID,ServerID=@wServerID,ScoreKind=@cbScoreKind,BattleNum=@dwBattleNum,OnlineStatus=1 WHERE UserID=@dwUserID		
	END
	
	-- 在线状态
	UPDATE LHAccountsDB.dbo.AccountsOnlineInfo SET KindID=@wKindID,ServerID=@wServerID,ServerKind=8,ModifyTime=GETDATE() WHERE UserID=@dwUserID

	-- 查询财富
	SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID
	SELECT @UserCard=Score FROM LHGameBattleDB.dbo.GameCardInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 调整变量
	IF @UserCard IS NULL SET @UserCard=0	
	IF @UserGold IS NULL SET @UserGold=0

	-- 权限标志
	SET @UserRight=@UserRight|@GameUserRight
	SET @MasterRight=@MasterRight|@GameMasterRight

	-- 权限等级
	IF @MasterOrder<>0 OR @GameMasterOrder<>0
	BEGIN
		IF @GameMasterOrder>@MasterOrder SET @MasterOrder=@GameMasterOrder
	END
	ELSE SET @MasterRight=0

	-- 进入记录
	INSERT RecordUserInout(UserID,KindID,ServerID,DeviceType,EnterTime,EnterScore,EnterMachine,EnterClientIP,ClubID,ScoreKind,BattleNum)
	VALUES(@dwUserID,@wKindID,@wServerID,@dwDeviceKind,GetDate(),@Score,@strMachineID,@strClientIP,@dwClubID,@cbScoreKind,@dwBattleNum)
	
	-- 记录标识
	DECLARE @InoutIndex BIGINT
	SET @InoutIndex=SCOPE_IDENTITY()

	-- 日期标识
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 登录统计	
	UPDATE SystemStreamInfo SET LogonCount=LogonCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, LogonCount) VALUES (@DateID, @wKindID, @wServerID, 1)

	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @LogonPass AS LogonPass, @NickName AS Accounts,@UnderWrite AS UnderWrite, @FaceID AS FaceID, @CustomID AS CustomID, @Age AS Age, @Gender AS Gender, 
		@Province AS Province, @City AS City,@Area AS Area, @MasterOrder AS MemberOrder, @MemberPoint AS MemberPoint, @UserRight AS UserRight, @MasterRight AS MasterRight, @MasterOrder AS MasterOrder, 
		@Score AS Score, @cbScoreKind AS ScoreKind, @UserGold AS Gold, @UserCard AS UserCard, @WinCount AS WinCount, @LostCount AS LostCount, @DrawCount AS DrawCount, @FleeCount AS FleeCount, 
		@WinStreak AS WinStreak, @MaxWinStreak AS MaxWinStreak,@InsureScore AS InsureScore, @Score AS FreeScore, @Experience AS Experience, @ShutUpStatus AS ShutUpStatus, @ShutUpOverTime AS ShutUpOverTime,
		@InoutIndex AS InoutIndex, @ClubMemberType AS MemberType

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------