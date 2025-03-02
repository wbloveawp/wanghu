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

-- 重置数据
CREATE PROC GSP_GP_ResetUserData
	@wDayOfWeek SMALLINT,					-- 时间标识			
	@strUserIDList NVARCHAR(MAX)			-- 用户列表	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 更新数据
	IF ISJSON(@strUserIDList)>0
	BEGIN
		UPDATE AccountsInfo SET ActionMaskPerDay=0,ResetDateTime=GETDATE() WHERE UserID IN (SELECT VALUE FROM OPENJSON(@strUserIDList))
	END
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------