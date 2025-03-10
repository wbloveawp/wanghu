
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_OnLineCountInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_OnLineCountInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

--在线信息
CREATE  PROCEDURE dbo.GSP_GP_OnLineCountInfo
	@strMachineID NVARCHAR(12),				-- 机器标识
	@strMachineServer NVARCHAR(32),				-- 机器名称
	@dwOnLineCountSum INT,						-- 总在线数
	@strOnLineCountKind NVARCHAR(2048)			-- 在线信息
WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

	--插入信息
	INSERT OnLineServerInfo (MachineSerial, MachineServer, OnLineCountSum, OnLineCountKind)
	VALUES (@strMachineID, @strMachineServer, @dwOnLineCountSum, @strOnLineCountKind)

RETURN 0

GO

----------------------------------------------------------------------------------------------------
