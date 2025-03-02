
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_EfficacyComputer]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_EfficacyComputer]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_EfficacyMobile]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_EfficacyMobile]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ���Ե�¼
CREATE PROC GSP_GR_EfficacyComputer
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT,						-- ���� I D
	@dwRequestScore BIGINT,						-- �������	
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @FaceID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @CustomID INT

-- ��չ��Ϣ
DECLARE @GameID INT
DECLARE @UserRight INT
DECLARE @MasterRight INT
DECLARE @MemberPoint INT
DECLARE @MasterOrder SMALLINT
DECLARE @MemberOrder SMALLINT

-- �û���Ϣ
DECLARE @Age TINYINT
DECLARE @Gender TINYINT
DECLARE @Area NVARCHAR(15)
DECLARE @City NVARCHAR(15)
DECLARE @Province NVARCHAR(15)

-- ���ֱ���
DECLARE @Score BIGINT
DECLARE @UserGold BIGINT
DECLARE @UserCard BIGINT
DECLARE @InsureScore BIGINT
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @Experience INT
DECLARE @WinStreak INT
DECLARE @MaxWinStreak INT

-- ��������
DECLARE @LockerScore BIGINT
DECLARE @EnjoinLogon BIGINT

-- ִ���߼�
BEGIN
	-- ϵͳ��ͣ
	SELECT @EnjoinLogon=StatusValue FROM LHPlatformDB..SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		-- ��ʾ��Ϣ
		DECLARE @StatusString NVARCHAR(128)
		SELECT @StatusString=StatusString FROM LHPlatformDB..SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		
		-- ������ʾ
		IF @StatusString IS NULL SET @StatusString=N''

		SET @strErrorDescribe=N'{"Idx":1002,"Msg":"'+@StatusString+N'"}'
		RETURN 2
	END
 
	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @ShutUpStatus BIT
	DECLARE @ShutDownStatus BIT
	DECLARE @ShutUpOverTime INT
	DECLARE @LogonPass NCHAR(32)	
	
	-- ��ѯ�û�
	SELECT @UserID=a.UserID, @GameID=a.GameID, @Accounts=a.Accounts, @NickName=a.NickName, @UnderWrite=a.UnderWrite, @LogonPass=a.LogonPass, @FaceID=a.FaceID,  
		@CustomID=a.CustomID, @Gender=a.Gender, @Nullity=a.Nullity, @Experience=a.Experience, @UserRight=a.UserRight, @MasterRight=a.MasterRight, @MasterOrder=a.MasterOrder,
		@MemberPoint=a.MemberPoint, @ShutDownStatus=b.ShutDownStatus, @ShutUpStatus=b.ShutupStatus, @ShutUpOverTime=b.ShutupOverTime		
	FROM LHAccountsDB.dbo.AccountsInfo a LEFT JOIN LHAccountsDB.dbo.AccountsStatus b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

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
	IF @ShutDownStatus<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�"}'
		RETURN 2
	END

	-- ��ѯ����
	SELECT @Age=Age, @Area=Area, @City=City, @Province=Province FROM LHAccountsDB.dbo.AccountsDatum(NOLOCK) WHERE UserID=@dwUserID

	-- ��������
	IF @Age IS NULL SET @Age=N''
	IF @Area IS NULL SET @Area=N''
	IF @City IS NULL SET @City=N''
	IF @Province IS NULL SET @Province=N''

	-- ����״̬
	IF @ShutUpStatus IS NULL SET @ShutUpStatus=0
	IF @ShutUpOverTime IS NULL SET @ShutUpOverTime=0

	-- �����ж�
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT	
	DECLARE @LockServerKind INT	

	-- ��ѯ����
	SELECT @LockKindID=KindID,@LockServerID=ServerID,@LockServerKind=ServerKind FROM LHAccountsDB.dbo.AccountsOnlineInfo(NOLOCK) WHERE UserID=@dwUserID

	-- �����ж�
	IF @LockServerID<>0 AND @LockServerID<>@wServerID
	BEGIN	
		-- ������Ϣ
		SELECT @LockKindID AS LockKindID,@LockServerID AS LockServerID,@LockServerKind AS LockServerKind

		-- ��ʾ��Ϣ
		SET @strErrorDescribe='{"Idx":1102,"Msg":"���Ѿ�����Ϸ�����ˣ�����ͬʱ�ٽ���������Ϸ���䣡"}'  
		RETURN 100
	END

	-- ���÷���
	SELECT @Score=0,@InsureScore=0,@WinCount=0, @LostCount=0, @DrawCount=0, @DrawCount=0, @FleeCount=0,@WinStreak=0, @MaxWinStreak=0

	-- ��ѯ�Ƹ�
	SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID
	SELECT @UserCard=Score FROM LHGameBattleDB.dbo.GameCardInfo(NOLOCK) WHERE UserID=@dwUserID

	-- ��������
	IF @UserCard IS NULL SET @UserCard=0
	IF @UserGold IS NULL SET @UserGold=0

	-- ������Ϣ
	UPDATE GameScoreInfo SET AllLogonTimes=AllLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,LastLogonMachine=@strMachineID WHERE UserID=@dwUserID

	-- �����¼
	INSERT RecordUserInout(UserID,KindID,ServerID,EnterTime,EnterScore,EnterMachine,EnterClientIP,ClubID,ScoreKind,BattleNum)
	VALUES(@dwUserID,@wKindID,@wServerID,GetDate(),@Score,@strMachineID,@strClientIP,0,0,0)
	
	-- ��¼��ʶ
	DECLARE @InoutIndex BIGINT
	SET @InoutIndex=SCOPE_IDENTITY()

	-- ���ڱ�ʶ
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��¼ͳ��	
	UPDATE SystemStreamInfo SET LogonCount=LogonCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, LogonCount) VALUES (@DateID, @wKindID, @wServerID, 1)

	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @LogonPass AS LogonPass, @NickName AS Accounts, @UnderWrite AS UnderWrite, @FaceID AS FaceID, 
		@CustomID AS CustomID, @Age AS Age, @Gender AS Gender, @Province AS Province, @City AS City, @Area AS Area, 0 AS ClubID, N'' AS ClubName, @MasterOrder AS MemberOrder, 
		@UserRight AS UserRight, @MasterRight AS MasterRight, @MasterOrder AS MasterOrder, @MemberPoint AS MemberPoint, @Score AS Score, 3 AS ScoreKind,
		@UserGold AS Gold, @UserCard AS UserCard, @WinCount AS WinCount, @LostCount AS LostCount, @DrawCount AS DrawCount, @FleeCount AS FleeCount, @InsureScore AS InsureScore, @Score AS FreeScore, 
		@Experience AS Experience,@InoutIndex AS InoutIndex,@WinStreak AS WinStreak, @MaxWinStreak AS MaxWinStreak,@ShutUpStatus AS ShutUpStatus, @ShutUpOverTime AS ShutUpOverTime

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �ֻ���¼
CREATE PROC GSP_GR_EfficacyMobile
	-- �û���Ϣ
	@dwUserID INT,								-- �û� I D
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT,						-- ���� I D
	@dwRequestScore BIGINT,						-- �������	
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NCHAR(32),					-- ������ʶ
	@dwDeviceKind SMALLINT,						-- �豸����

	-- Լս��Ϣ
	@dwClubID INT,								-- ��ݱ�ʶ	
	@dwBattleNum INT,							-- Լս����
	@cbScoreKind TINYINT,						-- ��������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @FaceID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @CustomID INT

-- ��չ��Ϣ
DECLARE @GameID INT
DECLARE @UserRight INT
DECLARE @MasterRight INT
DECLARE @MasterOrder SMALLINT
DECLARE @MemberPoint INT

-- �û���Ϣ
DECLARE @Age TINYINT
DECLARE @Gender TINYINT
DECLARE @Area NVARCHAR(15)
DECLARE @City NVARCHAR(15)
DECLARE @Province NVARCHAR(15)

-- ���ֱ���
DECLARE @Score BIGINT
DECLARE @UserGold BIGINT
DECLARE @UserCard BIGINT
DECLARE @InsureScore BIGINT
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @Experience INT
DECLARE @WinStreak INT
DECLARE @MaxWinStreak INT

-- Ⱥ����Ϣ
DECLARE @ClubMemberType TINYINT

-- ��������
DECLARE @LockerScore BIGINT
DECLARE @EnjoinLogon BIGINT

-- ִ���߼�
BEGIN
	-- ϵͳ��ͣ
	SELECT @EnjoinLogon=StatusValue FROM LHPlatformDB..SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		-- ��ʾ��Ϣ
		DECLARE @StatusString NVARCHAR(128)
		SELECT @StatusString=StatusString FROM LHPlatformDB..SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		
		-- ������ʾ
		IF @StatusString IS NULL SET @StatusString=N''

		SET @strErrorDescribe=N'{"Idx":1002,"Msg":"'+@StatusString+N'"}'
		RETURN 2
	END
 
	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @ShutUpStatus BIT
	DECLARE @ShutDownStatus BIT
	DECLARE @ShutUpOverTime INT
	DECLARE @LogonPass NCHAR(32)	
	
	-- ��ѯ�û�
	SELECT @UserID=a.UserID, @GameID=a.GameID, @Accounts=a.Accounts, @NickName=a.NickName, @UnderWrite=a.UnderWrite, @LogonPass=a.LogonPass, @FaceID=a.FaceID,  
		@CustomID=a.CustomID, @Gender=a.Gender, @Nullity=a.Nullity, @Experience=a.Experience, @UserRight=a.UserRight, @MasterRight=a.MasterRight, @MasterOrder=a.MasterOrder,
		@MemberPoint=a.MemberPoint, @ShutDownStatus=b.ShutDownStatus, @ShutUpStatus=b.ShutupStatus, @ShutUpOverTime=b.ShutupOverTime		
	FROM LHAccountsDB.dbo.AccountsInfo a LEFT JOIN LHAccountsDB.dbo.AccountsStatus b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

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
	IF @ShutDownStatus<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�"}'
		RETURN 2
	END

	-- ��ѯ����
	SELECT @Age=Age, @Area=Area, @City=City, @Province=Province FROM LHAccountsDB.dbo.AccountsDatum(NOLOCK) WHERE UserID=@dwUserID

	-- ��������
	IF @Age IS NULL SET @Age=N''
	IF @Area IS NULL SET @Area=N''
	IF @City IS NULL SET @City=N''
	IF @Province IS NULL SET @Province=N''

	-- ����״̬
	IF @ShutUpStatus IS NULL SET @ShutUpStatus=0
	IF @ShutUpOverTime IS NULL SET @ShutUpOverTime=0

	-- �����ж�
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT	
	DECLARE @LockServerKind INT	

	-- ��ѯ����
	SELECT @LockKindID=KindID,@LockServerID=ServerID,@LockServerKind=ServerKind FROM LHAccountsDB.dbo.AccountsOnlineInfo(NOLOCK) WHERE UserID=@dwUserID

	-- �����ж�
	IF @LockServerID<>0 AND @LockServerID<>@wServerID
	BEGIN	
		-- ������Ϣ
		SELECT @LockKindID AS LockKindID,@LockServerID AS LockServerID,@LockServerKind AS LockServerKind

		-- ��ʾ��Ϣ
		SET @strErrorDescribe='{"Idx":1102,"Msg":"���Ѿ�����Ϸ�����ˣ�����ͬʱ�ٽ���������Ϸ���䣡"}'  
		RETURN 100
	END

	-- Լս����	
	DECLARE @BattleNum INT
	DECLARE @BattleStatus TINYINT
	DECLARE @OnlineStatus TINYINT
	SELECT @BattleNum=BattleNum,@BattleStatus=BattleStatus,@OnlineStatus=OnlineStatus FROM BattleUserLocker(NOLOCK) WHERE UserID=@dwUserID

	-- ����״̬
	IF @BattleStatus IS NULL SET @BattleStatus=0

	-- Լս״̬
	IF @BattleStatus=1
	BEGIN	
		-- У�鷿��
		IF @BattleNum<>@dwBattleNum
		BEGIN
			-- ��ʾ��Ϣ
			SET @strErrorDescribe=N'{"Idx":1177,"Msg":"��������Լս����['+CONVERT(NVARCHAR(12),@BattleNum)+']����Ϸ��δ��ɣ���ʱ���ܼ�������Լս���䣡","Ops":["'+CONVERT(NVARCHAR(12),@BattleNum)+'"]}'

			RETURN 5
		END
	END
	
	-- ����״̬
	IF @OnlineStatus=1
	BEGIN
		-- ��ʾ��Ϣ
		SET @strErrorDescribe=N'{"Idx":1177,"Msg":"��������Լս����['+CONVERT(NVARCHAR(12),@BattleNum)+']����Ϸ��δ��ɣ���ʱ���ܼ�������Լս���䣡","Ops":["'+CONVERT(NVARCHAR(12),@BattleNum)+'"]}'
		RETURN 5		
	END		

	-- ��Ϸ��Ϣ
	DECLARE @PlayTimeCount INT
	DECLARE @GameUserRight INT
	DECLARE @GameMasterRight INT
	DECLARE @GameMasterOrder SMALLINT	

	-- ���÷���
	SELECT @InsureScore=0

	-- ��ѯ��Ϸ��Ϣ
	SELECT  @Score=Score, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount, @FleeCount=FleeCount, @GameUserRight=UserRight, @GameMasterRight=MasterRight, 
			@WinStreak=WinStreak, @MaxWinStreak=MaxWinStreak, @GameMasterOrder=MasterOrder, @PlayTimeCount=PlayTimeCount
	FROM GameScoreInfo WHERE UserID=@dwUserID

	-- ��������
	IF @WinCount IS NULL
	BEGIN
		INSERT INTO GameScoreInfo(UserID,Score,AllLogonTimes,LastLogonIP,LastLogonDate,LastLogonMachine,RegisterIP,RegisterDate,RegisterMachine,WinStreak,MaxWinStreak)
		VALUES (@dwUserID,0,1,@strClientIP,GetDate(),@strMachineID,@strClientIP,GetDate(),@strMachineID,0,0)	

		SELECT @Score=0, @WinCount=0, @LostCount=0, @DrawCount=0, @DrawCount=0, @FleeCount=0,@WinStreak=0,@MaxWinStreak=0, @GameUserRight=0, @GameMasterRight=0, @GameMasterOrder=0,@PlayTimeCount=0		
	END

	-- ����Լս
	IF  @dwClubID<>0  
	BEGIN
		-- ��������
		DECLARE	@TableID	INT											
		DECLARE	@TableName	NVARCHAR(128)		
		DECLARE @SqlString	NVARCHAR(512)								
		DECLARE @ClubUserScore BIGINT
		DECLARE @ClubMemberStatus TINYINT

		-- �ֱ��ʶ				
		SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

		-- Ⱥ���Ա��
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

		-- ��ѯ���ݱ�
		IF (EXISTS(SELECT 1 FROM LHGameBattleDB.dbo.sysobjects WHERE xtype='u' AND name=@TableName))
		BEGIN
			-- ��ѯ�ű�		
			SET	@SqlString = CONCAT( ' SELECT @MemberStatus=MemberStatus,@MemberType=MemberType,@ClubScore=ClubScore FROM  ',
								  @TableName,'(NOLOCK) WHERE MemberID=',CONVERT(NVARCHAR(18), @dwUserID),' AND ',' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

			-- ִ�нű�
			EXEC SP_EXECUTESQL @stmt=@SqlString,@params=N'@MemberStatus TINYINT OUT,@MemberType TINYINT OUT, @ClubScore INT OUT', @MemberStatus=@ClubMemberStatus OUTPUT,@MemberType=@ClubMemberType OUTPUT,@ClubScore=@ClubUserScore OUTPUT
		END	

		-- ��������
		IF @ClubUserScore IS NULL SET @ClubUserScore=0
		IF @ClubMemberStatus IS NULL SET @ClubMemberStatus=0

		-- ����У��
		IF @ClubMemberStatus=1
		BEGIN
			-- ��ʾ��Ϣ
			SET @strErrorDescribe='{"Idx":1178,"Msg":"���ѱ����ֲ��ϰ嶳����ϷȨ�ޣ�����ϵ�ϰ�����"}'  
			RETURN 6
		END			
		
		-- �������		
		IF @cbScoreKind=2 SET @Score=@ClubUserScore
	END	

	-- ��������	
	IF @ClubMemberType IS NULL SET @ClubMemberType=0	

	-- ����Լս����
	IF @cbScoreKind=3 AND(@BattleNum IS NULL OR @BattleNum=0)
	BEGIN
		SET @Score=0		
	END

	-- ���¼�¼
	UPDATE GameScoreInfo SET Score=@Score,AllLogonTimes=AllLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP, LastLogonMachine=@strMachineID WHERE UserID=@dwUserID 	

	-- Լս����
	IF @BattleNum IS NULL
	BEGIN			
		INSERT INTO BattleUserLocker(UserID,KindID,ServerID,ScoreKind,BattleNum,OnlineStatus) VALUES(@dwUserID,@wKindID,@wServerID,@cbScoreKind,@dwBattleNum,1)
	END	ELSE
	BEGIN
		UPDATE BattleUserLocker SET KindID=@wKindID,ServerID=@wServerID,ScoreKind=@cbScoreKind,BattleNum=@dwBattleNum,OnlineStatus=1 WHERE UserID=@dwUserID		
	END
	
	-- ����״̬
	UPDATE LHAccountsDB.dbo.AccountsOnlineInfo SET KindID=@wKindID,ServerID=@wServerID,ServerKind=8,ModifyTime=GETDATE() WHERE UserID=@dwUserID

	-- ��ѯ�Ƹ�
	SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID
	SELECT @UserCard=Score FROM LHGameBattleDB.dbo.GameCardInfo(NOLOCK) WHERE UserID=@dwUserID

	-- ��������
	IF @UserCard IS NULL SET @UserCard=0	
	IF @UserGold IS NULL SET @UserGold=0

	-- Ȩ�ޱ�־
	SET @UserRight=@UserRight|@GameUserRight
	SET @MasterRight=@MasterRight|@GameMasterRight

	-- Ȩ�޵ȼ�
	IF @MasterOrder<>0 OR @GameMasterOrder<>0
	BEGIN
		IF @GameMasterOrder>@MasterOrder SET @MasterOrder=@GameMasterOrder
	END
	ELSE SET @MasterRight=0

	-- �����¼
	INSERT RecordUserInout(UserID,KindID,ServerID,DeviceType,EnterTime,EnterScore,EnterMachine,EnterClientIP,ClubID,ScoreKind,BattleNum)
	VALUES(@dwUserID,@wKindID,@wServerID,@dwDeviceKind,GetDate(),@Score,@strMachineID,@strClientIP,@dwClubID,@cbScoreKind,@dwBattleNum)
	
	-- ��¼��ʶ
	DECLARE @InoutIndex BIGINT
	SET @InoutIndex=SCOPE_IDENTITY()

	-- ���ڱ�ʶ
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��¼ͳ��	
	UPDATE SystemStreamInfo SET LogonCount=LogonCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, LogonCount) VALUES (@DateID, @wKindID, @wServerID, 1)

	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @LogonPass AS LogonPass, @NickName AS Accounts,@UnderWrite AS UnderWrite, @FaceID AS FaceID, @CustomID AS CustomID, @Age AS Age, @Gender AS Gender, 
		@Province AS Province, @City AS City,@Area AS Area, @MasterOrder AS MemberOrder, @MemberPoint AS MemberPoint, @UserRight AS UserRight, @MasterRight AS MasterRight, @MasterOrder AS MasterOrder, 
		@Score AS Score, @cbScoreKind AS ScoreKind, @UserGold AS Gold, @UserCard AS UserCard, @WinCount AS WinCount, @LostCount AS LostCount, @DrawCount AS DrawCount, @FleeCount AS FleeCount, 
		@WinStreak AS WinStreak, @MaxWinStreak AS MaxWinStreak,@InsureScore AS InsureScore, @Score AS FreeScore, @Experience AS Experience, @ShutUpStatus AS ShutUpStatus, @ShutUpOverTime AS ShutUpOverTime,
		@InoutIndex AS InoutIndex, @ClubMemberType AS MemberType

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------