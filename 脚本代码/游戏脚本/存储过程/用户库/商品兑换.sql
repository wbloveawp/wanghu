----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ExchangeMallGoods]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ExchangeMallGoods]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- 兑换商品
CREATE PROC GSP_GP_ExchangeMallGoods
	@dwUserID INT,							-- 用户标识			
	@dwMallGoodsID INT,						-- 商品标识	
	@wReason INT,							-- 变更原因		
	@dwTimeStamp INT,						-- 时间戳			
	@strErrorDescribe NVARCHAR(127) OUTPUT  -- 错误描述
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 自动回滚
SET XACT_ABORT ON

-- 执行逻辑
BEGIN	
	
	-- 变量定义
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @Accounts NVARCHAR(32)
	DECLARE @MemberOrder SMALLINT
	
	-- 查询用户
	SELECT @UserID=a.UserID, @Accounts=a.Accounts, @Nullity=a.Nullity, @MemberOrder=a.MemberOrder, @ShutDown=b.ShutDownStatus 
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
		RETURN 2
	END	

	-- 帐号关闭
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
		RETURN 3
	END	

	-- 查询配置
	DECLARE @GoodsID BIGINT
	DECLARE @MoneyID BIGINT
	DECLARE @GoodsCount INT	
	DECLARE @GoodsIndate INT	
	DECLARE @SalePrice INT
	DECLARE @VipLimitMin SMALLINT	
	DECLARE @CommodityTypeID INT
	DECLARE @CommodityName NVARCHAR(32)
	SELECT @GoodsID=GoodsID,@GoodsCount=GoodsCount,@GoodsIndate=GoodsIndate,@MoneyID=MoneyID,@SalePrice=Price,@VipLimitMin=VipLimitMin,@CommodityName=CommodityName,@CommodityTypeID=CommodityTypeID 
	FROM LHInsureDB.dbo.EMallCommodity(NOLOCK) WHERE CommodityID=@dwMallgoodsID

	-- 商品不存在
	IF @GoodsID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1285,"Msg":"抱歉，系统未查询到该商品信息，购买失败！"}'	

		RETURN 4
	END

	-- 校验货币类型
	IF @MoneyID<>1 AND @MoneyID<>2
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1285,"Msg":"抱歉，该商品不能使用sawa point购买！"}'

		RETURN 5
	END

	-- 校验等级
	IF @VipLimitMin>@MemberOrder
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1286,"Msg":"抱歉，该商品需VIP'+CONVERT(NVARCHAR(3),@VipLimitMin)+N'才可购买！","Ops":["'+CONVERT(NVARCHAR(3),@VipLimitMin)+'"]}'

		RETURN 6
	END

	-- 查找物品
	DECLARE @GoodsKind SMALLINT	
	DECLARE @GoodsName NVARCHAR(64)
	SELECT @GoodsKind=GoodsKind, @GoodsName=GoodsName FROM LHPlatformDB.dbo.GameGoodsInfo(NOLOCK) WHERE GoodsID=@GoodsID

	-- 物品不存在
	IF @GoodsKind IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1285,"Msg":"抱歉，系统未查询到该商品信息，购买失败！"}'	

		RETURN 7
	END

	-- 变量定义
	DECLARE @ResultCode INT
	DECLARE @BeforeCount INT
	DECLARE @ResidueCount INT		
	DECLARE @DeductAmount BIGINT	
	DECLARE @ExpendAmount BIGINT

	-- 设置变量	
	SELECT @ExpendAmount=@SalePrice,@DeductAmount=-@SalePrice

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN	

	-- 扣除货币
	EXEC @ResultCode=GSP_GP_WriteGameWealth @dwUserID,@MoneyID,@wReason,@DeductAmount
	IF @ResultCode>0
	BEGIN
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 错误信息
		SET @strErrorDescribe=N'{"Idx":1296,"Msg":"抱歉，由于您身上的sawa point余额不足，兑换失败！"}'	
		RETURN 6
	END

	-- 添加物品
	IF dbo.LHF_IsMoneyGoods(@GoodsID)=1
	BEGIN
		EXEC GSP_GP_WriteGameWealth @dwUserID,@GoodsID,@wReason,@GoodsCount
	END ELSE
	BEGIN
		EXEC GSP_GP_AppendGoodsItem @dwUserID,@wReason,@GoodsID,1,@GoodsCount,@GoodsIndate,@dwTimeStamp
	END

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 兑换记录
	INSERT INTO LHInsureDB.dbo.EMallOrder(UserID,Accounts,CommodityID,CommodityName,CommodityTypeID,CommodityCount,Price,OrderAmount,GoodsID,GoodsCount,OrderStatus)
	VALUES(@dwUserID,@Accounts,@dwMallGoodsID,@CommodityName,@CommodityTypeID,1,@SalePrice,@SalePrice,@GoodsID,@GoodsCount,1)

	-- 查询余额
	DECLARE @MoneyBalance BIGINT

	-- 金币
	IF @MoneyID=1
	BEGIN
		SELECT @MoneyBalance=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID
	END

	-- 调整变量
	IF @MoneyBalance IS NULL SET @MoneyBalance=0

	-- 抛出记录	
	SELECT @MoneyBalance AS MoneyBalance,@MoneyID AS ExpendMoneyID, @GoodsID AS IncGoodsID, @GoodsCount AS IncGoodsCount, @GoodsIndate AS IncGoodsIndate
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------