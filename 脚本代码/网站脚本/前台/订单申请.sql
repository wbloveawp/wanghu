----------------------------------------------------------------------
-- ��;�����붩��
----------------------------------------------------------------------

USE LHInsureDB
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

-- ���붩��
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WEB_VAS_RequestOrder]') AND OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[WEB_VAS_RequestOrder]
GO

-- ��ȡ����
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WEB_VAS_GetOnLineOrder]') AND OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[WEB_VAS_GetOnLineOrder]
GO

----------------------------------------------------------------------------------------------------
-- ���붩��
CREATE PROC WEB_VAS_RequestOrder
	@dwChannelID		INT,						-- ������ʶ
	@strIpayOrderID		NVARCHAR(64),				-- ֧��ƽ̨��ˮ��	
	@dwCommodityID		INT,						-- ��Ʒ��ʶ	
	@strCommodityName	NVARCHAR(32),				-- ��Ʒ����
	@dtOrderKind		TINYINT,					-- �������ͣ�1���̳� 2�������
	@dwUserID			INT,						-- �û���ʶ
	@strOrderID			BIGINT,						-- ��������	
	@dcOrderAmount		DECIMAL(18,2),				-- �������	
	@strClientIP		NVARCHAR(15),				-- �µ���ַ
	@strErrorDescribe	NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

--��������
SET NOCOUNT ON

-- �ʺ�����
DECLARE @Accounts NVARCHAR(31)
DECLARE @GameID INT
DECLARE @UserID INT
DECLARE @Nullity TINYINT
DECLARE @SpreadUserID INT
DECLARE @SpreadAgentID INT
DECLARE @SpreadScale DECIMAL(18,3)
DECLARE @SpreadChannelID INT
DECLARE @SpreadChannelRate DECIMAL(18,3)

-- ������Ϣ
DECLARE @OrderID BIGINT
DECLARE @OrderAmount DECIMAL(18,2)
DECLARE @OrderState TINYINT
DECLARE @CreateClientIP NVARCHAR(15)

-- ִ���߼�
BEGIN
	--------------------------- ��֤�û� begin -----------------------------------
	SELECT @UserID=UserID,@GameID=GameID,@Accounts=Accounts,@Nullity=Nullity,@SpreadUserID=ParentID,@SpreadChannelID=SpreadChannelID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK)
	WHERE UserID=@dwUserID
	
	-- �û�����
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�'
		RETURN 100
	END	

	-- �ʺŷ��
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 101
	END		
	------------------------------ ��֤�û� end -----------------------------------

	IF EXISTS(SELECT OrderID FROM PayOrder(NOLOCK) WHERE OrderID=@strOrderID)
	BEGIN
		SET @strErrorDescribe=N'��Ǹ���ö����Ѵ���,�����³�ֵ��'
		RETURN 400
	END

	IF EXISTS(SELECT OrderID FROM PayOrder(NOLOCK) WHERE ChannelID=@dwChannelID AND IpayOrderID=@strIpayOrderID)
	BEGIN
		SET @strErrorDescribe=N'��Ǹ���ö����Ѵ���,�����³�ֵ��'
		RETURN 401
	END

	-- ��ȡ������Ϣ
	DECLARE @ChannelID INT
	DECLARE @ChannelRate DECIMAL(18,3)
	DECLARE @PlatformID INT	
	DECLARE @PlatformRate DECIMAL(18,3)
	SELECT @ChannelID=ChannelID,@ChannelRate=ChannelRate,@PlatformID=PlatformID,@PlatformRate=PlatformRate FROM ChannelConfig WHERE ChannelID=@dwChannelID
	IF @ChannelID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֧�����������ڡ�'
		RETURN 402
	END

	-- ��ȡ�ƹ�������Ϣ
	SELECT @SpreadChannelRate=ChannelRate FROM LHAccountsDB.dbo.ChannelInfo WHERE ChannelID=@SpreadChannelID
	IF @SpreadChannelRate IS NULL
		SET @SpreadChannelRate=0

	-- ��ȡ�ϼ�
	IF @SpreadUserID<>0
		SELECT @SpreadAgentID=AgentID,@SpreadScale=AgentScale FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@SpreadUserID
	ELSE
		SELECT @SpreadAgentID=0,@SpreadScale=0

	-- �����ƹ����
	IF @SpreadScale=0
		SET @SpreadScale=0.05 -- Ĭ��5%�ĳ�ֵ����

	-- ��ͨ������г�ֵ����
	IF @SpreadAgentID<>1
		SET @SpreadScale=0

	-- ���붩��
	INSERT INTO PayOrder(OrderID,OrderKind,OrderAmount,UserID,Accounts,ChannelID,ChannelRate,PlatformID,PlatformRate,CommodityID,CommodityName,SpreadChannelID,SpreadChannelRate,SpreadUserID,SpreadRate,CreateClientIP)
	VALUES(@strOrderID,@dtOrderKind,@dcOrderAmount,@UserID,@Accounts,@ChannelID,@ChannelRate,@PlatformID,@PlatformRate,@dwCommodityID,@strCommodityName,@SpreadChannelID,@SpreadChannelRate,@SpreadUserID,@SpreadScale,@strClientIP)

	-- ����������Ʒ
	IF @dtOrderKind=1
	BEGIN
		DECLARE @ProperGoodsList NVARCHAR(MAX)
		SELECT @ProperGoodsList=GoodsInfo FROM MallGoods WHERE MallgoodsID=@dwCommodityID
		IF @ProperGoodsList IS NOT NULL
		BEGIN
			-- ��������
			DECLARE @Index INT
			DECLARE @LoopCount INT	

			-- ��Ʒ��Ϣ
			DECLARE @IncGoodsID BIGINT
			DECLARE @IncGoodsCount BIGINT
			DECLARE @IncGoodsIndate INT	

			-- ���������
			DECLARE @TableProperGoods TABLE (ID INT identity(1,1),GoodsID BIGINT,GoodsCount INT,GoodsIndate INT)

			-- �����¼
			INSERT @TableProperGoods (GoodsID,GoodsCount,GoodsIndate)
			SELECT JSON_VALUE(VALUE,'$.GoodsID'),JSON_VALUE(VALUE,'$.GoodsCount'),JSON_VALUE(VALUE,'$.Indate') FROM OPENJSON(@ProperGoodsList)

			-- ������Ʒ
			SELECT @Index=1,@LoopCount=COUNT(1) FROM @TableProperGoods

			-- ѭ������
			WHILE @Index<=@LoopCount
			BEGIN	
				-- ��ѯ��Ϣ
				SELECT @IncGoodsID=GoodsID,@IncGoodsCount=GoodsCount,@IncGoodsIndate=GoodsIndate FROM @TableProperGoods WHERE ID=@Index		

				-- ���붩����Ʒ
				INSERT INTO PayOrderGoods(OrderID,GoodsID,GoodsNumber,Indate) VALUES(@strOrderID,@IncGoodsID,@IncGoodsCount,@IncGoodsIndate)
		
				-- ��������
				SET @Index=@Index+1
			END
		END
	END

	-- ������Ϣ��ѯ
	SELECT @ChannelID=ChannelID,@UserID=UserID,@Accounts=Accounts,@OrderID=OrderID,@OrderAmount=OrderAmount,@OrderState=OrderState,@CreateClientIP=CreateClientIP
	FROM PayOrder(NOLOCK) WHERE OrderID=@strOrderID

	-- �������
	SELECT @ChannelID AS ChannelID,@UserID AS UserID,@Accounts AS Accounts,@OrderID AS OrderID,@OrderAmount AS OrderAmount,@OrderState AS OrderState,@CreateClientIP AS CreateClientIP
END

RETURN 0 
GO

----------------------------------------------------------------------------------------------------
-- ��ȡ������Ϣ
CREATE PROC WEB_VAS_GetOnLineOrder
	@strOrderID			NVARCHAR(32)			-- ��������
WITH ENCRYPTION AS

--��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @ChannelID INT
DECLARE @UserID INT
DECLARE @Accounts NVARCHAR(32)
DECLARE @OrderID BIGINT
DECLARE @OrderAmount DECIMAL(18,2)
DECLARE @OrderState TINYINT
DECLARE @IPAddress NVARCHAR(15)

-- ִ���߼�
BEGIN
	IF NOT EXISTS(SELECT OrderID FROM PayOrder(NOLOCK) WHERE OrderID=@strOrderID)
	BEGIN
		RETURN 400
	END

	-- ������Ϣ��ѯ
	SELECT @ChannelID=ChannelID,@UserID=UserID,@Accounts=Accounts,@OrderID=OrderID,@OrderAmount=OrderAmount,@OrderState=OrderState,@IPAddress=CreateClientIP
	FROM PayOrder(NOLOCK) WHERE OrderID=@strOrderID

	-- �������
	SELECT @ChannelID AS ChannelID,@UserID AS UserID,@Accounts AS Accounts,@OrderID AS OrderID,@OrderAmount AS OrderAmount,@OrderState AS OrderState,@IPAddress AS IPAddress
END

RETURN 0
GO