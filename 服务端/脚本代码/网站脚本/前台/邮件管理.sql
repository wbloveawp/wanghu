----------------------------------------------------------------------
-- 时间：2021-11-01
-- 用途：邮件管理
----------------------------------------------------------------------

USE [LHAccountsDB]
GO

-- 发送邮件
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].WEB_PW_SendMail') AND OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].WEB_PW_SendMail
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

----------------------------------------------------------------------------------
-- 发送邮件
CREATE PROCEDURE WEB_PW_SendMail
	@dwUserID INT,								-- 用户标识
	@dwMailKind	TINYINT,						-- 邮件类型（1： 通知类型  2： 附件类型）	
	@dwMailType TINYINT,						-- 邮件分类（1、推广奖励邮件 2、比赛奖励邮件 3、排位奖励邮件 4、系统赠送邮件）
	@strMailTitle NVARCHAR(32),					-- 邮件标题
	@strMailContent	NVARCHAR(512),				-- 邮件内容
	@dwSendTime	INT,							-- 发送时间戳
	@dwExpireTime INT,							-- 过期时间戳

	@dwGoodsID	INT,							-- 物品标识
	@dwGoodsCount INT,							-- 物品数量

	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID			INT
DECLARE @Nullity		TINYINT

-- 执行逻辑
BEGIN
	-- 查询用户	
	SELECT @UserID=UserID,@Nullity=Nullity FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
	
	-- 用户存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试！'
		RETURN 100
	END	

	-- 帐号封禁
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 101
	END		

	-- 插入邮件内容
	INSERT INTO AccountsMailInfo(MailKind,MailType,MailTitle,MailContent,SendTime,ExpireTime,CollectTime) VALUES(@dwMailKind,@dwMailType,@strMailTitle,@strMailContent,@dwSendTime,@dwExpireTime,GETDATE())

	-- 记录标识
	DECLARE @MailID INT
	SET @MailID=SCOPE_IDENTITY()

	-- 插入用户邮件
	INSERT INTO AccountsMailBox(UserID,MailID,MailState,ExpireTime,UpdateTime) VALUES(@dwUserID,@MailID,1,@dwExpireTime,GETDATE())

	-- 插入附件
	IF @dwMailKind=2
	BEGIN
		INSERT INTO AccountsMailAttach(MailID,GoodsID,GoodsLevel,GoodsCount,GoodsIndate) VALUES(@MailID,@dwGoodsID,0,@dwGoodsCount,0)
	END

	-- 输出参数
	SELECT @MailID AS MailID,@UserID AS UserID
END
RETURN 0
GO