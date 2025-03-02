----------------------------------------------------------------------------------------------------
--
-- �޶����ڣ�2010-3-4 11:21:17
-- ��    �ݣ�ȷ���洢����ʼ��ִ��һ��
--
----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------

USE LHGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_WriteGameScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_WriteGameScore]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��Ϸд��
CREATE PROC GSP_GR_WriteGameScore
	@dwUserID INT,								-- �û� I D
	@lScore BIGINT,								-- �û�����
	@lRevenue BIGINT,							-- ��Ϸ˰��
	@lWinCount INT,								-- ʤ������
	@lLostCount INT,							-- ʧ������
	@lDrawCount INT,							-- �;�����
	@lFleeCount INT,							-- ������Ŀ
	@lWinStreak INT,							-- ��ʤ����
	@lMaxWinStreak INT,							-- ��ʤ����
	@lExperience INT,							-- �û�����
	@lUnLockScore BIGINT,						-- �ͷŻ���
	@dwPlayTimeCount INT,						-- ��Ϸʱ��
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D	
	
	-- ������Ϣ
	@dwMatchID INT,								-- ������ʶ
	@lMatchNo BIGINT							-- ��������
		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �Զ��ύ
SET XACT_ABORT ON

-- �û�����
DECLARE @WinScore BIGINT
DECLARE @LostScore BIGINT

-- ִ���߼�
BEGIN	

	-- ��������
	DECLARE @DateID INT
	SET @DateID = CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN	

	-- ��������
	UPDATE MatchScoreInfo SET Score+=@lScore, WinCount+=@lWinCount, LostCount+=@lLostCount, DrawCount+=@lDrawCount, FleeCount+=@lFleeCount, WinStreak=@lWinStreak, MaxWinStreak=@lMaxWinStreak, PlayTimeCount+=@dwPlayTimeCount
	WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo 	

	-- �û�����
	UPDATE LHAccountsDB.dbo.AccountsInfo SET Experience+=@lExperience,PlayTimeCount+=@dwPlayTimeCount WHERE UserID=@dwUserID

	-- ��ʼ��
	SELECT @LostScore=0, @WinScore=0

	-- ���÷���
	IF @lScore>=0  
		SET @WinScore=@lScore		
	ELSE 
		SET @LostScore=@lScore	

	-- ����ͳ��
	UPDATE ReportUserScore SET GainScore+=@WinScore,LoseScore+=@LostScore,WinCount+=@lWinCount,LostCount+=@lLostCount,DrawCount+=@lDrawCount,FleeCount+=@lFleeCount,UserRevenue+=@lRevenue,
		PlayTimeCount+=@dwPlayTimeCount,ModifyDateTime=GETDATE() 
	WHERE DateID=@DateID AND UserID=@dwUserID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO ReportUserScore(DateID,UserID,GainScore,LoseScore,WinCount,LostCount,DrawCount,FleeCount,UserRevenue,PlayTimeCount,InsertDateTime,ModifyDateTime)
		VALUES (@DateID,@dwUserID,@WinScore,@LostScore,@lWinCount,@lLostCount,@lDrawCount,@lFleeCount,@lRevenue,@dwPlayTimeCount,GETDATE(),GETDATE())
	END

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
