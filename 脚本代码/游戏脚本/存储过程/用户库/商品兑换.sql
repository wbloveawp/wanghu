----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ExchangeMallGoods]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ExchangeMallGoods]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- �һ���Ʒ
CREATE PROC GSP_GP_ExchangeMallGoods
	@dwUserID INT,							-- �û���ʶ			
	@dwMallGoodsID INT,						-- ��Ʒ��ʶ	
	@wReason INT,							-- ���ԭ��		
	@dwTimeStamp INT,						-- ʱ���			
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
	DECLARE @Accounts NVARCHAR(32)
	DECLARE @MemberOrder SMALLINT
	
	-- ��ѯ�û�
	SELECT @UserID=a.UserID, @Accounts=a.Accounts, @Nullity=a.Nullity, @MemberOrder=a.MemberOrder, @ShutDown=b.ShutDownStatus 
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- ��ѯ�û�
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

	-- ��ѯ����
	DECLARE @GoodsID BIGINT
	DECLARE @MoneyID BIGINT
	DECLARE @GoodsCount INT	
	DECLARE @GoodsIndate INT	
	DECLARE @SalePrice INT
	DECLARE @VipLimitMin SMALLINT	
	DECLARE @CommodityTypeID INT
	DECLARE @CommodityName NVARCHAR(32)
	SELECT @GoodsID=GoodsID,@GoodsCount=GoodsCount,@GoodsIndate=GoodsIndate,@MoneyID=MoneyID,@SalePrice=Price,@VipLimitMin=VipLimitMin,@CommodityName=CommodityName,@CommodityTypeID=CommodityTypeID 
	FROM LHInsureDB.dbo.EMallCommodity(NOLOCK) WHERE CommodityID=@dwMallgoodsID

	-- ��Ʒ������
	IF @GoodsID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1285,"Msg":"��Ǹ��ϵͳδ��ѯ������Ʒ��Ϣ������ʧ�ܣ�"}'	

		RETURN 4
	END

	-- У���������
	IF @MoneyID<>1 AND @MoneyID<>2
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1285,"Msg":"��Ǹ������Ʒ����ʹ��sawa point����"}'

		RETURN 5
	END

	-- У��ȼ�
	IF @VipLimitMin>@MemberOrder
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1286,"Msg":"��Ǹ������Ʒ��VIP'+CONVERT(NVARCHAR(3),@VipLimitMin)+N'�ſɹ���","Ops":["'+CONVERT(NVARCHAR(3),@VipLimitMin)+'"]}'

		RETURN 6
	END

	-- ������Ʒ
	DECLARE @GoodsKind SMALLINT	
	DECLARE @GoodsName NVARCHAR(64)
	SELECT @GoodsKind=GoodsKind, @GoodsName=GoodsName FROM LHPlatformDB.dbo.GameGoodsInfo(NOLOCK) WHERE GoodsID=@GoodsID

	-- ��Ʒ������
	IF @GoodsKind IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1285,"Msg":"��Ǹ��ϵͳδ��ѯ������Ʒ��Ϣ������ʧ�ܣ�"}'	

		RETURN 7
	END

	-- ��������
	DECLARE @ResultCode INT
	DECLARE @BeforeCount INT
	DECLARE @ResidueCount INT		
	DECLARE @DeductAmount BIGINT	
	DECLARE @ExpendAmount BIGINT

	-- ���ñ���	
	SELECT @ExpendAmount=@SalePrice,@DeductAmount=-@SalePrice

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN	

	-- �۳�����
	EXEC @ResultCode=GSP_GP_WriteGameWealth @dwUserID,@MoneyID,@wReason,@DeductAmount
	IF @ResultCode>0
	BEGIN
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ������Ϣ
		SET @strErrorDescribe=N'{"Idx":1296,"Msg":"��Ǹ�����������ϵ�sawa point���㣬�һ�ʧ�ܣ�"}'	
		RETURN 6
	END

	-- �����Ʒ
	IF dbo.LHF_IsMoneyGoods(@GoodsID)=1
	BEGIN
		EXEC GSP_GP_WriteGameWealth @dwUserID,@GoodsID,@wReason,@GoodsCount
	END ELSE
	BEGIN
		EXEC GSP_GP_AppendGoodsItem @dwUserID,@wReason,@GoodsID,1,@GoodsCount,@GoodsIndate,@dwTimeStamp
	END

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- �һ���¼
	INSERT INTO LHInsureDB.dbo.EMallOrder(UserID,Accounts,CommodityID,CommodityName,CommodityTypeID,CommodityCount,Price,OrderAmount,GoodsID,GoodsCount,OrderStatus)
	VALUES(@dwUserID,@Accounts,@dwMallGoodsID,@CommodityName,@CommodityTypeID,1,@SalePrice,@SalePrice,@GoodsID,@GoodsCount,1)

	-- ��ѯ���
	DECLARE @MoneyBalance BIGINT

	-- ���
	IF @MoneyID=1
	BEGIN
		SELECT @MoneyBalance=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID
	END

	-- ��������
	IF @MoneyBalance IS NULL SET @MoneyBalance=0

	-- �׳���¼	
	SELECT @MoneyBalance AS MoneyBalance,@MoneyID AS ExpendMoneyID, @GoodsID AS IncGoodsID, @GoodsCount AS IncGoodsCount, @GoodsIndate AS IncGoodsIndate
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------