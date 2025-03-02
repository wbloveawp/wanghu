USE LHGameGoldDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadUserControl]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadUserControl]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserControlStart]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserControlStart]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserControlFinish]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserControlFinish]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserControlUpdate]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserControlUpdate]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- ���ؿ���
CREATE PROC GSP_GR_LoadUserControl
	@dwUserID INT								-- �û���ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��ѯ��¼
	IF NOT EXISTS(SELECT 1 FROM GameControlInfo(NOLOCK) WHERE UserID=@dwUserID)
	BEGIN
		RETURN 1
	END

	-- �׳���¼
	SELECT * FROM GameControlInfo(NOLOCK) WHERE UserID=@dwUserID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GR_UserControlStart
	@dwUserID INT,								-- �û���ʶ
	@cbControlMode TINYINT,						-- ����ģʽ
	@cbControlKind TINYINT,						-- ���Ʒ�ʽ
	@cbControlStatus TINYINT,					-- ����״̬	
	@wTargetWinRate SMALLINT,					-- Ŀ��ʤ��					
	@wControlWinRate SMALLINT,					-- ����ʤ��
	@wInitailWinRate SMALLINT,					-- ��ǰʤ��	
	@lInitailWinScore BIGINT,					-- �ۼ�Ӯ��
	@lInitailLoseScore BIGINT					-- �ۼ����
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ɾ����¼
	DELETE GameControlInfo WHERE UserID=@dwUserID

	-- �����¼
	INSERT INTO GameControlInfo(UserID,ControlKind,ControlMode,ControlStatus,TargetWinRate,ControlWinRate,InitailWinRate,InitailWinScore,InitailLoseScore,ControlStartTime)
	VALUES (@dwUserID,@cbControlKind,@cbControlMode,@cbControlStatus,@wTargetWinRate,@wControlWinRate,@wInitailWinRate,@lInitailWinScore,@lInitailLoseScore,GetDate())	
	
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GR_UserControlFinish
	@dwUserID INT,								-- �û���ʶ
	@lTotalWinCount INT,						-- Ӯ������
	@lTotalLoseCount INT,						-- �������
	@lTotalWinScore BIGINT,						-- �ۼ�Ӯ��
	@lTotalLoseScore BIGINT,					-- �ۼ����	
	@lTotalBetAmount BIGINT = 0					-- �ۼƴ���	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��������
	UPDATE GameControlInfo SET TotalWinCount=@lTotalWinCount,TotalLoseCount=@lTotalLoseCount,TotalWinScore=@lTotalWinScore, TotalLoseScore=@lTotalLoseScore,TotalBetAmount=@lTotalBetAmount 
	WHERE UserID=@dwUserID

	INSERT INTO RecordGameControl(UserID,ControlKind,ControlMode,ControlStatus,TargetWinRate,ControlWinRate,InitailWinRate,InitailWinScore,InitailLoseScore,TotalWinCount,
		TotalLoseCount,TotalWinScore,TotalLoseScore,TotalBetAmount,ControlDuration,ControlPlayCount,ControlStartTime,ControlFinishTime,OperaterUserID,OperateRemark)
	SELECT UserID,ControlKind,ControlMode,ControlStatus,TargetWinRate,ControlWinRate,InitailWinRate,InitailWinScore,InitailLoseScore,TotalWinCount,
		TotalLoseCount,TotalWinScore,TotalLoseScore,TotalBetAmount,ControlDuration,ControlPlayCount,ControlStartTime,GETDATE(),OperaterMasterID,OperateRemark  FROM GameControlInfo 
	WHERE UserID=@dwUserID

	-- ɾ������
	DELETE GameControlInfo WHERE UserID=@dwUserID		
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���Ƹ���
CREATE PROC GSP_GR_UserControlUpdate
	@dwUserID INT,								-- �û���ʶ
	@lTotalWinCount INT,						-- Ӯ������
	@lTotalLoseCount INT,						-- �������
	@lTotalWinScore BIGINT,						-- �ۼ�Ӯ��
	@lTotalLoseScore BIGINT,					-- �ۼ����	
	@lTotalBetAmount BIGINT						-- �ۼƴ���	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��������
	UPDATE GameControlInfo SET TotalWinCount=@lTotalWinCount,TotalLoseCount=@lTotalLoseCount,TotalWinScore=@lTotalWinScore, TotalLoseScore=@lTotalLoseScore,TotalBetAmount=@lTotalBetAmount 
	WHERE UserID=@dwUserID

	-- ��ѯ��Ϣ
	DECLARE @WinCount SMALLINT
	DECLARE @LoseCount SMALLINT
	DECLARE @ControlKind SMALLINT
	DECLARE @ControlStatus SMALLINT	
	SELECT @ControlStatus=ControlStatus,@ControlKind=ControlKind,@WinCount=TotalWinCount,@LoseCount=TotalLoseCount FROM GameControlInfo(NOLOCK) WHERE UserID=@dwUserID

	-- ����״̬
	IF @ControlKind IS NULL SET @ControlKind=0xFF
	IF @ControlStatus IS NULL SET @ControlStatus=0

	-- ���ֱ�������
	IF @ControlKind=3 AND @ControlStatus<>0
	BEGIN

		------------------------------------�Զ�������˳��ж�------------------------------------------
		-- ��������
		DECLARE @MaxPlayCount INT = 50
		DECLARE @MinPayAmount DECIMAL(18,2) = 1
				
		--��Ϸ����
		DECLARE @PlayCount INT
		SELECT @PlayCount=@WinCount+@LoseCount
		
		-- У�����
		IF @PlayCount<@MaxPlayCount
		BEGIN
			-- ��ֵ��Ϣ
			DECLARE @PayAmount DECIMAL(18,2)=0
			SELECT @PayAmount=PayAmount FROM LHInsureDB.dbo.AccountsPay(NOLOCK) WHERE UserID=@dwUserID

			-- У���ֵ���
			IF @PayAmount>=@MinPayAmount
			BEGIN
				SET @ControlStatus=0
			END				

		END ELSE
		BEGIN
			SET @ControlStatus=0
		END
		
		------------------------------------�Զ�������˳��ж�------------------------------------------

		-- ���ƽ���
		IF @ControlStatus=0
		BEGIN
			-- ���Ƽ�¼
			INSERT INTO RecordGameControl(UserID,ControlKind,ControlMode,ControlStatus,TargetWinRate,ControlWinRate,InitailWinRate,InitailWinScore,InitailLoseScore,TotalWinCount,
				TotalLoseCount,TotalWinScore,TotalLoseScore,TotalBetAmount,ControlDuration,ControlPlayCount,ControlStartTime,ControlFinishTime,OperaterUserID,OperateRemark)
			SELECT UserID,ControlKind,ControlMode,ControlStatus,TargetWinRate,ControlWinRate,InitailWinRate,InitailWinScore,InitailLoseScore,TotalWinCount,
				TotalLoseCount,TotalWinScore,TotalLoseScore,TotalBetAmount,ControlDuration,ControlPlayCount,ControlStartTime,GETDATE(),OperaterMasterID,OperateRemark  FROM GameControlInfo 
			WHERE UserID=@dwUserID

			-- ɾ������
			DELETE GameControlInfo WHERE UserID=@dwUserID
		END

	END

	-- �׳�״̬
	SELECT @ControlStatus AS ControlStatus
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
