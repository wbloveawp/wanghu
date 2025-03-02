----------------------------------------------------------------------------------------------------
-- ʱ�䣺2021-11-01
-- ��;���ֻ���¼���ݻ�ȡ
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].WEB_PW_GetLoginLater') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].WEB_PW_GetLoginLater
GO

SET QUOTED_IDENTIFIER ON
GO

SET ANSI_NULLS ON
GO

----------------------------------------------------------------------------------------------------

CREATE PROCEDURE WEB_PW_GetLoginLater
	@dwUserID INT,								-- �û���ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID			INT
DECLARE @GameID			INT
DECLARE @Nullity		TINYINT
DECLARE @AgentID		INT
DECLARE @ParentID		INT
DECLARE @ParentGameID	INT
DECLARE @ParentUserID	INT

-- ִ���߼�
BEGIN
	-- ��ѯ�û�����
	SELECT @UserID=UserID,@GameID=GameID,@Nullity=Nullity,@AgentID=AgentID,@ParentID=ParentID FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
	
	-- �û�����
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�'
		RETURN 100
	END	

	-- �ʺŷ��
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 101
	END		

	SET @ParentGameID=0
	SET @ParentUserID=0
	IF @ParentID >0 
	BEGIN
		SELECT @ParentGameID=GameID,@ParentUserID=UserID FROM AccountsInfo(NOLOCK) WHERE UserID=@ParentID
	END

	-- �û���Ϣ
	SELECT @UserID AS UserID,@GameID AS GameID,@AgentID AS AgentID,@ParentGameID AS ParentGameID,@ParentUserID AS ParentUserID

	--����Ƿ��׳�
	SELECT OrderID FROM LHInsureDB.dbo.PayRecord(NOLOCK) WHERE UserID=@UserID
	
END

RETURN 0

GO
