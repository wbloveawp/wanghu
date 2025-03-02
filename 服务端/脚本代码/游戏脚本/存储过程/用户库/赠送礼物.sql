USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_SendGift]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_SendGift]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadGift]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadGift]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_SendGift]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_SendGift]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_GiftStatusUpdate]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_GiftStatusUpdate]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GP_SendGift
	@dwUserID INT,							 -- �û���ʶ		
	@dwTargetUserID INT,					 -- Ŀ���û�	
	@dwGoodsID INT,							 -- ��Ʒ��ʶ	
	@dwGoodsCount INT,						 -- ��Ʒ����	
	@szClientIP	NVARCHAR(15),				 -- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT	 -- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- �ж��Լ�
	IF @dwUserID=@dwTargetUserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1319,"Msg":"��Ǹ�������ܸ��Լ����������ʧ�ܣ�"}'	
		RETURN 5
	END

	-- ��ѯ���
	IF NOT EXISTS(SELECT * FROM AccountsInfo(NOLOCK) WHERE UserID=@dwTargetUserID)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1314,"Msg":"��Ǹ����Ҫ���͵���Ҳ����ڣ�����ʧ�ܣ�"}'	
		RETURN 1
	END

	-- ��������
	DECLARE @RecordID BIGINT
	DECLARE @PriceKind TINYINT
	DECLARE @PriceCount BIGINT
	DECLARE @MyLoveLiness INT
	DECLARE @TargetLoveLiness INT
	DECLARE @LoveLiness INT
	DECLARE @SendType TINYINT
	
	-- ���ñ���
	SET @RecordID=0
	
	-- ��ѯ��Ʒ
	SELECT @PriceKind=MoneyID, @PriceCount=Price, @LoveLiness=LoveLiness, @SendType=SendType FROM LHPlatformDB.dbo.GameGiftInfo(NOLOCK) WHERE GoodsID=@dwGoodsID
	IF @PriceKind IS NULL
	BEGIN 
		SET @strErrorDescribe=N'{"Idx":1313,"Msg":"��Ǹ��ϵͳδ��ѯ����������Ϣ������ʧ�ܣ�"}'	
		RETURN 2
	END 
	
	-- ��������
	DECLARE @Score BIGINT

	-- �ж�����Ƹ�
	IF @PriceKind=1
	BEGIN
		SELECT @Score=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID
		IF @Score<@PriceCount*@dwGoodsCount
		BEGIN 
			SET @strErrorDescribe=N'{"Idx":1316,"Msg":"��Ǹ�����Ľ�Ҳ��㣬����ʧ�ܣ�"}'	
			RETURN 4
		END
	END
	
	-- ���²Ƹ� @Reason=16 -> ��������
	DECLARE @Price BIGINT
	SET @Price=-@PriceCount*@dwGoodsCount
	EXEC GSP_GP_WriteGameWealth @dwUserID,@PriceKind,16,@Price
	
	-- ��������ֵ
	UPDATE AccountsInfo SET LoveLiness=LoveLiness+@LoveLiness WHERE UserID IN (@dwUserID, @dwTargetUserID)
	
	-- ��ѯ����ֵ
	SELECT @MyLoveLiness=LoveLiness FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
	SELECT @TargetLoveLiness=LoveLiness FROM AccountsInfo(NOLOCK) WHERE UserID=@dwTargetUserID
	
	-- ���ڱ�ʶ
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	
	-- ��¼�Լ�����ֵ
	UPDATE LHGameRecordDB.dbo.ReportDayUser SET LoveLiness=LoveLiness+@LoveLiness WHERE UserID=@dwUserID AND DateID=@DateID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO LHGameRecordDB.dbo.ReportDayUser(DateID,UserID,PayCount,PayAmount,LastPayAmount,LastPayDate,LoveLiness,ReportDateTime) 
		VALUES (@DateID,@dwUserID,0,0,0,GETDATE(),@LoveLiness,GETDATE())
	END
	
	-- ��¼�Է�����ֵ
	UPDATE LHGameRecordDB.dbo.ReportDayUser SET LoveLiness=LoveLiness+@LoveLiness WHERE UserID=@dwTargetUserID AND DateID=@DateID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO LHGameRecordDB.dbo.ReportDayUser(DateID,UserID,PayCount,PayAmount,LastPayAmount,LastPayDate,LoveLiness,ReportDateTime) 
		VALUES (@DateID,@dwTargetUserID,0,0,0,GETDATE(),@LoveLiness,GETDATE())
	END
	
	-- �����¼
	INSERT INTO LHGameRecordDB.dbo.RecordUserGift(UserID,TargetUserID,GoodsID,GoodsCount,LoveLiness,SendType,LoadStatus,CollectClientIP,CollectDateTime)
	VALUES (@dwUserID,@dwTargetUserID,@dwGoodsID,@dwGoodsCount,@LoveLiness,1,0,@szClientIP,GETDATE())
	
	-- ��¼��ʶ
	SET @RecordID=SCOPE_IDENTITY()

	-- �������
	SELECT @RecordID AS RecordID, @LoveLiness AS LoveLiness, @SendType AS SendType, @MyLoveLiness AS MyLoveLiness, @TargetLoveLiness AS TargetLoveLiness

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GP_LoadGift
	@dwUserID INT,							 -- �û���ʶ		
	@strErrorDescribe NVARCHAR(127) OUTPUT	 -- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ���������
	--DECLARE @GiftList TABLE (RecordID INT,UserID INT,GoodsID SMALLINT,GoodsCount SMALLINT,LoveLiness INT,CollectDateTime DATETIME,NickName NVARCHAR(31))
	
	-- ��������
	--INSERT INTO @GiftList (RecordID,UserID,GoodsID,GoodsCount,LoveLiness,CollectDateTime,NickName)
	--SELECT b.RecordID,b.UserID,b.GoodsID,b.GoodsCount,b.LoveLiness,b.CollectDateTime,(SELECT NickName FROM AccountsInfo(NOLOCK) a WHERE a.UserID=b.UserID) AS NickName 
	--FROM LHGameRecordDB.dbo.RecordUserGift b WHERE b.TargetUserID=@dwUserID AND b.LoadStatus=0 AND b.SendType=1

	-- �����¼
	--UPDATE LHGameRecordDB.dbo.RecordUserGift SET LoadStatus=1 WHERE TargetUserID=@dwUserID
	
	--SELECT RecordID,UserID,GoodsID,GoodsCount,LoveLiness,CollectDateTime,NickName FROM @GiftList
	
	SELECT b.RecordID,b.UserID,b.GoodsID,b.GoodsCount,b.LoveLiness,b.CollectDateTime,(SELECT NickName FROM AccountsInfo(NOLOCK) a WHERE a.UserID=b.UserID) AS NickName 
	FROM LHGameRecordDB.dbo.RecordUserGift b WHERE b.TargetUserID=@dwUserID AND b.LoadStatus=0 AND b.SendType=1
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ����״̬����
CREATE PROC GSP_GP_GiftStatusUpdate
	@dwUserID INT,							 -- �û���ʶ		
	@dwRecordID INT,						 -- ��¼��ʶ		
	@strErrorDescribe NVARCHAR(127) OUTPUT	 -- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- �޸ļ�¼
	UPDATE LHGameRecordDB.dbo.RecordUserGift SET LoadStatus=1 WHERE RecordID=@dwRecordID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GR_SendGift
	@dwUserID INT,							 -- �û���ʶ		
	@dwTargetUserID INT,					 -- Ŀ���û�
	@cbIsGoldRoom TINYINT,					 -- �Ƿ��ҷ�
	@lCellScore BIGINT,					 	 -- ���ӵ׷�
	@lMinTableScore BIGINT,					 -- ������ͷ���
	@dwGoodsID INT,							 -- ��Ʒ��ʶ	
	@dwGoodsCount INT,						 -- ��Ʒ����	
	@szClientIP	NVARCHAR(15),				 -- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT	 -- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- �ж��Լ�
	IF @dwUserID=@dwTargetUserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1319,"Msg":"��Ǹ�������ܸ��Լ����������ʧ�ܣ�"}'	
		RETURN 5
	END

	-- ��ʼ����
	DECLARE @MinRetainGold_1 BIGINT
	DECLARE @MinRetainGold_2 BIGINT
	DECLARE @MinRetainGold_3 BIGINT
	
	SET @MinRetainGold_1=3000000
	SET @MinRetainGold_2=5000000
	SET @MinRetainGold_3=12000000

	-- ��ѯ���
	IF NOT EXISTS(SELECT * FROM AccountsInfo(NOLOCK) WHERE UserID=@dwTargetUserID)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1314,"Msg":"��Ǹ����Ҫ���͵���Ҳ����ڣ�����ʧ�ܣ�"}'	
		RETURN 1
	END

	-- ��������
	DECLARE @RecordID BIGINT
	DECLARE @PriceKind TINYINT
	DECLARE @PriceCount BIGINT
	DECLARE @LoveLiness INT
	DECLARE @MyLoveLiness INT
	DECLARE @TargetLoveLiness INT
	
	-- ���ñ���
	SET @RecordID=0
	
	-- ��ѯ��Ʒ
	SELECT @PriceKind=MoneyID, @PriceCount=Price, @LoveLiness=LoveLiness FROM LHPlatformDB.dbo.GameGiftInfo(NOLOCK) WHERE GoodsID=@dwGoodsID
	IF @PriceKind IS NULL
	BEGIN 
		SET @strErrorDescribe=N'{"Idx":1313,"Msg":"��Ǹ��ϵͳδ��ѯ����������Ϣ������ʧ�ܣ�"}'	
		RETURN 2
	END 
	
	-- ��������
	DECLARE @Score BIGINT

	-- �ж�����Ƹ�
	IF @PriceKind=1
	BEGIN
		SELECT @Score=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID
			
		-- ��ҷ��жϵ׷�
		IF @cbIsGoldRoom=1
		BEGIN
			-- ���ñ������
			DECLARE @MinRetainGold BIGINT
			IF @lCellScore=10000 BEGIN SET @MinRetainGold=@MinRetainGold_1 END
			ELSE IF @lCellScore=20000 BEGIN SET @MinRetainGold=@MinRetainGold_2 END
			ELSE IF @lCellScore=50000 BEGIN SET @MinRetainGold=@MinRetainGold_3 END
			ELSE BEGIN SET @MinRetainGold=@lMinTableScore END
				
			IF @Score-@PriceCount*@dwGoodsCount<@MinRetainGold
			BEGIN 
				SET @strErrorDescribe=N'{"Idx":1317,"Msg":"��Ǹ������Ҫ����'+CAST(@MinRetainGold AS NVARCHAR(16))+'�Ļƽ����ڻƽ��㣬����ʧ�ܣ�","Ops":["'+CAST(@MinRetainGold/1000 AS NVARCHAR(16))+'"]}'	
				RETURN 6
			END
		END
		ELSE 
		BEGIN
			IF @Score<@PriceCount*@dwGoodsCount
			BEGIN 
				SET @strErrorDescribe=N'{"Idx":1316,"Msg":"��Ǹ�����Ļƽ��㣬����ʧ�ܣ�"}'	
				RETURN 4
			END
		END
			
	END ELSE
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1318,"Msg":"��֧�ֵ�֧����ʽ������ϵ����Ա��"}'	
		RETURN 5
	END	
	
	-- ���²Ƹ� @Reason=16 -> ��������
	DECLARE @Price BIGINT
	SET @Price=-@PriceCount*@dwGoodsCount
	EXEC GSP_GP_WriteGameWealth @dwUserID,@PriceKind,16,@Price
	
	-- ��������ֵ
	UPDATE AccountsInfo SET LoveLiness=LoveLiness+@LoveLiness WHERE UserID IN (@dwUserID, @dwTargetUserID)
	
	-- ��ѯ����ֵ
	SELECT @MyLoveLiness=LoveLiness FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
	SELECT @TargetLoveLiness=LoveLiness FROM AccountsInfo(NOLOCK) WHERE UserID=@dwTargetUserID
	
	-- ���ڱ�ʶ
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	
	-- ��¼�Լ�����ֵ
	UPDATE LHGameRecordDB.dbo.ReportDayUser SET LoveLiness=LoveLiness+@LoveLiness WHERE UserID=@dwUserID AND DateID=@DateID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO LHGameRecordDB.dbo.ReportDayUser(DateID,UserID,PayCount,PayAmount,LastPayAmount,LastPayDate,LoveLiness,ReportDateTime) 
		VALUES (@DateID,@dwUserID,0,0,0,GETDATE(),@LoveLiness,GETDATE())
	END
	
	-- ��¼�Է�����ֵ
	UPDATE LHGameRecordDB.dbo.ReportDayUser SET LoveLiness=LoveLiness+@LoveLiness WHERE UserID=@dwTargetUserID AND DateID=@DateID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO LHGameRecordDB.dbo.ReportDayUser(DateID,UserID,PayCount,PayAmount,LastPayAmount,LastPayDate,LoveLiness,ReportDateTime) 
		VALUES (@DateID,@dwTargetUserID,0,0,0,GETDATE(),@LoveLiness,GETDATE())
	END
	
	-- �����¼
	INSERT INTO LHGameRecordDB.dbo.RecordUserGift(UserID,TargetUserID,GoodsID,GoodsCount,LoveLiness,SendType,LoadStatus,CollectClientIP,CollectDateTime)
	VALUES (@dwUserID,@dwTargetUserID,@dwGoodsID,@dwGoodsCount,@LoveLiness,2,1,@szClientIP,GETDATE())
	
	-- �������
	SELECT @PriceKind AS MoneyID, -@Price AS Price, @LoveLiness AS LoveLiness, @MyLoveLiness AS MyLoveLiness, @TargetLoveLiness AS TargetLoveLiness

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
