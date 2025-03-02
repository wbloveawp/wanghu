
USE LHGameRankingDB
GO
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_EfficacyRobot]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_EfficacyRobot]
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

-- ������¼
CREATE PROC GSP_GR_EfficacyRobot
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ	
	@strMachineID NCHAR(32),					-- ������ʶ	
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT,						-- ���� I D
	@dwRequestScore BIGINT,						-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �Զ��ύ
SET XACT_ABORT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @FaceID SMALLINT
DECLARE @ActorID SMALLINT
DECLARE @FaceFrameID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @CustomID INT
DECLARE @CardBackID SMALLINT

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
DECLARE @Gold BIGINT
DECLARE @Score BIGINT
DECLARE @Ingot BIGINT
DECLARE @LoveHeart	INT
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
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		-- ��ʾ��Ϣ
		DECLARE @StatusString NVARCHAR(128)
		SELECT @StatusString=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		
		-- ������ʾ
		IF @StatusString IS NULL SET @StatusString=N''

		SET @strErrorDescribe=N'{"Idx":1002,"Msg":"'+@StatusString+N'"}'
		RETURN 2
	END
 
	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @IsRobot TINYINT
	DECLARE @LogonPass NCHAR(32)
	DECLARE	@MachineID NCHAR(32)
	DECLARE @MoorMachine TINYINT	
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @UnderWrite=UnderWrite, @LogonPass=LogonPass, @FaceID=FaceID, @CardBackID=CardBackID, @FaceFrameID=FaceFrameID, @ActorID=ActorID, @CustomID=CustomID, 
		  @Gender=Gender, @Nullity=Nullity, @Experience=Experience, @UserRight=UserRight, @MasterRight=MasterRight,@MasterOrder=MasterOrder, @MemberPoint=MemberPoint, @IsRobot=IsRobot, 
		  @MoorMachine=MoorMachine, @MachineID=LogonMachineID
	FROM LHAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

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
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�"}'
		RETURN 2
	END	
	
	-- �̶�����
	IF @MoorMachine=1
	BEGIN
		IF @MachineID<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1024,"Msg":"�����ʺ�ʹ������������½���ܣ�������ʹ�õĻ���������ָ���Ļ�����"}'
			RETURN 1
		END
	END

	-- �����ж�	
	IF @strClientIP<>N'0.0.0.0'
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
		RETURN 3			
	END			

	-- �̶�����
	IF @MoorMachine=2
	BEGIN
		SET @MoorMachine=1
		SET @strErrorDescribe=N'{"Idx":1025,"Msg":"�����ʺųɹ�ʹ��������������½���ܣ�"}'
		UPDATE LHAccountsDB.dbo.AccountsInfo SET MoorMachine=@MoorMachine, LogonMachineID=@strMachineID WHERE UserID=@UserID
	END

	-- ��ѯ����
	SELECT @Age=Age, @Area=Area, @City=City, @Province=Province FROM LHAccountsDB.dbo.AccountsDatum(NOLOCK) WHERE UserID=@dwUserID

	-- ��������
	IF @Age IS NULL SET @Age=N''
	IF @Area IS NULL SET @Area=N''
	IF @City IS NULL SET @City=N''
	IF @Province IS NULL SET @Province=N''

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
		SET @strErrorDescribe='{"Idx":1102,"Msg":"���Ѿ��ڽ����Ϸ�����ˣ�����ͬʱ�ٽ��������Ϸ�����ˣ�"}'  
		RETURN 100
	END

	-- ʱ���ʶ
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN	

	-- ��Ϸ��Ϣ
	SELECT @Gold=Score, @InsureScore=InsureScore FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID	

	-- ��Ϸ��Ϣ
	DECLARE @PlayTimeCount INT
	SELECT @Score=Score, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount, @FleeCount=FleeCount, @PlayTimeCount=PlayTimeCount,@WinStreak=WinStreak,
		   @MaxWinStreak=MaxWinStreak,@LoveHeart=LoveHeart
	FROM GameScoreInfo WHERE UserID=@dwUserID

	-- ��Ϣ�ж�
	IF @Score IS NULL
	BEGIN
		-- ��������
		INSERT INTO GameScoreInfo (UserID,RegisterIP, RegisterMachineID,LogonIP,LogonMachineID)	
		VALUES (@dwUserID,@strClientIP,@strMachineID,@strClientIP,@strMachineID)

		-- ��Ϸ��Ϣ		
		SELECT @Score=Score, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount, @FleeCount=FleeCount, @PlayTimeCount=PlayTimeCount,
				@WinStreak=WinStreak,@MaxWinStreak=MaxWinStreak,@LoveHeart=LoveHeart
		FROM GameScoreInfo WHERE UserID=@dwUserID
	END

	-- ������Ϣ
	UPDATE GameScoreInfo SET LogonTimes=LogonTimes+1, LogonDate=GETDATE(), LogonIP=@strClientIP, LogonMachineID=@strMachineID WHERE UserID=@dwUserID

	-- ��¼ͳ��	
	UPDATE SystemStreamInfo SET LogonCount=LogonCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, LogonCount) VALUES (@DateID, @wKindID, @wServerID, 1)

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- ��ʯ��Ϣ
	SELECT @Ingot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID

	-- ��������
	IF @Ingot IS NULL SET @Ingot=0

	-- ����״̬
	UPDATE LHAccountsDB.dbo.AccountsOnlineInfo SET KindID=@wKindID,ServerID=@wServerID,ServerKind=0x10,ModifyTime=GETDATE() WHERE UserID=@dwUserID

	-- �����¼
	INSERT RecordUserInout(UserID, KindID, ServerID, LockerScore, EnterTime, EnterScore, EnterMachine, EnterClientIP)
	VALUES(@dwUserID,@wKindID,@wServerID,@Score,GetDate(),@Score,@strMachineID,@strClientIP)
	
	-- ��¼��ʶ
	DECLARE @InoutIndex BIGINT
	SET @InoutIndex=SCOPE_IDENTITY()	

	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @NickName AS Accounts, @UnderWrite AS UnderWrite, @FaceID AS FaceID, @FaceFrameID AS FaceFrameID, @CardBackID AS CardBackID , @ActorID AS ActorID, @CustomID AS CustomID, @Age AS Age, @Gender AS Gender, 
		@Province AS Province, @City AS City, @Area AS Area, @MasterOrder AS MasterOrder, @UserRight AS UserRight, @MasterRight AS MasterRight, @MasterOrder AS MasterOrder, @MemberPoint AS MemberPoint, @Gold AS Gold, 
		@Score AS Score, @Ingot AS Ingot, @WinCount AS WinCount, @LostCount AS LostCount, @DrawCount AS DrawCount, @FleeCount AS FleeCount, @WinStreak AS WinStreak, @MaxWinStreak AS MaxWinStreak, @LoveHeart AS LoveHeart, 
		@InsureScore AS InsureScore, @Score AS FreeScore, @Experience AS Experience, @InoutIndex AS InoutIndex, 0 AS ShutUpStatus, 0 AS ShutUpOverTime

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���Ե�¼
CREATE PROC GSP_GR_EfficacyComputer
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ	
	@strMachineID NCHAR(32),					-- ������ʶ	
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT,						-- ���� I D
	@dwRequestScore BIGINT,						-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �Զ��ύ
SET XACT_ABORT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @FaceID SMALLINT
DECLARE @FaceFrameID SMALLINT
DECLARE @ActorID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @CustomID INT
DECLARE @CardBackID SMALLINT

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
DECLARE @Gold BIGINT
DECLARE @Score BIGINT
DECLARE @Ingot BIGINT
DECLARE @LoveHeart	INT
DECLARE @InsureScore BIGINT
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @Experience INT
DECLARE @WinStreak INT
DECLARE @MaxWinStreak INT

-- ��������
DECLARE @EnjoinLogon BIGINT

-- ִ���߼�
BEGIN
	-- ϵͳ��ͣ
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		-- ��ʾ��Ϣ
		DECLARE @StatusString NVARCHAR(128)
		SELECT @StatusString=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		
		-- ������ʾ
		IF @StatusString IS NULL SET @StatusString=N''

		SET @strErrorDescribe=N'{"Idx":1002,"Msg":"'+@StatusString+N'"}'
		RETURN 2
	END
 
	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @ShutUpStatus BIT
	DECLARE @ShupDownStatus BIT
	DECLARE @ShutUpOverTime INT
	DECLARE	@MachineID NCHAR(32)
	DECLARE @LogonPass AS NCHAR(32)	
	
	-- ��ѯ�û�
	SELECT @UserID=a.UserID, @GameID=a.GameID, @Accounts=a.Accounts, @NickName=a.NickName, @UnderWrite=a.UnderWrite, @LogonPass=a.LogonPass, @FaceID=a.FaceID, @CardBackID=a.CardBackID, @FaceFrameID=a.FaceFrameID, @ActorID=a.ActorID, 
		@CustomID=a.CustomID, @Gender=a.Gender, @Nullity=a.Nullity, @Experience=a.Experience, @UserRight=a.UserRight, @MasterRight=a.MasterRight, @MasterOrder=a.MasterOrder, @MemberPoint=a.MemberPoint, 
		@MachineID=a.LogonMachineID,@ShupDownStatus=b.ShutDownStatus, @ShutUpStatus=b.ShutupStatus, @ShutUpOverTime=b.ShutupOverTime
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
	IF @ShupDownStatus<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�"}'
		RETURN 2
	END	
	
	-- �����ж�	
	IF (@strPassword=N'') OR (@LogonPass<>@strPassword AND @strClientIP<>N'0.0.0.0')
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
		RETURN 3
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
		SET @strErrorDescribe='{"Idx":1102,"Msg":"���Ѿ��ڽ����Ϸ�����ˣ�����ͬʱ�ٽ��������Ϸ�����ˣ�"}'  
		RETURN 100
	END

	-- ʱ���ʶ
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��Ϸ��Ϣ
	DECLARE @PlayTimeCount INT
	SELECT @Score=Score, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount, @FleeCount=FleeCount, @PlayTimeCount=PlayTimeCount,@WinStreak=WinStreak,
		   @MaxWinStreak=MaxWinStreak,@LoveHeart=LoveHeart
	FROM GameScoreInfo WHERE UserID=@dwUserID

	-- ��Ϣ�ж�
	IF @Score IS NULL
	BEGIN
		-- ��������
		INSERT INTO GameScoreInfo (UserID,RegisterIP, RegisterMachineID,LogonIP,LogonMachineID)	
		VALUES (@dwUserID,@strClientIP,@strMachineID,@strClientIP,@strMachineID)

		-- ��Ϸ��Ϣ		
		SELECT @Score=Score, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount, @FleeCount=FleeCount, @PlayTimeCount=PlayTimeCount,@WinStreak=WinStreak,
			   @MaxWinStreak=MaxWinStreak,@LoveHeart=LoveHeart
		FROM GameScoreInfo WHERE UserID=@dwUserID
	END

	-- ������Ϣ
	UPDATE GameScoreInfo SET LogonTimes=LogonTimes+1, LogonDate=GETDATE(), LogonIP=@strClientIP, LogonMachineID=@strMachineID WHERE UserID=@dwUserID

	-- ��¼ͳ��	
	UPDATE SystemStreamInfo SET LogonCount=LogonCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, LogonCount) VALUES (@DateID, @wKindID, @wServerID, 1)

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- �����Ϣ
	SELECT @Gold=Score, @InsureScore=InsureScore FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID

	-- ��ʯ��Ϣ
	SELECT @Ingot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID

	-- ��������
	IF @Gold IS NULL SET @Gold=0
	IF @Ingot IS NULL SET @Ingot=0
	IF @InsureScore IS NULL SET @InsureScore=0
	
	-- ����״̬
	UPDATE LHAccountsDB.dbo.AccountsOnlineInfo SET KindID=@wKindID,ServerID=@wServerID,ServerKind=0x10,ModifyTime=GETDATE() WHERE UserID=@dwUserID

	-- �����¼
	INSERT RecordUserInout(UserID, KindID, ServerID, LockerScore, EnterTime, EnterScore, EnterMachine, EnterClientIP)
	VALUES(@dwUserID,@wKindID,@wServerID,@Score,GetDate(),@Score,@strMachineID,@strClientIP)
	
	-- ��¼��ʶ
	DECLARE @InoutIndex BIGINT
	SET @InoutIndex=SCOPE_IDENTITY()

	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @NickName AS Accounts, @UnderWrite AS UnderWrite, @FaceID AS FaceID, @FaceFrameID AS FaceFrameID, @CardBackID AS CardBackID , @ActorID AS ActorID, @CustomID AS CustomID, @Age AS Age, 
		@Gender AS Gender, @Province AS Province, @City AS City, @Area AS Area, @MasterOrder AS MemberOrder, @UserRight AS UserRight, @MasterRight AS MasterRight, @MasterOrder AS MasterOrder, 
		@MemberPoint AS MemberPoint, @Gold AS Gold, @Score AS Score, @Ingot AS Ingot, @WinCount AS WinCount, @LostCount AS LostCount, @DrawCount AS DrawCount, @FleeCount AS FleeCount,@WinStreak AS WinStreak, 
		@MaxWinStreak AS MaxWinStreak, @LoveHeart AS LoveHeart, @InsureScore AS InsureScore, 0 AS FreeScore, @Experience AS Experience, @InoutIndex AS InoutIndex, @ShutUpStatus AS ShutUpStatus, 
		@ShutUpOverTime AS ShutUpOverTime

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �ֻ���¼
CREATE PROC GSP_GR_EfficacyMobile
	@dwUserID INT,								-- �û� I D
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NCHAR(32),					-- ������ʶ	
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT,						-- ���� I D
	@dwRequestScore BIGINT,						-- �������
	@cbDeviceType SMALLINT,						-- �豸����
	@dwRankingNum INT,							-- ��λ����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �Զ��ύ
SET XACT_ABORT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @FaceID SMALLINT
DECLARE @FaceFrameID SMALLINT
DECLARE @ActorID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @CustomID INT
DECLARE @CardBackID SMALLINT

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
DECLARE @Gold BIGINT
DECLARE @Score BIGINT
DECLARE @Ingot BIGINT
DECLARE @LoveHeart INT
DECLARE @InsureScore BIGINT
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @Experience INT
DECLARE @WinStreak INT
DECLARE @MaxWinStreak INT

-- ��������
DECLARE @EnjoinLogon BIGINT

-- ִ���߼�
BEGIN
	-- ϵͳ��ͣ
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		-- ��ʾ��Ϣ
		DECLARE @StatusString NVARCHAR(128)
		SELECT @StatusString=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		
		-- ������ʾ
		IF @StatusString IS NULL SET @StatusString=N''

		SET @strErrorDescribe=N'{"Idx":1002,"Msg":"'+@StatusString+N'"}'
		RETURN 2
	END
 
	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @ShutUpStatus BIT
	DECLARE @ShupDownStatus BIT
	DECLARE @ShutUpOverTime INT
	DECLARE @LogonPass NCHAR(32)
	DECLARE	@MachineID NCHAR(32)	

	-- ��ѯ�û�
	SELECT @UserID=a.UserID, @GameID=a.GameID, @Accounts=a.Accounts, @NickName=a.NickName, @UnderWrite=a.UnderWrite, @LogonPass=a.LogonPass, @FaceID=a.FaceID, @CardBackID=a.CardBackID, @FaceFrameID=a.FaceFrameID, @ActorID=a.ActorID, 
		@CustomID=a.CustomID, @Gender=a.Gender, @Nullity=a.Nullity, @Experience=a.Experience, @UserRight=a.UserRight, @MasterRight=a.MasterRight, @MemberPoint=a.MemberPoint, @MasterOrder=a.MasterOrder, 
		@MachineID=a.LogonMachineID, @ShupDownStatus=b.ShutDownStatus, @ShutUpStatus=b.ShutupStatus, @ShutUpOverTime=b.ShutupOverTime
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
	IF @ShupDownStatus<>0
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
		SET @strErrorDescribe='{"Idx":1102,"Msg":"���Ѿ��ڽ����Ϸ�����ˣ�����ͬʱ�ٽ��������Ϸ�����ˣ�"}'  
		RETURN 100
	END

	-- ʱ���ʶ
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN	

	-- ��Ϸ��Ϣ
	DECLARE @PlayTimeCount INT
	SELECT @Score=Score, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount, @FleeCount=FleeCount, @PlayTimeCount=PlayTimeCount,@WinStreak=WinStreak,
		   @MaxWinStreak=MaxWinStreak,@LoveHeart=LoveHeart
	FROM GameScoreInfo WHERE UserID=@dwUserID

	-- ��Ϣ�ж�
	IF @Score IS NULL
	BEGIN
		-- ��������
		INSERT INTO GameScoreInfo (UserID, RegisterIP, RegisterMachineID,LogonIP,LogonMachineID)	
		VALUES (@dwUserID,@strClientIP,@strMachineID,@strClientIP,@strMachineID)

		-- ��Ϸ��Ϣ		
		SELECT @Score=Score, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount, @FleeCount=FleeCount, @PlayTimeCount=PlayTimeCount,@WinStreak=WinStreak,
			   @MaxWinStreak=MaxWinStreak,@LoveHeart=LoveHeart
		FROM GameScoreInfo WHERE UserID=@dwUserID
	END

	-- ������Ϣ
	UPDATE GameScoreInfo SET LogonTimes=LogonTimes+1, LogonDate=GETDATE(), LogonIP=@strClientIP, LogonMachineID=@strMachineID WHERE UserID=@dwUserID

	-- ��¼ͳ��	
	UPDATE SystemStreamInfo SET LogonCount=LogonCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, LogonCount) VALUES (@DateID, @wKindID, @wServerID, 1)

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- �����Ϣ
	SELECT @Gold=Score, @InsureScore=InsureScore FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID

	-- ��������
	IF @Score IS NULL
	BEGIN		
		SELECT @Score=0,@InsureScore=0
		INSERT INTO LHAccountsDB.dbo.AccountsGoldInfo(UserID,Score,InsureScore) VALUES (@dwUserID,@Score,@InsureScore)		
	END

	-- ��ʯ��Ϣ
	SELECT @Ingot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID

	-- ��������
	IF @Gold IS NULL SET @Gold=0
	IF @Ingot IS NULL SET @Ingot=0
	IF @InsureScore IS NULL SET @InsureScore=0
	
	-- ����״̬
	UPDATE LHAccountsDB.dbo.AccountsOnlineInfo SET KindID=@wKindID,ServerID=@wServerID,ServerKind=0x10,ModifyTime=GETDATE() WHERE UserID=@dwUserID

	-- �����¼
	INSERT RecordUserInout(UserID, KindID, ServerID, LockerScore, EnterTime, EnterScore, EnterMachine, EnterClientIP)
	VALUES(@dwUserID,@wKindID,@wServerID,@Score,GetDate(),@Score,@strMachineID,@strClientIP)

	-- ��¼��ʶ
	DECLARE @InoutIndex BIGINT
	SET @InoutIndex=SCOPE_IDENTITY()

	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @NickName AS Accounts, @UnderWrite AS UnderWrite, @FaceID AS FaceID, @FaceFrameID AS FaceFrameID, @CardBackID AS CardBackID , @ActorID AS ActorID, @CustomID AS CustomID, @Age AS Age, 
		@Gender AS Gender, @Province AS Province, @City AS City, @Area AS Area, @MasterOrder AS MemberOrder, @MemberPoint AS MemberPoint, @UserRight AS UserRight, @MasterRight AS MasterRight, @MasterOrder AS MasterOrder, @Gold AS Gold, 
		@Score AS Score, @Ingot AS Ingot, @WinCount AS WinCount, @LostCount AS LostCount, @DrawCount AS DrawCount, @FleeCount AS FleeCount, @InsureScore AS InsureScore, @Score AS FreeScore, @Experience AS Experience,@WinStreak AS WinStreak, 
		@MaxWinStreak AS MaxWinStreak, @InoutIndex AS InoutIndex, @LoveHeart AS LoveHeart, @ShutUpStatus AS ShutUpStatus, @ShutUpOverTime AS ShutUpOverTime

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
