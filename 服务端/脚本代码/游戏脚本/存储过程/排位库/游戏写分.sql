----------------------------------------------------------------------------------------------------

USE LHGameRankingDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_WriteGameScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_WriteGameScore]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 游戏写分
CREATE PROC GSP_GR_WriteGameScore
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
	@strClientIP NVARCHAR(15),					-- 连接地址
	@wKindID INT,								-- 游戏 I D
	@wServerID INT								-- 房间 I D	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 自动提交
SET XACT_ABORT ON

-- 用户分数
DECLARE @WinScore BIGINT
DECLARE @LostScore BIGINT

-- 执行逻辑
BEGIN

	-- 写分日统计
	DECLARE @DateID INT
	SET @DateID = CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN	

	-- 查询分数
	DECLARE @UserScore INT
	SELECT @UserScore=Score+@lScore FROM GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 段位定义
	DECLARE @MainOrder SMALLINT
	DECLARE @SubOrder SMALLINT
	DECLARE @StarOrder SMALLINT

	-- 查询段位
	SELECT @MainOrder=MainOrder,@SubOrder=SubOrder,@StarOrder=StarOrder FROM LHPlatformDB.dbo.GameRankLevel(NOLOCK) WHERE @UserScore>=NeedScore AND @UserScore<NeedScore+SpanScore

	-- 用户积分
	UPDATE GameScoreInfo SET Score+=@lScore, WinCount+=@lWinCount, LostCount+=@lLostCount, DrawCount+=@lDrawCount, FleeCount+=@lFleeCount, WinStreak = @lWinStreak, MaxWinStreak=@lMaxWinStreak,
		   MainOrder=@MainOrder, SubOrder=@SubOrder, StarOrder=@StarOrder	
	WHERE UserID=@dwUserID

	-- 用户经验
	UPDATE LHAccountsDB.dbo.AccountsInfo SET Experience+=@lExperience,PlayTimeCount+=@dwPlayTimeCount WHERE UserID=@dwUserID	

	-- 初始化
	SELECT @LostScore=0, @WinScore=0

	-- 设置分数
	IF @lScore>=0  
		SET @WinScore=@lScore		
	ELSE 
		SET @LostScore=@lScore	

	-- 分数统计
	UPDATE ReportUserScore SET GainScore+=@WinScore,LoseScore+=@LostScore,WinCount+=@lWinCount,LostCount+=@lLostCount,DrawCount+=@lDrawCount,FleeCount+=@lFleeCount,UserRevenue+=@lRevenue,
		PlayTimeCount+=@dwPlayTimeCount,ModifyDateTime=GETDATE() 
	WHERE DateID=@DateID AND UserID=@dwUserID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO ReportUserScore(DateID,UserID,GainScore,LoseScore,WinCount,LostCount,DrawCount,FleeCount,UserRevenue,PlayTimeCount,InsertDateTime,ModifyDateTime)
		VALUES (@DateID,@dwUserID,@WinScore,@LostScore,@lWinCount,@lLostCount,@lDrawCount,@lFleeCount,@lRevenue,@dwPlayTimeCount,GETDATE(),GETDATE())
	END

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
