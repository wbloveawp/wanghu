
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_ModifyInsurePassword]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_ModifyInsurePassword]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_InsureQuery]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_InsureQuery]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_InsureEnable]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_InsureEnable]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_InsureSave]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_InsureSave]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_InsureTake]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_InsureTake]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_InsureTransfer]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_InsureTransfer]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO
----------------------------------------------------------------------------------------------------

-- �����޸�����
CREATE PROC GSP_MB_ModifyInsurePassword
	@dwUserID INT,								-- �û� I D
	@strSrcPassword NVARCHAR(32),				-- �û�������
	@strDstPassword NVARCHAR(32),				-- �û�������
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

DECLARE @strInscurePassword NVARCHAR(32)
DECLARE @Score BIGINT
DECLARE @InsureScore BIGINT

BEGIN
	SELECT @strInscurePassword=InsurePass FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
	IF @strInscurePassword <> '' and @strInscurePassword <> @strSrcPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1335,"Msg":"��Ǹ, �����������"}'
		RETURN 1
	END

	UPDATE AccountsInfo SET InsurePass=@strDstPassword WHERE UserID=@dwUserID

	--
	SELECT @Score=Score,@InsureScore=InsureScore FROM LHGameGoldDB..GameScoreInfo WHERE UserID=@dwUserID
	IF @Score IS NULL SET @Score = 0
	IF @InsureScore IS NULL SET @InsureScore = 0

	--
	SELECT @Score AS Score,@InsureScore AS InsureScore
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------

-- ���в�ѯ
CREATE PROC GSP_MB_InsureQuery
	@dwUserID INT								-- �û� I D

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

DECLARE @strOption NVARCHAR(MAX)
DECLARE @Enable TINYINT

DECLARE @SaveAndTakeReserveAmount BIGINT
DECLARE @SaveAndTakeFeeRate INT
DECLARE @SaveAndTakeDealMin BIGINT
DECLARE @SaveAndTakeDealMax BIGINT
DECLARE @SaveAndTakeDailyMax BIGINT

DECLARE @TransferReserveAmount BIGINT
DECLARE @TransferFeeRate INT
DECLARE @TransferDealMin BIGINT
DECLARE @TransferDealMax BIGINT
DECLARE @TransferDailyMax BIGINT

DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT

BEGIN
	SELECT @SourceScore=Score,@SourceInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	IF @SourceScore IS NULL SET @SourceScore=0
	IF @SourceInsure IS NULL SET @SourceInsure=0

	-- ��ȡ������
	SELECT @strOption=StatusValue FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'InsureOption'
	IF ISJSON(@strOption) = 1
	BEGIN
		SELECT @Enable=JSON_VALUE(@strOption,'$.Enable'),
		@SaveAndTakeReserveAmount=JSON_VALUE(@strOption,'$.SaveAndTake.ReserveAmount'),
		@SaveAndTakeFeeRate=JSON_VALUE(@strOption,'$.SaveAndTake.Rate'),@SaveAndTakeDealMin=JSON_VALUE(@strOption,'$.SaveAndTake.DealMin'),
		@SaveAndTakeDealMax=JSON_VALUE(@strOption,'$.SaveAndTake.DealMax'),@SaveAndTakeDailyMax=JSON_VALUE(@strOption,'$.SaveAndTake.DailyMax'),
		@TransferReserveAmount=JSON_VALUE(@strOption,'$.Transfer.ReserveAmount'),
		@TransferFeeRate=JSON_VALUE(@strOption,'$.Transfer.Rate'),@TransferDealMin=JSON_VALUE(@strOption,'$.Transfer.DealMin'),
		@TransferDealMax=JSON_VALUE(@strOption,'$.Transfer.DealMax'),@TransferDailyMax=JSON_VALUE(@strOption,'$.Transfer.DailyMax')
	END
	IF @Enable IS NULL SET @Enable= 0
	IF @SaveAndTakeReserveAmount IS NULL SET @SaveAndTakeReserveAmount=0
	IF @SaveAndTakeFeeRate IS NULL SET @SaveAndTakeFeeRate=0
	IF @SaveAndTakeDealMin IS NULL SET @SaveAndTakeDealMin= 0
	IF @SaveAndTakeDealMax IS NULL SET @SaveAndTakeDealMax= 0
	IF @SaveAndTakeDailyMax IS NULL SET @SaveAndTakeDailyMax= 0

	IF @TransferReserveAmount IS NULL SET @TransferReserveAmount=0
	IF @TransferFeeRate IS NULL SET @TransferFeeRate=0
	IF @TransferDealMin IS NULL SET @TransferDealMin= 0
	IF @TransferDealMax IS NULL SET @TransferDealMax= 0
	IF @TransferDailyMax IS NULL SET @TransferDailyMax= 0

	SELECT @Enable AS Enable,@SourceScore AS Score,@SourceInsure AS InsureScore,
	@SaveAndTakeReserveAmount AS SaveAndTakeReserveAmount,@SaveAndTakeFeeRate AS SaveAndTakeFeeRate,@SaveAndTakeDealMin AS SaveAndTakeDealMin,@SaveAndTakeDealMax AS SaveAndTakeDealMax,@SaveAndTakeDailyMax AS SaveAndTakeDailyMax,
	@TransferReserveAmount AS TransferReserveAmount,@TransferFeeRate AS TransferFeeRate,@TransferDealMin AS TransferDealMin,@TransferDealMax AS TransferDealMax,@TransferDailyMax AS TransferDailyMax
	

END

RETURN 0

GO
----------------------------------------------------------------------------------------------------
-- ���п�ͨ
CREATE PROC GSP_MB_InsureEnable
	@dwUserID INT,								-- �û� I D
	@strPassword NVARCHAR(32),					-- �û�����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT

BEGIN
	-- ��ѯ�û�
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	DECLARE @InsurePassword NVARCHAR(33)
	SELECT @UserID=a.UserID,@InsurePassword=a.InsurePass,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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

	-- ��������
	UPDATE AccountsInfo SET InsurePass=@strPassword WHERE UserID=@dwUserID

	-- ��ѯ����
	SELECT @SourceScore=Score,@SourceInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	IF @SourceScore IS NULL SET @SourceScore=0
	IF @SourceInsure IS NULL SET @SourceInsure=0

	SELECT @SourceScore AS Score,@SourceInsure AS InsureScore

END

RETURN 0

GO
----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------

-- ���д洢
CREATE PROC GSP_MB_InsureSave
	@dwUserID INT,								-- �û� I D
	@nScore BIGINT,								-- �洢���
	@wReason INT,								-- ���ԭ��
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ��������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

DECLARE @strOption NVARCHAR(MAX)
DECLARE @Enable TINYINT
DECLARE @ReserveAmount BIGINT
DECLARE @FeeRate INT
DECLARE @DealMin BIGINT
DECLARE @DealMax BIGINT
DECLARE @DailyMax BIGINT

DECLARE @DateID INT
DECLARE @UserDailyOpScore BIGINT
DECLARE @UserDailyFee BIGINT

DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT

-- ִ���߼�
BEGIN
	-- ��ѯ�û�
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	DECLARE @InsurePassword NVARCHAR(33)
	SELECT @UserID=a.UserID,@InsurePassword=a.InsurePass,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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

	-- У��״̬
	IF EXISTS(SELECT 1 FROM AccountsOnlineInfo(NOLOCK) WHERE UserID=@dwUserID AND ServerID<>0)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1330,"Msg":"ϵͳ��⵽�����˺�������Ϸ�����У������˳������ٽ������в�����"}'
		RETURN 4
	END

	--��ѯ���
	SELECT @SourceScore=Score,@SourceInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	IF @SourceScore IS NULL SET @SourceScore=0
	IF @SourceScore < @nScore
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1337,"Msg":"��Ǹ, ��Ҳ��㣡"}'
		RETURN 5
	END

	-- ��ȡ������
	SELECT @strOption=StatusValue FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'InsureOption'
	IF ISJSON(@strOption) = 1
	BEGIN
		SELECT @Enable=JSON_VALUE(@strOption,'$.Enable'),
		@ReserveAmount=JSON_VALUE(@strOption,'$.SaveAndTake.ReserveAmount'),
		@FeeRate=JSON_VALUE(@strOption,'$.SaveAndTake.Rate'),@DealMin=JSON_VALUE(@strOption,'$.SaveAndTake.DealMin'),
		@DealMax=JSON_VALUE(@strOption,'$.SaveAndTake.DealMax'),@DailyMax=JSON_VALUE(@strOption,'$.SaveAndTake.DailyMax')
	END
	IF @Enable IS NULL SET @Enable= 0
	IF @ReserveAmount IS NULL SET @ReserveAmount=0
	IF @FeeRate IS NULL SET @FeeRate=0
	IF @DealMin IS NULL SET @DealMin= 0
	IF @DealMax IS NULL SET @DealMax= 0
	IF @DailyMax IS NULL SET @DailyMax= 0
	--
	IF @Enable = 0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1331,"Msg":"��Ǹ�����й����ѹرգ�"}'
		RETURN 6
	END
	--
	IF @nScore < @DealMin
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1332,"Msg":"��Ǹ, ÿ�ʲ�������С��%s��","Ops":["'+CONVERT(NVARCHAR(16),@DealMin)+'"]}'	
		RETURN 7
	END
	--
	IF @nScore > @DealMax
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1333,"Msg":"��Ǹ, ÿ�ʲ������ܴ���%s��","Ops":["'+CONVERT(NVARCHAR(16),@DealMax)+'"]}'	
		RETURN 8
	END
	-- ת������
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)	

	-- ��ѯ��ҵ��ռ�¼
	SELECT @UserDailyOpScore=Score FROM RecordUserInsureTransfer WHERE DateID=@DateID AND UserID=@dwUserID AND Type=0
	IF @UserDailyOpScore IS NULL 
	BEGIN
		INSERT INTO RecordUserInsureTransfer (DateID,UserID,Type,Score,Fee,CollectDate) VALUES(@DateID,@dwUserID,0,0,0,GETDATE())
		SELECT @UserDailyOpScore = 0
	END

	-- ÿ�մ洢����
	IF @UserDailyOpScore + @nScore > @DailyMax
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1334,"Msg":"��Ǹ, ÿ���ۼƲ������ܴ���%s��","Ops":["'+CONVERT(NVARCHAR(16),@DailyMax)+'"]}'	
		RETURN 9
	END

	DECLARE @ChangeScore BIGINT
	SET @ChangeScore=-@nScore
	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- д��Ƹ�
	EXEC GSP_GP_WriteGameWealth @dwUserID,1,@wReason,@ChangeScore 
	-- ���з���
	UPDATE LHGameGoldDB.dbo.GameScoreInfo SET InsureScore=InsureScore+@nScore WHERE UserID=@dwUserID

	-- д���м�¼
	INSERT INTO LHGameRecordDB.dbo.RecordInsure (SourceUserID,SourceScore,SourceInsure,TargetUserID,TargetScore,TargetInsure,OperateScore,OperateFee,OperateType,CientIP,CollectDate) 
	VALUES(@dwUserID,@SourceScore,@SourceInsure,0,0,0,@nScore,0,0,@strClientIP,GETDATE())

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- ������ҵ��ռ�¼
	UPDATE RecordUserInsureTransfer SET Score=Score+@nScore,CollectDate=GETDATE() WHERE DateID=@DateID AND UserID=@dwUserID AND Type=0

	--
	SELECT @SourceScore=Score,@SourceInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID

	--
	SELECT @SourceScore AS Score,@SourceInsure AS InsureScore
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------

-- ������ȡ
CREATE PROC GSP_MB_InsureTake
	@dwUserID INT,								-- �û� I D
	@nScore BIGINT,								-- ��ȡ���
	@strPassword NVARCHAR(32),					-- �û�����
	@wReason INT,								-- ���ԭ��
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ��������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

DECLARE @strOption NVARCHAR(MAX)
DECLARE @Enable TINYINT
DECLARE @ReserveAmount BIGINT
DECLARE @FeeRate INT
DECLARE @DealMin BIGINT
DECLARE @DealMax BIGINT
DECLARE @DailyMax BIGINT

DECLARE @DateID INT
DECLARE @UserDailyOpScore BIGINT
DECLARE @UserDailyFee BIGINT

DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT

-- ִ���߼�
BEGIN
	-- ��ѯ�û�
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	DECLARE @InsurePassword NVARCHAR(33)
	SELECT @UserID=a.UserID,@InsurePassword=a.InsurePass,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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

	-- У��״̬
	IF EXISTS(SELECT 1 FROM AccountsOnlineInfo(NOLOCK) WHERE UserID=@dwUserID AND ServerID<>0)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1330,"Msg":"ϵͳ��⵽�����˺�������Ϸ�����У������˳������ٽ������в�����"}'
		RETURN 4
	END

	-- У������
	IF @strPassword<>@InsurePassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1335,"Msg":"��Ǹ, �����������"}'
		RETURN 5
	END

	--��ѯ���
	SELECT @SourceScore=Score, @SourceInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	IF @SourceInsure IS NULL SET @SourceInsure=0
	IF @SourceInsure < @nScore
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1336,"Msg":"��Ǹ, �������㣡"}'
		RETURN 6
	END

	-- ��ȡ������
	SELECT @strOption=StatusValue FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'InsureOption'
	IF ISJSON(@strOption) = 1
	BEGIN
		SELECT @Enable=JSON_VALUE(@strOption,'$.Enable'),
		@ReserveAmount=JSON_VALUE(@strOption,'$.SaveAndTake.ReserveAmount'),
		@FeeRate=JSON_VALUE(@strOption,'$.SaveAndTake.Rate'),@DealMin=JSON_VALUE(@strOption,'$.SaveAndTake.DealMin'),
		@DealMax=JSON_VALUE(@strOption,'$.SaveAndTake.DealMax'),@DailyMax=JSON_VALUE(@strOption,'$.SaveAndTake.DailyMax')
	END
	IF @Enable IS NULL SET @Enable= 0
	IF @ReserveAmount IS NULL SET @ReserveAmount=0
	IF @FeeRate IS NULL SET @FeeRate=0
	IF @DealMin IS NULL SET @DealMin= 0
	IF @DealMax IS NULL SET @DealMax= 0
	IF @DailyMax IS NULL SET @DailyMax= 0
	--
	IF @Enable = 0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1331,"Msg":"��Ǹ�����й����ѹرգ�"}'
		RETURN 7
	END
	--
	IF @nScore < @DealMin
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1332,"Msg":"��Ǹ, ÿ�ʲ�������С��%s��","Ops":["'+CONVERT(NVARCHAR(16),@DealMin)+'"]}'	
		RETURN 8
	END
	--
	IF @nScore > @DealMax
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1333,"Msg":"��Ǹ, ÿ�ʲ������ܴ���%s��","Ops":["'+CONVERT(NVARCHAR(16),@DealMax)+'"]}'	
		RETURN 9
	END

	-- ת������
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)	

	-- ��������
	DECLARE @CurFee BIGINT 
	SELECT @CurFee=@nScore*@FeeRate/1000
	IF @CurFee < 0 SET @CurFee=0

	DECLARE @ChangeScore BIGINT
	SET @ChangeScore=(@nScore-@CurFee)
	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- д��Ƹ�
	EXEC GSP_GP_WriteGameWealth @dwUserID,1,@wReason,@ChangeScore 
	-- ���з���
	UPDATE LHGameGoldDB.dbo.GameScoreInfo SET InsureScore=InsureScore-@nScore WHERE UserID=@dwUserID

	-- д���м�¼
	INSERT INTO LHGameRecordDB..RecordInsure (SourceUserID,SourceScore,SourceInsure,TargetUserID,TargetScore,TargetInsure,OperateScore,OperateFee,OperateType,CientIP,CollectDate) 
	VALUES(@dwUserID,@SourceScore,@SourceInsure,0,0,0,@nScore,@CurFee,1,@strClientIP,GETDATE())

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	--
	SELECT @SourceScore=Score,@SourceInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID

	--
	SELECT @SourceScore AS Score,@SourceInsure AS InsureScore

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------

-- ����ת��
CREATE PROC GSP_MB_InsureTransfer
	@dwUserID INT,								-- �û� I D
	@dwGameID INT,								-- Ŀ���û�
	@nScore BIGINT,								-- ��ȡ���
	@strPassword NVARCHAR(32),					-- �û�����
	@wReason INT,								-- ���ԭ��
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ��������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

DECLARE @strOption NVARCHAR(MAX)
DECLARE @Enable TINYINT
DECLARE @ReserveAmount BIGINT
DECLARE @FeeRate INT
DECLARE @DealMin BIGINT
DECLARE @DealMax BIGINT
DECLARE @DailyMax BIGINT

DECLARE @DateID INT
DECLARE @UserDailyOpScore BIGINT
DECLARE @UserDailyFee BIGINT
DECLARE @UserInsureScore BIGINT

DECLARE @NickName NVARCHAR(32)
DECLARE @SourceGameID INT
DECLARE @TargetUserID INT
DECLARE @TargetNickName NVARCHAR(32)
DECLARE @TargetScore BIGINT
DECLARE @TargetInsure BIGINT

DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT

-- ִ���߼�
BEGIN
	-- ��ѯ�û�
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	DECLARE @InsurePassword NVARCHAR(33)
	SELECT @UserID=a.UserID,@SourceGameID=a.GameID,@InsurePassword=a.InsurePass,@NickName=NickName,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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

	-- Ŀ���û�
	SELECT @TargetUserID=a.UserID,@TargetNickName=a.NickName,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.GameID=@dwGameID

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

	-- У��״̬
	IF EXISTS(SELECT 1 FROM AccountsOnlineInfo(NOLOCK) WHERE UserID=@dwUserID AND ServerID<>0)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1330,"Msg":"ϵͳ��⵽�����˺�������Ϸ�����У������˳������ٽ������в�����"}'
		RETURN 8
	END

	-- У������
	IF @strPassword<>@InsurePassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1335,"Msg":"��Ǹ, �����������"}'
		RETURN 9
	END

	-- ��ȡ������
	SELECT @strOption=StatusValue FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'InsureOption'
	IF ISJSON(@strOption) = 1
	BEGIN
			SELECT @Enable=JSON_VALUE(@strOption,'$.Enable'),
		@ReserveAmount=JSON_VALUE(@strOption,'$.Transfer.ReserveAmount'),
		@FeeRate=JSON_VALUE(@strOption,'$.Transfer.Rate'),@DealMin=JSON_VALUE(@strOption,'$.Transfer.DealMin'),
		@DealMax=JSON_VALUE(@strOption,'$.Transfer.DealMax'),@DailyMax=JSON_VALUE(@strOption,'$.Transfer.DailyMax')
	END
	IF @Enable IS NULL SET @Enable= 0
	IF @ReserveAmount IS NULL SET @ReserveAmount=0
	IF @FeeRate IS NULL SET @FeeRate=0
	IF @DealMin IS NULL SET @DealMin= 0
	IF @DealMax IS NULL SET @DealMax= 0
	IF @DailyMax IS NULL SET @DailyMax= 0
	--
	IF @Enable = 0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1331,"Msg":"��Ǹ�����й����ѹرգ�"}'
		RETURN 10
	END
	--
	IF @nScore < @DealMin
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1332,"Msg":"��Ǹ, ÿ�ʲ�������С��%s��","Ops":["'+CONVERT(NVARCHAR(16),@DealMin)+'"]}'	
		RETURN 11
	END
	--
	IF @nScore > @DealMax
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1333,"Msg":"��Ǹ, ÿ�ʲ������ܴ���%s��","Ops":["'+CONVERT(NVARCHAR(16),@DealMax)+'"]}'	
		RETURN 12
	END
	-- ת������
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)	

	-- ��ѯ��ҵ��ռ�¼
	SELECT @UserDailyOpScore=Score FROM RecordUserInsureTransfer WHERE DateID=@DateID AND UserID=@dwUserID AND Type=2
	IF @UserDailyOpScore IS NULL 
	BEGIN
		INSERT INTO RecordUserInsureTransfer (DateID,UserID,Type,Score,Fee,CollectDate) VALUES(@DateID,@dwUserID,2,0,0,GETDATE())
		SELECT @UserDailyOpScore = 0
	END

	-- ÿ�մ洢����
	IF @UserDailyOpScore + @nScore > @DailyMax
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1334,"Msg":"��Ǹ, ÿ���ۼƲ������ܴ���%s��","Ops":["'+CONVERT(NVARCHAR(16),@DailyMax)+'"]}'	
		RETURN 9
	END

	-- У�����
	SELECT @UserInsureScore=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	IF @UserInsureScore IS NULL SET @UserInsureScore=0
	IF @UserInsureScore-@ReserveAmount < @nScore
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1336,"Msg":"��Ǹ, �������㣡"}'
		RETURN 13
	END

	-- ת������
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)	

	-- ��������
	DECLARE @CurFee BIGINT 
	SELECT @CurFee=@nScore*@FeeRate/1000

	DECLARE @ChangeScore BIGINT
	SET @ChangeScore=(@nScore-@CurFee)

	-- ���շ��ʼ�
	-- ��������
	DECLARE @MailID INT
	DECLARE @MailTitle NVARCHAR(128)
	DECLARE @MailContent NVARCHAR(512)

	-- �ʼ�����
	SET @MailTitle =N'���ת��֪ͨ'
	--SET @MailContent=N'{"Ops":["'+@TargetNickName+'","'+@NickName+'","'+CONVERT(NVARCHAR(18),@SourceGameID)+'","'+CONVERT(NVARCHAR(16),@ChangeScore)+'","'+ CONVERT(VARCHAR(100),GETDATE(),109)+'"]}'
	SET @MailContent=N'����,'+@NickName+'('+CONVERT(NVARCHAR(16),@SourceGameID)+N') ��� '+@TargetNickName+'('+CONVERT(NVARCHAR(16),@dwGameID)+N') ת��'+CONVERT(NVARCHAR(16),@ChangeScore)+N'ͭǮ����.'
	
	-- �����ʼ���Ϣ
	INSERT INTO AccountsMailInfo(MailKind,MailType,MailTitle,MailContent,SendTime,ExpireTime,CollectTime)
	VALUES (1,0,@MailTitle,@MailContent,dbo.LHF_GetTimestamp(),0,GetDate())

	-- �ʼ���ʶ
	SET @MailID=SCOPE_IDENTITY()	

	-- ��ѯ����
	SELECT @SourceScore=Score,@SourceInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	SELECT @TargetScore=Score,@TargetInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@TargetUserID

	IF @SourceScore IS NULL SET @SourceScore = 0
	IF @SourceInsure IS NULL SET @SourceInsure = 0
	IF @TargetScore IS NULL SET @TargetScore = 0
	IF @TargetInsure IS NULL SET @TargetInsure = 0

	-- ���븽��
	--INSERT INTO AccountsMailAttach(MailID,GoodsID,GoodsCount,GoodsIndate) VALUES (@MailID,1,@ChangeScore,0)			

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ת����
	UPDATE LHGameGoldDB.dbo.GameScoreInfo SET InsureScore=InsureScore-@nScore WHERE UserID=@dwUserID

	-- ���շ�
	UPDATE LHGameGoldDB.dbo.GameScoreInfo SET InsureScore=InsureScore+@ChangeScore WHERE UserID=@TargetUserID

	-- д���м�¼
	INSERT INTO LHGameRecordDB..RecordInsure (SourceUserID,SourceScore,SourceInsure,TargetUserID,TargetScore,TargetInsure,OperateScore,OperateFee,OperateType,CientIP,CollectDate) 
	VALUES(@dwUserID,@SourceScore,@SourceInsure,@TargetUserID,@TargetScore,@TargetInsure+@ChangeScore,@nScore,@CurFee,2,@strClientIP,GETDATE())

	-- �����û�����
	INSERT INTO AccountsMailBox(UserID,MailID,MailState,ExpireTime,UpdateTime) VALUES (@TargetUserID,@MailID,1,0,GetDate())


	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- ������ҵ��ռ�¼
	UPDATE RecordUserInsureTransfer SET Score=Score+@nScore,Fee=Fee+@CurFee ,CollectDate=GETDATE() WHERE DateID=@DateID AND UserID=@dwUserID AND Type=2

	--
	SELECT @SourceScore=Score,@SourceInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID

	--
	SELECT @SourceScore AS Score,@SourceInsure AS InsureScore,@TargetUserID AS TargetUserID,@MailID AS TargetMailID

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
