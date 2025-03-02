
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryGameWealth]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryGameWealth]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_WriteGameWealth]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_WriteGameWealth]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_WriteGameWealth]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_WriteGameWealth]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 查询财富
CREATE PROC GSP_GP_QueryGameWealth

	-- 用户信息
	@dwUserID INT								-- 用户标识	

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询房卡
	DECLARE @UserCard BIGINT
	SELECT @UserCard=Score FROM LHGameBattleDB.dbo.GameCardInfo(NOLOCK) WHERE UserID=@dwUserID
	
	-- 查询金币
	DECLARE @UserGold BIGINT		
	SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 调整变量
	IF @UserGold IS NULL SET @UserGold=0
	IF @UserCard IS NULL SET @UserCard=0	
	
	-- 抛出记录
	SELECT @UserCard AS UserCard, @UserGold AS UserGold

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 游戏财富
CREATE PROC GSP_GP_WriteGameWealth

	-- 用户信息
	@dwUserID INT,								-- 用户标识

	-- 财富信息
	@wMoneyID SMALLINT,							-- 财富标识
	@wReason SMALLINT,							-- 变更原因	
	@lVariationScore BIGINT						-- 变更分数

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 日期标识
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 查询用户
	DECLARE @GameID INT
	SELECT @GameID=GameID FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 校验用户
	IF @GameID IS NULL
	BEGIN
		RETURN 1
	END

	-- 金币更新
	IF @wMoneyID=1
	BEGIN
		-- 查询金币
		DECLARE @BeforeGold BIGINT		
		SELECT @BeforeGold=Score FROM  LHGameGoldDB.dbo.GameScoreInfo WITH(UPDLOCK) WHERE UserID=@dwUserID
		
		-- 调整变量
		IF @BeforeGold IS NULL
		BEGIN
			-- 设置变量
			SET @BeforeGold=0			

			-- 插入数据
			INSERT INTO LHGameGoldDB.dbo.GameScoreInfo (UserID,Score) VALUES (@dwUserID,0)
		END

		-- 负分校验
		IF @BeforeGold+@lVariationScore<0
		BEGIN
			RETURN 1
		END
		
		-- 更新金币
		UPDATE LHGameGoldDB.dbo.GameScoreInfo SET Score=Score+@lVariationScore WHERE UserID=@dwUserID

		-- 变更记录
		INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID, GameID, TypeID, BeforeGold, ChangeGold, CollectClientIP, CollectDateTime)
		VALUES (@dwUserID, @GameID, @wReason, @BeforeGold, @lVariationScore, N'------------', GetDate())
	END	

	-- 房卡更新
	IF @wMoneyID=2
	BEGIN
		-- 查询房卡
		DECLARE @BeforeCard BIGINT
		SELECT @BeforeCard=Score FROM LHGameBattleDB.dbo.GameCardInfo WITH(UPDLOCK)  WHERE UserID=@dwUserID

		-- 调整变量
		IF @BeforeCard IS NULL 
		BEGIN
			SET @BeforeCard=0
			INSERT INTO LHGameBattleDB.dbo.GameCardInfo (UserID,Score) VALUES (@dwUserID,0)
		END	

		-- 负分校验
		IF @BeforeCard+@lVariationScore<0
		BEGIN
			RETURN 1
		END

		-- 更新房卡
		UPDATE LHGameBattleDB.dbo.GameCardInfo SET Score+=@lVariationScore WHERE UserID=@dwUserID

		-- 变更记录
		INSERT INTO LHGameRecordDB.dbo.RecordChangeCard(UserID, GameID, TypeID, BeforeCard, ChangeCard,CollectClientIP, CollectDateTime)
		VALUES (@dwUserID, @GameID, @wReason, @BeforeCard, @lVariationScore,  N'-----------', GetDate())
	END	
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 游戏财富
CREATE PROC GSP_GR_WriteGameWealth

	-- 用户信息
	@dwUserID INT,								-- 用户标识

	-- 财富信息
	@wMoneyID SMALLINT,							-- 财富标识
	@wReason SMALLINT,							-- 变更原因	
	@lVariationScore BIGINT						-- 变更分数

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 结余金额
DECLARE @BalanceAmount BIGINT

-- 执行逻辑
BEGIN

	-- 日期标识
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 查询用户
	DECLARE @GameID INT
	SELECT @GameID=GameID FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 校验用户
	IF @GameID IS NULL
	BEGIN
		RETURN 1
	END

	-- 金币更新
	IF @wMoneyID=1
	BEGIN
		-- 查询金币
		DECLARE @BeforeGold BIGINT		
		SELECT @BeforeGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo WITH(UPDLOCK) WHERE UserID=@dwUserID
		
		-- 调整变量
		IF @BeforeGold IS NULL
		BEGIN
			-- 设置变量
			SET @BeforeGold=0			

			-- 插入数据
			INSERT INTO LHGameGoldDB.dbo.GameScoreInfo (UserID,Score) VALUES (@dwUserID,0)
		END

		-- 负分校验
		IF @BeforeGold+@lVariationScore<0
		BEGIN
			RETURN 1
		END
		
		-- 更新金币
		UPDATE LHGameGoldDB.dbo.GameScoreInfo SET Score=Score+@lVariationScore WHERE UserID=@dwUserID

		-- 变更记录
		INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID, GameID, TypeID, BeforeGold, ChangeGold, CollectClientIP, CollectDateTime)
		VALUES (@dwUserID, @GameID, @wReason, @BeforeGold, @lVariationScore, N'------------', GetDate())

		SET @BalanceAmount=@BeforeGold+@lVariationScore
	END	

	-- 房卡更新
	IF @wMoneyID=2
	BEGIN
		-- 查询房卡
		DECLARE @BeforeCard BIGINT
		SELECT @BeforeCard=Score FROM LHGameBattleDB.dbo.GameCardInfo WITH(UPDLOCK)  WHERE UserID=@dwUserID

		-- 调整变量
		IF @BeforeCard IS NULL 
		BEGIN
			SET @BeforeCard=0
			INSERT INTO LHGameBattleDB.dbo.GameCardInfo (UserID,Score) VALUES (@dwUserID,0)
		END	

		-- 负分校验
		IF @BeforeCard+@lVariationScore<0
		BEGIN
			RETURN 1
		END

		-- 更新房卡
		UPDATE LHGameBattleDB.dbo.GameCardInfo SET Score+=@lVariationScore WHERE UserID=@dwUserID

		-- 变更记录
		INSERT INTO LHGameRecordDB.dbo.RecordChangeCard (UserID,GameID,TypeID,KindID,ClubID,BeforeCard,ChangeCard,CollectClientIP,CollectDateTime)
		VALUES (@dwUserID,@GameID,@wReason,0,0,@BeforeCard,@lVariationScore,N'--------------',GETDATE())

		SET @BalanceAmount=@BeforeCard+@lVariationScore
	END		

	-- 默认处理
	IF @BalanceAmount IS NULL SET @BalanceAmount=0

	-- 抛出记录
	SELECT @BalanceAmount AS BalanceAmount

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------