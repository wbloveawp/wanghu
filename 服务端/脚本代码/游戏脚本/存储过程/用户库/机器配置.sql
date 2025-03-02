
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'dbo.GSP_GR_LoadRobotConfigure') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE dbo.GSP_GR_LoadRobotConfigure
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ���ػ�������
CREATE PROC GSP_GR_LoadRobotConfigure
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT							-- ���� I D
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	SELECT BatchID,ServiceMode,RobotCount,EnterTime,LeaveTime,EnterMinInterval,EnterMaxInterval,LeaveMinInterval,
		LeaveMaxInterval,TakeMinScore,TakeMaxScore,SwitchMinInnings,SwitchMaxInnings FROM Robotconfigure
	WHERE ServerID=@wServerID ORDER BY BatchID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
