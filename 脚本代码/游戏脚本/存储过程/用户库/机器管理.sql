
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_RobotQueryParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_RobotQueryParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_RobotAppendParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_RobotAppendParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_RobotModifyParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_RobotModifyParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_RobotDeleteParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_RobotDeleteParameter]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��ȡ����
CREATE PROC GSP_GP_RobotQueryParameter
	@wServerID INT								-- �����ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	SELECT * FROM RobotConfigure WHERE ServerID=@wServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��Ӳ���
CREATE PROC GSP_GP_RobotAppendParameter
	@wServerID INT,								-- �����ʶ
	@dwServiceMode INT,							-- ����ģʽ
	@dwRobotCount INT,							-- ������Ŀ
	@dwEnterTime INT,							-- ����ʱ��
	@dwLeaveTime INT,							-- �뿪ʱ��
	@dwEnterMinInterval INT,					-- ������
	@dwEnterMaxInterval INT,					-- ������
	@dwLeaveMinInterval	INT,					-- �뿪���
	@dwLeaveMaxInterval	INT,					-- �뿪���
	@lTakeMinScore	BIGINT,						-- Я������
	@lTakeMaxScore BIGINT,						-- Я������
	@dwSwitchMinInnings INT,					-- ��������
	@dwSwitchMaxInnings INT						-- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��������
	INSERT RobotConfigure(ServerID,ServiceMode,RobotCount,EnterTime,LeaveTime,EnterMinInterval,EnterMaxInterval,LeaveMinInterval,
			LeaveMaxInterval,TakeMinScore,TakeMaxScore,SwitchMinInnings,SwitchMaxInnings)
	VALUES(@wServerID,@dwServiceMode,@dwRobotCount,@dwEnterTime,@dwLeaveTime,@dwEnterMinInterval,@dwEnterMaxInterval,@dwLeaveMinInterval,
			@dwLeaveMaxInterval,@lTakeMinScore,@lTakeMaxScore,@dwSwitchMinInnings,@dwSwitchMaxInnings)

	-- ��ѯ����	
	DECLARE @dwBatchID INT
	SET @dwBatchID=SCOPE_IDENTITY()
	
	-- ��ѯ����
	SELECT * FROM RobotConfigure WHERE BatchID=@dwBatchID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- �޸Ĳ���
CREATE PROC GSP_GP_RobotModifyParameter
	@dwDatchID INT,								-- ���α�ʶ
	@dwServiceMode INT,							-- ����ģʽ
	@dwRobotCount INT,						-- ������Ŀ
	@dwEnterTime INT,							-- ����ʱ��
	@dwLeaveTime INT,							-- �뿪ʱ��
	@dwEnterMinInterval INT,					-- ������
	@dwEnterMaxInterval INT,					-- ������
	@dwLeaveMinInterval	INT,					-- �뿪���
	@dwLeaveMaxInterval	INT,					-- �뿪���
	@lTakeMinScore	BIGINT,						-- Я������
	@lTakeMaxScore	BIGINT,						-- Я������
	@dwSwitchMinInnings INT,					-- ��������
	@dwSwitchMaxInnings INT						-- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ���²���
	UPDATE RobotConfigure SET ServiceMode=@dwServiceMode,RobotCount=@dwRobotCount,EnterTime=@dwEnterTime,LeaveTime=@dwLeaveTime,
		EnterMinInterval=@dwEnterMinInterval,EnterMaxInterval=@dwEnterMaxInterval,LeaveMinInterval=@dwLeaveMinInterval,
		LeaveMaxInterval=@dwLeaveMaxInterval,TakeMinScore=@lTakeMinScore,TakeMaxScore=@lTakeMaxScore,SwitchMinInnings=@dwSwitchMinInnings,
		SwitchMaxInnings=@dwSwitchMaxInnings
	WHERE BatchID=@dwDatchID
	
	-- ��ѯ����
	SELECT * FROM RobotConfigure WHERE BatchID=@dwDatchID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ɾ������
CREATE PROC GSP_GP_RobotDeleteParameter
	@dwBatchID INT								-- ���α�ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	SELECT * FROM RobotConfigure WHERE BatchID=@dwBatchID

	-- ɾ������
	DELETE RobotConfigure WHERE BatchID=@dwBatchID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
