
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_BindParent]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_BindParent]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_BindInviteCode]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_BindInviteCode]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ���ϼ�
CREATE PROC GSP_MB_BindParent
	@dwUserID INT,								-- �û� I D
	@dwParentUserID INT,						-- �û� I D	
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@dwMailID BIGINT OUTPUT
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	DECLARE @ParentGameID BIGINT
	DECLARE @PParentUserID BIGINT
	SELECT @ParentGameID=GameID,@PParentUserID=ParentID FROM AccountsInfo(NOLOCK) WHERE UserID=@dwParentUserID
	IF @ParentGameID IS NULL 
	BEGIN
		SET @dwMailID=0
		RETURN 1
	END 	

	-- ��������
	IF @PParentUserID IS NULL SET @PParentUserID=0
	
	---- ��������
	--DECLARE @ExpireTime INT
	--DECLARE @CurrentTime INT
	--DECLARE @RewardGold BIGINT	
	--DECLARE @MailIndate BIGINT = 30*24*3600		

	---- ����ʱ��
	--SET @CurrentTime=dbo.LHF_GetTimestamp()
	--SET @ExpireTime=@CurrentTime+@MailIndate
	
	---- ��ѯ�󶨽���
	--SELECT @RewardGold=CONVERT(BIGINT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'BindInviteCodeReward'

	---- ��������
	--IF @dwMailID IS NULL SET @dwMailID=0
	--IF @RewardGold IS NULL SET @RewardGold=0

	-- �����ʼ�
	--IF @RewardGold>0
	--BEGIN
	--	-- �ʼ�����
	--	DECLARE @MailTitle NVARCHAR(128)
	--	DECLARE @MailContent NVARCHAR(512)

	--	-- ��������
	--	SET @MailTitle=N'Binding reward'
	--	SET @MailContent=N'{"Ops":["'+CONVERT(INT,@ParentGameID)+'","'+CONVERT(BIGINT,@RewardGold)+'"]}'

	--	-- �����ʼ���Ϣ
	--	INSERT INTO AccountsMailInfo(MailKind,MailType,MailTitle,MailContent,SendTime,ExpireTime,CollectTime)
	--	VALUES (2,1,@MailTitle,@MailContent,@CurrentTime,@ExpireTime,GetDate())

	--	-- �ʼ���ʶ
	--	SET @dwMailID=SCOPE_IDENTITY()		

	--	-- ���븽��
	--	INSERT INTO AccountsMailAttach(MailID,GoodsID,GoodsCount,GoodsIndate) VALUES (@dwMailID,2,@RewardGold,0)

	--	-- �����û�����
	--	INSERT INTO LHAccountsDB.dbo.AccountsMailBox(UserID,MailID,MailState,ExpireTime,UpdateTime)
	--	VALUES (@dwUserID,@dwMailID,1,@ExpireTime,GetDate())
	--END
	
	-- ���ڱ�ʶ
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- �����ϼ��ƹ�ͳ��
	UPDATE AccountsSpreadStat SET SpreadLowerCount+=1 WHERE UserID=@dwParentUserID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO AccountsSpreadStat(UserID,SpreadLowerCount,SpreadPayUser,SparedPayAmount,CollectDateTime) VALUES(@dwParentUserID,1,0,0,GETDATE())
	END

	-- ���´����ϼ�
	IF @PParentUserID<>0
	BEGIN
		UPDATE AccountsSpreadStat SET SpreadLLowerCount+=1 WHERE UserID=@PParentUserID
		IF @@ROWCOUNT=0
		BEGIN
			INSERT INTO AccountsSpreadStat(UserID,SpreadLLowerCount,SpreadPayUser,SparedPayAmount,CollectDateTime) VALUES(@PParentUserID,1,0,0,GETDATE())
		END
	END
		
	---- ��¼�Լ��ƹ㽱��
	--UPDATE LHGameRecordDB.dbo.ReportDayUser SET BindAward=BindAward+@RewardGold WHERE UserID=@dwUserID AND DateID=@DateID
	--IF @@ROWCOUNT=0
	--BEGIN
	--	INSERT INTO LHGameRecordDB.dbo.ReportDayUser(DateID,UserID,PayCount,PayAmount,LastPayAmount,LastPayDate,SpreadCount,
	--	SpreadValidCount,SpreadValidAward,SpreadPayUser,SpreadPayAmount,SpreadPayAward,BindAward,ReportDateTime) 
	--	VALUES (@DateID,@dwUserID,0,0,0,GETDATE(),0,0,0,0,0,0,@RewardGold,GETDATE())
	--END
	
	-- ��¼�ϼ��ƹ���
	UPDATE LHGameRecordDB.dbo.ReportDayUser SET SpreadLowerCount+=1 WHERE UserID=@dwParentUserID AND DateID=@DateID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO LHGameRecordDB.dbo.ReportDayUser(DateID,UserID,SpreadLowerCount,ReportDateTime) VALUES (@DateID,@dwParentUserID,1,GETDATE())
	END

	-- ��¼���ϼ��ƹ���
	IF @PParentUserID<>0
	BEGIN
		-- ��¼�ϼ��ƹ���
		UPDATE LHGameRecordDB.dbo.ReportDayUser SET SpreadLLowerCount+=1 WHERE UserID=@PParentUserID AND DateID=@DateID
		IF @@ROWCOUNT=0
		BEGIN
			INSERT INTO LHGameRecordDB.dbo.ReportDayUser(DateID,UserID,SpreadLLowerCount,ReportDateTime) VALUES (@DateID,@PParentUserID,1,GETDATE())
		END
	END
	
	-- �����ϼ�
	UPDATE AccountsInfo SET ParentID=@dwParentUserID WHERE UserID=@dwUserID

	-- �����¼
	INSERT INTO RecordBindParent(UserID,ParentID,TypeID,ClientIP,CollectDate) VALUES (@dwUserID,@dwParentUserID,0,@strClientIP,GetDate())

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_MB_BindInviteCode
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����	
	@dwInviteCode INT,							-- �� �� ��	
	@strMachine NVARCHAR(32),					-- ��������
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
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
	DECLARE @ParentUserID INT
	SELECT @ParentUserID=a.ParentID,@UserID=a.UserID,@LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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
	
	-- У���ϼ�
	IF @ParentUserID<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1292,"Msg":"��Ǹ�����Ѿ������ϼ�����������ʧ�ܣ�"}'
		RETURN 5
	END

	-- ��ѯ�û�
	DECLARE @ParentParentID INT
	DECLARE @ParentGameID INT
	DECLARE @ParentFaceID SMALLINT
	DECLARE @ParentNickName NVARCHAR(31)
	SELECT @ParentUserID=UserID,@ParentGameID=GameID,@ParentNickName=NickName,@ParentFaceID=FaceID,@ParentParentID=ParentID FROM AccountsInfo(NOLOCK) WHERE GameID=@dwInviteCode

	-- У���ʶ
	IF @ParentUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1293,"Msg":"��Ǹ�����ϼ���Ҳ����ڣ���������ʧ�ܣ�"}'
		RETURN 6
	END
	
	-- У������
	IF @ParentUserID=@UserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1294,"Msg":"��Ǹ�������ܰ��Լ�����������ʧ�ܣ�"}'
		RETURN 7
	END
	
	-- У���¼�
	IF @ParentParentID=@UserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1295,"Msg":"��Ǹ�������ܰ��Լ��������ң���������ʧ�ܣ�"}'
		RETURN 8
	END
	
	DECLARE @MailID BIGINT
	EXEC GSP_MB_BindParent @dwUserID,@ParentUserID,@strClientIP,@MailID OUTPUT

	-- �󶨳ɹ�
	SET @strErrorDescribe=N'{"Idx":1302,"Msg":"��������ɹ���"}'	

	-- �׳��ϼ���Ϣ
	SELECT @ParentGameID AS ParentGameID,@ParentNickName AS ParentNickName,@ParentFaceID AS ParentFaceID,@MailID AS MailID
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

