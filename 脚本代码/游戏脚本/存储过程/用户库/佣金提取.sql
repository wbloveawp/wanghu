
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_DrawCommission]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_DrawCommission]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 提取佣金
CREATE PROC GSP_MB_DrawCommission
	@dwUserID INT,								-- 用户 I D
	@lDrawAmount INT,							-- 提取金额
	@wReason INT,								-- 变更原因
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器序列
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 自动回滚
SET XACT_ABORT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @UserID INT
	DECLARE @GameID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	DECLARE @NickName NVARCHAR(32)
	DECLARE @Email NVARCHAR(64)
	DECLARE @MobilePhone NVARCHAR(21)
	SELECT @UserID=a.UserID,@GameID=a.GameID,@NickName=a.NickName,@Email=a.Email,@MobilePhone=a.MobilePhone, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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
	
	-- 取佣配置
	DECLARE @DrawEnabled TINYINT	
	DECLARE @MinDrawAmount INT	
	DECLARE @ServiceFeeRate SMALLINT		

	-- 查询配置
	SELECT @DrawEnabled=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'CommissionOption' AND StatusName=N'DrawEnabled'
	SELECT @MinDrawAmount=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'CommissionOption' AND StatusName=N'MinDrawAmount'	
	SELECT @ServiceFeeRate=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'CommissionOption' AND StatusName=N'ServiceFeeRate'	
	
	-- 调整配置
	IF @DrawEnabled IS NULL SET @DrawEnabled=1
	IF @MinDrawAmount IS NULL SET @MinDrawAmount=10000
	IF @ServiceFeeRate IS NULL SET @ServiceFeeRate=0	

	-- 校验开关
	IF @DrawEnabled=0
	BEGIN
		-- 错误信息
		SET @strErrorDescribe=N'{"Idx":1133,"Msg":"抱歉，佣金提取功能已关闭。"}'	
		RETURN 15
	END

	-- 校验金额
	IF @lDrawAmount<@MinDrawAmount
	BEGIN
		-- 错误信息
		SET @strErrorDescribe=N'{"Idx":1040,"Msg":"抱歉, 每笔提取金额不能小于%s。","Ops":["'+CONVERT(NVARCHAR(16),@MinDrawAmount/1000)+'"]}'	
		RETURN 16
	END		

	-- 服务费用
	DECLARE @ServiceFee INT
	DECLARE @ChangeAmount INT

	-- 计算费用
	SET @ServiceFee = @lDrawAmount*@ServiceFeeRate/1000	
	SET @ChangeAmount = @lDrawAmount-@ServiceFee

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 查询金币
	DECLARE @Commission BIGINT
	SELECT @Commission=Commission FROM AccountsCommission WITH(UPDLOCK) WHERE UserID=@dwUserID

	-- 调整佣金
	IF @Commission IS NULL SET @Commission=0

	-- 校验余额
	IF @Commission<@lDrawAmount
	BEGIN
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 错误信息
		SET @strErrorDescribe=N'{"Idx":1135,"Msg":"抱歉！您身上的佣金余额不足，提取失败。"}'	
		RETURN 18
	END	

	-- 更新佣金
	UPDATE AccountsCommission SET Commission-=@lDrawAmount WHERE UserID=@dwUserID

	-- 变更记录	
	INSERT INTO LHGameRecordDB.dbo.RecordDrawCommission(UserID,NickName,ServiceFee,DrawAmount,BeforeCommission,CollectClientIP,CollectMachineID,CollectDateTime)
	VALUES (@UserID,@NickName,@ServiceFee,@lDrawAmount,@Commission,@strClientIP,@strMachineID,GETDATE())

	-- 写入财富
	EXEC GSP_GP_WriteGameWealth @dwUserID,2,@wReason,@ChangeAmount
	
	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- 提取成功
	SET @strErrorDescribe=N'{"Idx":1051,"Msg":"操作成功！"}'

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

