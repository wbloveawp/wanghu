USE LHGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_QueryMatchRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_QueryMatchRecord]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_QueryMatchRecordBoard]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_QueryMatchRecordBoard]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��ѯ��¼
CREATE PROC GSP_GS_QueryMatchRecord
	@dwUserID INT					-- �û���ʶ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ��¼
	SELECT a.MatchID,a.MatchNo,a.UserID,a.RankID,a.MatchScore,a.RewardEntity,a.RewardGoods,a.MatchStartTime,a.MatchFinishTime,b.ShowType,ISNULL(b.MatchName,N'') AS MatchName FROM StreamMatchHistory(NOLOCK) a, MatchOption(NOLOCK) b 
	WHERE a.UserID=@dwUserID AND a.MatchID=b.MatchID AND a.MatchScore>0

END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- ��ѯ����
CREATE PROC GSP_GS_QueryMatchRecordBoard
	@dwMatchID INT,					-- ������ʶ
	@lMatchNo BIGINT				-- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��������
	SELECT UserID,RankID,MatchScore,RewardEntity,RewardGoods FROM StreamMatchHistory(NOLOCK) WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo AND RankID>0 ORDER BY RankID
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------