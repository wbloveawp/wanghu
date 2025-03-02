
----------------------------------------------------------------------------------------------------

USE LHGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadMatchOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadMatchOption]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_DeleteMatchOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_DeleteMatchOption]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_RemoveMatchOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_RemoveMatchOption]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_InsertMatchOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_InsertMatchOption]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_ModifyMatchOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_ModifyMatchOption]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ���ر���
CREATE  PROCEDURE dbo.GSP_GS_LoadMatchOption
	@wKindID   INT,								-- ���ͱ�ʶ	
	@strServiceMachine NCHAR(32),				-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ���ر���
	SELECT * FROM MatchOption WHERE ((KindID=@wKindID AND @wKindID<>0) OR @wKindID=0) AND ServiceMachine=@strServiceMachine AND Nullity=0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ɾ������
CREATE  PROCEDURE dbo.GSP_GS_DeleteMatchOption	
	@dwMatchID INT								-- ������ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ɾ������
	DELETE MatchOption WHERE MatchID=@dwMatchID			
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �Ƴ�����
CREATE  PROCEDURE dbo.GSP_GS_RemoveMatchOption	
	@dwMatchID INT								-- ������ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ɾ������
	UPDATE MatchOption SET Nullity=1 WHERE MatchID=@dwMatchID			
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �������
CREATE  PROCEDURE dbo.GSP_GS_InsertMatchOption	
	-- ������Ϣ
	@wKindID INT,								-- ���ͱ�ʶ
	@dwGroupID BIGINT,							-- ��ݱ�ʶ
	@strModuleName NVARCHAR(32),				-- ģ������		
	@strMatchName NVARCHAR(32),					-- ��������
	@strMatchDate NVARCHAR(64),					-- ����ʱ��	
	@cbShowType TINYINT,						-- չʾ����		
	@cbMatchType TINYINT,						-- ��������		
	
	-- ������Ϣ
	@wFeeMoneyID SMALLINT,						-- ��������	
	@lFeeAmount BIGINT,							-- ���ý��
	@wUseTicketCount SMALLINT,					-- ʹ������
	@wBuyTicketCount SMALLINT,					-- ��������
	@wBuyTicketOption SMALLINT,					-- ��Ʊѡ��
	@lBuyTicketAmount BIGINT,					-- ���ҽ��
	@wBuyTicketMoneyID SMALLINT,				-- ��������
	@wSignupCount SMALLINT,						-- ��������
	@cbSignupMode TINYINT,						-- ������ʽ
	@cbJoinCondition TINYINT,					-- ��������
	@cbMemberOrder TINYINT,						-- ��Ա�ȼ�	
	@lExperience INT,							-- ����ȼ�	

	-- ��������
	@cbMatchDistribute VARBINARY(64),			-- ��������
		
	-- ��������
	@wRewardCount SMALLINT,						-- ������Ŀ
	@cbMatchReward VARBINARY(MAX),				-- ��������

	-- �����ִ�
	@wRoundCount SMALLINT,						-- �ִ���Ŀ
	@cbMatchRound VARBINARY(512),				-- �����ִ�

	-- ��չ����
	@cbMatchRule VARBINARY(512),				-- ��������	
	@strServiceMachine NCHAR(32),				-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��������
	INSERT INTO MatchOption(KindID,GroupID,MatchName,ModuleName,MatchType,ShowType,SignupCount,SignupMode,FeeAmount,FeeMoneyID,UseTicketCount,BuyTicketCount,BuyTicketOption,BuyTicketAmount,BuyTicketMoneyID,JoinCondition,MemberOrder,
		Experience,MatchDistribute,MatchReward,RewardCount,MatchRound,RoundCount,MatchRule,ServiceMachine,CollectDate)		
	VALUES(@wKindID,@dwGroupID,@strMatchName,@strModuleName,@cbMatchType,@cbShowType,@wSignupCount,@cbSignupMode,@lFeeAmount,@wFeeMoneyID,@wUseTicketCount,@wBuyTicketCount,@wBuyTicketOption,@lBuyTicketAmount,@wBuyTicketMoneyID
		,@cbJoinCondition,@cbMemberOrder,@lExperience,@cbMatchDistribute,@cbMatchReward,@wRewardCount,@cbMatchRound,@wRoundCount,@cbMatchRule,@strServiceMachine,GETDATE())

	-- ������ʶ
	DECLARE @dwMatchID INT
	SET @dwMatchID=SCOPE_IDENTITY()	
	
	-- �����ж�
	IF NOT EXISTS(SELECT * FROM MatchOption WHERE MatchID=@dwMatchID) 
	BEGIN
		SET @strErrorDescribe=N'�������ݿ��쳣����������ʧ�ܣ����Ժ����ԣ�'
		RETURN 1
	END
	
	-- ��ѯ����	
	SELECT * FROM MatchOption WHERE MatchID=@dwMatchID	
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �޸ı���
CREATE  PROCEDURE dbo.GSP_GS_ModifyMatchOption
	-- ������Ϣ
	@wKindID INT,								-- ���ͱ�ʶ
	@dwMatchID INT,								-- ������ʶ
	@dwGroupID BIGINT,							-- ��ݱ�ʶ
	@strModuleName NVARCHAR(32),				-- ģ������	
	@strMatchName NVARCHAR(32),					-- ��������
	@strMatchDate NVARCHAR(64),					-- ����ʱ��
	@cbShowType TINYINT,						-- չʾ����
	@cbMatchType TINYINT,						-- ��������
	
	-- ������Ϣ
	@wFeeMoneyID SMALLINT,						-- ��������	
	@lFeeAmount BIGINT,							-- ���ý��
	@wUseTicketCount SMALLINT,					-- ʹ������
	@wBuyTicketCount SMALLINT,					-- ��������
	@wBuyTicketOption SMALLINT,					-- ��Ʊѡ��
	@lBuyTicketAmount BIGINT,					-- ���ҽ��
	@wBuyTicketMoneyID SMALLINT,				-- ��������
	@wSignupCount SMALLINT,						-- ��������
	@cbSignupMode TINYINT,						-- ������ʽ
	@cbJoinCondition TINYINT,					-- ��������
	@cbMemberOrder TINYINT,						-- ��Ա�ȼ�	
	@lExperience INT,							-- ����ȼ�	
	
	-- ��������
	@cbMatchDistribute VARBINARY(64),			-- ��������

	-- ��������
	@wRewardCount SMALLINT,						-- ������Ŀ
	@cbMatchReward VARBINARY(MAX),				-- ��������

	-- �����ִ�
	@wRoundCount SMALLINT,						-- �ִ���Ŀ
	@cbMatchRound VARBINARY(512),				-- �����ִ�

	-- ��������
	@cbMatchRule  VARBINARY(256),				-- ��������	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��������
	UPDATE MatchOption 	SET KindID=@wKindID,GroupID=@dwGroupID,MatchName=@strMatchName,ModuleName=@strModuleName,MatchType=@cbMatchType,ShowType=@cbShowType,FeeAmount=@lFeeAmount,FeeMoneyID=@wFeeMoneyID,
		UseTicketCount=@wUseTicketCount,BuyTicketCount=@wBuyTicketCount,BuyTicketOption=@wBuyTicketOption,BuyTicketAmount=@lBuyTicketAmount,BuyTicketMoneyID=@wBuyTicketMoneyID,SignupCount=@wSignupCount,SignupMode=@cbSignupMode,
		JoinCondition=@cbJoinCondition, MemberOrder=@cbMemberOrder,Experience=@lExperience,MatchDistribute=@cbMatchDistribute,MatchReward=@cbMatchReward,RewardCount=@wRewardCount,MatchRound=@cbMatchRound,RoundCount=@wRoundCount,
		MatchRule=@cbMatchRule	WHERE MatchID=@dwMatchID	
	
	-- ��ѯ����	
	SELECT * FROM MatchOption WHERE MatchID=@dwMatchID		
			
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
