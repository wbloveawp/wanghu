
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_ModifyLogonPassword]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_ModifyLogonPassword]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 修改密码
CREATE PROC GSP_MB_ModifyLogonPassword
	@dwUserID INT,								-- 用户 I D
	@strSrcPassword NCHAR(32),					-- 用户密码
	@strNewPassword NCHAR(32),					-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 变量定义
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @LogonPass AS NCHAR(32)

	-- 查询用户
	SELECT @UserID=a.UserID, @LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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
		RETURN 2
	END	
	
	-- 密码判断
	IF @LogonPass<>@strSrcPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 3
	END

	-- 修改密码
	UPDATE AccountsInfo SET LogonPass=@strNewPassword WHERE UserID=@dwUserID

	-- 设置信息
	IF @@ERROR=0 
	BEGIN
		INSERT LHGameRecordDB.dbo.RecordModifyPassword
			(UserID,TypeID,OldPassword,NewPassword,CollectClientIP,CollectDateTime,ChannelID,MasterID)
		VALUES
			(@UserID,1,@strSrcPassword,@strNewPassword,@strClientIP,GetDate(),0,0)

		SET @strErrorDescribe=N'{"Idx":1051,"Msg":"操作成功！"}'
	END

	RETURN 0

END

RETURN 0

GO
----------------------------------------------------------------------------------------------------
