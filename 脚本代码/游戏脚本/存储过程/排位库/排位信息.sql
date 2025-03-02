
----------------------------------------------------------------------------------------------------

USE LHGameRankingDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadRankingInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadRankingInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE  PROCEDURE dbo.GSP_GR_LoadRankingInfo	
	@dwUserID INT,						   -- �û���ʶ
	@strErrorDescribe NVARCHAR(128) OUTPUT -- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- �û���Ϣ
	DECLARE @FaceID INT		
	DECLARE @Gender TINYINT
	DECLARE @CustomID INT
	DECLARE @Gold INT
	DECLARE @NickName NVARCHAR(32)

	-- ��ѯ�û�
	SELECT @FaceID=FaceID,@CustomID=CustomID,@Gender=Gender,@NickName=NickName FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
	SELECT @Gold=Score FROM LHAccountsDB.dbo.AccountsGoldInfo(NOLOCK) WHERE UserID=@dwUserID
	
	-- У���û�
	IF @FaceID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
		RETURN 1
	END

	-- ��λ��Ϣ
	DECLARE @LoveHeart	INT
	DECLARE @RankScore	INT	
	DECLARE @RankOrder  INT

	-- ��ѯ��Ϣ
	SELECT @RankScore=Score,@LoveHeart=LoveHeart FROM GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

	-- ����ȼ�
	SET @RankOrder=LHPlatformDB.dbo.PFF_CalcRankOrder(@RankScore)

	-- �׳�����
	SELECT @Gender AS Gender, @FaceID AS FaceID, @CustomID AS CustomID, @Gold AS Gold, @NickName AS NickName, @LoveHeart AS LoveHeart, @RankScore AS RankScore, @RankOrder AS RankOrder
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
