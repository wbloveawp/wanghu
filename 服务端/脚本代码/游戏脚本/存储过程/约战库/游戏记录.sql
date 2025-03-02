
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_RecordDrawInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_RecordDrawInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_RecordDrawScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_RecordDrawScore]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 游戏记录
CREATE PROC GSP_GR_RecordDrawInfo
	@lRecordID BIGINT,							-- 记录 I D
	@wKindID INT,								-- 游戏 I D
	@wServerID INT,								-- 房间 I D
	@wTableID INT,								-- 桌子号码	
	@wUserCount SMALLINT,						-- 用户数目
	@lWasteCount BIGINT,						-- 损耗数目
    @dwWasterUserID INT,						-- 损耗玩家
	@lRevenueCount BIGINT,						-- 游戏税收
	@dwPlayTimeCount INT,						-- 游戏时间
	@ExecuteDate DATETIME,						-- 执行时间		
	@strVideoNumber NVARCHAR(21),				-- 录像编号
	@cbSocreKind TINYINT,						-- 分数类型	
	@lBattleRecordID BIGINT,					-- 约战记录	
	@strGameCourse NVARCHAR(1024)				-- 游戏过程	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 插入记录
	IF NOT EXISTS(SELECT 1 FROM RecordDrawInfo(NOLOCK) WHERE RecordID=@lRecordID)
	BEGIN
		INSERT RecordDrawInfo (RecordID, KindID, ServerID, ScoreKind, TableID, UserCount, RevenueCount, WasteCount, PlayTimeCount, BattleRecordID, GameCourse, VideoNumber, RecordDate)
		VALUES (@lRecordID, @wKindID, @wServerID, @cbSocreKind, @wTableID, @wUserCount, @lRevenueCount, @lWasteCount, @dwPlayTimeCount,@lBattleRecordID, @strGameCourse, @strVideoNumber, GetDate())
	END
	
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 写分记录
CREATE PROC GSP_GR_RecordDrawScore 
	@lRecordID BIGINT,							-- 记录 I D
	@strVideoNumber NVARCHAR(21),				-- 录像编号		
	@strScoreItemList NVARCHAR(MAX),			-- 分数信息	
	@lBattleRecordID BIGINT						-- 约战记录
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 插入记录
	IF ISJSON(@strScoreItemList)>0
	BEGIN
		INSERT INTO RecordDrawScore(RecordID,UserID,ChairID,Score,Revenue,VideoNumber,BattleRecordID,RecordDate)
		SELECT @lRecordID,JSON_VALUE(VALUE,N'$.UserID'),JSON_VALUE(VALUE,N'$.ChairID'),JSON_VALUE(VALUE,N'$.Score'),JSON_VALUE(VALUE,N'$.Revenue'),@strVideoNumber,@lBattleRecordID,GETDATE()
		FROM OPENJSON(@strScoreItemList)
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------