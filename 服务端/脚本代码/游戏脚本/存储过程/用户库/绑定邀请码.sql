
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_BindParent]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_BindParent]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_BindInviteCode]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_BindInviteCode]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 绑定上级
CREATE PROC GSP_MB_BindParent
	@dwUserID INT,								-- 用户 I D
	@dwParentUserID INT,						-- 用户 I D	
	@strClientIP NVARCHAR(15),					-- 连接地址
	@dwMailID BIGINT OUTPUT
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询代理
	DECLARE @ParentGameID BIGINT
	DECLARE @PParentUserID BIGINT
	SELECT @ParentGameID=GameID,@PParentUserID=ParentID FROM AccountsInfo(NOLOCK) WHERE UserID=@dwParentUserID
	IF @ParentGameID IS NULL 
	BEGIN
		SET @dwMailID=0
		RETURN 1
	END 	

	-- 调整变量
	IF @PParentUserID IS NULL SET @PParentUserID=0
	
	---- 变量定义
	--DECLARE @ExpireTime INT
	--DECLARE @CurrentTime INT
	--DECLARE @RewardGold BIGINT	
	--DECLARE @MailIndate BIGINT = 30*24*3600		

	---- 计算时间
	--SET @CurrentTime=dbo.LHF_GetTimestamp()
	--SET @ExpireTime=@CurrentTime+@MailIndate
	
	---- 查询绑定奖励
	--SELECT @RewardGold=CONVERT(BIGINT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'BindInviteCodeReward'

	---- 调整奖励
	--IF @dwMailID IS NULL SET @dwMailID=0
	--IF @RewardGold IS NULL SET @RewardGold=0

	-- 奖励邮件
	--IF @RewardGold>0
	--BEGIN
	--	-- 邮件内容
	--	DECLARE @MailTitle NVARCHAR(128)
	--	DECLARE @MailContent NVARCHAR(512)

	--	-- 构造内容
	--	SET @MailTitle=N'Binding reward'
	--	SET @MailContent=N'{"Ops":["'+CONVERT(INT,@ParentGameID)+'","'+CONVERT(BIGINT,@RewardGold)+'"]}'

	--	-- 插入邮件信息
	--	INSERT INTO AccountsMailInfo(MailKind,MailType,MailTitle,MailContent,SendTime,ExpireTime,CollectTime)
	--	VALUES (2,1,@MailTitle,@MailContent,@CurrentTime,@ExpireTime,GetDate())

	--	-- 邮件标识
	--	SET @dwMailID=SCOPE_IDENTITY()		

	--	-- 插入附件
	--	INSERT INTO AccountsMailAttach(MailID,GoodsID,GoodsCount,GoodsIndate) VALUES (@dwMailID,2,@RewardGold,0)

	--	-- 插入用户邮箱
	--	INSERT INTO LHAccountsDB.dbo.AccountsMailBox(UserID,MailID,MailState,ExpireTime,UpdateTime)
	--	VALUES (@dwUserID,@dwMailID,1,@ExpireTime,GetDate())
	--END
	
	-- 日期标识
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 更新上级推广统计
	UPDATE AccountsSpreadStat SET SpreadLowerCount+=1 WHERE UserID=@dwParentUserID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO AccountsSpreadStat(UserID,SpreadLowerCount,SpreadPayUser,SparedPayAmount,CollectDateTime) VALUES(@dwParentUserID,1,0,0,GETDATE())
	END

	-- 更新代理上级
	IF @PParentUserID<>0
	BEGIN
		UPDATE AccountsSpreadStat SET SpreadLLowerCount+=1 WHERE UserID=@PParentUserID
		IF @@ROWCOUNT=0
		BEGIN
			INSERT INTO AccountsSpreadStat(UserID,SpreadLLowerCount,SpreadPayUser,SparedPayAmount,CollectDateTime) VALUES(@PParentUserID,1,0,0,GETDATE())
		END
	END
		
	---- 记录自己推广奖励
	--UPDATE LHGameRecordDB.dbo.ReportDayUser SET BindAward=BindAward+@RewardGold WHERE UserID=@dwUserID AND DateID=@DateID
	--IF @@ROWCOUNT=0
	--BEGIN
	--	INSERT INTO LHGameRecordDB.dbo.ReportDayUser(DateID,UserID,PayCount,PayAmount,LastPayAmount,LastPayDate,SpreadCount,
	--	SpreadValidCount,SpreadValidAward,SpreadPayUser,SpreadPayAmount,SpreadPayAward,BindAward,ReportDateTime) 
	--	VALUES (@DateID,@dwUserID,0,0,0,GETDATE(),0,0,0,0,0,0,@RewardGold,GETDATE())
	--END
	
	-- 记录上级推广数
	UPDATE LHGameRecordDB.dbo.ReportDayUser SET SpreadLowerCount+=1 WHERE UserID=@dwParentUserID AND DateID=@DateID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO LHGameRecordDB.dbo.ReportDayUser(DateID,UserID,SpreadLowerCount,ReportDateTime) VALUES (@DateID,@dwParentUserID,1,GETDATE())
	END

	-- 记录上上级推广数
	IF @PParentUserID<>0
	BEGIN
		-- 记录上级推广数
		UPDATE LHGameRecordDB.dbo.ReportDayUser SET SpreadLLowerCount+=1 WHERE UserID=@PParentUserID AND DateID=@DateID
		IF @@ROWCOUNT=0
		BEGIN
			INSERT INTO LHGameRecordDB.dbo.ReportDayUser(DateID,UserID,SpreadLLowerCount,ReportDateTime) VALUES (@DateID,@PParentUserID,1,GETDATE())
		END
	END
	
	-- 更新上级
	UPDATE AccountsInfo SET ParentID=@dwParentUserID WHERE UserID=@dwUserID

	-- 插入记录
	INSERT INTO RecordBindParent(UserID,ParentID,TypeID,ClientIP,CollectDate) VALUES (@dwUserID,@dwParentUserID,0,@strClientIP,GetDate())

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 绑定邀请码
CREATE PROC GSP_MB_BindInviteCode
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码	
	@dwInviteCode INT,							-- 邀 请 码	
	@strMachine NVARCHAR(32),					-- 机器序列
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	DECLARE @LogonPass NCHAR(32)	
	DECLARE @ParentUserID INT
	SELECT @ParentUserID=a.ParentID,@UserID=a.UserID,@LogonPass=a.LogonPass, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM AccountsInfo(NOLOCK) a LEFT JOIN AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

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
	
	-- 密码判断	
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 4
	END	
	
	-- 校验上级
	IF @ParentUserID<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1292,"Msg":"抱歉，您已经绑定了上级，绑定邀请码失败！"}'
		RETURN 5
	END

	-- 查询用户
	DECLARE @ParentParentID INT
	DECLARE @ParentGameID INT
	DECLARE @ParentFaceID SMALLINT
	DECLARE @ParentNickName NVARCHAR(31)
	SELECT @ParentUserID=UserID,@ParentGameID=GameID,@ParentNickName=NickName,@ParentFaceID=FaceID,@ParentParentID=ParentID FROM AccountsInfo(NOLOCK) WHERE GameID=@dwInviteCode

	-- 校验标识
	IF @ParentUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1293,"Msg":"抱歉，该上级玩家不存在，绑定邀请码失败！"}'
		RETURN 6
	END
	
	-- 校验自身
	IF @ParentUserID=@UserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1294,"Msg":"抱歉，您不能绑定自己，绑定邀请码失败！"}'
		RETURN 7
	END
	
	-- 校验下级
	IF @ParentParentID=@UserID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1295,"Msg":"抱歉，您不能绑定自己邀请的玩家，绑定邀请码失败！"}'
		RETURN 8
	END
	
	DECLARE @MailID BIGINT
	EXEC GSP_MB_BindParent @dwUserID,@ParentUserID,@strClientIP,@MailID OUTPUT

	-- 绑定成功
	SET @strErrorDescribe=N'{"Idx":1302,"Msg":"绑定邀请码成功！"}'	

	-- 抛出上级信息
	SELECT @ParentGameID AS ParentGameID,@ParentNickName AS ParentNickName,@ParentFaceID AS ParentFaceID,@MailID AS MailID
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

