
----------------------------------------------------------------------------------------------------

USE LHGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadMatchOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadMatchOption]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_DeleteMatchOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_DeleteMatchOption]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_RemoveMatchOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_RemoveMatchOption]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_InsertMatchOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_InsertMatchOption]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_ModifyMatchOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_ModifyMatchOption]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 加载比赛
CREATE  PROCEDURE dbo.GSP_GS_LoadMatchOption
	@wKindID   INT,								-- 类型标识	
	@strServiceMachine NCHAR(32),				-- 服务机器
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 加载比赛
	SELECT * FROM MatchOption WHERE ((KindID=@wKindID AND @wKindID<>0) OR @wKindID=0) AND ServiceMachine=@strServiceMachine AND Nullity=0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 删除比赛
CREATE  PROCEDURE dbo.GSP_GS_DeleteMatchOption	
	@dwMatchID INT								-- 比赛标识	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 删除配置
	DELETE MatchOption WHERE MatchID=@dwMatchID			
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 移除比赛
CREATE  PROCEDURE dbo.GSP_GS_RemoveMatchOption	
	@dwMatchID INT								-- 比赛标识	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 删除配置
	UPDATE MatchOption SET Nullity=1 WHERE MatchID=@dwMatchID			
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 插入比赛
CREATE  PROCEDURE dbo.GSP_GS_InsertMatchOption	
	-- 基本信息
	@wKindID INT,								-- 类型标识
	@dwGroupID BIGINT,							-- 茶馆标识
	@strModuleName NVARCHAR(32),				-- 模块名称		
	@strMatchName NVARCHAR(32),					-- 比赛名称
	@strMatchDate NVARCHAR(64),					-- 比赛时间	
	@cbShowType TINYINT,						-- 展示类型		
	@cbMatchType TINYINT,						-- 比赛类型		
	
	-- 报名信息
	@wFeeMoneyID SMALLINT,						-- 货币类型	
	@lFeeAmount BIGINT,							-- 费用金额
	@wUseTicketCount SMALLINT,					-- 使用数量
	@wBuyTicketCount SMALLINT,					-- 购买数量
	@wBuyTicketOption SMALLINT,					-- 购票选项
	@lBuyTicketAmount BIGINT,					-- 货币金额
	@wBuyTicketMoneyID SMALLINT,				-- 货币类型
	@wSignupCount SMALLINT,						-- 报名次数
	@cbSignupMode TINYINT,						-- 报名方式
	@cbJoinCondition TINYINT,					-- 参赛条件
	@cbMemberOrder TINYINT,						-- 会员等级	
	@lExperience INT,							-- 经验等级	

	-- 比赛配桌
	@cbMatchDistribute VARBINARY(64),			-- 撮桌配置
		
	-- 比赛奖励
	@wRewardCount SMALLINT,						-- 奖励数目
	@cbMatchReward VARBINARY(MAX),				-- 比赛奖励

	-- 比赛轮次
	@wRoundCount SMALLINT,						-- 轮次数目
	@cbMatchRound VARBINARY(512),				-- 比赛轮次

	-- 扩展配置
	@cbMatchRule VARBINARY(512),				-- 比赛规则	
	@strServiceMachine NCHAR(32),				-- 服务机器
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 插入配置
	INSERT INTO MatchOption(KindID,GroupID,MatchName,ModuleName,MatchType,ShowType,SignupCount,SignupMode,FeeAmount,FeeMoneyID,UseTicketCount,BuyTicketCount,BuyTicketOption,BuyTicketAmount,BuyTicketMoneyID,JoinCondition,MemberOrder,
		Experience,MatchDistribute,MatchReward,RewardCount,MatchRound,RoundCount,MatchRule,ServiceMachine,CollectDate)		
	VALUES(@wKindID,@dwGroupID,@strMatchName,@strModuleName,@cbMatchType,@cbShowType,@wSignupCount,@cbSignupMode,@lFeeAmount,@wFeeMoneyID,@wUseTicketCount,@wBuyTicketCount,@wBuyTicketOption,@lBuyTicketAmount,@wBuyTicketMoneyID
		,@cbJoinCondition,@cbMemberOrder,@lExperience,@cbMatchDistribute,@cbMatchReward,@wRewardCount,@cbMatchRound,@wRoundCount,@cbMatchRule,@strServiceMachine,GETDATE())

	-- 比赛标识
	DECLARE @dwMatchID INT
	SET @dwMatchID=SCOPE_IDENTITY()	
	
	-- 存在判断
	IF NOT EXISTS(SELECT * FROM MatchOption WHERE MatchID=@dwMatchID) 
	BEGIN
		SET @strErrorDescribe=N'由于数据库异常，比赛创建失败，请稍后再试！'
		RETURN 1
	END
	
	-- 查询比赛	
	SELECT * FROM MatchOption WHERE MatchID=@dwMatchID	
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 修改比赛
CREATE  PROCEDURE dbo.GSP_GS_ModifyMatchOption
	-- 基本信息
	@wKindID INT,								-- 类型标识
	@dwMatchID INT,								-- 比赛标识
	@dwGroupID BIGINT,							-- 茶馆标识
	@strModuleName NVARCHAR(32),				-- 模块名称	
	@strMatchName NVARCHAR(32),					-- 比赛名称
	@strMatchDate NVARCHAR(64),					-- 比赛时间
	@cbShowType TINYINT,						-- 展示类型
	@cbMatchType TINYINT,						-- 比赛类型
	
	-- 报名信息
	@wFeeMoneyID SMALLINT,						-- 货币类型	
	@lFeeAmount BIGINT,							-- 费用金额
	@wUseTicketCount SMALLINT,					-- 使用数量
	@wBuyTicketCount SMALLINT,					-- 购买数量
	@wBuyTicketOption SMALLINT,					-- 购票选项
	@lBuyTicketAmount BIGINT,					-- 货币金额
	@wBuyTicketMoneyID SMALLINT,				-- 货币类型
	@wSignupCount SMALLINT,						-- 报名次数
	@cbSignupMode TINYINT,						-- 报名方式
	@cbJoinCondition TINYINT,					-- 参赛条件
	@cbMemberOrder TINYINT,						-- 会员等级	
	@lExperience INT,							-- 经验等级	
	
	-- 比赛配桌
	@cbMatchDistribute VARBINARY(64),			-- 撮桌配置

	-- 比赛奖励
	@wRewardCount SMALLINT,						-- 奖励数目
	@cbMatchReward VARBINARY(MAX),				-- 比赛奖励

	-- 比赛轮次
	@wRoundCount SMALLINT,						-- 轮次数目
	@cbMatchRound VARBINARY(512),				-- 比赛轮次

	-- 比赛规则
	@cbMatchRule  VARBINARY(256),				-- 比赛规则	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 更新配置
	UPDATE MatchOption 	SET KindID=@wKindID,GroupID=@dwGroupID,MatchName=@strMatchName,ModuleName=@strModuleName,MatchType=@cbMatchType,ShowType=@cbShowType,FeeAmount=@lFeeAmount,FeeMoneyID=@wFeeMoneyID,
		UseTicketCount=@wUseTicketCount,BuyTicketCount=@wBuyTicketCount,BuyTicketOption=@wBuyTicketOption,BuyTicketAmount=@lBuyTicketAmount,BuyTicketMoneyID=@wBuyTicketMoneyID,SignupCount=@wSignupCount,SignupMode=@cbSignupMode,
		JoinCondition=@cbJoinCondition, MemberOrder=@cbMemberOrder,Experience=@lExperience,MatchDistribute=@cbMatchDistribute,MatchReward=@cbMatchReward,RewardCount=@wRewardCount,MatchRound=@cbMatchRound,RoundCount=@wRoundCount,
		MatchRule=@cbMatchRule	WHERE MatchID=@dwMatchID	
	
	-- 查询比赛	
	SELECT * FROM MatchOption WHERE MatchID=@dwMatchID		
			
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
