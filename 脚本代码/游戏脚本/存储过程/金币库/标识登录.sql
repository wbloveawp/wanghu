
USE LHGameGoldDB
GO
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_EfficacyRobot]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_EfficacyRobot]
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

-- 机器登录
CREATE PROC GSP_GR_EfficacyRobot
	@dwUserID INT,								-- 用户 I D
	@wKindID SMALLINT,							-- 游戏 I D
	@wServerID SMALLINT,						-- 房间 I D
	@dwRequestScore BIGINT,						-- 请求积分
	@strClientIP NVARCHAR(15),					-- 连接地址	
	@strMachineID NCHAR(32),					-- 机器标识	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 自动提交
SET XACT_ABORT ON

-- 基本信息
DECLARE @UserID INT
DECLARE @FaceID SMALLINT
DECLARE @CustomID INT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)


-- 扩展信息
DECLARE @GameID INT
DECLARE @UserRight INT
DECLARE @MasterRight INT
DECLARE @MemberPoint INT
DECLARE @MasterOrder SMALLINT

-- 用户信息
DECLARE @Age TINYINT
DECLARE @Gender TINYINT
DECLARE @Area NVARCHAR(15)
DECLARE @City NVARCHAR(15)
DECLARE @Province NVARCHAR(15)

-- 积分变量
DECLARE @Score BIGINT
DECLARE @InsureScore BIGINT
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @Experience INT
DECLARE @WinStreak INT
DECLARE @MaxWinStreak INT

-- 输赢统计
DECLARE @AddupWinRate SMALLINT
DECLARE @AddupWinScore BIGINT
DECLARE @AddupLoseScore BIGINT

-- 辅助变量
DECLARE @LockerScore BIGINT
DECLARE @EnjoinLogon BIGINT

-- 执行逻辑
BEGIN
	-- 系统暂停
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		-- 提示信息
		DECLARE @StatusString NVARCHAR(128)
		SELECT @StatusString=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		
		-- 调整提示
		IF @StatusString IS NULL SET @StatusString=N''

		SET @strErrorDescribe=N'{"Idx":1002,"Msg":"'+@StatusString+N'"}'
		RETURN 2
	END
 
	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @IsRobot TINYINT
	DECLARE @LogonPass NCHAR(32)
	DECLARE	@MachineID NCHAR(32)
	DECLARE @MoorMachine TINYINT	
	SELECT @UserID=a.UserID, @GameID=a.GameID, @Accounts=a.Accounts, @NickName=a.NickName, @UnderWrite=a.UnderWrite, @LogonPass=a.LogonPass, @FaceID=a.FaceID, @CustomID=a.CustomID, @Gender=a.Gender, 
		@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus, @Experience=a.Experience, @UserRight=a.UserRight, @MasterRight=a.MasterRight, @MasterOrder=a.MasterOrder, @IsRobot=a.IsRobot, @MemberPoint=a.MemberPoint,
		@MoorMachine=a.MoorMachine, @MachineID=a.LogonMachineID
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
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
		RETURN 2
	END	
	
	-- 固定机器
	IF @MoorMachine=1
	BEGIN
		IF @MachineID<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1024,"Msg":"您的帐号使用锁定机器登陆功能，您现所使用的机器不是所指定的机器！"}'
			RETURN 1
		END
	END

	-- 密码判断	
	IF @strClientIP<>N'0.0.0.0'
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 3			
	END			

	-- 固定机器
	IF @MoorMachine=2
	BEGIN
		SET @MoorMachine=1
		SET @strErrorDescribe=N'{"Idx":1025,"Msg":"您的帐号成功使用了锁定机器登陆功能！"}'
		UPDATE LHAccountsDB.dbo.AccountsInfo SET MoorMachine=@MoorMachine, LogonMachineID=@strMachineID WHERE UserID=@UserID
	END

	-- 查询资料
	SELECT @Age=Age, @Area=Area, @City=City, @Province=Province FROM LHAccountsDB.dbo.AccountsDatum(NOLOCK) WHERE UserID=@dwUserID

	-- 调整资料
	IF @Age IS NULL SET @Age=N''
	IF @Area IS NULL SET @Area=N''
	IF @City IS NULL SET @City=N''
	IF @Province IS NULL SET @Province=N''
	
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
		SET @strErrorDescribe='{"Idx":1102,"Msg":"您已经在金币游戏房间了，不能同时再进入此类游戏房间了！"}'  
		RETURN 100
	END

	-- 时间标识
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN	

	-- 游戏信息
	DECLARE @PlayTimeCount INT
	SELECT @Score=Score, @InsureScore=InsureScore,@LockerScore=LockerScore, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount, @FleeCount=FleeCount, 
		   @PlayTimeCount=PlayTimeCount, @WinStreak=WinStreak, @MaxWinStreak=MaxWinStreak, @AddupWinRate=AddupWinRate, @AddupWinScore=AddupWinScore,@AddupLoseScore=AddupLoseScore
	FROM GameScoreInfo WHERE UserID=@dwUserID

	-- 信息判断
	IF @LockerScore IS NULL
	BEGIN
		-- 插入资料
		INSERT INTO GameScoreInfo (UserID,RegisterIP, RegisterMachineID, LogonIP, LogonMachineID)	
		VALUES (@dwUserID,@strClientIP,@strMachineID,@strClientIP,@strMachineID)

		-- 游戏信息		
		SELECT @Score=Score, @InsureScore=InsureScore,@LockerScore=LockerScore, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount, @FleeCount=FleeCount, 
			   @PlayTimeCount=PlayTimeCount, @WinStreak=WinStreak, @MaxWinStreak=MaxWinStreak,@AddupWinRate=AddupWinRate, @AddupWinScore=AddupWinScore,@AddupLoseScore=AddupLoseScore
		FROM GameScoreInfo WHERE UserID=@dwUserID
	END

	-- 异常判断
	IF @dwRequestScore<0 OR (@Score-@LockerScore)<0	OR @Score<0 OR @LockerScore<0
	BEGIN
		-- 结束事务
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 错误信息
		SET @strErrorDescribe=N'{"Idx":1103,"Msg":"抱歉地通知你，你的游戏币数据异常，请联系客户服务中心了解详细情况！"}'
		RETURN 14
	END	

	-- 更新信息
	UPDATE GameScoreInfo SET LogonTimes=LogonTimes+1, LogonDate=GETDATE(), LogonIP=@strClientIP, LogonMachineID=@strMachineID WHERE UserID=@dwUserID

	-- 登录统计	
	UPDATE SystemStreamInfo SET LogonCount=LogonCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, LogonCount) VALUES (@DateID, @wKindID, @wServerID, 1)

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- 更新状态
	UPDATE LHAccountsDB.dbo.AccountsOnlineInfo SET KindID=@wKindID,ServerID=@wServerID,ModifyTime=GETDATE() WHERE UserID=@dwUserID

	-- 进入记录
	INSERT RecordUserInout(UserID, KindID, ServerID, LockerScore, EnterTime, EnterScore, EnterMachine, EnterClientIP)
	VALUES(@dwUserID,@wKindID,@wServerID,@Score,GetDate(),@Score,@strMachineID,@strClientIP)
	
	-- 记录标识
	DECLARE @InoutIndex BIGINT
	SET @InoutIndex=SCOPE_IDENTITY()
	
	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @NickName AS Accounts, @LogonPass AS LogonPass, @UnderWrite AS UnderWrite, @FaceID AS FaceID, @CustomID AS CustomID, @Age AS Age, @Gender AS Gender, 
		@Province AS Province, @City AS City, @Area AS Area, @MasterOrder AS MemberOrder, @UserRight AS UserRight, @MasterRight AS MasterRight, @MasterOrder AS MasterOrder, @MemberPoint AS MemberPoint, 
		@Score AS Gold, @Score AS Score, 1 AS ScoreKind, 0 AS RoomCard, @WinCount AS WinCount, @LostCount AS LostCount, @DrawCount AS DrawCount, @FleeCount AS FleeCount,@WinStreak AS WinStreak, @MaxWinStreak AS MaxWinStreak, 
		@InsureScore AS InsureScore, @Score AS FreeScore, @Experience AS Experience, @InoutIndex AS InoutIndex,0 AS ShutUpStatus, 0 AS ShutUpOverTime, @AddupWinRate AS AddupWinRate, @AddupWinScore AS AddupWinScore, 
		@AddupLoseScore AS AddupLoseScore,0 AS UserCard
END

RETURN 0

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

-- 自动提交
SET XACT_ABORT ON

-- 基本信息
DECLARE @UserID INT
DECLARE @FaceID SMALLINT
DECLARE @CustomID INT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)

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
DECLARE @InsureScore BIGINT
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @Experience INT
DECLARE @WinStreak INT
DECLARE @MaxWinStreak INT

-- 输赢统计
DECLARE @AddupWinRate SMALLINT
DECLARE @AddupWinScore BIGINT
DECLARE @AddupLoseScore BIGINT

-- 辅助变量
DECLARE @LockerScore BIGINT
DECLARE @EnjoinLogon BIGINT

-- 执行逻辑
BEGIN
	-- 系统暂停
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		-- 提示信息
		DECLARE @StatusString NVARCHAR(128)
		SELECT @StatusString=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		
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
	DECLARE	@MachineID NCHAR(32)
	DECLARE @LogonPass AS NCHAR(32)	
	
	-- 查询用户
	SELECT @UserID=a.UserID, @GameID=a.GameID, @Accounts=a.Accounts, @NickName=a.NickName, @UnderWrite=a.UnderWrite, @LogonPass=a.LogonPass, @FaceID=a.FaceID, @CustomID=a.CustomID, @Gender=a.Gender, 
		@Nullity=a.Nullity, @Experience=a.Experience, @UserRight=a.UserRight, @MasterRight=a.MasterRight, @MasterOrder=a.MasterOrder, @MemberPoint=a.MemberPoint, @MachineID=a.LogonMachineID, 
		@ShutDownStatus=b.ShutDownStatus, @ShutUpStatus=b.ShutupStatus, @ShutUpOverTime=b.ShutupOverTime
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
		SET @strErrorDescribe='{"Idx":1102,"Msg":"您已经在金币游戏房间了，不能同时再进入此类游戏房间了！"}'  
		RETURN 100
	END

	-- 时间标识
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 游戏信息
	DECLARE @PlayTimeCount INT
	SELECT @Score=Score, @InsureScore=InsureScore, @LockerScore=LockerScore, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount, @FleeCount=FleeCount, @PlayTimeCount=PlayTimeCount, 
		   @WinStreak=WinStreak, @MaxWinStreak=MaxWinStreak,@AddupWinRate=AddupWinRate, @AddupWinScore=AddupWinScore,@AddupLoseScore=AddupLoseScore
	FROM GameScoreInfo WHERE UserID=@dwUserID

	-- 信息判断
	IF @LockerScore IS NULL
	BEGIN
		-- 插入资料
		INSERT INTO GameScoreInfo (UserID,RegisterIP, RegisterMachineID, LogonIP, LogonMachineID)	
		VALUES (@dwUserID,@strClientIP,@strMachineID,@strClientIP,@strMachineID)

		-- 游戏信息		
		SELECT @Score=Score, @InsureScore=InsureScore,@LockerScore=LockerScore, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount, @FleeCount=FleeCount, @PlayTimeCount=PlayTimeCount,
			   @WinStreak=WinStreak, @MaxWinStreak=MaxWinStreak,@AddupWinRate=AddupWinRate, @AddupWinScore=AddupWinScore,@AddupLoseScore=AddupLoseScore
		FROM GameScoreInfo WHERE UserID=@dwUserID
	END

	-- 异常判断
	IF @dwRequestScore<0 OR (@Score-@LockerScore)<0	OR @Score<0 OR @LockerScore<0
	BEGIN
		-- 结束事务
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 错误信息
		SET @strErrorDescribe=N'{"Idx":1103,"Msg":"抱歉地通知你，你的游戏币数据异常，请联系客户服务中心了解详细情况！"}'
		RETURN 14
	END	

	-- 更新信息
	UPDATE GameScoreInfo SET LogonTimes=LogonTimes+1, LogonDate=GETDATE(), LogonIP=@strClientIP, LogonMachineID=@strMachineID WHERE UserID=@dwUserID

	-- 登录统计	
	UPDATE SystemStreamInfo SET LogonCount=LogonCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, LogonCount) VALUES (@DateID, @wKindID, @wServerID, 1)

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- 更新状态
	UPDATE LHAccountsDB.dbo.AccountsOnlineInfo SET KindID=@wKindID,ServerID=@wServerID,ServerKind=1,ModifyTime=GETDATE() WHERE UserID=@dwUserID

	-- 进入记录
	INSERT RecordUserInout(UserID, KindID, ServerID, LockerScore, EnterTime, EnterScore, EnterMachine, EnterClientIP)
	VALUES(@dwUserID,@wKindID,@wServerID,@Score,GetDate(),@Score,@strMachineID,@strClientIP)
	
	-- 记录标识
	DECLARE @InoutIndex BIGINT
	SET @InoutIndex=SCOPE_IDENTITY()

	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @NickName AS Accounts, @LogonPass AS LogonPass, @UnderWrite AS UnderWrite, @FaceID AS FaceID, @CustomID AS CustomID, @Age AS Age, @Gender AS Gender, 
		@Province AS Province, @City AS City, @Area AS Area, @MasterOrder AS MemberOrder, @UserRight AS UserRight, @MasterRight AS MasterRight, @MasterOrder AS MasterOrder, @MemberPoint AS MemberPoint, 
		@Score AS Gold, @Score AS Score,1 AS ScoreKind, 0 AS RoomCard, @WinCount AS WinCount, @LostCount AS LostCount, @DrawCount AS DrawCount, @FleeCount AS FleeCount, @WinStreak AS WinStreak, @MaxWinStreak AS MaxWinStreak,
		@InsureScore AS InsureScore, 0 AS FreeScore, @Experience AS Experience, @InoutIndex AS InoutIndex, @ShutUpStatus AS ShutUpStatus, @ShutUpOverTime AS ShutUpOverTime, @AddupWinRate AS AddupWinRate,
		@AddupWinScore AS AddupWinScore, @AddupLoseScore AS AddupLoseScore,0 AS UserCard

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 手机登录
CREATE PROC GSP_GR_EfficacyMobile
	@dwUserID INT,								-- 用户 I D
	@wKindID SMALLINT,							-- 游戏 I D
	@wServerID SMALLINT,						-- 房间 I D
	@dwRequestScore BIGINT,						-- 请求积分
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NCHAR(32),					-- 机器标识
	@dwDeviceKind INT,							-- 设备类型
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 自动提交
SET XACT_ABORT ON

-- 基本信息
DECLARE @UserID INT
DECLARE @FaceID SMALLINT
DECLARE @CustomID INT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)

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
DECLARE @InsureScore BIGINT
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @Experience INT
DECLARE @WinStreak INT
DECLARE @MaxWinStreak INT

-- 输赢统计
DECLARE @AddupWinRate SMALLINT
DECLARE @AddupWinScore BIGINT
DECLARE @AddupLoseScore BIGINT

-- 辅助变量
DECLARE @LockerScore BIGINT
DECLARE @EnjoinLogon BIGINT

-- 执行逻辑
BEGIN
	-- 系统暂停
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		-- 提示信息
		DECLARE @StatusString NVARCHAR(128)
		SELECT @StatusString=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		
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
	DECLARE	@MachineID NCHAR(32)
	DECLARE @LogonPass AS NCHAR(32)	
	
	-- 查询用户
	SELECT @UserID=a.UserID, @GameID=a.GameID, @Accounts=a.Accounts, @NickName=a.NickName, @UnderWrite=a.UnderWrite, @LogonPass=a.LogonPass, @FaceID=a.FaceID, @CustomID=a.CustomID, @Gender=a.Gender, 
		@Nullity=a.Nullity, @Experience=a.Experience, @UserRight=a.UserRight, @MasterRight=a.MasterRight, @MasterOrder=a.MasterOrder, @MemberPoint=a.MemberPoint, @MachineID=a.LogonMachineID, 
		@ShutDownStatus=b.ShutDownStatus, @ShutUpStatus=b.ShutupStatus, @ShutUpOverTime=b.ShutupOverTime
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
		SET @strErrorDescribe='{"Idx":1102,"Msg":"您已经在金币游戏房间了，不能同时再进入此类游戏房间了！"}'  
		RETURN 100
	END

	-- 时间标识
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN	

	-- 游戏信息
	DECLARE @PlayTimeCount INT
	SELECT @Score=Score, @InsureScore=InsureScore, @LockerScore=LockerScore, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount, @FleeCount=FleeCount, @PlayTimeCount=PlayTimeCount, 
		   @WinStreak=WinStreak, @MaxWinStreak=MaxWinStreak, @AddupWinRate=AddupWinRate, @AddupWinScore=AddupWinScore, @AddupLoseScore=AddupLoseScore
	FROM GameScoreInfo WHERE UserID=@dwUserID

	-- 信息判断
	IF @LockerScore IS NULL
	BEGIN
		-- 插入资料
		INSERT INTO GameScoreInfo (UserID, RegisterIP, RegisterMachineID, LogonIP, LogonMachineID)	
		VALUES (@dwUserID,@strClientIP,@strMachineID,@strClientIP,@strMachineID)

		-- 游戏信息		
		SELECT @Score=Score, @InsureScore=InsureScore, @LockerScore=LockerScore, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount, @FleeCount=FleeCount, @PlayTimeCount=PlayTimeCount,
			   @WinStreak=WinStreak, @MaxWinStreak=MaxWinStreak, @AddupWinRate=AddupWinRate, @AddupWinScore=AddupWinScore, @AddupLoseScore=AddupLoseScore
		FROM GameScoreInfo WHERE UserID=@dwUserID
	END

	-- 异常判断
	IF @dwRequestScore<0 OR (@Score-@LockerScore)<0	OR @Score<0 OR @LockerScore<0
	BEGIN
		-- 结束事务
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 错误信息
		SET @strErrorDescribe=N'{"Idx":1103,"Msg":"抱歉地通知你，你的游戏币数据异常，请联系客户服务中心了解详细情况！"}'
		RETURN 14
	END	

	-- 更新信息
	UPDATE GameScoreInfo SET LogonTimes=LogonTimes+1, LogonDate=GETDATE(), LogonIP=@strClientIP, LogonMachineID=@strMachineID WHERE UserID=@dwUserID

	-- 登录统计	
	UPDATE SystemStreamInfo SET LogonCount=LogonCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, LogonCount) VALUES (@DateID, @wKindID, @wServerID, 1)

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- 更新状态
	UPDATE LHAccountsDB.dbo.AccountsOnlineInfo SET KindID=@wKindID,ServerID=@wServerID,ServerKind=1,ModifyTime=GETDATE() WHERE UserID=@dwUserID

	-- 进入记录
	INSERT RecordUserInout(UserID, KindID, ServerID, LockerScore, EnterTime, EnterScore, EnterMachine, EnterClientIP)
	VALUES(@dwUserID,@wKindID,@wServerID,@Score,GetDate(),@Score,@strMachineID,@strClientIP)

	-- 记录标识
	DECLARE @InoutIndex BIGINT 
	SET @InoutIndex=SCOPE_IDENTITY()

	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @LogonPass AS LogonPass, @NickName AS Accounts, @UnderWrite AS UnderWrite, @FaceID AS FaceID, @CustomID AS CustomID, @Age AS Age, 
		@Gender AS Gender, @Province AS Province, @City AS City, @Area AS Area, @MasterOrder AS MemberOrder, @UserRight AS UserRight, @MasterRight AS MasterRight, @MasterOrder AS MasterOrder, 
		@MemberPoint AS MemberPoint, @Score AS Gold, @Score AS Score, 1 AS ScoreKind, 0 AS UserCard, @WinCount AS WinCount, @LostCount AS LostCount, @DrawCount AS DrawCount, @FleeCount AS FleeCount,
		@WinStreak AS WinStreak, @MaxWinStreak AS MaxWinStreak, @InsureScore AS InsureScore, @Score AS FreeScore, @Experience AS Experience, @InoutIndex AS InoutIndex, @ShutUpStatus AS ShutUpStatus, 
		@ShutUpOverTime AS ShutUpOverTime,@AddupWinRate AS AddupWinRate, @AddupWinScore AS AddupWinScore, @AddupLoseScore AS AddupLoseScore

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
