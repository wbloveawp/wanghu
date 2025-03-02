USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_RecordBattleGrade]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_RecordBattleGrade]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- Լս��¼
CREATE PROC GSP_GR_RecordBattleGrade
	@dwBattleNum INT,							-- ӳ����
	@cbBattleFinish TINYINT,					-- ��ɱ�־
	@lBattleRecordID BIGINT,					-- ��¼��ʶ
	@wKindID SMALLINT,							-- ���ͱ�ʶ
	@wServerID SMALLINT,						-- �����ʶ
	@cbScoreKind TINYINT,						-- ��������
	@dwOwnerID INT,								-- ������ʶ
	@dwClubID INT,								-- ��ݱ�ʶ	
	@dwClubCreaterID INT,						-- ������ʶ
	@cbSettleKind TINYINT,						-- ��ɢ��ʽ
	@wFinishCount SMALLINT,						-- ��ɾ���
	@wPlayCount SMALLINT,						-- ��Ϸ����
	@dwPlayTime INT,							-- ��Ϸʱ��
	@dwCreateTime INT,							-- ����ʱ��
	@lBaseScore INT,							-- ��Ϸ�׷�
	@lWinnerScore INT,							-- Ӯ�ҽ���
	@wPayType SMALLINT,							-- ֧����ʽ
	@dwPayUserID INT,							-- ֧�����
	@dwConfigID INT,							-- �淨��ʶ
	@cbAllowPublic TINYINT,						-- ������
	@lConsumeCount INT,							-- ��������
	@cbCurrencyKind TINYINT,					-- ��������	
	@strGameOption NVARCHAR(128),				-- ��Ϸ����
	@strUserScoreString NVARCHAR(MAX)			-- ������Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ��������
DECLARE @UserID INT
DECLARE @Score INT
DECLARE @RankID INT
DECLARE @WinCount INT	
DECLARE @LostCount INT
DECLARE @RewardGold INT
DECLARE @ConsumeCard INT
DECLARE @ConsumeGold INT
DECLARE @SysRevenue INT
DECLARE @ClubRevenue INT
DECLARE @TotalSysRevenue INT	
DECLARE @TotalClubRevenue INT	
DECLARE @BigWinCount INT
DECLARE @MinWinnerScore INT

-- ��������
DECLARE	@TableID INT									
DECLARE @Sqlstr		NVARCHAR(512)						
DECLARE	@TableName	NVARCHAR(128)
DECLARE @UserMemberType SMALLINT

-- ִ���߼�
BEGIN

	-- ���ڱ��
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ����
	DECLARE @TableUserGrade TABLE (ID INT PRIMARY KEY IDENTITY(1,1),UserID INT,RankID INT, Score BIGINT, RewardScore BIGINT, SysRevenue INT,ClubRevenue INT)

	-- ��������
	IF ISJSON(@strUserScoreString)>0
	BEGIN
		INSERT INTO @TableUserGrade(UserID,RankID,Score,RewardScore,SysRevenue,ClubRevenue)
		SELECT JSON_VALUE(VALUE,N'$.UserID'),JSON_VALUE(VALUE,N'$.RankID'),JSON_VALUE(VALUE,N'$.Score'),JSON_VALUE(VALUE,N'$.RewardScore'),JSON_VALUE(VALUE,N'$.SysRevenue'),JSON_VALUE(VALUE,N'$.ClubRevenue') FROM OPENJSON(@strUserScoreString)
	END

	-- ��������
	DECLARE @Index INT
	DECLARE @UserCount INT

	-- ��ѯ����
	SELECT @Index=1,@UserCount=COUNT(1),@TotalSysRevenue=0,@TotalClubRevenue=0,@SysRevenue=0,@ClubRevenue=0,@RewardGold=0,@MinWinnerScore=0 FROM @TableUserGrade	

	-- �����¼
	IF NOT EXISTS(SELECT 1 FROM LHBattleRecordDB.dbo.RecordBattleInfo(NOLOCK) WHERE RecordID=@lBattleRecordID)
	BEGIN
		INSERT INTO LHBattleRecordDB.dbo.RecordBattleInfo (RecordID,DateID,BattleNum,BattleFinish,ClubID,KindID,ServerID,ScoreKind,OwnerID,SettleKind,FinishCount,PlayCount,
			PlayTime,CreateTime,BaseScore,WinnerScore,PayType,PayUserID,AllowPublic,ConsumeCount,CurrencyKind,GameOption,UserCount,UserScoreString,CollectDate)
		VALUES (@lBattleRecordID,@DateID,@dwBattleNum,@cbBattleFinish,@dwClubID,@wKindID,@wServerID,@cbScoreKind,@dwOwnerID,@cbSettleKind,@wFinishCount,@wPlayCount,
			@dwPlayTime,@dwCreateTime,@lBaseScore,@lWinnerScore,@wPayType,@dwPayUserID,@cbAllowPublic,@lConsumeCount,@cbCurrencyKind,@strGameOption,@UserCount,@strUserScoreString,GetDate())
	END

	-- ��ѯվ��								
	IF @dwClubID<>0
	BEGIN
		-- �ֱ��ʶ				
		SET	@TableID=0;

		-- Ⱥ���Ա��
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

		-- ��СӮ��
		SELECT @MinWinnerScore=ISNULL(MinWinnerScore,0) FROM ClubKindSetting(NOLOCK) WHERE ClubID=@dwClubID AND KindID=@wKindID
	END	

	-- ��������
	WHILE @Index<=@UserCount
	BEGIN
		-- ��ѯ����
		SELECT @UserID=UserID,@RankID=RankID,@Score=Score,@RewardGold=ISNULL(RewardScore,0),@SysRevenue=ISNULL(SysRevenue,0),@ClubRevenue=ISNULL(ClubRevenue,0) FROM @TableUserGrade WHERE ID=@Index

		-- ����ͳ��
		IF @cbBattleFinish=1
		BEGIN
			UPDATE StreamBattleKindInfo SET BattleCount=BattleCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND UserID=@UserID 
			IF @@ROWCOUNT=0
			BEGIN
				INSERT INTO StreamBattleKindInfo(DateID,UserID,KindID,BattleCount) VALUES(@DateID,@UserID,@wKindID,1)
			END
		END

		-- ����˰��
		SET @TotalSysRevenue+=@SysRevenue
		SET @TotalClubRevenue+=@ClubRevenue

		-- ����Լս
		IF @dwClubID>0
		BEGIN
			-- ���ñ���
			SELECT @WinCount=0,@LostCount=0,@ConsumeCard=0,@ConsumeGold=0,@BigWinCount=0			

			-- ��Ӯ����
			IF @RankID=@UserCount SET @LostCount=1 					
			IF @RankID=1 AND @Score>0 AND @wPayType<>2  SET @WinCount=1 	
			
			-- ��Ӯ��
			IF @RankID=1 AND @Score>=@MinWinnerScore SET @BigWinCount=1
			
			-- ���֧��
			IF @cbCurrencyKind=1
			BEGIN
				-- AA֧��
				IF @wPayType=2 SET @ConsumeGold=@lConsumeCount
			END

			-- ����֧��
			IF @cbCurrencyKind=2
			BEGIN
				-- AA֧��
				IF @wPayType=2 SET @ConsumeCard=@lConsumeCount

				-- ����֧��
				IF @wPayType=1 AND @UserID=@dwPayUserID SET @ConsumeCard=@lConsumeCount

				-- ��Ӯ��֧��
				IF @wPayType=4 AND @UserID=@dwPayUserID SET @ConsumeCard=@lConsumeCount
			END

			-- ����֧�����
			IF @UserID=@dwPayUserID SET @dwPayUserID=0		
		END

		-- ��Ҽ�¼
		IF NOT EXISTS(SELECT 1 FROM LHBattleRecordDB.dbo.RecordBattleUser(NOLOCK) WHERE RecordID=@lBattleRecordID AND UserID=@UserID)
		BEGIN
			IF @dwClubID=0
			BEGIN
				-- �����¼
				INSERT INTO LHBattleRecordDB.dbo.RecordBattleUser(RecordID,UserID,Score,SysRevenue,ClubRevenue,DateID,ClubID,RankID,BattleNum,CollectDate)
				VALUES (@lBattleRecordID,@UserID,@Score,@SysRevenue,@ClubRevenue,@DateID,@dwClubID,@RankID,@dwBattleNum,GetDate())

			END ELSE
			BEGIN
				-- �쳣����
				BEGIN TRY 
					-- ��ʼ����
					SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
					BEGIN TRAN										

					-- Ӯ��ͳ��
					IF @Score>=0
					BEGIN
						-- ��Ӯͳ��
						UPDATE StreamClubBalance SET BattleCount+=1,ConsumeCard+=@ConsumeCard,ConsumeGold+=@ConsumeGold,RewardGold+=@RewardGold,WinCount+=@WinCount,WinScore+=@Score,
							   SysRevenue+=@SysRevenue,ClubRevenue+=@ClubRevenue,BigWinCount+=@BigWinCount  
						WHERE DateID=@DateID AND ClubID=@dwClubID AND UserID=@UserID
						IF @@ROWCOUNT=0
						BEGIN		
							-- ��������	
							INSERT INTO StreamClubBalance(DateID,ClubID,UserID,WinCount,WinScore,SysRevenue,ClubRevenue,BattleCount,BalanceCount,ConsumeCard,ConsumeGold,RewardGold,BigWinCount)
							VALUES(@DateID,@dwClubID,@UserID,@WinCount,@Score,@SysRevenue,@ClubRevenue,1,0,@ConsumeCard,@ConsumeGold,@RewardGold,@BigWinCount)					
						END
				
						-- ��Ӯͳ��
						UPDATE StreamClubUser SET WinCount+=@WinCount,WinScore+=@Score,ConsumeCard+=@ConsumeCard,ConsumeGold+=@ConsumeGold,RewardGold+=@RewardGold,SysRevenue+=@SysRevenue,
							   ClubRevenue+=@ClubRevenue,BattleCount+=1,BigWinCount+=@BigWinCount,LastBattleTime=GetDate() 
						WHERE DateID=@DateID AND ClubID=@dwClubID AND UserID=@UserID AND KindID=@wKindID
						IF @@ROWCOUNT=0
						BEGIN			
							INSERT INTO StreamClubUser(DateID,ClubID,UserID,KindID,WinCount,WinScore,SysRevenue,ClubRevenue,ConsumeCard,ConsumeGold,RewardGold,BigWinCount,BattleCount,LastBattleTime)
							VALUES (@DateID,@dwClubID,@UserID,@wKindID,@WinCount,@Score,@SysRevenue,@ClubRevenue,@ConsumeCard,@ConsumeGold,@RewardGold,@BigWinCount,1,GetDate())
						END							
					END			
			
					-- ���ͳ��
					IF @Score<0
					BEGIN
						-- ��������
						UPDATE StreamClubBalance SET BattleCount+=1,ConsumeCard+=@ConsumeCard,ConsumeGold+=@ConsumeGold,RewardGold+=@RewardGold,LostCount+=@LostCount,LostScore+=@Score,SysRevenue+=@SysRevenue,ClubRevenue+=@ClubRevenue
						WHERE DateID=@DateID AND ClubID=@dwClubID AND UserID=@UserID 
						IF @@ROWCOUNT=0
						BEGIN		
							-- ��������	
							INSERT INTO StreamClubBalance(DateID,ClubID,UserID,LostCount,LostScore,SysRevenue,ClubRevenue,BattleCount,BalanceCount,ConsumeCard,ConsumeGold,RewardGold)
							VALUES(@DateID,@dwClubID,@UserID,@LostCount,@Score,@SysRevenue,@ClubRevenue,1,0,@ConsumeCard,@ConsumeGold,@RewardGold)					
						END		
				
						-- ��Ӯͳ��
						UPDATE StreamClubUser SET LostCount+=@LostCount,LostScore+=@Score,ConsumeCard+=@ConsumeCard,ConsumeGold+=@ConsumeGold,RewardGold+=@RewardGold,SysRevenue+=@SysRevenue,
							   ClubRevenue+=@ClubRevenue,BattleCount+=1,LastBattleTime=GetDate() 
						WHERE DateID=@DateID AND ClubID=@dwClubID AND UserID=@UserID AND KindID=@wKindID
						IF @@ROWCOUNT=0
						BEGIN			
							INSERT INTO StreamClubUser(DateID,ClubID,UserID,KindID,LostCount,LostScore,SysRevenue,ClubRevenue,ConsumeCard,ConsumeGold,RewardGold,BattleCount,LastBattleTime)
							VALUES (@DateID,@dwClubID,@UserID,@wKindID,@LostCount,@Score,@SysRevenue,@ClubRevenue,@ConsumeCard,@ConsumeGold,@RewardGold,1,GetDate())
						END							
					END

					-- �����¼
					INSERT INTO LHBattleRecordDB.dbo.RecordBattleUser(RecordID,UserID,Score,SysRevenue,ClubRevenue,DateID,ClubID,RankID,BattleNum,CollectDate)
					VALUES (@lBattleRecordID,@UserID,@Score,@SysRevenue,@ClubRevenue,@DateID,@dwClubID,@RankID,@dwBattleNum,GetDate())
							
					-- ��������
					COMMIT TRAN
					SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

				END TRY 
				BEGIN CATCH

					-- �ع�����
					ROLLBACK TRAN
					SET TRANSACTION ISOLATION LEVEL READ COMMITTED
					RETURN 1

				END CATCH			
			END													
		END

		SET @Index+=1
	END		

	-- ֧���������ͳ��
	IF @dwClubID<>0 AND @dwPayUserID<>0 AND @cbCurrencyKind=2
	BEGIN		
		-- ��������
		UPDATE StreamClubBalance SET ConsumeCard+=@lConsumeCount WHERE DateID=@DateID AND ClubID=@dwClubID AND UserID=@dwPayUserID
		IF @@ROWCOUNT=0
		BEGIN
			-- ��������
			INSERT INTO StreamClubBalance(DateID,ClubID,UserID,ConsumeCard) VALUES(@DateID,@dwClubID,@dwPayUserID,@lConsumeCount)
		END	
	END

	--------------------------------------- Լսͳ�� -------------------------------------------	

	-- ��������
	SET @ConsumeCard=0		
	IF @cbCurrencyKind=2 SET @ConsumeCard=@lConsumeCount

	-- ��ɾ���
	DECLARE @BattleFinishCount INT
	SET @BattleFinishCount=0
	IF @cbBattleFinish=1 SET @BattleFinishCount=1

	-- ����ͳ��
	UPDATE StreamBattleInfo SET BattleCount+=1,BattleFinishCount+=@BattleFinishCount,ConsumeCard+=@ConsumeCard
	WHERE DateID=@DateID AND KindID=@wKindID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO StreamBattleInfo(DateID,KindID,BattleCount,BattleFinishCount,ConsumeCard) VALUES(@DateID,@wKindID,1,@BattleFinishCount,@ConsumeCard)
	END

	-- ����ͳ��
	IF @dwClubID<>0
	BEGIN
		UPDATE StreamClubBattle SET BattleCount+=1,BattleFinishCount+=@BattleFinishCount,ConsumeCard+=@ConsumeCard,SysRevenue+=@TotalSysRevenue,ClubRevenue+=@TotalClubRevenue
		WHERE DateID=@DateID AND ClubID=@dwClubID AND KindID=@wKindID
		IF @@ROWCOUNT=0
		BEGIN
			INSERT INTO StreamClubBattle(DateID,ClubID,KindID,BattleCount,BattleFinishCount,SysRevenue,ClubRevenue,ConsumeCard) 
			VALUES(@DateID,@dwClubID,@wKindID,1,@BattleFinishCount,@TotalSysRevenue,@TotalClubRevenue,@ConsumeCard)
		END
	END

	--------------------------------------- Լսͳ�� -------------------------------------------
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------