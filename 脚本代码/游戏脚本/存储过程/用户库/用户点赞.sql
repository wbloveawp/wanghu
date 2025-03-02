
----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_LikePlayer]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_LikePlayer]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 修改资料
CREATE PROC GSP_MB_LikePlayer
	@dwUserID INT,								-- 用户 I D
	@dwDstUserID INT,							-- 目标用户	
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	IF NOT EXISTS (SELECT 1 FROM AccountsInfo(NOLOCK) WHERE UserID=@dwDstUserID)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1291,"Msg":"您点赞的用户信息不存在！"}'
		RETURN 1
	END

	-- 日期标识
	DECLARE @DateID	INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 点赞记录
	IF EXISTS (SELECT 1 FROM RecordLikePlayer(NOLOCK) WHERE UserID=@dwDstUserID AND LikerID=@dwUserID AND DateID=@DateID)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1278,"Msg":"您今天已经赞过该玩家！"}'
		RETURN 2
	END

	-- 修改数据
	UPDATE AccountsInfo SET LikedNumber+=1 WHERE UserID=@dwDstUserID

	-- 插入记录
	INSERT INTO RecordLikePlayer(UserID,LikerID,DateID,CollectDate) VALUES(@dwDstUserID,@dwUserID,@DateID,GetDate())

	-- 成功提示
	SET @strErrorDescribe=N'{"Idx":1279,"Msg":"点赞成功！"}'

	-- 抛出数量
	SELECT LikedNumber FROM AccountsInfo(NOLOCK) WHERE UserID=@dwDstUserID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
