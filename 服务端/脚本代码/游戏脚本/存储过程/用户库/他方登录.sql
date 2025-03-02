/***************************************************************************************************
** ���ƣ��ʺŵ�¼ (��Ϸ)
***************************************************************************************************/
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_EfficacyThirdParty]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_EfficacyThirdParty]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��������½
CREATE PROC GSP_MB_EfficacyThirdParty
	@wGateID INT,								-- ���ر�ʶ	
	@wLogonID INT,								-- ��¼��ʶ
	@dwSocketID INT,							-- �����ʶ
	@dwSpreaderID INT,							-- ����ID
	@dwExternalID INT,							-- ������ʶ
	@strUniqueID NVARCHAR(32),			        -- �û�Uin
	@strNickName NVARCHAR(31),					-- �û��ǳ�
	@strUserDatum NVARCHAR(256),				-- �û�����
	@strCompellation NVARCHAR(16),				-- ��ʵ����
	@cbGender TINYINT,							-- �û��Ա�	
	@wMarketID SMALLINT,						-- ������ʶ
	@dwDeviceKind INT,							-- �豸����
	@dwPlazaVersion INT,						-- �����汾
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ	
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strMachineOSID NVARCHAR(64),				-- ϵͳ�汾
	@strMobilePhone NVARCHAR(21),				-- �ֻ�����	
	@strPackageName NVARCHAR(64),				-- Ӧ�ð���
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
DECLARE @GameID INT
DECLARE @ProtectID INT
DECLARE @MasterOrder INT
DECLARE @LoveLiness INT
DECLARE @Experience INT
DECLARE @LikedNumber INT
DECLARE @InfoPublic  TINYINT
DECLARE @MemberPoint INT
DECLARE @MemberOverTime INT

-- �û���Ϣ
DECLARE @Gender TINYINT

-- ������Ϣ
DECLARE @UserGold BIGINT
DECLARE @UserCard BIGINT
DECLARE @InsureGold BIGINT
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

--ע������
DECLARE @szPresentGoodsList NVARCHAR(MAX)

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
 
	-- �û���Ϣ
	DECLARE @Nullity BIT
	DECLARE @ShutUpStatus BIT
	DECLARE @ShutDownStatus BIT
	DECLARE @GameLogonTimes INT 
	DECLARE @LogonPass NCHAR(32)
	DECLARE @InsurePass NVARCHAR(32)
	DECLARE	@MachineID NCHAR(32)
	DECLARE	@Email NVARCHAR(64)
	DECLARE	@MobilePhone NVARCHAR(21)
	DECLARE @PlazaVersion INT	
	DECLARE @ShutupOverTime INT
	DECLARE @ShutDownOverTime INT
	DECLARE @RegisterTime INT
	DECLARE @RegisterMode SMALLINT
	DECLARE	@LogonDateTime DATETIME
	DECLARE	@ResetDateTime DATETIME
	DECLARE @RegisterDeviceKind SMALLINT

	-- ���ڱ�ʶ
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ע������
	DECLARE @PresentGold BIGINT
	DECLARE @PresentCard BIGINT

	-- �û���ѯ
	SELECT @UserID=UserID FROM AccountsExternal(NOLOCK) WHERE UniqueID=@strUniqueID AND ExternalID=@dwExternalID	

	IF @UserID IS NULL
	BEGIN
		
		-- ����ǰ׺
		IF LEN(@strNickName)=0
		BEGIN
			-- IOS��IPAD
			IF (@dwDeviceKind&0x60)<>0 SET @strNickName=N'IPhone'

			-- ANDROID
			IF (@dwDeviceKind&0x80)<>0 SET @strNickName=N'Android'
		END
		
		-- �����˺�
		SET @Accounts=@strNickName
	
		-- �����˺�
		EXEC dbo.LHP_AdjustmentAccounts @Accounts OUTPUT,@strNickName OUTPUT		
		
		-- �����������
		SET @LogonPass=CONVERT(nvarchar(32),REPLACE(newid(),'-',''))	

		-- ע���û�
		INSERT AccountsInfo (Accounts,NickName,RegAccounts,LogonPass,InsurePass,Gender,FaceID,LogonMachineID, RegisterMode,RegisterMachineID,RegisterDeviceKind,RegisterPackageName,GameLogonTimes,
				RegisterIP,LogonIP,PlazaVersion,MarketID,ParentID)
		VALUES (@Accounts,@strNickName,@Accounts,@LogonPass,N'',@cbGender,CAST(RAND()*10 AS INT)+1,@strMachineID,5,@strMachineID,@dwDeviceKind,@strPackageName,0,
				@strClientIP,@strClientIP,@dwPlazaVersion,@wMarketID,0)

		-- �����ж�
		IF @@ERROR<>0
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1009,"Msg":"�ʺ��Ѵ��ڣ��뻻��һ�ʺ����ֳ����ٴ�ע�ᣡ"}'
			RETURN 8
		END

		-- �����û�
		SET @UserID=SCOPE_IDENTITY()
		INSERT AccountsExternal (UserID, UniqueID, ExternalID, UserDatum, UserFacePath, NickName, Gender, LogonClientIP) 
		VALUES (@UserID, @strUniqueID, @dwExternalID, @strUserDatum, N'', @strNickName, @cbGender, @strClientIP)

		-- �����ж�
		IF @@ERROR<>0
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1009,"Msg":"�ʺ��Ѵ��ڣ��뻻��һ�ʺ����ֳ����ٴ�ע�ᣡ"}'
			RETURN 1
		END

		-- ��ѯ�û�
		SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts,@NickName=NickName,@LogonPass=LogonPass,@InsurePass=InsurePass,@FaceID=FaceID,@CustomID=CustomID,@ProtectID=ProtectID, @MasterOrder=MasterOrder,
			   @Gender=Gender, @Experience=Experience, @LoveLiness=LoveLiness, @UnderWrite=UnderWrite, @MemberPoint=MemberPoint,@GameLogonTimes=GameLogonTimes, @MemberOverTime=MemberOverTime, @InfoPublic=InfoPublic,
			   @RegisterMode=RegisterMode,@RegisterDeviceKind=RegisterDeviceKind,@ActionMaskEver=ActionMaskEver,@ActionMaskPerDay=ActionMaskPerDay,@LogonDateTime=LogonDate, @ResetDateTime=ResetDateTime, @ActionMaskPerWeek=ActionMaskPerWeek,
			   @UserRight=UserRight,@LikedNumber=LikedNumber,@MobilePhone=MobilePhone,@Email=EMail,@RegisterMode=@RegisterMode,@RegisterTime=dbo.LHF_ConvertTimestamp(RegisterDate)
		FROM AccountsInfo(NOLOCK) WHERE UserID=@UserID
		
		-- �����ʶ
		SELECT @GameID=GameID FROM GameIdentifier(NOLOCK) WHERE UserID=@UserID
		IF @GameID IS NULL 
		BEGIN
			SET @GameID=0
			SET @strErrorDescribe=N'{"Idx":1010,"Msg":"�û�ע��ɹ�����δ�ɹ���ȡ��Ϸ ID ���룬ϵͳ�Ժ󽫸������䣡"}'
		END ELSE 
		BEGIN
			UPDATE AccountsInfo SET GameID=@GameID WHERE UserID=@UserID
		END	
		
		-- ��ѯ�û�
		DECLARE @ParentParentID INT
		SELECT @ParentUserID=UserID,@ParentParentID=ParentID FROM AccountsInfo(NOLOCK) WHERE GameID=@dwSpreaderID
		--�����
		IF @ParentUserID IS NULL OR @ParentUserID = 0
		BEGIN
			SELECT @ParentUserID=GameID FROM BindIP WHERE UserIP=@strClientIP
			SELECT @ParentUserID=UserID FROM AccountsInfo(NOLOCK) WHERE GameID=@ParentUserID
			DELETE FROM BindIP WHERE UserIP=@strClientIP
		END
		-- ������ʶ
		IF @ParentUserID IS NULL OR @ParentParentID=@UserID SET @ParentUserID=0

		-- ���ϼ�
		IF @ParentUserID<>0
		BEGIN		
			DECLARE @MailID BIGINT
			EXEC GSP_MB_BindParent @UserID,@ParentUserID,@strClientIP,@MailID OUTPUT
		END

		-- ��������		
		DECLARE @AgentLevel NVARCHAR(256)	
		SELECT @AgentLevel=StatusValue FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'AgentOption' AND StatusName=N'AgentLevel_0'

		-- У������
		IF ISJSON(@AgentLevel)>0
		BEGIN
			INSERT INTO AccountsAgentInfo(UserID,AgentLevel,LowerBetAwardRate,LowerPayAwardAmount,LLowerBetAwardRate,LLowerPayAwardAmount)
			SELECT @UserID,0,ISNULL(JSON_VALUE(@AgentLevel,N'$.LowerBetAwardRate'),0),ISNULL(JSON_VALUE(@AgentLevel,N'$.LowerPayAwardAmount'),0),ISNULL(JSON_VALUE(@AgentLevel,N'$.LLowerBetAwardRate'),0),ISNULL(JSON_VALUE(@AgentLevel,N'$.LLowerPayAwardAmount'),0)			
		END

		-- ע��ͳ��
		UPDATE SystemStreamRegister SET TotalCount+=1,LastDate=GETDATE()
		WHERE DateID=@DateID AND DeviceKind=@dwDeviceKind AND ExternalID=@dwExternalID
		IF @@ROWCOUNT=0
		BEGIN
			INSERT INTO SystemStreamRegister(DateID,DeviceKind,ExternalID,TotalCount) VALUES(@DateID,@dwDeviceKind,@dwExternalID,1)
		END
		
		-- ��¼��־
		UPDATE SystemStreamInfo SET GameRegisterSuccess=GameRegisterSuccess+1 WHERE DateID=@DateID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID,GameRegisterSuccess) VALUES (@DateID, 1)		

		-- ��Ҳ�ѯ
		SELECT @UserGold=Score,@InsureGold=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@UserID
		IF @UserGold IS NULL OR @InsureGold IS NULL
		BEGIN
			SELECT @UserGold=0,@InsureGold = 0

			INSERT LHGameGoldDB.dbo.GameScoreInfo (UserID, Score, InsureScore) VALUES (@UserID, @UserGold, @InsureGold) 
		END

		--------------------------------------ע������-----------------------------------------------------
		EXEC GSP_MB_RegisterPresent @UserID,5,@strClientIP,@strMachineID,@szPresentGoodsList OUTPUT

		--------------------------------------ע������-----------------------------------------------------

	END	ELSE
	BEGIN
		
		-- ��ѯ�û�
		SELECT @UserID=a.UserID, @GameID=a.GameID, @Accounts=a.Accounts, @NickName=a.NickName, @UnderWrite=a.UnderWrite, @LogonPass=a.LogonPass, @InsurePass=a.InsurePass, @FaceID=a.FaceID,@CustomID=a.CustomID,
			@ProtectID=a.ProtectID,@MasterOrder=a.MasterOrder,@Gender=a.Gender, @Nullity=a.Nullity, @Experience=a.Experience, @LoveLiness=a.LoveLiness, @GameLogonTimes=a.GameLogonTimes, @MemberPoint=a.MemberPoint,
			@MemberOverTime=a.MemberOverTime, @MachineID=a.LogonMachineID, @PlazaVersion=a.PlazaVersion,@LikedNumber=a.LikedNumber, @MobilePhone=MobilePhone, @Email=a.EMail, @RegisterMode=a.RegisterMode, 
			@InfoPublic=a.InfoPublic,@RegisterDeviceKind=a.RegisterDeviceKind,@ActionMaskEver=a.ActionMaskEver,@ActionMaskPerDay=a.ActionMaskPerDay, @ActionMaskPerWeek=a.ActionMaskPerWeek,@ParentUserID=a.ParentID,
			@ShutUpStatus=b.ShutupStatus,@ShutDownStatus=b.ShutDownStatus,@ShutupOverTime=b.ShutupOverTime,@ShutDownOverTime=b.ShutDownOverTime,@RegisterMode=a.RegisterMode, @LogonDateTime=a.LogonDate,@ResetDateTime=a.ResetDateTime,
			@UserRight=a.UserRight,@RegisterTime=dbo.LHF_ConvertTimestamp(a.RegisterDate)
		FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@UserID
	END

	-- ��������
	IF @Email IS NULL SET @Email=N''
	IF @MobilePhone IS NULL SET @MobilePhone=N''
	IF @PresentGold IS NULL SET @PresentGold=0
	IF @PresentCard IS NULL SET @PresentCard=0
	IF @ShutUpStatus IS NULL SET @ShutUpStatus=0
	IF @ShutDownStatus IS NULL SET @ShutDownStatus=0
	IF @ShutUpOverTime IS NULL SET @ShutUpOverTime=0
	IF @ShutDownOverTime IS NULL SET @ShutDownOverTime=0
	IF @InsurePass IS NULL SET @InsurePass=N''
	
	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����"}'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @ShutDownStatus<>0
	BEGIN
		-- ����״̬
		IF dbo.LHF_GetTimestamp()<@ShutDownOverTime
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1136,"Msg":"��ǰ�ʺ��ѱ���ʱ�����%s�����ʱ�䣩, ���޷�ʹ�á������������ʣ�����ϵ�ͷ���","Ops":["'+CONVERT(NVARCHAR(100),LHAccountsDB.dbo.LHF_ConvertTimestampToLocalTime(@ShutDownOverTime),120)+'"]}'	
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

	-- ��½����
	DECLARE @SecretKey NVARCHAR(128)
	SET @SecretKey=REPLACE(NEWID(), N'-', N'')

	-- ��¼����
	SET @GameLogonTimes+=1
	
	-- ������Ϣ
	UPDATE AccountsInfo SET SecretKey=@SecretKey,GameLogonTimes=@GameLogonTimes,LogonDate=GETDATE(), LogonIP=@strClientIP, LogonMode=5,LogonDeviceKind=@dwDeviceKind,
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
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID,GameLogonSuccess) VALUES (@DateID, 1)	

	-- ��Ҳ�ѯ
	SELECT @UserGold=Score,@InsureGold=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@UserID
	IF @UserGold IS NULL OR @InsureGold IS NULL
	BEGIN
		SET @UserGold=0
		SET @InsureGold = 0
	END

	-- ��ʯ��Ϣ
	SELECT @UserCard=Score FROM LHGameBattleDB.dbo.GameCardInfo(NOLOCK) WHERE UserID=@UserID

	-- ��ѯӶ��
	SELECT @Commission=Commission FROM AccountsCommission WHERE UserID=@UserID

	-- �����Ƹ�
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
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName, @LogonPass AS Password, @InsurePass AS InsurePass, @UnderWrite AS UnderWrite, @FaceID AS FaceID, @CustomID AS CustomID,
		@ProtectID AS ProtectID,@MasterOrder AS MasterOrder, @UserGold AS UserGold, @InsureGold AS InsureGold,@Commission AS Commission, @SecretKey AS SecretKey, @UserCard AS UserCard, @Gender AS Gender, @Experience AS Experience,
		@LoveLiness AS LoveLiness, @MemberPoint AS MemberPoint, @MemberOverTime AS MemberOverTime, @LockGateID AS LockGateID, @LockKindID As LockKindID, @LockLogonID AS LockLogonID, @LockSocketID AS LockSocketID,
		@LockServerID AS LockServerID, @MobilePhone AS MobilePhone,@Email AS Email, @StrUniqueID AS UniqueID, @RegisterTime AS RegisterTime, @LikedNumber AS LikedNumber,@InfoPublic AS InfoPublic, @ActionMaskEver AS ActionMaskEver,
		@ActionMaskPerDay AS ActionMaskPerDay, @ActionMaskPerWeek AS ActionMaskPerWeek, @GameLogonTimes AS GameLogonTimes, @LockServerKind AS LockServerKind,@ParentGameID AS ParentGameID,@ParentFaceID AS ParentFaceID,
		@ParentNickName AS ParentNickName,@ShutUpStatus AS ShutUpStatus, @ShutUpOverTime AS ShutUpOverTime, @RegisterMode AS RegisterMode, @UIShieldMask AS UIShieldMask, @ShieldKindList AS ShieldKindList, @LogonDateTime AS LastLogonDate,
		@UserRight AS UserRight, @szCompellation AS Compellation,@szPassPortID AS PassPortID,@szPresentGoodsList AS PresentGoodsList,@CheckInDays AS CheckInDays,@CheckInState AS CheckInState,@wNickNameDailyModifyTimes AS NickNameDailyModifyTimes,
		@wNickNameModifyTimes AS NickNameModifyTimes
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
