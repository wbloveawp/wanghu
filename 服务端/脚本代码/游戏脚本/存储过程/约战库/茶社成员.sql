
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LHF_GetTableID]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[LHF_GetTableID]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_AddMember]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_AddMember]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DeleteMember]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DeleteMember]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryClubMember]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryClubMember]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryMemberInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryMemberInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UpdateMemberType]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UpdateMemberType]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UpdateMemberStatus]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UpdateMemberStatus]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UpdateMemberBattleCount]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UpdateMemberBattleCount]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

CREATE FUNCTION [dbo].[LHF_GetTableID]
(
   @dwClubID INT		--Ⱥ���ʶ    
)
RETURNS INT
AS
BEGIN
    RETURN @dwClubID/20000
END

GO

----------------------------------------------------------------------------------------------------

-- ��ӳ�Ա
CREATE  PROCEDURE dbo.GSP_GR_AddMember
	--��Ա��Ϣ
	@dwClubID INT,								-- Ⱥ���ʶ
	@dwMemberID INT,							-- ��Ա��ʶ
	@cbMemberType TINYINT,						-- ��Ա����
	@dwOperaterID INT,							-- �����߱�ʶ
	
	-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
DECLARE	@TableID	INT;										
DECLARE @Sqlstr		 NVARCHAR(1024);
DECLARE	@TableMember NVARCHAR(128);								
DECLARE @TableChange NVARCHAR(128);								

-- ��������
SET NOCOUNT ON
SET XACT_ABORT ON

-- ִ���߼�
BEGIN

	-- ��ѯȺ��
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID FROM ClubProperty WHERE ClubID=@dwClubID

	-- ����ж�
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
		RETURN 1		
	END

	-- ��ѯ��Ա��Ϣ
	DECLARE	@MemberGameID INT
	DECLARE @MemberNickName NVARCHAR(32)
	SELECT @MemberGameID=GameID,@MemberNickName=NickName FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwMemberID

	-- ��������Ϣ
	DECLARE	@OperaterGameID INT
	SELECT @OperaterGameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwOperaterID

	-- ��������
	IF @OperaterGameID IS NULL SET @OperaterGameID=0 

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��ȡ����
	DECLARE @wMemberCount SMALLINT
	DECLARE @wMaxMemberCount SMALLINT
	SELECT @wMemberCount=MemberCount, @wMaxMemberCount=MaxMemberCount FROM ClubProperty WHERE ClubID=@dwClubID

	-- ����У��
	IF @wMemberCount IS NULL OR @wMemberCount=@wMaxMemberCount
	BEGIN
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		RETURN 2
	END	

	-- ��ȡȺ���б�
	DECLARE @strClubIDArray NVARCHAR(1024)
	SELECT @strClubIDArray=ClubIDArray FROM ClubUserInfo WHERE UserID=@dwMemberID
	IF @strClubIDArray IS NULL 
	BEGIN
		SET @strClubIDArray=';'
		INSERT INTO ClubUserInfo VALUES (@dwMemberID,@strClubIDArray)
	END

	-- ���ұ�ʶ
	DECLARE @strClubID NVARCHAR(20)
	SET @strClubID=CONCAT(';',CONVERT(NVARCHAR(18),@dwClubID),';');	
	IF CHARINDEX(@strClubID,@strClubIDArray)>0
	BEGIN		
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		RETURN 3
	END	

	-- �ֱ��ʶ
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- Ⱥ���Ա��
	SET	@TableMember='ClubMember_';
	SET	@TableMember= CONCAT(@TableMember, CONVERT(NVARCHAR(8), @TableID));		

	-- �������ݱ�
	IF (NOT EXISTS(SELECT 1 FROM dbo.sysobjects WHERE xtype='u' AND name=@TableMember))
	BEGIN
		SET	@Sqlstr = CONCAT(' USE LHGameBattleDB ',
							'  CREATE TABLE  ', @TableMember,
							' ( ClubID 	INT   NOT NULL, '  ,
							'	MemberID 	INT   NOT NULL , ',
							'	MemberType	TINYINT DEFAULT 0  NOT NULL, ',							
							'	MemberStatus TINYINT DEFAULT 0  NOT NULL, ',
							'	ManageRight	INT DEFAULT 0  NOT NULL, ',																
							'	ClubScore  INT DEFAULT 0  NOT NULL, ',								
							'	CreateCount	INT DEFAULT 0  NOT NULL, ',
							'	BattleCount	INT DEFAULT 0  NOT NULL, ',
							'	RevenueCount  INT DEFAULT 0  NOT NULL, ',											
							'	JoinDateTime DATETIME DEFAULT GETDATE() NOT NULL , ',
							'	LastBattleTime DATETIME DEFAULT GETDATE() NOT NULL , ',
							'	PRIMARY KEY(ClubID, MemberID)',
							'  );');			
		EXECUTE(@Sqlstr);
		IF @@ERROR<>0
		BEGIN
			-- ����ع�
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
			RETURN 4
		END
	END	

	-- ��������
	SET	@Sqlstr = CONCAT( '  INSERT INTO ', @TableMember,'(ClubID, MemberID, MemberType) ' ,
						  '  VALUES(' ,CONVERT(NVARCHAR(18), @dwClubID) , ',' ,CONVERT(NVARCHAR(12), @dwMemberID), ',',CONVERT(NVARCHAR(3), @cbMemberType),')');		
	EXECUTE(@Sqlstr);
	IF @@ERROR<>0
	BEGIN
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		RETURN 5
	END

	-- �������
	SET	@TableChange = 'ClubMemberChange_';							
	SET	@TableChange = concat(@TableChange,	CONVERT(NVARCHAR(8), @TableID));

	-- ���������
	IF (NOT EXISTS(SELECT 1 FROM LHGameBattleDB.dbo.sysobjects WHERE xtype='u' AND name=@TableChange))
	BEGIN
		SET @Sqlstr = CONCAT(' USE LHGameBattleDB ',
							'  CREATE TABLE ', @TableChange,
							' ( ReocrdID BIGINT IDENTITY(1,1) PRIMARY KEY,',
							'   ChangeType TINYINT NOT NULL, ',
							'	ClubID		INT NOT NULL,',
							'	OperaterID  INT NOT NULL,',
							'	OperaterType TINYINT NOT NULL,',
							'	OperaterGameID INT NOT NULL,',							
							'	MemberID	INT NOT NULL,',
							'	MemberType	TINYINT NOT NULL,',
							'	MemberGameID INT NOT NULL,',
							'	MemberScore	BIGINT NOT NULL,',
							'	MemberNickName  NVARCHAR(32) NOT NULL,',
							'	JoinDateTime DATETIME DEFAULT GETDATE() NOT NULL,',														
							'	ChangeDateTime DATETIME DEFAULT GETDATE() NOT NULL,',							
							'	);',
							'CREATE NONCLUSTERED INDEX IDX_GID_UID ON ',@TableChange, '(ClubID,MemberID)');
		EXECUTE(@Sqlstr);
		IF @@ERROR<>0  RETURN 6
	END

	-- �����¼
	SET	@Sqlstr = CONCAT('  USE LHGameBattleDB ',
						 '  INSERT INTO ',@TableChange,
						 '  (ChangeType, ClubID, OperaterID, OperaterType, OperaterGameID, MemberID,MemberType,MemberGameID,MemberScore,MemberNickName,JoinDateTime)  ',
						 '  SELECT  1,',CONVERT(NVARCHAR(12), @dwClubID),',',CONVERT(NVARCHAR(12), @dwOperaterID),',0,',CONVERT(NVARCHAR(12), @OperaterGameID),',MemberID,',
						 '	MemberType,',CONVERT(NVARCHAR(12), @MemberGameID),',ClubScore,''',@MemberNickName,''',JoinDateTime FROM ', @TableMember,'(NOLOCK)',
						 '  WHERE ClubID=',CONVERT(NVARCHAR(12), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(12), @dwMemberID)) 
		
	EXECUTE(@Sqlstr)
	IF @@ERROR<>0
	BEGIN
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		RETURN 7
	END

	-- ���Ⱥ���ʶ		
	SET @strClubIDArray=CONCAT(@strClubIDArray,CONVERT(NVARCHAR(18),@dwClubID),';');
	UPDATE ClubUserInfo SET ClubIDArray=@strClubIDArray WHERE UserID=@dwMemberID;

	-- ��������	
	UPDATE ClubProperty SET ClubStatus=1,MemberCount=MemberCount+1 WHERE ClubID=@dwClubID

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- ��ѯ�ű�		
	SET	@Sqlstr = CONCAT( ' SELECT a.UserID, a.GameID, a.NickName, a.Gender, a.FaceID, a.CustomID, b.*,c.MemberCount '+' FROM LHAccountsDB.dbo.AccountsInfo a, ',@TableMember,' b, ','ClubProperty c',
						  ' WHERE a.UserID=b.MemberID AND c.ClubID=b.ClubID AND ',' b.ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND b.MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))
	EXECUTE(@Sqlstr)

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ɾ����Ա
CREATE  PROCEDURE dbo.GSP_GR_DeleteMember
	-- �û���Ϣ
	@dwUserID INT,								-- �û���ʶ	
	@cbDeleteKind TINYINT,						-- ɾ������

	--Ⱥ����Ϣ
	@dwClubID INT,								-- Ⱥ���ʶ
	@dwMemberID INT,							-- ��Ա��ʶ
	
	-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON
SET XACT_ABORT ON

-- ��������
DECLARE	@TableID	INT;								
DECLARE @Sqlstr		NVARCHAR(1024)											
DECLARE	@TableMember NVARCHAR(128)
DECLARE	@TableChange NVARCHAR(128)

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @UserID	INT
	DECLARE @GameID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	DECLARE @NickName  NVARCHAR(32)			

	-- ��ѯ�û�
	SELECT @UserID=a.UserID,@GameID=a.GameID,@NickName=a.NickName,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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
	DECLARE @CreaterID INT
	DECLARE @PayrollerID INT
	DECLARE @ClubName NVARCHAR(32)
	SELECT @CreaterID=CreaterID,@ClubName=ClubName,@PayrollerID=PayrollerID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- ����ж�
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
		RETURN 5		
	END

	-- �������
	IF @PayrollerID=@dwMemberID
	BEGIN
		UPDATE ClubProperty SET PayrollerID=0 WHERE ClubID=@dwClubID
	END

	-- ��������
	DECLARE @MemberGameID INT
	DECLARE @OperaterType TINYINT	
	DECLARE @MemberNickName NVARCHAR(32)	

	-- ���ñ���
	SELECT @MemberGameID=@GameID,@MemberNickName=@NickName,@OperaterType=0

	-- �ֱ��ʶ
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- �������
	SET	@TableMember='ClubMember_';
	SET	@TableMember= CONCAT(@TableMember, CONVERT(NVARCHAR(8), @TableID));	

	-- ��ѯ��Ա����
	DECLARE @OUT_ClubScore INT
	DECLARE @OUT_MemberType TINYINT
	DECLARE @OUT_OperaterType TINYINT		
	DECLARE @OUT_OperaterRight TINYINT		
	SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType,@ManageRight=ManageRight FROM ',@TableMember,'(NOLOCK)',
						 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwUserID))				  
	
	-- ִ�����
	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT,@ManageRight INT OUT',@MemberType=@OUT_OperaterType OUTPUT,@ManageRight=@OUT_OperaterRight OUTPUT

	-- ���ǳ�Ա
	IF @OUT_OperaterType IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1170,"Msg":"��Ǹ����û��Ȩ���߳��ó�Ա��"}'
		RETURN 5
	END

	-- ��ѯ��Ա��Ϣ
	SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType,@ClubScore=ClubScore FROM ',@TableMember,'(NOLOCK)',
						 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))				  
	
	-- ִ�����
	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT, @ClubScore INT OUT',@MemberType=@OUT_MemberType OUTPUT, @ClubScore=@OUT_ClubScore OUTPUT

	-- ��Ա������
	IF @OUT_MemberType IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1171,"Msg":"�ó�Ա�����ڣ����Ѿ��߳��˲��磡"}'
		RETURN 6
	END	

	-- �߳���Ա
	IF @cbDeleteKind=1
	BEGIN						
		-- У�����
		--IF @OUT_ClubScore<>0
		--BEGIN
		--	SET @strErrorDescribe=N'��Ǹ���ó�Ա���ϻ��ֲ�Ϊ0�����߳���'
		--	RETURN 6				
		--END

		-- ɾ���Լ�
		IF @dwUserID=@dwMemberID
		BEGIN
			RETURN 6		   					
		END				

		-- У�����ͺ�Ȩ��
		IF @OUT_OperaterType<3 OR (@OUT_OperaterType=3 AND (@OUT_OperaterRight&0x0010)=0)
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1170,"Msg":"��Ǹ����û��Ȩ���߳��ó�Ա��"}'
			RETURN 6
		END

		-- У������
		IF @OUT_OperaterType<=@OUT_MemberType	
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1170,"Msg":"��Ǹ����û��Ȩ���߳��ó�Ա��"}'
			RETURN 6
		END

		-- ��ѯ��Ա
		SELECT @MemberGameID=GameID, @MemberNickName=NickName FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwMemberID

		-- ��������
		IF @MemberGameID IS NULL SET @MemberGameID=0
		IF @MemberNickName IS NULL SET @MemberNickName=N''

		-- ���ñ���
		SET @OperaterType=@OUT_OperaterType

		-- �û�֪ͨ
		INSERT INTO RecordUserNotice(UserID,NoticeKind,NoticeContent,CollectDateTime) VALUES(@dwMemberID,2,N'{"Idx":1211,"Ops":["'+@ClubName+'"]}',GETDATE())

	END ELSE
	BEGIN
		-- У�����
		--IF @OUT_ClubScore<>0
		--BEGIN
		--	SET @strErrorDescribe=N'��Ǹ���˳�������Ҫ��������֣�'
		--	RETURN 6				
		--END

		-- �����Լ�
		IF @dwUserID<>@dwMemberID
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
			RETURN 6		   					
		END

		-- �û�֪ͨ
		INSERT INTO RecordUserNotice(UserID,NoticeKind,NoticeContent,CollectDateTime) VALUES(@dwMemberID,1,N'{"Idx":1210,"Ops":["'+@ClubName+'"]}',GETDATE())
	END

	-- �������
	SET	@TableChange = 'ClubMemberChange_';							
	SET	@TableChange = concat(@TableChange,	CONVERT(NVARCHAR(8), @TableID));

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN					

	-- �����¼
	SET	@Sqlstr = CONCAT('  USE LHGameBattleDB ',
						 '  INSERT INTO ',@TableChange,
						 '  (ChangeType, ClubID, OperaterID, OperaterType, OperaterGameID, MemberID,MemberType,MemberGameID,MemberScore,MemberNickName,JoinDateTime)  ',
						 '  SELECT  2,',CONVERT(NVARCHAR(12), @dwClubID),',',CONVERT(NVARCHAR(12), @dwUserID),',',CONVERT(NVARCHAR(3), @OperaterType),',',CONVERT(NVARCHAR(12), @GameID),',MemberID,',
						 '	MemberType,',CONVERT(NVARCHAR(12), @MemberGameID),',ClubScore,''',@MemberNickName,''',JoinDateTime FROM ', @TableMember,'(NOLOCK)',
						 '  WHERE ClubID=',CONVERT(NVARCHAR(12), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(12), @dwMemberID)) 
		
	EXECUTE(@Sqlstr)
	IF @@ERROR<>0
	BEGIN
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		RETURN 8
	END

	-- ɾ����Ա
	SET	@Sqlstr = CONCAT( ' DELETE ', @TableMember, ' WHERE ' , ' ClubID=', CONVERT(NVARCHAR(18), @dwClubID) , ' AND ' ,' MemberID=', CONVERT(NVARCHAR(12), @dwMemberID)) 
	EXECUTE(@Sqlstr);
	IF @@ERROR<>0
	BEGIN
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
		
		RETURN 9
	END	

	-- ��ȡȺ����Ϣ
	DECLARE @strClubIDArray NVARCHAR(1024)
	SELECT @strClubIDArray=ClubIDArray FROM ClubUserInfo WHERE UserID=@dwMemberID

	-- �޸�Ⱥ����Ϣ
	DECLARE @strClubID NVARCHAR(20)		
	SET @strClubID=CONCAT(';',CONVERT(NVARCHAR(18),@dwClubID),';');
	SET @strClubIDArray=REPLACE(@strClubIDArray,@strClubID,';');

	-- ����Ⱥ��
	UPDATE ClubUserInfo SET ClubIDArray=@strClubIDArray WHERE UserID=@dwMemberID		

	-- ��������
	UPDATE ClubProperty SET MemberCount=MemberCount-1 WHERE ClubID=@dwClubID

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- �׳���Ϣ
	SELECT CreaterID,ClubName,MemberCount FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ѯȺ���Ա
CREATE  PROCEDURE dbo.GSP_GR_QueryClubMember
	@dwClubID INT			--Ⱥ���ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ��������
DECLARE	@TableID	INT;																			
DECLARE	@TableName	NVARCHAR(128);								
DECLARE @Sqlstr		NVARCHAR(512);								

-- ִ���߼�
BEGIN
	
	-- �ֱ��ʶ
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- Ⱥ���Ա��
	SET	@TableName='ClubMember_';
	SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

	-- ��ѯ���ݱ�
	IF (EXISTS(SELECT 1 FROM LHGameBattleDB.dbo.sysobjects WHERE xtype='u' AND name=@TableName))
	BEGIN
		-- ��ѯ�ű�		
		SET	@Sqlstr = CONCAT( ' SELECT a.UserID, a.GameID, a.NickName, a.Gender, a.FaceID, a.CustomID, b.*  FROM LHAccountsDB.dbo.AccountsInfo a, ',@TableName,' b ',
							  ' WHERE a.UserID=b.MemberID AND ',' b.ClubID=',CONVERT(NVARCHAR(18), @dwClubID))
		EXECUTE(@Sqlstr)

	END	

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- ��ѯ��Ա��Ϣ
CREATE  PROCEDURE dbo.GSP_GR_QueryMemberInfo
	@dwClubID INT,				--Ⱥ���ʶ	
	@strUserIDList NVARCHAR(MAX)--��ʶ�б�
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ��������
DECLARE	@TableID	INT;																			
DECLARE	@TableName	NVARCHAR(128);								
DECLARE @Sqlstr		NVARCHAR(512);								

-- ִ���߼�
BEGIN
	
	-- �ֱ��ʶ
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- Ⱥ���Ա��
	SET	@TableName='ClubMember_';
	SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

	-- ��ѯ���ݱ�
	IF (EXISTS(SELECT 1 FROM LHGameBattleDB.dbo.sysobjects WHERE xtype='u' AND name=@TableName))
	BEGIN
		-- ��ѯ�ű�		
		SET	@Sqlstr = CONCAT( ' SELECT * FROM ',@TableName,N' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),N' AND MemberID IN (SELECT VALUE FROM OPENJSON(''',@strUserIDList,'''))')
		EXECUTE(@Sqlstr)
	END	

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- ���³�Ա����
CREATE  PROCEDURE dbo.GSP_GR_UpdateMemberType
	-- �û���Ϣ
	@dwUserID INT,								-- �û���ʶ	

	--��Ա��Ϣ
	@dwClubID INT,								-- Ⱥ���ʶ
	@dwMemberID INT,							-- ��Ա��ʶ
	@cbMemberType TINYINT,						-- ��Ա����
	@dwManageRight INT,							-- ����Ȩ��

	-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ��������
DECLARE	@TableID	INT;																			
DECLARE	@TableName	NVARCHAR(128);								
DECLARE @Sqlstr		NVARCHAR(512);								

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @UserID	INT
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

	-- ��ѯȺ��
	DECLARE @CreaterID INT
	DECLARE @PayrollerID INT
	DECLARE @ClubName NVARCHAR(64)
	SELECT @CreaterID=CreaterID,@PayrollerID=PayrollerID,@ClubName=ClubName FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- ����ж�
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
		RETURN 5		
	END

	-- ��ʶ�ж�
	IF @dwUserID=@dwMemberID
	BEGIN
		RETURN 6
	END

	-- �ֱ��ʶ
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- Ⱥ���Ա��
	SET	@TableName='ClubMember_';
	SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

	-- ��ѯ��Ա����
	DECLARE @OUT_OperaterType SMALLINT
	SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType FROM ',@TableName,'(NOLOCK)',
						 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwUserID))				  
	
	-- ִ�����
	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT',@MemberType=@OUT_OperaterType OUTPUT 

	-- ����У��
	IF @OUT_OperaterType IS NULL 
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1172,"Msg":"ϵͳδ��ѯ�����ڸò���ĳ�Ա��Ϣ������ʧ�ܣ�"}'
		RETURN 6
	END

	-- ��ѯ��Ա�ϼ�
	DECLARE @OUT_MemberType SMALLINT		
	SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType FROM ',@TableName,'(NOLOCK)',
							' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))				  
	
	-- ִ�����
	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT',@MemberType=@OUT_MemberType OUTPUT

	-- ����У��
	IF @OUT_MemberType IS NULL 
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1173,"Msg":"ϵͳδ��ѯ���ó�Ա�ڲ��������Ϣ������ʧ�ܣ�"}'
		RETURN 5
	END
	
	-- ���ù���Ա
	IF @cbMemberType=3
	BEGIN
		IF @OUT_OperaterType<>4
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1174,"Msg":"�����ǲ����ϰ壬û��Ȩ�����ù���Ա��"}'
			RETURN 7		
		END

		-- �û�֪ͨ
		INSERT INTO RecordUserNotice(UserID,NoticeKind,NoticeContent,CollectDateTime) VALUES(@dwMemberID,3,N'{"Idx":1212,"Ops":["'+@ClubName+'"]}',GETDATE())
	END
	
	-- ȡ������Ա
	IF @cbMemberType=0 AND @OUT_MemberType=3
	BEGIN
		-- �����ϰ�
		IF @OUT_OperaterType<>4
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1175,"Msg":"�����ǲ����ϰ壬û��Ȩ��ȡ������Ա��"}'
			RETURN 8	
		END

		-- �������
		--IF @PayrollerID=@dwMemberID	
		--BEGIN
		--	SET @strErrorDescribe=N'{"Idx":1175,"Msg":"�ó�Ա�Ǵ�����ң�����ȡ������Ա��"}'
		--	RETURN 8						
		--END	

		-- �û�֪ͨ
		INSERT INTO RecordUserNotice(UserID,NoticeKind,NoticeContent,CollectDateTime) VALUES(@dwMemberID,4,N'{"Idx":1213,"Ops":["'+@ClubName+'"]}',GETDATE())
	END	

	-- ��������		
	SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName, 
						  ' SET MemberType=',CONVERT(NVARCHAR(3), @cbMemberType) ,', ManageRight=',CONVERT(NVARCHAR(3), @dwManageRight),							  
						  ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))		
	EXECUTE(@Sqlstr)	

	-- ��ѯ�ű�		
	SET	@Sqlstr = CONCAT( 'SELECT * FROM ',@TableName,'(NOLOCK) WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))		
	EXECUTE(@Sqlstr)	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���³�Ա״̬
CREATE  PROCEDURE dbo.GSP_GR_UpdateMemberStatus
	-- �û���Ϣ
	@dwUserID INT,								-- �û���ʶ	

	--��Ա��Ϣ
	@dwClubID INT,								-- Ⱥ���ʶ
	@dwMemberID INT,							-- ��Ա��ʶ
	@cbMemberStatus TINYINT,					-- ��Ա״̬

	-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ��������
DECLARE	@TableID	INT;																			
DECLARE	@TableMember NVARCHAR(128);		
DECLARe @TableChange NVARCHAR(128);		
DECLARE @Sqlstr		NVARCHAR(512);								

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @UserID	INT
	DECLARE @GameID	INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- ��ѯ�û�
	SELECT @UserID=a.UserID,@GameID=a.GameID,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID FROM ClubProperty WHERE ClubID=@dwClubID

	-- ����ж�
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
		RETURN 5		
	END

	-- Ⱥ���Լ�
	IF @CreaterID = @dwMemberID 
	BEGIN
		RETURN 5		
	END
	
	-- �ֱ��ʶ
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- Ⱥ���Ա��
	SET	@TableMember='ClubMember_';
	SET	@TableMember= CONCAT(@TableMember, CONVERT(NVARCHAR(8), @TableID));	

	-- ��ѯ��Ա����
	DECLARE @OUT_OperaterType SMALLINT
	DECLARE @OUT_OperaterRight SMALLINT
	SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType,@ManageRight=ManageRight FROM ',@TableMember,'(NOLOCK)',
						 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwUserID))				  
	
	-- ִ�����
	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT,@ManageRight INT OUT',@MemberType=@OUT_OperaterType OUTPUT,@ManageRight=@OUT_OperaterRight OUTPUT

	-- ����У��
	IF @OUT_OperaterType IS NULL 
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1172,"Msg":"ϵͳδ��ѯ�����ڸò���ĳ�Ա��Ϣ������ʧ�ܣ�"}'
		RETURN 6
	END

	-- ��ѯ��Ա�ϼ�
	DECLARE @OUT_MemberType SMALLINT		
	SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType FROM ',@TableMember,'(NOLOCK)',
						 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))				  
	
	-- ִ�����
	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT',@MemberType=@OUT_MemberType OUTPUT

	-- ����У��
	IF @OUT_MemberType IS NULL 
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1173,"Msg":"ϵͳδ��ѯ���ó�Ա�ڲ��������Ϣ������ʧ�ܣ�"}'
		RETURN 7
	END

	-- У�����ͺ�Ȩ��
	IF @OUT_OperaterType<3 OR (@OUT_OperaterType=3 AND (@OUT_OperaterRight&0x0008)=0)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1176,"Msg":"��Ǹ����û��Ȩ���޸ĳ�Ա״̬��"}'		
		RETURN 6
	END

	-- ��������		
	DECLARE @MemberGameID INT
	DECLARE @MemberNickName NVARCHAR(32)

	-- ��ѯ��Ա
	SELECT @MemberGameID=GameID, @MemberNickName=NickName FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwMemberID

	-- ��������
	IF @MemberGameID IS NULL SET @MemberGameID=0
	IF @MemberNickName IS NULL SET @MemberNickName=N''

	-- ��������
	DECLARE @OperateType TINYINT=3
	IF @cbMemberStatus=0 SET @OperateType=4

	-- �������
	SET	@TableChange = 'ClubMemberChange_';							
	SET	@TableChange = concat(@TableChange,	CONVERT(NVARCHAR(8), @TableID));		

	-- �����¼
	SET	@Sqlstr = CONCAT('  USE LHGameBattleDB ',
						 '  INSERT INTO ',@TableChange,
						 '  (ChangeType, ClubID, OperaterID, OperaterType, OperaterGameID, MemberID,MemberType,MemberGameID,MemberScore,MemberNickName,JoinDateTime)  ',
						 '  SELECT  '+CONVERT(NVARCHAR(4), @OperateType)+',',CONVERT(NVARCHAR(12), @dwClubID),',',CONVERT(NVARCHAR(12), @dwUserID),',',CONVERT(NVARCHAR(3), @OUT_OperaterType),',',CONVERT(NVARCHAR(12), @GameID),',MemberID,',
						 '	MemberType,',CONVERT(NVARCHAR(12), @MemberGameID),',ClubScore,''',@MemberNickName,''',JoinDateTime FROM ', @TableMember,'(NOLOCK)',
						 '  WHERE ClubID=',CONVERT(NVARCHAR(12), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(12), @dwMemberID)) 
		
	EXECUTE(@Sqlstr)
	IF @@ERROR<>0
	BEGIN
		RETURN 7
	END

	-- ���½ű�		
	SET	@Sqlstr = CONCAT( ' UPDATE ',@TableMember,' SET MemberStatus=',CONVERT(NVARCHAR(3), @cbMemberStatus) ,							  
						  ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))		
	EXECUTE(@Sqlstr)

	-- ��ѯ�ű�		
	SET	@Sqlstr = CONCAT( 'SELECT * FROM ',@TableMember,'(NOLOCK) WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))		
	EXECUTE(@Sqlstr)	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ����Լս����
CREATE  PROCEDURE dbo.GSP_GR_UpdateMemberBattleCount
	@dwClubID INT,			   --Ⱥ���ʶ	
	@dwMemberID BIGINT,		   --��Ա��ʶ	
	@wIncCreateCount SMALLINT, --��������
	@wIncBattleCount SMALLINT  --�������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ��������
DECLARE	@TableID	INT;																			
DECLARE	@TableName	NVARCHAR(128);								
DECLARE @Sqlstr		NVARCHAR(512);								

-- ִ���߼�
BEGIN
	
	-- �ֱ��ʶ
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- Ⱥ���Ա��
	SET	@TableName='ClubMember_';
	SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

	-- ��ѯ���ݱ�
	IF (EXISTS(SELECT 1 FROM LHGameBattleDB.dbo.sysobjects WHERE xtype='u' AND name=@TableName))
	BEGIN
		-- ���½ű�
		IF @wIncBattleCount>0
		BEGIN		
			SET	@Sqlstr = CONCAT( 'UPDATE ',@TableName, 
								  ' SET CreateCount=CreateCount+',CONVERT(NVARCHAR(5), @wIncCreateCount) ,
								  ',BattleCount=BattleCount+',CONVERT(NVARCHAR(5), @wIncBattleCount),
								  ',LastBattleTime=''',CONVERT(NVARCHAR(24), GetDate(),121),'''',
								  ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))		
		END ELSE
		BEGIN
			SET	@Sqlstr = CONCAT( 'UPDATE ',@TableName, 
								  ' SET CreateCount=CreateCount+',CONVERT(NVARCHAR(5), @wIncCreateCount) ,
								  ',BattleCount=BattleCount+',CONVERT(NVARCHAR(5), @wIncBattleCount),								 
								  ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))					
		END

		EXECUTE(@Sqlstr)

		-- ��ѯ�ű�		
		SET	@Sqlstr = CONCAT( 'SELECT * FROM ',@TableName,'(NOLOCK) WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))		
		EXECUTE(@Sqlstr)
	END	
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

