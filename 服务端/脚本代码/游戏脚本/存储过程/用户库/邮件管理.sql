----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LH_MailAttachExpire]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[LH_MailAttachExpire]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UserQueryMail]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UserQueryMail]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UserModifyMail]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UserModifyMail]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UserAppendMail]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UserAppendMail]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UserDeleteMail]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UserDeleteMail]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_InsertMailBox]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_InsertMailBox]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ClaimMailAttach]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ClaimMailAttach]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ClaimMailAttach]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ClaimMailAttach]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_CollectMailUser]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_CollectMailUser]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_BatchInsertMail]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_BatchInsertMail]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 邮件附件有效检测
CREATE FUNCTION [dbo].[LH_MailAttachExpire]
(		
	@SendTime INT,
	@ClaimTime INT,
	@GoodsIndate INT
)
RETURNS INT
AS
BEGIN
	--
	IF @GoodsIndate > 0
	BEGIN
		IF @ClaimTime = 0
		BEGIN
			IF @GoodsIndate <= ((dbo.LHF_GetTimestamp()-@SendTime)/(24*3600)) RETURN 1
		END ELSE
		BEGIN
			IF @GoodsIndate <= ((@ClaimTime - @SendTime)/(24*3600)) RETURN 1
		END
	END

	RETURN 0

END

GO

----------------------------------------------------------------------------------------------------

-- 加载邮件
CREATE PROC GSP_GP_UserQueryMail
	@dwUserID	INT,							-- 用户标识
	@dwMailID	INT,							-- 邮件标识
	@dwCurrentTime	INT							-- 当前时间
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 删除邮件
	--DELETE AccountsMailBox WHERE UserID=@dwUserID AND (MailState = 4 OR ExpireTime<@dwCurrentTime)
	
	-- 查询邮件信息
	SELECT a.MailID,b.MailKind,b.MailType,a.MailState,b.SendTime,a.ExpireTime,b.MailTitle,b.MailContent,
	ISNULL((SELECT GoodsID,GoodsLevel AS [Level],GoodsCount AS [Count], GoodsIndate AS Indate,.dbo.LH_MailAttachExpire(b.SendTime,a.ClaimTime,GoodsIndate) AS Expire FROM AccountsMailAttach WHERE MailID=a.MailID FOR JSON AUTO),N'') AS AttachGoods
	FROM AccountsMailBox(NOLOCK) a, AccountsMailInfo(NOLOCK) b WHERE a.UserID=@dwUserID AND a.MailID>@dwMailID AND a.MailState<>4 AND (a.ExpireTime=0 OR a.ExpireTime>=@dwCurrentTime) AND a.MailID=b.MailID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 修改邮件
CREATE PROC GSP_GP_UserModifyMail
	@dwUserID INT,							-- 用户标识
	@dwMailID INT,							-- 邮件标识
	@cbMailState TINYINT					-- 邮件状态
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 更新状态
	UPDATE AccountsMailBox SET MailState=@cbMailState WHERE UserID=@dwUserID AND MailID=@dwMailID	

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 添加邮件
CREATE PROC GSP_GP_UserAppendMail
	@cbMailKind			TINYINT,					-- 邮件种类	
	@cbMailType			TINYINT,					-- 邮件类型	
	@cbMailState		TINYINT,					-- 邮件状态
	@dwSendTime			INT,						-- 发送时间
	@dwExpireTime		INT,						-- 过期时间
	@szMailTitle		NVARCHAR(64),				-- 邮件标题
	@szMailContent		NVARCHAR(256),				-- 邮件内容
	@szAttachGoods		NVARCHAR(MAX),				-- 附带物品
	@szReceiverID		NVARCHAR(MAX),				-- 用户标识
	@strErrorDescribe	NVARCHAR(127) OUTPUT		-- 错误描述
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 插入邮件
	INSERT INTO [dbo].[AccountsMailInfo](MailKind, MailType, MailTitle, MailContent, SendTime, ExpireTime, CollectTime)
    VALUES (@cbMailKind, @cbMailType, @szMailTitle, @szMailContent, @dwSendTime, @dwExpireTime, GetDate())

	-- 插入失败
	IF @@ROWCOUNT=0
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1300,"Msg":"数据库异常，邮件无法插入！"}'
		RETURN 1
	END

	-- 插入判断	
	DECLARE @MailID INT			
	SET @MailID = SCOPE_IDENTITY() 

	-- 表变量定义
	DECLARE @AttachGoodsList TABLE (GoodsID SMALLINT,GoodsCount INT,GoodsIndate INT)

	-- 插入记录
	IF ISJSON(@szAttachGoods)>0
	BEGIN
		INSERT AccountsMailAttach (MailID,GoodsID,GoodsLevel,GoodsCount,GoodsIndate) 
		SELECT @MailID,JSON_VALUE(VALUE,'$.GoodsID'),ISNULL(JSON_VALUE(VALUE,'$.Level'),0), CONVERT(INT,JSON_VALUE(VALUE,'$.Count')),CONVERT(INT,JSON_VALUE(VALUE,'$.Indate')) FROM OPENJSON(@szAttachGoods)		
	END
	
	-- 返回信息
	SELECT @MailID AS MailID
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
-- 删除邮件
CREATE PROC GSP_GP_UserDeleteMail
	@dwUserID INT,							-- 用户标识		
	@dwMailID INT,							-- 邮件标识
	@strErrorDescribe NVARCHAR(127) OUTPUT  -- 错误描述
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 批量删除
	IF @dwMailID=0
	BEGIN
		-- 删除邮件
		--DELETE AccountsMailBox WHERE UserID=@dwUserID AND MailState > 1

		UPDATE AccountsMailBox SET MailState=4 WHERE UserID=@dwUserID AND MailState > 1

	END ELSE
	BEGIN
		-- 查询状态
		DECLARE @MailState TINYINT
		SELECT @MailState=MailState FROM AccountsMailBox WITH(UPDLOCK)  WHERE UserID=@dwUserID AND MailID=@dwMailID
		
		IF @MailState IS NULL
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1297,"Msg":"系统未查询到该邮件信息！"}'
			RETURN 1
		END ELSE
		BEGIN
			IF @MailState=1
			BEGIN
				SET @strErrorDescribe=N'{"Idx":1298,"Msg":"请先查阅邮件！"}'
				RETURN 2
			END
		END
		
		-- 删除邮件
		--DELETE AccountsMailBox WHERE UserID=@dwUserID AND MailID=@dwMailID
		UPDATE AccountsMailBox SET MailState=4 WHERE UserID=@dwUserID AND MailID=@dwMailID
	END
	
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 插入邮箱
CREATE PROC GSP_GP_InsertMailBox
	@dwMailID	INT,								-- 邮件标识
	@cbMailState TINYINT,							-- 邮件状态
	@dwExpireTime INT,								-- 过期时间
	@szReceiverID NVARCHAR(MAX)						-- 用户列表	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 插入邮箱
	IF ISJSON(@szReceiverID)>0
	BEGIN
		INSERT INTO AccountsMailBox (UserID,MailID,MailState,ExpireTime,UpdateTime)
		SELECT VALUE, @dwMailID, @cbMailState, @dwExpireTime, GetDate() FROM OPENJSON(@szReceiverID)
	END
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- 领取附件
CREATE PROC GSP_GR_ClaimMailAttach
	@dwUserID INT,							-- 用户标识		
	@dwMailID INT,							-- 邮件标识	
	@strErrorDescribe NVARCHAR(127) OUTPUT  -- 错误描述
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 查询状态
	DECLARE @MailKind TINYINT
	DECLARE @MailState TINYINT
	DECLARE @AttachGoods NVARCHAR(MAX)
	SELECT @MailKind=b.MailKind, @MailState=a.MailState FROM AccountsMailBox a, AccountsMailInfo b WHERE a.UserID=@dwUserID AND a.MailID=@dwMailID AND a.MailID=b.MailID
	
	--校验邮件
	IF @MailKind IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1297,"Msg":"系统未查询到该邮件信息！"}'
		RETURN 4
	END

	-- 校验类型
	IF @MailKind<>2
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1299,"Msg":"该邮件没有可领取的附件！"}'
		RETURN 5
	END

	--校验状态
	IF @MailState<>1
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1300,"Msg":"该邮件状态异常，请联系客服了解详细情况！"}'
		RETURN 6
	END

	-- 更新状态
	UPDATE AccountsMailBox SET MailState=4 WHERE UserID=@dwUserID AND MailID=@dwMailID	

	-- 查询附件
	SET  @AttachGoods=(SELECT GoodsID,GoodsLevel,GoodsCount,GoodsIndate FROM AccountsMailAttach WHERE MailID=@dwMailID FOR JSON AUTO)

	-- 抛出记录
	SELECT @AttachGoods AS AttachGoods
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- 领取附件
CREATE PROC GSP_GP_ClaimMailAttach
	@dwUserID INT,							-- 用户标识		
	@dwMailID INT,							-- 邮件标识
	@dwTimeStamp INT,						-- 时间戳		
	@wReason INT,							-- 变化原因	
	@strErrorDescribe NVARCHAR(127) OUTPUT  -- 错误描述
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 查询状态
	IF EXISTS(SELECT 1 FROM AccountsOnlineInfo(NOLOCK) WHERE UserID=@dwUserID AND ServerID<>0)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1301,"Msg":"系统查询到您正在游戏房间中，附件领取失败！"}'
		RETURN 1
	END	

	-- 查询状态
	DECLARE @MailKind TINYINT
	DECLARE @MailType SMALLINT
	DECLARE @MailState TINYINT
	DECLARE @AttachGoods NVARCHAR(MAX)
	DECLARE @nSendTimestamp INT
	SELECT @MailKind=b.MailKind,@MailType=b.MailType,@nSendTimestamp=b.SendTime FROM AccountsMailBox a, AccountsMailInfo b WHERE a.UserID=@dwUserID AND a.MailID=@dwMailID AND a.MailID=b.MailID
	
	--校验邮件
	IF @MailKind IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1297,"Msg":"系系统未查询到该邮件信息！"}'
		RETURN 4
	END

	-- 校验类型
	IF @MailKind<>2
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1299,"Msg":"该邮件没有可领取的附件！"}'
		RETURN 5
	END

	-- 变量定义
	DECLARE @Index INT
	DECLARE @LoopCount INT	
	DECLARE @ResidueCount INT	

	-- 物品信息
	DECLARE @IncGoodsID INT
	DECLARE @IncGoodsLevel INT
	DECLARE @IncGoodsCount INT
	DECLARE @IncGoodsIndate INT		
	DECLARE @Expire INT

	-- 表变量定义
	DECLARE @AttachGoodsList TABLE (ID INT identity(1,1),GoodsID SMALLINT,Level SMALLINT,Count INT,Indate INT,Expire SMALLINT)

	-- 插入记录
	--IF ISJSON(@AttachGoods)>0
	--BEGIN
		INSERT @AttachGoodsList (GoodsID,Level,Count,Indate,Expire) 
		SELECT GoodsID,GoodsLevel,Goodscount,GoodsIndate,dbo.LH_MailAttachExpire(@nSendTimestamp,0,GoodsIndate) FROM AccountsMailAttach(NOLOCK) WHERE MailID=@dwMailID
	--END

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 查询状态
	SELECT @MailState=MailState FROM AccountsMailBox WITH(UPDLOCK)  WHERE UserID=@dwUserID AND MailID=@dwMailID

	--校验状态
	IF @MailState<>1
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1300,"Msg":"该邮件状态异常，请联系客服了解详细情况！"}'

		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		RETURN 5		
	END


	-- 初始化
	SELECT @Index=1,@LoopCount=COUNT(1),@wReason+=@MailType FROM @AttachGoodsList
	
	-- 循环处理
	WHILE @Index<=@LoopCount
	BEGIN	
		-- 查询信息
		SELECT @IncGoodsID=GoodsID,@IncGoodsLevel=Level,@IncGoodsCount=Count,@IncGoodsIndate=Indate,@Expire=Expire FROM @AttachGoodsList WHERE ID=@Index
		--有效期内
		IF @Expire = 0
		BEGIN
			-- 校验物品类型
			IF dbo.LHF_IsMoneyGoods(@IncGoodsID) = 1
			BEGIN

				-- 写入财富
				EXEC GSP_GP_WriteGameWealth @dwUserID,@IncGoodsID,@wReason,@IncGoodsCount

			END ELSE
			BEGIN

				-- 添加物品
				EXEC @ResidueCount = GSP_GP_AppendGoodsItem @dwUserID,@wReason,@IncGoodsID,@IncGoodsLevel,@IncGoodsCount,@IncGoodsIndate,@dwTimeStamp			

				-- 校验数量
				IF @ResidueCount>0
				BEGIN
					SET @strErrorDescribe=N'{"Idx":1301,"Msg":"部分物品堆叠数量已超上限，请扩容后再次尝试！"}'

					-- 事务回滚
					ROLLBACK TRAN
					SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
					RETURN 5
				END		
			END
		END

		-- 更新索引
		SET @Index=@Index+1
	END	

	-- 过期时间
	DECLARE @ExpireTime INT
	SET @ExpireTime = dbo.LHF_GetTimestamp()+2*24*3600

	-- 更新状态 失效时间改为2天后
	UPDATE AccountsMailBox SET MailState=3,ExpireTime=@ExpireTime,UpdateTime=GETDATE(),ClaimTime=dbo.LHF_GetTimestamp() WHERE UserID=@dwUserID AND MailID=@dwMailID		

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 构造JSON
	SET @AttachGoods = (SELECT GoodsID,Level,Count,Indate,Expire FROM @AttachGoodsList FOR JSON AUTO)

	-- 抛出记录
	SELECT @AttachGoods AS AttachGoods
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- 收集用户
CREATE PROC GSP_GP_CollectMailUser
	@dwMailID	INT,								-- 邮件标识
	@LastLogonTime DATETIME							-- 登录时间
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 校验邮件
	IF EXISTS (SELECT TOP 1 1 FROM PendingMailUser WHERE MailID=@dwMailID)
	BEGIN
		RETURN 0
	END

	-- 收集用户
	INSERT INTO PendingMailUser(MailID,UserID)
	SELECT @dwMailID,UserID FROM AccountsInfo(NOLOCK) WHERE LogonDate>=@LastLogonTime
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- 批量插入
CREATE PROC GSP_GP_BatchInsertMail
	@dwMailID	INT,								-- 邮件标识
	@cbMailState TINYINT,							-- 邮件状态
	@dwExpireTime INT,								-- 过期时间
	@wUserCount INT									-- 用户数量
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 批量插入
	INSERT INTO AccountsMailBox(UserID,MailID,MailState,ExpireTime,UpdateTime)
	SELECT TOP(@wUserCount) UserID,@dwMailID,@cbMailState,@dwExpireTime,GETDATE() FROM PendingMailUser(NOLOCK) WHERE MailID=@dwMailID

	-- 删除用户
	DELETE PendingMailUser WHERE UserID IN (SELECT TOP(@wUserCount) UserID FROM PendingMailUser(NOLOCK) WHERE MailID=@dwMailID)

	-- 剩余数量
	SELECT COUNT(1) AS ResidueCount FROM PendingMailUser(NOLOCK) WHERE MailID=@dwMailID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
