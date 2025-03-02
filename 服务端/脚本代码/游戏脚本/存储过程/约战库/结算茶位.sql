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

-- �����λ
CREATE PROC GSP_GR_BalanceClubRevenue	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �Զ��ع�
SET XACT_ABORT ON

-- ִ���߼�
BEGIN

	-- ���ڱ�ʶ
	DECLARE @DateID INT
	SET @DateID = CAST(CAST(DATEADD(dd, -1, GETDATE()) AS FLOAT) AS INT)

	-- �ϰ�˰��
	DECLARE @TableCreaterRevenue TABLE
	(
		UserID INT NOT NULL,
		TotalRevenue BIGINT
	)

	-- �û�˰��
	DECLARE @TableUserRevenue TABLE
	(
		UserID INT NOT NULL PRIMARY KEY,
		GameID INT,
		UserScore BIGINT,
		TotalRevenue BIGINT
	)

	-- ��������
	INSERT INTO @TableCreaterRevenue(UserID,TotalRevenue)
	SELECT b.CreaterID,SUM(a.Revenue) FROM  StreamClubUser(NOLOCK) a LEFT JOIN ClubProperty(NOLOCK) b ON a.DateID=@DateID AND a.ClubID=b.ClubID WHERE b.ClubKind=0 
	GROUP BY a.ClubID,b.CreaterID
	
	-- �˳��ж�
	IF NOT EXISTS(SELECT 1 FROM @TableCreaterRevenue)
	BEGIN
		RETURN 1
	END
	
	-- �û�˰��
	INSERT INTO @TableUserRevenue(UserID,UserScore,TotalRevenue) SELECT UserID,0,SUM(TotalRevenue) FROM @TableCreaterRevenue GROUP BY UserID HAVING SUM(TotalRevenue)>0

	-- ��Ϸ��ʶ
	UPDATE @TableUserRevenue SET GameID=a.GameID FROM LHAccountsDB.dbo.AccountsInfo a, @TableUserRevenue b WHERE b.UserID=a.UserID

	-- ��ǰ���
	UPDATE @TableUserRevenue SET UserScore=a.Score FROM LHGameGoldDB.dbo.GameScoreInfo a, @TableUserRevenue b WHERE b.UserID=a.UserID	

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ���½��
	UPDATE LHGameGoldDB.dbo.GameScoreInfo SET Score+=b.TotalRevenue FROM LHGameGoldDB.dbo.GameScoreInfo a, @TableUserRevenue b WHERE a.UserID=b.UserID

	-- �����¼
	INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID, GameID, TypeID, BeforeGold, ChangeGold, CollectClientIP, CollectDateTime)
	SELECT UserID, GameID, 77, UserScore, TotalRevenue, N'------------', GetDate() FROM @TableUserRevenue

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
