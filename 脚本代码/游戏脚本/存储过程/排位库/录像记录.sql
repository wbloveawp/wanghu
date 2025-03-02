
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
-- ¼���¼
CREATE PROC GSP_GR_RecordGameVideo
	@wKindID SMALLINT,							-- ���ͱ�ʶ
	@dwBattleNum INT,							-- Լս���	
	@lBattleRecordID BIGINT,					-- ��¼��ʶ
	@strVideoNumber NVARCHAR(21),				-- ¼����		
	@VideoCreateTime DATETIME,					-- ����ʱ��	
	@wUserCount SMALLINT,						-- �û�����
	@strUserIDList NVARCHAR(MAX)				-- ��ʶ�б�	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ��������
DECLARE @UserID INT
DECLARE @IndexCurr SMALLINT
DECLARE @IndexPrev SMALLINT

-- ִ���߼�
BEGIN

	-- �����¼
	IF NOT EXISTS(SELECT 1 FROM UHWBattleRecordDB.dbo.RecordVideoInfo(NOLOCK) WHERE VideoNumber=@strVideoNumber)
	BEGIN
		-- ���ɻط���
		DECLARE @PlayBackCode int
		SET @PlayBackCode=100000+cast(rand()*900000 as int)

		-- �����¼
		INSERT INTO UHWBattleRecordDB.dbo.RecordVideoInfo (VideoNumber,KindID,MappedNum,BattleRecordID,VideoCreateTime,UserCount,PlayBackCode,RecordDate)
		VALUES (@strVideoNumber,@wKindID,@dwBattleNum,@lBattleRecordID,@VideoCreateTime,@wUserCount,@PlayBackCode,GetDate())		
	END

	-- �����¼
	IF ISJSON(@strUserIDList)>0
	BEGIN
		INSERT INTO UHWBattleRecordDB.dbo.RecordVideoUser(VideoNumber,UserID,MappedNum,RecordDate)
		SELECT @strVideoNumber,VALUE,@dwBattleNum,GETDATE() FROM OPENJSON(@strUserIDList)
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
