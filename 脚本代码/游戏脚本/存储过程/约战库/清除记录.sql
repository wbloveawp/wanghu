
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_CleanupSetup]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_CleanupSetup]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_CleanupScoreRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_CleanupScoreRecord]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_CleanupRevenueRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_CleanupRevenueRecord]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_CleanupRankListRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_CleanupRankListRecord]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GR_CleanupSetup
	-- �û���Ϣ
	@dwUserID INT,								-- �û� I D
	@dwClubID INT,								-- ��ݱ�ʶ
	@bAutoCleanup TINYINT,						-- �Զ�����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT

-- ִ���߼�
BEGIN
	-- ��������
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- ��ѯ�û�
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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

	-- ��ѯȺ��
	DECLARE @CreaterGameID INT
	SELECT @CreaterGameID=CreaterGameID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- ����ж�
	IF @CreaterGameID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
		RETURN 5		
	END

	-- �޸Ĺ���
	--UPDATE ClubRules SET AutoCleanup=@bAutoCleanup WHERE ClubID=@dwClubID

	-- ��ѯ����
	SELECT * FROM ClubRules(NOLOCK) WHERE ClubID=@dwClubID	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ������ּ�¼
CREATE PROC GSP_GR_CleanupScoreRecord
	-- �û���Ϣ
	@dwUserID INT,								-- �û� I D
	@dwClubID INT,								-- ��ݱ�ʶ
	@dwMemberID INT,							-- ��Ա��ʶ
	@cbCleanupObject TINYINT,					-- �������
	@dwCreaterGameID INT,						-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT

-- ��������
DECLARE	@TableID	INT;										
DECLARE @Sqlstr		NVARCHAR(512);									
DECLARE	@TableName	NVARCHAR(128);

-- ִ���߼�
BEGIN
	-- ��������
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- ��ѯ�û�
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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

	-- ��ѯȺ��
	DECLARE @CreaterGameID INT
	SELECT @CreaterGameID=CreaterGameID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- ����ж�
	IF @CreaterGameID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
		RETURN 5		
	END

	-- У�������ʶ
	IF @dwCreaterGameID<>@CreaterGameID
	BEGIN
		--SET @strErrorDescribe=N'��Ǹ��������Ĺ�����Ϸ��ʶ��ƥ�䣬��¼����ʧ�ܣ�'
		RETURN 6				
	END

	-- ����һ����Ա����
	IF @cbCleanupObject=0
	BEGIN
		-- ������Ϣ
		DECLARE @BattleNum INT
		DECLARE @BattleStatus TINYINT
		SELECT @BattleNum=BattleNum,@BattleStatus=BattleStatus FROM BattleUserLocker(NOLOCK) WHERE UseriD=@dwMemberID
	
		-- ��������
		IF @BattleStatus IS NULL SET @BattleStatus=0
		
		-- Լս״̬
		IF @BattleStatus=1
		BEGIN
			-- ��ʾ��Ϣ
			SET @strErrorDescribe=N'{"Idx":1197,"Msg":"���ڸù�Ա��Լս����['+CONVERT(NVARCHAR(18),@BattleNum)+']�е���Ϸ��δ��ɣ���ʱ���ܽ������������","Ops":["'+CONVERT(NVARCHAR(18),@BattleNum)+'"]}'
			RETURN 7		
		END

		-- �ֱ��ʶ
		SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

		-- Ⱥ���Ա��
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));					

		-- ���½ű�		
		SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName,' SET ClubScore=0',
							  ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))		
		EXECUTE(@Sqlstr)
	END

	-- �������г�Ա����
	IF @cbCleanupObject=1
	BEGIN
		-- ��������
		DECLARE @BattleUserCount INT
		SET @BattleUserCount=0

		-- �ֱ��ʶ
		SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

		-- Ⱥ���Ա��
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));					

		-- ���½ű�		
		SET	@Sqlstr = CONCAT( ' SELECT @BattleCount=COUNT(1) FROM BattleUserLocker(NOLOCK) WHERE BattleStatus=1 AND UserID IN ','(SELECT MemberID FROM ', @TableName,'(NOLOCK) ',
							  ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),')')		

		print @Sqlstr
		-- ִ�����
		EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@BattleCount SMALLINT OUT',@BattleCount=@BattleUserCount OUTPUT

		-- Լս�û���Ŀ
		IF @BattleUserCount>1
		BEGIN
			-- ��ʾ��Ϣ
			SET @strErrorDescribe=N'{"Idx":1198,"Msg":"���ڵ�ǰ�����в��ֳ�Ա���ڽ���Լս����ʱ���ܽ������������"}'
			RETURN 8		
		END
		
		-- ���½ű�		
		SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName,' SET ClubScore=0 WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID))		
		EXECUTE(@Sqlstr)							
	END
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- ���˰�ռ�¼
CREATE PROC GSP_GR_CleanupRevenueRecord
	-- �û���Ϣ
	@dwUserID INT,								-- �û� I D
	@dwClubID INT,								-- ��ݱ�ʶ
	@dwMemberID INT,							-- ��Ա��ʶ
	@cbCleanupObject TINYINT,					-- �������
	@dwCreaterGameID INT,						-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT

-- ��������
DECLARE	@TableID	INT;										
DECLARE @Sqlstr		NVARCHAR(512);									
DECLARE	@TableName	NVARCHAR(128);									

-- ִ���߼�
BEGIN
	-- ��������
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- ��ѯ�û�
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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

	-- ��ѯȺ��
	DECLARE @CreaterGameID INT
	SELECT @CreaterGameID=CreaterGameID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- ����ж�
	IF @CreaterGameID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
		RETURN 5		
	END

	-- У�������ʶ
	IF @dwCreaterGameID<>@CreaterGameID
	BEGIN
		--SET @strErrorDescribe=N'��Ǹ��������Ĺ�����Ϸ��ʶ��ƥ�䣬��¼����ʧ�ܣ�'
		RETURN 6				
	END

	-- ����һ����Ա˰��
	IF @cbCleanupObject=0
	BEGIN
		-- �ֱ��ʶ
		SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

		-- Ⱥ���Ա��
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));					

		-- ���½ű�		
		SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName,' SET RevenueCount=0',
							  ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))		
		EXECUTE(@Sqlstr)
	END

	-- �������г�Ա˰��
	IF @cbCleanupObject=1
	BEGIN
		-- �ֱ��ʶ
		SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

		-- Ⱥ���Ա��
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));					

		-- ���½ű�		
		SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName,' SET RevenueCount=0 WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID))		
		EXECUTE(@Sqlstr)					
	END

	-- ������ֲ�˰��
	IF @cbCleanupObject=2
	BEGIN
		-- ��������
		UPDATE StreamClubBalance SET Revenue=0 WHERE ClubID=@dwClubID		
		
		-- ��������
		UPDATE StreamClubPartner SET SupplyRevenue=0,ExtractRevenue=0 WHERE ClubID=@dwClubID						
	END
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- ����񵥼�¼
CREATE PROC GSP_GR_CleanupRankListRecord
	-- �û���Ϣ
	@dwUserID INT,								-- �û� I D
	@dwClubID INT,								-- ��ݱ�ʶ
	@dwMemberID INT,							-- ��Ա��ʶ
	@cbListKind TINYINT,						-- ������
	@cbCleanupObject TINYINT,					-- �������
	@dwCreaterGameID INT,						-- ��Ϸ��ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT

-- ִ���߼�
BEGIN
	-- ��������
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- ��ѯ�û�
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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

	-- ��ѯȺ��
	DECLARE @CreaterGameID INT
	SELECT @CreaterGameID=CreaterGameID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- ����ж�
	IF @CreaterGameID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
		RETURN 5		
	END

	-- У�������ʶ
	IF @dwCreaterGameID<>@CreaterGameID
	BEGIN
		--SET @strErrorDescribe=N'��Ǹ��������Ĺ�����Ϸ��ʶ��ƥ�䣬��¼����ʧ�ܣ�'
		RETURN 6				
	END

	-- ��Ӯ�Ұ�
	IF @cbListKind=1
	BEGIN
		-- ����һ����Ա
		IF @cbCleanupObject=0
		BEGIN
			UPDATE StreamClubUser SET WinCount=0 WHERE ClubID=@dwClubID AND UserID=@dwMemberID
		END

		-- �������г�Ա
		IF @cbCleanupObject=1
		BEGIN
			UPDATE StreamClubUser SET WinCount=0 WHERE ClubID=@dwClubID			
		END
	END 

	-- ������
	IF @cbListKind=2
	BEGIN
		-- ����һ����Ա
		IF @cbCleanupObject=0
		BEGIN
			UPDATE StreamClubUser SET LostCount=0 WHERE ClubID=@dwClubID AND UserID=@dwMemberID
		END

		-- �������г�Ա
		IF @cbCleanupObject=1
		BEGIN
			UPDATE StreamClubUser SET LostCount=0 WHERE ClubID=@dwClubID			
		END
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------