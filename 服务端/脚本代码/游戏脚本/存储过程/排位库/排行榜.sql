USE LHGameRankIngDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadRankingList]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadRankingList]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryUserRanking]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryUserRanking]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_FlushRanking]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_FlushRanking]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- 加载排行榜
CREATE PROC GSP_GR_LoadRankingList
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

SET XACT_ABORT ON

-- 执行逻辑
BEGIN
	SELECT TOP 1000 RankID,UserID,Score FROM UserRankingInfo
END

RETURN 0
GO


----------------------------------------------------------------------------------------------------
-- 查询玩家排名
CREATE PROC GSP_GR_QueryUserRanking
	@dwUserID INT								-- 用户 I D
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

SET XACT_ABORT ON

-- 执行逻辑
BEGIN
	DECLARE @RankID INT
	SELECT @RankID=RankID FROM UserRankingInfo WHERE UserID=@dwUserID
	IF @RankID IS NULL SET @RankID=10000
	
	SELECT @RankID AS RankID
END

RETURN 0
GO
----------------------------------------------------------------------------------------------------
-- 刷新排行榜
CREATE PROC GSP_GR_FlushRanking
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

SET XACT_ABORT ON

-- 执行逻辑
BEGIN

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN	
	
	-- 删除数据
	TRUNCATE TABLE UserRankingInfo
	
	-- 插入排行
	INSERT UserRankingInfo(RankID,UserID,Score)
	SELECT ROW_NUMBER() OVER (ORDER BY Score DESC) AS RankID,UserID,Score FROM GameScoreInfo 
	
	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
