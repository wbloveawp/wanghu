USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadUserRankingInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadUserRankingInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 加载排位信息
CREATE PROC GSP_GP_LoadUserRankingInfo
	@dwUserID INT							-- 用户标识		
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 系统信息
	DECLARE @SeasonID		SMALLINT

	-- 段位信息
	DECLARE @RankID			INT
	DECLARE @LoveHeart		INT
	DECLARE @RankScore		INT
	
	-- 对局信息
	DECLARE @WinCount		INT
	DECLARE @LostCount		INT
	DECLARE @WinStreakCount	INT

	-- 查询信息
	SELECT @RankScore=Score, @LoveHeart=LoveHeart, @WinCount=WinCount, @LostCount=LostCount, @WinStreakCount=MaxWinStreak
	FROM LHGameRankingDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 查询排名
	SELECT @RankID=RankID FROM LHGameRankingDB.dbo.UserRankingInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 调整变量
	IF @RankID IS NULL SET @RankID=0
	IF @LoveHeart IS NULL SET @LoveHeart=3
	IF @RankScore IS NULL SET @RankScore=900
	IF @WinCount IS NULL SET @WinCount=0
	IF @LostCount IS NULL SET @LostCount=0
	IF @WinStreakCount IS NULL SET @WinStreakCount=0

	-- 系统信息
	SET @SeasonID=LHPlatformDB.dbo.PFF_GetSeasonID()
	   
	 -- 输出变量
	SELECT @SeasonID AS SeasonID, @RankID AS RankID, @RankScore AS RankScore, @LoveHeart AS LoveHeart, @WinCount AS WinCount, @LostCount AS LostCount, @WinStreakCount AS WinStreakCount

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
