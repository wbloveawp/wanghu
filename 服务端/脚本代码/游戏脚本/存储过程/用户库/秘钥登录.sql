/***************************************************************************************************
** ���ƣ��ʺŵ�¼ (��Ϸ)
***************************************************************************************************/
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_EfficacySecretKey]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_EfficacySecretKey]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��Կ��½
CREATE PROC GSP_MB_EfficacySecretKey	
	@wGateID INT,								-- ���ر�ʶ	
	@wLogonID INT,								-- ��¼��ʶ
	@dwSocketID INT,							-- �����ʶ	
	@dwUserID INT,								-- �û���ʾ
	@strSecretKey NVARCHAR(128),				-- ��½����	
	@wMarketID SMALLINT,						-- ������ʶ
	@dwDeviceKind INT,							-- �豸����
	@dwPlazaVersion INT,						-- �����汾
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NCHAR(32),					-- ������ʶ
	@strMachineOSID NVARCHAR(64),				-- ϵͳ��ʶ
	@strMobilePhone NVARCHAR(21),				-- �ֻ�����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID			INT
DECLARE @FaceID			SMALLINT
DECLARE @Accounts		NVARCHAR(31)
DECLARE @NickName		NVARCHAR(31)
DECLARE @UnderWrite		NVARCHAR(63)
DECLARE @CustomID		INT
DECLARE @PasswordID		INT
DECLARE @UserRight		INT
DECLARE @ServiceRight	INT

-- ��չ��Ϣ
DECLARE @GameID			INT
DECLARE @ProtectID		INT
DECLARE @MasterOrder	INT
DECLARE @LoveLiness		INT
DECLARE @Experience		INT
DECLARE @LikedNumber	INT
DECLARE @InfoPublic		TINYINT
DECLARE @MemberPoint	INT
DECLARE @MemberOverTime INT

-- �û���Ϣ
DECLARE @Gender TINYINT

-- �����Ϣ
DECLARE @UserGold	BIGINT
DECLARE @UserCard	BIGINT
DECLARE @InsureGold	BIGINT
DECLARE @Commission BIGINT

-- ��Ϊ����
DECLARE @ActionMaskEver INT
DECLARE @ActionMaskPerDay INT
DECLARE @ActionMaskPerWeek INT

-- ��������
DECLARE @EnjoinLogon AS INT
DECLARE @StatusString NVARCHAR(128)

-- ������Ϣ
DECLARE @ParentUserID INT
DECLARE @ParentGameID INT
DECLARE @ParentFaceID INT
DECLARE @ParentNickName NVARCHAR(31)

--ʵ����Ϣ
DECLARE @szCompellation NVARCHAR(32)
DECLARE @szPassPortID NVARCHAR(32)

-- ִ���߼�
BEGIN
	-- ϵͳ��ͣ
	SELECT @EnjoinLogon=CONVERT(INT,StatusValue),@StatusString=StatusString FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		IF dbo.LHF_IsAccountExcept(@strClientIP, @strMachineID)<>0
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1002,"Msg":"'+@StatusString+N'"}'
			RETURN 1
		END

		SET @EnjoinLogon=0	
	END

	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1019,"Msg":"��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ�ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����"}'
		RETURN 4
	END
	
	-- Ч�����
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	
	-- У�����
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN					
		SET @strErrorDescribe=N'{"Idx":1020,"Msg":"��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ����ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����"}'
		RETURN 7
	END	
 
	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @ShutUpStatus BIT
	DECLARE @ShutDownStatus BIT
	DECLARE @PlazaVersion INT
	DECLARE @GameLogonTimes INT
	DECLARE @SecretKey NVARCHAR(32)
	DECLARE @LogonPass NCHAR(32)
	DECLARE @InsurePass NCHAR(32)
	DECLARE @UniqueID NVARCHAR(32)
	DECLARE	@MachineID NCHAR(32)
	DECLARE	@Email NVARCHAR(64)
	DECLARE @MobilePhone NVARCHAR(21)	
	DECLARE @ShutupOverTime INT
	DECLARE @ShutDownOverTime INT
	DECLARE @RegisterTime INT
	DECLARE @RegisterMode SMALLINT
	DECLARE	@LogonDateTime DATETIME
	DECLARE	@ResetDateTime DATETIME
	DECLARE @RegisterDeviceKind SMALLINT

	-- ��ѯ�û�
	SELECT @UserID=a.UserID, @GameID=a.GameID, @Accounts=a.Accounts, @NickName=a.NickName, @UnderWrite=a.UnderWrite, @LogonPass=a.LogonPass, @InsurePass=a.InsurePass, @FaceID=a.FaceID,@CustomID=a.CustomID,@ProtectID=a.ProtectID,
			@MasterOrder=a.MasterOrder,@Gender=a.Gender, @Nullity=a.Nullity, @Experience=a.Experience, @LoveLiness=a.LoveLiness, @GameLogonTimes=a.GameLogonTimes, @MemberPoint=a.MemberPoint, @MemberOverTime=a.MemberOverTime, 
			@MachineID=a.LogonMachineID, @PlazaVersion=a.PlazaVersion,@LikedNumber=a.LikedNumber, @MobilePhone=MobilePhone, @Email=a.Email, @RegisterMode=a.RegisterMode, @InfoPublic=a.InfoPublic,@RegisterDeviceKind=a.RegisterDeviceKind,
			@ActionMaskEver=a.ActionMaskEver,@ActionMaskPerDay=a.ActionMaskPerDay,@ActionMaskPerWeek=a.ActionMaskPerWeek, @ParentUserID=a.ParentID,@ShutUpStatus=b.ShutupStatus,@ShutDownStatus=b.ShutDownStatus,@ShutupOverTime=b.ShutupOverTime,
			@UserRight=a.UserRight,@ShutDownOverTime=b.ShutDownOverTime,@LogonDateTime=a.LogonDate,@ResetDateTime=a.ResetDateTime,@RegisterTime=dbo.LHF_ConvertTimestamp(a.RegisterDate)
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- У���ʶ
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��"}'
		RETURN 200
	END	

	-- �����ж�
	IF @SecretKey<>@strSecretKey
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��"}'
		RETURN 3
	END

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����"}'
		RETURN 2
	END	

	-- ����״̬
	IF @ShutUpStatus IS NULL SET @ShutUpStatus=0
	IF @ShutDownStatus IS NULL SET @ShutDownStatus=0
	IF @ShutUpOverTime IS NULL SET @ShutUpOverTime=0
	IF @ShutDownOverTime IS NULL SET @ShutDownOverTime=0

	-- �ʺŹر�
	IF @ShutDownStatus<>0
	BEGIN
		-- ����״̬
		IF dbo.LHF_GetTimestamp()<@ShutDownOverTime
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1023,"Msg":"�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�"}'
			RETURN 2
		END			

		-- ����״̬		
		UPDATE AccountsStatus SET ShutDownStatus=0,ShutDownOverTime=0 WHERE UserID=@UserID
	END	

	----------------------------------------------------------------------------------------------------

	-- ǩ����Ϣ 
	DECLARE @CheckInDays SMALLINT
	DECLARE @CheckInState TINYINT
	
	-- ��ѯǩ����Ϣ
	SELECT @CheckInDays=CheckInDays,@CheckInState=CheckInState FROM dbo.LHF_GetCheckInData(@UserID)	

	----------------------------------------------------------------------------------------------------

	-- ʱ�䶨��
	DECLARE @CurrDateTime DATETIME
	DECLARE @ReferDateTime DATETIME	
	
	--����ʱ��
	SET @CurrDateTime=GETDATE()
	SET @ReferDateTime=@LogonDateTime

	-- ����ʱ��
	IF @ResetDateTime>@LogonDateTime SET @ReferDateTime=@ResetDateTime

	-- ��ȡ����
	DECLARE @CurrDayID INT = CAST(CAST(@CurrDateTime AS FLOAT) AS INT)
	DECLARE @ReferDayID INT = CAST(CAST(@ReferDateTime AS FLOAT) AS INT)

	-- ��������
	IF @CurrDayID<>@ReferDayID
	BEGIN
		
		-- �����ո�������
		SELECT @ActionMaskPerDay=0

		-- ������Ϣ
		UPDATE AccountsInfo SET ActionMaskPerDay=@ActionMaskPerDay WHERE UserID=@UserID	
	END

	----------------------------------------------------------------------------------------------------

	-- ����ע��
	SELECT @UniqueID=UniqueID FROM AccountsExternal(NOLOCK) WHERE UserID=@UserID	

	-- ��������
	IF @UniqueID IS NULL SET @UniqueID=N''

	-- ת������
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��½����
	SET @GameLogonTimes+=1
	SET @SecretKey=REPLACE(NEWID(), N'-', N'')

	-- ������Ϣ
	UPDATE AccountsInfo SET SecretKey=@SecretKey,GameLogonTimes=@GameLogonTimes,LogonDate=GETDATE(), LogonIP=@strClientIP,
	PlazaVersion=@dwPlazaVersion,LogonMachineID=@strMachineID,LogonMobile=@strMobilePhone WHERE UserID=@UserID	
	
	-- ��¼��־
	INSERT INTO RecordLogonInfo(UserID,DateID,DeviceKind,MachineOSID,PlazaVersion,ClientIP,MachineID,CollectDate)
	VALUES (@UserID,@DateID,@dwDeviceKind,@strMachineOSID,@dwPlazaVersion,@strClientIP,@strMachineID,GETDATE())

	-- ��¼ͳ��	
	UPDATE SystemStreamLogon SET LogonCount=LogonCount+1,LastDate=GETDATE() WHERE DateID=@DateID AND UserID=@UserID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO SystemStreamLogon(DateID,UserID,LogonCount) VALUES(@DateID,@UserID,1)
	END

	-- ��¼��־	
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)	

	-- �����Ϣ
	SELECT @UserGold=Score,@InsureGold=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@UserID
	IF @UserGold IS NULL OR @InsureGold IS NULL
	BEGIN
		SET @UserGold=0
		SET @InsureGold = 0
	END

	-- �Ƹ���Ϣ
	SELECT @UserCard=Score FROM LHGameBattleDB.dbo.GameCardInfo(NOLOCK) WHERE UserID=@UserID
	SELECT @Commission=Commission FROM AccountsCommission(NOLOCK) WHERE UserID=@UserID

	-- ������ʯ
	IF @UserCard IS NULL SET @UserCard=0
	IF @Commission IS NULL SET @Commission=0

	-- ������Ϣ
	DECLARE @LockGateID INT
	DECLARE @LockLogonID INT
	DECLARE @LockSocketID INT
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT
	DECLARE @LockServerKind INT

	-- ��ѯ����
	SELECT @LockGateID=GateID,@LockLogonID=LogonID,@LockSocketID=SocketID,@LockKindID=KindID,@LockServerID=ServerID,@LockServerKind=ServerKind FROM AccountsOnlineInfo(NOLOCK) WHERE UserID=@UserID

	-- ��������
	IF @LockGateID IS NOT NULL
	BEGIN		
		-- ��������
		UPDATE AccountsOnlineInfo SET GateID=@wGateID,LogonID=@wLogonID,SocketID=@dwSocketID,InsertTime=GETDATE(),ModifyTime=GETDATE() WHERE UserID=@UserID	
	END ELSE
	BEGIN
		-- ��������
		SELECT @LockGateID=0,@LockLogonID=0,@LockSocketID=0,@LockKindID=0,@LockServerID=0,@LockServerKind=0

		-- ��������
		INSERT INTO AccountsOnlineInfo(UserID,GateID,LogonID,SocketID,InsertTime,ModifyTime) VALUES (@UserID,@wGateID,@wLogonID,@dwSocketID,GETDATE(),GETDATE())
	END
	
	-- ��ѯ����
	SELECT @ParentGameID=GameID,@ParentFaceID=FaceID,@ParentNickName=NickName FROM AccountsInfo(NOLOCK) WHERE UserID=@ParentUserID
	IF @ParentGameID IS NULL
	BEGIN
		SET @ParentGameID=0
		SET @ParentFaceID=0
		SET @ParentNickName=N''
	END

	--��ѯʵ��
	SELECT @szCompellation=Compellation,@szPassPortID=PassPortID FROM AccountsAuthentPresent WHERE UserID=@UserID
	IF @szCompellation IS NULL SET @szCompellation=N''
	IF @szPassPortID IS NULL SET @szPassPortID=N''

	-- ��ȡ��������
	DECLARE @UIShieldMask INT
	SET @UIShieldMask=dbo.LHF_GetUserUIShieldMask(@UserID,@strClientIP,@strMachineID)

	-- ��ȡ���������б�
	DECLARE @ShieldKindList NVARCHAR(256)
	SET @ShieldKindList=dbo.LHF_GetUserShieldKindList(@UserID)

	--�汾ͳ��
	UPDATE PlazaVersionCollect SET LogonCount=LogonCount+1 WHERE PlazaVersion=@dwPlazaVersion AND DateID=@DateID
	IF @@ROWCOUNT=0 INSERT INTO PlazaVersionCollect (DateID,PlazaVersion,LogonCount,CollectDate) VALUES(@DateID,@dwPlazaVersion,1,GETDATE())

	--�ǳ��޸Ĳ�ѯ
	DECLARE @wNickNameModifyTimes INT
	DECLARE @wNickNameDailyModifyTimes INT
	SELECT @wNickNameDailyModifyTimes=DailyModify,@wNickNameModifyTimes=TotalModify FROM RecordModifyUserIndividual WHERE DateID=@DateID AND UserID=@UserID
	IF @wNickNameModifyTimes IS NULL SELECT @wNickNameModifyTimes=COUNT(1) FROM RecordModifyUserIndividual WHERE UserID=@UserID
	IF @wNickNameModifyTimes IS NULL SET @wNickNameModifyTimes = 0
	IF @wNickNameDailyModifyTimes IS NULL SET @wNickNameDailyModifyTimes = 0

	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName, @LogonPass AS Password, @InsurePass AS InsurePass, @UnderWrite AS UnderWrite, @FaceID AS FaceID,	@CustomID AS CustomID,
		@ProtectID AS ProtectID,@MasterOrder AS MasterOrder, @UserGold AS UserGold, @InsureGold AS InsureGold, @SecretKey AS SecretKey, @UserCard AS UserCard, @Commission AS Commission, @Gender AS Gender, @Experience AS Experience, 
		@LoveLiness AS LoveLiness, @MemberPoint AS MemberPoint, @MemberOverTime AS MemberOverTime, @LockGateID AS LockGateID, @LockKindID As LockKindID, @LockLogonID AS LockLogonID, @LockSocketID AS LockSocketID,@UserRight AS UserRight,
		@LockServerID AS LockServerID, @MobilePhone AS MobilePhone,@Email AS Email, @UniqueID AS UniqueID, @RegisterTime AS RegisterTime, @LikedNumber AS LikedNumber,@InfoPublic AS InfoPublic, @ActionMaskEver AS ActionMaskEver,
		@ActionMaskPerDay AS ActionMaskPerDay, @ActionMaskPerWeek AS ActionMaskPerWeek, @GameLogonTimes AS GameLogonTimes, @LockServerKind AS LockServerKind,@ParentGameID AS ParentGameID,@ParentFaceID AS ParentFaceID,
		@ParentNickName AS ParentNickName,@ShutUpStatus AS ShutUpStatus, @ShutUpOverTime AS ShutUpOverTime,@RegisterMode AS RegisterMode, @UIShieldMask AS UIShieldMask,@ShieldKindList AS ShieldKindList,@LogonDateTime AS LastLogonDate,
		@szCompellation AS Compellation,@szPassPortID AS PassPortID, @CheckInDays AS CheckInDays,@CheckInState AS CheckInState,@wNickNameDailyModifyTimes AS NickNameDailyModifyTimes,@wNickNameModifyTimes AS NickNameModifyTimes

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------