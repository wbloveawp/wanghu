
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryClubScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryClubScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ModifyClubScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ModifyClubScore]
GO

--IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_PresentClubScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
--DROP PROCEDURE [dbo].[GSP_GR_PresentClubScore]
--GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- 查询积分
CREATE  PROCEDURE dbo.GSP_GR_QueryClubScore
	@dwMemberID INT,			-- 成员标识	
	@dwClubID	INT				-- 群组标识			
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN	

	-- 查询用户
	DECLARE @ClubScore INT

	-- 变量定义
	DECLARE	@TableID	INT																			
	DECLARE	@TableName	NVARCHAR(128)								
	DECLARE @Sqlstr		NVARCHAR(512)

	-- 分表标识				
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- 群组成员表
	SET	@TableName='ClubMember_';
	SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

	-- 查询数据表
	IF (EXISTS(SELECT 1 FROM LHGameBattleDB.dbo.sysobjects WHERE xtype='u' AND name=@TableName))
	BEGIN
		-- 查询脚本		
		SET	@Sqlstr = CONCAT( ' SELECT @Score=ClubScore FROM  ',@TableName,'(NOLOCK) WHERE MemberID=',CONVERT(NVARCHAR(18), @dwMemberID),' AND ',
								' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

		-- 执行脚本
		EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@Score INT OUT',@Score=@ClubScore OUTPUT
	END

	-- 调整分数
	IF @ClubScore IS NULL SET @ClubScore=0

	SELECT @ClubScore AS ClubScore
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 修改积分
CREATE PROC GSP_GR_ModifyClubScore
	-- 用户信息
	@dwUserID INT,								-- 用户 I D
	@dwClubID INT,								-- 茶馆标识
	@dwMemberID INT,							-- 成员标识
	@lModifyScore INT,							-- 变更分数	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT

-- 执行逻辑
BEGIN
	-- 查询用户
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

	-- 查询馆员
	DECLARE @MemberGameID INT
	DECLARE @MemberNickName NVARCHAR(32)
	SELECT @MemberGameID=GameID,@MemberNickName=NickName FROM LHAccountsDB.dbo.AccountsInfo WHERE UserID=@dwMemberID

	-- 查询用户
	IF @MemberGameID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1202,"Msg":"系统未查询到改成员的用户信息，请查证后再次尝试登录！"}'
		RETURN 4
	END
	
	-- 查询群主
	DECLARE @ClubName NVARCHAR(32)
	SELECT @ClubName=ClubName FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- 结果判断
	IF @ClubName IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'
		RETURN 5		
	END

	-- 茶馆分模式
	DECLARE @ClubScoreMode TINYINT
	--SELECT @ClubScoreMode=CONVERT(TINYINT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'ClubOption' AND StatusName='ClubScoreMode'
	
	-- 调整变量
	IF @ClubScoreMode IS NULL SET @ClubScoreMode=0

	-- 变量定义
	DECLARE @PayUserID INT									
	DECLARE @PayUserGameID INT	
	DECLARE @PayUserType TINYINT
	DECLARE	@MemberScore INT	

	-- 变量定义
	DECLARE	@TableID INT			
	DECLARE @Sqlstr		NVARCHAR(512)								
	DECLARE	@TableName	NVARCHAR(128)					

	-- 设置分数
	SET @PayUserType=0
	SET @PayUserID=@dwUserID

	-- 分表标识				
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- 群组成员表
	SET	@TableName='ClubMember_';
	SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

	-- 查询操作员信息
	DECLARE @OUT_OperaterMangeRight INT
	DECLARE @OUT_OperaterMemberType SMALLINT				
	SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType,@ManageRight=ManageRight FROM ',@TableName,'(NOLOCK)',
							' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwUserID))				  
	
	-- 执行语句
	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT,@ManageRight INT OUT',@MemberType=@OUT_OperaterMemberType OUTPUT,@ManageRight=@OUT_OperaterMangeRight OUTPUT

	-- 调整数据
	IF @OUT_OperaterMangeRight IS NULL SET @OUT_OperaterMangeRight=0
	IF @OUT_OperaterMemberType IS NULL SET @OUT_OperaterMemberType=0

	-- 校验类型
	IF @OUT_OperaterMemberType<3 OR (@OUT_OperaterMemberType=3 AND (@OUT_OperaterMangeRight&0x0020)=0)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1203,"Msg":"抱歉，您没有权限增减积分！"}'
		RETURN 6
	END

	-- 设置类型
	SET @PayUserType=@OUT_OperaterMemberType

	-- 免充值模式
	--IF  @ClubScoreMode=1	
	--BEGIN
	--	-- 查询分数		
	--	SET	@Sqlstr = CONCAT( ' SELECT @ClubScore=ClubScore FROM  ',@TableName,' WHERE MemberID=',CONVERT(NVARCHAR(18), @PayUserID),' AND ',
	--						  ' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

	--	-- 执行脚本
	--	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@ClubScore INT OUT',@ClubScore=@PayUserScore OUTPUT		
		
	--	-- 调整分数
	--	IF @PayUserScore IS NULL
	--	BEGIN
	--		SET @strErrorDescribe=N'系统未查询到您的积分信息，请联系客户人员了解详情！'
	--		RETURN 6
	--	END

	--	-- 校验馆分	
	--	IF @lModifyScore>0 AND @PayUserScore<@lModifyScore
	--	BEGIN
	--		-- 提示信息
	--		SET @strErrorDescribe='由于您的积分不足，请调整好积分额度后再次尝试！'		
	--		RETURN 7		
	--	END		
	--END	

	-- 锁定信息
	DECLARE @BattleNum INT
	DECLARE @KindID SMALLINT
	DECLARE @ServerID SMALLINT
	DECLARE @ScoreKind TINYINT	
	DECLARE @BattleStatus TINYINT
	DECLARE @OnlineStatus TINYINT
	SELECT @KindID=KindID,@ServerID=ServerID,@ScoreKind=ScoreKind,@BattleNum=BattleNum,@BattleStatus=BattleStatus,@OnlineStatus=OnlineStatus FROM BattleUserLocker(NOLOCK) WHERE UseriD=@dwMemberID
	
	-- 调整变量
	IF @BattleStatus IS NULL
	BEGIN
		SELECT @KindID=0,@ServerID=0,@ScoreKind=0,@BattleStatus=0,@OnlineStatus=0	
	END ELSE
	BEGIN
		IF @OnlineStatus=0 OR @ScoreKind<>2
		BEGIN
			SELECT @KindID=0,@ServerID=0
		END
	END

	-- 馆主减分
	IF @lModifyScore<0 AND @OnlineStatus=1
	BEGIN
		-- 提示信息
		SET @strErrorDescribe=N'{"Idx":1204,"Msg":"由于该馆员在约战房间['+CONVERT(NVARCHAR(18),@BattleNum)+']中的游戏尚未完成，暂时不能进行下分操作！","Ops":["'+CONVERT(NVARCHAR(18),@BattleNum)+'"]}'
		RETURN 7		
	END	
	
	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 查询分数		
	SET	@Sqlstr = CONCAT( ' SELECT @ClubScore=ClubScore FROM  ',@TableName,' WITH (UPDLOCK) WHERE MemberID=',CONVERT(NVARCHAR(18), @dwMemberID),' AND ',
						  ' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

	-- 执行脚本
	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@ClubScore INT OUT',@ClubScore=@MemberScore OUTPUT
	

	-- 调整分数
	IF @MemberScore IS NULL SET @MemberScore=0

	---- 校验分数
	--IF @lModifyScore<0 AND @MemberScore<-@lModifyScore
	--BEGIN				
	--	-- 事务回滚
	--	ROLLBACK TRAN
	--	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	--	-- 提示信息
	--	SET @strErrorDescribe='由于该馆员身上的茶馆分不足['+CONVERT(NVARCHAR(18),@lModifyScore/100)+']，请调整分数后再次操作！'		
	--	RETURN 4				
	--END

	-- 更新分数		
	SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName,' SET ClubScore=ClubScore+',CONVERT(NVARCHAR(18), @lModifyScore),' WHERE MemberID=',CONVERT(NVARCHAR(18), @dwMemberID),' AND ',
						  ' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

	-- 执行脚本
	EXECUTE(@Sqlstr)	
	IF @@ERROR<>0
	BEGIN				
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 提示信息
		SET @strErrorDescribe=N'{"Idx":1205,"Msg":"抱歉，由于系统原因，操作失败！"}'
		RETURN 5				
	END

	-- 设置分数
	SET @MemberScore=@MemberScore+@lModifyScore

	-- 变量定义
	DECLARE @DateID	INT
	DECLARE @TradeType SMALLINT	
	DECLARE @IncreaseScore BIGINT
	DECLARE @DecreaseScore BIGINT

	-- 计算日期
	SET @DateID = CAST(CAST(GETDATE() AS FLOAT) AS INT)

	--设置类型
	SELECT @TradeType=2,@IncreaseScore=@lModifyScore,@DecreaseScore=0
	IF @lModifyScore<0 
	BEGIN
		SELECT @TradeType=3,@IncreaseScore=0,@DecreaseScore=@lModifyScore
	END
	
	-- 积分记录
	UPDATE StreamClubBalance SET IncreaseScore=IncreaseScore+@IncreaseScore,DecreaseScore=DecreaseScore+@DecreaseScore 
	WHERE DateID=@DateID AND ClubID=@dwClubID AND UserID=@dwMemberID
	IF @@ROWCOUNT=0
	BEGIN
		-- 插入记录
		INSERT INTO StreamClubBalance(DateID,ClubID,UserID,IncreaseScore,DecreaseScore) 
		VALUES (@DateID,@dwClubID,@dwMemberID,@IncreaseScore,@DecreaseScore)
	END

	-- 查询游戏标识
	SELECT @PayUserGameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@PayUserID

	-- 变更记录
	INSERT INTO LHBattleRecordDB.dbo.RecordClubScoreChange(DateID,SrcUserID,SrcGameID,SrcClubScore,SrcUserType,DstUserID,DstGameID,DstClubScore,ClubID,TradeType,TradeClubScore)
	VALUES (@DateID,@PayUserID,@PayUserGameID,0,@PayUserType,@dwMemberID,@MemberGameID,@MemberScore,@dwClubID,@TradeType,-@lModifyScore)

	-- 校验类型
	--IF @OUT_OperaterMemberType=4 
	--BEGIN
	--	--充值模式		
	--	IF @ClubScoreMode=1
	--	BEGIN
	--		-- 查询积分
	--		UPDATE GameClubScoreInfo SET ClubScore=ClubScore-@lModifyScore WHERE UserID=@PayUserID				
	--	END

	--	-- 更新积分
	--	SELECT @PayUserScore=ClubScore FROM GameClubScoreInfo(NOLOCK) WHERE UserID=@PayUserID

	--	-- 馆分调整
	--	IF @PayUserScore IS NULL SET @PayUserScore=0
	--END	ELSE	
	--BEGIN
		-- 更新分数		
		--SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName,' SET ClubScore=ClubScore+',CONVERT(NVARCHAR(18), -@lModifyScore),' WHERE MemberID=',CONVERT(NVARCHAR(18), @PayUserID),' AND ',
		--					  ' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

		---- 执行脚本
		--EXECUTE(@Sqlstr)	
		--IF @@ERROR<>0
		--BEGIN				
		--	-- 事务回滚
		--	ROLLBACK TRAN
		--	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		--	-- 提示信息
		--	SET @strErrorDescribe='抱歉，由于数据库异常，本次操作执行失败！'		
		--	RETURN 5				
		--END	
		
		---- 更新积分
		--SET @PayUserScore=@PayUserScore-@lModifyScore	
	--END
		
	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	
	-- 输出变量
	SELECT @dwMemberID AS MemberID, @PayUserID AS PayUserID, @dwClubID AS ClubID, @ClubName AS ClubName, @KindID AS KindID, @ServerID AS ServerID, @PayUserType AS PayUserType,
	@MemberScore AS MemberScore, @MemberNickName AS MemberNickName
END

RETURN 0

GO


------------------------------------------------------------------------------------------------------

---- 赠送积分
--CREATE PROC GSP_GR_PresentClubScore
--	-- 用户信息
--	@dwUserID INT,								-- 用户 I D
--	@dwClubID INT,								-- 茶馆标识
--	@dwMemberID INT,							-- 成员标识
--	@lPresentScore INT,							-- 赠送分数	
--	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
--WITH ENCRYPTION AS

---- 属性设置
--SET NOCOUNT ON

---- 自动回滚
--SET XACT_ABORT ON

---- 基本信息
--DECLARE @UserID INT

---- 执行逻辑
--BEGIN
--	-- 查询用户
--	DECLARE @GameID INT
--	DECLARE @Nullity BIT
--	DECLARE @ShutDown BIT	
--	DECLARE @SrcNickName NVARCHAR(32)
	
--	-- 查询用户
--	SELECT @UserID=a.UserID,@GameID=a.GameID,@SrcNickName=NickName,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
--	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

--	-- 用户不存在
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

--	-- 查询馆员
--	DECLARE @DstGameID INT
--	DECLARE @DstNickName NVARCHAR(32)
--	SELECT @DstGameID=GameID,@DstNickName=NickName FROM LHAccountsDB.dbo.AccountsInfo WHERE UserID=@dwMemberID

--	-- 查询用户
--	IF @DstGameID IS NULL
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1202,"Msg":"系统未查询到改成员的用户信息，请查证后再次尝试登录！"}'
--		RETURN 4
--	END
	
--	-- 查询群主
--	DECLARE @ClubName NVARCHAR(32)
--	SELECT @ClubName=ClubName FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

--	-- 结果判断
--	IF @ClubName IS NULL
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'
--		RETURN 5		
--	END

--	-- 积分定义	
--	DECLARE	@SrcMemberType TINYINT
--	DECLARE	@DstMemberType TINYINT
--	DECLARE	@SrcMemberScore INT	
--	DECLARE @DstMemberScore INT					

--	-- 变量定义
--	DECLARE	@TableID INT			
--	DECLARE @Sqlstr		NVARCHAR(512)								
--	DECLARE	@TableName	NVARCHAR(128)						

--	-- 分表标识				
--	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

--	-- 群组成员表
--	SET	@TableName='ClubMember_';
--	SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	
	
--	-- 查询分数		
--	SET	@Sqlstr = CONCAT( ' SELECT @ClubScore=ClubScore,@MemberType=MemberType FROM  ',@TableName,' WHERE MemberID=',CONVERT(NVARCHAR(18), @UserID),' AND ',
--						  ' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

--	-- 执行脚本
--	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@ClubScore INT OUT, @MemberType TINYINT OUT',@ClubScore=@SrcMemberScore OUTPUT,@MemberType=@SrcMemberType OUTPUT		
		
--	-- 调整分数
--	IF @SrcMemberScore IS NULL
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1206,"Msg":"系统未查询到您的积分信息，请联系客户人员了解详情！"}'
--		RETURN 6
--	END

--	-- 校验积分
--	IF @SrcMemberScore < @lPresentScore
--	BEGIN
--		-- 提示信息
--		SET @strErrorDescribe=N'{"Idx":1207,"Msg":"由于您的积分不足，请调整好积分额度后再次尝试！"}'
--		RETURN 7		
--	END	
	
--	-- 查询分数		
--	SET	@Sqlstr = CONCAT( ' SELECT @ClubScore=ClubScore,@MemberType=MemberType FROM  ',@TableName,' WHERE MemberID=',CONVERT(NVARCHAR(18), @dwMemberID),' AND ',
--						  ' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

--	-- 执行脚本
--	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@ClubScore INT OUT, @MemberType TINYINT OUT',@ClubScore=@SrcMemberScore OUTPUT,@MemberType=@DstMemberType OUTPUT	
		
--	-- 调整分数
--	IF @DstMemberScore IS NULL
--	BEGIN
--		SET @strErrorDescribe=N'{"Idx":1208,"Msg":"系统未查询到该成员的积分信息，请联系客户人员了解详情！"}'
--		RETURN 8
--	END

--	-- 锁定信息
--	DECLARE @BattleNum INT
--	DECLARE @KindID SMALLINT
--	DECLARE @ServerID SMALLINT
--	DECLARE @ScoreKind TINYINT	
--	DECLARE @BattleStatus TINYINT
--	DECLARE @OnlineStatus TINYINT
--	SELECT @KindID=KindID,@ServerID=ServerID,@ScoreKind=ScoreKind,@BattleNum=BattleNum,@BattleStatus=BattleStatus,@OnlineStatus=OnlineStatus FROM BattleUserLocker(NOLOCK) WHERE UseriD=@dwMemberID
	
--	-- 调整变量
--	IF @BattleStatus IS NULL
--	BEGIN
--		SELECT @KindID=0,@ServerID=0,@ScoreKind=0,@BattleStatus=0,@OnlineStatus=0	
--	END ELSE
--	BEGIN
--		IF @OnlineStatus=0 OR @ScoreKind<>3
--		BEGIN
--			SELECT @KindID=0,@ServerID=0
--		END
--	END	
	
--	-- 开始事务
--	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
--	BEGIN TRAN

--	-- 更新分数		
--	SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName,' SET ClubScore=ClubScore-',CONVERT(NVARCHAR(18), @lPresentScore),' WHERE MemberID=',CONVERT(NVARCHAR(18), @dwUserID),' AND ',
--						  ' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

--	-- 执行脚本
--	EXECUTE(@Sqlstr)	
	
--	-- 更新分数		
--	SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName,' SET ClubScore=ClubScore+',CONVERT(NVARCHAR(18), @lPresentScore),' WHERE MemberID=',CONVERT(NVARCHAR(18), @dwMemberID),' AND ',
--						  ' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

--	-- 执行脚本
--	EXECUTE(@Sqlstr)	

--	-- 计算日期
--	DECLARE @DateID	INT	
--	SET @DateID = CAST(CAST(GETDATE() AS FLOAT) AS INT)

--	-- 积分记录
--	UPDATE StreamClubBalance SET TradeOut=TradeOut-@lPresentScore WHERE DateID=@DateID AND ClubID=@dwClubID AND UserID=@UserID
--	IF @@ROWCOUNT=0
--	BEGIN
--		-- 插入记录
--		INSERT INTO StreamClubBalance(DateID,ClubID,UserID,TradeOut) VALUES (@DateID,@dwClubID,@dwMemberID,-@lPresentScore)
--	END

--	-- 积分记录
--	UPDATE StreamClubBalance SET TradeIn=TradeIn+@lPresentScore WHERE DateID=@DateID AND ClubID=@dwClubID AND UserID=@dwMemberID
--	IF @@ROWCOUNT=0
--	BEGIN
--		-- 插入记录
--		INSERT INTO StreamClubBalance(DateID,ClubID,UserID,TradeIn) VALUES (@DateID,@dwClubID,@dwMemberID,@lPresentScore)
--	END

--	-- 变更记录
--	INSERT INTO LHBattleRecordDB.dbo.RecordClubScoreChange(DateID,SrcUserID,SrcGameID,SrcClubScore,SrcUserType,DstUserID,DstGameID,DstClubScore,ClubID,TradeType,TradeClubScore)
--	VALUES (@DateID,@UserID,@GameID,@SrcMemberScore,@SrcMemberType,@dwMemberID,@DstGameID,@DstMemberScore,@dwClubID,9,-@lPresentScore)	

--	-- 结束事务
--	COMMIT TRAN
--	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

--	-- 查询分数		
--	SET	@Sqlstr = CONCAT( ' SELECT @ClubScore=ClubScore FROM  ',@TableName,' WHERE MemberID=',CONVERT(NVARCHAR(18), @UserID),' AND ',
--						  ' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

--	-- 执行脚本
--	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@ClubScore INT OUT',@ClubScore=@SrcMemberScore OUTPUT

--	-- 查询分数		
--	SET	@Sqlstr = CONCAT( ' SELECT @ClubScore=ClubScore FROM  ',@TableName,' WHERE MemberID=',CONVERT(NVARCHAR(18), @dwMemberID),' AND ',
--						  ' ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

--	-- 执行脚本
--	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@ClubScore INT OUT',@ClubScore=@DstMemberScore OUTPUT
	
--	-- 输出变量
--	SELECT @ClubName AS ClubName, @KindID AS KindID, @ServerID AS ServerID, @SrcMemberScore AS SrcMemberScore, @SrcNickName AS SrcNickName, @DstMemberScore AS DstMemberScore,
--		   @DstNickName AS DstNickName
--END

--RETURN 0

--GO


----------------------------------------------------------------------------------------------------