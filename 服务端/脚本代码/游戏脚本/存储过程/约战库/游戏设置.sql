----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadKindSetting]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadKindSetting]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_SetWinnerScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_SetWinnerScore]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- ��������
CREATE  PROCEDURE dbo.GSP_GR_LoadKindSetting
	@dwClubID INT				-- Ⱥ���ʶ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	SELECT * FROM ClubKindSetting WHERE ClubID=@dwClubID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ����Ӯ��
CREATE  PROCEDURE dbo.GSP_GR_SetWinnerScore
	-- �û���Ϣ
	@dwUserID INT,								-- �û���ʶ		
	
	-- �淨��Ϣ
	@dwClubID INT,								-- Ⱥ���ʶ
	@wKindID INT,								-- ��Ϸ��ʶ
	@lMinWinnerScore INT,						-- ���Ӯ��

	-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- ִ���߼�
BEGIN

	-- ��������
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
	DECLARE @ClubKind SMALLINT	
	SELECT @CreaterID=CreaterID,@ClubKind=ClubKind FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID AND ClubStatus=1

	-- ����ж�
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"������Ϣ�����ڣ����֤���ٴγ��ԣ�"}'
		RETURN 5		
	END

	-- ����Ⱥ��
	IF @CreaterID <> @dwUserID 
	BEGIN
		-- ��������
		DECLARE	@TableID	INT;																			
		DECLARE	@TableName	NVARCHAR(128);								
		DECLARE @Sqlstr		NVARCHAR(512);

		-- �ֱ��ʶ
		SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

		-- Ⱥ���Ա��
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

		-- ��ѯ��Ա����
		DECLARE @OUT_OperaterType SMALLINT	
		DECLARE @OUT_OperaterRight SMALLINT	
		SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType,@ManageRight=ManageRight FROM ',@TableName,'(NOLOCK)',
							 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwUserID))				  
	
		-- ִ�����
		EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT,@ManageRight INT OUT',@MemberType=@OUT_OperaterType OUTPUT,@ManageRight=@OUT_OperaterRight OUTPUT

		-- ��������
		IF @OUT_OperaterType IS NULL SET @OUT_OperaterType=0

		-- У��Ȩ��
		IF @OUT_OperaterType<3 OR (@OUT_OperaterType=3 AND (@OUT_OperaterRight&0x0040)=0)
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1215,"Msg":"��Ǹ����û��Ȩ�����ô�Ӯ�ң�"}'
			RETURN 5		
		END		
	END

	-- ��������
	UPDATE ClubKindSetting SET MinWinnerScore=@lMinWinnerScore WHERE ClubID=@dwClubID AND KindID=@wKindID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO ClubKindSetting(ClubID,KindID,MinWinnerScore) VALUES(@dwClubID,@wKindID,@lMinWinnerScore)
	END
	
	-- ��ѯ�淨
	SELECT * FROM ClubKindSetting(NOLOCK) WHERE ClubID=@dwClubID AND KindID=@wKindID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------