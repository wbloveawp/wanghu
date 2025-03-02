
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadGamePageItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadGamePageItem]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------


--���ض���
CREATE  PROCEDURE dbo.GSP_LoadGamePageItem WITH ENCRYPTION AS

--��������
SET NOCOUNT ON

--��ѯ����
SELECT * FROM GamePageItem(NOLOCK) WHERE Nullity=0 ORDER BY SortID

RETURN 0

GO

----------------------------------------------------------------------------------------------------
