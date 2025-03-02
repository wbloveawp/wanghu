
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadStockInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadStockInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryStockInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryStockInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryStockList]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryStockList]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AppendStockInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AppendStockInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ModifyStockInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ModifyStockInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_DeleteStockInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_DeleteStockInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UpdateStockScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UpdateStockScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UpdateStockState]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UpdateStockState]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UpdateJackpotScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UpdateJackpotScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AdjustStockScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AdjustStockScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AdjustJackpotScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AdjustJackpotScore]
GO

----------------------------------------------------------------------------------------------------

-- 加载库存
CREATE  PROCEDURE dbo.GSP_GP_LoadStockInfo		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

	SELECT *, (SELECT * FROM GameJackpotInfo(NOLOCK) WHERE StockID=GameStockInfo.StockID FOR JSON AUTO) AS JackpotOption  FROM GameStockInfo(NOLOCK) WHERE Nullity=0

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 查询库存
CREATE  PROCEDURE dbo.GSP_GP_QueryStockInfo		
	@wStockID	INT
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

	SELECT *, (SELECT * FROM GameJackpotInfo(NOLOCK) WHERE StockID=GameStockInfo.StockID FOR JSON AUTO) AS JackpotOption  FROM GameStockInfo(NOLOCK) WHERE StockID=@wStockID AND Nullity=0

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 查询库存
CREATE  PROCEDURE dbo.GSP_GP_QueryStockList
	@strKindIDList	NVARCHAR(512)
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON
	
	IF ISJSON(@strKindIDList)>0
	BEGIN
		SELECT a.*, (SELECT * FROM GameJackpotInfo(NOLOCK) WHERE StockID=a.StockID FOR JSON AUTO) AS JackpotOption  FROM GameStockInfo(NOLOCK) a, OPENJSON(@strKindIDList) b WHERE Nullity=0 AND a.KindID =b.VALUE
		ORDER BY a.KindID,a.BetAmount
	END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 添加库存
CREATE  PROCEDURE dbo.GSP_GP_AppendStockInfo
	@wKindID			SMALLINT,				-- 类型标识
	@lBetAmount			INT,					-- 下注金额
	@wStockKind			INT,					-- 库存类型
	@wStockLevel		INT,					-- 库存级别
	@strStockName		NVARCHAR(32),			-- 库存名称
	@lExtraScore		BIGINT,					-- 额外配额
	@lStockScore		BIGINT,					-- 当前库存
	@cbEnableMode		TINYINT,				-- 启用模式
	@dwEnableEndTime	BIGINT,					-- 停用时间
	@dwEnableStartTime  BIGINT,					-- 启用时间
	@strShrinkOption    NVARCHAR(512),			-- 抽水配置
	@strRelationIDList	NVARCHAR(128),			-- 关联标识
	@strJackpotOption	NVARCHAR(1024),			-- 彩池信息
	@strErrorDescribe	NVARCHAR(127) OUTPUT	-- 输出信息	
	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON
	
	-- 抽水配置
	IF ISJSON(@strShrinkOption)=0
	BEGIN
		SET @strErrorDescribe=N'抽水配置格式有误，请重新配置！'
		RETURN 1
	END

	-- 关联配置
	IF LEN(@strRelationIDList)>0 AND ISJSON(@strRelationIDList)=0
	BEGIN
		SET @strErrorDescribe=N'关联标识格式有误，请重新配置！'
		RETURN 2
	END

	-- 彩金配置
	IF ISJSON(@strJackpotOption)=0
	BEGIN
		SET @strErrorDescribe=N'彩池配置格式有误，请重新配置！'
		RETURN 3
	END

	-- BET金额不能重复
	IF EXISTS(SELECT 1 FROM GameStockInfo(NOLOCK) WHERE KindID=@wKindID AND BetAmount=@lBetAmount)
	BEGIN
		SET @strErrorDescribe=N'抱歉,同一游戏下不能配置相同BET金额的库存！'
		RETURN 4
	END

	-- BET库存数量限制
	IF @wStockLevel=1 AND (SELECT COUNT(1) FROM GameStockInfo(NOLOCK) WHERE KindID=@wKindID AND StockLevel=1)>=12
	BEGIN
		SET @strErrorDescribe=N'抱歉,同一游戏下BET金额库存数量不能超过12个！'
		RETURN 5
	END

	-- 插入库存
	INSERT INTO GameStockInfo(KindID,StockKind,StockLevel,BetAmount,StockName,ExtraScore,StockScore,StockState,EnableMode,EnableEndTime,EnableStartTime,ShrinkOption,RelationIDList,FirstStock,Describe)
	VALUES(@wKindID,@wStockKind,@wStockLevel,@lBetAmount,@strStockName,@lExtraScore,@lStockScore,0,@cbEnableMode,@dwEnableEndTime,@dwEnableStartTime,@strShrinkOption,@strRelationIDList,@lStockScore,N'')

	-- 库存标识
	DECLARE @StockID INT = SCOPE_IDENTITY()

	-- 插入彩金
	INSERT INTO GameJackpotInfo(StockID,LevelID,ExtraScore,StockScore,VirtualScore,ShrinkRatio,PayoutProb,PayoutLimit,PayoutOrigin,FirstGold)
	SELECT @StockID,JSON_VALUE(VALUE,N'$.LevelID'),JSON_VALUE(VALUE,N'$.ExtraScore'),JSON_VALUE(VALUE,N'$.StockScore'),JSON_VALUE(VALUE,N'$.VirtualScore'),
		   JSON_VALUE(VALUE,N'$.ShrinkRatio'),JSON_VALUE(VALUE,N'$.PayoutProb'),JSON_VALUE(VALUE,N'$.PayoutLimit'),JSON_VALUE(VALUE,N'$.PayoutOrigin'),JSON_VALUE(VALUE,N'$.StockScore') 
	FROM OPENJSON(@strJackpotOption)

	-- 抛出数据
	SELECT *, (SELECT * FROM GameJackpotInfo(NOLOCK) WHERE StockID=GameStockInfo.StockID FOR JSON AUTO) AS JackpotOption  FROM GameStockInfo(NOLOCK) WHERE StockID=@StockID AND Nullity=0

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 添加库存
CREATE  PROCEDURE dbo.GSP_GP_ModifyStockInfo
	@wStockID			INT,					-- 库存标识
	@wKindID			INT,					-- 类型标识
	@lBetAmount			INT,					-- 下注金额
	@wStockKind			INT,					-- 库存类型
	@wStockLevel		INT,					-- 库存级别
	@strStockName		NVARCHAR(32),			-- 库存名称
	@lExtraScore		BIGINT,					-- 额外配额
	@cbEnableMode		TINYINT,				-- 启用模式
	@dwEnableEndTime	BIGINT,					-- 停用时间
	@dwEnableStartTime  BIGINT,					-- 启用时间
	@strShrinkOption    NVARCHAR(512),			-- 抽水配置
	@strRelationIDList	NVARCHAR(128),			-- 关联标识
	@strJackpotOption	NVARCHAR(1024),			-- 彩池信息
	@strErrorDescribe	NVARCHAR(127) OUTPUT	-- 输出信息	
	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON
	
	-- 抽水配置
	IF ISJSON(@strShrinkOption)=0
	BEGIN
		SET @strErrorDescribe=N'抽水配置格式有误，请重新配置！'
		RETURN 1
	END

	-- 关联配置
	IF LEN(@strRelationIDList)>0 AND ISJSON(@strRelationIDList)=0
	BEGIN
		SET @strErrorDescribe=N'关联标识格式有误，请重新配置！'
		RETURN 2
	END

	-- 彩金配置
	IF ISJSON(@strJackpotOption)=0
	BEGIN
		SET @strErrorDescribe=N'彩池配置格式有误，请重新配置！'
		RETURN 3
	END

	-- BET金额不能重复
	--IF EXISTS(SELECT 1 FROM GameStockInfo(NOLOCK) WHERE KindID=@wKindID AND BetAmount=@lBetAmount)
	--BEGIN
	--	SET @strErrorDescribe=N'同一游戏下不能配置相同BET金额的库存！'
	--	RETURN 4
	--END

	-- 更新库存
	UPDATE GameStockInfo SET 
		BetAmount=@lBetAmount,
		StockKind=@wStockKind,
		StockLevel=@wStockLevel,
		StockName=@strStockName,
		ExtraScore=@lExtraScore,
		EnableMode=@cbEnableMode,
		EnableEndTime=@dwEnableEndTime,
		EnableStartTime=@dwEnableStartTime,
		ShrinkOption=@strShrinkOption,
		RelationIDList=@strRelationIDList
	WHERE StockID=@wStockID	

	-- 表变量
	DECLARE @TableJackpotInfo TABLE(
		StockID SMALLINT,
		LevelID TINYINT,
		ExtraScore BIGINT,
		StockScore BIGINT,
		VirtualScore BIGINT,
		ShrinkRatio SMALLINT,
		PayoutProb SMALLINT,
		PayoutLimit BIGINT,
		PayoutOrigin BIGINT
	)

	-- 彩池信息
	INSERT INTO @TableJackpotInfo(StockID,LevelID,ExtraScore,StockScore,VirtualScore,ShrinkRatio,PayoutProb,PayoutLimit,PayoutOrigin)
	SELECT @wStockID,JSON_VALUE(VALUE,N'$.LevelID'),JSON_VALUE(VALUE, N'$.ExtraScore'),JSON_VALUE(VALUE, N'$.StockScore'),JSON_VALUE(VALUE, N'$.VirtualScore'),JSON_VALUE(VALUE, N'$.ShrinkRatio'),
			JSON_VALUE(VALUE, N'$.PayoutProb'),JSON_VALUE(VALUE, N'$.PayoutLimit'),JSON_VALUE(VALUE, N'$.PayoutOrigin')
	FROM OPENJSON(@strJackpotOption)

	-- 更新彩池
	UPDATE GameJackpotInfo SET ExtraScore=b.ExtraScore, VirtualScore=b.VirtualScore, ShrinkRatio=b.ShrinkRatio,PayoutProb=b.PayoutProb, PayoutLimit=b.PayoutLimit,PayoutOrigin=b.PayoutOrigin
	FROM GameJackpotInfo a, @TableJackpotInfo b WHERE a.StockID=@wStockID AND a.LevelID=b.LevelID	

	-- 插入彩池
	INSERT INTO GameJackpotInfo(StockID,LevelID,ExtraScore,StockScore,VirtualScore,ShrinkRatio,PayoutProb,PayoutLimit,PayoutOrigin,FirstGold) 
	SELECT StockID,LevelID,ExtraScore,StockScore,VirtualScore,ShrinkRatio,PayoutProb,PayoutLimit,PayoutOrigin,StockScore
	FROM @TableJackpotInfo WHERE LevelID NOT IN (SELECT LevelID FROM GameJackpotInfo(NOLOCK) WHERE StockID=@wStockID)

	-- 删除彩池
	DELETE GameJackpotInfo WHERE StockID=@wStockID AND LevelID NOT IN (SELECT LevelID FROM @TableJackpotInfo)

	-- 抛出数据
	SELECT *, (SELECT * FROM GameJackpotInfo(NOLOCK) WHERE StockID=GameStockInfo.StockID FOR JSON AUTO) AS JackpotOption  FROM GameStockInfo(NOLOCK) WHERE StockID=@wStockID AND Nullity=0

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 删除库存
CREATE  PROCEDURE dbo.GSP_GP_DeleteStockInfo
	@wStockID			INT				-- 库存标识
		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON
	
	-- 抛出数据
	SELECT *, (SELECT * FROM GameJackpotInfo(NOLOCK) WHERE StockID=GameStockInfo.StockID FOR JSON AUTO) AS JackpotOption  FROM GameStockInfo(NOLOCK) WHERE StockID=@wStockID AND Nullity=0

	-- 物理删除
	DELETE GameStockInfo WHERE StockID=@wStockID
	DELETE GameJackpotInfo WHERE StockID=@wStockID

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 更新库存
CREATE  PROCEDURE dbo.GSP_GP_UpdateStockScore
	@strVariationList		NVARCHAR(4000)			-- 变更列表
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

	-- 更新积分
	IF ISJSON(@strVariationList)>0
	BEGIN
		UPDATE GameStockInfo  SET GameStockInfo.StockScore+=a.Score,ShrinkProfit+=a.Profit FROM OPENJSON(@strVariationList) WITH (StockID INT, Score BIGINT, Profit BIGINT) a 
		WHERE GameStockInfo.StockID=a.StockID
	END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 更新状态
CREATE  PROCEDURE dbo.GSP_GP_UpdateStockState
	@strStockStateList		NVARCHAR(4000)  -- 状态列表
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

	-- 更新状态
	IF ISJSON(@strStockStateList)>0
	BEGIN
		-- 表变量
		DECLARE @StockStateTable TABLE(StockID SMALLINT PRIMARY KEY CLUSTERED,[State] TINYINT,ControlID BIGINT)

		-- 插入数据
		INSERT INTO @StockStateTable SELECT * FROM OPENJSON(@strStockStateList) WITH (StockID INT, [State] TINYINT, ControlID BIGINT)

		-- 更新状态
		UPDATE GameStockInfo SET GameStockInfo.StockState=a.[State],GameStockInfo.StockControlID=CASE WHEN a.ControlID=0 THEN GameStockInfo.StockControlID ELSE a.ControlID END 
		FROM @StockStateTable a WHERE GameStockInfo.StockID=a.StockID

		-- 插入数据
		--INSERT INTO LHGameRecordDB.dbo.RecordStockControl(ID,StockID,KindID,StartTime,EndTime,FirstStock,FirstShrinkProfit,ShrinkOption,FirstGold,ShrinkRatio,RelationIDList)
		--SELECT a.ControlID,a.StockID,b.KindID, LHAccountsDB.dbo.LHF_GetTimestamp(),0,b.StockScore,b.ShrinkProfit,b.ShrinkOption,c.StockScore,c.ShrinkRatio,b.RelationIDList  
		--FROM @StockStateTable a, GameStockInfo b, GameJackpotInfo c WHERE a.[State]=1 AND b.StockID=a.StockID AND c.StockID=a.StockID AND c.LevelID=1

		INSERT INTO LHGameRecordDB.dbo.RecordStockControl(ID,StockID,StockName,KindID,StartTime,EndTime,FirstStock,FirstShrinkProfit,ShrinkOption,RelationIDList,FirstGold)
		SELECT a.ControlID,a.StockID,b.StockName,b.KindID, LHAccountsDB.dbo.LHF_GetTimestamp(),0,b.StockScore,b.ShrinkProfit,b.ShrinkOption,b.RelationIDList,ISNULL((SELECT SUM(StockScore) FROM GameJackpotInfo(NOLOCK) WHERE StockID=a.StockID),0)
		FROM @StockStateTable a, GameStockInfo(NOLOCK) b WHERE a.[State]=1 AND b.StockID=a.StockID

		-- 更新数据
		--UPDATE LHGameRecordDB.dbo.RecordStockControl SET EndTime=LHAccountsDB.dbo.LHF_GetTimestamp(),StockScore=b.StockScore,ShrinkProfit=b.ShrinkProfit,GoldScore=c.StockScore
		--FROM @StockStateTable a, GameStockInfo b, GameJackpotInfo c WHERE ID=a.ControlID AND a.[State]<>1 AND b.StockID=a.StockID AND c.StockID=a.StockID AND c.LevelID=1

		UPDATE LHGameRecordDB.dbo.RecordStockControl SET EndTime=LHAccountsDB.dbo.LHF_GetTimestamp(),StockScore=b.StockScore,ShrinkProfit=b.ShrinkProfit,GoldScore=ISNULL((SELECT SUM(StockScore) FROM GameJackpotInfo(NOLOCK) WHERE StockID=b.StockID),0),
			   JackpotInfo=ISNULL((SELECT * FROM GameJackpotInfo(NOLOCK) WHERE StockID=a.StockID FOR JSON AUTO),N'[]')
		FROM @StockStateTable a, GameStockInfo(NOLOCK) b WHERE ID=a.ControlID AND a.[State]<>1 AND b.StockID=a.StockID
	END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 更新奖池
CREATE  PROCEDURE dbo.GSP_GP_UpdateJackpotScore
	@strVariationList		NVARCHAR(4000)  -- 状态列表
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

	-- 更新彩池
	IF ISJSON(@strVariationList)>0
	BEGIN
		UPDATE GameJackpotInfo  SET GameJackpotInfo.StockScore+=a.Score FROM OPENJSON(@strVariationList) WITH (StockID INT, LevelID TINYINT, Score BIGINT) a 
		WHERE GameJackpotInfo.StockID=a.StockID AND GameJackpotInfo.LevelID=a.LevelID
	END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 调整库存
CREATE  PROCEDURE dbo.GSP_GP_AdjustStockScore
	@wStockID				SMALLINT,	  -- 库存标识
	@lChangeScore			BIGINT		  -- 变化积分	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

	-- 更新库存
	UPDATE GameStockInfo SET AllAddSubtractStock+=@lChangeScore WHERE StockID=@wStockID

	-- 更新记录
	UPDATE LHGameRecordDB.dbo.RecordStockControl SET TotalAddReduceStock+=@lChangeScore WHERE ID=ISNULL((SELECT StockControlID  FROM GameStockInfo(NOLOCK) WHERE StockID=@wStockID),0)	

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 调整彩金
CREATE  PROCEDURE dbo.GSP_GP_AdjustJackpotScore
	@wStockID				SMALLINT,	  -- 库存标识
	@wLevelID				SMALLINT,	  -- 彩池等级
	@lChangeScore			BIGINT		  -- 变化积分	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

	-- 更新库存
	UPDATE GameStockInfo SET AllAddSubtractGold +=@lChangeScore WHERE StockID=@wStockID

	-- 更新记录
	UPDATE LHGameRecordDB.dbo.RecordStockControl SET TotalAddReduceGold+=@lChangeScore WHERE ID=ISNULL((SELECT StockControlID  FROM GameStockInfo(NOLOCK) WHERE StockID=@wStockID),0)	

RETURN 0

GO




----------------------------------------------------------------------------------------------------



