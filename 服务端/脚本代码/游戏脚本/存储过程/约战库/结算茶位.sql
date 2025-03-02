----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_BalanceClubRevenue]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_BalanceClubRevenue]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 结算茶位
CREATE PROC GSP_GR_BalanceClubRevenue	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 自动回滚
SET XACT_ABORT ON

-- 执行逻辑
BEGIN

	-- 日期标识
	DECLARE @DateID INT
	SET @DateID = CAST(CAST(DATEADD(dd, -1, GETDATE()) AS FLOAT) AS INT)

	-- 老板税收
	DECLARE @TableCreaterRevenue TABLE
	(
		UserID INT NOT NULL,
		TotalRevenue BIGINT
	)

	-- 用户税收
	DECLARE @TableUserRevenue TABLE
	(
		UserID INT NOT NULL PRIMARY KEY,
		GameID INT,
		UserScore BIGINT,
		TotalRevenue BIGINT
	)

	-- 插入数据
	INSERT INTO @TableCreaterRevenue(UserID,TotalRevenue)
	SELECT b.CreaterID,SUM(a.Revenue) FROM  StreamClubUser(NOLOCK) a LEFT JOIN ClubProperty(NOLOCK) b ON a.DateID=@DateID AND a.ClubID=b.ClubID WHERE b.ClubKind=0 
	GROUP BY a.ClubID,b.CreaterID
	
	-- 退出判断
	IF NOT EXISTS(SELECT 1 FROM @TableCreaterRevenue)
	BEGIN
		RETURN 1
	END
	
	-- 用户税收
	INSERT INTO @TableUserRevenue(UserID,UserScore,TotalRevenue) SELECT UserID,0,SUM(TotalRevenue) FROM @TableCreaterRevenue GROUP BY UserID HAVING SUM(TotalRevenue)>0

	-- 游戏标识
	UPDATE @TableUserRevenue SET GameID=a.GameID FROM LHAccountsDB.dbo.AccountsInfo a, @TableUserRevenue b WHERE b.UserID=a.UserID

	-- 当前金币
	UPDATE @TableUserRevenue SET UserScore=a.Score FROM LHGameGoldDB.dbo.GameScoreInfo a, @TableUserRevenue b WHERE b.UserID=a.UserID	

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 更新金币
	UPDATE LHGameGoldDB.dbo.GameScoreInfo SET Score+=b.TotalRevenue FROM LHGameGoldDB.dbo.GameScoreInfo a, @TableUserRevenue b WHERE a.UserID=b.UserID

	-- 变更记录
	INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID, GameID, TypeID, BeforeGold, ChangeGold, CollectClientIP, CollectDateTime)
	SELECT UserID, GameID, 77, UserScore, TotalRevenue, N'------------', GetDate() FROM @TableUserRevenue

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
