USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_RealAuth]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_RealAuth]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- 实名认证
CREATE PROC GSP_MB_RealAuth
	@dwUserID INT,								-- 用户 I D	
	@strPassword NCHAR(32),						-- 用户密码
	@strCompellation NVARCHAR(16),				-- 真实名字
	@strPassPortID NVARCHAR(19),				-- 证件号码
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 变量定义
	DECLARE @UserID INT
	DECLARE @Accounts NVARCHAR(31)
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @LogonPass AS NCHAR(32)

	-- 查询用户
	SELECT @UserID=a.UserID,@Accounts=a.Accounts,@LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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
		RETURN 2
	END	
	
	-- 密码判断
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 3
	END
	
	--重复验证
	IF EXISTS (SELECT UserID FROM AccountsAuthentPresent WHERE UserID=@dwUserID)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1327,"Msg":"已认证成功，请勿重复认证"}'
		RETURN 101		
	END
	
	-- 实名修改
	UPDATE AccountsAuthentPresent SET  Compellation=@strCompellation, PassPortID=@strPassPortID WHERE UserID=@dwUserID

	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO AccountsAuthentPresent (UserID,Compellation,PassPortID,CollectDate) VALUES(@dwUserID,@strCompellation,@strPassPortID,GETDATE())
	END
	
	SET @strErrorDescribe=N'{"Idx":1328,"Msg":"认证成功"}'
	
	RETURN 0

END

GO

----------------------------------------------------------------------------------------------------
