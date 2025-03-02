
----------------------------------------------------------------------------------------------------

USE LHGameRankingDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_SeansonBalance]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_SeansonBalance]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_InsertRewardMail]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_InsertRewardMail]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE  PROCEDURE dbo.GSP_GR_SeansonBalance		
	@dwCurrentTime INT,					-- ��ǰʱ��
	@dwMailIndate  INT					-- �ʼ���Ч��
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �Զ��ع�
SET XACT_ABORT ON

-- ִ���߼�
BEGIN
	
	-- ��ѯ����
	DECLARE @RankingOption NVARCHAR(1024)
	SELECT @RankingOption=OptionValue FROM LHPlatformDB.dbo.SystemOptionInfo(NOLOCK) WHERE OptionName=N'RankingOption'

	-- У������
	IF ISJSON(@RankingOption)=0
	BEGIN
		RETURN 0
	END

	-- ��������
	DECLARE @SeasonRewards NVARCHAR(MAX)
	SET @SeasonRewards=JSON_QUERY(@RankingOption,N'$.SeasonRewards')

	-- ��������
	DECLARE @TableRewardsOption Table(RankMainOrder SMALLINT, GoodsID SMALLINT, GoodsCount INT, GoodsIndate INT)

	-- ��������
	IF ISJSON(@SeasonRewards)>0
	BEGIN
		INSERT INTO @TableRewardsOption(RankMainOrder,GoodsID, GoodsCount,GoodsIndate)
		SELECT JSON_VALUE(a.VALUE,N'$.RankMainOrder'),b.GoodsID,b.GoodsCount,b.GoodsIndate FROM 
		OPENJSON(@SeasonRewards) a 
		CROSS APPLY OPENJSON(a.VALUE,N'$.Rewards')WITH
		(
			GoodsID INT N'$.GoodsID',
			GoodsCount INT N'$.Count',
			GoodsIndate INT N'$.Indate'
		) b
	END                                                                                                                                                                                                                                                                                                                                                                             

	-- ���������
	DECLARE @TableGameRankLevel TABLE (LevelID INT PRIMARY KEY, NeedScore INT,SpanScore INT, MainOrder SMALLINT, SubOrder SMALLINT,StarOrder SMALLINT)

	-- �������ñ�
	INSERT INTO @TableGameRankLevel(LevelID,NeedScore,SpanScore,MainOrder,SubOrder,StarOrder)
	SELECT LevelID,NeedScore,SpanScore,MainOrder,SubOrder,StarOrder FROM LHPlatformDB.dbo.GameRankLevel(NOLOCK)

	-- ������ʱ��
	SELECT UserID,Score,MainOrder,SubOrder,StarOrder INTO #UserRankInfo FROM GameScoreInfo(NOLOCK)

	-- ��������
	DECLARE @MailID BIGINT
	DECLARE @MaxOrder INT
	DECLARE @MainOrder INT	
	DECLARE @ExpireTime INT

	-- ����ʱ��
	SET @ExpireTime=@dwCurrentTime+@dwMailIndate

	-- ����ʼ�
	TRUNCATE TABLE RecordRankReward

	-- ���ñ���
	SELECT @MainOrder=MIN(MainOrder),@MaxOrder=MAX(MainOrder) FROM LHPlatformDB.dbo.GameRankLevel(NOLOCK)

	-- ��������
	DECLARE @TableRewardMailInfo TABLE(
		MainOrder SMALLINT,
		MailID BIGINT, 
		MailKind TINYINT, 
		MailState TINYINT, 
		SendTime INT, 
		ExpireTime INT,
		MailTitle NVARCHAR(64), 		
		MailContent NVARCHAR(256), 
		AttachGoods NVARCHAR(512)
	)

	-- �����ʼ�
	WHILE @MainOrder<=@MaxOrder
	BEGIN
		IF EXISTS(SELECT 1 FROM @TableRewardsOption WHERE RankMainOrder=@MainOrder)
		BEGIN
			-- �����ʼ�
			INSERT INTO UHWAccountsDb.dbo.AccountsMailInfo(MailKind,MailType,MailTitle,MailContent,SendTime,ExpireTime,CollectTime)
			VALUES (2,3,N'��λ��������',N'������������',@dwCurrentTime,@ExpireTime,GetDate())

			-- �ʼ���ʶ
			SET @MailID=SCOPE_IDENTITY()		

			-- ���븽��
			INSERT INTO UHWAccountsDb.dbo.AccountsMailAttach(MailID,GoodsID,GoodsCount,GoodsIndate)
			SELECT @MailID,GoodsID,GoodsCount,0 FROM @TableRewardsOption WHERE RankMainOrder=@MainOrder			

			-- �����ʼ�
			INSERT INTO @TableRewardMailInfo(MainOrder,MailID,MailKind,MailState,MailTitle,SendTime,ExpireTime,MailContent,AttachGoods)
			SELECT @MainOrder, MailID, MailKind, 1, MailTitle, SendTime, ExpireTime, MailContent, 
			(SELECT JSON_QUERY(VALUE,N'$.Rewards') FROM OPENJSON(@SeasonRewards) WHERE JSON_VALUE(VALUE,N'$.RankMainOrder')=@MainOrder)
			FROM UHWAccountsDb.dbo.AccountsMailInfo WHERE MailID=@MailID
		END

		-- �ȼ�����
		SET @MainOrder=@MainOrder+1
	END

	-- ������ʶ
	DECLARE @SeasonID INT
	SET @SeasonID=LHPlatformDB.dbo.PFF_GetSeasonID()
	
	-- �����¼
	TRUNCATE TABLE RecordRankReward

	-- ɾ������
	DELETE RecordRankHistory WHERE SeasonID=@SeasonID

	-- Ĭ�ϴ���
	DECLARE @DefaultScore INT
	DECLARE @DefaultMainOrder SMALLINT
	DECLARE @DefaultSubOrder SMALLINT
	DECLARE @DefaultStarOrder SMALLINT

	-- Ĭ�Ϸ���
	SET @DefaultScore=900

	-- ��ѯ��λ
	SELECT @DefaultMainOrder=MainOrder,@DefaultSubOrder=SubOrder,@DefaultStarOrder=StarOrder FROM @TableGameRankLevel WHERE @DefaultScore>=NeedScore AND @DefaultScore<NeedScore+SpanScore

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ���뽱��
	INSERT INTO RecordRankReward(UserID,MainOrder,MailID,ExpireTime,SendState)
	SELECT a.UserID,a.MainOrder,b.MailID,@ExpireTime,0 FROM #UserRankInfo a	CROSS APPLY @TableRewardMailInfo b WHERE b.MainOrder=a.MainOrder

	-- �����¼
	INSERT INTO RecordRankHistory (UserID,SeasonID,RankScore,MainOrder,SubOrder,StarOrder,CollectTime)
	SELECT UserID,@SeasonID,Score,MainOrder,SubOrder,StarOrder,GetDate() FROM #UserRankInfo(NOLOCK)

	-- ���δ���
	UPDATE GameScoreInfo SET Score = CASE WHEN b.NeedScore<@DefaultScore THEN @DefaultScore ELSE b.NeedScore END, MainOrder=b.MainOrder,SubOrder=b.SubOrder,StarOrder=b.StarOrder
	FROM GameScoreInfo(NOLOCK) a CROSS APPLY @TableGameRankLevel b 	WHERE 
	b.MainOrder=(
	CASE a.MainOrder
	WHEN 6 THEN 
		@DefaultMainOrder
	ELSE 
		a.MainOrder+1 
	END)	
	AND  
	b.SubOrder=(
	CASE a.MainOrder
	WHEN 3 THEN
		CASE a.SubOrder
		WHEN 5 THEN 4
		ELSE a.SubOrder END
	WHEN 5 THEN
		CASE a.SubOrder
		WHEN 4 THEN 3
		ELSE a.SubOrder END
	WHEN 6 THEN
		@DefaultSubOrder
	ELSE 	
		CASE a.SubOrder 
		WHEN 0 THEN 1 
		ELSE a.SubOrder END
	END)
	AND
	b.StarOrder=(
	CASE a.MainOrder
	WHEN 5 THEN
		CASE WHEN a.SubOrder>=3 THEN @DefaultStarOrder
		ELSE 
			CASE a.StarOrder
			WHEN 4 THEN 3
			ELSE a.StarOrder END
		END
	WHEN 6 THEN
		@DefaultStarOrder
	ELSE a.StarOrder
	END) 	

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- ɾ����ʱ��
	IF OBJECT_ID('tempdb..#UserRankInfo') IS NOT NULL
	BEGIN
		DROP TABLE #UserRankInfo
	END

	-- �׳��ʼ�
	SELECT * FROM @TableRewardMailInfo	

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- �����ʼ�
CREATE  PROCEDURE dbo.GSP_GR_InsertRewardMail		
	@wUserCount INT					-- �û�����
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �Զ��ع�
SET XACT_ABORT ON

-- ִ���߼�
BEGIN
	
	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

		-- ��ѯ�ʼ�
		SELECT TOP(@wUserCount) UserID,MainOrder,MailID,ExpireTime INTO #UserMailInfo FROM RecordRankReward WHERE SendState=0

		-- ����״̬
		UPDATE RecordRankReward SET SendState=1 WHERE UserID IN (SELECT UserID FROM #UserMailInfo)

		-- �����ʼ�
		INSERT INTO LHAccountsDB.dbo.AccountsMailBox(UserID,MailID,MailState,ExpireTime,UpdateTime)
		SELECT UserID,MailID,1,ExpireTime,GetDate() FROM #UserMailInfo

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- �׳���¼
	SELECT UserID,MainOrder FROM #UserMailInfo

	-- ɾ����ʱ��
	IF OBJECT_ID('tempdb..#UserMailInfo') IS NOT NULL
	BEGIN
		DROP TABLE #UserMailInfo
	END

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
