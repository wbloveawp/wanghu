USE LHGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchReward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchReward]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 比赛奖励
CREATE PROC GSP_GR_MatchReward
	-- 比赛信息
	@dwUserID INT,								-- 用户 I D	
	@dwMatchID INT,								-- 比赛标识
	@lMatchNo  BIGINT,							-- 比赛场次	
	@MatchStartTime DATETIME,					-- 开赛时间

	-- 奖励信息
	@wRankID  SMALLINT,							-- 比赛名次	
	@cbIsPromoted TINYINT,						-- 晋级标识
	@strRewardEntity NVARCHAR(128),				-- 奖励实物
	@strRewardGoods NVARCHAR(256)				-- 奖励物品

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询比赛
	DECLARE @MatchName NVARCHAR(64)
	SELECT @MatchName=MatchName FROM MatchOption(NOLOCK) WHERE MatchID=@dwMatchID

	-- 校验比赛
	IF @MatchName IS NULL
	BEGIN
		RETURN 1
	END
		
	-- 计算时间
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
		
	-- 时间定义
	DECLARE @MailID INT
	DECLARE @SendTime INT
	DECLARE @ExpireTime INT

	-- 邮件信息
	DECLARE @MatchRank NVARCHAR(32)
	DECLARE @MailTitle NVARCHAR(64)
	DECLARE @MailContent NVARCHAR(512)

	-- 计算时间
	SET @SendTime=LHAccountsDB.dbo.LHF_GetTimestamp()
	SET @ExpireTime=@SendTime+7*24*3600

	-- 游戏标题
	SET @MailTitle=N'Tournament Reward'

	-- 比赛名次
	SET @MatchRank = CASE @wRankID 
					WHEN 1 THEN N'Championship'
					WHEN 2 THEN N'Second place'
					WHEN 3 THEN N'Third place'										
					ELSE
						CONVERT(NVARCHAR(4),@wRankID)+N'th'
					END

	-- 邮件内容
	SET @MailContent=CONCAT('Congratulations!You got the ',@MatchRank,' in the ',@MatchName,' Tournament at ',CONVERT(NVARCHAR(24),@MatchStartTime,113))		

	-- 生成邮件
	INSERT INTO LHAccountsDB.dbo.AccountsMailInfo(MailKind,MailTitle,MailContent,SendTime,ExpireTime,CollectTime)
	VALUES (2,@MailTitle,@MailContent,@SendTime,@ExpireTime,GetDate())

	-- 邮件标识
	SET @MailID=SCOPE_IDENTITY()

	-- 添加附件
	INSERT INTO LHAccountsDB.dbo.AccountsMailAttach(MailID,GoodsID,GoodsLevel,GoodsCount,GoodsIndate)
	SELECT @MailID, GoodsID, ISNULL([Level],0),ISNULL([Count],0),ISNULL(Indate,0) 
	FROM  OPENJSON(@strRewardGoods)	WITH (GoodsID INT,[Level] INT,[Count] INT, Indate INT)

	-- 插入邮箱
	INSERT INTO LHAccountsDB.dbo.AccountsMailBox(UserID,MailID,MailState,ExpireTime,UpdateTime)
	VALUES (@dwUserID, @MailID, 1, @ExpireTime, GETDATE())

	-- 更新名次
	UPDATE StreamMatchHistory SET RankID=@wRankID, IsPromoted=@cbIsPromoted, RewardEntity=@strRewardEntity, RewardGoods=@strRewardGoods WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo AND UserID=@dwUserID

	-- 抛出数据
	SELECT @MailID AS MailID, @dwUserID AS UserID
END

RETURN 0
GO
