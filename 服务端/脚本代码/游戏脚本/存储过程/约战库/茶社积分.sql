
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryClubScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryClubScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ModifyClubScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ModifyClubScore]
GO

--IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_PresentClubScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
--DROP PROCEDURE [dbo].[GSP_GR_PresentClubScore]
--GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- ��ѯ����
CREATE  PROCEDURE dbo.GSP_GR_QueryClubScore
	@dwMemberID INT,			-- ��Ա��ʶ	
	@dwClubID	INT				-- Ⱥ���ʶ			
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	

	-- ��ѯ�û�
	DECLARE @ClubScore INT

	-- ��������
	DECLARE	@TableID	INT																			
	DECLARE	@TableName	NVARCHAR(128)								
	DECLARE @Sqlstr		NVARCHAR(512)

	-- �ֱ��ʶ				
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- Ⱥ���Ա��
	SET	@TableName='ClubMember_';
	SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

	-- ��ѯ���ݱ�
	IF (EXISTS(SELECT 1 FROM LHGameBattleDB.dbo.sysobjects WHERE xtype='u' AND name=@TableName))
	BEGIN
		-- ��ѯ�ű�		
		SET	@Sqlstr = CONCAT( ' SELECT @Score=ClubScore FROM  ',@TableName,'(NOLOCK) WHERE MemberID=',CONVERT(NVARCHAR(18), @dwMemberID),' AND ',
								' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

		-- ִ�нű�
		EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@Score INT OUT',@Score=@ClubScore OUTPUT
	END

	-- ��������
	IF @ClubScore IS NULL SET @ClubScore=0

	SELECT @ClubScore AS ClubScore
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �޸Ļ���
CREATE PROC GSP_GR_ModifyClubScore
	-- �û���Ϣ
	@dwUserID INT,								-- �û� I D
	@dwClubID INT,								-- ��ݱ�ʶ
	@dwMemberID INT,							-- ��Ա��ʶ
	@lModifyScore INT,							-- �������	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT

-- ִ���߼�
BEGIN
	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	
	-- ��ѯ�û�
	SELECT @UserID=a.UserID,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- �û�������
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����"}'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�"}'
		RETURN 3
	END	

	-- ��ѯ��Ա
	DECLARE @MemberGameID INT
	DECLARE @MemberNickName NVARCHAR(32)
	SELECT @MemberGameID=GameID,@MemberNickName=NickName FROM LHAccountsDB.dbo.AccountsInfo WHERE UserID=@dwMemberID

	-- ��ѯ�û�
	IF @MemberGameID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1202,"Msg":"ϵͳδ��ѯ���ĳ�Ա���û���Ϣ�����֤���ٴγ��Ե�¼��"}'
		RETURN 4
	END
	
	-- ��ѯȺ��
	DECLARE @ClubName NVARCHAR(32)
	SELECT @ClubName=ClubName FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- ����ж�
	IF @ClubName IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
		RETURN 5		
	END

	-- ��ݷ�ģʽ
	DECLARE @ClubScoreMode TINYINT
	--SELECT @ClubScoreMode=CONVERT(TINYINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'ClubOption' AND StatusName='ClubScoreMode'
	
	-- ��������
	IF @ClubScoreMode IS NULL SET @ClubScoreMode=0

	-- ��������
	DECLARE @PayUserID INT									
	DECLARE @PayUserGameID INT	
	DECLARE @PayUserType TINYINT
	DECLARE	@MemberScore INT	

	-- ��������
	DECLARE	@TableID INT			
	DECLARE @Sqlstr		NVARCHAR(512)								
	DECLARE	@TableName	NVARCHAR(128)					

	-- ���÷���
	SET @PayUserType=0
	SET @PayUserID=@dwUserID

	-- �ֱ��ʶ				
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- Ⱥ���Ա��
	SET	@TableName='ClubMember_';
	SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

	-- ��ѯ����Ա��Ϣ
	DECLARE @OUT_OperaterMangeRight INT
	DECLARE @OUT_OperaterMemberType SMALLINT				
	SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType,@ManageRight=ManageRight FROM ',@TableName,'(NOLOCK)',
							' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwUserID))				  
	
	-- ִ�����
	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT,@ManageRight INT OUT',@MemberType=@OUT_OperaterMemberType OUTPUT,@ManageRight=@OUT_OperaterMangeRight OUTPUT

	-- ��������
	IF @OUT_OperaterMangeRight IS NULL SET @OUT_OperaterMangeRight=0
	IF @OUT_OperaterMemberType IS NULL SET @OUT_OperaterMemberType=0

	-- У������
	IF @OUT_OperaterMemberType<3 OR (@OUT_OperaterMemberType=3 AND (@OUT_OperaterMangeRight&0x0020)=0)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1203,"Msg":"��Ǹ����û��Ȩ���������֣�"}'
		RETURN 6
	END

	-- ��������
	SET @PayUserType=@OUT_OperaterMemberType

	-- ���ֵģʽ
	--IF  @ClubScoreMode=1	
	--BEGIN
	--	-- ��ѯ����		
	--	SET	@Sqlstr = CONCAT( ' SELECT @ClubScore=ClubScore FROM  ',@TableName,' WHERE MemberID=',CONVERT(NVARCHAR(18), @PayUserID),' AND ',
	--						  ' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

	--	-- ִ�нű�
	--	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@ClubScore INT OUT',@ClubScore=@PayUserScore OUTPUT		
		
	--	-- ��������
	--	IF @PayUserScore IS NULL
	--	BEGIN
	--		SET @strErrorDescribe=N'ϵͳδ��ѯ�����Ļ�����Ϣ������ϵ�ͻ���Ա�˽����飡'
	--		RETURN 6
	--	END

	--	-- У��ݷ�	
	--	IF @lModifyScore>0 AND @PayUserScore<@lModifyScore
	--	BEGIN
	--		-- ��ʾ��Ϣ
	--		SET @strErrorDescribe='�������Ļ��ֲ��㣬������û��ֶ�Ⱥ��ٴγ��ԣ�'		
	--		RETURN 7		
	--	END		
	--END	

	-- ������Ϣ
	DECLARE @BattleNum INT
	DECLARE @KindID SMALLINT
	DECLARE @ServerID SMALLINT
	DECLARE @ScoreKind TINYINT	
	DECLARE @BattleStatus TINYINT
	DECLARE @OnlineStatus TINYINT
	SELECT @KindID=KindID,@ServerID=ServerID,@ScoreKind=ScoreKind,@BattleNum=BattleNum,@BattleStatus=BattleStatus,@OnlineStatus=OnlineStatus FROM BattleUserLocker(NOLOCK) WHERE UseriD=@dwMemberID
	
	-- ��������
	IF @BattleStatus IS NULL
	BEGIN
		SELECT @KindID=0,@ServerID=0,@ScoreKind=0,@BattleStatus=0,@OnlineStatus=0	
	END ELSE
	BEGIN
		IF @OnlineStatus=0 OR @ScoreKind<>2
		BEGIN
			SELECT @KindID=0,@ServerID=0
		END
	END

	-- ��������
	IF @lModifyScore<0 AND @OnlineStatus=1
	BEGIN
		-- ��ʾ��Ϣ
		SET @strErrorDescribe=N'{"Idx":1204,"Msg":"���ڸù�Ա��Լս����['+CONVERT(NVARCHAR(18),@BattleNum)+']�е���Ϸ��δ��ɣ���ʱ���ܽ����·ֲ�����","Ops":["'+CONVERT(NVARCHAR(18),@BattleNum)+'"]}'
		RETURN 7		
	END	
	
	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��ѯ����		
	SET	@Sqlstr = CONCAT( ' SELECT @ClubScore=ClubScore FROM  ',@TableName,' WITH (UPDLOCK) WHERE MemberID=',CONVERT(NVARCHAR(18), @dwMemberID),' AND ',
						  ' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

	-- ִ�нű�
	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@ClubScore INT OUT',@ClubScore=@MemberScore OUTPUT
	

	-- ��������
	IF @MemberScore IS NULL SET @MemberScore=0

	---- У�����
	--IF @lModifyScore<0 AND @MemberScore<-@lModifyScore
	--BEGIN				
	--	-- ����ع�
	--	ROLLBACK TRAN
	--	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	--	-- ��ʾ��Ϣ
	--	SET @strErrorDescribe='���ڸù�Ա���ϵĲ�ݷֲ���['+CONVERT(NVARCHAR(18),@lModifyScore/100)+']��������������ٴβ�����'		
	--	RETURN 4				
	--END

	-- ���·���		
	SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName,' SET ClubScore=ClubScore+',CONVERT(NVARCHAR(18), @lModifyScore),' WHERE MemberID=',CONVERT(NVARCHAR(18), @dwMemberID),' AND ',
						  ' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

	-- ִ�нű�
	EXECUTE(@Sqlstr)	
	IF @@ERROR<>0
	BEGIN				
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ��ʾ��Ϣ
		SET @strErrorDescribe=N'{"Idx":1205,"Msg":"��Ǹ������ϵͳԭ�򣬲���ʧ�ܣ�"}'
		RETURN 5				
	END

	-- ���÷���
	SET @MemberScore=@MemberScore+@lModifyScore

	-- ��������
	DECLARE @DateID	INT
	DECLARE @TradeType SMALLINT	
	DECLARE @IncreaseScore BIGINT
	DECLARE @DecreaseScore BIGINT

	-- ��������
	SET @DateID = CAST(CAST(GETDATE() AS FLOAT) AS INT)

	--��������
	SELECT @TradeType=2,@IncreaseScore=@lModifyScore,@DecreaseScore=0
	IF @lModifyScore<0 
	BEGIN
		SELECT @TradeType=3,@IncreaseScore=0,@DecreaseScore=@lModifyScore
	END
	
	-- ���ּ�¼
	UPDATE StreamClubBalance SET IncreaseScore=IncreaseScore+@IncreaseScore,DecreaseScore=DecreaseScore+@DecreaseScore 
	WHERE DateID=@DateID AND ClubID=@dwClubID AND UserID=@dwMemberID
	IF @@ROWCOUNT=0
	BEGIN
		-- �����¼
		INSERT INTO StreamClubBalance(DateID,ClubID,UserID,IncreaseScore,DecreaseScore) 
		VALUES (@DateID,@dwClubID,@dwMemberID,@IncreaseScore,@DecreaseScore)
	END

	-- ��ѯ��Ϸ��ʶ
	SELECT @PayUserGameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@PayUserID

	-- �����¼
	INSERT INTO LHBattleRecordDB.dbo.RecordClubScoreChange(DateID,SrcUserID,SrcGameID,SrcClubScore,SrcUserType,DstUserID,DstGameID,DstClubScore,ClubID,TradeType,TradeClubScore)
	VALUES (@DateID,@PayUserID,@PayUserGameID,0,@PayUserType,@dwMemberID,@MemberGameID,@MemberScore,@dwClubID,@TradeType,-@lModifyScore)

	-- У������
	--IF @OUT_OperaterMemberType=4 
	--BEGIN
	--	--��ֵģʽ		
	--	IF @ClubScoreMode=1
	--	BEGIN
	--		-- ��ѯ����
	--		UPDATE GameClubScoreInfo SET ClubScore=ClubScore-@lModifyScore WHERE UserID=@PayUserID				
	--	END

	--	-- ���»���
	--	SELECT @PayUserScore=ClubScore FROM GameClubScoreInfo(NOLOCK) WHERE UserID=@PayUserID

	--	-- �ݷֵ���
	--	IF @PayUserScore IS NULL SET @PayUserScore=0
	--END	ELSE	
	--BEGIN
		-- ���·���		
		--SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName,' SET ClubScore=ClubScore+',CONVERT(NVARCHAR(18), -@lModifyScore),' WHERE MemberID=',CONVERT(NVARCHAR(18), @PayUserID),' AND ',
		--					  ' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

		---- ִ�нű�
		--EXECUTE(@Sqlstr)	
		--IF @@ERROR<>0
		--BEGIN				
		--	-- ����ع�
		--	ROLLBACK TRAN
		--	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		--	-- ��ʾ��Ϣ
		--	SET @strErrorDescribe='��Ǹ���������ݿ��쳣�����β���ִ��ʧ�ܣ�'		
		--	RETURN 5				
		--END	
		
		---- ���»���
		--SET @PayUserScore=@PayUserScore-@lModifyScore	
	--END
		
	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	
	-- �������
	SELECT @dwMemberID AS MemberID, @PayUserID AS PayUserID, @dwClubID AS ClubID, @ClubName AS ClubName, @KindID AS KindID, @ServerID AS ServerID, @PayUserType AS PayUserType,
	@MemberScore AS MemberScore, @MemberNickName AS MemberNickName
END

RETURN 0

GO


------------------------------------------------------------------------------------------------------

---- ���ͻ���
--CREATE PROC GSP_GR_PresentClubScore
--	-- �û���Ϣ
--	@dwUserID INT,								-- �û� I D
--	@dwClubID INT,								-- ��ݱ�ʶ
--	@dwMemberID INT,							-- ��Ա��ʶ
--	@lPresentScore INT,							-- ���ͷ���	
--	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
--WITH ENCRYPTION AS

---- ��������
--SET NOCOUNT ON

---- �Զ��ع�
--SET XACT_ABORT ON

---- ������Ϣ
--DECLARE @UserID INT

---- ִ���߼�
--BEGIN
--	-- ��ѯ�û�
--	DECLARE @GameID INT
--	DECLARE @Nullity BIT
--	DECLARE @ShutDown BIT	
--	DECLARE @SrcNickName NVARCHAR(32)
	
--	-- ��ѯ�û�
--	SELECT @UserID=a.UserID,@GameID=a.GameID,@SrcNickName=NickName,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
--	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

--	-- �û�������
--	IF @UserID IS NULL
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
--		RETURN 1
--	END	

--	-- �ʺŽ�ֹ
--	IF @Nullity<>0
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����"}'
--		RETURN 2
--	END	

--	-- �ʺŹر�
--	IF @ShutDown<>0
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�"}'
--		RETURN 3
--	END

--	-- ��ѯ��Ա
--	DECLARE @DstGameID INT
--	DECLARE @DstNickName NVARCHAR(32)
--	SELECT @DstGameID=GameID,@DstNickName=NickName FROM LHAccountsDB.dbo.AccountsInfo WHERE UserID=@dwMemberID

--	-- ��ѯ�û�
--	IF @DstGameID IS NULL
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1202,"Msg":"ϵͳδ��ѯ���ĳ�Ա���û���Ϣ�����֤���ٴγ��Ե�¼��"}'
--		RETURN 4
--	END
	
--	-- ��ѯȺ��
--	DECLARE @ClubName NVARCHAR(32)
--	SELECT @ClubName=ClubName FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

--	-- ����ж�
--	IF @ClubName IS NULL
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
--		RETURN 5		
--	END

--	-- ���ֶ���	
--	DECLARE	@SrcMemberType TINYINT
--	DECLARE	@DstMemberType TINYINT
--	DECLARE	@SrcMemberScore INT	
--	DECLARE @DstMemberScore INT					

--	-- ��������
--	DECLARE	@TableID INT			
--	DECLARE @Sqlstr		NVARCHAR(512)								
--	DECLARE	@TableName	NVARCHAR(128)						

--	-- �ֱ��ʶ				
--	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

--	-- Ⱥ���Ա��
--	SET	@TableName='ClubMember_';
--	SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	
	
--	-- ��ѯ����		
--	SET	@Sqlstr = CONCAT( ' SELECT @ClubScore=ClubScore,@MemberType=MemberType FROM  ',@TableName,' WHERE MemberID=',CONVERT(NVARCHAR(18), @UserID),' AND ',
--						  ' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

--	-- ִ�нű�
--	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@ClubScore INT OUT, @MemberType TINYINT OUT',@ClubScore=@SrcMemberScore OUTPUT,@MemberType=@SrcMemberType OUTPUT		
		
--	-- ��������
--	IF @SrcMemberScore IS NULL
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1206,"Msg":"ϵͳδ��ѯ�����Ļ�����Ϣ������ϵ�ͻ���Ա�˽����飡"}'
--		RETURN 6
--	END

--	-- У�����
--	IF @SrcMemberScore < @lPresentScore
--	BEGIN
--		-- ��ʾ��Ϣ
--		SET @strErrorDescribe=N'{"Idx":1207,"Msg":"�������Ļ��ֲ��㣬������û��ֶ�Ⱥ��ٴγ��ԣ�"}'
--		RETURN 7		
--	END	
	
--	-- ��ѯ����		
--	SET	@Sqlstr = CONCAT( ' SELECT @ClubScore=ClubScore,@MemberType=MemberType FROM  ',@TableName,' WHERE MemberID=',CONVERT(NVARCHAR(18), @dwMemberID),' AND ',
--						  ' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

--	-- ִ�нű�
--	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@ClubScore INT OUT, @MemberType TINYINT OUT',@ClubScore=@SrcMemberScore OUTPUT,@MemberType=@DstMemberType OUTPUT	
		
--	-- ��������
--	IF @DstMemberScore IS NULL
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1208,"Msg":"ϵͳδ��ѯ���ó�Ա�Ļ�����Ϣ������ϵ�ͻ���Ա�˽����飡"}'
--		RETURN 8
--	END

--	-- ������Ϣ
--	DECLARE @BattleNum INT
--	DECLARE @KindID SMALLINT
--	DECLARE @ServerID SMALLINT
--	DECLARE @ScoreKind TINYINT	
--	DECLARE @BattleStatus TINYINT
--	DECLARE @OnlineStatus TINYINT
--	SELECT @KindID=KindID,@ServerID=ServerID,@ScoreKind=ScoreKind,@BattleNum=BattleNum,@BattleStatus=BattleStatus,@OnlineStatus=OnlineStatus FROM BattleUserLocker(NOLOCK) WHERE UseriD=@dwMemberID
	
--	-- ��������
--	IF @BattleStatus IS NULL
--	BEGIN
--		SELECT @KindID=0,@ServerID=0,@ScoreKind=0,@BattleStatus=0,@OnlineStatus=0	
--	END ELSE
--	BEGIN
--		IF @OnlineStatus=0 OR @ScoreKind<>3
--		BEGIN
--			SELECT @KindID=0,@ServerID=0
--		END
--	END	
	
--	-- ��ʼ����
--	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
--	BEGIN TRAN

--	-- ���·���		
--	SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName,' SET ClubScore=ClubScore-',CONVERT(NVARCHAR(18), @lPresentScore),' WHERE MemberID=',CONVERT(NVARCHAR(18), @dwUserID),' AND ',
--						  ' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

--	-- ִ�нű�
--	EXECUTE(@Sqlstr)	
	
--	-- ���·���		
--	SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName,' SET ClubScore=ClubScore+',CONVERT(NVARCHAR(18), @lPresentScore),' WHERE MemberID=',CONVERT(NVARCHAR(18), @dwMemberID),' AND ',
--						  ' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

--	-- ִ�нű�
--	EXECUTE(@Sqlstr)	

--	-- ��������
--	DECLARE @DateID	INT	
--	SET @DateID = CAST(CAST(GETDATE() AS FLOAT) AS INT)

--	-- ���ּ�¼
--	UPDATE StreamClubBalance SET TradeOut=TradeOut-@lPresentScore WHERE DateID=@DateID AND ClubID=@dwClubID AND UserID=@UserID
--	IF @@ROWCOUNT=0
--	BEGIN
--		-- �����¼
--		INSERT INTO StreamClubBalance(DateID,ClubID,UserID,TradeOut) VALUES (@DateID,@dwClubID,@dwMemberID,-@lPresentScore)
--	END

--	-- ���ּ�¼
--	UPDATE StreamClubBalance SET TradeIn=TradeIn+@lPresentScore WHERE DateID=@DateID AND ClubID=@dwClubID AND UserID=@dwMemberID
--	IF @@ROWCOUNT=0
--	BEGIN
--		-- �����¼
--		INSERT INTO StreamClubBalance(DateID,ClubID,UserID,TradeIn) VALUES (@DateID,@dwClubID,@dwMemberID,@lPresentScore)
--	END

--	-- �����¼
--	INSERT INTO LHBattleRecordDB.dbo.RecordClubScoreChange(DateID,SrcUserID,SrcGameID,SrcClubScore,SrcUserType,DstUserID,DstGameID,DstClubScore,ClubID,TradeType,TradeClubScore)
--	VALUES (@DateID,@UserID,@GameID,@SrcMemberScore,@SrcMemberType,@dwMemberID,@DstGameID,@DstMemberScore,@dwClubID,9,-@lPresentScore)	

--	-- ��������
--	COMMIT TRAN
--	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

--	-- ��ѯ����		
--	SET	@Sqlstr = CONCAT( ' SELECT @ClubScore=ClubScore FROM  ',@TableName,' WHERE MemberID=',CONVERT(NVARCHAR(18), @UserID),' AND ',
--						  ' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

--	-- ִ�нű�
--	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@ClubScore INT OUT',@ClubScore=@SrcMemberScore OUTPUT

--	-- ��ѯ����		
--	SET	@Sqlstr = CONCAT( ' SELECT @ClubScore=ClubScore FROM  ',@TableName,' WHERE MemberID=',CONVERT(NVARCHAR(18), @dwMemberID),' AND ',
--						  ' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

--	-- ִ�нű�
--	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@ClubScore INT OUT',@ClubScore=@DstMemberScore OUTPUT
	
--	-- �������
--	SELECT @ClubName AS ClubName, @KindID AS KindID, @ServerID AS ServerID, @SrcMemberScore AS SrcMemberScore, @SrcNickName AS SrcNickName, @DstMemberScore AS DstMemberScore,
--		   @DstNickName AS DstNickName
--END

--RETURN 0

--GO


----------------------------------------------------------------------------------------------------