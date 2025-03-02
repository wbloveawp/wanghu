
----------------------------------------------------------------------------------------------------

USE LHGameRankingDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_SeansonBalance]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_SeansonBalance]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_InsertRewardMail]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_InsertRewardMail]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 赛季结算
CREATE  PROCEDURE dbo.GSP_GR_SeansonBalance		
	@dwCurrentTime INT,					-- 当前时间
	@dwMailIndate  INT					-- 邮件有效期
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 自动回滚
SET XACT_ABORT ON

-- 执行逻辑
BEGIN
	
	-- 查询配置
	DECLARE @RankingOption NVARCHAR(1024)
	SELECT @RankingOption=OptionValue FROM LHPlatformDB.dbo.SystemOptionInfo(NOLOCK) WHERE OptionName=N'RankingOption'

	-- 校验配置
	IF ISJSON(@RankingOption)=0
	BEGIN
		RETURN 0
	END

	-- 赛季奖励
	DECLARE @SeasonRewards NVARCHAR(MAX)
	SET @SeasonRewards=JSON_QUERY(@RankingOption,N'$.SeasonRewards')

	-- 定义表变量
	DECLARE @TableRewardsOption Table(RankMainOrder SMALLINT, GoodsID SMALLINT, GoodsCount INT, GoodsIndate INT)

	-- 插入数据
	IF ISJSON(@SeasonRewards)>0
	BEGIN
		INSERT INTO @TableRewardsOption(RankMainOrder,GoodsID, GoodsCount,GoodsIndate)
		SELECT JSON_VALUE(a.VALUE,N'$.RankMainOrder'),b.GoodsID,b.GoodsCount,b.GoodsIndate FROM 
		OPENJSON(@SeasonRewards) a 
		CROSS APPLY OPENJSON(a.VALUE,N'$.Rewards')WITH
		(
			GoodsID INT N'$.GoodsID',
			GoodsCount INT N'$.Count',
			GoodsIndate INT N'$.Indate'
		) b
	END                                                                                                                                                                                                                                                                                                                                                                             

	-- 表变量定义
	DECLARE @TableGameRankLevel TABLE (LevelID INT PRIMARY KEY, NeedScore INT,SpanScore INT, MainOrder SMALLINT, SubOrder SMALLINT,StarOrder SMALLINT)

	-- 构建配置表
	INSERT INTO @TableGameRankLevel(LevelID,NeedScore,SpanScore,MainOrder,SubOrder,StarOrder)
	SELECT LevelID,NeedScore,SpanScore,MainOrder,SubOrder,StarOrder FROM LHPlatformDB.dbo.GameRankLevel(NOLOCK)

	-- 构建临时表
	SELECT UserID,Score,MainOrder,SubOrder,StarOrder INTO #UserRankInfo FROM GameScoreInfo(NOLOCK)

	-- 变量定义
	DECLARE @MailID BIGINT
	DECLARE @MaxOrder INT
	DECLARE @MainOrder INT	
	DECLARE @ExpireTime INT

	-- 计算时间
	SET @ExpireTime=@dwCurrentTime+@dwMailIndate

	-- 清除邮件
	TRUNCATE TABLE RecordRankReward

	-- 设置变量
	SELECT @MainOrder=MIN(MainOrder),@MaxOrder=MAX(MainOrder) FROM LHPlatformDB.dbo.GameRankLevel(NOLOCK)

	-- 定义表变量
	DECLARE @TableRewardMailInfo TABLE(
		MainOrder SMALLINT,
		MailID BIGINT, 
		MailKind TINYINT, 
		MailState TINYINT, 
		SendTime INT, 
		ExpireTime INT,
		MailTitle NVARCHAR(64), 		
		MailContent NVARCHAR(256), 
		AttachGoods NVARCHAR(512)
	)

	-- 生成邮件
	WHILE @MainOrder<=@MaxOrder
	BEGIN
		IF EXISTS(SELECT 1 FROM @TableRewardsOption WHERE RankMainOrder=@MainOrder)
		BEGIN
			-- 插入邮件
			INSERT INTO UHWAccountsDb.dbo.AccountsMailInfo(MailKind,MailType,MailTitle,MailContent,SendTime,ExpireTime,CollectTime)
			VALUES (2,3,N'排位赛季奖励',N'赛季奖励内容',@dwCurrentTime,@ExpireTime,GetDate())

			-- 邮件标识
			SET @MailID=SCOPE_IDENTITY()		

			-- 插入附件
			INSERT INTO UHWAccountsDb.dbo.AccountsMailAttach(MailID,GoodsID,GoodsCount,GoodsIndate)
			SELECT @MailID,GoodsID,GoodsCount,0 FROM @TableRewardsOption WHERE RankMainOrder=@MainOrder			

			-- 插入邮件
			INSERT INTO @TableRewardMailInfo(MainOrder,MailID,MailKind,MailState,MailTitle,SendTime,ExpireTime,MailContent,AttachGoods)
			SELECT @MainOrder, MailID, MailKind, 1, MailTitle, SendTime, ExpireTime, MailContent, 
			(SELECT JSON_QUERY(VALUE,N'$.Rewards') FROM OPENJSON(@SeasonRewards) WHERE JSON_VALUE(VALUE,N'$.RankMainOrder')=@MainOrder)
			FROM UHWAccountsDb.dbo.AccountsMailInfo WHERE MailID=@MailID
		END

		-- 等级递增
		SET @MainOrder=@MainOrder+1
	END

	-- 赛季标识
	DECLARE @SeasonID INT
	SET @SeasonID=LHPlatformDB.dbo.PFF_GetSeasonID()
	
	-- 清除记录
	TRUNCATE TABLE RecordRankReward

	-- 删除数据
	DELETE RecordRankHistory WHERE SeasonID=@SeasonID

	-- 默认处理
	DECLARE @DefaultScore INT
	DECLARE @DefaultMainOrder SMALLINT
	DECLARE @DefaultSubOrder SMALLINT
	DECLARE @DefaultStarOrder SMALLINT

	-- 默认分数
	SET @DefaultScore=900

	-- 查询段位
	SELECT @DefaultMainOrder=MainOrder,@DefaultSubOrder=SubOrder,@DefaultStarOrder=StarOrder FROM @TableGameRankLevel WHERE @DefaultScore>=NeedScore AND @DefaultScore<NeedScore+SpanScore

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 插入奖励
	INSERT INTO RecordRankReward(UserID,MainOrder,MailID,ExpireTime,SendState)
	SELECT a.UserID,a.MainOrder,b.MailID,@ExpireTime,0 FROM #UserRankInfo a	CROSS APPLY @TableRewardMailInfo b WHERE b.MainOrder=a.MainOrder

	-- 插入记录
	INSERT INTO RecordRankHistory (UserID,SeasonID,RankScore,MainOrder,SubOrder,StarOrder,CollectTime)
	SELECT UserID,@SeasonID,Score,MainOrder,SubOrder,StarOrder,GetDate() FROM #UserRankInfo(NOLOCK)

	-- 掉段处理
	UPDATE GameScoreInfo SET Score = CASE WHEN b.NeedScore<@DefaultScore THEN @DefaultScore ELSE b.NeedScore END, MainOrder=b.MainOrder,SubOrder=b.SubOrder,StarOrder=b.StarOrder
	FROM GameScoreInfo(NOLOCK) a CROSS APPLY @TableGameRankLevel b 	WHERE 
	b.MainOrder=(
	CASE a.MainOrder
	WHEN 6 THEN 
		@DefaultMainOrder
	ELSE 
		a.MainOrder+1 
	END)	
	AND  
	b.SubOrder=(
	CASE a.MainOrder
	WHEN 3 THEN
		CASE a.SubOrder
		WHEN 5 THEN 4
		ELSE a.SubOrder END
	WHEN 5 THEN
		CASE a.SubOrder
		WHEN 4 THEN 3
		ELSE a.SubOrder END
	WHEN 6 THEN
		@DefaultSubOrder
	ELSE 	
		CASE a.SubOrder 
		WHEN 0 THEN 1 
		ELSE a.SubOrder END
	END)
	AND
	b.StarOrder=(
	CASE a.MainOrder
	WHEN 5 THEN
		CASE WHEN a.SubOrder>=3 THEN @DefaultStarOrder
		ELSE 
			CASE a.StarOrder
			WHEN 4 THEN 3
			ELSE a.StarOrder END
		END
	WHEN 6 THEN
		@DefaultStarOrder
	ELSE a.StarOrder
	END) 	

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- 删除临时表
	IF OBJECT_ID('tempdb..#UserRankInfo') IS NOT NULL
	BEGIN
		DROP TABLE #UserRankInfo
	END

	-- 抛出邮件
	SELECT * FROM @TableRewardMailInfo	

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 插入邮件
CREATE  PROCEDURE dbo.GSP_GR_InsertRewardMail		
	@wUserCount INT					-- 用户数量
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 自动回滚
SET XACT_ABORT ON

-- 执行逻辑
BEGIN
	
	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

		-- 查询邮件
		SELECT TOP(@wUserCount) UserID,MainOrder,MailID,ExpireTime INTO #UserMailInfo FROM RecordRankReward WHERE SendState=0

		-- 更新状态
		UPDATE RecordRankReward SET SendState=1 WHERE UserID IN (SELECT UserID FROM #UserMailInfo)

		-- 插入邮件
		INSERT INTO LHAccountsDB.dbo.AccountsMailBox(UserID,MailID,MailState,ExpireTime,UpdateTime)
		SELECT UserID,MailID,1,ExpireTime,GetDate() FROM #UserMailInfo

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- 抛出记录
	SELECT UserID,MainOrder FROM #UserMailInfo

	-- 删除临时表
	IF OBJECT_ID('tempdb..#UserMailInfo') IS NOT NULL
	BEGIN
		DROP TABLE #UserMailInfo
	END

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
