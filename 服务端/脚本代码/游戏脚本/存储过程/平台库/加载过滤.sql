
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


--���ع���
CREATE  PROCEDURE dbo.GSP_LoadGameFilterItem WITH ENCRYPTION AS

--��������
SET NOCOUNT ON

--��ѯ����
SELECT * FROM GameSiftItem(NOLOCK) WHERE SiftMask<>0 AND Nullity=0

RETURN 0

GO

----------------------------------------------------------------------------------------------------

