
----------------------------------------------------------------------------------------------------

USE LHGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_GameColumnItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_GameColumnItem]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

--加载列表描述
CREATE  PROCEDURE dbo.GSP_GR_GameColumnItem 
WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

--查询游戏节点
SELECT * FROM GameColumnItem ORDER BY SortID

RETURN 0

GO

----------------------------------------------------------------------------------------------------
