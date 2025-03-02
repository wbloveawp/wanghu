----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_CleanGameScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_CleanGameScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_DeductGameScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_DeductGameScore]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �������
CREATE PROC GSP_GP_CleanGameScore	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �Զ��ع�
SET XACT_ABORT ON

-- ִ���߼�
BEGIN

	
	-- �û��б�
	DECLARE @TableUserList TABLE
	(
		ID INT identity(1,1),
		UserID INT NOT NULL 
	)

	-- ��������
	INSERT INTO @TableUserList SELECT a.UserID FROM RecordNeedClearGold a LEFT JOIN AccountsOnlineInfo b ON a.UserID=b.UserID WHERE DATEDIFF(ss,a.WithdrawDateTime,GETDATE())>=30*60 AND ISNULL(b.ServerID,0)=0

	-- ��������
	DECLARE @Index INT	
	DECLARE @LoopCount INT
	
	-- ͳ������
	SELECT @Index=1,@LoopCount=COUNT(1) FROM @TableUserList

	-- �˳��ж�
	IF @LoopCount=0	RETURN 1

	-- ��������
	IF @Index IS NULL SET @Index=1
	IF @LoopCount IS NULL SET @LoopCount=0

	-- �û���Ϣ
	DECLARE @UserID INT
	DECLARE @UserScore BIGINT

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ѭ������
	WHILE @Index<=@LoopCount
	BEGIN	
		-- ��ѯ��Ϣ
		SELECT @UserID=UserID FROM @TableUserList WHERE ID=@Index

		-- ��ѯ����
		SELECT @UserScore=-Score FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@UserID

		-- ��������
		IF @UserScore IS NULl SET @UserScore=0

		-- �۳�����
		IF @UserScore<>0
		BEGIN
			EXEC GSP_GP_WriteGameWealth @UserID,2,79,@UserScore
		END

		-- ��������
		SET @Index=@Index+1
	END

	-- ɾ������
	DELETE RecordNeedClearGold WHERE UserID IN (SELECT UserID FROM @TableUserList)

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	
	
	-- �׳�����
	SELECT UserID FROM @TableUserList

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �۳�����
CREATE PROC GSP_GP_DeductGameScore	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �Զ��ع�
SET XACT_ABORT ON

-- ִ���߼�
BEGIN

	
	-- �û��б�
	DECLARE @TableUserList TABLE
	(
		ID INT identity(1,1),
		UserID INT NOT NULL,
		DeductScore BIGINT	
	)

	-- ��������
	INSERT INTO @TableUserList SELECT a.UserID,a.DeductScore FROM RecordNeedDeductScore a LEFT JOIN AccountsOnlineInfo b ON a.UserID=b.UserID WHERE DATEDIFF(ss,a.CollectDateTime,GETDATE())>=5*60 AND ISNULL(b.ServerID,0)=0

	-- ��������
	DECLARE @Index INT	
	DECLARE @LoopCount INT
	
	-- ͳ������
	SELECT @Index=1,@LoopCount=COUNT(1) FROM @TableUserList

	-- �˳��ж�
	IF @LoopCount=0	RETURN 1

	-- ��������
	IF @Index IS NULL SET @Index=1
	IF @LoopCount IS NULL SET @LoopCount=0

	-- �û���Ϣ
	DECLARE @UserID INT
	DECLARE @UserScore BIGINT
	DECLARE @DeductScore BIGINT
	DECLARE @ChangeScore BIGINT

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ѭ������
	WHILE @Index<=@LoopCount
	BEGIN	
		-- ��ѯ��Ϣ
		SELECT @UserID=UserID,@DeductScore=DeductScore FROM @TableUserList WHERE ID=@Index

		-- ��ѯ����
		SELECT @UserScore=Score FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@UserID

		-- ��������
		IF @UserScore IS NULl SET @UserScore=0

		-- ����������
		IF @UserScore>@DeductScore 
		BEGIN
			SET @ChangeScore=-@DeductScore
		END ELSE
		BEGIN
			SET @ChangeScore=-@UserScore
		END

		-- �۳�����
		IF @UserScore<>0
		BEGIN
			EXEC GSP_GP_WriteGameWealth @UserID,2,78,@ChangeScore
		END

		-- �û����
		UPDATE AccountsInfo SET Nullity=0 WHERE UserID=@UserID

		-- ��������
		SET @Index=@Index+1
	END

	-- ɾ������
	DELETE RecordNeedDeductScore WHERE UserID IN (SELECT UserID FROM @TableUserList)

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
