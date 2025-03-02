
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

-- 加载配置
CREATE  PROCEDURE dbo.GSP_GR_LoadRankingInfo	
	@dwUserID INT,						   -- 用户标识
	@strErrorDescribe NVARCHAR(128) OUTPUT -- 错误描述
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 用户信息
	DECLARE @FaceID INT		
	DECLARE @Gender TINYINT
	DECLARE @CustomID INT
	DECLARE @Gold INT
	DECLARE @NickName NVARCHAR(32)

	-- 查询用户
	SELECT @FaceID=FaceID,@CustomID=CustomID,@Gender=Gender,@NickName=NickName FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
	SELECT @Gold=Score FROM LHAccountsDB.dbo.AccountsGoldInfo(NOLOCK) WHERE UserID=@dwUserID
	
	-- 校验用户
	IF @FaceID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 1
	END

	-- 排位信息
	DECLARE @LoveHeart	INT
	DECLARE @RankScore	INT	
	DECLARE @RankOrder  INT

	-- 查询信息
	SELECT @RankScore=Score,@LoveHeart=LoveHeart FROM GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 计算等级
	SET @RankOrder=LHPlatformDB.dbo.PFF_CalcRankOrder(@RankScore)

	-- 抛出数据
	SELECT @Gender AS Gender, @FaceID AS FaceID, @CustomID AS CustomID, @Gold AS Gold, @NickName AS NickName, @LoveHeart AS LoveHeart, @RankScore AS RankScore, @RankOrder AS RankOrder
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
