
----------------------------------------------------------------------------------------------------

USE LHGameMatchDB
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
	@dwMappedNum INT,							-- 约战编号	
	@lBattleRecordID BIGINT,					-- 记录标识
	@strVideoNumber NVARCHAR(21),				-- 录像编号		
	@VideoCreateTime DATETIME,					-- 创建时间	
	@wUserCount SMALLINT,						-- 用户数量
	@strUserIDList NVARCHAR(128)				-- 标识列表
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 变量定义
DECLARE @UserID INT
DECLARE @IndexCurr SMALLINT
DECLARE @IndexPrev SMALLINT

-- 执行逻辑
BEGIN

	-- 日期编号
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 生成回放码
	DECLARE @PlayBackCode int
	SET @PlayBackCode=100000+cast(rand()*900000 as int)

	-- 插入记录
	IF NOT EXISTS(SELECT 1 FROM LHGameRecordDB.dbo.RecordVideoInfo(NOLOCK) WHERE VideoNumber=@strVideoNumber)
	BEGIN
		-- 插入记录
		INSERT INTO LHGameRecordDB.dbo.RecordVideoInfo (VideoNumber,KindID,MappedNum,BattleRecordID,UserCount,PlayBackCode,RecordDate)
		VALUES (@strVideoNumber,@wKindID,@dwMappedNum,@lBattleRecordID,	@wUserCount,@PlayBackCode,GetDate())	
	END	

	-- 设置索引
	SET @IndexCurr=0
	SET @IndexPrev=0	

	-- 解析标识
	WHILE (@IndexPrev < LEN(@strUserIDList))
	BEGIN
		SET @IndexCurr=CHARINDEX( ';', @strUserIDList, @IndexPrev+1)		
		IF (@IndexCurr>0)
		BEGIN		
			-- 获取标识
			SET @UserID=CAST(SUBSTRING(@strUserIDList, @IndexPrev +1, @IndexCurr-@IndexPrev-1) AS INT)			
			SET @IndexPrev=@IndexCurr

			-- 玩家记录
			IF NOT EXISTS(SELECT 1 FROM LHGameRecordDB.dbo.RecordVideoUser(NOLOCK) WHERE VideoNumber=@strVideoNumber AND UserID=@UserID)
			BEGIN
				INSERT INTO LHGameRecordDB.dbo.RecordVideoUser(VideoNumber,UserID,RecordDate)
				VALUES (@strVideoNumber,@UserID,GetDate())
			END						
		End 
		ELSE BREAK
	End
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
