USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_LogoutAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_LogoutAccounts]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- 登出账号
CREATE PROCEDURE [GSP_MB_LogoutAccounts]
	@wGateID INT,								-- 网关标识	
	@wLogonID INT,								-- 登录标识
	@dwSocketID INT,							-- 网络标识	
	@dwUserID INT								-- 用户标识	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 在线信息
	DECLARE @LockGateID INT
	DECLARE @LockLogonID INT
	DECLARE @LockSocketID INT	
	DECLARE @InsertTime DATETIME

	-- 查询锁定
	SELECT @LockGateID=GateID,@LockLogonID=LogonID,@LockSocketID=SocketID,@InsertTime=InsertTime FROM AccountsOnlineInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 调整变量
	IF @LockGateID IS NULL SET @LockGateID=0
	IF @LockLogonID IS NULL SET @LockLogonID=0
	IF @LockSocketID IS NULL SET @LockSocketID=0

	-- 调整变量
	IF @LockGateID=@wGateID AND @LockLogonID=@wLogonID AND @LockSocketID=@dwSocketID
	BEGIN		
		-- 更新数据
		UPDATE AccountsOnlineInfo SET GateID=0,LogonID=0,SocketID=0,ModifyTime=GETDATE() WHERE UserID=@dwUserID
		
		-- 变量定义
		DECLARE @LogoutDateID INT		
		DECLARE @OnlineTimeCount INT

		-- 设置变量
		SET @LogoutDateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
		SET @OnlineTimeCount=DATEDIFF(s,@InsertTime,GETDATE())

		-- 更新时长
		UPDATE SystemStreamLogon SET OnlineTimeCount+=@OnlineTimeCount WHERE DateID=@LogoutDateID AND UserID=@dwUserID					

	END ELSE
	BEGIN	
		RETURN 1		
	END

	RETURN 0
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------