
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_DrawCommission]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_DrawCommission]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��ȡӶ��
CREATE PROC GSP_MB_DrawCommission
	@dwUserID INT,								-- �û� I D
	@lDrawAmount INT,							-- ��ȡ���
	@wReason INT,								-- ���ԭ��
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ��������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �Զ��ع�
SET XACT_ABORT ON

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
	
	-- ȡӶ����
	DECLARE @DrawEnabled TINYINT	
	DECLARE @MinDrawAmount INT	
	DECLARE @ServiceFeeRate SMALLINT		

	-- ��ѯ����
	SELECT @DrawEnabled=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'CommissionOption' AND StatusName=N'DrawEnabled'
	SELECT @MinDrawAmount=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'CommissionOption' AND StatusName=N'MinDrawAmount'	
	SELECT @ServiceFeeRate=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'CommissionOption' AND StatusName=N'ServiceFeeRate'	
	
	-- ��������
	IF @DrawEnabled IS NULL SET @DrawEnabled=1
	IF @MinDrawAmount IS NULL SET @MinDrawAmount=10000
	IF @ServiceFeeRate IS NULL SET @ServiceFeeRate=0	

	-- У�鿪��
	IF @DrawEnabled=0
	BEGIN
		-- ������Ϣ
		SET @strErrorDescribe=N'{"Idx":1133,"Msg":"��Ǹ��Ӷ����ȡ�����ѹرա�"}'	
		RETURN 15
	END

	-- У����
	IF @lDrawAmount<@MinDrawAmount
	BEGIN
		-- ������Ϣ
		SET @strErrorDescribe=N'{"Idx":1040,"Msg":"��Ǹ, ÿ����ȡ����С��%s��","Ops":["'+CONVERT(NVARCHAR(16),@MinDrawAmount/1000)+'"]}'	
		RETURN 16
	END		

	-- �������
	DECLARE @ServiceFee INT
	DECLARE @ChangeAmount INT

	-- �������
	SET @ServiceFee = @lDrawAmount*@ServiceFeeRate/1000	
	SET @ChangeAmount = @lDrawAmount-@ServiceFee

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��ѯ���
	DECLARE @Commission BIGINT
	SELECT @Commission=Commission FROM AccountsCommission WITH(UPDLOCK) WHERE UserID=@dwUserID

	-- ����Ӷ��
	IF @Commission IS NULL SET @Commission=0

	-- У�����
	IF @Commission<@lDrawAmount
	BEGIN
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ������Ϣ
		SET @strErrorDescribe=N'{"Idx":1135,"Msg":"��Ǹ�������ϵ�Ӷ�����㣬��ȡʧ�ܡ�"}'	
		RETURN 18
	END	

	-- ����Ӷ��
	UPDATE AccountsCommission SET Commission-=@lDrawAmount WHERE UserID=@dwUserID

	-- �����¼	
	INSERT INTO LHGameRecordDB.dbo.RecordDrawCommission(UserID,NickName,ServiceFee,DrawAmount,BeforeCommission,CollectClientIP,CollectMachineID,CollectDateTime)
	VALUES (@UserID,@NickName,@ServiceFee,@lDrawAmount,@Commission,@strClientIP,@strMachineID,GETDATE())

	-- д��Ƹ�
	EXEC GSP_GP_WriteGameWealth @dwUserID,2,@wReason,@ChangeAmount
	
	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- ��ȡ�ɹ�
	SET @strErrorDescribe=N'{"Idx":1051,"Msg":"�����ɹ���"}'

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

