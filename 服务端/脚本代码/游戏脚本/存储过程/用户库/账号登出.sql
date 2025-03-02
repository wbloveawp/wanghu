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

-- �ǳ��˺�
CREATE PROCEDURE [GSP_MB_LogoutAccounts]
	@wGateID INT,								-- ���ر�ʶ	
	@wLogonID INT,								-- ��¼��ʶ
	@dwSocketID INT,							-- �����ʶ	
	@dwUserID INT								-- �û���ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ������Ϣ
	DECLARE @LockGateID INT
	DECLARE @LockLogonID INT
	DECLARE @LockSocketID INT	
	DECLARE @InsertTime DATETIME

	-- ��ѯ����
	SELECT @LockGateID=GateID,@LockLogonID=LogonID,@LockSocketID=SocketID,@InsertTime=InsertTime FROM AccountsOnlineInfo(NOLOCK) WHERE UserID=@dwUserID

	-- ��������
	IF @LockGateID IS NULL SET @LockGateID=0
	IF @LockLogonID IS NULL SET @LockLogonID=0
	IF @LockSocketID IS NULL SET @LockSocketID=0

	-- ��������
	IF @LockGateID=@wGateID AND @LockLogonID=@wLogonID AND @LockSocketID=@dwSocketID
	BEGIN		
		-- ��������
		UPDATE AccountsOnlineInfo SET GateID=0,LogonID=0,SocketID=0,ModifyTime=GETDATE() WHERE UserID=@dwUserID
		
		-- ��������
		DECLARE @LogoutDateID INT		
		DECLARE @OnlineTimeCount INT

		-- ���ñ���
		SET @LogoutDateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
		SET @OnlineTimeCount=DATEDIFF(s,@InsertTime,GETDATE())

		-- ����ʱ��
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