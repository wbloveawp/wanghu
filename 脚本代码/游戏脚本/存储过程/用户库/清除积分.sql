----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_CleanGameScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_CleanGameScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_DeductGameScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_DeductGameScore]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 清除积分
CREATE PROC GSP_GP_CleanGameScore	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 自动回滚
SET XACT_ABORT ON

-- 执行逻辑
BEGIN

	
	-- 用户列表
	DECLARE @TableUserList TABLE
	(
		ID INT identity(1,1),
		UserID INT NOT NULL 
	)

	-- 插入数据
	INSERT INTO @TableUserList SELECT a.UserID FROM RecordNeedClearGold a LEFT JOIN AccountsOnlineInfo b ON a.UserID=b.UserID WHERE DATEDIFF(ss,a.WithdrawDateTime,GETDATE())>=30*60 AND ISNULL(b.ServerID,0)=0

	-- 变量定义
	DECLARE @Index INT	
	DECLARE @LoopCount INT
	
	-- 统计数量
	SELECT @Index=1,@LoopCount=COUNT(1) FROM @TableUserList

	-- 退出判断
	IF @LoopCount=0	RETURN 1

	-- 调整数量
	IF @Index IS NULL SET @Index=1
	IF @LoopCount IS NULL SET @LoopCount=0

	-- 用户信息
	DECLARE @UserID INT
	DECLARE @UserScore BIGINT

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 循环处理
	WHILE @Index<=@LoopCount
	BEGIN	
		-- 查询信息
		SELECT @UserID=UserID FROM @TableUserList WHERE ID=@Index

		-- 查询积分
		SELECT @UserScore=-Score FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@UserID

		-- 调整积分
		IF @UserScore IS NULl SET @UserScore=0

		-- 扣除积分
		IF @UserScore<>0
		BEGIN
			EXEC GSP_GP_WriteGameWealth @UserID,2,79,@UserScore
		END

		-- 更新索引
		SET @Index=@Index+1
	END

	-- 删除数据
	DELETE RecordNeedClearGold WHERE UserID IN (SELECT UserID FROM @TableUserList)

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	
	
	-- 抛出数据
	SELECT UserID FROM @TableUserList

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 扣除积分
CREATE PROC GSP_GP_DeductGameScore	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 自动回滚
SET XACT_ABORT ON

-- 执行逻辑
BEGIN

	
	-- 用户列表
	DECLARE @TableUserList TABLE
	(
		ID INT identity(1,1),
		UserID INT NOT NULL,
		DeductScore BIGINT	
	)

	-- 插入数据
	INSERT INTO @TableUserList SELECT a.UserID,a.DeductScore FROM RecordNeedDeductScore a LEFT JOIN AccountsOnlineInfo b ON a.UserID=b.UserID WHERE DATEDIFF(ss,a.CollectDateTime,GETDATE())>=5*60 AND ISNULL(b.ServerID,0)=0

	-- 变量定义
	DECLARE @Index INT	
	DECLARE @LoopCount INT
	
	-- 统计数量
	SELECT @Index=1,@LoopCount=COUNT(1) FROM @TableUserList

	-- 退出判断
	IF @LoopCount=0	RETURN 1

	-- 调整数量
	IF @Index IS NULL SET @Index=1
	IF @LoopCount IS NULL SET @LoopCount=0

	-- 用户信息
	DECLARE @UserID INT
	DECLARE @UserScore BIGINT
	DECLARE @DeductScore BIGINT
	DECLARE @ChangeScore BIGINT

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 循环处理
	WHILE @Index<=@LoopCount
	BEGIN	
		-- 查询信息
		SELECT @UserID=UserID,@DeductScore=DeductScore FROM @TableUserList WHERE ID=@Index

		-- 查询积分
		SELECT @UserScore=Score FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@UserID

		-- 调整积分
		IF @UserScore IS NULl SET @UserScore=0

		-- 计算变更分数
		IF @UserScore>@DeductScore 
		BEGIN
			SET @ChangeScore=-@DeductScore
		END ELSE
		BEGIN
			SET @ChangeScore=-@UserScore
		END

		-- 扣除积分
		IF @UserScore<>0
		BEGIN
			EXEC GSP_GP_WriteGameWealth @UserID,2,78,@ChangeScore
		END

		-- 用户解禁
		UPDATE AccountsInfo SET Nullity=0 WHERE UserID=@UserID

		-- 更新索引
		SET @Index=@Index+1
	END

	-- 删除数据
	DELETE RecordNeedDeductScore WHERE UserID IN (SELECT UserID FROM @TableUserList)

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
