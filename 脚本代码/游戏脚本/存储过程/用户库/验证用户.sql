/***************************************************************************************************
** 名称：帐号登录 (游戏)
***************************************************************************************************/
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_EfficacyUserID]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_EfficacyUserID]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 验证用户
CREATE PROC GSP_GP_EfficacyUserID
	@dwUserID INT,								-- 用户标识
	@strPassword NCHAR(32),						-- 用户密码
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 查询密码
	DECLARE  @LogonPass NVARCHAR(32)
	SELECT @LogonPass=LogonPass FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 校验密码
	IF @LogonPass IS NULL OR @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试登录！"}'
		RETURN 1
	END	

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
