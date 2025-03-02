USE LHGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadMatchUser]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadMatchUser]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 加载比赛用户
CREATE PROC GSP_GS_LoadMatchUser
	@dwMatchID INT,								-- 比赛标识
	@lMatchNo  BIGINT							-- 比赛场次		
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
		
	-- 统计数量
	DECLARE @UserCount INT
	SELECT a.UserID,a.CollectTime AS SignupTime,ISNULL(b.WinCount,0) AS WinCount, ISNULL(b.LostCount,0) AS LostCount, ISNULL(b.SignupCount,0) AS SignupCount, ISNULL(b.Score,0) AS MatchScore,
		   ISNULL(b.MaxWinStreak,0) AS MaxWinStreak,ISNULL(b.ServerID,0) AS ServerID 
	FROM StreamMatchTicket(NOLOCK) a LEFT JOIN MatchScoreInfo(NOLOCK) b ON a.MatchID=b.MatchID AND a.MatchNo=b.MatchNo 
	WHERE a.MatchID=@dwMatchID AND a.MatchNo=@lMatchNo
	
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------