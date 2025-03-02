
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_WealthTransfer]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_WealthTransfer]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ����ת��
CREATE PROC GSP_MB_WealthTransfer
	@dwUserID INT,								-- �û� I D
	@dwTargetGameID INT,						-- ��Ϸ I D
	@cbCodeKind TINYINT,						-- ��֤������
	@strAuthCode NCHAR(32),						-- ��֤��	
	@lTransferAmount INT,						-- ת�˽��
	@wReason INT,								-- ���ԭ��
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ��������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @UserID INT
	DECLARE @GameID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	DECLARE @NickName NVARCHAR(32)
	DECLARE @Email NVARCHAR(64)
	DECLARE @MobilePhone NVARCHAR(21)
	SELECT @UserID=a.UserID,@GameID=a.GameID,@NickName=a.NickName,@Email=a.Email,@MobilePhone=a.MobilePhone, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

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
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�"}'
		RETURN 3
	END	

	-- ��ѯ�û�
	DECLARE @TargetUserID INT
	DECLARE @TargetGameID INT
	DECLARE @TargetNickName NVARCHAR(32)	
	SELECT @TargetUserID=a.UserID, @TargetNickName=a.NickName, @TargetGameID=a.GameID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.GameID=@dwTargetGameID

	-- У���û�
	IF @TargetUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1036,"Msg":"��Ǹ,Ŀ���ʺŲ����ڡ�"}'
		RETURN 4
	END	

	-- У���û�
	IF @TargetUserID = @dwUserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1037,"Msg":"��Ǹ,ͬһ�ʺŲ�������н��ת�ˡ�"}'
		RETURN 5
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1038,"Msg":"��Ǹ��Ŀ���ʺ���ʱ���ڶ���״̬���޷����н��ת�ˡ�"}'
		RETURN 6
	END	

	-- �ʺŹر�
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1039,"Msg":"��Ǹ��Ŀ���ʺ�ʹ���˰�ȫ�رչ��ܣ��޷����н��ת�ˡ�"}'
		RETURN 7
	END	

	-- ��������
	DECLARE @MobileEmail NVARCHAR(64)

	-- ���ñ���
	IF @cbCodeKind=2 SET @MobileEmail=@Email
	IF @cbCodeKind=1 SET @MobileEmail=@MobilePhone	
	
	-- ��֤��Ϣ
	DECLARE @BindUserID INT
	DECLARE @AuthKind TINYINT
	DECLARE @AuthCodeMD5 NVARCHAR(32)
	DECLARE @AcquireMachine NVARCHAR(32)	
	DECLARE @LastAcquireDate DATETIME
	SELECT @BindUserID=UserID, @AuthKind=AuthKind, @AuthCodeMD5=AuthCodeMD5, @AcquireMachine=AcquireMachine, @LastAcquireDate=LastAcquireDate FROM RecordAuthCode(NOLOCK) WHERE CodeKind=@cbCodeKind AND MobileEmail=@MobileEmail

	-- ��֤�벻����
	IF @BindUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1027,"Msg":"��֤�����"}'
		RETURN 8
	END

	-- �û���ƥ��
	IF @BindUserID<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1027,"Msg":"��֤�����"}'
		RETURN 9
	END

	-- У������
	IF @AuthKind<>10
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1027,"Msg":"��֤�����"}'
		RETURN 10		
	END
	
	-- У����֤��
	IF @AuthCodeMD5<>@strAuthCode
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1027,"Msg":"��֤�����"}'
		RETURN 11				
	END

	-- У�������
	IF @AcquireMachine<>@strMachineID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1028,"Msg":"���Ļ����뷢���쳣��"}'
		RETURN 12
	END

	-- У��״̬
	IF EXISTS(SELECT 1 FROM AccountsOnlineInfo(NOLOCK) WHERE UserID=@dwUserID AND ServerID<>0)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1052,"Msg":"ϵͳ��⵽�����˺�������Ϸ�����У������˳������ٽ���ת�ˣ�"}'
		RETURN 13
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
		SET @strErrorDescribe=N'{"Idx":1029,"Msg":"��֤���ѹ��ڣ������»�ȡ��֤�룡"}'
		RETURN 14
	END
	
	-- ת������
	DECLARE @TransferEnabled TINYINT	
	DECLARE @ServiceFeeRate SMALLINT	
	DECLARE @ReservedAmount INT
	DECLARE @MinTransferAmount INT
	DECLARE @MaxTransferAmount INT

	-- ��ѯ����
	SELECT @TransferEnabled=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TransferOption' AND StatusName=N'TransferEnabled'
	SELECT @ServiceFeeRate=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TransferOption' AND StatusName=N'ServiceFeeRate'
	SELECT @ReservedAmount=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TransferOption' AND StatusName=N'ReservedAmount'
	SELECT @MinTransferAmount=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TransferOption' AND StatusName=N'MinTransferAmount'
	SELECT @MaxTransferAmount=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TransferOption' AND StatusName=N'MaxTransferAmount'
	
	-- ��������
	IF @ReservedAmount IS NULL SET @ReservedAmount=10000
	IF @TransferEnabled IS NULL SET @TransferEnabled=1
	IF @ServiceFeeRate IS NULL SET @ServiceFeeRate=30
	IF @MinTransferAmount IS NULL SET @MinTransferAmount=10000
	IF @MaxTransferAmount IS NULL SET @MaxTransferAmount=1000000000

	-- У�鿪��
	IF @TransferEnabled=0
	BEGIN
		-- ������Ϣ
		SET @strErrorDescribe=N'{"Idx":1035,"Msg":"��Ǹ��ת�˹����ѹرա�"}'	
		RETURN 15
	END

	-- У����
	IF @lTransferAmount<@MinTransferAmount
	BEGIN
		-- ������Ϣ
		SET @strErrorDescribe=N'{"Idx":1040,"Msg":"��Ǹ, ÿ��ת�˽���С��%s��","Ops":["'+CONVERT(NVARCHAR(16),@MinTransferAmount/1000)+'"]}'	
		RETURN 16
	END

	-- У����
	IF @lTransferAmount>@MaxTransferAmount
	BEGIN
		-- ������Ϣ
		SET @strErrorDescribe=N'{"Idx":1041,"Msg":"��Ǹ, ÿ��ת�˽��ܴ���%s��","Ops":["'+CONVERT(NVARCHAR(16),@MaxTransferAmount/1000)+'"]}'	
		RETURN 17
	END

	-- ��������
	DECLARE @MailID INT
	DECLARE @MailTitle NVARCHAR(128)
	DECLARE @MailContent NVARCHAR(512)

	-- �ʼ�����
	SET @MailTitle =N'Coin transfer notice'
	SET @MailContent=N'{"Ops":["'+@TargetNickName+'","'+@NickName+'","'+CONVERT(NVARCHAR(18),@GameID)+'","'+CONVERT(NVARCHAR(16),@lTransferAmount/1000)+'","'+ CONVERT(VARCHAR(100),GETDATE(),109)+'"]}'

	-- �����ʼ���Ϣ
	INSERT INTO AccountsMailInfo(MailKind,MailType,MailTitle,MailContent,SendTime,ExpireTime,CollectTime)
	VALUES (2,2,@MailTitle,@MailContent,dbo.LHF_GetTimestamp(),0,GetDate())

	-- �ʼ���ʶ
	SET @MailID=SCOPE_IDENTITY()	

	-- ���븽��
	INSERT INTO AccountsMailAttach(MailID,GoodsID,GoodsCount,GoodsIndate) VALUES (@MailID,2,@lTransferAmount,0)			

	-- �������
	DECLARE @ServiceFee INT
	DECLARE @DeductAmount INT

	-- �������
	SET @ServiceFee = @lTransferAmount*@ServiceFeeRate/1000
	SET @DeductAmount = -(@lTransferAmount+@ServiceFee)

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��ѯ���
	DECLARE @Gold BIGINT
	SELECT @Gold=Score FROM LHGameGoldDB.dbo.GameScoreInfo WITH(UPDLOCK) WHERE UserID=@dwUserID

	-- У�����
	IF @Gold<@ReservedAmount+@lTransferAmount+@ServiceFee
	BEGIN
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ������Ϣ
		SET @strErrorDescribe=N'{"Idx":1043,"Msg":"��Ǹ�������ϵĽ�����㣬ת��ʧ�ܡ�"}'	
		RETURN 18
	END	

	-- д��Ƹ�
	EXEC GSP_GP_WriteGameWealth @dwUserID,2,@wReason,@DeductAmount

	-- �����û�����
	INSERT INTO AccountsMailBox(UserID,MailID,MailState,ExpireTime,UpdateTime) VALUES (@TargetUserID,@MailID,1,0,GetDate())
	
	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- ת�˼�¼
	INSERT INTO LHGameRecordDB.dbo.RecordWealthTransfer(UserID,NickName,GameID,TargetUserID,TargetNickName,TargetGameID,ServiceFee,TransferAmount,CollectClientIP,CollectMachineID,CollectDateTime)
	VALUES (@UserID,@NickName,@GameID,@TargetUserID,@TargetNickName,@TargetGameID,@ServiceFee,@lTransferAmount,@strClientIP,@strMachineID,GETDATE())

	-- ������Ϣ
	DECLARE @ATAuthTimes SMALLINT
	DECLARE @ATLastAcquireDate DATETIME
	SELECT @ATAuthTimes=AuthTimes,@ATLastAcquireDate=LastAcquireDate FROM AccountsAuthInfo(NOLOCK) WHERE UserID=@UserID AND CodeKind=@cbCodeKind

	-- ��������
	IF @ATAuthTimes IS NULL
	BEGIN
		INSERT INTO AccountsAuthInfo(UserID,CodeKind,AuthTimes,AcquireTimes,LastAcquireDate,LastMobileEmail)	VALUES (@UserID,@cbCodeKind,1,0,GETDATE(),N'')		
	END ELSE
	BEGIN
		-- ʱ����
		DECLARE @DateDiff INT
		SET @DateDiff=DateDiff(day,@ATLastAcquireDate,GETDATE())

		-- ��������
		IF @DateDiff<>0 SET @ATAuthTimes=0

		-- ��������
		UPDATE AccountsAuthInfo SET AuthTimes=@ATAuthTimes+1,AcquireTimes=0,LastAcquireDate=GETDATE(),LastMobileEmail=N'' WHERE UserID=@UserID AND CodeKind=@cbCodeKind
	END

	-- ���´���
	UPDATE RecordAuthCode SET AuthCode=N'',AuthCodeMD5=N'',AuthTimes=AuthTimes+1,AcquireTimes=0 WHERE CodeKind=@cbCodeKind AND MobileEmail=@MobileEmail

	-- ת������
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)	

	-- ��¼��֤����
	UPDATE RecordAuthTimes SET AuthTimes=AuthTimes+1 WHERE DateID=@DateID AND UserID=@UserID AND CodeKind=@cbCodeKind AND AuthKind=@AuthKind
	IF @@ROWCOUNT=0
	BEGIN
		-- �����¼
		INSERT INTO RecordAuthTimes(DateID,UserID,CodeKind,AuthKind,AuthTimes) VALUES (@DateID,@UserID,@cbCodeKind,@AuthKind,1)
	END
		
	-- ת�˳ɹ�
	SET @strErrorDescribe=N'{"Idx":1051,"Msg":"ת�˳ɹ���"}'
	   
	-- �׳�����
	SELECT @MailID AS MailID, @TargetUserID AS TargetUserID
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

