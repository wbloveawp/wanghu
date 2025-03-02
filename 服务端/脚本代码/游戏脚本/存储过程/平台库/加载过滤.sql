
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadGameFilterItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadGameFilterItem]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------


--加载过滤
CREATE  PROCEDURE dbo.GSP_LoadGameFilterItem WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

--查询过滤
SELECT * FROM GameSiftItem(NOLOCK) WHERE SiftMask<>0 AND Nullity=0

RETURN 0

GO

----------------------------------------------------------------------------------------------------

