
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadGameModuleInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadGameModuleInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_UpdateGameModuleInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_UpdateGameModuleInfo]
GO

SET QUOTED_IDENTIFIER ON
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 加载模块
CREATE  PROCEDURE dbo.GSP_LoadGameModuleInfo
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 加载模块
	SELECT * FROM GameModuleInfo(NOLOCK) ORDER BY ModuleName

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 更新模块
CREATE  PROCEDURE dbo.GSP_UpdateGameModuleInfo

	@wModuleID				INT,				-- 模块标识
	@wDBListID				INT,				-- 数据库索引
	@wSupportType			INT,				-- 支持类型
	@szModuleName			NVARCHAR(31),		-- 模块名称
    @dwModuleVerion			INT,				-- 模块版本
	@dwClientVerion			INT,				-- 客户端版本
	@szServerDLLName		NVARCHAR(31),		-- 模块DLL名称
	@szClientExeName		NVARCHAR(31),		-- 模块EXE名称

	-- 输出信息
	@strErrorDescribe		NVARCHAR(127) OUTPUT		-- 输出信息

WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查找比赛
	DECLARE @ModuleID INT
	SELECT @ModuleID=ModuleID FROM GameModuleInfo(NOLOCK) WHERE ModuleID=@wModuleID

	-- 存在判断
	IF @ModuleID IS NULL
	BEGIN
		SET @strErrorDescribe=N'相应的模块信息不存在，模块修改失败！'
		RETURN 1
	END

	-- 更新模块
	UPDATE GameModuleInfo SET DBListID = @wDBListID,SupportType = @wSupportType,ModuleName = @szModuleName,ServerVersion = @dwModuleVerion,ClientVersion = @dwClientVerion,
		ServerDLLName = @szServerDLLName,ClientExeName = @szClientExeName
	WHERE ModuleID = @ModuleID

	-- 加载模块
	SELECT * FROM GameModuleInfo(NOLOCK) WHERE ModuleID=@ModuleID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
