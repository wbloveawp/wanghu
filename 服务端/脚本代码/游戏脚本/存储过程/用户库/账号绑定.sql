
USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_BindExternalAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_BindExternalAccounts]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- ���ⲿ�˺�
CREATE PROCEDURE GSP_MB_BindExternalAccounts
	@dwUserID			INT,					-- �û� I D
	@dwExternalID		INT,					-- ������ʶ
	@strUniqueID		NVARCHAR(32),			-- Ψһ��ʶ
	@strFacePath		NVARCHAR(256),			-- ͷ���ַ
	@strUserDatum		NVARCHAR(512),			-- �û�����	
	@strClientIP		NVARCHAR(15),			-- ���ӵ�ַ	
	@strErrorDescribe	NVARCHAR(127) OUTPUT	-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �������
DECLARE @UserID			INT
DECLARE @Gender			TINYINT
DECLARE @NickName		NVARCHAR(32)
DECLARE @Nullity		BIT
DECLARE @ShutDown		BIT
DECLARE @ProtectID		INT
DECLARE @LogonPass		NCHAR(32)
DECLARE @MoorMachine	TINYINT
DECLARE	@LogonMachineID	NCHAR(34)

-- ִ���߼�
BEGIN	
	-- ��ѯ�û�	
	SELECT @UserID=a.UserID, @Gender=a.Gender, @NickName=a.NickName, @ProtectID=a.ProtectID, @LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus, @MoorMachine=a.MoorMachine, 
			@LogonMachineID=a.LogonMachineID 
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����"}'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�"}'
		RETURN 3
	END	

	-- ��ѯ����
	DECLARE @BindUserID INT
	SELECT @BindUserID=UserID FROM AccountsExternal(NOLOCK) WHERE UniqueID=@strUniqueID AND ExternalID=@dwExternalID

	-- �Ѿ���
	IF @BindUserID IS NOT NULL
	BEGIN
		-- �ظ���
		IF @BindUserID=@dwUserID
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1303,"Msg":"���Ѿ����˵�ǰ�˺�,�����ظ��󶨣�"}'
			RETURN 4
		END ELSE
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1304,"Msg":"��ǰ�˺��Ѿ�������������Ϸ�˺ţ�"}'
			RETURN 5
		END
	END

	-- ��ѯ����
	DECLARE @UniqueID NVARCHAR(32)
	SELECT @UniqueID=UniqueID FROM AccountsExternal(NOLOCK) WHERE UserID=@dwUserID

	-- �Ѿ���
	IF @UniqueID IS NOT NULL
	BEGIN
		-- �ظ���		
		SET @strErrorDescribe=N'{"Idx":1303,"Msg":"���Ѿ����˵�ǰ�˺�,�����ظ��󶨣�"}'
		RETURN 6		
	END

	-- �����˺�
	INSERT INTO AccountsExternal(UniqueID,ExternalID,UserID,PublicID,NickName,Gender,UserDatum,UserFacePath,LogonClientIP,LogonDateTime,SubscribeFlag,SubscribeTime,CollectDateTime)
	VALUES (@strUniqueID,@dwExternalID,@UserID,N'',@NickName,@Gender,@strUserDatum,@strFacePath,@strClientIP,GETDATE(),1,GETDATE(),GETDATE())	

	-- �ɹ���ʾ
	SET @strErrorDescribe=N'{"Idx":1305,"Msg":"�˺Ű󶨳ɹ���"}'

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------