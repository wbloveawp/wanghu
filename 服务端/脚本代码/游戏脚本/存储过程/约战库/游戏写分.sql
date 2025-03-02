----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_WriteGameScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_WriteGameScore]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 游戏写分
CREATE PROC GSP_GR_WriteGameScore
	@dwUserID INT,								-- 用户 I D
	@lScore BIGINT,								-- 用户分数		
	@lRevenue BIGINT,							-- 游戏税收
	@lWinCount INT,								-- 胜利盘数
	@lLostCount INT,							-- 失败盘数
	@lDrawCount INT,							-- 和局盘数
	@lFleeCount INT,							-- 断线数目
	@lWinStreak INT,							-- 连胜盘数
	@lMaxWinStreak INT,							-- 连胜盘数
	@lExperience INT,							-- 用户经验
	@lUnLockScore BIGINT,						-- 释放积分
	@dwPlayTimeCount INT,						-- 游戏时间
	@strClientIP NVARCHAR(15),					-- 连接地址
	@wKindID INT,								-- 游戏 I D
	@wServerID INT,								-- 房间 I D	
	@dwInoutIndex INT
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 用户分数
DECLARE @WinScore BIGINT
DECLARE @LostScore BIGINT

-- 执行逻辑
BEGIN

	-- 写分日统计
	DECLARE @DateID INT
	SET @DateID = CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 初始化
	SELECT @LostScore=0, @WinScore=0

	-- 设置分数
	--IF @lScore>=0  
	--	SET @WinScore=@lScore		
	--ELSE 
	--	SET @LostScore=@lScore	

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 变量定义
	DECLARE @ClubID INT
	DECLARE @BattleNum INT
	DECLARE @ScoreKind TINYINT

	-- 查询约战信息
	SELECT @ClubID=ClubID,@BattleNum=BattleNum,@ScoreKind=ScoreKind FROM RecordUserInout(NOLOCK) WHERE ID=@dwInoutIndex
	
	-- 茶馆分类型
	IF @ScoreKind=2
	BEGIN
		-- 变量定义
		DECLARE	@TableID INT									
		DECLARE @Sqlstr	NVARCHAR(512)						
		DECLARE	@TableName NVARCHAR(128)
				
		-- 分表标识				
		SET	@TableID=dbo.LHF_GetTableID(@ClubID);

		-- 群组成员表
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));				

		-- 查询脚本		
		SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName,' SET ClubScore=ClubScore+',CONVERT(NVARCHAR(18), @lScore),',RevenueCount=RevenueCount+',CONVERT(NVARCHAR(18), @lRevenue),
							  ' WHERE MemberID=',CONVERT(NVARCHAR(18), @dwUserID),' AND ClubID=',CONVERT(NVARCHAR(18), @ClubID))

		-- 执行脚本
		EXECUTE(@Sqlstr)	
		IF @@ERROR<>0
		BEGIN
			-- 事务回滚
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
			RETURN 1
		END		
		
		-- 游戏信息
		UPDATE GameScoreInfo SET WinCount+=@lWinCount,LostCount+=@lLostCount, DrawCount+=@lDrawCount, FleeCount+=@lFleeCount,WinStreak=@lWinStreak, MaxWinStreak=@lMaxWinStreak WHERE UserID=@dwUserID
	END	

	-- 约战积分类型
	IF @ScoreKind=3
	BEGIN
		-- 游戏信息
		UPDATE GameScoreInfo SET Score+=@lScore,WinCount+=@lWinCount,LostCount+=@lLostCount, DrawCount+=@lDrawCount, FleeCount+=@lFleeCount,WinStreak=@lWinStreak, MaxWinStreak=@lMaxWinStreak WHERE UserID=@dwUserID
	END		

	-- 分数统计
	UPDATE LHGameGoldDB..ReportUserScore SET GainScore+=@WinScore,LoseScore+=@LostScore,WinCount+=@lWinCount,LostCount+=@lLostCount,DrawCount+=@lDrawCount,FleeCount+=@lFleeCount,UserRevenue+=@lRevenue,
		PlayTimeCount+=@dwPlayTimeCount,ModifyDateTime=GETDATE() 
	WHERE DateID=@DateID AND UserID=@dwUserID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO LHGameGoldDB..ReportUserScore(DateID,UserID,GainScore,LoseScore,WinCount,LostCount,DrawCount,FleeCount,UserRevenue,PlayTimeCount,InsertDateTime,ModifyDateTime)
		VALUES (@DateID,@dwUserID,@WinScore,@LostScore,@lWinCount,@lLostCount,@lDrawCount,@lFleeCount,@lRevenue,@dwPlayTimeCount,GETDATE(),GETDATE())
	END

	-- 分数统计
	UPDATE LHGameGoldDB..ReportUserKind SET GainScore+=@WinScore,LoseScore+=@LostScore,WinCount+=@lWinCount,LostCount+=@lLostCount,DrawCount+=@lDrawCount,FleeCount+=@lFleeCount,UserRevenue+=@lRevenue,
		PlayTimeCount+=@dwPlayTimeCount,ModifyDateTime=GETDATE() 
	WHERE DateID=@DateID AND UserID=@dwUserID AND KindID=@wKindID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO LHGameGoldDB..ReportUserKind(DateID,UserID,KindID,GainScore,LoseScore,WinCount,LostCount,DrawCount,FleeCount,UserRevenue,PlayTimeCount,InsertDateTime,ModifyDateTime)
		VALUES (@DateID,@dwUserID,@wKindID,@WinScore,@LostScore,@lWinCount,@lLostCount,@lDrawCount,@lFleeCount,@lRevenue,@dwPlayTimeCount,GETDATE(),GETDATE())
	END

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 用户经验
	IF @lExperience>0
	BEGIN
		UPDATE LHAccountsDB.dbo.AccountsInfo SET Experience=Experience+@lExperience WHERE UserID=@dwUserID	
	END

	-- 写分日统计
	UPDATE StreamBattleKindInfo SET WinCount=WinCount+@lWinCount,LostCount=LostCount+@lLostCount,PlayTimeCount=PlayTimeCount+@dwPlayTimeCount
	FROM StreamBattleKindInfo WHERE DateID=@DateID AND UserID=@dwUserID AND KindID=@wKindID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO StreamBattleKindInfo(DateID,UserID,KindID,WinCount,LostCount,PlayTimeCount) 
		VALUES (@DateID,@dwUserID,@wKindID,@lWinCount,@lLostCount,@dwPlayTimeCount)
	END
END



RETURN 0

GO

----------------------------------------------------------------------------------------------------