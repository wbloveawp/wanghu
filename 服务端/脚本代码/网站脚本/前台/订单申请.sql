----------------------------------------------------------------------
-- 用途：申请订单
----------------------------------------------------------------------

USE LHInsureDB
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

-- 申请订单
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WEB_VAS_RequestOrder]') AND OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[WEB_VAS_RequestOrder]
GO

-- 获取订单
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WEB_VAS_GetOnLineOrder]') AND OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[WEB_VAS_GetOnLineOrder]
GO

----------------------------------------------------------------------------------------------------
-- 申请订单
CREATE PROC WEB_VAS_RequestOrder
	@dwChannelID		INT,						-- 渠道标识
	@strIpayOrderID		NVARCHAR(64),				-- 支付平台流水号	
	@dwCommodityID		INT,						-- 商品标识	
	@strCommodityName	NVARCHAR(32),				-- 商品名称
	@dtOrderKind		TINYINT,					-- 订单类型（1：商城 2：礼包）
	@dwUserID			INT,						-- 用户标识
	@strOrderID			BIGINT,						-- 订单号码	
	@dcOrderAmount		DECIMAL(18,2),				-- 订单金额	
	@strClientIP		NVARCHAR(15),				-- 下单地址
	@strErrorDescribe	NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

-- 帐号资料
DECLARE @Accounts NVARCHAR(31)
DECLARE @GameID INT
DECLARE @UserID INT
DECLARE @Nullity TINYINT
DECLARE @SpreadUserID INT
DECLARE @SpreadAgentID INT
DECLARE @SpreadScale DECIMAL(18,3)
DECLARE @SpreadChannelID INT
DECLARE @SpreadChannelRate DECIMAL(18,3)

-- 订单信息
DECLARE @OrderID BIGINT
DECLARE @OrderAmount DECIMAL(18,2)
DECLARE @OrderState TINYINT
DECLARE @CreateClientIP NVARCHAR(15)

-- 执行逻辑
BEGIN
	--------------------------- 验证用户 begin -----------------------------------
	SELECT @UserID=UserID,@GameID=GameID,@Accounts=Accounts,@Nullity=Nullity,@SpreadUserID=ParentID,@SpreadChannelID=SpreadChannelID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK)
	WHERE UserID=@dwUserID
	
	-- 用户存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试！'
		RETURN 100
	END	

	-- 帐号封禁
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 101
	END		
	------------------------------ 验证用户 end -----------------------------------

	IF EXISTS(SELECT OrderID FROM PayOrder(NOLOCK) WHERE OrderID=@strOrderID)
	BEGIN
		SET @strErrorDescribe=N'抱歉！该订单已存在,请重新充值。'
		RETURN 400
	END

	IF EXISTS(SELECT OrderID FROM PayOrder(NOLOCK) WHERE ChannelID=@dwChannelID AND IpayOrderID=@strIpayOrderID)
	BEGIN
		SET @strErrorDescribe=N'抱歉！该订单已处理,请重新充值。'
		RETURN 401
	END

	-- 获取渠道信息
	DECLARE @ChannelID INT
	DECLARE @ChannelRate DECIMAL(18,3)
	DECLARE @PlatformID INT	
	DECLARE @PlatformRate DECIMAL(18,3)
	SELECT @ChannelID=ChannelID,@ChannelRate=ChannelRate,@PlatformID=PlatformID,@PlatformRate=PlatformRate FROM ChannelConfig WHERE ChannelID=@dwChannelID
	IF @ChannelID IS NULL
	BEGIN
		SET @strErrorDescribe=N'抱歉！支付渠道不存在。'
		RETURN 402
	END

	-- 获取推广渠道信息
	SELECT @SpreadChannelRate=ChannelRate FROM LHAccountsDB.dbo.ChannelInfo WHERE ChannelID=@SpreadChannelID
	IF @SpreadChannelRate IS NULL
		SET @SpreadChannelRate=0

	-- 获取上级
	IF @SpreadUserID<>0
		SELECT @SpreadAgentID=AgentID,@SpreadScale=AgentScale FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@SpreadUserID
	ELSE
		SELECT @SpreadAgentID=0,@SpreadScale=0

	-- 设置推广比例
	IF @SpreadScale=0
		SET @SpreadScale=0.05 -- 默认5%的充值比例

	-- 普通代理才有充值返利
	IF @SpreadAgentID<>1
		SET @SpreadScale=0

	-- 申请订单
	INSERT INTO PayOrder(OrderID,OrderKind,OrderAmount,UserID,Accounts,ChannelID,ChannelRate,PlatformID,PlatformRate,CommodityID,CommodityName,SpreadChannelID,SpreadChannelRate,SpreadUserID,SpreadRate,CreateClientIP)
	VALUES(@strOrderID,@dtOrderKind,@dcOrderAmount,@UserID,@Accounts,@ChannelID,@ChannelRate,@PlatformID,@PlatformRate,@dwCommodityID,@strCommodityName,@SpreadChannelID,@SpreadChannelRate,@SpreadUserID,@SpreadScale,@strClientIP)

	-- 新增订单物品
	IF @dtOrderKind=1
	BEGIN
		DECLARE @ProperGoodsList NVARCHAR(MAX)
		SELECT @ProperGoodsList=GoodsInfo FROM MallGoods WHERE MallgoodsID=@dwCommodityID
		IF @ProperGoodsList IS NOT NULL
		BEGIN
			-- 变量定义
			DECLARE @Index INT
			DECLARE @LoopCount INT	

			-- 物品信息
			DECLARE @IncGoodsID BIGINT
			DECLARE @IncGoodsCount BIGINT
			DECLARE @IncGoodsIndate INT	

			-- 表变量定义
			DECLARE @TableProperGoods TABLE (ID INT identity(1,1),GoodsID BIGINT,GoodsCount INT,GoodsIndate INT)

			-- 插入记录
			INSERT @TableProperGoods (GoodsID,GoodsCount,GoodsIndate)
			SELECT JSON_VALUE(VALUE,'$.GoodsID'),JSON_VALUE(VALUE,'$.GoodsCount'),JSON_VALUE(VALUE,'$.Indate') FROM OPENJSON(@ProperGoodsList)

			-- 处理物品
			SELECT @Index=1,@LoopCount=COUNT(1) FROM @TableProperGoods

			-- 循环处理
			WHILE @Index<=@LoopCount
			BEGIN	
				-- 查询信息
				SELECT @IncGoodsID=GoodsID,@IncGoodsCount=GoodsCount,@IncGoodsIndate=GoodsIndate FROM @TableProperGoods WHERE ID=@Index		

				-- 插入订单物品
				INSERT INTO PayOrderGoods(OrderID,GoodsID,GoodsNumber,Indate) VALUES(@strOrderID,@IncGoodsID,@IncGoodsCount,@IncGoodsIndate)
		
				-- 更新索引
				SET @Index=@Index+1
			END
		END
	END

	-- 订单信息查询
	SELECT @ChannelID=ChannelID,@UserID=UserID,@Accounts=Accounts,@OrderID=OrderID,@OrderAmount=OrderAmount,@OrderState=OrderState,@CreateClientIP=CreateClientIP
	FROM PayOrder(NOLOCK) WHERE OrderID=@strOrderID

	-- 输出参数
	SELECT @ChannelID AS ChannelID,@UserID AS UserID,@Accounts AS Accounts,@OrderID AS OrderID,@OrderAmount AS OrderAmount,@OrderState AS OrderState,@CreateClientIP AS CreateClientIP
END

RETURN 0 
GO

----------------------------------------------------------------------------------------------------
-- 获取订单信息
CREATE PROC WEB_VAS_GetOnLineOrder
	@strOrderID			NVARCHAR(32)			-- 订单号码
WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

-- 订单信息
DECLARE @ChannelID INT
DECLARE @UserID INT
DECLARE @Accounts NVARCHAR(32)
DECLARE @OrderID BIGINT
DECLARE @OrderAmount DECIMAL(18,2)
DECLARE @OrderState TINYINT
DECLARE @IPAddress NVARCHAR(15)

-- 执行逻辑
BEGIN
	IF NOT EXISTS(SELECT OrderID FROM PayOrder(NOLOCK) WHERE OrderID=@strOrderID)
	BEGIN
		RETURN 400
	END

	-- 订单信息查询
	SELECT @ChannelID=ChannelID,@UserID=UserID,@Accounts=Accounts,@OrderID=OrderID,@OrderAmount=OrderAmount,@OrderState=OrderState,@IPAddress=CreateClientIP
	FROM PayOrder(NOLOCK) WHERE OrderID=@strOrderID

	-- 输出参数
	SELECT @ChannelID AS ChannelID,@UserID AS UserID,@Accounts AS Accounts,@OrderID AS OrderID,@OrderAmount AS OrderAmount,@OrderState AS OrderState,@IPAddress AS IPAddress
END

RETURN 0
GO