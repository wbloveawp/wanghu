
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadDataBaseInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadDataBaseInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadDatabaseList]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadDatabaseList]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ������Ϣ
CREATE PROCEDURE GSP_LoadDataBaseInfo
	@wDBListID INT						-- ���ݿ�����
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ��������
DECLARE @DBPort INT
DECLARE @DBUser NVARCHAR(512)
DECLARE @DBPassword NVARCHAR(512)
DECLARE @DataBaseName VARCHAR(32)
DECLARE @DataBaseAddr VARCHAR(15)
DECLARE @DBInfoID INT
DECLARE @ErrorDescribe NVARCHAR(128)

-- ִ���߼�
BEGIN	
	-- ������Ϣ
	SELECT @DBPort=A.DBPort, @DBUser=A.DBUser, @DBPassword=A.DBPassword, @DataBaseName=B.DBAliasName,@DataBaseAddr=A.DBAddr
	FROM DataBaseInfo(NOLOCK) AS A, DataBaseList(NOLOCK) AS B
	WHERE B.DBListID = @wDBListID AND B.DBInfoID = A.DBInfoID

	-- �����ж�
	IF @@ROWCOUNT=0
	BEGIN
		SET @ErrorDescribe=N'���ݿ� [ '+STR(@wDBListID)+N' ] ������Ϣ�����ڣ����� LHPlatformDB ���ݿ�� DataBaseList �� DataBaseInfo ������'
		SELECT [ErrorDescribe]=@ErrorDescribe
		RETURN 1
	END

	-- �������
	SELECT @DataBaseName AS DataBaseName, @DataBaseAddr AS DataBaseAddr, @DBPort AS DBPort, @DBUser AS DBUser, @DBPassword AS DBPassword
	
END

RETURN 0

----------------------------------------------------------------------------------------------------
GO

----------------------------------------------------------------------------------------------------

-- ��¼��֤
CREATE  PROCEDURE dbo.GSP_LoadDatabaseList
	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON


-- ִ���߼�
BEGIN
	SELECT * FROM DataBaseList(NOLOCK) WHERE DBGenre = 1
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
