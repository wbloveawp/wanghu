
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadGameBasicConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadGameBasicConfig]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadGameWeightConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadGameWeightConfig]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_UpdateGameBasicConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_UpdateGameBasicConfig]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_UpdateGameWeightConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_UpdateGameWeightConfig]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 加载基础配置
CREATE  PROCEDURE dbo.GSP_LoadGameBasicConfig
	@wKindID	INT
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 输出数据
	SELECT ConfigValue FROM GameConfigBasic(NOLOCK) WHERE KindID=@wKindID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载倍数配置
CREATE  PROCEDURE dbo.GSP_LoadGameWeightConfig
	@wKindID	INT,
	@cbTypeID	TINYINT	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 输出数据
	SELECT * FROM GameConfigWeight(NOLOCK) WHERE KindID=@wKindID AND TypeID=@cbTypeID ORDER BY TypeID,MinTimes
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 保存基础配置
CREATE  PROCEDURE dbo.GSP_UpdateGameBasicConfig
	@wKindID			INT,
	@strBasicConfig		NVARCHAR(4000),
	@strErrorDescribe	NVARCHAR(127) OUTPUT	-- 输出信息	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 校验格式
	IF ISJSON(@strBasicConfig)=0
	BEGIN
		SET @strErrorDescribe=N'基础配置格式有误，保持失败！'
		RETURN 1
	END

	-- 输出数据
	UPDATE GameConfigBasic SET ConfigValue=@strBasicConfig  WHERE KindID=@wKindID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO GameConfigBasic(KindID,ConfigValue,KindName)
		VALUES (@wKindID,@strBasicConfig,ISNULL((SELECT ModuleName FROM GameModuleInfo(NOLOCK) WHERE ModuleID=@wKindID),N''))
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 保存基础配置
CREATE  PROCEDURE dbo.GSP_UpdateGameWeightConfig
	@wKindID			INT,
	@cbWeightType		TINYINT,
	@strWeightConfig	NVARCHAR(4000),
	@strErrorDescribe	NVARCHAR(127) OUTPUT	-- 输出信息	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 校验格式
	IF ISJSON(@strWeightConfig)=0
	BEGIN
		SET @strErrorDescribe=N'权重配置格式有误，保持失败！'
		RETURN 1
	END

	-- 删除数据
	DELETE GameConfigWeight WHERE KindID=@wKindID AND TypeID=@cbWeightType
	
	-- 插入数据
	INSERT INTO GameConfigWeight(KindID,TypeID,MinTimes,MaxTimes,TimesKind,WeightValue)
	SELECT @wKindID,@cbWeightType,JSON_VALUE(VALUE,N'$.MinTimes'),JSON_VALUE(VALUE,N'$.MaxTimes'),JSON_VALUE(VALUE,N'$.TimesKind'),JSON_VALUE(VALUE,N'$.WeightValue') FROM OPENJSON(@strWeightConfig)	
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
