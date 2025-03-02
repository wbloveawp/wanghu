
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_ValidateAccredit]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_ValidateAccredit]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��¼��֤
CREATE  PROCEDURE dbo.GSP_ValidateAccredit
	@AccreditID NVARCHAR(32),					-- ��Ȩ I D
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineSerial NCHAR(32),				-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ��������
DECLARE @StationID INT
DECLARE @StationName VARCHAR(32)
DECLARE @DataBaseName VARCHAR(32)
DECLARE @DataBaseAddr VARCHAR(15)

-- ������Ϣ
DECLARE @DBPort INT
DECLARE @DBUser NVARCHAR(512)
DECLARE @DBPassword NVARCHAR(512)

DECLARE @NativeDBListID INT

-- ִ���߼�
BEGIN

	---- ��ѯ����
	--DECLARE @Nullity BIT
	--SELECT @StationID=StationID, @StationName=StationName, @NativeDBListID = NativeDBListID, @Nullity=Nullity 
	--FROM GameStationInfo(NOLOCK)
	--WHERE AccreditID=@AccreditID

	---- ��ѯ�û�
	--IF @StationID IS NULL
	--BEGIN
	--	SET @strErrorDescribe=N'ϵͳ��Ȩ�Ų����ڻ��������������֤���ٴγ��ԣ�'
	--	RETURN 1
	--END	

	---- �ʺŽ�ֹ
	--IF @Nullity<>0
	--BEGIN
	--	SET @strErrorDescribe=N'��ϵͳ��Ȩ����ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
	--	RETURN 2
	--END

	---- �������
	--SELECT @StationID AS StationID, @StationName AS StationName

	RETURN 0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
