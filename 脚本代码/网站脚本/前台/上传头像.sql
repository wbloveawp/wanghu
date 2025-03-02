
----------------------------------------------------------------------
-- ��;���޸�ͷ��
----------------------------------------------------------------------


----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WEB_FP_InsertWxFace]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[WEB_FP_InsertWxFace]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WEB_FP_GetUserFace]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[WEB_FP_GetUserFace]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------
-- ����ͷ��
CREATE PROCEDURE WEB_FP_InsertWxFace
	@dwUserID			INT,						-- �û���ʶ
	@strFacePath		NVARCHAR(300),				-- ͷ���ַ
	@strClientIP		NVARCHAR(15),               -- ���ӵ�ַ
	@strErrorDescribe	NVARCHAR(127) OUTPUT	    -- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID			INT
DECLARE @LogonPass		NVARCHAR(32)
DECLARE @Nullity		TINYINT
DECLARE @OldCustomID		INT

-- ִ���߼�
BEGIN
	-- ��ѯ�û�	
	SELECT @UserID=UserID,@LogonPass=LogonPass, @Nullity=Nullity,@OldCustomID=CustomID
	FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- �û�����
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
		RETURN 100
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͷ������˽���ϸ�����'
		RETURN 101
	END	


	-- ��ѯͷ��
	IF @OldCustomID<>0
	BEGIN
		DECLARE @FacePath NVARCHAR(256)
		SELECT @FacePath=FacePath FROM AccountsFace(NOLOCK) WHERE UserID=@UserID
		IF @FacePath=@strFacePath 
		BEGIN
			-- ���ؽ��
			SELECT @OldCustomID AS CustomID
			RETURN 0
		END
	END	

	-- ����ͷ��
	--INSERT AccountsFace (UserID,CustomFace,InsertAddr,InsertMachine,WxFace) 
	--VALUES (@dwUserID, '', @strClientIP, '',@strWxUrl)

	-- �����û�
	DECLARE @CustomID INT
	SELECT @CustomID=SCOPE_IDENTITY()
	UPDATE AccountsInfo SET CustomID=SCOPE_IDENTITY() WHERE UserID=@dwUserID

	-- ���ؽ��
	SELECT @CustomID AS CustomID
END

SET NOCOUNT OFF

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- ��ȡͷ��
CREATE PROCEDURE WEB_FP_GetUserFace
	@dwUserID			INT						-- �û���ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID			INT
DECLARE @NickName		NVARCHAR(31)
DECLARE @FaceID			INT
DECLARE @CustomID		INT
DECLARE @FacePath		NVARCHAR(256)

-- ִ���߼�
BEGIN
	-- ��ѯ�û�	
	SELECT @UserID=UserID,@NickName=NickName,@FaceID=FaceID, @CustomID=CustomID FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID	

	-- �û�����
	IF @UserID IS NULL 
	BEGIN
		SET @UserID=0
		SET @FaceID=0
		SET @CustomID=0
		SET @FacePath=''
	END	

	IF @CustomID<>0
	BEGIN
		SELECT @FacePath=FacePath FROM AccountsFace(NOLOCK) WHERE UserID=@UserID
		IF @FacePath IS NULL
		BEGIN
			SET @FacePath=''
		END
	END
	ELSE
	BEGIN
		SET @FacePath=''
	END	
	-- ���ؽ��
	SELECT @UserID AS UserID,@NickName AS NickName,@FaceID AS FaceID,@CustomID AS CustomID,@FacePath AS FacePath
END

SET NOCOUNT OFF

RETURN 0

GO