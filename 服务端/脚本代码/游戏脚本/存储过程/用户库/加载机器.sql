
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

-- 加载机器
CREATE PROC GSP_GR_LoadRobotUser
	@wServerID	SMALLINT,					-- 房间标识
	@dwBatchID	INT,						-- 批次标识
	@dwRobotCount INT						-- 机器数目
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 删除在线表
	DELETE AccountsOnlineInfo WHERE UserID IN (SELECT UserID FROM RobotLockInfo(NOLOCK) WHERE DATEDIFF(hh,LockDateTime,GetDate())>=24 AND RobotStatus<>0)

	-- 解锁机器
	UPDATE RobotLockInfo SET RobotStatus=0,ServerID=0,BatchID=0 WHERE DATEDIFF(hh,LockDateTime,GetDate())>=24

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	BEGIN TRAN		

	-- 查询机器
	SELECT UserID,LogonPass INTO #TempRobots FROM RobotLockInfo(NOLOCK)
	WHERE UserID IN (SELECT TOP (@dwRobotCount) UserID FROM RobotLockInfo(NOLOCK) WHERE RobotStatus=0 AND ServerID=0 AND DATEDIFF(hh,LockDateTime,GetDate())>=12 ORDER BY NEWID()) 

	-- 更新状态
	UPDATE RobotLockInfo SET RobotStatus=1,BatchID=@dwBatchID,ServerID=@wServerID,LockDateTime=GetDate() 
	FROM RobotLockInfo a,#TempRobots b WHERE a.UserID = b.UserID

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- 查询机器
	SELECT * FROM #TempRobots

	-- 销毁临时表	
	IF OBJECT_ID('tempdb..#TempRobots') IS NOT NULL DROP TABLE #TempRobots
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 解锁机器
CREATE PROC GSP_GR_UnLockRobotUser
	@wServerID	SMALLINT					-- 房间标识	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 更新状态
	UPDATE RobotLockInfo SET RobotStatus=0,ServerID=0,BatchID=0,LockDateTime=N'1970-01-01 00:00:00'
	WHERE RobotStatus<>0 AND BatchID<>0 AND ServerID=@wServerID

END

RETURN 0

----------------------------------------------------------------------------------------------------
