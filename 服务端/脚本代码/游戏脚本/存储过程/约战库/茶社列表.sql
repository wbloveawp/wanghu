
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[IMF_SplitClubID]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[IMF_SplitClubID]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryClubList]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryClubList]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryClubNameCard]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryClubNameCard]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------
-- ���Ⱥ���ʶ
CREATE FUNCTION [dbo].[IMF_SplitClubID]
(
	@strSource NVARCHAR(1024),		--Դ�ַ���
	@strSeparator CHAR(1)			--�ָ�����
)
RETURNS @tbResult TABLE(ID INT IDENTITY(1,1), ClubID INT)
WITH ENCRYPTION AS  
BEGIN  
   -- ��������	
   DECLARE @dwIndexEnd INT;
   DECLARE @dwIndexStart INT;   
   DECLARE @strClubID NVARCHAR(18);

   -- ��ȡ����
   SET @strSource=RTRIM(LTRIM(@strSource));    
   SET @dwIndexStart=CHARINDEX(@strSeparator,@strSource)+1;
   SET @dwIndexEnd=CHARINDEX(@strSeparator,@strSource,@dwIndexStart);

   WHILE @dwIndexEnd>0
   BEGIN
	  SET @strClubID=SUBSTRING(@strSource,@dwIndexStart,@dwIndexEnd-@dwIndexStart);      
	  INSERT INTO @tbResult VALUES (CONVERT(INT,@strClubID))

	  SET @dwIndexStart=@dwIndexEnd+1;     
      SET @dwIndexEnd=CHARINDEX(@strSeparator,@strSource,@dwIndexStart);
   END 

   RETURN
END

GO


----------------------------------------------------------------------------------------------------

-- ����Ⱥ���б�
CREATE  PROCEDURE dbo.GSP_GR_QueryClubList
	@dwClubStartID INT,			-- Ⱥ���ʶ	
	@dwClubEndID	INT			-- Ⱥ���ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯȺ����Ϣ	
	SELECT A.*, B.*,C.*, 1 AS MemberID, D.FaceID AS CreaterFaceID, D.CustomID AS CreaterCustomID, D.NickName AS CreaterNickName
	FROM ClubProperty AS A, ClubRules AS B, ClubNotice C, LHAccountsDB.dbo.AccountsInfo D
	WHERE A.ClubID=B.ClubID AND a.ClubID=C.ClubID AND (A.ClubStatus=1 AND A.ClubID>=@dwClubStartID AND A.ClubID<=@dwClubEndID) AND A.CreaterID=D.UserID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ѯ������Ƭ
CREATE PROCEDURE dbo.GSP_GR_QueryClubNameCard
	@dwClubID INT
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ��Ϣ
	SELECT a.*,b.FaceID AS CreaterFaceID,b.CustomID AS CreaterCustomID,b.NickName AS CreaterNickName 
	FROM ClubProperty(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsInfo(NOLOCK) b  ON a.CreaterID=b.UserID WHERE a.ClubID=@dwClubID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

