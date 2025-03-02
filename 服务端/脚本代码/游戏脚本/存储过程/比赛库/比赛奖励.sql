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

-- ��������
CREATE PROC GSP_GR_MatchReward
	-- ������Ϣ
	@dwUserID INT,								-- �û� I D	
	@dwMatchID INT,								-- ������ʶ
	@lMatchNo  BIGINT,							-- ��������	
	@MatchStartTime DATETIME,					-- ����ʱ��

	-- ������Ϣ
	@wRankID  SMALLINT,							-- ��������	
	@cbIsPromoted TINYINT,						-- ������ʶ
	@strRewardEntity NVARCHAR(128),				-- ����ʵ��
	@strRewardGoods NVARCHAR(256)				-- ������Ʒ

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	DECLARE @MatchName NVARCHAR(64)
	SELECT @MatchName=MatchName FROM MatchOption(NOLOCK) WHERE MatchID=@dwMatchID

	-- У�����
	IF @MatchName IS NULL
	BEGIN
		RETURN 1
	END
		
	-- ����ʱ��
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
		
	-- ʱ�䶨��
	DECLARE @MailID INT
	DECLARE @SendTime INT
	DECLARE @ExpireTime INT

	-- �ʼ���Ϣ
	DECLARE @MatchRank NVARCHAR(32)
	DECLARE @MailTitle NVARCHAR(64)
	DECLARE @MailContent NVARCHAR(512)

	-- ����ʱ��
	SET @SendTime=LHAccountsDB.dbo.LHF_GetTimestamp()
	SET @ExpireTime=@SendTime+7*24*3600

	-- ��Ϸ����
	SET @MailTitle=N'Tournament Reward'

	-- ��������
	SET @MatchRank = CASE @wRankID 
					WHEN 1 THEN N'Championship'
					WHEN 2 THEN N'Second place'
					WHEN 3 THEN N'Third place'										
					ELSE
						CONVERT(NVARCHAR(4),@wRankID)+N'th'
					END

	-- �ʼ�����
	SET @MailContent=CONCAT('Congratulations!You got the ',@MatchRank,' in the ',@MatchName,' Tournament at ',CONVERT(NVARCHAR(24),@MatchStartTime,113))		

	-- �����ʼ�
	INSERT INTO LHAccountsDB.dbo.AccountsMailInfo(MailKind,MailTitle,MailContent,SendTime,ExpireTime,CollectTime)
	VALUES (2,@MailTitle,@MailContent,@SendTime,@ExpireTime,GetDate())

	-- �ʼ���ʶ
	SET @MailID=SCOPE_IDENTITY()

	-- ��Ӹ���
	INSERT INTO LHAccountsDB.dbo.AccountsMailAttach(MailID,GoodsID,GoodsLevel,GoodsCount,GoodsIndate)
	SELECT @MailID, GoodsID, ISNULL([Level],0),ISNULL([Count],0),ISNULL(Indate,0) 
	FROM  OPENJSON(@strRewardGoods)	WITH (GoodsID INT,[Level] INT,[Count] INT, Indate INT)

	-- ��������
	INSERT INTO LHAccountsDB.dbo.AccountsMailBox(UserID,MailID,MailState,ExpireTime,UpdateTime)
	VALUES (@dwUserID, @MailID, 1, @ExpireTime, GETDATE())

	-- ��������
	UPDATE StreamMatchHistory SET RankID=@wRankID, IsPromoted=@cbIsPromoted, RewardEntity=@strRewardEntity, RewardGoods=@strRewardGoods WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo AND UserID=@dwUserID

	-- �׳�����
	SELECT @MailID AS MailID, @dwUserID AS UserID
END

RETURN 0
GO
