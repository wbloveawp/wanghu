USE LHGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_BuyMatchTicket]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_BuyMatchTicket]
GO

--IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_ExchangeMatchTicket]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
--DROP PROCEDURE [dbo].[GSP_GS_ExchangeMatchTicket]
--GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_QueryMatchTicket]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_QueryMatchTicket]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadMatchTicketInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadMatchTicketInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 购买赛券
CREATE PROC GSP_GS_BuyMatchTicket
	-- 比赛信息
	@dwUserID INT,								-- 用户 I D	
	@dwMatchID INT,								-- 比赛标识
	@lMatchNo  BIGINT,							-- 比赛场次
	@MatchTime DATETIME,						-- 比赛时间
	@wReason SMALLINT,							-- 变更原因
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 描述信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

SET XACT_ABORT ON

-- 执行逻辑
BEGIN
		
	-- 变量定义
	DECLARE @UserID INT
	DECLARE @ParentID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT
	
	-- 查询用户
	SELECT @UserID=a.UserID, @ParentID=a.ParentID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus 
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

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

	-- 变量定义
	DECLARE @MatchName NVARCHAR(32)	
	DECLARE @BuyTicketCount INT
	DECLARE @BuyTicketOption INT
	DECLARE @BuyTicketAmount INT
	DECLARE @BuyTicketMoneyID SMALLINT	

	-- 查询比赛
	SELECT @MatchName=MatchName,@BuyTicketCount=BuyTicketCount,@BuyTicketOption=BuyTicketOption,@BuyTicketAmount=BuyTicketAmount,@BuyTicketMoneyID=BuyTicketMoneyID FROM MatchOption(NOLOCK) WHERE MatchID=@dwMatchID

	-- 校验结果
	IF @BuyTicketCount IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1288,"Msg":"系统未查询到相关的比赛信息，请查证后再次尝试！"}'
		RETURN 4
	END

	-- 钻石或黄金购买
	IF @BuyTicketMoneyID=1 OR @BuyTicketMoneyID=2
	BEGIN
		-- 开始事务
		SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
		BEGIN TRAN

		-- 消耗金额
		DECLARE  @ExpendAmount INT = -@BuyTicketAmount
		
		-- 扣除费用
		DECLARE @ResultCode INT
		EXEC @ResultCode=LHAccountsDB.dbo.GSP_GP_WriteGameWealth @dwUserID,@BuyTicketMoneyID,@wReason,@ExpendAmount

		-- 校验结果
		IF @ResultCode<>0
		BEGIN
			-- 回滚事务
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED

			IF @BuyTicketMoneyID=1 SET @strErrorDescribe=N'{"Idx":1289,"Msg":"您身上的钻石余额不足，比赛券购买失败！"}'
			IF @BuyTicketMoneyID=2 SET @strErrorDescribe=N'{"Idx":1320	Msg":"您身上的黄金余额不足，比赛券购买失败！"}'

			RETURN 5
		END

		-- 购买统计
		UPDATE StreamMatchTicket SET BuyTicketCount+=@BuyTicketCount,BuyTimes+=1,UpdateTime=GetDate() WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
		IF @@ROWCOUNT=0
		BEGIN
			INSERT INTO StreamMatchTicket(UserID,MatchID,MatchNo,BuyTicketCount,UseTicketCount,BuyTimes,BuyAmount,BuyMoneyID,UpdateTime,CollectTime)
			VALUES (@dwUserID,@dwMatchID,@lMatchNo,@BuyTicketCount,0,1,@BuyTicketAmount,@BuyTicketMoneyID,GetDate(),GetDate())
		END

		-- 重置比赛分
		IF (@BuyTicketOption&0x01)<>0
		BEGIN
			UPDATE MatchScoreInfo SET ServerID=0,RankID=0,Score=0,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0,WinStreak=0,MaxWinStreak=0,MatchRight=0,SignupCount=0 
			WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo
		END

		-- 首次购买记录
		IF @dwMatchID=1
		BEGIN
			IF NOT EXISTS (SELECT 1 FROM RecordFirstBuyTicket(NOLOCK) WHERE UserID=@dwUserID)
			BEGIN
				INSERT INTO RecordFirstBuyTicket(UserID,ParentID,MatchID,MatchNo,MatchName,MatchTime,BuyAmount,BuyMoneyID,CollectTime)
				VALUES(@dwUserID,@ParentID,@dwMatchID,@lMatchNo,@MatchName,@MatchTime,@BuyTicketAmount,@BuyTicketMoneyID,GETDATE())
			END
		END

		-- 结束事务
		COMMIT TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	END

	-- 财富定义
	DECLARE @MoneyBalance BIGINT

	-- 查询钻石
	IF @BuyTicketMoneyID=1
	BEGIN
		SELECT @MoneyBalance=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID
	END

	-- 查询黄金
	IF @BuyTicketMoneyID=2
	BEGIN
		SELECT @MoneyBalance=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID
	END

	-- 查询赛券
	DECLARE @UserTicket INT
	SELECT @UserTicket=BuyTicketCount-UseTicketCount FROM StreamMatchTicket(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo

	-- 调整变量
	IF @UserTicket IS NULL SET @UserTicket=0
	IF @MoneyBalance IS NULL SET @MoneyBalance=0

	-- 抛出数据
	SELECT @BuyTicketMoneyID AS MoneyID, @MoneyBalance AS MoneyBalance, @UserTicket AS TicketCount
END

RETURN 0
GO


----------------------------------------------------------------------------------------------------

---- 兑换赛券
--CREATE PROC GSP_GS_ExchangeMatchTicket
--	@dwUserID INT,								-- 用户 I D	
--	@dwMatchID INT,								-- 比赛标识
--	@lMatchNo  BIGINT,							-- 比赛场次
--	@MatchTime DATETIME,						-- 比赛时间
--	@RedeemCode NVARCHAR(12),					-- 兑换码	
--	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 描述信息
--WITH ENCRYPTION AS

---- 属性设置
--SET NOCOUNT ON

--SET XACT_ABORT ON

---- 执行逻辑
--BEGIN
		
--	-- 变量定义
--	DECLARE @UserID INT
--	DECLARE @ParentID INT
--	DECLARE @Nullity BIT
--	DECLARE @ShutDown BIT
	
--	-- 查询用户
--	SELECT @UserID=a.UserID, @ParentID=a.ParentID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus 
--	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

--	-- 查询用户
--	IF @UserID IS NULL
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
--		RETURN 1
--	END	

--	-- 帐号禁止
--	IF @Nullity<>0
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
--		RETURN 2
--	END	

--	-- 帐号关闭
--	IF @ShutDown<>0
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
--		RETURN 3
--	END

--	-- 变量定义
--	DECLARE @MatchName NVARCHAR(32)	
--	DECLARE @BuyTicketCount INT
--	DECLARE @BuyTicketAmount INT
--	DECLARE @BuyTicketMoneyID SMALLINT	

--	-- 查询比赛
--	SELECT @MatchName=MatchName,@BuyTicketCount=BuyTicketCount,@BuyTicketAmount=BuyTicketAmount,@BuyTicketMoneyID=BuyTicketMoneyID FROM MatchOption(NOLOCK) WHERE MatchID=@dwMatchID

--	-- 校验结果
--	IF @BuyTicketCount IS NULL
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1288,"Msg":"系统未查询到相关的比赛信息，请查证后再次尝试！"}'
--		RETURN 4
--	END

--	-- 钻石或黄金购买
--	IF @BuyTicketMoneyID=1 OR @BuyTicketMoneyID=2
--	BEGIN
--		-- 开始事务
--		SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
--		BEGIN TRAN

--		-- 消耗金额
--		DECLARE  @ExpendAmount INT = -@BuyTicketAmount
		
--		-- 扣除费用
--		DECLARE @ResultCode INT
--		EXEC @ResultCode=LHAccountsDB.dbo.GSP_GP_WriteGameWealth @dwUserID,@BuyTicketMoneyID,@wReason,@ExpendAmount

--		-- 校验结果
--		IF @ResultCode<>0
--		BEGIN
--			-- 回滚事务
--			ROLLBACK TRAN
--			SET TRANSACTION ISOLATION LEVEL READ COMMITTED

--			IF @BuyTicketMoneyID=1 SET @strErrorDescribe=N'{"Idx":1289,"Msg":"您身上的钻石余额不足，比赛券购买失败！"}'
--			IF @BuyTicketMoneyID=2 SET @strErrorDescribe=N'{"Idx":1320	Msg":"您身上的黄金余额不足，比赛券购买失败！"}'

--			RETURN 5
--		END

--		-- 购买统计
--		UPDATE StreamMatchTicket SET BuyTicketCount+=@BuyTicketCount,BuyTimes+=1,UpdateTime=GetDate() WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
--		IF @@ROWCOUNT=0
--		BEGIN
--			INSERT INTO StreamMatchTicket(UserID,MatchID,MatchNo,BuyTicketCount,UseTicketCount,BuyTimes,BuyAmount,BuyMoneyID,UpdateTime,CollectTime)
--			VALUES (@dwUserID,@dwMatchID,@lMatchNo,@BuyTicketCount,0,1,@BuyTicketAmount,@BuyTicketMoneyID,GetDate(),GetDate())
--		END

--		-- 首次购买记录
--		IF @dwMatchID=1
--		BEGIN
--			IF NOT EXISTS (SELECT 1 FROM RecordFirstBuyTicket(NOLOCK) WHERE UserID=@dwUserID)
--			BEGIN
--				INSERT INTO RecordFirstBuyTicket(UserID,ParentID,MatchID,MatchNo,MatchName,MatchTime,BuyAmount,BuyMoneyID,CollectTime)
--				VALUES(@dwUserID,@ParentID,@dwMatchID,@lMatchNo,@MatchName,@MatchTime,@BuyTicketAmount,@BuyTicketMoneyID,GETDATE())
--			END
--		END

--		-- 结束事务
--		COMMIT TRAN
--		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
--	END

--	-- 财富定义
--	DECLARE @MoneyBalance BIGINT

--	-- 查询钻石
--	IF @BuyTicketMoneyID=1
--	BEGIN
--		SELECT @MoneyBalance=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID
--	END

--	-- 查询黄金
--	IF @BuyTicketMoneyID=2
--	BEGIN
--		SELECT @MoneyBalance=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID
--	END

--	-- 查询赛券
--	DECLARE @UserTicket INT
--	SELECT @UserTicket=BuyTicketCount-UseTicketCount FROM StreamMatchTicket(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo

--	-- 调整变量
--	IF @UserTicket IS NULL SET @UserTicket=0
--	IF @MoneyBalance IS NULL SET @MoneyBalance=0

--	-- 抛出数据
--	SELECT @BuyTicketMoneyID AS MoneyID, @MoneyBalance AS MoneyBalance, @UserTicket AS TicketCount
--END

--RETURN 0
--GO


----------------------------------------------------------------------------------------------------

-- 查询赛券
CREATE PROC GSP_GS_QueryMatchTicket
	@dwUserID INT,								-- 用户 I D		
	@dwMatchID INT,								-- 比赛标识
	@lMatchNo  BIGINT							-- 比赛场次		
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
		
	-- 查询赛券
	DECLARE @TicketCount INT
	SELECT @TicketCount=BuyTicketCount-UseTicketCount FROM StreamMatchTicket(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
	
	-- 调整变量
	IF @TicketCount IS NULL SET @TicketCount=0

	-- 抛出数据
	SELECT @TicketCount AS TicketCount, @dwMatchID AS MatchID, @lMatchNo AS MatchNo
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- 加载赛券
CREATE PROC GSP_GS_LoadMatchTicketInfo
	@dwMatchID INT,								-- 比赛标识
	@lMatchNo  BIGINT							-- 比赛场次		
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
		
	-- 统计数量
	DECLARE @UserCount INT
	SELECT @UserCount=COUNT(1) FROM StreamMatchTicket(NOLOCK) WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo
	
	-- 调整变量
	IF @UserCount IS NULL SET @UserCount=0

	-- 抛出数据
	SELECT @UserCount AS UserCount, @dwMatchID AS MatchID, @lMatchNo AS MatchNo
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------