
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_OnLineCountCollect]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_OnLineCountCollect]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

--在线信息
CREATE  PROCEDURE dbo.GSP_GP_OnLineCountCollect	
WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

-- 变量定义
DECLARE @OnLineUserCount INT
DECLARE @OnLinePlazaCount INT
DECLARE @OnLineServerCount INT
DECLARE @OnLineGoldServerCount INT
DECLARE @OnLineMatchServerCount INT
DECLARE @OnLineBattleServerCount INT
DECLARE @OnLineRankingServerCount INT

BEGIN

	-- 表定义
	DECLARE @TableUserMarketID TABLE (ID INT identity(1,1),MarketID INT)
	DECLARE @TableUserCount TABLE (MarketID INT, ServerKind SMALLINT, UserCount SMALLINT)

	-- 统计人数
	INSERT INTO @TableUserCount(MarketID,ServerKind,UserCount) SELECT b.MarketID, a.ServerKind,COUNT(1) FROM LHAccountsDB.dbo.AccountsOnlineInfo(NOLOCK) a INNER JOIN LHAccountsDB.dbo.AccountsInfo(NOLOCK) b ON a.UserID=b.UserID
	WHERE a.LogonID<>0 OR a.ServerID<>0 GROUP BY b.MarketID,a.ServerKind 

	-- 统计包ID
	INSERT INTO @TableUserMarketID(MarketID) VALUES (0)
	INSERT INTO @TableUserMarketID(MarketID) SELECT MarketID FROM @TableUserCount GROUP BY MarketID

	-- 变量定义
	DECLARE @Index INT	
	DECLARE @LoopCount INT
	DECLARE @MarketID INT
	
	-- 统计数量
	SELECT @Index=1,@LoopCount=COUNT(1) FROM @TableUserMarketID

	-- 循环处理
	WHILE @Index<=@LoopCount
	BEGIN	
		-- 查询信息
		SELECT @MarketID=MarketID FROM @TableUserMarketID WHERE ID=@Index

		-- 统计认识
		SELECT @OnLineUserCount=SUM(UserCount) FROM @TableUserCount WHERE MarketID=@MarketID OR @MarketID=0
		SELECT @OnLineServerCount=SUM(UserCount) FROM @TableUserCount WHERE ServerKind<>0 AND (MarketID=@MarketID OR @MarketID=0)

		-- 分类统计
		SELECT @OnLinePlazaCount = SUM(UserCount) FROM @TableUserCount WHERE ServerKind=0 AND (MarketID=@MarketID OR @MarketID=0)
		SELECT @OnLineGoldServerCount = SUM(UserCount) FROM @TableUserCount WHERE ServerKind=0x01 AND (MarketID=@MarketID OR @MarketID=0)
		SELECT @OnLineMatchServerCount = SUM(UserCount) FROM @TableUserCount WHERE ServerKind=0x04 AND (MarketID=@MarketID OR @MarketID=0)
		SELECT @OnLineBattleServerCount = SUM(UserCount) FROM @TableUserCount WHERE ServerKind=0x08 AND (MarketID=@MarketID OR @MarketID=0)
		SELECT @OnLineRankingServerCount = SUM(UserCount) FROM @TableUserCount WHERE ServerKind=0x10 AND (MarketID=@MarketID OR @MarketID=0)

		-- 调整变量
		IF @OnLineUserCount IS NULL SET @OnLineUserCount=0
		IF @OnLinePlazaCount IS NULL SET @OnLinePlazaCount=0
		IF @OnLineServerCount IS NULL SET @OnLineServerCount=0	
		IF @OnLineGoldServerCount IS NULL SET @OnLineGoldServerCount=0
		IF @OnLineMatchServerCount IS NULL SET @OnLineMatchServerCount=0
		IF @OnLineBattleServerCount IS NULL SET @OnLineBattleServerCount=0
		IF @OnLineRankingServerCount IS NULL SET @OnLineRankingServerCount=0

		--插入信息
		INSERT OnLineStreamInfo (MarketID,OnLineUserCount,OnLinePlazaCount,OnLineServerCount,OnLineGoldServerCount,OnLineMatchServerCount,OnLineBattleServerCount,OnLineRankingServerCount)
		VALUES (@MarketID,@OnLineUserCount,@OnLinePlazaCount,@OnLineServerCount,@OnLineGoldServerCount,@OnLineMatchServerCount,@OnLineBattleServerCount,@OnLineRankingServerCount)	

		-- 更新索引
		SET @Index=@Index+1
	END

END
RETURN 0
GO

----------------------------------------------------------------------------------------------------
