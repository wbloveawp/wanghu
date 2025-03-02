----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LHF_GetCheckInData]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[LHF_GetCheckInData]
GO


IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_EfficacyCheckIn]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_EfficacyCheckIn]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


---------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------

-- 获取时间戳
CREATE FUNCTION [dbo].[LHF_GetCheckInData]
(	
	@dwUserID INT						-- 用户标识		
)
RETURNS @Result TABLE (CheckInDays SMALLINT, CheckInState TINYINT)
AS
BEGIN
	
	-- 变量定义
	DECLARE @CheckInDays SMALLINT
	DECLARE @CheckInState TINYINT
	DECLARE @LastCheckInTime DATETIME

	-- 查询签到信息
	SELECT @CheckInDays=CheckInDays,@LastCheckInTime=LastCheckInTime FROM AccountsCheckIn(NOLOCK) WHERE UserID=@dwUserID
	
	-- 校验结果
	IF @CheckInDays IS NULL
	BEGIN
		-- 调整变量
		SELECT @CheckInDays=0,@CheckInState=0

	END ELSE
	BEGIN
		-- 签到间隔
		DECLARE @CheckInDiff INT
		SET @CheckInDiff=DATEDIFF(day,@LastCheckInTime,GETDATE())
		
		-- 调整状态
		SET @CheckInState=0
		IF @CheckInDiff=0 SET @CheckInState=1

		-- 从头再来
		IF (@CheckInDays=7 AND @CheckInDiff=1) OR (@CheckInDiff>1)
		BEGIN
			-- 调整天数
			SET @CheckInDays=0
		END
	END

	-- 插入结果集
	INSERT INTO @Result(CheckInDays,CheckInState) VALUES (@CheckInDays,@CheckInState)

	RETURN
END

GO

---------------------------------------------------------------------------------------------------

-- 用户签到
CREATE PROC GSP_MB_EfficacyCheckIn
	@dwUserID INT,							-- 用户标识
	@cbQuery TINYINT,						-- 查询标识
	@strErrorDescribe NVARCHAR(127) OUTPUT  -- 错误描述
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 回滚设置
SET XACT_ABORT ON

-- 执行逻辑
BEGIN
	
	-- 查询用户
	DECLARE @UserID INT
	DECLARE @GameID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	DECLARE @NickName NVARCHAR(64)
	DECLARE @MemberOrder SMALLINT
	SELECT @UserID=a.UserID,@GameID=a.GameID,@NickName=a.NickName,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus,@MemberOrder=a.MemberOrder
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
	--IF @MemberOrder=0
	--BEGIN
	--	SET @strErrorDescribe=N'{"Idx":1141,"Msg":"签到功能仅限VIP会员使用！"} '
	--	RETURN 4
	--END

	-- 配置定义
	DECLARE @CheckInEnabled TINYINT
	DECLARE @CheckInRewards NVARCHAR(MAX)

	-- 读取配置
	SELECT @CheckInRewards=StatusValue FROM LHPlatformDB.dbo.SystemStatusInfo WHERE StatusKind=N'CheckInOption' AND StatusName=N'CheckInRewards'
	SELECT @CheckInEnabled=CONVERT(TINYINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo WHERE StatusKind=N'CheckInOption' AND StatusName=N'CheckInEnabled'

	-- 校验状态
	IF @CheckInEnabled=0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1138,"Msg":"抱歉，签到功能已暂时关闭！"} '
		RETURN 4
	END
	
	-- 签到信息
	DECLARE @CheckInDays INT
	DECLARE @CheckInState TINYINT
	DECLARE @LastCheckInTime DATETIME
	
	-- 查询信息
	SELECT @CheckInDays=CheckInDays,@LastCheckInTime=LastCheckInTime FROM AccountsCheckIn(NOLOCK) WHERE UserID=@UserID	

	-- 首次签到
	IF @CheckInDays IS NULL
	BEGIN
		SELECT @CheckInDays=0,@CheckInState=0
	END ELSE
	BEGIN
		-- 签到间隔
		DECLARE @CheckInDiff INT
		SET @CheckInDiff=DATEDIFF(day,@LastCheckInTime,GETDATE())
		
		-- 调整状态
		SET @CheckInState=0
		IF @CheckInDiff=0 SET @CheckInState=1

		-- 从头再来
		IF (@CheckInDays=7 AND @CheckInDiff=1) OR (@CheckInDays>0 AND @CheckInDiff>1)
		BEGIN
			-- 调整天数
			SET @CheckInDays=0					
		END
	END

	--仅查询
	IF @cbQuery > 0
	BEGIN
		SELECT @CheckInDays AS CheckInDays, @CheckInState AS CheckInState, 0 AS RewardGoodsID, 0 AS RewardAmount
		RETURN 0
	END

	-- 状态校验
	IF @CheckInState=1
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1139,"Msg":"您今天已经签到，每天仅可签到一次！"} '
		RETURN 5
	END		

	-- 表变量定义
	DECLARE @TableCheckInRewards TABLE (DayID SMALLINT,Rewards NVARCHAR(MAX))	
	DECLARE @TableRewardsItem TABLE (ItemIndex INT PRIMARY KEY IDENTITY(1,1),GoodsID SMALLINT,Amount BIGINT,[Weight] INT, StartRandValue INT, EndRandValue INT)

	-- 插入数据
	INSERT INTO @TableCheckInRewards(DayID, Rewards)
	SELECT JSON_VALUE(VALUE,N'$.DayID'), JSON_QUERY(VALUE,N'$.Rewards') FROM OPENJSON(@CheckInRewards)

	-- 更新变量
	SET @CheckInState=1
	SET @CheckInDays+=1

	-- 奖励配置
	DECLARE @RewardsOption NVARCHAR(MAX)
	SELECT @RewardsOption=Rewards FROM @TableCheckInRewards WHERE DayID=@CheckInDays

	-- 校验配置
	IF @RewardsOption IS NULL OR ISJSON(@RewardsOption)=0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1140,"Msg":"系统异常，签到失败，请联系客服了解详细情况！"} '
		RETURN 6
	END

	-- 插入数据
	INSERT INTO @TableRewardsItem(GoodsID,Amount,[Weight],StartRandValue,EndRandValue)
	SELECT  JSON_VALUE(VALUE,N'$.GoodsID'), JSON_VALUE(VALUE,N'$.Amount'), JSON_VALUE(VALUE,N'$.Weight'),0,0 FROM OPENJSON(@RewardsOption)

	-- 变量定义
	DECLARE @ItemIndex INT	
	DECLARE @ItemWeight INT
	DECLARE @LoopCount INT
	DECLARE @TotalWeight INT		
	DECLARE @LimitPlaces INT
	DECLARE @AddupWeight INT
	
	-- 初始化
	SELECT @ItemIndex=1,@LoopCount=COUNT(1), @AddupWeight=0, @TotalWeight=SUM([Weight]) FROM @TableRewardsItem

	-- 构造随机表
	WHILE @ItemIndex<=@LoopCount
	BEGIN
		-- 查询权重
		SELECT @ItemWeight=[Weight] FROM @TableRewardsItem WHERE ItemIndex=@ItemIndex

		-- 更新随机区间
		UPDATE @TableRewardsItem SET StartRandValue=@AddupWeight,EndRandValue=@AddupWeight+@ItemWeight-1 WHERE ItemIndex=@ItemIndex			

		-- 设置变量
		SET @ItemIndex+=1
		SET @AddupWeight+=@ItemWeight
	END	

	-- 变量定义
	DECLARE @HitIndex INT
	DECLARE @RandValue INT
	DECLARE @RewardAmount BIGINT
	DECLARE @RewardGoodsID SMALLINT

	-- 获取随机值
	SET @RandValue=CAST(RAND()*@TotalWeight AS INT)

	-- 选取奖励
	SELECT @HitIndex=ItemIndex,@RewardGoodsID=GoodsID, @RewardAmount=Amount FROM @TableRewardsItem WHERE @RandValue>=StartRandValue AND @RandValue<=EndRandValue

	-- 调整金额
	IF @HitIndex IS NULL
	BEGIN
		SET @HitIndex=1 
		SELECT @RewardAmount=Amount,@RewardGoodsID=GoodsID FROM @TableRewardsItem WHERE ItemIndex=@HitIndex
	END
	
	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 更新财富
	EXEC GSP_GP_WriteGameWealth @dwUserID,@RewardGoodsID,25,@RewardAmount
		
	-- 更新信息
	UPDATE AccountsCheckIn SET CheckInDays=@CheckInDays,LastCheckInTime=GetDate() WHERE UserID=@dwUserID	
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO AccountsCheckIn(UserID,CheckInDays,LastCheckInTime,CollectDateTime) VALUES (@dwUserID,@CheckInDays,GETDATE(),GETDATE())
	END

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 签到记录
	INSERT INTO LHGameRecordDB.dbo.RecordDailyCheckIn(UserID,NickName,GoodsID,Amount,CollectDate) 
	VALUES(@dwUserID,@NickName,@RewardGoodsID,@RewardAmount,GETDATE())

	-- 抛出数据
	SELECT @CheckInDays AS CheckInDays, @CheckInState AS CheckInState, @RewardGoodsID AS RewardGoodsID, @RewardAmount AS RewardAmount
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------