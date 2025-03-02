
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

-- ��Ϸ��¼
CREATE PROC GSP_GR_RecordDrawInfo
	@lRecordID BIGINT,							-- ��¼ I D
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D
	@wTableID INT,								-- ���Ӻ���	
	@wUserCount SMALLINT,						-- �û���Ŀ
	@lWasteCount BIGINT,						-- �����Ŀ
    @dwWasterUserID INT,						-- ������
	@lRevenueCount BIGINT,						-- ��Ϸ˰��
	@dwPlayTimeCount INT,						-- ��Ϸʱ��
	@ExecuteDate DATETIME,						-- ִ��ʱ��		
	@strVideoNumber NVARCHAR(21),				-- ¼����
	@cbSocreKind TINYINT,						-- ��������	
	@lBattleRecordID BIGINT,					-- Լս��¼	
	@strGameCourse NVARCHAR(1024)				-- ��Ϸ����	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- �����¼
	IF NOT EXISTS(SELECT 1 FROM RecordDrawInfo(NOLOCK) WHERE RecordID=@lRecordID)
	BEGIN
		INSERT RecordDrawInfo (RecordID, KindID, ServerID, ScoreKind, TableID, UserCount, RevenueCount, WasteCount, PlayTimeCount, BattleRecordID, GameCourse, VideoNumber, RecordDate)
		VALUES (@lRecordID, @wKindID, @wServerID, @cbSocreKind, @wTableID, @wUserCount, @lRevenueCount, @lWasteCount, @dwPlayTimeCount,@lBattleRecordID, @strGameCourse, @strVideoNumber, GetDate())
	END
	
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- д�ּ�¼
CREATE PROC GSP_GR_RecordDrawScore 
	@lRecordID BIGINT,							-- ��¼ I D
	@strVideoNumber NVARCHAR(21),				-- ¼����		
	@strScoreItemList NVARCHAR(MAX),			-- ������Ϣ	
	@lBattleRecordID BIGINT						-- Լս��¼
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- �����¼
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