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
-- �������а�
CREATE PROC GSP_GR_LoadRankingList
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

SET XACT_ABORT ON

-- ִ���߼�
BEGIN
	SELECT TOP 1000 RankID,UserID,Score FROM UserRankingInfo
END

RETURN 0
GO


----------------------------------------------------------------------------------------------------
-- ��ѯ�������
CREATE PROC GSP_GR_QueryUserRanking
	@dwUserID INT								-- �û� I D
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

SET XACT_ABORT ON

-- ִ���߼�
BEGIN
	DECLARE @RankID INT
	SELECT @RankID=RankID FROM UserRankingInfo WHERE UserID=@dwUserID
	IF @RankID IS NULL SET @RankID=10000
	
	SELECT @RankID AS RankID
END

RETURN 0
GO
----------------------------------------------------------------------------------------------------
-- ˢ�����а�
CREATE PROC GSP_GR_FlushRanking
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

SET XACT_ABORT ON

-- ִ���߼�
BEGIN

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN	
	
	-- ɾ������
	TRUNCATE TABLE UserRankingInfo
	
	-- ��������
	INSERT UserRankingInfo(RankID,UserID,Score)
	SELECT ROW_NUMBER() OVER (ORDER BY Score DESC) AS RankID,UserID,Score FROM GameScoreInfo 
	
	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
