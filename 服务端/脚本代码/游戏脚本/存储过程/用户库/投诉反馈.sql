
USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_SubmitInformInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_SubmitInformInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- 提交投诉
CREATE PROCEDURE GSP_MB_SubmitInformInfo
	@cbInformKind		TINYINT,				-- 投诉类型
	@cbInformSource		TINYINT,				-- 投诉来源
	@dwInformUserID		INT,					-- 投诉玩家
	@dwBeInformUserID	INT,					-- 被投诉玩家
	@strAttachmentUrl	NVARCHAR(128),			-- 附件地址
	@strInformMessage	NVARCHAR(512),			-- 投诉消息
	@strInformContent	NVARCHAR(512),			-- 投诉内容
	@strClientIP		NVARCHAR(15),			-- 终端地址
	@strErrorDescribe	NVARCHAR(127) OUTPUT	-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 定义变量
	DECLARE @UserID			INT
	DECLARE @Nullity		BIT
	DECLARE @ShutDown		BIT
	DECLARE @BeInformGameID	INT

	-- 查询用户	
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON  a.UserID=b.UserID WHERE a.UserID=@dwInformUserID

	-- 查询用户
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

	-- 查询用户	
	SELECT @BeInformGameID=GameID FROM AccountsInfo(NOLOCK) WHERE UserID=@dwBeInformUserID

	-- 校验用户
	IF @BeInformGameID IS NULL
	BEGIN
		--SET @strErrorDescribe=N'{"Idx":1306,"Msg":"您投诉的用户信息不存在！"}'
		--RETURN 4	

		SET @BeInformGameID=0
	END	

	-- 插入数据
	INSERT INTO RecordInform(InformUserID,InformKindID,InformSource,InformMessage,InformContent,BeInformUserID,BeInformGameID,AttachmentUrl,ClientIP,CollectTime,[Status])
	VALUES (@dwInformUserID,@cbInformKind,@cbInformSource,@strInformMessage,@strInformContent,@dwBeInformUserID,@BeInformGameID,@strAttachmentUrl,@strClientIP,GetDate(),0)

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
