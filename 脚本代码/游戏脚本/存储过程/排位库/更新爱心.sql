USE LHGameRankingDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UpdateLoveHeart]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UpdateLoveHeart]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- ���°���
CREATE PROC GSP_GR_UpdateLoveHeart
	@strUserList NVARCHAR(512),					-- �û��б�	
	@wKindID INT,								-- ��Ϸ��ʶ
	@wServerID INT,								-- �����ʶ	
	@strErrorDescribe NVARCHAR(127)	OUTPUT		-- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

SET XACT_ABORT ON

-- ִ���߼�
BEGIN

	-- ���������
	DECLARE @UserList TABLE (ID INT identity(1,1),UserID INT, LoveHeart INT)	

	-- ��������
	IF ISJSON(@strUserList)>0
	BEGIN
		INSERT @UserList (UserID,LoveHeart)
		SELECT JSON_VALUE(VALUE,'$.UserID'), JSON_VALUE(VALUE,'$.LoveHeart') FROM OPENJSON(@strUserList)
	END

	-- ��������
	DECLARE @Index INT
	DECLARE @LoopCount INT 

	-- �û���Ϣ
	DECLARE @GameID INT
	DECLARE @UserID INT	
	DECLARE @LoveHeart INT
	
	-- ��ʼ��
	SELECT @Index=1,@LoopCount=COUNT(1) FROM @UserList		

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN
	
	-- ѭ������
	WHILE @Index<=@LoopCount
	BEGIN	
		-- ��ѯ��Ϣ
		SELECT @UserID=UserID,@LoveHeart=LoveHeart FROM @UserList WHERE ID=@Index		

		-- ��Ϸ��ʶ
		SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@UserID

		-- ��ѯ����
		DECLARE @BeforeLoveHeart INT
		SELECT @BeforeLoveHeart=LoveHeart FROM GameScoreInfo WHERE UserID=@UserID	
		
		-- ���ͱ�ʶ
		DECLARE @TypeID TINYINT = 6		
		IF @LoveHeart>0 SET @TypeID=4		

		-- �����¼
		INSERT INTO LHGameRecordDB.dbo.RecordChangeLoveHeart(UserID,GameID,TypeID,BeforeLoveHeart,ChangeLoveHeart,CollectClientIP)
		VALUES (@UserID,@GameID,@TypeID,@BeforeLoveHeart,@LoveHeart,N'---------------')

		-- ���°���
		UPDATE GameScoreInfo SET LoveHeart+=@LoveHeart WHERE UserID=@UserID

		SET @Index=@Index+1
	END	

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- �׳�������Ϣ
	SELECT UserID, LoveHeart FROM GameScoreInfo(NOLOCK) WHERE UserID IN (SELECT UserID FROM @UserList)
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
