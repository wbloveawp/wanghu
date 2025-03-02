
----------------------------------------------------------------------------------------------------

USE LHPlatformDB
GO


IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadMarqueeNoticeOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadMarqueeNoticeOption]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- 加载公告选项
CREATE PROC GSP_LoadMarqueeNoticeOption
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN	
	
	-- 变量定义
	DECLARE @NoticeRollOption NVARCHAR(MAX)
	DECLARE @NoticeNotifyOption NVARCHAR(MAX)
	DECLARE @MarqueeNoticeOption NVARCHAR(MAX)	

	-- 查询配置
	SELECT @NoticeRollOption=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusKind=N'MarqueeOption' AND StatusName=N'RollOption'
	SELECT @NoticeNotifyOption=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusKind=N'MarqueeOption' AND StatusName=N'NotifyOption'
	
	-- 设置变量
	SET @MarqueeNoticeOption=N'{}'
	SET @MarqueeNoticeOption=JSON_MODIFY(@MarqueeNoticeOption,N'$.RollOption',JSON_QUERY(@NoticeRollOption,N'$'))
	SET @MarqueeNoticeOption=JSON_MODIFY(@MarqueeNoticeOption,N'$.NotifyOption',JSON_QUERY(@NoticeNotifyOption,N'$'))

	SELECT @MarqueeNoticeOption AS MarqueeNoticeOption

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
