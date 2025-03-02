
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
-- ¼���¼
CREATE PROC GSP_GR_RecordGameVideo
	@wKindID SMALLINT,							-- ���ͱ�ʶ
	@dwMappedNum INT,							-- Լս���	
	@lBattleRecordID BIGINT,					-- ��¼��ʶ
	@strVideoNumber NVARCHAR(21),				-- ¼����		
	@VideoCreateTime DATETIME,					-- ����ʱ��	
	@wUserCount SMALLINT,						-- �û�����
	@strUserIDList NVARCHAR(128)				-- ��ʶ�б�
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ��������
DECLARE @UserID INT
DECLARE @IndexCurr SMALLINT
DECLARE @IndexPrev SMALLINT

-- ִ���߼�
BEGIN

	-- ���ڱ��
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ���ɻط���
	DECLARE @PlayBackCode int
	SET @PlayBackCode=100000+cast(rand()*900000 as int)

	-- �����¼
	IF NOT EXISTS(SELECT 1 FROM LHGameRecordDB.dbo.RecordVideoInfo(NOLOCK) WHERE VideoNumber=@strVideoNumber)
	BEGIN
		-- �����¼
		INSERT INTO LHGameRecordDB.dbo.RecordVideoInfo (VideoNumber,KindID,MappedNum,BattleRecordID,UserCount,PlayBackCode,RecordDate)
		VALUES (@strVideoNumber,@wKindID,@dwMappedNum,@lBattleRecordID,	@wUserCount,@PlayBackCode,GetDate())	
	END	

	-- ��������
	SET @IndexCurr=0
	SET @IndexPrev=0	

	-- ������ʶ
	WHILE (@IndexPrev < LEN(@strUserIDList))
	BEGIN
		SET @IndexCurr=CHARINDEX( ';', @strUserIDList, @IndexPrev+1)		
		IF (@IndexCurr>0)
		BEGIN		
			-- ��ȡ��ʶ
			SET @UserID=CAST(SUBSTRING(@strUserIDList, @IndexPrev +1, @IndexCurr-@IndexPrev-1) AS INT)			
			SET @IndexPrev=@IndexCurr

			-- ��Ҽ�¼
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
