----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LHF_GetCheckInData]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[LHF_GetCheckInData]
GO


IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_EfficacyCheckIn]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_EfficacyCheckIn]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


---------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------

-- ��ȡʱ���
CREATE FUNCTION [dbo].[LHF_GetCheckInData]
(	
	@dwUserID INT						-- �û���ʶ		
)
RETURNS @Result TABLE (CheckInDays SMALLINT, CheckInState TINYINT)
AS
BEGIN
	
	-- ��������
	DECLARE @CheckInDays SMALLINT
	DECLARE @CheckInState TINYINT
	DECLARE @LastCheckInTime DATETIME

	-- ��ѯǩ����Ϣ
	SELECT @CheckInDays=CheckInDays,@LastCheckInTime=LastCheckInTime FROM AccountsCheckIn(NOLOCK) WHERE UserID=@dwUserID
	
	-- У����
	IF @CheckInDays IS NULL
	BEGIN
		-- ��������
		SELECT @CheckInDays=0,@CheckInState=0

	END ELSE
	BEGIN
		-- ǩ�����
		DECLARE @CheckInDiff INT
		SET @CheckInDiff=DATEDIFF(day,@LastCheckInTime,GETDATE())
		
		-- ����״̬
		SET @CheckInState=0
		IF @CheckInDiff=0 SET @CheckInState=1

		-- ��ͷ����
		IF (@CheckInDays=7 AND @CheckInDiff=1) OR (@CheckInDiff>1)
		BEGIN
			-- ��������
			SET @CheckInDays=0
		END
	END

	-- ��������
	INSERT INTO @Result(CheckInDays,CheckInState) VALUES (@CheckInDays,@CheckInState)

	RETURN
END

GO

---------------------------------------------------------------------------------------------------

-- �û�ǩ��
CREATE PROC GSP_MB_EfficacyCheckIn
	@dwUserID INT,							-- �û���ʶ
	@cbQuery TINYINT,						-- ��ѯ��ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT  -- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �ع�����
SET XACT_ABORT ON

-- ִ���߼�
BEGIN
	
	-- ��ѯ�û�
	DECLARE @UserID INT
	DECLARE @GameID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	DECLARE @NickName NVARCHAR(64)
	DECLARE @MemberOrder SMALLINT
	SELECT @UserID=a.UserID,@GameID=a.GameID,@NickName=a.NickName,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus,@MemberOrder=a.MemberOrder
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
	--IF @MemberOrder=0
	--BEGIN
	--	SET @strErrorDescribe=N'{"Idx":1141,"Msg":"ǩ�����ܽ���VIP��Աʹ�ã�"} '
	--	RETURN 4
	--END

	-- ���ö���
	DECLARE @CheckInEnabled TINYINT
	DECLARE @CheckInRewards NVARCHAR(MAX)

	-- ��ȡ����
	SELECT @CheckInRewards=StatusValue FROM LHPlatformDB.dbo.SystemStatusInfo WHERE StatusKind=N'CheckInOption' AND StatusName=N'CheckInRewards'
	SELECT @CheckInEnabled=CONVERT(TINYINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo WHERE StatusKind=N'CheckInOption' AND StatusName=N'CheckInEnabled'

	-- У��״̬
	IF @CheckInEnabled=0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1138,"Msg":"��Ǹ��ǩ����������ʱ�رգ�"} '
		RETURN 4
	END
	
	-- ǩ����Ϣ
	DECLARE @CheckInDays INT
	DECLARE @CheckInState TINYINT
	DECLARE @LastCheckInTime DATETIME
	
	-- ��ѯ��Ϣ
	SELECT @CheckInDays=CheckInDays,@LastCheckInTime=LastCheckInTime FROM AccountsCheckIn(NOLOCK) WHERE UserID=@UserID	

	-- �״�ǩ��
	IF @CheckInDays IS NULL
	BEGIN
		SELECT @CheckInDays=0,@CheckInState=0
	END ELSE
	BEGIN
		-- ǩ�����
		DECLARE @CheckInDiff INT
		SET @CheckInDiff=DATEDIFF(day,@LastCheckInTime,GETDATE())
		
		-- ����״̬
		SET @CheckInState=0
		IF @CheckInDiff=0 SET @CheckInState=1

		-- ��ͷ����
		IF (@CheckInDays=7 AND @CheckInDiff=1) OR (@CheckInDays>0 AND @CheckInDiff>1)
		BEGIN
			-- ��������
			SET @CheckInDays=0					
		END
	END

	--����ѯ
	IF @cbQuery > 0
	BEGIN
		SELECT @CheckInDays AS CheckInDays, @CheckInState AS CheckInState, 0 AS RewardGoodsID, 0 AS RewardAmount
		RETURN 0
	END

	-- ״̬У��
	IF @CheckInState=1
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1139,"Msg":"�������Ѿ�ǩ����ÿ�����ǩ��һ�Σ�"} '
		RETURN 5
	END		

	-- ���������
	DECLARE @TableCheckInRewards TABLE (DayID SMALLINT,Rewards NVARCHAR(MAX))	
	DECLARE @TableRewardsItem TABLE (ItemIndex INT PRIMARY KEY IDENTITY(1,1),GoodsID SMALLINT,Amount BIGINT,[Weight] INT, StartRandValue INT, EndRandValue INT)

	-- ��������
	INSERT INTO @TableCheckInRewards(DayID, Rewards)
	SELECT JSON_VALUE(VALUE,N'$.DayID'), JSON_QUERY(VALUE,N'$.Rewards') FROM OPENJSON(@CheckInRewards)

	-- ���±���
	SET @CheckInState=1
	SET @CheckInDays+=1

	-- ��������
	DECLARE @RewardsOption NVARCHAR(MAX)
	SELECT @RewardsOption=Rewards FROM @TableCheckInRewards WHERE DayID=@CheckInDays

	-- У������
	IF @RewardsOption IS NULL OR ISJSON(@RewardsOption)=0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1140,"Msg":"ϵͳ�쳣��ǩ��ʧ�ܣ�����ϵ�ͷ��˽���ϸ�����"} '
		RETURN 6
	END

	-- ��������
	INSERT INTO @TableRewardsItem(GoodsID,Amount,[Weight],StartRandValue,EndRandValue)
	SELECT  JSON_VALUE(VALUE,N'$.GoodsID'), JSON_VALUE(VALUE,N'$.Amount'), JSON_VALUE(VALUE,N'$.Weight'),0,0 FROM OPENJSON(@RewardsOption)

	-- ��������
	DECLARE @ItemIndex INT	
	DECLARE @ItemWeight INT
	DECLARE @LoopCount INT
	DECLARE @TotalWeight INT		
	DECLARE @LimitPlaces INT
	DECLARE @AddupWeight INT
	
	-- ��ʼ��
	SELECT @ItemIndex=1,@LoopCount=COUNT(1), @AddupWeight=0, @TotalWeight=SUM([Weight]) FROM @TableRewardsItem

	-- ���������
	WHILE @ItemIndex<=@LoopCount
	BEGIN
		-- ��ѯȨ��
		SELECT @ItemWeight=[Weight] FROM @TableRewardsItem WHERE ItemIndex=@ItemIndex

		-- �����������
		UPDATE @TableRewardsItem SET StartRandValue=@AddupWeight,EndRandValue=@AddupWeight+@ItemWeight-1 WHERE ItemIndex=@ItemIndex			

		-- ���ñ���
		SET @ItemIndex+=1
		SET @AddupWeight+=@ItemWeight
	END	

	-- ��������
	DECLARE @HitIndex INT
	DECLARE @RandValue INT
	DECLARE @RewardAmount BIGINT
	DECLARE @RewardGoodsID SMALLINT

	-- ��ȡ���ֵ
	SET @RandValue=CAST(RAND()*@TotalWeight AS INT)

	-- ѡȡ����
	SELECT @HitIndex=ItemIndex,@RewardGoodsID=GoodsID, @RewardAmount=Amount FROM @TableRewardsItem WHERE @RandValue>=StartRandValue AND @RandValue<=EndRandValue

	-- �������
	IF @HitIndex IS NULL
	BEGIN
		SET @HitIndex=1 
		SELECT @RewardAmount=Amount,@RewardGoodsID=GoodsID FROM @TableRewardsItem WHERE ItemIndex=@HitIndex
	END
	
	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ���²Ƹ�
	EXEC GSP_GP_WriteGameWealth @dwUserID,@RewardGoodsID,25,@RewardAmount
		
	-- ������Ϣ
	UPDATE AccountsCheckIn SET CheckInDays=@CheckInDays,LastCheckInTime=GetDate() WHERE UserID=@dwUserID	
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO AccountsCheckIn(UserID,CheckInDays,LastCheckInTime,CollectDateTime) VALUES (@dwUserID,@CheckInDays,GETDATE(),GETDATE())
	END

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- ǩ����¼
	INSERT INTO LHGameRecordDB.dbo.RecordDailyCheckIn(UserID,NickName,GoodsID,Amount,CollectDate) 
	VALUES(@dwUserID,@NickName,@RewardGoodsID,@RewardAmount,GETDATE())

	-- �׳�����
	SELECT @CheckInDays AS CheckInDays, @CheckInState AS CheckInState, @RewardGoodsID AS RewardGoodsID, @RewardAmount AS RewardAmount
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------