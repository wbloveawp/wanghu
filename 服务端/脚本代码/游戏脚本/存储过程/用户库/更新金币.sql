

----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_BankUpdate]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_BankUpdate]
GO

----------------------------------------------------------------------------------------------------

-- 取出金币
CREATE PROC [dbo].[GSP_GR_BankUpdate]
	@dwUserID INT,									--用户ID
	@dwStationID  INT,								--站点ID
	@szPassword NCHAR(32),							--登录密码
	@strErrorDescribe	NVARCHAR(127) OUTPUT		--输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON
DECLARE @CurrencyID INT
DECLARE @Gold BIGINT
DECLARE @InsureGold BIGINT
DECLARE @LogonPass NVARCHAR(32)

BEGIN
	SELECT @LogonPass=LogonPass FROM AccountsInfo WHERE @dwUserID=UserID
	IF @LogonPass IS NULL
	BEGIN
		SELECT @strErrorDescribe='{"Idx":1087,"Msg":"对不起，您还没有成功登录大厅，请重新登录!"}'
		RETURN 1
	END
	IF @LogonPass <> @szPassword
	BEGIN
		SELECT @strErrorDescribe='{"Idx":1088,"Msg":"对不起，您的密码不正确，请重新登录大厅!"}'
		RETURN 1
	END

	SELECT @Gold=Score,@InsureGold=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID
	
	IF @Gold IS NULL OR @InsureGold IS NULL
	BEGIN
		SELECT @strErrorDescribe='{"Idx":1090,"Msg":"对不起，您的数据有误，请重新登录大厅或者重新安装客户端。"}'
		RETURN 3
	END
	
	SELECT @Gold AS Gold, @InsureGold AS InsureGold
END
RETURN 0
GO

