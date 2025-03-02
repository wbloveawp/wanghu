
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryPayRankingList]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryPayRankingList]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryWealthRankingList]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryWealthRankingList]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QuerySpreadRankingList]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QuerySpreadRankingList]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryBigWinerRankingList]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryBigWinerRankingList]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryCommissionRankingList]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryCommissionRankingList]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

--��ֵ��
CREATE  PROCEDURE dbo.GSP_GP_QueryPayRankingList
	@cbListType TINYINT
WITH ENCRYPTION AS

--��������
SET NOCOUNT ON
BEGIN

	-- ʱ�䶨��
	DECLARE @DateID INT
	DECLARE @WeekStartID INT
	DECLARE @MonthStartID INT

	-- ����ʱ��
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	SET @WeekStartID=CAST(CAST(LHAccountsDB.dbo.LHF_GetWeekFirstDay(GETDATE()) AS FLOAT) AS INT)
	SET @MonthStartID=CAST(CAST(LHAccountsDB.dbo.LHF_GetMonthFirstDay(GETDATE()) AS FLOAT) AS INT)

	-- ��ѯ����
	DECLARE @RankingList NVARCHAR(MAX)	
	DECLARE @UserList_Week NVARCHAR(MAX)
	DECLARE @UserList_Month NVARCHAR(MAX)
	DECLARE @UserList_Total NVARCHAR(MAX)

	-- �ܰ�
	SET @UserList_Total = (SELECT TOP 20 ROW_NUMBER() OVER(ORDER BY PayAmount DESC, PayCount DESC, UserID) AS RankID, PayAmount AS RankValue, UserID  FROM
						   LHInsureDB.dbo.AccountsPay ORDER BY PayAmount DESC FOR JSON AUTO) 

	-- �°�
	SET @UserList_Month = (SELECT TOP 20 ROW_NUMBER() OVER(ORDER BY PayAmount DESC, PayCount DESC, UserID) AS RankID, PayAmount AS RankValue, UserID FROM  
						  (SELECT SUM(PayAmount) AS PayAmount, SUM(PayCount) AS PayCount, UserID FROM  LHGameRecordDB.dbo.ReportDayUser WHERE DateID>=@MonthStartID AND PayAmount>0 GROUP BY UserID) a
						   ORDER BY a.PayAmount DESC FOR JSON AUTO)

	-- �ܰ�
	SET @UserList_Week =  (SELECT TOP 20 ROW_NUMBER() OVER(ORDER BY PayAmount DESC, PayCount DESC, UserID) AS RankID, PayAmount AS RankValue, UserID FROM  
						  (SELECT SUM(PayAmount) AS PayAmount, SUM(PayCount) AS PayCount, UserID FROM  LHGameRecordDB.dbo.ReportDayUser WHERE DateID>=@WeekStartID AND PayAmount>0 GROUP BY UserID) a
						  ORDER BY a.PayAmount DESC FOR JSON AUTO)	

	-- �����б�
	IF @UserList_Week IS NULL SET @UserList_Week=N'[]'
	IF @UserList_Month IS NULL SET @UserList_Month=N'[]'
	IF @UserList_Total IS NULL SET @UserList_Total=N'[]'
	
	-- ����JSON
	SET @RankingList=N'{}'	
	SET @RankingList=JSON_MODIFY(@RankingList,N'$.ListType',@cbListType)	
	SET @RankingList=JSON_MODIFY(@RankingList,N'$.UserList_Week',JSON_QUERY(@UserList_Week,N'$'))
	SET @RankingList=JSON_MODIFY(@RankingList,N'$.UserList_Month',JSON_QUERY(@UserList_Month,N'$'))
	SET @RankingList=JSON_MODIFY(@RankingList,N'$.UserList_Total',JSON_QUERY(@UserList_Total,N'$'))

	-- �׳�JSON
	SELECT @RankingList AS RankingList

END
RETURN 0
GO

----------------------------------------------------------------------------------------------------

--�Ƹ���
CREATE  PROCEDURE dbo.GSP_GP_QueryWealthRankingList	
	@cbListType TINYINT
WITH ENCRYPTION AS

--��������
SET NOCOUNT ON
BEGIN

	-- ��ѯ����
	DECLARE @RankingList NVARCHAR(MAX)	
	DECLARE @UserList_Total NVARCHAR(MAX)	

	-- ��ѯ�б�
	SET @UserList_Total = (SELECT TOP 20 ROW_NUMBER() OVER(ORDER BY a.Score DESC, a.UserID) AS RankID, a.Score AS RankValue, a.UserID  FROM 
						   LHGameGoldDB.dbo.GameScoreInfo a, LHAccountsDB.dbo.AccountsInfo b WHERE a.UserID = b.UserID AND b.Nullity=0 ORDER BY a.Score DESC FOR JSON AUTO) 

	-- �����б�
	IF @UserList_Total IS NULL SET @UserList_Total=N'[]'
	
	-- ����JSON
	SET @RankingList=N'{}'
	SET @RankingList=JSON_MODIFY(@RankingList,N'$.ListType',@cbListType)
	SET @RankingList=JSON_MODIFY(@RankingList,N'$.UserList_Total',JSON_QUERY(@UserList_Total,N'$'))

	-- �׳�JSON
	SELECT @RankingList AS RankingList

END
RETURN 0
GO

----------------------------------------------------------------------------------------------------

--��Ӯ�Ұ�
CREATE  PROCEDURE dbo.GSP_GP_QueryBigWinerRankingList	
	@cbListType TINYINT
WITH ENCRYPTION AS

--��������
SET NOCOUNT ON
BEGIN
	
	-- ʱ�䶨��
	DECLARE @DateID INT
	DECLARE @WeekStartID INT
	DECLARE @MonthStartID INT

	-- ����ʱ��
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	SET @WeekStartID=CAST(CAST(LHAccountsDB.dbo.LHF_GetWeekFirstDay(GETDATE()) AS FLOAT) AS INT)
	SET @MonthStartID=CAST(CAST(LHAccountsDB.dbo.LHF_GetMonthFirstDay(GETDATE()) AS FLOAT) AS INT)

	-- ��ѯ����
	DECLARE @RankingList NVARCHAR(MAX)	
	DECLARE @UserList_Week NVARCHAR(MAX)
	DECLARE @UserList_Month NVARCHAR(MAX)
	DECLARE @UserList_Total NVARCHAR(MAX)

	-- �ܰ�
	SET @UserList_Total = (SELECT TOP 20 ROW_NUMBER() OVER(ORDER BY a.WinScore DESC, a.UserID) AS RankID, a.WinScore AS RankValue, a.UserID  
						   FROM LHGameGoldDB.dbo.GameScoreInfo a, LHAccountsDB.dbo.AccountsInfo b WHERE a.UserID = b.UserID AND b.Nullity=0 ORDER BY a.WinScore DESC FOR JSON AUTO) 

	-- �°�
	SET @UserList_Month = (SELECT TOP 20 ROW_NUMBER() OVER(ORDER BY GainScore DESC, WinCount DESC, UserID) AS RankID, GainScore AS RankValue, UserID FROM  
						  (SELECT SUM(GainScore) AS GainScore, SUM(WinCount) AS WinCount, UserID FROM  LHGameGoldDB.dbo.ReportUserScore WHERE DateID>=@MonthStartID GROUP BY UserID) a
						   ORDER BY a.GainScore DESC FOR JSON AUTO)

	-- �ܰ�
	SET @UserList_Week =  (SELECT TOP 20 ROW_NUMBER() OVER(ORDER BY GainScore DESC, WinCount DESC, UserID) AS RankID, GainScore AS RankValue, UserID FROM  
						  (SELECT SUM(GainScore) AS GainScore, SUM(WinCount) AS WinCount, UserID FROM  LHGameGoldDB.dbo.ReportUserScore WHERE DateID>=@WeekStartID GROUP BY UserID) a
						   ORDER BY a.GainScore DESC FOR JSON AUTO)
	
	-- �����б�
	IF @UserList_Week IS NULL SET @UserList_Week=N'[]'
	IF @UserList_Month IS NULL SET @UserList_Month=N'[]'
	IF @UserList_Total IS NULL SET @UserList_Total=N'[]'
	
	-- ����JSON
	SET @RankingList=N'{}'	
	SET @RankingList=JSON_MODIFY(@RankingList,N'$.ListType',@cbListType)	
	SET @RankingList=JSON_MODIFY(@RankingList,N'$.UserList_Week',JSON_QUERY(@UserList_Week,N'$'))
	SET @RankingList=JSON_MODIFY(@RankingList,N'$.UserList_Month',JSON_QUERY(@UserList_Month,N'$'))
	SET @RankingList=JSON_MODIFY(@RankingList,N'$.UserList_Total',JSON_QUERY(@UserList_Total,N'$'))

	-- �׳�JSON
	SELECT @RankingList AS RankingList

END
RETURN 0
GO

----------------------------------------------------------------------------------------------------

--�Ƽ���
CREATE  PROCEDURE dbo.GSP_GP_QuerySpreadRankingList	
	@cbListType TINYINT
WITH ENCRYPTION AS

--��������
SET NOCOUNT ON
BEGIN

	-- ʱ�䶨��
	DECLARE @DateID INT
	DECLARE @WeekStartID INT
	DECLARE @MonthStartID INT

	-- ����ʱ��
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	SET @WeekStartID=CAST(CAST(LHAccountsDB.dbo.LHF_GetWeekFirstDay(GETDATE()) AS FLOAT) AS INT)
	SET @MonthStartID=CAST(CAST(LHAccountsDB.dbo.LHF_GetMonthFirstDay(GETDATE()) AS FLOAT) AS INT)

	-- ��ѯ����
	DECLARE @RankingList NVARCHAR(MAX)	
	DECLARE @UserList_Week NVARCHAR(MAX)
	DECLARE @UserList_Month NVARCHAR(MAX)
	DECLARE @UserList_Total NVARCHAR(MAX)

	-- ��ѯ�б�
	SET @UserList_Total = (SELECT TOP 20 ROW_NUMBER() OVER(ORDER BY SpreadLowerCount DESC, UserID) AS RankID, SpreadLowerCount AS RankValue, UserID  FROM LHAccountsDB.dbo.AccountsSpreadStat ORDER BY SpreadLowerCount DESC FOR JSON AUTO) 
	
	-- �°�
	SET @UserList_Month = (SELECT TOP 20 ROW_NUMBER() OVER(ORDER BY SpreadLowerCount DESC, UserID) AS RankID, SpreadLowerCount AS RankValue, UserID FROM  
						  (SELECT SUM(SpreadLowerCount) AS SpreadLowerCount, UserID FROM  LHGameRecordDB.dbo.ReportDayUser WHERE DateID>=@MonthStartID AND SpreadLowerCount>0 GROUP BY UserID) a
						   ORDER BY a.SpreadLowerCount DESC FOR JSON AUTO)

	-- �ܰ�
	SET @UserList_Week =  (SELECT TOP 20 ROW_NUMBER() OVER(ORDER BY SpreadLowerCount DESC, UserID) AS RankID, SpreadLowerCount AS RankValue, UserID FROM  
						  (SELECT SUM(SpreadLowerCount) AS SpreadLowerCount, UserID FROM  LHGameRecordDB.dbo.ReportDayUser WHERE DateID>=@WeekStartID AND SpreadLowerCount>0  GROUP BY UserID) a
						  ORDER BY a.SpreadLowerCount DESC FOR JSON AUTO)	

	-- �����б�
	IF @UserList_Week IS NULL SET @UserList_Week=N'[]'
	IF @UserList_Month IS NULL SET @UserList_Month=N'[]'
	IF @UserList_Total IS NULL SET @UserList_Total=N'[]'

	-- ����JSON
	SET @RankingList=N'{}'	
	SET @RankingList=JSON_MODIFY(@RankingList,N'$.ListType',@cbListType)	
	SET @RankingList=JSON_MODIFY(@RankingList,N'$.UserList_Week',JSON_QUERY(@UserList_Week,N'$'))
	SET @RankingList=JSON_MODIFY(@RankingList,N'$.UserList_Month',JSON_QUERY(@UserList_Month,N'$'))
	SET @RankingList=JSON_MODIFY(@RankingList,N'$.UserList_Total',JSON_QUERY(@UserList_Total,N'$'))

	-- �׳�JSON
	SELECT @RankingList AS RankingList

END
RETURN 0
GO

----------------------------------------------------------------------------------------------------


--Ӷ���
CREATE  PROCEDURE dbo.GSP_GP_QueryCommissionRankingList	
	@cbListType TINYINT
WITH ENCRYPTION AS

--��������
SET NOCOUNT ON
BEGIN
	
	-- ʱ�䶨��
	DECLARE @DateID INT
	DECLARE @WeekStartID INT
	DECLARE @MonthStartID INT

	-- ����ʱ��
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	SET @WeekStartID=CAST(CAST(LHAccountsDB.dbo.LHF_GetWeekFirstDay(GETDATE()) AS FLOAT) AS INT)
	SET @MonthStartID=CAST(CAST(LHAccountsDB.dbo.LHF_GetMonthFirstDay(GETDATE()) AS FLOAT) AS INT)

	-- ��ѯ����
	DECLARE @RankingList NVARCHAR(MAX)	
	DECLARE @UserList_Week NVARCHAR(MAX)
	DECLARE @UserList_Month NVARCHAR(MAX)
	DECLARE @UserList_Total NVARCHAR(MAX)

	-- ��ѯ�б�
	SET @UserList_Total = (SELECT TOP 20 ROW_NUMBER() OVER(ORDER BY Commission DESC, UserID) AS RankID, Commission AS RankValue, UserID  FROM LHAccountsDB.dbo.AccountsCommission ORDER BY Commission DESC FOR JSON AUTO) 
	
	-- �°�
	SET @UserList_Month = (SELECT TOP 20 ROW_NUMBER() OVER(ORDER BY Commission DESC, UserID) AS RankID, Commission AS RankValue, UserID FROM  
						  (SELECT SUM(Commission) AS Commission,  UserID FROM  LHGameRecordDB.dbo.ReportDayCommission WHERE DateID>=@MonthStartID GROUP BY UserID) a
						   ORDER BY a.Commission DESC FOR JSON AUTO)

	-- �ܰ�
	SET @UserList_Week =  (SELECT TOP 20 ROW_NUMBER() OVER(ORDER BY Commission DESC, UserID) AS RankID, Commission AS RankValue, UserID FROM  
						  (SELECT SUM(Commission) AS Commission,  UserID FROM  LHGameRecordDB.dbo.ReportDayCommission WHERE DateID>=@WeekStartID GROUP BY UserID) a
						   ORDER BY a.Commission DESC FOR JSON AUTO)
	
	-- �����б�
	IF @UserList_Week IS NULL SET @UserList_Week=N'[]'
	IF @UserList_Month IS NULL SET @UserList_Month=N'[]'
	IF @UserList_Total IS NULL SET @UserList_Total=N'[]'
	
	-- ����JSON
	SET @RankingList=N'{}'	
	SET @RankingList=JSON_MODIFY(@RankingList,N'$.ListType',@cbListType)	
	SET @RankingList=JSON_MODIFY(@RankingList,N'$.UserList_Week',JSON_QUERY(@UserList_Week,N'$'))
	SET @RankingList=JSON_MODIFY(@RankingList,N'$.UserList_Month',JSON_QUERY(@UserList_Month,N'$'))
	SET @RankingList=JSON_MODIFY(@RankingList,N'$.UserList_Total',JSON_QUERY(@UserList_Total,N'$'))

	-- �׳�JSON
	SELECT @RankingList AS RankingList
END
RETURN 0
GO

----------------------------------------------------------------------------------------------------
