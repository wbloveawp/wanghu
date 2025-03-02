
----------------------------------------------------------------------------------------------------

USE LHGameRankingDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_RecordGameVideo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_RecordGameVideo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- 录像记录
CREATE PROC GSP_GR_RecordGameVideo
	@wKindID SMALLINT,							-- 类型标识
	@dwBattleNum INT,							-- 约战编号	
	@lBattleRecordID BIGINT,					-- 记录标识
	@strVideoNumber NVARCHAR(21),				-- 录像编号		
	@VideoCreateTime DATETIME,					-- 创建时间	
	@wUserCount SMALLINT,						-- 用户数量
	@strUserIDList NVARCHAR(MAX)				-- 标识列表	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 变量定义
DECLARE @UserID INT
DECLARE @IndexCurr SMALLINT
DECLARE @IndexPrev SMALLINT

-- 执行逻辑
BEGIN

	-- 插入记录
	IF NOT EXISTS(SELECT 1 FROM UHWBattleRecordDB.dbo.RecordVideoInfo(NOLOCK) WHERE VideoNumber=@strVideoNumber)
	BEGIN
		-- 生成回放码
		DECLARE @PlayBackCode int
		SET @PlayBackCode=100000+cast(rand()*900000 as int)

		-- 插入记录
		INSERT INTO UHWBattleRecordDB.dbo.RecordVideoInfo (VideoNumber,KindID,MappedNum,BattleRecordID,VideoCreateTime,UserCount,PlayBackCode,RecordDate)
		VALUES (@strVideoNumber,@wKindID,@dwBattleNum,@lBattleRecordID,@VideoCreateTime,@wUserCount,@PlayBackCode,GetDate())		
	END

	-- 插入记录
	IF ISJSON(@strUserIDList)>0
	BEGIN
		INSERT INTO UHWBattleRecordDB.dbo.RecordVideoUser(VideoNumber,UserID,MappedNum,RecordDate)
		SELECT @strVideoNumber,VALUE,@dwBattleNum,GETDATE() FROM OPENJSON(@strUserIDList)
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
