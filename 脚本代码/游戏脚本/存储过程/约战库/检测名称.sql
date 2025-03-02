
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DetectClubName]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DetectClubName]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- 标记记录
CREATE  PROCEDURE dbo.GSP_GR_DetectClubName	
	@strClubName NVARCHAR(64)			-- 茶社名称	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN	
	
	IF EXISTS(SELECT 1 FROM ClubProperty(NOLOCK) WHERE ClubName=@strClubName)
	BEGIN		
		RETURN 1
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------