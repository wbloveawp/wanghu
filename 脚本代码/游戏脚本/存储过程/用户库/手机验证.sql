
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_AcquireAuthCode]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_AcquireAuthCode]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_BindEMail]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_BindEMail]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_UnBindEMail]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_UnBindEMail]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_BindMobilePhone]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_BindMobilePhone]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_UnBindMobilePhone]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_UnBindMobilePhone]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_ResetLogonPasswd]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_ResetLogonPasswd]
GO

--IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_ReceiptAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
--DROP PROCEDURE [dbo].[GSP_MB_ReceiptAccounts]
--GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��ȡ��֤��
CREATE PROC GSP_MB_AcquireAuthCode
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strMachineID NVARCHAR(32),					-- ��������
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@cbCodeKind	 TINYINT,						-- ��֤������
	@cbAuthKind TINYINT,						-- ��֤����		
	@dwBindUserID INT,							-- �󶨱�ʶ	
	@strMobileEmail NVARCHAR(64),				-- �ֻ�����
	@strAuthCode NCHAR(6),						-- ��֤��
	@strAuthCodeMD5 NCHAR(32),					-- ��֤��	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ʱ����
DECLARE @DateDiff INT
DECLARE @SecondDiff INT

-- ��ʶ����
DECLARE @BindUserID INT
DECLARE @BindAccounts NVARCHAR(31)

-- ִ���߼�
BEGIN	
	
	-- У���ʶ
	IF @dwUserID<>0
	BEGIN
		-- ��ѯ�û�
		DECLARE @UserID INT
		DECLARE @Nullity BIT
		DECLARE @ShutDown BIT	
		DECLARE @RegisterMode INT
		DECLARE @LogonPass NCHAR(32)
		DECLARE @EMail NVARCHAR(64)
		DECLARE @MobilePhone NVARCHAR(21)				
		SELECT @UserID=a.UserID, @LogonPass=a.LogonPass, @RegisterMode=a.RegisterMode, @Nullity=a.Nullity, @EMail=a.EMail, @MobilePhone=a.MobilePhone, @ShutDown=b.ShutDownStatus
		FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

		-- ��ѯ�û�
		IF @UserID IS NULL
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1021,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
			RETURN 10
		END	

		-- �ʺŽ�ֹ
		IF @Nullity<>0
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1022,"Msg":"�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����"}'
			RETURN 11
		END	

		-- �ʺŹر�
		IF @ShutDown<>0
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1023,"Msg":"�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�"}'
			RETURN 12
		END	
	
		-- �����ж�	
		IF @LogonPass<>@strPassword
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1021,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
			RETURN 13
		END	

		-- ��������
		DECLARE @strSubString NVARCHAR(31)

		-- ���ֻ���
		IF @cbAuthKind=1
		BEGIN
			-- У���ֻ���
			--IF @MobilePhone<>N''
			--BEGIN
			--	-- ������ʾ
			--	SET @strErrorDescribe=N'{"Idx":1055,"Msg":"�����˺��Ѱ�β��Ϊ��'+SUBSTRING(@MobilePhone,LEN(@MobilePhone)-3,4)+'���ֻ��ţ���������ֻ���,���Ƚ���󶨣�","Ops":["'+SUBSTRING(@MobilePhone,LEN(@MobilePhone)-3,4)+'"]}'
			--	RETURN 14
			--END

			-- ��ѯ�˺�			
			SELECT @BindUserID=UserID,@BindAccounts=NickName FROM AccountsInfo(NOLOCK) WHERE MobilePhone=@strMobileEmail
			IF @BindUserID IS NOT NULL AND @BindUserID<>@dwBindUserID
			--IF @BindUserID IS NOT NULL
			BEGIN
				SET @strErrorDescribe=N'{"Idx":1067,"Msg":"��Ǹ�����ֻ����Ѿ���������Ϸ�˺Ű󶨣�������µ��ֻ��Ű󶨣�"}'
				RETURN 14		

				-- �˺ű���
				--IF LEN(@BindAccounts)>3
				--BEGIN
				--	SET @strSubString=SUBSTRING(@BindAccounts,2,LEN(@BindAccounts)-3)

				--	 -- �滻�ַ�
				--	SET @BindAccounts=REPLACE(@BindAccounts,@strSubString,REPLICATE(N'*',LEN(@strSubString)))			
				--END

				---- ������ʾ�ַ�
				--SET @strErrorDescribe=N'"Idx":1057,"Msg":"'+@strMobileEmail+N'�Ѿ�����Ϸ�˺ţ�'+@BindAccounts+'�󶨣��Ƿ����½��ֻ��󶨵���ǰ�ʺ��ϡ�","Ops":["'+@strMobileEmail+'","'+@BindAccounts+'"]}'

				---- �׳���¼
				--SELECT @BindUserID AS BindUserID

				--RETURN 2											
			END
		END

		-- ����ֻ���
		IF @cbAuthKind=2 AND @MobilePhone<>@strMobileEmail
		BEGIN
			-- ������ʾ
			SET @strErrorDescribe=N'{"Idx":1056,"Msg":"�������ʹ��ԭ�ֻ����룬��������ֻ����벻ƥ�䣬����ϸ�˶Ժ��ٴγ��ԣ�"}'
			RETURN 15
		END	

		-- ������
		IF @cbAuthKind=3
		BEGIN
			IF @Email<>N''
			BEGIN
				-- ������ʾ
				SET @strErrorDescribe=N'{"Idx":1045,"Msg":"�����˺��Ѱ����䣺'+@Email+'������������Ƚ���󶨣�","Ops":["'+@Email+'"]}'
				RETURN 14
			END

			-- ��ѯ����
			SELECT @BindUserID=UserID,@BindAccounts=NickName FROM AccountsInfo(NOLOCK) WHERE EMail=@strMobileEmail
			IF @BindUserID IS NOT NULL AND @BindUserID<>@dwBindUserID
			BEGIN
				-- �˺ű���
				IF LEN(@BindAccounts)>3
				BEGIN					
					SET @strSubString=SUBSTRING(@BindAccounts,2,LEN(@BindAccounts)-3)

					 -- �滻�ַ�
					SET @BindAccounts=REPLACE(@BindAccounts,@strSubString,REPLICATE(N'*',LEN(@strSubString)))			
				END

				-- ������ʾ�ַ�
				SET @strErrorDescribe=N'{"Idx":1048,"Msg":"'+@strMobileEmail+N'�Ѿ�����Ϸ�˺ţ�'+@BindAccounts+'�󶨣��Ƿ����½�����󶨵���ǰ�ʺ��ϡ�","Ops":["'+@strMobileEmail+'","'+@BindAccounts+'"]}'

				-- �׳���¼
				SELECT @BindUserID AS BindUserID

				RETURN 2											
			END
		END

		-- �������
		IF @cbAuthKind=4 AND @Email<>@strMobileEmail
		BEGIN
			-- ������ʾ
			SET @strErrorDescribe=N'{"Idx":1046,"Msg":"�������ʹ��ԭ�����䣬����������䲻ƥ�䣬����ϸ�˶Ժ��ٴγ��ԣ�"}'
			RETURN 15
		END	

		-- ����ת��
		IF @cbAuthKind=10
		BEGIN
			-- У���ֻ���
			IF @cbCodeKind=1 AND @MobilePhone<>@strMobileEmail
			BEGIN
				-- ������ʾ
				SET @strErrorDescribe=N'{"Idx":1053,"Msg":"ת����ʹ����Ϸ�˺Ű󶨵��ֻ��ţ���������ֻ����벻ƥ�䣬����ϸ�˶Ժ��ٴγ��ԣ�"}'
				RETURN 16
			END

			-- У������
			IF @cbCodeKind=2 AND @Email<>@strMobileEmail
			BEGIN
				-- ������ʾ
				SET @strErrorDescribe=N'{"Idx":1047,"Msg":"ת����ʹ����Ϸ�˺Ű󶨵����䣬�������䲻ƥ�䣬����ϸ�˶Ժ��ٴγ��ԣ�"}'
				RETURN 16
			END
		END	

		-- �������˺�
		IF @cbAuthKind=11
		BEGIN
			-- У���ֻ���
			IF @cbCodeKind=1 AND @MobilePhone<>@strMobileEmail
			BEGIN
				-- ������ʾ
				SET @strErrorDescribe=N'{"Idx":1083,"Msg":"�������˺���ʹ����Ϸ�˺Ű󶨵��ֻ��ţ���������ֻ����벻ƥ�䣬����ϸ�˶Ժ��ٴγ��ԣ�"}'
				RETURN 17
			END

			-- У������
			IF @cbCodeKind=2 AND @Email<>@strMobileEmail
			BEGIN
				-- ������ʾ
				SET @strErrorDescribe=N'{"Idx":1084,"Msg":"�������˺���ʹ����Ϸ�˺Ű󶨵����䣬�������䲻ƥ�䣬����ϸ�˶Ժ��ٴγ��ԣ�"}'
				RETURN 17
			END
		END
		
	END ELSE
	BEGIN
		-- ��������
		DECLARE @DateID INT
		DECLARE @AuthTimes SMALLINT		

		-- ��������			
		SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

		-- ��������
		IF @cbAuthKind=5
		BEGIN		
			-- �ֻ���֤��
			IF @cbCodeKind=1
			BEGIN
				-- ��ѯ�˺�
				SELECT @BindUserID=UserID FROM AccountsInfo(NOLOCK) WHERE MobilePhone=@strMobileEmail
				IF @BindUserID IS NULL
				BEGIN
					-- ������ʾ
					SET @strErrorDescribe=N'{"Idx":1058,"Msg":"���ֻ���û�а󶨵���Ϸ�˺ţ�����ϸ�˶��ֻ����ٴγ��ԣ�"}'

					RETURN 16
				END	
			END

			-- ������֤��
			IF @cbCodeKind=2
			BEGIN
				-- ��ѯ�˺�
				SELECT @BindUserID=UserID FROM AccountsInfo(NOLOCK) WHERE EMail=@strMobileEmail
				IF @BindUserID IS NULL
				BEGIN
					-- ������ʾ
					SET @strErrorDescribe=N'{"Idx":1044,"Msg":"������û�а󶨵���Ϸ�˺ţ�����ϸ�˶�������ٴγ��ԣ�"}'

					RETURN 16
				END	
			END

			-- ��ȡ��¼
			SELECT @AuthTimes=AuthTimes FROM RecordAuthTimes(NOLOCK) WHERE DateID=@DateID AND UserID=@BindUserID AND CodeKind=@cbCodeKind AND AuthKind=@cbAuthKind

			-- ��������
			IF @AuthTimes IS NULL SET @AuthTimes=0

			-- ��ȡ����			
			DECLARE @ResetLogonPassTimes SMALLINT			
			SELECT @ResetLogonPassTimes=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ResetLogonPassTimes'

			-- ��������
			IF @ResetLogonPassTimes IS NULL SET @ResetLogonPassTimes=3		

			-- У�����
			IF @AuthTimes>=@ResetLogonPassTimes
			BEGIN
				-- ������ʾ
				SET @strErrorDescribe=N'{"Idx":1059,"Msg":"������Ϸ�˺Ž�����������Ĵ����Ѵﵽ'+CONVERT(NVARCHAR(3),@ResetLogonPassTimes)+'�Σ�Ϊ�������ʺŰ�ȫ����������֤�����������ԣ�","Ops":["'+CONVERT(NVARCHAR(3),@ResetLogonPassTimes)+'"]}'

				RETURN 17
			END
		END	
		
		-- ��֤���½
		IF @cbAuthKind=6
		BEGIN
			-- ��ȡ��¼
			SELECT @AuthTimes=AuthTimes FROM RecordAuthTimes(NOLOCK) WHERE DateID=@DateID AND UserID=@BindUserID AND CodeKind=@cbCodeKind AND  AuthKind=@cbAuthKind

			-- ��������
			IF @AuthTimes IS NULL SET @AuthTimes=0

			-- ��ȡ����			
			DECLARE @AuthCodeLogonTimes SMALLINT			
			SELECT @AuthCodeLogonTimes=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND  StatusName=N'AuthCodeLogonTimes'

			-- ��������
			IF @AuthCodeLogonTimes IS NULL SET @AuthCodeLogonTimes=10

			-- У�����
			IF @AuthTimes>=@AuthCodeLogonTimes
			BEGIN
				-- ������ʾ
				IF @cbCodeKind=1 SET @strErrorDescribe=N'{"Idx":1061,"Msg":"�����ֻ��Ž���ʹ����֤���¼�Ĵ����Ѵﵽ'+CONVERT(NVARCHAR(3),@AuthCodeLogonTimes)+'�Σ�Ϊ�������ʺŰ�ȫ����������֤�����������ԣ�","Ops":["'+CONVERT(NVARCHAR(3),@AuthCodeLogonTimes)+'"]}'
				IF @cbCodeKind=2 SET @strErrorDescribe=N'{"Idx":1082,"Msg":"�����������ʹ����֤���¼�Ĵ����Ѵﵽ'+CONVERT(NVARCHAR(3),@AuthCodeLogonTimes)+'�Σ�Ϊ�������ʺŰ�ȫ����������֤�����������ԣ�","Ops":["'+CONVERT(NVARCHAR(3),@AuthCodeLogonTimes)+'"]}'

				RETURN 17
			END
		END		
	END

	-- ��������
	IF @BindUserID IS NULL SET @BindUserID=0

	-- ϵͳ����
	DECLARE @ACValidSeconds INT
	DECLARE @MaxMPAuthTimes SMALLINT 
	DECLARE @MaxMPAcquireTimes SMALLINT
	DECLARE @MaxATAuthTimes SMALLINT 
	DECLARE @MaxATAcquireTimes SMALLINT

	-- ��ȡ����
	SELECT @ACValidSeconds=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ACValidSecond'
	SELECT @MaxMPAuthTimes=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND  StatusName=N'MaxMPAuthTimes'
	SELECT @MaxMPAcquireTimes=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'MaxMPAcquireTimes'
	SELECT @MaxATAuthTimes=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND  StatusName=N'MaxATAuthTimes'
	SELECT @MaxATAcquireTimes=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'MaxATAcquireTimes'

	-- ��������
	IF @ACValidSeconds IS NULL SET @ACValidSeconds=600
	IF @MaxMPAuthTimes IS NULL SET @MaxMPAuthTimes=10
	IF @MaxMPAcquireTimes IS NULL SET @MaxMPAuthTimes=5
	IF @MaxATAuthTimes IS NULL SET @MaxMPAuthTimes=10
	IF @MaxATAcquireTimes IS NULL SET @MaxMPAuthTimes=5

	-- �˺���֤��¼
	DECLARE @ATAuthDate DATETIME
	DECLARE @ATAuthTimes SMALLINT		
	DECLARE @ATAcquireTimes SMALLINT		

	-- �ֻ�����֤��¼
	DECLARE @MPAuthDate DATETIME
	DECLARE @MPAuthKind SMALLINT	
	DECLARE @MPAuthTimes SMALLINT	
	DECLARE @MPAuthCode NVARCHAR(32)				
	DECLARE @MPAcquireTimes SMALLINT	

	-- ��ѯ��Ϣ
	SELECT @ATAuthTimes=AuthTimes,@ATAcquireTimes=AcquireTimes,@ATAuthDate=LastAcquireDate FROM AccountsAuthInfo(NOLOCK) WHERE UserID=@dwUserID	AND CodeKind=@cbCodeKind
	
	-- ��¼������
	IF @ATAuthTimes IS NULL
	BEGIN
		-- ��֤���¼				
		SELECT @MPAuthTimes=AuthTimes,@MPAuthKind=AuthKind,@MPAuthCode=AuthCode,@MPAcquireTimes=AcquireTimes,@MPAuthDate=LastAcquireDate FROM RecordAuthCode(NOLOCK) WHERE CodeKind=@cbCodeKind AND MobileEmail=@strMobileEmail

		-- ��¼����
		IF @MPAuthTimes IS NULL
		BEGIN
			-- �����¼
			INSERT INTO RecordAuthCode(CodeKind,MobileEmail,UserID,AuthKind,AuthCode,AuthCodeMD5,AuthTimes,AcquireTimes,AcquireClientIP,AcquireMachine,LastAcquireDate)										
			VALUES (@cbCodeKind,@strMobileEmail,@dwUserID,@cbAuthKind,@strAuthCode,@strAuthCodeMD5,0,1,@strClientIP,@strMachineID,GetDate())

		END ELSE
		BEGIN
			-- ������
			SET @DateDiff=DateDiff(day,@MPAuthDate,GetDate())
			SET @SecondDiff=DateDiff(s,@MPAuthDate,GetDate())			

			-- ��������	
			IF @DateDiff=0 AND @MPAuthTimes>=@MaxMPAuthTimes
			BEGIN		
				IF @cbCodeKind=1 SET @strErrorDescribe=N'{"Idx":1062,"Msg":"���ֻ��Ž������֤�����ѳ���'+CONVERT(NVARCHAR(3),@MaxMPAuthTimes)+'�Σ�Ϊ�������ʺŰ�ȫ����������֤�����������ԣ�","Ops":["'+CONVERT(NVARCHAR(3),@MaxMPAuthTimes)+'"]}'
				IF @cbCodeKind=2 SET @strErrorDescribe=N'{"Idx":1077,"Msg":"������������֤�����ѳ���'+CONVERT(NVARCHAR(3),@MaxMPAuthTimes)+'�Σ�Ϊ�������ʺŰ�ȫ����������֤�����������ԣ�","Ops":["'+CONVERT(NVARCHAR(3),@MaxMPAuthTimes)+'"]}'
				RETURN 20						
			END

			-- ��������	
			IF @DateDiff=0 AND @MPAcquireTimes>=@MaxMPAcquireTimes
			BEGIN	
				IF @cbCodeKind=1 SET @strErrorDescribe=N'{"Idx":1063,"Msg":"���ֻ��Ž������֤��������ѳ���'+CONVERT(NVARCHAR(3),@MaxMPAcquireTimes)+'�Σ�Ϊ�������ʺŰ�ȫ����������֤��������ֻ�������������ԣ�","Ops":["'+CONVERT(NVARCHAR(3),@MaxMPAcquireTimes)+'"]}'
				IF @cbCodeKind=2 SET @strErrorDescribe=N'{"Idx":1078,"Msg":"������������֤��������ѳ���'+CONVERT(NVARCHAR(3),@MaxMPAcquireTimes)+'�Σ�Ϊ�������ʺŰ�ȫ����������֤�������������������ԣ�","Ops":["'+CONVERT(NVARCHAR(3),@MaxMPAcquireTimes)+'"]}'
				RETURN 21						
			END

			-- ʱ������
			IF @MPAuthCode<>N'' AND @SecondDiff<@ACValidSeconds	
			BEGIN			
				-- ��������
				IF @MPAuthKind<>@cbAuthKind
				BEGIN
					UPDATE RecordAuthCode SET AuthKind=@cbAuthKind WHERE CodeKind=@cbCodeKind AND MobileEmail=@strMobileEmail
				END

				-- ��ʾ��Ϣ
				IF @cbCodeKind=1 SET @strErrorDescribe=N'{"Idx":1064,"Msg":"�ֻ���֤���ѳɹ�������β��Ϊ��'+SUBSTRING(@strMobileEmail,LEN(@strMobileEmail)-3,4)+'���ֻ������������֪ͨ��","Ops":["'+SUBSTRING(@strMobileEmail,LEN(@strMobileEmail)-3,4)+'"]}'
				IF @cbCodeKind=2 SET @strErrorDescribe=N'{"Idx":1079,"Msg":"��֤���ѳɹ����������䣺'+@strMobileEmail+'���������ʼ�֪ͨ��","Ops":["'+@strMobileEmail+'"]}'

				-- �׳���ʶ
				SELECT @BindUserID AS BindUserID

				RETURN 3
			END			

			-- ��������
			IF @DateDiff<>0 SELECT @MPAuthTimes=0,@MPAcquireTimes=0

			-- ���¼�¼
			UPDATE RecordAuthCode SET UserID=@dwUserID,AuthKind=@cbAuthKind,AuthCode=@strAuthCode,AuthCodeMD5=@strAuthCodeMD5,AuthTimes=@MPAuthTimes,AcquireTimes=@MPAcquireTimes+1,
			AcquireClientIP=@strClientIP,AcquireMachine=@strMachineID,LastAcquireDate=GetDate() WHERE CodeKind=@cbCodeKind AND MobileEmail=@strMobileEmail
		END			
				
		-- �����¼
		IF @dwUserID<>0
		BEGIN
			INSERT INTO AccountsAuthInfo(UserID,CodeKind,AuthTimes,AcquireTimes,LastAcquireDate,LastMobileEmail) VALUES (@dwUserID,@cbCodeKind,0,1,GetDate(),@strMobileEmail)
		END

	END ELSE
	BEGIN	
		-- ������
		SET @DateDiff=DateDiff(day,@ATAuthDate,GetDate())		

		-- ��������	
		IF @DateDiff=0 AND @ATAuthTimes>=@MaxATAuthTimes
		BEGIN		
			IF @cbCodeKind=1 SET @strErrorDescribe=N'{"Idx":1065,"Msg":"����Ϸ�˻�������ֻ���֤�����ѳ���'+CONVERT(NVARCHAR(3),@MaxATAuthTimes)+'�Σ�Ϊ�������ʺŰ�ȫ����������֤�����������ԣ�","Ops":["'+CONVERT(NVARCHAR(3),@MaxATAuthTimes)+'"]}'
			IF @cbCodeKind=2 SET @strErrorDescribe=N'{"Idx":1080,"Msg":"����Ϸ�˻������������֤�����ѳ���'+CONVERT(NVARCHAR(3),@MaxATAuthTimes)+'�Σ�Ϊ�������ʺŰ�ȫ����������֤�����������ԣ�","Ops":["'+CONVERT(NVARCHAR(3),@MaxATAuthTimes)+'"]}'
			RETURN 23						
		END	
		
		-- ��������	
		IF @DateDiff=0 AND @ATAcquireTimes>=@MaxATAcquireTimes
		BEGIN		
			IF @cbCodeKind=1 SET @strErrorDescribe=N'{"Idx":1066,"Msg":"����Ϸ�˺Ž�����ֻ���֤��������ѳ���'+CONVERT(NVARCHAR(3),@MaxATAcquireTimes)+'�Σ�Ϊ�������ʺŰ�ȫ����������֤�����������ԣ�","Ops":["'+CONVERT(NVARCHAR(3),@MaxATAcquireTimes)+'"]}'
			IF @cbCodeKind=2 SET @strErrorDescribe=N'{"Idx":1081,"Msg":"����Ϸ�˺Ž����������֤��������ѳ���'+CONVERT(NVARCHAR(3),@MaxATAcquireTimes)+'�Σ�Ϊ�������ʺŰ�ȫ����������֤�����������ԣ�","Ops":["'+CONVERT(NVARCHAR(3),@MaxATAcquireTimes)+'"]}'
			RETURN 24						
		END	

		-- ��֤���¼
		SELECT @MPAuthTimes=AuthTimes,@MPAuthCode=AuthCode,@MPAcquireTimes=AcquireTimes,@MPAuthDate=LastAcquireDate FROM RecordAuthCode(NOLOCK) WHERE CodeKind=@cbCodeKind AND MobileEmail=@strMobileEmail

		-- ��¼����
		IF @MPAuthTimes IS NULL
		BEGIN
			-- �����¼
			INSERT INTO RecordAuthCode(CodeKind, MobileEmail,UserID,AuthKind,AuthCode,AuthCodeMD5,AuthTimes,AcquireTimes,AcquireClientIP,AcquireMachine,LastAcquireDate)										
			VALUES (@cbCodeKind,@strMobileEmail,@UserID,@cbAuthKind,@strAuthCode,@strAuthCodeMD5,0,1,@strClientIP,@strMachineID,GetDate())

		END ELSE
		BEGIN
			-- ������
			SET @DateDiff=DateDiff(day,@MPAuthDate,GetDate())
			SET @SecondDiff=DateDiff(s,@MPAuthDate,GetDate())			

			-- ��������	
			IF @DateDiff=0 AND @MPAuthTimes>=@MaxMPAuthTimes
			BEGIN		
				IF @cbCodeKind=1 SET @strErrorDescribe=N'{"Idx":1062,"Msg":"���ֻ��Ž������֤�����ѳ���'+CONVERT(NVARCHAR(3),@MaxMPAuthTimes)+'�Σ�Ϊ�������ʺŰ�ȫ����������֤�����������ԣ�","Ops":["'+CONVERT(NVARCHAR(3),@MaxMPAuthTimes)+'"]}'
				IF @cbCodeKind=2 SET @strErrorDescribe=N'{"Idx":1077,"Msg":"������������֤�����ѳ���'+CONVERT(NVARCHAR(3),@MaxMPAuthTimes)+'�Σ�Ϊ�������ʺŰ�ȫ����������֤�����������ԣ�","Ops":["'+CONVERT(NVARCHAR(3),@MaxMPAuthTimes)+'"]}'
				RETURN 25						
			END

			-- ��������	
			IF @DateDiff=0 AND @MPAcquireTimes>=@MaxMPAcquireTimes
			BEGIN		
				IF @cbCodeKind=1 SET @strErrorDescribe=N'{"Idx":1063,"Msg":"���ֻ��Ž������֤��������ѳ���'+CONVERT(NVARCHAR(3),@MaxMPAcquireTimes)+'�Σ�Ϊ�������ʺŰ�ȫ����������֤��������ֻ�������������ԣ�","Ops":["'+CONVERT(NVARCHAR(3),@MaxMPAcquireTimes)+'"]}'
				IF @cbCodeKind=2 SET @strErrorDescribe=N'{"Idx":1078,"Msg":"������������֤��������ѳ���'+CONVERT(NVARCHAR(3),@MaxMPAcquireTimes)+'�Σ�Ϊ�������ʺŰ�ȫ����������֤�������������������ԣ�","Ops":["'+CONVERT(NVARCHAR(3),@MaxMPAcquireTimes)+'"]}'
				RETURN 26						
			END

			-- ʱ������			
			IF @MPAuthCode<>N'' AND @SecondDiff<@ACValidSeconds	
			BEGIN	
				-- ��������
				IF @MPAuthKind<>@cbAuthKind
				BEGIN
					UPDATE RecordAuthCode SET AuthKind=@cbAuthKind WHERE CodeKind=@cbCodeKind AND MobileEmail=@strMobileEmail
				END			
				
				-- ��ʾ��Ϣ
				IF @cbCodeKind=1 SET @strErrorDescribe=N'{"Idx":1064,"Msg":"�ֻ���֤���ѳɹ�������β��Ϊ��'+SUBSTRING(@strMobileEmail,LEN(@strMobileEmail)-3,4)+'���ֻ������������֪ͨ��","Ops":["'+SUBSTRING(@strMobileEmail,LEN(@strMobileEmail)-3,4)+'"]}'
				IF @cbCodeKind=2 SET @strErrorDescribe=N'{"Idx":1079,"Msg":"��֤���ѳɹ����������䣺'+@strMobileEmail+'���������ʼ�֪ͨ��","Ops":["'+@strMobileEmail+'"]}'

				-- �׳���ʶ
				SELECT @BindUserID AS BindUserID

				RETURN 3
			END		

			-- ��������
			IF @DateDiff<>0 SELECT @MPAuthTimes=0,@MPAcquireTimes=0
			
			-- ���¼�¼
			UPDATE RecordAuthCode SET UserID=@dwUserID,AuthKind=@cbAuthKind,AuthCode=@strAuthCode,AuthCodeMD5=@strAuthCodeMD5,AuthTimes=@MPAuthTimes,AcquireTimes=@MPAcquireTimes+1,
			AcquireClientIP=@strClientIP,AcquireMachine=@strMachineID,LastAcquireDate=GetDate() WHERE CodeKind=@cbCodeKind AND MobileEmail=@strMobileEmail
		END	
		
		-- ��������
		SET @DateDiff=DateDiff(day,@ATAuthDate,GetDate())

		-- ��������
		IF @DateDiff<>0 SELECT @ATAuthTimes=0,@ATAcquireTimes=0				

		-- ���¼�¼
		UPDATE AccountsAuthInfo SET AcquireTimes=@ATAcquireTimes+1,LastAcquireDate=GetDate(),LastMobileEmail=@strMobileEmail WHERE UserID=@dwUserID AND CodeKind=@cbCodeKind
	END	

	-- ��ʾ��Ϣ
	IF @cbCodeKind=1 SET @strErrorDescribe=N'{"Idx":1064,"Msg":"�ֻ���֤���ѳɹ�������β��Ϊ��'+SUBSTRING(@strMobileEmail,LEN(@strMobileEmail)-3,4)+'���ֻ������������֪ͨ��","Ops":["'+SUBSTRING(@strMobileEmail,LEN(@strMobileEmail)-3,4)+'"]}'
	IF @cbCodeKind=2 SET @strErrorDescribe=N'{"Idx":1079,"Msg":"��֤���ѳɹ����������䣺'+@strMobileEmail+'���������ʼ�֪ͨ��","Ops":["'+@strMobileEmail+'"]}'

	-- �׳���ʶ
	SELECT @BindUserID AS BindUserID
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- ������
CREATE PROC GSP_MB_BindEMail
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����	
	@strNewPassword NCHAR(32),					-- �û�����
	@strMachineID NVARCHAR(32),					-- ��������
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strEmail NVARCHAR(64),						-- ��    ��
	@strAuthCode NCHAR(32),						-- �� ֤ ��
	@dwUnBindUserID INT,						-- ����ʶ	
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
	DECLARE @LogonPass NCHAR(32)	
	SELECT @UserID=a.UserID,@LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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
	
	-- �����ж�	
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
		RETURN 4
	END	

	-- ��ѯ�û�
	DECLARE @BindUserID INT
	SELECT @BindUserID=UserID FROM AccountsInfo(NOLOCK) WHERE EMail=@strEmail

	-- У���ʶ
	IF @BindUserID IS NOT NULL AND @BindUserID<>@dwUnBindUserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1085,"Msg":"��Ǹ���������Ѿ���������Ϸ�˺Ű󶨣������ʧ�ܣ�"}'
		RETURN 5		
	END

	-- ��ѯ����
	DECLARE @ATLastAcquireDate DATETIME
	DECLARE @LastEmail NVARCHAR(32)		
	SELECT @LastEmail=LastMobileEmail,@ATLastAcquireDate=LastAcquireDate FROM AccountsAuthInfo(NOLOCK) WHERE UserID=@dwUserID AND CodeKind=2
	
	-- У���¼
	IF @LastEmail IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1086,"Msg":"��֤����������ʧ�ܣ�"}'
		RETURN 51
	END

	-- У������
	IF @LastEmail<>@strEmail
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1086,"Msg":"��֤����������ʧ�ܣ�"}'
		RETURN 52
	END

	-- ��֤��Ϣ
	DECLARE @AuthKind TINYINT	
	DECLARE @AuthCodeMD5 NVARCHAR(32)
	DECLARE @AcquireMachine NVARCHAR(32)
	DECLARE @MBLastAcquireDate DATETIME
	SELECT @BindUserID=UserID,@AuthKind=AuthKind, @AuthCodeMD5=AuthCodeMD5, @AcquireMachine=AcquireMachine, @MBLastAcquireDate=LastAcquireDate FROM RecordAuthCode(NOLOCK) WHERE CodeKind=2 AND MobileEmail=@strEmail

	-- ��֤�벻����
	IF @BindUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1086,"Msg":"��֤����������ʧ�ܣ�"}'
		RETURN 61
	END

	-- �û���ƥ��
	IF @BindUserID<>@UserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1086,"Msg":"��֤����������ʧ�ܣ�"}'
		RETURN 62
	END

	-- У������
	IF @AuthKind<>3
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1086,"Msg":"��֤����������ʧ�ܣ�"}'
		RETURN 63		
	END

	-- У����֤��
	IF @AuthCodeMD5<>@strAuthCode
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1086,"Msg":"��֤����������ʧ�ܣ�"}'
		RETURN 7				
	END

	-- У�������
	IF @AcquireMachine<>@strMachineID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1069,"Msg":"���Ļ����뷢���쳣������ʧ�ܣ�"}'
		RETURN 8
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

	-- ������ʶ
	DECLARE @ExternalID INT	
	SELECT @ExternalID=ExternalID FROM AccountsExternal(NOLOCK) WHERE UserID=@dwUserID

	-- ��������	
	IF @ExternalID IS NULL SET @ExternalID=0

	-- �û��Ƹ�
	DECLARE @UserCard INT	

	-- �󶨼�¼
	IF NOT EXISTS(SELECT 1 FROM RecordBindEMail(NOLOCK) WHERE UserID=@dwUserID)
	BEGIN		
		-- �����¼
		INSERT INTO RecordBindEMail(UserID,EMail,CollectDate) VALUES (@dwUserID,@strEmail,GetDate())
	END	
	
	-- ������Ϣ
	UPDATE AccountsAuthInfo SET AuthTimes=AuthTimes+1,AcquireTimes=0, LastAcquireDate=GetDate(),LastMobileEmail=N'' WHERE UserID=@dwUserID AND CodeKind=2

	-- ���¼�¼
	UPDATE RecordAuthCode SET AuthCode=N'',AuthCodeMD5=N'', AuthTimes=AuthTimes+1,AcquireTimes=0 WHERE CodeKind=2 AND MobileEmail=@strEmail			

	-- ��������
	IF @strNewPassword<>N'' SET @LogonPass=@strNewPassword

	-- �������
	IF @dwUnBindUserID<>0 UPDATE AccountsInfo SET EMail=N'' WHERE UserID=@dwUnBindUserID

	-- ������Ϣ
	UPDATE AccountsInfo SET EMail=@strEmail,LogonPass=@LogonPass WHERE UserID=@dwUserID	

	-- ��������
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��¼��֤����
	UPDATE RecordAuthTimes SET AuthTimes=AuthTimes+1 WHERE DateID=@DateID AND UserID=@dwUserID AND CodeKind=2 AND AuthKind=@AuthKind
	IF @@ROWCOUNT=0
	BEGIN
		-- �����¼
		INSERT INTO RecordAuthTimes(DateID,UserID,CodeKind,AuthKind,AuthTimes) VALUES (@DateID,@dwUserID,2,@AuthKind,1)
	END
	
	-- ��ѯ�Ƹ�
	SELECT @UserCard =Score FROM LHGameBattleDB.dbo.GameCardInfo(NOLOCK) WHERE UserID=@dwUserID	

	-- ��������
	IF @UserCard IS NULL SET @UserCard=0

	-- �󶨳ɹ�
	SET @strErrorDescribe=N'{"Idx":1070,"Msg":"����󶨳ɹ���"}'	

	-- �׳��Ƹ�
	SELECT @UserCard AS UserCard
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �������
CREATE PROC GSP_MB_UnBindEMail
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strMachineID NVARCHAR(32),					-- ��������
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strEmail NVARCHAR(32),						-- ��	 ��
	@strAuthCode NCHAR(32),						-- �� ֤ ��	
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
	DECLARE @LogonPass NCHAR(32)
	DECLARE @BindEMail NVARCHAR(32)
	SELECT @UserID=a.UserID, @LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus, @BindEMail=a.EMail 
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
	
	-- �����ж�	
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
		RETURN 4
	END	

	-- ��ѯ����
	DECLARE @LastEmail NVARCHAR(32)	
	SELECT @LastEmail=LastMobileEmail FROM AccountsAuthInfo(NOLOCK) WHERE UserID=@dwUserID AND CodeKind=2
	
	-- У���¼
	IF @LastEmail IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"��֤�����[1]��������ʧ�ܣ�"}'
		RETURN 5
	END

	-- У������
	IF @LastEmail<>@strEmail
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"��֤�����[2]��������ʧ�ܣ�"}'
		RETURN 5
	END

	-- ��֤��Ϣ
	DECLARE @BindUserID INT
	DECLARE @AuthKind TINYINT
	DECLARE @AuthCodeMD5 NVARCHAR(32)
	DECLARE @AcquireMachine NVARCHAR(32)	
	DECLARE @LastAcquireDate DATETIME
	SELECT @BindUserID=UserID, @AuthKind=AuthKind, @AuthCodeMD5=AuthCodeMD5, @AcquireMachine=AcquireMachine, @LastAcquireDate=LastAcquireDate FROM RecordAuthCode(NOLOCK) WHERE CodeKind=2 AND MobileEmail=@strEmail

	-- ��֤�벻����
	IF @BindUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"��֤�����[3]��������ʧ�ܣ�"}'
		RETURN 6
	END

	-- �û���ƥ��
	IF @BindUserID<>@UserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"��֤�����[4]��������ʧ�ܣ�"}'
		RETURN 6
	END

	-- У������
	IF @AuthKind<>4
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"��֤�����[5]��������ʧ�ܣ�"}'
		RETURN 6		
	END

	-- У����֤��
	IF @AuthCodeMD5<>@strAuthCode
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"��֤�����[6]��������ʧ�ܣ�"}'
		RETURN 7				
	END

	-- У�������
	IF @AcquireMachine<>@strMachineID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1069,"Msg":"���Ļ����뷢���쳣��������ʧ�ܣ�"}'
		RETURN 8
	END

	-- ��Чʱ��
	DECLARE @ACValidSeconds INT
	SELECT @ACValidSeconds=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ACValidSecond'

	-- ��������
	IF @ACValidSeconds IS NULL SET @ACValidSeconds=600

	-- ʱ����
	DECLARE @SecondDiff INT
	SET @SecondDiff=DateDiff(s,@LastAcquireDate,GetDate())

	-- ʱ������
	IF @SecondDiff>@ACValidSeconds	
	BEGIN			
		SET @strErrorDescribe=N'{"Idx":1029,"Msg":"��֤���ѹ��ڣ������»�ȡ��֤�룡"}'
		RETURN 8
	END

	-- ��������
	--DECLARE @BindMPDeductCard INT
	
	---- ��ȡ����
	--SELECT @BindMPDeductCard=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'BindMPDeductCard'				
			
	---- ��������
	--IF @BindMPDeductCard IS NULL SET @BindMPDeductCard=10			
	
	---- �û��Ƹ�
	DECLARE @UserCard BIGINT
			
	---- �۳�����
	--IF @BindMPDeductCard>0
	--BEGIN
	--	-- ��ѯ����				
	--	SELECT @UserCard=Card FROM LHGameBattleDB.dbo.GameCardInfo WHERE UserID=@dwUserID	

	--	-- ��������
	--	IF @UserCard IS NULL SET @UserCard=0

	--	-- ���У��
	--	IF @BindMPDeductCard>@UserCard
	--	BEGIN
	--		SET @strErrorDescribe=N'{"Idx":1072,"Msg":"��ķ������㣬���ֵ���ٴγ��ԣ�"}'
	--		RETURN 9	
	--	END

	--	-- ���·���
	--	UPDATE LHGameBattleDB.dbo.GameCardInfo SET Card=Card-@BindMPDeductCard WHERE UserID=@dwUserID		

	---- �����¼
	--INSERT INTO LHGameRecordDB.dbo.RecordChangeCard(UserID, GameID, TypeID, BeforeCard, ChangeCard,CollectClientIP, CollectDateTime)
	--VALUES (@dwUserID, @GameID, 6, @UserCard, -@BindMPDeductCard,  N'-----------', GetDate())

	--END		

	-- ������Ϣ
	UPDATE AccountsAuthInfo SET AuthTimes=AuthTimes+1,AcquireTimes=0,LastMobileEmail=N'' WHERE UserID=@dwUserID AND CodeKind=2

	-- ���¼�¼
	UPDATE RecordAuthCode SET AuthCode=N'',AuthCodeMD5=N'',AuthTimes=AuthTimes+1,AcquireTimes=0 WHERE CodeKind=2 AND MobileEmail=@strEmail

	-- ��������
	UPDATE AccountsInfo SET EMail=N'' WHERE UserID=@dwUserID

	-- ��������
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��¼��֤����
	UPDATE RecordAuthTimes SET AuthTimes=AuthTimes+1 WHERE DateID=@DateID AND UserID=@dwUserID AND CodeKind=2 AND AuthKind=@AuthKind
	IF @@ROWCOUNT=0
	BEGIN
		-- �����¼
		INSERT INTO RecordAuthTimes(DateID,UserID,CodeKind,AuthKind,AuthTimes) VALUES (@DateID,@dwUserID,2,@AuthKind,1)
	END

	-- ��ѯ�Ƹ�
	SELECT @UserCard=Score FROM LHGameBattleDB.dbo.GameCardInfo WHERE UserID=@dwUserID	
	
	-- ��������
	IF @UserCard IS NULL SET @UserCard=0
	
	-- �󶨳ɹ�
	SET @strErrorDescribe=N'{"Idx":1070,"Msg":"������ɹ���"}'	

	-- �׳��Ƹ�
	SELECT @UserCard AS UserCard
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���ֻ�����
CREATE PROC GSP_MB_BindMobilePhone
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����	
	@strNewPassword NCHAR(32),					-- �û�����
	@strMachineID NVARCHAR(32),					-- ��������
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMobilePhone NVARCHAR(21),				-- �ֻ�����
	@strAuthCode NCHAR(32),						-- �� ֤ ��
	@dwUnBindUserID INT,						-- ����ʶ	
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
	DECLARE @LogonPass NCHAR(32)		
	SELECT @UserID=a.UserID, @LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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
	
	-- �����ж�	
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
		RETURN 4
	END	

	-- ��ѯ�û�
	DECLARE @BindUserID INT
	SELECT @BindUserID=UserID FROM AccountsInfo(NOLOCK) WHERE MobilePhone=@strMobilePhone

	-- У���ʶ
	IF @BindUserID IS NOT NULL AND @BindUserID<>@dwUnBindUserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1067,"Msg":"��Ǹ�����ֻ����Ѿ���������Ϸ�˺Ű󶨣�������µ��ֻ��Ű󶨣�"}'
		RETURN 5		
	END

	-- ��ѯ�ֻ���
	DECLARE @ATLastAcquireDate DATETIME
	DECLARE @LastMobilePhone NVARCHAR(21)		
	SELECT @LastMobilePhone=LastMobileEmail,@ATLastAcquireDate=LastAcquireDate FROM AccountsAuthInfo(NOLOCK) WHERE UserID=@dwUserID AND CodeKind=1
	
	-- У���¼
	IF @LastMobilePhone IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1068,"Msg":"��֤������ֻ��Ű�ʧ�ܣ�"}'
		RETURN 51
	END

	-- У���ֻ���
	IF @LastMobilePhone<>@strMobilePhone
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1068,"Msg":"��֤������ֻ��Ű�ʧ�ܣ�"}'
		RETURN 52
	END

	-- ��֤��Ϣ
	DECLARE @AuthKind TINYINT	
	DECLARE @AuthCodeMD5 NVARCHAR(32)
	DECLARE @AcquireMachine NVARCHAR(32)
	DECLARE @MBLastAcquireDate DATETIME
	SELECT @BindUserID=UserID,@AuthKind=AuthKind, @AuthCodeMD5=AuthCodeMD5, @AcquireMachine=AcquireMachine, @MBLastAcquireDate=LastAcquireDate FROM RecordAuthCode(NOLOCK) WHERE CodeKind=1 AND MobileEmail=@strMobilePhone

	-- ��֤�벻����
	IF @BindUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1068,"Msg":"��֤������ֻ��Ű�ʧ�ܣ�"}'
		RETURN 61
	END

	-- �û���ƥ��
	IF @BindUserID<>@UserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1068,"Msg":"��֤������ֻ��Ű�ʧ�ܣ�"}'
		RETURN 62
	END

	-- У������
	IF @AuthKind<>1
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1068,"Msg":"��֤������ֻ��Ű�ʧ�ܣ�"}'
		RETURN 63		
	END

	-- У����֤��
	IF @AuthCodeMD5<>@strAuthCode
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1068,"Msg":"��֤������ֻ��Ű�ʧ�ܣ�"}'
		RETURN 7				
	END

	-- У�������
	IF @AcquireMachine<>@strMachineID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1069,"Msg":"���Ļ����뷢���쳣���ֻ��Ű�ʧ�ܣ�"}'
		RETURN 8
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

	-- ������ʶ
	DECLARE @ExternalID INT	
	SELECT @ExternalID=ExternalID FROM AccountsExternal(NOLOCK) WHERE UserID=@dwUserID

	-- ��������	
	IF @ExternalID IS NULL SET @ExternalID=0

	-- �û��Ƹ�
	DECLARE @UserCard INT	

	-- �󶨼�¼
	IF NOT EXISTS(SELECT 1 FROM RecordBindMobilePhone(NOLOCK) WHERE UserID=@dwUserID)
	BEGIN			
		-- �����¼
		INSERT INTO RecordBindMobilePhone(UserID,MobilePhone,CollectDate) VALUES (@dwUserID,@strMobilePhone,GetDate())
	END	

	-- ������Ϣ
	UPDATE AccountsAuthInfo SET AuthTimes=AuthTimes+1,AcquireTimes=0, LastAcquireDate=GetDate(),LastMobileEmail=N'' WHERE UserID=@dwUserID AND CodeKind=1

	-- ���¼�¼
	UPDATE RecordAuthCode SET AuthCode=N'',AuthCodeMD5=N'', AuthTimes=AuthTimes+1,AcquireTimes=0 WHERE CodeKind=1 AND MobileEmail=@strMobilePhone			
	UPDATE RecordBindMobilePhone SET MobilePhone=@strMobilePhone,CollectDate=GetDate() WHERE UserID=@dwUserID

	-- ��������
	IF @strNewPassword<>N'' SET @LogonPass=@strNewPassword

	-- ����ֻ�
	IF @dwUnBindUserID<>0 
		UPDATE AccountsInfo SET MobilePhone=N'' WHERE UserID=@dwUnBindUserID
	ELSE
		-- ������Ϣ
		UPDATE AccountsInfo SET MobilePhone=@strMobilePhone,LogonPass=@LogonPass WHERE UserID=@dwUserID	
	
	-- �ο͸���Ϊ��ʽ�˺�,�����˺���
	IF @dwUnBindUserID = 0 AND EXISTS(SELECT 1 FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID AND RegisterMode=1)
	BEGIN
		DECLARE @Accounts NVARCHAR(32)
		DECLARE @strNickName NVARCHAR(32)
		SET @Accounts=CONCAT(N'Mobile',SUBSTRING(@strMobilePhone,LEN(@strMobilePhone)-3,4))
		SET @strNickName=CONCAT(SUBSTRING(@strMobilePhone,LEN(@strMobilePhone)-3,4),N':',N'Mobile')	

		EXEC dbo.LHP_AdjustmentAccounts @Accounts OUTPUT,@strNickName OUTPUT

		UPDATE AccountsInfo SET RegisterMode=2,NickName=@strNickName,Accounts=@Accounts,RegAccounts=@Accounts WHERE UserID=@dwUserID AND RegisterMode=1
	END
	
	-- ��������
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��¼��֤����
	UPDATE RecordAuthTimes SET AuthTimes=AuthTimes+1 WHERE DateID=@DateID AND UserID=@dwUserID AND CodeKind=1 AND AuthKind=@AuthKind
	IF @@ROWCOUNT=0
	BEGIN
		-- �����¼
		INSERT INTO RecordAuthTimes(DateID,UserID,CodeKind, AuthKind,AuthTimes) VALUES (@DateID,@dwUserID,1,@AuthKind,1)
	END
	
	-- ��ѯ�Ƹ�
	SELECT @UserCard =Score FROM LHGameBattleDB.dbo.GameCardInfo(NOLOCK) WHERE UserID=@dwUserID	

	-- ��������
	IF @UserCard IS NULL SET @UserCard=0

	-- �󶨳ɹ�
	SET @strErrorDescribe=N'{"Idx":1070,"Msg":"�ֻ��Ű󶨳ɹ���"}'	

	-- �׳��Ƹ�
	SELECT @UserCard AS UserCard
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ����ֻ���
CREATE PROC GSP_MB_UnBindMobilePhone
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strMachineID NVARCHAR(32),					-- ��������
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMobilePhone NVARCHAR(21),				-- �ֻ�����
	@strAuthCode NCHAR(32),						-- ��֤��	
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
	DECLARE @LogonPass NCHAR(32)
	DECLARE @BindMobilePhone NVARCHAR(21)
	SELECT @UserID=a.UserID, @GameID=a.GameID, @LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus, @BindMobilePhone=a.MobilePhone 
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
	
	-- �����ж�	
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
		RETURN 4
	END	

	-- ��ѯ�ֻ���
	DECLARE @LastMobilePhone NVARCHAR(21)	
	SELECT @LastMobilePhone=LastMobileEmail FROM AccountsAuthInfo(NOLOCK) WHERE UserID=@dwUserID
	
	-- У���¼
	IF @LastMobilePhone IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"��֤�����[1]���ֻ��Ž��ʧ�ܣ�"}'
		RETURN 5
	END

	-- У���ֻ���
	IF @LastMobilePhone<>@strMobilePhone
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"��֤�����[2]���ֻ��Ž��ʧ�ܣ�"}'
		RETURN 5
	END

	-- ��֤��Ϣ
	DECLARE @BindUserID INT
	DECLARE @AuthKind TINYINT
	DECLARE @AuthCodeMD5 NVARCHAR(32)
	DECLARE @AcquireMachine NVARCHAR(32)	
	DECLARE @LastAcquireDate DATETIME
	SELECT @BindUserID=UserID, @AuthKind=AuthKind, @AuthCodeMD5=AuthCodeMD5, @AcquireMachine=AcquireMachine, @LastAcquireDate=LastAcquireDate FROM RecordAuthCode(NOLOCK) WHERE  CodeKind=1 AND MobileEmail=@strMobilePhone

	-- ��֤�벻����
	IF @BindUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"��֤�����[3]���ֻ��Ž��ʧ�ܣ�"}'
		RETURN 6
	END

	-- �û���ƥ��
	IF @BindUserID<>@UserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"��֤�����[4]���ֻ��Ž��ʧ�ܣ�"}'
		RETURN 6
	END

	-- У������
	IF @AuthKind<>2
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"��֤�����[5]���ֻ��Ž��ʧ�ܣ�"}'
		RETURN 6		
	END

	-- У����֤��
	IF @AuthCodeMD5<>@strAuthCode
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1071,"Msg":"��֤�����[6]���ֻ��Ž��ʧ�ܣ�"}'
		RETURN 7				
	END

	-- У�������
	IF @AcquireMachine<>@strMachineID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1069,"Msg":"���Ļ����뷢���쳣���ֻ��Ž��ʧ�ܣ�"}'
		RETURN 8
	END

	-- ��Чʱ��
	DECLARE @ACValidSeconds INT
	SELECT @ACValidSeconds=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ACValidSecond'

	-- ��������
	IF @ACValidSeconds IS NULL SET @ACValidSeconds=600

	-- ʱ����
	DECLARE @SecondDiff INT
	SET @SecondDiff=DateDiff(s,@LastAcquireDate,GetDate())

	-- ʱ������
	IF @SecondDiff>@ACValidSeconds	
	BEGIN			
		SET @strErrorDescribe=N'{"Idx":1029,"Msg":"��֤���ѹ��ڣ������»�ȡ��֤�룡"}'
		RETURN 8
	END

	-- ��������
	--DECLARE @BindMPDeductCard INT
	
	---- ��ȡ����
	--SELECT @BindMPDeductCard=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'BindMPDeductCard'				
			
	---- ��������
	--IF @BindMPDeductCard IS NULL SET @BindMPDeductCard=10			
	
	---- �û��Ƹ�
	DECLARE @UserCard INT
			
	---- �۳�����
	--IF @BindMPDeductCard>0
	--BEGIN
	--	-- ��ѯ����				
	--	SELECT @UserCard=Score FROM LHGameBattleDB.dbo.GameCardInfo WHERE UserID=@dwUserID	

	--	-- ��������
	--	IF @UserCard IS NULL SET @UserCard=0

	--	-- ���У��
	--	IF @BindMPDeductCard>@UserCard
	--	BEGIN
	--		SET @strErrorDescribe=N'{"Idx":1072,"Msg":"��ķ������㣬���ֵ���ٴγ��ԣ�"}'
	--		RETURN 9	
	--	END

	--	-- ���·���
	--	UPDATE LHGameBattleDB.dbo.GameCardInfo SET Score-=@BindMPDeductCard WHERE UserID=@dwUserID		

	---- �����¼
	--INSERT INTO LHGameRecordDB.dbo.RecordChangeCard(UserID, GameID, TypeID, BeforeCard, ChangeCard,CollectClientIP, CollectDateTime)
	--VALUES (@dwUserID, @GameID, 5, @UserCard, -@BindMPDeductCard,  N'-----------', GetDate())
	--END	
	
	-- ������Ϣ
	UPDATE AccountsAuthInfo SET AuthTimes=AuthTimes+1,AcquireTimes=0,LastMobileEmail=N'' WHERE UserID=@dwUserID

	-- ���¼�¼
	UPDATE RecordAuthCode SET AuthCode=N'',AuthCodeMD5=N'',AuthTimes=AuthTimes+1,AcquireTimes=0 WHERE CodeKind=1 AND MobileEmail=@strMobilePhone

	-- �����ֻ���
	UPDATE AccountsInfo SET MobilePhone=N'' WHERE UserID=@dwUserID

	-- ��������
	DECLARE @ShutDownOverTime INT
	SET @ShutDownOverTime=dbo.LHF_GetTimestamp()+24*60*60*365*3

	-- ע���˺�
	UPDATE AccountsStatus SET ShutDownStatus=1,ShutDownOverTime=@ShutDownOverTime WHERE UserID=@dwUserID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO AccountsStatus(UserID,ShutupStatus,ShutupOverTime,ShutDownStatus,ShutDownOverTime,LastShutupDateTime,LastShutDownDateTime)
		VALUES (@dwUserID,0,0,1,@ShutDownOverTime,GETDATE(),GETDATE())
	END

	-- ��������
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��¼��֤����
	UPDATE RecordAuthTimes SET AuthTimes=AuthTimes+1 WHERE DateID=@DateID AND UserID=@dwUserID AND CodeKind=1 AND AuthKind=@AuthKind
	IF @@ROWCOUNT=0
	BEGIN
		-- �����¼
		INSERT INTO RecordAuthTimes(DateID,UserID,CodeKind, AuthKind,AuthTimes) VALUES (@DateID,@dwUserID,1,@AuthKind,1)
	END

	-- ��ѯ�Ƹ�
	SELECT @UserCard=Score FROM LHGameBattleDB.dbo.GameCardInfo WHERE UserID=@dwUserID

	-- ��������
	IF @UserCard IS NULL SET @UserCard=0
	
	-- �󶨳ɹ�
	SET @strErrorDescribe=N'{"Idx":1070,"Msg":"�ֻ��Ž��ɹ���"}'	

	-- �׳��Ƹ�
	SELECT @UserCard AS UserCard
	
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_MB_ResetLogonPasswd
	@dwBindUserID INT,							-- �󶨱�ʶ
	@strPassword NCHAR(32),						-- �û�����
	@strMachineID NVARCHAR(32),					-- ��������
	@cbCodeKind TINYINT,						-- ��֤������
	@strMobileEmail NVARCHAR(64),				-- �ֻ�����
	@strAuthCode NCHAR(32),						-- ��֤��	
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
	DECLARE @LogonPass NCHAR(32)	

	-- �ֻ���֤��
	IF @cbCodeKind=1
	BEGIN
		SELECT @UserID=a.UserID, @LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
		FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.MobilePhone=@strMobileEmail
	END

	-- ������֤��
	IF @cbCodeKind=2
	BEGIN
		SELECT @UserID=a.UserID, @LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
		FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.Email=@strMobileEmail
	END

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

	-- У���ʶ
	IF @UserID<>@dwBindUserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1075,"Msg":"��֤���쳣[1]����������ʧ�ܣ�"}'
		RETURN 4
	END	

	-- ��֤��Ϣ
	DECLARE @BindUserID INT
	DECLARE @AuthKind TINYINT
	DECLARE @AuthCodeMD5 NVARCHAR(32)
	DECLARE @AcquireMachine NVARCHAR(32)	
	DECLARE @LastAcquireDate DATETIME
	SELECT @BindUserID=UserID, @AuthKind=AuthKind, @AuthCodeMD5=AuthCodeMD5, @AcquireMachine=AcquireMachine, @LastAcquireDate=LastAcquireDate FROM RecordAuthCode(NOLOCK) WHERE CodeKind=@cbCodeKind AND MobileEmail=@strMobileEmail

	-- ��֤�벻����
	IF @BindUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1075,"Msg":"��֤���쳣[2]����������ʧ�ܣ�"}'
		RETURN 6
	END

	-- �û���ƥ��
	IF @BindUserID<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1075,"Msg":"��֤���쳣[3]����������ʧ�ܣ�"}'
		RETURN 7
	END

	-- У������
	IF @AuthKind<>5
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1075,"Msg":"��֤���쳣[4]����������ʧ�ܣ�"}'
		RETURN 8
	END
	
	-- У����֤��
	IF @AuthCodeMD5<>@strAuthCode
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1075,"Msg":"��֤���쳣[5]����������ʧ�ܣ�"}'
		RETURN 9
	END

	-- У�������
	IF @AcquireMachine<>@strMachineID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1069,"Msg":"���Ļ����뷢���쳣����������ʧ�ܣ�"}'
		RETURN 10
	END

	-- ��Чʱ��
	DECLARE @ACValidSeconds INT
	SELECT @ACValidSeconds=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ACValidSecond'

	-- ��������
	IF @ACValidSeconds IS NULL SET @ACValidSeconds=600

	-- ʱ����
	DECLARE @SecondDiff INT
	SET @SecondDiff=DateDiff(s,@LastAcquireDate,GetDate())

	-- ʱ������
	IF @SecondDiff>@ACValidSeconds	
	BEGIN			
		SET @strErrorDescribe=N'{"Idx":1029,"Msg":"��֤���ѹ��ڣ������»�ȡ��֤�룡"}'
		RETURN 11
	END

	-- ������Ϣ
	DECLARE @ATAuthTimes SMALLINT
	DECLARE @ATLastAcquireDate DATETIME
	SELECT @ATAuthTimes=AuthTimes,@ATLastAcquireDate=LastAcquireDate FROM AccountsAuthInfo(NOLOCK) WHERE UserID=@UserID AND CodeKind=@cbCodeKind

	-- ��������
	IF @ATAuthTimes IS NULL
	BEGIN
		INSERT INTO AccountsAuthInfo(UserID,CodeKind,AuthTimes,AcquireTimes,LastAcquireDate,LastMobileEmail) VALUES (@UserID,@cbCodeKind,1,0,GetDate(),N'')		
	END ELSE
	BEGIN
		-- ʱ����
		DECLARE @DateDiff INT
		SET @DateDiff=DateDiff(day,@ATLastAcquireDate,GetDate())

		-- ��������
		IF @DateDiff<>0 SET @ATAuthTimes=0

		-- ��������
		UPDATE AccountsAuthInfo SET AuthTimes=@ATAuthTimes+1,AcquireTimes=0,LastAcquireDate=GetDate(),LastMobileEmail=N'' WHERE UserID=@UserID AND CodeKind=@cbCodeKind
	END

	-- ���´���
	UPDATE RecordAuthCode SET AuthCode=N'',AuthCodeMD5=N'',AuthTimes=AuthTimes+1,AcquireTimes=0 WHERE CodeKind=@cbCodeKind AND MobileEmail=@strMobileEmail

	-- ��������
	UPDATE AccountsInfo SET LogonPass=@strPassword WHERE UserID=@UserID	

	-- ��������
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��¼��֤����
	UPDATE RecordAuthTimes SET AuthTimes=AuthTimes+1 WHERE DateID=@DateID AND UserID=@UserID AND CodeKind=@cbCodeKind AND AuthKind=@AuthKind
	IF @@ROWCOUNT=0
	BEGIN
		-- �����¼
		INSERT INTO RecordAuthTimes(DateID,UserID,CodeKind,AuthKind,AuthTimes) VALUES (@DateID,@UserID,@cbCodeKind,@AuthKind,1)
	END
	
	-- �󶨳ɹ�
	SET @strErrorDescribe=N'{"Idx":1076,"Msg":"�������óɹ���"}'	
END

RETURN 0

GO


------------------------------------------------------------------------------------------------------

---- �տ��˺�
--CREATE PROC GSP_MB_ReceiptAccounts
--	@dwUserID INT,								-- �û���ʶ
--	@strPassword NCHAR(32),						-- �û�����
--	@strAccountsName NVARCHAR(32),				-- �˺�����
--	@strMachineID NVARCHAR(32),					-- ��������
--	@strClientIP NVARCHAR(15),					-- �ն˵�ַ
--	@strMobilePhone NVARCHAR(21),				-- �ֻ�����
--	@strAuthCode NCHAR(32),					-- ��֤��		
--	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
--WITH ENCRYPTION AS

---- ��������
--SET NOCOUNT ON

---- ִ���߼�
--BEGIN

--	-- ��ѯ�û�
--	DECLARE @UserID INT
--	DECLARE @Nullity BIT
--	DECLARE @ShutDown BIT	
--	DECLARE @LogonPass NCHAR(32)	
--	DECLARe @MobilePhone NVARCHAR(21)
--	SELECT @UserID=a.UserID,@MobilePhone=a.MobilePhone, @LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
--	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

--	-- ��ѯ�û�
--	IF @MobilePhone IS NULL
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
--		RETURN 1
--	END	

--	-- �ʺŽ�ֹ
--	IF @Nullity<>0
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����"}'
--		RETURN 2
--	END	

--	-- �ʺŹر�
--	IF @ShutDown<>0
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�"}'
--		RETURN 3
--	END	

--	-- У���ʶ
--	IF @MobilePhone<>@strMobilePhone
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1276,"Msg":"��֤���쳣[1]���տ��˺�����ʧ�ܣ�"}'
--		RETURN 4
--	END	

--	-- ��֤��Ϣ
--	DECLARE @BindUserID INT
--	DECLARE @AuthKind TINYINT
--	DECLARE @AuthCodeMD5 NVARCHAR(32)
--	DECLARE @AcquireMachine NVARCHAR(32)	
--	DECLARE @LastAcquireDate DATETIME
--	SELECT @BindUserID=UserID, @AuthKind=AuthKind, @AuthCodeMD5=AuthCodeMD5, @AcquireMachine=AcquireMachine, @LastAcquireDate=LastAcquireDate FROM RecordAuthCode(NOLOCK) WHERE MobilePhone=@strMobilePhone

--	-- ��֤�벻����
--	IF @BindUserID IS NULL
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1276,"Msg":"��֤���쳣[2]���տ��˺�����ʧ�ܣ�"}'
--		RETURN 6
--	END

--	-- �û���ƥ��
--	IF @BindUserID<>0
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1276,"Msg":"��֤���쳣[3]���տ��˺�����ʧ�ܣ�"}'
--		RETURN 6
--	END

--	-- У������
--	IF @AuthKind<>8
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1276,"Msg":"��֤���쳣[4]���տ��˺�����ʧ�ܣ�"}'
--		RETURN 6		
--	END
	
--	-- У����֤��
--	IF @AuthCodeMD5<>@strAuthCode
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1276,"Msg":"��֤���쳣[5]���տ��˺�����ʧ�ܣ�"}'
--		RETURN 7				
--	END

--	-- У�������
--	IF @AcquireMachine<>@strMachineID
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1276,"Msg":"���Ļ����뷢���쳣���տ��˺�����ʧ�ܣ�"}'
--		RETURN 8
--	END

--	-- ��Чʱ��
--	DECLARE @ACValidSeconds INT
--	SELECT @ACValidSeconds=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ACValidSecond'

--	-- ��������
--	IF @ACValidSeconds IS NULL SET @ACValidSeconds=600

--	-- ʱ����
--	DECLARE @SecondDiff INT
--	SET @SecondDiff=DateDiff(s,@LastAcquireDate,GetDate())

--	-- ʱ������
--	IF @SecondDiff>@ACValidSeconds	
--	BEGIN			
--		SET @strErrorDescribe=N'{"Idx":1029,"Msg":"��֤���ѹ��ڣ������»�ȡ��֤�룡"}'
--		RETURN 8
--	END

--	-- ������Ϣ
--	DECLARE @ATAuthTimes SMALLINT
--	DECLARE @ATLastAcquireDate DATETIME
--	SELECT @ATAuthTimes=AuthTimes,@ATLastAcquireDate=LastAcquireDate FROM AccountsAuthInfo(NOLOCK) WHERE UserID=@UserID

--	-- ��������
--	IF @ATAuthTimes IS NULL
--	BEGIN
--		INSERT INTO AccountsAuthInfo(UserID,AuthTimes,AcquireTimes,LastAcquireDate,LastMobilePhone)	VALUES (@UserID,1,0,GetDate(),N'')		
--	END ELSE
--	BEGIN
--		-- ʱ����
--		DECLARE @DateDiff INT
--		SET @DateDiff=DateDiff(day,@ATLastAcquireDate,GetDate())

--		-- ��������
--		IF @DateDiff<>0 SET @ATAuthTimes=0

--		-- ��������
--		UPDATE AccountsAuthInfo SET AuthTimes=@ATAuthTimes+1,AcquireTimes=0,LastAcquireDate=GetDate(),LastMobilePhone=N'' WHERE UserID=@UserID
--	END

--	-- ���´���
--	UPDATE RecordAuthCode SET AuthCode=N'',AuthCodeMD5=N'',AuthTimes=AuthTimes+1,AcquireTimes=0 WHERE MobilePhone=@strMobilePhone

--	-- �����˺�
--	--UPDATE LHInsureDB.dbo.AgentReceiptInfo SET AccountName=@strAccountsName,PhoneNum=@strMobilePhone,CollectDate=GetDate() WHERE UserID=@UserID	
--	--IF @@ROWCOUNT=0
--	--BEGIN
--	--	INSERT INTO LHInsureDB.dbo.AgentReceiptInfo(UserID,AccountName,PhoneNum,CollectDate)
--	--	VALUES (@dwUserID,@strAccountsName,@strMobilePhone,GetDate())
--	--END

--	-- ��������
--	DECLARE @DateID INT
--	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

--	-- ��¼��֤����
--	UPDATE RecordAuthTimes SET AuthTimes=AuthTimes+1 WHERE DateID=@DateID AND UserID=@dwUserID AND AuthKind=@AuthKind
--	IF @@ROWCOUNT=0
--	BEGIN
--		-- �����¼
--		INSERT INTO RecordAuthTimes(DateID,UserID,AuthKind,AuthTimes) VALUES (@DateID,@dwUserID,@AuthKind,1)
--	END
	
--	-- ���óɹ�
--	SET @strErrorDescribe=N'{"Idx":1277,"Msg":"�տ��˺����óɹ���"}'	
--END

--RETURN 0

--GO

----------------------------------------------------------------------------------------------------

