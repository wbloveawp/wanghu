
----------------------------------------------------------------------------------------------------

USE LHGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchStart]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchStart]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchEliminate]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchEliminate]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchFinish]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchFinish]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MR_MatchFinish]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MR_MatchFinish]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchComplete]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchComplete]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchCancel]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchCancel]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchGiveup]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchGiveup]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchRoundSwitch]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchRoundSwitch]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- ������ʼ
CREATE PROC GSP_GR_MatchStart
	@wServerID		INT,		-- �����ʶ		
	@dwMatchID		INT,		-- ������ʶ
	@lMatchNo		BIGINT,		-- ��������
	@cbMatchType	TINYINT		-- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN 

	-- ��������
	UPDATE MatchScoreInfo SET RankID=0,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0,WinStreak=0  WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo				
END

RETURN 0
	
GO

----------------------------------------------------------------------------------------------------

-- ������̭
CREATE PROC GSP_GR_MatchEliminate
	@dwUserID		INT,		-- �û���ʶ
	@dwMatchID		INT,		-- ������ʶ
	@lMatchNo		BIGINT,		-- ��������
	@cbMatchType	TINYINT,	-- ��������
	@wRankID		SMALLINT	-- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��������
	UPDATE MatchScoreInfo SET RankID=@wRankID,MatchRight=0 WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo	
	
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
-- ��������
CREATE PROC GSP_GR_MatchFinish
	@wServerID		INT,		-- �����ʶ	
	@dwMatchID		INT,		-- ������ʶ
	@lMatchNo		BIGINT,		-- ��������
	@cbMatchType	TINYINT,	-- ��������
	@wPlayCount		SMALLINT,	-- ��Ϸ����	
	@MatchStartTime DATETIME,	-- ����ʱ��
	@MatchFinishTime DATETIME	-- ����ʱ��		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- �����¼
	INSERT INTO StreamMatchHistory(UserID,GameID,MatchID,MatchNo,MatchType,ServerID,RankID,MatchScore,WinCount,LostCount,DrawCount,FleeCount,WinStreak,MaxWinStreak,MatchStartTime,MatchFinishTime,PlayTimeCount)
	SELECT UserID,GameID,MatchID,MatchNo,@cbMatchType,@wServerID,0,Score, WinCount,LostCount,DrawCount,FleeCount,WinStreak,MaxWinStreak,@MatchStartTime,@MatchFinishTime,PlayTimeCount FROM MatchScoreInfo(NOLOCK)  
	WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo AND ServerID=@wServerID AND SignupCount>0

	-- ɾ��������
	IF @@ROWCOUNT>0 AND @cbMatchType<>0x04
	BEGIN
		DELETE MatchScoreInfo WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo
	END	
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
-- ��������
CREATE PROC GSP_MR_MatchFinish
	@dwMatchID		INT,		 -- ������ʶ
	@lMatchNo		BIGINT,		 -- ��������
	@cbMatchType	SMALLINT,	 -- ��������
	@MatchStartTime DATETIME,	 -- ����ʱ��
	@MatchFinishTime DATETIME,	 -- ����ʱ��
	@wRewardUsers   SMALLINT,	 -- ��������
	@strMatchReward NVARCHAR(MAX)-- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

SET XACT_ABORT ON

-- ִ���߼�
BEGIN
	
	-- ��ѯ����
	DECLARE @MatchName NVARCHAR(64)
	SELECT @MatchName=MatchName FROM MatchOption(NOLOCK) WHERE MatchID=@dwMatchID

	-- У�����
	IF @MatchName IS NULL OR @cbMatchType<>0x04
	BEGIN
		RETURN 1
	END

	-- ��������
	DECLARE @MatchRanking TABLE (
		ID INT identity(1,1),
		UserID INT, 
		RankID INT, 
		Score BIGINT, 
		MailID INT, 
		RewardEntity NVARCHAR(32),
		RewardGoods NVARCHAR(256))
	
	-- ��������
	INSERT INTO @MatchRanking(UserID, RankID, Score, MailID, RewardEntity, RewardGoods)
	--SELECT TOP(@wRewardUsers) UserID, ROW_NUMBER() OVER(ORDER BY Score DESC, WinCount DESC, MaxWinStreak DESC, SignupCount DESC, UserID) AS RankID, Score, 0, N'',N'' 
	SELECT TOP(@wRewardUsers) UserID, ROW_NUMBER() OVER(ORDER BY Score DESC, WinCount DESC, SignupCount DESC, UserID) AS RankID, Score, 0, N'',N'' 
	FROM  MatchScoreInfo(NOLOCK) WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo AND SignupCount>0
	ORDER BY Score DESC, WinCount DESC, SignupCount DESC, UserID

	-- ͳ����Ϣ
	DECLARE @AvgMatchTimes INT
	DECLARE @MatchUserCount INT
	DECLARE @SingupUserCount INT	
	DECLARE @TotalMatchTimes INT	
	DECLARE @ConsumeIngot BIGINT	

	-- ͳ������
	SELECT @MatchUserCount=COUNT(1),@TotalMatchTimes=SUM(SignupCount),@AvgMatchTimes=AVG(SignupCount) FROM  MatchScoreInfo(NOLOCK) 
	WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo AND SignupCount>0

	-- ��������
	SELECT @SingupUserCount=COUNT(1) FROM StreamMatchTicket WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo

	-- ��ʯ����
	SELECT @ConsumeIngot=SUM(BuyAmount*BuyTimes) FROM StreamMatchTicket(NOLOCK) WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo

	-- ��������
	DECLARE @MatchReward TABLE (ID INT identity(1,1), StartID INT, EndID INT, Entity NVARCHAR(64), GoodsList NVARCHAR(512))

	-- ���콱��
	IF ISJSON(@strMatchReward)>0
	BEGIN
		INSERT INTO @MatchReward(StartID, EndID, Entity, GoodsList)
		SELECT JSON_VALUE([VALUE],N'$.StartID'),JSON_VALUE([VALUE],N'$.EndID'),ISNULL(JSON_QUERY([VALUE],N'$.Entity'),''),ISNULL(JSON_QUERY([VALUE],N'$.GoodsList'),'') FROM OPENJSON(@strMatchReward)
	END	

	-- ���½���
	UPDATE @MatchRanking SET RewardEntity=b.Entity,RewardGoods=b.GoodsList FROM @MatchRanking a, @MatchReward b WHERE a.RankID>=b.StartID AND a.RankID<=b.EndID	

	-- ��������
	DECLARE @Index INT
	DECLARE @LoopCount INT		

	-- ��������
	DECLARE @RankID INT
	DECLARE @EndID INT
	DECLARE @StartID INT	
	DECLARE @Entity NVARCHAR(64)
	DECLARE @GoodsList NVARCHAR(512)

	-- ʱ�䶨��
	DECLARE @MailID INT
	DECLARE @SendTime INT
	DECLARE @ExpireTime INT

	-- �ʼ���Ϣ
	DECLARE @MatchRank NVARCHAR(32)
	DECLARE @MailTitle NVARCHAR(64)
	DECLARE @MailContent NVARCHAR(512)
	DECLARE @RewardEntity NVARCHAR(128)

	-- ����ʱ��
	SET @SendTime=LHAccountsDB.dbo.LHF_GetTimestamp()
	SET @ExpireTime=@SendTime+7*24*3600

	-- ���ñ���
	SELECT @Index=1,@LoopCount=COUNT(1) FROM @MatchRanking
	
	-- ѭ������
	WHILE @Index<=@LoopCount
	BEGIN
		-- ��ѯ����
		SELECT @RankID=RankID,@RewardEntity=RewardEntity FROM @MatchRanking WHERE ID=@Index		

		-- ��Ϸ����
		SET @MailTitle=N'Tournament Reward'

		-- ��������
		SET @MatchRank = CASE @RankID 
						WHEN 1 THEN N'Championship'
						WHEN 2 THEN N'Second place'
						WHEN 3 THEN N'Third place'										
						ELSE
							CONVERT(NVARCHAR(4),@RankID)+N'th'
						END

		-- �ʼ�����
		SET @MailContent=CONCAT('Congratulations!You got the ',@MatchRank,' in the ',@MatchName,' Tournament at ',CONVERT(NVARCHAR(24),@MatchStartTime,120),'.')
		
		-- ʵ�ｱ��
		IF ISJSON(@RewardEntity)>0
		BEGIN
			SET @MailContent=CONCAT(@MailContent,'\r\nNote��You won the reward in kind, please contact our customer service.')
		END

		-- �����ʼ�
		INSERT INTO LHAccountsDB.dbo.AccountsMailInfo(MailKind,MailType,MailTitle,MailContent,SendTime,ExpireTime,CollectTime)
		VALUES (2,2,@MailTitle,@MailContent,@SendTime,@ExpireTime,GetDate())

		-- �ʼ���ʶ
		SET @MailID=SCOPE_IDENTITY()		

		-- �����ʼ�
		UPDATE @MatchRanking SET MailID=@MailID WHERE ID=@Index

		SET @Index+=1
	END		

	-- ��Ӹ���
	INSERT INTO LHAccountsDB.dbo.AccountsMailAttach(MailID,GoodsID,GoodsLevel,GoodsCount,GoodsIndate)
	SELECT a.MailID, b.GoodsID, ISNULL(b.[Level],0),ISNULL(b.[Count],0),ISNULL(b.Indate,0) 
	FROM  @MatchRanking a CROSS APPLY OPENJSON(a.RewardGoods) 	WITH (GoodsID INT,[Level] INT,[Count] INT, Indate INT) AS b	
	WHERE ISJSON(a.RewardGoods)>0	

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ���볡��
	IF @MatchUserCount>0
	BEGIN
		INSERT INTO StreamMatchSession(MatchID,MatchNo,MatchUserCount,SingupUserCount,AvgMatchTimes,TotalMatchTimes,ConsumeIngot,MatchStartTime,MatchFinishTime,CollectTime)
		VALUES (@dwMatchID,@lMatchNo,@MatchUserCount,@SingupUserCount,@AvgMatchTimes,@TotalMatchTimes,@ConsumeIngot,@MatchStartTime,@MatchFinishTime,GETDATE())
	END

	-- ��������
	INSERT INTO LHAccountsDB.dbo.AccountsMailBox(UserID,MailID,MailState,ExpireTime,UpdateTime)
	SELECT UserID,MailID,1,@ExpireTime,GETDATE() FROM @MatchRanking

	-- ��������
	UPDATE StreamMatchHistory SET RankID=b.RankID,RewardEntity=ISNULL(b.RewardEntity,N''),RewardGoods=ISNULL(b.RewardGoods,N'') FROM StreamMatchHistory a, @MatchRanking b 
	WHERE a.MatchID=@dwMatchID AND a.MatchNo=@lMatchNo AND a.UserID=b.UserID

	-- ɾ��������
	DELETE MatchScoreInfo WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- �׳�����
	SELECT UserID,MailID FROM @MatchRanking
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- �������
CREATE PROC GSP_GR_MatchComplete
	@wServerID		INT,			-- �����ʶ	
	@dwMatchID		INT,			-- ������ʶ
	@lMatchNo		BIGINT,			-- ��������
	@strUserList	NVARCHAR(256)	-- �û��б�
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ����Ȩ��
	IF ISJSON(@strUserList)>0
	BEGIN
		DECLARE @MatchRight SMALLINT = 0x40
		UPDATE MatchScoreInfo SET MatchRight&=~@MatchRight WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo AND UserID IN (SELECT [VALUE] FROM OPENJSON(@strUserList))
	END	
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
-- ����ȡ��
CREATE PROC GSP_GR_MatchCancel
	@wServerID		INT,		-- �����ʶ		
	@dwMatchID		INT,		-- ������ʶ
	@lMatchNo		BIGINT		-- ��������	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN 

	-- ɾ��������
	DELETE MatchScoreInfo WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo	
END

RETURN 0
	
GO

----------------------------------------------------------------------------------------------------
-- ��������
CREATE PROC GSP_GR_MatchGiveup
	@dwUserID		INT,		-- �û���ʶ
	@dwMatchID		INT,		-- ������ʶ
	@lMatchNo		BIGINT		-- ��������	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN 

	-- ɾ��������
	DELETE MatchScoreInfo WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo	
END

RETURN 0
	
GO

----------------------------------------------------------------------------------------------------
-- �ִ��л�
CREATE PROC GSP_GR_MatchRoundSwitch	
	@dwMatchID		INT,		-- ������ʶ
	@lMatchNo		BIGINT,		-- ��������
	@wReserveRatio  SMALLINT,	-- ��������
	@cbMScoreSetType TINYINT,	-- ���ַ�ʽ
	@lInitMatchScore  BIGINT	-- ��ʼ����
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN 

	-- ���÷���
	IF @cbMScoreSetType=0
	BEGIN
		UPDATE MatchScoreInfo SET Score=@lInitMatchScore,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0 WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo	
	END	

	-- ���÷���
	IF @cbMScoreSetType=1
	BEGIN
		UPDATE MatchScoreInfo SET Score=CONVERT(INT,Score/100.0*@wReserveRatio),WinCount=0,LostCount=0,DrawCount=0,FleeCount=0 WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo	
	END
	
END

RETURN 0
	
GO

----------------------------------------------------------------------------------------------------
