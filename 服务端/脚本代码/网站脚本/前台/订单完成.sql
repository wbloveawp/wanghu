USE LHInsureDB
GO

/************************��ɶ���************************/
/*
OrderKind���������ͣ�1���̳� 2�����
*/
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WEB_VAS_FilledOnline]')) 
DROP PROCEDURE [dbo].[WEB_VAS_FilledOnline]

GO

CREATE PROC WEB_VAS_FilledOnline
	@strOrderID				BIGINT,						-- ��������
	@dcPayAmount			DECIMAL(18,2),				-- ʵ��֧�����
	@dwChangeWay			TINYINT,					-- �ı䷽ʽ 1��֧������ 2����̨�ֶ�����
	@strIpayOrderID			NVARCHAR(64),				-- ֧��ƽ̨��ˮ��	
	@strClientIP			NVARCHAR(15),				-- ���õ�ַ		
	@strErrorDescribe		NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON
SET XACT_ABORT ON 

DECLARE @OrderID BIGINT
DECLARE @UserID INT
DECLARE @ChannelID INT
DECLARE @PlatformID INT
DECLARE @SpreadChannelID INT
DECLARE @CommodityID INT
DECLARE @PayUserCount INT
DECLARE @PayFirst TINYINT

DECLARE @OrderAmount DECIMAL(18,3)
DECLARE @ChannelAmount DECIMAL(18,3)
DECLARE @PlatformAmount DECIMAL(18,3)
DECLARE @SpreadChannelAmount DECIMAL(18,3)
DECLARE @CompanyAmount DECIMAL(18,3)
DECLARE @SpreadUserID INT
DECLARE @SpreadRate DECIMAL(18,2)
DECLARE @SpreadDivideAmount DECIMAL(18,3)
DECLARE @SpreadDivideScore BIGINT
DECLARE @SpreadAgentID INT
DECLARE @SpreadValidCount INT
DECLARE @SpreadValidAward BIGINT

DECLARE @OrderState TINYINT
DECLARE @OrderKind TINYINT
DECLARE @CurrentDate DATETIME
DECLARE @DateID INT

DECLARE @ChannelRate DECIMAL(18,3)
DECLARE @PlatformRate DECIMAL(18,3)
DECLARE @SpreadChannelRate DECIMAL(18,3)
DECLARE @MemberPoint INT
DECLARE @GameID INT
DECLARE @DoubleStatus TINYINT
DECLARE @ProfitAmount DECIMAL(18,3)

-- ִ���߼�
BEGIN
	SET @CurrentDate=GETDATE()
	SET @DateID=CAST(CAST(@CurrentDate AS FLOAT) AS INT)
	SELECT @DoubleStatus=0,@PayUserCount=0,@PayFirst=0,@SpreadValidCount=0,@SpreadValidAward=0

	-- ��ѯ��ǰ����״̬�������������Ⲣ��������ݴ���
	SELECT @OrderID=OrderID,@UserID=UserID,@ChannelID=ChannelID,@PlatformID=PlatformID,@CommodityID=CommodityID,@OrderAmount=OrderAmount,@OrderState=OrderState,@OrderKind=OrderKind,
		@ChannelRate=ChannelRate,@PlatformRate=PlatformRate,@SpreadChannelRate=SpreadChannelRate,@SpreadUserID=SpreadUserID,@SpreadRate=SpreadRate
	FROM PayOrder(NOLOCK) WHERE OrderID=@strOrderID

	-- ��֤����
	IF @OrderID IS NULL
	BEGIN
		SET @strErrorDescribe='����������'
		RETURN 410
	END

	-- ���¶���״̬
	UPDATE PayOrder SET OrderState=1 WHERE OrderID=@strOrderID AND OrderState<>1
	IF @@ROWCOUNT=0
	BEGIN
		SET @strErrorDescribe='������֧��'
		RETURN 411
	END

	-- ��ֵ��¼�ظ�
	IF EXISTS(SELECT OrderID FROM PayRecord(NOLOCK) WHERE OrderID=@strOrderID)
	BEGIN
		SET @strErrorDescribe='������֧��'
		RETURN 412
	END 

	-- �жϽ��
	IF @dcPayAmount<@OrderAmount
	BEGIN
		SET @strErrorDescribe='֧�����Ͷ�����һ��'
		RETURN 413
	END

	-- ��ȡ�ƹ�Ĵ���ȼ�
	SELECT @SpreadAgentID=AgentID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@SpreadUserID
	IF @SpreadAgentID IS NULL
		SET @SpreadAgentID=0

	-- �û���Ϣ
	SELECT @MemberPoint=MemberPoint,@GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo WHERE UserID=@UserID

	--�׳�˫��/ÿ�շ���
	DECLARE @ProperGoodsList NVARCHAR(MAX)
	DECLARE @MallGoodsCount INT
	DECLARE @LastBuyDateTime DATETIME
	SELECT @ProperGoodsList=GoodsInfo,@DoubleStatus=DoubleStatus FROM MallGoods WHERE MallgoodsID=@CommodityID
	SELECT @MallGoodsCount=MallgoodsBuyCount,@LastBuyDateTime=LastBuyDateTime FROM MallGoodsBuyStat WHERE UserID=@UserID AND MallgoodsID=@CommodityID
	IF @DoubleStatus=1 AND (@MallGoodsCount IS NULL OR @MallGoodsCount=0)
	BEGIN
		UPDATE PayOrderGoods SET GoodsNumber=GoodsNumber*2 WHERE OrderID=@OrderID
	END
	ELSE IF @DoubleStatus=2 AND (@LastBuyDateTime IS NULL OR (@LastBuyDateTime IS NOT NULL AND CAST(CAST(@LastBuyDateTime AS FLOAT) AS INT)<>@DateID))
	BEGIN
		UPDATE PayOrderGoods SET GoodsNumber=GoodsNumber*2 WHERE OrderID=@OrderID
	END

	-- ��Ʒͳ��
	UPDATE MallGoodsBuyStat SET MallgoodsBuyCount=MallgoodsBuyCount+1,LastBuyDateTime=@CurrentDate WHERE UserID=@UserID AND MallgoodsID=@CommodityID
	IF @@ROWCOUNT=0
		INSERT INTO MallGoodsBuyStat(UserID,MallgoodsID,MallgoodsBuyCount,LastBuyDateTime) VALUES(@UserID,@CommodityID,1,@CurrentDate)

	-- ������
	SET @ChannelAmount=@dcPayAmount*@ChannelRate
	SET @PlatformAmount=@dcPayAmount*@PlatformRate
	SET @SpreadChannelAmount=@dcPayAmount*@SpreadChannelRate
	SET @SpreadDivideAmount=0

	-- ������ ֧�����-֧������������-ƽ̨���-�ƹ������̷ֳ�-�ƹ�Ա�ֳ�
	SET @ProfitAmount=@dcPayAmount-@ChannelAmount-@PlatformAmount-@SpreadChannelAmount-@SpreadDivideAmount
	SET @CompanyAmount=@ProfitAmount

	-- �����ƹ��ֵ����
	DECLARE @PayScore BIGINT
	SET @PayScore=CEILING(@dcPayAmount)*2600000
	SET @SpreadDivideScore=@PayScore*@SpreadRate

	-- �����ֵ����
	IF NOT EXISTS (SELECT OrderID FROM PayRecord(NOLOCK) WHERE UserID=@UserID)
	BEGIN
		SET @PayFirst=1

		-- ������Ч�û�
		UPDATE LHAccountsDB.dbo.AccountsInfo SET IsValid=1 WHERE UserID=@UserID
	END

	IF NOT EXISTS (SELECT OrderID FROM PayRecord(NOLOCK) WHERE UserID=@UserID AND DATEDIFF(D,PayDateTime,@CurrentDate)=0)
	BEGIN
		SET @PayUserCount=1
	END

	-- ���¶���
	UPDATE PayOrder SET PayAmount=@dcPayAmount,CompanyAmount=@CompanyAmount,ChannelAmount=@ChannelAmount,PlatformAmount=@PlatformAmount,DoubleStatus=@DoubleStatus,GoodsArrivalStatus=1,
		PayDateTime=@CurrentDate,CallbackClientIP=@strClientIP,IpayOrderID=@strIpayOrderID,OrderStatusChangeWay=@dwChangeWay,SpreadChannelAmount=@SpreadChannelAmount,
		SpreadDivideAmount=@SpreadDivideAmount,SpreadDivideScore=@SpreadDivideScore WHERE OrderID=@OrderID

	-- �����¼
	INSERT INTO PayRecord(OrderID,OrderKind,OrderAmount,PayAmount,UserID,Accounts,ChannelID,ChannelRate,ChannelAmount,PlatformID,PlatformRate,PlatformAmount,CommodityID,CommodityName,CompanyAmount,
		GoodsArrivalStatus,DoubleStatus,CollectDateTime,PayDateTime,CreateClientIP,CallbackClientIP,IpayOrderID,OrderStatusChangeWay,SpreadChannelID,SpreadChannelRate,SpreadChannelAmount,
		SpreadUserID,SpreadRate,SpreadDivideAmount,SpreadDivideScore) 
		SELECT OrderID,OrderKind,OrderAmount,PayAmount,UserID,Accounts,ChannelID,ChannelRate,ChannelAmount,PlatformID,PlatformRate,PlatformAmount,CommodityID,CommodityName,CompanyAmount,
		GoodsArrivalStatus,DoubleStatus,CollectDateTime,PayDateTime,CreateClientIP,CallbackClientIP,IpayOrderID,OrderStatusChangeWay,SpreadChannelID,SpreadChannelRate,SpreadChannelAmount, 
		SpreadUserID,SpreadRate,SpreadDivideAmount,SpreadDivideScore FROM PayOrder WHERE OrderID=@OrderID

	-- ������Ч�ƹ㽱��
	IF @SpreadUserID>0 AND @PayFirst=1
	BEGIN
		SET @SpreadValidCount=1
		IF @SpreadAgentID=0
			SET @SpreadValidAward=800000
	END

	-- ����ͳ��
	UPDATE ReportDayPay SET PayAmount=PayAmount+@dcPayAmount,PayCount=PayCount+1,ChannelAmount=ChannelAmount+@ChannelAmount,PlatformAmount=PlatformAmount+@PlatformAmount,PayUserCount=PayUserCount+@PayUserCount
		WHERE DateID=@DateID AND ChannelID=@ChannelID 
	IF @@ROWCOUNT=0
		INSERT INTO ReportDayPay(DateID,ChannelID,PlatformID,PayAmount,ChannelAmount,PlatformAmount,PayCount,PayUserCount) VALUES(@DateID,@ChannelID,@PlatformID,@dcPayAmount,@ChannelAmount,@PlatformAmount,1,1)

	-- ��Ʒͳ��
	UPDATE ReportDayCommodityPay SET PaySumAmount=PaySumAmount+@dcPayAmount,PayCount=PayCount+1,LastPayDateTime=@CurrentDate,LastPayAmount=@dcPayAmount
		WHERE DateID=@DateID AND CommodityID=@CommodityID AND OrderKind=@OrderKind
	IF @@ROWCOUNT=0
		INSERT INTO ReportDayCommodityPay(DateID,CommodityID,OrderKind,PaySumAmount,PayCount,LastPayDateTime,LastPayAmount) VALUES(@DateID,@CommodityID,@OrderKind,@dcPayAmount,1,@CurrentDate,@dcPayAmount)

	-- �û�ͳ��
	UPDATE LHGameRecordDB.dbo.ReportDayUser SET PayAmount=PayAmount+@dcPayAmount,PayCount=PayCount+1,LastPayAmount=@dcPayAmount,LastPayDate=@CurrentDate WHERE DateID=@DateID AND UserID=@UserID
	IF @@ROWCOUNT=0
		INSERT INTO LHGameRecordDB.dbo.ReportDayUser(DateID,UserID,PayCount,PayAmount,LastPayAmount,LastPayDate) VALUES(@DateID,@UserID,1,@dcPayAmount,@dcPayAmount,@CurrentDate)
	UPDATE AccountsPay SET PayCount=PayCount+1,PayAmount=PayAmount+@dcPayAmount,LastPayAmount=@dcPayAmount,LastPayDateTime=@CurrentDate WHERE UserID=@UserID
	IF @@ROWCOUNT=0
		INSERT INTO AccountsPay(UserID,PayCount,PayAmount,LastPayAmount,LastPayDateTime) VALUES(@UserID,1,@dcPayAmount,@dcPayAmount,@CurrentDate)

	-- ����ͳ��
	IF @SpreadUserID>0
	BEGIN
		UPDATE LHGameRecordDB.dbo.ReportDayUser SET SpreadValidCount=SpreadValidCount+@SpreadValidCount,SpreadValidAward=SpreadValidAward+@SpreadValidAward,SpreadPayUser=SpreadPayUser+@PayUserCount,
			SpreadPayAmount=SpreadPayAmount+@dcPayAmount,SpreadPayAward=SpreadPayAward+@SpreadDivideScore,ReportDateTime=@CurrentDate WHERE DateID=@DateID AND UserID=@SpreadUserID
		IF @@ROWCOUNT=0
			INSERT INTO LHGameRecordDB.dbo.ReportDayUser(DateID,UserID,SpreadValidCount,SpreadValidAward,SpreadPayUser,SpreadPayAmount,SpreadPayAward,ReportDateTime)
			VALUES(@DateID,@SpreadUserID,@SpreadValidCount,@SpreadValidAward,@PayUserCount,@dcPayAmount,@SpreadDivideScore,@CurrentDate)
	END

	-- ��ֵ
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

	-- ˫��
	IF @DoubleStatus<>0
	BEGIN
		UPDATE @TableProperGoods SET GoodsCount=GoodsCount*2
	END

	-- ������Ʒ
	SELECT @Index=1,@LoopCount=COUNT(1) FROM @TableProperGoods

	-- ѭ������
	WHILE @Index<=@LoopCount
	BEGIN	
		-- ��ѯ��Ϣ
		SELECT @IncGoodsID=GoodsID,@IncGoodsCount=GoodsCount,@IncGoodsIndate=GoodsIndate FROM @TableProperGoods WHERE ID=@Index		

		-- У����Ʒ����
		IF LHAccountsDB.dbo.BYF_IsMoneyGoods(@IncGoodsID) = 1
		BEGIN
			-- д��Ƹ�
			EXEC LHAccountsDB.dbo.GSP_GP_WriteGameWealth @UserID,@IncGoodsID,102,@IncGoodsCount

			-- ��ʯ���⴦��
			IF @IncGoodsID=1 
			BEGIN
				UPDATE LHAccountsDB.dbo.AccountsIngotInfo SET FillTimes=FillTimes+1,FillAmount=FillAmount+@IncGoodsCount WHERE UserID=@UserID
			END

			-- �ƽ����⴦��
			IF @IncGoodsID=2 
			BEGIN
				UPDATE LHAccountsDB.dbo.AccountsGoldInfo SET FillTimes=FillTimes+1,FillAmount=FillAmount+@IncGoodsCount WHERE UserID=@UserID
			END
		END 
		
		-- ��������
		SET @Index=@Index+1
	END

	-- �������
	SELECT  @UserID AS UserID,@GameID AS GameID,@dcPayAmount AS PayAmount,@SpreadUserID AS SpreadUserID,@SpreadAgentID AS SpreadAgentID,
		@SpreadDivideScore AS SpreadDivideScore,@SpreadValidCount AS SpreadValidCount,@SpreadValidAward AS SpreadValidAward
END

RETURN 0
GO
