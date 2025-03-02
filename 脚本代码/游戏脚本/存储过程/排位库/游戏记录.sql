
----------------------------------------------------------------------------------------------------

USE LHGameRankingDB
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
	@ExecuteDate DATETIME,						-- ����ִ��ʱ��	
	@strVideoNumber NVARCHAR(21)				-- ¼����		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- �����¼
	IF NOT EXISTS(SELECT 1 FROM RecordDrawInfo(NOLOCK) WHERE RecordID=@lRecordID)
	BEGIN
		INSERT RecordDrawInfo (RecordID, KindID, ServerID, TableID, UserCount, RevenueCount, WasteCount, PlayTimeCount, VideoNumber, RecordDate)
		VALUES (@lRecordID, @wKindID, @wServerID, @wTableID, @wUserCount, @lRevenueCount, @lWasteCount, @dwPlayTimeCount, @strVideoNumber, GETDATE())
	END	

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- д�ּ�¼
CREATE PROC GSP_GR_RecordDrawScore 
	@lRecordID BIGINT,							-- ��¼ I D
	@strVideoNumber NVARCHAR(21),				-- ¼����
	@strScoreItemList NVARCHAR(MAX)				-- ������Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- �����¼
	IF ISJSON(@strScoreItemList)>0
	BEGIN
		INSERT INTO RecordDrawScore(RecordID,UserID,ChairID,Score,Revenue,VideoNumber,RecordDate)
		SELECT @lRecordID,JSON_VALUE(VALUE,N'$.UserID'),JSON_VALUE(VALUE,N'$.ChairID'),JSON_VALUE(VALUE,N'$.Score'),JSON_VALUE(VALUE,N'$.Revenue'),@strVideoNumber,GETDATE()
		FROM OPENJSON(@strScoreItemList)
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
