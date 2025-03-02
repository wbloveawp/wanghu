
----------------------------------------------------------------------
-- 用途：修改头像
----------------------------------------------------------------------


----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WEB_FP_InsertWxFace]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[WEB_FP_InsertWxFace]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WEB_FP_GetUserFace]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[WEB_FP_GetUserFace]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------
-- 插入头像
CREATE PROCEDURE WEB_FP_InsertWxFace
	@dwUserID			INT,						-- 用户标识
	@strFacePath		NVARCHAR(300),				-- 头像地址
	@strClientIP		NVARCHAR(15),               -- 连接地址
	@strErrorDescribe	NVARCHAR(127) OUTPUT	    -- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID			INT
DECLARE @LogonPass		NVARCHAR(32)
DECLARE @Nullity		TINYINT
DECLARE @OldCustomID		INT

-- 执行逻辑
BEGIN
	-- 查询用户	
	SELECT @UserID=UserID,@LogonPass=LogonPass, @Nullity=Nullity,@OldCustomID=CustomID
	FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 用户存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试登录！'
		RETURN 100
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时处于冻结状态，请联系客服中心了解详细情况！'
		RETURN 101
	END	


	-- 查询头像
	IF @OldCustomID<>0
	BEGIN
		DECLARE @FacePath NVARCHAR(256)
		SELECT @FacePath=FacePath FROM AccountsFace(NOLOCK) WHERE UserID=@UserID
		IF @FacePath=@strFacePath 
		BEGIN
			-- 返回结果
			SELECT @OldCustomID AS CustomID
			RETURN 0
		END
	END	

	-- 插入头像
	--INSERT AccountsFace (UserID,CustomFace,InsertAddr,InsertMachine,WxFace) 
	--VALUES (@dwUserID, '', @strClientIP, '',@strWxUrl)

	-- 更新用户
	DECLARE @CustomID INT
	SELECT @CustomID=SCOPE_IDENTITY()
	UPDATE AccountsInfo SET CustomID=SCOPE_IDENTITY() WHERE UserID=@dwUserID

	-- 返回结果
	SELECT @CustomID AS CustomID
END

SET NOCOUNT OFF

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- 获取头像
CREATE PROCEDURE WEB_FP_GetUserFace
	@dwUserID			INT						-- 用户标识	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID			INT
DECLARE @NickName		NVARCHAR(31)
DECLARE @FaceID			INT
DECLARE @CustomID		INT
DECLARE @FacePath		NVARCHAR(256)

-- 执行逻辑
BEGIN
	-- 查询用户	
	SELECT @UserID=UserID,@NickName=NickName,@FaceID=FaceID, @CustomID=CustomID FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID	

	-- 用户存在
	IF @UserID IS NULL 
	BEGIN
		SET @UserID=0
		SET @FaceID=0
		SET @CustomID=0
		SET @FacePath=''
	END	

	IF @CustomID<>0
	BEGIN
		SELECT @FacePath=FacePath FROM AccountsFace(NOLOCK) WHERE UserID=@UserID
		IF @FacePath IS NULL
		BEGIN
			SET @FacePath=''
		END
	END
	ELSE
	BEGIN
		SET @FacePath=''
	END	
	-- 返回结果
	SELECT @UserID AS UserID,@NickName AS NickName,@FaceID AS FaceID,@CustomID AS CustomID,@FacePath AS FacePath
END

SET NOCOUNT OFF

RETURN 0

GO