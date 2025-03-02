
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DeleteRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DeleteRecord]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 删除记录
CREATE PROC GSP_GR_DeleteRecord
	-- 用户信息
	@dwUserID INT,								-- 用户 I D
	@dwClubID INT,								-- 茶馆标识
	@dwRecordID INT,							-- 记录标识
	@cbRecordKind TINYINT,						-- 记录类型	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT

-- 变量定义
DECLARE	@TableID	INT;										
DECLARE @Sqlstr		NVARCHAR(512);									
DECLARE	@TableName	NVARCHAR(128);

-- 执行逻辑
BEGIN
	-- 变量定义
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- 查询用户
	SELECT @UserID=a.UserID, @Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
	FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) a LEFT JOIN LHAccountsDB.dbo.AccountsStatus(NOLOCK) b ON a.UserID=b.UserID WHERE a.UserID=@dwUserID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1021,"Msg":"您的帐号不存在或者密码输入有误，请查证后再次尝试！"}'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1022,"Msg":"您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"}'
		RETURN 2
	END	

	-- 帐号关闭
	IF @ShutDown<>0
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1023,"Msg":"您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"}'
		RETURN 3
	END	
	
	-- 查询茶社
	IF @dwClubID<>0
	BEGIN
		DECLARE @CreaterID INT
		SELECT @CreaterID=CreaterID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

		-- 结果判断
		IF @CreaterID IS NULL
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'
			RETURN 5		
		END

		-- 校验馆主标识
		IF @CreaterID<>@dwUserID
		BEGIN

			-- 分表标识
			--SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

			---- 群组成员表
			--SET	@TableName='ClubMember_';
			--SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

			---- 查询成员类型
			--DECLARE @OUT_OperaterType SMALLINT	
			--DECLARE @OUT_OperaterRight SMALLINT	
			--SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType,@ManageRight=ManageRight FROM ',@TableName,'(NOLOCK)',
			--					 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwUserID))				  
	
			---- 执行语句
			--EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT,@ManageRight INT OUT',@MemberType=@OUT_OperaterType OUTPUT,@ManageRight=@OUT_OperaterRight OUTPUT

			---- 调整变量
			--IF @OUT_OperaterType IS NULL SET @OUT_OperaterType=0

			---- 校验权限
			--IF @OUT_OperaterType<3 OR (@OUT_OperaterType=3 AND (@OUT_OperaterRight&0x0002)=0)
			--BEGIN
			--	SET @strErrorDescribe=N'{"Idx":1180,"Msg":"抱歉，您没有权限操作约战玩法！"}'
			--	RETURN 6
			--END		
			
			SET @strErrorDescribe=N''
			RETURN 6
		END
	END

	-- 删除成员变更记录
	IF @cbRecordKind=1
	BEGIN	
		
		-- 分表标识				
		SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

		-- 群组成员表
		SET	@TableName='ClubMemberChange_';
		SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	
		
		-- 清理所有记录
		IF @dwRecordID=0
		BEGIN
			-- 查询分数		
			SET	@Sqlstr = CONCAT( ' DELETE ',@TableName,' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID))

			-- 执行脚本
			EXECUTE(@Sqlstr)			
		END ELSE
		BEGIN

			-- 查询分数		
			SET	@Sqlstr = CONCAT( ' DELETE ',@TableName,' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND RecordID=',CONVERT(NVARCHAR(18), @dwRecordID))

			-- 执行脚本
			EXECUTE(@Sqlstr)
		END		
	END	

	-- 删除成员通知记录
	IF @cbRecordKind=2
	BEGIN
		IF @dwRecordID=0
		BEGIN
			DELETE RecordUserNotice WHERE UserID=@dwUserID
		END ELSE
		BEGIN
			DELETE RecordUserNotice WHERE RecordID=@dwRecordID
		END
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------