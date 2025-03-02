USE LHAccountsDB
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_DeleteFriend]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_DeleteFriend]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ApplyAddFriend]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ApplyAddFriend]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AgreeAddFriend]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AgreeAddFriend]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_RefuseAddFriend]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_RefuseAddFriend]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_SetupInfoPublic]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_SetupInfoPublic]
GO


----------------------------------------------------------------------------------------------------

-- 删除好友
CREATE PROC [dbo].[GSP_GP_DeleteFriend]
	@dwUserID			INT,					--用户ID	
	@dwFriendID			INT,					--好友标识
	@strErrorDescribe	NVARCHAR(127) OUTPUT	--输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 自动回滚
SET XACT_ABORT ON

BEGIN
	
	-- 变量定义
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	

	-- 查询用户
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
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

	-- 查询分区
	DECLARE @UserShardIndex INT
	DECLARE @FriendShardIndex INT
	SELECT @UserShardIndex=CASE WHEN UserID=@dwUserID THEN FriendShardIndex ELSE @UserShardIndex END, @FriendShardIndex=CASE WHEN UserID=@dwFriendID THEN FriendShardIndex ELSE @FriendShardIndex END 
	FROM AccountsShardIndex(NOLOCK) WHERE UserID IN (@dwUserID,@dwFriendID)

	-- 调整变量
	IF @UserShardIndex IS NULL SET @UserShardIndex=0
	IF @FriendShardIndex IS NULL SET @FriendShardIndex=0

	-- 变量定义
	DECLARE @Sqlstr NVARCHAR(MAX)
	DECLARE @FriendTableName NVARCHAR(128)

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 好友表名称
	SET	@FriendTableName = CONCAT('AccountsFriends_', CONVERT(NVARCHAR(8), @UserShardIndex));	

	-- 删除好友
	SET	@Sqlstr = CONCAT( '  DELETE ', @FriendTableName, ' WHERE UserID=',CONVERT(NVARCHAR(12), @dwUserID),' AND FriendID=',CONVERT(NVARCHAR(12), @dwFriendID))						  
	EXECUTE(@Sqlstr);

	-- 好友表名称
	SET	@FriendTableName = CONCAT('AccountsFriends_', CONVERT(NVARCHAR(8), @FriendShardIndex));	

	-- 删除好友
	SET	@Sqlstr = CONCAT( '  DELETE ', @FriendTableName, ' WHERE UserID=',CONVERT(NVARCHAR(12), @dwFriendID),' AND FriendID=',CONVERT(NVARCHAR(12), @dwUserID))						  
	EXECUTE(@Sqlstr);

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

END
RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- 申请加好友
CREATE PROC [dbo].[GSP_GP_ApplyAddFriend]
	@dwUserID			INT,					--用户ID	
	@dwObjectID			INT,					--目标标识
	@dwExpireTime		INT,					--过期时间
	@strApplyDesc		NVARCHAR(128),			--申请描述	
	@strErrorDescribe	NVARCHAR(127) OUTPUT	--输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

BEGIN
	
	-- 变量定义
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	

	-- 查询用户
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
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

	-- 校验标识
	IF @dwUserID=@dwObjectID
	BEGIN
		SET @strErrorDescribe=N''
		RETURN 4
	END

	-- 查询分区
	DECLARE @ApplyerShardIndex INT
	DECLARE @ObjectShardIndex INT
	SELECT @ApplyerShardIndex=CASE WHEN UserID=@dwUserID THEN FriendShardIndex ELSE @ApplyerShardIndex END, @ObjectShardIndex=CASE WHEN UserID=@dwObjectID THEN FriendShardIndex ELSE @ObjectShardIndex END 
	FROM AccountsShardIndex(NOLOCK) WHERE UserID IN (@dwUserID,@dwObjectID)

	-- 变量定义
	DECLARE @Sqlstr NVARCHAR(MAX)
	DECLARE @FriendTableName1 NVARCHAR(128)
	DECLARE @FriendTableName2 NVARCHAR(128)

	-- 好友表名称
	SET	@FriendTableName1 = CONCAT('AccountsFriends_', CONVERT(NVARCHAR(8), @ApplyerShardIndex));	
	SET	@FriendTableName2 = CONCAT('AccountsFriends_', CONVERT(NVARCHAR(8), @ObjectShardIndex));	

	-- 创建好友表
	IF (NOT EXISTS(SELECT 1 FROM LHAccountsDB.dbo.sysobjects WHERE xtype='u' AND name=@FriendTableName1))
	BEGIN
		SET	@Sqlstr = dbo.LHF_Script_FriendTable(@FriendTableName1)
		EXECUTE(@Sqlstr);
	END

	-- 创建好友表
	IF (NOT EXISTS(SELECT 1 FROM LHAccountsDB.dbo.sysobjects WHERE xtype='u' AND name=@FriendTableName2))
	BEGIN
		SET	@Sqlstr = dbo.LHF_Script_FriendTable(@FriendTableName2)
		EXECUTE(@Sqlstr);
	END	

	-- 变量定义
	DECLARE @RecordCount SMALLINT=0	
	
	-- 构造语句
	SET	@Sqlstr = CONCAT( '  IF EXISTS(SELECT 1 FROM ', @FriendTableName1,' WHERE UserID=', CONVERT(NVARCHAR(12), @dwUserID),' AND FriendID=',CONVERT(NVARCHAR(12), @dwObjectID),' )',
						  '  BEGIN ',
						  '    SET @Count+=1',
						  '  END',
						  '  IF EXISTS(SELECT 1 FROM ', @FriendTableName2,' WHERE UserID=', CONVERT(NVARCHAR(12), @dwObjectID),' AND FriendID=',CONVERT(NVARCHAR(12), @dwUserID),' )',
						  '  BEGIN ',
						  '    SET @Count+=1',
						  '	 END')

	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@Count SMALLINT=0 OUT',@Count=@RecordCount OUTPUT

	-- 校验数量
	IF @RecordCount=2
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1284,"Msg":"已经是好友无需再申请！"}'
		RETURN 5
	END

	-- 插入申请
	INSERT INTO AccountsApplyInfo(ApplyUid,ObjectID,ApplyType,ApplyDesc,ApplyState,ObjectUidSet,OperateUid,UpdateTime,ExpireTime,CollectTime)
	VALUES (@dwUserID,@dwObjectID,1,@strApplyDesc,0,N'[]',0,GetDate(),@dwExpireTime,GetDate())

	-- 获取标识
	DECLARE @ApplyID BIGINT
	SET @ApplyID=SCOPE_IDENTITY()

	-- 查询信息
	SELECT ApplyID,ApplyUid,ObjectID,ApplyState FROM AccountsApplyInfo(NOLOCK) WHERE ApplyID=@ApplyID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 同意加好友
CREATE PROC [dbo].[GSP_GP_AgreeAddFriend]
	@dwUserID			INT,					--用户ID	
	@dwApplyID			BIGINT,					--申请标识
	@dwCurrTime			INT,					--当前时间			
	@strErrorDescribe	NVARCHAR(127) OUTPUT	--输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 自动回滚
SET XACT_ABORT ON

BEGIN
	
	-- 变量定义
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	

	-- 查询用户
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
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

	-- 变量定义
	DECLARE @ApplyUid INT
	DECLARE @ObjectID INT
	DECLARE @ApplyState TINYINT
	DECLARE @ExpireTime INT

	-- 查询申请
	SELECT @ApplyUid=ApplyUid,@ObjectID=ObjectID,@ApplyState=ApplyState,@ExpireTime=ExpireTime FROM AccountsApplyInfo(NOLOCK) WHERE ApplyID=@dwApplyID

	-- 校验结果
	IF @ApplyUid IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1280,"Msg":"该好友申请不存在！"}'
		RETURN 4
	END

	-- 已过期
	IF @dwCurrTime>=@ExpireTime
	BEGIN
		--更新状态
		IF @ApplyState=0
		BEGIN
			UPDATE AccountsApplyInfo SET ApplyState=3 WHERE ApplyID=@dwApplyID
		END

		-- 返回信息
		SET @strErrorDescribe=N'{"Idx":1282,"Msg":"该好友申请已过期！"}'
		RETURN 5
	END

	-- 校验状态
	IF @ApplyState <> 0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1281,"Msg":"该好友申请已处理！"}'
		RETURN 6
	END

	-- 校验用户
	IF @ObjectID <> @dwUserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1283,"Msg":"您没有权限处理该好友申请！"}'
		RETURN 7
	END

	-- 查询分区
	DECLARE @ApplyerShardIndex INT
	DECLARE @ObjectShardIndex INT
	SELECT @ApplyerShardIndex=CASE WHEN UserID=@ApplyUid THEN FriendShardIndex ELSE @ApplyerShardIndex END, @ObjectShardIndex=CASE WHEN UserID=@ObjectID THEN FriendShardIndex ELSE @ObjectShardIndex END 
	FROM AccountsShardIndex(NOLOCK) WHERE UserID IN (@ApplyUid,@dwUserID)

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 变量定义
	DECLARE @Sqlstr NVARCHAR(MAX)
	DECLARE @FriendTableName NVARCHAR(128)

	-- 好友表名称
	SET	@FriendTableName = CONCAT('AccountsFriends_', CONVERT(NVARCHAR(8), @ApplyerShardIndex));	

	-- 创建数据表
	IF (NOT EXISTS(SELECT 1 FROM LHAccountsDB.dbo.sysobjects WHERE xtype='u' AND name=@FriendTableName))
	BEGIN
		SET	@Sqlstr = dbo.LHF_Script_FriendTable(@FriendTableName)	
		EXECUTE(@Sqlstr);
	END
	
	-- 构造语句
	SET	@Sqlstr = CONCAT( '  IF NOT EXISTS(SELECT 1 FROM ', @FriendTableName,' WHERE UserID=', CONVERT(NVARCHAR(12), @ApplyUid),' AND FriendID=',CONVERT(NVARCHAR(12), @dwUserID),' )',
						  '  BEGIN ',
						  '    INSERT INTO ',@FriendTableName,'(UserID, FriendID)' ,' VALUES(',CONVERT(NVARCHAR(12), @ApplyUid),',',CONVERT(NVARCHAR(12), @dwUserID),')',
						  '  END ELSE ',
						  '  BEGIN ',
						  '    UPDATE ',@FriendTableName,' SET CollectTime=GETDATE()',' WHERE UserID=', CONVERT(NVARCHAR(12), @ApplyUid),' AND FriendID=',CONVERT(NVARCHAR(12), @dwUserID),
						  '	 END')

	EXECUTE(@Sqlstr);
	IF @@ERROR<>0
	BEGIN
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		RETURN 9
	END

	-- 好友表名称
	SET	@FriendTableName = CONCAT('AccountsFriends_', CONVERT(NVARCHAR(8), @ObjectShardIndex));	

	-- 创建数据表
	IF (NOT EXISTS(SELECT 1 FROM LHAccountsDB.dbo.sysobjects WHERE xtype='u' AND name=@FriendTableName))
	BEGIN
		SET	@Sqlstr = dbo.LHF_Script_FriendTable(@FriendTableName)	
		EXECUTE(@Sqlstr);
	END	

	-- 构造语句
	SET	@Sqlstr = CONCAT( '  IF NOT EXISTS(SELECT 1 FROM ', @FriendTableName,' WHERE UserID=', CONVERT(NVARCHAR(12), @dwUserID),' AND FriendID=',CONVERT(NVARCHAR(12), @ApplyUid),' )',
						  '  BEGIN ',
						  '    INSERT INTO ',@FriendTableName,'(UserID, FriendID)' ,' VALUES(',CONVERT(NVARCHAR(12), @dwUserID),',',CONVERT(NVARCHAR(12), @ApplyUid),')',
						  '  END ELSE ',
						  '  BEGIN ',
						  '    UPDATE ',@FriendTableName,' SET CollectTime=GETDATE()',' WHERE UserID=', CONVERT(NVARCHAR(12), @dwUserID),' AND FriendID=',CONVERT(NVARCHAR(12), @ApplyUid),
						  '	 END')

	PRINT @Sqlstr
	EXECUTE(@Sqlstr);
	IF @@ERROR<>0
	BEGIN
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		RETURN 9
	END

	-- 更新数据
	UPDATE AccountsApplyInfo SET ApplyState=1,OperateUid=@dwUserID,UpdateTime=GetDate() WHERE ApplyID=@dwApplyID	

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 抛出信息
	SELECT * FROM AccountsApplyInfo(NOLOCK) WHERE ApplyID=@dwApplyID

END
RETURN 0
GO


----------------------------------------------------------------------------------------------------

-- 拒绝加好友
CREATE PROC [dbo].[GSP_GP_RefuseAddFriend]
	@dwUserID			INT,					--用户ID	
	@dwApplyID			BIGINT,					--申请标识
	@dwCurrTime			INT,					--当前时间		
	@strRefuseDesc		NVARCHAR(128),			--拒绝描述	
	@strErrorDescribe	NVARCHAR(127) OUTPUT	--输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

BEGIN
	
	-- 变量定义
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	

	-- 查询用户
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
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

	-- 变量定义
	DECLARE @ApplyUid INT
	DECLARE @ObjectID INT
	DECLARE @ApplyState TINYINT
	DECLARE @ExpireTime INT

	-- 查询申请
	SELECT @ApplyUid=ApplyUid,@ObjectID=ObjectID,@ApplyState=ApplyState,@ExpireTime=ExpireTime FROM AccountsApplyInfo(NOLOCK) WHERE ApplyID=@dwApplyID

	-- 校验结果
	IF @ApplyUid IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1280,"Msg":"该好友申请不存在！"}'
		RETURN 4
	END

	-- 已过期
	IF @dwCurrTime>=@ExpireTime
	BEGIN
		--更新状态
		IF @ApplyState=0
		BEGIN
			UPDATE AccountsApplyInfo SET ApplyState=3 WHERE ApplyID=@dwApplyID
		END

		-- 返回信息
		SET @strErrorDescribe=N'{"Idx":1282,"Msg":"该好友申请已过期！"}'
		RETURN 5
	END

	-- 校验状态
	IF @ApplyState <> 0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1281,"Msg":"该好友申请已处理！"}'
		RETURN 6
	END

	-- 校验用户
	IF @ObjectID <> @dwUserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1283,"Msg":"您没有权限处理该好友申请！"}'
		RETURN 7
	END

	-- 更新数据
	UPDATE AccountsApplyInfo SET ApplyState=2,RefuseDesc=@strRefuseDesc,OperateUid=@dwUserID,UpdateTime=GetDate() WHERE ApplyID=@dwApplyID

	-- 抛出信息
	SELECT * FROM AccountsApplyInfo(NOLOCK) WHERE ApplyID=@dwApplyID

END
RETURN 0
GO


----------------------------------------------------------------------------------------------------

-- 信息公开设置
CREATE PROC [dbo].[GSP_GP_SetupInfoPublic]
	@dwUserID			INT,					--用户ID	
	@bInfoPublic		TINYINT,				--公开标志
	@strErrorDescribe	NVARCHAR(127) OUTPUT	--输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

BEGIN
	
	-- 变量定义
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	

	-- 查询用户
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
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

	-- 更新标志
	UPDATE AccountsInfo SET InfoPublic=@bInfoPublic WHERE UserID=@dwUserID

END
RETURN 0
GO

----------------------------------------------------------------------------------------------------

