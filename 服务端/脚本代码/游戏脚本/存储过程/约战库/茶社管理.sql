----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_CreateClub]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_CreateClub]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DeleteClub]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DeleteClub]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_SearchClub]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_SearchClub]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_TransferClub]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_TransferClub]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UpdateClubProperty]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UpdateClubProperty]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UpdateClubRules]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UpdateClubRules]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UpdateClubNotice]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UpdateClubNotice]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- 创建群组
CREATE  PROCEDURE dbo.GSP_GR_CreateClub
	-- 用户信息
	@dwCreaterID INT,							-- 用户标识
	
	-- 群组属性
	@wClubKind SMALLINT,						-- 群组类型
	@wClubIconID SMALLINT,						-- 群组图标
	@strClubName NVARCHAR(32),					-- 群组名称
	@strClubIntroduce NVARCHAR(128),			-- 群组介绍

	-- 群组规则
	@cbPayType	TINYINT,						-- 支付方式
	@cbCurrencyKind TINYINT,					-- 货币类型
	@dwSwitchOption INT,						-- 开关选项

	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @AgentID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	DECLARE @UserRight INT
	
	-- 查询用户
	SELECT @UserID=a.UserID,@GameID=a.GameID,@UserRight=a.UserRight, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus, @NickName=a.NickName
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwCreaterID

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

	-- 群组类型
	DECLARE @strClubOptionName NVARCHAR(32)

	-- 选项名称
	IF @wClubKind=0 SET @strClubOptionName=N'ClubOption_Gold'
	IF @wClubKind=1 SET @strClubOptionName=N'ClubOption_Normal'
	IF @wClubKind=2 SET @strClubOptionName=N'ClubOption_Advanced'

	-- 变量定义
	DECLARE @ScoreKind TINYINT	
	DECLARE @PayTypeMask TINYINT	
	DECLARE @CurrencyKindMask TINYINT
	DECLARE @MaxMemberCount SMALLINT	
	DECLARE @MaxCreateClubCount SMALLINT	
	DECLARE @CreateNeedUserRight INT
	DECLARE @CreateClubTakeCard BIGINT
	DECLARE @CreateClubDeductCard BIGINT	

	-- 查询配置
	SELECT @MaxMemberCount=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'ClubOption' AND StatusName='MaxMemberCount'	
	SELECT @MaxCreateClubCount=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'ClubOption' AND StatusName='MaxCreateClubCount'		

	-- 查询配置	
	SELECT @ScoreKind=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='ScoreKind'			
	SELECT @PayTypeMask=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='PayTypeMask'
	SELECT @CurrencyKindMask=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='CurrencyKindMask'			
	SELECT @CreateNeedUserRight=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='CreateNeedUserRight'
	SELECT @CreateClubTakeCard=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='CreateClubTakeCard'	
	SELECT @CreateClubDeductCard=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='CreateClubDeductCard'						

	-- 调整配置	
	IF @ScoreKind IS NULL SET @ScoreKind=0	
	IF @PayTypeMask IS NULL SET @PayTypeMask=0	
	IF @CurrencyKindMask IS NULL SET @CurrencyKindMask=0
	IF @MaxMemberCount IS NULL SET @MaxMemberCount=0	
	IF @MaxCreateClubCount IS NULL SET @MaxCreateClubCount=0
	IF @CreateNeedUserRight IS NULL SET @CreateNeedUserRight=0
	IF @CreateClubTakeCard IS NULL SET @CreateClubTakeCard=0
	IF @CreateClubDeductCard IS NULL SET @CreateClubDeductCard=0

	-- 校验权限
	IF @CreateNeedUserRight>0 AND (@UserRight&@CreateNeedUserRight)=0
	BEGIN
		-- 错误信息
		SET @strErrorDescribe=N'{"Idx":1155,"Msg":"由于系统原因, 茶社创建失败[1]， 请联系客户服务中心了解详细情况！","Ops":["1"]}'
		RETURN 5
	END
	
	-- 查询财富
	DECLARE @UserCard BIGINT	
	SELECT @UserCard=Score FROM GameCardInfo(NOLOCK) WHERE UserID=@dwCreaterID

	-- 调整财富
	IF @UserCard IS NULL SET @UserCard=0
	
	-- 判断房卡
	IF @UserCard<@CreateClubTakeCard
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1150,"Msg":"抱歉，您身上携带的房卡不足'+CONVERT(NVARCHAR(18),@CreateClubTakeCard)+',茶社创建失败！","Ops":["'+CONVERT(NVARCHAR(18),@CreateClubTakeCard)+'"]}'
		RETURN 5		
	END

	-- 判断房卡
	IF @UserCard<@CreateClubDeductCard
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1151,"Msg":"抱歉，当前帐号携带的房卡少于'+CONVERT(NVARCHAR(18),@CreateClubDeductCard)+',茶社创建失败！","Ops":["'+CONVERT(NVARCHAR(18),@CreateClubDeductCard)+'"]}'
		RETURN 5		
	END

	-- 创建数量
	DECLARE @wCreateClubCount SMALLINT
	SELECT @wCreateClubCount=COUNT(ClubID) FROM ClubProperty(NOLOCK) WHERE CreaterID=@dwCreaterID AND ClubStatus=1

	-- 判断数量
	IF @wCreateClubCount>=@MaxCreateClubCount
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1152,"Msg":"抱歉，您创建的茶社数目已经达到上限，如有疑问，请联系客服！"}'
		RETURN 5		
	END

	-- 校验茶社名
	IF EXISTS(SELECT 1 FROM ClubProperty(NOLOCK) WHERE ClubName=@strClubName)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1153,"Msg":"系统中已经存在相同名字的茶社，创建失败！"}'
		RETURN 6		
	END

	-- 校验支付方式
	IF @cbPayType=0 OR ((@PayTypeMask&@cbPayType)<>@cbPayType)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1154,"Msg":"您设置的支付类型与系统配置不符[1]，创建失败！"}'
		RETURN 5				
	END

	-- 校验货币类型
	IF @cbCurrencyKind=0 OR ((@CurrencyKindMask&@cbCurrencyKind)<>@cbCurrencyKind)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1154,"Msg":"您设置的支付类型与系统配置不符[2]，创建失败！"}'
		RETURN 5				
	END	

	-- 插入群组
	DECLARE @CreateDateTime DATETIME
	SET @CreateDateTime=GetDate()

	-- 查询群组ID
	DECLARE @dwClubID INT	
	SELECT TOP 1 @dwClubID=ClubID FROM NormalClubID(NOLOCK) WHERE Distribute=0
	IF @dwClubID IS NULL
	BEGIN		
		-- 错误信息
		SET @strErrorDescribe=N'{"Idx":1155,"Msg":"由于系统原因, 茶社创建失败[2]， 请联系客户服务中心了解详细情况！","Ops":["2"]}'
		RETURN 5
	END

	-- 更新状态
	UPDATE NormalClubID SET Distribute=1 WHERE ClubID=@dwClubID

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN		

	-- 群组属性
	INSERT INTO ClubProperty(ClubID,CreaterID, CreaterGameID, ClubName, ClubKind, ClubStatus, ClubIconID, ClubIntroduce, MemberCount, MaxMemberCount, CreateDateTime)
	VALUES (@dwClubID, @dwCreaterID, @GameID, @strClubName, @wClubKind, 0, @wClubIconID, @strClubIntroduce, 0, @MaxMemberCount, @CreateDateTime)
	IF @@ERROR<>0
	BEGIN
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		-- 错误信息
		SET @strErrorDescribe=N'{"Idx":1155,"Msg":"由于系统原因,茶社创建失败[3]，请联系客户服务中心了解详细情况！","Ops":["3"]}'
		RETURN 6
	END	

	-- 群组规则
	INSERT INTO ClubRules(ClubID, PayType, CurrencyKind, ScoreKind, BattleMask, SwitchOption) 
	VALUES (@dwClubID, @cbPayType, @cbCurrencyKind, @ScoreKind, 0, @dwSwitchOption)	
	IF @@ERROR<>0
	BEGIN
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		-- 错误信息
		SET @strErrorDescribe=N'{"Idx":1155,"Msg":"由于系统原因,茶社创建失败[4]，请联系客户服务中心了解详细情况！","Ops":["4"]}'
		RETURN 6
	END	

	-- 群组公告
	INSERT INTO ClubNotice(ClubID, PublishState, PublishMode, NoticeContent) VALUES (@dwClubID, 0, 0, N'')	
	IF @@ERROR<>0
	BEGIN
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		-- 错误信息
		SET @strErrorDescribe=N'{"Idx":1155,"Msg":"由于系统原因,茶社创建失败[5]，请联系客户服务中心了解详细情况！","Ops":["5"]}'
		RETURN 6
	END

	-- 扣除房卡
	IF @CreateClubDeductCard>0
	BEGIN
		-- 变更记录
		INSERT INTO LHGameRecordDB.dbo.RecordChangeCard (UserID,GameID,TypeID,KindID,ClubID,BeforeCard,ChangeCard,CollectClientIP,CollectDateTime)
		VALUES (@dwCreaterID,@GameID,6,0,@dwClubID,@UserCard,@CreateClubDeductCard,N'--------------',GETDATE())

		-- 更新房卡
		UPDATE GameCardInfo SET Score-=@CreateClubDeductCard WHERE UserID=@dwCreaterID			
	END	

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED		

	-- 查询财富	
	SELECT @UserCard=Score FROM GameCardInfo(NOLOCK) WHERE UserID=@dwCreaterID

	-- 调整财富
	IF @UserCard IS NULL SET @UserCard=0

	-- 查询信息
	SELECT a.*, b.FaceID AS CreaterFaceID, b.CustomID AS CreaterCustomID, b.NickName AS CreaterNickName, @UserCard AS UserCard 
	FROM ClubProperty(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsInfo(NOLOCK) b ON a.CreaterID=b.UserID  WHERE a.ClubID=@dwClubID
	
END

RETURN 0

GO



----------------------------------------------------------------------------------------------------

-- 删除群组
CREATE  PROCEDURE dbo.GSP_GR_DeleteClub
	-- 用户信息
	@dwCreaterID INT,							-- 用户标识
	@dwTargetClubID INT,						-- 群组标识
	
	-- 输出参数
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- 变量定义
DECLARE	@TableID	INT;																			
DECLARE	@TableName  NVARCHAR(128);								
DECLARE @ExecSqlstr	NVARCHAR(512);	

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	
	SELECT @UserID=a.UserID,@GameID=a.GameID,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwCreaterID

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

	-- 查询群主
	DECLARE @CreaterID INT
	DECLARE @ClubName NVARCHAR(32)
	SELECT @ClubName=ClubName,@CreaterID=CreaterID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwTargetClubID

	-- 结果判断
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'
		RETURN 5		
	END

	-- 不是群主
	IF @CreaterID <> @dwCreaterID 
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1157,"Msg":"抱歉，您不是该茶社的老板，没有权限进行删除！"}'
		RETURN 5		
	END

	-- 修改状态
	UPDATE ClubProperty SET ClubStatus=2,CollectDateTime=GETDATE() WHERE ClubID=@dwTargetClubID

	-- 用户通知
	-- INSERT INTO RecordUserNotice(UserID,NoticeKind,NoticeContent,CollectDateTime) VALUES(@UserID,5,N'{"Idx":1214,"Ops":["'+@ClubName+'"]}',GETDATE())	

	-- 分表标识
	SET	@TableID=dbo.LHF_GetTableID(@dwTargetClubID);

	-- 群组成员表
	SET	@TableName='ClubMember_';
	SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

	-- 构造标识
	DECLARE @strClubID NVARCHAR(20)
	SET @strClubID=CONCAT(';',CONVERT(NVARCHAR(18),@dwTargetClubID),';');

	-- 更新数据
	SET	@ExecSqlstr = CONCAT( '  INSERT INTO RecordUserNotice(UserID,NoticeKind,NoticeContent,CollectDateTime) ',
							  '  SELECT MemberID,5,',N'''{"Idx":1214,"Ops":["'+@ClubName+'"]}',''',GETDATE() FROM  ',@TableName,' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwTargetClubID))	
	EXECUTE(@ExecSqlstr);


	-- 更新数据
	SET	@ExecSqlstr = CONCAT( '  UPDATE ClubUserInfo SET ClubIDArray=REPLACE(ClubIDArray,''',@strClubID,''','';''',')',
							  '  WHERE UserID IN (SELECT MemberID FROM  ',@TableName,' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwTargetClubID),')')	
						  
	EXECUTE(@ExecSqlstr);		
	
	SELECT @ClubName AS ClubName					  
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 查找群组
CREATE  PROCEDURE dbo.GSP_GR_SearchClub
	-- 查找信息
	@dwClubID INT,								-- 群组标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON
-- 执行逻辑
BEGIN

	-- 查询茶社
	DECLARE @ClubID INT
	SELECT @ClubID=ClubID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID AND ClubStatus=1

	-- 校验茶社
	IF @ClubID IS NULL
	BEGIN
	   SET @strErrorDescribe=N'{"Idx":1158,"Msg":"抱歉，您查找的茶社信息不存在，请查证后再次尝试！"}'
	   RETURN 1	
	END

	-- 查询信息
	SELECT a.*,b.FaceID AS CreaterFaceID, b.CustomID AS CreaterCustomID, b.NickName AS CreaterNickName 
	FROM ClubProperty(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsInfo(NOLOCK) b ON a.CreaterID=b.UserID WHERE a.ClubID=@dwClubID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 转让群组
CREATE  PROCEDURE dbo.GSP_GR_TransferClub
	-- 用户信息
	@dwClubID	INT,							-- 群组标识	
	@dwCreaterID INT,							-- 用户标识		
	@dwNewCreaterID INT,						-- 用户标识
	@strAuthCodeMD5 NCHAR(32),					-- 验证码 
	
	-- 输出参数
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- 变量定义
DECLARE	@TableID	INT;																			
DECLARE	@TableName  NVARCHAR(128);								
DECLARE @ExecSqlstr	NVARCHAR(512);	

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	DECLARE @LogonPass NCHAR(32)		
	DECLARE @MobilePhone NVARCHAR(11)	

	-- 查询用户
	SELECT @UserID=a.UserID,@GameID=a.GameID,@NickName=a.NickName,@Nullity=a.Nullity,@MobilePhone=a.MobilePhone, @ShutDown=b.ShutDownStatus
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwCreaterID

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

	-- 校验手机
	IF @MobilePhone=N'' OR LEN(@MobilePhone)<>11
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1159,"Msg":"您的帐号没有绑定手机号，请先绑定手机号再进行转让！"}'
		RETURN 4		
	END

	-- 查询手机号
	DECLARE @ATLastAcquireDate DATETIME
	DECLARE @LastMobilePhone NVARCHAR(11)		
	SELECT @LastMobilePhone=LastMobileEmail,@ATLastAcquireDate=LastAcquireDate FROM LHAccountsDB.dbo.AccountsAuthInfo(NOLOCK) WHERE UserID=@dwCreaterID AND CodeKind=1
	
	-- 校验记录
	IF @LastMobilePhone IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1160,"Msg":"验证码错误，茶社转让失败[1]！","Ops":["1"]}'
		RETURN 5
	END

	-- 校验手机号
	IF @LastMobilePhone<>@MobilePhone
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1160,"Msg":"验证码错误，茶社转让失败[2]！","Ops":["2"]}'		
		RETURN 5
	END

	-- 验证信息
	DECLARE @AuthKind TINYINT	
	DECLARE @AuthCodeMD5 NVARCHAR(32)
	DECLARE @AcquireMachine NVARCHAR(32)
	DECLARE @MBLastAcquireDate DATETIME
	SELECT @AuthKind=AuthKind, @AuthCodeMD5=AuthCodeMD5, @AcquireMachine=AcquireMachine, @MBLastAcquireDate=LastAcquireDate FROM LHAccountsDB.dbo.RecordAuthCode(NOLOCK) WHERE MobileEmail=@MobilePhone AND CodeKind=1

	-- 校验类型
	IF @AuthKind<>6
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1160,"Msg":"验证码错误，茶社转让失败[3]！","Ops":["3"]}'	
		RETURN 6		
	END

	-- 校验验证码
	IF @AuthCodeMD5<>@strAuthCodeMD5
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1160,"Msg":"验证码错误，茶社转让失败[4]！","Ops":["4"]}'	
		RETURN 7				
	END	

	-- 有效时长
	DECLARE @ACValidSeconds INT
	SELECT @ACValidSeconds=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'ACValidSecond'

	-- 调整参数
	IF @ACValidSeconds IS NULL SET @ACValidSeconds=600

	-- 时间间隔
	DECLARE @SecondDiff INT
	SET @SecondDiff=DateDiff(s,@MBLastAcquireDate,GetDate())

	-- 时间限制
	IF @SecondDiff>@ACValidSeconds	
	BEGIN			
		SET @strErrorDescribe=N'{"Idx":1029,"Msg":"验证码已过期，请重新获取验证码！"}'
		RETURN 8
	END

	-- 查询群主
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- 结果判断
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'
		RETURN 5		
	END

	-- 不是群主
	IF @CreaterID <> @dwCreaterID 
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1162,"Msg":"抱歉，您不是该茶社的老板，没有权限进行转让！"}'
		RETURN 5		
	END

	-- 分表标识
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- 群组成员表
	SET	@TableName ='ClubMember_';
	SET	@TableName = CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

	-- 查询成员类型
	DECLARE @OUT_MemberType SMALLINT	
	SET	@ExecSqlstr = CONCAT(' SELECT @MemberType=MemberType FROM ',@TableName,'(NOLOCK)',
						     ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwNewCreaterID))				  
	
	-- 执行语句
	EXEC SP_EXECUTESQL @stmt=@ExecSqlstr,@params=N'@MemberType TINYINT OUT',@MemberType=@OUT_MemberType OUTPUT

	-- 存在校验
	IF @OUT_MemberType IS NULL 
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1163,"Msg":"茶社转让失败！新老板必须是该茶社的成员。"}'
		RETURN 6
	END

	-- 新馆主信息
	DECLARE @CreaterGameID INT
	DECLARE @CreaterNickName NVARCHAR(32)	
	SELECT @CreaterGameID=GameID, @CreaterNickName=NickName FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwNewCreaterID
	
	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 修改状态
	UPDATE ClubProperty SET CreaterID=@dwNewCreaterID,CreaterGameID=@CreaterGameID WHERE ClubID=@dwClubID	

	-- 更新新老板信息
	SET	@ExecSqlstr = CONCAT(' UPDATE ',@TableName,' SET MemberType=4',
							 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwNewCreaterID),CHAR(13))	
	EXECUTE(@ExecSqlstr);	
	IF @@ERROR<>0
	BEGIN
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
		
		SET @strErrorDescribe=N'{"Idx":1164,"Msg":"由于系统原因,茶社转让失败[1]，请联系客服了解详细情况！","Ops":["1"]}'	
		RETURN 8
	END

	-- 更新旧老板成员类型和上级
	SET	@ExecSqlstr = CONCAT(' UPDATE ',@TableName,' SET MemberType=0',
							 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwCreaterID),CHAR(13))	
	EXECUTE(@ExecSqlstr);	
	IF @@ERROR<>0
	BEGIN
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		-- 错误信息
		SET @strErrorDescribe=N'{"Idx":1164,"Msg":"由于系统原因,茶社转让失败[2]，请联系客服了解详细情况！","Ops":["2"]}'	
		RETURN 9
	END	
	
	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- 更新信息
	UPDATE LHAccountsDB.dbo.AccountsAuthInfo SET AuthTimes=AuthTimes+1,AcquireTimes=0, LastAcquireDate=GetDate(),LastMobileEmail=N'' WHERE UserID=@dwCreaterID AND CodeKind=1

	-- 更新记录
	UPDATE LHAccountsDB.dbo.RecordAuthCode SET AuthCode=N'',AuthCodeMD5=N'', AuthTimes=AuthTimes+1,AcquireTimes=0 WHERE MobileEmail=@MobilePhone AND CodeKind=1

	-- 转让成功
	SET @strErrorDescribe=N'{"Idx":1165,"Msg":"茶社转让成功！"}'
				  
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 更新属性
CREATE  PROCEDURE dbo.GSP_GR_UpdateClubProperty
	-- 用户信息
	@dwUserID INT,								-- 用户标识
	@dwClubID INT,								-- 群组标识

	-- 群组信息
	@wClubIconID SMALLINT,						-- 群组图标
	@strClubName NVARCHAR(32),					-- 群组名称
	@strClubIntroduce NVARCHAR(128),			-- 群组介绍	
	
	-- 输出参数
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	
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

	-- 查询群主
	DECLARE @CreaterID INT
	DECLARE @ClubName NVARCHAR(32)
	DECLARE @ClubKind TINYINT
	DECLARE @ClubIconID SMALLINT
	DECLARE @ClubIntroduce NVARCHAR(128)
	SELECT @CreaterID=CreaterID,@ClubKind=ClubKind,@ClubIconID=ClubIconID,@ClubName=ClubName,@ClubIntroduce=ClubIntroduce FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID AND ClubStatus=1

	-- 结果判断
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'
		RETURN 5		
	END

	-- 不是群主
	IF @CreaterID <> @dwUserID 
	BEGIN
		-- 变量定义
		DECLARE	@TableID	INT;																			
		DECLARE	@TableName	NVARCHAR(128);								
		DECLARE @Sqlstr		NVARCHAR(512);

		-- 分表标识
		SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

		-- 群组成员表
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

		-- 查询成员类型
		DECLARE @OUT_OperaterType SMALLINT	
		DECLARE @OUT_OperaterRight SMALLINT	
		SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType,@ManageRight=ManageRight FROM ',@TableName,'(NOLOCK)',
							 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwUserID))				  
	
		-- 执行语句
		EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT,@ManageRight INT OUT',@MemberType=@OUT_OperaterType OUTPUT,@ManageRight=@OUT_OperaterRight OUTPUT

		-- 调整变量
		IF @OUT_OperaterType IS NULL SET @OUT_OperaterType=0

		-- 校验权限
		IF @OUT_OperaterType<3 OR (@OUT_OperaterType=3 AND (@OUT_OperaterRight&0x0001)=0)
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1166,"Msg":"抱歉，您没有权限修改茶社！"}'
			RETURN 5		
		END			
	END

	-- 更新图标
	IF @wClubIconID<>@ClubIconID
	BEGIN
		UPDATE ClubProperty SET ClubIconID=@wClubIconID WHERE ClubID=@dwClubID
	END

	-- 修改茶社名称
	IF @ClubName<>@strClubName
	BEGIN
		-- 茶社选项
		DECLARE @strClubOptionName NVARCHAR(32)

		-- 茶社选项
		IF @ClubKind=0 SET @strClubOptionName=N'ClubOption_Gold'		
		IF @ClubKind=1 SET @strClubOptionName=N'ClubOption_Normal'
		IF @ClubKind=2 SET @strClubOptionName=N'ClubOption_Advanced'

		-- 变量定义		
		DECLARE @ModifyClubNameDeductCard BIGINT
		SELECT @ModifyClubNameDeductCard=CONVERT(TINYINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='ModifyClubNameDeductCard'

		-- 调整配置	
		IF @ModifyClubNameDeductCard IS NULL SET @ModifyClubNameDeductCard=0

		-- 变量定义
		DECLARE @UserCard BIGINT		
		SELECT @UserCard=Score FROM GameCardInfo(NOLOCK) WHERE UserID=@CreaterID

		-- 调整财富
		IF @UserCard IS NULL SET @UserCard=0

		-- 校验房卡
		IF @UserCard<@ModifyClubNameDeductCard
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1167,"Msg":"抱歉，茶社老板携带的房卡不足，茶社名称修改失败！"}'
			IF @CreaterID=@dwUserID SET @strErrorDescribe=N'{"Idx":1168,"Msg":"抱歉，您身上携带的房卡不足，茶社名称修改失败！"}'
			RETURN 6
		END

		-- 扣除房卡
		IF @ModifyClubNameDeductCard>0
		BEGIN
			-- 变更记录
			INSERT INTO LHGameRecordDB.dbo.RecordChangeCard (UserID,GameID,TypeID,KindID,ClubID,BeforeCard,ChangeCard,CollectClientIP,CollectDateTime)
			VALUES (@CreaterID,@GameID,7,0,@dwClubID,@UserCard,-@ModifyClubNameDeductCard,N'--------------',GETDATE())

			-- 更新房卡
			UPDATE GameCardInfo SET Score-=@ModifyClubNameDeductCard WHERE UserID=@CreaterID
		END

		-- 重复判断
		IF EXISTS (SELECT 1 FROM ClubProperty(NOLOCK) WHERE ClubName=@strClubName)
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1169,"Msg":"抱歉，茶社名称已被占用，请重新输入后再次尝试！"}'
			RETURN 6
		END		

		-- 更新名称
		UPDATE ClubProperty SET ClubName=@strClubName WHERE ClubID=@dwClubID			
	END

	-- 修改茶社介绍
	IF @ClubIntroduce<>@strClubIntroduce
	BEGIN
		-- 更新介绍
		UPDATE ClubProperty SET ClubIntroduce=@strClubIntroduce WHERE ClubID=@dwClubID		
	END	

	-- 查询信息
	SELECT a.*,b.FaceID AS CreaterFaceID, b.CustomID AS CreaterCustomID, b.NickName AS CreaterNickName 
	FROM ClubProperty(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsInfo(NOLOCK) b ON a.CreaterID=b.UserID WHERE a.ClubID=@dwClubID
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 更新规则
CREATE  PROCEDURE dbo.GSP_GR_UpdateClubRules
	-- 用户信息
	@dwUserID INT,								-- 用户标识
	@dwClubID INT,								-- 群组标识

	-- 群组规则
	@cbBattleMask TINYINT,						-- 约战掩码		
	@wSwitchOption SMALLINT,					-- 开关选项
	
	-- 输出参数
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	

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

	-- 查询群主
	DECLARE @ClubKind SMALLINT
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID,@ClubKind=ClubKind FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID AND ClubStatus=1

	-- 结果判断
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'
		RETURN 5		
	END

	-- 不是群主
	IF @CreaterID <> @dwUserID
	BEGIN
		-- 变量定义
		DECLARE	@TableID	INT;																			
		DECLARE	@TableName	NVARCHAR(128);								
		DECLARE @Sqlstr		NVARCHAR(512);

		-- 分表标识
		SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

		-- 群组成员表
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

		-- 查询成员类型
		DECLARE @OUT_OperaterType SMALLINT	
		DECLARE @OUT_OperaterRight SMALLINT	
		SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType,@ManageRight=ManageRight FROM ',@TableName,'(NOLOCK)',
							 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwUserID))				  
	
		-- 执行语句
		EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT,@ManageRight INT OUT',@MemberType=@OUT_OperaterType OUTPUT,@ManageRight=@OUT_OperaterRight OUTPUT

		-- 调整变量
		IF @OUT_OperaterType IS NULL SET @OUT_OperaterType=0

		-- 校验权限
		IF @OUT_OperaterType<3 OR (@OUT_OperaterType=3 AND (@OUT_OperaterRight&0x0001)=0)
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1166,"Msg":"抱歉，您没有权限修改茶社！"}'
			RETURN 5		
		END		
	END

	-- 更新规则
	UPDATE ClubRules SET BattleMask=@cbBattleMask,SwitchOption=@wSwitchOption WHERE ClubID=@dwClubID

	-- 查询信息
	SELECT * FROM ClubRules(NOLOCK) WHERE ClubID=@dwClubID	
	
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 更新公告
CREATE  PROCEDURE dbo.GSP_GR_UpdateClubNotice
	-- 用户信息	
	@dwUserID	INT,							-- 用户标识
	@dwClubID	INT,							-- 茶社标识	
	@cbPublishMode TINYINT,						-- 发布模式
	@cbPublishState TINYINT,					-- 发布状态
	@strNoticeContent NVARCHAR(128),			-- 公告内容	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 查询用户
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- 查询用户
	SELECT @UserID=a.UserID,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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

	-- 查询群主
	DECLARE @CreaterID INT	
	DECLARE @ClubKind SMALLINT
	SELECT @CreaterID=CreaterID,@ClubKind=ClubKind FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID AND ClubStatus=1

	-- 结果判断
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'
		RETURN 5		
	END

	-- 不是群主
	IF @CreaterID <> @dwUserID 
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1209,"Msg":"您不是茶社的老板，没有权限修改公告！"}'
		RETURN 6
	END

	-- 更新公告
	UPDATE ClubNotice SET PublishMode=@cbPublishMode,PublishState=@cbPublishState,NoticeContent=@strNoticeContent WHERE ClubID=@dwClubID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO ClubNotice(ClubID,PublishState,PublishMode,NoticeContent) VALUES(@dwClubID,@cbPublishState,@cbPublishMode,@strNoticeContent)
	END
	
	-- 抛出记录
	SELECT * FROM ClubNotice(NOLOCK) WHERE ClubID=@dwClubID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------