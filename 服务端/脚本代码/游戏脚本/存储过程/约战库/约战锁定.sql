USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LockBattleUser]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LockBattleUser]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UnlockBattleUser]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UnlockBattleUser]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 锁定用户
CREATE PROC GSP_GR_LockBattleUser
	-- 约战信息	
	@dwBattleNum INT,							-- 约战房号
	@cbSocreKind TINYINT,						-- 分数类型

	-- 用户信息
	@strUserIDList NVARCHAR(128)				-- 标识列表
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 更新状态
	IF ISJSON(@strUserIDList)>0
	BEGIN
		UPDATE BattleUserLocker SET ScoreKind=@cbSocreKind,BattleNum=@dwBattleNum,BattleStatus=1,CollectDate=GetDate() 
		WHERE UserID IN (SELECT VALUE FROM OPENJSON(@strUserIDList))
	END

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 解锁用户
CREATE PROC GSP_GR_UnlockBattleUser
	@dwBattleNum INT,							-- 约战房号	
	@strUserIDList NVARCHAR(128)				-- 标识列表
WITH ENCRYPTION AS

-- 变量定义
DECLARE @UserID INT
DECLARE @IndexCurr SMALLINT
DECLARE @IndexPrev SMALLINT
DECLARE @BattleNum INT

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 更新状态
	IF ISJSON(@strUserIDList)>0
	BEGIN
		UPDATE BattleUserLocker SET ScoreKind=0,BattleNum=0,BattleStatus=0 WHERE UserID IN (SELECT VALUE FROM OPENJSON(@strUserIDList))
	END
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------