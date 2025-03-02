
----------------------------------------------------------------------------------------------------

USE LHGameRankingDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadRankingConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadRankingConfig]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 加载配置
CREATE  PROCEDURE dbo.GSP_GR_LoadRankingConfig	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 查询配置
	DECLARE @RankingOption NVARCHAR(1024)
	SELECT @RankingOption=OptionValue FROM LHPlatformDB.dbo.SystemOptionInfo(NOLOCK) WHERE OptionName=N'RankingOption'

	-- 变量定义		
	DECLARE @RankingCost INT
	DECLARE @WinConvertRate INT
	DECLARE @LoseConvertRate INT
	DECLARE @RiseLoveHeartCount INT
	DECLARE @LostLoveHeartCount INT
	DECLARE @SeasonMonths SMALLINT
	DECLARE @SeasonStartTime DATETIME		

	-- 设置时间
	SET @RankingCost=ISNULL(JSON_VALUE(@RankingOption,N'$.RankingCost'),300000)
	SET @SeasonMonths=ISNULL(JSON_VALUE(@RankingOption,N'$.SeasonMonths'),3)
	SET @WinConvertRate=ISNULL(JSON_VALUE(@RankingOption,N'$.WinConvertRate'),70)
	SET @LoseConvertRate=ISNULL(JSON_VALUE(@RankingOption,N'$.LoseConvertRate'),50)	
	SET @RiseLoveHeartCount=ISNULL(JSON_VALUE(@RankingOption,N'$.RiseLoveHeartCount'),3)
	SET @LostLoveHeartCount=ISNULL(JSON_VALUE(@RankingOption,N'$.LostLoveHeartCount'),1)
	SET @SeasonStartTime=ISNULL(JSON_VALUE(@RankingOption,N'$.SeasonStartTime'),N'2021-09-01 00:00:00')	

	-- 输出数据
	SELECT @RankingCost AS RankingCost, @RiseLoveHeartCount AS RiseLoveHeartCount, @LostLoveHeartCount AS LostLoveHeartCount, @WinConvertRate AS WinConvertRate, @LoseConvertRate AS LoseConvertRate, 
		   @SeasonMonths AS SeasonMonths, @SeasonStartTime AS SeasonStartTime
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
