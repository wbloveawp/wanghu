USE LHGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_BuyMatchTicket]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_BuyMatchTicket]
GO

--IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_ExchangeMatchTicket]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
--DROP PROCEDURE [dbo].[GSP_GS_ExchangeMatchTicket]
--GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_QueryMatchTicket]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_QueryMatchTicket]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadMatchTicketInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadMatchTicketInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ������ȯ
CREATE PROC GSP_GS_BuyMatchTicket
	-- ������Ϣ
	@dwUserID INT,								-- �û� I D	
	@dwMatchID INT,								-- ������ʶ
	@lMatchNo  BIGINT,							-- ��������
	@MatchTime DATETIME,						-- ����ʱ��
	@wReason SMALLINT,							-- ���ԭ��
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- ������Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

SET XACT_ABORT ON

-- ִ���߼�
BEGIN
		
	-- ��������
	DECLARE @UserID INT
	DECLARE @ParentID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	
	-- ��ѯ�û�
	SELECT @UserID=a.UserID, @ParentID=a.ParentID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus 
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- ��ѯ�û�
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

	-- ��������
	DECLARE @MatchName NVARCHAR(32)	
	DECLARE @BuyTicketCount INT
	DECLARE @BuyTicketOption INT
	DECLARE @BuyTicketAmount INT
	DECLARE @BuyTicketMoneyID SMALLINT	

	-- ��ѯ����
	SELECT @MatchName=MatchName,@BuyTicketCount=BuyTicketCount,@BuyTicketOption=BuyTicketOption,@BuyTicketAmount=BuyTicketAmount,@BuyTicketMoneyID=BuyTicketMoneyID FROM MatchOption(NOLOCK) WHERE MatchID=@dwMatchID

	-- У����
	IF @BuyTicketCount IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1288,"Msg":"ϵͳδ��ѯ����صı�����Ϣ�����֤���ٴγ��ԣ�"}'
		RETURN 4
	END

	-- ��ʯ��ƽ���
	IF @BuyTicketMoneyID=1 OR @BuyTicketMoneyID=2
	BEGIN
		-- ��ʼ����
		SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
		BEGIN TRAN

		-- ���Ľ��
		DECLARE  @ExpendAmount INT = -@BuyTicketAmount
		
		-- �۳�����
		DECLARE @ResultCode INT
		EXEC @ResultCode=LHAccountsDB.dbo.GSP_GP_WriteGameWealth @dwUserID,@BuyTicketMoneyID,@wReason,@ExpendAmount

		-- У����
		IF @ResultCode<>0
		BEGIN
			-- �ع�����
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED

			IF @BuyTicketMoneyID=1 SET @strErrorDescribe=N'{"Idx":1289,"Msg":"�����ϵ���ʯ���㣬����ȯ����ʧ�ܣ�"}'
			IF @BuyTicketMoneyID=2 SET @strErrorDescribe=N'{"Idx":1320	Msg":"�����ϵĻƽ����㣬����ȯ����ʧ�ܣ�"}'

			RETURN 5
		END

		-- ����ͳ��
		UPDATE StreamMatchTicket SET BuyTicketCount+=@BuyTicketCount,BuyTimes+=1,UpdateTime=GetDate() WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
		IF @@ROWCOUNT=0
		BEGIN
			INSERT INTO StreamMatchTicket(UserID,MatchID,MatchNo,BuyTicketCount,UseTicketCount,BuyTimes,BuyAmount,BuyMoneyID,UpdateTime,CollectTime)
			VALUES (@dwUserID,@dwMatchID,@lMatchNo,@BuyTicketCount,0,1,@BuyTicketAmount,@BuyTicketMoneyID,GetDate(),GetDate())
		END

		-- ���ñ�����
		IF (@BuyTicketOption&0x01)<>0
		BEGIN
			UPDATE MatchScoreInfo SET ServerID=0,RankID=0,Score=0,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0,WinStreak=0,MaxWinStreak=0,MatchRight=0,SignupCount=0 
			WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo
		END

		-- �״ι����¼
		IF @dwMatchID=1
		BEGIN
			IF NOT EXISTS (SELECT 1 FROM RecordFirstBuyTicket(NOLOCK) WHERE UserID=@dwUserID)
			BEGIN
				INSERT INTO RecordFirstBuyTicket(UserID,ParentID,MatchID,MatchNo,MatchName,MatchTime,BuyAmount,BuyMoneyID,CollectTime)
				VALUES(@dwUserID,@ParentID,@dwMatchID,@lMatchNo,@MatchName,@MatchTime,@BuyTicketAmount,@BuyTicketMoneyID,GETDATE())
			END
		END

		-- ��������
		COMMIT TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	END

	-- �Ƹ�����
	DECLARE @MoneyBalance BIGINT

	-- ��ѯ��ʯ
	IF @BuyTicketMoneyID=1
	BEGIN
		SELECT @MoneyBalance=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID
	END

	-- ��ѯ�ƽ�
	IF @BuyTicketMoneyID=2
	BEGIN
		SELECT @MoneyBalance=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID
	END

	-- ��ѯ��ȯ
	DECLARE @UserTicket INT
	SELECT @UserTicket=BuyTicketCount-UseTicketCount FROM StreamMatchTicket(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo

	-- ��������
	IF @UserTicket IS NULL SET @UserTicket=0
	IF @MoneyBalance IS NULL SET @MoneyBalance=0

	-- �׳�����
	SELECT @BuyTicketMoneyID AS MoneyID, @MoneyBalance AS MoneyBalance, @UserTicket AS TicketCount
END

RETURN 0
GO


----------------------------------------------------------------------------------------------------

---- �һ���ȯ
--CREATE PROC GSP_GS_ExchangeMatchTicket
--	@dwUserID INT,								-- �û� I D	
--	@dwMatchID INT,								-- ������ʶ
--	@lMatchNo  BIGINT,							-- ��������
--	@MatchTime DATETIME,						-- ����ʱ��
--	@RedeemCode NVARCHAR(12),					-- �һ���	
--	@strErrorDescribe NVARCHAR(127) OUTPUT		-- ������Ϣ
--WITH ENCRYPTION AS

---- ��������
--SET NOCOUNT ON

--SET XACT_ABORT ON

---- ִ���߼�
--BEGIN
		
--	-- ��������
--	DECLARE @UserID INT
--	DECLARE @ParentID INT
--	DECLARE @Nullity BIT
--	DECLARE @ShutDown BIT
	
--	-- ��ѯ�û�
--	SELECT @UserID=a.UserID, @ParentID=a.ParentID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus 
--	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

--	-- ��ѯ�û�
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

--	-- ��������
--	DECLARE @MatchName NVARCHAR(32)	
--	DECLARE @BuyTicketCount INT
--	DECLARE @BuyTicketAmount INT
--	DECLARE @BuyTicketMoneyID SMALLINT	

--	-- ��ѯ����
--	SELECT @MatchName=MatchName,@BuyTicketCount=BuyTicketCount,@BuyTicketAmount=BuyTicketAmount,@BuyTicketMoneyID=BuyTicketMoneyID FROM MatchOption(NOLOCK) WHERE MatchID=@dwMatchID

--	-- У����
--	IF @BuyTicketCount IS NULL
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1288,"Msg":"ϵͳδ��ѯ����صı�����Ϣ�����֤���ٴγ��ԣ�"}'
--		RETURN 4
--	END

--	-- ��ʯ��ƽ���
--	IF @BuyTicketMoneyID=1 OR @BuyTicketMoneyID=2
--	BEGIN
--		-- ��ʼ����
--		SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
--		BEGIN TRAN

--		-- ���Ľ��
--		DECLARE  @ExpendAmount INT = -@BuyTicketAmount
		
--		-- �۳�����
--		DECLARE @ResultCode INT
--		EXEC @ResultCode=LHAccountsDB.dbo.GSP_GP_WriteGameWealth @dwUserID,@BuyTicketMoneyID,@wReason,@ExpendAmount

--		-- У����
--		IF @ResultCode<>0
--		BEGIN
--			-- �ع�����
--			ROLLBACK TRAN
--			SET TRANSACTION ISOLATION LEVEL READ COMMITTED

--			IF @BuyTicketMoneyID=1 SET @strErrorDescribe=N'{"Idx":1289,"Msg":"�����ϵ���ʯ���㣬����ȯ����ʧ�ܣ�"}'
--			IF @BuyTicketMoneyID=2 SET @strErrorDescribe=N'{"Idx":1320	Msg":"�����ϵĻƽ����㣬����ȯ����ʧ�ܣ�"}'

--			RETURN 5
--		END

--		-- ����ͳ��
--		UPDATE StreamMatchTicket SET BuyTicketCount+=@BuyTicketCount,BuyTimes+=1,UpdateTime=GetDate() WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
--		IF @@ROWCOUNT=0
--		BEGIN
--			INSERT INTO StreamMatchTicket(UserID,MatchID,MatchNo,BuyTicketCount,UseTicketCount,BuyTimes,BuyAmount,BuyMoneyID,UpdateTime,CollectTime)
--			VALUES (@dwUserID,@dwMatchID,@lMatchNo,@BuyTicketCount,0,1,@BuyTicketAmount,@BuyTicketMoneyID,GetDate(),GetDate())
--		END

--		-- �״ι����¼
--		IF @dwMatchID=1
--		BEGIN
--			IF NOT EXISTS (SELECT 1 FROM RecordFirstBuyTicket(NOLOCK) WHERE UserID=@dwUserID)
--			BEGIN
--				INSERT INTO RecordFirstBuyTicket(UserID,ParentID,MatchID,MatchNo,MatchName,MatchTime,BuyAmount,BuyMoneyID,CollectTime)
--				VALUES(@dwUserID,@ParentID,@dwMatchID,@lMatchNo,@MatchName,@MatchTime,@BuyTicketAmount,@BuyTicketMoneyID,GETDATE())
--			END
--		END

--		-- ��������
--		COMMIT TRAN
--		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
--	END

--	-- �Ƹ�����
--	DECLARE @MoneyBalance BIGINT

--	-- ��ѯ��ʯ
--	IF @BuyTicketMoneyID=1
--	BEGIN
--		SELECT @MoneyBalance=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID
--	END

--	-- ��ѯ�ƽ�
--	IF @BuyTicketMoneyID=2
--	BEGIN
--		SELECT @MoneyBalance=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID
--	END

--	-- ��ѯ��ȯ
--	DECLARE @UserTicket INT
--	SELECT @UserTicket=BuyTicketCount-UseTicketCount FROM StreamMatchTicket(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo

--	-- ��������
--	IF @UserTicket IS NULL SET @UserTicket=0
--	IF @MoneyBalance IS NULL SET @MoneyBalance=0

--	-- �׳�����
--	SELECT @BuyTicketMoneyID AS MoneyID, @MoneyBalance AS MoneyBalance, @UserTicket AS TicketCount
--END

--RETURN 0
--GO


----------------------------------------------------------------------------------------------------

-- ��ѯ��ȯ
CREATE PROC GSP_GS_QueryMatchTicket
	@dwUserID INT,								-- �û� I D		
	@dwMatchID INT,								-- ������ʶ
	@lMatchNo  BIGINT							-- ��������		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
		
	-- ��ѯ��ȯ
	DECLARE @TicketCount INT
	SELECT @TicketCount=BuyTicketCount-UseTicketCount FROM StreamMatchTicket(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
	
	-- ��������
	IF @TicketCount IS NULL SET @TicketCount=0

	-- �׳�����
	SELECT @TicketCount AS TicketCount, @dwMatchID AS MatchID, @lMatchNo AS MatchNo
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- ������ȯ
CREATE PROC GSP_GS_LoadMatchTicketInfo
	@dwMatchID INT,								-- ������ʶ
	@lMatchNo  BIGINT							-- ��������		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
		
	-- ͳ������
	DECLARE @UserCount INT
	SELECT @UserCount=COUNT(1) FROM StreamMatchTicket(NOLOCK) WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo
	
	-- ��������
	IF @UserCount IS NULL SET @UserCount=0

	-- �׳�����
	SELECT @UserCount AS UserCount, @dwMatchID AS MatchID, @lMatchNo AS MatchNo
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------