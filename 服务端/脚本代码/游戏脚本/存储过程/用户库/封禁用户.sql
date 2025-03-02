
----------------------------------------------------------------------
-- 用途：封禁黑名单游戏用户
----------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_DisableBlackListGameUser]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_DisableBlackListGameUser]
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

-------------------------------------------------------------------------------------------------------
-- 禁用黑名单游戏用户
CREATE PROCEDURE [GSP_GP_DisableBlackListGameUser]
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 回滚设置
SET XACT_ABORT ON

-- 执行逻辑
BEGIN 
		
	-- 游戏列表
	DECLARE @BlackListKind TABLE
	(
	    ID INT IDENTITY(1,1),
		ListID INT,
		KindID INT,
		KindName NVARCHAR(32),
		EndDate DATETIME,
		StartDate DATETIME			
	)

	-- 查询游戏列表
	INSERT INTO @BlackListKind SELECT a.ListID,a.KindID,b.ModuleName,a.EndDate,a.StartDate FROM BlackListKind a,[LHPlatformDB].[dbo].[GameModuleInfo] b WHERE a.KindID=b.ModuleID AND a.ProcessState=0

	-- 列表为空则退出
	IF (SELECT COUNT(1) FROM @BlackListKind)=0
	BEGIN
		RETURN 1
	END

	-- 用户列表
	DECLARE @UserList TABLE 
	(  	
		UserID INT,
		KindName NVARCHAR(128),
		WinScore BIGINT
	)

	-- 用户列表
	DECLARE @UserScoreList TABLE 
	(  		
		UserID INT,
		Score  BIGINT,
		KindName NVARCHAR(128)
	) 		

	-- 循环变量
	DECLARE @Index INT		
	DECLARE @LoopCount INT 

	-- 类型信息
	DECLARE @KindID INT
	DECLARE @KindName NVARCHAR(32)
	DECLARE @EndDate DATETIME
	DECLARE @StartDate DATETIME

	-- 初始化
	SELECT @Index=1,@LoopCount=COUNT(1) FROM @BlackListKind

	-- 循环处理
	WHILE @Index<=@LoopCount
	BEGIN
	
		-- 查询信息
		SELECT @KindID=KindID,@KindName=KindName,@EndDate=EndDate,@StartDate=StartDate FROM @BlackListKind WHERE ID=@Index		

		-- 过滤用户
		INSERT @UserScoreList  SELECT UserID, SUM(Score), @KindName FROM LHGameGoldDB.dbo.RecordDrawScore
		WHERE RecordID in (SELECT RecordID FROM LHGameGoldDB.dbo.RecordDrawInfo
		WHERE RecordDate>=@StartDate AND RecordDate<=@EndDate And KindID=@KindID) GROUP BY UserID
		
		-- 更新索引
		SET @Index=@Index+1
	END	

	-- 筛选用户
	INSERT INTO @UserList(UserID,KindName,WinScore) SELECT UserID,STRING_AGG(KindName,','),SUM(Score) FROM @UserScoreList GROUP BY UserID HAVING SUM(Score)>0

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 封禁账号
    UPDATE AccountsInfo  SET  Nullity=1  WHERE USERID IN (SELECT UserID FROM @UserList)	

	-- 更新状态
	UPDATE BlackListKind SET ProcessState=1 WHERE ListID IN (SELECT ListID FROM @BlackListKind)

	-- 更新数据
	UPDATE RecordNeedDeductScore SET DeductScore+=a.WinScore FROM @UserList a, RecordNeedDeductScore(NOLOCK) b WHERE a.UserID=b.UserID

	-- 插入数据
	INSERT INTO RecordNeedDeductScore(UserID,DeductScore,CollectDateTime) 
	SELECT UserID,WinScore,GETDATE() FROM @UserList WHERE UserID NOT IN (SELECT UserID FROM RecordNeedDeductScore(NOLOCK))

	-- 封禁记录
	INSERT LHGameRecordDB.dbo.RecordUserNullity (UserID,Remark,CollectDateTime)  SELECT UserID,+'在黑名单游戏'+KindName+'中盈利',GETDATE() FROM @UserList    

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	
		
	-- 抛出记录
	SELECT UserID FROM @UserList
	     
END

SET NOCOUNT OFF

RETURN 0

GO 

-------------------------------------------------------------------------------------------------------