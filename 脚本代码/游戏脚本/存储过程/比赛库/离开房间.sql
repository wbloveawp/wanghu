
----------------------------------------------------------------------------------------------------

USE [LHGameMatchDB]
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

	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D
	@szMachineID NVARCHAR(32),					-- ���� I D
	@cbLeaveReason INT,							-- �뿪ԭ��
	@dwInoutIndex INT							-- ��������	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- У��ԭ��
	IF @cbLeaveReason<>7
	BEGIN
		-- ��������
		DECLARE @DateID INT
		SET @DateID = CAST(CAST(GETDATE() AS FLOAT) AS INT)

		-- ����������
		IF @strClientIP=N'0.0.0.0'
		BEGIN
			UPDATE LHAccountsDB.dbo.RobotLockInfo SET RobotStatus=0,ServerID=0,BatchID=0,LockDateTime=GETDATE() WHERE UserID=@dwUserID
		END

		-- �û�����
		UPDATE GameScoreInfo SET PlayTimeCount=PlayTimeCount+@dwPlayTimeCount, OnLineTimeCount=OnLineTimeCount+@dwOnLineTimeCount WHERE UserID=@dwUserID

		-- �û�����
		UPDATE LHAccountsDB.dbo.AccountsInfo SET Experience+=@lExperience,PlayTimeCount+=@dwPlayTimeCount,OnlineTimeCount+=@dwOnLineTimeCount WHERE UserID=@dwUserID

		-- �������
		UPDATE LHAccountsDB.dbo.AccountsOnlineInfo SET KindID=0,ServerID=0,ServerKind=0,ModifyTime=GETDATE() WHERE UserID=@dwUserID		

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
