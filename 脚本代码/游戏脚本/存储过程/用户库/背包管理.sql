----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AppendGoodsItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AppendGoodsItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_RemoveGoodsItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_RemoveGoodsItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UserQueryGoods]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UserQueryGoods]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UserUpdateGoods]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UserUpdateGoods]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UserRecordGoods]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UserRecordGoods]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 添加物品
CREATE PROC GSP_GP_AppendGoodsItem
	@dwUserID INT,							-- 用户标识		
	@wReason SMALLINT,						-- 添加原因
	@wGoodsID SMALLINT,						-- 物品标识
	@wGoodsLevel SMALLINT,					-- 物品等级
	@dwIncCount INT,						-- 增加数量
	@dwIncIndate INT,						-- 增加有效期		
	@dwTimestamp INT						-- 时间戳	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 自动回滚
SET XACT_ABORT ON

-- 执行逻辑
BEGIN

	-- 物品属性
	DECLARE @HasIndate TINYINT
	DECLARE @GoodsKind SMALLINT
	SELECT @HasIndate=HasIndate,@GoodsKind=GoodsKind FROM LHPlatformDB.dbo.GameGoodsInfo(NOLOCK) WHERE GoodsID=@wGoodsID

	-- 物品不存在
	IF @GoodsKind IS NULL 
	BEGIN
		RETURN @dwIncCount
	END

	-- 调整等级
	IF @GoodsKind=11 AND @wGoodsLevel=0
	BEGIN
		SET @wGoodsLevel=1
	END

	-- 物品信息
	DECLARE @GoodsCount INT	
	DECLARE @GoodsIndate INT		
	DECLARE @GoodsExpireTime INT	

	-- 物品信息	
	DECLARE @BeforeCount INT
	DECLARE @BeforeIndate INT
	DECLARE @BeforeExpireTime INT
	SELECT @BeforeCount=GoodsCount,@BeforeIndate=GoodsIndate,@BeforeExpireTime=ExpireTime FROM AccountsPack WITH(UPDLOCK) WHERE UserID=@dwUserID AND GoodsID=@wGoodsID

	-- 更新物品
	IF @BeforeCount IS NOT NULL
	BEGIN	
		-- 判断有效期
		IF @HasIndate=1
		BEGIN		
			-- 调整数量
			SET @GoodsCount=1
			SET @GoodsExpireTime=0

			-- 数量判断
			IF @BeforeCount=0
			BEGIN
				IF @dwIncIndate=0 SET @GoodsIndate=0
				IF @dwIncIndate>0 
				BEGIN
					SET @GoodsIndate=@dwIncIndate
					SET @GoodsExpireTime=@dwTimestamp+@dwIncIndate
				END
			END ELSE
			BEGIN
				IF @BeforeIndate=0 
				BEGIN
					SET @GoodsIndate=0
					SET @GoodsExpireTime=0					
				END ELSE
				BEGIN
					IF @dwIncIndate=0 SET @GoodsIndate=0
					IF @dwIncIndate>0 
					BEGIN
						SET @GoodsIndate=@BeforeIndate+@dwIncIndate
						SET @GoodsExpireTime=@BeforeExpireTime+@dwIncIndate
					END
				END
			END			
		END ELSE
		BEGIN
			SET @GoodsIndate=0
			SET @GoodsExpireTime=0
			SET @GoodsCount=@BeforeCount+@dwIncCount
		END	
		
		-- 更新物品
		UPDATE AccountsPack SET GoodsCount=@GoodsCount,GoodsLevel=@wGoodsLevel,GoodsIndate=@GoodsIndate,ExpireTime=@GoodsExpireTime WHERE UserID=@dwUserID AND GoodsID=@wGoodsID						
	END ELSE
	BEGIN
		-- 判断有效期
		IF @HasIndate=1
		BEGIN		
			-- 调整数量
			SET @GoodsCount=1
			SET @GoodsExpireTime=0
			
			-- 调整有效期
			IF @dwIncIndate=0 SET @GoodsIndate=0
			IF @dwIncIndate>0 
			BEGIN
				SET @GoodsIndate=@dwIncIndate
				SET @GoodsExpireTime=@dwTimestamp+@dwIncIndate
			END			
		END ELSE
		BEGIN
			SET @GoodsIndate=0
			SET @GoodsExpireTime=0
			SET @GoodsCount=@dwIncCount
		END

		-- 添加物品
		INSERT INTO AccountsPack(UserID, GoodsID, GoodsKind, GoodsLevel, GoodsCount, GoodsIndate, BindState, ExpireTime, CollectTime)
		VALUES (@dwUserID, @wGoodsID, @GoodsKind, @wGoodsLevel, @GoodsCount, @GoodsIndate, 0, @GoodsExpireTime, GetDate())	
	END

	-- 调整变量	
	IF @BeforeCount IS NULL SET @BeforeCount=0
	IF @BeforeIndate IS NULL SET @BeforeIndate=0
	IF @BeforeExpireTime IS NULL SET @BeforeExpireTime=0

	-- 变更记录
	IF @GoodsCount-@BeforeCount<>0
	BEGIN
		INSERT INTO LHGameRecordDB.dbo.RecordChangeGoods(UserID, GoodsID, GoodsKind, TypeID, RelationID, BeforeCount, ChangeCount, CollectClientIP, CollectDateTime)
		VALUES (@dwUserID, @wGoodsID, @GoodsKind, @wReason, 0, @BeforeCount, @GoodsCount-@BeforeCount, N'------------' ,GetDate())
	END
	
	RETURN 0

END 

GO

----------------------------------------------------------------------------------------------------

-- 移除物品
CREATE PROC GSP_GP_RemoveGoodsItem
	@dwUserID INT,							-- 用户标识			
	@wReason SMALLINT,						-- 添加原因
	@wGoodsID SMALLINT,						-- 物品标识
	@dwDesCount INT							-- 减少数量		
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 自动回滚
SET XACT_ABORT ON

-- 执行逻辑
BEGIN

	-- 物品信息	
	DECLARE @BeforeCount INT	
	DECLARE @GoodsKind SMALLINT
	SELECT @GoodsKind=GoodsKind,@BeforeCount=GoodsCount FROM AccountsPack WITH(UPDLOCK) WHERE UserID=@dwUserID AND GoodsID=@wGoodsID

	-- 调整数据
	IF @BeforeCount IS NULL SET @BeforeCount=0
	
	-- 校验数量
	IF @dwDesCount>@BeforeCount
	BEGIN
		RETURN 1
	END
	
	-- 更新数量
	UPDATE AccountsPack SET GoodsCount=GoodsCount-@dwDesCount WHERE UserID=@dwUserID AND GoodsID=@wGoodsID						

	-- 变更记录
	INSERT INTO LHGameRecordDB.dbo.RecordChangeGoods(UserID, GoodsID, GoodsKind, TypeID, RelationID, BeforeCount, ChangeCount, CollectDateTime)
	VALUES (@dwUserID, @wGoodsID, @GoodsKind, @wReason, 0, @BeforeCount, -@dwDesCount, GetDate())

END 

GO

----------------------------------------------------------------------------------------------------

-- 查询物品
CREATE PROC GSP_GP_UserQueryGoods
	@dwUserID INT,							-- 用户标识	
	@szGoodsIDList NVARCHAR(256)=''			-- 物品标识		
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 加载物品
	IF ISJSON(@szGoodsIDList)=0
	BEGIN
		-- 查询物品
		SELECT * FROM AccountsPack(NOLOCK) WHERE UserID=@dwUserID AND GoodsCount>0

	END ELSE
	BEGIN
		SELECT * FROM AccountsPack(NOLOCK) WHERE UserID=@dwUserID AND GoodsID IN (SELECT VALUE FROM OPENJSON(@szGoodsIDList))
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 记录物品
CREATE PROC GSP_GP_UserRecordGoods
	@dwUserID INT,							-- 用户标识	
	@strRecordList NVARCHAR(MAX)			-- 记录列表		
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 自动回滚
SET XACT_ABORT ON

-- 执行逻辑
BEGIN
	-- 插入记录
	IF ISJSON(@strRecordList)>0
	BEGIN
		INSERT INTO LHGameRecordDB.dbo.RecordChangeGoods(
			UserID, 
			GoodsID, 
			GoodsKind, 
			TypeID, 
			RelationID, 
			BeforeCount, 
			ChangeCount, 
			CollectDateTime)
		SELECT 
			@dwUserID, 
			JSON_VALUE(VALUE,'$.GoodsID'), 
			JSON_VALUE(VALUE,'$.GoodsKind'),
			JSON_VALUE(VALUE,'$.Reason'), 
			0, 
			JSON_VALUE(VALUE,'$.BeforeCount'), 
			JSON_VALUE(VALUE,'$.VariationCount'), 
			JSON_VALUE(VALUE,'$.VariationTime')
		FROM OPENJSON(@strRecordList)
		ORDER BY JSON_VALUE(VALUE,'$.VariationTime')
	END

END 

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 更新物品
CREATE PROC GSP_GP_UserUpdateGoods
	@dwUserID INT,							-- 用户标识		
	@strGoodsList NVARCHAR(MAX)				-- 物品列表	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 校验Json
	IF ISJSON(@strGoodsList)>0
	BEGIN
		-- 物品列表
		DECLARE @GoodsList TABLE (GoodsID SMALLINT,GoodsLevel SMALLINT,GoodsKind SMALLINT,GoodsCount INT,GoodsIndate INT,BindState TINYINT,ExpireTime INT)

		-- 插入记录
		INSERT @GoodsList SELECT JSON_VALUE(VALUE,'$.GoodsID'),ISNULL(JSON_VALUE(VALUE,'$.GoodsLevel'),0), JSON_VALUE(VALUE,'$.GoodsKind'), JSON_VALUE(VALUE,'$.GoodsCount'), JSON_VALUE(VALUE,'$.GoodsIndate'), 
		JSON_VALUE(VALUE,'$.BindState'),JSON_VALUE(VALUE,'$.ExpireTime') FROM OPENJSON(@strGoodsList)

		-- 更新物品
		UPDATE AccountsPack SET GoodsLevel=b.GoodsLevel,GoodsCount=b.GoodsCount, GoodsIndate=b.GoodsIndate,BindState=b.BindState,ExpireTime=b.ExpireTime 
		FROM AccountsPack a, @GoodsList b WHERE a.UserID=@dwUserID AND a.GoodsID=b.GoodsID

		-- 删除现有
		DELETE @GoodsList WHERE GoodsID IN (SELECT GoodsID FROM AccountsPack(NOLOCK) WHERE UserID=@dwUserID)

		-- 插入物品
		INSERT INTO AccountsPack(UserID, GoodsID, GoodsKind, GoodsLevel, GoodsCount, GoodsIndate, BindState, ExpireTime, CollectTime) 
		SELECT @dwUserID, GoodsID, GoodsKind, GoodsLevel, GoodsCount, GoodsIndate, BindState, ExpireTime, GetDate() FROM @GoodsList 
	END

END 

RETURN 0

GO

----------------------------------------------------------------------------------------------------