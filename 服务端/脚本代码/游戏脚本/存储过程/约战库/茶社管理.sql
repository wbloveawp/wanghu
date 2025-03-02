----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_CreateClub]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_CreateClub]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DeleteClub]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DeleteClub]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_SearchClub]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_SearchClub]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_TransferClub]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_TransferClub]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UpdateClubProperty]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UpdateClubProperty]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UpdateClubRules]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UpdateClubRules]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UpdateClubNotice]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UpdateClubNotice]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- ����Ⱥ��
CREATE  PROCEDURE dbo.GSP_GR_CreateClub
	-- �û���Ϣ
	@dwCreaterID INT,							-- �û���ʶ
	
	-- Ⱥ������
	@wClubKind SMALLINT,						-- Ⱥ������
	@wClubIconID SMALLINT,						-- Ⱥ��ͼ��
	@strClubName NVARCHAR(32),					-- Ⱥ������
	@strClubIntroduce NVARCHAR(128),			-- Ⱥ�����

	-- Ⱥ�����
	@cbPayType	TINYINT,						-- ֧����ʽ
	@cbCurrencyKind TINYINT,					-- ��������
	@dwSwitchOption INT,						-- ����ѡ��

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

	-- ��ѯ�û�
	DECLARE @AgentID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	DECLARE @UserRight INT
	
	-- ��ѯ�û�
	SELECT @UserID=a.UserID,@GameID=a.GameID,@UserRight=a.UserRight, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus, @NickName=a.NickName
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwCreaterID

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

	-- Ⱥ������
	DECLARE @strClubOptionName NVARCHAR(32)

	-- ѡ������
	IF @wClubKind=0 SET @strClubOptionName=N'ClubOption_Gold'
	IF @wClubKind=1 SET @strClubOptionName=N'ClubOption_Normal'
	IF @wClubKind=2 SET @strClubOptionName=N'ClubOption_Advanced'

	-- ��������
	DECLARE @ScoreKind TINYINT	
	DECLARE @PayTypeMask TINYINT	
	DECLARE @CurrencyKindMask TINYINT
	DECLARE @MaxMemberCount SMALLINT	
	DECLARE @MaxCreateClubCount SMALLINT	
	DECLARE @CreateNeedUserRight INT
	DECLARE @CreateClubTakeCard BIGINT
	DECLARE @CreateClubDeductCard BIGINT	

	-- ��ѯ����
	SELECT @MaxMemberCount=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'ClubOption' AND StatusName='MaxMemberCount'	
	SELECT @MaxCreateClubCount=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'ClubOption' AND StatusName='MaxCreateClubCount'		

	-- ��ѯ����	
	SELECT @ScoreKind=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='ScoreKind'			
	SELECT @PayTypeMask=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='PayTypeMask'
	SELECT @CurrencyKindMask=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='CurrencyKindMask'			
	SELECT @CreateNeedUserRight=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='CreateNeedUserRight'
	SELECT @CreateClubTakeCard=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='CreateClubTakeCard'	
	SELECT @CreateClubDeductCard=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='CreateClubDeductCard'						

	-- ��������	
	IF @ScoreKind IS NULL SET @ScoreKind=0	
	IF @PayTypeMask IS NULL SET @PayTypeMask=0	
	IF @CurrencyKindMask IS NULL SET @CurrencyKindMask=0
	IF @MaxMemberCount IS NULL SET @MaxMemberCount=0	
	IF @MaxCreateClubCount IS NULL SET @MaxCreateClubCount=0
	IF @CreateNeedUserRight IS NULL SET @CreateNeedUserRight=0
	IF @CreateClubTakeCard IS NULL SET @CreateClubTakeCard=0
	IF @CreateClubDeductCard IS NULL SET @CreateClubDeductCard=0

	-- У��Ȩ��
	IF @CreateNeedUserRight>0 AND (@UserRight&@CreateNeedUserRight)=0
	BEGIN
		-- ������Ϣ
		SET @strErrorDescribe=N'{"Idx":1155,"Msg":"����ϵͳԭ��, ���紴��ʧ��[1]�� ����ϵ�ͻ����������˽���ϸ�����","Ops":["1"]}'
		RETURN 5
	END
	
	-- ��ѯ�Ƹ�
	DECLARE @UserCard BIGINT	
	SELECT @UserCard=Score FROM GameCardInfo(NOLOCK) WHERE UserID=@dwCreaterID

	-- �����Ƹ�
	IF @UserCard IS NULL SET @UserCard=0
	
	-- �жϷ���
	IF @UserCard<@CreateClubTakeCard
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1150,"Msg":"��Ǹ��������Я���ķ�������'+CONVERT(NVARCHAR(18),@CreateClubTakeCard)+',���紴��ʧ�ܣ�","Ops":["'+CONVERT(NVARCHAR(18),@CreateClubTakeCard)+'"]}'
		RETURN 5		
	END

	-- �жϷ���
	IF @UserCard<@CreateClubDeductCard
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1151,"Msg":"��Ǹ����ǰ�ʺ�Я���ķ�������'+CONVERT(NVARCHAR(18),@CreateClubDeductCard)+',���紴��ʧ�ܣ�","Ops":["'+CONVERT(NVARCHAR(18),@CreateClubDeductCard)+'"]}'
		RETURN 5		
	END

	-- ��������
	DECLARE @wCreateClubCount SMALLINT
	SELECT @wCreateClubCount=COUNT(ClubID) FROM ClubProperty(NOLOCK) WHERE CreaterID=@dwCreaterID AND ClubStatus=1

	-- �ж�����
	IF @wCreateClubCount>=@MaxCreateClubCount
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1152,"Msg":"��Ǹ���������Ĳ�����Ŀ�Ѿ��ﵽ���ޣ��������ʣ�����ϵ�ͷ���"}'
		RETURN 5		
	END

	-- У�������
	IF EXISTS(SELECT 1 FROM ClubProperty(NOLOCK) WHERE ClubName=@strClubName)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1153,"Msg":"ϵͳ���Ѿ�������ͬ���ֵĲ��磬����ʧ�ܣ�"}'
		RETURN 6		
	END

	-- У��֧����ʽ
	IF @cbPayType=0 OR ((@PayTypeMask&@cbPayType)<>@cbPayType)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1154,"Msg":"�����õ�֧��������ϵͳ���ò���[1]������ʧ�ܣ�"}'
		RETURN 5				
	END

	-- У���������
	IF @cbCurrencyKind=0 OR ((@CurrencyKindMask&@cbCurrencyKind)<>@cbCurrencyKind)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1154,"Msg":"�����õ�֧��������ϵͳ���ò���[2]������ʧ�ܣ�"}'
		RETURN 5				
	END	

	-- ����Ⱥ��
	DECLARE @CreateDateTime DATETIME
	SET @CreateDateTime=GetDate()

	-- ��ѯȺ��ID
	DECLARE @dwClubID INT	
	SELECT TOP 1 @dwClubID=ClubID FROM NormalClubID(NOLOCK) WHERE Distribute=0
	IF @dwClubID IS NULL
	BEGIN		
		-- ������Ϣ
		SET @strErrorDescribe=N'{"Idx":1155,"Msg":"����ϵͳԭ��, ���紴��ʧ��[2]�� ����ϵ�ͻ����������˽���ϸ�����","Ops":["2"]}'
		RETURN 5
	END

	-- ����״̬
	UPDATE NormalClubID SET Distribute=1 WHERE ClubID=@dwClubID

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN		

	-- Ⱥ������
	INSERT INTO ClubProperty(ClubID,CreaterID, CreaterGameID, ClubName, ClubKind, ClubStatus, ClubIconID, ClubIntroduce, MemberCount, MaxMemberCount, CreateDateTime)
	VALUES (@dwClubID, @dwCreaterID, @GameID, @strClubName, @wClubKind, 0, @wClubIconID, @strClubIntroduce, 0, @MaxMemberCount, @CreateDateTime)
	IF @@ERROR<>0
	BEGIN
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		-- ������Ϣ
		SET @strErrorDescribe=N'{"Idx":1155,"Msg":"����ϵͳԭ��,���紴��ʧ��[3]������ϵ�ͻ����������˽���ϸ�����","Ops":["3"]}'
		RETURN 6
	END	

	-- Ⱥ�����
	INSERT INTO ClubRules(ClubID, PayType, CurrencyKind, ScoreKind, BattleMask, SwitchOption) 
	VALUES (@dwClubID, @cbPayType, @cbCurrencyKind, @ScoreKind, 0, @dwSwitchOption)	
	IF @@ERROR<>0
	BEGIN
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		-- ������Ϣ
		SET @strErrorDescribe=N'{"Idx":1155,"Msg":"����ϵͳԭ��,���紴��ʧ��[4]������ϵ�ͻ����������˽���ϸ�����","Ops":["4"]}'
		RETURN 6
	END	

	-- Ⱥ�鹫��
	INSERT INTO ClubNotice(ClubID, PublishState, PublishMode, NoticeContent) VALUES (@dwClubID, 0, 0, N'')	
	IF @@ERROR<>0
	BEGIN
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		-- ������Ϣ
		SET @strErrorDescribe=N'{"Idx":1155,"Msg":"����ϵͳԭ��,���紴��ʧ��[5]������ϵ�ͻ����������˽���ϸ�����","Ops":["5"]}'
		RETURN 6
	END

	-- �۳�����
	IF @CreateClubDeductCard>0
	BEGIN
		-- �����¼
		INSERT INTO LHGameRecordDB.dbo.RecordChangeCard (UserID,GameID,TypeID,KindID,ClubID,BeforeCard,ChangeCard,CollectClientIP,CollectDateTime)
		VALUES (@dwCreaterID,@GameID,6,0,@dwClubID,@UserCard,@CreateClubDeductCard,N'--------------',GETDATE())

		-- ���·���
		UPDATE GameCardInfo SET Score-=@CreateClubDeductCard WHERE UserID=@dwCreaterID			
	END	

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED		

	-- ��ѯ�Ƹ�	
	SELECT @UserCard=Score FROM GameCardInfo(NOLOCK) WHERE UserID=@dwCreaterID

	-- �����Ƹ�
	IF @UserCard IS NULL SET @UserCard=0

	-- ��ѯ��Ϣ
	SELECT a.*, b.FaceID AS CreaterFaceID, b.CustomID AS CreaterCustomID, b.NickName AS CreaterNickName, @UserCard AS UserCard 
	FROM ClubProperty(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsInfo(NOLOCK) b ON a.CreaterID=b.UserID  WHERE a.ClubID=@dwClubID
	
END

RETURN 0

GO



----------------------------------------------------------------------------------------------------

-- ɾ��Ⱥ��
CREATE  PROCEDURE dbo.GSP_GR_DeleteClub
	-- �û���Ϣ
	@dwCreaterID INT,							-- �û���ʶ
	@dwTargetClubID INT,						-- Ⱥ���ʶ
	
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
DECLARE	@TableName  NVARCHAR(128);								
DECLARE @ExecSqlstr	NVARCHAR(512);	

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	
	SELECT @UserID=a.UserID,@GameID=a.GameID,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwCreaterID

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
	DECLARE @ClubName NVARCHAR(32)
	SELECT @ClubName=ClubName,@CreaterID=CreaterID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwTargetClubID

	-- ����ж�
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
		RETURN 5		
	END

	-- ����Ⱥ��
	IF @CreaterID <> @dwCreaterID 
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1157,"Msg":"��Ǹ�������Ǹò�����ϰ壬û��Ȩ�޽���ɾ����"}'
		RETURN 5		
	END

	-- �޸�״̬
	UPDATE ClubProperty SET ClubStatus=2,CollectDateTime=GETDATE() WHERE ClubID=@dwTargetClubID

	-- �û�֪ͨ
	-- INSERT INTO RecordUserNotice(UserID,NoticeKind,NoticeContent,CollectDateTime) VALUES(@UserID,5,N'{"Idx":1214,"Ops":["'+@ClubName+'"]}',GETDATE())	

	-- �ֱ��ʶ
	SET	@TableID=dbo.LHF_GetTableID(@dwTargetClubID);

	-- Ⱥ���Ա��
	SET	@TableName='ClubMember_';
	SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

	-- �����ʶ
	DECLARE @strClubID NVARCHAR(20)
	SET @strClubID=CONCAT(';',CONVERT(NVARCHAR(18),@dwTargetClubID),';');

	-- ��������
	SET	@ExecSqlstr = CONCAT( '  INSERT INTO RecordUserNotice(UserID,NoticeKind,NoticeContent,CollectDateTime) ',
							  '  SELECT MemberID,5,',N'''{"Idx":1214,"Ops":["'+@ClubName+'"]}',''',GETDATE() FROM  ',@TableName,' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwTargetClubID))	
	EXECUTE(@ExecSqlstr);


	-- ��������
	SET	@ExecSqlstr = CONCAT( '  UPDATE ClubUserInfo SET ClubIDArray=REPLACE(ClubIDArray,''',@strClubID,''','';''',')',
							  '  WHERE UserID IN (SELECT MemberID FROM  ',@TableName,' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwTargetClubID),')')	
						  
	EXECUTE(@ExecSqlstr);		
	
	SELECT @ClubName AS ClubName					  
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ����Ⱥ��
CREATE  PROCEDURE dbo.GSP_GR_SearchClub
	-- ������Ϣ
	@dwClubID INT,								-- Ⱥ���ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON
-- ִ���߼�
BEGIN

	-- ��ѯ����
	DECLARE @ClubID INT
	SELECT @ClubID=ClubID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID AND ClubStatus=1

	-- У�����
	IF @ClubID IS NULL
	BEGIN
	   SET @strErrorDescribe=N'{"Idx":1158,"Msg":"��Ǹ�������ҵĲ�����Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
	   RETURN 1	
	END

	-- ��ѯ��Ϣ
	SELECT a.*,b.FaceID AS CreaterFaceID, b.CustomID AS CreaterCustomID, b.NickName AS CreaterNickName 
	FROM ClubProperty(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsInfo(NOLOCK) b ON a.CreaterID=b.UserID WHERE a.ClubID=@dwClubID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ת��Ⱥ��
CREATE  PROCEDURE dbo.GSP_GR_TransferClub
	-- �û���Ϣ
	@dwClubID	INT,							-- Ⱥ���ʶ	
	@dwCreaterID INT,							-- �û���ʶ		
	@dwNewCreaterID INT,						-- �û���ʶ
	@strAuthCodeMD5 NCHAR(32),					-- ��֤�� 
	
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
DECLARE	@TableName  NVARCHAR(128);								
DECLARE @ExecSqlstr	NVARCHAR(512);	

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @LogonPass NCHAR(32)		
	DECLARE @MobilePhone NVARCHAR(11)	

	-- ��ѯ�û�
	SELECT @UserID=a.UserID,@GameID=a.GameID,@NickName=a.NickName,@Nullity=a.Nullity,@MobilePhone=a.MobilePhone, @ShutDown=b.ShutDownStatus
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwCreaterID

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

	-- У���ֻ�
	IF @MobilePhone=N'' OR LEN(@MobilePhone)<>11
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1159,"Msg":"�����ʺ�û�а��ֻ��ţ����Ȱ��ֻ����ٽ���ת�ã�"}'
		RETURN 4		
	END

	-- ��ѯ�ֻ���
	DECLARE @ATLastAcquireDate DATETIME
	DECLARE @LastMobilePhone NVARCHAR(11)		
	SELECT @LastMobilePhone=LastMobileEmail,@ATLastAcquireDate=LastAcquireDate FROM LHAccountsDB.dbo.AccountsAuthInfo(NOLOCK) WHERE UserID=@dwCreaterID AND CodeKind=1
	
	-- У���¼
	IF @LastMobilePhone IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1160,"Msg":"��֤����󣬲���ת��ʧ��[1]��","Ops":["1"]}'
		RETURN 5
	END

	-- У���ֻ���
	IF @LastMobilePhone<>@MobilePhone
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1160,"Msg":"��֤����󣬲���ת��ʧ��[2]��","Ops":["2"]}'		
		RETURN 5
	END

	-- ��֤��Ϣ
	DECLARE @AuthKind TINYINT	
	DECLARE @AuthCodeMD5 NVARCHAR(32)
	DECLARE @AcquireMachine NVARCHAR(32)
	DECLARE @MBLastAcquireDate DATETIME
	SELECT @AuthKind=AuthKind, @AuthCodeMD5=AuthCodeMD5, @AcquireMachine=AcquireMachine, @MBLastAcquireDate=LastAcquireDate FROM LHAccountsDB.dbo.RecordAuthCode(NOLOCK) WHERE MobileEmail=@MobilePhone AND CodeKind=1

	-- У������
	IF @AuthKind<>6
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1160,"Msg":"��֤����󣬲���ת��ʧ��[3]��","Ops":["3"]}'	
		RETURN 6		
	END

	-- У����֤��
	IF @AuthCodeMD5<>@strAuthCodeMD5
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1160,"Msg":"��֤����󣬲���ת��ʧ��[4]��","Ops":["4"]}'	
		RETURN 7				
	END	

	-- ��Чʱ��
	DECLARE @ACValidSeconds INT
	SELECT @ACValidSeconds=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ACValidSecond'

	-- ��������
	IF @ACValidSeconds IS NULL SET @ACValidSeconds=600

	-- ʱ����
	DECLARE @SecondDiff INT
	SET @SecondDiff=DateDiff(s,@MBLastAcquireDate,GetDate())

	-- ʱ������
	IF @SecondDiff>@ACValidSeconds	
	BEGIN			
		SET @strErrorDescribe=N'{"Idx":1029,"Msg":"��֤���ѹ��ڣ������»�ȡ��֤�룡"}'
		RETURN 8
	END

	-- ��ѯȺ��
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- ����ж�
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
		RETURN 5		
	END

	-- ����Ⱥ��
	IF @CreaterID <> @dwCreaterID 
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1162,"Msg":"��Ǹ�������Ǹò�����ϰ壬û��Ȩ�޽���ת�ã�"}'
		RETURN 5		
	END

	-- �ֱ��ʶ
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- Ⱥ���Ա��
	SET	@TableName ='ClubMember_';
	SET	@TableName = CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

	-- ��ѯ��Ա����
	DECLARE @OUT_MemberType SMALLINT	
	SET	@ExecSqlstr = CONCAT(' SELECT @MemberType=MemberType FROM ',@TableName,'(NOLOCK)',
						     ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwNewCreaterID))				  
	
	-- ִ�����
	EXEC SP_EXECUTESQL @stmt=@ExecSqlstr,@params=N'@MemberType TINYINT OUT',@MemberType=@OUT_MemberType OUTPUT

	-- ����У��
	IF @OUT_MemberType IS NULL 
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1163,"Msg":"����ת��ʧ�ܣ����ϰ�����Ǹò���ĳ�Ա��"}'
		RETURN 6
	END

	-- �¹�����Ϣ
	DECLARE @CreaterGameID INT
	DECLARE @CreaterNickName NVARCHAR(32)	
	SELECT @CreaterGameID=GameID, @CreaterNickName=NickName FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwNewCreaterID
	
	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- �޸�״̬
	UPDATE ClubProperty SET CreaterID=@dwNewCreaterID,CreaterGameID=@CreaterGameID WHERE ClubID=@dwClubID	

	-- �������ϰ���Ϣ
	SET	@ExecSqlstr = CONCAT(' UPDATE ',@TableName,' SET MemberType=4',
							 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwNewCreaterID),CHAR(13))	
	EXECUTE(@ExecSqlstr);	
	IF @@ERROR<>0
	BEGIN
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
		
		SET @strErrorDescribe=N'{"Idx":1164,"Msg":"����ϵͳԭ��,����ת��ʧ��[1]������ϵ�ͷ��˽���ϸ�����","Ops":["1"]}'	
		RETURN 8
	END

	-- ���¾��ϰ��Ա���ͺ��ϼ�
	SET	@ExecSqlstr = CONCAT(' UPDATE ',@TableName,' SET MemberType=0',
							 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwCreaterID),CHAR(13))	
	EXECUTE(@ExecSqlstr);	
	IF @@ERROR<>0
	BEGIN
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		-- ������Ϣ
		SET @strErrorDescribe=N'{"Idx":1164,"Msg":"����ϵͳԭ��,����ת��ʧ��[2]������ϵ�ͷ��˽���ϸ�����","Ops":["2"]}'	
		RETURN 9
	END	
	
	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- ������Ϣ
	UPDATE LHAccountsDB.dbo.AccountsAuthInfo SET AuthTimes=AuthTimes+1,AcquireTimes=0, LastAcquireDate=GetDate(),LastMobileEmail=N'' WHERE UserID=@dwCreaterID AND CodeKind=1

	-- ���¼�¼
	UPDATE LHAccountsDB.dbo.RecordAuthCode SET AuthCode=N'',AuthCodeMD5=N'', AuthTimes=AuthTimes+1,AcquireTimes=0 WHERE MobileEmail=@MobilePhone AND CodeKind=1

	-- ת�óɹ�
	SET @strErrorDescribe=N'{"Idx":1165,"Msg":"����ת�óɹ���"}'
				  
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE  PROCEDURE dbo.GSP_GR_UpdateClubProperty
	-- �û���Ϣ
	@dwUserID INT,								-- �û���ʶ
	@dwClubID INT,								-- Ⱥ���ʶ

	-- Ⱥ����Ϣ
	@wClubIconID SMALLINT,						-- Ⱥ��ͼ��
	@strClubName NVARCHAR(32),					-- Ⱥ������
	@strClubIntroduce NVARCHAR(128),			-- Ⱥ�����	
	
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

	-- ��ѯ�û�
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
	DECLARE @ClubName NVARCHAR(32)
	DECLARE @ClubKind TINYINT
	DECLARE @ClubIconID SMALLINT
	DECLARE @ClubIntroduce NVARCHAR(128)
	SELECT @CreaterID=CreaterID,@ClubKind=ClubKind,@ClubIconID=ClubIconID,@ClubName=ClubName,@ClubIntroduce=ClubIntroduce FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID AND ClubStatus=1

	-- ����ж�
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
		RETURN 5		
	END

	-- ����Ⱥ��
	IF @CreaterID <> @dwUserID 
	BEGIN
		-- ��������
		DECLARE	@TableID	INT;																			
		DECLARE	@TableName	NVARCHAR(128);								
		DECLARE @Sqlstr		NVARCHAR(512);

		-- �ֱ��ʶ
		SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

		-- Ⱥ���Ա��
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

		-- ��ѯ��Ա����
		DECLARE @OUT_OperaterType SMALLINT	
		DECLARE @OUT_OperaterRight SMALLINT	
		SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType,@ManageRight=ManageRight FROM ',@TableName,'(NOLOCK)',
							 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwUserID))				  
	
		-- ִ�����
		EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT,@ManageRight INT OUT',@MemberType=@OUT_OperaterType OUTPUT,@ManageRight=@OUT_OperaterRight OUTPUT

		-- ��������
		IF @OUT_OperaterType IS NULL SET @OUT_OperaterType=0

		-- У��Ȩ��
		IF @OUT_OperaterType<3 OR (@OUT_OperaterType=3 AND (@OUT_OperaterRight&0x0001)=0)
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1166,"Msg":"��Ǹ����û��Ȩ���޸Ĳ��磡"}'
			RETURN 5		
		END			
	END

	-- ����ͼ��
	IF @wClubIconID<>@ClubIconID
	BEGIN
		UPDATE ClubProperty SET ClubIconID=@wClubIconID WHERE ClubID=@dwClubID
	END

	-- �޸Ĳ�������
	IF @ClubName<>@strClubName
	BEGIN
		-- ����ѡ��
		DECLARE @strClubOptionName NVARCHAR(32)

		-- ����ѡ��
		IF @ClubKind=0 SET @strClubOptionName=N'ClubOption_Gold'		
		IF @ClubKind=1 SET @strClubOptionName=N'ClubOption_Normal'
		IF @ClubKind=2 SET @strClubOptionName=N'ClubOption_Advanced'

		-- ��������		
		DECLARE @ModifyClubNameDeductCard BIGINT
		SELECT @ModifyClubNameDeductCard=CONVERT(TINYINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='ModifyClubNameDeductCard'

		-- ��������	
		IF @ModifyClubNameDeductCard IS NULL SET @ModifyClubNameDeductCard=0

		-- ��������
		DECLARE @UserCard BIGINT		
		SELECT @UserCard=Score FROM GameCardInfo(NOLOCK) WHERE UserID=@CreaterID

		-- �����Ƹ�
		IF @UserCard IS NULL SET @UserCard=0

		-- У�鷿��
		IF @UserCard<@ModifyClubNameDeductCard
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1167,"Msg":"��Ǹ�������ϰ�Я���ķ������㣬���������޸�ʧ�ܣ�"}'
			IF @CreaterID=@dwUserID SET @strErrorDescribe=N'{"Idx":1168,"Msg":"��Ǹ��������Я���ķ������㣬���������޸�ʧ�ܣ�"}'
			RETURN 6
		END

		-- �۳�����
		IF @ModifyClubNameDeductCard>0
		BEGIN
			-- �����¼
			INSERT INTO LHGameRecordDB.dbo.RecordChangeCard (UserID,GameID,TypeID,KindID,ClubID,BeforeCard,ChangeCard,CollectClientIP,CollectDateTime)
			VALUES (@CreaterID,@GameID,7,0,@dwClubID,@UserCard,-@ModifyClubNameDeductCard,N'--------------',GETDATE())

			-- ���·���
			UPDATE GameCardInfo SET Score-=@ModifyClubNameDeductCard WHERE UserID=@CreaterID
		END

		-- �ظ��ж�
		IF EXISTS (SELECT 1 FROM ClubProperty(NOLOCK) WHERE ClubName=@strClubName)
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1169,"Msg":"��Ǹ�����������ѱ�ռ�ã�������������ٴγ��ԣ�"}'
			RETURN 6
		END		

		-- ��������
		UPDATE ClubProperty SET ClubName=@strClubName WHERE ClubID=@dwClubID			
	END

	-- �޸Ĳ������
	IF @ClubIntroduce<>@strClubIntroduce
	BEGIN
		-- ���½���
		UPDATE ClubProperty SET ClubIntroduce=@strClubIntroduce WHERE ClubID=@dwClubID		
	END	

	-- ��ѯ��Ϣ
	SELECT a.*,b.FaceID AS CreaterFaceID, b.CustomID AS CreaterCustomID, b.NickName AS CreaterNickName 
	FROM ClubProperty(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsInfo(NOLOCK) b ON a.CreaterID=b.UserID WHERE a.ClubID=@dwClubID
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���¹���
CREATE  PROCEDURE dbo.GSP_GR_UpdateClubRules
	-- �û���Ϣ
	@dwUserID INT,								-- �û���ʶ
	@dwClubID INT,								-- Ⱥ���ʶ

	-- Ⱥ�����
	@cbBattleMask TINYINT,						-- Լս����		
	@wSwitchOption SMALLINT,					-- ����ѡ��
	
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

	-- ��ѯ�û�
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
	DECLARE @ClubKind SMALLINT
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID,@ClubKind=ClubKind FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID AND ClubStatus=1

	-- ����ж�
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
		RETURN 5		
	END

	-- ����Ⱥ��
	IF @CreaterID <> @dwUserID
	BEGIN
		-- ��������
		DECLARE	@TableID	INT;																			
		DECLARE	@TableName	NVARCHAR(128);								
		DECLARE @Sqlstr		NVARCHAR(512);

		-- �ֱ��ʶ
		SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

		-- Ⱥ���Ա��
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

		-- ��ѯ��Ա����
		DECLARE @OUT_OperaterType SMALLINT	
		DECLARE @OUT_OperaterRight SMALLINT	
		SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType,@ManageRight=ManageRight FROM ',@TableName,'(NOLOCK)',
							 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwUserID))				  
	
		-- ִ�����
		EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT,@ManageRight INT OUT',@MemberType=@OUT_OperaterType OUTPUT,@ManageRight=@OUT_OperaterRight OUTPUT

		-- ��������
		IF @OUT_OperaterType IS NULL SET @OUT_OperaterType=0

		-- У��Ȩ��
		IF @OUT_OperaterType<3 OR (@OUT_OperaterType=3 AND (@OUT_OperaterRight&0x0001)=0)
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1166,"Msg":"��Ǹ����û��Ȩ���޸Ĳ��磡"}'
			RETURN 5		
		END		
	END

	-- ���¹���
	UPDATE ClubRules SET BattleMask=@cbBattleMask,SwitchOption=@wSwitchOption WHERE ClubID=@dwClubID

	-- ��ѯ��Ϣ
	SELECT * FROM ClubRules(NOLOCK) WHERE ClubID=@dwClubID	
	
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- ���¹���
CREATE  PROCEDURE dbo.GSP_GR_UpdateClubNotice
	-- �û���Ϣ	
	@dwUserID	INT,							-- �û���ʶ
	@dwClubID	INT,							-- �����ʶ	
	@cbPublishMode TINYINT,						-- ����ģʽ
	@cbPublishState TINYINT,					-- ����״̬
	@strNoticeContent NVARCHAR(128),			-- ��������	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��ѯ�û�
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- ��ѯ�û�
	SELECT @UserID=a.UserID,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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
	DECLARE @ClubKind SMALLINT
	SELECT @CreaterID=CreaterID,@ClubKind=ClubKind FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID AND ClubStatus=1

	-- ����ж�
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
		RETURN 5		
	END

	-- ����Ⱥ��
	IF @CreaterID <> @dwUserID 
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1209,"Msg":"�����ǲ�����ϰ壬û��Ȩ���޸Ĺ��棡"}'
		RETURN 6
	END

	-- ���¹���
	UPDATE ClubNotice SET PublishMode=@cbPublishMode,PublishState=@cbPublishState,NoticeContent=@strNoticeContent WHERE ClubID=@dwClubID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO ClubNotice(ClubID,PublishState,PublishMode,NoticeContent) VALUES(@dwClubID,@cbPublishState,@cbPublishMode,@strNoticeContent)
	END
	
	-- �׳���¼
	SELECT * FROM ClubNotice(NOLOCK) WHERE ClubID=@dwClubID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------