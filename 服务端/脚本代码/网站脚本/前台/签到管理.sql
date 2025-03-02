----------------------------------------------------------------------
-- 时间：2021-11-01
-- 用途：每日签到  
----------------------------------------------------------------------

USE [LHGameGoldDB]
GO

-- 签到信息
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].WEB_GameMBCheckInInfo') AND OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].WEB_GameMBCheckInInfo
GO

-- 每日签到
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].WEB_GameMBCheckIn') AND OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].WEB_GameMBCheckIn
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

----------------------------------------------------------------------------------
-- 签到信息
CREATE PROCEDURE WEB_GameMBCheckInInfo
	@dwUserID INT,								-- 用户标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID			INT
DECLARE @Nullity		TINYINT

-- 记录信息
DECLARE @RecordID INT

-- 执行逻辑
BEGIN
	-- 查询用户	
	SELECT @UserID=UserID,@Nullity=Nullity FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
	
	-- 用户存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试！'
		RETURN 100
	END	

	-- 帐号封禁
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 101
	END		

	-- 签到总次数
	DECLARE @AllCount INT
	SELECT @AllCount=COUNT(*) FROM CheckInItem

	-- 签到逻辑
	DECLARE @IsCheck	INT	 -- 是否签到
	DECLARE @LxCount	INT  -- 连续次数
	DECLARE @LxScore	INT  -- 连续金币			

	SELECT @LxCount=LxCount,@LxScore=LxScore FROM LHGameRecordDB.dbo.RecordCheckIn(NOLOCK) WHERE UserID=@dwUserID AND DATEDIFF(D,CollectDate,GETDATE())=0
	IF @LxCount IS NULL
	BEGIN	
		SET @IsCheck=0
		SELECT @LxCount=LxCount,@LxScore=LxScore FROM LHGameRecordDB.dbo.RecordCheckIn(NOLOCK) WHERE UserID=@dwUserID AND DATEDIFF(D,CollectDate,GETDATE())=1
		IF @LxCount IS NULL
		BEGIN
			SET @LxCount=0
			SET @LxScore=0
		END
		ELSE
		BEGIN
			IF @LxCount=@AllCount
			BEGIN
				SET @LxCount=0
				SET @LxScore=0
			END
		END
	END
	ELSE
	BEGIN
		SET @IsCheck=1
	END	

	-- 输出参数
	SELECT @UserID AS UserID,@IsCheck AS IsCheck,@AllCount AS AllCount,@LxCount AS LxCount,@LxScore AS LxScore
	
END
RETURN 0
GO

----------------------------------------------------------------------------------
-- 每日签到
CREATE PROCEDURE WEB_GameMBCheckIn
	@dwUserID			INT,					-- 用户标识	
	@strMachineID		NVARCHAR(32),			-- 机器码         
	@strClientIP		NVARCHAR(15),			-- 签到地址
	@strErrorDescribe	NVARCHAR(127) OUTPUT	-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID			INT
DECLARE @GameID			INT
DECLARE @Accounts		NVARCHAR(32)
DECLARE @Nullity		TINYINT
DECLARE @Score			BIGINT
DECLARE @BeforeScore	BIGINT

-- 执行逻辑
BEGIN
	-- 查询用户	
	SELECT @UserID=UserID,@GameID=GameID,@Accounts=Accounts,@Nullity=Nullity FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID	

	-- 用户存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试！'
		RETURN 100
	END	

	-- 帐号封禁
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 101
	END	

	-- 记录查询
	IF EXISTS (SELECT RecordID FROM LHGameRecordDB.dbo.RecordCheckIn(NOLOCK) WHERE UserID=@dwUserID AND DATEDIFF(D,CollectDate,GETDATE())=0)
	BEGIN
		SET @strErrorDescribe=N'抱歉！您今天已签到,请明天再来。'
		RETURN 4
	END

	-- 签到机器限制
	DECLARE @CheckInCount INT
	SELECT @CheckInCount=COUNT(RecordID) FROM LHGameRecordDB.dbo.RecordCheckIn(NOLOCK) WHERE MachineID=@strMachineID AND DATEDIFF(D,CollectDate,GETDATE())=0 
	IF @CheckInCount>=3
	BEGIN
		SET @strErrorDescribe=N'抱歉！您今天已进行了多次签到操作，请明天再来。'
		RETURN 5
	END

	-- 签到总次数
	DECLARE @AllCount INT
	SELECT @AllCount=COUNT(*) FROM CheckInItem
	IF @AllCount=0
	BEGIN
		SET @strErrorDescribe=N'抱歉！签到数据加载失败。'
		RETURN 6
	END

	-- 签到逻辑
	DECLARE @ItemCount      INT	 -- 奖品数量			
	DECLARE @dwPresentScore	INT  -- 本次领取金币
	DECLARE @dwLxCount		INT  -- 本次连续次数
	DECLARE @dwLxScore		INT  -- 本次连续金币			
	DECLARE @dwLastLxCount	INT  -- 上次连续次数
	DECLARE @dwLastLxScore	INT  -- 上次连续金币

	SELECT @dwLastLxCount=LxCount,@dwLastLxScore=LxScore FROM LHGameRecordDB.dbo.RecordCheckIn(NOLOCK) WHERE UserID=@dwUserID AND DATEDIFF(D,CollectDate,GETDATE())=1
	IF @dwLastLxCount IS NULL
	BEGIN
		SET @dwLxCount=1
		SET @dwLastLxCount=0
		SET @dwLastLxScore=0
	END
	ELSE
	BEGIN
		IF @dwLastLxCount=@AllCount
		BEGIN		
			SET @dwLxCount=1
			SET @dwLastLxCount=0
			SET @dwLastLxScore=0
		END
		ELSE
		BEGIN		
			SET @dwLxCount=@dwLastLxCount+1
		END
	END

	-- 获取赠送信息
	SELECT @ItemCount=ItemCount FROM CheckInItem WHERE ItemIndex=@dwLxCount

	-- 赠送金币
	SET @dwPresentScore=@ItemCount
	SET @dwLxScore=@dwLastLxScore+@dwPresentScore

	-- 签到记录
	INSERT INTO LHGameRecordDB.dbo.RecordCheckIn(UserID,PresentScore,LxCount,LxScore,MachineID,IPAddress)
	VALUES(@dwUserID,@dwPresentScore,@dwLxCount,@dwLxScore,@strMachineID,@strClientIP) 

	-- 签到之前
	SELECT @BeforeScore=Score FROM LHAccountsDB.dbo.AccountsGoldInfo(NOLOCK) WHERE UserID=@dwUserID
	IF @BeforeScore IS NULL
	BEGIN
		SET @BeforeScore=0
	END

	-- 赠送金币
	UPDATE LHAccountsDB.dbo.AccountsGoldInfo SET Score=Score+@dwPresentScore,GrantTimes=GrantTimes+1,GrantAmount=GrantAmount+@dwPresentScore WHERE UserID=@dwUserID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO LHAccountsDB.dbo.AccountsGoldInfo(UserID,Score,GrantTimes,GrantAmount)
		VALUES(@dwUserID,@dwPresentScore,1,@dwPresentScore)
	END

	-- 变化日志
	INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID,GameID,TypeID,BeforeGold,ChangeGold,CollectClientIP,CollectDateTime)
	VALUES(@dwUserID,@GameID,101,@BeforeScore,@dwPresentScore,@strClientIP,GETDATE())
	
	-- 刷新财富
	SELECT @Score=Score FROM LHAccountsDB.dbo.AccountsGoldInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 输出变量
	SELECT @UserID AS UserID,
		   @dwPresentScore AS PresentScore,
		   @dwLxCount AS LxCount,
		   @dwLxScore AS LxScore,
		   @Score AS Score
END
RETURN 0
GO
