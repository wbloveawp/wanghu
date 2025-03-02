
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

-- 读取参数
CREATE PROC GSP_GP_RobotQueryParameter
	@wServerID INT								-- 房间标识	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询参数
	SELECT * FROM RobotConfigure WHERE ServerID=@wServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 添加参数
CREATE PROC GSP_GP_RobotAppendParameter
	@wServerID INT,								-- 房间标识
	@dwServiceMode INT,							-- 服务模式
	@dwRobotCount INT,							-- 机器数目
	@dwEnterTime INT,							-- 进入时间
	@dwLeaveTime INT,							-- 离开时间
	@dwEnterMinInterval INT,					-- 进入间隔
	@dwEnterMaxInterval INT,					-- 进入间隔
	@dwLeaveMinInterval	INT,					-- 离开间隔
	@dwLeaveMaxInterval	INT,					-- 离开间隔
	@lTakeMinScore	BIGINT,						-- 携带分数
	@lTakeMaxScore BIGINT,						-- 携带分数
	@dwSwitchMinInnings INT,					-- 换桌局数
	@dwSwitchMaxInnings INT						-- 换桌局数
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 插入数据
	INSERT RobotConfigure(ServerID,ServiceMode,RobotCount,EnterTime,LeaveTime,EnterMinInterval,EnterMaxInterval,LeaveMinInterval,
			LeaveMaxInterval,TakeMinScore,TakeMaxScore,SwitchMinInnings,SwitchMaxInnings)
	VALUES(@wServerID,@dwServiceMode,@dwRobotCount,@dwEnterTime,@dwLeaveTime,@dwEnterMinInterval,@dwEnterMaxInterval,@dwLeaveMinInterval,
			@dwLeaveMaxInterval,@lTakeMinScore,@lTakeMaxScore,@dwSwitchMinInnings,@dwSwitchMaxInnings)

	-- 查询批次	
	DECLARE @dwBatchID INT
	SET @dwBatchID=SCOPE_IDENTITY()
	
	-- 查询数据
	SELECT * FROM RobotConfigure WHERE BatchID=@dwBatchID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- 修改参数
CREATE PROC GSP_GP_RobotModifyParameter
	@dwDatchID INT,								-- 批次标识
	@dwServiceMode INT,							-- 服务模式
	@dwRobotCount INT,						-- 机器数目
	@dwEnterTime INT,							-- 进入时间
	@dwLeaveTime INT,							-- 离开时间
	@dwEnterMinInterval INT,					-- 进入间隔
	@dwEnterMaxInterval INT,					-- 进入间隔
	@dwLeaveMinInterval	INT,					-- 离开间隔
	@dwLeaveMaxInterval	INT,					-- 离开间隔
	@lTakeMinScore	BIGINT,						-- 携带分数
	@lTakeMaxScore	BIGINT,						-- 携带分数
	@dwSwitchMinInnings INT,					-- 换桌局数
	@dwSwitchMaxInnings INT						-- 换桌局数
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 更新参数
	UPDATE RobotConfigure SET ServiceMode=@dwServiceMode,RobotCount=@dwRobotCount,EnterTime=@dwEnterTime,LeaveTime=@dwLeaveTime,
		EnterMinInterval=@dwEnterMinInterval,EnterMaxInterval=@dwEnterMaxInterval,LeaveMinInterval=@dwLeaveMinInterval,
		LeaveMaxInterval=@dwLeaveMaxInterval,TakeMinScore=@lTakeMinScore,TakeMaxScore=@lTakeMaxScore,SwitchMinInnings=@dwSwitchMinInnings,
		SwitchMaxInnings=@dwSwitchMaxInnings
	WHERE BatchID=@dwDatchID
	
	-- 查询数据
	SELECT * FROM RobotConfigure WHERE BatchID=@dwDatchID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 删除参数
CREATE PROC GSP_GP_RobotDeleteParameter
	@dwBatchID INT								-- 批次标识	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询参数
	SELECT * FROM RobotConfigure WHERE BatchID=@dwBatchID

	-- 删除参数
	DELETE RobotConfigure WHERE BatchID=@dwBatchID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
