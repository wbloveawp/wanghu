----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LHF_VerifyBattleConfig]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[LHF_VerifyBattleConfig]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadBattleConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadBattleConfig]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_AppendBattleConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_AppendBattleConfig]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ModifyBattleConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ModifyBattleConfig]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DeleteBattleConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DeleteBattleConfig]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

CREATE FUNCTION [dbo].[LHF_VerifyBattleConfig]
(
   @strBattleConfig NVARCHAR(512)				-- Լս�淨   
)
RETURNS TINYINT
AS
BEGIN

	-- У���淨
	IF ISJSON(@strBattleConfig)=0
	BEGIN
		RETURN 1
	END

	-- ��ȡ����
	DECLARE @KindID SMALLINT = ISNULL(JSON_VALUE(@strBattleConfig,N'$.KindID'),0);
	DECLARE @PayType SMALLINT = ISNULL(JSON_VALUE(@strBattleConfig,N'$.PayType'),0);
	DECLARE @PlayCount SMALLINT	 = ISNULL(JSON_VALUE(@strBattleConfig,N'$.PlayCount'),0);
	DECLARE @BaseScore INT = ISNULL(JSON_VALUE(@strBattleConfig,N'$.BaseScore'),0);
	DECLARE @EntryCost INT = ISNULL(JSON_VALUE(@strBattleConfig,N'$.EntryCost'),0);
	DECLARE @ServiceFee INT = ISNULL(JSON_VALUE(@strBattleConfig,N'$.ServiceFee'),0);
	DECLARE @ScoreKind TINYINT = ISNULL(JSON_VALUE(@strBattleConfig,N'$.ScoreKind'),0);
	DECLARE @SettleKind SMALLINT = ISNULL(JSON_VALUE(@strBattleConfig,N'$.SettleKind'),0);		
	DECLARE @AllowVoice TINYINT = ISNULL(JSON_VALUE(@strBattleConfig,N'$.AllowVoice'),0);
	DECLARE @AllowLookon TINYINT = ISNULL(JSON_VALUE(@strBattleConfig,N'$.AllowLookon'),0);
	DECLARE @AllowSameIP TINYINT = ISNULL(JSON_VALUE(@strBattleConfig,N'$.AllowSameIP'),0);
	DECLARE @AllowPublic TINYINT = ISNULL(JSON_VALUE(@strBattleConfig,N'$.AllowPublic'),0);
	DECLARE @PlayerCount SMALLINT = ISNULL(JSON_VALUE(@strBattleConfig,N'$.PlayerCount'),0);
	DECLARE @RevenueRate SMALLINT = ISNULL(JSON_VALUE(@strBattleConfig,N'$.RevenueRate'),0);
	DECLARE @EmployScene SMALLINT = ISNULL(JSON_VALUE(@strBattleConfig,N'$.EmployScene'),0);
	DECLARE @CurrencyKind SMALLINT = ISNULL(JSON_VALUE(@strBattleConfig,N'$.CurrencyKind'),0);	

	-- Լս����
	DECLARE @OptionID INT
	DECLARE @OptionMask INT
	DECLARE @MinBaseScore BIGINT
	DECLARE @MaxBaseScore BIGINT
	DECLARE @MinEntryCost BIGINT
	DECLARE @MaxEntryCost BIGINT
	DECLARE @SuportPayType SMALLINT
	DECLARE @MinRevenueRate SMALLINT
	DECLARE @MaxRevenueRate SMALLINT
	DECLARE @strClubOptionName NVARCHAR(32)

	-- ��ѯ����
	SELECT @OptionID=OptionID,@OptionMask=OptionMask,@SuportPayType=SuportPayType,@MinEntryCost=MinEntryCost,@MaxEntryCost=MaxEntryCost,@MinBaseScore=MinBaseScore,@MaxBaseScore=MaxBaseScore 
	FROM BattleBaseOption(NOLOCK) WHERE KindID=@KindID AND ScoreKind=@ScoreKind AND PlayerCount=@PlayerCount AND EmployScene=@EmployScene		
	
	-- ѡ������
	IF @EmployScene=0 SET @strClubOptionName=N'ClubOption_Gold'
	IF @EmployScene=1 SET @strClubOptionName=N'ClubOption_Normal'
	IF @EmployScene=2 SET @strClubOptionName=N'ClubOption_Advanced'

	-- ��ѯ����
	SELECT @MinRevenueRate=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='MinRevenueRate'	
	SELECT @MaxRevenueRate=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='MaxRevenueRate'		

	-- ��������
	IF @MinRevenueRate IS NULL SET @MinRevenueRate=0
	IF @MaxRevenueRate IS NULL SET @MaxRevenueRate=100

	-- У������
	IF @OptionID IS NULL RETURN 2	
	-- У��֧������
	IF @PayType&@SuportPayType=0 RETURN 3
	-- У���������
	IF @CurrencyKind<>1 AND @CurrencyKind<>2 RETURN 4
	-- У��������
	IF @OptionMask&0x10=0 AND @CurrencyKind=0 RETURN 5
	-- У�鷿������
	IF @OptionMask&0x20=0 AND @CurrencyKind=1 RETURN 6
	-- У������
	IF @ServiceFee>0 AND @ScoreKind<>2 RETURN 7
	-- У������
	IF @OptionMask&0x01=0 AND @AllowVoice=1 RETURN 8
	-- У���Թ�
	IF @OptionMask&0x02=0 AND @AllowLookon=1 RETURN 9
	-- У�鹲��
	IF @OptionMask&0x04=0 AND @AllowPublic=1 RETURN 10
	-- У��ͬIP
	IF @OptionMask&0x08=0 AND @AllowSameIP=1 RETURN 11
	-- У��׷�
	IF @BaseScore<@MinBaseScore OR @BaseScore>@MaxBaseScore RETURN 12
	-- У��˰�շ�Χ
	IF @RevenueRate<@MinRevenueRate OR @RevenueRate>@MaxRevenueRate RETURN 13
	-- У���볡��
	IF @CurrencyKind=0 AND (@EntryCost<@MinEntryCost OR @EntryCost>@MaxEntryCost) RETURN 14
	-- У����������
	IF NOT EXISTS(SELECT 1 FROM BattleOptionItem(NOLOCK) WHERE OptionID=@OptionID AND PlayCount=@PlayCount) RETURN 15		

	RETURN 0
END

GO

----------------------------------------------------------------------------------------------------

-- �����淨
CREATE  PROCEDURE dbo.GSP_GR_LoadBattleConfig
	@dwClubID INT				-- Ⱥ���ʶ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�淨
	SELECT * FROM ClubBattleConfig WHERE ClubID=@dwClubID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ����淨
CREATE  PROCEDURE dbo.GSP_GR_AppendBattleConfig
	-- �û���Ϣ
	@dwUserID INT,								-- �û���ʶ	

	-- �淨��Ϣ
	@wKindID SMALLINT,							-- ���ͱ�ʶ
	@dwClubID INT,								-- Ⱥ���ʶ	
	@strBattleConfig NVARCHAR(512),				-- Լս�淨

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

	-- �淨У��
	DECLARE @ResultCode TINYINT
	SET @ResultCode=dbo.LHF_VerifyBattleConfig(@strBattleConfig)
	IF @ResultCode>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1179,"Msg":"Լս�淨У��ʧ��!�������['+CONVERT(NVARCHAR(3),@ResultCode) +N']","Ops":["'+CONVERT(NVARCHAR(3),@ResultCode)+'"]}'
		RETURN 4
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
		IF @OUT_OperaterType<3 OR (@OUT_OperaterType=3 AND (@OUT_OperaterRight&0x0002)=0)
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1180,"Msg":"��Ǹ����û��Ȩ�޲���Լս�淨��"}'
			RETURN 5		
		END		
	END

	-- �淨����
	DECLARE @ConfigCount SMALLINT
	SELECT @ConfigCount=COUNT(1) FROM ClubBattleConfig(NOLOCK) WHERE ClubID=@dwClubID

	-- �������� 
	IF @ConfigCount IS NULL SET @ConfigCount=0

	-- У������
	IF @ConfigCount>0
	BEGIN
		-- ��������	
		DECLARE @MaxConfigCount SMALLINT
		SELECT @MaxConfigCount=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo WHERE StatusKind=N'ClubOption' AND StatusName='MaxConfigCount'
		
		-- ��������
		IF @MaxConfigCount IS NULL SET @MaxConfigCount=1
		
		-- �Ƚϸ���
		IF 	@ConfigCount>=@MaxConfigCount
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1181,"Msg":"��Ǹ����������ֻ�����'+CONVERT(NVARCHAR(5),@MaxConfigCount)+'��Լս�淨��","Ops":["'+CONVERT(NVARCHAR(5),@MaxConfigCount)+'"]}'
			RETURN 6	
		END
	END

	-- �����淨
	INSERT INTO ClubBattleConfig(KindID,ClubID,BattleConfig) VALUES (@wKindID,@dwClubID,@strBattleConfig)

	-- �淨��ʶ
	DECLARE @ConfigID INT
	SET @ConfigID=SCOPE_IDENTITY()

	-- ��ѯ�淨
	SELECT * FROM ClubBattleConfig(NOLOCK) WHERE ConfigID=@ConfigID	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �޸��淨
CREATE  PROCEDURE dbo.GSP_GR_ModifyBattleConfig
	-- �û���Ϣ
	@dwUserID INT,								-- �û���ʶ		
	
	-- �淨��Ϣ
	@wKindID SMALLINT,							-- ���ͱ�ʶ
	@dwClubID INT,								-- Ⱥ���ʶ
	@dwConfigID INT,							-- �淨��ʶ
	@strBattleConfig NVARCHAR(512),				-- Լս�淨

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

	-- �淨У��
	DECLARE @ResultCode TINYINT
	SET @ResultCode=dbo.LHF_VerifyBattleConfig(@strBattleConfig)
	IF @ResultCode>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1179,"Msg":"Լս�淨У��ʧ��!�������['+CONVERT(NVARCHAR(3),@ResultCode) +N']","Ops":["'+CONVERT(NVARCHAR(3),@ResultCode)+'"]}'
		RETURN 4
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
		IF @OUT_OperaterType<3 OR (@OUT_OperaterType=3 AND (@OUT_OperaterRight&0x0002)=0)
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1180,"Msg":"��Ǹ����û��Ȩ�޲���Լս�淨��"}'
			RETURN 5		
		END		
	END

	-- ��������
	IF NOT EXISTS(SELECT 1 FROM ClubBattleConfig(NOLOCK) WHERE ConfigID=@dwConfigID)		
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1182,"Msg":"��Ǹ���淨�����ڣ�"}'
		RETURN 6				
	END
	
	-- �����淨
	UPDATE ClubBattleConfig SET BattleConfig=@strBattleConfig WHERE ConfigID=@dwConfigID		
	
	-- ��ѯ�淨
	SELECT * FROM ClubBattleConfig(NOLOCK) WHERE ConfigID=@dwConfigID		
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ɾ���淨
CREATE  PROCEDURE dbo.GSP_GR_DeleteBattleConfig
	-- �û���Ϣ
	@dwUserID INT,								-- �û���ʶ		
	
	-- �淨��Ϣ
	@wKindID SMALLINT,							-- ���ͱ�ʶ
	@dwClubID INT,								-- Ⱥ���ʶ	
	@dwConfigID INT,							-- �淨��ʶ	

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
	SELECT @CreaterID=CreaterID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID AND ClubStatus=1

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
		IF @OUT_OperaterType<3 OR (@OUT_OperaterType=3 AND (@OUT_OperaterRight&0x0002)=0)
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1180,"Msg":"��Ǹ����û��Ȩ�޲���Լս�淨��"}'
			RETURN 5		
		END		
	END

	-- ɾ���淨
	DELETE ClubBattleConfig WHERE ConfigID=@dwConfigID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------