
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

-- 插入房间
CREATE  PROCEDURE dbo.GSP_InsertGameServer

	@wModuleID INT,								-- 模块标识	
	@wKindID INT,								-- 类型标识
	@wNodeID INT,								-- 挂接标识
	@wSortID INT,								-- 排序标识
	@wVisibleMask INT,							-- 可视掩码
	@strServerName NVARCHAR(32),				-- 房间名字
    @wTableCount INT,							-- 桌子数目
	@wServerType INT,							-- 服务类型
	@wServerPort INT,							-- 服务端口
	@wServerLevel INT,							-- 房间等级
	@cbMatchType TINYINT,						-- 比赛类型
    @lCellScore BIGINT,							-- 最低积分
	@lLessScore BIGINT,							-- 最低积分
	@lRestrictScore BIGINT,						-- 限制积分
	@lRevenue INT,								-- 税收比例
	@lExpGainRate INT,							-- 经验增益
    @wDBListID INT,								-- 数据库索引
    @lMinEnterScore BIGINT,						-- 最低进入
    @lMaxEnterScore BIGINT,						-- 最大进入
    @lMinEnterMember INT,						-- 最低会员等级
    @lMaxEnterMember INT,						-- 最高会员等级
   	@cbDistributeMode TINYINT,					-- 分组模式
	@cbDistributeRule TINYINT,					-- 分组规则
	@wMinPlayerCount  SMALLINT,					-- 最少人数
	@wMinDistributeUser SMALLINT,				-- 分组人数
	@wMinPartakeGameUser SMALLINT,				-- 游戏人数
	@wMaxPartakeGameUser SMALLINT,				-- 游戏人数
	@wDistributeTimespace SMALLINT,				-- 分组间隔
	@dwBattleRule INT,							-- 约战规则
	@wMaxPlayer INT,							-- 游戏人数
	@dwServerRule INT,							-- 房间规则
    @dwAttachUserRight INT,						-- 符加权限
	@strServiceAddr NVARCHAR(32),				-- 服务地址
	@strServiceDomain NVARCHAR(64),				-- 服务域名
	@strServiceMachine NCHAR(32),				-- 服务机器		
	@cbCustomRule VARBINARY(1024),				-- 定制规则

	-- 输出信息
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息

WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查找模块
	DECLARE @ModuleID INT
	SELECT @ModuleID=ModuleID FROM GameModuleInfo(NOLOCK) WHERE ModuleID=@wModuleID

	-- 存在判断
	IF @ModuleID IS NULL
	BEGIN
		SET @strErrorDescribe=N'相应的服务组件信息不存在，房间创建失败！'
		RETURN 1
	END

	-- 获取端口
    DECLARE @ServerPort INT
    IF @wServerPort <> 0 
    BEGIN
      SELECT @ServerPort=ServerPort FROM GameServerInfo(NOLOCK) WHERE ServerPort=@wServerPort AND ServiceMachine=@strServiceMachine

      -- 端口判断
      IF @ServerPort=@wServerPort
      BEGIN
          SET @strErrorDescribe=N'存在服务端口相同的游戏房间，房间创建失败！'
          RETURN 1
      END
    END
    SET @ServerPort = @wServerPort

	-- 创建房间
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

	-- 加载房间
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

-- 修改房间
CREATE  PROCEDURE dbo.GSP_ModifyGameServer

	-- 索引变量
	@wServerID INT,								-- 房间标识
	@wModuleID INT,								-- 模块标识
	@wKindID INT,								-- 类型标识
	@wNodeID INT,								-- 挂接标识
	@wSortID INT,								-- 排序标识
	@wVisibleMask INT,							-- 可视掩码
	@strServerName NVARCHAR(32),				-- 房间名字
    @wTableCount INT,							-- 桌子数目
	@wServerType INT,							-- 服务类型
	@wServerPort INT,							-- 服务端口
	@wServerLevel INT,							-- 房间等级
	@cbMatchType TINYINT,						-- 比赛类型
    @lCellScore BIGINT,							-- 最低积分
	@lLessScore BIGINT,							-- 最低积分
	@lRestrictScore BIGINT,						-- 限制积分
	@lRevenue INT,								-- 税收比例
	@lExpGainRate INT,							-- 经验增益
    @wDBListID INT,								-- 数据库索引
    @lMinEnterScore BIGINT,						-- 最低进入
    @lMaxEnterScore BIGINT,						-- 最大进入
    @lMinEnterMember INT,						-- 最低会员等级
    @lMaxEnterMember INT,						-- 最高会员等级
	@cbDistributeMode TINYINT,					-- 分组模式
	@cbDistributeRule TINYINT,					-- 分组规则
	@wMinPlayerCount  SMALLINT,					-- 最少人数
	@wMinDistributeUser SMALLINT,				-- 分组人数
	@wMinPartakeGameUser SMALLINT,				-- 游戏人数
	@wMaxPartakeGameUser SMALLINT,				-- 游戏人数
	@wDistributeTimespace SMALLINT,				-- 分组间隔   
	@dwBattleRule INT,							-- 约战规则 
	@wMaxPlayer INT,							-- 游戏人数
	@dwServerRule INT,							-- 房间规则
    @dwAttachUserRight INT,						-- 符加权限
	@strServiceAddr NVARCHAR(32),				-- 服务地址
	@strServiceDomain NVARCHAR(64),				-- 服务域名
	@strServiceMachine NCHAR(32),				-- 服务机器
	@cbCustomRule VARBINARY(1024),				-- 定制规则
	
	-- 输出信息
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息

WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查找模块
	DECLARE @ModuleID INT
	SELECT @ModuleID=ModuleID FROM GameModuleInfo(NOLOCK) WHERE ModuleID=@wModuleID

	-- 存在判断
	IF @ModuleID IS NULL
	BEGIN
		SET @strErrorDescribe=N'相应的服务组件信息不存在，房间创建失败！'
		RETURN 1
	END

	-- 查找房间
	DECLARE @ServerID INT
    DECLARE @ServerPort INT
	SELECT @ServerID=ServerID, @ServerPort=ServerPort FROM GameServerInfo(NOLOCK) WHERE ServerID=@wServerID AND ModuleID=@wModuleID

	-- 结果判断
	IF @ServerID IS NULL
	BEGIN
		SET @strErrorDescribe=N'游戏房间不存在或者已经被删除了，房间修改失败！'
		RETURN 2
	END
    
    -- 端口判断
    IF @ServerPort <> @wServerPort AND @wServerPort <> 0
    BEGIN
    	IF EXISTS(SELECT * FROM GameServerInfo(NOLOCK) WHERE ServerPort=@wServerPort AND ServiceMachine=@strServiceMachine)
    	BEGIN
        	SET @strErrorDescribe=N'存在服务端口相同的游戏房间，房间创建失败！'
    		RETURN 3
        END
    END
    SET @ServerPort = @wServerPort
    
	-- 更新房间
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
    

	-- 加载房间
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

-- 删除房间
CREATE  PROCEDURE dbo.GSP_DeleteGameServer
	@wServerID INT,								-- 房间标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查找房间
	DECLARE @ServerID INT
	SELECT @ServerID=ServerID FROM GameServerInfo(NOLOCK) WHERE ServerID=@wServerID

	-- 结果判断
	IF @ServerID IS NULL
	BEGIN
		SET @strErrorDescribe=N'游戏房间不存在或者已经被删除了，房间修改失败！'
		RETURN 1
	END

	-- 删除房间
	DELETE GameServerInfo WHERE ServerID=@wServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载房间
CREATE  PROCEDURE dbo.GSP_LoadGameServerInfo
	@wServerID INT,								-- 房间标识
	@strMachineID NCHAR(32),					-- 服务机器
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 加载房间
/*	SELECT * FROM GameServerInfo(NOLOCK),GameModuleInfo(NOLOCK)
	WHERE GameServerInfo.ModuleID=GameModuleInfo.ModuleID AND ((@wServerID=ServerID) OR (@wServerID=0))
		AND ((ServiceMachine=@strMachineID) OR (@strMachineID=''))
	ORDER BY ModuleName,ServerName
*/	
    -- 加载房间
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
