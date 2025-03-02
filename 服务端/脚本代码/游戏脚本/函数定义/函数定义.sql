
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LHF_GetTimestamp]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[LHF_GetTimestamp]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LHF_ConvertTimestamp]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.LHF_ConvertTimestamp
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LHF_ConvertTimestampToLocalTime]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[LHF_ConvertTimestampToLocalTime]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LHF_IsMoneyGoods]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[LHF_IsMoneyGoods]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LHF_GetWeekFirstDay]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[LHF_GetWeekFirstDay]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LHF_GetMonthFirstDay]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[LHF_GetMonthFirstDay]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LHF_IsAccountExcept]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[LHF_IsAccountExcept]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LHF_Script_FriendTable]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[LHF_Script_FriendTable]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LHF_GetGroupShardIndex]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[LHF_GetGroupShardIndex]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LHF_GetBlackShardIndex]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[LHF_GetBlackShardIndex]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LHF_GetFriendShardIndex]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[LHF_GetFriendShardIndex]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LHP_AdjustmentAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[LHP_AdjustmentAccounts]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LHF_GetUserUIShieldMask]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[LHF_GetUserUIShieldMask]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LHF_GetUserShieldKindList]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[LHF_GetUserShieldKindList]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 获取时间戳
CREATE FUNCTION [dbo].[LHF_GetTimestamp]
(		
)
RETURNS INT
AS
BEGIN

	RETURN DATEDIFF(S,'1970-01-01 00:00:00', GETUTCDATE())

END

GO

----------------------------------------------------------------------------------------------------

-- 转换时间戳
CREATE FUNCTION [dbo].[LHF_ConvertTimestamp]
(		
	@DateTime DATETIME
)
RETURNS INT
AS
BEGIN
	--DECLARE @HourDiff SMALLINT = DATEDIFF(hh,GETUTCDATE(),GETDATE())

	RETURN DATEDIFF(S,'1970-01-01 00:00:00', DATEADD(hh,3,@DateTime))

END

GO

----------------------------------------------------------------------------------------------------

-- 转换时间戳为本地时间
CREATE FUNCTION [dbo].[LHF_ConvertTimestampToLocalTime]
(		
	@TimeStamp INT
)
RETURNS DATETIME
AS
BEGIN
	DECLARE @TimeOffset INT
	SET @TimeOffset = DATEDIFF(ss,SYSUTCDATETIME(),GETDATE())

	RETURN DATEADD(s,@TimeStamp+@TimeOffset,'1970-1-1 00:00:00')

END

GO

----------------------------------------------------------------------------------------------------

-- 是否货币
CREATE FUNCTION [dbo].[LHF_IsMoneyGoods]
(
	@dwGoodsID INT
)
RETURNS NVARCHAR(MAX)
AS
BEGIN
	-- 校验物品
	IF @dwGoodsID IN (1,2,3,4)
	BEGIN
		RETURN 1
	END

	RETURN 0
END

GO

----------------------------------------------------------------------------------------------------

-- 当周第一天
CREATE FUNCTION [dbo].[LHF_GetWeekFirstDay]
(		
	@DateTime DATETIME
)
RETURNS DATETIME
AS
BEGIN
	
	-- 变量定义
	DECLARE @DayOfWeek INT 
	DECLARE @FirstDateTime DATETIME
	
	-- 设置变量
	SET @DayOfWeek = (DATEPART(dw,@DateTime)-2+7)%7;

	-- 推算时间
	SET @FirstDateTime = DATEADD(day,-@DayOfWeek,CONVERT(DATE,@DateTime))
	
	RETURN @FirstDateTime

END

GO

----------------------------------------------------------------------------------------------------

-- 当月第一天
CREATE FUNCTION [dbo].[LHF_GetMonthFirstDay]
(	
	@DateTime DATETIME
)
RETURNS DATETIME
AS
BEGIN

	-- 变量定义
	DECLARE @DayOfMonth INT 
	DECLARE @FirstDateTime DATETIME
	
	-- 设置变量
	SET @DayOfMonth = DATEPART(dd,@DateTime)-1;

	-- 推算时间
	SET @FirstDateTime = DATEADD(day,-@DayOfMonth,CONVERT(DATE,@DateTime))
	
	RETURN @FirstDateTime

END

GO

----------------------------------------------------------------------------------------------------
-- 维护中是否允许登录
CREATE FUNCTION [dbo].[LHF_IsAccountExcept]
(
	@IPAddress NVARCHAR(15),
	@MachineID NVARCHAR(32)
)
RETURNS TINYINT
AS
BEGIN
	
	-- 查询IP
	IF EXISTS(SELECT 1 FROM LHPlatformDB.dbo.ExceptAddress WHERE IPAddress=@IPAddress AND Nullity=0)
	BEGIN
		RETURN 0
	END

	-- 查询机器
	IF EXISTS(SELECT 1 FROM LHPlatformDB.dbo.ExceptMachine WHERE MachineID=@MachineID AND Nullity=0)
	BEGIN
		RETURN 0
	END
	
	RETURN 1
END

GO

----------------------------------------------------------------------------------------------------

-- 好友表脚本
CREATE FUNCTION [dbo].[LHF_Script_FriendTable]
(
	@TableName NVARCHAR(128)
)
RETURNS NVARCHAR(MAX)
AS
BEGIN
	
	-- 变量定义
	DECLARE @SqlScript NVARCHAR(MAX)

	-- 生成脚本
	SET	@SqlScript = CONCAT(' USE LHAccountsDB ',
							'  CREATE TABLE  ', @TableName,
							' ( UserID	 	INT   NOT NULL, '  ,
							'	FriendID 	INT   NOT NULL , ',
							'	Remarks 	NVARCHAR(32) DEFAULT N'''' NULL , ',
							'	CollectTime DATETIME DEFAULT GETDATE() NOT NULL',
							'	PRIMARY KEY(UserID, FriendID)',
							'  )');
	
	RETURN @SqlScript
END

GO


----------------------------------------------------------------------------------------------------

-- 群组表索引
CREATE FUNCTION [dbo].[LHF_GetGroupShardIndex]
(
	@dwUserID INT
)
RETURNS NVARCHAR(MAX)
AS
BEGIN
	RETURN 	@dwUserID/100000;
END

GO

----------------------------------------------------------------------------------------------------

-- 黑名单表索引
CREATE FUNCTION [dbo].[LHF_GetBlackShardIndex]
(
	@dwUserID INT
)
RETURNS NVARCHAR(MAX)
AS
BEGIN
	RETURN 	@dwUserID/500000;
END

GO

----------------------------------------------------------------------------------------------------

-- 好友表索引
CREATE FUNCTION [dbo].[LHF_GetFriendShardIndex]
(
	@dwUserID INT
)
RETURNS NVARCHAR(MAX)
AS
BEGIN
	RETURN 	@dwUserID/100000;
END

GO


----------------------------------------------------------------------------------------------------

-- 调整账号
CREATE PROC [dbo].[LHP_AdjustmentAccounts]
	@strAccounts NVARCHAR(32) OUTPUT,		-- 用户账号
	@strNickName NVARCHAR(32) OUTPUT		-- 用户昵称
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

BEGIN
	
	-- 变量定义
	DECLARE @nLen INT
	DECLARE @nIndex INT				
	DECLARE @nMaxLen INT
	DECLARE @nSubLoop INT			
	DECLARE @nMainLoop INT			 	
	DECLARE @Suffix NVARCHAR(10)	
	DECLARE @strSpecial NVARCHAR(64)				 							 	 
	DECLARE @strCharacter NVARCHAR(100)  	
		
	-- 设置变量
	SET @strSpecial=N',2B,SB,MB,NND,'	 		
	SET @strCharacter=N'0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'		

	-- 查询账号
	IF LEN(@strAccounts)<6 OR EXISTS (SELECT 1 FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts)
	BEGIN		 
		-- 设置变量					
		SELECT @nMainLoop=0,@nLen=2,@nMaxLen=31-LEN(@strAccounts)	
		
		-- 调整长度
		IF LEN(@strAccounts)<6 SET @nLen=6-LEN(@strAccounts)	
			 
		WHILE @nLen<@nMaxLen
		BEGIN
			
			-- 初始化
			SELECT @nSubLoop=0, @Suffix=N''				
				
			-- 生成后缀
			WHILE @nSubLoop<@nLen
			BEGIN
				SET @nSubLoop+=1
				SET @nIndex=CONVERT(INT,RAND()*(LEN(@strCharacter)-1)+1)
				SET @Suffix=@Suffix+SUBSTRING(@strCharacter,@nIndex,1)
			END				
				
			-- 过滤后缀
			IF CHARINDEX(N','+@Suffix+N',',@strSpecial)=0
			BEGIN				
				-- 连接后缀					
				DECLARE @TempAccounts VARCHAR(31)
				SET @TempAccounts=@strAccounts+@Suffix
				
				-- 存在判断
				IF NOT EXISTS (SELECT 1 FROM AccountsInfo(NOLOCK) WHERE Accounts=@TempAccounts)
				BEGIN
					SET @strAccounts=@TempAccounts	  
					BREAK				 
				END
			END
				
			-- 调整变量
			SET @nMainLoop+=1
			IF @nMainLoop>=100
			BEGIN
				SET @nLen+=1  	   
				SET @nMainLoop=0				
			END	
		END	
			 
		-- 随机生成账号
		IF @nLen>@nMaxLen		 			 
		BEGIN							
			SET @strAccounts=CONVERT(NVARCHAR(31),REPLACE(NEWID(),'-',''))			   
		END				 			 
	END

	-- 查询昵称
	IF LEN(@strNickName)<6 OR  EXISTS (SELECT 1 FROM AccountsInfo(NOLOCK) WHERE NickName=@strNickName)
	BEGIN
		
		-- 设置变量					
		SELECT @nMainLoop=0,@nLen=2,@nMaxLen=31-LEN(@strNickName)	
		
		-- 调整长度
		IF LEN(@strNickName)<6 SET @nLen=6-LEN(@strNickName)				
			 
		WHILE @nLen<@nMaxLen
		BEGIN

			-- 初始化
			SELECT @nSubLoop=0, @Suffix=N''		
				
			-- 生成后缀
			WHILE @nSubLoop<@nLen
			BEGIN
				SET @nSubLoop+=1
				SET @nIndex=CONVERT(INT,(RAND()*(LEN(@strCharacter)-1))+1) 
				SET @Suffix=@Suffix+SUBSTRING(@strCharacter, @nIndex, 1)
			END				
				
			-- 过滤后缀
			IF CHARINDEX(N','+@Suffix+N',',@strSpecial)=0
			BEGIN				
				-- 连接后缀
				DECLARE @TempNickName VARCHAR(31)
				SET @TempNickName=@strNickName+@Suffix
				
				-- 存在判断
				IF NOT EXISTS (SELECT 1 FROM AccountsInfo(NOLOCK) WHERE NickName=@TempNickName)
				BEGIN
					SET @strNickName=@TempNickName	  
					BREAK				 
				END
			END
				
			-- 调整变量
			SET @nMainLoop=@nMainLoop+1
			IF @nMainLoop>=100
			BEGIN
				SET @nLen+=1  	   
				SET @nMainLoop=0				
			END	
		END	
			 
		-- 随机生成昵称
		IF @nLen>@nMaxLen		 			 
		BEGIN							
			SET @strNickName=CONVERT(NVARCHAR(31),REPLACE(NEWID(),'-',''))			   
		END				 			 
	END

	RETURN 0
END

GO

----------------------------------------------------------------------------------------------------
-- 获取用户界面屏蔽掩码
CREATE FUNCTION [dbo].[LHF_GetUserUIShieldMask]
(
	@UserID INT,
	@strIPAddress NVARCHAR(32),
	@strMachineID NVARCHAR(32)
)
RETURNS TINYINT
AS
BEGIN

	-- 屏蔽掩码
	DECLARE @UIShieldMask INT
	SELECT @UIShieldMask=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SystemOption' AND StatusName=N'DefaultUIShieldMask'

	-- 调整参数
	IF @UIShieldMask IS NULL SET @UIShieldMask=0	

	-- 屏蔽掩码
	DECLARE @ShieldMask INT

	-- 特殊账号
	SELECT @ShieldMask=ShieldMask FROM LHPlatformDB.dbo.UIShieldAccount WHERE UserID=@UserID AND Nullity=0
	IF @ShieldMask IS NOT NULL
	BEGIN
		SET @UIShieldMask=@ShieldMask

		RETURN @UIShieldMask
	END	

	-- 构造Json
	--SET @strIPAddress='['+REPLACE(@strIPAddress,'.',',')+']'	
	--IF ISJSON(@strIPAddress)=0
	--BEGIN
	--	RETURN @UIShieldMask
	--END

	---- 转换IP	
	--DECLARE @IPAddress BIGINT	
	--SET @IPAddress=(CONVERT(BIGINT,JSON_VALUE(@strIPAddress,'$[3]'))*power(2, 24))|(CONVERT(BIGINT,JSON_VALUE(@strIPAddress,'$[2]'))*power(2, 16))|(CONVERT(BIGINT,JSON_VALUE(@strIPAddress,'$[1]'))*power(2, 8))|CONVERT(BIGINT,JSON_VALUE(@strIPAddress,'$[0]'))

	------------------------------------------------ 代收代付控制 --------------------------------------------------

	---- 特殊地址
	--SELECT @ShieldMask=ShieldMask FROM LHPlatformDB.dbo.UIShieldAddress WHERE @IPAddress>=Start_IPAddress AND @IPAddress<=Finish_IPAddress AND Nullity=0
	--IF @ShieldMask IS NOT NULL
	--BEGIN
	--	SET @UIShieldMask|=@ShieldMask
	--	--RETURN @UIShieldMask
	--END

	---- 特殊机器
	--SELECT @ShieldMask=ShieldMask FROM LHPlatformDB.dbo.UIShieldMachine WHERE MachineID=@strMachineID AND Nullity=0
	--IF @ShieldMask IS NOT NULL
	--BEGIN
	--	SET @UIShieldMask|=@ShieldMask
	--	--RETURN @UIShieldMask
	--END
	
	------------------------------------------------ 每日特惠控制 --------------------------------------------------
	
	---- 校验掩码
	--IF (@UIShieldMask&0x04)=0
	--BEGIN
	--	-- 查询剩余库存
	--	DECLARE @RemainInventory INT
	--	SELECT @RemainInventory=DisDailyRemainInventory FROM LHInsureDB.dbo.MallGoods(NOLOCK) WHERE MallgoodsID=10000
		
	--	-- 调整变量
	--	IF @RemainInventory IS NULL SET @RemainInventory=0

	--	-- 库存判断
	--	IF @RemainInventory=0
	--	BEGIN
	--		SET @UIShieldMask|=0x04
	--	END ELSE
	--	BEGIN		
	--		-- 查询特惠商品购买日期
	--		DECLARE @LastBuyDateTime DATETIME
	--		SELECT @LastBuyDateTime=LastBuyDateTime FROM LHInsureDB.dbo.MallGoodsBuyStat(NOLOCK) WHERE UserID=@UserID AND MallgoodsID=10000

	--		-- 最后购买是当天
	--		IF @LastBuyDateTime IS NOT NULL AND DATEDIFF(dd,@LastBuyDateTime,GETDATE())=0
	--		BEGIN
	--			SET @UIShieldMask|=0x04
	--		END
	--	END
	--END	
	
	RETURN @UIShieldMask
END

GO


----------------------------------------------------------------------------------------------------
-- 获取用户被屏蔽的类型列表
CREATE FUNCTION [dbo].[LHF_GetUserShieldKindList]
(
	@UserID INT
)
RETURNS NVARCHAR(256)
AS
BEGIN

	-- 变量定义
	DECLARE @ShieldKindList NVARCHAR(256)=N'['

	-- 查询信息
	DECLARE @MarketID INT
	SELECT @MarketID=MarketID FROM AccountsInfo(NOLOCK) WHERE UserID=@UserID
	
	---------------------CrashFootball(3006)---------------------- 
	-- 非 Classic Slot 谷歌包或裂变包用户
	--IF @MarketID NOT IN (100,101)
	--BEGIN
	--	IF LEN(@ShieldKindList)>1 SET @ShieldKindList+=N','
	--	SET @ShieldKindList+=N'3006'
	--END

	-----------------------CrashPlus(3004)---------------------- 
	---- 非 Classic Slot 谷歌包或裂变包用户
	--IF @MarketID NOT IN (100,101)
	--BEGIN
	--	IF LEN(@ShieldKindList)>1 SET @ShieldKindList+=N','
	--	SET @ShieldKindList+=N'3004'
	--END

	-----------------------Double(154)---------------------- 
	---- 非 Classic Slot 谷歌包或裂变包用户
	--IF @MarketID NOT IN (100,101)
	--BEGIN
	--	IF LEN(@ShieldKindList)>1 SET @ShieldKindList+=N','
	--	SET @ShieldKindList+=N'154'
	--END

	----------------------------------------------------

	-- 回括号
	SET @ShieldKindList+=N']'

	RETURN @ShieldKindList
END

GO

----------------------------------------------------------------------------------------------------