
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadDataBaseInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadDataBaseInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadDatabaseList]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadDatabaseList]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 连接信息
CREATE PROCEDURE GSP_LoadDataBaseInfo
	@wDBListID INT						-- 数据库索引
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 变量定义
DECLARE @DBPort INT
DECLARE @DBUser NVARCHAR(512)
DECLARE @DBPassword NVARCHAR(512)
DECLARE @DataBaseName VARCHAR(32)
DECLARE @DataBaseAddr VARCHAR(15)
DECLARE @DBInfoID INT
DECLARE @ErrorDescribe NVARCHAR(128)

-- 执行逻辑
BEGIN	
	-- 连接信息
	SELECT @DBPort=A.DBPort, @DBUser=A.DBUser, @DBPassword=A.DBPassword, @DataBaseName=B.DBAliasName,@DataBaseAddr=A.DBAddr
	FROM DataBaseInfo(NOLOCK) AS A, DataBaseList(NOLOCK) AS B
	WHERE B.DBListID = @wDBListID AND B.DBInfoID = A.DBInfoID

	-- 存在判断
	IF @@ROWCOUNT=0
	BEGIN
		SET @ErrorDescribe=N'数据库 [ '+STR(@wDBListID)+N' ] 连接信息不存在，请检查 LHPlatformDB 数据库的 DataBaseList 和 DataBaseInfo 表数据'
		SELECT [ErrorDescribe]=@ErrorDescribe
		RETURN 1
	END

	-- 输出变量
	SELECT @DataBaseName AS DataBaseName, @DataBaseAddr AS DataBaseAddr, @DBPort AS DBPort, @DBUser AS DBUser, @DBPassword AS DBPassword
	
END

RETURN 0

----------------------------------------------------------------------------------------------------
GO

----------------------------------------------------------------------------------------------------

-- 登录验证
CREATE  PROCEDURE dbo.GSP_LoadDatabaseList
	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON


-- 执行逻辑
BEGIN
	SELECT * FROM DataBaseList(NOLOCK) WHERE DBGenre = 1
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
