
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_SettleBattle]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_SettleBattle]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- Լս����
CREATE  PROCEDURE dbo.GSP_GR_SettleBattle	
	@dwDateID	INT,			-- ���ڱ�ʶ	
	@dwClubID	INT,			-- Ⱥ���ʶ	
	@dwMemberID INT,			-- ��Ա��ʶ
	@wSettleCount SMALLINT,		-- �������
	@dwCreaterID INT,			-- �û���ʶ

	-- �������	
	@strErrorDescribe NVARCHAR(127) OUTPUT -- �����Ϣ
WITH ENCRYPTION AS

-- �û���Ϣ
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	

	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	
	-- ��ѯ�û�
	SELECT @UserID=a.UserID,@GameID=a.GameID,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwCreaterID

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

	-- ��ѯ���
	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- ��ݲ�����
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
		RETURN 5
	END

	-- ����Ⱥ��
	IF @CreaterID<>@dwCreaterID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1199,"Msg":"��Ǹ�������Ǹò�����ϰ壬û��Ȩ�޽��н��㣡"}'
		RETURN 6
	END

	-- ��������
	DECLARE @BattleCount INT
	SELECT @BattleCount=BattleCount FROM LHGameBattleDB.dbo.StreamClubBalance(NOLOCK) WHERE DateID=@dwDateID AND ClubID=@dwClubID AND UserID=@dwMemberID

	-- ����У��
	IF @BattleCount IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1200,"Msg":"��Ǹ��ϵͳδ��ѯ���ó�Ա��Լսͳ����Ϣ������ʧ�ܣ�"}'
		RETURN 7				
	END

	-- ����У��
	IF @wSettleCount>@BattleCount
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1201,"Msg":"��Ǹ������������ܳ���Լս����������ʧ�ܣ�"}'
		RETURN 7
	END

	-- ��������
	UPDATE LHGameBattleDB.dbo.StreamClubBalance SET BalanceCount=@wSettleCount WHERE DateID=@dwDateID AND ClubID=@dwClubID AND UserID=@dwMemberID	


END

RETURN 0

GO

----------------------------------------------------------------------------------------------------