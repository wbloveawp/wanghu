----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryTurntableLottery]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryTurntableLottery]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_RequestTurntableLottery]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_RequestTurntableLottery]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- ת�̳齱
CREATE PROC GSP_GP_QueryTurntableLottery
	@dwUserID INT,							-- �û���ʶ
	@dwRecordID INT,						-- ��¼��ʶ
	@strClientIP NVARCHAR(16),				-- �ͻ���ַ	
	@strMachineID NVARCHAR(32)				-- ������ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �Զ��ع�
SET XACT_ABORT ON

-- ִ���߼�
BEGIN	
	-- ���ڱ�ʶ
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��ѯ����
	DECLARE @MayLotteryTimes INT
	DECLARE @UseLotteryTimes INT
	DECLARE @PayLotteryTimes INT
	DECLARE @FreeLotteryTimes INT
	DECLARE @GameTimeLotteryTimes INT
	DECLARE @GameCountLotteryTimes INT
	DECLARE @GameExpendLotteryTimes INT
	
	-- ��ѯ����
	SELECT @MayLotteryTimes=MayLotteryTimes,@UseLotteryTimes=UseLotteryTimes,@PayLotteryTimes=PayLotteryTimes,@FreeLotteryTimes=FreeLotteryTimes,@GameTimeLotteryTimes=GameTimeLotteryTimes,
			@GameCountLotteryTimes=GameCountLotteryTimes,@GameExpendLotteryTimes=GameExpendLotteryTimes
	FROM AccountsTurntableLottery(NOLOCK) WHERE DateID=@DateID AND UserID=@dwUserID

	-- ��ȡ����
	DECLARE @DailyMayLotteryTimes INT
	SELECT @DailyMayLotteryTimes=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TurntableOption' AND StatusName=N'DailyMayLotteryTimes'

	-- ��������
	IF @DailyMayLotteryTimes IS NULL SET @DailyMayLotteryTimes=3

	-- У������
	IF @MayLotteryTimes IS NULL 
	BEGIN

		-- ��������
		SELECT @MayLotteryTimes=0,@UseLotteryTimes=0,@PayLotteryTimes=0,@FreeLotteryTimes=0,@GameTimeLotteryTimes=0,@GameCountLotteryTimes=0,@GameExpendLotteryTimes=0

		-- ��������
		DECLARE @ConfineMachine TINYINT
		DECLARE @ConfineClientIP TINYINT
		DECLARE @DailyFreeLotteryTimes SMALLINT

		-- ��ȡ����
		SELECT @ConfineMachine=CONVERT(TINYINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TurntableOption' AND StatusName=N'ConfineMachine'	
		SELECT @ConfineClientIP=CONVERT(TINYINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TurntableOption' AND StatusName=N'ConfineClientIP'
		SELECT @DailyFreeLotteryTimes=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TurntableOption' AND StatusName=N'DailyFreeLotteryTimes'

		-- ��������
		IF @ConfineMachine IS NULL SET @ConfineMachine=0
		IF @ConfineClientIP IS NULL SET @ConfineClientIP=0
		IF @DailyFreeLotteryTimes IS NULL SET @DailyFreeLotteryTimes=3

		-- ��Ѵ���
		SET @FreeLotteryTimes=@DailyFreeLotteryTimes		
		
		-- ���ƻ���
		IF @ConfineMachine<>0
		BEGIN
			IF EXISTS(SELECT 1 FROM RecordTurntableMachine(NOLOCK) WHERE DateID=@DateID AND MachineID=@strMachineID)
			BEGIN
				SET @FreeLotteryTimes=0
			END ELSE
			BEGIN
				INSERT INTO RecordTurntableMachine(DateID,MachineID,CollectDate) VALUES(@DateID,@strMachineID,GETDATE())
			END
		END

		-- ���Ƶ�ַ
		IF @ConfineClientIP<>0
		BEGIN
			IF EXISTS(SELECT 1 FROM RecordTurntableClientIP(NOLOCK) WHERE DateID=@DateID AND ClientIP=@strClientIP)
			BEGIN
				SET @FreeLotteryTimes=0
			END ELSE
			BEGIN
				INSERT INTO RecordTurntableClientIP(DateID,ClientIP,CollectDate) VALUES(@DateID,@strClientIP,GETDATE())
			END
		END

		-- �����ܴ���
		SET @MayLotteryTimes=@FreeLotteryTimes

		-- �������� 
		IF @MayLotteryTimes>=@DailyMayLotteryTimes SET @MayLotteryTimes=@DailyMayLotteryTimes

		-- ��������
		INSERT INTO AccountsTurntableLottery(DateID,UserID,MayLotteryTimes,UseLotteryTimes,PayLotteryTimes,FreeLotteryTimes,GameTimeLotteryTimes,GameCountLotteryTimes,GameExpendLotteryTimes,CollectDateTime)
		VALUES(@DateID,@dwUserID,@MayLotteryTimes,@UseLotteryTimes,@PayLotteryTimes,@FreeLotteryTimes,@GameTimeLotteryTimes,@GameCountLotteryTimes,@GameExpendLotteryTimes,GETDATE())
	END 

	-- ��������
	DECLARE @SliceGameTime INT
	DECLARE @SliceGameCount INT
	DECLARE @SliceGameExpend BIGINT	

	-- ��ȡ����
	SELECT @SliceGameTime=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TurntableOption' AND StatusName=N'SliceGameTime'	
	SELECT @SliceGameCount=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TurntableOption' AND StatusName=N'SliceGameCount'
	SELECT @SliceGameExpend=CONVERT(BIGINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TurntableOption' AND StatusName=N'SliceGameExpend'	

	-- ��������
	IF @SliceGameTime IS NULL SET @SliceGameTime=0
	IF @SliceGameCount IS NULL SET @SliceGameCount=0
	IF @SliceGameExpend IS NULL SET @SliceGameExpend=0	

	-- ��Ϸ��Ϣ
	DECLARE @CurrGameTime INT
	DECLARE @CurrGameCount INT	
	DECLARE @CurrGameExpend BIGINT	

	-- ��ѯ����
	SELECT @CurrGameExpend=-LoseScore,@CurrGameCount=WinCount+LostCount,@CurrGameTime=PlayTimeCount FROM LHGameGoldDB.dbo.ReportUserScore(NOLOCK) WHERE DateID=@DateID AND UserID=@dwUserID

	-- У����
	IF @CurrGameExpend IS NOT NULL
	BEGIN				

		-- ��������
		DECLARE @GameTimeTimes INT = 0
		DECLARE @GameCountTimes INT = 0
		DECLARE @GameExpendTimes INT = 0

		-- ������Ϸʱ������
		IF @SliceGameTime>0
		BEGIN
			SET @GameTimeTimes = @CurrGameTime/@SliceGameTime
			SET @CurrGameTime = @CurrGameTime%@SliceGameTime
		END

		-- ������Ϸ��������
		IF @SliceGameCount>0
		BEGIN
			SET @GameCountTimes = @CurrGameCount/@SliceGameCount
			SET @CurrGameCount = @CurrGameCount%@SliceGameCount
		END

		-- ������Ϸ���Ĵ���
		IF @SliceGameExpend>0
		BEGIN
			SET @GameExpendTimes = @CurrGameExpend/@SliceGameExpend
			SET @CurrGameExpend = @CurrGameExpend%@SliceGameExpend
		END
		
		-- ��������
		IF @GameTimeTimes>@GameTimeLotteryTimes OR @GameCountTimes>@GameCountLotteryTimes OR @GameExpendTimes>@GameExpendLotteryTimes
		BEGIN
			-- ���´���
			SET @GameTimeLotteryTimes=@GameTimeTimes
			SET @GameCountLotteryTimes=@GameCountTimes
			SET @GameExpendLotteryTimes=@GameExpendTimes			
			SET @MayLotteryTimes=@FreeLotteryTimes+@PayLotteryTimes+@GameTimeLotteryTimes+@GameCountLotteryTimes+@GameExpendLotteryTimes

			-- �������� 
			IF @MayLotteryTimes>=@DailyMayLotteryTimes SET @MayLotteryTimes=@DailyMayLotteryTimes
			
			-- ��������
			UPDATE AccountsTurntableLottery SET MayLotteryTimes=@MayLotteryTimes,GameTimeLotteryTimes =@GameTimeLotteryTimes,GameCountLotteryTimes=@GameCountLotteryTimes,GameExpendLotteryTimes=@GameExpendLotteryTimes
			WHERE DateID=@DateID AND UserID=@dwUserID
		END
	END ELSE
	BEGIN
		SELECT @CurrGameTime=0,@CurrGameCount=0,@CurrGameExpend=0
	END

	-- ת�̼�¼
	DECLARE @TurntableRecord NVARCHAR(MAX)
	SET @TurntableRecord = (SELECT TOP 10 RecordID, NickName, GoodsID, GoodsCount, CollectDate FROM LHGameRecordDB.dbo.RecordTurntableLottery(NOLOCK) WHERE RecordID>@dwRecordID ORDER BY RecordID DESC FOR JSON AUTO)

	-- ��������
	IF @TurntableRecord IS NULL SET @TurntableRecord=N'[]'	

	-- ʣ�����
	DECLARE @ResidueLotteryTimes INT
	SET @ResidueLotteryTimes=@MayLotteryTimes-@UseLotteryTimes

	-- �׳�����
	SELECT @DailyMayLotteryTimes AS DailyMayLotteryTimes, @UseLotteryTimes AS UseLotteryTimes, @ResidueLotteryTimes AS ResidueLotteryTimes,@FreeLotteryTimes AS FreeLotteryTimes,@PayLotteryTimes AS PayLotteryTimes, @GameTimeLotteryTimes AS GameTimeLotteryTimes,@GameCountLotteryTimes AS GameCountLotteryTimes,
		@GameExpendLotteryTimes AS GameExpendLotteryTimes,@TurntableRecord AS TurntableRecord,@CurrGameTime AS CurrGameTime, @CurrGameCount AS CurrGameCount, @CurrGameExpend AS CurrGameExpend,@SliceGameTime AS SliceGameTime,
		@SliceGameCount AS SliceGameCount,@SliceGameExpend AS SliceGameExpend

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- ת�̳齱
CREATE PROC GSP_GP_RequestTurntableLottery
	@dwUserID INT,							-- �û���ʶ			
	@wReason INT,							-- ���ԭ��			
	@strErrorDescribe NVARCHAR(127) OUTPUT  -- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �Զ��ع�
SET XACT_ABORT ON

-- ִ���߼�
BEGIN	
	
	-- ��������
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @NickName NVARCHAR(32)
	DECLARE @MemberOrder SMALLINT
	
	-- ��ѯ�û�
	SELECT @UserID=a.UserID, @NickName=a.NickName, @Nullity=a.Nullity, @MemberOrder=a.MemberOrder, @ShutDown=b.ShutDownStatus 
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
		RETURN 3
	END	

	-- ��ѯ�齱
	DECLARE @TuantableEnabled TINYINT
	SELECT @TuantableEnabled=CONVERT(TINYINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'TurntableOption' AND StatusName=N'TurantableEnabled'

	-- ��������
	IF @TuantableEnabled IS NULL SET @TuantableEnabled=0

	-- У�鿪��
	IF @TuantableEnabled=0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1324,"Msg":"ת�̳齱��ѹرգ�"}'
		RETURN 4
	END
	
	-- ���ڱ�ʶ
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- �齱��Ϣ
	DECLARE @MayLotteryTimes INT
	DECLARE @UseLotteryTimes INT	

	-- ��ѯ����
	SELECT @MayLotteryTimes=MayLotteryTimes,@UseLotteryTimes=UseLotteryTimes FROM AccountsTurntableLottery(NOLOCK) WHERE DateID=@DateID AND UserID=@dwUserID

	-- ���ݲ�����
	IF @MayLotteryTimes IS NULL
	BEGIN
		RETURN 100
	END

	-- У�����
	IF @UseLotteryTimes>=@MayLotteryTimes
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1325,"Msg":"�齱�����Ѿ����꣡"}'
		RETURN 5
	END

	-- ���ڱ�ʶ
	DECLARE @DateTime DATETIME
	SET @DateTime=GETDATE()

	-- ת�̽���
	DECLARE @TableAwardItem TABLE (ItemIndex INT, ItemWeight INT, AwardName NVARCHAR(64), GoodsID SMALLINT, GoodsCount INT, GoodsLevel INT, GoodsIndate INT, DailyLimitPlaces INT, StartRandValue INT, EndRandValue INT)	

	-- ��ȡ����
	INSERT INTO @TableAwardItem(ItemIndex,ItemWeight,AwardName,GoodsID,GoodsCount,GoodsLevel,GoodsIndate,DailyLimitPlaces,StartRandValue,EndRandValue)
	SELECT ItemIndex,ItemWeight,AwardName,GoodsID,GoodsCount,GoodsLevel,GoodsIndate,DailyLimitPlaces,0,0 FROM LHPlatformDB.dbo.TurntableAwardConfig(NOLOCK)

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��ѯ������
	DECLARE @HitIndex INT
	SELECT @HitIndex=HitIndex FROM TurntableLotteryWriteList WITH(UPDLOCK)  WHERE UserID=@dwUserID
	
	IF @HitIndex IS NULL
	BEGIN
		-- ��������
		DECLARE @Index INT			
		DECLARE @RandValue INT
		DECLARE @LoopCount INT	
		DECLARE @HitTimes INT
		DECLARE @ItemIndex INT
		DECLARE @ItemWeight INT
		DECLARE @TotalWeight INT		
		DECLARE @LimitPlaces INT
		DECLARE @AddupWeight INT

		-- ��ʼ��
		SELECT @Index=0, @LoopCount=COUNT(1), @AddupWeight=0, @TotalWeight=SUM(ItemWeight) FROM @TableAwardItem

		-- ���������
		WHILE @Index<@LoopCount
		BEGIN

			SELECT @ItemIndex=ItemIndex,@ItemWeight=ItemWeight,@LimitPlaces=DailyLimitPlaces FROM @TableAwardItem WHERE ItemIndex=@Index

			UPDATE @TableAwardItem SET StartRandValue=@AddupWeight,EndRandValue=@AddupWeight+ItemWeight-1 WHERE ItemIndex=@ItemIndex			

			SET @Index=@Index+1
			SET @AddupWeight+=@ItemWeight
		END	

		-- ��������
		SELECT @Index=0, @LoopCount=100

		-- ѭ���齱
		WHILE @Index<=@LoopCount
		BEGIN

			-- ��ȡ���ֵ
			SET @RandValue=CAST(RAND()*@TotalWeight AS INT)

			-- �������
			SELECT @HitIndex=ItemIndex,@LimitPlaces=DailyLimitPlaces FROM @TableAwardItem WHERE @RandValue>=StartRandValue AND @RandValue<=EndRandValue

			-- ���д���
			SELECT @HitTimes=HitTimes FROM TurntableLotteryHitStat WHERE DateID=@DateID AND ItemIndex=@HitIndex
			IF @HitTimes IS NULL SET @HitTimes=0

			-- У������
			IF @HitTimes<@LimitPlaces OR @LimitPlaces=0
			BEGIN
				UPDATE TurntableLotteryHitStat SET HitTimes+=1 WHERE DateID=@DateID AND ItemIndex=@HitIndex	
				IF @@ROWCOUNT=0
				BEGIN
					INSERT INTO TurntableLotteryHitStat(DateID,ItemIndex,HitTimes) VALUES(@DateID,@HitIndex,1)
				END
				BREAK
			END

			SET @Index+=1
		END
	END ELSE
	BEGIN		
		-- ɾ������
		DELETE TurntableLotteryWriteList WHERE UserID=@dwUserID
	END

	-- ��Ʒ��Ϣ
	DECLARE @IncGoodsID INT	
	DECLARE @IncGoodsCount INT	
	DECLARE @IncGoodsLevel INT
	DECLARE @IncGoodsIndate INT

	-- ��ѯ��Ʒ
	SELECT @IncGoodsID=GoodsID,@IncGoodsCount=GoodsCount,@IncGoodsLevel=GoodsLevel,@IncGoodsIndate=GoodsIndate FROM @TableAwardItem WHERE ItemIndex=@HitIndex
	
	-- ����ʱ���
	DECLARE @dwTimeStamp INT = dbo.LHF_GetTimeStamp();
	
	-- д��Ƹ�
	IF dbo.LHF_IsMoneyGoods(@IncGoodsID)=1
	BEGIN		
		EXEC GSP_GP_WriteGameWealth @dwUserID,@IncGoodsID,@wReason,@IncGoodsCount
	END ELSE
	BEGIN			
		EXEC GSP_GP_AppendGoodsItem @dwUserID,@wReason,@IncGoodsID,@IncGoodsLevel,@IncGoodsCount,@IncGoodsIndate,@dwTimeStamp			
	END		
	
	-- ���´���
	UPDATE AccountsTurntableLottery SET UseLotteryTimes+=1,CollectDateTime=GetDate() WHERE DateID=@DateID AND UserID=@dwUserID

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- ��Ʒ����
	DECLARE @AwardName NVARCHAR(64)
	SELECT @AwardName=AwardName FROM @TableAwardItem WHERE ItemIndex=@HitIndex

	-- �齱��¼
	INSERT INTO LHGameRecordDB.dbo.RecordTurntableLottery(UserID,NickName,HitIndex,AwardName,GoodsID,GoodsCount,GoodsLevel,GoodsIndate,CollectDate)
	VALUES(@dwUserID,@NickName,@HitIndex,@AwardName,@IncGoodsID,@IncGoodsCount,@IncGoodsLevel,@IncGoodsIndate,GETDATE())

	-- �׳�����
	SELECT (MayLotteryTimes-UseLotteryTimes) AS ResidueLotteryTimes,UseLotteryTimes,@HitIndex AS HitIndex,@IncGoodsID AS GoodsID, @IncGoodsCount AS GoodsCount,@IncGoodsLevel AS GoodsLevel,@IncGoodsIndate AS GoodsIndate FROM AccountsTurntableLottery 
	WHERE DateID=@DateID AND UserID=@dwUserID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
