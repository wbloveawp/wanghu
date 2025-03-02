
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

-- 校验标识
CREATE  PROCEDURE dbo.GSP_GR_EfficacyUserID
	@dwUserID INT,							-- 用户标识
	@strErrorDescribe NVARCHAR(127) OUTPUT  -- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID			INT
DECLARE @Gender			TINYINT
DECLARE @GameID			INT
DECLARE @FaceID			INT
DECLARE @CustomID		INT
DECLARE @NickName		NVARCHAR(31)

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- 查询用户
	SELECT @UserID=a.UserID,@GameID=a.GameID,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
		RETURN 2
	END	

	-- 帐号关闭
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
		RETURN 3
	END

	-- 查询群组
	DECLARE @ClubIDArray NVARCHAR(512)
	SELECT @ClubIDArray=ClubIDArray FROM ClubUserInfo(NOLOCK) WHERE UserID=@dwUserID

	--调整变量
	IF @ClubIDArray IS NULL SET @ClubIDArray=N';'

	-- 查询茶社信息
	SELECT a.*,b.FaceID AS CreaterFaceID, b.CustomID AS CreaterCustomID, b.NickName AS CreaterNickName 
	FROM ClubProperty(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsInfo(NOLOCK) b  ON a.CreaterID=b.UserID WHERE a.ClubID IN (SELECT ClubID FROM dbo.IMF_SplitClubID(@ClubIDArray,';'))		
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------