/***************************************************************************************************
** ���ƣ�ע������ (��Ϸ)
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


-- ע������
CREATE PROC [dbo].[GSP_MB_RegisterPresent]
	@UserID INT,								-- �û�ID
	@RegisterMode SMALLINT,						-- ע������
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ	
	@szPresentGoodsList NVARCHAR(MAX) OUTPUT	-- �����б�

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ���ڱ�ʶ
	DECLARE @DateID	INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��������		
	DECLARE @PresentGold BIGINT
	DECLARE @PresentCard BIGINT
	DECLARE @PresentLimit SMALLINT	
	SELECT @PresentGold=0,@PresentCard=0

	-- ��������
	SELECT @PresentLimit=CONVERT(INT,StatusValue) FROM LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'RegisterOption' AND StatusName=N'PresentLimit'
		
	-- ��������
	IF @PresentLimit IS NULL SET @PresentLimit=0
	
	--0x01���ο��˺�ע�᲻����  0x02�������˺�ע�᲻���� 0x04����ͨ�˺�ע�᲻���� 0x08���������˺�ע�᲻���� 0x10���ֻ��˺�ע�᲻����
	-- �ο�
	IF @PresentLimit&0x01<>0 AND @RegisterMode=1 RETURN 0
	-- �����˺�
	IF @PresentLimit&0x02<>0 AND @RegisterMode=2 RETURN 0
	-- ��ͨ�˺�
	IF @PresentLimit&0x04<>0 AND @RegisterMode=2 RETURN 0
	-- ������
	IF @PresentLimit&0x08<>0 AND @RegisterMode=5 RETURN 0
	-- �ֻ�
	IF @PresentLimit&0x10<>0 AND @RegisterMode=4 RETURN 0

	-- ����IP��ַ
	IF @PresentLimit&0x40<>0
	BEGIN	
		-- ��������
		DECLARE @PresentCount INT
		-- ��ѯ��ַ
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
	
	-- ���ƻ�����
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

	-- �����б�
	DECLARE @TableGoodsItem TABLE (ItemIndex int identity(0,1), GoodsID INT, GoodsCount INT,GoodsLevel INT, GoodsIndate INT)	

	-- ��ȡ����
	INSERT INTO @TableGoodsItem(GoodsID,GoodsCount,GoodsLevel,GoodsIndate)
	SELECT GoodsID,GoodsCount,GoodsLevel,GoodsIndate FROM LHPlatformDB..RegisterPresentConfig(NOLOCK)

	--����
	DECLARE @GoodsID INT
	DECLARE @GoodsCount INT
	DECLARE @GoodsLevel INT
	DECLARE @GoodsIndate INT
	
	-- ����ʱ���
	DECLARE @dwTimeStamp INT = dbo.LHF_GetTimeStamp();
	--
	DECLARE @ItemIndex INT
	DECLARE @MaxItemIndex INT
	SELECT @ItemIndex = 0,@MaxItemIndex=MAX(ItemIndex) FROM @TableGoodsItem
	WHILE @ItemIndex <= @MaxItemIndex 
	BEGIN
		
		SELECT @GoodsID=GoodsID,@GoodsCount=GoodsCount,@GoodsLevel=GoodsLevel,@GoodsIndate=GoodsIndate FROM @TableGoodsItem WHERE ItemIndex=@ItemIndex

		-- д��Ƹ�
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

	-- ���¼�¼
	UPDATE RecordPresentClientIP SET PresentGold+=@PresentGold,PresentCard+=@PresentCard,PresentCount+=1 WHERE ClientIP=@strClientIP
	UPDATE RecordPresentMachine SET PresentGold+=@PresentGold,PresentCard+=@PresentCard,PresentCount+=1 WHERE MachineID=@strMachineID

	-- �����б�
	SET @szPresentGoodsList=(SELECT GoodsID,GoodsLevel AS Level,GoodsCount AS [Count],GoodsIndate AS Indate FROM @TableGoodsItem FOR JSON AUTO)

END

RETURN 0

GO