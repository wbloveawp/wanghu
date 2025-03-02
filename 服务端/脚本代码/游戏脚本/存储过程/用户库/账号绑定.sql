
USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_BindExternalAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_BindExternalAccounts]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- 绑定外部账号
CREATE PROCEDURE GSP_MB_BindExternalAccounts
	@dwUserID			INT,					-- 用户 I D
	@dwExternalID		INT,					-- 他方标识
	@strUniqueID		NVARCHAR(32),			-- 唯一标识
	@strFacePath		NVARCHAR(256),			-- 头像地址
	@strUserDatum		NVARCHAR(512),			-- 用户资料	
	@strClientIP		NVARCHAR(15),			-- 连接地址	
	@strErrorDescribe	NVARCHAR(127) OUTPUT	-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 定义变量
DECLARE @UserID			INT
DECLARE @Gender			TINYINT
DECLARE @NickName		NVARCHAR(32)
DECLARE @Nullity		BIT
DECLARE @ShutDown		BIT
DECLARE @ProtectID		INT
DECLARE @LogonPass		NCHAR(32)
DECLARE @MoorMachine	TINYINT
DECLARE	@LogonMachineID	NCHAR(34)

-- 执行逻辑
BEGIN	
	-- 查询用户	
	SELECT @UserID=a.UserID, @Gender=a.Gender, @NickName=a.NickName, @ProtectID=a.ProtectID, @LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus, @MoorMachine=a.MoorMachine, 
			@LogonMachineID=a.LogonMachineID 
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
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

	-- 查询他方
	DECLARE @BindUserID INT
	SELECT @BindUserID=UserID FROM AccountsExternal(NOLOCK) WHERE UniqueID=@strUniqueID AND ExternalID=@dwExternalID

	-- 已经绑定
	IF @BindUserID IS NOT NULL
	BEGIN
		-- 重复绑定
		IF @BindUserID=@dwUserID
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1303,"Msg":"您已经绑定了当前账号,无需重复绑定！"}'
			RETURN 4
		END ELSE
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1304,"Msg":"当前账号已经绑定了其他的游戏账号！"}'
			RETURN 5
		END
	END

	-- 查询他方
	DECLARE @UniqueID NVARCHAR(32)
	SELECT @UniqueID=UniqueID FROM AccountsExternal(NOLOCK) WHERE UserID=@dwUserID

	-- 已经绑定
	IF @UniqueID IS NOT NULL
	BEGIN
		-- 重复绑定		
		SET @strErrorDescribe=N'{"Idx":1303,"Msg":"您已经绑定了当前账号,无需重复绑定！"}'
		RETURN 6		
	END

	-- 插入账号
	INSERT INTO AccountsExternal(UniqueID,ExternalID,UserID,PublicID,NickName,Gender,UserDatum,UserFacePath,LogonClientIP,LogonDateTime,SubscribeFlag,SubscribeTime,CollectDateTime)
	VALUES (@strUniqueID,@dwExternalID,@UserID,N'',@NickName,@Gender,@strUserDatum,@strFacePath,@strClientIP,GETDATE(),1,GETDATE(),GETDATE())	

	-- 成功提示
	SET @strErrorDescribe=N'{"Idx":1305,"Msg":"账号绑定成功！"}'

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------