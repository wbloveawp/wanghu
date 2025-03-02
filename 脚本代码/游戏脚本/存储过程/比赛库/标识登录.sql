
----------------------------------------------------------------------------------------------------

USE LHGameMatchDB
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

-- I D ��¼
CREATE PROC GSP_GR_EfficacyComputer
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NCHAR(32),					-- ������ʶ	
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT,						-- ���� I D
	@dwRequestScore BIGINT,						-- �������
	@MatchID INT,								-- ���� I D 
	@MatchNo BIGINT,							-- ��������	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

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
DECLARE @StationID INT
DECLARE @UserRight INT
DECLARE @MasterRight INT
DECLARE @MasterOrder SMALLINT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberPoint INT
DECLARE @SignupCount INT

-- �û���Ϣ
DECLARE @Age TINYINT
DECLARE @Gender TINYINT
DECLARE @Area NVARCHAR(15)
DECLARE @City NVARCHAR(15)
DECLARE @Province NVARCHAR(15)

-- ���ֱ���
DECLARE @Score BIGINT
DECLARE @InsureScore BIGINT
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @WinStreak INT
DECLARE @Experience INT
DECLARE @TicketCount INT
DECLARE @MaxWinStreak INT

-- ��������
DECLARE @EnjoinLogon INT

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
	DECLARE @LogonPass NCHAR(32)		

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

	-- �����ж�
	IF @LogonPass<>@strPassword AND @strClientIP<>N'0.0.0.0' /*AND @strPassword<>N''*/
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

	-- ��ʼ����
	--SET TRANSACTION ISOLATION LEVEL REPEATABLE READ	
	--BEGIN TRAN

	-- ��Ϸ��Ϣ
	DECLARE @MatchRight INT
	DECLARE @GameUserRight INT
	DECLARE @GameMasterRight INT
	DECLARE @GameMasterOrder SMALLINT
	declare @LastLogonDate datetime
	SELECT @GameUserRight=UserRight, @GameMasterRight=MasterRight, @GameMasterOrder=MasterOrder,@LastLogonDate=LastLogonDate
	FROM GameScoreInfo WHERE UserID=@dwUserID

	-- ��Ϣ�ж�
	IF @GameUserRight IS NULL
	BEGIN
		-- ��������
		INSERT INTO GameScoreInfo (UserID,LastLogonIP,LastLogonMachine)	VALUES (@dwUserID,@strClientIP,@strMachineID)

		-- ��Ϸ��Ϣ
		SELECT @GameUserRight=UserRight, @GameMasterRight=MasterRight, @GameMasterOrder=MasterOrder,@LastLogonDate=LastLogonDate
		FROM GameScoreInfo WHERE UserID=@dwUserID		
	END	

	-- ������Ϣ	
	SELECT @Score=Score, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @FleeCount=FleeCount, @WinStreak=WinStreak, @MaxWinStreak=MaxWinStreak, @MatchRight=MatchRight, @SignupCount=SignupCount
	FROM MatchScoreInfo(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@MatchID AND MatchNo=@MatchNo
		
	-- ��Ϣ�ж�
	IF @Score IS NULL
	BEGIN
		-- ��Ϸ��Ϣ
		SELECT @Score=0,@WinCount=0,@LostCount=0,@DrawCount=0,@DrawCount=0,@FleeCount=0,@WinStreak=0,@MatchRight=0,@SignupCount=0,@MaxWinStreak=0		
	END
	ELSE
	BEGIN
		-- ����ʱ��
		UPDATE GameScoreInfo SET LastLogonDate=GETDATE() WHERE UserID=@dwUserID
	END	

	-- ��ѯ����ȯ
	SELECT @TicketCount=BuyTicketCount-UseTicketCount FROM StreamMatchTicket(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@MatchID AND MatchNo=@MatchNo

	-- ��������
	IF @TicketCount IS NULL SET @TicketCount=0

	-- ��������
	--COMMIT TRAN
	--SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- �����Ϣ
	DECLARE @Gold BIGINT
	SELECT @Gold=Score, @InsureScore=InsureScore FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID

	-- ��ʯ��Ϣ
	DECLARE @Ingot BIGINT
	SELECT @Ingot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID

	-- ��������
	IF @Gold IS NULl SET @Gold=0
	IF @Ingot IS NULL SET @Ingot=0
	IF @InsureScore IS NULL SET @InsureScore=0

	-- ����״̬
	UPDATE LHAccountsDB.dbo.AccountsOnlineInfo SET KindID=@wKindID,ServerID=@wServerID,ServerKind=0x04,ModifyTime=GETDATE() WHERE UserID=@dwUserID

	-- ����Ȩ��	
	IF @GameUserRight IS NULL SET @GameUserRight=0
	IF @GameMasterRight IS NULL SET @GameMasterRight=0

	-- �û�Ȩ��	
	SET @UserRight=@UserRight|@MatchRight|@GameUserRight

	-- ����Ȩ��	
	SET @MasterRight=@MasterRight|@GameMasterRight

	-- Ȩ�޵ȼ�
	IF @MasterOrder<>0 OR @GameMasterOrder<>0
	BEGIN
		IF @GameMasterOrder>@MasterOrder SET @MasterOrder=@GameMasterOrder
	END
	ELSE SET @MasterRight=0

	-- �����¼
	INSERT RecordUserInout(UserID,KindID,ServerID,LockerScore,EnterTime,EnterScore,EnterMachine,EnterClientIP)
	VALUES(@dwUserID,@wKindID,@wServerID,0,GetDate(),@Score,@strMachineID,@strClientIP)
	
	-- ��¼��ʶ
	DECLARE @InoutIndex BIGINT
	SET @InoutIndex=SCOPE_IDENTITY()

	-- ��������
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ͳ������
	UPDATE SystemStreamInfo SET LogonCount=LogonCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
	IF @@ROWCOUNT=0 
	BEGIN
		INSERT SystemStreamInfo (DateID, KindID, ServerID,LogonCount) VALUES (@DateID, @wKindID, @wServerID, 1)
	END

	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @NickName AS Accounts, @UnderWrite AS UnderWrite, @FaceID AS FaceID, @FaceFrameID AS FaceFrameID, @CardBackID AS CardBackID , @ActorID AS ActorID, @Age AS Age, @Gender AS Gender, 
		@Province AS Province, @City AS City, @Area AS Area, @MasterOrder AS MemberOrder, @UserRight AS UserRight, @MasterRight AS MasterRight, @MasterOrder AS MasterOrder, @MemberOrder AS MemberOrder, 
		@MemberPoint AS MemberPoint, @Score AS Score, @WinCount AS WinCount, @LostCount AS LostCount, @DrawCount AS DrawCount, @FleeCount AS FleeCount, @Gold AS Gold, @Ingot AS Ingot, @InsureScore AS InsureScore, 
		@Score AS FreeScore, @WinStreak AS WinStreak, @MaxWinStreak AS MaxWinStreak, @Experience AS Experience,@CustomID AS CustomID,@InoutIndex AS InoutIndex,@SignupCount AS SignupCount, @TicketCount AS TicketCount, 
		@ShutUpStatus AS ShutUpStatus, @ShutUpOverTime AS ShutUpOverTime

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- I D ��¼
CREATE PROC GSP_GR_EfficacyMobile
	@dwUserID INT,								-- �û� I D
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NCHAR(32),					-- ������ʶ	
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT,						-- ���� I D
	@dwRequestScore BIGINT,						-- �������
	@cbDeviceType SMALLINT,						-- �豸����
	@dwMatchID INT,								-- ���� I D 
	@lMatchNo BIGINT,							-- ��������	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

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
DECLARE @SignupCount INT

-- �û���Ϣ
DECLARE @Age TINYINT
DECLARE @Gender TINYINT
DECLARE @Area NVARCHAR(15)
DECLARE @City NVARCHAR(15)
DECLARE @Province NVARCHAR(15)

-- ���ֱ���
DECLARE @Score BIGINT
DECLARE @InsureScore BIGINT
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @WinStreak INT
DECLARE @MaxWinStreak INT
DECLARE @Experience INT
DECLARE @TicketCount INT

-- ��������
DECLARE @EnjoinLogon INT

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
	DECLARE @LogonPass NCHAR(32)	
	
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

	-- ��Ϸ��Ϣ
	DECLARE @MatchRight INT
	DECLARE @GameUserRight INT
	DECLARE @GameMasterRight INT
	DECLARE @GameMasterOrder SMALLINT
	declare @LastLogonDate datetime
	SELECT @GameUserRight=UserRight, @GameMasterRight=MasterRight, @GameMasterOrder=MasterOrder,@LastLogonDate=LastLogonDate
	FROM GameScoreInfo WHERE UserID=@dwUserID

	-- ��Ϣ�ж�
	IF @GameUserRight IS NULL
	BEGIN
		-- ��������
		INSERT INTO GameScoreInfo (UserID,LastLogonIP,LastLogonMachine)	VALUES (@dwUserID,@strClientIP,@strMachineID)

		-- ��Ϸ��Ϣ
		SELECT @GameUserRight=UserRight, @GameMasterRight=MasterRight, @GameMasterOrder=MasterOrder,@LastLogonDate=LastLogonDate
		FROM GameScoreInfo WHERE UserID=@dwUserID		
	END	

	-- ������Ϣ	
	SELECT @Score=Score, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @FleeCount=FleeCount, @WinStreak=WinStreak, @MaxWinStreak=MaxWinStreak, @MatchRight=MatchRight, @SignupCount=SignupCount
	FROM MatchScoreInfo WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
			
	-- ��Ϣ�ж�
	IF @Score IS NULL
	BEGIN
		-- ��Ϸ��Ϣ
		SELECT @Score=0,@WinCount=0,@LostCount=0,@DrawCount=0,@DrawCount=0,@FleeCount=0,@MatchRight=0,@WinStreak=0,@SignupCount=0,@MaxWinStreak=0		
	END
	ELSE
	BEGIN
		-- ����ʱ��
		UPDATE GameScoreInfo SET LastLogonDate=GETDATE() WHERE UserID=@dwUserID
	END	

	-- ��ѯ����ȯ
	SELECT @TicketCount=BuyTicketCount-UseTicketCount FROM StreamMatchTicket(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo

	-- ��������
	IF @TicketCount IS NULL SET @TicketCount=0

	-- ����Ȩ��	
	IF @GameUserRight IS NULL SET @GameUserRight=0
	IF @GameMasterRight IS NULL SET @GameMasterRight=0

	-- �û�Ȩ��	
	SET @UserRight=@UserRight|@MatchRight|@GameUserRight

	-- ����Ȩ��	
	SET @MasterRight=@MasterRight|@GameMasterRight

	-- Ȩ�޵ȼ�
	IF @MasterOrder<>0 OR @GameMasterOrder<>0
	BEGIN
		IF @GameMasterOrder>@MasterOrder SET @MasterOrder=@GameMasterOrder
	END
	ELSE SET @MasterRight=0

	-- ��������
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- �����Ϣ
	DECLARE @Gold BIGINT
	SELECT @Gold=Score, @InsureScore=InsureScore FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID

	-- ��ʯ��Ϣ
	DECLARE @Ingot BIGINT
	SELECT @Ingot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID

	-- ��������
	IF @Gold IS NULl SET @Gold=0
	IF @Ingot IS NULL SET @Ingot=0
	IF @InsureScore IS NULL SET @InsureScore=0

	-- ����״̬
	UPDATE LHAccountsDB.dbo.AccountsOnlineInfo SET KindID=@wKindID,ServerID=@wServerID,ServerKind=0x04,ModifyTime=GETDATE() WHERE UserID=@dwUserID

	-- �����¼
	INSERT RecordUserInout(UserID,KindID,ServerID,LockerScore,EnterTime,EnterScore,EnterMachine,EnterClientIP)
	VALUES(@dwUserID,@wKindID,@wServerID,0,GetDate(),@Score,@strMachineID,@strClientIP)
	
	-- ��¼��ʶ
	DECLARE @InoutIndex BIGINT
	SET @InoutIndex=SCOPE_IDENTITY()

	-- ��¼ͳ��	
	UPDATE SystemStreamInfo SET LogonCount=LogonCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, LogonCount) VALUES (@DateID, @wKindID, @wServerID, 1)

	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @NickName AS Accounts, @UnderWrite AS UnderWrite, @FaceID AS FaceID, @CardBackID AS CardBackID , @Age AS Age, @Gender AS Gender, @Province AS Province, @City AS City,
		@Area AS Area, @FaceFrameID AS FaceFrameID, @ActorID AS ActorID, @UserRight AS UserRight, @MasterRight AS MasterRight, @MasterOrder AS MasterOrder, @MemberPoint AS MemberPoint, 
		@Score AS Score, @WinCount AS WinCount, @LostCount AS LostCount, @DrawCount AS DrawCount, @FleeCount AS FleeCount, @WinStreak AS WinStreak, @MaxWinStreak AS MaxWinStreak, @Gold AS Gold, 
		@Ingot AS Ingot, @InsureScore AS InsureScore, @Score AS FreeScore, @Experience AS Experience, @CustomID AS CustomID,@InoutIndex AS InoutIndex, @SignupCount AS SignupCount, 
		@TicketCount AS TicketCount, @ShutUpStatus AS ShutUpStatus, @ShutUpOverTime AS ShutUpOverTime

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
