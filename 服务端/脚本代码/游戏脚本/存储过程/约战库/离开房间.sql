
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

	-- 服务信息
	@strClientIP NVARCHAR(15),					-- 连接地址
	@wKindID INT,								-- 游戏 I D
	@wServerID INT,								-- 房间 I D
	@szComputerID NVARCHAR(32),					-- 机器序列
	@cbLeaveReason INT,							-- 离开原因
	@dwInoutIndex INT							-- 进出索引

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 变量定义
	DECLARE @ClubID INT
	DECLARE @BattleNum INT
	DECLARE @ScoreKind TINYINT

	-- 查询约战信息
	SELECT @ClubID=ClubID,@BattleNum=BattleNum,@ScoreKind=ScoreKind FROM RecordUserInout(NOLOCK) WHERE ID=@dwInoutIndex

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN	

	-- 茶馆分
	IF @ScoreKind=2
	BEGIN
		-- 变量定义
		DECLARE	@TableID INT								
		DECLARE @Sqlstr		NVARCHAR(512)						
		DECLARE	@TableName	NVARCHAR(128)

		-- 分表标识				
		SET	@TableID=dbo.LHF_GetTableID(@ClubID);		

		-- 群组成员表
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));			

		-- 查询脚本		
		SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName,' SET ClubScore=ClubScore+',CONVERT(NVARCHAR(18), @lScore),',RevenueCount=RevenueCount+',CONVERT(NVARCHAR(18), @lRevenue),
							  ' WHERE MemberID=',CONVERT(NVARCHAR(18), @dwUserID),' AND ClubID=',CONVERT(NVARCHAR(18), @ClubID))		

		-- 执行脚本
		EXECUTE(@Sqlstr)	
		IF @@ERROR<>0
		BEGIN
			-- 事务回滚
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
			RETURN 1
		END		
		
		-- 积分信息
		UPDATE GameScoreInfo SET WinCount+=@lWinCount,LostCount+=@lLostCount, DrawCount+=@lDrawCount, FleeCount+=@lFleeCount, PlayTimeCount+=@dwPlayTimeCount,OnLineTimeCount+=@dwOnLineTimeCount,
			   WinStreak=@lWinStreak, MaxWinStreak=@lMaxWinStreak 
		WHERE UserID=@dwUserID
	END	

	-- 约战积分
	IF @ScoreKind=3
	BEGIN
		UPDATE GameScoreInfo SET Score+=@lScore,WinCount+=@lWinCount,LostCount+=@lLostCount, DrawCount+=@lDrawCount, FleeCount+=@lFleeCount, PlayTimeCount+=@dwPlayTimeCount,OnLineTimeCount+=@dwOnLineTimeCount,
			   WinStreak=@lWinStreak, MaxWinStreak=@lMaxWinStreak 
		WHERE UserID=@dwUserID
	END	

	-- 更新锁表
	UPDATE BattleUserLocker SET KindID=0,ServerID=0,OnlineStatus=0 WHERE UserID=@dwUserID

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 解除锁定
	UPDATE LHAccountsDB.dbo.AccountsOnlineInfo SET KindID=0,ServerID=0,ServerKind=0,ModifyTime=GETDATE() WHERE UserID=@dwUserID	

	-- 用户经验
	IF @lExperience>0
	BEGIN
		UPDATE LHAccountsDB.dbo.AccountsInfo SET Experience=Experience+@lExperience WHERE UserID=@dwUserID
	END

	-- 计算日期
	DECLARE @DateID INT
	SET @DateID = CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 写分日统计
	UPDATE StreamBattleKindInfo SET WinCount=WinCount+@lWinCount,LostCount=LostCount+@lLostCount,PlayTimeCount=PlayTimeCount+@dwPlayTimeCount
	FROM StreamBattleKindInfo WHERE DateID=@DateID AND UserID=@dwUserID AND KindID=@wKindID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO StreamBattleKindInfo(DateID,UserID,KindID,WinCount,LostCount,PlayTimeCount) 
		VALUES(@DateID,@dwUserID,@wKindID,@lWinCount,@lLostCount,@dwPlayTimeCount)
	END		

	-- 更新进出记录
	UPDATE RecordUserInout SET LeaveTime=Getdate(),LeaveReason=@cbLeaveReason,LeaveMachine=@szComputerID,LeaveClientIP=@strClientIP,Score=@lRecordScore,Revenue=@lRecordRevenue,
		WinCount=@lRecordWinCount,LostCount=@lRecordLostCount,DrawCount=@lRecordDrawCount,FleeCount=@lRecordFleeCount,Experience=@lRecordExperience,PlayTimeCount=@dwRecordPlayTimeCount,
		OnLineTimeCount=@dwOnLineTimeCount
	WHERE ID=@dwInoutIndex
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------