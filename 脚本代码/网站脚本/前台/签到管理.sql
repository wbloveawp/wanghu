----------------------------------------------------------------------
-- ʱ�䣺2021-11-01
-- ��;��ÿ��ǩ��  
----------------------------------------------------------------------

USE [LHGameGoldDB]
GO

-- ǩ����Ϣ
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].WEB_GameMBCheckInInfo') AND OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].WEB_GameMBCheckInInfo
GO

-- ÿ��ǩ��
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].WEB_GameMBCheckIn') AND OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].WEB_GameMBCheckIn
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

----------------------------------------------------------------------------------
-- ǩ����Ϣ
CREATE PROCEDURE WEB_GameMBCheckInInfo
	@dwUserID INT,								-- �û���ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID			INT
DECLARE @Nullity		TINYINT

-- ��¼��Ϣ
DECLARE @RecordID INT

-- ִ���߼�
BEGIN
	-- ��ѯ�û�	
	SELECT @UserID=UserID,@Nullity=Nullity FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
	
	-- �û�����
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�'
		RETURN 100
	END	

	-- �ʺŷ��
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 101
	END		

	-- ǩ���ܴ���
	DECLARE @AllCount INT
	SELECT @AllCount=COUNT(*) FROM CheckInItem

	-- ǩ���߼�
	DECLARE @IsCheck	INT	 -- �Ƿ�ǩ��
	DECLARE @LxCount	INT  -- ��������
	DECLARE @LxScore	INT  -- �������			

	SELECT @LxCount=LxCount,@LxScore=LxScore FROM LHGameRecordDB.dbo.RecordCheckIn(NOLOCK) WHERE UserID=@dwUserID AND DATEDIFF(D,CollectDate,GETDATE())=0
	IF @LxCount IS NULL
	BEGIN	
		SET @IsCheck=0
		SELECT @LxCount=LxCount,@LxScore=LxScore FROM LHGameRecordDB.dbo.RecordCheckIn(NOLOCK) WHERE UserID=@dwUserID AND DATEDIFF(D,CollectDate,GETDATE())=1
		IF @LxCount IS NULL
		BEGIN
			SET @LxCount=0
			SET @LxScore=0
		END
		ELSE
		BEGIN
			IF @LxCount=@AllCount
			BEGIN
				SET @LxCount=0
				SET @LxScore=0
			END
		END
	END
	ELSE
	BEGIN
		SET @IsCheck=1
	END	

	-- �������
	SELECT @UserID AS UserID,@IsCheck AS IsCheck,@AllCount AS AllCount,@LxCount AS LxCount,@LxScore AS LxScore
	
END
RETURN 0
GO

----------------------------------------------------------------------------------
-- ÿ��ǩ��
CREATE PROCEDURE WEB_GameMBCheckIn
	@dwUserID			INT,					-- �û���ʶ	
	@strMachineID		NVARCHAR(32),			-- ������         
	@strClientIP		NVARCHAR(15),			-- ǩ����ַ
	@strErrorDescribe	NVARCHAR(127) OUTPUT	-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID			INT
DECLARE @GameID			INT
DECLARE @Accounts		NVARCHAR(32)
DECLARE @Nullity		TINYINT
DECLARE @Score			BIGINT
DECLARE @BeforeScore	BIGINT

-- ִ���߼�
BEGIN
	-- ��ѯ�û�	
	SELECT @UserID=UserID,@GameID=GameID,@Accounts=Accounts,@Nullity=Nullity FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID	

	-- �û�����
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�'
		RETURN 100
	END	

	-- �ʺŷ��
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 101
	END	

	-- ��¼��ѯ
	IF EXISTS (SELECT RecordID FROM LHGameRecordDB.dbo.RecordCheckIn(NOLOCK) WHERE UserID=@dwUserID AND DATEDIFF(D,CollectDate,GETDATE())=0)
	BEGIN
		SET @strErrorDescribe=N'��Ǹ����������ǩ��,������������'
		RETURN 4
	END

	-- ǩ����������
	DECLARE @CheckInCount INT
	SELECT @CheckInCount=COUNT(RecordID) FROM LHGameRecordDB.dbo.RecordCheckIn(NOLOCK) WHERE MachineID=@strMachineID AND DATEDIFF(D,CollectDate,GETDATE())=0 
	IF @CheckInCount>=3
	BEGIN
		SET @strErrorDescribe=N'��Ǹ���������ѽ����˶��ǩ��������������������'
		RETURN 5
	END

	-- ǩ���ܴ���
	DECLARE @AllCount INT
	SELECT @AllCount=COUNT(*) FROM CheckInItem
	IF @AllCount=0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��ǩ�����ݼ���ʧ�ܡ�'
		RETURN 6
	END

	-- ǩ���߼�
	DECLARE @ItemCount      INT	 -- ��Ʒ����			
	DECLARE @dwPresentScore	INT  -- ������ȡ���
	DECLARE @dwLxCount		INT  -- ������������
	DECLARE @dwLxScore		INT  -- �����������			
	DECLARE @dwLastLxCount	INT  -- �ϴ���������
	DECLARE @dwLastLxScore	INT  -- �ϴ��������

	SELECT @dwLastLxCount=LxCount,@dwLastLxScore=LxScore FROM LHGameRecordDB.dbo.RecordCheckIn(NOLOCK) WHERE UserID=@dwUserID AND DATEDIFF(D,CollectDate,GETDATE())=1
	IF @dwLastLxCount IS NULL
	BEGIN
		SET @dwLxCount=1
		SET @dwLastLxCount=0
		SET @dwLastLxScore=0
	END
	ELSE
	BEGIN
		IF @dwLastLxCount=@AllCount
		BEGIN		
			SET @dwLxCount=1
			SET @dwLastLxCount=0
			SET @dwLastLxScore=0
		END
		ELSE
		BEGIN		
			SET @dwLxCount=@dwLastLxCount+1
		END
	END

	-- ��ȡ������Ϣ
	SELECT @ItemCount=ItemCount FROM CheckInItem WHERE ItemIndex=@dwLxCount

	-- ���ͽ��
	SET @dwPresentScore=@ItemCount
	SET @dwLxScore=@dwLastLxScore+@dwPresentScore

	-- ǩ����¼
	INSERT INTO LHGameRecordDB.dbo.RecordCheckIn(UserID,PresentScore,LxCount,LxScore,MachineID,IPAddress)
	VALUES(@dwUserID,@dwPresentScore,@dwLxCount,@dwLxScore,@strMachineID,@strClientIP) 

	-- ǩ��֮ǰ
	SELECT @BeforeScore=Score FROM LHAccountsDB.dbo.AccountsGoldInfo(NOLOCK) WHERE UserID=@dwUserID
	IF @BeforeScore IS NULL
	BEGIN
		SET @BeforeScore=0
	END

	-- ���ͽ��
	UPDATE LHAccountsDB.dbo.AccountsGoldInfo SET Score=Score+@dwPresentScore,GrantTimes=GrantTimes+1,GrantAmount=GrantAmount+@dwPresentScore WHERE UserID=@dwUserID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO LHAccountsDB.dbo.AccountsGoldInfo(UserID,Score,GrantTimes,GrantAmount)
		VALUES(@dwUserID,@dwPresentScore,1,@dwPresentScore)
	END

	-- �仯��־
	INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID,GameID,TypeID,BeforeGold,ChangeGold,CollectClientIP,CollectDateTime)
	VALUES(@dwUserID,@GameID,101,@BeforeScore,@dwPresentScore,@strClientIP,GETDATE())
	
	-- ˢ�²Ƹ�
	SELECT @Score=Score FROM LHAccountsDB.dbo.AccountsGoldInfo(NOLOCK) WHERE UserID=@dwUserID

	-- �������
	SELECT @UserID AS UserID,
		   @dwPresentScore AS PresentScore,
		   @dwLxCount AS LxCount,
		   @dwLxScore AS LxScore,
		   @Score AS Score
END
RETURN 0
GO
