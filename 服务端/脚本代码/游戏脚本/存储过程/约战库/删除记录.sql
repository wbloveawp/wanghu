
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DeleteRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DeleteRecord]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ɾ����¼
CREATE PROC GSP_GR_DeleteRecord
	-- �û���Ϣ
	@dwUserID INT,								-- �û� I D
	@dwClubID INT,								-- ��ݱ�ʶ
	@dwRecordID INT,							-- ��¼��ʶ
	@cbRecordKind TINYINT,						-- ��¼����	
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
	
	-- ��ѯ����
	IF @dwClubID<>0
	BEGIN
		DECLARE @CreaterID INT
		SELECT @CreaterID=CreaterID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

		-- ����ж�
		IF @CreaterID IS NULL
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
			RETURN 5		
		END

		-- У�������ʶ
		IF @CreaterID<>@dwUserID
		BEGIN

			-- �ֱ��ʶ
			--SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

			---- Ⱥ���Ա��
			--SET	@TableName='ClubMember_';
			--SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

			---- ��ѯ��Ա����
			--DECLARE @OUT_OperaterType SMALLINT	
			--DECLARE @OUT_OperaterRight SMALLINT	
			--SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType,@ManageRight=ManageRight FROM ',@TableName,'(NOLOCK)',
			--					 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwUserID))				  
	
			---- ִ�����
			--EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT,@ManageRight INT OUT',@MemberType=@OUT_OperaterType OUTPUT,@ManageRight=@OUT_OperaterRight OUTPUT

			---- ��������
			--IF @OUT_OperaterType IS NULL SET @OUT_OperaterType=0

			---- У��Ȩ��
			--IF @OUT_OperaterType<3 OR (@OUT_OperaterType=3 AND (@OUT_OperaterRight&0x0002)=0)
			--BEGIN
			--	SET @strErrorDescribe=N'{"Idx":1180,"Msg":"��Ǹ����û��Ȩ�޲���Լս�淨��"}'
			--	RETURN 6
			--END		
			
			SET @strErrorDescribe=N''
			RETURN 6
		END
	END

	-- ɾ����Ա�����¼
	IF @cbRecordKind=1
	BEGIN	
		
		-- �ֱ��ʶ				
		SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

		-- Ⱥ���Ա��
		SET	@TableName='ClubMemberChange_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	
		
		-- �������м�¼
		IF @dwRecordID=0
		BEGIN
			-- ��ѯ����		
			SET	@Sqlstr = CONCAT( ' DELETE ',@TableName,' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

			-- ִ�нű�
			EXECUTE(@Sqlstr)			
		END ELSE
		BEGIN

			-- ��ѯ����		
			SET	@Sqlstr = CONCAT( ' DELETE ',@TableName,' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND RecordID=',CONVERT(NVARCHAR(18), @dwRecordID))

			-- ִ�нű�
			EXECUTE(@Sqlstr)
		END		
	END	

	-- ɾ����Ա֪ͨ��¼
	IF @cbRecordKind=2
	BEGIN
		IF @dwRecordID=0
		BEGIN
			DELETE RecordUserNotice WHERE UserID=@dwUserID
		END ELSE
		BEGIN
			DELETE RecordUserNotice WHERE RecordID=@dwRecordID
		END
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------