USE LHGameRecordDB
GO

/************************赠送物品************************/
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WSP_GrantGoods]')) 
DROP PROCEDURE [dbo].[WSP_GrantGoods]

GO

CREATE PROC WSP_GrantGoods
	@dwUserID INT,							-- 用户标识			
	@dwGoodsID INT,							-- 物品标识
	@wGoodsLevel SMALLINT,					-- 物品等级
	@dwIncCount BIGINT,						-- 增加数量
	@dwIncIndate INT,						-- 增加有效期		
	@dwTimestamp INT,						-- 时间戳	
	@strClientIP NVARCHAR(15),				-- IP地址
	@dwMasterID INT,						-- 管理账号
	@strNote NVARCHAR(200),					-- 备注
	@Message NVARCHAR(127) OUT				-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 判断在线
	IF EXISTS(SELECT * FROM LHAccountsDB.dbo.AccountsOnlineInfo WHERE UserID=@dwUserID AND ServerID<>0) 
	BEGIN
		SET @Message='用户处于在线状态，无法赠送，请先让玩家下线或者踢玩家下线'
		RETURN -1
	END

	-- 查询用户
	DECLARE @GameID INT
	SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 校验用户
	IF @GameID IS NULL
	BEGIN
		SET @Message='用户不存在'
		RETURN -2
	END

	DECLARE @ChangeType INT
	SET @ChangeType=100

	-- 赠送钻石
	IF @dwGoodsID=1
	BEGIN
		-- 查询钻石
		DECLARE @BeforeIngot BIGINT
		SELECT @BeforeIngot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WITH(UPDLOCK)  WHERE UserID=@dwUserID

		-- 调整变量
		IF @BeforeIngot IS NULL 
		BEGIN
			SET @BeforeIngot=0
			INSERT INTO LHAccountsDB.dbo.AccountsIngotInfo(UserID,Score) VALUES (@dwUserID,0)
		END

		-- 负分校验
		IF @BeforeIngot+@dwIncCount<0
		BEGIN
			SET @Message='钻石不能为负'
			RETURN -3
		END

		-- 更新钻石
		UPDATE LHAccountsDB.dbo.AccountsIngotInfo SET Score+=@dwIncCount,GrantTimes=GrantTimes+1,GrantAmount=GrantAmount+@dwIncCount WHERE UserID=@dwUserID

		-- 变更记录
		INSERT INTO LHGameRecordDB.dbo.RecordChangeIngot(UserID,GameID,TypeID,BeforeIngot,ChangeIngot,CollectClientIP,CollectDateTime)
		VALUES(@dwUserID,@GameID,@ChangeType,@BeforeIngot,@dwIncCount,@strClientIP,GetDate())

		-- 赠送记录
		INSERT INTO LHGameRecordDB.dbo.RecordGrantIngot(UserID,CurIngot,AddIngot,MasterID,Reason,CollectClientIP)
		VALUES(@dwUserID,@BeforeIngot,@dwIncCount,@dwMasterID,@strNote,@strClientIP)
	END
	-- 赠送金币
	ELSE IF @dwGoodsID=2
	BEGIN
		-- 查询金币
		DECLARE @BeforeGold BIGINT		
		SELECT @BeforeGold=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WITH(UPDLOCK) WHERE UserID=@dwUserID
		
		-- 调整变量
		IF @BeforeGold IS NULL
		BEGIN
			-- 设置变量
			SET @BeforeGold=0			

			-- 插入数据
			INSERT INTO LHAccountsDB.dbo.AccountsGoldInfo(UserID,Score) VALUES (@dwUserID,0)
		END

		-- 负分校验
		IF @BeforeGold+@dwIncCount<0
		BEGIN
			SET @Message='金币不能为负'
			RETURN -3
		END
		
		-- 更新金币
		UPDATE LHAccountsDB.dbo.AccountsGoldInfo SET Score=Score+@dwIncCount,GrantTimes=GrantTimes+1,GrantAmount=GrantAmount+@dwIncCount WHERE UserID=@dwUserID

		-- 变更记录
		INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID,GameID,TypeID,BeforeGold,ChangeGold,CollectClientIP,CollectDateTime)
		VALUES(@dwUserID,@GameID,@ChangeType,@BeforeGold,@dwIncCount,@strClientIP,GetDate())

		-- 赠送记录
		INSERT INTO LHGameRecordDB.dbo.RecordGrantGold(UserID,CurGold,AddGold,MasterID,Reason,CollectClientIP)
		VALUES(@dwUserID,@BeforeGold,@dwIncCount,@dwMasterID,@strNote,@strClientIP)
	END
	-- 赠送爱心
	ELSE IF @dwGoodsID=4
	BEGIN
		-- 查询金币
		DECLARE @BeforeHeart BIGINT		
		SELECT @BeforeHeart=LoveHeart FROM LHGameRankingDB.dbo.GameScoreInfo WITH(UPDLOCK) WHERE UserID=@dwUserID
		
		-- 调整变量
		IF @BeforeHeart IS NULL
		BEGIN
			-- 设置变量
			SET @BeforeHeart=0			

			-- 插入数据
			INSERT INTO LHGameRankingDB.dbo.GameScoreInfo (UserID,LoveHeart) VALUES (@dwUserID,0)
		END

		-- 负分校验
		IF @BeforeHeart+@dwIncCount<0
		BEGIN
			SET @Message='爱心不能为负'
			RETURN -3
		END
		
		-- 更新爱心
		UPDATE LHGameRankingDB.dbo.GameScoreInfo SET LoveHeart=LoveHeart+@dwIncCount WHERE UserID=@dwUserID

		-- 变更记录
		INSERT INTO LHGameRecordDB.dbo.RecordChangeLoveHeart(UserID, GameID, TypeID, BeforeLoveHeart, ChangeLoveHeart, CollectClientIP, CollectDateTime)
		VALUES (@dwUserID,@GameID,@ChangeType,@BeforeHeart,@dwIncCount,@strClientIP,GetDate())

		-- 赠送记录
		INSERT INTO LHGameRecordDB.dbo.RecordGrantLoveHeart(UserID,CurLoveHeart,AddLoveHeart,MasterID,Reason,CollectClientIP)
		VALUES(@dwUserID,@BeforeHeart,@dwIncCount,@dwMasterID,@strNote,@strClientIP)
	END	
	ELSE 
	BEGIN
		-- 物品属性
		DECLARE @HasIndate TINYINT
		DECLARE @GoodsKind TINYINT
		DECLARE @GoodsOption NVARCHAR(2048)
		SELECT @HasIndate=HasIndate,@GoodsKind=GoodsKind,@GoodsOption=GoodsOption FROM LHPlatformDB.dbo.GameGoodsInfo WHERE GoodsID=@dwGoodsID
		-- 物品不存在
		IF @GoodsKind IS NULL 
		BEGIN
			RETURN @dwIncCount
		END

		-- 调整等级
		IF @GoodsKind=11 AND @wGoodsLevel=0
		BEGIN
			SET @wGoodsLevel=1
		END

		-- 物品信息
		DECLARE @GoodsCount INT	
		DECLARE @GoodsIndate INT		
		DECLARE @GoodsExpireTime INT	

		-- 物品信息	
		DECLARE @BeforeCount INT
		DECLARE @BeforeIndate INT
		DECLARE @BeforeExpireTime INT
		SELECT @BeforeCount=GoodsCount,@BeforeIndate=GoodsIndate,@BeforeExpireTime=ExpireTime FROM LHAccountsDB.dbo.AccountsPack WITH(UPDLOCK) WHERE UserID=@dwUserID AND GoodsID=@dwGoodsID

		-- 更新物品
		IF @BeforeCount IS NOT NULL
		BEGIN	
			-- 判断有效期
			IF @HasIndate=1
			BEGIN		
				-- 调整数量
				SET @GoodsCount=1
				SET @GoodsExpireTime=0

				-- 数量判断
				IF @BeforeCount=0
				BEGIN
					IF @dwIncIndate=0 SET @GoodsIndate=0
					IF @dwIncIndate>0 
					BEGIN
						SET @GoodsIndate=@dwIncIndate
						SET @GoodsExpireTime=@dwTimestamp+@dwIncIndate
					END
				END ELSE
				BEGIN
					IF @BeforeIndate=0 
					BEGIN
						SET @GoodsIndate=0
						SET @GoodsExpireTime=0					
					END ELSE
					BEGIN
						IF @dwIncIndate=0 SET @GoodsIndate=0
						IF @dwIncIndate>0 
						BEGIN
							SET @GoodsIndate=@BeforeIndate+@dwIncIndate
							SET @GoodsExpireTime=@BeforeExpireTime+@dwIncIndate
						END
					END
				END			
			END ELSE
			BEGIN
				SET @GoodsIndate=0
				SET @GoodsExpireTime=0
				SET @GoodsCount=@BeforeCount+@dwIncCount
			END	
		
			-- 更新物品
			UPDATE LHAccountsDB.dbo.AccountsPack SET GoodsCount=@GoodsCount,GoodsLevel=@wGoodsLevel,GoodsIndate=@GoodsIndate,ExpireTime=@GoodsExpireTime WHERE UserID=@dwUserID AND GoodsID=@dwGoodsID						
		END 
		ELSE
		BEGIN
			-- 判断有效期
			IF @HasIndate=1
			BEGIN		
				-- 调整数量
				SET @GoodsCount=1
				SET @GoodsExpireTime=0
			
				-- 调整有效期
				IF @dwIncIndate=0 SET @GoodsIndate=0
				IF @dwIncIndate>0 
				BEGIN
					SET @GoodsIndate=@dwIncIndate
					SET @GoodsExpireTime=@dwTimestamp+@dwIncIndate
				END			
			END ELSE
			BEGIN
				SET @GoodsIndate=0
				SET @GoodsExpireTime=0
				SET @GoodsCount=@dwIncCount
			END

			-- 设置数量
			SET @BeforeCount=0

			-- 添加物品
			INSERT INTO LHAccountsDB.dbo.AccountsPack(UserID, GoodsID, GoodsKind, GoodsLevel, GoodsCount, GoodsIndate, BindState, ExpireTime, CollectTime)
			VALUES (@dwUserID, @dwGoodsID, @GoodsKind, @wGoodsLevel, @GoodsCount, @GoodsIndate, 0, @GoodsExpireTime, GETDATE())	
		END

		-- 变更记录
		IF @GoodsCount-@BeforeCount<>0
		BEGIN
			INSERT INTO LHGameRecordDB.dbo.RecordChangeGoods(UserID, GoodsID, GoodsKind, TypeID, RelationID, BeforeCount, ChangeCount, CollectClientIP, CollectDateTime,CollectNote)
			VALUES (@dwUserID, @dwGoodsID, @GoodsKind, 100, 0, @BeforeCount, @GoodsCount-@BeforeCount, @strClientIP ,GETDATE(),@strNote)

			INSERT INTO LHGameRecordDB.dbo.RecordGrantGoods(UserID,GoodsID,CurGoodsCount,AddGoodsCount,MasterID,Reason,CollectClientIP)
			VALUES(@dwUserID,@dwGoodsID,@BeforeCount,@GoodsCount-@BeforeCount,@dwMasterID,@strNote,@strClientIP)
		END
	END

	SET @Message='赠送成功，成功数量：'+LTRIM(@GoodsCount)
END

RETURN 0
GO

