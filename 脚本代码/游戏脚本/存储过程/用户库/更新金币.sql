

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

-- ȡ�����
CREATE PROC [dbo].[GSP_GR_BankUpdate]
	@dwUserID INT,									--�û�ID
	@dwStationID  INT,								--վ��ID
	@szPassword NCHAR(32),							--��¼����
	@strErrorDescribe	NVARCHAR(127) OUTPUT		--�����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON
DECLARE @CurrencyID INT
DECLARE @Gold BIGINT
DECLARE @InsureGold BIGINT
DECLARE @LogonPass NVARCHAR(32)

BEGIN
	SELECT @LogonPass=LogonPass FROM AccountsInfo WHERE @dwUserID=UserID
	IF @LogonPass IS NULL
	BEGIN
		SELECT @strErrorDescribe='{"Idx":1087,"Msg":"�Բ�������û�гɹ���¼�����������µ�¼!"}'
		RETURN 1
	END
	IF @LogonPass <> @szPassword
	BEGIN
		SELECT @strErrorDescribe='{"Idx":1088,"Msg":"�Բ����������벻��ȷ�������µ�¼����!"}'
		RETURN 1
	END

	SELECT @Gold=Score,@InsureGold=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID
	
	IF @Gold IS NULL OR @InsureGold IS NULL
	BEGIN
		SELECT @strErrorDescribe='{"Idx":1090,"Msg":"�Բ��������������������µ�¼�����������°�װ�ͻ��ˡ�"}'
		RETURN 3
	END
	
	SELECT @Gold AS Gold, @InsureGold AS InsureGold
END
RETURN 0
GO

