
USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_SubmitInformInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_SubmitInformInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- �ύͶ��
CREATE PROCEDURE GSP_MB_SubmitInformInfo
	@cbInformKind		TINYINT,				-- Ͷ������
	@cbInformSource		TINYINT,				-- Ͷ����Դ
	@dwInformUserID		INT,					-- Ͷ�����
	@dwBeInformUserID	INT,					-- ��Ͷ�����
	@strAttachmentUrl	NVARCHAR(128),			-- ������ַ
	@strInformMessage	NVARCHAR(512),			-- Ͷ����Ϣ
	@strInformContent	NVARCHAR(512),			-- Ͷ������
	@strClientIP		NVARCHAR(15),			-- �ն˵�ַ
	@strErrorDescribe	NVARCHAR(127) OUTPUT	-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- �������
	DECLARE @UserID			INT
	DECLARE @Nullity		BIT
	DECLARE @ShutDown		BIT
	DECLARE @BeInformGameID	INT

	-- ��ѯ�û�	
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON  a.UserID=b.UserID WHERE a.UserID=@dwInformUserID

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

	-- ��ѯ�û�	
	SELECT @BeInformGameID=GameID FROM AccountsInfo(NOLOCK) WHERE UserID=@dwBeInformUserID

	-- У���û�
	IF @BeInformGameID IS NULL
	BEGIN
		--SET @strErrorDescribe=N'{"Idx":1306,"Msg":"��Ͷ�ߵ��û���Ϣ�����ڣ�"}'
		--RETURN 4	

		SET @BeInformGameID=0
	END	

	-- ��������
	INSERT INTO RecordInform(InformUserID,InformKindID,InformSource,InformMessage,InformContent,BeInformUserID,BeInformGameID,AttachmentUrl,ClientIP,CollectTime,[Status])
	VALUES (@dwInformUserID,@cbInformKind,@cbInformSource,@strInformMessage,@strInformContent,@dwBeInformUserID,@BeInformGameID,@strAttachmentUrl,@strClientIP,GetDate(),0)

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
