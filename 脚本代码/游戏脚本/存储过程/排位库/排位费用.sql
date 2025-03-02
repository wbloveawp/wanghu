USE LHGameRankIngDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DeductRankingCost]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DeductRankingCost]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- 扣除费用
CREATE PROC GSP_GR_DeductRankingCost
	@strUserList NVARCHAR(512),					-- 用户列表	
	@wKindID INT,								-- 游戏标识
	@wServerID INT,								-- 房间标识	
	@lConsumeCount INT,							-- 消耗数量	
	@strErrorDescribe NVARCHAR(127)	OUTPUT		-- 错误描述
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

SET XACT_ABORT ON

-- 执行逻辑
BEGIN

	-- 表变量定义
	DECLARE @UserIDList TABLE (ID INT identity(1,1),UserID INT, ClientAddr NVARCHAR(16))	

	-- 插入数据
	IF ISJSON(@strUserList)>0
	BEGIN
		INSERT @UserIDList (UserID,ClientAddr)
		SELECT JSON_VALUE(VALUE,'$.UserID'), JSON_VALUE(VALUE,'$.ClientAddr') FROM OPENJSON(@strUserList)
	END

	-- 变量定义
	DECLARE @Index INT
	DECLARE @LoopCount INT 

	-- 用户信息
	DECLARE @GameID INT
	DECLARE @UserID INT	
	DECLARE @ClientAddr NVARCHAR(16)	

	-- 写分日统计
	DECLARE @DateID INT
	SET @DateID = CAST(CAST(GETDATE() AS FLOAT) AS INT)
	
	-- 初始化
	SELECT @Index=1,@LoopCount=COUNT(1) FROM @UserIDList		

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN
	
	-- 循环处理
	WHILE @Index<=@LoopCount
	BEGIN
	
		-- 查询信息
		SELECT @UserID=UserID,@ClientAddr=ClientAddr FROM @UserIDList WHERE ID=@Index		

		-- 查询黄金
		DECLARE @UserGold BIGINT
		SELECT @UserGold=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@UserID

		-- 更新索引
		IF @UserGold IS NULL OR @UserGold < @lConsumeCount
		BEGIN
			SET @strErrorDescribe=N'本桌有玩家黄金余额不足，游戏无法开始！'

			-- 事务回滚
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED

			-- 抛出记录
			SELECT @UserID AS FailUserID
			
			RETURN 6
		END

		-- 游戏标识
		SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@UserID

		-- 变更记录
		INSERT INTO LHGameRecordDB.dbo.RecordChangeGold (UserID,GameID,TypeID,BeforeGold,ChangeGold,CollectClientIP)
		VALUES (@UserID,@GameID,5,@UserGold,-@lConsumeCount,@ClientAddr)

		-- 更新黄金
		UPDATE LHAccountsDB.dbo.AccountsGoldInfo SET Score-=@lConsumeCount WHERE UserID=@UserID

		-- 服务费统计
		UPDATE ReportUserScore SET UserService+=@lConsumeCount WHERE DateID=@DateID AND UserID=@UserID
		IF @@ROWCOUNT=0
		BEGIN
			INSERT INTO ReportUserScore(DateID,UserID,UserService) VALUES(@DateID,@UserID,@lConsumeCount)
		END

		SET @Index=@Index+1
	END	

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 抛出金币信息
	SELECT UserID, Score AS UserGold FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID IN (SELECT UserID FROM @UserIDList)
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
