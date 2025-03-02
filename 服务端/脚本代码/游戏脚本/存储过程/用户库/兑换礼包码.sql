----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ExchangeGiftbagCode]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ExchangeGiftbagCode]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �һ������
CREATE PROC GSP_GP_ExchangeGiftbagCode
	@dwUserID INT,							-- �û���ʶ			
	@wReasonID INT,							-- ���ԭ��
	@strGiftbagCode NVARCHAR(16),			-- �����
	@strClientAddr NVARCHAR(16),			-- �û���ַ	
	@strErrorDescribe NVARCHAR(127) OUTPUT  -- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �Զ��ع�
SET XACT_ABORT ON

-- ִ���߼�
BEGIN
	
	-- ��������
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @MarketID INT
	DECLARE @Accounts NVARCHAR(31)	

	-- ��ѯ�û�
	SELECT @UserID=a.UserID,@Accounts=a.Accounts,@MarketID=a.MarketID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- У���û�
	IF @UserID IS NULL 
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
		RETURN 1
	END

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����"}'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�"}'
		RETURN 3
	END	

	-- �����ж�
	IF LEN(@strGiftbagCode) < 6 OR LEN(@strGiftbagCode) > 12
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1307,"Msg":"�������Ч�����������룡"}' 
		RETURN 4
	END

	-- ��������
	DECLARE @CurDateTiem DATETIME
	SET @CurDateTiem=GETDATE()

	-- �������Ϣ
	DECLARE @Count INT
	DECLARE @UsedCount INT
	DECLARE @LimitNum INT
	DECLARE @PlatformID INT
	DECLARE @Status TINYINT
	DECLARE @StartTime DATETIME
	DECLARE @ExpiresTime DATETIME
	SELECT @Count=Count, @UsedCount=UsedCount, @LimitNum=LimitNum, @PlatformID=PlatformID, @Status=Status, @StartTime=StartTime, @ExpiresTime=ExpiresTime
	FROM LHInsureDB.dbo.GiftbagCode WHERE GiftbagCodeID=@strGiftbagCode

	-- �����ж�
	IF @Count IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1308,"Msg":"����벻��ȷ�����������룡"}'
		RETURN 4
	END

	-- �����ж�
	IF @PlatformID <> 0 AND @MarketID <> @PlatformID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1308,"Msg":"����벻��ȷ�����������룡"}'
		RETURN 5
	END

	-- У��״̬
	IF @Status<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1309,"Msg":"�������ʧЧ���һ�ʧ�ܣ�"}'
		RETURN 5
	END

	-- �����ж�
	IF @CurDateTiem < @StartTime OR @CurDateTiem > @ExpiresTime
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1309,"Msg":"�������ʧЧ���һ�ʧ�ܣ�"}'
		RETURN 6
	END

	-- �����ж�
	IF @UsedCount >= @Count
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1310,"Msg":"��������Ѷһ��꣡"}'
		RETURN 7
	END

	-- �û���ȡ����
	DECLARE @UserUsedCount INT
	SELECT @UserUsedCount=COUNT(1) FROM LHInsureDB.dbo.GiftbagCodeExRecord WHERE GiftbagCodeID=@strGiftbagCode AND UserID=@dwUserID

	-- ��������
	IF @UserUsedCount IS NULL SET @UserUsedCount=0

	-- �����ж�
	IF @UserUsedCount >= @LimitNum
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1311,"Msg":"����������Ѷһ��꣡"}'
		RETURN 8
	END

	-- ��������
	DECLARE @Index INT
	DECLARE @LoopCount INT	
	DECLARE @ResidueCount INT	
	DECLARE @CurrentTime INT

	-- ��Ʒ��Ϣ
	DECLARE @IncGoodsID INT
	DECLARE @IncGoodsLevel INT
	DECLARE @IncGoodsCount INT
	DECLARE @IncGoodsIndate INT		

	-- ����ʱ��
	SET @CurrentTime=dbo.LHF_GetTimestamp()

	-- ���������
	DECLARE @BagGoodsList TABLE (ID INT identity(1,1),GoodsID SMALLINT,GoodsLevel SMALLINT,GoodsCount INT,ResidueCount INT,GoodsIndate INT)

	-- �����¼
	INSERT @BagGoodsList (GoodsID,GoodsLevel,GoodsCount,ResidueCount,GoodsIndate)
	SELECT GoodsID, 1, GoodsNumber, 0, Indate FROM LHInsureDB.dbo.GiftbagCodeGoods WHERE GiftbagCodeID=@strGiftbagCode
	
	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN
	
	-- ������Ʒ
	SELECT @Index=1,@LoopCount=COUNT(1) FROM @BagGoodsList	
	
	-- ѭ������
	WHILE @Index<=@LoopCount
	BEGIN	
		-- ��ѯ��Ϣ
		SELECT @IncGoodsID=GoodsID,@IncGoodsLevel=GoodsLevel,@IncGoodsCount=GoodsCount,@IncGoodsIndate=GoodsIndate FROM @BagGoodsList WHERE ID=@Index		

		-- У����Ʒ����
		IF dbo.LHF_IsMoneyGoods(@IncGoodsID) = 1
		BEGIN
			-- д��Ƹ�
			EXEC GSP_GP_WriteGameWealth @dwUserID,@IncGoodsID,@wReasonID,@IncGoodsCount

		END ELSE
		BEGIN
			-- �����Ʒ
			EXEC @ResidueCount = GSP_GP_AppendGoodsItem @dwUserID,@wReasonID,@IncGoodsID,@IncGoodsLevel,@IncGoodsCount,@IncGoodsIndate,@CurrentTime

			-- ʣ�ദ��
			IF @ResidueCount>0
			BEGIN
				-- ����ʣ��
				UPDATE @BagGoodsList SET ResidueCount=@ResidueCount WHERE ID=@Index				
			END		
		END
		
		-- ��������
		SET @Index=@Index+1
	END	

	-- ���´���
	UPDATE LHInsureDB.dbo.GiftbagCode SET UsedCount=UsedCount+1 WHERE GiftbagCodeID=@strGiftbagCode

	-- ��¼����
	DECLARE @RecordID INT
	INSERT INTO LHInsureDB.dbo.GiftbagCodeExRecord(GiftbagCodeID, UserID, Accounts, GoodsArrivalStatus, GoodsArrivalTime, ClientIP)
	VALUES(@strGiftbagCode, @dwUserID, @Accounts, 1, @CurDateTiem, @strClientAddr)

	-- ��¼ID
	SELECT @RecordID=SCOPE_IDENTITY()

	-- ��¼����
	INSERT INTO LHInsureDB.dbo.GiftbagCodeExGoods([RecordID], [GoodsID], [GoodsNumber], [Indate])
	SELECT @RecordID, GoodsID, GoodsNumber, Indate FROM LHInsureDB.dbo.GiftbagCodeGoods WHERE GiftbagCodeID=@strGiftbagCode

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- ʣ����Ʒ
	DECLARE @GiftBagGoodsList NVARCHAR(MAX)	

	-- ����JSON
	SET @GiftBagGoodsList=(SELECT GoodsID, GoodsLevel AS Level, GoodsCount AS Count, GoodsIndate AS Indate FROM @BagGoodsList FOR JSON AUTO)

	-- ��������	
	IF @GiftBagGoodsList IS NULL SET @GiftBagGoodsList=N''

	-- �׳���¼
	SELECT @GiftBagGoodsList AS GiftBagGoodsList
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------