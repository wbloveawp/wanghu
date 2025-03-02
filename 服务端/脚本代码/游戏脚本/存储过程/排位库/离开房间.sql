----------------------------------------------------------------------------------------------------

USE LHGameRankingDB
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
	@lExperience INT,							-- �û�����
	@lWinStreak INT,							-- ��ʤ����
	@lMaxWinStreak INT,							-- ��ʤ����
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
	@szMachineID NVARCHAR(32),					-- ��������
	@cbLeaveReason INT,							-- �뿪ԭ��	
	@dwInoutIndex INT							-- ��������
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
	
	-- ���ڱ�ʶ
	DECLARE @DateID	BIGINT
	SET @DateID = CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- У��ԭ��
	IF @cbLeaveReason<>7
	BEGIN
		
		-- ����������
		IF @strClientIP=N'0.0.0.0'
		BEGIN
			UPDATE LHAccountsDB.dbo.RobotLockInfo SET RobotStatus=0,ServerID=0,BatchID=0,LockDateTime=GETDATE() WHERE UserID=@dwUserID
		END

		-- ��ʼ����
		SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
		BEGIN TRAN

		-- ��ѯ����
		DECLARE @UserScore INT
		SELECT @UserScore=Score+@lScore FROM GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

		-- ��λ����
		DECLARE @MainOrder SMALLINT
		DECLARE @SubOrder SMALLINT
		DECLARE @StarOrder SMALLINT

		-- ��ѯ��λ
		SELECT @MainOrder=MainOrder,@SubOrder=SubOrder,@StarOrder=StarOrder FROM LHPlatformDB.dbo.GameRankLevel(NOLOCK) WHERE @UserScore>=NeedScore AND @UserScore<NeedScore+SpanScore

		-- �û�����
		UPDATE GameScoreInfo SET Score+=@lScore, WinCount+=@lWinCount, LostCount+=@lLostCount, DrawCount+=@lDrawCount, FleeCount+=@lFleeCount, PlayTimeCount+=@dwPlayTimeCount, OnLineTimeCount+=@dwOnLineTimeCount,
			   WinStreak=@lWinStreak, MaxWinStreak=@lMaxWinStreak, MainOrder=@MainOrder, SubOrder=@SubOrder, StarOrder=@StarOrder	
		WHERE UserID=@dwUserID

		-- �������
		UPDATE LHAccountsDB.dbo.AccountsOnlineInfo SET KindID=0,ServerID=0,ServerKind=0,ModifyTime=GETDATE() WHERE UserID=@dwUserID		

		-- �û���Ϣ
		UPDATE LHAccountsDB.dbo.AccountsInfo SET Experience+=@lExperience,PlayTimeCount+=@dwPlayTimeCount,OnlineTimeCount+=@dwOnLineTimeCount WHERE UserID=@dwUserID

		-- ��ʼ��
		SELECT @LostScore=0, @WinScore=0

		-- ���÷���
		IF @lScore>=0  
			SET @WinScore=@lScore		
		ELSE 
			SET @LostScore=@lScore	

		-- ����ͳ��
		IF @dwPlayTimeCount>0
		BEGIN
			UPDATE ReportUserScore SET GainScore+=@WinScore,LoseScore+=@LostScore,WinCount+=@lWinCount,LostCount+=@lLostCount,DrawCount+=@lDrawCount,FleeCount+=@lFleeCount,UserRevenue+=@lRevenue,
				PlayTimeCount+=@dwPlayTimeCount,ModifyDateTime=GETDATE() 
			WHERE DateID=@DateID AND UserID=@dwUserID
			IF @@ROWCOUNT=0
			BEGIN
				INSERT INTO ReportUserScore(DateID,UserID,GainScore,LoseScore,WinCount,LostCount,DrawCount,FleeCount,UserRevenue,PlayTimeCount,InsertDateTime,ModifyDateTime)
				VALUES (@DateID,@dwUserID,@WinScore,@LostScore,@lWinCount,@lLostCount,@lDrawCount,@lFleeCount,@lRevenue,@dwPlayTimeCount,GETDATE(),GETDATE())
			END
		END

		-- ��������
		COMMIT TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ����ͳ��
		IF @lRecordExperience>0
		BEGIN
			UPDATE [LHGameRecordDB].dbo.ReportDayUser SET Experience+=@lRecordExperience WHERE DateID=@DateID AND UserID=@dwUserID
			IF @@ROWCOUNT=0
			BEGIN
				INSERT INTO [LHGameRecordDB].dbo.ReportDayUser(DateID,UserID,Experience,ReportDateTime) VALUES(@DateID,@dwUserID,@lRecordExperience,GETDATE())
			END
		END

	END
	
	-- ���½�����¼
	UPDATE RecordUserInout SET LeaveTime=GETDATE(),LeaveReason=@cbLeaveReason,LeaveMachine=@szMachineID,LeaveClientIP=@strClientIP,Score=@lRecordScore,
		WinCount=@lRecordWinCount,LostCount=@lRecordLostCount,DrawCount=@lRecordDrawCount,FleeCount=@lRecordFleeCount,Experience=@lRecordExperience,
		PlayTimeCount=@dwRecordPlayTimeCount,OnLineTimeCount=@dwOnLineTimeCount
	WHERE ID=@dwInoutIndex

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
