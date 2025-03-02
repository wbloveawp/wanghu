
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'dbo.GSP_GR_LoadRobotUser') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE dbo.GSP_GR_LoadRobotUser
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'dbo.GSP_GR_UnLockRobotUser') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE dbo.GSP_GR_UnLockRobotUser
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ���ػ���
CREATE PROC GSP_GR_LoadRobotUser
	@wServerID	SMALLINT,					-- �����ʶ
	@dwBatchID	INT,						-- ���α�ʶ
	@dwRobotCount INT						-- ������Ŀ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ɾ�����߱�
	DELETE AccountsOnlineInfo WHERE UserID IN (SELECT UserID FROM RobotLockInfo(NOLOCK) WHERE DATEDIFF(hh,LockDateTime,GetDate())>=24 AND RobotStatus<>0)

	-- ��������
	UPDATE RobotLockInfo SET RobotStatus=0,ServerID=0,BatchID=0 WHERE DATEDIFF(hh,LockDateTime,GetDate())>=24

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	BEGIN TRAN		

	-- ��ѯ����
	SELECT UserID,LogonPass INTO #TempRobots FROM RobotLockInfo(NOLOCK)
	WHERE UserID IN (SELECT TOP (@dwRobotCount) UserID FROM RobotLockInfo(NOLOCK) WHERE RobotStatus=0 AND ServerID=0 AND DATEDIFF(hh,LockDateTime,GetDate())>=12 ORDER BY NEWID()) 

	-- ����״̬
	UPDATE RobotLockInfo SET RobotStatus=1,BatchID=@dwBatchID,ServerID=@wServerID,LockDateTime=GetDate() 
	FROM RobotLockInfo a,#TempRobots b WHERE a.UserID = b.UserID

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- ��ѯ����
	SELECT * FROM #TempRobots

	-- ������ʱ��	
	IF OBJECT_ID('tempdb..#TempRobots') IS NOT NULL DROP TABLE #TempRobots
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GR_UnLockRobotUser
	@wServerID	SMALLINT					-- �����ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ����״̬
	UPDATE RobotLockInfo SET RobotStatus=0,ServerID=0,BatchID=0,LockDateTime=N'1970-01-01 00:00:00'
	WHERE RobotStatus<>0 AND BatchID<>0 AND ServerID=@wServerID

END

RETURN 0

----------------------------------------------------------------------------------------------------
