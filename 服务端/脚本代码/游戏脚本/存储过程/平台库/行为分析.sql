----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_PlayerBehaviorAnalysis]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_PlayerBehaviorAnalysis]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �����Ϊ����
CREATE PROC GSP_GP_PlayerBehaviorAnalysis	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �Զ��ع�
SET XACT_ABORT ON

-- ִ���߼�
BEGIN

	
	-- �û��б�
	DECLARE @TableUserList TABLE( UserID INT NOT NULL)

	-- ��ǰʱ��
	DECLARE @CurrentTime DATETIME
	SET @CurrentTime = DATEADD(dd,-1,GETDATE())

	-- ɸѡ���ʽ����ڵ���100�����
	INSERT INTO @TableUserList SELECT UserID FROM LHInsureDB.dbo.PayRecord(NOLOCK) WHERE DATEDIFF(dd,PayDateTime,@CurrentTime)=0 AND PayAmount>=100.00

	-- ɸѡ���ճ�ֵ�ܽ����ڵ���200�����
	INSERT INTO @TableUserList SELECT UserID FROM LHInsureDB.dbo.PayRecord(NOLOCK) WHERE DATEDIFF(dd,PayDateTime,@CurrentTime)=0 GROUP BY UserID HAVING SUM(PayAmount)>=200.00

	-- ɸѡ������ע����10�����ۼ�15�����ϵ����
	INSERT INTO @TableUserList SELECT UserID FROM LHGameGoldDB.dbo.RecordDrawScore(NOLOCK) WHERE DATEDIFF(dd,RecordDate,@CurrentTime)=0 AND BetAmount>=10000 GROUP BY UserID HAVING COUNT(RecordID)>=15
	
	-- ���ϴ�R��ǩ
	UPDATE AccountsInfo SET LabelMask|=0x0001 WHERE UserID IN (SELECT UserID FROM @TableUserList GROUP BY UserID)

	--SELECT UserID FROM @TableUserList GROUP BY UserID	

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

