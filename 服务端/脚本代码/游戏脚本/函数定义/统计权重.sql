
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LHF_SumTimesWeight]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[LHF_SumTimesWeight]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- Ȩ��ֵ���
CREATE FUNCTION [dbo].[LHF_SumTimesWeight]
(	
	@KindID	 SMALLINT,		-- ��Ϸ��ʶ
	@TypeID  TINYINT,		-- ���ͱ�ʶ	
	@TimesKind TINYINT		-- �������
)
RETURNS INT
AS
BEGIN
	
	-- ����Ȩ�غ�
	DECLARE @TotalValue INT
	SELECT @TotalValue=SUM(WeightValue) FROM GameConfigWeight(NOLOCK) WHERE KindID=@KindID AND TypeID=@TypeID AND TimesKind=@TimesKind	

	-- ����Ȩ��
	IF @TotalValue IS NULL OR @TotalValue=0 SET @TotalValue=1

	RETURN @TotalValue

END

GO

----------------------------------------------------------------------------------------------------