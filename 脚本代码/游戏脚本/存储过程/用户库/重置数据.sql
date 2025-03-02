----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ResetUserData]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ResetUserData]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GP_ResetUserData
	@wDayOfWeek SMALLINT,					-- ʱ���ʶ			
	@strUserIDList NVARCHAR(MAX)			-- �û��б�	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��������
	IF ISJSON(@strUserIDList)>0
	BEGIN
		UPDATE AccountsInfo SET ActionMaskPerDay=0,ResetDateTime=GETDATE() WHERE UserID IN (SELECT VALUE FROM OPENJSON(@strUserIDList))
	END
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------