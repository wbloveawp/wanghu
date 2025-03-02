
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadGameGoodsInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadGameGoodsInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadGameLevelInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadGameLevelInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadUserLevelInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadUserLevelInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadRankLevelInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadRankLevelInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QuerySystemOptionInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QuerySystemOptionInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadNickNameOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadNickNameOption]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ������Ʒ
CREATE  PROCEDURE dbo.GSP_GS_LoadGameGoodsInfo		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

	-- ��Ϸ��Ʒ
	SELECT * FROM GameGoodsInfo(NOLOCK)

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ������Ϸ�ȼ�
CREATE  PROCEDURE dbo.GSP_GS_LoadGameLevelInfo	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

	-- ��Ϸ�ȼ�
	SELECT * FROM GameExpLevel(NOLOCK)

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �����û��ȼ�
CREATE  PROCEDURE dbo.GSP_GS_LoadUserLevelInfo	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

	-- �û��ȼ�
	SELECT * FROM GameUserLevel(NOLOCK)

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ������λ�ȼ�
CREATE  PROCEDURE dbo.GSP_GS_LoadRankLevelInfo	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

	-- �û��ȼ�
	SELECT * FROM GameRankLevel(NOLOCK) ORDER BY LevelID 

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GP_QuerySystemOptionInfo
	@szOptionName NVARCHAR(32)							-- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- �������
	DECLARE @OptionValue VARCHAR(MAX)

	-- ������Ϣ
	SELECT @OptionValue=OptionValue FROM SystemOptionInfo(NOLOCK) WHERE OptionName=@szOptionName

	-- ���ñ���
	IF @OptionValue IS NULL SET @OptionValue=''

	-- �������
	SELECT @OptionValue AS OptionValue, @szOptionName AS OptionName

END

RETURN 0

GO
----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------

-- �ǳ�����
CREATE PROC GSP_LoadNickNameOption
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ȡ����
	DECLARE @szModifyNickNameOption NVARCHAR(MAX)
	SELECT @szModifyNickNameOption=StatusValue FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ModifyNickNameOption'

	DECLARE @MoneyID INT
	DECLARE @MoneyAmount INT
	DECLARE @FreeTimesPerAccounts INT
	DECLARE @MaxTimesPerDay INT

	SET @MoneyID = JSON_VALUE(@szModifyNickNameOption,'$.MoneyID')
	SET @MoneyAmount = JSON_VALUE(@szModifyNickNameOption,'$.MoneyAmount')
	SET @FreeTimesPerAccounts = JSON_VALUE(@szModifyNickNameOption,'$.FreeTimesPerAccounts')
	SET @MaxTimesPerDay = JSON_VALUE(@szModifyNickNameOption,'$.MaxTimesPerDay')

	IF @MoneyID IS NULL SET @MoneyID = 1
	IF @MoneyAmount IS NULL SET @MoneyAmount = 400
	IF @FreeTimesPerAccounts IS NULL SET @FreeTimesPerAccounts = 1
	IF @MaxTimesPerDay IS NULL SET @MaxTimesPerDay = 1

	SELECT @MoneyID AS GoodsID,@MoneyAmount AS GoodsCount,@FreeTimesPerAccounts AS FreeModifyTimes ,@MaxTimesPerDay AS DailyModifyTimes
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------
