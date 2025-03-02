
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_SettleBattle]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_SettleBattle]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- 约战结算
CREATE  PROCEDURE dbo.GSP_GR_SettleBattle	
	@dwDateID	INT,			-- 日期标识	
	@dwClubID	INT,			-- 群组标识	
	@dwMemberID INT,			-- 成员标识
	@wSettleCount SMALLINT,		-- 结算次数
	@dwCreaterID INT,			-- 用户标识

	-- 输出参数	
	@strErrorDescribe NVARCHAR(127) OUTPUT -- 输出信息
WITH ENCRYPTION AS

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN	

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	
	-- 查询用户
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

	-- 查询茶馆
	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- 茶馆不存在
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'
		RETURN 5
	END

	-- 不是群主
	IF @CreaterID<>@dwCreaterID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1199,"Msg":"抱歉，您不是该茶社的老板，没有权限进行结算！"}'
		RETURN 6
	END

	-- 更新数据
	DECLARE @BattleCount INT
	SELECT @BattleCount=BattleCount FROM LHGameBattleDB.dbo.StreamClubBalance(NOLOCK) WHERE DateID=@dwDateID AND ClubID=@dwClubID AND UserID=@dwMemberID

	-- 存在校验
	IF @BattleCount IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1200,"Msg":"抱歉，系统未查询到该成员的约战统计信息，结算失败！"}'
		RETURN 7				
	END

	-- 次数校验
	IF @wSettleCount>@BattleCount
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1201,"Msg":"抱歉，结算次数不能超过约战次数，结算失败！"}'
		RETURN 7
	END

	-- 更新数据
	UPDATE LHGameBattleDB.dbo.StreamClubBalance SET BalanceCount=@wSettleCount WHERE DateID=@dwDateID AND ClubID=@dwClubID AND UserID=@dwMemberID	


END

RETURN 0

GO

----------------------------------------------------------------------------------------------------