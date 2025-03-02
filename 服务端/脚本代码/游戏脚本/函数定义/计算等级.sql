
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[PFF_CalcUserOrder]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[PFF_CalcUserOrder]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[PFF_CalcGameOrder]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[PFF_CalcGameOrder]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[PFF_CalcRankOrder]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[PFF_CalcRankOrder]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[PFF_GetSeasonID]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[PFF_GetSeasonID]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- 计算用户等级
CREATE FUNCTION [dbo].[PFF_CalcUserOrder]
(
	@MemberPoint INT	
)
RETURNS TINYINT
AS
BEGIN
	
	-- 变量定义
	DECLARE @MemberOrder TINYINT			

	-- 查询等级
	SELECT TOP 1 @MemberOrder=LevelID-1 FROM GameUserLevel(NOLOCK) WHERE NeedPoint>@MemberPoint
	IF @MemberOrder IS NULL
	BEGIN
		-- 查询数据
		SELECT @MemberOrder=MAX(LevelID) FROM GameUserLevel(NOLOCK)
	END

	RETURN @MemberOrder
END

GO

----------------------------------------------------------------------------------------------------
-- 计算游戏等级
CREATE FUNCTION [dbo].[PFF_CalcGameOrder]
(
	@Experience  INT	
)
RETURNS TINYINT
AS
BEGIN
	
	-- 变量定义
	DECLARE @GameOrder TINYINT			

	-- 查询等级
	SELECT TOP 1 @GameOrder=LevelID-1 FROM GameExpLevel(NOLOCK) WHERE NeedPoint>@Experience
	IF @GameOrder IS NULL
	BEGIN
		-- 查询数据
		SELECT @GameOrder=MAX(LevelID) FROM GameExpLevel(NOLOCK)
	END

	RETURN @GameOrder
END

GO

----------------------------------------------------------------------------------------------------
-- 计算排位等级
CREATE FUNCTION [dbo].[PFF_CalcRankOrder]
(
	@RankScore  INT	
)
RETURNS TINYINT
AS
BEGIN
	
	-- 变量定义
	DECLARE @RankOrder TINYINT			

	-- 查询等级
	SELECT TOP 1 @RankOrder=LevelID-1 FROM GameRankLevel(NOLOCK) WHERE NeedScore>@RankScore
	IF @RankOrder IS NULL
	BEGIN
		-- 查询数据
		SELECT @RankOrder=MAX(LevelID) FROM GameRankLevel(NOLOCK)
	END

	RETURN @RankOrder
END

GO

----------------------------------------------------------------------------------------------------
-- 计算赛季标识
CREATE FUNCTION [dbo].[PFF_GetSeasonID]
(	
)
RETURNS TINYINT
AS
BEGIN
	-- 查询配置
	DECLARE @RankingOption NVARCHAR(1024)
	SELECT @RankingOption=OptionValue FROM SystemOptionInfo(NOLOCK) WHERE OptionName=N'RankingOption'

	-- 校验配置
	IF ISJSON(@RankingOption)=0
	BEGIN
		RETURN 0
	END

	-- 时间定义	
	DECLARE @CurrDateTime DATETIME
	DECLARE @SeasonMonths SMALLINT
	DECLARE @SeasonStartTime DATETIME

	-- 设置时间
	SET @CurrDateTime=GETDATE()
	SET @SeasonMonths=ISNULL(JSON_VALUE(@RankingOption,N'$.SeasonMonths'),3)
	SET @SeasonStartTime=ISNULL(JSON_VALUE(@RankingOption,N'$.SeasonStartTime'),N'2021-09-13 00:00:00')

	-- 计算间隔
	DECLARE @MonthDiff SMALLINT
	SET @MonthDiff=DATEDIFF(mm,@SeasonStartTime,@CurrDateTime)

	-- 计算标识
	DECLARE @SeasonID SMALLINT
	SET @SeasonID=(@MonthDiff/@SeasonMonths)+1

	RETURN @SeasonID
END

GO

----------------------------------------------------------------------------------------------------