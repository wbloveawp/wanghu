

----------------------------------------------------------------------------------------------------

USE LHAccountsDB
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QuerySubsidy]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QuerySubsidy]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LargessSubsidy]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LargessSubsidy]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QuerySubsidy]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QuerySubsidy]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LargessSubsidy]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LargessSubsidy]
GO

----------------------------------------------------------------------------------------------------
-- ��ѯ�ͱ�
CREATE PROC [dbo].[GSP_GP_QuerySubsidy]
	@dwUserID int,							 -- �û�ID
	@szMachine NVARCHAR(32),				 -- ������ʶ
	@strErrorDescribe	NVARCHAR(127) OUTPUT -- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

BEGIN
	-- ��ѯ�û�
	IF NOT Exists(SELECT * FROM AccountsInfo WHERE UserID=@dwUserID)	
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1091,"Msg":"��Ǹ�������û���Ϣ�����ڣ�����ϵ�ͷ����ģ�"}'
		RETURN 1
	END

	-- �����ѯ
	IF Exists(SELECT 1 FROM AccountsOnlineInfo WHERE UserID=@dwUserID AND ServerID<>0)	
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1092,"Msg":"��Ǹ��ϵͳ��⵽�㵱ǰ������Ϸ�����У����˳�����������ȡ��"}'
		RETURN 2
	END
 
	-- ����ʱ��
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- �ͱ���Ϣ
	IF NOT Exists(SELECT 1 FROM AccountsSubsidyClaim WHERE DateID=@DateID AND UserID=@dwUserID)
	BEGIN
		-- �����¼
		INSERT INTO AccountsSubsidyClaim(DateID, UserID, MachineID) VALUES (@DateID, @dwUserID,  @szMachine)
	END ELSE
	BEGIN
		-- ���¼�¼
		UPDATE AccountsSubsidyClaim SET MachineID=@szMachine WHERE DateID=@DateID AND UserID=@dwUserID	
	END	

	-- �ͱ�����
	DECLARE @ClaimGold BIGINT
	DECLARE @ClaimCondition BIGINT
	DECLARE	@ClaimTimesPerAccounts TINYINT
	
	-- ��ѯ����
	SELECT @ClaimGold=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND StatusName='SubsidyGold'
	SELECT @ClaimCondition=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND StatusName='SubsidyCondition'
	SELECT @ClaimTimesPerAccounts=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND StatusName='SubsidyTimesPerAccounts'

	-- ��������
	IF @ClaimGold IS NULL SET @ClaimGold=0
	IF @ClaimCondition IS NULL SET @ClaimCondition=0
	IF @ClaimTimesPerAccounts IS NULL SET @ClaimTimesPerAccounts=0
	
	-- ��ȡ��¼
	DECLARE @ClaimTimes TINYINT
	SELECT @ClaimTimes=ClaimTimes FROM AccountsSubsidyClaim WHERE DateID=@DateID AND UserID=@dwUserID

	-- �ж���ȡ����	
	DECLARE @CurrScore BIGINT
	DECLARE @CurrInsure BIGINT
	SELECT @CurrScore=Score, @CurrInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	
	-- �������
	IF @CurrScore IS NULL SET @CurrScore=0
	IF @CurrInsure IS NULL SET @CurrInsure=0

	-- �׳���¼
	SELECT @ClaimTimes AS ClaimTimes, @ClaimGold AS ClaimGold, @ClaimCondition AS ClaimCondition, @ClaimTimesPerAccounts AS ClaimTimesPerAccounts,
		   @CurrScore AS CurrScore, @CurrInsure AS CurrInsureScore	 			
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
-- ���͵ͱ�
CREATE PROC [dbo].[GSP_GP_LargessSubsidy]
	@dwUserID INT,							 -- �û�ID
	@szPassword NVARCHAR(33),				 -- �˺�����
	@szClientIP	NVARCHAR(15),				 -- ���ӵ�ַ
	@szMachine NVARCHAR(32),				 -- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT	 -- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON
BEGIN	
	-- ��ѯ�û�
	DECLARe @Nullity TINYINT
	DECLARE @LogonPass NVARCHAR(33)
	DECLARE @LogonMachine NVARCHAR(32)
	SELECT @LogonPass=LogonPass,@LogonMachine=LogonMachineID,@Nullity=Nullity FROM AccountsInfo WHERE UserID=@dwUserID 
	
	-- �����ж�
	IF @LogonPass IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
		RETURN 1
	END

	-- �˺ű�����
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����"}'
		RETURN 2		
	END

	-- ����У��
	IF @LogonPass<>@szPassword
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�"}'
		RETURN 3		
	END

	-- �����ѯ
	IF Exists(SELECT 1 FROM AccountsOnlineInfo WHERE UserID=@dwUserID AND ServerID<>0)	
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1092,"Msg":"��Ǹ��ϵͳ��⵽�㵱ǰ������Ϸ�����У����˳�����������ȡ��"}'
		RETURN 4
	END

	-- �ͱ�����
	DECLARE @ClaimGold BIGINT
	DECLARE @ClaimCondition BIGINT
	DECLARE	@ClaimTimesPerMachine TINYINT
	DECLARE	@ClaimTimesPerAccounts TINYINT
	
	-- ��ѯ����
	SELECT @ClaimGold=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND StatusName='SubsidyGold'
	SELECT @ClaimCondition=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND StatusName='SubsidyCondition'
	SELECT @ClaimTimesPerMachine=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND  StatusName='SubsidyTimesPerMachine'
	SELECT @ClaimTimesPerAccounts=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND  StatusName='SubsidyTimesPerAccounts'	

	-- ��������
	IF @ClaimGold IS NULL SET @ClaimGold=400000
	IF @ClaimCondition IS NULL SET @ClaimCondition=400000
	IF @ClaimTimesPerMachine IS NULL SET @ClaimTimesPerMachine=3	
	IF @ClaimTimesPerAccounts IS NULL SET @ClaimTimesPerAccounts=3

	-- �ж���ȡ����	
	DECLARE @CurrScore BIGINT
	DECLARE @CurrInsure BIGINT
	SELECT @CurrScore=Score, @CurrInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	
	-- �������
	IF @CurrScore IS NULL SET @CurrScore=0
	IF @CurrInsure IS NULL SET @CurrInsure=0

	-- �жϽ��
	IF (@CurrScore+@CurrInsure)>=@ClaimCondition OR @CurrScore<0 OR @CurrInsure<0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1093,"Msg":"��Ǹ������ǰ���ϵ���Ϸ�Ҳ���������������"}'
		RETURN 5		
	END		

	-- ����ʱ��
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��ѯ�ͱ�
	DECLARE @ClaimTimes TINYINT
	DECLARE @MachineID NVARCHAR(32)
	SELECT @ClaimTimes=ClaimTimes,@MachineID=MachineID FROM AccountsSubsidyClaim WHERE DateID=@DateID AND UserID=@dwUserID

	-- �����ж�
	IF @ClaimTimes IS NULL OR @MachineID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1094,"Msg":"��Ǹ�����ĵͷ����ͼ�¼�����쳣������ϵ�ͷ����ģ�"}'
		RETURN 6					
	END

	-- У�����
	IF @szMachine<>@LogonMachine
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1095,"Msg":"��Ǹ�����Ļ��������쳣����Ϸ������ʧ�ܣ�"}'
		RETURN 7			
	END	

	-- У�����
	IF @szMachine<>@MachineID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1095,"Msg":"��Ǹ�����Ļ��������쳣����Ϸ������ʧ�ܣ�"}'
		RETURN 7			
	END

	-- �ж���ȡ����
	IF @ClaimTimes>=@ClaimTimesPerAccounts
	BEGIN		
		SET @strErrorDescribe=N'{"Idx":1096,"Msg":"��Ǹ������������ȡ��ϵͳ���͸�������Ϸ�ң���������ͨ����ֵ����ȡ��Ϸ�ң�"}'
		RETURN 8																																
	END	

	-- ��ѯ����
	DECLARE @MachineClaimTimes TINYINT
	DECLARE @UserIDString NVARCHAR(512)
	SELECT @MachineClaimTimes=PresentCount,@UserIDString=UserIDString FROM RecordSubsidyMachine	WHERE DateID=@DateID AND MachineID=@szMachine

	-- ��������
	IF @MachineClaimTimes IS NULL
	BEGIN
		-- ���ñ���
		SET @UserIDString=N''
		SET @MachineClaimTimes=0		

		-- �����¼
		INSERT INTO RecordSubsidyMachine(DateID, MachineID, PresentGold, PresentCount, UserIDString, FirstGrantDate, LastGrantDate)
		VALUES (@DateID, @szMachine, 0, 0, '', GetDate(), GetDate())		
	END
	
	-- �жϻ�������
	IF @MachineClaimTimes>=@ClaimTimesPerMachine
	BEGIN		
		SET @strErrorDescribe=N'{"Idx":1098,"Msg":"��Ǹ�������ڵĻ�����������ȡ��ϵͳ���͵���Ϸ�ң���������ͨ����ֵ����ȡ��Ϸ�ң�"}'
		RETURN 9																																
	END

	-- ���ͽ��	
	DECLARE @PresentScore BIGINT
    SET @PresentScore=@ClaimGold-@CurrScore

	-- �ͱ���¼
	UPDATE AccountsSubsidyClaim SET ClaimTimes=ClaimTimes+1, ClaimGolds=ClaimGolds+@PresentScore WHERE DateID=@DateID AND UserID=@dwUserID	

	-- ��ʶ�ж�
	DECLARE @szUserID NVARCHAR(12)
	SELECT @szUserID=CAST(@dwUserID AS nvarchar(12))

	-- �����ж�
	IF @UserIDString=N'' 
	BEGIN
		SET @UserIDString=@szUserID				
	END ELSE
	BEGIN
		SET @UserIDString=@UserIDString + N',' + @szUserID	
	END		

	-- ���¼�¼	
	UPDATE RecordSubsidyMachine SET PresentGold=PresentGold+@PresentScore,PresentCount=PresentCount+1,UserIDString=@UserIDString,LastGrantDate=GetDate()
	WHERE DateID=@DateID AND MachineID=@szMachine
		
	-- д��Ƹ�
	EXEC GSP_GP_WriteGameWealth @dwUserID,2,15,@PresentScore	

	-- ��������
	DECLARE	@OutCurrGold BIGINT
	DECLARE @OutClaimTimes TINYINT	

	-- ��ѯ��ȡ����
	SELECT @OutClaimTimes=ClaimTimes FROM AccountsSubsidyClaim WHERE DateID=@DateID AND UserID=@dwUserID
	
	-- ��ѯ��ǰ���
	SELECT @OutCurrGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID

	-- �׳���ʾ
	SET @strErrorDescribe=N'{"Idx":1099,"Msg":"��ϲ������Ϸ�����ͳɹ���"}'
	
	-- �׳�����
	SELECT @OutClaimTimes AS ClaimdTimes, @OutCurrGold AS CurrGold
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
-- ��ѯ�ͱ�
CREATE PROC [dbo].[GSP_GR_QuerySubsidy]
	@dwUserID int,							 -- �û�ID
	@szMachine NVARCHAR(32),				 -- ������ʶ
	@strErrorDescribe	NVARCHAR(127) OUTPUT -- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

BEGIN
	-- ��ѯ�û�
	IF NOT Exists(SELECT 1 FROM AccountsInfo WHERE UserID=@dwUserID)	
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1091,"Msg":"��Ǹ�������û���Ϣ�����ڣ�����ϵ�ͷ����ģ�"}'
		RETURN 1
	END

	-- ����ʱ��
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ���¼�¼
	UPDATE AccountsSubsidyClaim SET MachineID=@szMachine WHERE DateID=@DateID AND UserID=@dwUserID	

	-- ������Ϣ
	IF @@ROWCOUNT=0	
	BEGIN
		-- �����¼
		INSERT INTO AccountsSubsidyClaim(DateID, UserID, MachineID) VALUES (@DateID, @dwUserID, @szMachine)
	END	

	-- �ͱ�����
	DECLARE @ClaimGold BIGINT
	DECLARE @ClaimCondition BIGINT
	DECLARE	@ClaimTimesPerMachine TINYINT
	DECLARE	@ClaimTimesPerAccounts TINYINT
	
	-- ��ѯ����
	SELECT @ClaimGold=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND  StatusName='SubsidyGold'
	SELECT @ClaimCondition=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND  StatusName='SubsidyCondition'
	SELECT @ClaimTimesPerMachine=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND  StatusName='SubsidyTimesPerMachine'
	SELECT @ClaimTimesPerAccounts=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND  StatusName='SubsidyTimesPerAccounts'

	-- ��������
	IF @ClaimGold IS NULL SET @ClaimGold=400000
	IF @ClaimCondition IS NULL SET @ClaimCondition=400000
	IF @ClaimTimesPerMachine IS NULL SET @ClaimTimesPerMachine=3	
	IF @ClaimTimesPerAccounts IS NULL SET @ClaimTimesPerAccounts=3

	-- ��ѯ����
	DECLARE @MachineClaimTimes TINYINT		 
	SELECT @MachineClaimTimes=PresentCount FROM RecordSubsidyMachine WHERE DateID=@DateID AND MachineID=@szMachine		
	
	-- ��ȡ��¼
	DECLARE @ClaimTimes TINYINT
	DECLARE @RechargeStatus TINYINT
	SELECT @ClaimTimes=ClaimTimes FROM AccountsSubsidyClaim WHERE DateID=@DateID AND UserID=@dwUserID

	-- ʣ�����
	DECLARE @ResidualTimes TINYINT
	SET @ResidualTimes=@ClaimTimesPerAccounts-@ClaimTimes
		
	-- ����ʣ��
	IF @ResidualTimes > (@ClaimTimesPerMachine-@MachineClaimTimes)
	BEGIN
		SET @ResidualTimes=@ClaimTimesPerMachine-@MachineClaimTimes	
	END

	-- �������
	IF @ResidualTimes > @ClaimTimesPerAccounts SET @ResidualTimes=0	

	-- ��ѯ�ͱ���Ҫ�ų��Ľ��
	/*
	1���ʼ�δ��ȡ
	2���������ġ�ת������  (����Ӧ��ֻ����ÿ�յģ������ۼ�ֵ̫��ͱ���û����)  -- ����
	*/
	DECLARE @nConsumeScore BIGINT
	SELECT @nConsumeScore=SUM(GoodsCount) FROM LHAccountsDB..AccountsMailAttach(NOLOCK) WHERE GoodsID=2 AND MailID IN (
		SELECT MailID FROM LHAccountsDB..AccountsMailBox(NOLOCK) WHERE UserID=@dwUserID AND MailState IN (1,2)
	)
	IF @nConsumeScore IS NULL SET @nConsumeScore = 0

	-- �׳���¼
	SELECT @ResidualTimes AS ResidualTimes,@nConsumeScore AS ConsumeScore
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
-- ���͵ͱ�
CREATE PROC [dbo].[GSP_GR_LargessSubsidy]
	@dwUserID INT,							 -- �û�ID
	@dwClientIP INT,						 -- ���ӵ�ַ
	@szClientIP	NVARCHAR(15),				 -- ���ӵ�ַ
	@szMachine NVARCHAR(32),				 -- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT	 -- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON
BEGIN	

	-- �ͱ�����
	DECLARE @ClaimGold BIGINT
	DECLARE @ClaimCondition BIGINT
	DECLARE	@ClaimTimesPerMachine TINYINT
	DECLARE	@ClaimTimesPerIP TINYINT
	DECLARE	@ClaimTimesPerAccounts TINYINT

	-- ��ѯ����
	SELECT @ClaimGold=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND StatusName='SubsidyGold'
	SELECT @ClaimCondition=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND StatusName='SubsidyCondition'
	SELECT @ClaimTimesPerMachine=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND StatusName='SubsidyTimesPerMachine'
	SELECT @ClaimTimesPerIP=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND StatusName='SubsidyTimesPerIP'
	SELECT @ClaimTimesPerAccounts=CONVERT(INT,StatusValue) FROM  LHPlatformDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusKind=N'SubsidyOption' AND StatusName='SubsidyTimesPerAccounts'	

	-- ��������
	IF @ClaimGold IS NULL SET @ClaimGold=400000
	IF @ClaimCondition IS NULL SET @ClaimCondition=400000
	IF @ClaimTimesPerMachine IS NULL SET @ClaimTimesPerMachine=3	
	IF @ClaimTimesPerIP IS NULL SET @ClaimTimesPerIP=3
	IF @ClaimTimesPerAccounts IS NULL SET @ClaimTimesPerAccounts=3

	-- �ж���ȡ����	
	DECLARE @CurrScore BIGINT
	DECLARE @CurrInsure BIGINT
	DECLARE @Commission BIGINT
	DECLARE @CurrMailScore BIGINT
	SELECT @CurrScore=Score, @CurrInsure=InsureScore FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	SELECT @Commission=Commission FROM AccountsCommission WHERE UserID=@dwUserID
	SELECT @CurrMailScore=SUM(GoodsCount) FROM AccountsMailAttach WHERE GoodsID=1 AND MailID IN (SELECT MailID FROM AccountsMailBox WHERE UserID=@dwUserID AND MailState=1)

	-- �������
	IF @CurrScore IS NULL SET @CurrScore=0
	IF @CurrInsure IS NULL SET @CurrInsure=0
	IF @Commission IS NULL SET @Commission=0
	IF @CurrMailScore IS NULL SET @CurrMailScore=0

	-- �жϽ��
	IF (@CurrScore+@CurrInsure+@Commission+@CurrMailScore)>=@ClaimCondition OR @CurrScore<0 OR @CurrInsure<0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1093,"Msg":"��Ǹ������ǰ���ϵ���Ϸ�Ҳ���������������"}'
		RETURN 5		
	END		

	-- ����ʱ��
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��ѯ�ͱ�
	DECLARE @PStationID INT
	DECLARE @ClaimTimes TINYINT
	DECLARE @MachineID NVARCHAR(32)
	SELECT @ClaimTimes=ClaimTimes,@MachineID=MachineID FROM AccountsSubsidyClaim WHERE DateID=@DateID AND UserID=@dwUserID

	-- �����ж�
	IF @ClaimTimes IS NULL OR @MachineID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1109,"Msg":"��Ǹ�����ĵͷ����ͼ�¼�����쳣������ϵ�ͷ����ģ�"}'
		RETURN 6					
	END

	-- У�����
	IF @szMachine<>@MachineID
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1095,"Msg":"��Ǹ�����Ļ��������쳣����Ϸ������ʧ�ܣ�"}'
		RETURN 7			
	END	

	-- �ж���ȡ����
	IF @ClaimTimes>=@ClaimTimesPerAccounts
	BEGIN		
		SET @strErrorDescribe=N'{"Idx":1096,"Msg":"��Ǹ������������ȡ��ϵͳ���͸�������Ϸ�ң���������ͨ����ֵ����ȡ��Ϸ�ң�"}'
		RETURN 8																																
	END	

	-- ��ѯ����
	DECLARE @MachineClaimTimes TINYINT
	DECLARE @UserIDString NVARCHAR(512)
	SELECT @MachineClaimTimes=PresentCount,@UserIDString=UserIDString FROM RecordSubsidyMachine WHERE DateID=@DateID AND MachineID=@szMachine

	-- ��������
	IF @MachineClaimTimes IS NULL
	BEGIN
		-- ���ñ���
		SET @UserIDString=N''
		SET @MachineClaimTimes=0		

		-- �����¼
		INSERT INTO RecordSubsidyMachine(DateID, MachineID, PresentGold, PresentCount, UserIDString, FirstGrantDate, LastGrantDate)
		VALUES (@DateID, @szMachine, 0, 0, N'', GetDate(), GetDate())		
	END
	
	-- �жϻ�������
	IF @MachineClaimTimes>=@ClaimTimesPerMachine
	BEGIN		
		SET @strErrorDescribe=N'{"Idx":1098,"Msg":"��Ǹ�������ڵĻ�����������ȡ��ϵͳ���͵���Ϸ�ң���������ͨ����ֵ����ȡ��Ϸ�ң�"}'
		RETURN 9																																
	END

	-- ��ѯIP
	DECLARE @IPClaimTimes TINYINT
	SELECT @IPClaimTimes=PresentCount,@UserIDString=UserIDString FROM RecordSubsidyClientIP WHERE DateID=@DateID AND ClientIP=@szClientIP

	-- ��������
	IF @IPClaimTimes IS NULL
	BEGIN
		-- ���ñ���
		SET @UserIDString=N''
		SET @IPClaimTimes=0		

		-- �����¼
		INSERT INTO RecordSubsidyClientIP(DateID, ClientIP, PresentGold, PresentCount, UserIDString, FirstGrantDate, LastGrantDate)
		VALUES (@DateID, @szClientIP, 0, 0, N'', GetDate(), GetDate())		
	END
	
	-- �ж�IP����
	IF @IPClaimTimes>=@ClaimTimesPerIP
	BEGIN		
		SET @strErrorDescribe=N'{"Idx":1338,"Msg":"��Ǹ�������ڵ�IP��������ȡ��ϵͳ���͵���Ϸ�ң���������ͨ����ֵ����ȡ��Ϸ�ң�"}'
		RETURN 9																																
	END

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ	
	BEGIN TRAN

	-- ���ͽ��	
	DECLARE @PresentScore BIGINT
    SET @PresentScore=@ClaimGold-@CurrScore

	-- �ͱ���¼
	UPDATE AccountsSubsidyClaim SET ClaimTimes=ClaimTimes+1,ClaimGolds=ClaimGolds+@PresentScore WHERE DateID=@DateID AND UserID=@dwUserID	

	-- ��ʶ�ж�
	DECLARE @szUserID NVARCHAR(12)
	SELECT @szUserID=CAST(@dwUserID AS nvarchar(12))

	-- �����ж�
	IF @UserIDString=N'' 
	BEGIN
		SET @UserIDString=@szUserID				
	END ELSE
	BEGIN
		SET @UserIDString=@UserIDString + N',' + @szUserID	
	END		

	-- ���¼�¼	
	UPDATE RecordSubsidyMachine SET PresentGold=PresentGold+@PresentScore,PresentCount=PresentCount+1,UserIDString=@UserIDString,LastGrantDate=GetDate()
	WHERE DateID=@DateID AND MachineID=@szMachine
	UPDATE RecordSubsidyClientIP SET PresentGold=PresentGold+@PresentScore,PresentCount=PresentCount+1,UserIDString=@UserIDString,LastGrantDate=GetDate()
	WHERE DateID=@DateID AND ClientIP=@szClientIP
	
	-- д��Ƹ�
	EXEC GSP_GP_WriteGameWealth @dwUserID,1,15,@PresentScore

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	
	-- ��������
	DECLARE	@OutCurrGold BIGINT
	DECLARE @OutClaimTimes TINYINT	

	-- ��ѯ����
	SELECT @OutClaimTimes=ClaimTimes FROM AccountsSubsidyClaim WHERE DateID=@DateID AND UserID=@dwUserID

	-- ��ѯ��ǰ���
	SELECT @OutCurrGold=Score FROM LHGameGoldDB.dbo.GameScoreInfo WHERE UserID=@dwUserID

	-- ʣ�����
	DECLARE @ResidualTimes TINYINT
	SET @ResidualTimes=@ClaimTimesPerAccounts-@OutClaimTimes
		
	-- ����ʣ��
	IF @ResidualTimes>(@ClaimTimesPerMachine-@MachineClaimTimes-@IPClaimTimes)
	BEGIN
		SET @ResidualTimes=@ClaimTimesPerMachine-@MachineClaimTimes-@IPClaimTimes
	END

	-- �������
	IF @ResidualTimes>@ClaimTimesPerAccounts SET @ResidualTimes=0
	
	-- �׳���ʾ
	SET @strErrorDescribe=N'{"Idx":1099,"Msg":"��ϲ������Ϸ�����ͳɹ���"}'
	
	-- �׳�����
	SELECT @ResidualTimes AS ResidualTimes, @OutCurrGold AS CurrGold
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
