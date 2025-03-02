
USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_QueryPayOrders]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_QueryPayOrders]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_QueryWithdrawOrders]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_QueryWithdrawOrders]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- ��ѯ����
CREATE PROCEDURE GSP_MB_QueryPayOrders
	@dwUserID		INT					-- ��ұ�ʶ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	SELECT ISNULL((SELECT CONVERT(NVARCHAR(21),OrderID) AS OrderID, PayAmount FROM LHInsureDB.dbo.PayRecord(NOLOCK) WHERE  UserID = @dwUserID AND PayDateTime>=DATEADD("HH",-12,GETDATE())  FOR JSON AUTO),N'[]') AS PayOrders

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- ��ѯ����
CREATE PROCEDURE GSP_MB_QueryWithdrawOrders
	@dwUserID		INT					-- ��ұ�ʶ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	SELECT ISNULL((SELECT CONVERT(NVARCHAR(21),OrderID) AS OrderID, ApplyAmount FROM LHInsureDB.dbo.WithdrawOrder(NOLOCK) WHERE  UserID = @dwUserID AND OrderState=4 AND PayDateTime>=DATEADD("HH",-12,GETDATE()) FOR JSON AUTO),N'[]') AS WithdrawOrders

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
