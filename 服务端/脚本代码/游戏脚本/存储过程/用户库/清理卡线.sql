/***************************************************************************************************
** 名称：帐号登录 (游戏)
***************************************************************************************************/
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_CleanOnlineInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_CleanOnlineInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_CleanOnlineLogonInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_CleanOnlineLogonInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- 清理卡线
CREATE PROC GSP_GP_CleanOnlineInfo
	@dwUserID INT								-- 用户标识	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 更新数据
	UPDATE AccountsOnlineInfo SET KindID=0,ServerID=0,ServerKind=0,ModifyTime=GETDATE() WHERE UserID=@dwUserID	
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 清理登陆信息
CREATE PROC GSP_GP_CleanOnlineLogonInfo
	@wLogonID SMALLINT								-- 登陆标识	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 更新数据
	UPDATE AccountsOnlineInfo SET GateID=0,LogonID=0,SocketID=0,ModifyTime=GETDATE() WHERE LogonID=@wLogonID	
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------