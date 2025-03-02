USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ReturnBattleCost]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ReturnBattleCost]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DeductBattleCost]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DeductBattleCost]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DeductBattleCostAA]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DeductBattleCostAA]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �˻�����
CREATE PROC GSP_GR_ReturnBattleCost
	@wKindID INT,								-- ��Ϸ��ʶ
	@wServerID INT,								-- �����ʶ
	@dwClubID INT,								-- ���ֲ���ʶ
	@dwPayUserID INT,							-- �������	
	@lReturnCount BIGINT,						-- �˻�����
	@cbCurrencyKind	TINYINT,					-- ��������
	@strErrorDescribe NVARCHAR(127)	OUTPUT		-- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

DECLARE @UserGold BIGINT
DECLARE @UserCard BIGINT

-- ִ���߼�
BEGIN
	
	-- �û���ѯ
	DECLARE @GameID INT                                                                                                                                                                                                         
	SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwPayUserID
	
	-- �������
	IF @cbCurrencyKind=1
	BEGIN
		-- ��ѯ���	
		SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwPayUserID

		-- ���У��
		IF @UserGold IS NULL 
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1240,"Msg":"��Ǹ��ϵͳδ��ѯ�����Ľ����Ϣ����ҷ���ʧ�ܣ�"}'
			RETURN 1	
		END

		-- �����¼
		INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID,GameID,TypeID,BeforeGold,ChangeGold,CollectClientIP,CollectDateTime)
		VALUES (@dwPayUserID,@GameID,15,@UserGold,@lReturnCount,N'0.0.0.0',GETDATE())

		-- ���½��
		UPDATE LHGameGoldDB.dbo.GameScoreInfo SET Score+=@lReturnCount WHERE UserID=@dwPayUserID
	END

	-- ��������
	IF @cbCurrencyKind=2
	BEGIN
		-- ��ѯ����		
		SELECT @UserCard=Score FROM GameCardInfo(NOLOCK) WHERE UserID=@dwPayUserID

		-- ��ʯ����
		IF @UserCard IS NULL 
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1240,"Msg":"��Ǹ��ϵͳδ��ѯ�����ķ�����Ϣ����������ʧ�ܣ�"}'
			RETURN 1	
		END

		-- �����¼
		INSERT INTO LHGameRecordDB.dbo.RecordChangeCard (UserID,GameID,TypeID,KindID,ClubID,BeforeCard,ChangeCard,CollectClientIP,CollectDateTime)
		VALUES (@dwPayUserID,@GameID,4,@wKindID,@dwClubID,@UserCard,@lReturnCount,N'--------------',GETDATE())

		-- ���·���
		UPDATE GameCardInfo SET Score+=@lReturnCount WHERE UserID=@dwPayUserID
	END			

	-- ��ѯ�Ƹ�
	SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwPayUserID
	SELECT @UserCard=Score FROM GameCardInfo(NOLOCK) WHERE UserID=@dwPayUserID

	-- ��������
	IF @UserGold IS NULL SET @UserGold=0
	IF @UserCard IS NULL SET @UserCard=0		

	-- �׳�����
	SELECT @dwPayUserID AS UserID, @UserGold AS UserGold, @UserCard AS UserCard	
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- �۳�����
CREATE PROC GSP_GR_DeductBattleCost
	@dwUserID INT,								-- �û� I D	
	@wKindID INT,								-- ��Ϸ��ʶ
	@wServerID INT,								-- �����ʶ
	@dwClubID INT,								-- Ⱥ���ʶ
	@wPayType SMALLINT,							-- ֧����ʽ
	@lConsumeCount BIGINT,						-- ��������	
	@cbCurrencyKind	TINYINT,					-- ��������	
	@strErrorDescribe NVARCHAR(127)	OUTPUT		-- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

DECLARE @UserGold BIGINT
DECLARE @UserCard BIGINT

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @UserID INT
	DECLARE @GameID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- ��ѯ�û�
	SELECT @UserID=a.UserID,@GameID=a.GameID,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- �û�������
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����"}'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�"}'
		RETURN 3
	END	

	-- ���ֲ�����
	IF @dwClubID>0
	BEGIN	
		-- ��������
		DECLARE @PayUserID INT
		DECLARE @PayGameID INT
		DECLARE @ClubCreaterID INT
		DECLARE @ClubPayrollerID INT
		
		--��ѯ���ֲ�
		SELECT @PayUserID=CreaterID,@ClubCreaterID=CreaterID,@ClubPayrollerID=PayrollerID FROM ClubProperty WHERE ClubID=@dwClubID AND ClubStatus=1

		--���ֲ�������
		IF @ClubCreaterID IS NULL
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1241,"Msg":"��Ǹ��ϵͳδ��ѯ����صĲ�����Ϣ�� ���䴴��ʧ�ܣ�"}'
			RETURN 1				
		END

		-- ��ʼ����
		SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
		BEGIN TRAN			
			
		--��������
		IF @cbCurrencyKind=2
		BEGIN
			-- ���Դ���
			IF @ClubPayrollerID>0
			BEGIN
				-- ��ѯ����
				SELECT @UserCard=Score FROM GameCardInfo WITH (UPDLOCK) WHERE UserID=@ClubPayrollerID

				-- ��������
				IF @UserCard IS NULL SET @UserCard=0

				-- �������
				SET @PayUserID=@ClubPayrollerID
			END

			-- ����У��
			IF @UserCard IS NULL OR @lConsumeCount>@UserCard 
			BEGIN
				-- ��ѯ����		
				SELECT @UserCard=Score FROM GameCardInfo WITH (UPDLOCK) WHERE UserID=@ClubCreaterID

				-- ��������
				IF @UserCard IS NULL 
				BEGIN
					-- ����ع�
					ROLLBACK TRAN
					SET TRANSACTION ISOLATION LEVEL READ COMMITTED

					SET @strErrorDescribe=N'{"Idx":1243,"Msg":"��Ǹ����ǰ���ֲ��ϰ����ϵķ������㡣���䴴��ʧ�ܣ�����ϵ�ϰ塣"}'
					RETURN 20
				END

				-- ���Ƚ�
				IF @lConsumeCount>@UserCard
				BEGIN
					-- ����ع�
					ROLLBACK TRAN
					SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
					-- ������Ϣ
					SET @strErrorDescribe=N'{"Idx":1243,"Msg":"��Ǹ����ǰ���ֲ��ϰ����ϵķ������㡣���䴴��ʧ�ܣ�����ϵ�ϰ塣"}'
					RETURN 20	
				END

				-- �������
				IF @ClubPayrollerID=0 SET @PayUserID=@ClubCreaterID
			END

			-- �����¼
			INSERT INTO LHGameRecordDB.dbo.RecordChangeCard (UserID,GameID,TypeID,KindID,ClubID,BeforeCard,ChangeCard,CollectClientIP,CollectDateTime)
			VALUES (@PayUserID,@GameID,3,@wKindID,@dwClubID,@UserCard,-@lConsumeCount,N'--------------',GETDATE())

			-- ���·���
			UPDATE GameCardInfo SET Score-=@lConsumeCount WHERE UserID=@PayUserID
		END			

		-- ��������
		COMMIT TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ��ѯ�Ƹ�		
		SELECT @UserCard=Score FROM GameCardInfo(NOLOCK) WHERE UserID=@PayUserID
		SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@PayUserID

		-- ��������
		IF @UserGold IS NULL SET @UserGold=0
		IF @UserCard IS NULL SET @UserCard=0

		-- �׳�����
		SELECT @PayUserID AS PayUserID, @UserGold AS UserGold, @UserCard AS UserCard

	END ELSE
	BEGIN
		
		-- ����֧��
		IF @wPayType=0x01
		BEGIN
			-- �������
			DECLARE @OnlineStatus TINYINT		
			SELECT @OnlineStatus=OnlineStatus FROM BattleUserLocker(NOLOCK) WHERE UserID=@dwUserID

			-- ����״̬
			IF @OnlineStatus IS NULL SET @OnlineStatus=0

			IF @OnlineStatus=1
			BEGIN
				SET @strErrorDescribe=N'{"Idx":1244,"Msg":"��Ǹ������ʧ�ܣ����������Լս���������˳��ٽ��в�����"}'
				RETURN 15
			END
		END

		-- �û���ѯ
		SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

		-- �������
		IF @cbCurrencyKind=1
		BEGIN
			-- ��ѯ���	
			SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

			-- �������
			IF @UserGold IS NULL SET @UserGold=0
			
			-- ���Ƚ�
			IF @lConsumeCount>@UserGold
			BEGIN
				SET @strErrorDescribe=N'{"Idx":1245,"Msg":"���Ľ��Я�������㣬���ѿ۳�ʧ�ܣ�"}'
				RETURN 20	
			END

			-- �����¼
			INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID,GameID,TypeID,BeforeGold,ChangeGold,CollectClientIP,CollectDateTime)
			VALUES (@dwUserID,@GameID,14,@UserGold,-@lConsumeCount,N'0.0.0.0',GETDATE())

			-- ���½��
			UPDATE LHGameGoldDB.dbo.GameScoreInfo SET Score-=@lConsumeCount WHERE UserID=@dwUserID
		END

		-- ��������
		IF @cbCurrencyKind=2
		BEGIN
			-- ��ѯ����
			SELECT @UserCard=Score FROM GameCardInfo WHERE UserID=@dwUserID

			-- ��������
			IF @UserCard IS NULL SET @UserCard=0

			-- ���Ƚ�
			IF @lConsumeCount>@UserCard
			BEGIN
				SET @strErrorDescribe=N'{"Idx":1246,"Msg":"���ķ����������㣬���ȡ���෿�������ԣ�"}'
				RETURN 20	
			END

			-- �����¼
			INSERT INTO LHGameRecordDB.dbo.RecordChangeCard (UserID,GameID,TypeID,KindID,ClubID,BeforeCard,ChangeCard,CollectClientIP,CollectDateTime)
			VALUES (@dwUserID,@GameID,3,@wKindID,@dwClubID,@UserCard,-@lConsumeCount,N'--------------',GETDATE())

			-- ���·���
			UPDATE GameCardInfo SET Score-=@lConsumeCount WHERE UserID=@dwUserID
		END		

		-- ��ѯ�Ƹ�
		SELECT @UserCard=Score FROM GameCardInfo(NOLOCK) WHERE UserID=@dwUserID
		SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@PayUserID

		-- ��������
		IF @UserGold IS NULL SET @UserGold=0
		IF @UserCard IS NULL SET @UserCard=0	

		-- �׳�����
		SELECT @dwUserID AS PayUserID, @UserGold AS UserGold, @UserCard AS UserCard
	END			
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
-- �۳�����
CREATE PROC GSP_GR_DeductBattleCostAA
	@strUserList NVARCHAR(512),					-- �û��б�	
	@wKindID INT,								-- ��Ϸ��ʶ
	@wServerID INT,								-- �����ʶ	
	@dwClubID INT,								-- Ⱥ���ʶ
	@lConsumeCount INT,							-- ��������	
	@cbCurrencyKind	TINYINT,					-- ��������	
	@strErrorDescribe NVARCHAR(127)	OUTPUT		-- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON
SET XACT_ABORT ON

DECLARE @UserID INT
DECLARE @GameID INT
DECLARE @ClientAddr NVARCHAR(15)

-- ִ���߼�
BEGIN

	-- ����
	DECLARE @TableUserList TABLE (ID INT PRIMARY KEY IDENTITY(1,1),UserID INT)

	-- ��������
	IF ISJSON(@strUserList)>0
	BEGIN
		INSERT INTO @TableUserList(UserID) SELECT VALUE FROM OPENJSON(@strUserList)
	END

	-- ��������
	DECLARE @Index INT
	DECLARE @UserCount INT
	
	-- ��ѯ����
	SELECT @Index=1,@UserCount=COUNT(1) FROM @TableUserList

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��������
	WHILE @Index<=@UserCount
	BEGIN
		-- ��ѯ����
		SELECT @UserID=UserID FROM @TableUserList WHERE ID=@Index

		-- �������
		IF @cbCurrencyKind=1
		BEGIN
			-- ��ѯ���	
			DECLARE @UserGold BIGINT
			SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo WITH (UPDLOCK) WHERE UserID=@UserID

			-- �������
			IF @UserGold IS NULL SET @UserGold=0
			
			-- ���Ƚ�
			IF @lConsumeCount>@UserGold
			BEGIN
				-- ����ع�
				ROLLBACK TRAN
				SET TRANSACTION ISOLATION LEVEL READ COMMITTED

				-- �׳���ʶ
				SELECT @UserID AS FailUserID

				RETURN 10
			END

			-- ��ѯ��ϷID
			SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@UserID

			-- �����¼
			INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID,GameID,TypeID,BeforeGold,ChangeGold,CollectClientIP,CollectDateTime)
			VALUES (@UserID,@GameID,14,@UserGold,-@lConsumeCount,N'0.0.0.0',GETDATE())

			-- ���½��
			UPDATE LHGameGoldDB.dbo.GameScoreInfo SET Score-=@lConsumeCount WHERE UserID=@UserID
		END

		-- ��������
		IF @cbCurrencyKind=2
		BEGIN
			-- ��ѯ����
			DECLARE @UserCard BIGINT
			SELECT @UserCard=Score FROM GameCardInfo WITH (UPDLOCK) WHERE UserID=@UserID

			-- ��������
			IF @UserCard IS NULL SET @UserCard=0

			-- ���Ƚ�
			IF @lConsumeCount>@UserCard
			BEGIN
				-- ����ع�
				ROLLBACK TRAN
				SET TRANSACTION ISOLATION LEVEL READ COMMITTED

				-- �׳���ʶ
				SELECT @UserID AS FailUserID

				RETURN 10	
			END

			-- ��ѯ��ϷID
			SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@UserID

			-- �����¼
			INSERT INTO LHGameRecordDB.dbo.RecordChangeCard (UserID,GameID,TypeID,KindID,ClubID,BeforeCard,ChangeCard,CollectClientIP,CollectDateTime)
			VALUES (@UserID,@GameID,3,@wKindID,@dwClubID,@UserCard,-@lConsumeCount,N'--------------',GETDATE())

			-- ���·���
			UPDATE GameCardInfo SET Score-=@lConsumeCount WHERE UserID=@UserID
		END
		
		SET @Index+=1
	END

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- �׳��Ƹ�
	SELECT a.UserID, ISNULL(b.Score,0) AS UserCard, ISNULL(c.Score,0) AS UserGold  FROM @TableUserList a LEFT JOIN GameCardInfo b ON b.UserID=a.UserID LEFT JOIN LHGameGoldDB.dbo.GameScoreInfo c ON c.UserID=a.UserID
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------