
----------------------------------------------------------------------
-- 时间：2021-11-01
-- 用途：缓存排行榜
----------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WEB_GR_CacheCharts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[WEB_GR_CacheCharts]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

---------------------------------------------------------------------------
--	缓存排行榜
CREATE  PROCEDURE WEB_GR_CacheCharts
	@dwTypeID		INT,			-- 榜单类型（1、充值排行 2、黄金排行  3、经验排行  4、魅力排行  5、约战排行）
	@dwTopN			INT				-- 取前 N 名
WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

-- 最大Top
DECLARE @MaxTopN INT

BEGIN
	SET @MaxTopN=1000
	IF @dwTopN>@MaxTopN SET @dwTopN=@MaxTopN

	-- 设置日期
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 更新榜单时间
	UPDATE LHGameRecordDB.dbo.RankingListVersion SET ModifyDateTime=GETDATE() WHERE TypeID=@dwTypeID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO LHGameRecordDB.dbo.RankingListVersion(TypeID) VALUES(@dwTypeID)
	END
	
	-- 删除数据
	DELETE LHGameRecordDB.dbo.RankingListRange WHERE TypeID=@dwTypeID AND DateID=@DateID
	DELETE LHGameRecordDB.dbo.RankingListRange WHERE TypeID=@dwTypeID AND DateID=0

	-- 表变量定义
	DECLARE @TableRank TABLE (RankID INT,DateID INT,UserID INT,Score BIGINT,GameID INT,NickName NVARCHAR(32),Gender TINYINT,FaceID SMALLINT,CustomID INT,ActorID SMALLINT,FaceFrameID SMALLINT,VipPoint INT)

	-- 插入数据

	-- 充值排行数据
	IF @dwTypeID=1
	BEGIN
		-- 查询当天数据
		INSERT INTO @TableRank
		SELECT TOP(@dwTopN)	ROW_NUMBER() OVER (ORDER BY PayAmount DESC) AS RankID,@DateID AS DateID,A.UserID,A.PayAmount*1000 AS Score,B.GameID,B.NickName,B.Gender,B.FaceID,B.CustomID,B.ActorID,B.FaceFrameID,B.MemberPoint AS VipPoint 
		FROM LHGameRecordDB.dbo.ReportDayUser(NOLOCK) A LEFT JOIN AccountsInfo(NOLOCK) B ON A.UserID=B.UserID WHERE A.DateID=@DateID AND A.PayAmount>0 AND B.IsRobot=0 

		-- 查询全部数据
		INSERT INTO @TableRank
		SELECT TOP(@dwTopN)	ROW_NUMBER() OVER (ORDER BY PayAmount DESC) AS RankID,0 AS DateID,A.UserID,PayAmount*1000 AS Score,B.GameID,B.NickName,B.Gender,B.FaceID,B.CustomID,B.ActorID,B.FaceFrameID,B.MemberPoint AS VipPoint 
		FROM (SELECT UserID,SUM(PayAmount) AS PayAmount FROM LHGameRecordDB.dbo.ReportDayUser(NOLOCK) WHERE PayAmount>0 GROUP BY UserID) A LEFT JOIN AccountsInfo(NOLOCK) B ON A.UserID=B.UserID WHERE PayAmount<>0 AND B.IsRobot=0 

		-- 插入数据
		INSERT INTO LHGameRecordDB.dbo.RankingListRange(TypeID,DateID,UserID,GameID,NickName,Gender,FaceID,CustomID,ActorID,FaceFrameID,VipPoint,RankID,Score)
		SELECT @dwTypeID,DateID,UserID,GameID,NickName,Gender,FaceID,CustomID,ActorID,FaceFrameID,VipPoint,RankID,Score FROM @TableRank
	END

	-- 黄金排行数据
	IF @dwTypeID=2
	BEGIN
		-- 查询数据
		INSERT INTO @TableRank
		SELECT TOP(@dwTopN)	ROW_NUMBER() OVER (ORDER BY Score DESC) AS RankID,0 AS DateID,A.UserID,A.Score,B.GameID,B.NickName,B.Gender,B.FaceID,B.CustomID,B.ActorID,B.FaceFrameID,B.MemberPoint AS VipPoint 
		FROM AccountsGoldInfo(NOLOCK) A LEFT JOIN AccountsInfo(NOLOCK) B ON A.UserID=B.UserID WHERE B.IsRobot=0

		-- 插入数据
		INSERT INTO LHGameRecordDB.dbo.RankingListRange(TypeID,DateID,UserID,GameID,NickName,Gender,FaceID,CustomID,ActorID,FaceFrameID,VipPoint,RankID,Score)
		SELECT @dwTypeID,DateID,UserID,GameID,NickName,Gender,FaceID,CustomID,ActorID,FaceFrameID,VipPoint,RankID,Score FROM @TableRank
	END

	-- 经验排行数据
	IF @dwTypeID=3
	BEGIN
		-- 查询当天数据
		INSERT INTO @TableRank
		SELECT TOP(@dwTopN)	ROW_NUMBER() OVER (ORDER BY A.Experience DESC) AS RankID,@DateID AS DateID,A.UserID,A.Experience AS Score,B.GameID,B.NickName,B.Gender,B.FaceID,B.CustomID,B.ActorID,B.FaceFrameID,B.MemberPoint AS VipPoint 
		FROM LHGameRecordDB.dbo.ReportDayUser(NOLOCK) A LEFT JOIN AccountsInfo(NOLOCK) B ON A.UserID=B.UserID WHERE A.DateID=@DateID AND A.Experience>0 AND B.IsRobot=0 

		-- 查询全部数据
		INSERT INTO @TableRank
		SELECT TOP(@dwTopN)	ROW_NUMBER() OVER (ORDER BY Experience DESC) AS RankID,0 AS DateID,UserID,Experience AS Score,GameID,NickName,Gender,FaceID,CustomID,ActorID,FaceFrameID,MemberPoint AS VipPoint
		FROM AccountsInfo(NOLOCK) WHERE IsRobot=0

		-- 插入数据
		INSERT INTO LHGameRecordDB.dbo.RankingListRange(TypeID,DateID,UserID,GameID,NickName,Gender,FaceID,CustomID,ActorID,FaceFrameID,VipPoint,RankID,Score)
		SELECT @dwTypeID,DateID,UserID,GameID,NickName,Gender,FaceID,CustomID,ActorID,FaceFrameID,VipPoint,RankID,Score FROM @TableRank
	END

	-- 魅力排行数据
	IF @dwTypeID=4
	BEGIN
		-- 查询当天数据
		INSERT INTO @TableRank
		SELECT TOP(@dwTopN)	ROW_NUMBER() OVER (ORDER BY A.Loveliness DESC) AS RankID,@DateID AS DateID,A.UserID,A.Loveliness AS Score,B.GameID,B.NickName,B.Gender,B.FaceID,B.CustomID,B.ActorID,B.FaceFrameID,B.MemberPoint AS VipPoint 
		FROM LHGameRecordDB.dbo.ReportDayUser(NOLOCK) A LEFT JOIN AccountsInfo(NOLOCK) B ON A.UserID=B.UserID WHERE A.DateID=@DateID AND A.Loveliness>0 AND B.IsRobot=0 

		-- 查询全部数据
		INSERT INTO @TableRank
		SELECT TOP(@dwTopN)	ROW_NUMBER() OVER (ORDER BY Loveliness DESC) AS RankID,0 AS DateID,UserID,Loveliness AS Score,GameID,NickName,Gender,FaceID,CustomID,ActorID,FaceFrameID,MemberPoint AS VipPoint
		FROM AccountsInfo(NOLOCK) WHERE IsRobot=0

		-- 插入数据
		INSERT INTO LHGameRecordDB.dbo.RankingListRange(TypeID,DateID,UserID,GameID,NickName,Gender,FaceID,CustomID,ActorID,FaceFrameID,VipPoint,RankID,Score)
		SELECT @dwTypeID,DateID,UserID,GameID,NickName,Gender,FaceID,CustomID,ActorID,FaceFrameID,VipPoint,RankID,Score FROM @TableRank
	END

	-- 约战排行数据
	IF @dwTypeID=5
	BEGIN
		-- 查询当天数据
		INSERT INTO @TableRank
		SELECT TOP(@dwTopN)	ROW_NUMBER() OVER (ORDER BY GainScore DESC) AS RankID,@DateID AS DateID,A.UserID,GainScore AS Score,B.GameID,B.NickName,B.Gender,B.FaceID,B.CustomID,B.ActorID,B.FaceFrameID,B.MemberPoint AS VipPoint 
		FROM LHGameBattleDB.dbo.ReportUserScore(NOLOCK) A LEFT JOIN AccountsInfo(NOLOCK) B ON A.UserID=B.UserID WHERE A.DateID=@DateID AND B.IsRobot=0 

		-- 查询全部数据
		INSERT INTO @TableRank
		SELECT TOP(@dwTopN)	ROW_NUMBER() OVER (ORDER BY WinScore DESC) AS RankID,0 AS DateID,A.UserID,WinScore AS Score,B.GameID,B.NickName,B.Gender,B.FaceID,B.CustomID,B.ActorID,B.FaceFrameID,B.MemberPoint AS VipPoint 
		FROM LHGameBattleDB.dbo.GameScoreInfo(NOLOCK) A LEFT JOIN AccountsInfo(NOLOCK) B ON A.UserID=B.UserID WHERE B.IsRobot=0

		-- 插入数据
		INSERT INTO LHGameRecordDB.dbo.RankingListRange(TypeID,DateID,UserID,GameID,NickName,Gender,FaceID,CustomID,ActorID,FaceFrameID,VipPoint,RankID,Score)
		SELECT @dwTypeID,DateID,UserID,GameID,NickName,Gender,FaceID,CustomID,ActorID,FaceFrameID,VipPoint,RankID,Score FROM @TableRank
	END
END

SET NOCOUNT OFF

RETURN 0
GO
