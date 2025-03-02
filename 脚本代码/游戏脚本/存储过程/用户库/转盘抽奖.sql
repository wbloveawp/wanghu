----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryTurntableLottery]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryTurntableLottery]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_RequestTurntableLottery]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_RequestTurntableLottery]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- 转盘抽奖
CREATE PROC GSP_GP_QueryTurntableLottery
	@dwUserID INT,							-- 用户标识
	@dwRecordID INT,						-- 记录标识
	@strClientIP NVARCHAR(16),				-- 客户地址	
	@strMachineID NVARCHAR(32)				-- 机器标识	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 自动回滚
SET XACT_ABORT ON

-- 执行逻辑
BEGIN	
	-- 日期标识
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 查询数据
	DECLARE @MayLotteryTimes INT
	DECLARE @UseLotteryTimes INT
	DECLARE @PayLotteryTimes INT
	DECLARE @FreeLotteryTimes INT
	DECLARE @GameTimeLotteryTimes INT
	DECLARE @GameCountLotteryTimes INT
	DECLARE @GameExpendLotteryTimes INT
	
	-- 查询配置
	SELECT @MayLotteryTimes=MayLotteryTimes,@UseLotteryTimes=UseLotteryTimes,@PayLotteryTimes=PayLotteryTimes,@FreeLotteryTimes=FreeLotteryTimes,@GameTimeLotteryTimes=GameTimeLotteryTimes,
			@GameCountLotteryTimes=GameCountLotteryTimes,@GameExpendLotteryTimes=GameExpendLotteryTimes
	FROM AccountsTurntableLottery(NOLOCK) WHERE DateID=@DateID AND UserID=@dwUserID

	-- 读取配置
	DECLARE @DailyMayLotteryTimes INT
	SELECT @DailyMayLotteryTimes=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TurntableOption' AND StatusName=N'DailyMayLotteryTimes'

	-- 调整配置
	IF @DailyMayLotteryTimes IS NULL SET @DailyMayLotteryTimes=3

	-- 校验配置
	IF @MayLotteryTimes IS NULL 
	BEGIN

		-- 调整配置
		SELECT @MayLotteryTimes=0,@UseLotteryTimes=0,@PayLotteryTimes=0,@FreeLotteryTimes=0,@GameTimeLotteryTimes=0,@GameCountLotteryTimes=0,@GameExpendLotteryTimes=0

		-- 限制配置
		DECLARE @ConfineMachine TINYINT
		DECLARE @ConfineClientIP TINYINT
		DECLARE @DailyFreeLotteryTimes SMALLINT

		-- 读取配置
		SELECT @ConfineMachine=CONVERT(TINYINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TurntableOption' AND StatusName=N'ConfineMachine'	
		SELECT @ConfineClientIP=CONVERT(TINYINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TurntableOption' AND StatusName=N'ConfineClientIP'
		SELECT @DailyFreeLotteryTimes=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TurntableOption' AND StatusName=N'DailyFreeLotteryTimes'

		-- 调整配置
		IF @ConfineMachine IS NULL SET @ConfineMachine=0
		IF @ConfineClientIP IS NULL SET @ConfineClientIP=0
		IF @DailyFreeLotteryTimes IS NULL SET @DailyFreeLotteryTimes=3

		-- 免费次数
		SET @FreeLotteryTimes=@DailyFreeLotteryTimes		
		
		-- 限制机器
		IF @ConfineMachine<>0
		BEGIN
			IF EXISTS(SELECT 1 FROM RecordTurntableMachine(NOLOCK) WHERE DateID=@DateID AND MachineID=@strMachineID)
			BEGIN
				SET @FreeLotteryTimes=0
			END ELSE
			BEGIN
				INSERT INTO RecordTurntableMachine(DateID,MachineID,CollectDate) VALUES(@DateID,@strMachineID,GETDATE())
			END
		END

		-- 限制地址
		IF @ConfineClientIP<>0
		BEGIN
			IF EXISTS(SELECT 1 FROM RecordTurntableClientIP(NOLOCK) WHERE DateID=@DateID AND ClientIP=@strClientIP)
			BEGIN
				SET @FreeLotteryTimes=0
			END ELSE
			BEGIN
				INSERT INTO RecordTurntableClientIP(DateID,ClientIP,CollectDate) VALUES(@DateID,@strClientIP,GETDATE())
			END
		END

		-- 更新总次数
		SET @MayLotteryTimes=@FreeLotteryTimes

		-- 调整次数 
		IF @MayLotteryTimes>=@DailyMayLotteryTimes SET @MayLotteryTimes=@DailyMayLotteryTimes

		-- 插入数据
		INSERT INTO AccountsTurntableLottery(DateID,UserID,MayLotteryTimes,UseLotteryTimes,PayLotteryTimes,FreeLotteryTimes,GameTimeLotteryTimes,GameCountLotteryTimes,GameExpendLotteryTimes,CollectDateTime)
		VALUES(@DateID,@dwUserID,@MayLotteryTimes,@UseLotteryTimes,@PayLotteryTimes,@FreeLotteryTimes,@GameTimeLotteryTimes,@GameCountLotteryTimes,@GameExpendLotteryTimes,GETDATE())
	END 

	-- 换算配置
	DECLARE @SliceGameTime INT
	DECLARE @SliceGameCount INT
	DECLARE @SliceGameExpend BIGINT	

	-- 读取配置
	SELECT @SliceGameTime=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TurntableOption' AND StatusName=N'SliceGameTime'	
	SELECT @SliceGameCount=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TurntableOption' AND StatusName=N'SliceGameCount'
	SELECT @SliceGameExpend=CONVERT(BIGINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TurntableOption' AND StatusName=N'SliceGameExpend'	

	-- 调整变量
	IF @SliceGameTime IS NULL SET @SliceGameTime=0
	IF @SliceGameCount IS NULL SET @SliceGameCount=0
	IF @SliceGameExpend IS NULL SET @SliceGameExpend=0	

	-- 游戏信息
	DECLARE @CurrGameTime INT
	DECLARE @CurrGameCount INT	
	DECLARE @CurrGameExpend BIGINT	

	-- 查询数据
	SELECT @CurrGameExpend=-LoseScore,@CurrGameCount=WinCount+LostCount,@CurrGameTime=PlayTimeCount FROM LHGameGoldDB.dbo.ReportUserScore(NOLOCK) WHERE DateID=@DateID AND UserID=@dwUserID

	-- 校验结果
	IF @CurrGameExpend IS NOT NULL
	BEGIN				

		-- 变量定义
		DECLARE @GameTimeTimes INT = 0
		DECLARE @GameCountTimes INT = 0
		DECLARE @GameExpendTimes INT = 0

		-- 换算游戏时长次数
		IF @SliceGameTime>0
		BEGIN
			SET @GameTimeTimes = @CurrGameTime/@SliceGameTime
			SET @CurrGameTime = @CurrGameTime%@SliceGameTime
		END

		-- 换算游戏局数次数
		IF @SliceGameCount>0
		BEGIN
			SET @GameCountTimes = @CurrGameCount/@SliceGameCount
			SET @CurrGameCount = @CurrGameCount%@SliceGameCount
		END

		-- 换算游戏消耗次数
		IF @SliceGameExpend>0
		BEGIN
			SET @GameExpendTimes = @CurrGameExpend/@SliceGameExpend
			SET @CurrGameExpend = @CurrGameExpend%@SliceGameExpend
		END
		
		-- 次数增加
		IF @GameTimeTimes>@GameTimeLotteryTimes OR @GameCountTimes>@GameCountLotteryTimes OR @GameExpendTimes>@GameExpendLotteryTimes
		BEGIN
			-- 更新次数
			SET @GameTimeLotteryTimes=@GameTimeTimes
			SET @GameCountLotteryTimes=@GameCountTimes
			SET @GameExpendLotteryTimes=@GameExpendTimes			
			SET @MayLotteryTimes=@FreeLotteryTimes+@PayLotteryTimes+@GameTimeLotteryTimes+@GameCountLotteryTimes+@GameExpendLotteryTimes

			-- 调整次数 
			IF @MayLotteryTimes>=@DailyMayLotteryTimes SET @MayLotteryTimes=@DailyMayLotteryTimes
			
			-- 更新数据
			UPDATE AccountsTurntableLottery SET MayLotteryTimes=@MayLotteryTimes,GameTimeLotteryTimes =@GameTimeLotteryTimes,GameCountLotteryTimes=@GameCountLotteryTimes,GameExpendLotteryTimes=@GameExpendLotteryTimes
			WHERE DateID=@DateID AND UserID=@dwUserID
		END
	END ELSE
	BEGIN
		SELECT @CurrGameTime=0,@CurrGameCount=0,@CurrGameExpend=0
	END

	-- 转盘记录
	DECLARE @TurntableRecord NVARCHAR(MAX)
	SET @TurntableRecord = (SELECT TOP 10 RecordID, NickName, GoodsID, GoodsCount, CollectDate FROM LHGameRecordDB.dbo.RecordTurntableLottery(NOLOCK) WHERE RecordID>@dwRecordID ORDER BY RecordID DESC FOR JSON AUTO)

	-- 调整变量
	IF @TurntableRecord IS NULL SET @TurntableRecord=N'[]'	

	-- 剩余次数
	DECLARE @ResidueLotteryTimes INT
	SET @ResidueLotteryTimes=@MayLotteryTimes-@UseLotteryTimes

	-- 抛出数据
	SELECT @DailyMayLotteryTimes AS DailyMayLotteryTimes, @UseLotteryTimes AS UseLotteryTimes, @ResidueLotteryTimes AS ResidueLotteryTimes,@FreeLotteryTimes AS FreeLotteryTimes,@PayLotteryTimes AS PayLotteryTimes, @GameTimeLotteryTimes AS GameTimeLotteryTimes,@GameCountLotteryTimes AS GameCountLotteryTimes,
		@GameExpendLotteryTimes AS GameExpendLotteryTimes,@TurntableRecord AS TurntableRecord,@CurrGameTime AS CurrGameTime, @CurrGameCount AS CurrGameCount, @CurrGameExpend AS CurrGameExpend,@SliceGameTime AS SliceGameTime,
		@SliceGameCount AS SliceGameCount,@SliceGameExpend AS SliceGameExpend

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- 转盘抽奖
CREATE PROC GSP_GP_RequestTurntableLottery
	@dwUserID INT,							-- 用户标识			
	@wReason INT,							-- 变更原因			
	@strErrorDescribe NVARCHAR(127) OUTPUT  -- 错误描述
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 自动回滚
SET XACT_ABORT ON

-- 执行逻辑
BEGIN	
	
	-- 变量定义
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @NickName NVARCHAR(32)
	DECLARE @MemberOrder SMALLINT
	
	-- 查询用户
	SELECT @UserID=a.UserID, @NickName=a.NickName, @Nullity=a.Nullity, @MemberOrder=a.MemberOrder, @ShutDown=b.ShutDownStatus 
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
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

	-- 查询抽奖
	DECLARE @TuantableEnabled TINYINT
	SELECT @TuantableEnabled=CONVERT(TINYINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TurntableOption' AND StatusName=N'TurantableEnabled'

	-- 调整开关
	IF @TuantableEnabled IS NULL SET @TuantableEnabled=0

	-- 校验开关
	IF @TuantableEnabled=0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1324,"Msg":"转盘抽奖活动已关闭！"}'
		RETURN 4
	END
	
	-- 日期标识
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 抽奖信息
	DECLARE @MayLotteryTimes INT
	DECLARE @UseLotteryTimes INT	

	-- 查询次数
	SELECT @MayLotteryTimes=MayLotteryTimes,@UseLotteryTimes=UseLotteryTimes FROM AccountsTurntableLottery(NOLOCK) WHERE DateID=@DateID AND UserID=@dwUserID

	-- 数据不存在
	IF @MayLotteryTimes IS NULL
	BEGIN
		RETURN 100
	END

	-- 校验次数
	IF @UseLotteryTimes>=@MayLotteryTimes
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1325,"Msg":"抽奖机会已经用完！"}'
		RETURN 5
	END

	-- 日期标识
	DECLARE @DateTime DATETIME
	SET @DateTime=GETDATE()

	-- 转盘奖励
	DECLARE @TableAwardItem TABLE (ItemIndex INT, ItemWeight INT, AwardName NVARCHAR(64), GoodsID SMALLINT, GoodsCount INT, GoodsLevel INT, GoodsIndate INT, DailyLimitPlaces INT, StartRandValue INT, EndRandValue INT)	

	-- 读取配置
	INSERT INTO @TableAwardItem(ItemIndex,ItemWeight,AwardName,GoodsID,GoodsCount,GoodsLevel,GoodsIndate,DailyLimitPlaces,StartRandValue,EndRandValue)
	SELECT ItemIndex,ItemWeight,AwardName,GoodsID,GoodsCount,GoodsLevel,GoodsIndate,DailyLimitPlaces,0,0 FROM LHPlatformDB.dbo.TurntableAwardConfig(NOLOCK)

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 查询白名单
	DECLARE @HitIndex INT
	SELECT @HitIndex=HitIndex FROM TurntableLotteryWriteList WITH(UPDLOCK)  WHERE UserID=@dwUserID
	
	IF @HitIndex IS NULL
	BEGIN
		-- 变量定义
		DECLARE @Index INT			
		DECLARE @RandValue INT
		DECLARE @LoopCount INT	
		DECLARE @HitTimes INT
		DECLARE @ItemIndex INT
		DECLARE @ItemWeight INT
		DECLARE @TotalWeight INT		
		DECLARE @LimitPlaces INT
		DECLARE @AddupWeight INT

		-- 初始化
		SELECT @Index=0, @LoopCount=COUNT(1), @AddupWeight=0, @TotalWeight=SUM(ItemWeight) FROM @TableAwardItem

		-- 构造随机表
		WHILE @Index<@LoopCount
		BEGIN

			SELECT @ItemIndex=ItemIndex,@ItemWeight=ItemWeight,@LimitPlaces=DailyLimitPlaces FROM @TableAwardItem WHERE ItemIndex=@Index

			UPDATE @TableAwardItem SET StartRandValue=@AddupWeight,EndRandValue=@AddupWeight+ItemWeight-1 WHERE ItemIndex=@ItemIndex			

			SET @Index=@Index+1
			SET @AddupWeight+=@ItemWeight
		END	

		-- 重置数据
		SELECT @Index=0, @LoopCount=100

		-- 循环抽奖
		WHILE @Index<=@LoopCount
		BEGIN

			-- 获取随机值
			SET @RandValue=CAST(RAND()*@TotalWeight AS INT)

			-- 随机索引
			SELECT @HitIndex=ItemIndex,@LimitPlaces=DailyLimitPlaces FROM @TableAwardItem WHERE @RandValue>=StartRandValue AND @RandValue<=EndRandValue

			-- 命中次数
			SELECT @HitTimes=HitTimes FROM TurntableLotteryHitStat WHERE DateID=@DateID AND ItemIndex=@HitIndex
			IF @HitTimes IS NULL SET @HitTimes=0

			-- 校验名额
			IF @HitTimes<@LimitPlaces OR @LimitPlaces=0
			BEGIN
				UPDATE TurntableLotteryHitStat SET HitTimes+=1 WHERE DateID=@DateID AND ItemIndex=@HitIndex	
				IF @@ROWCOUNT=0
				BEGIN
					INSERT INTO TurntableLotteryHitStat(DateID,ItemIndex,HitTimes) VALUES(@DateID,@HitIndex,1)
				END
				BREAK
			END

			SET @Index+=1
		END
	END ELSE
	BEGIN		
		-- 删除名单
		DELETE TurntableLotteryWriteList WHERE UserID=@dwUserID
	END

	-- 奖品信息
	DECLARE @IncGoodsID INT	
	DECLARE @IncGoodsCount INT	
	DECLARE @IncGoodsLevel INT
	DECLARE @IncGoodsIndate INT

	-- 查询物品
	SELECT @IncGoodsID=GoodsID,@IncGoodsCount=GoodsCount,@IncGoodsLevel=GoodsLevel,@IncGoodsIndate=GoodsIndate FROM @TableAwardItem WHERE ItemIndex=@HitIndex
	
	-- 计算时间戳
	DECLARE @dwTimeStamp INT = dbo.LHF_GetTimeStamp();
	
	-- 写入财富
	IF dbo.LHF_IsMoneyGoods(@IncGoodsID)=1
	BEGIN		
		EXEC GSP_GP_WriteGameWealth @dwUserID,@IncGoodsID,@wReason,@IncGoodsCount
	END ELSE
	BEGIN			
		EXEC GSP_GP_AppendGoodsItem @dwUserID,@wReason,@IncGoodsID,@IncGoodsLevel,@IncGoodsCount,@IncGoodsIndate,@dwTimeStamp			
	END		
	
	-- 更新次数
	UPDATE AccountsTurntableLottery SET UseLotteryTimes+=1,CollectDateTime=GetDate() WHERE DateID=@DateID AND UserID=@dwUserID

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 奖品名称
	DECLARE @AwardName NVARCHAR(64)
	SELECT @AwardName=AwardName FROM @TableAwardItem WHERE ItemIndex=@HitIndex

	-- 抽奖记录
	INSERT INTO LHGameRecordDB.dbo.RecordTurntableLottery(UserID,NickName,HitIndex,AwardName,GoodsID,GoodsCount,GoodsLevel,GoodsIndate,CollectDate)
	VALUES(@dwUserID,@NickName,@HitIndex,@AwardName,@IncGoodsID,@IncGoodsCount,@IncGoodsLevel,@IncGoodsIndate,GETDATE())

	-- 抛出数据
	SELECT (MayLotteryTimes-UseLotteryTimes) AS ResidueLotteryTimes,UseLotteryTimes,@HitIndex AS HitIndex,@IncGoodsID AS GoodsID, @IncGoodsCount AS GoodsCount,@IncGoodsLevel AS GoodsLevel,@IncGoodsIndate AS GoodsIndate FROM AccountsTurntableLottery 
	WHERE DateID=@DateID AND UserID=@dwUserID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
