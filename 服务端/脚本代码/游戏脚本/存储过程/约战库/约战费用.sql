USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ReturnBattleCost]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ReturnBattleCost]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DeductBattleCost]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DeductBattleCost]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DeductBattleCostAA]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DeductBattleCostAA]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 退还费用
CREATE PROC GSP_GR_ReturnBattleCost
	@wKindID INT,								-- 游戏标识
	@wServerID INT,								-- 房间标识
	@dwClubID INT,								-- 俱乐部标识
	@dwPayUserID INT,							-- 付费玩家	
	@lReturnCount BIGINT,						-- 退还数量
	@cbCurrencyKind	TINYINT,					-- 货币类型
	@strErrorDescribe NVARCHAR(127)	OUTPUT		-- 错误描述
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

DECLARE @UserGold BIGINT
DECLARE @UserCard BIGINT

-- 执行逻辑
BEGIN
	
	-- 用户查询
	DECLARE @GameID INT                                                                                                                                                                                                         
	SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwPayUserID
	
	-- 金币类型
	IF @cbCurrencyKind=1
	BEGIN
		-- 查询金币	
		SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwPayUserID

		-- 金币校验
		IF @UserGold IS NULL 
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1240,"Msg":"抱歉，系统未查询到您的金币信息，金币返还失败！"}'
			RETURN 1	
		END

		-- 变更记录
		INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID,GameID,TypeID,BeforeGold,ChangeGold,CollectClientIP,CollectDateTime)
		VALUES (@dwPayUserID,@GameID,15,@UserGold,@lReturnCount,N'0.0.0.0',GETDATE())

		-- 更新金币
		UPDATE LHGameGoldDB.dbo.GameScoreInfo SET Score+=@lReturnCount WHERE UserID=@dwPayUserID
	END

	-- 房卡类型
	IF @cbCurrencyKind=2
	BEGIN
		-- 查询房卡		
		SELECT @UserCard=Score FROM GameCardInfo(NOLOCK) WHERE UserID=@dwPayUserID

		-- 钻石调整
		IF @UserCard IS NULL 
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1240,"Msg":"抱歉，系统未查询到您的房卡信息，房卡返还失败！"}'
			RETURN 1	
		END

		-- 变更记录
		INSERT INTO LHGameRecordDB.dbo.RecordChangeCard (UserID,GameID,TypeID,KindID,ClubID,BeforeCard,ChangeCard,CollectClientIP,CollectDateTime)
		VALUES (@dwPayUserID,@GameID,4,@wKindID,@dwClubID,@UserCard,@lReturnCount,N'--------------',GETDATE())

		-- 更新房卡
		UPDATE GameCardInfo SET Score+=@lReturnCount WHERE UserID=@dwPayUserID
	END			

	-- 查询财富
	SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwPayUserID
	SELECT @UserCard=Score FROM GameCardInfo(NOLOCK) WHERE UserID=@dwPayUserID

	-- 调整数据
	IF @UserGold IS NULL SET @UserGold=0
	IF @UserCard IS NULL SET @UserCard=0		

	-- 抛出数据
	SELECT @dwPayUserID AS UserID, @UserGold AS UserGold, @UserCard AS UserCard	
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- 扣除费用
CREATE PROC GSP_GR_DeductBattleCost
	@dwUserID INT,								-- 用户 I D	
	@wKindID INT,								-- 游戏标识
	@wServerID INT,								-- 房间标识
	@dwClubID INT,								-- 群组标识
	@wPayType SMALLINT,							-- 支付方式
	@lConsumeCount BIGINT,						-- 消耗数量	
	@cbCurrencyKind	TINYINT,					-- 货币类型	
	@strErrorDescribe NVARCHAR(127)	OUTPUT		-- 错误描述
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

DECLARE @UserGold BIGINT
DECLARE @UserCard BIGINT

-- 执行逻辑
BEGIN

	-- 变量定义
	DECLARE @UserID INT
	DECLARE @GameID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- 查询用户
	SELECT @UserID=a.UserID,@GameID=a.GameID,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
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

	-- 俱乐部开房
	IF @dwClubID>0
	BEGIN	
		-- 变量定义
		DECLARE @PayUserID INT
		DECLARE @PayGameID INT
		DECLARE @ClubCreaterID INT
		DECLARE @ClubPayrollerID INT
		
		--查询俱乐部
		SELECT @PayUserID=CreaterID,@ClubCreaterID=CreaterID,@ClubPayrollerID=PayrollerID FROM ClubProperty WHERE ClubID=@dwClubID AND ClubStatus=1

		--俱乐部不存在
		IF @ClubCreaterID IS NULL
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1241,"Msg":"抱歉，系统未查询到相关的茶社信息。 房间创建失败！"}'
			RETURN 1				
		END

		-- 开始事务
		SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
		BEGIN TRAN			
			
		--房卡类型
		IF @cbCurrencyKind=2
		BEGIN
			-- 尝试代付
			IF @ClubPayrollerID>0
			BEGIN
				-- 查询房卡
				SELECT @UserCard=Score FROM GameCardInfo WITH (UPDLOCK) WHERE UserID=@ClubPayrollerID

				-- 调整变量
				IF @UserCard IS NULL SET @UserCard=0

				-- 付费玩家
				SET @PayUserID=@ClubPayrollerID
			END

			-- 房卡校验
			IF @UserCard IS NULL OR @lConsumeCount>@UserCard 
			BEGIN
				-- 查询房卡		
				SELECT @UserCard=Score FROM GameCardInfo WITH (UPDLOCK) WHERE UserID=@ClubCreaterID

				-- 房卡调整
				IF @UserCard IS NULL 
				BEGIN
					-- 事务回滚
					ROLLBACK TRAN
					SET TRANSACTION ISOLATION LEVEL READ COMMITTED

					SET @strErrorDescribe=N'{"Idx":1243,"Msg":"抱歉，当前俱乐部老板身上的房卡不足。房间创建失败，请联系老板。"}'
					RETURN 20
				END

				-- 余额比较
				IF @lConsumeCount>@UserCard
				BEGIN
					-- 事务回滚
					ROLLBACK TRAN
					SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
					-- 错误信息
					SET @strErrorDescribe=N'{"Idx":1243,"Msg":"抱歉，当前俱乐部老板身上的房卡不足。房间创建失败，请联系老板。"}'
					RETURN 20	
				END

				-- 付费玩家
				IF @ClubPayrollerID=0 SET @PayUserID=@ClubCreaterID
			END

			-- 变更记录
			INSERT INTO LHGameRecordDB.dbo.RecordChangeCard (UserID,GameID,TypeID,KindID,ClubID,BeforeCard,ChangeCard,CollectClientIP,CollectDateTime)
			VALUES (@PayUserID,@GameID,3,@wKindID,@dwClubID,@UserCard,-@lConsumeCount,N'--------------',GETDATE())

			-- 更新房卡
			UPDATE GameCardInfo SET Score-=@lConsumeCount WHERE UserID=@PayUserID
		END			

		-- 结束事务
		COMMIT TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 查询财富		
		SELECT @UserCard=Score FROM GameCardInfo(NOLOCK) WHERE UserID=@PayUserID
		SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@PayUserID

		-- 调整变量
		IF @UserGold IS NULL SET @UserGold=0
		IF @UserCard IS NULL SET @UserCard=0

		-- 抛出数据
		SELECT @PayUserID AS PayUserID, @UserGold AS UserGold, @UserCard AS UserCard

	END ELSE
	BEGIN
		
		-- 馆主支付
		IF @wPayType=0x01
		BEGIN
			-- 锁定检测
			DECLARE @OnlineStatus TINYINT		
			SELECT @OnlineStatus=OnlineStatus FROM BattleUserLocker(NOLOCK) WHERE UserID=@dwUserID

			-- 调整状态
			IF @OnlineStatus IS NULL SET @OnlineStatus=0

			IF @OnlineStatus=1
			BEGIN
				SET @strErrorDescribe=N'{"Idx":1244,"Msg":"抱歉！操作失败，如果您正在约战房间请先退出再进行操作！"}'
				RETURN 15
			END
		END

		-- 用户查询
		SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

		-- 金币类型
		IF @cbCurrencyKind=1
		BEGIN
			-- 查询金币	
			SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

			-- 调整金币
			IF @UserGold IS NULL SET @UserGold=0
			
			-- 余额比较
			IF @lConsumeCount>@UserGold
			BEGIN
				SET @strErrorDescribe=N'{"Idx":1245,"Msg":"您的金币携带量不足，房费扣除失败！"}'
				RETURN 20	
			END

			-- 变更记录
			INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID,GameID,TypeID,BeforeGold,ChangeGold,CollectClientIP,CollectDateTime)
			VALUES (@dwUserID,@GameID,14,@UserGold,-@lConsumeCount,N'0.0.0.0',GETDATE())

			-- 更新金币
			UPDATE LHGameGoldDB.dbo.GameScoreInfo SET Score-=@lConsumeCount WHERE UserID=@dwUserID
		END

		-- 房卡类型
		IF @cbCurrencyKind=2
		BEGIN
			-- 查询房卡
			SELECT @UserCard=Score FROM GameCardInfo WHERE UserID=@dwUserID

			-- 房卡调整
			IF @UserCard IS NULL SET @UserCard=0

			-- 余额比较
			IF @lConsumeCount>@UserCard
			BEGIN
				SET @strErrorDescribe=N'{"Idx":1246,"Msg":"您的房卡数量不足，请获取更多房卡后重试！"}'
				RETURN 20	
			END

			-- 变更记录
			INSERT INTO LHGameRecordDB.dbo.RecordChangeCard (UserID,GameID,TypeID,KindID,ClubID,BeforeCard,ChangeCard,CollectClientIP,CollectDateTime)
			VALUES (@dwUserID,@GameID,3,@wKindID,@dwClubID,@UserCard,-@lConsumeCount,N'--------------',GETDATE())

			-- 更新房卡
			UPDATE GameCardInfo SET Score-=@lConsumeCount WHERE UserID=@dwUserID
		END		

		-- 查询财富
		SELECT @UserCard=Score FROM GameCardInfo(NOLOCK) WHERE UserID=@dwUserID
		SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@PayUserID

		-- 调整数据
		IF @UserGold IS NULL SET @UserGold=0
		IF @UserCard IS NULL SET @UserCard=0	

		-- 抛出数据
		SELECT @dwUserID AS PayUserID, @UserGold AS UserGold, @UserCard AS UserCard
	END			
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
-- 扣除费用
CREATE PROC GSP_GR_DeductBattleCostAA
	@strUserList NVARCHAR(512),					-- 用户列表	
	@wKindID INT,								-- 游戏标识
	@wServerID INT,								-- 房间标识	
	@dwClubID INT,								-- 群组标识
	@lConsumeCount INT,							-- 消耗数量	
	@cbCurrencyKind	TINYINT,					-- 货币类型	
	@strErrorDescribe NVARCHAR(127)	OUTPUT		-- 错误描述
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON
SET XACT_ABORT ON

DECLARE @UserID INT
DECLARE @GameID INT
DECLARE @ClientAddr NVARCHAR(15)

-- 执行逻辑
BEGIN

	-- 表定义
	DECLARE @TableUserList TABLE (ID INT PRIMARY KEY IDENTITY(1,1),UserID INT)

	-- 插入数据
	IF ISJSON(@strUserList)>0
	BEGIN
		INSERT INTO @TableUserList(UserID) SELECT VALUE FROM OPENJSON(@strUserList)
	END

	-- 变量定义
	DECLARE @Index INT
	DECLARE @UserCount INT
	
	-- 查询数据
	SELECT @Index=1,@UserCount=COUNT(1) FROM @TableUserList

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 解析数据
	WHILE @Index<=@UserCount
	BEGIN
		-- 查询数据
		SELECT @UserID=UserID FROM @TableUserList WHERE ID=@Index

		-- 金币类型
		IF @cbCurrencyKind=1
		BEGIN
			-- 查询金币	
			DECLARE @UserGold BIGINT
			SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo WITH (UPDLOCK) WHERE UserID=@UserID

			-- 调整金币
			IF @UserGold IS NULL SET @UserGold=0
			
			-- 余额比较
			IF @lConsumeCount>@UserGold
			BEGIN
				-- 事务回滚
				ROLLBACK TRAN
				SET TRANSACTION ISOLATION LEVEL READ COMMITTED

				-- 抛出标识
				SELECT @UserID AS FailUserID

				RETURN 10
			END

			-- 查询游戏ID
			SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@UserID

			-- 变更记录
			INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID,GameID,TypeID,BeforeGold,ChangeGold,CollectClientIP,CollectDateTime)
			VALUES (@UserID,@GameID,14,@UserGold,-@lConsumeCount,N'0.0.0.0',GETDATE())

			-- 更新金币
			UPDATE LHGameGoldDB.dbo.GameScoreInfo SET Score-=@lConsumeCount WHERE UserID=@UserID
		END

		-- 房卡类型
		IF @cbCurrencyKind=2
		BEGIN
			-- 查询房卡
			DECLARE @UserCard BIGINT
			SELECT @UserCard=Score FROM GameCardInfo WITH (UPDLOCK) WHERE UserID=@UserID

			-- 房卡调整
			IF @UserCard IS NULL SET @UserCard=0

			-- 余额比较
			IF @lConsumeCount>@UserCard
			BEGIN
				-- 事务回滚
				ROLLBACK TRAN
				SET TRANSACTION ISOLATION LEVEL READ COMMITTED

				-- 抛出标识
				SELECT @UserID AS FailUserID

				RETURN 10	
			END

			-- 查询游戏ID
			SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@UserID

			-- 变更记录
			INSERT INTO LHGameRecordDB.dbo.RecordChangeCard (UserID,GameID,TypeID,KindID,ClubID,BeforeCard,ChangeCard,CollectClientIP,CollectDateTime)
			VALUES (@UserID,@GameID,3,@wKindID,@dwClubID,@UserCard,-@lConsumeCount,N'--------------',GETDATE())

			-- 更新房卡
			UPDATE GameCardInfo SET Score-=@lConsumeCount WHERE UserID=@UserID
		END
		
		SET @Index+=1
	END

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 抛出财富
	SELECT a.UserID, ISNULL(b.Score,0) AS UserCard, ISNULL(c.Score,0) AS UserGold  FROM @TableUserList a LEFT JOIN GameCardInfo b ON b.UserID=a.UserID LEFT JOIN LHGameGoldDB.dbo.GameScoreInfo c ON c.UserID=a.UserID
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------