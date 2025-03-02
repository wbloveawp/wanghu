
----------------------------------------------------------------------------------------------------

USE LHGameBattleDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[LHF_GetTableID]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[LHF_GetTableID]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_AddMember]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_AddMember]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DeleteMember]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DeleteMember]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryClubMember]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryClubMember]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryMemberInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryMemberInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UpdateMemberType]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UpdateMemberType]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UpdateMemberStatus]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UpdateMemberStatus]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UpdateMemberBattleCount]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UpdateMemberBattleCount]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

CREATE FUNCTION [dbo].[LHF_GetTableID]
(
   @dwClubID INT		--群组标识    
)
RETURNS INT
AS
BEGIN
    RETURN @dwClubID/20000
END

GO

----------------------------------------------------------------------------------------------------

-- 添加成员
CREATE  PROCEDURE dbo.GSP_GR_AddMember
	--成员信息
	@dwClubID INT,								-- 群组标识
	@dwMemberID INT,							-- 成员标识
	@cbMemberType TINYINT,						-- 成员类型
	@dwOperaterID INT,							-- 操作者标识
	
	-- 输出参数
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 变量定义
DECLARE	@TableID	INT;										
DECLARE @Sqlstr		 NVARCHAR(1024);
DECLARE	@TableMember NVARCHAR(128);								
DECLARE @TableChange NVARCHAR(128);								

-- 设置属性
SET NOCOUNT ON
SET XACT_ABORT ON

-- 执行逻辑
BEGIN

	-- 查询群主
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID FROM ClubProperty WHERE ClubID=@dwClubID

	-- 结果判断
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'
		RETURN 1		
	END

	-- 查询成员信息
	DECLARE	@MemberGameID INT
	DECLARE @MemberNickName NVARCHAR(32)
	SELECT @MemberGameID=GameID,@MemberNickName=NickName FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwMemberID

	-- 操作者信息
	DECLARE	@OperaterGameID INT
	SELECT @OperaterGameID=GameID FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwOperaterID

	-- 调整变量
	IF @OperaterGameID IS NULL SET @OperaterGameID=0 

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 获取人数
	DECLARE @wMemberCount SMALLINT
	DECLARE @wMaxMemberCount SMALLINT
	SELECT @wMemberCount=MemberCount, @wMaxMemberCount=MaxMemberCount FROM ClubProperty WHERE ClubID=@dwClubID

	-- 人数校验
	IF @wMemberCount IS NULL OR @wMemberCount=@wMaxMemberCount
	BEGIN
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		RETURN 2
	END	

	-- 读取群组列表
	DECLARE @strClubIDArray NVARCHAR(1024)
	SELECT @strClubIDArray=ClubIDArray FROM ClubUserInfo WHERE UserID=@dwMemberID
	IF @strClubIDArray IS NULL 
	BEGIN
		SET @strClubIDArray=';'
		INSERT INTO ClubUserInfo VALUES (@dwMemberID,@strClubIDArray)
	END

	-- 查找标识
	DECLARE @strClubID NVARCHAR(20)
	SET @strClubID=CONCAT(';',CONVERT(NVARCHAR(18),@dwClubID),';');	
	IF CHARINDEX(@strClubID,@strClubIDArray)>0
	BEGIN		
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		RETURN 3
	END	

	-- 分表标识
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- 群组成员表
	SET	@TableMember='ClubMember_';
	SET	@TableMember= CONCAT(@TableMember, CONVERT(NVARCHAR(8), @TableID));		

	-- 创建数据表
	IF (NOT EXISTS(SELECT 1 FROM dbo.sysobjects WHERE xtype='u' AND name=@TableMember))
	BEGIN
		SET	@Sqlstr = CONCAT(' USE LHGameBattleDB ',
							'  CREATE TABLE  ', @TableMember,
							' ( ClubID 	INT   NOT NULL, '  ,
							'	MemberID 	INT   NOT NULL , ',
							'	MemberType	TINYINT DEFAULT 0  NOT NULL, ',							
							'	MemberStatus TINYINT DEFAULT 0  NOT NULL, ',
							'	ManageRight	INT DEFAULT 0  NOT NULL, ',																
							'	ClubScore  INT DEFAULT 0  NOT NULL, ',								
							'	CreateCount	INT DEFAULT 0  NOT NULL, ',
							'	BattleCount	INT DEFAULT 0  NOT NULL, ',
							'	RevenueCount  INT DEFAULT 0  NOT NULL, ',											
							'	JoinDateTime DATETIME DEFAULT GETDATE() NOT NULL , ',
							'	LastBattleTime DATETIME DEFAULT GETDATE() NOT NULL , ',
							'	PRIMARY KEY(ClubID, MemberID)',
							'  );');			
		EXECUTE(@Sqlstr);
		IF @@ERROR<>0
		BEGIN
			-- 事务回滚
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
			RETURN 4
		END
	END	

	-- 插入数据
	SET	@Sqlstr = CONCAT( '  INSERT INTO ', @TableMember,'(ClubID, MemberID, MemberType) ' ,
						  '  VALUES(' ,CONVERT(NVARCHAR(18), @dwClubID) , ',' ,CONVERT(NVARCHAR(12), @dwMemberID), ',',CONVERT(NVARCHAR(3), @cbMemberType),')');		
	EXECUTE(@Sqlstr);
	IF @@ERROR<>0
	BEGIN
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		RETURN 5
	END

	-- 构造表名
	SET	@TableChange = 'ClubMemberChange_';							
	SET	@TableChange = concat(@TableChange,	CONVERT(NVARCHAR(8), @TableID));

	-- 创建变更表
	IF (NOT EXISTS(SELECT 1 FROM LHGameBattleDB.dbo.sysobjects WHERE xtype='u' AND name=@TableChange))
	BEGIN
		SET @Sqlstr = CONCAT(' USE LHGameBattleDB ',
							'  CREATE TABLE ', @TableChange,
							' ( ReocrdID BIGINT IDENTITY(1,1) PRIMARY KEY,',
							'   ChangeType TINYINT NOT NULL, ',
							'	ClubID		INT NOT NULL,',
							'	OperaterID  INT NOT NULL,',
							'	OperaterType TINYINT NOT NULL,',
							'	OperaterGameID INT NOT NULL,',							
							'	MemberID	INT NOT NULL,',
							'	MemberType	TINYINT NOT NULL,',
							'	MemberGameID INT NOT NULL,',
							'	MemberScore	BIGINT NOT NULL,',
							'	MemberNickName  NVARCHAR(32) NOT NULL,',
							'	JoinDateTime DATETIME DEFAULT GETDATE() NOT NULL,',														
							'	ChangeDateTime DATETIME DEFAULT GETDATE() NOT NULL,',							
							'	);',
							'CREATE NONCLUSTERED INDEX IDX_GID_UID ON ',@TableChange, '(ClubID,MemberID)');
		EXECUTE(@Sqlstr);
		IF @@ERROR<>0  RETURN 6
	END

	-- 变更记录
	SET	@Sqlstr = CONCAT('  USE LHGameBattleDB ',
						 '  INSERT INTO ',@TableChange,
						 '  (ChangeType, ClubID, OperaterID, OperaterType, OperaterGameID, MemberID,MemberType,MemberGameID,MemberScore,MemberNickName,JoinDateTime)  ',
						 '  SELECT  1,',CONVERT(NVARCHAR(12), @dwClubID),',',CONVERT(NVARCHAR(12), @dwOperaterID),',0,',CONVERT(NVARCHAR(12), @OperaterGameID),',MemberID,',
						 '	MemberType,',CONVERT(NVARCHAR(12), @MemberGameID),',ClubScore,''',@MemberNickName,''',JoinDateTime FROM ', @TableMember,'(NOLOCK)',
						 '  WHERE ClubID=',CONVERT(NVARCHAR(12), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(12), @dwMemberID)) 
		
	EXECUTE(@Sqlstr)
	IF @@ERROR<>0
	BEGIN
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		RETURN 7
	END

	-- 添加群组标识		
	SET @strClubIDArray=CONCAT(@strClubIDArray,CONVERT(NVARCHAR(18),@dwClubID),';');
	UPDATE ClubUserInfo SET ClubIDArray=@strClubIDArray WHERE UserID=@dwMemberID;

	-- 更新人数	
	UPDATE ClubProperty SET ClubStatus=1,MemberCount=MemberCount+1 WHERE ClubID=@dwClubID

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- 查询脚本		
	SET	@Sqlstr = CONCAT( ' SELECT a.UserID, a.GameID, a.NickName, a.Gender, a.FaceID, a.CustomID, b.*,c.MemberCount '+' FROM LHAccountsDB.dbo.AccountsInfo a, ',@TableMember,' b, ','ClubProperty c',
						  ' WHERE a.UserID=b.MemberID AND c.ClubID=b.ClubID AND ',' b.ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND b.MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))
	EXECUTE(@Sqlstr)

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 删除成员
CREATE  PROCEDURE dbo.GSP_GR_DeleteMember
	-- 用户信息
	@dwUserID INT,								-- 用户标识	
	@cbDeleteKind TINYINT,						-- 删除类型

	--群组信息
	@dwClubID INT,								-- 群组标识
	@dwMemberID INT,							-- 成员标识
	
	-- 输出参数
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON
SET XACT_ABORT ON

-- 变量定义
DECLARE	@TableID	INT;								
DECLARE @Sqlstr		NVARCHAR(1024)											
DECLARE	@TableMember NVARCHAR(128)
DECLARE	@TableChange NVARCHAR(128)

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @UserID	INT
	DECLARE @GameID INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	DECLARE @NickName  NVARCHAR(32)			

	-- 查询用户
	SELECT @UserID=a.UserID,@GameID=a.GameID,@NickName=a.NickName,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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

	-- 查询群主
	DECLARE @CreaterID INT
	DECLARE @PayrollerID INT
	DECLARE @ClubName NVARCHAR(32)
	SELECT @CreaterID=CreaterID,@ClubName=ClubName,@PayrollerID=PayrollerID FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- 结果判断
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'
		RETURN 5		
	END

	-- 清除代付
	IF @PayrollerID=@dwMemberID
	BEGIN
		UPDATE ClubProperty SET PayrollerID=0 WHERE ClubID=@dwClubID
	END

	-- 变量定义
	DECLARE @MemberGameID INT
	DECLARE @OperaterType TINYINT	
	DECLARE @MemberNickName NVARCHAR(32)	

	-- 设置变量
	SELECT @MemberGameID=@GameID,@MemberNickName=@NickName,@OperaterType=0

	-- 分表标识
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- 构造表名
	SET	@TableMember='ClubMember_';
	SET	@TableMember= CONCAT(@TableMember, CONVERT(NVARCHAR(8), @TableID));	

	-- 查询成员类型
	DECLARE @OUT_ClubScore INT
	DECLARE @OUT_MemberType TINYINT
	DECLARE @OUT_OperaterType TINYINT		
	DECLARE @OUT_OperaterRight TINYINT		
	SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType,@ManageRight=ManageRight FROM ',@TableMember,'(NOLOCK)',
						 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwUserID))				  
	
	-- 执行语句
	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT,@ManageRight INT OUT',@MemberType=@OUT_OperaterType OUTPUT,@ManageRight=@OUT_OperaterRight OUTPUT

	-- 不是成员
	IF @OUT_OperaterType IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1170,"Msg":"抱歉，您没有权限踢出该成员！"}'
		RETURN 5
	END

	-- 查询成员信息
	SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType,@ClubScore=ClubScore FROM ',@TableMember,'(NOLOCK)',
						 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))				  
	
	-- 执行语句
	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT, @ClubScore INT OUT',@MemberType=@OUT_MemberType OUTPUT, @ClubScore=@OUT_ClubScore OUTPUT

	-- 成员不存在
	IF @OUT_MemberType IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1171,"Msg":"该成员不存在，或已经踢出了茶社！"}'
		RETURN 6
	END	

	-- 踢出成员
	IF @cbDeleteKind=1
	BEGIN						
		-- 校验积分
		--IF @OUT_ClubScore<>0
		--BEGIN
		--	SET @strErrorDescribe=N'抱歉，该成员身上积分不为0不能踢出！'
		--	RETURN 6				
		--END

		-- 删除自己
		IF @dwUserID=@dwMemberID
		BEGIN
			RETURN 6		   					
		END				

		-- 校验类型和权限
		IF @OUT_OperaterType<3 OR (@OUT_OperaterType=3 AND (@OUT_OperaterRight&0x0010)=0)
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1170,"Msg":"抱歉，您没有权限踢出该成员！"}'
			RETURN 6
		END

		-- 校验类型
		IF @OUT_OperaterType<=@OUT_MemberType	
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1170,"Msg":"抱歉，您没有权限踢出该成员！"}'
			RETURN 6
		END

		-- 查询成员
		SELECT @MemberGameID=GameID, @MemberNickName=NickName FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwMemberID

		-- 调整变量
		IF @MemberGameID IS NULL SET @MemberGameID=0
		IF @MemberNickName IS NULL SET @MemberNickName=N''

		-- 设置变量
		SET @OperaterType=@OUT_OperaterType

		-- 用户通知
		INSERT INTO RecordUserNotice(UserID,NoticeKind,NoticeContent,CollectDateTime) VALUES(@dwMemberID,2,N'{"Idx":1211,"Ops":["'+@ClubName+'"]}',GETDATE())

	END ELSE
	BEGIN
		-- 校验积分
		--IF @OUT_ClubScore<>0
		--BEGIN
		--	SET @strErrorDescribe=N'抱歉，退出茶社需要先清除积分！'
		--	RETURN 6				
		--END

		-- 不是自己
		IF @dwUserID<>@dwMemberID
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'
			RETURN 6		   					
		END

		-- 用户通知
		INSERT INTO RecordUserNotice(UserID,NoticeKind,NoticeContent,CollectDateTime) VALUES(@dwMemberID,1,N'{"Idx":1210,"Ops":["'+@ClubName+'"]}',GETDATE())
	END

	-- 构造表名
	SET	@TableChange = 'ClubMemberChange_';							
	SET	@TableChange = concat(@TableChange,	CONVERT(NVARCHAR(8), @TableID));

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN					

	-- 变更记录
	SET	@Sqlstr = CONCAT('  USE LHGameBattleDB ',
						 '  INSERT INTO ',@TableChange,
						 '  (ChangeType, ClubID, OperaterID, OperaterType, OperaterGameID, MemberID,MemberType,MemberGameID,MemberScore,MemberNickName,JoinDateTime)  ',
						 '  SELECT  2,',CONVERT(NVARCHAR(12), @dwClubID),',',CONVERT(NVARCHAR(12), @dwUserID),',',CONVERT(NVARCHAR(3), @OperaterType),',',CONVERT(NVARCHAR(12), @GameID),',MemberID,',
						 '	MemberType,',CONVERT(NVARCHAR(12), @MemberGameID),',ClubScore,''',@MemberNickName,''',JoinDateTime FROM ', @TableMember,'(NOLOCK)',
						 '  WHERE ClubID=',CONVERT(NVARCHAR(12), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(12), @dwMemberID)) 
		
	EXECUTE(@Sqlstr)
	IF @@ERROR<>0
	BEGIN
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		RETURN 8
	END

	-- 删除成员
	SET	@Sqlstr = CONCAT( ' DELETE ', @TableMember, ' WHERE ' , ' ClubID=', CONVERT(NVARCHAR(18), @dwClubID) , ' AND ' ,' MemberID=', CONVERT(NVARCHAR(12), @dwMemberID)) 
	EXECUTE(@Sqlstr);
	IF @@ERROR<>0
	BEGIN
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
		
		RETURN 9
	END	

	-- 读取群组信息
	DECLARE @strClubIDArray NVARCHAR(1024)
	SELECT @strClubIDArray=ClubIDArray FROM ClubUserInfo WHERE UserID=@dwMemberID

	-- 修改群组信息
	DECLARE @strClubID NVARCHAR(20)		
	SET @strClubID=CONCAT(';',CONVERT(NVARCHAR(18),@dwClubID),';');
	SET @strClubIDArray=REPLACE(@strClubIDArray,@strClubID,';');

	-- 更新群组
	UPDATE ClubUserInfo SET ClubIDArray=@strClubIDArray WHERE UserID=@dwMemberID		

	-- 更新人数
	UPDATE ClubProperty SET MemberCount=MemberCount-1 WHERE ClubID=@dwClubID

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- 抛出信息
	SELECT CreaterID,ClubName,MemberCount FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 查询群组成员
CREATE  PROCEDURE dbo.GSP_GR_QueryClubMember
	@dwClubID INT			--群组标识	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 变量定义
DECLARE	@TableID	INT;																			
DECLARE	@TableName	NVARCHAR(128);								
DECLARE @Sqlstr		NVARCHAR(512);								

-- 执行逻辑
BEGIN
	
	-- 分表标识
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- 群组成员表
	SET	@TableName='ClubMember_';
	SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

	-- 查询数据表
	IF (EXISTS(SELECT 1 FROM LHGameBattleDB.dbo.sysobjects WHERE xtype='u' AND name=@TableName))
	BEGIN
		-- 查询脚本		
		SET	@Sqlstr = CONCAT( ' SELECT a.UserID, a.GameID, a.NickName, a.Gender, a.FaceID, a.CustomID, b.*  FROM LHAccountsDB.dbo.AccountsInfo a, ',@TableName,' b ',
							  ' WHERE a.UserID=b.MemberID AND ',' b.ClubID=',CONVERT(NVARCHAR(18), @dwClubID))
		EXECUTE(@Sqlstr)

	END	

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 查询成员信息
CREATE  PROCEDURE dbo.GSP_GR_QueryMemberInfo
	@dwClubID INT,				--群组标识	
	@strUserIDList NVARCHAR(MAX)--标识列表
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 变量定义
DECLARE	@TableID	INT;																			
DECLARE	@TableName	NVARCHAR(128);								
DECLARE @Sqlstr		NVARCHAR(512);								

-- 执行逻辑
BEGIN
	
	-- 分表标识
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- 群组成员表
	SET	@TableName='ClubMember_';
	SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

	-- 查询数据表
	IF (EXISTS(SELECT 1 FROM LHGameBattleDB.dbo.sysobjects WHERE xtype='u' AND name=@TableName))
	BEGIN
		-- 查询脚本		
		SET	@Sqlstr = CONCAT( ' SELECT * FROM ',@TableName,N' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),N' AND MemberID IN (SELECT VALUE FROM OPENJSON(''',@strUserIDList,'''))')
		EXECUTE(@Sqlstr)
	END	

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 更新成员类型
CREATE  PROCEDURE dbo.GSP_GR_UpdateMemberType
	-- 用户信息
	@dwUserID INT,								-- 用户标识	

	--成员信息
	@dwClubID INT,								-- 群组标识
	@dwMemberID INT,							-- 成员标识
	@cbMemberType TINYINT,						-- 成员类型
	@dwManageRight INT,							-- 管理权限

	-- 输出参数
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 变量定义
DECLARE	@TableID	INT;																			
DECLARE	@TableName	NVARCHAR(128);								
DECLARE @Sqlstr		NVARCHAR(512);								

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @UserID	INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- 查询用户
	SELECT @UserID=a.UserID,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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

	-- 查询群主
	DECLARE @CreaterID INT
	DECLARE @PayrollerID INT
	DECLARE @ClubName NVARCHAR(64)
	SELECT @CreaterID=CreaterID,@PayrollerID=PayrollerID,@ClubName=ClubName FROM ClubProperty(NOLOCK) WHERE ClubID=@dwClubID

	-- 结果判断
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'
		RETURN 5		
	END

	-- 标识判断
	IF @dwUserID=@dwMemberID
	BEGIN
		RETURN 6
	END

	-- 分表标识
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- 群组成员表
	SET	@TableName='ClubMember_';
	SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

	-- 查询成员类型
	DECLARE @OUT_OperaterType SMALLINT
	SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType FROM ',@TableName,'(NOLOCK)',
						 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwUserID))				  
	
	-- 执行语句
	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT',@MemberType=@OUT_OperaterType OUTPUT 

	-- 存在校验
	IF @OUT_OperaterType IS NULL 
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1172,"Msg":"系统未查询到您在该茶社的成员信息，操作失败！"}'
		RETURN 6
	END

	-- 查询成员上级
	DECLARE @OUT_MemberType SMALLINT		
	SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType FROM ',@TableName,'(NOLOCK)',
							' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))				  
	
	-- 执行语句
	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT',@MemberType=@OUT_MemberType OUTPUT

	-- 存在校验
	IF @OUT_MemberType IS NULL 
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1173,"Msg":"系统未查询到该成员在茶社里的信息，操作失败！"}'
		RETURN 5
	END
	
	-- 设置管理员
	IF @cbMemberType=3
	BEGIN
		IF @OUT_OperaterType<>4
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1174,"Msg":"您不是茶社老板，没有权限设置管理员！"}'
			RETURN 7		
		END

		-- 用户通知
		INSERT INTO RecordUserNotice(UserID,NoticeKind,NoticeContent,CollectDateTime) VALUES(@dwMemberID,3,N'{"Idx":1212,"Ops":["'+@ClubName+'"]}',GETDATE())
	END
	
	-- 取消管理员
	IF @cbMemberType=0 AND @OUT_MemberType=3
	BEGIN
		-- 不是老板
		IF @OUT_OperaterType<>4
		BEGIN
			SET @strErrorDescribe=N'{"Idx":1175,"Msg":"您不是茶社老板，没有权限取消管理员！"}'
			RETURN 8	
		END

		-- 代付玩家
		--IF @PayrollerID=@dwMemberID	
		--BEGIN
		--	SET @strErrorDescribe=N'{"Idx":1175,"Msg":"该成员是代付玩家，不能取消管理员！"}'
		--	RETURN 8						
		--END	

		-- 用户通知
		INSERT INTO RecordUserNotice(UserID,NoticeKind,NoticeContent,CollectDateTime) VALUES(@dwMemberID,4,N'{"Idx":1213,"Ops":["'+@ClubName+'"]}',GETDATE())
	END	

	-- 更新类型		
	SET	@Sqlstr = CONCAT( ' UPDATE ',@TableName, 
						  ' SET MemberType=',CONVERT(NVARCHAR(3), @cbMemberType) ,', ManageRight=',CONVERT(NVARCHAR(3), @dwManageRight),							  
						  ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))		
	EXECUTE(@Sqlstr)	

	-- 查询脚本		
	SET	@Sqlstr = CONCAT( 'SELECT * FROM ',@TableName,'(NOLOCK) WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))		
	EXECUTE(@Sqlstr)	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 更新成员状态
CREATE  PROCEDURE dbo.GSP_GR_UpdateMemberStatus
	-- 用户信息
	@dwUserID INT,								-- 用户标识	

	--成员信息
	@dwClubID INT,								-- 群组标识
	@dwMemberID INT,							-- 成员标识
	@cbMemberStatus TINYINT,					-- 成员状态

	-- 输出参数
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 变量定义
DECLARE	@TableID	INT;																			
DECLARE	@TableMember NVARCHAR(128);		
DECLARe @TableChange NVARCHAR(128);		
DECLARE @Sqlstr		NVARCHAR(512);								

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @UserID	INT
	DECLARE @GameID	INT
	DECLARE @Nullity BIT
	DECLARE @ShutDown BIT	
	
	-- 查询用户
	SELECT @UserID=a.UserID,@GameID=a.GameID,@Nullity=a.Nullity, @ShutDown=b.ShutDownStatus
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

	-- 查询群主
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID FROM ClubProperty WHERE ClubID=@dwClubID

	-- 结果判断
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1161,"Msg":"茶社信息不存在，请查证后再次尝试！"}'
		RETURN 5		
	END

	-- 群主自己
	IF @CreaterID = @dwMemberID 
	BEGIN
		RETURN 5		
	END
	
	-- 分表标识
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- 群组成员表
	SET	@TableMember='ClubMember_';
	SET	@TableMember= CONCAT(@TableMember, CONVERT(NVARCHAR(8), @TableID));	

	-- 查询成员类型
	DECLARE @OUT_OperaterType SMALLINT
	DECLARE @OUT_OperaterRight SMALLINT
	SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType,@ManageRight=ManageRight FROM ',@TableMember,'(NOLOCK)',
						 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwUserID))				  
	
	-- 执行语句
	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT,@ManageRight INT OUT',@MemberType=@OUT_OperaterType OUTPUT,@ManageRight=@OUT_OperaterRight OUTPUT

	-- 存在校验
	IF @OUT_OperaterType IS NULL 
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1172,"Msg":"系统未查询到您在该茶社的成员信息，操作失败！"}'
		RETURN 6
	END

	-- 查询成员上级
	DECLARE @OUT_MemberType SMALLINT		
	SET	@Sqlstr = CONCAT(' SELECT @MemberType=MemberType FROM ',@TableMember,'(NOLOCK)',
						 ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))				  
	
	-- 执行语句
	EXEC SP_EXECUTESQL @stmt=@Sqlstr,@params=N'@MemberType TINYINT OUT',@MemberType=@OUT_MemberType OUTPUT

	-- 存在校验
	IF @OUT_MemberType IS NULL 
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1173,"Msg":"系统未查询到该成员在茶社里的信息，操作失败！"}'
		RETURN 7
	END

	-- 校验类型和权限
	IF @OUT_OperaterType<3 OR (@OUT_OperaterType=3 AND (@OUT_OperaterRight&0x0008)=0)
	BEGIN
		SET @strErrorDescribe=N'{"Idx":1176,"Msg":"抱歉，您没有权限修改成员状态！"}'		
		RETURN 6
	END

	-- 变量定义		
	DECLARE @MemberGameID INT
	DECLARE @MemberNickName NVARCHAR(32)

	-- 查询成员
	SELECT @MemberGameID=GameID, @MemberNickName=NickName FROM LHAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwMemberID

	-- 调整变量
	IF @MemberGameID IS NULL SET @MemberGameID=0
	IF @MemberNickName IS NULL SET @MemberNickName=N''

	-- 操作类型
	DECLARE @OperateType TINYINT=3
	IF @cbMemberStatus=0 SET @OperateType=4

	-- 构造表名
	SET	@TableChange = 'ClubMemberChange_';							
	SET	@TableChange = concat(@TableChange,	CONVERT(NVARCHAR(8), @TableID));		

	-- 变更记录
	SET	@Sqlstr = CONCAT('  USE LHGameBattleDB ',
						 '  INSERT INTO ',@TableChange,
						 '  (ChangeType, ClubID, OperaterID, OperaterType, OperaterGameID, MemberID,MemberType,MemberGameID,MemberScore,MemberNickName,JoinDateTime)  ',
						 '  SELECT  '+CONVERT(NVARCHAR(4), @OperateType)+',',CONVERT(NVARCHAR(12), @dwClubID),',',CONVERT(NVARCHAR(12), @dwUserID),',',CONVERT(NVARCHAR(3), @OUT_OperaterType),',',CONVERT(NVARCHAR(12), @GameID),',MemberID,',
						 '	MemberType,',CONVERT(NVARCHAR(12), @MemberGameID),',ClubScore,''',@MemberNickName,''',JoinDateTime FROM ', @TableMember,'(NOLOCK)',
						 '  WHERE ClubID=',CONVERT(NVARCHAR(12), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(12), @dwMemberID)) 
		
	EXECUTE(@Sqlstr)
	IF @@ERROR<>0
	BEGIN
		RETURN 7
	END

	-- 更新脚本		
	SET	@Sqlstr = CONCAT( ' UPDATE ',@TableMember,' SET MemberStatus=',CONVERT(NVARCHAR(3), @cbMemberStatus) ,							  
						  ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))		
	EXECUTE(@Sqlstr)

	-- 查询脚本		
	SET	@Sqlstr = CONCAT( 'SELECT * FROM ',@TableMember,'(NOLOCK) WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))		
	EXECUTE(@Sqlstr)	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 更新约战次数
CREATE  PROCEDURE dbo.GSP_GR_UpdateMemberBattleCount
	@dwClubID INT,			   --群组标识	
	@dwMemberID BIGINT,		   --成员标识	
	@wIncCreateCount SMALLINT, --创建次数
	@wIncBattleCount SMALLINT  --参与次数
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 变量定义
DECLARE	@TableID	INT;																			
DECLARE	@TableName	NVARCHAR(128);								
DECLARE @Sqlstr		NVARCHAR(512);								

-- 执行逻辑
BEGIN
	
	-- 分表标识
	SET	@TableID=dbo.LHF_GetTableID(@dwClubID);

	-- 群组成员表
	SET	@TableName='ClubMember_';
	SET	@TableName= CONCAT(@TableName, CONVERT(NVARCHAR(8), @TableID));	

	-- 查询数据表
	IF (EXISTS(SELECT 1 FROM LHGameBattleDB.dbo.sysobjects WHERE xtype='u' AND name=@TableName))
	BEGIN
		-- 更新脚本
		IF @wIncBattleCount>0
		BEGIN		
			SET	@Sqlstr = CONCAT( 'UPDATE ',@TableName, 
								  ' SET CreateCount=CreateCount+',CONVERT(NVARCHAR(5), @wIncCreateCount) ,
								  ',BattleCount=BattleCount+',CONVERT(NVARCHAR(5), @wIncBattleCount),
								  ',LastBattleTime=''',CONVERT(NVARCHAR(24), GetDate(),121),'''',
								  ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))		
		END ELSE
		BEGIN
			SET	@Sqlstr = CONCAT( 'UPDATE ',@TableName, 
								  ' SET CreateCount=CreateCount+',CONVERT(NVARCHAR(5), @wIncCreateCount) ,
								  ',BattleCount=BattleCount+',CONVERT(NVARCHAR(5), @wIncBattleCount),								 
								  ' WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))					
		END

		EXECUTE(@Sqlstr)

		-- 查询脚本		
		SET	@Sqlstr = CONCAT( 'SELECT * FROM ',@TableName,'(NOLOCK) WHERE ClubID=',CONVERT(NVARCHAR(18), @dwClubID),' AND MemberID=',CONVERT(NVARCHAR(18), @dwMemberID))		
		EXECUTE(@Sqlstr)
	END	
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

