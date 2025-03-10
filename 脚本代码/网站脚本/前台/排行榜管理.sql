
----------------------------------------------------------------------
-- 时间：2021-11-01
-- 用途：缓存排行榜
----------------------------------------------------------------------

USE LHGameRecordDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WEB_GR_GetCharts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[WEB_GR_GetCharts]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WEB_GR_GetKindCharts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[WEB_GR_GetKindCharts]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WEB_GR_GetInviteCharts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[WEB_GR_GetInviteCharts]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WEB_GR_GetInviteChartsAward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[WEB_GR_GetInviteChartsAward]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

---------------------------------------------------------------------------
--	用户榜单
CREATE  PROCEDURE WEB_GR_GetCharts
	@dwTypeID		INT,					-- 榜单类型（1、充值排行 2、黄金排行  3、经验排行  4、魅力排行  5、约战排行）
	@dwSDateID		INT,					-- 开始时间
	@dwEDateID		INT,					-- 结束时间
	@dwTopN			INT,					-- 取前 N 名
	@dwUserID	INT,						-- 用户标识
	@strErrorDescribe NVARCHAR(127) OUTPUT	-- 输出信息
WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

-- 账户信息
DECLARE @UserID INT
DECLARE @Nullity TINYINT
DECLARE @GameID INT
DECLARE @NickName NVARCHAR(32)
DECLARE @Gender TINYINT
DECLARE @FaceID SMALLINT
DECLARE @CustomID INT
DECLARE @ActorID SMALLINT
DECLARE @FaceFrameID SMALLINT
DECLARE @VipPoint INT

-- 缓存信息
DECLARE @RankID INT
DECLARE @Score BIGINT

-- 最大Top
DECLARE @MaxTopN INT

BEGIN
	SET @MaxTopN=10000
	IF @dwTopN>@MaxTopN SET @dwTopN=@MaxTopN

	-- 验证用户
	SELECT @UserID=UserID,@Nullity=Nullity,@GameID=GameID,@NickName=NickName,@Gender=Gender,@FaceID=FaceID,@CustomID=CustomID,@ActorID=ActorID,@FaceFrameID=FaceFrameID,@VipPoint=MemberPoint
	FROM LHAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 101
	END
	
	-- 判断用户是否存在榜单
	IF @dwSDateID=0
	BEGIN
		SELECT @RankID=RankID,@Score=Score FROM RankingListRange WHERE TypeID=@dwTypeID AND DateID=0 AND UserID=@dwUserID
		IF @RankID IS NOT NULL
		BEGIN
			SELECT @RankID AS RankID,@UserID AS UserID,@GameID AS GameID,@NickName AS NickName,@Gender AS Gender,@FaceID AS FaceID,@CustomID AS CustomID,@ActorID AS ActorID,@FaceFrameID AS FaceFrameID,@VipPoint AS VipPoint,@Score AS Score
			UNION ALL
			SELECT * FROM (
			SELECT TOP (@dwTopN) RankID,UserID,GameID,NickName,Gender,FaceID,CustomID,ActorID,FaceFrameID,VipPoint,Score FROM RankingListRange(NOLOCK) WHERE TypeID=@dwTypeID AND DateID=0 ORDER BY RankID ASC) A
		END
		ELSE
		BEGIN
			SELECT 0 AS RankID,@UserID AS UserID,@GameID AS GameID,@NickName AS NickName,@Gender AS Gender,@FaceID AS FaceID,@CustomID AS CustomID,@ActorID AS ActorID,@FaceFrameID AS FaceFrameID,@VipPoint AS VipPoint,0 AS Score
			UNION ALL
			SELECT * FROM (
			SELECT TOP (@dwTopN) RankID,UserID,GameID,NickName,Gender,FaceID,CustomID,ActorID,FaceFrameID,VipPoint,Score FROM RankingListRange(NOLOCK) WHERE TypeID=@dwTypeID AND DateID=0 ORDER BY RankID ASC) A
		END	
	END
	ELSE
	BEGIN
		SELECT @Score=SUM(Score) FROM RankingListRange WHERE TypeID=@dwTypeID AND DateID>=@dwSDateID AND DateID<=@dwEDateID AND UserID=@dwUserID
		IF @Score IS NOT NULL
		BEGIN
			SELECT * FROM (
			SELECT TOP (@dwTopN) ROW_NUMBER() OVER (ORDER BY Score DESC) AS RankID,A.UserID,B.GameID,B.NickName,B.Gender,B.FaceID,B.CustomID,B.ActorID,B.FaceFrameID,B.MemberPoint AS VipPoint,A.Score
			FROM (SELECT UserID,SUM(Score) AS Score FROM RankingListRange(NOLOCK) WHERE TypeID=@dwTypeID AND DateID>=@dwSDateID AND DateID<=@dwEDateID GROUP BY UserID) A  
			LEFT JOIN LHAccountsDB.dbo.AccountsInfo(NOLOCK) B ON A.UserID=B.UserID) C WHERE C.UserID=@dwUserID
			UNION ALL
			SELECT * FROM (
			SELECT TOP (@dwTopN) ROW_NUMBER() OVER (ORDER BY Score DESC) AS RankID,A.UserID,B.GameID,B.NickName,B.Gender,B.FaceID,B.CustomID,B.ActorID,B.FaceFrameID,B.MemberPoint AS VipPoint,A.Score
			FROM (SELECT UserID,SUM(Score) AS Score FROM RankingListRange(NOLOCK) WHERE TypeID=@dwTypeID AND DateID>=@dwSDateID AND DateID<=@dwEDateID GROUP BY UserID) A  
			LEFT JOIN LHAccountsDB.dbo.AccountsInfo(NOLOCK) B ON A.UserID=B.UserID) D
		END
		ELSE
		BEGIN
			SELECT 0 AS RankID,@UserID AS UserID,@GameID AS GameID,@NickName AS NickName,@Gender AS Gender,@FaceID AS FaceID,@CustomID AS CustomID,@ActorID AS ActorID,@FaceFrameID AS FaceFrameID,@VipPoint AS VipPoint,0 AS Score
			UNION ALL
			SELECT * FROM (
			SELECT TOP (@dwTopN) ROW_NUMBER() OVER (ORDER BY Score DESC) AS RankID,A.UserID,B.GameID,B.NickName,B.Gender,B.FaceID,B.CustomID,B.ActorID,B.FaceFrameID,B.MemberPoint AS VipPoint,A.Score
			FROM (SELECT UserID,SUM(Score) AS Score FROM RankingListRange(NOLOCK) WHERE TypeID=@dwTypeID AND DateID>=@dwSDateID AND DateID<=@dwEDateID GROUP BY UserID) A  
			LEFT JOIN LHAccountsDB.dbo.AccountsInfo(NOLOCK) B ON A.UserID=B.UserID) D
		END
	END	
END

SET NOCOUNT OFF

RETURN 0
GO

---------------------------------------------------------------------------
--	游戏榜单
CREATE  PROCEDURE WEB_GR_GetKindCharts
	@dtSDate		DATETIME,				-- 开始时间
	@dtEDate		DATETIME,				-- 结束时间
	@dwKindID		INT,					-- 游戏标识
	@dwTopN			INT,					-- 取前 N 名
	@dwUserID		INT,					-- 用户标识
	@strErrorDescribe NVARCHAR(127) OUTPUT	-- 输出信息
WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

-- 账户信息
DECLARE @UserID INT
DECLARE @Nullity TINYINT
DECLARE @GameID INT
DECLARE @NickName NVARCHAR(32)
DECLARE @Gender TINYINT
DECLARE @FaceID SMALLINT
DECLARE @CustomID INT
DECLARE @ActorID SMALLINT
DECLARE @FaceFrameID SMALLINT
DECLARE @VipPoint INT

-- 缓存信息
DECLARE @RankID INT
DECLARE @Score BIGINT

-- 最大Top
DECLARE @MaxTopN INT

-- 时间信息
DECLARE @SDateID INT
DECLARE @EDateID INT

BEGIN
	SET @MaxTopN=10000
	IF @dwTopN>@MaxTopN SET @dwTopN=@MaxTopN

	-- 设置时间
	SET @SDateID=CAST(CAST(@dtSDate AS FLOAT) AS INT)
	SET @EDateID=CAST(CAST(@dtEDate AS FLOAT) AS INT)-1

	-- 验证用户
	SELECT @UserID=UserID,@Nullity=Nullity,@GameID=GameID,@NickName=NickName,@Gender=Gender,@FaceID=FaceID,@CustomID=CustomID,@ActorID=ActorID,@FaceFrameID=FaceFrameID,@VipPoint=MemberPoint
	FROM LHAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 101
	END

	-- 表变量定义
	DECLARE @TableRank TABLE (RankID INT,UserID INT,Score BIGINT,GameID INT,NickName NVARCHAR(32),Gender TINYINT,FaceID SMALLINT,CustomID INT,ActorID SMALLINT,FaceFrameID SMALLINT,VipPoint INT)

	-- 查询数据
	INSERT INTO @TableRank
	SELECT TOP(100)	ROW_NUMBER() OVER (ORDER BY GainScore DESC) AS RankID,A.UserID,GainScore AS Score,B.GameID,B.NickName,B.Gender,B.FaceID,B.CustomID,B.ActorID,B.FaceFrameID,B.MemberPoint AS VipPoint 
	FROM (SELECT UserID,SUM(GainScore) AS GainScore FROM LHGameGoldDB.dbo.ReportUserKind(NOLOCK) WHERE DateID>=@SDateID AND @EDateID<=@EDateID AND KindID=@dwKindID GROUP BY UserID) A
	LEFT JOIN LHAccountsDB.dbo.AccountsInfo(NOLOCK) B ON A.UserID=B.UserID WHERE GainScore<>0 AND B.IsRobot=0 
	
	-- 判断用户是否存在榜单
	SELECT @RankID=RankID,@Score=Score FROM @TableRank WHERE UserID=@dwUserID
	IF @RankID IS NOT NULL
	BEGIN
		SELECT @RankID AS RankID,@UserID AS UserID,@GameID AS GameID,@NickName AS NickName,@Gender AS Gender,@FaceID AS FaceID,@CustomID AS CustomID,@ActorID AS ActorID,@FaceFrameID AS FaceFrameID,@VipPoint AS VipPoint,@Score AS Score
		UNION ALL
		SELECT * FROM (
		SELECT TOP (@dwTopN) RankID,UserID,GameID,NickName,Gender,FaceID,CustomID,ActorID,FaceFrameID,VipPoint,Score FROM @TableRank ORDER BY RankID ASC) A
	END
	ELSE
	BEGIN
		SELECT 0 AS RankID,@UserID AS UserID,@GameID AS GameID,@NickName AS NickName,@Gender AS Gender,@FaceID AS FaceID,@CustomID AS CustomID,@ActorID AS ActorID,@FaceFrameID AS FaceFrameID,@VipPoint AS VipPoint,0 AS Score
		UNION ALL
		SELECT * FROM (
		SELECT TOP (@dwTopN) RankID,UserID,GameID,NickName,Gender,FaceID,CustomID,ActorID,FaceFrameID,VipPoint,Score FROM @TableRank ORDER BY RankID ASC) A
	END	
END

SET NOCOUNT OFF

RETURN 0
GO

---------------------------------------------------------------------------
--	邀请榜单
CREATE  PROCEDURE WEB_GR_GetInviteCharts
	@dtSDate		DATETIME,				-- 开始时间
	@dtEDate		DATETIME,				-- 结束时间
	@dwTopN			INT,					-- 取前 N 名
	@dwUserID		INT,					-- 用户标识
	@strErrorDescribe NVARCHAR(127) OUTPUT	-- 输出信息
WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

-- 账户信息
DECLARE @UserID INT
DECLARE @Nullity TINYINT
DECLARE @GameID INT
DECLARE @NickName NVARCHAR(32)
DECLARE @Gender TINYINT
DECLARE @FaceID SMALLINT
DECLARE @CustomID INT
DECLARE @ActorID SMALLINT
DECLARE @FaceFrameID SMALLINT
DECLARE @VipPoint INT

-- 缓存信息
DECLARE @RankID INT
DECLARE @ValidCount INT

-- 最大Top
DECLARE @MaxTopN INT

-- 时间信息
DECLARE @SDateID INT
DECLARE @EDateID INT

BEGIN
	SET @MaxTopN=10000
	IF @dwTopN>@MaxTopN SET @dwTopN=@MaxTopN

	-- 设置时间
	SET @SDateID=CAST(CAST(@dtSDate AS FLOAT) AS INT)
	SET @EDateID=CAST(CAST(@dtEDate AS FLOAT) AS INT)-1

	-- 验证用户
	SELECT @UserID=UserID,@Nullity=Nullity,@GameID=GameID,@NickName=NickName,@Gender=Gender,@FaceID=FaceID,@CustomID=CustomID,@ActorID=ActorID,@FaceFrameID=FaceFrameID,@VipPoint=MemberPoint
	FROM LHAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 101
	END

	-- 查询数据
	SELECT TOP(@dwTopN) ROW_NUMBER() OVER (ORDER BY SUM(SpreadValidCount) DESC) AS RankID,UserID,SUM(SpreadValidCount) AS ValidCount  
	INTO #tbCharts
	FROM ReportDayUser(NOLOCK) WHERE DateID>=@SDateID AND @EDateID<=@EDateID
	GROUP BY UserID
	
	-- 判断用户是否存在榜单
	SELECT @RankID=RankID,@ValidCount=ValidCount FROM #tbCharts WHERE UserID=@dwUserID
	IF @RankID IS NOT NULL
	BEGIN
		SELECT @RankID AS RankID,@UserID AS UserID,@ValidCount AS ValidCount
		UNION ALL
		SELECT * FROM (
		SELECT TOP (@dwTopN) RankID,UserID,ValidCount FROM #tbCharts(NOLOCK) ORDER BY RankID ASC) A
	END
	ELSE
	BEGIN
		SELECT 0 AS RankID,@UserID AS UserID,0 AS ValidCount
		UNION ALL
		SELECT * FROM (
		SELECT TOP (@dwTopN) RankID,UserID,ValidCount FROM #tbCharts(NOLOCK) ORDER BY RankID ASC) A
	END	
END

SET NOCOUNT OFF

RETURN 0
GO

---------------------------------------------------------------------------
--	邀请榜单奖励
CREATE  PROCEDURE WEB_GR_GetInviteChartsAward
	
WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

-- 最大Top
DECLARE @TopN INT
DECLARE @Award INT
DECLARE @SendTime INT
DECLARE @ExpireTime INT

BEGIN
	-- 设置参数
	SET @TopN=10
	SET @Award=11000
	SET @SendTime=DATEDIFF(S,'1970-01-01 03:00:00', GETDATE()) 
	SET @ExpireTime=DATEDIFF(S,'1970-01-01 03:00:00', DATEADD(M,1,GETDATE())) 

	--定义时间
	DECLARE @dSDate DATETIME		-- 今日开始时间
	DECLARE @mLSDate DATETIME		-- 上月开始时间
	DECLARE @mLEDate DATETIME		-- 上月结束时间
	DECLARE @SDateID INT
	DECLARE @EDateID INT
	SET @dSDate=CAST(CONVERT(VARCHAR(10),GETDATE(),121) AS DATETIME)	
	SET @mLSDate=DATEADD(m,-3,DATEADD(d,1-DAY(@dSDate),@dSDate))
	SET @mLEDate=DATEADD(d,1-DAY(@dSDate),@dSDate)
	SET @SDateID=CAST(CAST(@mLSDate AS FLOAT) AS INT)
	SET @EDateID=CAST(CAST(@mLEDate AS FLOAT) AS INT)-1

	-- 查询数据
	SELECT TOP(@TopN) ROW_NUMBER() OVER (ORDER BY SUM(SpreadValidCount) DESC) AS RankID,UserID,SUM(SpreadValidCount) AS ValidCount  
	INTO #tbCharts
	FROM ReportDayUser(NOLOCK) WHERE DateID>=@SDateID AND @EDateID<=@EDateID
	GROUP BY UserID

	-- 奖励
	DECLARE @Index INT
	DECLARE @UserID INT
	DECLARE @ValidCount INT
	DECLARE @MailID BIGINT
	DECLARE @MailKind TINYINT
	DECLARE @MailTitle NVARCHAR(32)
	DECLARE @MailContent NVARCHAR(512)
	SET @MailTitle='Monthly invitation rewards'
	SET @Index=1
	WHILE @Index<=10
	BEGIN
		-- 查询用户
		SELECT @UserID=UserID,@ValidCount=ValidCount FROM #tbCharts WHERE RankID=@Index
		IF @UserID IS NOT NULL AND @ValidCount>0
		BEGIN
			-- 奖励
			IF @Index=1
			BEGIN
				IF @ValidCount>=30
				BEGIN
					SET @MailKind=1
					SET @MailContent='Congratulations!You got the championship in the invitation ranking.You won the reward in kind, please contact our customer service.'
				END
				ELSE
				BEGIN
					SET @MailKind=2
					SET @MailContent='Congratulations!You got the fourth place in the invitation ranking.You got reward:'+LTRIM(STR(@Award))+' gold.'
				END
			END
			IF @Index=2
			BEGIN
				IF @ValidCount>=30
				BEGIN
					SET @MailKind=1
					SET @MailContent='Congratulations!You got the second place in the invitation ranking.You won the reward in kind, please contact our customer service.'
				END
				ELSE
				BEGIN
					SET @MailKind=2
					SET @MailContent='Congratulations!You got the fourth place in the invitation ranking.You got reward:'+LTRIM(STR(@Award))+' gold.'
				END
			END
			IF @Index=3
			BEGIN
				IF @ValidCount>=30
				BEGIN
					SET @MailKind=1
					SET @MailContent='Congratulations!You got the third place in the invitation ranking.You won the reward in kind, please contact our customer service.'
				END
				BEGIN
					SET @MailKind=2
					SET @MailContent='Congratulations!You got the fourth place in the invitation ranking.You got reward:'+LTRIM(STR(@Award))+' gold.'
				END
			END
			IF @Index=4
			BEGIN
				SET @MailKind=2
				SET @MailContent='Congratulations!You got the fourth place in the invitation ranking.You got reward:'+LTRIM(STR(@Award))+' gold.'
			END
			IF @Index=5
			BEGIN
				SET @MailKind=2
				SET @MailContent='Congratulations!You got the fifth place in the invitation ranking.You got reward:'+LTRIM(STR(@Award))+' gold.'
			END
			IF @Index=6
			BEGIN
				SET @MailKind=2
				SET @MailContent='Congratulations!You got the sixth place in the invitation ranking.You got reward:'+LTRIM(STR(@Award))+' gold.'
			END
			IF @Index=7
			BEGIN
				SET @MailKind=2
				SET @MailContent='Congratulations!You got the seventh place in the invitation ranking.You got reward:'+LTRIM(STR(@Award))+' gold.'
			END
			IF @Index=8
			BEGIN
				SET @MailKind=2
				SET @MailContent='Congratulations!You got the eighth place in the invitation ranking.You got reward:'+LTRIM(STR(@Award))+' gold.'
			END
			IF @Index=9
			BEGIN
				SET @MailKind=2
				SET @MailContent='Congratulations!You got the ninth place in the invitation ranking.You got reward:'+LTRIM(STR(@Award))+' gold.'
			END
			IF @Index=10
			BEGIN
				SET @MailKind=2
				SET @MailContent='Congratulations!You got the tenth place in the invitation ranking.You got reward:'+LTRIM(STR(@Award))+' gold.'
			END

			-- 发送邮件
			INSERT INTO LHAccountsDB.dbo.AccountsMailInfo(MailKind,MailType,MailTitle,MailContent,SendKind,SendTime,ExpireTime,CollectTime)
			VALUES(@MailKind,1,@MailTitle,@MailContent,2,@SendTime,@ExpireTime,GETDATE())

			-- 设置ID
			SET @MailID=SCOPE_IDENTITY()

			-- 邮件用户
			INSERT INTO LHAccountsDB.dbo.AccountsMailBox(UserID,MailID,MailState,ExpireTime,UpdateTime)
			VALUES(@UserID,@MailID,1,@ExpireTime,GETDATE())

			-- 邮件附件
			IF @MailKind=2
			BEGIN
				INSERT INTO LHAccountsDB.dbo.AccountsMailAttach(MailID,GoodsID,GoodsLevel,GoodsCount,GoodsIndate)
				VALUES(@MailID,2,0,@Award,0)
			END
		END

		SET @UserID=NULL
		SET @Index=@Index+1
	END
END

SET NOCOUNT OFF

RETURN 0
GO
