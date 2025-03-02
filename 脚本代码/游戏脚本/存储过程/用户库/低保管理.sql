

----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QuerySubsidy]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QuerySubsidy]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LargessSubsidy]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LargessSubsidy]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QuerySubsidy]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QuerySubsidy]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LargessSubsidy]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LargessSubsidy]
GO

----------------------------------------------------------------------------------------------------
-- 查询低保
CREATE PROC [dbo].[GSP_GP_QuerySubsidy]
	@dwUserID int,							 -- 用户ID
	@szMachine NVARCHAR(32),				 -- 机器标识
	@strErrorDescribe	NVARCHAR(127) OUTPUT -- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

BEGIN
	-- 查询用户
	IF NOT Exists(SELECT * FROM AccountsInfo WHERE UserID=@dwUserID)	
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1091,"Msg":"抱歉，您的用户信息不存在，请联系客服中心！"}'
		RETURN 1
	END

	-- 锁表查询
	IF Exists(SELECT 1 FROM AccountsOnlineInfo WHERE UserID=@dwUserID AND ServerID<>0)	
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1092,"Msg":"抱歉，系统检测到你当前正在游戏房间中，请退出房间再来领取！"}'
		RETURN 2
	END
 
	-- 计算时间
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 低保信息
	IF NOT Exists(SELECT 1 FROM AccountsSubsidyClaim WHERE DateID=@DateID AND UserID=@dwUserID)
	BEGIN
		-- 插入记录
		INSERT INTO AccountsSubsidyClaim(DateID, UserID, MachineID) VALUES (@DateID, @dwUserID,  @szMachine)
	END ELSE
	BEGIN
		-- 更新记录
		UPDATE AccountsSubsidyClaim SET MachineID=@szMachine WHERE DateID=@DateID AND UserID=@dwUserID	
	END	

	-- 低保配置
	DECLARE @ClaimGold BIGINT
	DECLARE @ClaimCondition BIGINT
	DECLARE	@ClaimTimesPerAccounts TINYINT
	
	-- 查询配置
	SELECT @ClaimGold=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND StatusName='SubsidyGold'
	SELECT @ClaimCondition=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND StatusName='SubsidyCondition'
	SELECT @ClaimTimesPerAccounts=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND StatusName='SubsidyTimesPerAccounts'

	-- 调整配置
	IF @ClaimGold IS NULL SET @ClaimGold=0
	IF @ClaimCondition IS NULL SET @ClaimCondition=0
	IF @ClaimTimesPerAccounts IS NULL SET @ClaimTimesPerAccounts=0
	
	-- 领取记录
	DECLARE @ClaimTimes TINYINT
	SELECT @ClaimTimes=ClaimTimes FROM AccountsSubsidyClaim WHERE DateID=@DateID AND UserID=@dwUserID

	-- 判断领取条件	
	DECLARE @CurrScore BIGINT
	DECLARE @CurrInsure BIGINT
	SELECT @CurrScore=Score, @CurrInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	
	-- 调整金币
	IF @CurrScore IS NULL SET @CurrScore=0
	IF @CurrInsure IS NULL SET @CurrInsure=0

	-- 抛出记录
	SELECT @ClaimTimes AS ClaimTimes, @ClaimGold AS ClaimGold, @ClaimCondition AS ClaimCondition, @ClaimTimesPerAccounts AS ClaimTimesPerAccounts,
		   @CurrScore AS CurrScore, @CurrInsure AS CurrInsureScore	 			
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
-- 赠送低保
CREATE PROC [dbo].[GSP_GP_LargessSubsidy]
	@dwUserID INT,							 -- 用户ID
	@szPassword NVARCHAR(33),				 -- 账号密码
	@szClientIP	NVARCHAR(15),				 -- 连接地址
	@szMachine NVARCHAR(32),				 -- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT	 -- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON
BEGIN	
	-- 查询用户
	DECLARe @Nullity TINYINT
	DECLARE @LogonPass NVARCHAR(33)
	DECLARE @LogonMachine NVARCHAR(32)
	SELECT @LogonPass=LogonPass,@LogonMachine=LogonMachineID,@Nullity=Nullity FROM AccountsInfo WHERE UserID=@dwUserID 
	
	-- 存在判断
	IF @LogonPass IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 1
	END

	-- 账号被冻结
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
		RETURN 2		
	END

	-- 密码校验
	IF @LogonPass<>@szPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 3		
	END

	-- 锁表查询
	IF Exists(SELECT 1 FROM AccountsOnlineInfo WHERE UserID=@dwUserID AND ServerID<>0)	
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1092,"Msg":"抱歉，系统检测到你当前正在游戏房间中，请退出房间再来领取！"}'
		RETURN 4
	END

	-- 低保配置
	DECLARE @ClaimGold BIGINT
	DECLARE @ClaimCondition BIGINT
	DECLARE	@ClaimTimesPerMachine TINYINT
	DECLARE	@ClaimTimesPerAccounts TINYINT
	
	-- 查询配置
	SELECT @ClaimGold=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND StatusName='SubsidyGold'
	SELECT @ClaimCondition=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND StatusName='SubsidyCondition'
	SELECT @ClaimTimesPerMachine=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND  StatusName='SubsidyTimesPerMachine'
	SELECT @ClaimTimesPerAccounts=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND  StatusName='SubsidyTimesPerAccounts'	

	-- 调整配置
	IF @ClaimGold IS NULL SET @ClaimGold=400000
	IF @ClaimCondition IS NULL SET @ClaimCondition=400000
	IF @ClaimTimesPerMachine IS NULL SET @ClaimTimesPerMachine=3	
	IF @ClaimTimesPerAccounts IS NULL SET @ClaimTimesPerAccounts=3

	-- 判断领取条件	
	DECLARE @CurrScore BIGINT
	DECLARE @CurrInsure BIGINT
	SELECT @CurrScore=Score, @CurrInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	
	-- 调整金币
	IF @CurrScore IS NULL SET @CurrScore=0
	IF @CurrInsure IS NULL SET @CurrInsure=0

	-- 判断金币
	IF (@CurrScore+@CurrInsure)>=@ClaimCondition OR @CurrScore<0 OR @CurrInsure<0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1093,"Msg":"抱歉，您当前身上的游戏币不符合赠送条件！"}'
		RETURN 5		
	END		

	-- 计算时间
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 查询低保
	DECLARE @ClaimTimes TINYINT
	DECLARE @MachineID NVARCHAR(32)
	SELECT @ClaimTimes=ClaimTimes,@MachineID=MachineID FROM AccountsSubsidyClaim WHERE DateID=@DateID AND UserID=@dwUserID

	-- 存在判断
	IF @ClaimTimes IS NULL OR @MachineID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1094,"Msg":"抱歉，您的低分赠送记录出现异常，请联系客服中心！"}'
		RETURN 6					
	END

	-- 校验机器
	IF @szMachine<>@LogonMachine
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1095,"Msg":"抱歉，您的机器出现异常，游戏币赠送失败！"}'
		RETURN 7			
	END	

	-- 校验机器
	IF @szMachine<>@MachineID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1095,"Msg":"抱歉，您的机器出现异常，游戏币赠送失败！"}'
		RETURN 7			
	END

	-- 判断领取次数
	IF @ClaimTimes>=@ClaimTimesPerAccounts
	BEGIN		
		SET @strErrorDescribe=N'{"Idx":1096,"Msg":"抱歉，您今天已领取完系统赠送给您的游戏币，您还可以通过充值来获取游戏币！"}'
		RETURN 8																																
	END	

	-- 查询机器
	DECLARE @MachineClaimTimes TINYINT
	DECLARE @UserIDString NVARCHAR(512)
	SELECT @MachineClaimTimes=PresentCount,@UserIDString=UserIDString FROM RecordSubsidyMachine	WHERE DateID=@DateID AND MachineID=@szMachine

	-- 调整变量
	IF @MachineClaimTimes IS NULL
	BEGIN
		-- 设置变量
		SET @UserIDString=N''
		SET @MachineClaimTimes=0		

		-- 插入记录
		INSERT INTO RecordSubsidyMachine(DateID, MachineID, PresentGold, PresentCount, UserIDString, FirstGrantDate, LastGrantDate)
		VALUES (@DateID, @szMachine, 0, 0, '', GetDate(), GetDate())		
	END
	
	-- 判断机器次数
	IF @MachineClaimTimes>=@ClaimTimesPerMachine
	BEGIN		
		SET @strErrorDescribe=N'{"Idx":1098,"Msg":"抱歉，您所在的机器今天已领取完系统赠送的游戏币，您还可以通过充值来获取游戏币！"}'
		RETURN 9																																
	END

	-- 赠送金额	
	DECLARE @PresentScore BIGINT
    SET @PresentScore=@ClaimGold-@CurrScore

	-- 低保记录
	UPDATE AccountsSubsidyClaim SET ClaimTimes=ClaimTimes+1, ClaimGolds=ClaimGolds+@PresentScore WHERE DateID=@DateID AND UserID=@dwUserID	

	-- 标识判断
	DECLARE @szUserID NVARCHAR(12)
	SELECT @szUserID=CAST(@dwUserID AS nvarchar(12))

	-- 存在判断
	IF @UserIDString=N'' 
	BEGIN
		SET @UserIDString=@szUserID				
	END ELSE
	BEGIN
		SET @UserIDString=@UserIDString + N',' + @szUserID	
	END		

	-- 更新记录	
	UPDATE RecordSubsidyMachine SET PresentGold=PresentGold+@PresentScore,PresentCount=PresentCount+1,UserIDString=@UserIDString,LastGrantDate=GetDate()
	WHERE DateID=@DateID AND MachineID=@szMachine
		
	-- 写入财富
	EXEC GSP_GP_WriteGameWealth @dwUserID,2,15,@PresentScore	

	-- 变量定义
	DECLARE	@OutCurrGold BIGINT
	DECLARE @OutClaimTimes TINYINT	

	-- 查询领取次数
	SELECT @OutClaimTimes=ClaimTimes FROM AccountsSubsidyClaim WHERE DateID=@DateID AND UserID=@dwUserID
	
	-- 查询当前金币
	SELECT @OutCurrGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID

	-- 抛出提示
	SET @strErrorDescribe=N'{"Idx":1099,"Msg":"恭喜您，游戏币赠送成功！"}'
	
	-- 抛出数据
	SELECT @OutClaimTimes AS ClaimdTimes, @OutCurrGold AS CurrGold
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
-- 查询低保
CREATE PROC [dbo].[GSP_GR_QuerySubsidy]
	@dwUserID int,							 -- 用户ID
	@szMachine NVARCHAR(32),				 -- 机器标识
	@strErrorDescribe	NVARCHAR(127) OUTPUT -- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

BEGIN
	-- 查询用户
	IF NOT Exists(SELECT 1 FROM AccountsInfo WHERE UserID=@dwUserID)	
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1091,"Msg":"抱歉，您的用户信息不存在，请联系客服中心！"}'
		RETURN 1
	END

	-- 计算时间
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 更新记录
	UPDATE AccountsSubsidyClaim SET MachineID=@szMachine WHERE DateID=@DateID AND UserID=@dwUserID	

	-- 插入信息
	IF @@ROWCOUNT=0	
	BEGIN
		-- 插入记录
		INSERT INTO AccountsSubsidyClaim(DateID, UserID, MachineID) VALUES (@DateID, @dwUserID, @szMachine)
	END	

	-- 低保配置
	DECLARE @ClaimGold BIGINT
	DECLARE @ClaimCondition BIGINT
	DECLARE	@ClaimTimesPerMachine TINYINT
	DECLARE	@ClaimTimesPerAccounts TINYINT
	
	-- 查询配置
	SELECT @ClaimGold=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND  StatusName='SubsidyGold'
	SELECT @ClaimCondition=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND  StatusName='SubsidyCondition'
	SELECT @ClaimTimesPerMachine=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND  StatusName='SubsidyTimesPerMachine'
	SELECT @ClaimTimesPerAccounts=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND  StatusName='SubsidyTimesPerAccounts'

	-- 调整配置
	IF @ClaimGold IS NULL SET @ClaimGold=400000
	IF @ClaimCondition IS NULL SET @ClaimCondition=400000
	IF @ClaimTimesPerMachine IS NULL SET @ClaimTimesPerMachine=3	
	IF @ClaimTimesPerAccounts IS NULL SET @ClaimTimesPerAccounts=3

	-- 查询机器
	DECLARE @MachineClaimTimes TINYINT		 
	SELECT @MachineClaimTimes=PresentCount FROM RecordSubsidyMachine WHERE DateID=@DateID AND MachineID=@szMachine		
	
	-- 领取记录
	DECLARE @ClaimTimes TINYINT
	DECLARE @RechargeStatus TINYINT
	SELECT @ClaimTimes=ClaimTimes FROM AccountsSubsidyClaim WHERE DateID=@DateID AND UserID=@dwUserID

	-- 剩余次数
	DECLARE @ResidualTimes TINYINT
	SET @ResidualTimes=@ClaimTimesPerAccounts-@ClaimTimes
		
	-- 调整剩余
	IF @ResidualTimes > (@ClaimTimesPerMachine-@MachineClaimTimes)
	BEGIN
		SET @ResidualTimes=@ClaimTimesPerMachine-@MachineClaimTimes	
	END

	-- 溢出处理
	IF @ResidualTimes > @ClaimTimesPerAccounts SET @ResidualTimes=0	

	-- 查询低保需要排除的金额
	/*
	1、邮件未领取
	2、道具消耗、转账消耗  (这里应该只计算每日的，否则累计值太大低保就没意义)  -- 待定
	*/
	DECLARE @nConsumeScore BIGINT
	SELECT @nConsumeScore=SUM(GoodsCount) FROM LHAccountsDB..AccountsMailAttach(NOLOCK) WHERE GoodsID=2 AND MailID IN (
		SELECT MailID FROM LHAccountsDB..AccountsMailBox(NOLOCK) WHERE UserID=@dwUserID AND MailState IN (1,2)
	)
	IF @nConsumeScore IS NULL SET @nConsumeScore = 0

	-- 抛出记录
	SELECT @ResidualTimes AS ResidualTimes,@nConsumeScore AS ConsumeScore
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
-- 赠送低保
CREATE PROC [dbo].[GSP_GR_LargessSubsidy]
	@dwUserID INT,							 -- 用户ID
	@dwClientIP INT,						 -- 连接地址
	@szClientIP	NVARCHAR(15),				 -- 连接地址
	@szMachine NVARCHAR(32),				 -- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT	 -- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON
BEGIN	

	-- 低保配置
	DECLARE @ClaimGold BIGINT
	DECLARE @ClaimCondition BIGINT
	DECLARE	@ClaimTimesPerMachine TINYINT
	DECLARE	@ClaimTimesPerIP TINYINT
	DECLARE	@ClaimTimesPerAccounts TINYINT

	-- 查询配置
	SELECT @ClaimGold=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND StatusName='SubsidyGold'
	SELECT @ClaimCondition=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND StatusName='SubsidyCondition'
	SELECT @ClaimTimesPerMachine=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND StatusName='SubsidyTimesPerMachine'
	SELECT @ClaimTimesPerIP=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND StatusName='SubsidyTimesPerIP'
	SELECT @ClaimTimesPerAccounts=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND StatusName='SubsidyTimesPerAccounts'	

	-- 调整配置
	IF @ClaimGold IS NULL SET @ClaimGold=400000
	IF @ClaimCondition IS NULL SET @ClaimCondition=400000
	IF @ClaimTimesPerMachine IS NULL SET @ClaimTimesPerMachine=3	
	IF @ClaimTimesPerIP IS NULL SET @ClaimTimesPerIP=3
	IF @ClaimTimesPerAccounts IS NULL SET @ClaimTimesPerAccounts=3

	-- 判断领取条件	
	DECLARE @CurrScore BIGINT
	DECLARE @CurrInsure BIGINT
	DECLARE @Commission BIGINT
	DECLARE @CurrMailScore BIGINT
	SELECT @CurrScore=Score, @CurrInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	SELECT @Commission=Commission FROM AccountsCommission WHERE UserID=@dwUserID
	SELECT @CurrMailScore=SUM(GoodsCount) FROM AccountsMailAttach WHERE GoodsID=1 AND MailID IN (SELECT MailID FROM AccountsMailBox WHERE UserID=@dwUserID AND MailState=1)

	-- 调整金币
	IF @CurrScore IS NULL SET @CurrScore=0
	IF @CurrInsure IS NULL SET @CurrInsure=0
	IF @Commission IS NULL SET @Commission=0
	IF @CurrMailScore IS NULL SET @CurrMailScore=0

	-- 判断金币
	IF (@CurrScore+@CurrInsure+@Commission+@CurrMailScore)>=@ClaimCondition OR @CurrScore<0 OR @CurrInsure<0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1093,"Msg":"抱歉，您当前身上的游戏币不符合赠送条件！"}'
		RETURN 5		
	END		

	-- 计算时间
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 查询低保
	DECLARE @PStationID INT
	DECLARE @ClaimTimes TINYINT
	DECLARE @MachineID NVARCHAR(32)
	SELECT @ClaimTimes=ClaimTimes,@MachineID=MachineID FROM AccountsSubsidyClaim WHERE DateID=@DateID AND UserID=@dwUserID

	-- 存在判断
	IF @ClaimTimes IS NULL OR @MachineID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1109,"Msg":"抱歉，您的低分赠送记录出现异常，请联系客服中心！"}'
		RETURN 6					
	END

	-- 校验机器
	IF @szMachine<>@MachineID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1095,"Msg":"抱歉，您的机器出现异常，游戏币赠送失败！"}'
		RETURN 7			
	END	

	-- 判断领取次数
	IF @ClaimTimes>=@ClaimTimesPerAccounts
	BEGIN		
		SET @strErrorDescribe=N'{"Idx":1096,"Msg":"抱歉，您今天已领取完系统赠送给您的游戏币，您还可以通过充值来获取游戏币！"}'
		RETURN 8																																
	END	

	-- 查询机器
	DECLARE @MachineClaimTimes TINYINT
	DECLARE @UserIDString NVARCHAR(512)
	SELECT @MachineClaimTimes=PresentCount,@UserIDString=UserIDString FROM RecordSubsidyMachine WHERE DateID=@DateID AND MachineID=@szMachine

	-- 调整变量
	IF @MachineClaimTimes IS NULL
	BEGIN
		-- 设置变量
		SET @UserIDString=N''
		SET @MachineClaimTimes=0		

		-- 插入记录
		INSERT INTO RecordSubsidyMachine(DateID, MachineID, PresentGold, PresentCount, UserIDString, FirstGrantDate, LastGrantDate)
		VALUES (@DateID, @szMachine, 0, 0, N'', GetDate(), GetDate())		
	END
	
	-- 判断机器次数
	IF @MachineClaimTimes>=@ClaimTimesPerMachine
	BEGIN		
		SET @strErrorDescribe=N'{"Idx":1098,"Msg":"抱歉，您所在的机器今天已领取完系统赠送的游戏币，您还可以通过充值来获取游戏币！"}'
		RETURN 9																																
	END

	-- 查询IP
	DECLARE @IPClaimTimes TINYINT
	SELECT @IPClaimTimes=PresentCount,@UserIDString=UserIDString FROM RecordSubsidyClientIP WHERE DateID=@DateID AND ClientIP=@szClientIP

	-- 调整变量
	IF @IPClaimTimes IS NULL
	BEGIN
		-- 设置变量
		SET @UserIDString=N''
		SET @IPClaimTimes=0		

		-- 插入记录
		INSERT INTO RecordSubsidyClientIP(DateID, ClientIP, PresentGold, PresentCount, UserIDString, FirstGrantDate, LastGrantDate)
		VALUES (@DateID, @szClientIP, 0, 0, N'', GetDate(), GetDate())		
	END
	
	-- 判断IP次数
	IF @IPClaimTimes>=@ClaimTimesPerIP
	BEGIN		
		SET @strErrorDescribe=N'{"Idx":1338,"Msg":"抱歉，您所在的IP今天已领取完系统赠送的游戏币，您还可以通过充值来获取游戏币！"}'
		RETURN 9																																
	END

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ	
	BEGIN TRAN

	-- 赠送金额	
	DECLARE @PresentScore BIGINT
    SET @PresentScore=@ClaimGold-@CurrScore

	-- 低保记录
	UPDATE AccountsSubsidyClaim SET ClaimTimes=ClaimTimes+1,ClaimGolds=ClaimGolds+@PresentScore WHERE DateID=@DateID AND UserID=@dwUserID	

	-- 标识判断
	DECLARE @szUserID NVARCHAR(12)
	SELECT @szUserID=CAST(@dwUserID AS nvarchar(12))

	-- 存在判断
	IF @UserIDString=N'' 
	BEGIN
		SET @UserIDString=@szUserID				
	END ELSE
	BEGIN
		SET @UserIDString=@UserIDString + N',' + @szUserID	
	END		

	-- 更新记录	
	UPDATE RecordSubsidyMachine SET PresentGold=PresentGold+@PresentScore,PresentCount=PresentCount+1,UserIDString=@UserIDString,LastGrantDate=GetDate()
	WHERE DateID=@DateID AND MachineID=@szMachine
	UPDATE RecordSubsidyClientIP SET PresentGold=PresentGold+@PresentScore,PresentCount=PresentCount+1,UserIDString=@UserIDString,LastGrantDate=GetDate()
	WHERE DateID=@DateID AND ClientIP=@szClientIP
	
	-- 写入财富
	EXEC GSP_GP_WriteGameWealth @dwUserID,1,15,@PresentScore

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	
	-- 变量定义
	DECLARE	@OutCurrGold BIGINT
	DECLARE @OutClaimTimes TINYINT	

	-- 查询数据
	SELECT @OutClaimTimes=ClaimTimes FROM AccountsSubsidyClaim WHERE DateID=@DateID AND UserID=@dwUserID

	-- 查询当前金币
	SELECT @OutCurrGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID

	-- 剩余次数
	DECLARE @ResidualTimes TINYINT
	SET @ResidualTimes=@ClaimTimesPerAccounts-@OutClaimTimes
		
	-- 调整剩余
	IF @ResidualTimes>(@ClaimTimesPerMachine-@MachineClaimTimes-@IPClaimTimes)
	BEGIN
		SET @ResidualTimes=@ClaimTimesPerMachine-@MachineClaimTimes-@IPClaimTimes
	END

	-- 溢出处理
	IF @ResidualTimes>@ClaimTimesPerAccounts SET @ResidualTimes=0
	
	-- 抛出提示
	SET @strErrorDescribe=N'{"Idx":1099,"Msg":"恭喜您，游戏币赠送成功！"}'
	
	-- 抛出数据
	SELECT @ResidualTimes AS ResidualTimes, @OutCurrGold AS CurrGold
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
