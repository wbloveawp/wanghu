
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadGameGoodsInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadGameGoodsInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadGameLevelInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadGameLevelInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadUserLevelInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadUserLevelInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadRankLevelInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadRankLevelInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QuerySystemOptionInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QuerySystemOptionInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadNickNameOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadNickNameOption]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 加载物品
CREATE  PROCEDURE dbo.GSP_GS_LoadGameGoodsInfo		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

	-- 游戏物品
	SELECT * FROM GameGoodsInfo(NOLOCK)

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载游戏等级
CREATE  PROCEDURE dbo.GSP_GS_LoadGameLevelInfo	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

	-- 游戏等级
	SELECT * FROM GameExpLevel(NOLOCK)

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载用户等级
CREATE  PROCEDURE dbo.GSP_GS_LoadUserLevelInfo	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

	-- 用户等级
	SELECT * FROM GameUserLevel(NOLOCK)

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载排位等级
CREATE  PROCEDURE dbo.GSP_GS_LoadRankLevelInfo	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

	-- 用户等级
	SELECT * FROM GameRankLevel(NOLOCK) ORDER BY LevelID 

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载配置
CREATE PROC GSP_GP_QuerySystemOptionInfo
	@szOptionName NVARCHAR(32)							-- 配置名称
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 定义变量
	DECLARE @OptionValue VARCHAR(MAX)

	-- 加载信息
	SELECT @OptionValue=OptionValue FROM SystemOptionInfo(NOLOCK) WHERE OptionName=@szOptionName

	-- 设置变量
	IF @OptionValue IS NULL SET @OptionValue=''

	-- 输出数据
	SELECT @OptionValue AS OptionValue, @szOptionName AS OptionName

END

RETURN 0

GO
----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------

-- 昵称配置
CREATE PROC GSP_LoadNickNameOption
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 读取配置
	DECLARE @szModifyNickNameOption NVARCHAR(MAX)
	SELECT @szModifyNickNameOption=StatusValue FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ModifyNickNameOption'

	DECLARE @MoneyID INT
	DECLARE @MoneyAmount INT
	DECLARE @FreeTimesPerAccounts INT
	DECLARE @MaxTimesPerDay INT

	SET @MoneyID = JSON_VALUE(@szModifyNickNameOption,'$.MoneyID')
	SET @MoneyAmount = JSON_VALUE(@szModifyNickNameOption,'$.MoneyAmount')
	SET @FreeTimesPerAccounts = JSON_VALUE(@szModifyNickNameOption,'$.FreeTimesPerAccounts')
	SET @MaxTimesPerDay = JSON_VALUE(@szModifyNickNameOption,'$.MaxTimesPerDay')

	IF @MoneyID IS NULL SET @MoneyID = 1
	IF @MoneyAmount IS NULL SET @MoneyAmount = 400
	IF @FreeTimesPerAccounts IS NULL SET @FreeTimesPerAccounts = 1
	IF @MaxTimesPerDay IS NULL SET @MaxTimesPerDay = 1

	SELECT @MoneyID AS GoodsID,@MoneyAmount AS GoodsCount,@FreeTimesPerAccounts AS FreeModifyTimes ,@MaxTimesPerDay AS DailyModifyTimes
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------
