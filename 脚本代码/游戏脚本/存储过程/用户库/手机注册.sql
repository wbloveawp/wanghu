
/***************************************************************************************************
** ���ƣ��ʺ�ע�� (��Ϸ)
***************************************************************************************************/

----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO


IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_RegisterMobilePhone]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_RegisterMobilePhone]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------


-- �ʺ�ע��
CREATE PROC [dbo].[GSP_MB_RegisterMobilePhone]
	@wGateID INT,								-- ���ر�ʶ	
	@wLogonID INT,								-- ��¼��ʶ
	@dwSocketID INT,							-- �����ʶ
	@dwSpreaderID INT,							-- ����ID
	@strAuthCode NCHAR(32),						-- ��֤��
	@strMobilePhone NVARCHAR(21),				-- �ֻ�����		
	@strNickName NVARCHAR(32),					-- �û��ǳ�
	@strPassword NCHAR(32),						-- �û�����	
	@wFaceID SMALLINT,							-- ͷ���ʶ
	@cbGender TINYINT,							-- �û��Ա�
	@wMarketID SMALLINT,						-- ������ʶ
	@dwDeviceKind INT,							-- �豸����
	@dwPlazaVersion INT,						-- �����汾
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NCHAR(32),					-- ������ʶ	
	@strMachineOSID NVARCHAR(64),				-- ϵͳ��ʶ
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
DECLARE @UserRight		INT
DECLARE @CustomID		INT
DECLARE @InsurePass		NVARCHAR(32)

-- ��չ��Ϣ
DECLARE @GameID INT
DECLARE @ProtectID INT
DECLARE @MasterOrder INT
DECLARE @LoveLiness INT
DECLARE @Experience INT
DECLARE @LikedNumber INT
DECLARE @InfoPublic TINYINT
DECLARE @MemberPoint INT
DECLARE @MemberOverTime INT
DECLARE @RegisterTime INT
DECLARE @RegisterMode TINYINT

-- �û���Ϣ
DECLARE @Gender TINYINT

-- ��Ϊ����
DECLARE @ActionMaskEver INT
DECLARE @ActionMaskPerDay INT
DECLARE @ActionMaskPerWeek INT

-- ��������
DECLARE @EnjoinLogon AS INT
DECLARE @EnjoinRegister AS INT

-- �Ƹ���Ϣ
DECLARE @UserGold BIGINT
DECLARE @UserCard BIGINT
DECLARE @InsureGold BIGINT
DECLARE @Commission BIGINT

-- ��������
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

	-- ע����ͣ
	SELECT @EnjoinRegister=CONVERT(INT,StatusValue),@StatusString=StatusString FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'EnjoinRegister'
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN		
		IF @StatusString IS NULL SET @StatusString=N''
		SET @strErrorDescribe=N'{"Idx":1001,"Msg":"'+@StatusString+N'"}'
		RETURN 1
	END

	-- ��½��ͣ
	SELECT @EnjoinLogon=CONVERT(INT,StatusValue),@StatusString=StatusString FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		IF dbo.LHF_IsAccountExcept(@strClientIP, @strMachineID)<>0
		BEGIN
			IF @StatusString IS NULL SET @StatusString=N''
			SET @strErrorDescribe=N'{"Idx":1002,"Msg":"'+@StatusString+N'"}'

			RETURN 1
		END

		SET @EnjoinLogon=0	
	END

	-- ��֤��Ϣ
	DECLARE @BindUserID INT
	DECLARE @AuthKind TINYINT
	DECLARE @AuthCodeMD5 NVARCHAR(32)
	DECLARE @AcquireMachine NVARCHAR(32)	
	DECLARE @LastAcquireDate DATETIME
	SELECT @BindUserID=UserID, @AuthKind=AuthKind, @AuthCodeMD5=AuthCodeMD5, @AcquireMachine=AcquireMachine, @LastAcquireDate=LastAcquireDate FROM RecordAuthCode(NOLOCK) WHERE CodeKind=1 AND MobileEmail=@strMobilePhone

	-- ��֤�벻����
	IF @BindUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1012,"Msg":"��֤�����ע��ʧ��1��"}'
		RETURN 6
	END

	-- �û���ƥ��
	IF @BindUserID<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1012,"Msg":"��֤�����ע��ʧ��2��"}'
		RETURN 6
	END

	-- У������
	IF @AuthKind<>7
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1012,"Msg":"��֤�����ע��ʧ��3��"}'
		RETURN 6		
	END
	
	-- У����֤��
	IF @AuthCodeMD5<>@strAuthCode
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1012,"Msg":"��֤�����ע��ʧ��4��"}'
		RETURN 7				
	END

	-- У�������
	IF @AcquireMachine<>@strMachineID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1013,"Msg":"���Ļ����뷢���쳣��ע��ʧ�ܣ�"}'
		RETURN 8
	END

	-- ��Чʱ��
	DECLARE @ACValidSeconds INT
	SELECT @ACValidSeconds=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ACValidSecond'

	-- ��������
	IF @ACValidSeconds IS NULL SET @ACValidSeconds=600

	-- ʱ����
	DECLARE @SecondDiff INT
	SET @SecondDiff=DateDiff(s,@LastAcquireDate,GETDATE())

	-- ʱ������
	IF @SecondDiff>@ACValidSeconds	
	BEGIN			
		SET @strErrorDescribe=N'{"Idx":1014,"Msg":"��֤���ѹ��ڣ������»�ȡ��֤�룡"}'
		RETURN 8
	END

	-- У���ǳ�
	--IF (SELECT COUNT(1) FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strNickName)>0)>0
	--BEGIN
	--	SET @strErrorDescribe=N'{"Idx":1004,"Msg":"��Ǹ��֪ͨ��������������ǳƺ��������ַ�����������Ǻ��ٴ������ǳƣ�"}'
	--	RETURN 4
	--END

	--IF @strNickName NOT LIKE '%[^0-9]%' AND LEN(@strNickName)=16
	--BEGIN
	--	SET @strErrorDescribe=N'{"Idx":1004,"Msg":"��Ǹ��֪ͨ��������������ǳƺ��������ַ�����������Ǻ��ٴ������ǳƣ�"}'
	--	RETURN 116
	--END

	-- Ч���ַ
	SELECT @EnjoinRegister=EnjoinRegister FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1005,"Msg":"��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ��ע�Ṧ�ܣ�����ϵ�ͻ����������˽���ϸ�����"}'
		RETURN 5
	END
	
	-- Ч�����
	SELECT @EnjoinRegister=EnjoinRegister FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1006,"Msg":"��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ�����ע�Ṧ�ܣ�����ϵ�ͻ����������˽���ϸ�����"}'
		RETURN 6
	END
 
	-- ��ѯ�û�
	IF EXISTS (SELECT 1 FROM AccountsInfo(NOLOCK) WHERE Accounts=@strMobilePhone)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1015,"Msg":"���ֻ������ѱ�ע�ᣬ�뻻��һ�ֻ����볢���ٴ�ע�ᣡ"}'
		RETURN 7
	END

	-- ��ѯ�û�
	--IF EXISTS (SELECT 1 FROM AccountsInfo(NOLOCK) WHERE NickName=@strNickName)
	--BEGIN
	--	SET @strErrorDescribe=N'{"Idx":1008,"Msg":"���ǳ��ѱ�ע�ᣬ�뻻��һ�ǳƳ����ٴ�ע�ᣡ"}'
	--	RETURN 7
	--END
	
	-- ��ѯ�ֻ�
	IF EXISTS (SELECT 1 FROM AccountsInfo(NOLOCK) WHERE MobilePhone=@strMobilePhone)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1016,"Msg":"���ֻ������ѱ�������Ϸ�˺Ű󶨣��뻻��һ�ֻ����볢���ٴ�ע�ᣡ"}'
		RETURN 7
	END	

	-- �����˺�
	SET @Accounts=CONCAT(N'Mobile',SUBSTRING(@strMobilePhone,LEN(@strMobilePhone)-3,4))
	SET @strNickName=CONCAT(SUBSTRING(@strMobilePhone,LEN(@strMobilePhone)-3,4),N':',N'Mobile')	

	-- �����˺�
	EXEC dbo.LHP_AdjustmentAccounts @Accounts OUTPUT,@strNickName OUTPUT
	
	-- ��½����
	DECLARE @SecretKey NVARCHAR(128)
	SET @SecretKey=REPLACE(NEWID(), N'-', N'')

	-- ע���û�
	INSERT AccountsInfo (Accounts,NickName,RegAccounts,LogonPass,InsurePass,Gender,FaceID,LogonMachineID, SecretKey, RegisterMode,RegisterMachineID,RegisterDeviceKind,RegisterPackageName,GameLogonTimes,
		  RegisterIP,LogonIP,LogonMode,LogonDeviceKind,PlazaVersion,MarketID,MobilePhone)
	VALUES (@strMobilePhone,@strNickName,@strMobilePhone,@strPassword,N'',@cbGender,@wFaceID,@strMachineID,@SecretKey,4,@strMachineID,@dwDeviceKind,@strPackageName,1,
	     @strClientIP,@strClientIP,4,@dwDeviceKind,@dwPlazaVersion,@wMarketID,@strMobilePhone)

	-- �����ж�
	IF @@ERROR<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1018,"Msg":"�ֻ����Ѵ��ڣ��뻻��һ�ֻ����볢���ٴ�ע�ᣡ"}'
		RETURN 8
	END	
	
	-- ��ѯ�û�
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts,@NickName=NickName,@FaceID=FaceID, @CustomID=CustomID, @ProtectID=ProtectID, @MasterOrder=MasterOrder, @Gender=Gender, @Experience=Experience,
		@LoveLiness=LoveLiness, @UnderWrite=UnderWrite,@UserRight=UserRight,@MemberPoint=MemberPoint, @MemberOverTime=MemberOverTime, @LikedNumber=LikedNumber,@InfoPublic=InfoPublic,@ActionMaskEver=ActionMaskEver,
		@ActionMaskPerDay=ActionMaskPerDay, @ActionMaskPerWeek=ActionMaskPerWeek,@RegisterMode=RegisterMode,@RegisterTime=dbo.LHF_ConvertTimestamp(RegisterDate)
	FROM AccountsInfo(NOLOCK) WHERE MobilePhone=@strMobilePhone

	-- �����ʶ
	SELECT @GameID=GameID FROM GameIdentifier(NOLOCK) WHERE UserID=@UserID
	IF @GameID IS NULL 
	BEGIN
		SET @GameID=0
		SET @strErrorDescribe=N'{"Idx":1010,"Msg":"�û�ע��ɹ�����δ�ɹ���ȡ��Ϸ ID ���룬ϵͳ�Ժ󽫸������䣡"}'
	END
	ELSE UPDATE AccountsInfo SET GameID=@GameID WHERE UserID=@UserID

	-- ���ڱ�ʶ
	DECLARE @DateID	INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	
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

	-- ��¼��־
	INSERT INTO RecordLogonInfo(UserID,DateID,DeviceKind,MachineOSID,PlazaVersion,ClientIP,MachineID,CollectDate)
	VALUES (@UserID,@DateID,@dwDeviceKind,@strMachineOSID,@dwPlazaVersion,@strClientIP,@strMachineID,GETDATE())

	-- ��¼ͳ��	
	UPDATE SystemStreamLogon SET LogonCount=LogonCount+1,LastDate=GETDATE() WHERE DateID=@DateID AND UserID=@UserID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO SystemStreamLogon(DateID,UserID,LogonCount) VALUES(@DateID,@UserID,1)
	END

	-- ע��ͳ��	
	UPDATE SystemStreamRegister SET TotalCount=TotalCount+1,LastDate=GETDATE()
	WHERE DateID=@DateID AND DeviceKind=@dwDeviceKind AND ExternalID=0
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO SystemStreamRegister(DateID,DeviceKind,ExternalID,TotalCount) VALUES(@DateID,@dwDeviceKind,0,1)
	END

	-- ��¼��־
	UPDATE SystemStreamInfo SET GameRegisterSuccess=GameRegisterSuccess+1 WHERE DateID=@DateID 
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID,GameRegisterSuccess) VALUES (@DateID, 1)
	   
	-- ��Ҳ�ѯ
	SELECT @UserGold=Score,@InsureGold=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@UserID
	IF @UserGold IS NULL OR @InsureGold IS NULL
	BEGIN
		SELECT @UserGold=0, @InsureGold = 0

		INSERT LHGameGoldDB.dbo.GameScoreInfo (UserID,Score,InsureScore)  VALUES (@UserID, @UserGold, 0) 
	END

	----------------------------------------------------------------------------------------------------

	-- ǩ����Ϣ 
	DECLARE @CheckInDays SMALLINT
	DECLARE @CheckInState TINYINT
	
	-- ��ѯǩ����Ϣ
	SELECT @CheckInDays=CheckInDays,@CheckInState=CheckInState FROM dbo.LHF_GetCheckInData(@UserID)	


	--------------------------------------ע������-----------------------------------------------------

	EXEC GSP_MB_RegisterPresent @UserID,4,@strClientIP,@strMachineID,@szPresentGoodsList OUTPUT

	--------------------------------------ע������-----------------------------------------------------

	-- ��ʯ��Ϣ
	SELECT @UserCard=Score FROM LHGameBattleDB.dbo.GameCardInfo(NOLOCK) WHERE UserID=@UserID

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
	SELECT @LockGateID=GateID,@LockLogonID=LogonID,@LockSocketID=SocketID,@LockServerID=ServerID,@LockKindID=KindID,@LockServerKind=ServerKind FROM AccountsOnlineInfo(NOLOCK) WHERE UserID=@UserID

	-- ��������
	IF @LockGateID IS NOT NULL
	BEGIN		
		-- ��������
		UPDATE AccountsOnlineInfo SET GateID=@wGateID,LogonID=@wLogonID,SocketID=@dwSocketID,InsertTime=GETDATE(),ModifyTime=GETDATE() WHERE UserID=@UserID	
	END ELSE
	BEGIN
		-- ��������
		SELECT @LockGateID=0,@LockLogonID=0,@LockSocketID=0,@LockServerID=0,@LockKindID=0,@LockServerKind=0

		-- ��������
		INSERT INTO AccountsOnlineInfo(UserID,GateID,LogonID,SocketID,InsertTime,ModifyTime) VALUES (@UserID,@wGateID,@wLogonID,@dwSocketID,GETDATE(),GETDATE())
	END

	--�汾ͳ��
	UPDATE PlazaVersionCollect SET LogonCount=LogonCount+1 WHERE PlazaVersion=@dwPlazaVersion AND DateID=@DateID
	IF @@ROWCOUNT=0 INSERT INTO PlazaVersionCollect (DateID,PlazaVersion,LogonCount,CollectDate) VALUES(@DateID,@dwPlazaVersion,1,GETDATE())

	-- ������Ϣ
	DECLARE @ATAuthTimes SMALLINT
	DECLARE @ATLastAcquireDate DATETIME
	SELECT @ATAuthTimes=AuthTimes,@ATLastAcquireDate=LastAcquireDate FROM AccountsAuthInfo(NOLOCK) WHERE UserID=@UserID

	-- ��������
	IF @ATAuthTimes IS NULL
	BEGIN
		INSERT INTO AccountsAuthInfo(UserID,CodeKind,AuthTimes,AcquireTimes,LastAcquireDate,LastMobileEmail) VALUES (@UserID,1,7,0,GETDATE(),N'')		
	END ELSE
	BEGIN
		-- ʱ����
		DECLARE @DateDiff INT
		SET @DateDiff=DateDiff(day,@ATLastAcquireDate,GETDATE())

		-- ��������
		IF @DateDiff<>0 SET @ATAuthTimes=0

		-- ��������
		UPDATE AccountsAuthInfo SET AuthTimes=@ATAuthTimes+1,AcquireTimes=0,LastAcquireDate=GETDATE(),LastMobileEmail=N'' WHERE UserID=@UserID AND CodeKind=1
	END

	-- ���´���
	UPDATE RecordAuthCode SET AuthCode=N'',AuthCodeMD5=N'',AuthTimes=AuthTimes+1,AcquireTimes=0 WHERE CodeKind=1 AND MobileEmail=@strMobilePhone

	-- ��¼��֤����
	UPDATE RecordAuthTimes SET AuthTimes=AuthTimes+1 WHERE DateID=@DateID AND UserID=@UserID AND CodeKind=1 AND AuthKind=@AuthKind
	IF @@ROWCOUNT=0
	BEGIN
		-- �����¼
		INSERT INTO RecordAuthTimes(DateID,UserID,CodeKind,AuthKind,AuthTimes) VALUES (@DateID,@UserID,1,@AuthKind,1)
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

	--�ǳ��޸Ĳ�ѯ
	DECLARE @wNickNameModifyTimes INT
	DECLARE @wNickNameDailyModifyTimes INT
	SELECT @wNickNameDailyModifyTimes=DailyModify,@wNickNameModifyTimes=TotalModify FROM RecordModifyUserIndividual WHERE DateID=@DateID AND UserID=@UserID
	IF @wNickNameModifyTimes IS NULL SELECT @wNickNameModifyTimes=COUNT(1) FROM RecordModifyUserIndividual WHERE UserID=@UserID
	IF @wNickNameModifyTimes IS NULL SET @wNickNameModifyTimes = 0
	IF @wNickNameDailyModifyTimes IS NULL SET @wNickNameDailyModifyTimes = 0

	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName, @strPassword AS Password, @InsurePass AS InsurePass, @FaceID AS FaceID, @CustomID AS CustomID, @ProtectID AS ProtectID,
		   @MasterOrder AS MasterOrder, @SecretKey AS SecretKey, @UserGold AS Gold, @InsureGold AS InsureGold, @UserCard AS UserCard, @UserGold AS UserGold, @Commission AS Commission, @Gender AS Gender, @LockGateID AS LockGateID,
		   @LockLogonID AS LockLogonID, @LockSocketID AS LockSocketID, @LockServerID AS LockServerID, @LockKindID AS LockKindID, @LockServerKind AS LockServerKind, @Experience AS Experience, @LoveLiness AS LoveLiness, 
		   @MemberPoint AS MemberPoint, @MemberOverTime AS MemberOverTime, @UnderWrite AS UnderWrite, @strMobilePhone AS MobilePhone, N'' AS Email, N'' AS UniqueID, @LikedNumber AS LikedNumber, @InfoPublic AS InfoPublic, 
		   @ActionMaskEver AS ActionMaskEver, @ActionMaskPerDay AS ActionMaskPerDay, @ActionMaskPerWeek AS ActionMaskPerWeek, @RegisterTime AS RegisterTime, 1 AS GameLogonTimes, @ParentGameID AS ParentGameID,@ParentFaceID AS ParentFaceID,
		   @ParentNickName AS ParentNickName, 0 AS ShutUpStatus, 0 AS ShutUpOverTime, @RegisterMode AS RegisterMode, @UIShieldMask AS UIShieldMask, @ShieldKindList AS ShieldKindList, @szCompellation AS Compellation, @szPassPortID AS PassPortID,
		   @szPresentGoodsList AS PresentGoodsList,@CheckInDays AS CheckInDays,@CheckInState AS CheckInState,@wNickNameDailyModifyTimes AS NickNameDailyModifyTimes,@wNickNameModifyTimes AS NickNameModifyTimes,@UserRight AS UserRight
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
