
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadStockInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadStockInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryStockInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryStockInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryStockList]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryStockList]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AppendStockInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AppendStockInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ModifyStockInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ModifyStockInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_DeleteStockInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_DeleteStockInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UpdateStockScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UpdateStockScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UpdateStockState]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UpdateStockState]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UpdateJackpotScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UpdateJackpotScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AdjustStockScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AdjustStockScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AdjustJackpotScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AdjustJackpotScore]
GO

----------------------------------------------------------------------------------------------------

-- ���ؿ��
CREATE  PROCEDURE dbo.GSP_GP_LoadStockInfo		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

	SELECT *, (SELECT * FROM GameJackpotInfo(NOLOCK) WHERE StockID=GameStockInfo.StockID FOR JSON AUTO) AS JackpotOption  FROM GameStockInfo(NOLOCK) WHERE Nullity=0

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ѯ���
CREATE  PROCEDURE dbo.GSP_GP_QueryStockInfo		
	@wStockID	INT
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

	SELECT *, (SELECT * FROM GameJackpotInfo(NOLOCK) WHERE StockID=GameStockInfo.StockID FOR JSON AUTO) AS JackpotOption  FROM GameStockInfo(NOLOCK) WHERE StockID=@wStockID AND Nullity=0

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ѯ���
CREATE  PROCEDURE dbo.GSP_GP_QueryStockList
	@strKindIDList	NVARCHAR(512)
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON
	
	IF ISJSON(@strKindIDList)>0
	BEGIN
		SELECT a.*, (SELECT * FROM GameJackpotInfo(NOLOCK) WHERE StockID=a.StockID FOR JSON AUTO) AS JackpotOption  FROM GameStockInfo(NOLOCK) a, OPENJSON(@strKindIDList) b WHERE Nullity=0 AND a.KindID =b.VALUE
		ORDER BY a.KindID,a.BetAmount
	END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ӿ��
CREATE  PROCEDURE dbo.GSP_GP_AppendStockInfo
	@wKindID			SMALLINT,				-- ���ͱ�ʶ
	@lBetAmount			INT,					-- ��ע���
	@wStockKind			INT,					-- �������
	@wStockLevel		INT,					-- ��漶��
	@strStockName		NVARCHAR(32),			-- �������
	@lExtraScore		BIGINT,					-- �������
	@lStockScore		BIGINT,					-- ��ǰ���
	@cbEnableMode		TINYINT,				-- ����ģʽ
	@dwEnableEndTime	BIGINT,					-- ͣ��ʱ��
	@dwEnableStartTime  BIGINT,					-- ����ʱ��
	@strShrinkOption    NVARCHAR(512),			-- ��ˮ����
	@strRelationIDList	NVARCHAR(128),			-- ������ʶ
	@strJackpotOption	NVARCHAR(1024),			-- �ʳ���Ϣ
	@strErrorDescribe	NVARCHAR(127) OUTPUT	-- �����Ϣ	
	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON
	
	-- ��ˮ����
	IF ISJSON(@strShrinkOption)=0
	BEGIN
		SET @strErrorDescribe=N'��ˮ���ø�ʽ�������������ã�'
		RETURN 1
	END

	-- ��������
	IF LEN(@strRelationIDList)>0 AND ISJSON(@strRelationIDList)=0
	BEGIN
		SET @strErrorDescribe=N'������ʶ��ʽ�������������ã�'
		RETURN 2
	END

	-- �ʽ�����
	IF ISJSON(@strJackpotOption)=0
	BEGIN
		SET @strErrorDescribe=N'�ʳ����ø�ʽ�������������ã�'
		RETURN 3
	END

	-- BET�����ظ�
	IF EXISTS(SELECT 1 FROM GameStockInfo(NOLOCK) WHERE KindID=@wKindID AND BetAmount=@lBetAmount)
	BEGIN
		SET @strErrorDescribe=N'��Ǹ,ͬһ��Ϸ�²���������ͬBET���Ŀ�棡'
		RETURN 4
	END

	-- BET�����������
	IF @wStockLevel=1 AND (SELECT COUNT(1) FROM GameStockInfo(NOLOCK) WHERE KindID=@wKindID AND StockLevel=1)>=12
	BEGIN
		SET @strErrorDescribe=N'��Ǹ,ͬһ��Ϸ��BET������������ܳ���12����'
		RETURN 5
	END

	-- ������
	INSERT INTO GameStockInfo(KindID,StockKind,StockLevel,BetAmount,StockName,ExtraScore,StockScore,StockState,EnableMode,EnableEndTime,EnableStartTime,ShrinkOption,RelationIDList,FirstStock,Describe)
	VALUES(@wKindID,@wStockKind,@wStockLevel,@lBetAmount,@strStockName,@lExtraScore,@lStockScore,0,@cbEnableMode,@dwEnableEndTime,@dwEnableStartTime,@strShrinkOption,@strRelationIDList,@lStockScore,N'')

	-- ����ʶ
	DECLARE @StockID INT = SCOPE_IDENTITY()

	-- ����ʽ�
	INSERT INTO GameJackpotInfo(StockID,LevelID,ExtraScore,StockScore,VirtualScore,ShrinkRatio,PayoutProb,PayoutLimit,PayoutOrigin,FirstGold)
	SELECT @StockID,JSON_VALUE(VALUE,N'$.LevelID'),JSON_VALUE(VALUE,N'$.ExtraScore'),JSON_VALUE(VALUE,N'$.StockScore'),JSON_VALUE(VALUE,N'$.VirtualScore'),
		   JSON_VALUE(VALUE,N'$.ShrinkRatio'),JSON_VALUE(VALUE,N'$.PayoutProb'),JSON_VALUE(VALUE,N'$.PayoutLimit'),JSON_VALUE(VALUE,N'$.PayoutOrigin'),JSON_VALUE(VALUE,N'$.StockScore') 
	FROM OPENJSON(@strJackpotOption)

	-- �׳�����
	SELECT *, (SELECT * FROM GameJackpotInfo(NOLOCK) WHERE StockID=GameStockInfo.StockID FOR JSON AUTO) AS JackpotOption  FROM GameStockInfo(NOLOCK) WHERE StockID=@StockID AND Nullity=0

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ӿ��
CREATE  PROCEDURE dbo.GSP_GP_ModifyStockInfo
	@wStockID			INT,					-- ����ʶ
	@wKindID			INT,					-- ���ͱ�ʶ
	@lBetAmount			INT,					-- ��ע���
	@wStockKind			INT,					-- �������
	@wStockLevel		INT,					-- ��漶��
	@strStockName		NVARCHAR(32),			-- �������
	@lExtraScore		BIGINT,					-- �������
	@cbEnableMode		TINYINT,				-- ����ģʽ
	@dwEnableEndTime	BIGINT,					-- ͣ��ʱ��
	@dwEnableStartTime  BIGINT,					-- ����ʱ��
	@strShrinkOption    NVARCHAR(512),			-- ��ˮ����
	@strRelationIDList	NVARCHAR(128),			-- ������ʶ
	@strJackpotOption	NVARCHAR(1024),			-- �ʳ���Ϣ
	@strErrorDescribe	NVARCHAR(127) OUTPUT	-- �����Ϣ	
	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON
	
	-- ��ˮ����
	IF ISJSON(@strShrinkOption)=0
	BEGIN
		SET @strErrorDescribe=N'��ˮ���ø�ʽ�������������ã�'
		RETURN 1
	END

	-- ��������
	IF LEN(@strRelationIDList)>0 AND ISJSON(@strRelationIDList)=0
	BEGIN
		SET @strErrorDescribe=N'������ʶ��ʽ�������������ã�'
		RETURN 2
	END

	-- �ʽ�����
	IF ISJSON(@strJackpotOption)=0
	BEGIN
		SET @strErrorDescribe=N'�ʳ����ø�ʽ�������������ã�'
		RETURN 3
	END

	-- BET�����ظ�
	--IF EXISTS(SELECT 1 FROM GameStockInfo(NOLOCK) WHERE KindID=@wKindID AND BetAmount=@lBetAmount)
	--BEGIN
	--	SET @strErrorDescribe=N'ͬһ��Ϸ�²���������ͬBET���Ŀ�棡'
	--	RETURN 4
	--END

	-- ���¿��
	UPDATE GameStockInfo SET 
		BetAmount=@lBetAmount,
		StockKind=@wStockKind,
		StockLevel=@wStockLevel,
		StockName=@strStockName,
		ExtraScore=@lExtraScore,
		EnableMode=@cbEnableMode,
		EnableEndTime=@dwEnableEndTime,
		EnableStartTime=@dwEnableStartTime,
		ShrinkOption=@strShrinkOption,
		RelationIDList=@strRelationIDList
	WHERE StockID=@wStockID	

	-- �����
	DECLARE @TableJackpotInfo TABLE(
		StockID SMALLINT,
		LevelID TINYINT,
		ExtraScore BIGINT,
		StockScore BIGINT,
		VirtualScore BIGINT,
		ShrinkRatio SMALLINT,
		PayoutProb SMALLINT,
		PayoutLimit BIGINT,
		PayoutOrigin BIGINT
	)

	-- �ʳ���Ϣ
	INSERT INTO @TableJackpotInfo(StockID,LevelID,ExtraScore,StockScore,VirtualScore,ShrinkRatio,PayoutProb,PayoutLimit,PayoutOrigin)
	SELECT @wStockID,JSON_VALUE(VALUE,N'$.LevelID'),JSON_VALUE(VALUE, N'$.ExtraScore'),JSON_VALUE(VALUE, N'$.StockScore'),JSON_VALUE(VALUE, N'$.VirtualScore'),JSON_VALUE(VALUE, N'$.ShrinkRatio'),
			JSON_VALUE(VALUE, N'$.PayoutProb'),JSON_VALUE(VALUE, N'$.PayoutLimit'),JSON_VALUE(VALUE, N'$.PayoutOrigin')
	FROM OPENJSON(@strJackpotOption)

	-- ���²ʳ�
	UPDATE GameJackpotInfo SET ExtraScore=b.ExtraScore, VirtualScore=b.VirtualScore, ShrinkRatio=b.ShrinkRatio,PayoutProb=b.PayoutProb, PayoutLimit=b.PayoutLimit,PayoutOrigin=b.PayoutOrigin
	FROM GameJackpotInfo a, @TableJackpotInfo b WHERE a.StockID=@wStockID AND a.LevelID=b.LevelID	

	-- ����ʳ�
	INSERT INTO GameJackpotInfo(StockID,LevelID,ExtraScore,StockScore,VirtualScore,ShrinkRatio,PayoutProb,PayoutLimit,PayoutOrigin,FirstGold) 
	SELECT StockID,LevelID,ExtraScore,StockScore,VirtualScore,ShrinkRatio,PayoutProb,PayoutLimit,PayoutOrigin,StockScore
	FROM @TableJackpotInfo WHERE LevelID NOT IN (SELECT LevelID FROM GameJackpotInfo(NOLOCK) WHERE StockID=@wStockID)

	-- ɾ���ʳ�
	DELETE GameJackpotInfo WHERE StockID=@wStockID AND LevelID NOT IN (SELECT LevelID FROM @TableJackpotInfo)

	-- �׳�����
	SELECT *, (SELECT * FROM GameJackpotInfo(NOLOCK) WHERE StockID=GameStockInfo.StockID FOR JSON AUTO) AS JackpotOption  FROM GameStockInfo(NOLOCK) WHERE StockID=@wStockID AND Nullity=0

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ɾ�����
CREATE  PROCEDURE dbo.GSP_GP_DeleteStockInfo
	@wStockID			INT				-- ����ʶ
		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON
	
	-- �׳�����
	SELECT *, (SELECT * FROM GameJackpotInfo(NOLOCK) WHERE StockID=GameStockInfo.StockID FOR JSON AUTO) AS JackpotOption  FROM GameStockInfo(NOLOCK) WHERE StockID=@wStockID AND Nullity=0

	-- ����ɾ��
	DELETE GameStockInfo WHERE StockID=@wStockID
	DELETE GameJackpotInfo WHERE StockID=@wStockID

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���¿��
CREATE  PROCEDURE dbo.GSP_GP_UpdateStockScore
	@strVariationList		NVARCHAR(4000)			-- ����б�
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

	-- ���»���
	IF ISJSON(@strVariationList)>0
	BEGIN
		UPDATE GameStockInfo  SET GameStockInfo.StockScore+=a.Score,ShrinkProfit+=a.Profit FROM OPENJSON(@strVariationList) WITH (StockID INT, Score BIGINT, Profit BIGINT) a 
		WHERE GameStockInfo.StockID=a.StockID
	END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ����״̬
CREATE  PROCEDURE dbo.GSP_GP_UpdateStockState
	@strStockStateList		NVARCHAR(4000)  -- ״̬�б�
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

	-- ����״̬
	IF ISJSON(@strStockStateList)>0
	BEGIN
		-- �����
		DECLARE @StockStateTable TABLE(StockID SMALLINT PRIMARY KEY CLUSTERED,[State] TINYINT,ControlID BIGINT)

		-- ��������
		INSERT INTO @StockStateTable SELECT * FROM OPENJSON(@strStockStateList) WITH (StockID INT, [State] TINYINT, ControlID BIGINT)

		-- ����״̬
		UPDATE GameStockInfo SET GameStockInfo.StockState=a.[State],GameStockInfo.StockControlID=CASE WHEN a.ControlID=0 THEN GameStockInfo.StockControlID ELSE a.ControlID END 
		FROM @StockStateTable a WHERE GameStockInfo.StockID=a.StockID

		-- ��������
		--INSERT INTO LHGameRecordDB.dbo.RecordStockControl(ID,StockID,KindID,StartTime,EndTime,FirstStock,FirstShrinkProfit,ShrinkOption,FirstGold,ShrinkRatio,RelationIDList)
		--SELECT a.ControlID,a.StockID,b.KindID, LHAccountsDB.dbo.LHF_GetTimestamp(),0,b.StockScore,b.ShrinkProfit,b.ShrinkOption,c.StockScore,c.ShrinkRatio,b.RelationIDList  
		--FROM @StockStateTable a, GameStockInfo b, GameJackpotInfo c WHERE a.[State]=1 AND b.StockID=a.StockID AND c.StockID=a.StockID AND c.LevelID=1

		INSERT INTO LHGameRecordDB.dbo.RecordStockControl(ID,StockID,StockName,KindID,StartTime,EndTime,FirstStock,FirstShrinkProfit,ShrinkOption,RelationIDList,FirstGold)
		SELECT a.ControlID,a.StockID,b.StockName,b.KindID, LHAccountsDB.dbo.LHF_GetTimestamp(),0,b.StockScore,b.ShrinkProfit,b.ShrinkOption,b.RelationIDList,ISNULL((SELECT SUM(StockScore) FROM GameJackpotInfo(NOLOCK) WHERE StockID=a.StockID),0)
		FROM @StockStateTable a, GameStockInfo(NOLOCK) b WHERE a.[State]=1 AND b.StockID=a.StockID

		-- ��������
		--UPDATE LHGameRecordDB.dbo.RecordStockControl SET EndTime=LHAccountsDB.dbo.LHF_GetTimestamp(),StockScore=b.StockScore,ShrinkProfit=b.ShrinkProfit,GoldScore=c.StockScore
		--FROM @StockStateTable a, GameStockInfo b, GameJackpotInfo c WHERE ID=a.ControlID AND a.[State]<>1 AND b.StockID=a.StockID AND c.StockID=a.StockID AND c.LevelID=1

		UPDATE LHGameRecordDB.dbo.RecordStockControl SET EndTime=LHAccountsDB.dbo.LHF_GetTimestamp(),StockScore=b.StockScore,ShrinkProfit=b.ShrinkProfit,GoldScore=ISNULL((SELECT SUM(StockScore) FROM GameJackpotInfo(NOLOCK) WHERE StockID=b.StockID),0),
			   JackpotInfo=ISNULL((SELECT * FROM GameJackpotInfo(NOLOCK) WHERE StockID=a.StockID FOR JSON AUTO),N'[]')
		FROM @StockStateTable a, GameStockInfo(NOLOCK) b WHERE ID=a.ControlID AND a.[State]<>1 AND b.StockID=a.StockID
	END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���½���
CREATE  PROCEDURE dbo.GSP_GP_UpdateJackpotScore
	@strVariationList		NVARCHAR(4000)  -- ״̬�б�
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

	-- ���²ʳ�
	IF ISJSON(@strVariationList)>0
	BEGIN
		UPDATE GameJackpotInfo  SET GameJackpotInfo.StockScore+=a.Score FROM OPENJSON(@strVariationList) WITH (StockID INT, LevelID TINYINT, Score BIGINT) a 
		WHERE GameJackpotInfo.StockID=a.StockID AND GameJackpotInfo.LevelID=a.LevelID
	END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �������
CREATE  PROCEDURE dbo.GSP_GP_AdjustStockScore
	@wStockID				SMALLINT,	  -- ����ʶ
	@lChangeScore			BIGINT		  -- �仯����	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

	-- ���¿��
	UPDATE GameStockInfo SET AllAddSubtractStock+=@lChangeScore WHERE StockID=@wStockID

	-- ���¼�¼
	UPDATE LHGameRecordDB.dbo.RecordStockControl SET TotalAddReduceStock+=@lChangeScore WHERE ID=ISNULL((SELECT StockControlID  FROM GameStockInfo(NOLOCK) WHERE StockID=@wStockID),0)	

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- �����ʽ�
CREATE  PROCEDURE dbo.GSP_GP_AdjustJackpotScore
	@wStockID				SMALLINT,	  -- ����ʶ
	@wLevelID				SMALLINT,	  -- �ʳصȼ�
	@lChangeScore			BIGINT		  -- �仯����	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

	-- ���¿��
	UPDATE GameStockInfo SET AllAddSubtractGold +=@lChangeScore WHERE StockID=@wStockID

	-- ���¼�¼
	UPDATE LHGameRecordDB.dbo.RecordStockControl SET TotalAddReduceGold+=@lChangeScore WHERE ID=ISNULL((SELECT StockControlID  FROM GameStockInfo(NOLOCK) WHERE StockID=@wStockID),0)	

RETURN 0

GO




----------------------------------------------------------------------------------------------------



