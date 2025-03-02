USE LHGameGoldDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadUserControl]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadUserControl]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserControlStart]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserControlStart]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserControlFinish]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserControlFinish]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserControlUpdate]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserControlUpdate]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- 加载控制
CREATE PROC GSP_GR_LoadUserControl
	@dwUserID INT								-- 用户标识	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 查询记录
	IF NOT EXISTS(SELECT 1 FROM GameControlInfo(NOLOCK) WHERE UserID=@dwUserID)
	BEGIN
		RETURN 1
	END

	-- 抛出记录
	SELECT * FROM GameControlInfo(NOLOCK) WHERE UserID=@dwUserID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 启动控制
CREATE PROC GSP_GR_UserControlStart
	@dwUserID INT,								-- 用户标识
	@cbControlMode TINYINT,						-- 控制模式
	@cbControlKind TINYINT,						-- 控制方式
	@cbControlStatus TINYINT,					-- 控制状态	
	@wTargetWinRate SMALLINT,					-- 目标胜率					
	@wControlWinRate SMALLINT,					-- 控制胜率
	@wInitailWinRate SMALLINT,					-- 当前胜率	
	@lInitailWinScore BIGINT,					-- 累计赢分
	@lInitailLoseScore BIGINT					-- 累计输分
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 删除记录
	DELETE GameControlInfo WHERE UserID=@dwUserID

	-- 插入记录
	INSERT INTO GameControlInfo(UserID,ControlKind,ControlMode,ControlStatus,TargetWinRate,ControlWinRate,InitailWinRate,InitailWinScore,InitailLoseScore,ControlStartTime)
	VALUES (@dwUserID,@cbControlKind,@cbControlMode,@cbControlStatus,@wTargetWinRate,@wControlWinRate,@wInitailWinRate,@lInitailWinScore,@lInitailLoseScore,GetDate())	
	
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 结束控制
CREATE PROC GSP_GR_UserControlFinish
	@dwUserID INT,								-- 用户标识
	@lTotalWinCount INT,						-- 赢局数量
	@lTotalLoseCount INT,						-- 输局数量
	@lTotalWinScore BIGINT,						-- 累计赢分
	@lTotalLoseScore BIGINT,					-- 累计输分	
	@lTotalBetAmount BIGINT = 0					-- 累计打码	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 更新数据
	UPDATE GameControlInfo SET TotalWinCount=@lTotalWinCount,TotalLoseCount=@lTotalLoseCount,TotalWinScore=@lTotalWinScore, TotalLoseScore=@lTotalLoseScore,TotalBetAmount=@lTotalBetAmount 
	WHERE UserID=@dwUserID

	INSERT INTO RecordGameControl(UserID,ControlKind,ControlMode,ControlStatus,TargetWinRate,ControlWinRate,InitailWinRate,InitailWinScore,InitailLoseScore,TotalWinCount,
		TotalLoseCount,TotalWinScore,TotalLoseScore,TotalBetAmount,ControlDuration,ControlPlayCount,ControlStartTime,ControlFinishTime,OperaterUserID,OperateRemark)
	SELECT UserID,ControlKind,ControlMode,ControlStatus,TargetWinRate,ControlWinRate,InitailWinRate,InitailWinScore,InitailLoseScore,TotalWinCount,
		TotalLoseCount,TotalWinScore,TotalLoseScore,TotalBetAmount,ControlDuration,ControlPlayCount,ControlStartTime,GETDATE(),OperaterMasterID,OperateRemark  FROM GameControlInfo 
	WHERE UserID=@dwUserID

	-- 删除控制
	DELETE GameControlInfo WHERE UserID=@dwUserID		
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 控制更新
CREATE PROC GSP_GR_UserControlUpdate
	@dwUserID INT,								-- 用户标识
	@lTotalWinCount INT,						-- 赢局数量
	@lTotalLoseCount INT,						-- 输局数量
	@lTotalWinScore BIGINT,						-- 累计赢分
	@lTotalLoseScore BIGINT,					-- 累计输分	
	@lTotalBetAmount BIGINT						-- 累计打码	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 更新数据
	UPDATE GameControlInfo SET TotalWinCount=@lTotalWinCount,TotalLoseCount=@lTotalLoseCount,TotalWinScore=@lTotalWinScore, TotalLoseScore=@lTotalLoseScore,TotalBetAmount=@lTotalBetAmount 
	WHERE UserID=@dwUserID

	-- 查询信息
	DECLARE @WinCount SMALLINT
	DECLARE @LoseCount SMALLINT
	DECLARE @ControlKind SMALLINT
	DECLARE @ControlStatus SMALLINT	
	SELECT @ControlStatus=ControlStatus,@ControlKind=ControlKind,@WinCount=TotalWinCount,@LoseCount=TotalLoseCount FROM GameControlInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 调整状态
	IF @ControlKind IS NULL SET @ControlKind=0xFF
	IF @ControlStatus IS NULL SET @ControlStatus=0

	-- 新手保护类型
	IF @ControlKind=3 AND @ControlStatus<>0
	BEGIN

		------------------------------------自定义控制退出判断------------------------------------------
		-- 参数定义
		DECLARE @MaxPlayCount INT = 50
		DECLARE @MinPayAmount DECIMAL(18,2) = 1
				
		--游戏局数
		DECLARE @PlayCount INT
		SELECT @PlayCount=@WinCount+@LoseCount
		
		-- 校验局数
		IF @PlayCount<@MaxPlayCount
		BEGIN
			-- 充值信息
			DECLARE @PayAmount DECIMAL(18,2)=0
			SELECT @PayAmount=PayAmount FROM LHInsureDB.dbo.AccountsPay(NOLOCK) WHERE UserID=@dwUserID

			-- 校验充值金额
			IF @PayAmount>=@MinPayAmount
			BEGIN
				SET @ControlStatus=0
			END				

		END ELSE
		BEGIN
			SET @ControlStatus=0
		END
		
		------------------------------------自定义控制退出判断------------------------------------------

		-- 控制结束
		IF @ControlStatus=0
		BEGIN
			-- 控制记录
			INSERT INTO RecordGameControl(UserID,ControlKind,ControlMode,ControlStatus,TargetWinRate,ControlWinRate,InitailWinRate,InitailWinScore,InitailLoseScore,TotalWinCount,
				TotalLoseCount,TotalWinScore,TotalLoseScore,TotalBetAmount,ControlDuration,ControlPlayCount,ControlStartTime,ControlFinishTime,OperaterUserID,OperateRemark)
			SELECT UserID,ControlKind,ControlMode,ControlStatus,TargetWinRate,ControlWinRate,InitailWinRate,InitailWinScore,InitailLoseScore,TotalWinCount,
				TotalLoseCount,TotalWinScore,TotalLoseScore,TotalBetAmount,ControlDuration,ControlPlayCount,ControlStartTime,GETDATE(),OperaterMasterID,OperateRemark  FROM GameControlInfo 
			WHERE UserID=@dwUserID

			-- 删除控制
			DELETE GameControlInfo WHERE UserID=@dwUserID
		END

	END

	-- 抛出状态
	SELECT @ControlStatus AS ControlStatus
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
