

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_ModifySystemFace]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_ModifySystemFace]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_ModifyCustomFace]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_ModifyCustomFace]
GO

----------------------------------------------------------------------------------------------------

-- 修改头像
CREATE PROC GSP_MB_ModifySystemFace
	@dwUserID INT,								-- 用户标识
	@wFaceID SMALLINT,							-- 头像标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 变量定义
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @ActionMaskEver INT
	DECLARE @ActionMaskPerDay INT
	DECLARE @ActionMaskPerWeek INT

	-- 查询用户
	SELECT @UserID=a.UserID,@Nullity=a.Nullity,@ActionMaskEver=a.ActionMaskEver,@ActionMaskPerDay=a.ActionMaskPerDay,@ActionMaskPerWeek=a.ActionMaskPerWeek,@ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
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
		RETURN 2
	END	

	-- 校验掩码
	IF (@ActionMaskEver&0x0004)<>0
	BEGIN
		-- 读取配置
		DECLARE @ModifyUserFaceCost INT
		SELECT @ModifyUserFaceCost=-CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ModifyUserFaceCost'

		-- 调整变量
		IF @ModifyUserFaceCost IS NULL SET @ModifyUserFaceCost=-400
				
		-- 写入财富
		DECLARE @ResultCode INT
		EXEC @ResultCode=GSP_GP_WriteGameWealth @dwUserID,1,18,@ModifyUserFaceCost

		-- 校验结果
		IF @ResultCode<>0
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1322,"Msg":"您身上的黄金余额不足'+CONVERT(NVARCHAR(12),-@ModifyUserFaceCost)+'，修改头像失败！","Ops":["'+CONVERT(NVARCHAR(12),-@ModifyUserFaceCost)+'"]}'
			RETURN 4
		END
	END

	-- 设置掩码
	IF (@ActionMaskEver&0x0004)=0 SET @ActionMaskEver|=0x0004

	-- 更新用户
	UPDATE AccountsInfo SET FaceID=@wFaceID,CustomID=0,ActionMaskEver=@ActionMaskEver WHERE UserID=@dwUserID

	-- 读取黄金
	DECLARE @UserGold BIGINT
	SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 调整变量
	IF @UserGold IS NULL SET @UserGold=0

	-- 返回结果
	SELECT FaceID,CustomID,@ActionMaskEver AS ActionMaskEver, @ActionMaskPerDay AS ActionMaskPerDay, @ActionMaskPerWeek AS ActionMaskPerWeek,@UserGold AS UserGold FROM AccountsInfo(NOLOCK) 
	WHERE UserID=@dwUserID

END	

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 插入头像
CREATE PROC GSP_MB_ModifyCustomFace
	@dwUserID INT,								-- 用户标识
	@dwCustomID　INT,							-- 自定义标识
	@strFacePath NVARCHAR(256),					-- 头像路径
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 变量定义
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @ActionMaskEver INT
	DECLARE @ActionMaskPerDay INT
	DECLARE @ActionMaskPerWeek INT

	-- 查询用户
	SELECT @UserID=a.UserID,@Nullity=a.Nullity,@ActionMaskEver=a.ActionMaskEver,@ActionMaskPerDay=a.ActionMaskPerDay,@ActionMaskPerWeek=a.ActionMaskPerWeek,@ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1031,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
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
		RETURN 2
	END

	-- 校验掩码
	IF (@ActionMaskEver&0x0004)<>0
	BEGIN
		-- 读取配置
		DECLARE @ModifyUserFaceCost INT
		SELECT @ModifyUserFaceCost=-CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ModifyUserFaceCost'

		-- 调整变量
		IF @ModifyUserFaceCost IS NULL SET @ModifyUserFaceCost=-400
				
		-- 写入财富
		DECLARE @ResultCode INT
		EXEC @ResultCode=GSP_GP_WriteGameWealth @dwUserID,1,18,@ModifyUserFaceCost

		-- 校验结果
		IF @ResultCode<>0
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1322,"Msg":"您身上的黄金余额不足'+CONVERT(NVARCHAR(12),-@ModifyUserFaceCost)+'，修改头像失败！",Ops":["'+CONVERT(NVARCHAR(12),-@ModifyUserFaceCost)+'"]}'
			RETURN 4
		END
	END

	-- 设置掩码
	IF (@ActionMaskEver&0x0004)=0 SET @ActionMaskEver|=0x0004

	-- 更新头像
	UPDATE AccountsInfo SET CustomID=@dwCustomID,ActionMaskEver=@ActionMaskEver WHERE UserID=@dwUserID

	-- 更新地址
	UPDATE AccountsFace SET CustomID=@dwCustomID,FacePath=@strFacePath,UpdateTime=GETDATE() WHERE UserID=@dwUserID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO AccountsFace(UserID,CustomID,FacePath,UpdateTime,CollectTime)
		VALUES (@dwUserID,@dwCustomID,@strFacePath,GETDATE(),GETDATE())
	END	

	-- 读取黄金
	DECLARE @UserGold BIGINT
	SELECT @UserGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 调整变量
	IF @UserGold IS NULL SET @UserGold=0

	-- 返回结果
	SELECT FaceID, CustomID, @ActionMaskEver AS ActionMaskEver, @ActionMaskPerDay AS ActionMaskPerDay, @ActionMaskPerWeek AS ActionMaskPerWeek, @UserGold AS UserGold FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
	

END	

RETURN 0

GO

----------------------------------------------------------------------------------------------------
