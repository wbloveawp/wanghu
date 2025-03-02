----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_PlayerBehaviorAnalysis]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_PlayerBehaviorAnalysis]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 玩家行为分析
CREATE PROC GSP_GP_PlayerBehaviorAnalysis	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 自动回滚
SET XACT_ABORT ON

-- 执行逻辑
BEGIN

	
	-- 用户列表
	DECLARE @TableUserList TABLE( UserID INT NOT NULL)

	-- 当前时间
	DECLARE @CurrentTime DATETIME
	SET @CurrentTime = DATEADD(dd,-1,GETDATE())

	-- 筛选单笔金额大于等于100的玩家
	INSERT INTO @TableUserList SELECT UserID FROM LHInsureDB.dbo.PayRecord(NOLOCK) WHERE DATEDIFF(dd,PayDateTime,@CurrentTime)=0 AND PayAmount>=100.00

	-- 筛选单日充值总金额大于等于200的玩家
	INSERT INTO @TableUserList SELECT UserID FROM LHInsureDB.dbo.PayRecord(NOLOCK) WHERE DATEDIFF(dd,PayDateTime,@CurrentTime)=0 GROUP BY UserID HAVING SUM(PayAmount)>=200.00

	-- 筛选单次下注超过10，且累计15次以上的玩家
	INSERT INTO @TableUserList SELECT UserID FROM LHGameGoldDB.dbo.RecordDrawScore(NOLOCK) WHERE DATEDIFF(dd,RecordDate,@CurrentTime)=0 AND BetAmount>=10000 GROUP BY UserID HAVING COUNT(RecordID)>=15
	
	-- 打上大R标签
	UPDATE AccountsInfo SET LabelMask|=0x0001 WHERE UserID IN (SELECT UserID FROM @TableUserList GROUP BY UserID)

	--SELECT UserID FROM @TableUserList GROUP BY UserID	

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

