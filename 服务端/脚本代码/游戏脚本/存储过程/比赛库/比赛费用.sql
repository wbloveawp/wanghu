USE LHGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchSignup]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchSignup]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchUnSignup]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchUnSignup]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchShare]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchShare]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchRevive]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchRevive]
GO
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchReviveRepeal]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchReviveRepeal]
GO



SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �۳�����
CREATE PROC GSP_GR_MatchSignup
	@dwUserID INT,								-- �û� I D	
	@InitScore BIGINT,							-- ��ʼ����	
	@wMaxSignupUser SMALLINT,					-- �������
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D
	@dwMatchID INT,								-- ���� I D
	@lMatchNo BIGINT,							-- ��������
	@wReason SMALLINT,							-- ���ԭ��
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ	
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	
	-- ������Ϣ
	DECLARE @FeeMoneyID SMALLINT
	DECLARE @FeeAmount INT
	DECLARE @Experience INT	
	DECLARE @SignupMode TINYINT	
	DECLARE @SignupCount SMALLINT
	DECLARE @MatchType TINYINT
	DECLARE @MemberOrder TINYINT	
	DECLARE @JoinCondition TINYINT
	DECLARE @UseTicketCount INT	
	
	-- ��ѯ����	
	SELECT @FeeMoneyID=FeeMoneyID, @FeeAmount=FeeAmount,@UseTicketCount=UseTicketCount, @MatchType=MatchType, @Experience=Experience, @JoinCondition=JoinCondition, 
		  @SignupMode=SignupMode,@SignupCount=SignupCount, @MemberOrder=MemberOrder FROM MatchOption(NOLOCK) WHERE MatchID=@dwMatchID

	-- �����ж�
	IF @FeeMoneyID IS NULL
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1110,"Msg":"��Ǹ,�������ı��������ڣ�"}'
		return 1		
	END
	
	-- ��ѯ�û�
	DECLARE @dwGameID INT
	DECLARE @lExperience INT
	DECLARE @cbMemberOrder TINYINT
	SELECT @dwGameID=GameID, @lExperience=Experience, @cbMemberOrder=MemberOrder FROM LHAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	
	-- �����ж�
	IF @lExperience IS NULL
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1091,"Msg":"��Ǹ,�����û���Ϣ�����ڣ�����ϵ�ͷ����Ĺ�����Ա�˽������"}'
		return 1	 
	END	
	
	-- ��Ա�ȼ�
	IF (@JoinCondition&0x01)<>0 AND @cbMemberOrder<@MemberOrder
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1111,"Msg":"��Ǹ�����Ļ�Ա�ȼ��ﲻ����������������ʧ�ܣ�"}'
		return 2				
	END
	
	-- �û�����
	IF (@JoinCondition&0x02)<>0 AND @lExperience<@Experience
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1112,"Msg":"��Ǹ�����ľ���ֵ�ﲻ����������������ʧ�ܣ�"}'
		return 2				
	END
	
	-- ��������
	DECLARE @MaxSignupUser SMALLINT
	SELECT @MaxSignupUser=COUNT(1) FROM MatchScoreInfo(NOLOCK) WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo
	
	-- �û���Ϣ
	DECLARE @IsAndroidUser TINYINT
	SET @IsAndroidUser=0
	
	-- �����ж�
	IF @strClientIP=N'0.0.0.0' SET @IsAndroidUser=1

	-- �ж�����
	IF @IsAndroidUser=0 AND @wMaxSignupUser>0 AND @MaxSignupUser>=@wMaxSignupUser
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1113,"Msg":"��Ǹ�����������Ѵﵽ���������������ʧ�ܣ�"}'
		return 3
	END	

	-- ��������
	DECLARE @wSignupCount INT
	DECLARE @SignupTime DATETIME
	SELECT @wSignupCount=SignupCount,@SignupTime=SignupTime FROM MatchScoreInfo(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo

	-- ��������
	IF @wSignupCount IS NULL SET @wSignupCount=0

	-- У�����
	IF @SignupCount>0 AND @wSignupCount>=@SignupCount
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1115,"Msg":"���Ѿ�������������������Ҫ�ظ�������"}'
		return 4
	END	

	-- ��������
	DECLARE @cbSignupMode TINYINT
	DECLARE @dwSingupRecordID INT

	-- ���ñ���
	SET @cbSignupMode=0
	SET @dwSingupRecordID=0

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ʹ�ñ���ȯ
	IF (@SignupMode&0x02)<>0 AND @cbSignupMode=0 AND @IsAndroidUser=0
	BEGIN	
		-- ��ѯ����ȯ
		DECLARE @TicketCount INT
		SELECT @TicketCount=BuyTicketCount-UseTicketCount FROM StreamMatchTicket(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo

		-- ��������
		IF @TicketCount IS NULL SET @TicketCount=0

		-- ��ȯ����
		IF @TicketCount < @UseTicketCount		
		BEGIN
			-- �ع�����
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED

			SET @strErrorDescribe = N'{"Idx":1290,"Msg":"�����ϵı���ȯ����, �޷����������"}'
			return 5				
		END

		-- ����ģʽ
		SET @cbSignupMode=0x02

		-- ������ȯ
		UPDATE StreamMatchTicket SET UseTicketCount+=@UseTicketCount WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo

		-- �����¼	
		INSERT StreamMatchFeeInfo (UserID,ServerID,MatchID,MatchNo,MatchType,SignupMode,FeeMoneyID,FeeAmount,UseTicketCount,CollectDate) 
		VALUES(@dwUserID,@wServerID,@dwMatchID,@lMatchNo,@MatchType,@cbSignupMode,0,0,@UseTicketCount,GETDATE())	
		
		-- ��¼��ʶ
		SET @dwSingupRecordID=SCOPE_IDENTITY()
	END	

	-- �۳�������
	IF (@SignupMode&0x01)<>0 AND @cbSignupMode<>0 AND @IsAndroidUser=0
	BEGIN	
		-- �۳�����
		IF @FeeAmount > 0
		BEGIN	
			-- �۳���ʯ
			IF @FeeMoneyID = 1
			BEGIN
				-- ��ѯ��ʯ
				DECLARE @UserIngot BIGINT
				SELECT @UserIngot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID
					
				-- ������ʯ
				IF @UserIngot IS NULL SET @UserIngot=0

				-- ��ʯ����
				IF @UserIngot < @FeeAmount		
				BEGIN
					-- �ع�����
					ROLLBACK TRAN
					SET TRANSACTION ISOLATION LEVEL READ COMMITTED

					SET @strErrorDescribe = N'{"Idx":1118,"Msg":"�����ϵ���ʯ����,�޷�������"}'
					return 4				
				END

				-- �����¼
				INSERT INTO LHGameRecordDB.dbo.RecordChangeIngot(UserID, GameID, TypeID, BeforeIngot, ChangeIngot,CollectClientIP, CollectDateTime)
				VALUES (@dwUserID, @dwGameID, @wReason, @UserIngot, -@FeeAmount,  N'-----------', GetDate())

				-- ������ʯ
				UPDATE LHAccountsDB.dbo.AccountsIngotInfo SET Score=Score-@FeeAmount WHERE UserID=@dwUserID		
			END		

			-- �۳����
			IF @FeeMoneyID = 2
			BEGIN
				-- ��ѯ���
				DECLARE @UserScore BIGINT
				SELECT @UserScore=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID
					
				-- �������
				IF @UserScore IS NULL SET @UserScore=0

				-- ��Ҳ���
				IF @UserScore < @FeeAmount		
				BEGIN
					SET @strErrorDescribe = N'{"Idx":1116,"Msg":"�����ϵĽ�Ҳ���,�޷�������"}'
					return 4				
				END

				-- �����¼
				INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID, GameID, TypeID, BeforeGold, ChangeGold, CollectClientIP, CollectDateTime)
				VALUES (@dwUserID, @dwGameID, @wReason, @UserScore, -@FeeAmount, N'------------', GetDate())

				-- ���½��
				UPDATE LHAccountsDB.dbo.AccountsGoldInfo SET Score=Score-@FeeAmount WHERE UserID=@dwUserID					
			END					
							
		END	

		-- ����ģʽ
		SET @cbSignupMode=0x01

		-- �����¼	
		INSERT StreamMatchFeeInfo (UserID,ServerID,MatchID,MatchNo,MatchType,SignupMode,FeeMoneyID,FeeAmount,UseTicketCount,CollectDate) 
		VALUES(@dwUserID,@wServerID,@dwMatchID,@lMatchNo,@MatchType,@cbSignupMode,@FeeMoneyID,@FeeAmount,0,GETDATE())
		
		-- ��¼��ʶ
		SET @dwSingupRecordID=SCOPE_IDENTITY()
	END		

	-- ���·���
	IF @SignupTime IS NOT NULL
	BEGIN
		-- �״α���
		IF @wSignupCount=0 OR @MatchType<>0x04
		BEGIN
			UPDATE MatchScoreInfo SET ServerID=@wServerID,Score=@InitScore,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0,PlayTimeCount=0,WinStreak=0,
			MatchRight=0x40,SignupTime=GetDate(),SignupCount+=1,SignupRecordID=@dwSingupRecordID
			WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo	
		END ELSE
		BEGIN
			UPDATE MatchScoreInfo SET ServerID=@wServerID,MatchRight=0x40,SignupTime=GetDate(),SignupCount+=1,SignupRecordID=@dwSingupRecordID 
			WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo	
		END
	END ELSE
	BEGIN
		INSERT INTO MatchScoreInfo (UserID,MatchID,MatchNo,GameID,ServerID,Score,MatchRight,SignupCount,SignupRecordID) 
		VALUES (@dwUserID,@dwMatchID,@lMatchNo,@dwGameID,@wServerID,@InitScore,0x40,1,@dwSingupRecordID)				
	END

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- ��ѯ���
	DECLARE @CurrUserScore BIGINT
	SELECT @CurrUserScore=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID
	
	-- ��ѯ��ʯ
	DECLARE @CurrUserIngot BIGINT
	SELECT @CurrUserIngot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID		

	-- ��ѯ��ȯ
	DECLARE @CurrTicketCount SMALLINT
	SELECT @CurrTicketCount=BuyTicketCount-UseTicketCount FROM StreamMatchTicket(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo	
	
	-- ��������
	IF @CurrUserScore IS NULL SET @CurrUserScore=0
	IF @CurrUserIngot IS NULL SET @CurrUserIngot=0
	IF @CurrTicketCount IS NULL SET @CurrTicketCount=0

	-- �׳�����
	SELECT @cbSignupMode AS SignupMode, @CurrUserScore AS UserScore, @CurrUserIngot AS UserIngot, @CurrTicketCount AS TicketCount
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- ȡ������
CREATE PROC GSP_GR_MatchUnSignup
	@dwUserID INT,								-- �û� I D
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@dwMatchID	INT,							-- ���� I D
	@lMatchNo BIGINT,							-- ��������
	@wReason SMALLINT,							-- ���ԭ��
	@strMachineID NVARCHAR(32)	,				-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- �����ж�
	DECLARE @GameID INT
	SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- У���ʶ
	IF @GameID IS NULL
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1091,"Msg":"��Ǹ,�����û���Ϣ�����ڣ�����ϵ�ͷ����Ĺ�����Ա�˽������"}'
		return 1		
	END
	
	-- ������Ϣ
	DECLARE @FeeMoneyID TINYINT
	DECLARE @FeeAmount BIGINT
	DECLARE @UseTicketCount SMALLINT
	DECLARE @SignupCount SMALLINT
	SELECT @SignupCount=SignupCount,@FeeMoneyID=FeeMoneyID,@FeeAmount=FeeAmount,@UseTicketCount=UseTicketCount FROM MatchOption(NOLOCK) WHERE MatchID=@dwMatchID

	-- �����ж�	
	IF @FeeMoneyID IS NULL
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1110,"Msg":"��Ǹ,�������ı��������ڣ�"}'
		return 2		
	END

	-- ��ѯ��¼
	DECLARE @SignupRecordID INT
	SELECT @SignupRecordID=SignupRecordID FROM MatchScoreInfo(NOLOCK) WHERE UserID=@dwUserID AND MatcHID=@dwMatchID AND MatchNo=@lMatchNo

	-- �����ж�	
	IF @SignupRecordID IS NULL
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1110,"Msg":"��Ǹ,�������ı��������ڣ�"}'
		return 3
	END	

	-- ��ѯ��¼
	DECLARE @SignupMode TINYINT	
	DECLARE @SignupDate DATETIME
	SELECT @SignupMode=SignupMode,@SignupDate=CollectDate FROM StreamMatchFeeInfo(NOLOCK) WHERE RecordID=@SignupRecordID

	-- �����ж�	
	IF @SignupMode IS NULL
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1110,"Msg":"��Ǹ,�������ı��������ڣ�"}'
		return 4
	END	

	-- ������ʶ
	DECLARE @IsAndroidUser TINYINT	

	-- �����ж�
	SET @IsAndroidUser=0
	IF @strClientIP=N'0.0.0.0' SET @IsAndroidUser=1	

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��������
	IF @IsAndroidUser=0
	BEGIN		
		-- �۷�ģʽ
		IF @SignupMode=0x01 AND @FeeAmount>0
		BEGIN		

			-- ��ʯ֧��
			IF @FeeMoneyID = 1
			BEGIN
				-- ��ѯ��ʯ
				DECLARE @UserIngot BIGINT
				SELECT @UserIngot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID
					
				IF @UserIngot IS NULL
				BEGIN
					-- �ع�����
					ROLLBACK TRAN
					SET TRANSACTION ISOLATION LEVEL READ COMMITTED

					SET @strErrorDescribe = N'{"Idx":1121,"Msg":ϵͳδ��ѯ��������ʯ��Ϣ,���������ǵĿͷ���Ա��ϵ��"}'
					return 3				
				END				

				-- �����¼
				INSERT INTO LHGameRecordDB.dbo.RecordChangeIngot(UserID, GameID, TypeID, BeforeIngot, ChangeIngot,CollectClientIP, CollectDateTime)
				VALUES (@dwUserID, @GameID, @wReason, @UserIngot, @FeeAmount,  N'-----------', GetDate())

				-- ������ʯ
				UPDATE LHAccountsDB.dbo.AccountsIngotInfo SET Score+=@FeeAmount WHERE UserID=@dwUserID					
			END

			-- ���֧��
			IF @FeeMoneyID = 2
			BEGIN
				-- ��ѯ���
				DECLARE @Score BIGINT
				SELECT @Score=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID
				
				IF @Score IS NULL
				BEGIN
					-- �ع�����
					ROLLBACK TRAN
					SET TRANSACTION ISOLATION LEVEL READ COMMITTED

					SET @strErrorDescribe = N'{"Idx":1119,"Msg":"ϵͳδ��ѯ�����Ľ����Ϣ,���������ǵĿͷ���Ա��ϵ��"}'
					return 3						
				END

				-- �����¼
				INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID, GameID, TypeID, BeforeGold, ChangeGold, CollectClientIP, CollectDateTime)
				VALUES (@dwUserID, @GameID, @wReason, @Score, -@FeeAmount, N'------------', GetDate())

				-- ���½��
				UPDATE LHAccountsDB.dbo.AccountsGoldInfo SET Score=@Score+@FeeAmount WHERE UserID=@dwUserID				
			END				
		END	
		
		-- ����ȯģʽ
		IF @SignupMode=0x02
		BEGIN	
			-- ���±���ȯ
			UPDATE StreamMatchTicket SET UseTicketCount-=@UseTicketCount WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
		END
	END

	-- ɾ����¼
	IF @IsAndroidUser=0 
	BEGIN
		DELETE StreamMatchFeeInfo WHERE RecordID=@SignupRecordID
	END

	-- ���±�����
	IF @SignupCount=1
	BEGIN
		DELETE MatchScoreInfo WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
	END ELSE
	BEGIN
		UPDATE MatchScoreInfo SET SignupCount-=1, MatchRight=0, SignupRecordID=0 WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
	END

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- ��ѯ���
	DECLARE @CurrUserScore BIGINT
	SELECT @CurrUserScore=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID

	-- ��ѯ��ʯ
	DECLARE @CurrUserIngot BIGINT
	SELECT @CurrUserIngot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID	

	-- ����ȯ
	DECLARE @CurrUserTicket INT
	SELECT @CurrUserTicket=BuyTicketCount-UseTicketCount FROM StreamMatchTicket(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
	
	-- ��������
	IF @CurrUserScore IS NULL SET @CurrUserScore=0
	IF @CurrUserIngot IS NULL SET @CurrUserIngot=0
	IF @CurrUserTicket IS NULL SET @CurrUserTicket=0

	-- �׳�����
	SELECT @SignupMode AS SignupMode, @CurrUserScore AS UserScore, @CurrUserIngot AS UserIngot, @CurrUserTicket AS UserTicket
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- ��Ҹ���
CREATE PROC GSP_GR_MatchRevive
	@dwUserID INT,								-- �û� I D
	@dwMatchID	INT,							-- ���� I D	
	@lMatchNo	BIGINT,							-- ��������
	@wServerID	SMALLINT,						-- �����ʶ
	@lInitScore  BIGINT,						-- ��ʼ����	
	@wFeeMoneyID SMALLINT,						-- ��������
	@lFeeAmount  INT,							-- ���ý��
	@cbReviveTimes TINYINT,						-- �������
	@wReason SMALLINT,							-- ���ԭ��	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	

	-- ��ѯ�û�
	DECLARE @GameID INT	
	SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- �û�������
	IF @GameID IS NULL
	BEGIN
		SET @strErrorDescribe = N'{"Idx":1091,"Msg":"��Ǹ,�����û���Ϣ�����ڣ�����ϵ�ͷ����Ĺ�����Ա�˽������"}'
		return 1			
	END
	
	-- ��ѯ������Ϣ
	DECLARE @ReviveTimes TINYINT
	SELECT @ReviveTimes=ReviveTimes FROM MatchReviveInfo(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo

	-- ��Ϣ������
	IF @ReviveTimes IS NULL SET @ReviveTimes=0;
	
	-- �жϴ���
	IF @ReviveTimes>=@cbReviveTimes
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1126,"Msg":"��Ǹ������������������� ' +CONVERT(nvarchar(20),@cbReviveTimes)+N' �Σ�����ʧ�ܣ�","Ops":["'+CONVERT(nvarchar(20),@cbReviveTimes)+'"]}'	
		RETURN 2					 
	END 

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN
		
	-- �۳�����
	IF @lFeeAmount > 0
	BEGIN		
		-- �۳���ʯ
		IF @wFeeMoneyID = 1
		BEGIN
			-- ��ѯ��ʯ
			DECLARE @UserIngot BIGINT
			SELECT @UserIngot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID
					
			-- ������ʯ
			IF @UserIngot IS NULL SET @UserIngot=0

			-- ��ʯ����
			IF @UserIngot < @lFeeAmount		
			BEGIN
				-- �ع�����
				ROLLBACK TRAN
				SET TRANSACTION ISOLATION LEVEL READ COMMITTED

				SET @strErrorDescribe = N'{"Idx":1129,"Msg":"��Ǹ,�����ϵ���ʯ����,����ʧ�ܣ�"}'
				return 4				
			END
						
			-- �����¼
			INSERT INTO LHGameRecordDB.dbo.RecordChangeIngot(UserID, GameID, TypeID, BeforeIngot, ChangeIngot,CollectClientIP, CollectDateTime)
			VALUES (@dwUserID, @GameID, @wReason, @UserIngot, @lFeeAmount,  N'-----------', GetDate())

			-- ������ʯ
			UPDATE LHAccountsDB.dbo.AccountsIngotInfo SET Score=Score-@lFeeAmount WHERE UserID=@dwUserID		
		END

		-- �۳����
		IF @wFeeMoneyID = 2
		BEGIN
			-- ��ѯ���
			DECLARE @Score BIGINT
			SELECT @Score=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID
					
			-- �������
			IF @Score IS NULL SET @Score=0

			-- ��Ҳ���
			IF @Score < @lFeeAmount		
			BEGIN
				-- �ع�����
				ROLLBACK TRAN
				SET TRANSACTION ISOLATION LEVEL READ COMMITTED

				SET @strErrorDescribe = N'{"Idx":1127,"Msg":"��Ǹ,�����ϵ���Ϸ�Ҳ���,����ʧ�ܣ�"}'
				return 4				
			END

			-- �����¼
			INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID, GameID, TypeID, BeforeGold, ChangeGold, CollectClientIP, CollectDateTime)
			VALUES (@dwUserID, @GameID, @wReason, @Score, -@lFeeAmount, N'------------', GetDate())

			-- ���½��
			UPDATE LHAccountsDB.dbo.AccountsGoldInfo SET Score=@Score-@lFeeAmount WHERE UserID=@dwUserID					
		END		
		
	END
		
	-- ����״̬
	UPDATE MatchReviveInfo SET FeeAmount+=@lFeeAmount,ReviveTimes=ReviveTimes+1 
	WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo		
	
	-- ��������
	IF @@ROWCOUNT=0
	BEGIN
		-- ��������
		INSERT INTO MatchReviveInfo(UserID,MatchID,MatchNo,FeeMoneyID,FeeAmount,ReviveTimes,CollectDate)
		VALUES (@dwUserID,@dwMatchID,@lMatchNo,@wFeeMoneyID,@lFeeAmount,1,GetDate())
	END		
	
	-- ���·���
	UPDATE MatchScoreInfo SET Score=@lInitScore,RankID=0,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0,PlayTimeCount=0,MatchRight=0x40,SignupTime=GetDate()
	WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo				
	
	-- �������
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO MatchScoreInfo (UserID,MatchID,MatchNo,GameID,ServerID,Score,MatchRight) 
		VALUES (@dwUserID,@dwMatchID,@lMatchNo,@GameID,@wServerID,@lInitScore,0x40) 				
	END

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	
	-- ��ѯ���
	DECLARE @CurrUserScore BIGINT
	SELECT @CurrUserScore=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID

	-- ��ѯ��ʯ
	DECLARE @CurrUserIngot BIGINT
	SELECT @CurrUserIngot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID	
	
	-- ��������
	IF @CurrUserScore IS NULL SET @CurrUserScore=0	
	IF @CurrUserIngot IS NULL SET @CurrUserIngot=0

	-- �׳�����
	SELECT @CurrUserScore AS UserScore, @CurrUserIngot AS UserIngot
END

RETURN 0
GO


----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GR_MatchReviveRepeal
	@dwUserID INT,								-- �û� I D
	@dwMatchID INT,								-- ���� I D	
	@lMatchNo  BIGINT,							-- ��������	
	@wFeeMoneyID  SMALLINT,						-- ��������
	@lFeeAmount INT,							-- ���ý��
	@wReason SMALLINT							-- ���ԭ��
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	
	
	-- ��ѯ�û�
	DECLARE @GameID INT	
	SELECT @GameID=GameID FROM LHAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- �û�������
	IF @GameID IS NULL
	BEGIN
		return 1			
	END

	-- ��ѯ������Ϣ	
	IF EXISTS(SELECT * FROM MatchReviveInfo(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo)
	BEGIN
		RETURN 1
	END
	
	-- �˻�����
	IF @lFeeAmount > 0
	BEGIN				
		-- �۳����
		IF @wFeeMoneyID = 2
		BEGIN
			-- ��ѯ���
			DECLARE @UserScore BIGINT
			SELECT  @UserScore=Score FROM LHAccountsDB.dbo.AccountsGoldInfo WHERE UserID=@dwUserID	

			-- ���½��
			UPDATE LHAccountsDB.dbo.AccountsGoldInfo SET Score=Score+@lFeeAmount WHERE UserID=@dwUserID		
			
			-- �����¼
			INSERT INTO LHGameRecordDB.dbo.RecordChangeGold(UserID, GameID, TypeID, BeforeGold, ChangeGold, CollectClientIP, CollectDateTime)
			VALUES (@dwUserID, @GameID, @wReason, @UserScore, @lFeeAmount, N'------------', GetDate())
		END
				
		-- �۳���ʯ
		IF @wFeeMoneyID = 1
		BEGIN
			-- ��ѯ��ʯ
			DECLARE @UserIngot BIGINT
			SELECT @UserIngot=Score FROM LHAccountsDB.dbo.AccountsIngotInfo WHERE UserID=@dwUserID
					
			-- ������ʯ
			IF @UserIngot IS NULL SET @UserIngot=0
						
			-- �����¼
			INSERT INTO LHGameRecordDB.dbo.RecordChangeIngot(UserID, GameID, TypeID, BeforeIngot, ChangeIngot,CollectClientIP, CollectDateTime)
			VALUES (@dwUserID, @GameID, @wReason, @UserIngot, @lFeeAmount,  N'-----------', GetDate())

			-- ������ʯ
			UPDATE LHAccountsDB.dbo.AccountsIngotInfo SET Score=Score+@lFeeAmount WHERE UserID=@dwUserID		
		END
	END
		
	-- ����״̬
	UPDATE MatchReviveInfo SET FeeAmount=FeeAmount-@lFeeAmount,ReviveTimes=ReviveTimes-1 
	WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo				
	
	-- ɾ������
	DELETE MatchScoreInfo WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo				
	
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
