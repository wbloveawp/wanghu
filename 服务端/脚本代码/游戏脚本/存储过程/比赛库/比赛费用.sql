USE LHGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchSignup]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchSignup]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchUnSignup]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchUnSignup]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchShare]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchShare]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchRevive]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchRevive]
GO
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchReviveRepeal]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchReviveRepeal]
GO



SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 扣除费用
CREATE PROC GSP_GR_MatchSignup
	@dwUserID INT,								-- 用户 I D	
	@InitScore BIGINT,							-- 初始积分	
	@wMaxSignupUser SMALLINT,					-- 最大人数
	@wKindID INT,								-- 游戏 I D
	@wServerID INT,								-- 房间 I D
	@dwMatchID INT,								-- 比赛 I D
	@lMatchNo BIGINT,							-- 比赛场次
	@wReason SMALLINT,							-- 变更原因
	@strClientIP NVARCHAR(15),					-- 连接地址	
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN	
	-- 比赛信息
	DECLARE @FeeMoneyID SMALLINT
	DECLARE @FeeAmount INT
	DECLARE @Experience INT	
	DECLARE @SignupMode TINYINT	
	DECLARE @SignupCount SMALLINT
	DECLARE @MatchType TINYINT
	DECLARE @MemberOrder TINYINT	
	DECLARE @JoinCondition TINYINT
	DECLARE @UseTicketCount INT	
	
	-- 查询比赛	
	SELECT @FeeMoneyID=FeeMoneyID, @FeeAmount=FeeAmount,@UseTicketCount=UseTicketCount, @MatchType=MatchType, @Experience=Experience, @JoinCondition=JoinCondition, 
		  @SignupMode=SignupMode,@SignupCount=SignupCount, @MemberOrder=MemberOrder FROM MatchOption(NOLOCK) WHERE MatchID=@dwMatchID

	-- 存在判断
	IF @FeeMoneyID IS NULL
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1110,"Msg":"抱歉,您报名的比赛不存在！"}'
		return 1		
	END
	
	-- 查询用户
	DECLARE @dwGameID INT
	DECLARE @lExperience INT
	DECLARE @cbMemberOrder TINYINT
	SELECT @dwGameID=GameID, @lExperience=Experience, @cbMemberOrder=MemberOrder FROM LHAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	
	-- 存在判断
	IF @lExperience IS NULL
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1091,"Msg":"抱歉,您的用户信息不存在，请联系客服中心工作人员了解情况！"}'
		return 1	 
	END	
	
	-- 会员等级
	IF (@JoinCondition&0x01)<>0 AND @cbMemberOrder<@MemberOrder
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1111,"Msg":"抱歉，您的会员等级达不到报名条件，报名失败！"}'
		return 2				
	END
	
	-- 用户经验
	IF (@JoinCondition&0x02)<>0 AND @lExperience<@Experience
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1112,"Msg":"抱歉，您的经验值达不到报名条件，报名失败！"}'
		return 2				
	END
	
	-- 报名人数
	DECLARE @MaxSignupUser SMALLINT
	SELECT @MaxSignupUser=COUNT(1) FROM MatchScoreInfo(NOLOCK) WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo
	
	-- 用户信息
	DECLARE @IsAndroidUser TINYINT
	SET @IsAndroidUser=0
	
	-- 机器判断
	IF @strClientIP=N'0.0.0.0' SET @IsAndroidUser=1

	-- 判断人数
	IF @IsAndroidUser=0 AND @wMaxSignupUser>0 AND @MaxSignupUser>=@wMaxSignupUser
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1113,"Msg":"抱歉，本场比赛已达到最大报名人数，报名失败！"}'
		return 3
	END	

	-- 报名次数
	DECLARE @wSignupCount INT
	DECLARE @SignupTime DATETIME
	SELECT @wSignupCount=SignupCount,@SignupTime=SignupTime FROM MatchScoreInfo(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo

	-- 调整变量
	IF @wSignupCount IS NULL SET @wSignupCount=0

	-- 校验次数
	IF @SignupCount>0 AND @wSignupCount>=@SignupCount
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1115,"Msg":"您已经报名本场比赛，不需要重复报名！"}'
		return 4
	END	

	-- 变量定义
	DECLARE @cbSignupMode TINYINT
	DECLARE @dwSingupRecordID INT

	-- 设置变量
	SET @cbSignupMode=0
	SET @dwSingupRecordID=0

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 使用比赛券
	IF (@SignupMode&0x02)<>0 AND @cbSignupMode=0 AND @IsAndroidUser=0
	BEGIN	
		-- 查询比赛券
		DECLARE @TicketCount INT
		SELECT @TicketCount=BuyTicketCount-UseTicketCount FROM StreamMatchTicket(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo

		-- 调整变量
		IF @TicketCount IS NULL SET @TicketCount=0

		-- 赛券不足
		IF @TicketCount < @UseTicketCount		
		BEGIN
			-- 回滚事务
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED

			SET @strErrorDescribe = N'{"Idx":1290,"Msg":"您身上的比赛券不足, 无法参与比赛！"}'
			return 5				
		END

		-- 报名模式
		SET @cbSignupMode=0x02

		-- 更新赛券
		UPDATE StreamMatchTicket SET UseTicketCount+=@UseTicketCount WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo

		-- 插入记录	
		INSERT StreamMatchFeeInfo (UserID,ServerID,MatchID,MatchNo,MatchType,SignupMode,FeeMoneyID,FeeAmount,UseTicketCount,CollectDate) 
		VALUES(@dwUserID,@wServerID,@dwMatchID,@lMatchNo,@MatchType,@cbSignupMode,0,0,@UseTicketCount,GETDATE())	
		
		-- 记录标识
		SET @dwSingupRecordID=SCOPE_IDENTITY()
	END	

	-- 扣除报名费
	IF (@SignupMode&0x01)<>0 AND @cbSignupMode<>0 AND @IsAndroidUser=0
	BEGIN	
		-- 扣除费用
		IF @FeeAmount > 0
		BEGIN	
			-- 扣除钻石
			IF @FeeMoneyID = 1
			BEGIN
				-- 查询钻石
				DECLARE @UserIngot BIGINT
				SELECT @UserIngot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID
					
				-- 调整钻石
				IF @UserIngot IS NULL SET @UserIngot=0

				-- 钻石不足
				IF @UserIngot < @FeeAmount		
				BEGIN
					-- 回滚事务
					ROLLBACK TRAN
					SET TRANSACTION ISOLATION LEVEL READ COMMITTED

					SET @strErrorDescribe = N'{"Idx":1118,"Msg":"您身上的钻石不足,无法报名！"}'
					return 4				
				END

				-- 变更记录
				INSERT INTO LHGameRecordDB.dbo.RecordChangeIngot(UserID, GameID, TypeID, BeforeIngot, ChangeIngot,CollectClientIP, CollectDateTime)
				VALUES (@dwUserID, @dwGameID, @wReason, @UserIngot, -@FeeAmount,  N'-----------', GetDate())

				-- 更新钻石
				UPDATE LHAccountsDB.dbo.AccountsIngotInfo SET Score=Score-@FeeAmount WHERE UserID=@dwUserID		
			END		

			-- 扣除金币
			IF @FeeMoneyID = 2
			BEGIN
				-- 查询金币
				DECLARE @UserScore BIGINT
				SELECT @UserScore=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID
					
				-- 调整金币
				IF @UserScore IS NULL SET @UserScore=0

				-- 金币不足
				IF @UserScore < @FeeAmount		
				BEGIN
					SET @strErrorDescribe = N'{"Idx":1116,"Msg":"您身上的金币不足,无法报名！"}'
					return 4				
				END

				-- 变更记录
				INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID, GameID, TypeID, BeforeGold, ChangeGold, CollectClientIP, CollectDateTime)
				VALUES (@dwUserID, @dwGameID, @wReason, @UserScore, -@FeeAmount, N'------------', GetDate())

				-- 更新金币
				UPDATE LHAccountsDB.dbo.AccountsGoldInfo SET Score=Score-@FeeAmount WHERE UserID=@dwUserID					
			END					
							
		END	

		-- 报名模式
		SET @cbSignupMode=0x01

		-- 插入记录	
		INSERT StreamMatchFeeInfo (UserID,ServerID,MatchID,MatchNo,MatchType,SignupMode,FeeMoneyID,FeeAmount,UseTicketCount,CollectDate) 
		VALUES(@dwUserID,@wServerID,@dwMatchID,@lMatchNo,@MatchType,@cbSignupMode,@FeeMoneyID,@FeeAmount,0,GETDATE())
		
		-- 记录标识
		SET @dwSingupRecordID=SCOPE_IDENTITY()
	END		

	-- 更新分数
	IF @SignupTime IS NOT NULL
	BEGIN
		-- 首次报名
		IF @wSignupCount=0 OR @MatchType<>0x04
		BEGIN
			UPDATE MatchScoreInfo SET ServerID=@wServerID,Score=@InitScore,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0,PlayTimeCount=0,WinStreak=0,
			MatchRight=0x40,SignupTime=GetDate(),SignupCount+=1,SignupRecordID=@dwSingupRecordID
			WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo	
		END ELSE
		BEGIN
			UPDATE MatchScoreInfo SET ServerID=@wServerID,MatchRight=0x40,SignupTime=GetDate(),SignupCount+=1,SignupRecordID=@dwSingupRecordID 
			WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo	
		END
	END ELSE
	BEGIN
		INSERT INTO MatchScoreInfo (UserID,MatchID,MatchNo,GameID,ServerID,Score,MatchRight,SignupCount,SignupRecordID) 
		VALUES (@dwUserID,@dwMatchID,@lMatchNo,@dwGameID,@wServerID,@InitScore,0x40,1,@dwSingupRecordID)				
	END

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 查询金币
	DECLARE @CurrUserScore BIGINT
	SELECT @CurrUserScore=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID
	
	-- 查询钻石
	DECLARE @CurrUserIngot BIGINT
	SELECT @CurrUserIngot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID		

	-- 查询赛券
	DECLARE @CurrTicketCount SMALLINT
	SELECT @CurrTicketCount=BuyTicketCount-UseTicketCount FROM StreamMatchTicket(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo	
	
	-- 调整数据
	IF @CurrUserScore IS NULL SET @CurrUserScore=0
	IF @CurrUserIngot IS NULL SET @CurrUserIngot=0
	IF @CurrTicketCount IS NULL SET @CurrTicketCount=0

	-- 抛出数据
	SELECT @cbSignupMode AS SignupMode, @CurrUserScore AS UserScore, @CurrUserIngot AS UserIngot, @CurrTicketCount AS TicketCount
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- 取消报名
CREATE PROC GSP_GR_MatchUnSignup
	@dwUserID INT,								-- 用户 I D
	@wKindID INT,								-- 游戏 I D
	@wServerID INT,								-- 房间 I D
	@strClientIP NVARCHAR(15),					-- 连接地址
	@dwMatchID	INT,							-- 比赛 I D
	@lMatchNo BIGINT,							-- 比赛场次
	@wReason SMALLINT,							-- 变更原因
	@strMachineID NVARCHAR(32)	,				-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 存在判断
	DECLARE @GameID INT
	SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 校验标识
	IF @GameID IS NULL
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1091,"Msg":"抱歉,您的用户信息不存在，请联系客服中心工作人员了解情况！"}'
		return 1		
	END
	
	-- 比赛信息
	DECLARE @FeeMoneyID TINYINT
	DECLARE @FeeAmount BIGINT
	DECLARE @UseTicketCount SMALLINT
	DECLARE @SignupCount SMALLINT
	SELECT @SignupCount=SignupCount,@FeeMoneyID=FeeMoneyID,@FeeAmount=FeeAmount,@UseTicketCount=UseTicketCount FROM MatchOption(NOLOCK) WHERE MatchID=@dwMatchID

	-- 存在判断	
	IF @FeeMoneyID IS NULL
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1110,"Msg":"抱歉,您报名的比赛不存在！"}'
		return 2		
	END

	-- 查询记录
	DECLARE @SignupRecordID INT
	SELECT @SignupRecordID=SignupRecordID FROM MatchScoreInfo(NOLOCK) WHERE UserID=@dwUserID AND MatcHID=@dwMatchID AND MatchNo=@lMatchNo

	-- 存在判断	
	IF @SignupRecordID IS NULL
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1110,"Msg":"抱歉,您报名的比赛不存在！"}'
		return 3
	END	

	-- 查询记录
	DECLARE @SignupMode TINYINT	
	DECLARE @SignupDate DATETIME
	SELECT @SignupMode=SignupMode,@SignupDate=CollectDate FROM StreamMatchFeeInfo(NOLOCK) WHERE RecordID=@SignupRecordID

	-- 存在判断	
	IF @SignupMode IS NULL
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1110,"Msg":"抱歉,您报名的比赛不存在！"}'
		return 4
	END	

	-- 机器标识
	DECLARE @IsAndroidUser TINYINT	

	-- 机器判断
	SET @IsAndroidUser=0
	IF @strClientIP=N'0.0.0.0' SET @IsAndroidUser=1	

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 返还费用
	IF @IsAndroidUser=0
	BEGIN		
		-- 扣费模式
		IF @SignupMode=0x01 AND @FeeAmount>0
		BEGIN		

			-- 钻石支付
			IF @FeeMoneyID = 1
			BEGIN
				-- 查询钻石
				DECLARE @UserIngot BIGINT
				SELECT @UserIngot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID
					
				IF @UserIngot IS NULL
				BEGIN
					-- 回滚事务
					ROLLBACK TRAN
					SET TRANSACTION ISOLATION LEVEL READ COMMITTED

					SET @strErrorDescribe = N'{"Idx":1121,"Msg":系统未查询到您的钻石信息,请您与我们的客服人员联系！"}'
					return 3				
				END				

				-- 变更记录
				INSERT INTO LHGameRecordDB.dbo.RecordChangeIngot(UserID, GameID, TypeID, BeforeIngot, ChangeIngot,CollectClientIP, CollectDateTime)
				VALUES (@dwUserID, @GameID, @wReason, @UserIngot, @FeeAmount,  N'-----------', GetDate())

				-- 更新钻石
				UPDATE LHAccountsDB.dbo.AccountsIngotInfo SET Score+=@FeeAmount WHERE UserID=@dwUserID					
			END

			-- 金币支付
			IF @FeeMoneyID = 2
			BEGIN
				-- 查询金币
				DECLARE @Score BIGINT
				SELECT @Score=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID
				
				IF @Score IS NULL
				BEGIN
					-- 回滚事务
					ROLLBACK TRAN
					SET TRANSACTION ISOLATION LEVEL READ COMMITTED

					SET @strErrorDescribe = N'{"Idx":1119,"Msg":"系统未查询到您的金币信息,请您与我们的客服人员联系！"}'
					return 3						
				END

				-- 变更记录
				INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID, GameID, TypeID, BeforeGold, ChangeGold, CollectClientIP, CollectDateTime)
				VALUES (@dwUserID, @GameID, @wReason, @Score, -@FeeAmount, N'------------', GetDate())

				-- 更新金币
				UPDATE LHAccountsDB.dbo.AccountsGoldInfo SET Score=@Score+@FeeAmount WHERE UserID=@dwUserID				
			END				
		END	
		
		-- 比赛券模式
		IF @SignupMode=0x02
		BEGIN	
			-- 更新比赛券
			UPDATE StreamMatchTicket SET UseTicketCount-=@UseTicketCount WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
		END
	END

	-- 删除记录
	IF @IsAndroidUser=0 
	BEGIN
		DELETE StreamMatchFeeInfo WHERE RecordID=@SignupRecordID
	END

	-- 更新比赛分
	IF @SignupCount=1
	BEGIN
		DELETE MatchScoreInfo WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
	END ELSE
	BEGIN
		UPDATE MatchScoreInfo SET SignupCount-=1, MatchRight=0, SignupRecordID=0 WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
	END

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 查询金币
	DECLARE @CurrUserScore BIGINT
	SELECT @CurrUserScore=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID

	-- 查询钻石
	DECLARE @CurrUserIngot BIGINT
	SELECT @CurrUserIngot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID	

	-- 比赛券
	DECLARE @CurrUserTicket INT
	SELECT @CurrUserTicket=BuyTicketCount-UseTicketCount FROM StreamMatchTicket(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
	
	-- 调整数据
	IF @CurrUserScore IS NULL SET @CurrUserScore=0
	IF @CurrUserIngot IS NULL SET @CurrUserIngot=0
	IF @CurrUserTicket IS NULL SET @CurrUserTicket=0

	-- 抛出数据
	SELECT @SignupMode AS SignupMode, @CurrUserScore AS UserScore, @CurrUserIngot AS UserIngot, @CurrUserTicket AS UserTicket
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- 玩家复活
CREATE PROC GSP_GR_MatchRevive
	@dwUserID INT,								-- 用户 I D
	@dwMatchID	INT,							-- 比赛 I D	
	@lMatchNo	BIGINT,							-- 比赛场次
	@wServerID	SMALLINT,						-- 房间标识
	@lInitScore  BIGINT,						-- 初始积分	
	@wFeeMoneyID SMALLINT,						-- 费用类型
	@lFeeAmount  INT,							-- 费用金额
	@cbReviveTimes TINYINT,						-- 复活次数
	@wReason SMALLINT,							-- 变更原因	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN	

	-- 查询用户
	DECLARE @GameID INT	
	SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- 用户不存在
	IF @GameID IS NULL
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1091,"Msg":"抱歉,您的用户信息不存在，请联系客服中心工作人员了解情况！"}'
		return 1			
	END
	
	-- 查询复活信息
	DECLARE @ReviveTimes TINYINT
	SELECT @ReviveTimes=ReviveTimes FROM MatchReviveInfo(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo

	-- 信息不存在
	IF @ReviveTimes IS NULL SET @ReviveTimes=0;
	
	-- 判断次数
	IF @ReviveTimes>=@cbReviveTimes
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1126,"Msg":"抱歉，本场比赛最多允许复活 ' +CONVERT(nvarchar(20),@cbReviveTimes)+N' 次，复活失败！","Ops":["'+CONVERT(nvarchar(20),@cbReviveTimes)+'"]}'	
		RETURN 2					 
	END 

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN
		
	-- 扣除费用
	IF @lFeeAmount > 0
	BEGIN		
		-- 扣除钻石
		IF @wFeeMoneyID = 1
		BEGIN
			-- 查询钻石
			DECLARE @UserIngot BIGINT
			SELECT @UserIngot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID
					
			-- 调整钻石
			IF @UserIngot IS NULL SET @UserIngot=0

			-- 钻石不足
			IF @UserIngot < @lFeeAmount		
			BEGIN
				-- 回滚事务
				ROLLBACK TRAN
				SET TRANSACTION ISOLATION LEVEL READ COMMITTED

				SET @strErrorDescribe = N'{"Idx":1129,"Msg":"抱歉,您身上的钻石不足,复活失败！"}'
				return 4				
			END
						
			-- 变更记录
			INSERT INTO LHGameRecordDB.dbo.RecordChangeIngot(UserID, GameID, TypeID, BeforeIngot, ChangeIngot,CollectClientIP, CollectDateTime)
			VALUES (@dwUserID, @GameID, @wReason, @UserIngot, @lFeeAmount,  N'-----------', GetDate())

			-- 更新钻石
			UPDATE LHAccountsDB.dbo.AccountsIngotInfo SET Score=Score-@lFeeAmount WHERE UserID=@dwUserID		
		END

		-- 扣除金币
		IF @wFeeMoneyID = 2
		BEGIN
			-- 查询金币
			DECLARE @Score BIGINT
			SELECT @Score=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID
					
			-- 调整金币
			IF @Score IS NULL SET @Score=0

			-- 金币不足
			IF @Score < @lFeeAmount		
			BEGIN
				-- 回滚事务
				ROLLBACK TRAN
				SET TRANSACTION ISOLATION LEVEL READ COMMITTED

				SET @strErrorDescribe = N'{"Idx":1127,"Msg":"抱歉,您身上的游戏币不足,复活失败！"}'
				return 4				
			END

			-- 变更记录
			INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID, GameID, TypeID, BeforeGold, ChangeGold, CollectClientIP, CollectDateTime)
			VALUES (@dwUserID, @GameID, @wReason, @Score, -@lFeeAmount, N'------------', GetDate())

			-- 更新金币
			UPDATE LHAccountsDB.dbo.AccountsGoldInfo SET Score=@Score-@lFeeAmount WHERE UserID=@dwUserID					
		END		
		
	END
		
	-- 更新状态
	UPDATE MatchReviveInfo SET FeeAmount+=@lFeeAmount,ReviveTimes=ReviveTimes+1 
	WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo		
	
	-- 插入数据
	IF @@ROWCOUNT=0
	BEGIN
		-- 插入数据
		INSERT INTO MatchReviveInfo(UserID,MatchID,MatchNo,FeeMoneyID,FeeAmount,ReviveTimes,CollectDate)
		VALUES (@dwUserID,@dwMatchID,@lMatchNo,@wFeeMoneyID,@lFeeAmount,1,GetDate())
	END		
	
	-- 更新分数
	UPDATE MatchScoreInfo SET Score=@lInitScore,RankID=0,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0,PlayTimeCount=0,MatchRight=0x40,SignupTime=GetDate()
	WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo				
	
	-- 插入分数
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO MatchScoreInfo (UserID,MatchID,MatchNo,GameID,ServerID,Score,MatchRight) 
		VALUES (@dwUserID,@dwMatchID,@lMatchNo,@GameID,@wServerID,@lInitScore,0x40) 				
	END

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	
	-- 查询金币
	DECLARE @CurrUserScore BIGINT
	SELECT @CurrUserScore=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID

	-- 查询钻石
	DECLARE @CurrUserIngot BIGINT
	SELECT @CurrUserIngot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID	
	
	-- 调整数据
	IF @CurrUserScore IS NULL SET @CurrUserScore=0	
	IF @CurrUserIngot IS NULL SET @CurrUserIngot=0

	-- 抛出数据
	SELECT @CurrUserScore AS UserScore, @CurrUserIngot AS UserIngot
END

RETURN 0
GO


----------------------------------------------------------------------------------------------------

-- 撤销复活
CREATE PROC GSP_GR_MatchReviveRepeal
	@dwUserID INT,								-- 用户 I D
	@dwMatchID INT,								-- 比赛 I D	
	@lMatchNo  BIGINT,							-- 比赛场次	
	@wFeeMoneyID  SMALLINT,						-- 费用类型
	@lFeeAmount INT,							-- 费用金额
	@wReason SMALLINT							-- 变更原因
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN	
	
	-- 查询用户
	DECLARE @GameID INT	
	SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- 用户不存在
	IF @GameID IS NULL
	BEGIN
		return 1			
	END

	-- 查询复活信息	
	IF EXISTS(SELECT * FROM MatchReviveInfo(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo)
	BEGIN
		RETURN 1
	END
	
	-- 退还费用
	IF @lFeeAmount > 0
	BEGIN				
		-- 扣除金币
		IF @wFeeMoneyID = 2
		BEGIN
			-- 查询金币
			DECLARE @UserScore BIGINT
			SELECT  @UserScore=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID	

			-- 更新金币
			UPDATE LHAccountsDB.dbo.AccountsGoldInfo SET Score=Score+@lFeeAmount WHERE UserID=@dwUserID		
			
			-- 变更记录
			INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID, GameID, TypeID, BeforeGold, ChangeGold, CollectClientIP, CollectDateTime)
			VALUES (@dwUserID, @GameID, @wReason, @UserScore, @lFeeAmount, N'------------', GetDate())
		END
				
		-- 扣除钻石
		IF @wFeeMoneyID = 1
		BEGIN
			-- 查询钻石
			DECLARE @UserIngot BIGINT
			SELECT @UserIngot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID
					
			-- 调整钻石
			IF @UserIngot IS NULL SET @UserIngot=0
						
			-- 变更记录
			INSERT INTO LHGameRecordDB.dbo.RecordChangeIngot(UserID, GameID, TypeID, BeforeIngot, ChangeIngot,CollectClientIP, CollectDateTime)
			VALUES (@dwUserID, @GameID, @wReason, @UserIngot, @lFeeAmount,  N'-----------', GetDate())

			-- 更新钻石
			UPDATE LHAccountsDB.dbo.AccountsIngotInfo SET Score=Score+@lFeeAmount WHERE UserID=@dwUserID		
		END
	END
		
	-- 更新状态
	UPDATE MatchReviveInfo SET FeeAmount=FeeAmount-@lFeeAmount,ReviveTimes=ReviveTimes-1 
	WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo				
	
	-- 删除分数
	DELETE MatchScoreInfo WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo				
	
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
