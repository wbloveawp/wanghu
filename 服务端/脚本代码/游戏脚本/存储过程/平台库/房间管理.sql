
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_InsertGameServer]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_InsertGameServer]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_ModifyGameServer]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_ModifyGameServer]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_DeleteGameServer]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_DeleteGameServer]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadGameServerInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadGameServerInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ���뷿��
CREATE  PROCEDURE dbo.GSP_InsertGameServer

	@wModuleID INT,								-- ģ���ʶ	
	@wKindID INT,								-- ���ͱ�ʶ
	@wNodeID INT,								-- �ҽӱ�ʶ
	@wSortID INT,								-- �����ʶ
	@wVisibleMask INT,							-- ��������
	@strServerName NVARCHAR(32),				-- ��������
    @wTableCount INT,							-- ������Ŀ
	@wServerType INT,							-- ��������
	@wServerPort INT,							-- ����˿�
	@wServerLevel INT,							-- ����ȼ�
	@cbMatchType TINYINT,						-- ��������
    @lCellScore BIGINT,							-- ��ͻ���
	@lLessScore BIGINT,							-- ��ͻ���
	@lRestrictScore BIGINT,						-- ���ƻ���
	@lRevenue INT,								-- ˰�ձ���
	@lExpGainRate INT,							-- ��������
    @wDBListID INT,								-- ���ݿ�����
    @lMinEnterScore BIGINT,						-- ��ͽ���
    @lMaxEnterScore BIGINT,						-- ������
    @lMinEnterMember INT,						-- ��ͻ�Ա�ȼ�
    @lMaxEnterMember INT,						-- ��߻�Ա�ȼ�
   	@cbDistributeMode TINYINT,					-- ����ģʽ
	@cbDistributeRule TINYINT,					-- �������
	@wMinPlayerCount  SMALLINT,					-- ��������
	@wMinDistributeUser SMALLINT,				-- ��������
	@wMinPartakeGameUser SMALLINT,				-- ��Ϸ����
	@wMaxPartakeGameUser SMALLINT,				-- ��Ϸ����
	@wDistributeTimespace SMALLINT,				-- ������
	@dwBattleRule INT,							-- Լս����
	@wMaxPlayer INT,							-- ��Ϸ����
	@dwServerRule INT,							-- �������
    @dwAttachUserRight INT,						-- ����Ȩ��
	@strServiceAddr NVARCHAR(32),				-- �����ַ
	@strServiceDomain NVARCHAR(64),				-- ��������
	@strServiceMachine NCHAR(32),				-- �������		
	@cbCustomRule VARBINARY(1024),				-- ���ƹ���

	-- �����Ϣ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ����ģ��
	DECLARE @ModuleID INT
	SELECT @ModuleID=ModuleID FROM GameModuleInfo(NOLOCK) WHERE ModuleID=@wModuleID

	-- �����ж�
	IF @ModuleID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ӧ�ķ��������Ϣ�����ڣ����䴴��ʧ�ܣ�'
		RETURN 1
	END

	-- ��ȡ�˿�
    DECLARE @ServerPort INT
    IF @wServerPort <> 0 
    BEGIN
      SELECT @ServerPort=ServerPort FROM GameServerInfo(NOLOCK) WHERE ServerPort=@wServerPort AND ServiceMachine=@strServiceMachine

      -- �˿��ж�
      IF @ServerPort=@wServerPort
      BEGIN
          SET @strErrorDescribe=N'���ڷ���˿���ͬ����Ϸ���䣬���䴴��ʧ�ܣ�'
          RETURN 1
      END
    END
    SET @ServerPort = @wServerPort

	-- ��������
    INSERT INTO dbo.GameServerInfo
		(
        ServerName,
  		KindID,
  		NodeID,
  		SortID,
  		VisibleMask,	
  		ModuleID,
  		TableCount,
  		ServerType,
  		ServerPort,
		ServerLevel,
		MatchType,
  		CellScore,
  		ServiceScore,
		ExpGainRate,
  		DBListID,
  		RestrictScore,
  		MinTableScore,
  		MinEnterScore,
  		MaxEnterScore,
  		MinEnterMember,
  		MaxEnterMember,
		DistributeMode,
		DistributeRule,
		MinPlayerCount,
		MinDistributeUser,
		MinPartakeGameUser,
		MaxPartakeGameUser,
		DistributeTimespace,
		BattleRule,
  		MaxPlayer,
  		ServerRule,
  		AttachUserRight,
		ServiceAddr,
		ServiceDomain,
  		ServiceMachine,
  		CustomRule
		) 
	VALUES 
        (
  		@strServerName,  		
  		@wKindID,
  		@wNodeID,
  		@wSortID,
  		@wVisibleMask,
  		@wModuleID,
  		@wTableCount,
  		@wServerType,
		@ServerPort,
		@wServerLevel,  	
		@cbMatchType,
        @lCellScore,
        @lRevenue,
		@lExpGainRate,
        @wDBListID,
        @lRestrictScore,
        @lLessScore,
        @lMinEnterScore,
        @lMaxEnterScore,
        @lMinEnterMember,
        @lMaxEnterMember,
		@cbDistributeMode,
		@cbDistributeRule,
		@wMinPlayerCount,
		@wMinDistributeUser,
		@wMinPartakeGameUser,
		@wMaxPartakeGameUser,
		@wDistributeTimespace,
		@dwBattleRule,
        @wMaxPlayer,
        @dwServerRule,
        @dwAttachUserRight,
		@strServiceAddr,
		@strServiceDomain,
        @strServiceMachine,
        @cbCustomRule
      	);

	-- ���ط���
	SELECT 
		A.ServerID,
    	A.ServerName,
  		A.KindID,
  		A.NodeID,
  		A.SortID,
  		A.VisibleMask,
  		A.ModuleID,
  		A.TableCount,
  		A.ServerType,
  		A.ServerPort,
		A.ServerLevel,
		A.MatchType,
  		A.CellScore,
  		A.ServiceScore,
		A.ExpGainRate,
  		A.DBListID,
  		A.RestrictScore,
  		A.MinTableScore,
  		A.MinEnterScore,
  		A.MaxEnterScore,
  		A.MinEnterMember,
  		A.MaxEnterMember,  		
		A.DistributeMode,
		A.DistributeRule,
		A.MinPlayerCount,
		A.MinDistributeUser,
		A.MinPartakeGameUser,
		A.MaxPartakeGameUser,
		A.DistributeTimespace,
		A.BattleRule,
  		A.MaxPlayer,
  		A.ServerRule,
  		A.AttachUserRight,
		A.ServiceAddr,
		A.ServiceDomain,
  		A.ServiceMachine,
  		A.CustomRule,  				
        B.ModuleName,
        B.SupportType,
        B.ServerVersion,
        B.ClientVersion,
        B.ServerDllName,
        B.ClientExeName
    FROM 
    	GameServerInfo(NOLOCK) AS A,
        GameModuleInfo(NOLOCK) AS B
	WHERE ServerID=SCOPE_IDENTITY() AND 
    	A.ModuleID=B.ModuleID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �޸ķ���
CREATE  PROCEDURE dbo.GSP_ModifyGameServer

	-- ��������
	@wServerID INT,								-- �����ʶ
	@wModuleID INT,								-- ģ���ʶ
	@wKindID INT,								-- ���ͱ�ʶ
	@wNodeID INT,								-- �ҽӱ�ʶ
	@wSortID INT,								-- �����ʶ
	@wVisibleMask INT,							-- ��������
	@strServerName NVARCHAR(32),				-- ��������
    @wTableCount INT,							-- ������Ŀ
	@wServerType INT,							-- ��������
	@wServerPort INT,							-- ����˿�
	@wServerLevel INT,							-- ����ȼ�
	@cbMatchType TINYINT,						-- ��������
    @lCellScore BIGINT,							-- ��ͻ���
	@lLessScore BIGINT,							-- ��ͻ���
	@lRestrictScore BIGINT,						-- ���ƻ���
	@lRevenue INT,								-- ˰�ձ���
	@lExpGainRate INT,							-- ��������
    @wDBListID INT,								-- ���ݿ�����
    @lMinEnterScore BIGINT,						-- ��ͽ���
    @lMaxEnterScore BIGINT,						-- ������
    @lMinEnterMember INT,						-- ��ͻ�Ա�ȼ�
    @lMaxEnterMember INT,						-- ��߻�Ա�ȼ�
	@cbDistributeMode TINYINT,					-- ����ģʽ
	@cbDistributeRule TINYINT,					-- �������
	@wMinPlayerCount  SMALLINT,					-- ��������
	@wMinDistributeUser SMALLINT,				-- ��������
	@wMinPartakeGameUser SMALLINT,				-- ��Ϸ����
	@wMaxPartakeGameUser SMALLINT,				-- ��Ϸ����
	@wDistributeTimespace SMALLINT,				-- ������   
	@dwBattleRule INT,							-- Լս���� 
	@wMaxPlayer INT,							-- ��Ϸ����
	@dwServerRule INT,							-- �������
    @dwAttachUserRight INT,						-- ����Ȩ��
	@strServiceAddr NVARCHAR(32),				-- �����ַ
	@strServiceDomain NVARCHAR(64),				-- ��������
	@strServiceMachine NCHAR(32),				-- �������
	@cbCustomRule VARBINARY(1024),				-- ���ƹ���
	
	-- �����Ϣ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ����ģ��
	DECLARE @ModuleID INT
	SELECT @ModuleID=ModuleID FROM GameModuleInfo(NOLOCK) WHERE ModuleID=@wModuleID

	-- �����ж�
	IF @ModuleID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ӧ�ķ��������Ϣ�����ڣ����䴴��ʧ�ܣ�'
		RETURN 1
	END

	-- ���ҷ���
	DECLARE @ServerID INT
    DECLARE @ServerPort INT
	SELECT @ServerID=ServerID, @ServerPort=ServerPort FROM GameServerInfo(NOLOCK) WHERE ServerID=@wServerID AND ModuleID=@wModuleID

	-- ����ж�
	IF @ServerID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ϸ���䲻���ڻ����Ѿ���ɾ���ˣ������޸�ʧ�ܣ�'
		RETURN 2
	END
    
    -- �˿��ж�
    IF @ServerPort <> @wServerPort AND @wServerPort <> 0
    BEGIN
    	IF EXISTS(SELECT * FROM GameServerInfo(NOLOCK) WHERE ServerPort=@wServerPort AND ServiceMachine=@strServiceMachine)
    	BEGIN
        	SET @strErrorDescribe=N'���ڷ���˿���ͬ����Ϸ���䣬���䴴��ʧ�ܣ�'
    		RETURN 3
        END
    END
    SET @ServerPort = @wServerPort
    
	-- ���·���
    UPDATE GameServerInfo  
    SET 
      ServerName = @strServerName,
      KindID = @wKindID,
      NodeID = @wNodeID,
      SortID = @wSortID,
      VisibleMask = @wVisibleMask,
      TableCount = @wTableCount,
      ServerType = @wServerType,
      ServerPort = @ServerPort,
	  ServerLevel = @wServerLevel,
	  MatchType = @cbMatchType,
      CellScore = @lCellScore,
      ServiceScore = @lRevenue,
	  ExpGainRate = @lExpGainRate,
      DBListID = @wDBListID,
      RestrictScore = @lRestrictScore,
      MinTableScore = @lLessScore,
      MinEnterScore = @lMinEnterScore,
      MaxEnterScore = @lMaxEnterScore,
      MinEnterMember = @lMinEnterMember,
      MaxEnterMember = @lMaxEnterMember,
	  DistributeMode=@cbDistributeMode,
	  DistributeRule=@cbDistributeRule,
	  MinPlayerCount=@wMinPlayerCount,
	  MinDistributeUser=@wMinDistributeUser,
	  MinPartakeGameUser=@wMinPartakeGameUser,
	  MaxPartakeGameUser=@wMaxPartakeGameUser,
	  DistributeTimespace=@wDistributeTimespace,
	  BattleRule=@dwBattleRule,
      MaxPlayer = @wMaxPlayer,
      ServerRule = @dwServerRule,
      AttachUserRight = @dwAttachUserRight,
	  ServiceAddr = @strServiceAddr,
	  ServiceDomain = @strServiceDomain,
      ServiceMachine = @strServiceMachine,
      CustomRule = @cbCustomRule,
      ModifyDateTime = GetDate()
    WHERE 
      ServerID=@ServerID
    

	-- ���ط���
	SELECT 
		A.ServerID,
    	A.ServerName,
  		A.KindID,
  		A.NodeID,
  		A.SortID,
  		A.VisibleMask,
  		A.ModuleID,
  		A.TableCount,
  		A.ServerType,
  		A.ServerPort,
		A.ServerLevel,
		A.MatchType,
  		A.CellScore,
  		A.ServiceScore,
		A.ExpGainRate,
  		A.DBListID,
  		A.RestrictScore,
  		A.MinTableScore,
  		A.MinEnterScore,
  		A.MaxEnterScore,
  		A.MinEnterMember,
  		A.MaxEnterMember,  		
		A.DistributeMode,
		A.DistributeRule,
		A.MinPlayerCount,
		A.MinDistributeUser,
		A.MinPartakeGameUser,
		A.MaxPartakeGameUser,
		A.DistributeTimespace,		
		A.BattleRule,
  		A.MaxPlayer,
  		A.ServerRule,
  		A.AttachUserRight,
		A.ServiceAddr,
		A.ServiceDomain,
  		A.ServiceMachine,
  		A.CustomRule,  		
        B.ModuleName,
        B.SupportType,
        B.ServerVersion,
        B.ClientVersion,
        B.ServerDllName,
        B.ClientExeName
    FROM 
    	GameServerInfo(NOLOCK) AS A,
        GameModuleInfo(NOLOCK) AS B
	WHERE ServerID=@wServerID AND 
    	A.ModuleID=B.ModuleID 

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ɾ������
CREATE  PROCEDURE dbo.GSP_DeleteGameServer
	@wServerID INT,								-- �����ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ���ҷ���
	DECLARE @ServerID INT
	SELECT @ServerID=ServerID FROM GameServerInfo(NOLOCK) WHERE ServerID=@wServerID

	-- ����ж�
	IF @ServerID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ϸ���䲻���ڻ����Ѿ���ɾ���ˣ������޸�ʧ�ܣ�'
		RETURN 1
	END

	-- ɾ������
	DELETE GameServerInfo WHERE ServerID=@wServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���ط���
CREATE  PROCEDURE dbo.GSP_LoadGameServerInfo
	@wServerID INT,								-- �����ʶ
	@strMachineID NCHAR(32),					-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ���ط���
/*	SELECT * FROM GameServerInfo(NOLOCK),GameModuleInfo(NOLOCK)
	WHERE GameServerInfo.ModuleID=GameModuleInfo.ModuleID AND ((@wServerID=ServerID) OR (@wServerID=0))
		AND ((ServiceMachine=@strMachineID) OR (@strMachineID=''))
	ORDER BY ModuleName,ServerName
*/	
    -- ���ط���
	SELECT 
		A.ServerID,
    	A.ServerName,  		
  		A.KindID,
  		A.NodeID,
  		A.SortID,
  		A.VisibleMask,
  		A.ModuleID,
  		A.TableCount,
  		A.ServerType,
  		A.ServerPort,
		A.ServerLevel,
		A.MatchType,
  		A.CellScore,
  		A.ServiceScore,
		A.ExpGainRate,
  		A.DBListID AS DBListID,
  		A.RestrictScore,
  		A.MinTableScore,
  		A.MinEnterScore,
  		A.MaxEnterScore,
  		A.MinEnterMember,
  		A.MaxEnterMember,
		A.DistributeMode,
		A.DistributeRule,
		A.MinPlayerCount,
		A.MinDistributeUser,
		A.MinPartakeGameUser,
		A.MaxPartakeGameUser,
		A.DistributeTimespace,	
		A.BattleRule,
  		A.MaxPlayer,
  		A.ServerRule,
  		A.AttachUserRight,
		A.ServiceAddr,
		A.ServiceDomain,
  		A.ServiceMachine,
  		A.CustomRule,  		
        B.ModuleName,
        B.SupportType,
        B.ServerVersion,
        B.ClientVersion,
        B.ServerDllName,
        B.ClientExeName
    FROM 
    	GameServerInfo(NOLOCK) AS A,
        GameModuleInfo(NOLOCK) AS B
	WHERE A.Nullity=0 AND 
		 A.ModuleID=B.ModuleID AND 
    	((@wServerID=A.ServerID) OR (@wServerID=0)) AND 
		((A.ServiceMachine=@strMachineID) OR (@strMachineID=''))
	ORDER BY B.ModuleName,A.ServerName
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
