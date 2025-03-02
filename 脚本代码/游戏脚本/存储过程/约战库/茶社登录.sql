
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_EfficacyUserID]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_EfficacyUserID]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- У���ʶ
CREATE  PROCEDURE dbo.GSP_GR_EfficacyUserID
	@dwUserID INT,							-- �û���ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT  -- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID			INT
DECLARE @Gender			TINYINT
DECLARE @GameID			INT
DECLARE @FaceID			INT
DECLARE @CustomID		INT
DECLARE @NickName		NVARCHAR(31)

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

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
	DECLARE @ClubIDArray NVARCHAR(512)
	SELECT @ClubIDArray=ClubIDArray FROM ClubUserInfo(NOLOCK) WHERE UserID=@dwUserID

	--��������
	IF @ClubIDArray IS NULL SET @ClubIDArray=N';'

	-- ��ѯ������Ϣ
	SELECT a.*,b.FaceID AS CreaterFaceID, b.CustomID AS CreaterCustomID, b.NickName AS CreaterNickName 
	FROM ClubProperty(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsInfo(NOLOCK) b  ON a.CreaterID=b.UserID WHERE a.ClubID IN (SELECT ClubID FROM dbo.IMF_SplitClubID(@ClubIDArray,';'))		
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------