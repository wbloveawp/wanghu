----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ExchangeGiftbagCode]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ExchangeGiftbagCode]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 兑换礼包码
CREATE PROC GSP_GP_ExchangeGiftbagCode
	@dwUserID INT,							-- 用户标识			
	@wReasonID INT,							-- 变更原因
	@strGiftbagCode NVARCHAR(16),			-- 礼包码
	@strClientAddr NVARCHAR(16),			-- 用户地址	
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
	DECLARE @MarketID INT
	DECLARE @Accounts NVARCHAR(31)	

	-- 查询用户
	SELECT @UserID=a.UserID,@Accounts=a.Accounts,@MarketID=a.MarketID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 校验用户
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

	-- 长度判断
	IF LEN(@strGiftbagCode) < 6 OR LEN(@strGiftbagCode) > 12
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1307,"Msg":"礼包码无效，请重新输入！"}' 
		RETURN 4
	END

	-- 变量定义
	DECLARE @CurDateTiem DATETIME
	SET @CurDateTiem=GETDATE()

	-- 礼包码信息
	DECLARE @Count INT
	DECLARE @UsedCount INT
	DECLARE @LimitNum INT
	DECLARE @PlatformID INT
	DECLARE @Status TINYINT
	DECLARE @StartTime DATETIME
	DECLARE @ExpiresTime DATETIME
	SELECT @Count=Count, @UsedCount=UsedCount, @LimitNum=LimitNum, @PlatformID=PlatformID, @Status=Status, @StartTime=StartTime, @ExpiresTime=ExpiresTime
	FROM LHInsureDB.dbo.GiftbagCode WHERE GiftbagCodeID=@strGiftbagCode

	-- 存在判断
	IF @Count IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1308,"Msg":"礼包码不正确，请重新输入！"}'
		RETURN 4
	END

	-- 渠道判断
	IF @PlatformID <> 0 AND @MarketID <> @PlatformID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1308,"Msg":"礼包码不正确，请重新输入！"}'
		RETURN 5
	END

	-- 校验状态
	IF @Status<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1309,"Msg":"礼包码已失效，兑换失败！"}'
		RETURN 5
	END

	-- 过期判断
	IF @CurDateTiem < @StartTime OR @CurDateTiem > @ExpiresTime
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1309,"Msg":"礼包码已失效，兑换失败！"}'
		RETURN 6
	END

	-- 总数判断
	IF @UsedCount >= @Count
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1310,"Msg":"此礼包码已兑换完！"}'
		RETURN 7
	END

	-- 用户领取次数
	DECLARE @UserUsedCount INT
	SELECT @UserUsedCount=COUNT(1) FROM LHInsureDB.dbo.GiftbagCodeExRecord WHERE GiftbagCodeID=@strGiftbagCode AND UserID=@dwUserID

	-- 调整变量
	IF @UserUsedCount IS NULL SET @UserUsedCount=0

	-- 总数判断
	IF @UserUsedCount >= @LimitNum
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1311,"Msg":"此礼包码您已兑换完！"}'
		RETURN 8
	END

	-- 变量定义
	DECLARE @Index INT
	DECLARE @LoopCount INT	
	DECLARE @ResidueCount INT	
	DECLARE @CurrentTime INT

	-- 物品信息
	DECLARE @IncGoodsID INT
	DECLARE @IncGoodsLevel INT
	DECLARE @IncGoodsCount INT
	DECLARE @IncGoodsIndate INT		

	-- 设置时间
	SET @CurrentTime=dbo.LHF_GetTimestamp()

	-- 表变量定义
	DECLARE @BagGoodsList TABLE (ID INT identity(1,1),GoodsID SMALLINT,GoodsLevel SMALLINT,GoodsCount INT,ResidueCount INT,GoodsIndate INT)

	-- 插入记录
	INSERT @BagGoodsList (GoodsID,GoodsLevel,GoodsCount,ResidueCount,GoodsIndate)
	SELECT GoodsID, 1, GoodsNumber, 0, Indate FROM LHInsureDB.dbo.GiftbagCodeGoods WHERE GiftbagCodeID=@strGiftbagCode
	
	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN
	
	-- 处理物品
	SELECT @Index=1,@LoopCount=COUNT(1) FROM @BagGoodsList	
	
	-- 循环处理
	WHILE @Index<=@LoopCount
	BEGIN	
		-- 查询信息
		SELECT @IncGoodsID=GoodsID,@IncGoodsLevel=GoodsLevel,@IncGoodsCount=GoodsCount,@IncGoodsIndate=GoodsIndate FROM @BagGoodsList WHERE ID=@Index		

		-- 校验物品类型
		IF dbo.LHF_IsMoneyGoods(@IncGoodsID) = 1
		BEGIN
			-- 写入财富
			EXEC GSP_GP_WriteGameWealth @dwUserID,@IncGoodsID,@wReasonID,@IncGoodsCount

		END ELSE
		BEGIN
			-- 添加物品
			EXEC @ResidueCount = GSP_GP_AppendGoodsItem @dwUserID,@wReasonID,@IncGoodsID,@IncGoodsLevel,@IncGoodsCount,@IncGoodsIndate,@CurrentTime

			-- 剩余处理
			IF @ResidueCount>0
			BEGIN
				-- 更新剩余
				UPDATE @BagGoodsList SET ResidueCount=@ResidueCount WHERE ID=@Index				
			END		
		END
		
		-- 更新索引
		SET @Index=@Index+1
	END	

	-- 更新次数
	UPDATE LHInsureDB.dbo.GiftbagCode SET UsedCount=UsedCount+1 WHERE GiftbagCodeID=@strGiftbagCode

	-- 记录次数
	DECLARE @RecordID INT
	INSERT INTO LHInsureDB.dbo.GiftbagCodeExRecord(GiftbagCodeID, UserID, Accounts, GoodsArrivalStatus, GoodsArrivalTime, ClientIP)
	VALUES(@strGiftbagCode, @dwUserID, @Accounts, 1, @CurDateTiem, @strClientAddr)

	-- 记录ID
	SELECT @RecordID=SCOPE_IDENTITY()

	-- 记录次数
	INSERT INTO LHInsureDB.dbo.GiftbagCodeExGoods([RecordID], [GoodsID], [GoodsNumber], [Indate])
	SELECT @RecordID, GoodsID, GoodsNumber, Indate FROM LHInsureDB.dbo.GiftbagCodeGoods WHERE GiftbagCodeID=@strGiftbagCode

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 剩余物品
	DECLARE @GiftBagGoodsList NVARCHAR(MAX)	

	-- 构造JSON
	SET @GiftBagGoodsList=(SELECT GoodsID, GoodsLevel AS Level, GoodsCount AS Count, GoodsIndate AS Indate FROM @BagGoodsList FOR JSON AUTO)

	-- 调整变量	
	IF @GiftBagGoodsList IS NULL SET @GiftBagGoodsList=N''

	-- 抛出记录
	SELECT @GiftBagGoodsList AS GiftBagGoodsList
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------