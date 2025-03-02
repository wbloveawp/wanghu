USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_RecordBattleGrade]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_RecordBattleGrade]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 约战记录
CREATE PROC GSP_GR_RecordBattleGrade
	@dwBattleNum INT,							-- 映射编号
	@cbBattleFinish TINYINT,					-- 完成标志
	@lBattleRecordID BIGINT,					-- 记录标识
	@wKindID SMALLINT,							-- 类型标识
	@wServerID SMALLINT,						-- 房间标识
	@cbScoreKind TINYINT,						-- 分数类型
	@dwOwnerID INT,								-- 桌主标识
	@dwClubID INT,								-- 茶馆标识	
	@dwClubCreaterID INT,						-- 馆主标识
	@cbSettleKind TINYINT,						-- 解散方式
	@wFinishCount SMALLINT,						-- 完成局数
	@wPlayCount SMALLINT,						-- 游戏局数
	@dwPlayTime INT,							-- 游戏时长
	@dwCreateTime INT,							-- 创建时间
	@lBaseScore INT,							-- 游戏底分
	@lWinnerScore INT,							-- 赢家奖金
	@wPayType SMALLINT,							-- 支付方式
	@dwPayUserID INT,							-- 支付玩家
	@dwConfigID INT,							-- 玩法标识
	@cbAllowPublic TINYINT,						-- 允许公开
	@lConsumeCount INT,							-- 消耗数量
	@cbCurrencyKind TINYINT,					-- 货币类型	
	@strGameOption NVARCHAR(128),				-- 游戏配置
	@strUserScoreString NVARCHAR(MAX)			-- 分数信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 变量定义
DECLARE @UserID INT
DECLARE @Score INT
DECLARE @RankID INT
DECLARE @WinCount INT	
DECLARE @LostCount INT
DECLARE @RewardGold INT
DECLARE @ConsumeCard INT
DECLARE @ConsumeGold INT
DECLARE @SysRevenue INT
DECLARE @ClubRevenue INT
DECLARE @TotalSysRevenue INT	
DECLARE @TotalClubRevenue INT	
DECLARE @BigWinCount INT
DECLARE @MinWinnerScore INT

-- 变量定义
DECLARE	@TableID INT									
DECLARE @Sqlstr		NVARCHAR(512)						
DECLARE	@TableName	NVARCHAR(128)
DECLARE @UserMemberType SMALLINT

-- 执行逻辑
BEGIN

	-- 日期编号
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 表定义
	DECLARE @TableUserGrade TABLE (ID INT PRIMARY KEY IDENTITY(1,1),UserID INT,RankID INT, Score BIGINT, RewardScore BIGINT, SysRevenue INT,ClubRevenue INT)

	-- 插入数据
	IF ISJSON(@strUserScoreString)>0
	BEGIN
		INSERT INTO @TableUserGrade(UserID,RankID,Score,RewardScore,SysRevenue,ClubRevenue)
		SELECT JSON_VALUE(VALUE,N'$.UserID'),JSON_VALUE(VALUE,N'$.RankID'),JSON_VALUE(VALUE,N'$.Score'),JSON_VALUE(VALUE,N'$.RewardScore'),JSON_VALUE(VALUE,N'$.SysRevenue'),JSON_VALUE(VALUE,N'$.ClubRevenue') FROM OPENJSON(@strUserScoreString)
	END

	-- 变量定义
	DECLARE @Index INT
	DECLARE @UserCount INT

	-- 查询数据
	SELECT @Index=1,@UserCount=COUNT(1),@TotalSysRevenue=0,@TotalClubRevenue=0,@SysRevenue=0,@ClubRevenue=0,@RewardGold=0,@MinWinnerScore=0 FROM @TableUserGrade	

	-- 插入记录
	IF NOT EXISTS(SELECT 1 FROM LHBattleRecordDB.dbo.RecordBattleInfo(NOLOCK) WHERE RecordID=@lBattleRecordID)
	BEGIN
		INSERT INTO LHBattleRecordDB.dbo.RecordBattleInfo (RecordID,DateID,BattleNum,BattleFinish,ClubID,KindID,ServerID,ScoreKind,OwnerID,SettleKind,FinishCount,PlayCount,
			PlayTime,CreateTime,BaseScore,WinnerScore,PayType,PayUserID,AllowPublic,ConsumeCount,CurrencyKind,GameOption,UserCount,UserScoreString,CollectDate)
		VALUES (@lBattleRecordID,@DateID,@dwBattleNum,@cbBattleFinish,@dwClubID,@wKindID,@wServerID,@cbScoreKind,@dwOwnerID,@cbSettleKind,@wFinishCount,@wPlayCount,
			@dwPlayTime,@dwCreateTime,@lBaseScore,@lWinnerScore,@wPayType,@dwPayUserID,@cbAllowPublic,@lConsumeCount,@cbCurrencyKind,@strGameOption,@UserCount,@strUserScoreString,GetDate())
	END

	-- 查询站点								
	IF @dwClubID<>0
	BEGIN
		-- 分表标识				
		SET	@TableID=0;

		-- 群组成员表
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

		-- 最小赢分
		SELECT @MinWinnerScore=ISNULL(MinWinnerScore,0) FROM ClubKindSetting(NOLOCK) WHERE ClubID=@dwClubID AND KindID=@wKindID
	END	

	-- 解析数据
	WHILE @Index<=@UserCount
	BEGIN
		-- 查询数据
		SELECT @UserID=UserID,@RankID=RankID,@Score=Score,@RewardGold=ISNULL(RewardScore,0),@SysRevenue=ISNULL(SysRevenue,0),@ClubRevenue=ISNULL(ClubRevenue,0) FROM @TableUserGrade WHERE ID=@Index

		-- 数据统计
		IF @cbBattleFinish=1
		BEGIN
			UPDATE StreamBattleKindInfo SET BattleCount=BattleCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND UserID=@UserID 
			IF @@ROWCOUNT=0
			BEGIN
				INSERT INTO StreamBattleKindInfo(DateID,UserID,KindID,BattleCount) VALUES(@DateID,@UserID,@wKindID,1)
			END
		END

		-- 更新税收
		SET @TotalSysRevenue+=@SysRevenue
		SET @TotalClubRevenue+=@ClubRevenue

		-- 馆内约战
		IF @dwClubID>0
		BEGIN
			-- 设置变量
			SELECT @WinCount=0,@LostCount=0,@ConsumeCard=0,@ConsumeGold=0,@BigWinCount=0			

			-- 输赢局数
			IF @RankID=@UserCount SET @LostCount=1 					
			IF @RankID=1 AND @Score>0 AND @wPayType<>2  SET @WinCount=1 	
			
			-- 大赢家
			IF @RankID=1 AND @Score>=@MinWinnerScore SET @BigWinCount=1
			
			-- 金币支付
			IF @cbCurrencyKind=1
			BEGIN
				-- AA支付
				IF @wPayType=2 SET @ConsumeGold=@lConsumeCount
			END

			-- 房卡支付
			IF @cbCurrencyKind=2
			BEGIN
				-- AA支付
				IF @wPayType=2 SET @ConsumeCard=@lConsumeCount

				-- 馆主支付
				IF @wPayType=1 AND @UserID=@dwPayUserID SET @ConsumeCard=@lConsumeCount

				-- 大赢家支付
				IF @wPayType=4 AND @UserID=@dwPayUserID SET @ConsumeCard=@lConsumeCount
			END

			-- 调整支付玩家
			IF @UserID=@dwPayUserID SET @dwPayUserID=0		
		END

		-- 玩家记录
		IF NOT EXISTS(SELECT 1 FROM LHBattleRecordDB.dbo.RecordBattleUser(NOLOCK) WHERE RecordID=@lBattleRecordID AND UserID=@UserID)
		BEGIN
			IF @dwClubID=0
			BEGIN
				-- 插入记录
				INSERT INTO LHBattleRecordDB.dbo.RecordBattleUser(RecordID,UserID,Score,SysRevenue,ClubRevenue,DateID,ClubID,RankID,BattleNum,CollectDate)
				VALUES (@lBattleRecordID,@UserID,@Score,@SysRevenue,@ClubRevenue,@DateID,@dwClubID,@RankID,@dwBattleNum,GetDate())

			END ELSE
			BEGIN
				-- 异常捕获
				BEGIN TRY 
					-- 开始事务
					SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
					BEGIN TRAN										

					-- 赢分统计
					IF @Score>=0
					BEGIN
						-- 输赢统计
						UPDATE StreamClubBalance SET BattleCount+=1,ConsumeCard+=@ConsumeCard,ConsumeGold+=@ConsumeGold,RewardGold+=@RewardGold,WinCount+=@WinCount,WinScore+=@Score,
							   SysRevenue+=@SysRevenue,ClubRevenue+=@ClubRevenue,BigWinCount+=@BigWinCount  
						WHERE DateID=@DateID AND ClubID=@dwClubID AND UserID=@UserID
						IF @@ROWCOUNT=0
						BEGIN		
							-- 插入数据	
							INSERT INTO StreamClubBalance(DateID,ClubID,UserID,WinCount,WinScore,SysRevenue,ClubRevenue,BattleCount,BalanceCount,ConsumeCard,ConsumeGold,RewardGold,BigWinCount)
							VALUES(@DateID,@dwClubID,@UserID,@WinCount,@Score,@SysRevenue,@ClubRevenue,1,0,@ConsumeCard,@ConsumeGold,@RewardGold,@BigWinCount)					
						END
				
						-- 输赢统计
						UPDATE StreamClubUser SET WinCount+=@WinCount,WinScore+=@Score,ConsumeCard+=@ConsumeCard,ConsumeGold+=@ConsumeGold,RewardGold+=@RewardGold,SysRevenue+=@SysRevenue,
							   ClubRevenue+=@ClubRevenue,BattleCount+=1,BigWinCount+=@BigWinCount,LastBattleTime=GetDate() 
						WHERE DateID=@DateID AND ClubID=@dwClubID AND UserID=@UserID AND KindID=@wKindID
						IF @@ROWCOUNT=0
						BEGIN			
							INSERT INTO StreamClubUser(DateID,ClubID,UserID,KindID,WinCount,WinScore,SysRevenue,ClubRevenue,ConsumeCard,ConsumeGold,RewardGold,BigWinCount,BattleCount,LastBattleTime)
							VALUES (@DateID,@dwClubID,@UserID,@wKindID,@WinCount,@Score,@SysRevenue,@ClubRevenue,@ConsumeCard,@ConsumeGold,@RewardGold,@BigWinCount,1,GetDate())
						END							
					END			
			
					-- 输分统计
					IF @Score<0
					BEGIN
						-- 更新数据
						UPDATE StreamClubBalance SET BattleCount+=1,ConsumeCard+=@ConsumeCard,ConsumeGold+=@ConsumeGold,RewardGold+=@RewardGold,LostCount+=@LostCount,LostScore+=@Score,SysRevenue+=@SysRevenue,ClubRevenue+=@ClubRevenue
						WHERE DateID=@DateID AND ClubID=@dwClubID AND UserID=@UserID 
						IF @@ROWCOUNT=0
						BEGIN		
							-- 插入数据	
							INSERT INTO StreamClubBalance(DateID,ClubID,UserID,LostCount,LostScore,SysRevenue,ClubRevenue,BattleCount,BalanceCount,ConsumeCard,ConsumeGold,RewardGold)
							VALUES(@DateID,@dwClubID,@UserID,@LostCount,@Score,@SysRevenue,@ClubRevenue,1,0,@ConsumeCard,@ConsumeGold,@RewardGold)					
						END		
				
						-- 输赢统计
						UPDATE StreamClubUser SET LostCount+=@LostCount,LostScore+=@Score,ConsumeCard+=@ConsumeCard,ConsumeGold+=@ConsumeGold,RewardGold+=@RewardGold,SysRevenue+=@SysRevenue,
							   ClubRevenue+=@ClubRevenue,BattleCount+=1,LastBattleTime=GetDate() 
						WHERE DateID=@DateID AND ClubID=@dwClubID AND UserID=@UserID AND KindID=@wKindID
						IF @@ROWCOUNT=0
						BEGIN			
							INSERT INTO StreamClubUser(DateID,ClubID,UserID,KindID,LostCount,LostScore,SysRevenue,ClubRevenue,ConsumeCard,ConsumeGold,RewardGold,BattleCount,LastBattleTime)
							VALUES (@DateID,@dwClubID,@UserID,@wKindID,@LostCount,@Score,@SysRevenue,@ClubRevenue,@ConsumeCard,@ConsumeGold,@RewardGold,1,GetDate())
						END							
					END

					-- 插入记录
					INSERT INTO LHBattleRecordDB.dbo.RecordBattleUser(RecordID,UserID,Score,SysRevenue,ClubRevenue,DateID,ClubID,RankID,BattleNum,CollectDate)
					VALUES (@lBattleRecordID,@UserID,@Score,@SysRevenue,@ClubRevenue,@DateID,@dwClubID,@RankID,@dwBattleNum,GetDate())
							
					-- 结束事务
					COMMIT TRAN
					SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

				END TRY 
				BEGIN CATCH

					-- 回滚事务
					ROLLBACK TRAN
					SET TRANSACTION ISOLATION LEVEL READ COMMITTED
					RETURN 1

				END CATCH			
			END													
		END

		SET @Index+=1
	END		

	-- 支付玩家消耗统计
	IF @dwClubID<>0 AND @dwPayUserID<>0 AND @cbCurrencyKind=2
	BEGIN		
		-- 更新数据
		UPDATE StreamClubBalance SET ConsumeCard+=@lConsumeCount WHERE DateID=@DateID AND ClubID=@dwClubID AND UserID=@dwPayUserID
		IF @@ROWCOUNT=0
		BEGIN
			-- 插入数据
			INSERT INTO StreamClubBalance(DateID,ClubID,UserID,ConsumeCard) VALUES(@DateID,@dwClubID,@dwPayUserID,@lConsumeCount)
		END	
	END

	--------------------------------------- 约战统计 -------------------------------------------	

	-- 房卡数量
	SET @ConsumeCard=0		
	IF @cbCurrencyKind=2 SET @ConsumeCard=@lConsumeCount

	-- 完成局数
	DECLARE @BattleFinishCount INT
	SET @BattleFinishCount=0
	IF @cbBattleFinish=1 SET @BattleFinishCount=1

	-- 数据统计
	UPDATE StreamBattleInfo SET BattleCount+=1,BattleFinishCount+=@BattleFinishCount,ConsumeCard+=@ConsumeCard
	WHERE DateID=@DateID AND KindID=@wKindID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO StreamBattleInfo(DateID,KindID,BattleCount,BattleFinishCount,ConsumeCard) VALUES(@DateID,@wKindID,1,@BattleFinishCount,@ConsumeCard)
	END

	-- 数据统计
	IF @dwClubID<>0
	BEGIN
		UPDATE StreamClubBattle SET BattleCount+=1,BattleFinishCount+=@BattleFinishCount,ConsumeCard+=@ConsumeCard,SysRevenue+=@TotalSysRevenue,ClubRevenue+=@TotalClubRevenue
		WHERE DateID=@DateID AND ClubID=@dwClubID AND KindID=@wKindID
		IF @@ROWCOUNT=0
		BEGIN
			INSERT INTO StreamClubBattle(DateID,ClubID,KindID,BattleCount,BattleFinishCount,SysRevenue,ClubRevenue,ConsumeCard) 
			VALUES(@DateID,@dwClubID,@wKindID,1,@BattleFinishCount,@TotalSysRevenue,@TotalClubRevenue,@ConsumeCard)
		END
	END

	--------------------------------------- 约战统计 -------------------------------------------
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------