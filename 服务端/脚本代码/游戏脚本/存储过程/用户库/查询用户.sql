

----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryUserID]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryUserID]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryUserInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryUserInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryMasterOrder]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryMasterOrder]
GO
----------------------------------------------------------------------------------------------------

-- 查询用户ID
CREATE PROC [dbo].[GSP_GP_QueryUserID]
	@strGameID  NVARCHAR(MAX)					--游戏ID	
WITH ENCRYPTION AS

BEGIN
	
	-- 查询用户信息
	SELECT UserID FROM AccountsInfo(NOLOCK) WHERE GameID IN (SELECT [VALUE] FROM OPENJSON(@strGameID))

END
RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- 查询用户
CREATE PROC [dbo].[GSP_GP_QueryUserInfo]
	@strUserID  NVARCHAR(MAX)					--用户ID	
WITH ENCRYPTION AS

BEGIN
	
	-- 查询用户信息
	--SELECT a.UserID,a.GameID,a.Gender,a.FaceID,a.FaceFrameID,a.ChatFrameID,a.LikedNumber,a.InfoPublic,a.CustomID,a.NickName,a.UnderWrite,a.Experience,a.MemberPoint,a.LoveLiness,ISNULL(b.Score,0) AS RankScore,a.MasterOrder 
	--FROM AccountsInfo(NOLOCK) a LEFT JOIN LHGameRankingDB.dbo.GameScoreInfo(NOLOCK) b ON a.UserID=b.UserID 
	--WHERE a.UserID IN (SELECT [VALUE] FROM OPENJSON(@strUserID))

	-- 查询用户信息
	SELECT UserID,GameID,Gender,FaceID,LikedNumber,InfoPublic,CustomID,NickName,UnderWrite,Experience,MemberPoint,LoveLiness,0 AS RankScore,MasterOrder 
	FROM AccountsInfo(NOLOCK) WHERE UserID IN (SELECT [VALUE] FROM OPENJSON(@strUserID))

END
RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- 查询等级
CREATE PROC [dbo].[GSP_GP_QueryMasterOrder]
	@dwUserID  INT								--用户ID	
WITH ENCRYPTION AS

BEGIN
	
	-- 查询用户信息
	SELECT MasterOrder FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID	

END
RETURN 0
GO

