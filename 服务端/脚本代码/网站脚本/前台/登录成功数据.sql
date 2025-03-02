----------------------------------------------------------------------------------------------------
-- 时间：2021-11-01
-- 用途：手机登录数据获取
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].WEB_PW_GetLoginLater') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].WEB_PW_GetLoginLater
GO

SET QUOTED_IDENTIFIER ON
GO

SET ANSI_NULLS ON
GO

----------------------------------------------------------------------------------------------------

CREATE PROCEDURE WEB_PW_GetLoginLater
	@dwUserID INT,								-- 用户标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID			INT
DECLARE @GameID			INT
DECLARE @Nullity		TINYINT
DECLARE @AgentID		INT
DECLARE @ParentID		INT
DECLARE @ParentGameID	INT
DECLARE @ParentUserID	INT

-- 执行逻辑
BEGIN
	-- 查询用户代理
	SELECT @UserID=UserID,@GameID=GameID,@Nullity=Nullity,@AgentID=AgentID,@ParentID=ParentID FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
	
	-- 用户存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试！'
		RETURN 100
	END	

	-- 帐号封禁
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 101
	END		

	SET @ParentGameID=0
	SET @ParentUserID=0
	IF @ParentID >0 
	BEGIN
		SELECT @ParentGameID=GameID,@ParentUserID=UserID FROM AccountsInfo(NOLOCK) WHERE UserID=@ParentID
	END

	-- 用户信息
	SELECT @UserID AS UserID,@GameID AS GameID,@AgentID AS AgentID,@ParentGameID AS ParentGameID,@ParentUserID AS ParentUserID

	--玩家是否首冲
	SELECT OrderID FROM LHInsureDB.dbo.PayRecord(NOLOCK) WHERE UserID=@UserID
	
END

RETURN 0

GO
