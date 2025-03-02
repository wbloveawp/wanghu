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
   @strBattleConfig NVARCHAR(512)				-- 约战玩法   
)
RETURNS TINYINT
AS
BEGIN

	-- 校验玩法
	IF ISJSON(@strBattleConfig)=0
	BEGIN
		RETURN 1
	END

	-- 读取配置
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

	-- 约战配置
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

	-- 查询配置
	SELECT @OptionID=OptionID,@OptionMask=OptionMask,@SuportPayType=SuportPayType,@MinEntryCost=MinEntryCost,@MaxEntryCost=MaxEntryCost,@MinBaseScore=MinBaseScore,@MaxBaseScore=MaxBaseScore 
	FROM BattleBaseOption(NOLOCK) WHERE KindID=@KindID AND ScoreKind=@ScoreKind AND PlayerCount=@PlayerCount AND EmployScene=@EmployScene		
	
	-- 选项名称
	IF @EmployScene=0 SET @strClubOptionName=N'ClubOption_Gold'
	IF @EmployScene=1 SET @strClubOptionName=N'ClubOption_Normal'
	IF @EmployScene=2 SET @strClubOptionName=N'ClubOption_Advanced'

	-- 查询配置
	SELECT @MinRevenueRate=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='MinRevenueRate'	
	SELECT @MaxRevenueRate=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=@strClubOptionName AND StatusName='MaxRevenueRate'		

	-- 调整配置
	IF @MinRevenueRate IS NULL SET @MinRevenueRate=0
	IF @MaxRevenueRate IS NULL SET @MaxRevenueRate=100

	-- 校验配置
	IF @OptionID IS NULL RETURN 2	
	-- 校验支付类型
	IF @PayType&@SuportPayType=0 RETURN 3
	-- 校验货币类型
	IF @CurrencyKind<>1 AND @CurrencyKind<>2 RETURN 4
	-- 校验金币类型
	IF @OptionMask&0x10=0 AND @CurrencyKind=0 RETURN 5
	-- 校验房卡类型
	IF @OptionMask&0x20=0 AND @CurrencyKind=1 RETURN 6
	-- 校验服务费
	IF @ServiceFee>0 AND @ScoreKind<>2 RETURN 7
	-- 校验语音
	IF @OptionMask&0x01=0 AND @AllowVoice=1 RETURN 8
	-- 校验旁观
	IF @OptionMask&0x02=0 AND @AllowLookon=1 RETURN 9
	-- 校验共开
	IF @OptionMask&0x04=0 AND @AllowPublic=1 RETURN 10
	-- 校验同IP
	IF @OptionMask&0x08=0 AND @AllowSameIP=1 RETURN 11
	-- 校验底分
	IF @BaseScore<@MinBaseScore OR @BaseScore>@MaxBaseScore RETURN 12
	-- 校验税收范围
	IF @RevenueRate<@MinRevenueRate OR @RevenueRate>@MaxRevenueRate RETURN 13
	-- 校验入场费
	IF @CurrencyKind=0 AND (@EntryCost<@MinEntryCost OR @EntryCost>@MaxEntryCost) RETURN 14
	-- 校验配置子项
	IF NOT EXISTS(SELECT 1 FROM BattleOptionItem(NOLOCK) WHERE OptionID=@OptionID AND PlayCount=@PlayCount) RETURN 15		

	RETURN 0
END

GO

----------------------------------------------------------------------------------------------------

-- 加载玩法
CREATE  PROCEDURE dbo.GSP_GR_LoadBattleConfig
	@dwClubID INT				-- 群组标识		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询玩法
	SELECT * FROM ClubBattleConfig WHERE ClubID=@dwClubID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 添加玩法
CREATE  PROCEDURE dbo.GSP_GR_AppendBattleConfig
	-- 用户信息
	@dwUserID INT,								-- 用户标识	

	-- 玩法信息
	@wKindID SMALLINT,							-- 类型标识
	@dwClubID INT,								-- 群组标识	
	@strBattleConfig NVARCHAR(512),				-- 约战玩法

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

	-- 变量定义
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

	-- 玩法校验
	DECLARE @ResultCode TINYINT
	SET @ResultCode=dbo.LHF_VerifyBattleConfig(@strBattleConfig)
	IF @ResultCode>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1179,"Msg":"约战玩法校验失败!错误代码['+CONVERT(NVARCHAR(3),@ResultCode) +N']","Ops":["'+CONVERT(NVARCHAR(3),@ResultCode)+'"]}'
		RETURN 4
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
		IF @OUT_OperaterType<3 OR (@OUT_OperaterType=3 AND (@OUT_OperaterRight&0x0002)=0)
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1180,"Msg":"抱歉，您没有权限操作约战玩法！"}'
			RETURN 5		
		END		
	END

	-- 玩法数量
	DECLARE @ConfigCount SMALLINT
	SELECT @ConfigCount=COUNT(1) FROM ClubBattleConfig(NOLOCK) WHERE ClubID=@dwClubID

	-- 调整变量 
	IF @ConfigCount IS NULL SET @ConfigCount=0

	-- 校验配置
	IF @ConfigCount>0
	BEGIN
		-- 变量定义	
		DECLARE @MaxConfigCount SMALLINT
		SELECT @MaxConfigCount=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo WHERE StatusKind=N'ClubOption' AND StatusName='MaxConfigCount'
		
		-- 调整变量
		IF @MaxConfigCount IS NULL SET @MaxConfigCount=1
		
		-- 比较个数
		IF 	@ConfigCount>=@MaxConfigCount
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1181,"Msg":"抱歉，茶馆内最多只能添加'+CONVERT(NVARCHAR(5),@MaxConfigCount)+'个约战玩法！","Ops":["'+CONVERT(NVARCHAR(5),@MaxConfigCount)+'"]}'
			RETURN 6	
		END
	END

	-- 插入玩法
	INSERT INTO ClubBattleConfig(KindID,ClubID,BattleConfig) VALUES (@wKindID,@dwClubID,@strBattleConfig)

	-- 玩法标识
	DECLARE @ConfigID INT
	SET @ConfigID=SCOPE_IDENTITY()

	-- 查询玩法
	SELECT * FROM ClubBattleConfig(NOLOCK) WHERE ConfigID=@ConfigID	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 修改玩法
CREATE  PROCEDURE dbo.GSP_GR_ModifyBattleConfig
	-- 用户信息
	@dwUserID INT,								-- 用户标识		
	
	-- 玩法信息
	@wKindID SMALLINT,							-- 类型标识
	@dwClubID INT,								-- 群组标识
	@dwConfigID INT,							-- 玩法标识
	@strBattleConfig NVARCHAR(512),				-- 约战玩法

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

	-- 变量定义
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

	-- 玩法校验
	DECLARE @ResultCode TINYINT
	SET @ResultCode=dbo.LHF_VerifyBattleConfig(@strBattleConfig)
	IF @ResultCode>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1179,"Msg":"约战玩法校验失败!错误代码['+CONVERT(NVARCHAR(3),@ResultCode) +N']","Ops":["'+CONVERT(NVARCHAR(3),@ResultCode)+'"]}'
		RETURN 4
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
		IF @OUT_OperaterType<3 OR (@OUT_OperaterType=3 AND (@OUT_OperaterRight&0x0002)=0)
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1180,"Msg":"抱歉，您没有权限操作约战玩法！"}'
			RETURN 5		
		END		
	END

	-- 变量定义
	IF NOT EXISTS(SELECT 1 FROM ClubBattleConfig(NOLOCK) WHERE ConfigID=@dwConfigID)		
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1182,"Msg":"抱歉，玩法不存在！"}'
		RETURN 6				
	END
	
	-- 更新玩法
	UPDATE ClubBattleConfig SET BattleConfig=@strBattleConfig WHERE ConfigID=@dwConfigID		
	
	-- 查询玩法
	SELECT * FROM ClubBattleConfig(NOLOCK) WHERE ConfigID=@dwConfigID		
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 删除玩法
CREATE  PROCEDURE dbo.GSP_GR_DeleteBattleConfig
	-- 用户信息
	@dwUserID INT,								-- 用户标识		
	
	-- 玩法信息
	@wKindID SMALLINT,							-- 类型标识
	@dwClubID INT,								-- 群组标识	
	@dwConfigID INT,							-- 玩法标识	

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

	-- 变量定义
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
	SELECT @CreaterID=CreaterID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID AND ClubStatus=1

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
		IF @OUT_OperaterType<3 OR (@OUT_OperaterType=3 AND (@OUT_OperaterRight&0x0002)=0)
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1180,"Msg":"抱歉，您没有权限操作约战玩法！"}'
			RETURN 5		
		END		
	END

	-- 删除玩法
	DELETE ClubBattleConfig WHERE ConfigID=@dwConfigID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------