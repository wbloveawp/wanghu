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

-- �����û�
CREATE PROC GSP_GR_LockBattleUser
	-- Լս��Ϣ	
	@dwBattleNum INT,							-- Լս����
	@cbSocreKind TINYINT,						-- ��������

	-- �û���Ϣ
	@strUserIDList NVARCHAR(128)				-- ��ʶ�б�
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ����״̬
	IF ISJSON(@strUserIDList)>0
	BEGIN
		UPDATE BattleUserLocker SET ScoreKind=@cbSocreKind,BattleNum=@dwBattleNum,BattleStatus=1,CollectDate=GetDate() 
		WHERE UserID IN (SELECT VALUE FROM OPENJSON(@strUserIDList))
	END

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �����û�
CREATE PROC GSP_GR_UnlockBattleUser
	@dwBattleNum INT,							-- Լս����	
	@strUserIDList NVARCHAR(128)				-- ��ʶ�б�
WITH ENCRYPTION AS

-- ��������
DECLARE @UserID INT
DECLARE @IndexCurr SMALLINT
DECLARE @IndexPrev SMALLINT
DECLARE @BattleNum INT

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ����״̬
	IF ISJSON(@strUserIDList)>0
	BEGIN
		UPDATE BattleUserLocker SET ScoreKind=0,BattleNum=0,BattleStatus=0 WHERE UserID IN (SELECT VALUE FROM OPENJSON(@strUserIDList))
	END
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------