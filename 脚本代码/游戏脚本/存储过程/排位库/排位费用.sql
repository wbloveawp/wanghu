USE LHGameRankIngDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DeductRankingCost]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DeductRankingCost]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- �۳�����
CREATE PROC GSP_GR_DeductRankingCost
	@strUserList NVARCHAR(512),					-- �û��б�	
	@wKindID INT,								-- ��Ϸ��ʶ
	@wServerID INT,								-- �����ʶ	
	@lConsumeCount INT,							-- ��������	
	@strErrorDescribe NVARCHAR(127)	OUTPUT		-- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

SET XACT_ABORT ON

-- ִ���߼�
BEGIN

	-- ���������
	DECLARE @UserIDList TABLE (ID INT identity(1,1),UserID INT, ClientAddr NVARCHAR(16))	

	-- ��������
	IF ISJSON(@strUserList)>0
	BEGIN
		INSERT @UserIDList (UserID,ClientAddr)
		SELECT JSON_VALUE(VALUE,'$.UserID'), JSON_VALUE(VALUE,'$.ClientAddr') FROM OPENJSON(@strUserList)
	END

	-- ��������
	DECLARE @Index INT
	DECLARE @LoopCount INT 

	-- �û���Ϣ
	DECLARE @GameID INT
	DECLARE @UserID INT	
	DECLARE @ClientAddr NVARCHAR(16)	

	-- д����ͳ��
	DECLARE @DateID INT
	SET @DateID = CAST(CAST(GETDATE() AS FLOAT) AS INT)
	
	-- ��ʼ��
	SELECT @Index=1,@LoopCount=COUNT(1) FROM @UserIDList		

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN
	
	-- ѭ������
	WHILE @Index<=@LoopCount
	BEGIN
	
		-- ��ѯ��Ϣ
		SELECT @UserID=UserID,@ClientAddr=ClientAddr FROM @UserIDList WHERE ID=@Index		

		-- ��ѯ�ƽ�
		DECLARE @UserGold BIGINT
		SELECT @UserGold=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@UserID

		-- ��������
		IF @UserGold IS NULL OR @UserGold < @lConsumeCount
		BEGIN
			SET @strErrorDescribe=N'��������һƽ����㣬��Ϸ�޷���ʼ��'

			-- ����ع�
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED

			-- �׳���¼
			SELECT @UserID AS FailUserID
			
			RETURN 6
		END

		-- ��Ϸ��ʶ
		SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@UserID

		-- �����¼
		INSERT INTO LHGameRecordDB.dbo.RecordChangeGold (UserID,GameID,TypeID,BeforeGold,ChangeGold,CollectClientIP)
		VALUES (@UserID,@GameID,5,@UserGold,-@lConsumeCount,@ClientAddr)

		-- ���»ƽ�
		UPDATE LHAccountsDB.dbo.AccountsGoldInfo SET Score-=@lConsumeCount WHERE UserID=@UserID

		-- �����ͳ��
		UPDATE ReportUserScore SET UserService+=@lConsumeCount WHERE DateID=@DateID AND UserID=@UserID
		IF @@ROWCOUNT=0
		BEGIN
			INSERT INTO ReportUserScore(DateID,UserID,UserService) VALUES(@DateID,@UserID,@lConsumeCount)
		END

		SET @Index=@Index+1
	END	

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- �׳������Ϣ
	SELECT UserID, Score AS UserGold FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID IN (SELECT UserID FROM @UserIDList)
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
