-- 变量定义
DECLARE @Sqlstr NVARCHAR(512)
DECLARE @strExcludeTable NVARCHAR(MAX)				 							 	 

 --账号库
SET @strExcludeTable=N'[
					   {"DBName":"LHAccountsDB","TableList": ["GameIdentifier","SystemStatusInfo"]},
					   {"DBName":"LHGameGoldDB","TableList": ["GameColumnItem"]},
					   {"DBName":"LHGameBattleDB","TableList": ["GameColumnItem"]},
					   {"DBName":"LHGameRankingDB","TableList": ["GameColumnItem"]},
					   {"DBName":"LHGameMatchDB","TableList": ["GameColumnItem"]},
					   {"DBName":"LHGameRecordDB","TableList": [""]}
					   ]'

-- 表变量定义
DECLARE @DeleteTableName TABLE (TableName NVARCHAR(64))
DECLARE @ExcludeTableList TABLE (ID INT identity(1,1),DBName NVARCHAR(64), TableList NVARCHAR(MAX))

-- 解析Json
IF ISJSON(@strExcludeTable)>0
BEGIN
	INSERT @ExcludeTableList (DBName,TableList) 
	SELECT JSON_VALUE(VALUE,'$.DBName'), JSON_QUERY(VALUE,'$.TableList') FROM OPENJSON(@strExcludeTable)
END

-- 变量定义
DECLARE @Index INT
DECLARE @LoopCount INT	
DECLARE @DBName NVARCHAR(64)
DECLARE @TableList NVARCHAR(MAX)

-- 处理物品
SELECT @Index=1,@LoopCount=COUNT(1) FROM @ExcludeTableList	
	
-- 循环处理
WHILE @Index<=@LoopCount
BEGIN	
	-- 查询信息
	SELECT @DBName=DBName,@TableList=TableList FROM @ExcludeTableList WHERE ID=@Index	

	-- 构造脚本
	SET	@Sqlstr = CONCAT( N'SELECT Name FROM ',@DBName,N'.dbo.sysobjects WHERE xtype=''U''')			
	INSERT INTO @DeleteTableName(TableName) EXEC(@Sqlstr)	

	--创建游标
	DECLARE MyCursor SCROLL CURSOR
	FOR SELECT TableName FROM @DeleteTableName

	--打开游标
	OPEN MyCursor

	--定义变量
	DECLARE @TableName NVARCHAR(64)

	--提取表名
	FETCH NEXT FROM MyCursor INTO @TableName
	WHILE (@@FETCH_STATUS=0)
	BEGIN      
		-- 过滤表名
		IF CHARINDEX(N'"'+@TableName+N'"',@TableList)=0
		BEGIN
			-- 构造脚本
			SET	@Sqlstr = CONCAT( 'TRUNCATE TABLE ',@DBName,'.dbo.',@TableName)		

			-- 执行脚本
			EXECUTE(@Sqlstr)	
		END 
	
		FETCH NEXT FROM MyCursor INTO @TableName
	END

	-- 清除数据
	DELETE @DeleteTableName

	--关闭游标
	CLOSE MyCursor
	DEALLOCATE MyCursor

	-- 更新索引
	SET @Index=@Index+1
END	
