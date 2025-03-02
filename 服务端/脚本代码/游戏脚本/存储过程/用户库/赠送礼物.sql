USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_SendGift]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_SendGift]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadGift]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadGift]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_SendGift]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_SendGift]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_GiftStatusUpdate]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_GiftStatusUpdate]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 赠送礼物
CREATE PROC GSP_GP_SendGift
	@dwUserID INT,							 -- 用户标识		
	@dwTargetUserID INT,					 -- 目标用户	
	@dwGoodsID INT,							 -- 物品标识	
	@dwGoodsCount INT,						 -- 物品数量	
	@szClientIP	NVARCHAR(15),				 -- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT	 -- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 判断自己
	IF @dwUserID=@dwTargetUserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1319,"Msg":"抱歉，您不能给自己送礼物，赠送失败！"}'	
		RETURN 5
	END

	-- 查询玩家
	IF NOT EXISTS(SELECT * FROM AccountsInfo(NOLOCK) WHERE UserID=@dwTargetUserID)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1314,"Msg":"抱歉，您要赠送的玩家不存在，赠送失败！"}'	
		RETURN 1
	END

	-- 变量定义
	DECLARE @RecordID BIGINT
	DECLARE @PriceKind TINYINT
	DECLARE @PriceCount BIGINT
	DECLARE @MyLoveLiness INT
	DECLARE @TargetLoveLiness INT
	DECLARE @LoveLiness INT
	DECLARE @SendType TINYINT
	
	-- 设置变量
	SET @RecordID=0
	
	-- 查询物品
	SELECT @PriceKind=MoneyID, @PriceCount=Price, @LoveLiness=LoveLiness, @SendType=SendType FROM LHPlatformDB.dbo.GameGiftInfo(NOLOCK) WHERE GoodsID=@dwGoodsID
	IF @PriceKind IS NULL
	BEGIN 
		SET @strErrorDescribe=N'{"Idx":1313,"Msg":"抱歉，系统未查询到该礼物信息，赠送失败！"}'	
		RETURN 2
	END 
	
	-- 变量定义
	DECLARE @Score BIGINT

	-- 判断自身财富
	IF @PriceKind=1
	BEGIN
		SELECT @Score=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID
		IF @Score<@PriceCount*@dwGoodsCount
		BEGIN 
			SET @strErrorDescribe=N'{"Idx":1316,"Msg":"抱歉，您的金币不足，赠送失败！"}'	
			RETURN 4
		END
	END
	
	-- 更新财富 @Reason=16 -> 礼物赠送
	DECLARE @Price BIGINT
	SET @Price=-@PriceCount*@dwGoodsCount
	EXEC GSP_GP_WriteGameWealth @dwUserID,@PriceKind,16,@Price
	
	-- 更新魅力值
	UPDATE AccountsInfo SET LoveLiness=LoveLiness+@LoveLiness WHERE UserID IN (@dwUserID, @dwTargetUserID)
	
	-- 查询魅力值
	SELECT @MyLoveLiness=LoveLiness FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
	SELECT @TargetLoveLiness=LoveLiness FROM AccountsInfo(NOLOCK) WHERE UserID=@dwTargetUserID
	
	-- 日期标识
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	
	-- 记录自己魅力值
	UPDATE LHGameRecordDB.dbo.ReportDayUser SET LoveLiness=LoveLiness+@LoveLiness WHERE UserID=@dwUserID AND DateID=@DateID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO LHGameRecordDB.dbo.ReportDayUser(DateID,UserID,PayCount,PayAmount,LastPayAmount,LastPayDate,LoveLiness,ReportDateTime) 
		VALUES (@DateID,@dwUserID,0,0,0,GETDATE(),@LoveLiness,GETDATE())
	END
	
	-- 记录对方魅力值
	UPDATE LHGameRecordDB.dbo.ReportDayUser SET LoveLiness=LoveLiness+@LoveLiness WHERE UserID=@dwTargetUserID AND DateID=@DateID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO LHGameRecordDB.dbo.ReportDayUser(DateID,UserID,PayCount,PayAmount,LastPayAmount,LastPayDate,LoveLiness,ReportDateTime) 
		VALUES (@DateID,@dwTargetUserID,0,0,0,GETDATE(),@LoveLiness,GETDATE())
	END
	
	-- 插入记录
	INSERT INTO LHGameRecordDB.dbo.RecordUserGift(UserID,TargetUserID,GoodsID,GoodsCount,LoveLiness,SendType,LoadStatus,CollectClientIP,CollectDateTime)
	VALUES (@dwUserID,@dwTargetUserID,@dwGoodsID,@dwGoodsCount,@LoveLiness,1,0,@szClientIP,GETDATE())
	
	-- 记录标识
	SET @RecordID=SCOPE_IDENTITY()

	-- 输出变量
	SELECT @RecordID AS RecordID, @LoveLiness AS LoveLiness, @SendType AS SendType, @MyLoveLiness AS MyLoveLiness, @TargetLoveLiness AS TargetLoveLiness

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载礼物
CREATE PROC GSP_GP_LoadGift
	@dwUserID INT,							 -- 用户标识		
	@strErrorDescribe NVARCHAR(127) OUTPUT	 -- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 表变量定义
	--DECLARE @GiftList TABLE (RecordID INT,UserID INT,GoodsID SMALLINT,GoodsCount SMALLINT,LoveLiness INT,CollectDateTime DATETIME,NickName NVARCHAR(31))
	
	-- 插入数据
	--INSERT INTO @GiftList (RecordID,UserID,GoodsID,GoodsCount,LoveLiness,CollectDateTime,NickName)
	--SELECT b.RecordID,b.UserID,b.GoodsID,b.GoodsCount,b.LoveLiness,b.CollectDateTime,(SELECT NickName FROM AccountsInfo(NOLOCK) a WHERE a.UserID=b.UserID) AS NickName 
	--FROM LHGameRecordDB.dbo.RecordUserGift b WHERE b.TargetUserID=@dwUserID AND b.LoadStatus=0 AND b.SendType=1

	-- 清理记录
	--UPDATE LHGameRecordDB.dbo.RecordUserGift SET LoadStatus=1 WHERE TargetUserID=@dwUserID
	
	--SELECT RecordID,UserID,GoodsID,GoodsCount,LoveLiness,CollectDateTime,NickName FROM @GiftList
	
	SELECT b.RecordID,b.UserID,b.GoodsID,b.GoodsCount,b.LoveLiness,b.CollectDateTime,(SELECT NickName FROM AccountsInfo(NOLOCK) a WHERE a.UserID=b.UserID) AS NickName 
	FROM LHGameRecordDB.dbo.RecordUserGift b WHERE b.TargetUserID=@dwUserID AND b.LoadStatus=0 AND b.SendType=1
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 礼物状态更新
CREATE PROC GSP_GP_GiftStatusUpdate
	@dwUserID INT,							 -- 用户标识		
	@dwRecordID INT,						 -- 记录标识		
	@strErrorDescribe NVARCHAR(127) OUTPUT	 -- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 修改记录
	UPDATE LHGameRecordDB.dbo.RecordUserGift SET LoadStatus=1 WHERE RecordID=@dwRecordID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 赠送礼物
CREATE PROC GSP_GR_SendGift
	@dwUserID INT,							 -- 用户标识		
	@dwTargetUserID INT,					 -- 目标用户
	@cbIsGoldRoom TINYINT,					 -- 是否金币房
	@lCellScore BIGINT,					 	 -- 桌子底分
	@lMinTableScore BIGINT,					 -- 桌子最低分数
	@dwGoodsID INT,							 -- 物品标识	
	@dwGoodsCount INT,						 -- 物品数量	
	@szClientIP	NVARCHAR(15),				 -- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT	 -- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 判断自己
	IF @dwUserID=@dwTargetUserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1319,"Msg":"抱歉，您不能给自己送礼物，赠送失败！"}'	
		RETURN 5
	END

	-- 初始配置
	DECLARE @MinRetainGold_1 BIGINT
	DECLARE @MinRetainGold_2 BIGINT
	DECLARE @MinRetainGold_3 BIGINT
	
	SET @MinRetainGold_1=3000000
	SET @MinRetainGold_2=5000000
	SET @MinRetainGold_3=12000000

	-- 查询玩家
	IF NOT EXISTS(SELECT * FROM AccountsInfo(NOLOCK) WHERE UserID=@dwTargetUserID)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1314,"Msg":"抱歉，您要赠送的玩家不存在，赠送失败！"}'	
		RETURN 1
	END

	-- 变量定义
	DECLARE @RecordID BIGINT
	DECLARE @PriceKind TINYINT
	DECLARE @PriceCount BIGINT
	DECLARE @LoveLiness INT
	DECLARE @MyLoveLiness INT
	DECLARE @TargetLoveLiness INT
	
	-- 设置变量
	SET @RecordID=0
	
	-- 查询物品
	SELECT @PriceKind=MoneyID, @PriceCount=Price, @LoveLiness=LoveLiness FROM LHPlatformDB.dbo.GameGiftInfo(NOLOCK) WHERE GoodsID=@dwGoodsID
	IF @PriceKind IS NULL
	BEGIN 
		SET @strErrorDescribe=N'{"Idx":1313,"Msg":"抱歉，系统未查询到该礼物信息，赠送失败！"}'	
		RETURN 2
	END 
	
	-- 变量定义
	DECLARE @Score BIGINT

	-- 判断自身财富
	IF @PriceKind=1
	BEGIN
		SELECT @Score=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID
			
		-- 金币房判断底分
		IF @cbIsGoldRoom=1
		BEGIN
			-- 设置保留金币
			DECLARE @MinRetainGold BIGINT
			IF @lCellScore=10000 BEGIN SET @MinRetainGold=@MinRetainGold_1 END
			ELSE IF @lCellScore=20000 BEGIN SET @MinRetainGold=@MinRetainGold_2 END
			ELSE IF @lCellScore=50000 BEGIN SET @MinRetainGold=@MinRetainGold_3 END
			ELSE BEGIN SET @MinRetainGold=@lMinTableScore END
				
			IF @Score-@PriceCount*@dwGoodsCount<@MinRetainGold
			BEGIN 
				SET @strErrorDescribe=N'{"Idx":1317,"Msg":"抱歉，您需要保留'+CAST(@MinRetainGold AS NVARCHAR(16))+'的黄金，由于黄金不足，赠送失败！","Ops":["'+CAST(@MinRetainGold/1000 AS NVARCHAR(16))+'"]}'	
				RETURN 6
			END
		END
		ELSE 
		BEGIN
			IF @Score<@PriceCount*@dwGoodsCount
			BEGIN 
				SET @strErrorDescribe=N'{"Idx":1316,"Msg":"抱歉，您的黄金不足，赠送失败！"}'	
				RETURN 4
			END
		END
			
	END ELSE
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1318,"Msg":"不支持的支付方式，请联系管理员！"}'	
		RETURN 5
	END	
	
	-- 更新财富 @Reason=16 -> 礼物赠送
	DECLARE @Price BIGINT
	SET @Price=-@PriceCount*@dwGoodsCount
	EXEC GSP_GP_WriteGameWealth @dwUserID,@PriceKind,16,@Price
	
	-- 更新魅力值
	UPDATE AccountsInfo SET LoveLiness=LoveLiness+@LoveLiness WHERE UserID IN (@dwUserID, @dwTargetUserID)
	
	-- 查询魅力值
	SELECT @MyLoveLiness=LoveLiness FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
	SELECT @TargetLoveLiness=LoveLiness FROM AccountsInfo(NOLOCK) WHERE UserID=@dwTargetUserID
	
	-- 日期标识
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	
	-- 记录自己魅力值
	UPDATE LHGameRecordDB.dbo.ReportDayUser SET LoveLiness=LoveLiness+@LoveLiness WHERE UserID=@dwUserID AND DateID=@DateID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO LHGameRecordDB.dbo.ReportDayUser(DateID,UserID,PayCount,PayAmount,LastPayAmount,LastPayDate,LoveLiness,ReportDateTime) 
		VALUES (@DateID,@dwUserID,0,0,0,GETDATE(),@LoveLiness,GETDATE())
	END
	
	-- 记录对方魅力值
	UPDATE LHGameRecordDB.dbo.ReportDayUser SET LoveLiness=LoveLiness+@LoveLiness WHERE UserID=@dwTargetUserID AND DateID=@DateID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO LHGameRecordDB.dbo.ReportDayUser(DateID,UserID,PayCount,PayAmount,LastPayAmount,LastPayDate,LoveLiness,ReportDateTime) 
		VALUES (@DateID,@dwTargetUserID,0,0,0,GETDATE(),@LoveLiness,GETDATE())
	END
	
	-- 插入记录
	INSERT INTO LHGameRecordDB.dbo.RecordUserGift(UserID,TargetUserID,GoodsID,GoodsCount,LoveLiness,SendType,LoadStatus,CollectClientIP,CollectDateTime)
	VALUES (@dwUserID,@dwTargetUserID,@dwGoodsID,@dwGoodsCount,@LoveLiness,2,1,@szClientIP,GETDATE())
	
	-- 输出变量
	SELECT @PriceKind AS MoneyID, -@Price AS Price, @LoveLiness AS LoveLiness, @MyLoveLiness AS MyLoveLiness, @TargetLoveLiness AS TargetLoveLiness

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
