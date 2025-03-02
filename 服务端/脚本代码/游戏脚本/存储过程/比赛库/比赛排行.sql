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

-- ��������
CREATE PROC GSP_GS_LoadMatchRanking
	@dwMatchID INT,					-- ������ʶ		
	@lMatchNo BIGINT OUTPUT			-- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	SELECT @lMatchNo=MAX([MatchNo]) FROM [dbo].[StreamMatchSession] WHERE MatchID=@dwMatchID

	-- ��������
	IF @lMatchNo IS NULL SET @lMatchNo=0
		
	-- ��������
	SELECT UserID,GameID,RankID,MatchScore FROM StreamMatchHistory(NOLOCK) WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo

END

RETURN 0
GO

----------------------------------------------------------------------------------------------------