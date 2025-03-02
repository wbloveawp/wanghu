USE LHGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadMatchRanking]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadMatchRanking]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 加载排行
CREATE PROC GSP_GS_LoadMatchRanking
	@dwMatchID INT,					-- 比赛标识		
	@lMatchNo BIGINT OUTPUT			-- 比赛场次
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询场次
	SELECT @lMatchNo=MAX([MatchNo]) FROM [dbo].[StreamMatchSession] WHERE MatchID=@dwMatchID

	-- 调整变量
	IF @lMatchNo IS NULL SET @lMatchNo=0
		
	-- 分数排序
	SELECT UserID,GameID,RankID,MatchScore FROM StreamMatchHistory(NOLOCK) WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo

END

RETURN 0
GO

----------------------------------------------------------------------------------------------------