
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadBattleOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadBattleOption]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadClubOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadClubOption]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadClubKindOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadClubKindOption]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadClubForbidWords]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadClubForbidWords]

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 加载配置
CREATE  PROCEDURE dbo.GSP_GR_LoadBattleOption			
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN	
	-- 桌数配置
	DECLARE @ClubMaxTableCount SMAlLINT			
	DECLARE @NormalMaxTableCount SMAlLINT			

	-- 查询配置
	SELECT @ClubMaxTableCount=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'BattleOption' AND StatusName='ClubMaxTableCount'
	SELECT @NormalMaxTableCount=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'BattleOption' AND StatusName='NormalMaxTableCount'
	
	-- 调整变量 
	IF @ClubMaxTableCount IS NULL SET @ClubMaxTableCount=10
	IF @NormalMaxTableCount IS NULL SET @NormalMaxTableCount=5
	
	-- 抛出数据
	SELECT @ClubMaxTableCount AS ClubMaxTableCount,@NormalMaxTableCount AS NormalMaxTableCount
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载配置
CREATE  PROCEDURE dbo.GSP_GR_LoadClubOption			
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN	
	-- 变量定义
	DECLARE @SupportClubKind SMAlLINT
	DECLARE @MaxJoinClubCount SMAlLINT
	DECLARE @MaxCreateClubCount SMAlLINT
	DECLARE @MaxClubMemberCount SMAlLINT
	DECLARE @MaxBattleConfigCount SMAlLINT	

	-- 查询配置
	SELECT @SupportClubKind=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'ClubOption' AND StatusName='SupportClubKind'
	SELECT @MaxClubMemberCount=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'ClubOption' AND StatusName='MaxMemberCount'
	SELECT @MaxBattleConfigCount=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'ClubOption' AND StatusName='MaxConfigCount'
	SELECT @MaxJoinClubCount=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'ClubOption' AND StatusName='MaxJoinClubCount'
	SELECT @MaxCreateClubCount=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'ClubOption' AND StatusName='MaxCreateClubCount'	
	
	-- 调整变量 
	IF @SupportClubKind IS NULL SET @SupportClubKind=3
	IF @MaxJoinClubCount IS NULL SET @MaxJoinClubCount=10
	IF @MaxCreateClubCount IS NULL SET @MaxCreateClubCount=5
	IF @MaxClubMemberCount IS NULL SET @MaxClubMemberCount=500
	IF @MaxBattleConfigCount IS NULL SET @MaxBattleConfigCount=10
		
	-- 抛出数据
	SELECT @SupportClubKind AS SupportClubKind,@MaxJoinClubCount AS MaxJoinClubCount, @MaxCreateClubCount AS MaxCreateClubCount, @MaxBattleConfigCount AS MaxBattleConfigCount,@MaxClubMemberCount AS MaxClubMemberCount
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载配置
CREATE  PROCEDURE dbo.GSP_GR_LoadClubKindOption		
	@wClubKind SMALLINT							
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN	
	-- 变量定义
	DECLARE @PayTypeMask SMALLINT
	DECLARE @CurrencyKindMask SMALLINT
	DECLARE @MinRevenueRate SMALLINT
	DECLARE @MaxRevenueRate SMALLINT
	DECLARE @CreateClubTakeCard INT
	DECLARE @CreateClubDeductCard INT
	DECLARE @CreateNeedUserRight INT
	DECLARE @ModifyClubNameDeductCard INT
	
	-- 群组类型
	DECLARE @strClubOptionName NVARCHAR(32)

	-- 选项名称
	IF @wClubKind=0 SET @strClubOptionName=N'ClubOption_Gold'
	IF @wClubKind=1 SET @strClubOptionName=N'ClubOption_Normal'
	IF @wClubKind=2 SET @strClubOptionName=N'ClubOption_Advanced'

	-- 查询配置
	SELECT @PayTypeMask=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='PayTypeMask'
	SELECT @CurrencyKindMask=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='CurrencyKindMask'	
	SELECT @MinRevenueRate=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='MinRevenueRate'	
	SELECT @MaxRevenueRate=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='MaxRevenueRate'	
	SELECT @CreateClubTakeCard=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='CreateClubTakeCard'	
	SELECT @CreateClubDeductCard=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='CreateClubDeductCard'	
	SELECT @CreateNeedUserRight=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='CreateNeedUserRight'		
	SELECT @ModifyClubNameDeductCard=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='ModifyClubNameDeductCard'	
	
	-- 调整变量 
	IF @PayTypeMask IS NULL SET @PayTypeMask=0
	IF @CurrencyKindMask IS NULL SET @CurrencyKindMask=0
	IF @MinRevenueRate IS NULL SET @MinRevenueRate=0
	IF @MaxRevenueRate IS NULL SET @MaxRevenueRate=100
	IF @CreateClubTakeCard IS NULL SET @CreateClubTakeCard=0
	IF @CreateClubDeductCard IS NULL SET @CreateClubDeductCard=0
	IF @ModifyClubNameDeductCard IS NULL SET @ModifyClubNameDeductCard=0
		
	-- 抛出数据
	SELECT @PayTypeMask AS PayTypeMask, @CurrencyKindMask AS CurrencyKindMask,@MinRevenueRate AS MinRevenueRate, @MaxRevenueRate AS MaxRevenueRate, @CreateClubTakeCard AS CreateClubTakeCard, @CreateClubDeductCard AS CreateClubDeductCard, @CreateNeedUserRight AS CreateNeedUserRight, 
		   @ModifyClubNameDeductCard AS ModifyClubNameDeductCard
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载配置
CREATE  PROCEDURE dbo.GSP_GR_LoadClubForbidWords	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN	

	-- 查询配置
	SELECT ForbidWords FROM ClubForbidWords(NOLOCK)

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------