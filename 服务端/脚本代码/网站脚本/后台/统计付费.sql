----------------------------------------------------------------------
-- ������ͳ��ע��������ֵ
----------------------------------------------------------------------
USE [LHAccountsDB]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WSP_NewPayUserCount]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[WSP_NewPayUserCount]
GO

----------------------------------------------------------------------
CREATE PROC [WSP_NewPayUserCount]
		@MonthID		INT			-- ��ǰ�·�
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

DECLARE @StartDate DATETIME
DECLARE @EndDate DATETIME

BEGIN
	
	SET @StartDate=CONVERT(DATETIME,LTRIM(@MonthID)+'01',20)
	SET @EndDate=DATEADD(MONTH,1,@StartDate)

	-- ע��������ֵ
	DECLARE @MonthRegPayUserCount INT
	SELECT @MonthRegPayUserCount=COUNT(A.UserID) FROM (SELECT UserID FROM LHInsureDB.dbo.PayRecord WHERE PayDateTime<@EndDate AND UserID IN (SELECT UserID FROM AccountsInfo WHERE RegisterDate>=@StartDate AND RegisterDate<@EndDate) GROUP BY UserID) AS A
	IF @MonthRegPayUserCount IS NULL
		SET @MonthRegPayUserCount=0
	
	SELECT @MonthRegPayUserCount AS MonthRegPayUserCount

END
RETURN 0
GO