
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_ValidateAccredit]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_ValidateAccredit]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 登录验证
CREATE  PROCEDURE dbo.GSP_ValidateAccredit
	@AccreditID NVARCHAR(32),					-- 授权 I D
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineSerial NCHAR(32),				-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 变量定义
DECLARE @StationID INT
DECLARE @StationName VARCHAR(32)
DECLARE @DataBaseName VARCHAR(32)
DECLARE @DataBaseAddr VARCHAR(15)

-- 连接信息
DECLARE @DBPort INT
DECLARE @DBUser NVARCHAR(512)
DECLARE @DBPassword NVARCHAR(512)

DECLARE @NativeDBListID INT

-- 执行逻辑
BEGIN

	---- 查询数据
	--DECLARE @Nullity BIT
	--SELECT @StationID=StationID, @StationName=StationName, @NativeDBListID = NativeDBListID, @Nullity=Nullity 
	--FROM GameStationInfo(NOLOCK)
	--WHERE AccreditID=@AccreditID

	---- 查询用户
	--IF @StationID IS NULL
	--BEGIN
	--	SET @strErrorDescribe=N'系统授权号不存在或者输入有误，请查证后再次尝试！'
	--	RETURN 1
	--END	

	---- 帐号禁止
	--IF @Nullity<>0
	--BEGIN
	--	SET @strErrorDescribe=N'此系统授权号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
	--	RETURN 2
	--END

	---- 输出变量
	--SELECT @StationID AS StationID, @StationName AS StationName

	RETURN 0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
