
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ApplyRequest]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ApplyRequest]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ApplyRespond]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ApplyRespond]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ApplyDelete]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ApplyDelete]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ApplyRepeal]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ApplyRepeal]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadApplyMessage]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadApplyMessage]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- ��������
CREATE  PROCEDURE dbo.GSP_GR_ApplyRequest
	-- �û���Ϣ	
	@dwClubID INT,								-- Ⱥ���ʶ	
	@dwApplyerID INT,							-- �û���ʶ	
	
	-- �������	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @FaceID		INT
DECLARE @Gender		TINYINT
DECLARE @CustomID	INT
DECLARE @NickName	NVARCHAR(32)

-- Ⱥ����Ϣ
DECLARE @CreaterID  INT
DECLARE @ClubName  NVARCHAR(32)
DECLARE @ClubStatus TINYINT
DECLARE @MemberCount SMALLINT
DECLARE @MaxMemberCount SMALLINT

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- ��ѯ�û�
	SELECT @UserID=a.UserID,@GameID=a.GameID,@Nullity=a.Nullity, @NickName=a.NickName, @Gender=a.Gender, @FaceID=a.FaceID, @CustomID=a.CustomID, @ShutDown=b.ShutDownStatus
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwApplyerID

	-- �û�������
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

	-- �����ж�
	IF EXISTS(SELECT 1 FROM RecordClubApply(NOLOCK) WHERE ClubID=@dwClubID AND ApplyerID=@dwApplyerID AND ApplyStatus=0)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1183,"Msg":"���Ѿ��������ò��磬����Ҫ�ظ����룬�����ĵȴ������ϰ���ˣ�"}'
		RETURN 5					
	END	

	-- У����ֲ�
	---------------------------------------------------------------------------------------------------------------------------

	-- ��ѯȺ��	
	SELECT @CreaterID=CreaterID,@ClubName=ClubName,@ClubStatus=ClubStatus,@MemberCount=MemberCount,@MaxMemberCount=MaxMemberCount FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- ���ֲ�������
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
		RETURN 6
	END

	-- ���ֲ���ͣ��
	IF @ClubStatus=2
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'				
		RETURN 7
	END

	-- ���ֲ���Ա
	IF @MemberCount=@MaxMemberCount
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1184,"Msg":"��Ǹ���ò�������Ա��������������룡"}'
		RETURN 8		
	END	

	-- У��������
	---------------------------------------------------------------------------------------------------------------------------

	-- ��������
	DECLARE @wCreateClubCount SMALLINT
	SELECT @wCreateClubCount=COUNT(ClubID) FROM ClubProperty(NOLOCK) WHERE CreaterID=@dwApplyerID AND ClubStatus=1

	-- ��ȡȺ���б�
	DECLARE @strClubIDArray NVARCHAR(1024)
	SELECT @strClubIDArray=ClubIDArray FROM ClubUserInfo(NOLOCK) WHERE UserID=@dwApplyerID
	IF @strClubIDArray IS NULL 
	BEGIN
		SET @strClubIDArray=';'
		INSERT INTO ClubUserInfo VALUES (@dwApplyerID,@strClubIDArray)
	END

	-- ���ұ�ʶ
	DECLARE @strClubID NVARCHAR(20)
	SET @strClubID=CONCAT(';',CONVERT(NVARCHAR(18),@dwClubID),';');	
	IF CHARINDEX(@strClubID,@strClubIDArray)>0
	BEGIN		
		SET @strErrorDescribe=N'{"Idx":1185,"Msg":"���Ѿ��Ǹò���ĳ�Ա��"}'
		RETURN 9
	END	

	-- ��ѯ����
	DECLARE @MaxJoinClubCount SMALLINT		
	SELECT @MaxJoinClubCount=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'ClubOption' AND StatusName='MaxJoinClubCount'
	
	-- Ⱥ����Ŀ
	DECLARE @dwClubCount SMALLINT	
	SELECT @dwClubCount=(LEN(@strClubIDArray)-LEN(REPLACE(@strClubIDArray, ';', '')))

	-- ��������
	IF @dwClubCount>0 SET @dwClubCount=@dwClubCount-1
	
	-- ͳ�ƴ���	
	DECLARE @ApplyCount SMALLINT
	SELECT @ApplyCount=COUNT(ClubID) FROM RecordClubApply(NOLOCK) WHERE ApplyerID=@dwApplyerID AND ApplyStatus=0

	-- ����У��
	IF (@dwClubCount-@wCreateClubCount)+@ApplyCount>=@MaxJoinClubCount
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1186,"Msg":"��Ǹ����ͬʱ�ɼ����������Ϊ'+CONVERT(NVARCHAR(6),@MaxJoinClubCount)+'�������˳������������ԣ�","Ops":["'+CONVERT(NVARCHAR(6),@MaxJoinClubCount)+'"]}'
		
		RETURN 10				
	END

	---------------------------------------------------------------------------------------------------------------------------	

	-- �����¼
	INSERT INTO RecordClubApply(ClubID, ApplyerID, ApplyerGameID, ApplyerNickName, ApplyerGender, ApplyerFaceID, ApplyerCustomID, ApplyStatus,CreaterID, ResponderID, ClubName)
	VALUES (@dwClubID, @dwApplyerID, @GameID, @NickName, @Gender, @FaceID, @CustomID, 0, @CreaterID, 0, @ClubName)	

	-- ��¼��ʶ
	DECLARE @RecordID INT
	SELECT 	@RecordID=SCOPE_IDENTITY()

	-- ��ѯ����ѡ��
	DECLARE @SwitchOption SMALLINT
	SELECT @SwitchOption=SwitchOption FROM ClubRules(NOLOCK) WHERE ClubID=@dwClubID

	-- ��������
	IF @SwitchOption IS NULL SET @SwitchOption=0	

	-- ���У��
	IF (@SwitchOption&0x01)=0
	BEGIN		
		-- ����״̬
		UPDATE RecordClubApply SET ApplyStatus=1,OperateDateTime=GETDATE() WHERE RecordID=@RecordID	

		-- ��ѯ��Ϣ
		SELECT a.*,b.*,c.FaceID AS CreaterFaceID,c.CustomID AS CreaterCustomID,c.NickName AS CreaterNickName 
		FROM RecordClubApply(NOLOCK) a, ClubProperty(NOLOCK) b, LHAccountsDB.dbo.AccountsInfo(NOLOCK) c
		WHERE a.ClubID=b.ClubID AND a.RecordID=@RecordID AND a.CreaterID=c.UserID
				
		-- ����ɹ�
		SET @strErrorDescribe=N'{"Idx":1187,"Msg":"���ѳɹ�������ֲ���ף����Ϸ��죡"}'

	END ELSE
	BEGIN

		-- ��ѯ��Ϣ
		SELECT * FROM RecordClubApply(NOLOCK) WHERE RecordID=@RecordID

		-- ����ɹ�
		SET @strErrorDescribe=N'{"Idx":1188,"Msg":"����ɹ��������ĵȴ������ϰ���ˣ�"}'
	END		
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ������Ӧ
CREATE  PROCEDURE dbo.GSP_GR_ApplyRespond
	-- �û���Ϣ
	@dwRecordID INT,							-- ��¼��ʶ 
	@dwUserID INT,								-- �û���ʶ
	@dwApplyerID INT,							-- �û���ʶ	
	@dwClubID INT,								-- վ���ʶ		
	@cbApplyStatus TINYINT,						-- ����״̬
	
	-- �������	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- ��������
DECLARE	@TableID	INT;								
DECLARE @Sqlstr		NVARCHAR(512)											
DECLARE	@TableName	NVARCHAR(128)

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- ��ѯ�û�
	SELECT @UserID=a.UserID,@GameID=a.GameID,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- �û�������
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

	-- ��ѯȺ��
	DECLARE @CreaterID INT
	DECLARE @MemberCount SMALLINT
	DECLARE @MaxMemberCount SMALLINT
	SELECT @CreaterID=CreaterID, @MemberCount=MemberCount, @MaxMemberCount=MaxMemberCount FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- ���粻����
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
		RETURN 5		
	END

	-- ���ֲ���Ա
	IF @MemberCount=@MaxMemberCount AND @cbApplyStatus=1
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1189,"Msg":"��Ǹ�����Ĳ����Ѿ���Ա����Ա���ʧ�ܣ�"}'
		RETURN 6		
	END

	-- �ֱ��ʶ
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- �������
	SET	@TableName='ClubMember_';
	SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

	-- ��ѯ��Ա����
	DECLARE @OUT_MemberType SMALLINT	
	DECLARE @OUT_ManageRight INT
	SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType,@ManageRight=ManageRight FROM ',@TableName,'(NOLOCK)',
						 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwUserID))				  
	
	-- ִ�����
	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT,@ManageRight INT OUT',@MemberType=@OUT_MemberType OUTPUT,@ManageRight=@OUT_ManageRight OUTPUT

	-- У�����ͺ�Ȩ��
	IF @OUT_MemberType<3 OR (@OUT_MemberType=3 AND (@OUT_ManageRight&0x0004)=0)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1190,"Msg":"��Ǹ����û��Ȩ������û����룡"}'
		RETURN 7
	END

	IF @dwRecordID<>0
	BEGIN
		-- ��ѯ��¼
		DECLARE @ClubID INT
		DECLARE @ApplyerID INT
		DECLARE @ApplyStatus TINYINT	
		SELECT @ClubID=ClubID,@ApplyerID=ApplyerID,@ApplyStatus=ApplyStatus FROM RecordClubApply(NOLOCK) WHERE RecordID=@dwRecordID

		IF @ApplyerID IS NULL
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1191,"Msg":"ϵͳδ��ѯ����ص������¼���ü�¼�����ѱ������߳�����"}'
			RETURN 8		
		END 

		IF @dwApplyerID<>@ApplyerID OR @dwClubID<>@ClubID
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1192,"Msg":"ϵͳ��⵽�����¼�쳣������ϵ�ͻ����������˽���ϸ�����"}'
			RETURN 9
		END	

		-- Ⱥ������
		UPDATE RecordClubApply SET ApplyStatus=@cbApplyStatus,ResponderID=@dwUserID,OperateDateTime=GETDATE() WHERE RecordID=@dwRecordID		

		-- ��ѯ��¼
		SELECT * FROM RecordClubApply(NOLOCK) WHERE RecordID=@dwRecordID

	END ELSE
	BEGIN
		-- ��ѯ��¼
		SELECT * FROM RecordClubApply(NOLOCK) WHERE ClubID=@dwClubID AND ApplyStatus=0

		-- �޸�״̬
		UPDATE RecordClubApply SET ApplyStatus=@cbApplyStatus,ResponderID=@dwUserID,OperateDateTime=GETDATE() WHERE ClubID=@dwClubID AND ApplyStatus=0				
	END
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- ����ɾ��
CREATE  PROCEDURE dbo.GSP_GR_ApplyDelete
	-- �û���Ϣ
	@dwUserID INT,								-- �û���ʶ				
	@dwClubID INT,								-- �����ʶ	
	@dwMessageID INT,							-- ��Ϣ��ʶ 
	@cbMessageType INT,							-- ��Ϣ���� 				
	
	-- �������	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- ��������
DECLARE	@TableID	INT;								
DECLARE @Sqlstr		NVARCHAR(512)											
DECLARE	@TableName	NVARCHAR(128)

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- ��ѯ�û�
	SELECT @UserID=a.UserID,@GameID=a.GameID,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- �û�������
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

	-- ��Ӧ����
	IF @cbMessageType=2
	BEGIN

		-- �ֱ��ʶ
		SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

		-- �������
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

		-- ��ѯ��Ա����
		DECLARE @OUT_MemberType SMALLINT	
		DECLARE @OUT_ManageRight INT
		SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType,@ManageRight=ManageRight FROM ',@TableName,'(NOLOCK)',
							 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwUserID))				  
	
		-- ִ�����
		EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT,@ManageRight INT OUT',@MemberType=@OUT_MemberType OUTPUT,@ManageRight=@OUT_ManageRight OUTPUT

		-- У�����ͺ�Ȩ��
		IF @OUT_MemberType<3 OR (@OUT_MemberType=3 AND (@OUT_ManageRight&0x0004)=0)
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1190,"Msg":"��Ǹ����û��Ȩ������û����룡"}'
			RETURN 7
		END
	END

	IF @dwMessageID<>0
	BEGIN
		-- ��ѯ��Ϣ
		DECLARE @ApplyerID INT
		DECLARE @ApplyStatus TINYINT
		DECLARE @DeleteMask TINYINT
		SELECT @ApplyerID=ApplyerID,@ApplyStatus=ApplyStatus,@DeleteMask=DeleteMask FROM RecordClubApply(NOLOCK) WHERE RecordID=@dwMessageID

		-- ��Ϣ������
		IF @ApplyerID IS NULL
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1192,"Msg":"ϵͳ��⵽�����¼�쳣������ϵ�ͻ����������˽���ϸ�����"}'
			RETURN 5		
		END

		-- �ж�״̬
		IF @ApplyStatus=0
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1193,"Msg":"��Ǹ������Ϣδͨ���ϰ���ˣ��޷�ɾ����"}'
			RETURN 5		
		END		
		
		-- У������
		IF @DeleteMask<>0
		BEGIN
			-- ɾ����Ϣ
			DELETE RecordClubApply WHERE RecordID=@dwMessageID	

		END ELSE
		BEGIN
			-- ����״̬
			UPDATE RecordClubApply SET DeleteMask=CASE @cbMessageType WHEN 1 THEN 1 ELSE 2 END WHERE RecordID=@dwMessageID
		END

		-- ��ѯ��Ϣ
		SELECT @dwMessageID AS RecordID
			
	END ELSE
	BEGIN
		-- ��������
		IF @cbMessageType=1
		BEGIN
			-- ��ѯ��Ϣ
			SELECT RecordID FROM RecordClubApply WHERE ApplyerID=@dwUserID AND ApplyStatus<>0

			-- ɾ����Ϣ
			DELETE RecordClubApply WHERE ApplyerID=@dwUserID AND ApplyStatus<>0 AND DeleteMask=2
			
			-- ��������
			UPDATE RecordClubApply SET DeleteMask=1 WHERE ApplyerID=@dwUserID AND ApplyStatus<>0 AND DeleteMask=0
		END
		
		-- ��Ӧ����
		IF @cbMessageType=2
		BEGIN
			-- ��ѯ��Ϣ
			SELECT RecordID FROM RecordClubApply WHERE ClubID=@dwClubID AND ApplyStatus<>0
			
			-- ɾ����Ϣ
			DELETE RecordClubApply WHERE ClubID=@dwClubID AND ApplyStatus<>0 AND DeleteMask=1
			
			-- ��������
			UPDATE RecordClubApply SET DeleteMask=2 WHERE ClubID=@dwClubID AND ApplyStatus<>0 AND DeleteMask=0
		END
	END
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- ���볷��
CREATE  PROCEDURE dbo.GSP_GR_ApplyRepeal
	-- �û���Ϣ
	@dwMessageID INT,							-- ��Ϣ��ʶ 
	@cbMessageStatus TINYINT,					-- ��Ϣ״̬
	@dwUserID INT,								-- �û���ʶ	
		
	-- �������	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- ��ѯ�û�
	SELECT @UserID=a.UserID,@GameID=a.GameID,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- �û�������
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

	IF @dwMessageID<>0
	BEGIN
		-- ��ѯ��Ϣ
		DECLARE @ApplyerID INT
		DECLARE @ApplyStatus TINYINT
		DECLARE @DeleteMask TINYINT
		SELECT @ApplyerID=ApplyerID,@ApplyStatus=ApplyStatus,@DeleteMask=DeleteMask FROM RecordClubApply(NOLOCK) WHERE RecordID=@dwMessageID

		-- ��Ϣ������
		IF @ApplyerID IS NULL
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1192,"Msg":"��Ǹ����Ϣ�����ڣ�"}'
			RETURN 5		
		END

		-- �ж�״̬
		IF @ApplyStatus<>0
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1195,"Msg":"��Ǹ������Ϣ��ͨ���ϰ���ˣ��޷�������"}'
			RETURN 5		
		END

		-- �����߳���
		IF @dwUserID<>@ApplyerID
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1196,"Msg":"��Ǹ��������������,�޷���������Ϣ��"}'
			RETURN 6		
		END	

		-- ����״̬
		UPDATE RecordClubApply SET ApplyStatus=@cbMessageStatus,OperateDateTime=GETDATE() WHERE RecordID=@dwMessageID

		-- �׳���¼
		SELECT * FROM RecordClubApply(NOLOCK) WHERE RecordID=@dwMessageID

	END ELSE
	BEGIN
		-- ��ѯ��Ϣ
		SELECT * FROM RecordClubApply(NOLOCK) WHERE ApplyerID=@dwUserID AND ApplyStatus=0 AND DeleteMask<>1

		-- ����״̬
		UPDATE RecordClubApply SET ApplyStatus=@cbMessageStatus,OperateDateTime=GETDATE() WHERE ApplyerID=@dwUserID AND ApplyStatus=0 AND DeleteMask<>1
	END
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- ������Ӧ
CREATE  PROCEDURE dbo.GSP_GR_LoadApplyMessage
	-- �û���Ϣ	
	@dwUserID INT,								-- �û���ʶ
	@dwClubID INT,								-- Ⱥ���ʶ
	@cbMessageType INT							-- ��Ϣ����		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��������Ϣ
	IF @cbMessageType=1
	BEGIN
		
		-- ��ѯ��Ϣ
		SELECT * FROM RecordClubApply(NOLOCK) WHERE ApplyerID=@dwUserID AND DeleteMask<>1
	END

	-- ��Ӧ����Ϣ
	IF @cbMessageType=2
	BEGIN

		-- ��ѯ��Ϣ
		SELECT * FROM RecordClubApply(NOLOCK) WHERE ClubID=@dwClubID AND DeleteMask<>2
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------