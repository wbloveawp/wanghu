USE LHGameRecordDB
GO

/************************������Ʒ************************/
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WSP_GrantGoods]')) 
DROP PROCEDURE [dbo].[WSP_GrantGoods]

GO

CREATE PROC WSP_GrantGoods
	@dwUserID INT,							-- �û���ʶ			
	@dwGoodsID INT,							-- ��Ʒ��ʶ
	@wGoodsLevel SMALLINT,					-- ��Ʒ�ȼ�
	@dwIncCount BIGINT,						-- ��������
	@dwIncIndate INT,						-- ������Ч��		
	@dwTimestamp INT,						-- ʱ���	
	@strClientIP NVARCHAR(15),				-- IP��ַ
	@dwMasterID INT,						-- �����˺�
	@strNote NVARCHAR(200),					-- ��ע
	@Message NVARCHAR(127) OUT				-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- �ж�����
	IF EXISTS(SELECT * FROM LHAccountsDB.dbo.AccountsOnlineInfo WHERE UserID=@dwUserID AND ServerID<>0) 
	BEGIN
		SET @Message='�û���������״̬���޷����ͣ�������������߻������������'
		RETURN -1
	END

	-- ��ѯ�û�
	DECLARE @GameID INT
	SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- У���û�
	IF @GameID IS NULL
	BEGIN
		SET @Message='�û�������'
		RETURN -2
	END

	DECLARE @ChangeType INT
	SET @ChangeType=100

	-- ������ʯ
	IF @dwGoodsID=1
	BEGIN
		-- ��ѯ��ʯ
		DECLARE @BeforeIngot BIGINT
		SELECT @BeforeIngot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WITH(UPDLOCK)  WHERE UserID=@dwUserID

		-- ��������
		IF @BeforeIngot IS NULL 
		BEGIN
			SET @BeforeIngot=0
			INSERT INTO LHAccountsDB.dbo.AccountsIngotInfo(UserID,Score) VALUES (@dwUserID,0)
		END

		-- ����У��
		IF @BeforeIngot+@dwIncCount<0
		BEGIN
			SET @Message='��ʯ����Ϊ��'
			RETURN -3
		END

		-- ������ʯ
		UPDATE LHAccountsDB.dbo.AccountsIngotInfo SET Score+=@dwIncCount,GrantTimes=GrantTimes+1,GrantAmount=GrantAmount+@dwIncCount WHERE UserID=@dwUserID

		-- �����¼
		INSERT INTO LHGameRecordDB.dbo.RecordChangeIngot(UserID,GameID,TypeID,BeforeIngot,ChangeIngot,CollectClientIP,CollectDateTime)
		VALUES(@dwUserID,@GameID,@ChangeType,@BeforeIngot,@dwIncCount,@strClientIP,GetDate())

		-- ���ͼ�¼
		INSERT INTO LHGameRecordDB.dbo.RecordGrantIngot(UserID,CurIngot,AddIngot,MasterID,Reason,CollectClientIP)
		VALUES(@dwUserID,@BeforeIngot,@dwIncCount,@dwMasterID,@strNote,@strClientIP)
	END
	-- ���ͽ��
	ELSE IF @dwGoodsID=2
	BEGIN
		-- ��ѯ���
		DECLARE @BeforeGold BIGINT		
		SELECT @BeforeGold=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WITH(UPDLOCK) WHERE UserID=@dwUserID
		
		-- ��������
		IF @BeforeGold IS NULL
		BEGIN
			-- ���ñ���
			SET @BeforeGold=0			

			-- ��������
			INSERT INTO LHAccountsDB.dbo.AccountsGoldInfo(UserID,Score) VALUES (@dwUserID,0)
		END

		-- ����У��
		IF @BeforeGold+@dwIncCount<0
		BEGIN
			SET @Message='��Ҳ���Ϊ��'
			RETURN -3
		END
		
		-- ���½��
		UPDATE LHAccountsDB.dbo.AccountsGoldInfo SET Score=Score+@dwIncCount,GrantTimes=GrantTimes+1,GrantAmount=GrantAmount+@dwIncCount WHERE UserID=@dwUserID

		-- �����¼
		INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID,GameID,TypeID,BeforeGold,ChangeGold,CollectClientIP,CollectDateTime)
		VALUES(@dwUserID,@GameID,@ChangeType,@BeforeGold,@dwIncCount,@strClientIP,GetDate())

		-- ���ͼ�¼
		INSERT INTO LHGameRecordDB.dbo.RecordGrantGold(UserID,CurGold,AddGold,MasterID,Reason,CollectClientIP)
		VALUES(@dwUserID,@BeforeGold,@dwIncCount,@dwMasterID,@strNote,@strClientIP)
	END
	-- ���Ͱ���
	ELSE IF @dwGoodsID=4
	BEGIN
		-- ��ѯ���
		DECLARE @BeforeHeart BIGINT		
		SELECT @BeforeHeart=LoveHeart FROM LHGameRankingDB.dbo.GameScoreInfo WITH(UPDLOCK) WHERE UserID=@dwUserID
		
		-- ��������
		IF @BeforeHeart IS NULL
		BEGIN
			-- ���ñ���
			SET @BeforeHeart=0			

			-- ��������
			INSERT INTO LHGameRankingDB.dbo.GameScoreInfo (UserID,LoveHeart) VALUES (@dwUserID,0)
		END

		-- ����У��
		IF @BeforeHeart+@dwIncCount<0
		BEGIN
			SET @Message='���Ĳ���Ϊ��'
			RETURN -3
		END
		
		-- ���°���
		UPDATE LHGameRankingDB.dbo.GameScoreInfo SET LoveHeart=LoveHeart+@dwIncCount WHERE UserID=@dwUserID

		-- �����¼
		INSERT INTO LHGameRecordDB.dbo.RecordChangeLoveHeart(UserID, GameID, TypeID, BeforeLoveHeart, ChangeLoveHeart, CollectClientIP, CollectDateTime)
		VALUES (@dwUserID,@GameID,@ChangeType,@BeforeHeart,@dwIncCount,@strClientIP,GetDate())

		-- ���ͼ�¼
		INSERT INTO LHGameRecordDB.dbo.RecordGrantLoveHeart(UserID,CurLoveHeart,AddLoveHeart,MasterID,Reason,CollectClientIP)
		VALUES(@dwUserID,@BeforeHeart,@dwIncCount,@dwMasterID,@strNote,@strClientIP)
	END	
	ELSE 
	BEGIN
		-- ��Ʒ����
		DECLARE @HasIndate TINYINT
		DECLARE @GoodsKind TINYINT
		DECLARE @GoodsOption NVARCHAR(2048)
		SELECT @HasIndate=HasIndate,@GoodsKind=GoodsKind,@GoodsOption=GoodsOption FROM LHPlatformDB.dbo.GameGoodsInfo WHERE GoodsID=@dwGoodsID
		-- ��Ʒ������
		IF @GoodsKind IS NULL 
		BEGIN
			RETURN @dwIncCount
		END

		-- �����ȼ�
		IF @GoodsKind=11 AND @wGoodsLevel=0
		BEGIN
			SET @wGoodsLevel=1
		END

		-- ��Ʒ��Ϣ
		DECLARE @GoodsCount INT	
		DECLARE @GoodsIndate INT		
		DECLARE @GoodsExpireTime INT	

		-- ��Ʒ��Ϣ	
		DECLARE @BeforeCount INT
		DECLARE @BeforeIndate INT
		DECLARE @BeforeExpireTime INT
		SELECT @BeforeCount=GoodsCount,@BeforeIndate=GoodsIndate,@BeforeExpireTime=ExpireTime FROM LHAccountsDB.dbo.AccountsPack WITH(UPDLOCK) WHERE UserID=@dwUserID AND GoodsID=@dwGoodsID

		-- ������Ʒ
		IF @BeforeCount IS NOT NULL
		BEGIN	
			-- �ж���Ч��
			IF @HasIndate=1
			BEGIN		
				-- ��������
				SET @GoodsCount=1
				SET @GoodsExpireTime=0

				-- �����ж�
				IF @BeforeCount=0
				BEGIN
					IF @dwIncIndate=0 SET @GoodsIndate=0
					IF @dwIncIndate>0 
					BEGIN
						SET @GoodsIndate=@dwIncIndate
						SET @GoodsExpireTime=@dwTimestamp+@dwIncIndate
					END
				END ELSE
				BEGIN
					IF @BeforeIndate=0 
					BEGIN
						SET @GoodsIndate=0
						SET @GoodsExpireTime=0					
					END ELSE
					BEGIN
						IF @dwIncIndate=0 SET @GoodsIndate=0
						IF @dwIncIndate>0 
						BEGIN
							SET @GoodsIndate=@BeforeIndate+@dwIncIndate
							SET @GoodsExpireTime=@BeforeExpireTime+@dwIncIndate
						END
					END
				END			
			END ELSE
			BEGIN
				SET @GoodsIndate=0
				SET @GoodsExpireTime=0
				SET @GoodsCount=@BeforeCount+@dwIncCount
			END	
		
			-- ������Ʒ
			UPDATE LHAccountsDB.dbo.AccountsPack SET GoodsCount=@GoodsCount,GoodsLevel=@wGoodsLevel,GoodsIndate=@GoodsIndate,ExpireTime=@GoodsExpireTime WHERE UserID=@dwUserID AND GoodsID=@dwGoodsID						
		END 
		ELSE
		BEGIN
			-- �ж���Ч��
			IF @HasIndate=1
			BEGIN		
				-- ��������
				SET @GoodsCount=1
				SET @GoodsExpireTime=0
			
				-- ������Ч��
				IF @dwIncIndate=0 SET @GoodsIndate=0
				IF @dwIncIndate>0 
				BEGIN
					SET @GoodsIndate=@dwIncIndate
					SET @GoodsExpireTime=@dwTimestamp+@dwIncIndate
				END			
			END ELSE
			BEGIN
				SET @GoodsIndate=0
				SET @GoodsExpireTime=0
				SET @GoodsCount=@dwIncCount
			END

			-- ��������
			SET @BeforeCount=0

			-- �����Ʒ
			INSERT INTO LHAccountsDB.dbo.AccountsPack(UserID, GoodsID, GoodsKind, GoodsLevel, GoodsCount, GoodsIndate, BindState, ExpireTime, CollectTime)
			VALUES (@dwUserID, @dwGoodsID, @GoodsKind, @wGoodsLevel, @GoodsCount, @GoodsIndate, 0, @GoodsExpireTime, GETDATE())	
		END

		-- �����¼
		IF @GoodsCount-@BeforeCount<>0
		BEGIN
			INSERT INTO LHGameRecordDB.dbo.RecordChangeGoods(UserID, GoodsID, GoodsKind, TypeID, RelationID, BeforeCount, ChangeCount, CollectClientIP, CollectDateTime,CollectNote)
			VALUES (@dwUserID, @dwGoodsID, @GoodsKind, 100, 0, @BeforeCount, @GoodsCount-@BeforeCount, @strClientIP ,GETDATE(),@strNote)

			INSERT INTO LHGameRecordDB.dbo.RecordGrantGoods(UserID,GoodsID,CurGoodsCount,AddGoodsCount,MasterID,Reason,CollectClientIP)
			VALUES(@dwUserID,@dwGoodsID,@BeforeCount,@GoodsCount-@BeforeCount,@dwMasterID,@strNote,@strClientIP)
		END
	END

	SET @Message='���ͳɹ����ɹ�������'+LTRIM(@GoodsCount)
END

RETURN 0
GO

