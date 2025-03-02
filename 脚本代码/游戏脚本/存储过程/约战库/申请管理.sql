
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ApplyRequest]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ApplyRequest]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ApplyRespond]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ApplyRespond]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ApplyDelete]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ApplyDelete]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ApplyRepeal]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ApplyRepeal]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadApplyMessage]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadApplyMessage]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- 申请请求
CREATE  PROCEDURE dbo.GSP_GR_ApplyRequest
	-- 用户信息	
	@dwClubID INT,								-- 群组标识	
	@dwApplyerID INT,							-- 用户标识	
	
	-- 输出参数	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @FaceID		INT
DECLARE @Gender		TINYINT
DECLARE @CustomID	INT
DECLARE @NickName	NVARCHAR(32)

-- 群组信息
DECLARE @CreaterID  INT
DECLARE @ClubName  NVARCHAR(32)
DECLARE @ClubStatus TINYINT
DECLARE @MemberCount SMALLINT
DECLARE @MaxMemberCount SMALLINT

-- 执行逻辑
BEGIN

	-- 变量定义
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- 查询用户
	SELECT @UserID=a.UserID,@GameID=a.GameID,@Nullity=a.Nullity, @NickName=a.NickName, @Gender=a.Gender, @FaceID=a.FaceID, @CustomID=a.CustomID, @ShutDown=b.ShutDownStatus
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwApplyerID

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

	-- 存在判断
	IF EXISTS(SELECT 1 FROM RecordClubApply(NOLOCK) WHERE ClubID=@dwClubID AND ApplyerID=@dwApplyerID AND ApplyStatus=0)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1183,"Msg":"您已经申请加入该茶社，不需要重复申请，请耐心等待茶社老板审核！"}'
		RETURN 5					
	END	

	-- 校验俱乐部
	---------------------------------------------------------------------------------------------------------------------------

	-- 查询群组	
	SELECT @CreaterID=CreaterID,@ClubName=ClubName,@ClubStatus=ClubStatus,@MemberCount=MemberCount,@MaxMemberCount=MaxMemberCount FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- 俱乐部不存在
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'
		RETURN 6
	END

	-- 俱乐部已停用
	IF @ClubStatus=2
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'				
		RETURN 7
	END

	-- 俱乐部满员
	IF @MemberCount=@MaxMemberCount
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1184,"Msg":"抱歉，该茶社已满员，您不能申请加入！"}'
		RETURN 8		
	END	

	-- 校验申请者
	---------------------------------------------------------------------------------------------------------------------------

	-- 创建数量
	DECLARE @wCreateClubCount SMALLINT
	SELECT @wCreateClubCount=COUNT(ClubID) FROM ClubProperty(NOLOCK) WHERE CreaterID=@dwApplyerID AND ClubStatus=1

	-- 读取群组列表
	DECLARE @strClubIDArray NVARCHAR(1024)
	SELECT @strClubIDArray=ClubIDArray FROM ClubUserInfo(NOLOCK) WHERE UserID=@dwApplyerID
	IF @strClubIDArray IS NULL 
	BEGIN
		SET @strClubIDArray=';'
		INSERT INTO ClubUserInfo VALUES (@dwApplyerID,@strClubIDArray)
	END

	-- 查找标识
	DECLARE @strClubID NVARCHAR(20)
	SET @strClubID=CONCAT(';',CONVERT(NVARCHAR(18),@dwClubID),';');	
	IF CHARINDEX(@strClubID,@strClubIDArray)>0
	BEGIN		
		SET @strErrorDescribe=N'{"Idx":1185,"Msg":"您已经是该茶社的成员！"}'
		RETURN 9
	END	

	-- 查询配置
	DECLARE @MaxJoinClubCount SMALLINT		
	SELECT @MaxJoinClubCount=CONVERT(SMALLINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'ClubOption' AND StatusName='MaxJoinClubCount'
	
	-- 群组数目
	DECLARE @dwClubCount SMALLINT	
	SELECT @dwClubCount=(LEN(@strClubIDArray)-LEN(REPLACE(@strClubIDArray, ';', '')))

	-- 调整变量
	IF @dwClubCount>0 SET @dwClubCount=@dwClubCount-1
	
	-- 统计次数	
	DECLARE @ApplyCount SMALLINT
	SELECT @ApplyCount=COUNT(ClubID) FROM RecordClubApply(NOLOCK) WHERE ApplyerID=@dwApplyerID AND ApplyStatus=0

	-- 配置校验
	IF (@dwClubCount-@wCreateClubCount)+@ApplyCount>=@MaxJoinClubCount
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1186,"Msg":"抱歉，您同时可加入茶社数量为'+CONVERT(NVARCHAR(6),@MaxJoinClubCount)+'个，请退出多余茶社后再试！","Ops":["'+CONVERT(NVARCHAR(6),@MaxJoinClubCount)+'"]}'
		
		RETURN 10				
	END

	---------------------------------------------------------------------------------------------------------------------------	

	-- 申请记录
	INSERT INTO RecordClubApply(ClubID, ApplyerID, ApplyerGameID, ApplyerNickName, ApplyerGender, ApplyerFaceID, ApplyerCustomID, ApplyStatus,CreaterID, ResponderID, ClubName)
	VALUES (@dwClubID, @dwApplyerID, @GameID, @NickName, @Gender, @FaceID, @CustomID, 0, @CreaterID, 0, @ClubName)	

	-- 记录标识
	DECLARE @RecordID INT
	SELECT 	@RecordID=SCOPE_IDENTITY()

	-- 查询开关选项
	DECLARE @SwitchOption SMALLINT
	SELECT @SwitchOption=SwitchOption FROM ClubRules(NOLOCK) WHERE ClubID=@dwClubID

	-- 调整变量
	IF @SwitchOption IS NULL SET @SwitchOption=0	

	-- 审核校验
	IF (@SwitchOption&0x01)=0
	BEGIN		
		-- 更新状态
		UPDATE RecordClubApply SET ApplyStatus=1,OperateDateTime=GETDATE() WHERE RecordID=@RecordID	

		-- 查询信息
		SELECT a.*,b.*,c.FaceID AS CreaterFaceID,c.CustomID AS CreaterCustomID,c.NickName AS CreaterNickName 
		FROM RecordClubApply(NOLOCK) a, ClubProperty(NOLOCK) b, LHAccountsDB.dbo.AccountsInfo(NOLOCK) c
		WHERE a.ClubID=b.ClubID AND a.RecordID=@RecordID AND a.CreaterID=c.UserID
				
		-- 申请成功
		SET @strErrorDescribe=N'{"Idx":1187,"Msg":"您已成功加入俱乐部，祝您游戏愉快！"}'

	END ELSE
	BEGIN

		-- 查询信息
		SELECT * FROM RecordClubApply(NOLOCK) WHERE RecordID=@RecordID

		-- 申请成功
		SET @strErrorDescribe=N'{"Idx":1188,"Msg":"申请成功，请耐心等待茶社老板审核！"}'
	END		
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 申请响应
CREATE  PROCEDURE dbo.GSP_GR_ApplyRespond
	-- 用户信息
	@dwRecordID INT,							-- 记录标识 
	@dwUserID INT,								-- 用户标识
	@dwApplyerID INT,							-- 用户标识	
	@dwClubID INT,								-- 站点标识		
	@cbApplyStatus TINYINT,						-- 申请状态
	
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
DECLARE @Sqlstr		NVARCHAR(512)											
DECLARE	@TableName	NVARCHAR(128)

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
	DECLARE @MemberCount SMALLINT
	DECLARE @MaxMemberCount SMALLINT
	SELECT @CreaterID=CreaterID, @MemberCount=MemberCount, @MaxMemberCount=MaxMemberCount FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- 茶社不存在
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'
		RETURN 5		
	END

	-- 俱乐部满员
	IF @MemberCount=@MaxMemberCount AND @cbApplyStatus=1
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1189,"Msg":"抱歉，您的茶社已经满员，成员添加失败！"}'
		RETURN 6		
	END

	-- 分表标识
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- 构造表名
	SET	@TableName='ClubMember_';
	SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

	-- 查询成员类型
	DECLARE @OUT_MemberType SMALLINT	
	DECLARE @OUT_ManageRight INT
	SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType,@ManageRight=ManageRight FROM ',@TableName,'(NOLOCK)',
						 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwUserID))				  
	
	-- 执行语句
	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT,@ManageRight INT OUT',@MemberType=@OUT_MemberType OUTPUT,@ManageRight=@OUT_ManageRight OUTPUT

	-- 校验类型和权限
	IF @OUT_MemberType<3 OR (@OUT_MemberType=3 AND (@OUT_ManageRight&0x0004)=0)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1190,"Msg":"抱歉，您没有权限审核用户申请！"}'
		RETURN 7
	END

	IF @dwRecordID<>0
	BEGIN
		-- 查询记录
		DECLARE @ClubID INT
		DECLARE @ApplyerID INT
		DECLARE @ApplyStatus TINYINT	
		SELECT @ClubID=ClubID,@ApplyerID=ApplyerID,@ApplyStatus=ApplyStatus FROM RecordClubApply(NOLOCK) WHERE RecordID=@dwRecordID

		IF @ApplyerID IS NULL
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1191,"Msg":"系统未查询到相关的申请记录，该记录可能已被申请者撤销！"}'
			RETURN 8		
		END 

		IF @dwApplyerID<>@ApplyerID OR @dwClubID<>@ClubID
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1192,"Msg":"系统检测到申请记录异常，请联系客户服务中心了解详细情况！"}'
			RETURN 9
		END	

		-- 群组属性
		UPDATE RecordClubApply SET ApplyStatus=@cbApplyStatus,ResponderID=@dwUserID,OperateDateTime=GETDATE() WHERE RecordID=@dwRecordID		

		-- 查询记录
		SELECT * FROM RecordClubApply(NOLOCK) WHERE RecordID=@dwRecordID

	END ELSE
	BEGIN
		-- 查询记录
		SELECT * FROM RecordClubApply(NOLOCK) WHERE ClubID=@dwClubID AND ApplyStatus=0

		-- 修改状态
		UPDATE RecordClubApply SET ApplyStatus=@cbApplyStatus,ResponderID=@dwUserID,OperateDateTime=GETDATE() WHERE ClubID=@dwClubID AND ApplyStatus=0				
	END
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 申请删除
CREATE  PROCEDURE dbo.GSP_GR_ApplyDelete
	-- 用户信息
	@dwUserID INT,								-- 用户标识				
	@dwClubID INT,								-- 茶社标识	
	@dwMessageID INT,							-- 消息标识 
	@cbMessageType INT,							-- 消息类型 				
	
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
DECLARE @Sqlstr		NVARCHAR(512)											
DECLARE	@TableName	NVARCHAR(128)

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

	-- 响应类型
	IF @cbMessageType=2
	BEGIN

		-- 分表标识
		SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

		-- 构造表名
		SET	@TableName='ClubMember_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

		-- 查询成员类型
		DECLARE @OUT_MemberType SMALLINT	
		DECLARE @OUT_ManageRight INT
		SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType,@ManageRight=ManageRight FROM ',@TableName,'(NOLOCK)',
							 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwUserID))				  
	
		-- 执行语句
		EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT,@ManageRight INT OUT',@MemberType=@OUT_MemberType OUTPUT,@ManageRight=@OUT_ManageRight OUTPUT

		-- 校验类型和权限
		IF @OUT_MemberType<3 OR (@OUT_MemberType=3 AND (@OUT_ManageRight&0x0004)=0)
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1190,"Msg":"抱歉，您没有权限审核用户申请！"}'
			RETURN 7
		END
	END

	IF @dwMessageID<>0
	BEGIN
		-- 查询消息
		DECLARE @ApplyerID INT
		DECLARE @ApplyStatus TINYINT
		DECLARE @DeleteMask TINYINT
		SELECT @ApplyerID=ApplyerID,@ApplyStatus=ApplyStatus,@DeleteMask=DeleteMask FROM RecordClubApply(NOLOCK) WHERE RecordID=@dwMessageID

		-- 消息不存在
		IF @ApplyerID IS NULL
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1192,"Msg":"系统检测到申请记录异常，请联系客户服务中心了解详细情况！"}'
			RETURN 5		
		END

		-- 判断状态
		IF @ApplyStatus=0
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1193,"Msg":"抱歉，该消息未通过老板审核，无法删除！"}'
			RETURN 5		
		END		
		
		-- 校验掩码
		IF @DeleteMask<>0
		BEGIN
			-- 删除消息
			DELETE RecordClubApply WHERE RecordID=@dwMessageID	

		END ELSE
		BEGIN
			-- 更新状态
			UPDATE RecordClubApply SET DeleteMask=CASE @cbMessageType WHEN 1 THEN 1 ELSE 2 END WHERE RecordID=@dwMessageID
		END

		-- 查询消息
		SELECT @dwMessageID AS RecordID
			
	END ELSE
	BEGIN
		-- 申请类型
		IF @cbMessageType=1
		BEGIN
			-- 查询消息
			SELECT RecordID FROM RecordClubApply WHERE ApplyerID=@dwUserID AND ApplyStatus<>0

			-- 删除消息
			DELETE RecordClubApply WHERE ApplyerID=@dwUserID AND ApplyStatus<>0 AND DeleteMask=2
			
			-- 更新掩码
			UPDATE RecordClubApply SET DeleteMask=1 WHERE ApplyerID=@dwUserID AND ApplyStatus<>0 AND DeleteMask=0
		END
		
		-- 响应类型
		IF @cbMessageType=2
		BEGIN
			-- 查询消息
			SELECT RecordID FROM RecordClubApply WHERE ClubID=@dwClubID AND ApplyStatus<>0
			
			-- 删除消息
			DELETE RecordClubApply WHERE ClubID=@dwClubID AND ApplyStatus<>0 AND DeleteMask=1
			
			-- 更新掩码
			UPDATE RecordClubApply SET DeleteMask=2 WHERE ClubID=@dwClubID AND ApplyStatus<>0 AND DeleteMask=0
		END
	END
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 申请撤销
CREATE  PROCEDURE dbo.GSP_GR_ApplyRepeal
	-- 用户信息
	@dwMessageID INT,							-- 消息标识 
	@cbMessageStatus TINYINT,					-- 消息状态
	@dwUserID INT,								-- 用户标识	
		
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

	IF @dwMessageID<>0
	BEGIN
		-- 查询消息
		DECLARE @ApplyerID INT
		DECLARE @ApplyStatus TINYINT
		DECLARE @DeleteMask TINYINT
		SELECT @ApplyerID=ApplyerID,@ApplyStatus=ApplyStatus,@DeleteMask=DeleteMask FROM RecordClubApply(NOLOCK) WHERE RecordID=@dwMessageID

		-- 消息不存在
		IF @ApplyerID IS NULL
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1192,"Msg":"抱歉，消息不存在！"}'
			RETURN 5		
		END

		-- 判断状态
		IF @ApplyStatus<>0
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1195,"Msg":"抱歉，该消息已通过老板审核，无法撤销！"}'
			RETURN 5		
		END

		-- 申请者撤销
		IF @dwUserID<>@ApplyerID
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1196,"Msg":"抱歉，您不是申请人,无法撤销该消息！"}'
			RETURN 6		
		END	

		-- 更新状态
		UPDATE RecordClubApply SET ApplyStatus=@cbMessageStatus,OperateDateTime=GETDATE() WHERE RecordID=@dwMessageID

		-- 抛出记录
		SELECT * FROM RecordClubApply(NOLOCK) WHERE RecordID=@dwMessageID

	END ELSE
	BEGIN
		-- 查询消息
		SELECT * FROM RecordClubApply(NOLOCK) WHERE ApplyerID=@dwUserID AND ApplyStatus=0 AND DeleteMask<>1

		-- 更新状态
		UPDATE RecordClubApply SET ApplyStatus=@cbMessageStatus,OperateDateTime=GETDATE() WHERE ApplyerID=@dwUserID AND ApplyStatus=0 AND DeleteMask<>1
	END
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 申请响应
CREATE  PROCEDURE dbo.GSP_GR_LoadApplyMessage
	-- 用户信息	
	@dwUserID INT,								-- 用户标识
	@dwClubID INT,								-- 群组标识
	@cbMessageType INT							-- 消息类型		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 请求者消息
	IF @cbMessageType=1
	BEGIN
		
		-- 查询消息
		SELECT * FROM RecordClubApply(NOLOCK) WHERE ApplyerID=@dwUserID AND DeleteMask<>1
	END

	-- 响应者消息
	IF @cbMessageType=2
	BEGIN

		-- 查询消息
		SELECT * FROM RecordClubApply(NOLOCK) WHERE ClubID=@dwClubID AND DeleteMask<>2
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------