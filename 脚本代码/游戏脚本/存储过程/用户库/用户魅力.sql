
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryUserLoveLiness]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryUserLoveLiness]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryUserLoveLiness]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryUserLoveLiness]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��ѯ����
CREATE PROC GSP_GP_QueryUserLoveLiness

	-- �û���Ϣ
	@dwUserID INT								-- �û���ʶ	

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	DECLARE @LoveLiness INT
	SELECT @LoveLiness=LoveLiness FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- ��������
	IF @LoveLiness IS NULL SET @LoveLiness=0
	
	-- �׳���¼
	SELECT @LoveLiness AS LoveLiness

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ѯ����
CREATE PROC GSP_GR_QueryUserLoveLiness

	-- �û���Ϣ
	@dwUserID INT								-- �û���ʶ	

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	DECLARE @LoveLiness INT
	SELECT @LoveLiness=LoveLiness FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- ��������
	IF @LoveLiness IS NULL SET @LoveLiness=0
	
	-- �׳���¼
	SELECT @LoveLiness AS LoveLiness

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------