
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

-- ���ػ�������
CREATE  PROCEDURE dbo.GSP_LoadGameBasicConfig
	@wKindID	INT
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- �������
	SELECT ConfigValue FROM GameConfigBasic(NOLOCK) WHERE KindID=@wKindID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���ر�������
CREATE  PROCEDURE dbo.GSP_LoadGameWeightConfig
	@wKindID	INT,
	@cbTypeID	TINYINT	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- �������
	SELECT * FROM GameConfigWeight(NOLOCK) WHERE KindID=@wKindID AND TypeID=@cbTypeID ORDER BY TypeID,MinTimes
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �����������
CREATE  PROCEDURE dbo.GSP_UpdateGameBasicConfig
	@wKindID			INT,
	@strBasicConfig		NVARCHAR(4000),
	@strErrorDescribe	NVARCHAR(127) OUTPUT	-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- У���ʽ
	IF ISJSON(@strBasicConfig)=0
	BEGIN
		SET @strErrorDescribe=N'�������ø�ʽ���󣬱���ʧ�ܣ�'
		RETURN 1
	END

	-- �������
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

-- �����������
CREATE  PROCEDURE dbo.GSP_UpdateGameWeightConfig
	@wKindID			INT,
	@cbWeightType		TINYINT,
	@strWeightConfig	NVARCHAR(4000),
	@strErrorDescribe	NVARCHAR(127) OUTPUT	-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- У���ʽ
	IF ISJSON(@strWeightConfig)=0
	BEGIN
		SET @strErrorDescribe=N'Ȩ�����ø�ʽ���󣬱���ʧ�ܣ�'
		RETURN 1
	END

	-- ɾ������
	DELETE GameConfigWeight WHERE KindID=@wKindID AND TypeID=@cbWeightType
	
	-- ��������
	INSERT INTO GameConfigWeight(KindID,TypeID,MinTimes,MaxTimes,TimesKind,WeightValue)
	SELECT @wKindID,@cbWeightType,JSON_VALUE(VALUE,N'$.MinTimes'),JSON_VALUE(VALUE,N'$.MaxTimes'),JSON_VALUE(VALUE,N'$.TimesKind'),JSON_VALUE(VALUE,N'$.WeightValue') FROM OPENJSON(@strWeightConfig)	
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
