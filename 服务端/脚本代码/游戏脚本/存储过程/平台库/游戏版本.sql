 
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

-- 版本信息
CREATE  PROCEDURE dbo.GSP_LoadGameVersionInfo
	@wModuleID SMALLINT								-- 游戏 I D		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON


-- 执行逻辑
BEGIN

	SELECT * FROM GameVersionInfo(NOLOCK) WHERE ModuleID=@wModuleID OR ModuleID=0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
