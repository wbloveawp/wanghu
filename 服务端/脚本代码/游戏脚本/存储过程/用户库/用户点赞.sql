
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

-- �޸�����
CREATE PROC GSP_MB_LikePlayer
	@dwUserID INT,								-- �û� I D
	@dwDstUserID INT,							-- Ŀ���û�	
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	IF NOT EXISTS (SELECT 1 FROM AccountsInfo(NOLOCK) WHERE UserID=@dwDstUserID)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1291,"Msg":"�����޵��û���Ϣ�����ڣ�"}'
		RETURN 1
	END

	-- ���ڱ�ʶ
	DECLARE @DateID	INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ���޼�¼
	IF EXISTS (SELECT 1 FROM RecordLikePlayer(NOLOCK) WHERE UserID=@dwDstUserID AND LikerID=@dwUserID AND DateID=@DateID)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1278,"Msg":"�������Ѿ��޹�����ң�"}'
		RETURN 2
	END

	-- �޸�����
	UPDATE AccountsInfo SET LikedNumber+=1 WHERE UserID=@dwDstUserID

	-- �����¼
	INSERT INTO RecordLikePlayer(UserID,LikerID,DateID,CollectDate) VALUES(@dwDstUserID,@dwUserID,@DateID,GetDate())

	-- �ɹ���ʾ
	SET @strErrorDescribe=N'{"Idx":1279,"Msg":"���޳ɹ���"}'

	-- �׳�����
	SELECT LikedNumber FROM AccountsInfo(NOLOCK) WHERE UserID=@dwDstUserID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
