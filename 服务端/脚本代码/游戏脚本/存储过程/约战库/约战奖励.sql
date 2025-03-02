USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_BattleReward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_BattleReward]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 写入奖励
CREATE PROC GSP_GR_BattleReward
	@dwUserID INT,								-- 用户 I D	
	@lRewardGold INT,							-- 奖励金币
	@wKindID INT,								-- 游戏 I D
	@wServerID INT								-- 房间 I D		
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

DECLARE @UserGold BIGINT

-- 执行逻辑
BEGIN
	
	-- 奖牌奖励
	IF @lRewardGold>0
	BEGIN
		-- 用户查询
		DECLARE @GameID INT                                                                                                                                                                                                         
		SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

		-- 查询金币			
		SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

		-- 变更记录
		INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID,GameID,TypeID,BeforeGold,ChangeGold,CollectClientIP,CollectDateTime)
		VALUES (@dwUserID,@GameID,16,@UserGold,@lRewardGold,N'0.0.0.0',GETDATE())

		-- 更新金币
		UPDATE LHGameGoldDB.dbo.GameScoreInfo SET Score+=@lRewardGold WHERE UserID=@dwUserID
	END
	
	-- 查询金币
	SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 调整数据
	IF @UserGold IS NULL SET @UserGold=0		

	-- 抛出数据
	SELECT @UserGold AS UserGold
END

RETURN 0
GO