 
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadGameVersionInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadGameVersionInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �汾��Ϣ
CREATE  PROCEDURE dbo.GSP_LoadGameVersionInfo
	@wModuleID SMALLINT								-- ��Ϸ I D		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON


-- ִ���߼�
BEGIN

	SELECT * FROM GameVersionInfo(NOLOCK) WHERE ModuleID=@wModuleID OR ModuleID=0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
