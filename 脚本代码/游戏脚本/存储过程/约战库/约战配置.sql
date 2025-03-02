
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadBattleReward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadBattleReward]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadBattleBaseOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadBattleBaseOption]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadBattleOptionItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadBattleOptionItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadBattleGameOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadBattleGameOption]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- ���ؽ���
CREATE  PROCEDURE dbo.GSP_GR_LoadBattleReward
	@wKindID SMALLINT					-- ��Ϸ����
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
		
	-- ��ѯ����
	SELECT * FROM BattleReward WHERE OptionID IN (SELECT OptionID FROM BattleBaseOption WHERE KindID=@wKindID)
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE  PROCEDURE dbo.GSP_GR_LoadBattleOptionItem
	@wOptionID SMALLINT,					-- ���ñ�ʶ
	@wItemCount TINYINT						-- ������Ŀ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
		
	-- ��ѯ����
	SELECT * FROM BattleOptionItem WHERE OptionID=@wOptionID AND ItemIndex<@wItemCount
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ����ѡ��
CREATE  PROCEDURE dbo.GSP_GR_LoadBattleBaseOption	
	@wKindID SMALLINT						-- ���ͱ�ʶ			
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
		
	-- ��ѯ����
	SELECT * FROM BattleBaseOption WHERE (@wKindID=0 OR KindID=@wKindID) AND Nullity=0
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------


-- ��������
CREATE  PROCEDURE dbo.GSP_GR_LoadBattleGameOption	
	@wKindID SMALLINT						-- ���ͱ�ʶ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
		
	-- ��ѯ����
	SELECT * FROM BattleGameOption WHERE (@wKindID=0 OR KindID=@wKindID)
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


