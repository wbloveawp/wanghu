----------------------------------------------------------------------
-- ʱ�䣺2021-11-01
-- ��;���ʼ�����
----------------------------------------------------------------------

USE [LHAccountsDB]
GO

-- �����ʼ�
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].WEB_PW_SendMail') AND OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].WEB_PW_SendMail
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

----------------------------------------------------------------------------------
-- �����ʼ�
CREATE PROCEDURE WEB_PW_SendMail
	@dwUserID INT,								-- �û���ʶ
	@dwMailKind	TINYINT,						-- �ʼ����ͣ�1�� ֪ͨ����  2�� �������ͣ�	
	@dwMailType TINYINT,						-- �ʼ����ࣨ1���ƹ㽱���ʼ� 2�����������ʼ� 3����λ�����ʼ� 4��ϵͳ�����ʼ���
	@strMailTitle NVARCHAR(32),					-- �ʼ�����
	@strMailContent	NVARCHAR(512),				-- �ʼ�����
	@dwSendTime	INT,							-- ����ʱ���
	@dwExpireTime INT,							-- ����ʱ���

	@dwGoodsID	INT,							-- ��Ʒ��ʶ
	@dwGoodsCount INT,							-- ��Ʒ����

	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID			INT
DECLARE @Nullity		TINYINT

-- ִ���߼�
BEGIN
	-- ��ѯ�û�	
	SELECT @UserID=UserID,@Nullity=Nullity FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
	
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

	-- �����ʼ�����
	INSERT INTO AccountsMailInfo(MailKind,MailType,MailTitle,MailContent,SendTime,ExpireTime,CollectTime) VALUES(@dwMailKind,@dwMailType,@strMailTitle,@strMailContent,@dwSendTime,@dwExpireTime,GETDATE())

	-- ��¼��ʶ
	DECLARE @MailID INT
	SET @MailID=SCOPE_IDENTITY()

	-- �����û��ʼ�
	INSERT INTO AccountsMailBox(UserID,MailID,MailState,ExpireTime,UpdateTime) VALUES(@dwUserID,@MailID,1,@dwExpireTime,GETDATE())

	-- ���븽��
	IF @dwMailKind=2
	BEGIN
		INSERT INTO AccountsMailAttach(MailID,GoodsID,GoodsLevel,GoodsCount,GoodsIndate) VALUES(@MailID,@dwGoodsID,0,@dwGoodsCount,0)
	END

	-- �������
	SELECT @MailID AS MailID,@UserID AS UserID
END
RETURN 0
GO