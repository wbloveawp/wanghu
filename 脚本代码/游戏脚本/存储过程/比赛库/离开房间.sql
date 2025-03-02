
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

-- 离开房间
CREATE PROC GSP_GR_LeaveGameServer
	@dwUserID INT,								-- 用户 I D
	@lScore BIGINT,								-- 用户分数
	@lRevenue BIGINT,							-- 游戏税收
	@lWinCount INT,								-- 胜利盘数
	@lLostCount INT,							-- 失败盘数
	@lDrawCount INT,							-- 和局盘数
	@lFleeCount INT,							-- 断线数目
	@lWinStreak INT,							-- 连胜盘数
	@lMaxWinStreak INT,							-- 连胜盘数
	@lExperience INT,							-- 用户经验
	@lUnLockScore BIGINT,						-- 释放积分
	@dwPlayTimeCount INT,						-- 游戏时间
	@dwOnLineTimeCount INT,						-- 在线时间

	-- 记录成绩
	@lRecordScore BIGINT,						-- 用户分数
	@lRecordRevenue BIGINT,						-- 游戏税收
	@lRecordWinCount INT,						-- 胜利盘数
	@lRecordLostCount INT,						-- 失败盘数
	@lRecordDrawCount INT,						-- 和局盘数
	@lRecordFleeCount INT,						-- 断线数目
	@lRecordExperience INT,						-- 用户经验	
	@dwRecordPlayTimeCount INT,					-- 游戏时间

	@strClientIP NVARCHAR(15),					-- 连接地址
	@wKindID INT,								-- 游戏 I D
	@wServerID INT,								-- 房间 I D
	@szMachineID NVARCHAR(32),					-- 机器 I D
	@cbLeaveReason INT,							-- 离开原因
	@dwInoutIndex INT							-- 进出索引	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 校验原因
	IF @cbLeaveReason<>7
	BEGIN
		-- 计算日期
		DECLARE @DateID INT
		SET @DateID = CAST(CAST(GETDATE() AS FLOAT) AS INT)

		-- 解锁机器人
		IF @strClientIP=N'0.0.0.0'
		BEGIN
			UPDATE LHAccountsDB.dbo.RobotLockInfo SET RobotStatus=0,ServerID=0,BatchID=0,LockDateTime=GETDATE() WHERE UserID=@dwUserID
		END

		-- 用户积分
		UPDATE GameScoreInfo SET PlayTimeCount=PlayTimeCount+@dwPlayTimeCount, OnLineTimeCount=OnLineTimeCount+@dwOnLineTimeCount WHERE UserID=@dwUserID

		-- 用户经验
		UPDATE LHAccountsDB.dbo.AccountsInfo SET Experience+=@lExperience,PlayTimeCount+=@dwPlayTimeCount,OnlineTimeCount+=@dwOnLineTimeCount WHERE UserID=@dwUserID

		-- 解除锁定
		UPDATE LHAccountsDB.dbo.AccountsOnlineInfo SET KindID=0,ServerID=0,ServerKind=0,ModifyTime=GETDATE() WHERE UserID=@dwUserID		

		-- 经验统计
		IF @lRecordExperience>0
		BEGIN
			UPDATE [LHGameRecordDB].dbo.ReportDayUser SET Experience+=@lRecordExperience WHERE DateID=@DateID AND UserID=@dwUserID
			IF @@ROWCOUNT=0
			BEGIN
				INSERT INTO [LHGameRecordDB].dbo.ReportDayUser(DateID,UserID,Experience,ReportDateTime) VALUES(@DateID,@dwUserID,@lRecordExperience,GETDATE())
			END
		END
	END
	
	-- 更新进出记录
	UPDATE RecordUserInout SET LeaveTime=GETDATE(),LeaveReason=@cbLeaveReason,LeaveMachine=@szMachineID,LeaveClientIP=@strClientIP,Score=@lRecordScore,
		WinCount=@lRecordWinCount,LostCount=@lRecordLostCount,DrawCount=@lRecordDrawCount,FleeCount=@lRecordFleeCount,Experience=@lRecordExperience,
		PlayTimeCount=@dwRecordPlayTimeCount,OnLineTimeCount=@dwOnLineTimeCount
	WHERE ID=@dwInoutIndex

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
