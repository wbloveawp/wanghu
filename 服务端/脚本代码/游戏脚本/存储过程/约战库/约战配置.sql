
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadBattleReward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadBattleReward]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadBattleBaseOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadBattleBaseOption]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadBattleOptionItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadBattleOptionItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadBattleGameOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadBattleGameOption]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- 加载奖励
CREATE  PROCEDURE dbo.GSP_GR_LoadBattleReward
	@wKindID SMALLINT					-- 游戏类型
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
		
	-- 查询数据
	SELECT * FROM BattleReward WHERE OptionID IN (SELECT OptionID FROM BattleBaseOption WHERE KindID=@wKindID)
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载子项
CREATE  PROCEDURE dbo.GSP_GR_LoadBattleOptionItem
	@wOptionID SMALLINT,					-- 配置标识
	@wItemCount TINYINT						-- 子项数目
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
		
	-- 查询数据
	SELECT * FROM BattleOptionItem WHERE OptionID=@wOptionID AND ItemIndex<@wItemCount
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载选项
CREATE  PROCEDURE dbo.GSP_GR_LoadBattleBaseOption	
	@wKindID SMALLINT						-- 类型标识			
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
		
	-- 查询数据
	SELECT * FROM BattleBaseOption WHERE (@wKindID=0 OR KindID=@wKindID) AND Nullity=0
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------


-- 加载配置
CREATE  PROCEDURE dbo.GSP_GR_LoadBattleGameOption	
	@wKindID SMALLINT						-- 类型标识		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
		
	-- 查询数据
	SELECT * FROM BattleGameOption WHERE (@wKindID=0 OR KindID=@wKindID)
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


