----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UpdateUserLevel]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UpdateUserLevel]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �����û��ȼ�
CREATE PROC GSP_GP_UpdateUserLevel
	@dwUserID INT,							-- �û���ʶ			
	@dwIncPoint INT,						-- ��������		
	@dwTimeStamp INT,						-- ʱ���		
	@wReason INT,							-- ���ԭ��	
	@strErrorDescribe NVARCHAR(127) OUTPUT  -- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��ѯ�û�
	DECLARE @MemberPoint INT
	DECLARE @MemberOrder TINYINT		
	SELECT @MemberOrder=MemberOrder, @MemberPoint=MemberPoint FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- У���û�
	IF @MemberOrder IS NULL 
	BEGIN
		SET @strErrorDescribe=N'ϵͳδ��ѯ�������û���Ϣ�����֤���ٴγ��ԣ�'
		RETURN 1
	END	

	-- ���µ���
	SET @MemberPoint+=@dwIncPoint

	-- ��������
	DECLARE @MaxNeedExp INT
	DECLARE @MaxMemberOrder TINYINT
	DECLARE @NewMemberOrder TINYINT

	-- ��ѯ����
	SELECT TOP 1 @MaxNeedExp=NeedPoint,@MaxMemberOrder=LevelID FROM LHPlatformDB.dbo.GameUserLevel ORDER BY LevelID DESC

	-- У�����
	IF @MemberPoint>=@MaxNeedExp
	BEGIN
		SET @NewMemberOrder=@MaxMemberOrder
	END ELSE 
	BEGIN
		SELECT TOP 1 @NewMemberOrder=LevelID-1 FROM LHPlatformDB.dbo.GameUserLevel WHERE NeedPoint>@MemberPoint
	END

	-- �����ȼ�
	IF @NewMemberOrder IS NULL SET @NewMemberOrder=@MemberPoint

	-- ������Ʒ
	DECLARE @UpgradeReward NVARCHAR(MAX)	

	-- ��ʼ��
	SET @UpgradeReward=N''

	-- ��������
	IF @NewMemberOrder>@MemberOrder
	BEGIN
		-- ��������
		DECLARE @Index INT
		DECLARE @LoopCount INT	
		DECLARE @ResidueCount INT	

		-- ��Ʒ��Ϣ
		DECLARE @IncGoodsID INT
		DECLARE @IncGoodsLevel INT
		DECLARE @IncGoodsCount INT
		DECLARE @IncGoodsIndate INT		

		-- ���������			
		DECLARE @TableRewardGoodsList TABLE (ID INT identity(1,1),GoodsID SMALLINT,GoodsLevel SMALLINT, GoodsCount INT, ResidueCount INT, GoodsIndate INT)		

		-- �ϲ�����
		SELECT @UpgradeReward+=SUBSTRING(Rewards,2,LEN(Rewards)-2)+N',' FROM LHPlatformDB.dbo.GameUserLevel WHERE LevelID>@MemberOrder AND LevelID<=@NewMemberOrder	

		-- ��ʽ��
		IF LEN(@UpgradeReward)>0 SET @UpgradeReward=N'['+SUBSTRING(@UpgradeReward,1,LEN(@UpgradeReward)-1)+N']'

		-- �����¼
		IF ISJSON(@UpgradeReward)>0
		BEGIN
			INSERT @TableRewardGoodsList (GoodsID,GoodsLevel,GoodsCount,ResidueCount,GoodsIndate)
			SELECT JSON_VALUE(VALUE,'$.GoodsID'), MIN(CONVERT(INT,ISNULL(JSON_VALUE(VALUE,'$.Level'),1))),SUM(CONVERT(INT,JSON_VALUE(VALUE,'$.Count'))),0,SUM(CONVERT(INT,ISNULL(JSON_VALUE(VALUE,'$.Indate'),0))) 
			FROM OPENJSON(@UpgradeReward) GROUP BY JSON_VALUE(VALUE,'$.GoodsID')
		END

		-- ��ʼ����
		SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
		BEGIN TRAN
	
		-- ������Ʒ
		SELECT @Index=1,@LoopCount=COUNT(1) FROM @TableRewardGoodsList
	
		-- ѭ������
		WHILE @Index<=@LoopCount
		BEGIN	
			-- ��ѯ��Ϣ
			SELECT @IncGoodsID=GoodsID,@IncGoodsLevel=GoodsLevel,@IncGoodsCount=GoodsCount,@IncGoodsIndate=GoodsIndate FROM @TableRewardGoodsList WHERE ID=@Index		

			-- У����Ʒ����
			IF dbo.LHF_IsMoneyGoods(@IncGoodsID) = 1
			BEGIN
				-- д��Ƹ�
				EXEC GSP_GP_WriteGameWealth @dwUserID,@IncGoodsID,@wReason,@IncGoodsCount

			END ELSE
			BEGIN
				-- �����Ʒ
				EXEC @ResidueCount = dbo.GSP_GP_AppendGoodsItem @dwUserID,@wReason,@IncGoodsID,@IncGoodsLevel,@IncGoodsCount,@IncGoodsIndate,@dwTimeStamp

				-- ʣ�ദ��
				IF @ResidueCount>0
				BEGIN
					-- ����ʣ��
					UPDATE @TableRewardGoodsList SET ResidueCount=@ResidueCount WHERE ID=@Index				
				END		
			END
		
			-- ��������
			SET @Index=@Index+1
		END	

		-- ���µ���
		UPDATE AccountsInfo SET MemberOrder=@NewMemberOrder,MemberPoint=@MemberPoint WHERE UserID=@dwUserID

		-- ��������
		COMMIT TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	END	ELSE
	BEGIN
		-- ���µ�ֵ
		UPDATE AccountsInfo SET MemberOrder=@NewMemberOrder,MemberPoint=@MemberPoint WHERE UserID=@dwUserID
	END

	-- ʣ����Ʒ
	DECLARE @RewardGoodsList NVARCHAR(MAX)	

	-- ����JSON
	SET @RewardGoodsList=(SELECT GoodsID, GoodsLevel AS [Level], GoodsCount AS [Count], GoodsIndate AS [Indate] FROM @TableRewardGoodsList FOR JSON AUTO)	

	-- ��������	
	IF @RewardGoodsList IS NULL SET @RewardGoodsList=N''

	-- �׳���¼
	SELECT @RewardGoodsList AS RewardGoodsList, @MemberPoint AS MemberPoint, @MemberOrder AS LastMemberOrder, @NewMemberOrder AS CurrMemberOrder
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------