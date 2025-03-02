
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
-- �����û��ȼ�
CREATE FUNCTION [dbo].[PFF_CalcUserOrder]
(
	@MemberPoint INT	
)
RETURNS TINYINT
AS
BEGIN
	
	-- ��������
	DECLARE @MemberOrder TINYINT			

	-- ��ѯ�ȼ�
	SELECT TOP 1 @MemberOrder=LevelID-1 FROM GameUserLevel(NOLOCK) WHERE NeedPoint>@MemberPoint
	IF @MemberOrder IS NULL
	BEGIN
		-- ��ѯ����
		SELECT @MemberOrder=MAX(LevelID) FROM GameUserLevel(NOLOCK)
	END

	RETURN @MemberOrder
END

GO

----------------------------------------------------------------------------------------------------
-- ������Ϸ�ȼ�
CREATE FUNCTION [dbo].[PFF_CalcGameOrder]
(
	@Experience  INT	
)
RETURNS TINYINT
AS
BEGIN
	
	-- ��������
	DECLARE @GameOrder TINYINT			

	-- ��ѯ�ȼ�
	SELECT TOP 1 @GameOrder=LevelID-1 FROM GameExpLevel(NOLOCK) WHERE NeedPoint>@Experience
	IF @GameOrder IS NULL
	BEGIN
		-- ��ѯ����
		SELECT @GameOrder=MAX(LevelID) FROM GameExpLevel(NOLOCK)
	END

	RETURN @GameOrder
END

GO

----------------------------------------------------------------------------------------------------
-- ������λ�ȼ�
CREATE FUNCTION [dbo].[PFF_CalcRankOrder]
(
	@RankScore  INT	
)
RETURNS TINYINT
AS
BEGIN
	
	-- ��������
	DECLARE @RankOrder TINYINT			

	-- ��ѯ�ȼ�
	SELECT TOP 1 @RankOrder=LevelID-1 FROM GameRankLevel(NOLOCK) WHERE NeedScore>@RankScore
	IF @RankOrder IS NULL
	BEGIN
		-- ��ѯ����
		SELECT @RankOrder=MAX(LevelID) FROM GameRankLevel(NOLOCK)
	END

	RETURN @RankOrder
END

GO

----------------------------------------------------------------------------------------------------
-- ����������ʶ
CREATE FUNCTION [dbo].[PFF_GetSeasonID]
(	
)
RETURNS TINYINT
AS
BEGIN
	-- ��ѯ����
	DECLARE @RankingOption NVARCHAR(1024)
	SELECT @RankingOption=OptionValue FROM SystemOptionInfo(NOLOCK) WHERE OptionName=N'RankingOption'

	-- У������
	IF ISJSON(@RankingOption)=0
	BEGIN
		RETURN 0
	END

	-- ʱ�䶨��	
	DECLARE @CurrDateTime DATETIME
	DECLARE @SeasonMonths SMALLINT
	DECLARE @SeasonStartTime DATETIME

	-- ����ʱ��
	SET @CurrDateTime=GETDATE()
	SET @SeasonMonths=ISNULL(JSON_VALUE(@RankingOption,N'$.SeasonMonths'),3)
	SET @SeasonStartTime=ISNULL(JSON_VALUE(@RankingOption,N'$.SeasonStartTime'),N'2021-09-13 00:00:00')

	-- ������
	DECLARE @MonthDiff SMALLINT
	SET @MonthDiff=DATEDIFF(mm,@SeasonStartTime,@CurrDateTime)

	-- �����ʶ
	DECLARE @SeasonID SMALLINT
	SET @SeasonID=(@MonthDiff/@SeasonMonths)+1

	RETURN @SeasonID
END

GO

----------------------------------------------------------------------------------------------------