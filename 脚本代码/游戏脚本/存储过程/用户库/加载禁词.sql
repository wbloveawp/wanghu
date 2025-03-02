
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadConfineContent]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadConfineContent]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

--读取限制词汇
CREATE  PROCEDURE dbo.GSP_GP_LoadConfineContent 
WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

SELECT String AS ForbidWords FROM ConfineContent(NOLOCK)

RETURN 0

GO

----------------------------------------------------------------------------------------------------
