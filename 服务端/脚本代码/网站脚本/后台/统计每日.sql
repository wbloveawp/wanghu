----------------------------------------------------------------------
-- ������ͳ���û�����
----------------------------------------------------------------------
USE [LHGameRecordDB]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WSP_Statistics]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[WSP_Statistics]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WSP_StatisticsGoodsStock]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[WSP_StatisticsGoodsStock]
GO

-------------------------------ÿ��ͳ��---------------------------------------
CREATE PROC [WSP_Statistics]
		@ReportDate		DATETIME			-- ����ʱ��
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON
SET XACT_ABORT ON

DECLARE @DateID INT
DECLARE @CurrentDate DATETIME
DECLARE @ReportTimestamp INT
DECLARE @CurrentTimestamp INT

BEGIN

	SET @CurrentDate=CONVERT(VARCHAR(100),DATEADD(DAY,1,@ReportDate),23)
	SET @ReportDate=CONVERT(VARCHAR(100),@ReportDate,23)
	SET @DateID=CAST(CAST(@ReportDate AS FLOAT) AS INT)
	SET @CurrentTimestamp=DATEDIFF(S,'1970-01-01 00:00:00',DATEADD(HOUR, DATEDIFF(hour,GETDATE(),GETUTCDATE()),@CurrentDate))
	SET @ReportTimestamp=DATEDIFF(S,'1970-01-01 00:00:00',DATEADD(HOUR, DATEDIFF(hour,GETDATE(),GETUTCDATE()),@ReportDate))

	IF EXISTS(SELECT * FROM ReportDay WHERE DateID=@DateID)
	BEGIN
		RETURN -1
	END

	BEGIN TRAN

	-- ÿ������

	-- �˺�����
	DECLARE @UserCount INT
	SELECT @UserCount=COUNT(UserID) FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE RegisterDate<@CurrentDate AND IsRobot=0
	IF @UserCount IS NULL
		SET @UserCount=0
			
	-- ��ֵ�û�����
	DECLARE @PayUserCount INT
	SELECT @PayUserCount=COUNT(A.UserID) FROM (SELECT UserID FROM LHInsureDB.dbo.PayRecord WHERE PayDateTime<@CurrentDate GROUP BY UserID) AS A
	IF @PayUserCount IS NULL
		SET @PayUserCount=0

	-- ��ֵ2���û�����
	DECLARE @PayTwiceUserCount INT
	SELECT @PayTwiceUserCount=COUNT(A.UserID) FROM (SELECT UserID FROM LHInsureDB.dbo.PayRecord WHERE PayDateTime<@CurrentDate GROUP BY UserID HAVING Count(UserID)>1) AS A
	IF @PayTwiceUserCount IS NULL
		SET @PayTwiceUserCount=0
			
	-- ����ע���û�
	DECLARE @TodayRegUserCount INT
	SELECT @TodayRegUserCount=COUNT(UserID) FROM LHAccountsDB.dbo.AccountsInfo WHERE RegisterDate>=@ReportDate AND RegisterDate<@CurrentDate AND IsRobot=0
	IF @TodayRegUserCount IS NULL
		SET @TodayRegUserCount=0

	-- ����ע���ֵ
	DECLARE @TodayRegPayUserCount INT
	SELECT @TodayRegPayUserCount=COUNT(A.UserID) FROM (SELECT UserID FROM LHInsureDB.dbo.PayRecord WHERE PayDateTime<@CurrentDate AND UserID IN (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE RegisterDate>=@ReportDate AND RegisterDate<@CurrentDate) GROUP BY UserID) AS A
	IF @TodayRegPayUserCount IS NULL
		SET @TodayRegPayUserCount=0

	-- ���յ�¼�û�
	DECLARE @TodayLoginUserCount INT
	SELECT @TodayLoginUserCount=COUNT(UserID) FROM (SELECT UserID FROM LHAccountsDB.dbo.RecordLogonInfo WHERE CollectDate>=@ReportDate AND CollectDate<@CurrentDate GROUP BY UserID) AS A
	IF @TodayLoginUserCount IS NULL
		SET @TodayLoginUserCount=0
			
	-- ���ճ�ֵ
	DECLARE @TodayPayUserCount INT
	SELECT @TodayPayUserCount=COUNT(A.UserID) FROM (SELECT UserID FROM LHInsureDB.dbo.PayRecord WHERE PayDateTime>=@ReportDate AND PayDateTime<@CurrentDate GROUP BY UserID) AS A
	IF @TodayPayUserCount IS NULL
		SET @TodayPayUserCount=0
			
	-- ���ճ�ֵ
	DECLARE @TodayPayMaxAmount DECIMAL(18,2)
	SELECT @TodayPayMaxAmount=MAX(PayAmount) FROM LHInsureDB.dbo.PayRecord WHERE PayDateTime>@ReportDate AND PayDateTime<@CurrentDate
	IF @TodayPayMaxAmount IS NULL
		SET @TodayPayMaxAmount=0

	-- ���պ��ѷ�����
	DECLARE @TodayBattleCreateRoomCount INT
	SELECT @TodayBattleCreateRoomCount=COUNT(*) FROM LHGameBattleDB.dbo.RecordBattleInfo WHERE DateID=@DateID

	-- ������ʱ��
	DECLARE @TodaySumOnlineTime BIGINT
	SELECT @TodaySumOnlineTime=SUM(OnlineTimeCount) FROM LHAccountsDB.dbo.SystemStreamLogon WHERE DateID=@DateID
	IF @TodaySumOnlineTime IS NULL
		SET @TodaySumOnlineTime=0

	-- �ܲ�����Ϸ�û���
	DECLARE @PlayCommonUserCount INT
	DECLARE @PlayVipUserCount INT
	SELECT @PlayCommonUserCount=COUNT(*) FROM (SELECT UserID FROM LHGameBattleDB.dbo.ReportUserScore WHERE DateID=@DateID AND UserID IN (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE MemberOverTime<@CurrentTimestamp)
		UNION SELECT UserID FROM LHGameGoldDB.dbo.ReportUserScore WHERE DateID=@DateID AND UserID IN (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE MemberOverTime<@CurrentTimestamp)
		UNION SELECT UserID FROM LHGameMatchDB.dbo.ReportUserScore WHERE DateID=@DateID AND UserID IN (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE MemberOverTime<@CurrentTimestamp)
        UNION SELECT UserID FROM LHGameRankingDB.dbo.ReportUserScore WHERE DateID=@DateID AND UserID IN (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE MemberOverTime<@CurrentTimestamp)) AS A

	SELECT @PlayVipUserCount=COUNT(*) FROM (SELECT UserID FROM LHGameBattleDB.dbo.ReportUserScore WHERE DateID=@DateID AND UserID IN (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE MemberOverTime>=@CurrentTimestamp)
		UNION SELECT UserID FROM LHGameGoldDB.dbo.ReportUserScore WHERE DateID=@DateID AND UserID IN (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE MemberOverTime>=@CurrentTimestamp)
		UNION SELECT UserID FROM LHGameMatchDB.dbo.ReportUserScore WHERE DateID=@DateID AND UserID IN (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE MemberOverTime>=@CurrentTimestamp)
        UNION SELECT UserID FROM LHGameRankingDB.dbo.ReportUserScore WHERE DateID=@DateID AND UserID IN (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE MemberOverTime>=@CurrentTimestamp)) AS A

	-- �������
	DECLARE @RegCount DECIMAL(18,4)
	DECLARE @RemainCount DECIMAL(18,4)
			
	-- 1������
	DECLARE @OneDayRemainRate DECIMAL(18,4)
	SELECT @RegCount=COUNT(UserID) FROM LHAccountsDB.dbo.AccountsInfo WHERE RegisterDate>=DATEADD(DAY,-1,@ReportDate) AND RegisterDate<@ReportDate AND IsRobot=0
	IF @RegCount=0
	BEGIN
		SET @OneDayRemainRate=0
	END
	ELSE
	BEGIN
		SELECT @RemainCount=COUNT(UserID) FROM (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE RegisterDate>=DATEADD(DAY,-1,@ReportDate) AND RegisterDate<@ReportDate AND IsRobot=0) AS A 
		WHERE A.UserID IN(SELECT UserID FROM LHAccountsDB.dbo.RecordLogonInfo WHERE DateID=@DateID)
		SET @OneDayRemainRate=@RemainCount/@RegCount
	END

	-- 2������
	SET @RegCount=0
	SET @RemainCount=0
	DECLARE @TwoDayRemainRate DECIMAL(18,4)
	SELECT @RegCount=COUNT(UserID) FROM LHAccountsDB.dbo.AccountsInfo WHERE RegisterDate>=DATEADD(DAY,-2,@ReportDate) AND RegisterDate<DATEADD(DAY,-1,@ReportDate) AND IsRobot=0
	IF @RegCount=0
	BEGIN
		SET @TwoDayRemainRate=0
	END
	ELSE
	BEGIN
		SELECT @RemainCount=COUNT(UserID) FROM (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE RegisterDate>=DATEADD(DAY,-2,@ReportDate) AND RegisterDate<DATEADD(DAY,-1,@ReportDate) AND IsRobot=0) AS A 
		WHERE A.UserID IN(SELECT UserID FROM LHAccountsDB.dbo.RecordLogonInfo WHERE DateID=@DateID)
		SET @TwoDayRemainRate=@RemainCount/@RegCount
	END

	-- 3������
	SET @RegCount=0
	SET @RemainCount=0
	DECLARE @ThreeDayRemainRate DECIMAL(18,4)
	SELECT @RegCount=COUNT(UserID) FROM LHAccountsDB.dbo.AccountsInfo WHERE RegisterDate>=DATEADD(DAY,-3,@ReportDate) AND RegisterDate<DATEADD(DAY,-2,@ReportDate) AND IsRobot=0
	IF @RegCount=0
	BEGIN
		SET @ThreeDayRemainRate=0
	END
	ELSE
	BEGIN
		SELECT @RemainCount=COUNT(UserID) FROM (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE RegisterDate>=DATEADD(DAY,-3,@ReportDate) AND RegisterDate<DATEADD(DAY,-2,@ReportDate) AND IsRobot=0) AS A 
		WHERE A.UserID IN(SELECT UserID FROM LHAccountsDB.dbo.RecordLogonInfo WHERE DateID=@DateID)
		SET @ThreeDayRemainRate=@RemainCount/@RegCount
	END

	-- 4������
	SET @RegCount=0
	SET @RemainCount=0
	DECLARE @FourDayRemainRate DECIMAL(18,4)
	SELECT @RegCount=COUNT(UserID) FROM LHAccountsDB.dbo.AccountsInfo WHERE RegisterDate>=DATEADD(DAY,-4,@ReportDate) AND RegisterDate<DATEADD(DAY,-3,@ReportDate) AND IsRobot=0
	IF @RegCount=0
	BEGIN
		SET @FourDayRemainRate=0
	END
	ELSE
	BEGIN
		SELECT @RemainCount=COUNT(UserID) FROM (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE RegisterDate>=DATEADD(DAY,-4,@ReportDate) AND RegisterDate<DATEADD(DAY,-3,@ReportDate) AND IsRobot=0) AS A 
		WHERE A.UserID IN(SELECT UserID FROM LHAccountsDB.dbo.RecordLogonInfo WHERE DateID=@DateID)
		SET @FourDayRemainRate=@RemainCount/@RegCount
	END

	-- 5������
	SET @RegCount=0
	SET @RemainCount=0
	DECLARE @FiveDayRemainRate DECIMAL(18,4)
	SELECT @RegCount=COUNT(UserID) FROM LHAccountsDB.dbo.AccountsInfo WHERE RegisterDate>=DATEADD(DAY,-5,@ReportDate) AND RegisterDate<DATEADD(DAY,-4,@ReportDate) AND IsRobot=0
	IF @RegCount=0
	BEGIN
		SET @FiveDayRemainRate=0
	END
	ELSE
	BEGIN
		SELECT @RemainCount=COUNT(UserID) FROM (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE RegisterDate>=DATEADD(DAY,-5,@ReportDate) AND RegisterDate<DATEADD(DAY,-4,@ReportDate) AND IsRobot=0) AS A 
		WHERE A.UserID IN(SELECT UserID FROM LHAccountsDB.dbo.RecordLogonInfo WHERE DateID=@DateID)
		SET @FiveDayRemainRate=@RemainCount/@RegCount
	END

	-- 7������
	SET @RegCount=0
	SET @RemainCount=0
	DECLARE @ServenDayRemainRate DECIMAL(18,4)
	SELECT @RegCount=COUNT(UserID) FROM LHAccountsDB.dbo.AccountsInfo WHERE RegisterDate>=DATEADD(DAY,-7,@ReportDate) AND RegisterDate<DATEADD(DAY,-6,@ReportDate) AND IsRobot=0
	IF @RegCount=0
	BEGIN
		SET @ServenDayRemainRate=0
	END
	ELSE
	BEGIN
		SELECT @RemainCount=COUNT(UserID) FROM (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE RegisterDate>=DATEADD(DAY,-7,@ReportDate) AND RegisterDate<DATEADD(DAY,-6,@ReportDate) AND IsRobot=0) AS A 
		WHERE A.UserID IN(SELECT UserID FROM LHAccountsDB.dbo.RecordLogonInfo WHERE DateID=@DateID)
		SET @ServenDayRemainRate=@RemainCount/@RegCount
	END

	-- 15������
	SET @RegCount=0
	SET @RemainCount=0
	DECLARE @FiftenDayRemainRate DECIMAL(18,4)
	SELECT @RegCount=COUNT(UserID) FROM LHAccountsDB.dbo.AccountsInfo WHERE RegisterDate>=DATEADD(DAY,-15,@ReportDate) AND RegisterDate<DATEADD(DAY,-14,@ReportDate) AND IsRobot=0
	IF @RegCount=0
	BEGIN
		SET @FiftenDayRemainRate=0
	END
	ELSE
	BEGIN
		SELECT @RemainCount=COUNT(UserID) FROM (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE RegisterDate>=DATEADD(DAY,-15,@ReportDate) AND RegisterDate<DATEADD(DAY,-14,@ReportDate) AND IsRobot=0) AS A 
		WHERE A.UserID IN(SELECT UserID FROM LHAccountsDB.dbo.RecordLogonInfo WHERE DateID=@DateID)
		SET @FiftenDayRemainRate=@RemainCount/@RegCount
	END

	-- 30������
	SET @RegCount=0
	SET @RemainCount=0
	DECLARE @ThirtyDayRemainRate DECIMAL(18,4)
	SELECT @RegCount=COUNT(UserID) FROM LHAccountsDB.dbo.AccountsInfo WHERE RegisterDate>=DATEADD(DAY,-30,@ReportDate) AND RegisterDate<DATEADD(DAY,-29,@ReportDate) AND IsRobot=0
	IF @RegCount=0
	BEGIN
		SET @ThirtyDayRemainRate=0
	END
	ELSE
	BEGIN
		SELECT @RemainCount=COUNT(UserID) FROM (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE RegisterDate>=DATEADD(DAY,-30,@ReportDate) AND RegisterDate<DATEADD(DAY,-29,@ReportDate) AND IsRobot=0) AS A 
		WHERE A.UserID IN(SELECT UserID FROM LHAccountsDB.dbo.RecordLogonInfo WHERE DateID=@DateID)
		SET @ThirtyDayRemainRate=@RemainCount/@RegCount
	END

	DECLARE @TotalPlayers INT
	SELECT @TotalPlayers=COUNT(UserID) FROM LHAccountsDB.dbo.AccountsInfo WHERE RegisterDate<@CurrentDate AND PlayTimeCount>0 AND IsRobot=0
						
	-- ���ձ���
	INSERT INTO ReportDay(DateID,UserCount,PayUserCount,TodayLoginUserCount,TodayRegUserCount,TodayRegPayUserCount,TodayPayUserCount,TodayDayActiveUser,PayTwiceUserCount,TodayPayMaxAmount,TodayPlayCommonUserCount,TodayPlayVipUserCount,TodayBattleCreateRoomCount,TodaySumOnlineTime,ReportDateTime,OneDayRemainRate,TwoDayRemainRate,ThreeDayRemainRate,FourDayRemainRate,FiveDayRemainRate,ServenDayRemainRate,FiftenDayRemainRate,ThirtyDayRemainRate,TotalPlayers)
	VALUES(@DateID,@UserCount,@PayUserCount,@TodayLoginUserCount,@TodayRegUserCount,@TodayRegPayUserCount,@TodayPayUserCount,0,@PayTwiceUserCount,@TodayPayMaxAmount,@PlayCommonUserCount,@PlayVipUserCount,@TodayBattleCreateRoomCount,@TodaySumOnlineTime,@ReportDate,@OneDayRemainRate,@TwoDayRemainRate,@ThreeDayRemainRate,@FourDayRemainRate,@FiveDayRemainRate,@ServenDayRemainRate,@FiftenDayRemainRate,@ThirtyDayRemainRate,@TotalPlayers)

	-- ��Ա�ֲ�
	DECLARE @I INT
	DECLARE @VipUserCount INT
	DECLARE @NeedPoint INT
	DECLARE @SpanPoint INT
	SET @VipUserCount=0
	SET @I=1
	WHILE @I<=9
	BEGIN
		-- ��ѯ��Ա����ֵ
		SELECT @NeedPoint=NeedPoint,@SpanPoint=SpanPoint FROM LHPlatformDB.dbo.GameUserLevel WHERE LevelID=@I
		IF @NeedPoint IS NOT NULL
		BEGIN
			SELECT @VipUserCount=COUNT(UserID) FROM LHAccountsDB.dbo.AccountsInfo WHERE MemberPoint>=@NeedPoint AND MemberPoint<@NeedPoint+@SpanPoint AND IsRobot=0
			INSERT INTO ReportVip(DateID,VipOrder,UserCount) VALUES(@DateID,@I,@VipUserCount)
		END
		SET @I=@I+1
	END

	-- �ն˷ֲ�
	SELECT ROW_NUMBER() OVER(ORDER BY LogonDeviceKind ASC) AS ID,* INTO #tmpTable FROM (SELECT LogonDeviceKind&0xFF AS LogonDeviceKind,Count(UserID) AS TerminalCount FROM LHAccountsDB.dbo.AccountsInfo WHERE RegisterDate<@CurrentDate AND IsRobot=0 GROUP BY LogonDeviceKind&0xFF) AS A
	SET @I=1
	DECLARE @Counts INT
	SELECT @Counts=COUNT(LogonDeviceKind) FROM #tmpTable
	WHILE @I<=@Counts
	BEGIN
		INSERT INTO ReportTerminal(DateID,TerminalID,UserCount) VALUES(@DateID,@I,@VipUserCount)
		SET @I=@I+1
	END

	-- �����Ϸ
	DECLARE @ZoneID INT
	DECLARE @GoldPlayCommonUserCount INT
	DECLARE @GoldPlayVipUserCount INT
	DECLARE @GoldGameNumber INT
	DECLARE @GoldUserRevenue BIGINT
	DECLARE @GoldUserService BIGINT
	DECLARE @GoldPlayTimeCount INT
	SET @ZoneID=1

	SELECT @GoldPlayCommonUserCount=COUNT(*) FROM LHGameGoldDB.dbo.ReportUserScore WHERE DateID=@DateID AND UserID IN (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE MemberOverTime<@CurrentTimestamp AND IsRobot=0) 
	SELECT @GoldPlayVIPUserCount=COUNT(*) FROM LHGameGoldDB.dbo.ReportUserScore WHERE DateID=@DateID AND UserID IN (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE MemberOverTime>=@CurrentTimestamp AND IsRobot=0)
	SELECT @GoldGameNumber=COUNT(*) FROM LHGameGoldDB.dbo.RecordDrawInfo WHERE RecordDate>=@ReportDate AND RecordDate<@CurrentDate
	SELECT @GoldUserRevenue=ISNULL(SUM(UserRevenue),0),@GoldUserService=ISNULL(SUM(UserService),0),@GoldPlayTimeCount=ISNULL(SUM(PlayTimeCount),0) FROM LHGameGoldDB.dbo.ReportUserScore WHERE DateID=@DateID

	INSERT INTO ReportGameData(DateID,ZoneID,PlayCommonUserCount,PlayVipUserCount,GameNumber,UserRevenue,UserService,PlayTimeCount)
	VALUES(@DateID,@ZoneID,@GoldPlayCommonUserCount,@GoldPlayVIPUserCount,@GoldGameNumber,@GoldUserRevenue,@GoldUserService,@GoldPlayTimeCount)

	-- ��λ��Ϸ
	DECLARE @RankingPlayCommonUserCount INT
	DECLARE @RankingPlayVipUserCount INT
	DECLARE @RankingGameNumber INT
	DECLARE @RankingUserRevenue BIGINT
	DECLARE @RankingUserService BIGINT
	DECLARE @RankingPlayTimeCount INT
	SET @ZoneID=2

	SELECT @RankingPlayCommonUserCount=COUNT(*) FROM LHGameRankingDB.dbo.ReportUserScore WHERE DateID=@DateID AND UserID IN (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE MemberOverTime<@CurrentTimestamp AND IsRobot=0)
	SELECT @RankingPlayVIPUserCount=COUNT(*) FROM LHGameRankingDB.dbo.ReportUserScore WHERE DateID=@DateID AND UserID IN (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE MemberOverTime>=@CurrentTimestamp AND IsRobot=0)
	SELECT @RankingGameNumber=COUNT(*) FROM LHGameRankingDB.dbo.RecordDrawInfo WHERE RecordDate>=@ReportDate AND RecordDate<@CurrentDate
	SELECT @RankingUserRevenue=ISNULL(SUM(UserRevenue),0),@RankingUserService=ISNULL(SUM(UserService),0),@RankingPlayTimeCount=ISNULL(SUM(PlayTimeCount),0) FROM LHGameRankingDB.dbo.ReportUserScore WHERE DateID=@DateID

	INSERT INTO ReportGameData(DateID,ZoneID,PlayCommonUserCount,PlayVipUserCount,GameNumber,UserRevenue,UserService,PlayTimeCount)
	VALUES(@DateID,@ZoneID,@RankingPlayCommonUserCount,@RankingPlayVIPUserCount,@RankingGameNumber,@RankingUserRevenue,@RankingUserService,@RankingPlayTimeCount)

	-- Լս��Ϸ
	DECLARE @BattlePlayCommonUserCount INT
	DECLARE @BattlePlayVipUserCount INT
	DECLARE @BattleGameNumber INT
	DECLARE @BattleUserRevenue BIGINT
	DECLARE @BattleUserService BIGINT
	DECLARE @BattlePlayTimeCount INT
	SET @ZoneID=3

	SELECT @BattlePlayCommonUserCount=COUNT(*) FROM LHGameBattleDB.dbo.ReportUserScore WHERE DateID=@DateID AND UserID IN (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE MemberOverTime<@CurrentTimestamp AND IsRobot=0)
	SELECT @BattlePlayVIPUserCount=COUNT(*) FROM LHGameBattleDB.dbo.ReportUserScore WHERE DateID=@DateID AND UserID IN (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE MemberOverTime>=@CurrentTimestamp AND IsRobot=0)
	SELECT @BattleGameNumber=COUNT(*) FROM LHGameBattleDB.dbo.RecordDrawInfo WHERE RecordDate>=@ReportDate AND RecordDate<@CurrentDate
	SELECT @BattleUserRevenue=ISNULL(SUM(UserRevenue),0),@BattleUserService=ISNULL(SUM(UserService),0),@BattlePlayTimeCount=ISNULL(SUM(PlayTimeCount),0) FROM LHGameBattleDB.dbo.ReportUserScore WHERE DateID=@DateID

	INSERT INTO ReportGameData(DateID,ZoneID,PlayCommonUserCount,PlayVipUserCount,GameNumber,UserRevenue,UserService,PlayTimeCount)
	VALUES(@DateID,@ZoneID,@BattlePlayCommonUserCount,@BattlePlayVIPUserCount,@BattleGameNumber,@BattleUserRevenue,@BattleUserService,@BattlePlayTimeCount)

	-- ��������
	DECLARE @MatchPlayCommonUserCount INT
	DECLARE @MatchPlayVipUserCount INT
	DECLARE @MatchGameNumber INT
	DECLARE @MatchUserRevenue BIGINT
	DECLARE @MatchUserService BIGINT
	DECLARE @MatchPlayTimeCount INT
	SET @ZoneID=4

	SELECT @MatchPlayCommonUserCount=COUNT(*) FROM LHGameMatchDB.dbo.ReportUserScore WHERE DateID=@DateID AND UserID IN (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE MemberOverTime<@CurrentTimestamp AND IsRobot=0)
	SELECT @MatchPlayVIPUserCount=COUNT(*) FROM LHGameMatchDB.dbo.ReportUserScore WHERE DateID=@DateID AND UserID IN (SELECT UserID FROM LHAccountsDB.dbo.AccountsInfo WHERE MemberOverTime>=@CurrentTimestamp AND IsRobot=0)
	SELECT @MatchGameNumber=COUNT(*) FROM LHGameMatchDB.dbo.RecordDrawInfo WHERE RecordDate>=@ReportDate AND RecordDate<@CurrentDate
	SELECT @MatchUserRevenue=ISNULL(SUM(UserRevenue),0),@MatchUserService=ISNULL(SUM(UserService),0),@MatchPlayTimeCount=ISNULL(SUM(PlayTimeCount),0) FROM LHGameMatchDB.dbo.ReportUserScore WHERE DateID=@DateID

	INSERT INTO ReportGameData(DateID,ZoneID,PlayCommonUserCount,PlayVipUserCount,GameNumber,UserRevenue,UserService,PlayTimeCount)
	VALUES(@DateID,@ZoneID,@MatchPlayCommonUserCount,@MatchPlayVIPUserCount,@MatchGameNumber,@MatchUserRevenue,@MatchUserService,@MatchPlayTimeCount)
	
	-- ��������
	CREATE TABLE #tmpTableGoods(OpOrCn TINYINT,GoodsID INT,TypeID INT,GoodsCount BIGINT)
	DECLARE @GoodsID INT

	-- ��ʯ����
	SET @GoodsID=1
	/*
	ע�����Ͳ���  1
	ϵͳ�����ʼ����� 84
	��̨���Ͳ��� 100
	�û���ֵ���� 102

	�һ���Ʒ���� 7
	�������ȯ���� 8
	*/
	DECLARE @BeanOpType NVARCHAR(512)
	SET @BeanOpType='1,13,84,100,101,102'
	DECLARE @BeanCnType NVARCHAR(512)
	SET @BeanCnType='7,8'
	INSERT INTO #tmpTableGoods SELECT 0,@GoodsID,TypeID,GoodsCount=SUM(ChangeIngot) FROM RecordChangeIngot WHERE ChangeIngot>0 AND CollectDateTime>=@ReportDate AND CollectDateTime<@CurrentDate AND TypeID IN(SELECT rs FROM WF_SplitStr(@BeanOpType,',')) GROUP BY TypeID
	INSERT INTO #tmpTableGoods SELECT 1,@GoodsID,TypeID,GoodsCount=0-SUM(ChangeIngot) FROM RecordChangeIngot WHERE ChangeIngot<0 AND CollectDateTime>=@ReportDate AND CollectDateTime<@CurrentDate AND TypeID IN(SELECT rs FROM WF_SplitStr(@BeanCnType,',')) GROUP BY TypeID

	-- ��Ҳ���
	SET @GoodsID=2
	/*
	ע�����Ͳ���  1
	��Ϸ������������ 2
	VIP������������ 3
	�һ���Ʒ���� 7
	�һ������ 13
	��ȡ�ͱ� 15

	�ƹ��ʼ����� 81 
	�����ʼ����� 82
	��λ�ʼ����� 83 
	ϵͳ�����ʼ����� 84
	��̨���Ͳ��� 100
	�û�ǩ������ 101
	�û���ֵ���� 102
	������Ӯȡ

	���ѷ��볡�� 14
	�������� 16
	�޸��ǳ� 17
	�޸�ͷ�� 18
	��ͨ��˰������ 150
	Լս����������˰������ 151
	��λ����� 152
	��������� 1000

	*/

	-- ��������Ӯ
	DECLARE @RobotWinOrLose BIGINT
	SELECT @RobotWinOrLose=ISNULL(SUM(WasteCount),0) FROM LHGameGoldDB.dbo.RecordDrawInfo WHERE RecordDate>=@ReportDate AND RecordDate<@CurrentDate
	IF @RobotWinOrLose>0
	BEGIN
		INSERT INTO #tmpTableGoods VALUES(1,@GoodsID,1000,@RobotWinOrLose)
	END
	ELSE IF @RobotWinOrLose<0
	BEGIN
		INSERT INTO #tmpTableGoods VALUES(0,@GoodsID,1000,0-@RobotWinOrLose)
	END

	DECLARE @GoldOpType NVARCHAR(MAX)
	SET @GoldOpType='1,2,3,7,13,15,16,17,18,81,82,83,84,100,101,102'
	INSERT INTO #tmpTableGoods SELECT 0,@GoodsID,TypeID,GoodsCount=SUM(ChangeGold) FROM RecordChangeGold WHERE ChangeGold>0 AND CollectDateTime>=@ReportDate AND CollectDateTime<@CurrentDate AND TypeID IN (SELECT rs FROM WF_SplitStr(@GoldOpType,',')) GROUP BY TypeID

	DECLARE @GoldCnType NVARCHAR(MAX)
	SET @GoldCnType='14,16,17,18'
	INSERT INTO #tmpTableGoods SELECT 1,@GoodsID,TypeID,GoodsCount=0-SUM(ChangeGold) FROM RecordChangeGold WHERE ChangeGold<0 AND CollectDateTime>=@ReportDate AND CollectDateTime<@CurrentDate AND TypeID IN (SELECT rs FROM WF_SplitStr(@GoldCnType,',')) GROUP BY TypeID

	INSERT INTO #tmpTableGoods VALUES(1,@GoodsID,150,@GoldUserRevenue+@GoldUserService)
	INSERT INTO #tmpTableGoods VALUES(1,@GoodsID,151,@BattleUserRevenue+@BattleUserService)
	INSERT INTO #tmpTableGoods VALUES(1,@GoodsID,152,@RankingUserRevenue+@RankingUserService)

	-- ��Ʒ����
	--INSERT INTO #tmpTableGoods SELECT 0,GoodsID,TypeID,GoodsCount=SUM(ChangeCount) FROM RecordChangeGoods WHERE ChangeCount>0 AND CollectDateTime>=@ReportDate AND CollectDateTime<@CurrentDate GROUP BY GoodsID,TypeID
	--INSERT INTO #tmpTableGoods SELECT 1,GoodsID,TypeID,GoodsCount=0-SUM(ChangeCount) FROM RecordChangeGoods WHERE ChangeCount<0 AND CollectDateTime>=@ReportDate AND CollectDateTime<@CurrentDate GROUP BY GoodsID,TypeID

	-- �������
	DECLARE @OpOrCn TINYINT
	DECLARE @OcGoodsID INT
	DECLARE @OcTypeID INT
	DECLARE @OcGoodsCount BIGINT
	DECLARE c_tempTableGoods CURSOR FAST_FORWARD FOR SELECT OpOrCn,GoodsID,TypeID,GoodsCount FROM #tmpTableGoods
	OPEN c_tempTableGoods
	WHILE 1=1
	BEGIN
		FETCH NEXT FROM c_tempTableGoods INTO @OpOrCn,@OcGoodsID,@OcTypeID,@OcGoodsCount
		IF @@FETCH_STATUS!=0
			BREAK
		IF @OpOrCn=0
		BEGIN
			UPDATE ReportGoodsOpCn SET OutputCount=OutputCount+@OcGoodsCount WHERE DateID=@DateID AND GoodsID=@OcGoodsID AND ChangeTypeID=@OcTypeID
			IF @@ROWCOUNT=0
			BEGIN
				INSERT INTO ReportGoodsOpCn(DateID,GoodsID,ChangeTypeID,OutputCount,ConsumeCount,CollectDateTime) VALUES(@DateID,@OcGoodsID,@OcTypeID,@OcGoodsCount,0,GETDATE())
			END
		END
		ELSE
		BEGIN
			UPDATE ReportGoodsOpCn SET ConsumeCount=ConsumeCount+@OcGoodsCount WHERE DateID=@DateID AND GoodsID=@OcGoodsID AND ChangeTypeID=@OcTypeID
			IF @@ROWCOUNT=0
			BEGIN
				INSERT INTO ReportGoodsOpCn(DateID,GoodsID,ChangeTypeID,OutputCount,ConsumeCount,CollectDateTime) VALUES(@DateID,@OcGoodsID,@OcTypeID,0,@OcGoodsCount,GETDATE())
			END
		END
	END
	CLOSE c_tempTableGoods
	DEALLOCATE c_tempTableGoods

	DROP TABLE #tmpTableGoods
	COMMIT TRAN
END
RETURN 0
GO


-------------------------------ͳ����Ʒ������2022.3.18 ���ͳ�ƽ�ҷ����棩 ÿ��0��ִ�� ---------------------------------------
CREATE PROC [WSP_StatisticsGoodsStock]
	 
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON
SET XACT_ABORT ON

BEGIN
	
	DECLARE @CurrentDate DATETIME
	DECLARE @DateID INT
	SET @CurrentDate=GETDATE()
	SET @DateID=CAST(CAST(@CurrentDate AS FLOAT) AS INT)

	IF EXISTS(SELECT * FROM LHGameRecordDB.dbo.ReportGoods WHERE DateID=@DateID)
	BEGIN
		RETURN -1
	END

	BEGIN TRAN

	-- ��Ҵ���
	DECLARE @GoldCount BIGINT
	SELECT @GoldCount=SUM(Score) FROM LHAccountsDB.dbo.AccountsGoldInfo(NOLOCK)
	IF @GoldCount IS NULL
		SET @GoldCount=0
	INSERT INTO LHGameRecordDB.dbo.ReportGoods VALUES(@DateID-1,2,@GoldCount,GETDATE())
	
	-- ��ʯ����
	DECLARE @BeanCount BIGINT
	SELECT @BeanCount=SUM(Score) FROM LHAccountsDB.dbo.AccountsIngotInfo(NOLOCK)
	IF @BeanCount IS NULL
		SET @BeanCount=0
	INSERT INTO LHGameRecordDB.dbo.ReportGoods VALUES(@DateID-1,1,@BeanCount,GETDATE())

	-- ��ҷ�����
	-- ������ֵ
	DECLARE c_Stock CURSOR FAST_FORWARD FOR SELECT StockID,CurrentScore FROM LHPlatformDB.dbo.GameStockInfo
	OPEN c_Stock
	DECLARE @CurrentStockID INT
	DECLARE @CurrentScore BIGINT
	WHILE 1=1
	BEGIN
		FETCH NEXT FROM c_Stock INTO @CurrentStockID,@CurrentScore
		IF @@fetch_status!=0
            BREAK;

		DECLARE @StartStock BIGINT
		DECLARE @ChangStock BIGINT
		SELECT @StartStock=StockStart FROM ReportStock WHERE StockID=@CurrentStockID AND DateID=@DateID-1
		IF @StartStock IS NOT NULL
		BEGIN
			DECLARE @TodayModifyStock BIGINT
			SELECT @TodayModifyStock=ISNULL(SUM(ChangeStock),0) FROM RecordStockModify WHERE StockID=@CurrentStockID AND CAST(CAST(CollectDate AS FLOAT) AS INT)=@DateID-1
			SET @ChangStock=@CurrentScore-(@StartStock+@TodayModifyStock)
		END

		UPDATE ReportStock SET StockChange=@ChangStock WHERE StockID=@CurrentStockID AND DateID=@DateID-1

		INSERT INTO ReportStock(DateID,StockID,StockStart,CollectDateTime) VALUES(@DateID,@CurrentStockID,@CurrentScore,@CurrentDate)
	END
	CLOSE c_Stock
	DEALLOCATE c_Stock;

	
	COMMIT TRAN
END
RETURN 0
GO