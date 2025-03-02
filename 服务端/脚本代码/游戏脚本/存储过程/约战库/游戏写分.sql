----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
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
	@dwInoutIndex INT
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û�����
DECLARE @WinScore BIGINT
DECLARE @LostScore BIGINT

-- ִ���߼�
BEGIN

	-- д����ͳ��
	DECLARE @DateID INT
	SET @DateID = CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��ʼ��
	SELECT @LostScore=0, @WinScore=0

	-- ���÷���
	--IF @lScore>=0  
	--	SET @WinScore=@lScore		
	--ELSE 
	--	SET @LostScore=@lScore	

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��������
	DECLARE @ClubID INT
	DECLARE @BattleNum INT
	DECLARE @ScoreKind TINYINT

	-- ��ѯԼս��Ϣ
	SELECT @ClubID=ClubID,@BattleNum=BattleNum,@ScoreKind=ScoreKind FROM RecordUserInout(NOLOCK) WHERE ID=@dwInoutIndex
	
	-- ��ݷ�����
	IF @ScoreKind=2
	BEGIN
		-- ��������
		DECLARE	@TableID INT									
		DECLARE @Sqlstr	NVARCHAR(512)						
		DECLARE	@TableName NVARCHAR(128)
				
		-- �ֱ��ʶ				
		SET	@TableID=dbo.LHF_GetTableID(@ClubID);

		-- Ⱥ���Ա��
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));				

		-- ��ѯ�ű�		
		SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName,' SET ClubScore=ClubScore+',CONVERT(NVARCHAR(18), @lScore),',RevenueCount=RevenueCount+',CONVERT(NVARCHAR(18), @lRevenue),
							  ' WHERE MemberID=',CONVERT(NVARCHAR(18), @dwUserID),' AND ClubID=',CONVERT(NVARCHAR(18), @ClubID))

		-- ִ�нű�
		EXECUTE(@Sqlstr)	
		IF @@ERROR<>0
		BEGIN
			-- ����ع�
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
			RETURN 1
		END		
		
		-- ��Ϸ��Ϣ
		UPDATE GameScoreInfo SET WinCount+=@lWinCount,LostCount+=@lLostCount, DrawCount+=@lDrawCount, FleeCount+=@lFleeCount,WinStreak=@lWinStreak, MaxWinStreak=@lMaxWinStreak WHERE UserID=@dwUserID
	END	

	-- Լս��������
	IF @ScoreKind=3
	BEGIN
		-- ��Ϸ��Ϣ
		UPDATE GameScoreInfo SET Score+=@lScore,WinCount+=@lWinCount,LostCount+=@lLostCount, DrawCount+=@lDrawCount, FleeCount+=@lFleeCount,WinStreak=@lWinStreak, MaxWinStreak=@lMaxWinStreak WHERE UserID=@dwUserID
	END		

	-- ����ͳ��
	UPDATE LHGameGoldDB..ReportUserScore SET GainScore+=@WinScore,LoseScore+=@LostScore,WinCount+=@lWinCount,LostCount+=@lLostCount,DrawCount+=@lDrawCount,FleeCount+=@lFleeCount,UserRevenue+=@lRevenue,
		PlayTimeCount+=@dwPlayTimeCount,ModifyDateTime=GETDATE() 
	WHERE DateID=@DateID AND UserID=@dwUserID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO LHGameGoldDB..ReportUserScore(DateID,UserID,GainScore,LoseScore,WinCount,LostCount,DrawCount,FleeCount,UserRevenue,PlayTimeCount,InsertDateTime,ModifyDateTime)
		VALUES (@DateID,@dwUserID,@WinScore,@LostScore,@lWinCount,@lLostCount,@lDrawCount,@lFleeCount,@lRevenue,@dwPlayTimeCount,GETDATE(),GETDATE())
	END

	-- ����ͳ��
	UPDATE LHGameGoldDB..ReportUserKind SET GainScore+=@WinScore,LoseScore+=@LostScore,WinCount+=@lWinCount,LostCount+=@lLostCount,DrawCount+=@lDrawCount,FleeCount+=@lFleeCount,UserRevenue+=@lRevenue,
		PlayTimeCount+=@dwPlayTimeCount,ModifyDateTime=GETDATE() 
	WHERE DateID=@DateID AND UserID=@dwUserID AND KindID=@wKindID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO LHGameGoldDB..ReportUserKind(DateID,UserID,KindID,GainScore,LoseScore,WinCount,LostCount,DrawCount,FleeCount,UserRevenue,PlayTimeCount,InsertDateTime,ModifyDateTime)
		VALUES (@DateID,@dwUserID,@wKindID,@WinScore,@LostScore,@lWinCount,@lLostCount,@lDrawCount,@lFleeCount,@lRevenue,@dwPlayTimeCount,GETDATE(),GETDATE())
	END

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- �û�����
	IF @lExperience>0
	BEGIN
		UPDATE LHAccountsDB.dbo.AccountsInfo SET Experience=Experience+@lExperience WHERE UserID=@dwUserID	
	END

	-- д����ͳ��
	UPDATE StreamBattleKindInfo SET WinCount=WinCount+@lWinCount,LostCount=LostCount+@lLostCount,PlayTimeCount=PlayTimeCount+@dwPlayTimeCount
	FROM StreamBattleKindInfo WHERE DateID=@DateID AND UserID=@dwUserID AND KindID=@wKindID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO StreamBattleKindInfo(DateID,UserID,KindID,WinCount,LostCount,PlayTimeCount) 
		VALUES (@DateID,@dwUserID,@wKindID,@lWinCount,@lLostCount,@dwPlayTimeCount)
	END
END



RETURN 0

GO

----------------------------------------------------------------------------------------------------