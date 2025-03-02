/***************************************************************************************************
** ���ƣ��ʺŵ�¼ (��Ϸ)
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

-- ������
CREATE PROC GSP_GP_CleanOnlineInfo
	@dwUserID INT								-- �û���ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��������
	UPDATE AccountsOnlineInfo SET KindID=0,ServerID=0,ServerKind=0,ModifyTime=GETDATE() WHERE UserID=@dwUserID	
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �����½��Ϣ
CREATE PROC GSP_GP_CleanOnlineLogonInfo
	@wLogonID SMALLINT								-- ��½��ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��������
	UPDATE AccountsOnlineInfo SET GateID=0,LogonID=0,SocketID=0,ModifyTime=GETDATE() WHERE LogonID=@wLogonID	
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------