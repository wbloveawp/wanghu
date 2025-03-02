
----------------------------------------------------------------------------------------------------

USE LHGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchStart]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchStart]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchEliminate]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchEliminate]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchFinish]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchFinish]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MR_MatchFinish]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MR_MatchFinish]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchComplete]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchComplete]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchCancel]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchCancel]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchGiveup]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchGiveup]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchRoundSwitch]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchRoundSwitch]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- 比赛开始
CREATE PROC GSP_GR_MatchStart
	@wServerID		INT,		-- 房间标识		
	@dwMatchID		INT,		-- 比赛标识
	@lMatchNo		BIGINT,		-- 比赛场次
	@cbMatchType	TINYINT		-- 比赛类型
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN 

	-- 更新数据
	UPDATE MatchScoreInfo SET RankID=0,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0,WinStreak=0  WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo				
END

RETURN 0
	
GO

----------------------------------------------------------------------------------------------------

-- 比赛淘汰
CREATE PROC GSP_GR_MatchEliminate
	@dwUserID		INT,		-- 用户标识
	@dwMatchID		INT,		-- 比赛标识
	@lMatchNo		BIGINT,		-- 比赛场次
	@cbMatchType	TINYINT,	-- 比赛类型
	@wRankID		SMALLINT	-- 比赛名次
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 更新名次
	UPDATE MatchScoreInfo SET RankID=@wRankID,MatchRight=0 WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo	
	
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
-- 比赛结束
CREATE PROC GSP_GR_MatchFinish
	@wServerID		INT,		-- 房间标识	
	@dwMatchID		INT,		-- 比赛标识
	@lMatchNo		BIGINT,		-- 比赛场次
	@cbMatchType	TINYINT,	-- 比赛类型
	@wPlayCount		SMALLINT,	-- 游戏局数	
	@MatchStartTime DATETIME,	-- 开赛时间
	@MatchFinishTime DATETIME	-- 结束时间		
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 插入记录
	INSERT INTO StreamMatchHistory(UserID,GameID,MatchID,MatchNo,MatchType,ServerID,RankID,MatchScore,WinCount,LostCount,DrawCount,FleeCount,WinStreak,MaxWinStreak,MatchStartTime,MatchFinishTime,PlayTimeCount)
	SELECT UserID,GameID,MatchID,MatchNo,@cbMatchType,@wServerID,0,Score, WinCount,LostCount,DrawCount,FleeCount,WinStreak,MaxWinStreak,@MatchStartTime,@MatchFinishTime,PlayTimeCount FROM MatchScoreInfo(NOLOCK)  
	WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo AND ServerID=@wServerID AND SignupCount>0

	-- 删除比赛分
	IF @@ROWCOUNT>0 AND @cbMatchType<>0x04
	BEGIN
		DELETE MatchScoreInfo WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo
	END	
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
-- 比赛结束
CREATE PROC GSP_MR_MatchFinish
	@dwMatchID		INT,		 -- 比赛标识
	@lMatchNo		BIGINT,		 -- 比赛场次
	@cbMatchType	SMALLINT,	 -- 比赛类型
	@MatchStartTime DATETIME,	 -- 开赛时间
	@MatchFinishTime DATETIME,	 -- 结束时间
	@wRewardUsers   SMALLINT,	 -- 奖励人数
	@strMatchReward NVARCHAR(MAX)-- 比赛奖励
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

SET XACT_ABORT ON

-- 执行逻辑
BEGIN
	
	-- 查询比赛
	DECLARE @MatchName NVARCHAR(64)
	SELECT @MatchName=MatchName FROM MatchOption(NOLOCK) WHERE MatchID=@dwMatchID

	-- 校验比赛
	IF @MatchName IS NULL OR @cbMatchType<>0x04
	BEGIN
		RETURN 1
	END

	-- 比赛排行
	DECLARE @MatchRanking TABLE (
		ID INT identity(1,1),
		UserID INT, 
		RankID INT, 
		Score BIGINT, 
		MailID INT, 
		RewardEntity NVARCHAR(32),
		RewardGoods NVARCHAR(256))
	
	-- 分数排序
	INSERT INTO @MatchRanking(UserID, RankID, Score, MailID, RewardEntity, RewardGoods)
	--SELECT TOP(@wRewardUsers) UserID, ROW_NUMBER() OVER(ORDER BY Score DESC, WinCount DESC, MaxWinStreak DESC, SignupCount DESC, UserID) AS RankID, Score, 0, N'',N'' 
	SELECT TOP(@wRewardUsers) UserID, ROW_NUMBER() OVER(ORDER BY Score DESC, WinCount DESC, SignupCount DESC, UserID) AS RankID, Score, 0, N'',N'' 
	FROM  MatchScoreInfo(NOLOCK) WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo AND SignupCount>0
	ORDER BY Score DESC, WinCount DESC, SignupCount DESC, UserID

	-- 统计信息
	DECLARE @AvgMatchTimes INT
	DECLARE @MatchUserCount INT
	DECLARE @SingupUserCount INT	
	DECLARE @TotalMatchTimes INT	
	DECLARE @ConsumeIngot BIGINT	

	-- 统计数据
	SELECT @MatchUserCount=COUNT(1),@TotalMatchTimes=SUM(SignupCount),@AvgMatchTimes=AVG(SignupCount) FROM  MatchScoreInfo(NOLOCK) 
	WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo AND SignupCount>0

	-- 报名人数
	SELECT @SingupUserCount=COUNT(1) FROM StreamMatchTicket WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo

	-- 钻石消耗
	SELECT @ConsumeIngot=SUM(BuyAmount*BuyTimes) FROM StreamMatchTicket(NOLOCK) WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo

	-- 奖励配置
	DECLARE @MatchReward TABLE (ID INT identity(1,1), StartID INT, EndID INT, Entity NVARCHAR(64), GoodsList NVARCHAR(512))

	-- 构造奖励
	IF ISJSON(@strMatchReward)>0
	BEGIN
		INSERT INTO @MatchReward(StartID, EndID, Entity, GoodsList)
		SELECT JSON_VALUE([VALUE],N'$.StartID'),JSON_VALUE([VALUE],N'$.EndID'),ISNULL(JSON_QUERY([VALUE],N'$.Entity'),''),ISNULL(JSON_QUERY([VALUE],N'$.GoodsList'),'') FROM OPENJSON(@strMatchReward)
	END	

	-- 更新奖励
	UPDATE @MatchRanking SET RewardEntity=b.Entity,RewardGoods=b.GoodsList FROM @MatchRanking a, @MatchReward b WHERE a.RankID>=b.StartID AND a.RankID<=b.EndID	

	-- 变量定义
	DECLARE @Index INT
	DECLARE @LoopCount INT		

	-- 奖励配置
	DECLARE @RankID INT
	DECLARE @EndID INT
	DECLARE @StartID INT	
	DECLARE @Entity NVARCHAR(64)
	DECLARE @GoodsList NVARCHAR(512)

	-- 时间定义
	DECLARE @MailID INT
	DECLARE @SendTime INT
	DECLARE @ExpireTime INT

	-- 邮件信息
	DECLARE @MatchRank NVARCHAR(32)
	DECLARE @MailTitle NVARCHAR(64)
	DECLARE @MailContent NVARCHAR(512)
	DECLARE @RewardEntity NVARCHAR(128)

	-- 计算时间
	SET @SendTime=LHAccountsDB.dbo.LHF_GetTimestamp()
	SET @ExpireTime=@SendTime+7*24*3600

	-- 设置变量
	SELECT @Index=1,@LoopCount=COUNT(1) FROM @MatchRanking
	
	-- 循环处理
	WHILE @Index<=@LoopCount
	BEGIN
		-- 查询奖励
		SELECT @RankID=RankID,@RewardEntity=RewardEntity FROM @MatchRanking WHERE ID=@Index		

		-- 游戏标题
		SET @MailTitle=N'Tournament Reward'

		-- 比赛名次
		SET @MatchRank = CASE @RankID 
						WHEN 1 THEN N'Championship'
						WHEN 2 THEN N'Second place'
						WHEN 3 THEN N'Third place'										
						ELSE
							CONVERT(NVARCHAR(4),@RankID)+N'th'
						END

		-- 邮件内容
		SET @MailContent=CONCAT('Congratulations!You got the ',@MatchRank,' in the ',@MatchName,' Tournament at ',CONVERT(NVARCHAR(24),@MatchStartTime,120),'.')
		
		-- 实物奖励
		IF ISJSON(@RewardEntity)>0
		BEGIN
			SET @MailContent=CONCAT(@MailContent,'\r\nNote：You won the reward in kind, please contact our customer service.')
		END

		-- 生成邮件
		INSERT INTO LHAccountsDB.dbo.AccountsMailInfo(MailKind,MailType,MailTitle,MailContent,SendTime,ExpireTime,CollectTime)
		VALUES (2,2,@MailTitle,@MailContent,@SendTime,@ExpireTime,GetDate())

		-- 邮件标识
		SET @MailID=SCOPE_IDENTITY()		

		-- 设置邮件
		UPDATE @MatchRanking SET MailID=@MailID WHERE ID=@Index

		SET @Index+=1
	END		

	-- 添加附件
	INSERT INTO LHAccountsDB.dbo.AccountsMailAttach(MailID,GoodsID,GoodsLevel,GoodsCount,GoodsIndate)
	SELECT a.MailID, b.GoodsID, ISNULL(b.[Level],0),ISNULL(b.[Count],0),ISNULL(b.Indate,0) 
	FROM  @MatchRanking a CROSS APPLY OPENJSON(a.RewardGoods) 	WITH (GoodsID INT,[Level] INT,[Count] INT, Indate INT) AS b	
	WHERE ISJSON(a.RewardGoods)>0	

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 插入场次
	IF @MatchUserCount>0
	BEGIN
		INSERT INTO StreamMatchSession(MatchID,MatchNo,MatchUserCount,SingupUserCount,AvgMatchTimes,TotalMatchTimes,ConsumeIngot,MatchStartTime,MatchFinishTime,CollectTime)
		VALUES (@dwMatchID,@lMatchNo,@MatchUserCount,@SingupUserCount,@AvgMatchTimes,@TotalMatchTimes,@ConsumeIngot,@MatchStartTime,@MatchFinishTime,GETDATE())
	END

	-- 插入邮箱
	INSERT INTO LHAccountsDB.dbo.AccountsMailBox(UserID,MailID,MailState,ExpireTime,UpdateTime)
	SELECT UserID,MailID,1,@ExpireTime,GETDATE() FROM @MatchRanking

	-- 更新名次
	UPDATE StreamMatchHistory SET RankID=b.RankID,RewardEntity=ISNULL(b.RewardEntity,N''),RewardGoods=ISNULL(b.RewardGoods,N'') FROM StreamMatchHistory a, @MatchRanking b 
	WHERE a.MatchID=@dwMatchID AND a.MatchNo=@lMatchNo AND a.UserID=b.UserID

	-- 删除比赛分
	DELETE MatchScoreInfo WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 抛出数据
	SELECT UserID,MailID FROM @MatchRanking
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- 比赛完成
CREATE PROC GSP_GR_MatchComplete
	@wServerID		INT,			-- 房间标识	
	@dwMatchID		INT,			-- 比赛标识
	@lMatchNo		BIGINT,			-- 比赛场次
	@strUserList	NVARCHAR(256)	-- 用户列表
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 更新权限
	IF ISJSON(@strUserList)>0
	BEGIN
		DECLARE @MatchRight SMALLINT = 0x40
		UPDATE MatchScoreInfo SET MatchRight&=~@MatchRight WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo AND UserID IN (SELECT [VALUE] FROM OPENJSON(@strUserList))
	END	
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
-- 比赛取消
CREATE PROC GSP_GR_MatchCancel
	@wServerID		INT,		-- 房间标识		
	@dwMatchID		INT,		-- 比赛标识
	@lMatchNo		BIGINT		-- 比赛场次	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN 

	-- 删除比赛分
	DELETE MatchScoreInfo WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo	
END

RETURN 0
	
GO

----------------------------------------------------------------------------------------------------
-- 比赛放弃
CREATE PROC GSP_GR_MatchGiveup
	@dwUserID		INT,		-- 用户标识
	@dwMatchID		INT,		-- 比赛标识
	@lMatchNo		BIGINT		-- 比赛场次	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN 

	-- 删除比赛分
	DELETE MatchScoreInfo WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo	
END

RETURN 0
	
GO

----------------------------------------------------------------------------------------------------
-- 轮次切换
CREATE PROC GSP_GR_MatchRoundSwitch	
	@dwMatchID		INT,		-- 比赛标识
	@lMatchNo		BIGINT,		-- 比赛场次
	@wReserveRatio  SMALLINT,	-- 保留比例
	@cbMScoreSetType TINYINT,	-- 定分方式
	@lInitMatchScore  BIGINT	-- 初始分数
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN 

	-- 重置分数
	IF @cbMScoreSetType=0
	BEGIN
		UPDATE MatchScoreInfo SET Score=@lInitMatchScore,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0 WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo	
	END	

	-- 重置分数
	IF @cbMScoreSetType=1
	BEGIN
		UPDATE MatchScoreInfo SET Score=CONVERT(INT,Score/100.0*@wReserveRatio),WinCount=0,LostCount=0,DrawCount=0,FleeCount=0 WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo	
	END
	
END

RETURN 0
	
GO

----------------------------------------------------------------------------------------------------
