USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadUserRankingInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadUserRankingInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ������λ��Ϣ
CREATE PROC GSP_GP_LoadUserRankingInfo
	@dwUserID INT							-- �û���ʶ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ϵͳ��Ϣ
	DECLARE @SeasonID		SMALLINT

	-- ��λ��Ϣ
	DECLARE @RankID			INT
	DECLARE @LoveHeart		INT
	DECLARE @RankScore		INT
	
	-- �Ծ���Ϣ
	DECLARE @WinCount		INT
	DECLARE @LostCount		INT
	DECLARE @WinStreakCount	INT

	-- ��ѯ��Ϣ
	SELECT @RankScore=Score, @LoveHeart=LoveHeart, @WinCount=WinCount, @LostCount=LostCount, @WinStreakCount=MaxWinStreak
	FROM LHGameRankingDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

	-- ��ѯ����
	SELECT @RankID=RankID FROM LHGameRankingDB.dbo.UserRankingInfo(NOLOCK) WHERE UserID=@dwUserID

	-- ��������
	IF @RankID IS NULL SET @RankID=0
	IF @LoveHeart IS NULL SET @LoveHeart=3
	IF @RankScore IS NULL SET @RankScore=900
	IF @WinCount IS NULL SET @WinCount=0
	IF @LostCount IS NULL SET @LostCount=0
	IF @WinStreakCount IS NULL SET @WinStreakCount=0

	-- ϵͳ��Ϣ
	SET @SeasonID=LHPlatformDB.dbo.PFF_GetSeasonID()
	   
	 -- �������
	SELECT @SeasonID AS SeasonID, @RankID AS RankID, @RankScore AS RankScore, @LoveHeart AS LoveHeart, @WinCount AS WinCount, @LostCount AS LostCount, @WinStreakCount AS WinStreakCount

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
