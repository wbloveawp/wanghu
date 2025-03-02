
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ModifyAccountsInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ModifyAccountsInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryUserIndividual]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryUserIndividual]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ModifyUserIndividual]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ModifyUserIndividual]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_ModifyUserIndividual]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_ModifyUserIndividual]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �޸��ʺ�
CREATE PROC GSP_GP_ModifyAccountsInfo
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strAccounts NVARCHAR(32),					-- �û��ʺ�
	@strUnderWrite NVARCHAR(63),				-- ����ǩ��
	@wFaceID SMALLINT,							-- ͷ���ʶ
	@cbAge TINYINT,								-- �û�����
	@cbGender TINYINT,							-- �û��Ա�
	@strProvince NVARCHAR(15),					-- �û�ʡ��
	@strCity NVARCHAR(15),						-- �û�����
	@strArea NVARCHAR(15),						-- �û�����
	@bOldVersion TINYINT,						-- �Ƿ�ɰ�
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @UserID INT
	DECLARE @Accounts NVARCHAR(31)
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @LogonPass AS NCHAR(32)

	-- ��ѯ�û�
	SELECT @UserID=a.UserID,@Accounts=a.Accounts,@LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
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
		RETURN 2
	END	
	
	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
		RETURN 3
	END
	
	-- Ч������
	IF (SELECT COUNT(*) FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strAccounts)>0)>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1032,"Msg":"��Ǹ��֪ͨ��������������ǳƺ��������ַ�����������ǳƺ����ٴ��޸ģ�"}'
		RETURN 4
	END

	-- У������
	IF @strAccounts NOT LIKE '%[^0-9]%' AND LEN(@strAccounts)=16
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1032,"Msg":"��Ǹ��֪ͨ��������������ǳƺ��������ַ�����������ǳƺ����ٴ��޸ģ�"}'
		RETURN 116
	END

	-- �����ж�
	IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE NickName=@strAccounts AND UserID<>@dwUserID)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1033,"Msg":"���ǳ��ѱ��������ע���ˣ��뻻��һ�ǳƳ����ٴ��޸ģ�"}'
		RETURN 4
	END

	-- ��������
	UPDATE AccountsInfo SET UnderWrite=@strUnderWrite, FaceID=@wFaceID,  Gender=@cbGender WHERE UserID=@dwUserID

	-- ��������
	UPDATE AccountsDatum SET Age=@cbAge,Province=@strProvince, City=@strCity, Area=@strArea WHERE UserID=@dwUserID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO AccountsDatum(UserID,Age,Area,City,Province) VALUES(@dwUserID,@cbAge,@strArea,@strCity,@strProvince)
	END

	SET @strErrorDescribe=N'{"Idx":1034,"Msg":"�������Ѿ����������������ϣ�"}'	

	RETURN 0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ѯ����
CREATE PROC GSP_GP_QueryUserIndividual
	@dwUserID INT,								-- �û� I D
	@strClientIP NVARCHAR(15)					-- ���ӵ�ַ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @UserID INT
	DECLARE @QQ NVARCHAR(16)
	DECLARE @EMail NVARCHAR(33)
	DECLARE @UserNote NVARCHAR(256)
	DECLARE @SeatPhone NVARCHAR(32)
	DECLARE @MobilePhone NVARCHAR(16)
	DECLARE @Compellation NVARCHAR(16)
	DECLARE @DwellingPlace NVARCHAR(128)

	-- ��ѯ�û�
	SELECT @UserID=UserID, @QQ=QQ, @EMail=EMail, @UserNote=UserNote, @SeatPhone=SeatPhone,
		@MobilePhone=MobilePhone, @Compellation=Compellation, @DwellingPlace=DwellingPlace
	FROM AccountsDatum(NOLOCK) WHERE UserID=@dwUserID

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @QQ=N''	
		SET @EMail=N''	
		SET @UserNote=N''	
		SET @SeatPhone=N''	
		SET @MobilePhone=N''	
		SET @Compellation=N''	
		SET @DwellingPlace=N''	
	END

	-- �����Ϣ
	SELECT @dwUserID AS UserID, @Compellation AS Compellation, @QQ AS QQ, @EMail AS EMail, @SeatPhone AS SeatPhone,
		@MobilePhone AS MobilePhone, @DwellingPlace AS DwellingPlace, @UserNote AS UserNote

	RETURN 0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GP_ModifyUserIndividual
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strCompellation NVARCHAR(16),				-- ��ʵ����
	@strQQ NVARCHAR(16),						-- Q Q ����
	@strEMail NVARCHAR(33),						-- �����ʵ�
	@strSeatPhone NVARCHAR(32),					-- �̶��绰
	@strMobilePhone NVARCHAR(16),				-- �ƶ��绰
	@strDwellingPlace NVARCHAR(128),			-- ��ϸ��ַ
	@strUserNote NVARCHAR(256),					-- �û���ע				
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @LogonPass AS NCHAR(32)

	-- ��ѯ�û�
	SELECT @UserID=a.UserID, @LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
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
		RETURN 2
	END	
	
	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
		RETURN 3
	END

	-- �޸�����
	UPDATE AccountsDatum SET Compellation=@strCompellation, QQ=@strQQ, EMail=@strEMail, SeatPhone=@strSeatPhone,
		MobilePhone=@strMobilePhone, DwellingPlace=@strDwellingPlace, UserNote=@strUserNote WHERE UserID=@dwUserID
		
	-- �޸�����
	IF @@ROWCOUNT=0
	INSERT AccountsDatum (UserID, Compellation, QQ, EMail, SeatPhone, MobilePhone, DwellingPlace, UserNote)
	VALUES (@dwUserID, @strCompellation, @strQQ, @strEMail, @strSeatPhone, @strMobilePhone, @strDwellingPlace, @strUserNote)

	-- ������Ϣ
	IF @@ERROR=0 SET @strErrorDescribe=N'{"Idx":1034,"Msg":"�������Ѿ����������������ϣ�"}'	

	RETURN 0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �޸�����
CREATE PROC GSP_MB_ModifyUserIndividual
	@dwUserID INT,								-- �û� I D
	@cbGender TINYINT,							-- �û��Ա�	
	@wInfoMask INT,								-- ��Ϣ����
	@strNickName NVARCHAR(32),					-- �û��ǳ�
	@strUnderWrite NVARCHAR(63),				-- ����ǩ��	
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @UserID INT
	DECLARE @Gender TINYINT
	DECLARE @NickName NVARCHAR(31)
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @ActionMaskEver INT
	DECLARE @ActionMaskPerDay INT
	DECLARE @ActionMaskPerWeek INT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @UnderWrite NVARCHAR(63)	

	-- ��ѯ�û�
	SELECT @UserID=a.UserID,@Gender=a.Gender,@NickName=a.NickName,@UnderWrite=a.UnderWrite,@LogonPass=a.LogonPass, @ActionMaskEver=a.ActionMaskEver,@ActionMaskPerDay=a.ActionMaskPerDay,
		@ActionMaskPerWeek=a.ActionMaskPerWeek,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
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
		RETURN 2
	END	

	-- �Ա��޸�
	IF @Gender<>@cbGender
	BEGIN
		-- У������
		--IF (@ActionMaskEver&0x01)<>0
		--BEGIN
		--	SET @strErrorDescribe=N'{"Idx":1277,"Msg":"��Ǹ���Ա���Ϣϵͳ��ʱֻ����ÿ������޸�һ�Σ����Ѿ��޸Ĺ��ˣ�"}'
		--	RETURN 3
		--END

		-- ��������
		SET @ActionMaskEver|=0x01
	END
	
	-- �޸��ǳ�
	IF (@wInfoMask&0x0004)<>0
	BEGIN
		DECLARE @DateID INT
		SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

		-- ��ȡ����
		DECLARE @szModifyNickNameOption NVARCHAR(MAX)
		SELECT @szModifyNickNameOption=StatusValue FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ModifyNickNameOption'
		--
		DECLARE @DailyModify INT
		DECLARE @TotalModify INT
		SELECT @DailyModify=DailyModify,@TotalModify=TotalModify FROM RecordModifyUserIndividual WHERE DateID=@DateID AND UserID=@dwUserID AND ActionMask=0x0002
		IF @DailyModify IS NULL
		BEGIN
			SET @DailyModify = 0
			SELECT @TotalModify = COUNT(1) FROM RecordModifyUserIndividual WHERE UserID=@dwUserID AND ActionMask=0x0002
			IF @TotalModify IS NULL SET @TotalModify=0
			INSERT INTO RecordModifyUserIndividual (DateID,UserID,ActionMask,DailyModify,TotalModify,CollectDate) VALUES(@DateID,@dwUserID,0x0002,0,@TotalModify,GETDATE())
				
		END

		DECLARE @MoneyID INT
		DECLARE @MoneyAmount INT
		DECLARE @FreeTimesPerAccounts INT
		DECLARE @MaxTimesPerDay INT
		SET @MoneyID = JSON_VALUE(@szModifyNickNameOption,'$.MoneyID')
		SET @MoneyAmount = JSON_VALUE(@szModifyNickNameOption,'$.MoneyAmount')
		SET @FreeTimesPerAccounts = JSON_VALUE(@szModifyNickNameOption,'$.FreeTimesPerAccounts')
		SET @MaxTimesPerDay = JSON_VALUE(@szModifyNickNameOption,'$.MaxTimesPerDay')
		IF @DailyModify >= @MaxTimesPerDay
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1329,"Msg":"�޸Ĵ����Ѵ�����"}'
			RETURN 4
		END
		IF @MoneyAmount > 0 AND @TotalModify >= @FreeTimesPerAccounts
			BEGIN
			SET @MoneyAmount = -@MoneyAmount
			-- д��Ƹ�
			DECLARE @ResultCode INT
			EXEC @ResultCode=GSP_GP_WriteGameWealth @dwUserID,@MoneyID,17,@MoneyAmount

			-- У����
			IF @ResultCode<>0
			BEGIN
				SET @strErrorDescribe=N'{"Idx":1321,"Msg":"�����ϵĻƽ�����'+CONVERT(NVARCHAR(12),-@MoneyAmount)+'���޸��ǳ�ʧ�ܣ�","Ops":["'+CONVERT(NVARCHAR(12),-@MoneyAmount)+'"]}'
				RETURN 4
			END
		END

		-- �����ж�
		IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE NickName=@strNickName AND UserID<>@dwUserID)
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1033,"Msg":"���ǳ��ѱ��������ע���ˣ��뻻��һ�ǳƳ����ٴ��޸ģ�"}'
			RETURN 4
		END

		-- �����ǳ�
		SET @NickName = @strNickName

		-- ��������
		SET @ActionMaskEver|=0x0002

		-- ���¼�¼
		UPDATE RecordModifyUserIndividual SET DateID=@DateID,DailyModify=DailyModify+1,TotalModify=TotalModify+1,CollectDate=GETDATE() WHERE DateID=@DateID AND UserID=@dwUserID AND ActionMask=0x0002
	END

	-- �޸�ǩ��
	IF (@wInfoMask&0x0080)<>0
	BEGIN
		SET @UnderWrite=@strUnderWrite
	END

	-- �޸�����
	UPDATE AccountsInfo SET Gender=@cbGender,NickName=@NickName,UnderWrite=@UnderWrite,ActionMaskEver=@ActionMaskEver WHERE UserID=@dwUserID
	
	-- ������Ϣ
	IF @@ERROR=0 
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1034,"Msg":"�������Ѿ����������������ϣ�"}'	
	END

	-- ��ȡ�ƽ�
	DECLARE @UserGold BIGINT
	SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

	-- ��������
	IF @UserGold IS NULL SET @UserGold=0

	-- �׳�����
	SELECT @ActionMaskEver AS ActionMaskEver, @ActionMaskPerDay AS ActionMaskPerDay, @ActionMaskPerWeek AS ActionMaskPerWeek,@UserGold AS UserGold

	RETURN 0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
