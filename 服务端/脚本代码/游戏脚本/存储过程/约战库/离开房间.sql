
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LeaveGameServer]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LeaveGameServer]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �뿪����
CREATE PROC GSP_GR_LeaveGameServer
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
	@dwOnLineTimeCount INT,						-- ����ʱ��

	-- ��¼�ɼ�
	@lRecordScore BIGINT,						-- �û�����
	@lRecordRevenue BIGINT,						-- ��Ϸ˰��
	@lRecordWinCount INT,						-- ʤ������
	@lRecordLostCount INT,						-- ʧ������
	@lRecordDrawCount INT,						-- �;�����
	@lRecordFleeCount INT,						-- ������Ŀ
	@lRecordExperience INT,						-- �û�����	
	@dwRecordPlayTimeCount INT,					-- ��Ϸʱ��

	-- ������Ϣ
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D
	@szComputerID NVARCHAR(32),					-- ��������
	@cbLeaveReason INT,							-- �뿪ԭ��
	@dwInoutIndex INT							-- ��������

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��������
	DECLARE @ClubID INT
	DECLARE @BattleNum INT
	DECLARE @ScoreKind TINYINT

	-- ��ѯԼս��Ϣ
	SELECT @ClubID=ClubID,@BattleNum=BattleNum,@ScoreKind=ScoreKind FROM RecordUserInout(NOLOCK) WHERE ID=@dwInoutIndex

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN	

	-- ��ݷ�
	IF @ScoreKind=2
	BEGIN
		-- ��������
		DECLARE	@TableID INT								
		DECLARE @Sqlstr		NVARCHAR(512)						
		DECLARE	@TableName	NVARCHAR(128)

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
		
		-- ������Ϣ
		UPDATE GameScoreInfo SET WinCount+=@lWinCount,LostCount+=@lLostCount, DrawCount+=@lDrawCount, FleeCount+=@lFleeCount, PlayTimeCount+=@dwPlayTimeCount,OnLineTimeCount+=@dwOnLineTimeCount,
			   WinStreak=@lWinStreak, MaxWinStreak=@lMaxWinStreak 
		WHERE UserID=@dwUserID
	END	

	-- Լս����
	IF @ScoreKind=3
	BEGIN
		UPDATE GameScoreInfo SET Score+=@lScore,WinCount+=@lWinCount,LostCount+=@lLostCount, DrawCount+=@lDrawCount, FleeCount+=@lFleeCount, PlayTimeCount+=@dwPlayTimeCount,OnLineTimeCount+=@dwOnLineTimeCount,
			   WinStreak=@lWinStreak, MaxWinStreak=@lMaxWinStreak 
		WHERE UserID=@dwUserID
	END	

	-- ��������
	UPDATE BattleUserLocker SET KindID=0,ServerID=0,OnlineStatus=0 WHERE UserID=@dwUserID

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- �������
	UPDATE LHAccountsDB.dbo.AccountsOnlineInfo SET KindID=0,ServerID=0,ServerKind=0,ModifyTime=GETDATE() WHERE UserID=@dwUserID	

	-- �û�����
	IF @lExperience>0
	BEGIN
		UPDATE LHAccountsDB.dbo.AccountsInfo SET Experience=Experience+@lExperience WHERE UserID=@dwUserID
	END

	-- ��������
	DECLARE @DateID INT
	SET @DateID = CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- д����ͳ��
	UPDATE StreamBattleKindInfo SET WinCount=WinCount+@lWinCount,LostCount=LostCount+@lLostCount,PlayTimeCount=PlayTimeCount+@dwPlayTimeCount
	FROM StreamBattleKindInfo WHERE DateID=@DateID AND UserID=@dwUserID AND KindID=@wKindID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO StreamBattleKindInfo(DateID,UserID,KindID,WinCount,LostCount,PlayTimeCount) 
		VALUES(@DateID,@dwUserID,@wKindID,@lWinCount,@lLostCount,@dwPlayTimeCount)
	END		

	-- ���½�����¼
	UPDATE RecordUserInout SET LeaveTime=Getdate(),LeaveReason=@cbLeaveReason,LeaveMachine=@szComputerID,LeaveClientIP=@strClientIP,Score=@lRecordScore,Revenue=@lRecordRevenue,
		WinCount=@lRecordWinCount,LostCount=@lRecordLostCount,DrawCount=@lRecordDrawCount,FleeCount=@lRecordFleeCount,Experience=@lRecordExperience,PlayTimeCount=@dwRecordPlayTimeCount,
		OnLineTimeCount=@dwOnLineTimeCount
	WHERE ID=@dwInoutIndex
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------