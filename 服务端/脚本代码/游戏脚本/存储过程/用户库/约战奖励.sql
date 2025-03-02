USE LHAccountsDB
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
	@dwRewardMedal INT,							-- 奖励奖牌
	@wKindID INT,								-- 游戏 I D
	@wServerID INT,								-- 房间 I D	
	@strClientIP NVARCHAR(15)					-- 连接地址
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	---- 奖牌奖励
	--IF @dwRewardMedal>0
	--BEGIN
	--	-- 查询奖牌
	--	DECLARE @UserMedal INT 
	--	SELECT @UserMedal=UserMedal FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID		
			
	--	-- 用户不存在
	--	IF 	@UserMedal IS NULL RETURN 1

	--	-- 写入记录
	--	INSERT INTO RecordMedalChange(UserID,SrcMedal,TradeMedal,TypeID,ClientIP) VALUES(@dwUserID,@UserMedal,@dwRewardMedal,1,@strClientIP)

	--	-- 更新奖牌
	--	UPDATE AccountsInfo SET UserMedal=UserMedal+@dwRewardMedal WHERE UserID=@dwUserID	
		
	--	-- 统计奖牌
	--	DECLARE @DateID	INT
	--	SET @DateID = CAST(CAST(GETDATE() AS FLOAT) AS INT)	
	--	UPDATE StreamMedalInfo SET RewardMedal=RewardMedal+@dwRewardMedal,LastCollectDate=GETDATE() WHERE DateID=@DateID AND UserID=@dwUserID AND KindID=@wKindID
	--	IF @@ROWCOUNT=0
	--	BEGIN
	--		INSERT INTO StreamMedalInfo(DateID,UserID,KindID,RewardMedal) VALUES(@DateID,@dwUserID,@wKindID,@dwRewardMedal)
	--	END 
	--END
	
	---- 查询奖牌
	--DECLARE @CurrMedal BIGINT	
	--SELECT @CurrMedal=UserMedal FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID		

	---- 调整数据
	--IF @CurrMedal IS NULL SET @CurrMedal=0		

	---- 抛出数据
	--SELECT @CurrMedal AS CurrMedal

	RETURN 0
END

RETURN 0
GO
