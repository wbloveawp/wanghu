/***************************************************************************************************
** 名称：注册赠送 (游戏)
***************************************************************************************************/

----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_RegisterPresent]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_RegisterPresent]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------


-- 注册赠送
CREATE PROC [dbo].[GSP_MB_RegisterPresent]
	@UserID INT,								-- 用户ID
	@RegisterMode SMALLINT,						-- 注册类型
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识	
	@szPresentGoodsList NVARCHAR(MAX) OUTPUT	-- 赠送列表

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 日期标识
	DECLARE @DateID	INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 变量定义		
	DECLARE @PresentGold BIGINT
	DECLARE @PresentCard BIGINT
	DECLARE @PresentLimit SMALLINT	
	SELECT @PresentGold=0,@PresentCard=0

	-- 赠送配置
	SELECT @PresentLimit=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'RegisterOption' AND StatusName=N'PresentLimit'
		
	-- 调整配置
	IF @PresentLimit IS NULL SET @PresentLimit=0
	
	--0x01、游客账号注册不赠送  0x02、邮箱账号注册不赠送 0x04、普通账号注册不赠送 0x08、第三方账号注册不赠送 0x10、手机账号注册不赠送
	-- 游客
	IF @PresentLimit&0x01<>0 AND @RegisterMode=1 RETURN 0
	-- 邮箱账号
	IF @PresentLimit&0x02<>0 AND @RegisterMode=2 RETURN 0
	-- 普通账号
	IF @PresentLimit&0x04<>0 AND @RegisterMode=2 RETURN 0
	-- 第三方
	IF @PresentLimit&0x08<>0 AND @RegisterMode=5 RETURN 0
	-- 手机
	IF @PresentLimit&0x10<>0 AND @RegisterMode=4 RETURN 0

	-- 限制IP地址
	IF @PresentLimit&0x40<>0
	BEGIN	
		-- 变量定义
		DECLARE @PresentCount INT
		-- 查询地址
		SELECT @PresentCount=PresentCount FROM RecordPresentClientIP WHERE ClientIP=@strClientIP
		IF @PresentCount IS NULL 
		BEGIN
			INSERT INTO RecordPresentClientIP(ClientIP,DateID,PresentGold,PresentCard,PresentCount,CollectDateTime)
			VALUES (@strClientIP,@DateID,@PresentGold,@PresentCard,0,GETDATE())
			SET @PresentCount= 0
		END 
		IF @PresentCount>=1 
		BEGIN
			RETURN 0
		END
	END
	
	-- 限制机器码
	IF @PresentLimit&0x80<>0
	BEGIN
		SELECT @PresentCount=PresentCount FROM RecordPresentMachine WHERE MachineID=@strMachineID
		IF @PresentCount IS NULL 
		BEGIN
			INSERT INTO RecordPresentMachine(MachineID,DateID,PresentGold,PresentCard,PresentCount,CollectDateTime)
			VALUES (@strMachineID,@DateID,@PresentGold,@PresentCard,0,GETDATE())
			SET @PresentCount = 0
		END
		IF @PresentCount >=1
		BEGIN
			RETURN 0
		END
	END		

	-- 奖励列表
	DECLARE @TableGoodsItem TABLE (ItemIndex int identity(0,1), GoodsID INT, GoodsCount INT,GoodsLevel INT, GoodsIndate INT)	

	-- 读取配置
	INSERT INTO @TableGoodsItem(GoodsID,GoodsCount,GoodsLevel,GoodsIndate)
	SELECT GoodsID,GoodsCount,GoodsLevel,GoodsIndate FROM LHPlatformDB..RegisterPresentConfig(NOLOCK)

	--赠送
	DECLARE @GoodsID INT
	DECLARE @GoodsCount INT
	DECLARE @GoodsLevel INT
	DECLARE @GoodsIndate INT
	
	-- 计算时间戳
	DECLARE @dwTimeStamp INT = dbo.LHF_GetTimeStamp();
	--
	DECLARE @ItemIndex INT
	DECLARE @MaxItemIndex INT
	SELECT @ItemIndex = 0,@MaxItemIndex=MAX(ItemIndex) FROM @TableGoodsItem
	WHILE @ItemIndex <= @MaxItemIndex 
	BEGIN
		
		SELECT @GoodsID=GoodsID,@GoodsCount=GoodsCount,@GoodsLevel=GoodsLevel,@GoodsIndate=GoodsIndate FROM @TableGoodsItem WHERE ItemIndex=@ItemIndex

		-- 写入财富
		IF dbo.LHF_IsMoneyGoods(@GoodsID)=1
		BEGIN		
			EXEC GSP_GP_WriteGameWealth @UserID,@GoodsID,1,@GoodsCount
		END ELSE
		BEGIN			
			EXEC GSP_GP_AppendGoodsItem @UserID,1,@GoodsID,@GoodsLevel,@GoodsCount,@GoodsIndate,@dwTimeStamp			
		END	
		--
		SET @ItemIndex=@ItemIndex+1
	END

	-- 更新记录
	UPDATE RecordPresentClientIP SET PresentGold+=@PresentGold,PresentCard+=@PresentCard,PresentCount+=1 WHERE ClientIP=@strClientIP
	UPDATE RecordPresentMachine SET PresentGold+=@PresentGold,PresentCard+=@PresentCard,PresentCount+=1 WHERE MachineID=@strMachineID

	-- 赠送列表
	SET @szPresentGoodsList=(SELECT GoodsID,GoodsLevel AS Level,GoodsCount AS [Count],GoodsIndate AS Indate FROM @TableGoodsItem FOR JSON AUTO)

END

RETURN 0

GO