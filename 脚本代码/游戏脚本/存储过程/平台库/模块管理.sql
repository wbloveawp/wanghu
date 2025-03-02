
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

-- ����ģ��
CREATE  PROCEDURE dbo.GSP_LoadGameModuleInfo
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ����ģ��
	SELECT * FROM GameModuleInfo(NOLOCK) ORDER BY ModuleName

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ����ģ��
CREATE  PROCEDURE dbo.GSP_UpdateGameModuleInfo

	@wModuleID				INT,				-- ģ���ʶ
	@wDBListID				INT,				-- ���ݿ�����
	@wSupportType			INT,				-- ֧������
	@szModuleName			NVARCHAR(31),		-- ģ������
    @dwModuleVerion			INT,				-- ģ��汾
	@dwClientVerion			INT,				-- �ͻ��˰汾
	@szServerDLLName		NVARCHAR(31),		-- ģ��DLL����
	@szClientExeName		NVARCHAR(31),		-- ģ��EXE����

	-- �����Ϣ
	@strErrorDescribe		NVARCHAR(127) OUTPUT		-- �����Ϣ

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ���ұ���
	DECLARE @ModuleID INT
	SELECT @ModuleID=ModuleID FROM GameModuleInfo(NOLOCK) WHERE ModuleID=@wModuleID

	-- �����ж�
	IF @ModuleID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ӧ��ģ����Ϣ�����ڣ�ģ���޸�ʧ�ܣ�'
		RETURN 1
	END

	-- ����ģ��
	UPDATE GameModuleInfo SET DBListID = @wDBListID,SupportType = @wSupportType,ModuleName = @szModuleName,ServerVersion = @dwModuleVerion,ClientVersion = @dwClientVerion,
		ServerDLLName = @szServerDLLName,ClientExeName = @szClientExeName
	WHERE ModuleID = @ModuleID

	-- ����ģ��
	SELECT * FROM GameModuleInfo(NOLOCK) WHERE ModuleID=@ModuleID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
