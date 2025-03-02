
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryGameWealth]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryGameWealth]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_WriteGameWealth]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_WriteGameWealth]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_WriteGameWealth]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_WriteGameWealth]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��ѯ�Ƹ�
CREATE PROC GSP_GP_QueryGameWealth

	-- �û���Ϣ
	@dwUserID INT								-- �û���ʶ	

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	DECLARE @UserCard BIGINT
	SELECT @UserCard=Score FROM LHGameBattleDB.dbo.GameCardInfo(NOLOCK) WHERE UserID=@dwUserID
	
	-- ��ѯ���
	DECLARE @UserGold BIGINT		
	SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

	-- ��������
	IF @UserGold IS NULL SET @UserGold=0
	IF @UserCard IS NULL SET @UserCard=0	
	
	-- �׳���¼
	SELECT @UserCard AS UserCard, @UserGold AS UserGold

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��Ϸ�Ƹ�
CREATE PROC GSP_GP_WriteGameWealth

	-- �û���Ϣ
	@dwUserID INT,								-- �û���ʶ

	-- �Ƹ���Ϣ
	@wMoneyID SMALLINT,							-- �Ƹ���ʶ
	@wReason SMALLINT,							-- ���ԭ��	
	@lVariationScore BIGINT						-- �������

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ���ڱ�ʶ
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��ѯ�û�
	DECLARE @GameID INT
	SELECT @GameID=GameID FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- У���û�
	IF @GameID IS NULL
	BEGIN
		RETURN 1
	END

	-- ��Ҹ���
	IF @wMoneyID=1
	BEGIN
		-- ��ѯ���
		DECLARE @BeforeGold BIGINT		
		SELECT @BeforeGold=Score FROM  LHGameGoldDB.dbo.GameScoreInfo WITH(UPDLOCK) WHERE UserID=@dwUserID
		
		-- ��������
		IF @BeforeGold IS NULL
		BEGIN
			-- ���ñ���
			SET @BeforeGold=0			

			-- ��������
			INSERT INTO LHGameGoldDB.dbo.GameScoreInfo (UserID,Score) VALUES (@dwUserID,0)
		END

		-- ����У��
		IF @BeforeGold+@lVariationScore<0
		BEGIN
			RETURN 1
		END
		
		-- ���½��
		UPDATE LHGameGoldDB.dbo.GameScoreInfo SET Score=Score+@lVariationScore WHERE UserID=@dwUserID

		-- �����¼
		INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID, GameID, TypeID, BeforeGold, ChangeGold, CollectClientIP, CollectDateTime)
		VALUES (@dwUserID, @GameID, @wReason, @BeforeGold, @lVariationScore, N'------------', GetDate())
	END	

	-- ��������
	IF @wMoneyID=2
	BEGIN
		-- ��ѯ����
		DECLARE @BeforeCard BIGINT
		SELECT @BeforeCard=Score FROM LHGameBattleDB.dbo.GameCardInfo WITH(UPDLOCK)  WHERE UserID=@dwUserID

		-- ��������
		IF @BeforeCard IS NULL 
		BEGIN
			SET @BeforeCard=0
			INSERT INTO LHGameBattleDB.dbo.GameCardInfo (UserID,Score) VALUES (@dwUserID,0)
		END	

		-- ����У��
		IF @BeforeCard+@lVariationScore<0
		BEGIN
			RETURN 1
		END

		-- ���·���
		UPDATE LHGameBattleDB.dbo.GameCardInfo SET Score+=@lVariationScore WHERE UserID=@dwUserID

		-- �����¼
		INSERT INTO LHGameRecordDB.dbo.RecordChangeCard(UserID, GameID, TypeID, BeforeCard, ChangeCard,CollectClientIP, CollectDateTime)
		VALUES (@dwUserID, @GameID, @wReason, @BeforeCard, @lVariationScore,  N'-----------', GetDate())
	END	
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��Ϸ�Ƹ�
CREATE PROC GSP_GR_WriteGameWealth

	-- �û���Ϣ
	@dwUserID INT,								-- �û���ʶ

	-- �Ƹ���Ϣ
	@wMoneyID SMALLINT,							-- �Ƹ���ʶ
	@wReason SMALLINT,							-- ���ԭ��	
	@lVariationScore BIGINT						-- �������

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������
DECLARE @BalanceAmount BIGINT

-- ִ���߼�
BEGIN

	-- ���ڱ�ʶ
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��ѯ�û�
	DECLARE @GameID INT
	SELECT @GameID=GameID FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- У���û�
	IF @GameID IS NULL
	BEGIN
		RETURN 1
	END

	-- ��Ҹ���
	IF @wMoneyID=1
	BEGIN
		-- ��ѯ���
		DECLARE @BeforeGold BIGINT		
		SELECT @BeforeGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo WITH(UPDLOCK) WHERE UserID=@dwUserID
		
		-- ��������
		IF @BeforeGold IS NULL
		BEGIN
			-- ���ñ���
			SET @BeforeGold=0			

			-- ��������
			INSERT INTO LHGameGoldDB.dbo.GameScoreInfo (UserID,Score) VALUES (@dwUserID,0)
		END

		-- ����У��
		IF @BeforeGold+@lVariationScore<0
		BEGIN
			RETURN 1
		END
		
		-- ���½��
		UPDATE LHGameGoldDB.dbo.GameScoreInfo SET Score=Score+@lVariationScore WHERE UserID=@dwUserID

		-- �����¼
		INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID, GameID, TypeID, BeforeGold, ChangeGold, CollectClientIP, CollectDateTime)
		VALUES (@dwUserID, @GameID, @wReason, @BeforeGold, @lVariationScore, N'------------', GetDate())

		SET @BalanceAmount=@BeforeGold+@lVariationScore
	END	

	-- ��������
	IF @wMoneyID=2
	BEGIN
		-- ��ѯ����
		DECLARE @BeforeCard BIGINT
		SELECT @BeforeCard=Score FROM LHGameBattleDB.dbo.GameCardInfo WITH(UPDLOCK)  WHERE UserID=@dwUserID

		-- ��������
		IF @BeforeCard IS NULL 
		BEGIN
			SET @BeforeCard=0
			INSERT INTO LHGameBattleDB.dbo.GameCardInfo (UserID,Score) VALUES (@dwUserID,0)
		END	

		-- ����У��
		IF @BeforeCard+@lVariationScore<0
		BEGIN
			RETURN 1
		END

		-- ���·���
		UPDATE LHGameBattleDB.dbo.GameCardInfo SET Score+=@lVariationScore WHERE UserID=@dwUserID

		-- �����¼
		INSERT INTO LHGameRecordDB.dbo.RecordChangeCard (UserID,GameID,TypeID,KindID,ClubID,BeforeCard,ChangeCard,CollectClientIP,CollectDateTime)
		VALUES (@dwUserID,@GameID,@wReason,0,0,@BeforeCard,@lVariationScore,N'--------------',GETDATE())

		SET @BalanceAmount=@BeforeCard+@lVariationScore
	END		

	-- Ĭ�ϴ���
	IF @BalanceAmount IS NULL SET @BalanceAmount=0

	-- �׳���¼
	SELECT @BalanceAmount AS BalanceAmount

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------