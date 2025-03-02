----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadKindSetting]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadKindSetting]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_SetWinnerScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_SetWinnerScore]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- 加载设置
CREATE  PROCEDURE dbo.GSP_GR_LoadKindSetting
	@dwClubID INT				-- 群组标识		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询设置
	SELECT * FROM ClubKindSetting WHERE ClubID=@dwClubID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 设置赢分
CREATE  PROCEDURE dbo.GSP_GR_SetWinnerScore
	-- 用户信息
	@dwUserID INT,								-- 用户标识		
	
	-- 玩法信息
	@dwClubID INT,								-- 群组标识
	@wKindID INT,								-- 游戏标识
	@lMinWinnerScore INT,						-- 最低赢分

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
		IF @OUT_OperaterType<3 OR (@OUT_OperaterType=3 AND (@OUT_OperaterRight&0x0040)=0)
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1215,"Msg":"抱歉，您没有权限设置大赢家！"}'
			RETURN 5		
		END		
	END

	-- 更新设置
	UPDATE ClubKindSetting SET MinWinnerScore=@lMinWinnerScore WHERE ClubID=@dwClubID AND KindID=@wKindID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO ClubKindSetting(ClubID,KindID,MinWinnerScore) VALUES(@dwClubID,@wKindID,@lMinWinnerScore)
	END
	
	-- 查询玩法
	SELECT * FROM ClubKindSetting(NOLOCK) WHERE ClubID=@dwClubID AND KindID=@wKindID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------