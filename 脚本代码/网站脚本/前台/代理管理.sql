----------------------------------------------------------------------------------------------------
-- 时间：2021-11-01
-- 用途：代理信息
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].WEB_PW_GetAgentInfo') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].WEB_PW_GetAgentInfo
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].WEB_PW_StatAgentView') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].WEB_PW_StatAgentView
GO

SET QUOTED_IDENTIFIER ON
GO

SET ANSI_NULLS ON
GO

----------------------------------------------------------------------------------------------------
-- 代理信息
CREATE PROCEDURE WEB_PW_GetAgentInfo
	@dwUserID INT								-- 用户标识
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID			INT
DECLARE @GameID			INT
DECLARE @Nullity		TINYINT
DECLARE @AgentID		INT
DECLARE @ParentID		INT
DECLARE @ParentGameID	INT
DECLARE @ParentUserID	INT

-- 推广信息
DECLARE @SpreadCount INT
DECLARE @SpreadValidCount INT
DECLARE @SpreadValidAward BIGINT
DECLARE @SpreadPayAward BIGINT
DECLARE @TodaySpreadValidAward BIGINT
DECLARE @TodaySpreadPayAward BIGINT

-- 执行逻辑
BEGIN
	-- 设置时间
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 查询用户信息
	SELECT @UserID=UserID,@GameID=GameID,@Nullity=Nullity,@AgentID=AgentID,@ParentID=ParentID FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
	
	-- 查询上级推广信息
	SET @ParentGameID=0
	SET @ParentUserID=0
	IF @ParentID >0 
	BEGIN
		SELECT @ParentGameID=GameID,@ParentUserID=UserID FROM AccountsInfo(NOLOCK) WHERE UserID=@ParentID
	END

	-- 查询推广奖励信息
	SELECT @TodaySpreadValidAward=ISNULL(SUM(SpreadValidAward),0),@TodaySpreadPayAward=ISNULL(SUM(SpreadPayAward),0) FROM LHGameRecordDB.dbo.ReportDayUser(NOLOCK) WHERE DateID=@DateID AND UserID=@UserID
	SELECT @SpreadValidCount=ISNULL(SUM(SpreadValidCount),0),@SpreadValidAward=ISNULL(SUM(SpreadValidAward),0),@SpreadPayAward=ISNULL(SUM(SpreadPayAward),0) FROM LHGameRecordDB.dbo.ReportDayUser(NOLOCK) WHERE UserID=@UserID
	SELECT @SpreadCount=COUNT(UserID) FROM AccountsInfo(NOLOCK) WHERE ParentID=@UserID

	-- 用户信息
	SELECT @UserID AS UserID,@GameID AS GameID,@AgentID AS AgentID,@ParentGameID AS ParentGameID,@ParentUserID AS ParentUserID,@TodaySpreadValidAward AS TodaySpreadValidAward,
		@TodaySpreadPayAward AS TodaySpreadPayAward,@SpreadCount AS SpreadCount,@SpreadValidCount AS SpreadValidCount,@SpreadValidAward AS SpreadValidAward,@SpreadPayAward AS SpreadPayAward

	-- 查询下线列表
	SELECT TOP 20 UserID,RegisterDate,PayCount=(SELECT COUNT(OrderID) FROM LHInsureDB.dbo.PayRecord(NOLOCK) WHERE UserID=A.UserID) FROM AccountsInfo(NOLOCK) A WHERE ParentID=@UserID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- 代理链接点击统计
CREATE PROCEDURE WEB_PW_StatAgentView
	@dwGameID INT								-- 游戏标识
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID			INT
DECLARE @GameID			INT
DECLARE @Nullity		TINYINT
DECLARE @AgentID		INT
DECLARE @ParentID		INT
DECLARE @ParentGameID	INT
DECLARE @ParentUserID	INT

-- 推广信息
DECLARE @SpreadCount INT
DECLARE @SpreadValidCount INT
DECLARE @SpreadValidAward BIGINT
DECLARE @SpreadPayAward BIGINT
DECLARE @TodaySpreadValidAward BIGINT
DECLARE @TodaySpreadPayAward BIGINT

-- 执行逻辑
BEGIN
	-- 设置时间
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 查询用户信息
	SELECT @UserID=UserID,@GameID=GameID,@Nullity=Nullity,@AgentID=AgentID,@ParentID=ParentID FROM AccountsInfo(NOLOCK) WHERE GameID=@dwGameID
	IF @UserID IS NULL
	BEGIN
		RETURN 100
	END
	
	-- 统计数据
	UPDATE LHGameRecordDB.dbo.ReportDayUser SET SpreadViewCount=SpreadViewCount+1 WHERE DateID=@DateID AND UserID=@UserID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO LHGameRecordDB.dbo.ReportDayUser(DateID,UserID,SpreadViewCount) VALUES(@DateID,@UserID,1)
	END
END

RETURN 0

GO
