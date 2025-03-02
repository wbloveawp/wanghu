USE LHInsureDB
GO

/************************完成订单************************/
/*
OrderKind（订单类型）1：商城 2：礼包
*/
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WEB_VAS_FilledOnline]')) 
DROP PROCEDURE [dbo].[WEB_VAS_FilledOnline]

GO

CREATE PROC WEB_VAS_FilledOnline
	@strOrderID				BIGINT,						-- 订单号码
	@dcPayAmount			DECIMAL(18,2),				-- 实际支付金额
	@dwChangeWay			TINYINT,					-- 改变方式 1：支付调用 2：后台手动补单
	@strIpayOrderID			NVARCHAR(64),				-- 支付平台流水号	
	@strClientIP			NVARCHAR(15),				-- 来访地址		
	@strErrorDescribe		NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON
SET XACT_ABORT ON 

DECLARE @OrderID BIGINT
DECLARE @UserID INT
DECLARE @ChannelID INT
DECLARE @PlatformID INT
DECLARE @SpreadChannelID INT
DECLARE @CommodityID INT
DECLARE @PayUserCount INT
DECLARE @PayFirst TINYINT

DECLARE @OrderAmount DECIMAL(18,3)
DECLARE @ChannelAmount DECIMAL(18,3)
DECLARE @PlatformAmount DECIMAL(18,3)
DECLARE @SpreadChannelAmount DECIMAL(18,3)
DECLARE @CompanyAmount DECIMAL(18,3)
DECLARE @SpreadUserID INT
DECLARE @SpreadRate DECIMAL(18,2)
DECLARE @SpreadDivideAmount DECIMAL(18,3)
DECLARE @SpreadDivideScore BIGINT
DECLARE @SpreadAgentID INT
DECLARE @SpreadValidCount INT
DECLARE @SpreadValidAward BIGINT

DECLARE @OrderState TINYINT
DECLARE @OrderKind TINYINT
DECLARE @CurrentDate DATETIME
DECLARE @DateID INT

DECLARE @ChannelRate DECIMAL(18,3)
DECLARE @PlatformRate DECIMAL(18,3)
DECLARE @SpreadChannelRate DECIMAL(18,3)
DECLARE @MemberPoint INT
DECLARE @GameID INT
DECLARE @DoubleStatus TINYINT
DECLARE @ProfitAmount DECIMAL(18,3)

-- 执行逻辑
BEGIN
	SET @CurrentDate=GETDATE()
	SET @DateID=CAST(CAST(@CurrentDate AS FLOAT) AS INT)
	SELECT @DoubleStatus=0,@PayUserCount=0,@PayFirst=0,@SpreadValidCount=0,@SpreadValidAward=0

	-- 查询当前订单状态，用排他锁避免并发造成数据错误
	SELECT @OrderID=OrderID,@UserID=UserID,@ChannelID=ChannelID,@PlatformID=PlatformID,@CommodityID=CommodityID,@OrderAmount=OrderAmount,@OrderState=OrderState,@OrderKind=OrderKind,
		@ChannelRate=ChannelRate,@PlatformRate=PlatformRate,@SpreadChannelRate=SpreadChannelRate,@SpreadUserID=SpreadUserID,@SpreadRate=SpreadRate
	FROM PayOrder(NOLOCK) WHERE OrderID=@strOrderID

	-- 验证订单
	IF @OrderID IS NULL
	BEGIN
		SET @strErrorDescribe='订单不存在'
		RETURN 410
	END

	-- 更新订单状态
	UPDATE PayOrder SET OrderState=1 WHERE OrderID=@strOrderID AND OrderState<>1
	IF @@ROWCOUNT=0
	BEGIN
		SET @strErrorDescribe='订单已支付'
		RETURN 411
	END

	-- 充值记录重复
	IF EXISTS(SELECT OrderID FROM PayRecord(NOLOCK) WHERE OrderID=@strOrderID)
	BEGIN
		SET @strErrorDescribe='订单已支付'
		RETURN 412
	END 

	-- 判断金额
	IF @dcPayAmount<@OrderAmount
	BEGIN
		SET @strErrorDescribe='支付金额和订单金额不一样'
		RETURN 413
	END

	-- 获取推广的代理等级
	SELECT @SpreadAgentID=AgentID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@SpreadUserID
	IF @SpreadAgentID IS NULL
		SET @SpreadAgentID=0

	-- 用户信息
	SELECT @MemberPoint=MemberPoint,@GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo WHERE UserID=@UserID

	--首充双倍/每日翻倍
	DECLARE @ProperGoodsList NVARCHAR(MAX)
	DECLARE @MallGoodsCount INT
	DECLARE @LastBuyDateTime DATETIME
	SELECT @ProperGoodsList=GoodsInfo,@DoubleStatus=DoubleStatus FROM MallGoods WHERE MallgoodsID=@CommodityID
	SELECT @MallGoodsCount=MallgoodsBuyCount,@LastBuyDateTime=LastBuyDateTime FROM MallGoodsBuyStat WHERE UserID=@UserID AND MallgoodsID=@CommodityID
	IF @DoubleStatus=1 AND (@MallGoodsCount IS NULL OR @MallGoodsCount=0)
	BEGIN
		UPDATE PayOrderGoods SET GoodsNumber=GoodsNumber*2 WHERE OrderID=@OrderID
	END
	ELSE IF @DoubleStatus=2 AND (@LastBuyDateTime IS NULL OR (@LastBuyDateTime IS NOT NULL AND CAST(CAST(@LastBuyDateTime AS FLOAT) AS INT)<>@DateID))
	BEGIN
		UPDATE PayOrderGoods SET GoodsNumber=GoodsNumber*2 WHERE OrderID=@OrderID
	END

	-- 商品统计
	UPDATE MallGoodsBuyStat SET MallgoodsBuyCount=MallgoodsBuyCount+1,LastBuyDateTime=@CurrentDate WHERE UserID=@UserID AND MallgoodsID=@CommodityID
	IF @@ROWCOUNT=0
		INSERT INTO MallGoodsBuyStat(UserID,MallgoodsID,MallgoodsBuyCount,LastBuyDateTime) VALUES(@UserID,@CommodityID,1,@CurrentDate)

	-- 金额计算
	SET @ChannelAmount=@dcPayAmount*@ChannelRate
	SET @PlatformAmount=@dcPayAmount*@PlatformRate
	SET @SpreadChannelAmount=@dcPayAmount*@SpreadChannelRate
	SET @SpreadDivideAmount=0

	-- 结算金额 支付金额-支付渠道手续费-平台抽成-推广渠道商分成-推广员分成
	SET @ProfitAmount=@dcPayAmount-@ChannelAmount-@PlatformAmount-@SpreadChannelAmount-@SpreadDivideAmount
	SET @CompanyAmount=@ProfitAmount

	-- 计算推广充值返利
	DECLARE @PayScore BIGINT
	SET @PayScore=CEILING(@dcPayAmount)*2600000
	SET @SpreadDivideScore=@PayScore*@SpreadRate

	-- 结算充值次数
	IF NOT EXISTS (SELECT OrderID FROM PayRecord(NOLOCK) WHERE UserID=@UserID)
	BEGIN
		SET @PayFirst=1

		-- 更新有效用户
		UPDATE LHAccountsDB.dbo.AccountsInfo SET IsValid=1 WHERE UserID=@UserID
	END

	IF NOT EXISTS (SELECT OrderID FROM PayRecord(NOLOCK) WHERE UserID=@UserID AND DATEDIFF(D,PayDateTime,@CurrentDate)=0)
	BEGIN
		SET @PayUserCount=1
	END

	-- 更新订单
	UPDATE PayOrder SET PayAmount=@dcPayAmount,CompanyAmount=@CompanyAmount,ChannelAmount=@ChannelAmount,PlatformAmount=@PlatformAmount,DoubleStatus=@DoubleStatus,GoodsArrivalStatus=1,
		PayDateTime=@CurrentDate,CallbackClientIP=@strClientIP,IpayOrderID=@strIpayOrderID,OrderStatusChangeWay=@dwChangeWay,SpreadChannelAmount=@SpreadChannelAmount,
		SpreadDivideAmount=@SpreadDivideAmount,SpreadDivideScore=@SpreadDivideScore WHERE OrderID=@OrderID

	-- 插入记录
	INSERT INTO PayRecord(OrderID,OrderKind,OrderAmount,PayAmount,UserID,Accounts,ChannelID,ChannelRate,ChannelAmount,PlatformID,PlatformRate,PlatformAmount,CommodityID,CommodityName,CompanyAmount,
		GoodsArrivalStatus,DoubleStatus,CollectDateTime,PayDateTime,CreateClientIP,CallbackClientIP,IpayOrderID,OrderStatusChangeWay,SpreadChannelID,SpreadChannelRate,SpreadChannelAmount,
		SpreadUserID,SpreadRate,SpreadDivideAmount,SpreadDivideScore) 
		SELECT OrderID,OrderKind,OrderAmount,PayAmount,UserID,Accounts,ChannelID,ChannelRate,ChannelAmount,PlatformID,PlatformRate,PlatformAmount,CommodityID,CommodityName,CompanyAmount,
		GoodsArrivalStatus,DoubleStatus,CollectDateTime,PayDateTime,CreateClientIP,CallbackClientIP,IpayOrderID,OrderStatusChangeWay,SpreadChannelID,SpreadChannelRate,SpreadChannelAmount, 
		SpreadUserID,SpreadRate,SpreadDivideAmount,SpreadDivideScore FROM PayOrder WHERE OrderID=@OrderID

	-- 计算有效推广奖励
	IF @SpreadUserID>0 AND @PayFirst=1
	BEGIN
		SET @SpreadValidCount=1
		IF @SpreadAgentID=0
			SET @SpreadValidAward=800000
	END

	-- 汇总统计
	UPDATE ReportDayPay SET PayAmount=PayAmount+@dcPayAmount,PayCount=PayCount+1,ChannelAmount=ChannelAmount+@ChannelAmount,PlatformAmount=PlatformAmount+@PlatformAmount,PayUserCount=PayUserCount+@PayUserCount
		WHERE DateID=@DateID AND ChannelID=@ChannelID 
	IF @@ROWCOUNT=0
		INSERT INTO ReportDayPay(DateID,ChannelID,PlatformID,PayAmount,ChannelAmount,PlatformAmount,PayCount,PayUserCount) VALUES(@DateID,@ChannelID,@PlatformID,@dcPayAmount,@ChannelAmount,@PlatformAmount,1,1)

	-- 商品统计
	UPDATE ReportDayCommodityPay SET PaySumAmount=PaySumAmount+@dcPayAmount,PayCount=PayCount+1,LastPayDateTime=@CurrentDate,LastPayAmount=@dcPayAmount
		WHERE DateID=@DateID AND CommodityID=@CommodityID AND OrderKind=@OrderKind
	IF @@ROWCOUNT=0
		INSERT INTO ReportDayCommodityPay(DateID,CommodityID,OrderKind,PaySumAmount,PayCount,LastPayDateTime,LastPayAmount) VALUES(@DateID,@CommodityID,@OrderKind,@dcPayAmount,1,@CurrentDate,@dcPayAmount)

	-- 用户统计
	UPDATE LHGameRecordDB.dbo.ReportDayUser SET PayAmount=PayAmount+@dcPayAmount,PayCount=PayCount+1,LastPayAmount=@dcPayAmount,LastPayDate=@CurrentDate WHERE DateID=@DateID AND UserID=@UserID
	IF @@ROWCOUNT=0
		INSERT INTO LHGameRecordDB.dbo.ReportDayUser(DateID,UserID,PayCount,PayAmount,LastPayAmount,LastPayDate) VALUES(@DateID,@UserID,1,@dcPayAmount,@dcPayAmount,@CurrentDate)
	UPDATE AccountsPay SET PayCount=PayCount+1,PayAmount=PayAmount+@dcPayAmount,LastPayAmount=@dcPayAmount,LastPayDateTime=@CurrentDate WHERE UserID=@UserID
	IF @@ROWCOUNT=0
		INSERT INTO AccountsPay(UserID,PayCount,PayAmount,LastPayAmount,LastPayDateTime) VALUES(@UserID,1,@dcPayAmount,@dcPayAmount,@CurrentDate)

	-- 代理统计
	IF @SpreadUserID>0
	BEGIN
		UPDATE LHGameRecordDB.dbo.ReportDayUser SET SpreadValidCount=SpreadValidCount+@SpreadValidCount,SpreadValidAward=SpreadValidAward+@SpreadValidAward,SpreadPayUser=SpreadPayUser+@PayUserCount,
			SpreadPayAmount=SpreadPayAmount+@dcPayAmount,SpreadPayAward=SpreadPayAward+@SpreadDivideScore,ReportDateTime=@CurrentDate WHERE DateID=@DateID AND UserID=@SpreadUserID
		IF @@ROWCOUNT=0
			INSERT INTO LHGameRecordDB.dbo.ReportDayUser(DateID,UserID,SpreadValidCount,SpreadValidAward,SpreadPayUser,SpreadPayAmount,SpreadPayAward,ReportDateTime)
			VALUES(@DateID,@SpreadUserID,@SpreadValidCount,@SpreadValidAward,@PayUserCount,@dcPayAmount,@SpreadDivideScore,@CurrentDate)
	END

	-- 充值
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

	-- 双倍
	IF @DoubleStatus<>0
	BEGIN
		UPDATE @TableProperGoods SET GoodsCount=GoodsCount*2
	END

	-- 处理物品
	SELECT @Index=1,@LoopCount=COUNT(1) FROM @TableProperGoods

	-- 循环处理
	WHILE @Index<=@LoopCount
	BEGIN	
		-- 查询信息
		SELECT @IncGoodsID=GoodsID,@IncGoodsCount=GoodsCount,@IncGoodsIndate=GoodsIndate FROM @TableProperGoods WHERE ID=@Index		

		-- 校验物品类型
		IF LHAccountsDB.dbo.BYF_IsMoneyGoods(@IncGoodsID) = 1
		BEGIN
			-- 写入财富
			EXEC LHAccountsDB.dbo.GSP_GP_WriteGameWealth @UserID,@IncGoodsID,102,@IncGoodsCount

			-- 钻石特殊处理
			IF @IncGoodsID=1 
			BEGIN
				UPDATE LHAccountsDB.dbo.AccountsIngotInfo SET FillTimes=FillTimes+1,FillAmount=FillAmount+@IncGoodsCount WHERE UserID=@UserID
			END

			-- 黄金特殊处理
			IF @IncGoodsID=2 
			BEGIN
				UPDATE LHAccountsDB.dbo.AccountsGoldInfo SET FillTimes=FillTimes+1,FillAmount=FillAmount+@IncGoodsCount WHERE UserID=@UserID
			END
		END 
		
		-- 更新索引
		SET @Index=@Index+1
	END

	-- 输出参数
	SELECT  @UserID AS UserID,@GameID AS GameID,@dcPayAmount AS PayAmount,@SpreadUserID AS SpreadUserID,@SpreadAgentID AS SpreadAgentID,
		@SpreadDivideScore AS SpreadDivideScore,@SpreadValidCount AS SpreadValidCount,@SpreadValidAward AS SpreadValidAward
END

RETURN 0
GO
