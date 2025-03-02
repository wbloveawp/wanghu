
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryUserLoveLiness]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryUserLoveLiness]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryUserLoveLiness]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryUserLoveLiness]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 查询魅力
CREATE PROC GSP_GP_QueryUserLoveLiness

	-- 用户信息
	@dwUserID INT								-- 用户标识	

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询魅力
	DECLARE @LoveLiness INT
	SELECT @LoveLiness=LoveLiness FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 调整变量
	IF @LoveLiness IS NULL SET @LoveLiness=0
	
	-- 抛出记录
	SELECT @LoveLiness AS LoveLiness

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 查询魅力
CREATE PROC GSP_GR_QueryUserLoveLiness

	-- 用户信息
	@dwUserID INT								-- 用户标识	

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询魅力
	DECLARE @LoveLiness INT
	SELECT @LoveLiness=LoveLiness FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 调整变量
	IF @LoveLiness IS NULL SET @LoveLiness=0
	
	-- 抛出记录
	SELECT @LoveLiness AS LoveLiness

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------