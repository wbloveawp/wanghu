----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UpdateUserLevel]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UpdateUserLevel]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 更新用户等级
CREATE PROC GSP_GP_UpdateUserLevel
	@dwUserID INT,							-- 用户标识			
	@dwIncPoint INT,						-- 增长点数		
	@dwTimeStamp INT,						-- 时间戳		
	@wReason INT,							-- 变更原因	
	@strErrorDescribe NVARCHAR(127) OUTPUT  -- 错误描述
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 查询用户
	DECLARE @MemberPoint INT
	DECLARE @MemberOrder TINYINT		
	SELECT @MemberOrder=MemberOrder, @MemberPoint=MemberPoint FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 校验用户
	IF @MemberOrder IS NULL 
	BEGIN
		SET @strErrorDescribe=N'系统未查询到您的用户信息，请查证后再次尝试！'
		RETURN 1
	END	

	-- 更新点数
	SET @MemberPoint+=@dwIncPoint

	-- 变量定义
	DECLARE @MaxNeedExp INT
	DECLARE @MaxMemberOrder TINYINT
	DECLARE @NewMemberOrder TINYINT

	-- 查询数据
	SELECT TOP 1 @MaxNeedExp=NeedPoint,@MaxMemberOrder=LevelID FROM LHPlatformDB.dbo.GameUserLevel ORDER BY LevelID DESC

	-- 校验点数
	IF @MemberPoint>=@MaxNeedExp
	BEGIN
		SET @NewMemberOrder=@MaxMemberOrder
	END ELSE 
	BEGIN
		SELECT TOP 1 @NewMemberOrder=LevelID-1 FROM LHPlatformDB.dbo.GameUserLevel WHERE NeedPoint>@MemberPoint
	END

	-- 调整等级
	IF @NewMemberOrder IS NULL SET @NewMemberOrder=@MemberPoint

	-- 奖励物品
	DECLARE @UpgradeReward NVARCHAR(MAX)	

	-- 初始化
	SET @UpgradeReward=N''

	-- 升级处理
	IF @NewMemberOrder>@MemberOrder
	BEGIN
		-- 变量定义
		DECLARE @Index INT
		DECLARE @LoopCount INT	
		DECLARE @ResidueCount INT	

		-- 物品信息
		DECLARE @IncGoodsID INT
		DECLARE @IncGoodsLevel INT
		DECLARE @IncGoodsCount INT
		DECLARE @IncGoodsIndate INT		

		-- 表变量定义			
		DECLARE @TableRewardGoodsList TABLE (ID INT identity(1,1),GoodsID SMALLINT,GoodsLevel SMALLINT, GoodsCount INT, ResidueCount INT, GoodsIndate INT)		

		-- 合并奖励
		SELECT @UpgradeReward+=SUBSTRING(Rewards,2,LEN(Rewards)-2)+N',' FROM LHPlatformDB.dbo.GameUserLevel WHERE LevelID>@MemberOrder AND LevelID<=@NewMemberOrder	

		-- 格式化
		IF LEN(@UpgradeReward)>0 SET @UpgradeReward=N'['+SUBSTRING(@UpgradeReward,1,LEN(@UpgradeReward)-1)+N']'

		-- 插入记录
		IF ISJSON(@UpgradeReward)>0
		BEGIN
			INSERT @TableRewardGoodsList (GoodsID,GoodsLevel,GoodsCount,ResidueCount,GoodsIndate)
			SELECT JSON_VALUE(VALUE,'$.GoodsID'), MIN(CONVERT(INT,ISNULL(JSON_VALUE(VALUE,'$.Level'),1))),SUM(CONVERT(INT,JSON_VALUE(VALUE,'$.Count'))),0,SUM(CONVERT(INT,ISNULL(JSON_VALUE(VALUE,'$.Indate'),0))) 
			FROM OPENJSON(@UpgradeReward) GROUP BY JSON_VALUE(VALUE,'$.GoodsID')
		END

		-- 开始事务
		SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
		BEGIN TRAN
	
		-- 处理物品
		SELECT @Index=1,@LoopCount=COUNT(1) FROM @TableRewardGoodsList
	
		-- 循环处理
		WHILE @Index<=@LoopCount
		BEGIN	
			-- 查询信息
			SELECT @IncGoodsID=GoodsID,@IncGoodsLevel=GoodsLevel,@IncGoodsCount=GoodsCount,@IncGoodsIndate=GoodsIndate FROM @TableRewardGoodsList WHERE ID=@Index		

			-- 校验物品类型
			IF dbo.LHF_IsMoneyGoods(@IncGoodsID) = 1
			BEGIN
				-- 写入财富
				EXEC GSP_GP_WriteGameWealth @dwUserID,@IncGoodsID,@wReason,@IncGoodsCount

			END ELSE
			BEGIN
				-- 添加物品
				EXEC @ResidueCount = dbo.GSP_GP_AppendGoodsItem @dwUserID,@wReason,@IncGoodsID,@IncGoodsLevel,@IncGoodsCount,@IncGoodsIndate,@dwTimeStamp

				-- 剩余处理
				IF @ResidueCount>0
				BEGIN
					-- 更新剩余
					UPDATE @TableRewardGoodsList SET ResidueCount=@ResidueCount WHERE ID=@Index				
				END		
			END
		
			-- 更新索引
			SET @Index=@Index+1
		END	

		-- 更新点数
		UPDATE AccountsInfo SET MemberOrder=@NewMemberOrder,MemberPoint=@MemberPoint WHERE UserID=@dwUserID

		-- 结束事务
		COMMIT TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	END	ELSE
	BEGIN
		-- 更新点值
		UPDATE AccountsInfo SET MemberOrder=@NewMemberOrder,MemberPoint=@MemberPoint WHERE UserID=@dwUserID
	END

	-- 剩余物品
	DECLARE @RewardGoodsList NVARCHAR(MAX)	

	-- 构造JSON
	SET @RewardGoodsList=(SELECT GoodsID, GoodsLevel AS [Level], GoodsCount AS [Count], GoodsIndate AS [Indate] FROM @TableRewardGoodsList FOR JSON AUTO)	

	-- 调整变量	
	IF @RewardGoodsList IS NULL SET @RewardGoodsList=N''

	-- 抛出记录
	SELECT @RewardGoodsList AS RewardGoodsList, @MemberPoint AS MemberPoint, @MemberOrder AS LastMemberOrder, @NewMemberOrder AS CurrMemberOrder
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------