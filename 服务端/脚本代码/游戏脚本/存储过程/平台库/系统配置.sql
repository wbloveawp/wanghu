
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadSubsidyParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadSubsidyParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadControlParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadControlParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadCheckInParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadCheckInParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadTransferParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadTransferParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadTurntableParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadTurntableParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadCommissionParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadCommissionParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadSuperPlayerControl]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadSuperPlayerControl]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 加载低保配置
CREATE  PROCEDURE dbo.GSP_LoadSubsidyParameter
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 低保配置
	DECLARE @TakeSubsidyGold BIGINT	
	DECLARE @TakeSubsidyCondition BIGINT	
	DECLARE @EnableSubsidy TINYINT

	-- 读取配置		
	SELECT @TakeSubsidyGold=CONVERT(BIGINT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'SubsidyOption' AND StatusName=N'SubsidyGold'
	SELECT @TakeSubsidyCondition=CONVERT(BIGINT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'SubsidyOption' AND StatusName=N'SubsidyCondition'	
	SELECT @EnableSubsidy=CONVERT(BIGINT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'SubsidyOption' AND StatusName=N'EnjoinSubsidy'	

	-- 调整配置	
	IF @TakeSubsidyGold IS NULL SET @TakeSubsidyGold=400000
	IF @TakeSubsidyCondition IS NULL SET @TakeSubsidyCondition=400000	
	IF @EnableSubsidy IS NULL SET @EnableSubsidy = 1
	IF @EnableSubsidy > 0 
	BEGIN
		SET @TakeSubsidyGold = 0
		SET @TakeSubsidyCondition = 0
	END

	-- 输出数据
	SELECT @TakeSubsidyGold AS TakeSubsidyGold, @TakeSubsidyCondition AS TakeSubsidyCondition
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 加载控制配置
CREATE  PROCEDURE dbo.GSP_LoadControlParameter
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 控制参数
	DECLARE @ControlEnabled TINYINT
	DECLARE	@BenchmarkValue INT	
	DECLARE	@StartPlayCount INT
	DECLARE @WLControlRate SMALLINT
	DECLARe @BLControlRate SMALLINT
	DECLARE @InWhiteListRate SMALLINT
	DECLARE @InBlackListRate SMALLINT
	DECLARE @OutWhiteListRate SMALLINT
	DECLARE @OutBlackListRate SMALLINT
	DECLARE	@SupportControlMode SMALLINT

	-- 读取配置	
	SELECT @ControlEnabled=CONVERT(TINYINT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'ControlOption' AND StatusName=N'ControlEnabled'
	SELECT @BenchmarkValue=CONVERT(INT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'ControlOption' AND StatusName=N'BenchmarkValue'
	SELECT @StartPlayCount=CONVERT(INT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'ControlOption' AND StatusName=N'StartPlayCount'
	SELECT @WLControlRate=CONVERT(SMALLINT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'ControlOption' AND StatusName=N'WLControlRate'
	SELECT @BLControlRate=CONVERT(SMALLINT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'ControlOption' AND StatusName=N'BLControlRate'
	SELECT @InWhiteListRate=CONVERT(SMALLINT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'ControlOption' AND StatusName=N'InWhiteListRate'
	SELECT @InBlackListRate=CONVERT(SMALLINT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'ControlOption' AND StatusName=N'InBlackListRate'	
	SELECT @OutWhiteListRate=CONVERT(SMALLINT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'ControlOption' AND StatusName=N'OutWhiteListRate'
	SELECT @OutBlackListRate=CONVERT(SMALLINT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'ControlOption' AND StatusName=N'OutBlackListRate'
	SELECT @SupportControlMode=CONVERT(SMALLINT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'ControlOption' AND StatusName=N'SupportControlMode'	

	-- 控制参数
	IF @ControlEnabled IS NULL SET @ControlEnabled=0
	IF @StartPlayCount IS NULL SET @StartPlayCount=20
	IF @BenchmarkValue IS NULL SET @BenchmarkValue=100000
	IF @WLControlRate IS NULL SET @WLControlRate=70
	IF @BLControlRate IS NULL SET @BLControlRate=30
	IF @InWhiteListRate IS NULL SET @InWhiteListRate=35
	IF @InBlackListRate IS NULL SET @InBlackListRate=70
	IF @OutWhiteListRate IS NULL SET @OutWhiteListRate=55
	IF @OutBlackListRate IS NULL SET @OutBlackListRate=55
	IF @SupportControlMode IS NULL SET @SupportControlMode=1

	-- 输出数据
	SELECT @ControlEnabled AS ControlEnabled, @BenchmarkValue AS BenchmarkValue, @StartPlayCount AS StartPlayCount, @WLControlRate AS WLControlRate, @BLControlRate AS BLControlRate, @InWhiteListRate AS InWhiteListRate, 
		   @InBlackListRate AS InBlackListRate, @OutWhiteListRate AS OutWhiteListRate, @OutBlackListRate AS OutBlackListRate, @SupportControlMode AS SupportControlMode
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 加载签到参数
CREATE  PROCEDURE dbo.GSP_LoadCheckInParameter
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 转账配置
	DECLARE @CheckInEnabled TINYINT
	DECLARE @CheckInRewards NVARCHAR(MAX)
	
	-- 读取配置		
	SELECT @CheckInRewards=StatusValue FROM SystemStatusInfo WHERE StatusKind=N'CheckInOption' AND StatusName=N'CheckInRewards'	
	SELECT @CheckInEnabled=CONVERT(BIGINT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'CheckInOption' AND StatusName=N'CheckInEnabled'	
	
	-- 调整配置	
	IF @CheckInEnabled IS NULL SET @CheckInEnabled=0
	IF @CheckInRewards IS NULL SET @CheckInRewards=N'[]'
	
	-- 构造配置
	DECLARE @CheckInOption NVARCHAR(MAX)=N'{}'
	SET @CheckInOption = JSON_MODIFY(@CheckInOption,N'$.CheckInEnabled',@CheckInEnabled);
	SET @CheckInOption = JSON_MODIFY(@CheckInOption,N'$.CheckInRewards',@CheckInRewards);

	-- 抛出数据
	SELECT @CheckInOption AS CheckInOption	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载转账配置
CREATE  PROCEDURE dbo.GSP_LoadTransferParameter
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 转账配置
	DECLARE @TransferEnabled TINYINT
	DECLARE @ServiceFeeRate SMALLINT
	DECLARE @ReservedAmount INT
	DECLARE @MinTransferAmount INT
	DECLARE @MaxTransferAmount INT
	
	-- 读取配置		
	SELECT @TransferEnabled=CONVERT(BIGINT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'TransferOption' AND StatusName=N'TransferEnabled'
	SELECT @ServiceFeeRate=CONVERT(BIGINT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'TransferOption' AND StatusName=N'ServiceFeeRate'	
	SELECT @ReservedAmount=CONVERT(BIGINT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'TransferOption' AND StatusName=N'ReservedAmount'	
	SELECT @MinTransferAmount=CONVERT(BIGINT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'TransferOption' AND StatusName=N'MinTransferAmount'	
	SELECT @MaxTransferAmount=CONVERT(BIGINT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'TransferOption' AND StatusName=N'MaxTransferAmount'	

	-- 调整配置	
	IF @TransferEnabled IS NULL SET @TransferEnabled=1
	IF @ServiceFeeRate IS NULL SET @ServiceFeeRate=30
	IF @ReservedAmount IS NULL SET @ReservedAmount=10000
	IF @MinTransferAmount IS NULL SET @MinTransferAmount=10000
	IF @MaxTransferAmount IS NULL SET @MaxTransferAmount=100000000

	-- 输出数据
	SELECT @TransferEnabled AS TransferEnabled, @ServiceFeeRate AS ServiceFeeRate, @ReservedAmount AS ReservedAmount, @MinTransferAmount AS MinTransferAmount, @MaxTransferAmount AS MaxTransferAmount
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
-- 加载转盘选项
CREATE PROC GSP_LoadTurntableParameter
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN	
	-- 查询抽奖
	DECLARE @TuantableEnabled TINYINT
	SELECT @TuantableEnabled=CONVERT(TINYINT,StatusValue) FROM SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TurntableOption' AND StatusName=N'TurantableEnabled'

	-- 调整开关
	IF @TuantableEnabled IS NULL SET @TuantableEnabled=0
		
	-- 转盘奖项
	DECLARE @TurntableAwards NVARCHAR(MAX)	
	SET @TurntableAwards = (SELECT ItemIndex,AwardName,GoodsID,GoodsCount,GoodsLevel,GoodsIndate FROM TurntableAwardConfig FOR JSON AUTO)

	-- 构造配置
	DECLARE @TurntableOption NVARCHAR(MAX)=N'{}'
	SET @TurntableOption = JSON_MODIFY(@TurntableOption,N'$.TurntableEnabled',@TuantableEnabled);
	SET @TurntableOption = JSON_MODIFY(@TurntableOption,N'$.TurntableAwards',@TurntableAwards);

	-- 抛出数据
	SELECT @TurntableOption AS TurntableOption
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 加载佣金配置
CREATE  PROCEDURE dbo.GSP_LoadCommissionParameter
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 转账配置
	DECLARE @DrawEnabled TINYINT
	DECLARE @MinDrawAmount INT
	DECLARE @ServiceFeeRate SMALLINT	
	
	-- 读取配置		
	SELECT @DrawEnabled=CONVERT(BIGINT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'CommissionOption' AND StatusName=N'DrawEnabled'
	SELECT @MinDrawAmount=CONVERT(BIGINT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'CommissionOption' AND StatusName=N'MinDrawAmount'	
	SELECT @ServiceFeeRate=CONVERT(BIGINT,StatusValue) FROM SystemStatusInfo WHERE StatusKind=N'CommissionOption' AND StatusName=N'ServiceFeeRate'			

	-- 调整配置	
	IF @DrawEnabled IS NULL SET @DrawEnabled=1
	IF @ServiceFeeRate IS NULL SET @ServiceFeeRate=30
	IF @MinDrawAmount IS NULL SET @MinDrawAmount=10000	

	-- 输出数据
	SELECT @DrawEnabled AS DrawEnabled, @ServiceFeeRate AS ServiceFeeRate, @MinDrawAmount AS MinDrawAmount
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载大R控制
CREATE  PROCEDURE dbo.GSP_LoadSuperPlayerControl
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 控制属性
	DECLARE @EnabledState TINYINT
	DECLARE @ShrinkOption NVARCHAR(256)
	DECLARE @SuperPlayerControl NVARCHAR(512)
	
	-- 读取配置		
	SELECT @SuperPlayerControl=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusKind=N'StockControlOption' AND StatusName=N'SuperPlayerControl'
	
	-- 调整配置	
	IF ISJSON(@SuperPlayerControl)>0
	BEGIN
		SET @EnabledState=JSON_VALUE(@SuperPlayerControl,N'$.EnabledState')
		SET @ShrinkOption=JSON_QUERY(@SuperPlayerControl,N'$.ShrinkOption')
	END

	-- 调整变量
	IF @EnabledState IS NULL SET @EnabledState=0
	IF @ShrinkOption IS NULL SET @ShrinkOption=N'[]'

	-- 输出数据
	SELECT @EnabledState AS EnabledState, @ShrinkOption AS ShrinkOption
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
