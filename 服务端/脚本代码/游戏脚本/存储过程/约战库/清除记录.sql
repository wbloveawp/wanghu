
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_CleanupSetup]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_CleanupSetup]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_CleanupScoreRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_CleanupScoreRecord]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_CleanupRevenueRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_CleanupRevenueRecord]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_CleanupRankListRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_CleanupRankListRecord]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- 清理设置
CREATE PROC GSP_GR_CleanupSetup
	-- 用户信息
	@dwUserID INT,								-- 用户 I D
	@dwClubID INT,								-- 茶馆标识
	@bAutoCleanup TINYINT,						-- 自动清理
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT

-- 执行逻辑
BEGIN
	-- 变量定义
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- 查询用户
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
		RETURN 2
	END	

	-- 帐号关闭
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
		RETURN 3
	END

	-- 查询群主
	DECLARE @CreaterGameID INT
	SELECT @CreaterGameID=CreaterGameID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- 结果判断
	IF @CreaterGameID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'
		RETURN 5		
	END

	-- 修改规则
	--UPDATE ClubRules SET AutoCleanup=@bAutoCleanup WHERE ClubID=@dwClubID

	-- 查询规则
	SELECT * FROM ClubRules(NOLOCK) WHERE ClubID=@dwClubID	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 清除积分记录
CREATE PROC GSP_GR_CleanupScoreRecord
	-- 用户信息
	@dwUserID INT,								-- 用户 I D
	@dwClubID INT,								-- 茶馆标识
	@dwMemberID INT,							-- 成员标识
	@cbCleanupObject TINYINT,					-- 清除对象
	@dwCreaterGameID INT,						-- 馆主标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT

-- 变量定义
DECLARE	@TableID	INT;										
DECLARE @Sqlstr		NVARCHAR(512);									
DECLARE	@TableName	NVARCHAR(128);

-- 执行逻辑
BEGIN
	-- 变量定义
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- 查询用户
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
		RETURN 2
	END	

	-- 帐号关闭
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
		RETURN 3
	END	

	-- 查询群主
	DECLARE @CreaterGameID INT
	SELECT @CreaterGameID=CreaterGameID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- 结果判断
	IF @CreaterGameID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'
		RETURN 5		
	END

	-- 校验馆主标识
	IF @dwCreaterGameID<>@CreaterGameID
	BEGIN
		--SET @strErrorDescribe=N'抱歉，您输入的馆主游戏标识不匹配，记录清理失败！'
		RETURN 6				
	END

	-- 清理一个成员积分
	IF @cbCleanupObject=0
	BEGIN
		-- 锁定信息
		DECLARE @BattleNum INT
		DECLARE @BattleStatus TINYINT
		SELECT @BattleNum=BattleNum,@BattleStatus=BattleStatus FROM BattleUserLocker(NOLOCK) WHERE UseriD=@dwMemberID
	
		-- 调整变量
		IF @BattleStatus IS NULL SET @BattleStatus=0
		
		-- 约战状态
		IF @BattleStatus=1
		BEGIN
			-- 提示信息
			SET @strErrorDescribe=N'{"Idx":1197,"Msg":"由于该馆员在约战房间['+CONVERT(NVARCHAR(18),@BattleNum)+']中的游戏尚未完成，暂时不能进行清理操作！","Ops":["'+CONVERT(NVARCHAR(18),@BattleNum)+'"]}'
			RETURN 7		
		END

		-- 分表标识
		SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

		-- 群组成员表
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));					

		-- 更新脚本		
		SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName,' SET ClubScore=0',
							  ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))		
		EXECUTE(@Sqlstr)
	END

	-- 清理所有成员积分
	IF @cbCleanupObject=1
	BEGIN
		-- 变量定义
		DECLARE @BattleUserCount INT
		SET @BattleUserCount=0

		-- 分表标识
		SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

		-- 群组成员表
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));					

		-- 更新脚本		
		SET	@Sqlstr = CONCAT( ' SELECT @BattleCount=COUNT(1) FROM BattleUserLocker(NOLOCK) WHERE BattleStatus=1 AND UserID IN ','(SELECT MemberID FROM ', @TableName,'(NOLOCK) ',
							  ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),')')		

		print @Sqlstr
		-- 执行语句
		EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@BattleCount SMALLINT OUT',@BattleCount=@BattleUserCount OUTPUT

		-- 约战用户数目
		IF @BattleUserCount>1
		BEGIN
			-- 提示信息
			SET @strErrorDescribe=N'{"Idx":1198,"Msg":"由于当前茶社中部分成员还在进行约战，暂时不能进行清理操作！"}'
			RETURN 8		
		END
		
		-- 更新脚本		
		SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName,' SET ClubScore=0 WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID))		
		EXECUTE(@Sqlstr)							
	END
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 清除税收记录
CREATE PROC GSP_GR_CleanupRevenueRecord
	-- 用户信息
	@dwUserID INT,								-- 用户 I D
	@dwClubID INT,								-- 茶馆标识
	@dwMemberID INT,							-- 成员标识
	@cbCleanupObject TINYINT,					-- 清除对象
	@dwCreaterGameID INT,						-- 馆主标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT

-- 变量定义
DECLARE	@TableID	INT;										
DECLARE @Sqlstr		NVARCHAR(512);									
DECLARE	@TableName	NVARCHAR(128);									

-- 执行逻辑
BEGIN
	-- 变量定义
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- 查询用户
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
		RETURN 2
	END	

	-- 帐号关闭
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
		RETURN 3
	END

	-- 查询群主
	DECLARE @CreaterGameID INT
	SELECT @CreaterGameID=CreaterGameID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- 结果判断
	IF @CreaterGameID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'
		RETURN 5		
	END

	-- 校验馆主标识
	IF @dwCreaterGameID<>@CreaterGameID
	BEGIN
		--SET @strErrorDescribe=N'抱歉，您输入的馆主游戏标识不匹配，记录清理失败！'
		RETURN 6				
	END

	-- 清理一个成员税收
	IF @cbCleanupObject=0
	BEGIN
		-- 分表标识
		SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

		-- 群组成员表
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));					

		-- 更新脚本		
		SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName,' SET RevenueCount=0',
							  ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))		
		EXECUTE(@Sqlstr)
	END

	-- 清理所有成员税收
	IF @cbCleanupObject=1
	BEGIN
		-- 分表标识
		SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

		-- 群组成员表
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));					

		-- 更新脚本		
		SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName,' SET RevenueCount=0 WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID))		
		EXECUTE(@Sqlstr)					
	END

	-- 清理俱乐部税收
	IF @cbCleanupObject=2
	BEGIN
		-- 清理数据
		UPDATE StreamClubBalance SET Revenue=0 WHERE ClubID=@dwClubID		
		
		-- 清理数据
		UPDATE StreamClubPartner SET SupplyRevenue=0,ExtractRevenue=0 WHERE ClubID=@dwClubID						
	END
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 清除榜单记录
CREATE PROC GSP_GR_CleanupRankListRecord
	-- 用户信息
	@dwUserID INT,								-- 用户 I D
	@dwClubID INT,								-- 茶馆标识
	@dwMemberID INT,							-- 成员标识
	@cbListKind TINYINT,						-- 榜单类型
	@cbCleanupObject TINYINT,					-- 清除对象
	@dwCreaterGameID INT,						-- 游戏标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT

-- 执行逻辑
BEGIN
	-- 变量定义
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- 查询用户
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
		RETURN 2
	END	

	-- 帐号关闭
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
		RETURN 3
	END

	-- 查询群主
	DECLARE @CreaterGameID INT
	SELECT @CreaterGameID=CreaterGameID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- 结果判断
	IF @CreaterGameID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'
		RETURN 5		
	END

	-- 校验馆主标识
	IF @dwCreaterGameID<>@CreaterGameID
	BEGIN
		--SET @strErrorDescribe=N'抱歉，您输入的馆主游戏标识不匹配，记录清理失败！'
		RETURN 6				
	END

	-- 大赢家榜
	IF @cbListKind=1
	BEGIN
		-- 清理一个成员
		IF @cbCleanupObject=0
		BEGIN
			UPDATE StreamClubUser SET WinCount=0 WHERE ClubID=@dwClubID AND UserID=@dwMemberID
		END

		-- 清理所有成员
		IF @cbCleanupObject=1
		BEGIN
			UPDATE StreamClubUser SET WinCount=0 WHERE ClubID=@dwClubID			
		END
	END 

	-- 土豪榜
	IF @cbListKind=2
	BEGIN
		-- 清理一个成员
		IF @cbCleanupObject=0
		BEGIN
			UPDATE StreamClubUser SET LostCount=0 WHERE ClubID=@dwClubID AND UserID=@dwMemberID
		END

		-- 清理所有成员
		IF @cbCleanupObject=1
		BEGIN
			UPDATE StreamClubUser SET LostCount=0 WHERE ClubID=@dwClubID			
		END
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------