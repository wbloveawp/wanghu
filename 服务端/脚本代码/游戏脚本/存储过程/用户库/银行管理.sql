
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_ModifyInsurePassword]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_ModifyInsurePassword]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_InsureQuery]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_InsureQuery]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_InsureEnable]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_InsureEnable]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_InsureSave]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_InsureSave]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_InsureTake]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_InsureTake]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_InsureTransfer]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_InsureTransfer]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO
----------------------------------------------------------------------------------------------------

-- 银行修改密码
CREATE PROC GSP_MB_ModifyInsurePassword
	@dwUserID INT,								-- 用户 I D
	@strSrcPassword NVARCHAR(32),				-- 用户旧密码
	@strDstPassword NVARCHAR(32),				-- 用户新密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

DECLARE @strInscurePassword NVARCHAR(32)
DECLARE @Score BIGINT
DECLARE @InsureScore BIGINT

BEGIN
	SELECT @strInscurePassword=InsurePass FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
	IF @strInscurePassword <> '' and @strInscurePassword <> @strSrcPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1335,"Msg":"抱歉, 银行密码错误！"}'
		RETURN 1
	END

	UPDATE AccountsInfo SET InsurePass=@strDstPassword WHERE UserID=@dwUserID

	--
	SELECT @Score=Score,@InsureScore=InsureScore FROM LHGameGoldDB..GameScoreInfo WHERE UserID=@dwUserID
	IF @Score IS NULL SET @Score = 0
	IF @InsureScore IS NULL SET @InsureScore = 0

	--
	SELECT @Score AS Score,@InsureScore AS InsureScore
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------

-- 银行查询
CREATE PROC GSP_MB_InsureQuery
	@dwUserID INT								-- 用户 I D

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

DECLARE @strOption NVARCHAR(MAX)
DECLARE @Enable TINYINT

DECLARE @SaveAndTakeReserveAmount BIGINT
DECLARE @SaveAndTakeFeeRate INT
DECLARE @SaveAndTakeDealMin BIGINT
DECLARE @SaveAndTakeDealMax BIGINT
DECLARE @SaveAndTakeDailyMax BIGINT

DECLARE @TransferReserveAmount BIGINT
DECLARE @TransferFeeRate INT
DECLARE @TransferDealMin BIGINT
DECLARE @TransferDealMax BIGINT
DECLARE @TransferDailyMax BIGINT

DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT

BEGIN
	SELECT @SourceScore=Score,@SourceInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	IF @SourceScore IS NULL SET @SourceScore=0
	IF @SourceInsure IS NULL SET @SourceInsure=0

	-- 获取配置项
	SELECT @strOption=StatusValue FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'InsureOption'
	IF ISJSON(@strOption) = 1
	BEGIN
		SELECT @Enable=JSON_VALUE(@strOption,'$.Enable'),
		@SaveAndTakeReserveAmount=JSON_VALUE(@strOption,'$.SaveAndTake.ReserveAmount'),
		@SaveAndTakeFeeRate=JSON_VALUE(@strOption,'$.SaveAndTake.Rate'),@SaveAndTakeDealMin=JSON_VALUE(@strOption,'$.SaveAndTake.DealMin'),
		@SaveAndTakeDealMax=JSON_VALUE(@strOption,'$.SaveAndTake.DealMax'),@SaveAndTakeDailyMax=JSON_VALUE(@strOption,'$.SaveAndTake.DailyMax'),
		@TransferReserveAmount=JSON_VALUE(@strOption,'$.Transfer.ReserveAmount'),
		@TransferFeeRate=JSON_VALUE(@strOption,'$.Transfer.Rate'),@TransferDealMin=JSON_VALUE(@strOption,'$.Transfer.DealMin'),
		@TransferDealMax=JSON_VALUE(@strOption,'$.Transfer.DealMax'),@TransferDailyMax=JSON_VALUE(@strOption,'$.Transfer.DailyMax')
	END
	IF @Enable IS NULL SET @Enable= 0
	IF @SaveAndTakeReserveAmount IS NULL SET @SaveAndTakeReserveAmount=0
	IF @SaveAndTakeFeeRate IS NULL SET @SaveAndTakeFeeRate=0
	IF @SaveAndTakeDealMin IS NULL SET @SaveAndTakeDealMin= 0
	IF @SaveAndTakeDealMax IS NULL SET @SaveAndTakeDealMax= 0
	IF @SaveAndTakeDailyMax IS NULL SET @SaveAndTakeDailyMax= 0

	IF @TransferReserveAmount IS NULL SET @TransferReserveAmount=0
	IF @TransferFeeRate IS NULL SET @TransferFeeRate=0
	IF @TransferDealMin IS NULL SET @TransferDealMin= 0
	IF @TransferDealMax IS NULL SET @TransferDealMax= 0
	IF @TransferDailyMax IS NULL SET @TransferDailyMax= 0

	SELECT @Enable AS Enable,@SourceScore AS Score,@SourceInsure AS InsureScore,
	@SaveAndTakeReserveAmount AS SaveAndTakeReserveAmount,@SaveAndTakeFeeRate AS SaveAndTakeFeeRate,@SaveAndTakeDealMin AS SaveAndTakeDealMin,@SaveAndTakeDealMax AS SaveAndTakeDealMax,@SaveAndTakeDailyMax AS SaveAndTakeDailyMax,
	@TransferReserveAmount AS TransferReserveAmount,@TransferFeeRate AS TransferFeeRate,@TransferDealMin AS TransferDealMin,@TransferDealMax AS TransferDealMax,@TransferDailyMax AS TransferDailyMax
	

END

RETURN 0

GO
----------------------------------------------------------------------------------------------------
-- 银行开通
CREATE PROC GSP_MB_InsureEnable
	@dwUserID INT,								-- 用户 I D
	@strPassword NVARCHAR(32),					-- 用户密码
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT

BEGIN
	-- 查询用户
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	DECLARE @InsurePassword NVARCHAR(33)
	SELECT @UserID=a.UserID,@InsurePassword=a.InsurePass,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
		RETURN 2
	END	

	-- 帐号关闭
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
		RETURN 3
	END	

	-- 更新数据
	UPDATE AccountsInfo SET InsurePass=@strPassword WHERE UserID=@dwUserID

	-- 查询数据
	SELECT @SourceScore=Score,@SourceInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	IF @SourceScore IS NULL SET @SourceScore=0
	IF @SourceInsure IS NULL SET @SourceInsure=0

	SELECT @SourceScore AS Score,@SourceInsure AS InsureScore

END

RETURN 0

GO
----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------

-- 银行存储
CREATE PROC GSP_MB_InsureSave
	@dwUserID INT,								-- 用户 I D
	@nScore BIGINT,								-- 存储金额
	@wReason INT,								-- 变更原因
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器序列
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

DECLARE @strOption NVARCHAR(MAX)
DECLARE @Enable TINYINT
DECLARE @ReserveAmount BIGINT
DECLARE @FeeRate INT
DECLARE @DealMin BIGINT
DECLARE @DealMax BIGINT
DECLARE @DailyMax BIGINT

DECLARE @DateID INT
DECLARE @UserDailyOpScore BIGINT
DECLARE @UserDailyFee BIGINT

DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT

-- 执行逻辑
BEGIN
	-- 查询用户
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	DECLARE @InsurePassword NVARCHAR(33)
	SELECT @UserID=a.UserID,@InsurePassword=a.InsurePass,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
		RETURN 2
	END	

	-- 帐号关闭
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
		RETURN 3
	END	

	-- 校验状态
	IF EXISTS(SELECT 1 FROM AccountsOnlineInfo(NOLOCK) WHERE UserID=@dwUserID AND ServerID<>0)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1330,"Msg":"系统检测到您的账号正在游戏房间中，请先退出房间再进行银行操作！"}'
		RETURN 4
	END

	--查询余额
	SELECT @SourceScore=Score,@SourceInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	IF @SourceScore IS NULL SET @SourceScore=0
	IF @SourceScore < @nScore
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1337,"Msg":"抱歉, 金币不足！"}'
		RETURN 5
	END

	-- 获取配置项
	SELECT @strOption=StatusValue FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'InsureOption'
	IF ISJSON(@strOption) = 1
	BEGIN
		SELECT @Enable=JSON_VALUE(@strOption,'$.Enable'),
		@ReserveAmount=JSON_VALUE(@strOption,'$.SaveAndTake.ReserveAmount'),
		@FeeRate=JSON_VALUE(@strOption,'$.SaveAndTake.Rate'),@DealMin=JSON_VALUE(@strOption,'$.SaveAndTake.DealMin'),
		@DealMax=JSON_VALUE(@strOption,'$.SaveAndTake.DealMax'),@DailyMax=JSON_VALUE(@strOption,'$.SaveAndTake.DailyMax')
	END
	IF @Enable IS NULL SET @Enable= 0
	IF @ReserveAmount IS NULL SET @ReserveAmount=0
	IF @FeeRate IS NULL SET @FeeRate=0
	IF @DealMin IS NULL SET @DealMin= 0
	IF @DealMax IS NULL SET @DealMax= 0
	IF @DailyMax IS NULL SET @DailyMax= 0
	--
	IF @Enable = 0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1331,"Msg":"抱歉，银行功能已关闭！"}'
		RETURN 6
	END
	--
	IF @nScore < @DealMin
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1332,"Msg":"抱歉, 每笔操作金额不能小于%s。","Ops":["'+CONVERT(NVARCHAR(16),@DealMin)+'"]}'	
		RETURN 7
	END
	--
	IF @nScore > @DealMax
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1333,"Msg":"抱歉, 每笔操作金额不能大于%s。","Ops":["'+CONVERT(NVARCHAR(16),@DealMax)+'"]}'	
		RETURN 8
	END
	-- 转换日期
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)	

	-- 查询玩家当日记录
	SELECT @UserDailyOpScore=Score FROM RecordUserInsureTransfer WHERE DateID=@DateID AND UserID=@dwUserID AND Type=0
	IF @UserDailyOpScore IS NULL 
	BEGIN
		INSERT INTO RecordUserInsureTransfer (DateID,UserID,Type,Score,Fee,CollectDate) VALUES(@DateID,@dwUserID,0,0,0,GETDATE())
		SELECT @UserDailyOpScore = 0
	END

	-- 每日存储上限
	IF @UserDailyOpScore + @nScore > @DailyMax
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1334,"Msg":"抱歉, 每日累计操作金额不能大于%s。","Ops":["'+CONVERT(NVARCHAR(16),@DailyMax)+'"]}'	
		RETURN 9
	END

	DECLARE @ChangeScore BIGINT
	SET @ChangeScore=-@nScore
	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 写入财富
	EXEC GSP_GP_WriteGameWealth @dwUserID,1,@wReason,@ChangeScore 
	-- 银行分数
	UPDATE LHGameGoldDB.dbo.GameScoreInfo SET InsureScore=InsureScore+@nScore WHERE UserID=@dwUserID

	-- 写银行记录
	INSERT INTO LHGameRecordDB.dbo.RecordInsure (SourceUserID,SourceScore,SourceInsure,TargetUserID,TargetScore,TargetInsure,OperateScore,OperateFee,OperateType,CientIP,CollectDate) 
	VALUES(@dwUserID,@SourceScore,@SourceInsure,0,0,0,@nScore,0,0,@strClientIP,GETDATE())

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 更新玩家当日记录
	UPDATE RecordUserInsureTransfer SET Score=Score+@nScore,CollectDate=GETDATE() WHERE DateID=@DateID AND UserID=@dwUserID AND Type=0

	--
	SELECT @SourceScore=Score,@SourceInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID

	--
	SELECT @SourceScore AS Score,@SourceInsure AS InsureScore
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------

-- 银行提取
CREATE PROC GSP_MB_InsureTake
	@dwUserID INT,								-- 用户 I D
	@nScore BIGINT,								-- 提取金额
	@strPassword NVARCHAR(32),					-- 用户密码
	@wReason INT,								-- 变更原因
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器序列
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

DECLARE @strOption NVARCHAR(MAX)
DECLARE @Enable TINYINT
DECLARE @ReserveAmount BIGINT
DECLARE @FeeRate INT
DECLARE @DealMin BIGINT
DECLARE @DealMax BIGINT
DECLARE @DailyMax BIGINT

DECLARE @DateID INT
DECLARE @UserDailyOpScore BIGINT
DECLARE @UserDailyFee BIGINT

DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT

-- 执行逻辑
BEGIN
	-- 查询用户
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	DECLARE @InsurePassword NVARCHAR(33)
	SELECT @UserID=a.UserID,@InsurePassword=a.InsurePass,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
		RETURN 2
	END	

	-- 帐号关闭
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
		RETURN 3
	END	

	-- 校验状态
	IF EXISTS(SELECT 1 FROM AccountsOnlineInfo(NOLOCK) WHERE UserID=@dwUserID AND ServerID<>0)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1330,"Msg":"系统检测到您的账号正在游戏房间中，请先退出房间再进行银行操作！"}'
		RETURN 4
	END

	-- 校验密码
	IF @strPassword<>@InsurePassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1335,"Msg":"抱歉, 银行密码错误！"}'
		RETURN 5
	END

	--查询余额
	SELECT @SourceScore=Score, @SourceInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	IF @SourceInsure IS NULL SET @SourceInsure=0
	IF @SourceInsure < @nScore
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1336,"Msg":"抱歉, 银行余额不足！"}'
		RETURN 6
	END

	-- 获取配置项
	SELECT @strOption=StatusValue FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'InsureOption'
	IF ISJSON(@strOption) = 1
	BEGIN
		SELECT @Enable=JSON_VALUE(@strOption,'$.Enable'),
		@ReserveAmount=JSON_VALUE(@strOption,'$.SaveAndTake.ReserveAmount'),
		@FeeRate=JSON_VALUE(@strOption,'$.SaveAndTake.Rate'),@DealMin=JSON_VALUE(@strOption,'$.SaveAndTake.DealMin'),
		@DealMax=JSON_VALUE(@strOption,'$.SaveAndTake.DealMax'),@DailyMax=JSON_VALUE(@strOption,'$.SaveAndTake.DailyMax')
	END
	IF @Enable IS NULL SET @Enable= 0
	IF @ReserveAmount IS NULL SET @ReserveAmount=0
	IF @FeeRate IS NULL SET @FeeRate=0
	IF @DealMin IS NULL SET @DealMin= 0
	IF @DealMax IS NULL SET @DealMax= 0
	IF @DailyMax IS NULL SET @DailyMax= 0
	--
	IF @Enable = 0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1331,"Msg":"抱歉，银行功能已关闭！"}'
		RETURN 7
	END
	--
	IF @nScore < @DealMin
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1332,"Msg":"抱歉, 每笔操作金额不能小于%s。","Ops":["'+CONVERT(NVARCHAR(16),@DealMin)+'"]}'	
		RETURN 8
	END
	--
	IF @nScore > @DealMax
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1333,"Msg":"抱歉, 每笔操作金额不能大于%s。","Ops":["'+CONVERT(NVARCHAR(16),@DealMax)+'"]}'	
		RETURN 9
	END

	-- 转换日期
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)	

	-- 计算服务费
	DECLARE @CurFee BIGINT 
	SELECT @CurFee=@nScore*@FeeRate/1000
	IF @CurFee < 0 SET @CurFee=0

	DECLARE @ChangeScore BIGINT
	SET @ChangeScore=(@nScore-@CurFee)
	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 写入财富
	EXEC GSP_GP_WriteGameWealth @dwUserID,1,@wReason,@ChangeScore 
	-- 银行分数
	UPDATE LHGameGoldDB.dbo.GameScoreInfo SET InsureScore=InsureScore-@nScore WHERE UserID=@dwUserID

	-- 写银行记录
	INSERT INTO LHGameRecordDB..RecordInsure (SourceUserID,SourceScore,SourceInsure,TargetUserID,TargetScore,TargetInsure,OperateScore,OperateFee,OperateType,CientIP,CollectDate) 
	VALUES(@dwUserID,@SourceScore,@SourceInsure,0,0,0,@nScore,@CurFee,1,@strClientIP,GETDATE())

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	--
	SELECT @SourceScore=Score,@SourceInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID

	--
	SELECT @SourceScore AS Score,@SourceInsure AS InsureScore

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------

-- 银行转账
CREATE PROC GSP_MB_InsureTransfer
	@dwUserID INT,								-- 用户 I D
	@dwGameID INT,								-- 目标用户
	@nScore BIGINT,								-- 提取金额
	@strPassword NVARCHAR(32),					-- 用户密码
	@wReason INT,								-- 变更原因
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器序列
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

DECLARE @strOption NVARCHAR(MAX)
DECLARE @Enable TINYINT
DECLARE @ReserveAmount BIGINT
DECLARE @FeeRate INT
DECLARE @DealMin BIGINT
DECLARE @DealMax BIGINT
DECLARE @DailyMax BIGINT

DECLARE @DateID INT
DECLARE @UserDailyOpScore BIGINT
DECLARE @UserDailyFee BIGINT
DECLARE @UserInsureScore BIGINT

DECLARE @NickName NVARCHAR(32)
DECLARE @SourceGameID INT
DECLARE @TargetUserID INT
DECLARE @TargetNickName NVARCHAR(32)
DECLARE @TargetScore BIGINT
DECLARE @TargetInsure BIGINT

DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT

-- 执行逻辑
BEGIN
	-- 查询用户
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	DECLARE @InsurePassword NVARCHAR(33)
	SELECT @UserID=a.UserID,@SourceGameID=a.GameID,@InsurePassword=a.InsurePass,@NickName=NickName,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
		RETURN 2
	END	

	-- 帐号关闭
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
		RETURN 3
	END	

	-- 目标用户
	SELECT @TargetUserID=a.UserID,@TargetNickName=a.NickName,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.GameID=@dwGameID

	-- 校验用户
	IF @TargetUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1036,"Msg":"抱歉,目标帐号不存在。"}'
		RETURN 4
	END	

	-- 校验用户
	IF @TargetUserID = @dwUserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1037,"Msg":"抱歉,同一帐号不允许进行金币转账。"}'
		RETURN 5
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1038,"Msg":"抱歉！目标帐号暂时处于冻结状态，无法进行金币转账。"}'
		RETURN 6
	END	

	-- 帐号关闭
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1039,"Msg":"抱歉！目标帐号使用了安全关闭功能，无法进行金币转账。"}'
		RETURN 7
	END		

	-- 校验状态
	IF EXISTS(SELECT 1 FROM AccountsOnlineInfo(NOLOCK) WHERE UserID=@dwUserID AND ServerID<>0)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1330,"Msg":"系统检测到您的账号正在游戏房间中，请先退出房间再进行银行操作！"}'
		RETURN 8
	END

	-- 校验密码
	IF @strPassword<>@InsurePassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1335,"Msg":"抱歉, 银行密码错误！"}'
		RETURN 9
	END

	-- 获取配置项
	SELECT @strOption=StatusValue FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'InsureOption'
	IF ISJSON(@strOption) = 1
	BEGIN
			SELECT @Enable=JSON_VALUE(@strOption,'$.Enable'),
		@ReserveAmount=JSON_VALUE(@strOption,'$.Transfer.ReserveAmount'),
		@FeeRate=JSON_VALUE(@strOption,'$.Transfer.Rate'),@DealMin=JSON_VALUE(@strOption,'$.Transfer.DealMin'),
		@DealMax=JSON_VALUE(@strOption,'$.Transfer.DealMax'),@DailyMax=JSON_VALUE(@strOption,'$.Transfer.DailyMax')
	END
	IF @Enable IS NULL SET @Enable= 0
	IF @ReserveAmount IS NULL SET @ReserveAmount=0
	IF @FeeRate IS NULL SET @FeeRate=0
	IF @DealMin IS NULL SET @DealMin= 0
	IF @DealMax IS NULL SET @DealMax= 0
	IF @DailyMax IS NULL SET @DailyMax= 0
	--
	IF @Enable = 0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1331,"Msg":"抱歉，银行功能已关闭！"}'
		RETURN 10
	END
	--
	IF @nScore < @DealMin
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1332,"Msg":"抱歉, 每笔操作金额不能小于%s。","Ops":["'+CONVERT(NVARCHAR(16),@DealMin)+'"]}'	
		RETURN 11
	END
	--
	IF @nScore > @DealMax
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1333,"Msg":"抱歉, 每笔操作金额不能大于%s。","Ops":["'+CONVERT(NVARCHAR(16),@DealMax)+'"]}'	
		RETURN 12
	END
	-- 转换日期
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)	

	-- 查询玩家当日记录
	SELECT @UserDailyOpScore=Score FROM RecordUserInsureTransfer WHERE DateID=@DateID AND UserID=@dwUserID AND Type=2
	IF @UserDailyOpScore IS NULL 
	BEGIN
		INSERT INTO RecordUserInsureTransfer (DateID,UserID,Type,Score,Fee,CollectDate) VALUES(@DateID,@dwUserID,2,0,0,GETDATE())
		SELECT @UserDailyOpScore = 0
	END

	-- 每日存储上限
	IF @UserDailyOpScore + @nScore > @DailyMax
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1334,"Msg":"抱歉, 每日累计操作金额不能大于%s。","Ops":["'+CONVERT(NVARCHAR(16),@DailyMax)+'"]}'	
		RETURN 9
	END

	-- 校验余额
	SELECT @UserInsureScore=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	IF @UserInsureScore IS NULL SET @UserInsureScore=0
	IF @UserInsureScore-@ReserveAmount < @nScore
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1336,"Msg":"抱歉, 银行余额不足！"}'
		RETURN 13
	END

	-- 转换日期
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)	

	-- 计算服务费
	DECLARE @CurFee BIGINT 
	SELECT @CurFee=@nScore*@FeeRate/1000

	DECLARE @ChangeScore BIGINT
	SET @ChangeScore=(@nScore-@CurFee)

	-- 接收方邮件
	-- 变量定义
	DECLARE @MailID INT
	DECLARE @MailTitle NVARCHAR(128)
	DECLARE @MailContent NVARCHAR(512)

	-- 邮件内容
	SET @MailTitle =N'金币转账通知'
	--SET @MailContent=N'{"Ops":["'+@TargetNickName+'","'+@NickName+'","'+CONVERT(NVARCHAR(18),@SourceGameID)+'","'+CONVERT(NVARCHAR(16),@ChangeScore)+'","'+ CONVERT(VARCHAR(100),GETDATE(),109)+'"]}'
	SET @MailContent=N'您好,'+@NickName+'('+CONVERT(NVARCHAR(16),@SourceGameID)+N') 玩家 '+@TargetNickName+'('+CONVERT(NVARCHAR(16),@dwGameID)+N') 转账'+CONVERT(NVARCHAR(16),@ChangeScore)+N'铜钱给您.'
	
	-- 插入邮件信息
	INSERT INTO AccountsMailInfo(MailKind,MailType,MailTitle,MailContent,SendTime,ExpireTime,CollectTime)
	VALUES (1,0,@MailTitle,@MailContent,dbo.LHF_GetTimestamp(),0,GetDate())

	-- 邮件标识
	SET @MailID=SCOPE_IDENTITY()	

	-- 查询分数
	SELECT @SourceScore=Score,@SourceInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	SELECT @TargetScore=Score,@TargetInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@TargetUserID

	IF @SourceScore IS NULL SET @SourceScore = 0
	IF @SourceInsure IS NULL SET @SourceInsure = 0
	IF @TargetScore IS NULL SET @TargetScore = 0
	IF @TargetInsure IS NULL SET @TargetInsure = 0

	-- 插入附件
	--INSERT INTO AccountsMailAttach(MailID,GoodsID,GoodsCount,GoodsIndate) VALUES (@MailID,1,@ChangeScore,0)			

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 转出方
	UPDATE LHGameGoldDB.dbo.GameScoreInfo SET InsureScore=InsureScore-@nScore WHERE UserID=@dwUserID

	-- 接收方
	UPDATE LHGameGoldDB.dbo.GameScoreInfo SET InsureScore=InsureScore+@ChangeScore WHERE UserID=@TargetUserID

	-- 写银行记录
	INSERT INTO LHGameRecordDB..RecordInsure (SourceUserID,SourceScore,SourceInsure,TargetUserID,TargetScore,TargetInsure,OperateScore,OperateFee,OperateType,CientIP,CollectDate) 
	VALUES(@dwUserID,@SourceScore,@SourceInsure,@TargetUserID,@TargetScore,@TargetInsure+@ChangeScore,@nScore,@CurFee,2,@strClientIP,GETDATE())

	-- 插入用户邮箱
	INSERT INTO AccountsMailBox(UserID,MailID,MailState,ExpireTime,UpdateTime) VALUES (@TargetUserID,@MailID,1,0,GetDate())


	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 更新玩家当日记录
	UPDATE RecordUserInsureTransfer SET Score=Score+@nScore,Fee=Fee+@CurFee ,CollectDate=GETDATE() WHERE DateID=@DateID AND UserID=@dwUserID AND Type=2

	--
	SELECT @SourceScore=Score,@SourceInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID

	--
	SELECT @SourceScore AS Score,@SourceInsure AS InsureScore,@TargetUserID AS TargetUserID,@MailID AS TargetMailID

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
