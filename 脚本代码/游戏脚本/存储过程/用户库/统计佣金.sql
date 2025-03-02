----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_StatisticsCommission]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_StatisticsCommission]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 统计佣金
CREATE PROC GSP_GR_StatisticsCommission	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 自动回滚
SET XACT_ABORT ON

-- 执行逻辑
BEGIN

	-- 写分日统计
	DECLARE @DateID INT
	SET @DateID = CAST(CAST(DATEADD(dd, -1, GETDATE()) AS FLOAT) AS INT)

	-- 高级代理配置
	DECLARE @AgentOption NVARCHAR(MAX)
	DECLARE @LowerBetAwardRate INT
	DECLARE @LowerPayAwardAmount INT
	DECLARE @LLowerBetAwardRate INT
	DECLARE @LLowerPayAwardAmount INT
	DECLARE @UpgradeNeedCommission BIGINT

	-- 读取配置
	SELECT @AgentOption=StatusValue FROM LHPlatformDB.dbo.SystemStatusInfo WHERE StatusKind=N'AgentOption' AND StatusName=N'AgentLevel_'+CONVERT(NVARCHAR(2),1)

	-- 调整配置
	IF ISJSON(@AgentOption)>0
	BEGIN
		SET @LowerBetAwardRate=CONVERT(BIGINT,ISNULL(JSON_VALUE(@AgentOption,N'$.LowerBetAwardRate'),N'0'))
		SET @LowerPayAwardAmount=CONVERT(BIGINT,ISNULL(JSON_VALUE(@AgentOption,N'$.LowerPayAwardAmount'),N'0'))
		SET @LLowerBetAwardRate=CONVERT(BIGINT,ISNULL(JSON_VALUE(@AgentOption,N'$.LLowerBetAwardRate'),N'0'))
		SET @LLowerPayAwardAmount=CONVERT(BIGINT,ISNULL(JSON_VALUE(@AgentOption,N'$.LLowerPayAwardAmount'),N'0'))
		SET @UpgradeNeedCommission=CONVERT(BIGINT,ISNULL(JSON_VALUE(@AgentOption,N'$.NeedCommissionAmount'),N'0'))
	END

	-- 调整变量
	IF @LowerBetAwardRate IS NULL SET @LowerBetAwardRate=0
	IF @LowerPayAwardAmount IS NULL SET @LowerPayAwardAmount=0
	IF @LLowerBetAwardRate IS NULL SET @LLowerBetAwardRate=0
	IF @LLowerPayAwardAmount IS NULL SET @LLowerPayAwardAmount=0
	IF @UpgradeNeedCommission IS NULL SET @UpgradeNeedCommission=0

	-- 定义表变量
	DECLARE @TableAgentCommission TABLE
	(
		UserID INT NOT NULL PRIMARY KEY,
		LowerPayAmount BIGINT,
		LowerBetAmount BIGINT,
		LLowerPayAmount BIGINT,
		LLowerBetAmount BIGINT,
		LowerPayContribute BIGINT,
		LowerBetContribute BIGINT,
		LLowerPayContribute BIGINT,
		LLowerBetContribute BIGINT,
		Commission BIGINT
	)

	-- 插入数据
	INSERT INTO @TableAgentCommission
	SELECT 
		a.UserID,
		SUM(a.LowerPayAmount) AS LowerPayAmount, 
		SUM(a.LowerBetAmount) AS LowerBetAmount, 
		SUM(a.LLowerPayAmount) AS LLowerPayAmount, 
		SUM(a.LLowerBetAmount) AS LLowerBetAmount, 
		SUM(a.LowerPayContribute) AS LowerPayContribute, 
		SUM(a.LowerBetContribute) AS LowerBetContribute,
		SUM(a.LLowerPayContribute) AS LLowerPayContribute, 
		SUM(a.LLowerBetContribute) AS LLowerBetContribute,
		SUM(a.LowerPayContribute)+SUM(a.LowerBetContribute)+SUM(a.LLowerPayContribute)+SUM(a.LLowerBetContribute) AS Commission		
	FROM
	  ((SELECT 
			ParentID AS UserID,
			SUM(ParentPayAmount) AS LowerPayAmount, 
			SUM(ParentBetAmount) AS LowerBetAmount, 
			0 AS LLowerPayAmount,
			0 AS LLowerBetAmount,
			SUM(ParentPayContribute) AS LowerPayContribute, 
			SUM(ParentBetContribute) AS LowerBetContribute, 
			0 AS LLowerPayContribute, 
			0 AS LLowerBetContribute 
		FROM LHGameGoldDB.dbo.ReportUserCommission  
		WHERE DateID=@DateID AND ParentID<>0 GROUP BY ParentID)
		UNION ALL
		(SELECT 
			PParentID AS UserID,
			0 AS LowerPayAmount,
			0 AS LowerBetAmount,
			SUM(PParentPayAmount) AS LLowerPayAmount, 
			SUM(PParentBetAmount) AS LLowerBetAmount, 	
			0 AS LowerPayContribute, 
			0 AS LowerBetContribute, 
			SUM(PParentPayContribute) AS LLowerPayContribute, 
			SUM(PParentBetContribute) AS LLowerBetContribute 
		FROM LHGameGoldDB.dbo.ReportUserCommission 
		WHERE DateID=@DateID AND PParentID<>0 GROUP BY PParentID)) a
	GROUP BY a.UserID

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 插入统计数据
	INSERT INTO LHGameRecordDB.dbo.ReportDayCommission (
		DateID,
		UserID,
		Commission,
		LowerPayAmount,
		LowerBetAmount,
		LLowerPayAmount,
		LLowerBetAmount,
		LowerPayContribute,
		LowerBetContribute,
		LLowerPayContribute,
		LLowerBetContribute,
		TotalPayAmount,
		TotalBetAmount,
		TotalPayContribute,
		TotalBetContribute
	) 
	SELECT 
		@DateID,
		UserID,
		Commission,
		LowerPayAmount,
		LowerBetAmount,
		LLowerPayAmount,
		LLowerBetAmount,
		LowerPayContribute,
		LowerBetContribute,
		LLowerPayContribute,
		LLowerBetContribute,
		LowerPayAmount+LLowerPayAmount,
		LowerBetAmount+LLowerBetAmount,
		LowerPayContribute+LLowerPayContribute,
		LowerBetContribute+LLowerBetContribute
	FROM @TableAgentCommission

	-- 更新状态
	UPDATE LHGameGoldDB.dbo.ReportUserCommission SET BalanceState=1 WHERE DateID=@DateID
	
	-- 插入用户
	INSERT INTO AccountsCommission(UserID,Commission,CollectDateTime) 
	(SELECT UserID,0,GetDate() FROM  @TableAgentCommission a WHERE (SELECT COUNT(1) AS Num FROM AccountsCommission(NOLOCK) b WHERE a.UserID=b.UserID)=0)

	-- 更新佣金
	UPDATE a SET a.CollectDateTime = GETDATE(),
				 --a.LowerPayAmount+= b.LowerPayAmount,
				 a.LowerBetAmount+= b.LowerBetAmount,
				 --a.LLowerPayAmount+= b.LLowerPayAmount,
				 a.LLowerBetAmount+= b.LLowerBetAmount,				 
				 --a.LowerPayContribute+= b.LowerPayContribute,
				 a.LowerBetContribute+= b.LowerBetContribute,
				 --a.LLowerPayContribute+= b.LLowerPayContribute,
				 a.LLowerBetContribute+= b.LLowerBetContribute,
				 --a.TotalPayAmount+= b.LowerPayAmount+b.LLowerPayAmount,
				 a.TotalBetAmount+= b.LowerBetAmount+b.LLowerBetAmount,
				 --a.TotalPayContribute+= b.LowerPayContribute+b.LLowerPayContribute,
				 a.TotalBetContribute+= b.LowerBetContribute+b.LLowerBetContribute,				 
				 --a.Commission+=b.LowerPayContribute+b.LowerBetContribute+b.LLowerPayContribute+b.LLowerBetContribute
				 a.Commission+=b.LowerBetContribute+b.LLowerBetContribute
	FROM AccountsCommission a INNER JOIN @TableAgentCommission b ON a.UserID=b.UserID

	-- 升级处理
	IF @UpgradeNeedCommission>0
	BEGIN
		UPDATE AccountsAgentInfo SET AgentLevel=1,LowerBetAwardRate=@LowerBetAwardRate,LowerPayAwardAmount=@LowerPayAwardAmount,LLowerBetAwardRate=@LLowerBetAwardRate,
			   LLowerPayAwardAmount=@LLowerPayAwardAmount 
		WHERE UserID IN (SELECT UserID FROM AccountsCommission(NOLOCK) WHERE Commission>=@UpgradeNeedCommission) AND AgentLevel=0
	END
	
	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
