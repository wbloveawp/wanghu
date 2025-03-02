----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LH_MailAttachExpire]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[LH_MailAttachExpire]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UserQueryMail]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UserQueryMail]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UserModifyMail]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UserModifyMail]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UserAppendMail]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UserAppendMail]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UserDeleteMail]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UserDeleteMail]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_InsertMailBox]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_InsertMailBox]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ClaimMailAttach]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ClaimMailAttach]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ClaimMailAttach]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ClaimMailAttach]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_CollectMailUser]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_CollectMailUser]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_BatchInsertMail]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_BatchInsertMail]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �ʼ�������Ч���
CREATE FUNCTION [dbo].[LH_MailAttachExpire]
(		
	@SendTime INT,
	@ClaimTime INT,
	@GoodsIndate INT
)
RETURNS INT
AS
BEGIN
	--
	IF @GoodsIndate > 0
	BEGIN
		IF @ClaimTime = 0
		BEGIN
			IF @GoodsIndate <= ((dbo.LHF_GetTimestamp()-@SendTime)/(24*3600)) RETURN 1
		END ELSE
		BEGIN
			IF @GoodsIndate <= ((@ClaimTime - @SendTime)/(24*3600)) RETURN 1
		END
	END

	RETURN 0

END

GO

----------------------------------------------------------------------------------------------------

-- �����ʼ�
CREATE PROC GSP_GP_UserQueryMail
	@dwUserID	INT,							-- �û���ʶ
	@dwMailID	INT,							-- �ʼ���ʶ
	@dwCurrentTime	INT							-- ��ǰʱ��
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ɾ���ʼ�
	--DELETE AccountsMailBox WHERE UserID=@dwUserID AND (MailState = 4 OR ExpireTime<@dwCurrentTime)
	
	-- ��ѯ�ʼ���Ϣ
	SELECT a.MailID,b.MailKind,b.MailType,a.MailState,b.SendTime,a.ExpireTime,b.MailTitle,b.MailContent,
	ISNULL((SELECT GoodsID,GoodsLevel AS [Level],GoodsCount AS [Count], GoodsIndate AS Indate,.dbo.LH_MailAttachExpire(b.SendTime,a.ClaimTime,GoodsIndate) AS Expire FROM AccountsMailAttach WHERE MailID=a.MailID FOR JSON AUTO),N'') AS AttachGoods
	FROM AccountsMailBox(NOLOCK) a, AccountsMailInfo(NOLOCK) b WHERE a.UserID=@dwUserID AND a.MailID>@dwMailID AND a.MailState<>4 AND (a.ExpireTime=0 OR a.ExpireTime>=@dwCurrentTime) AND a.MailID=b.MailID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �޸��ʼ�
CREATE PROC GSP_GP_UserModifyMail
	@dwUserID INT,							-- �û���ʶ
	@dwMailID INT,							-- �ʼ���ʶ
	@cbMailState TINYINT					-- �ʼ�״̬
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ����״̬
	UPDATE AccountsMailBox SET MailState=@cbMailState WHERE UserID=@dwUserID AND MailID=@dwMailID	

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ����ʼ�
CREATE PROC GSP_GP_UserAppendMail
	@cbMailKind			TINYINT,					-- �ʼ�����	
	@cbMailType			TINYINT,					-- �ʼ�����	
	@cbMailState		TINYINT,					-- �ʼ�״̬
	@dwSendTime			INT,						-- ����ʱ��
	@dwExpireTime		INT,						-- ����ʱ��
	@szMailTitle		NVARCHAR(64),				-- �ʼ�����
	@szMailContent		NVARCHAR(256),				-- �ʼ�����
	@szAttachGoods		NVARCHAR(MAX),				-- ������Ʒ
	@szReceiverID		NVARCHAR(MAX),				-- �û���ʶ
	@strErrorDescribe	NVARCHAR(127) OUTPUT		-- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- �����ʼ�
	INSERT INTO [dbo].[AccountsMailInfo](MailKind, MailType, MailTitle, MailContent, SendTime, ExpireTime, CollectTime)
    VALUES (@cbMailKind, @cbMailType, @szMailTitle, @szMailContent, @dwSendTime, @dwExpireTime, GetDate())

	-- ����ʧ��
	IF @@ROWCOUNT=0
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1300,"Msg":"���ݿ��쳣���ʼ��޷����룡"}'
		RETURN 1
	END

	-- �����ж�	
	DECLARE @MailID INT			
	SET @MailID = SCOPE_IDENTITY() 

	-- ���������
	DECLARE @AttachGoodsList TABLE (GoodsID SMALLINT,GoodsCount INT,GoodsIndate INT)

	-- �����¼
	IF ISJSON(@szAttachGoods)>0
	BEGIN
		INSERT AccountsMailAttach (MailID,GoodsID,GoodsLevel,GoodsCount,GoodsIndate) 
		SELECT @MailID,JSON_VALUE(VALUE,'$.GoodsID'),ISNULL(JSON_VALUE(VALUE,'$.Level'),0), CONVERT(INT,JSON_VALUE(VALUE,'$.Count')),CONVERT(INT,JSON_VALUE(VALUE,'$.Indate')) FROM OPENJSON(@szAttachGoods)		
	END
	
	-- ������Ϣ
	SELECT @MailID AS MailID
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
-- ɾ���ʼ�
CREATE PROC GSP_GP_UserDeleteMail
	@dwUserID INT,							-- �û���ʶ		
	@dwMailID INT,							-- �ʼ���ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT  -- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ����ɾ��
	IF @dwMailID=0
	BEGIN
		-- ɾ���ʼ�
		--DELETE AccountsMailBox WHERE UserID=@dwUserID AND MailState > 1

		UPDATE AccountsMailBox SET MailState=4 WHERE UserID=@dwUserID AND MailState > 1

	END ELSE
	BEGIN
		-- ��ѯ״̬
		DECLARE @MailState TINYINT
		SELECT @MailState=MailState FROM AccountsMailBox WITH(UPDLOCK)  WHERE UserID=@dwUserID AND MailID=@dwMailID
		
		IF @MailState IS NULL
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1297,"Msg":"ϵͳδ��ѯ�����ʼ���Ϣ��"}'
			RETURN 1
		END ELSE
		BEGIN
			IF @MailState=1
			BEGIN
				SET @strErrorDescribe=N'{"Idx":1298,"Msg":"���Ȳ����ʼ���"}'
				RETURN 2
			END
		END
		
		-- ɾ���ʼ�
		--DELETE AccountsMailBox WHERE UserID=@dwUserID AND MailID=@dwMailID
		UPDATE AccountsMailBox SET MailState=4 WHERE UserID=@dwUserID AND MailID=@dwMailID
	END
	
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GP_InsertMailBox
	@dwMailID	INT,								-- �ʼ���ʶ
	@cbMailState TINYINT,							-- �ʼ�״̬
	@dwExpireTime INT,								-- ����ʱ��
	@szReceiverID NVARCHAR(MAX)						-- �û��б�	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��������
	IF ISJSON(@szReceiverID)>0
	BEGIN
		INSERT INTO AccountsMailBox (UserID,MailID,MailState,ExpireTime,UpdateTime)
		SELECT VALUE, @dwMailID, @cbMailState, @dwExpireTime, GetDate() FROM OPENJSON(@szReceiverID)
	END
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- ��ȡ����
CREATE PROC GSP_GR_ClaimMailAttach
	@dwUserID INT,							-- �û���ʶ		
	@dwMailID INT,							-- �ʼ���ʶ	
	@strErrorDescribe NVARCHAR(127) OUTPUT  -- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��ѯ״̬
	DECLARE @MailKind TINYINT
	DECLARE @MailState TINYINT
	DECLARE @AttachGoods NVARCHAR(MAX)
	SELECT @MailKind=b.MailKind, @MailState=a.MailState FROM AccountsMailBox a, AccountsMailInfo b WHERE a.UserID=@dwUserID AND a.MailID=@dwMailID AND a.MailID=b.MailID
	
	--У���ʼ�
	IF @MailKind IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1297,"Msg":"ϵͳδ��ѯ�����ʼ���Ϣ��"}'
		RETURN 4
	END

	-- У������
	IF @MailKind<>2
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1299,"Msg":"���ʼ�û�п���ȡ�ĸ�����"}'
		RETURN 5
	END

	--У��״̬
	IF @MailState<>1
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1300,"Msg":"���ʼ�״̬�쳣������ϵ�ͷ��˽���ϸ�����"}'
		RETURN 6
	END

	-- ����״̬
	UPDATE AccountsMailBox SET MailState=4 WHERE UserID=@dwUserID AND MailID=@dwMailID	

	-- ��ѯ����
	SET  @AttachGoods=(SELECT GoodsID,GoodsLevel,GoodsCount,GoodsIndate FROM AccountsMailAttach WHERE MailID=@dwMailID FOR JSON AUTO)

	-- �׳���¼
	SELECT @AttachGoods AS AttachGoods
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- ��ȡ����
CREATE PROC GSP_GP_ClaimMailAttach
	@dwUserID INT,							-- �û���ʶ		
	@dwMailID INT,							-- �ʼ���ʶ
	@dwTimeStamp INT,						-- ʱ���		
	@wReason INT,							-- �仯ԭ��	
	@strErrorDescribe NVARCHAR(127) OUTPUT  -- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��ѯ״̬
	IF EXISTS(SELECT 1 FROM AccountsOnlineInfo(NOLOCK) WHERE UserID=@dwUserID AND ServerID<>0)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1301,"Msg":"ϵͳ��ѯ����������Ϸ�����У�������ȡʧ�ܣ�"}'
		RETURN 1
	END	

	-- ��ѯ״̬
	DECLARE @MailKind TINYINT
	DECLARE @MailType SMALLINT
	DECLARE @MailState TINYINT
	DECLARE @AttachGoods NVARCHAR(MAX)
	DECLARE @nSendTimestamp INT
	SELECT @MailKind=b.MailKind,@MailType=b.MailType,@nSendTimestamp=b.SendTime FROM AccountsMailBox a, AccountsMailInfo b WHERE a.UserID=@dwUserID AND a.MailID=@dwMailID AND a.MailID=b.MailID
	
	--У���ʼ�
	IF @MailKind IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1297,"Msg":"ϵϵͳδ��ѯ�����ʼ���Ϣ��"}'
		RETURN 4
	END

	-- У������
	IF @MailKind<>2
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1299,"Msg":"���ʼ�û�п���ȡ�ĸ�����"}'
		RETURN 5
	END

	-- ��������
	DECLARE @Index INT
	DECLARE @LoopCount INT	
	DECLARE @ResidueCount INT	

	-- ��Ʒ��Ϣ
	DECLARE @IncGoodsID INT
	DECLARE @IncGoodsLevel INT
	DECLARE @IncGoodsCount INT
	DECLARE @IncGoodsIndate INT		
	DECLARE @Expire INT

	-- ���������
	DECLARE @AttachGoodsList TABLE (ID INT identity(1,1),GoodsID SMALLINT,Level SMALLINT,Count INT,Indate INT,Expire SMALLINT)

	-- �����¼
	--IF ISJSON(@AttachGoods)>0
	--BEGIN
		INSERT @AttachGoodsList (GoodsID,Level,Count,Indate,Expire) 
		SELECT GoodsID,GoodsLevel,Goodscount,GoodsIndate,dbo.LH_MailAttachExpire(@nSendTimestamp,0,GoodsIndate) FROM AccountsMailAttach(NOLOCK) WHERE MailID=@dwMailID
	--END

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��ѯ״̬
	SELECT @MailState=MailState FROM AccountsMailBox WITH(UPDLOCK)  WHERE UserID=@dwUserID AND MailID=@dwMailID

	--У��״̬
	IF @MailState<>1
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1300,"Msg":"���ʼ�״̬�쳣������ϵ�ͷ��˽���ϸ�����"}'

		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		RETURN 5		
	END


	-- ��ʼ��
	SELECT @Index=1,@LoopCount=COUNT(1),@wReason+=@MailType FROM @AttachGoodsList
	
	-- ѭ������
	WHILE @Index<=@LoopCount
	BEGIN	
		-- ��ѯ��Ϣ
		SELECT @IncGoodsID=GoodsID,@IncGoodsLevel=Level,@IncGoodsCount=Count,@IncGoodsIndate=Indate,@Expire=Expire FROM @AttachGoodsList WHERE ID=@Index
		--��Ч����
		IF @Expire = 0
		BEGIN
			-- У����Ʒ����
			IF dbo.LHF_IsMoneyGoods(@IncGoodsID) = 1
			BEGIN

				-- д��Ƹ�
				EXEC GSP_GP_WriteGameWealth @dwUserID,@IncGoodsID,@wReason,@IncGoodsCount

			END ELSE
			BEGIN

				-- �����Ʒ
				EXEC @ResidueCount = GSP_GP_AppendGoodsItem @dwUserID,@wReason,@IncGoodsID,@IncGoodsLevel,@IncGoodsCount,@IncGoodsIndate,@dwTimeStamp			

				-- У������
				IF @ResidueCount>0
				BEGIN
					SET @strErrorDescribe=N'{"Idx":1301,"Msg":"������Ʒ�ѵ������ѳ����ޣ������ݺ��ٴγ��ԣ�"}'

					-- ����ع�
					ROLLBACK TRAN
					SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
					RETURN 5
				END		
			END
		END

		-- ��������
		SET @Index=@Index+1
	END	

	-- ����ʱ��
	DECLARE @ExpireTime INT
	SET @ExpireTime = dbo.LHF_GetTimestamp()+2*24*3600

	-- ����״̬ ʧЧʱ���Ϊ2���
	UPDATE AccountsMailBox SET MailState=3,ExpireTime=@ExpireTime,UpdateTime=GETDATE(),ClaimTime=dbo.LHF_GetTimestamp() WHERE UserID=@dwUserID AND MailID=@dwMailID		

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- ����JSON
	SET @AttachGoods = (SELECT GoodsID,Level,Count,Indate,Expire FROM @AttachGoodsList FOR JSON AUTO)

	-- �׳���¼
	SELECT @AttachGoods AS AttachGoods
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- �ռ��û�
CREATE PROC GSP_GP_CollectMailUser
	@dwMailID	INT,								-- �ʼ���ʶ
	@LastLogonTime DATETIME							-- ��¼ʱ��
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- У���ʼ�
	IF EXISTS (SELECT TOP 1 1 FROM PendingMailUser WHERE MailID=@dwMailID)
	BEGIN
		RETURN 0
	END

	-- �ռ��û�
	INSERT INTO PendingMailUser(MailID,UserID)
	SELECT @dwMailID,UserID FROM AccountsInfo(NOLOCK) WHERE LogonDate>=@LastLogonTime
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- ��������
CREATE PROC GSP_GP_BatchInsertMail
	@dwMailID	INT,								-- �ʼ���ʶ
	@cbMailState TINYINT,							-- �ʼ�״̬
	@dwExpireTime INT,								-- ����ʱ��
	@wUserCount INT									-- �û�����
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��������
	INSERT INTO AccountsMailBox(UserID,MailID,MailState,ExpireTime,UpdateTime)
	SELECT TOP(@wUserCount) UserID,@dwMailID,@cbMailState,@dwExpireTime,GETDATE() FROM PendingMailUser(NOLOCK) WHERE MailID=@dwMailID

	-- ɾ���û�
	DELETE PendingMailUser WHERE UserID IN (SELECT TOP(@wUserCount) UserID FROM PendingMailUser(NOLOCK) WHERE MailID=@dwMailID)

	-- ʣ������
	SELECT COUNT(1) AS ResidueCount FROM PendingMailUser(NOLOCK) WHERE MailID=@dwMailID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
