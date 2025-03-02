USE LHAccountsDB
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_DeleteFriend]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_DeleteFriend]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ApplyAddFriend]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ApplyAddFriend]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AgreeAddFriend]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AgreeAddFriend]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_RefuseAddFriend]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_RefuseAddFriend]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_SetupInfoPublic]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_SetupInfoPublic]
GO


----------------------------------------------------------------------------------------------------

-- ɾ������
CREATE PROC [dbo].[GSP_GP_DeleteFriend]
	@dwUserID			INT,					--�û�ID	
	@dwFriendID			INT,					--���ѱ�ʶ
	@strErrorDescribe	NVARCHAR(127) OUTPUT	--�����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �Զ��ع�
SET XACT_ABORT ON

BEGIN
	
	-- ��������
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	

	-- ��ѯ�û�
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
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
	DECLARE @UserShardIndex INT
	DECLARE @FriendShardIndex INT
	SELECT @UserShardIndex=CASE WHEN UserID=@dwUserID THEN FriendShardIndex ELSE @UserShardIndex END, @FriendShardIndex=CASE WHEN UserID=@dwFriendID THEN FriendShardIndex ELSE @FriendShardIndex END 
	FROM AccountsShardIndex(NOLOCK) WHERE UserID IN (@dwUserID,@dwFriendID)

	-- ��������
	IF @UserShardIndex IS NULL SET @UserShardIndex=0
	IF @FriendShardIndex IS NULL SET @FriendShardIndex=0

	-- ��������
	DECLARE @Sqlstr NVARCHAR(MAX)
	DECLARE @FriendTableName NVARCHAR(128)

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ���ѱ�����
	SET	@FriendTableName = CONCAT('AccountsFriends_', CONVERT(NVARCHAR(8), @UserShardIndex));	

	-- ɾ������
	SET	@Sqlstr = CONCAT( '  DELETE ', @FriendTableName, ' WHERE UserID=',CONVERT(NVARCHAR(12), @dwUserID),' AND FriendID=',CONVERT(NVARCHAR(12), @dwFriendID))						  
	EXECUTE(@Sqlstr);

	-- ���ѱ�����
	SET	@FriendTableName = CONCAT('AccountsFriends_', CONVERT(NVARCHAR(8), @FriendShardIndex));	

	-- ɾ������
	SET	@Sqlstr = CONCAT( '  DELETE ', @FriendTableName, ' WHERE UserID=',CONVERT(NVARCHAR(12), @dwFriendID),' AND FriendID=',CONVERT(NVARCHAR(12), @dwUserID))						  
	EXECUTE(@Sqlstr);

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

END
RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- ����Ӻ���
CREATE PROC [dbo].[GSP_GP_ApplyAddFriend]
	@dwUserID			INT,					--�û�ID	
	@dwObjectID			INT,					--Ŀ���ʶ
	@dwExpireTime		INT,					--����ʱ��
	@strApplyDesc		NVARCHAR(128),			--��������	
	@strErrorDescribe	NVARCHAR(127) OUTPUT	--�����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

BEGIN
	
	-- ��������
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	

	-- ��ѯ�û�
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
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

	-- У���ʶ
	IF @dwUserID=@dwObjectID
	BEGIN
		SET @strErrorDescribe=N''
		RETURN 4
	END

	-- ��ѯ����
	DECLARE @ApplyerShardIndex INT
	DECLARE @ObjectShardIndex INT
	SELECT @ApplyerShardIndex=CASE WHEN UserID=@dwUserID THEN FriendShardIndex ELSE @ApplyerShardIndex END, @ObjectShardIndex=CASE WHEN UserID=@dwObjectID THEN FriendShardIndex ELSE @ObjectShardIndex END 
	FROM AccountsShardIndex(NOLOCK) WHERE UserID IN (@dwUserID,@dwObjectID)

	-- ��������
	DECLARE @Sqlstr NVARCHAR(MAX)
	DECLARE @FriendTableName1 NVARCHAR(128)
	DECLARE @FriendTableName2 NVARCHAR(128)

	-- ���ѱ�����
	SET	@FriendTableName1 = CONCAT('AccountsFriends_', CONVERT(NVARCHAR(8), @ApplyerShardIndex));	
	SET	@FriendTableName2 = CONCAT('AccountsFriends_', CONVERT(NVARCHAR(8), @ObjectShardIndex));	

	-- �������ѱ�
	IF (NOT EXISTS(SELECT 1 FROM LHAccountsDB.dbo.sysobjects WHERE xtype='u' AND name=@FriendTableName1))
	BEGIN
		SET	@Sqlstr = dbo.LHF_Script_FriendTable(@FriendTableName1)
		EXECUTE(@Sqlstr);
	END

	-- �������ѱ�
	IF (NOT EXISTS(SELECT 1 FROM LHAccountsDB.dbo.sysobjects WHERE xtype='u' AND name=@FriendTableName2))
	BEGIN
		SET	@Sqlstr = dbo.LHF_Script_FriendTable(@FriendTableName2)
		EXECUTE(@Sqlstr);
	END	

	-- ��������
	DECLARE @RecordCount SMALLINT=0	
	
	-- �������
	SET	@Sqlstr = CONCAT( '  IF EXISTS(SELECT 1 FROM ', @FriendTableName1,' WHERE UserID=', CONVERT(NVARCHAR(12), @dwUserID),' AND FriendID=',CONVERT(NVARCHAR(12), @dwObjectID),' )',
						  '  BEGIN ',
						  '    SET @Count+=1',
						  '  END',
						  '  IF EXISTS(SELECT 1 FROM ', @FriendTableName2,' WHERE UserID=', CONVERT(NVARCHAR(12), @dwObjectID),' AND FriendID=',CONVERT(NVARCHAR(12), @dwUserID),' )',
						  '  BEGIN ',
						  '    SET @Count+=1',
						  '	 END')

	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@Count SMALLINT=0 OUT',@Count=@RecordCount OUTPUT

	-- У������
	IF @RecordCount=2
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1284,"Msg":"�Ѿ��Ǻ������������룡"}'
		RETURN 5
	END

	-- ��������
	INSERT INTO AccountsApplyInfo(ApplyUid,ObjectID,ApplyType,ApplyDesc,ApplyState,ObjectUidSet,OperateUid,UpdateTime,ExpireTime,CollectTime)
	VALUES (@dwUserID,@dwObjectID,1,@strApplyDesc,0,N'[]',0,GetDate(),@dwExpireTime,GetDate())

	-- ��ȡ��ʶ
	DECLARE @ApplyID BIGINT
	SET @ApplyID=SCOPE_IDENTITY()

	-- ��ѯ��Ϣ
	SELECT ApplyID,ApplyUid,ObjectID,ApplyState FROM AccountsApplyInfo(NOLOCK) WHERE ApplyID=@ApplyID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ͬ��Ӻ���
CREATE PROC [dbo].[GSP_GP_AgreeAddFriend]
	@dwUserID			INT,					--�û�ID	
	@dwApplyID			BIGINT,					--�����ʶ
	@dwCurrTime			INT,					--��ǰʱ��			
	@strErrorDescribe	NVARCHAR(127) OUTPUT	--�����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �Զ��ع�
SET XACT_ABORT ON

BEGIN
	
	-- ��������
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	

	-- ��ѯ�û�
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
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

	-- ��������
	DECLARE @ApplyUid INT
	DECLARE @ObjectID INT
	DECLARE @ApplyState TINYINT
	DECLARE @ExpireTime INT

	-- ��ѯ����
	SELECT @ApplyUid=ApplyUid,@ObjectID=ObjectID,@ApplyState=ApplyState,@ExpireTime=ExpireTime FROM AccountsApplyInfo(NOLOCK) WHERE ApplyID=@dwApplyID

	-- У����
	IF @ApplyUid IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1280,"Msg":"�ú������벻���ڣ�"}'
		RETURN 4
	END

	-- �ѹ���
	IF @dwCurrTime>=@ExpireTime
	BEGIN
		--����״̬
		IF @ApplyState=0
		BEGIN
			UPDATE AccountsApplyInfo SET ApplyState=3 WHERE ApplyID=@dwApplyID
		END

		-- ������Ϣ
		SET @strErrorDescribe=N'{"Idx":1282,"Msg":"�ú��������ѹ��ڣ�"}'
		RETURN 5
	END

	-- У��״̬
	IF @ApplyState <> 0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1281,"Msg":"�ú��������Ѵ���"}'
		RETURN 6
	END

	-- У���û�
	IF @ObjectID <> @dwUserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1283,"Msg":"��û��Ȩ�޴���ú������룡"}'
		RETURN 7
	END

	-- ��ѯ����
	DECLARE @ApplyerShardIndex INT
	DECLARE @ObjectShardIndex INT
	SELECT @ApplyerShardIndex=CASE WHEN UserID=@ApplyUid THEN FriendShardIndex ELSE @ApplyerShardIndex END, @ObjectShardIndex=CASE WHEN UserID=@ObjectID THEN FriendShardIndex ELSE @ObjectShardIndex END 
	FROM AccountsShardIndex(NOLOCK) WHERE UserID IN (@ApplyUid,@dwUserID)

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��������
	DECLARE @Sqlstr NVARCHAR(MAX)
	DECLARE @FriendTableName NVARCHAR(128)

	-- ���ѱ�����
	SET	@FriendTableName = CONCAT('AccountsFriends_', CONVERT(NVARCHAR(8), @ApplyerShardIndex));	

	-- �������ݱ�
	IF (NOT EXISTS(SELECT 1 FROM LHAccountsDB.dbo.sysobjects WHERE xtype='u' AND name=@FriendTableName))
	BEGIN
		SET	@Sqlstr = dbo.LHF_Script_FriendTable(@FriendTableName)	
		EXECUTE(@Sqlstr);
	END
	
	-- �������
	SET	@Sqlstr = CONCAT( '  IF NOT EXISTS(SELECT 1 FROM ', @FriendTableName,' WHERE UserID=', CONVERT(NVARCHAR(12), @ApplyUid),' AND FriendID=',CONVERT(NVARCHAR(12), @dwUserID),' )',
						  '  BEGIN ',
						  '    INSERT INTO ',@FriendTableName,'(UserID, FriendID)' ,' VALUES(',CONVERT(NVARCHAR(12), @ApplyUid),',',CONVERT(NVARCHAR(12), @dwUserID),')',
						  '  END ELSE ',
						  '  BEGIN ',
						  '    UPDATE ',@FriendTableName,' SET CollectTime=GETDATE()',' WHERE UserID=', CONVERT(NVARCHAR(12), @ApplyUid),' AND FriendID=',CONVERT(NVARCHAR(12), @dwUserID),
						  '	 END')

	EXECUTE(@Sqlstr);
	IF @@ERROR<>0
	BEGIN
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		RETURN 9
	END

	-- ���ѱ�����
	SET	@FriendTableName = CONCAT('AccountsFriends_', CONVERT(NVARCHAR(8), @ObjectShardIndex));	

	-- �������ݱ�
	IF (NOT EXISTS(SELECT 1 FROM LHAccountsDB.dbo.sysobjects WHERE xtype='u' AND name=@FriendTableName))
	BEGIN
		SET	@Sqlstr = dbo.LHF_Script_FriendTable(@FriendTableName)	
		EXECUTE(@Sqlstr);
	END	

	-- �������
	SET	@Sqlstr = CONCAT( '  IF NOT EXISTS(SELECT 1 FROM ', @FriendTableName,' WHERE UserID=', CONVERT(NVARCHAR(12), @dwUserID),' AND FriendID=',CONVERT(NVARCHAR(12), @ApplyUid),' )',
						  '  BEGIN ',
						  '    INSERT INTO ',@FriendTableName,'(UserID, FriendID)' ,' VALUES(',CONVERT(NVARCHAR(12), @dwUserID),',',CONVERT(NVARCHAR(12), @ApplyUid),')',
						  '  END ELSE ',
						  '  BEGIN ',
						  '    UPDATE ',@FriendTableName,' SET CollectTime=GETDATE()',' WHERE UserID=', CONVERT(NVARCHAR(12), @dwUserID),' AND FriendID=',CONVERT(NVARCHAR(12), @ApplyUid),
						  '	 END')

	PRINT @Sqlstr
	EXECUTE(@Sqlstr);
	IF @@ERROR<>0
	BEGIN
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		RETURN 9
	END

	-- ��������
	UPDATE AccountsApplyInfo SET ApplyState=1,OperateUid=@dwUserID,UpdateTime=GetDate() WHERE ApplyID=@dwApplyID	

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- �׳���Ϣ
	SELECT * FROM AccountsApplyInfo(NOLOCK) WHERE ApplyID=@dwApplyID

END
RETURN 0
GO


----------------------------------------------------------------------------------------------------

-- �ܾ��Ӻ���
CREATE PROC [dbo].[GSP_GP_RefuseAddFriend]
	@dwUserID			INT,					--�û�ID	
	@dwApplyID			BIGINT,					--�����ʶ
	@dwCurrTime			INT,					--��ǰʱ��		
	@strRefuseDesc		NVARCHAR(128),			--�ܾ�����	
	@strErrorDescribe	NVARCHAR(127) OUTPUT	--�����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

BEGIN
	
	-- ��������
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	

	-- ��ѯ�û�
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
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

	-- ��������
	DECLARE @ApplyUid INT
	DECLARE @ObjectID INT
	DECLARE @ApplyState TINYINT
	DECLARE @ExpireTime INT

	-- ��ѯ����
	SELECT @ApplyUid=ApplyUid,@ObjectID=ObjectID,@ApplyState=ApplyState,@ExpireTime=ExpireTime FROM AccountsApplyInfo(NOLOCK) WHERE ApplyID=@dwApplyID

	-- У����
	IF @ApplyUid IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1280,"Msg":"�ú������벻���ڣ�"}'
		RETURN 4
	END

	-- �ѹ���
	IF @dwCurrTime>=@ExpireTime
	BEGIN
		--����״̬
		IF @ApplyState=0
		BEGIN
			UPDATE AccountsApplyInfo SET ApplyState=3 WHERE ApplyID=@dwApplyID
		END

		-- ������Ϣ
		SET @strErrorDescribe=N'{"Idx":1282,"Msg":"�ú��������ѹ��ڣ�"}'
		RETURN 5
	END

	-- У��״̬
	IF @ApplyState <> 0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1281,"Msg":"�ú��������Ѵ���"}'
		RETURN 6
	END

	-- У���û�
	IF @ObjectID <> @dwUserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1283,"Msg":"��û��Ȩ�޴���ú������룡"}'
		RETURN 7
	END

	-- ��������
	UPDATE AccountsApplyInfo SET ApplyState=2,RefuseDesc=@strRefuseDesc,OperateUid=@dwUserID,UpdateTime=GetDate() WHERE ApplyID=@dwApplyID

	-- �׳���Ϣ
	SELECT * FROM AccountsApplyInfo(NOLOCK) WHERE ApplyID=@dwApplyID

END
RETURN 0
GO


----------------------------------------------------------------------------------------------------

-- ��Ϣ��������
CREATE PROC [dbo].[GSP_GP_SetupInfoPublic]
	@dwUserID			INT,					--�û�ID	
	@bInfoPublic		TINYINT,				--������־
	@strErrorDescribe	NVARCHAR(127) OUTPUT	--�����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

BEGIN
	
	-- ��������
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	

	-- ��ѯ�û�
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
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

	-- ���±�־
	UPDATE AccountsInfo SET InfoPublic=@bInfoPublic WHERE UserID=@dwUserID

END
RETURN 0
GO

----------------------------------------------------------------------------------------------------

