USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_BattleReward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_BattleReward]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- д�뽱��
CREATE PROC GSP_GR_BattleReward
	@dwUserID INT,								-- �û� I D	
	@lRewardGold INT,							-- �������
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT								-- ���� I D		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

DECLARE @UserGold BIGINT

-- ִ���߼�
BEGIN
	
	-- ���ƽ���
	IF @lRewardGold>0
	BEGIN
		-- �û���ѯ
		DECLARE @GameID INT                                                                                                                                                                                                         
		SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

		-- ��ѯ���			
		SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

		-- �����¼
		INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID,GameID,TypeID,BeforeGold,ChangeGold,CollectClientIP,CollectDateTime)
		VALUES (@dwUserID,@GameID,16,@UserGold,@lRewardGold,N'0.0.0.0',GETDATE())

		-- ���½��
		UPDATE LHGameGoldDB.dbo.GameScoreInfo SET Score+=@lRewardGold WHERE UserID=@dwUserID
	END
	
	-- ��ѯ���
	SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

	-- ��������
	IF @UserGold IS NULL SET @UserGold=0		

	-- �׳�����
	SELECT @UserGold AS UserGold
END

RETURN 0
GO