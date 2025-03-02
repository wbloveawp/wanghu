USE LHAccountsDB
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
	@dwRewardMedal INT,							-- ��������
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D	
	@strClientIP NVARCHAR(15)					-- ���ӵ�ַ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	---- ���ƽ���
	--IF @dwRewardMedal>0
	--BEGIN
	--	-- ��ѯ����
	--	DECLARE @UserMedal INT 
	--	SELECT @UserMedal=UserMedal FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID		
			
	--	-- �û�������
	--	IF 	@UserMedal IS NULL RETURN 1

	--	-- д���¼
	--	INSERT INTO RecordMedalChange(UserID,SrcMedal,TradeMedal,TypeID,ClientIP) VALUES(@dwUserID,@UserMedal,@dwRewardMedal,1,@strClientIP)

	--	-- ���½���
	--	UPDATE AccountsInfo SET UserMedal=UserMedal+@dwRewardMedal WHERE UserID=@dwUserID	
		
	--	-- ͳ�ƽ���
	--	DECLARE @DateID	INT
	--	SET @DateID = CAST(CAST(GETDATE() AS FLOAT) AS INT)	
	--	UPDATE StreamMedalInfo SET RewardMedal=RewardMedal+@dwRewardMedal,LastCollectDate=GETDATE() WHERE DateID=@DateID AND UserID=@dwUserID AND KindID=@wKindID
	--	IF @@ROWCOUNT=0
	--	BEGIN
	--		INSERT INTO StreamMedalInfo(DateID,UserID,KindID,RewardMedal) VALUES(@DateID,@dwUserID,@wKindID,@dwRewardMedal)
	--	END 
	--END
	
	---- ��ѯ����
	--DECLARE @CurrMedal BIGINT	
	--SELECT @CurrMedal=UserMedal FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID		

	---- ��������
	--IF @CurrMedal IS NULL SET @CurrMedal=0		

	---- �׳�����
	--SELECT @CurrMedal AS CurrMedal

	RETURN 0
END

RETURN 0
GO
