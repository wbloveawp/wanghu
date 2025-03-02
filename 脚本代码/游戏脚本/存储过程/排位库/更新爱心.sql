USE LHGameRankingDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UpdateLoveHeart]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UpdateLoveHeart]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- 更新爱心
CREATE PROC GSP_GR_UpdateLoveHeart
	@strUserList NVARCHAR(512),					-- 用户列表	
	@wKindID INT,								-- 游戏标识
	@wServerID INT,								-- 房间标识	
	@strErrorDescribe NVARCHAR(127)	OUTPUT		-- 错误描述
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

SET XACT_ABORT ON

-- 执行逻辑
BEGIN

	-- 表变量定义
	DECLARE @UserList TABLE (ID INT identity(1,1),UserID INT, LoveHeart INT)	

	-- 插入数据
	IF ISJSON(@strUserList)>0
	BEGIN
		INSERT @UserList (UserID,LoveHeart)
		SELECT JSON_VALUE(VALUE,'$.UserID'), JSON_VALUE(VALUE,'$.LoveHeart') FROM OPENJSON(@strUserList)
	END

	-- 变量定义
	DECLARE @Index INT
	DECLARE @LoopCount INT 

	-- 用户信息
	DECLARE @GameID INT
	DECLARE @UserID INT	
	DECLARE @LoveHeart INT
	
	-- 初始化
	SELECT @Index=1,@LoopCount=COUNT(1) FROM @UserList		

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN
	
	-- 循环处理
	WHILE @Index<=@LoopCount
	BEGIN	
		-- 查询信息
		SELECT @UserID=UserID,@LoveHeart=LoveHeart FROM @UserList WHERE ID=@Index		

		-- 游戏标识
		SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@UserID

		-- 查询爱心
		DECLARE @BeforeLoveHeart INT
		SELECT @BeforeLoveHeart=LoveHeart FROM GameScoreInfo WHERE UserID=@UserID	
		
		-- 类型标识
		DECLARE @TypeID TINYINT = 6		
		IF @LoveHeart>0 SET @TypeID=4		

		-- 变更记录
		INSERT INTO LHGameRecordDB.dbo.RecordChangeLoveHeart(UserID,GameID,TypeID,BeforeLoveHeart,ChangeLoveHeart,CollectClientIP)
		VALUES (@UserID,@GameID,@TypeID,@BeforeLoveHeart,@LoveHeart,N'---------------')

		-- 更新爱心
		UPDATE GameScoreInfo SET LoveHeart+=@LoveHeart WHERE UserID=@UserID

		SET @Index=@Index+1
	END	

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 抛出爱心信息
	SELECT UserID, LoveHeart FROM GameScoreInfo(NOLOCK) WHERE UserID IN (SELECT UserID FROM @UserList)
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
