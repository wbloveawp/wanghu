
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

-- 权重值求和
CREATE FUNCTION [dbo].[LHF_SumTimesWeight]
(	
	@KindID	 SMALLINT,		-- 游戏标识
	@TypeID  TINYINT,		-- 类型标识	
	@TimesKind TINYINT		-- 倍数标记
)
RETURNS INT
AS
BEGIN
	
	-- 计算权重和
	DECLARE @TotalValue INT
	SELECT @TotalValue=SUM(WeightValue) FROM GameConfigWeight(NOLOCK) WHERE KindID=@KindID AND TypeID=@TypeID AND TimesKind=@TimesKind	

	-- 调整权重
	IF @TotalValue IS NULL OR @TotalValue=0 SET @TotalValue=1

	RETURN @TotalValue

END

GO

----------------------------------------------------------------------------------------------------