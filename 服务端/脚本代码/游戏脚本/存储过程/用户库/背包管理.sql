----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AppendGoodsItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AppendGoodsItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_RemoveGoodsItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_RemoveGoodsItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UserQueryGoods]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UserQueryGoods]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UserUpdateGoods]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UserUpdateGoods]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UserRecordGoods]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UserRecordGoods]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �����Ʒ
CREATE PROC GSP_GP_AppendGoodsItem
	@dwUserID INT,							-- �û���ʶ		
	@wReason SMALLINT,						-- ���ԭ��
	@wGoodsID SMALLINT,						-- ��Ʒ��ʶ
	@wGoodsLevel SMALLINT,					-- ��Ʒ�ȼ�
	@dwIncCount INT,						-- ��������
	@dwIncIndate INT,						-- ������Ч��		
	@dwTimestamp INT						-- ʱ���	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �Զ��ع�
SET XACT_ABORT ON

-- ִ���߼�
BEGIN

	-- ��Ʒ����
	DECLARE @HasIndate TINYINT
	DECLARE @GoodsKind SMALLINT
	SELECT @HasIndate=HasIndate,@GoodsKind=GoodsKind FROM LHPlatformDB.dbo.GameGoodsInfo(NOLOCK) WHERE GoodsID=@wGoodsID

	-- ��Ʒ������
	IF @GoodsKind IS NULL 
	BEGIN
		RETURN @dwIncCount
	END

	-- �����ȼ�
	IF @GoodsKind=11 AND @wGoodsLevel=0
	BEGIN
		SET @wGoodsLevel=1
	END

	-- ��Ʒ��Ϣ
	DECLARE @GoodsCount INT	
	DECLARE @GoodsIndate INT		
	DECLARE @GoodsExpireTime INT	

	-- ��Ʒ��Ϣ	
	DECLARE @BeforeCount INT
	DECLARE @BeforeIndate INT
	DECLARE @BeforeExpireTime INT
	SELECT @BeforeCount=GoodsCount,@BeforeIndate=GoodsIndate,@BeforeExpireTime=ExpireTime FROM AccountsPack WITH(UPDLOCK) WHERE UserID=@dwUserID AND GoodsID=@wGoodsID

	-- ������Ʒ
	IF @BeforeCount IS NOT NULL
	BEGIN	
		-- �ж���Ч��
		IF @HasIndate=1
		BEGIN		
			-- ��������
			SET @GoodsCount=1
			SET @GoodsExpireTime=0

			-- �����ж�
			IF @BeforeCount=0
			BEGIN
				IF @dwIncIndate=0 SET @GoodsIndate=0
				IF @dwIncIndate>0 
				BEGIN
					SET @GoodsIndate=@dwIncIndate
					SET @GoodsExpireTime=@dwTimestamp+@dwIncIndate
				END
			END ELSE
			BEGIN
				IF @BeforeIndate=0 
				BEGIN
					SET @GoodsIndate=0
					SET @GoodsExpireTime=0					
				END ELSE
				BEGIN
					IF @dwIncIndate=0 SET @GoodsIndate=0
					IF @dwIncIndate>0 
					BEGIN
						SET @GoodsIndate=@BeforeIndate+@dwIncIndate
						SET @GoodsExpireTime=@BeforeExpireTime+@dwIncIndate
					END
				END
			END			
		END ELSE
		BEGIN
			SET @GoodsIndate=0
			SET @GoodsExpireTime=0
			SET @GoodsCount=@BeforeCount+@dwIncCount
		END	
		
		-- ������Ʒ
		UPDATE AccountsPack SET GoodsCount=@GoodsCount,GoodsLevel=@wGoodsLevel,GoodsIndate=@GoodsIndate,ExpireTime=@GoodsExpireTime WHERE UserID=@dwUserID AND GoodsID=@wGoodsID						
	END ELSE
	BEGIN
		-- �ж���Ч��
		IF @HasIndate=1
		BEGIN		
			-- ��������
			SET @GoodsCount=1
			SET @GoodsExpireTime=0
			
			-- ������Ч��
			IF @dwIncIndate=0 SET @GoodsIndate=0
			IF @dwIncIndate>0 
			BEGIN
				SET @GoodsIndate=@dwIncIndate
				SET @GoodsExpireTime=@dwTimestamp+@dwIncIndate
			END			
		END ELSE
		BEGIN
			SET @GoodsIndate=0
			SET @GoodsExpireTime=0
			SET @GoodsCount=@dwIncCount
		END

		-- �����Ʒ
		INSERT INTO AccountsPack(UserID, GoodsID, GoodsKind, GoodsLevel, GoodsCount, GoodsIndate, BindState, ExpireTime, CollectTime)
		VALUES (@dwUserID, @wGoodsID, @GoodsKind, @wGoodsLevel, @GoodsCount, @GoodsIndate, 0, @GoodsExpireTime, GetDate())	
	END

	-- ��������	
	IF @BeforeCount IS NULL SET @BeforeCount=0
	IF @BeforeIndate IS NULL SET @BeforeIndate=0
	IF @BeforeExpireTime IS NULL SET @BeforeExpireTime=0

	-- �����¼
	IF @GoodsCount-@BeforeCount<>0
	BEGIN
		INSERT INTO LHGameRecordDB.dbo.RecordChangeGoods(UserID, GoodsID, GoodsKind, TypeID, RelationID, BeforeCount, ChangeCount, CollectClientIP, CollectDateTime)
		VALUES (@dwUserID, @wGoodsID, @GoodsKind, @wReason, 0, @BeforeCount, @GoodsCount-@BeforeCount, N'------------' ,GetDate())
	END
	
	RETURN 0

END 

GO

----------------------------------------------------------------------------------------------------

-- �Ƴ���Ʒ
CREATE PROC GSP_GP_RemoveGoodsItem
	@dwUserID INT,							-- �û���ʶ			
	@wReason SMALLINT,						-- ���ԭ��
	@wGoodsID SMALLINT,						-- ��Ʒ��ʶ
	@dwDesCount INT							-- ��������		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �Զ��ع�
SET XACT_ABORT ON

-- ִ���߼�
BEGIN

	-- ��Ʒ��Ϣ	
	DECLARE @BeforeCount INT	
	DECLARE @GoodsKind SMALLINT
	SELECT @GoodsKind=GoodsKind,@BeforeCount=GoodsCount FROM AccountsPack WITH(UPDLOCK) WHERE UserID=@dwUserID AND GoodsID=@wGoodsID

	-- ��������
	IF @BeforeCount IS NULL SET @BeforeCount=0
	
	-- У������
	IF @dwDesCount>@BeforeCount
	BEGIN
		RETURN 1
	END
	
	-- ��������
	UPDATE AccountsPack SET GoodsCount=GoodsCount-@dwDesCount WHERE UserID=@dwUserID AND GoodsID=@wGoodsID						

	-- �����¼
	INSERT INTO LHGameRecordDB.dbo.RecordChangeGoods(UserID, GoodsID, GoodsKind, TypeID, RelationID, BeforeCount, ChangeCount, CollectDateTime)
	VALUES (@dwUserID, @wGoodsID, @GoodsKind, @wReason, 0, @BeforeCount, -@dwDesCount, GetDate())

END 

GO

----------------------------------------------------------------------------------------------------

-- ��ѯ��Ʒ
CREATE PROC GSP_GP_UserQueryGoods
	@dwUserID INT,							-- �û���ʶ	
	@szGoodsIDList NVARCHAR(256)=''			-- ��Ʒ��ʶ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ������Ʒ
	IF ISJSON(@szGoodsIDList)=0
	BEGIN
		-- ��ѯ��Ʒ
		SELECT * FROM AccountsPack(NOLOCK) WHERE UserID=@dwUserID AND GoodsCount>0

	END ELSE
	BEGIN
		SELECT * FROM AccountsPack(NOLOCK) WHERE UserID=@dwUserID AND GoodsID IN (SELECT VALUE FROM OPENJSON(@szGoodsIDList))
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��¼��Ʒ
CREATE PROC GSP_GP_UserRecordGoods
	@dwUserID INT,							-- �û���ʶ	
	@strRecordList NVARCHAR(MAX)			-- ��¼�б�		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �Զ��ع�
SET XACT_ABORT ON

-- ִ���߼�
BEGIN
	-- �����¼
	IF ISJSON(@strRecordList)>0
	BEGIN
		INSERT INTO LHGameRecordDB.dbo.RecordChangeGoods(
			UserID, 
			GoodsID, 
			GoodsKind, 
			TypeID, 
			RelationID, 
			BeforeCount, 
			ChangeCount, 
			CollectDateTime)
		SELECT 
			@dwUserID, 
			JSON_VALUE(VALUE,'$.GoodsID'), 
			JSON_VALUE(VALUE,'$.GoodsKind'),
			JSON_VALUE(VALUE,'$.Reason'), 
			0, 
			JSON_VALUE(VALUE,'$.BeforeCount'), 
			JSON_VALUE(VALUE,'$.VariationCount'), 
			JSON_VALUE(VALUE,'$.VariationTime')
		FROM OPENJSON(@strRecordList)
		ORDER BY JSON_VALUE(VALUE,'$.VariationTime')
	END

END 

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ������Ʒ
CREATE PROC GSP_GP_UserUpdateGoods
	@dwUserID INT,							-- �û���ʶ		
	@strGoodsList NVARCHAR(MAX)				-- ��Ʒ�б�	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- У��Json
	IF ISJSON(@strGoodsList)>0
	BEGIN
		-- ��Ʒ�б�
		DECLARE @GoodsList TABLE (GoodsID SMALLINT,GoodsLevel SMALLINT,GoodsKind SMALLINT,GoodsCount INT,GoodsIndate INT,BindState TINYINT,ExpireTime INT)

		-- �����¼
		INSERT @GoodsList SELECT JSON_VALUE(VALUE,'$.GoodsID'),ISNULL(JSON_VALUE(VALUE,'$.GoodsLevel'),0), JSON_VALUE(VALUE,'$.GoodsKind'), JSON_VALUE(VALUE,'$.GoodsCount'), JSON_VALUE(VALUE,'$.GoodsIndate'), 
		JSON_VALUE(VALUE,'$.BindState'),JSON_VALUE(VALUE,'$.ExpireTime') FROM OPENJSON(@strGoodsList)

		-- ������Ʒ
		UPDATE AccountsPack SET GoodsLevel=b.GoodsLevel,GoodsCount=b.GoodsCount, GoodsIndate=b.GoodsIndate,BindState=b.BindState,ExpireTime=b.ExpireTime 
		FROM AccountsPack a, @GoodsList b WHERE a.UserID=@dwUserID AND a.GoodsID=b.GoodsID

		-- ɾ������
		DELETE @GoodsList WHERE GoodsID IN (SELECT GoodsID FROM AccountsPack(NOLOCK) WHERE UserID=@dwUserID)

		-- ������Ʒ
		INSERT INTO AccountsPack(UserID, GoodsID, GoodsKind, GoodsLevel, GoodsCount, GoodsIndate, BindState, ExpireTime, CollectTime) 
		SELECT @dwUserID, GoodsID, GoodsKind, GoodsLevel, GoodsCount, GoodsIndate, BindState, ExpireTime, GetDate() FROM @GoodsList 
	END

END 

RETURN 0

GO

----------------------------------------------------------------------------------------------------