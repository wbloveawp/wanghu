-- ��������
DECLARE @Sqlstr NVARCHAR(512)
DECLARE @strExcludeTable NVARCHAR(MAX)				 							 	 

 --�˺ſ�
SET @strExcludeTable=N'[
					   {"DBName":"LHAccountsDB","TableList": ["GameIdentifier","SystemStatusInfo"]},
					   {"DBName":"LHGameGoldDB","TableList": ["GameColumnItem"]},
					   {"DBName":"LHGameBattleDB","TableList": ["GameColumnItem"]},
					   {"DBName":"LHGameRankingDB","TableList": ["GameColumnItem"]},
					   {"DBName":"LHGameMatchDB","TableList": ["GameColumnItem"]},
					   {"DBName":"LHGameRecordDB","TableList": [""]}
					   ]'

-- ���������
DECLARE @DeleteTableName TABLE (TableName NVARCHAR(64))
DECLARE @ExcludeTableList TABLE (ID INT identity(1,1),DBName NVARCHAR(64), TableList NVARCHAR(MAX))

-- ����Json
IF ISJSON(@strExcludeTable)>0
BEGIN
	INSERT @ExcludeTableList (DBName,TableList) 
	SELECT JSON_VALUE(VALUE,'$.DBName'), JSON_QUERY(VALUE,'$.TableList') FROM OPENJSON(@strExcludeTable)
END

-- ��������
DECLARE @Index INT
DECLARE @LoopCount INT	
DECLARE @DBName NVARCHAR(64)
DECLARE @TableList NVARCHAR(MAX)

-- ������Ʒ
SELECT @Index=1,@LoopCount=COUNT(1) FROM @ExcludeTableList	
	
-- ѭ������
WHILE @Index<=@LoopCount
BEGIN	
	-- ��ѯ��Ϣ
	SELECT @DBName=DBName,@TableList=TableList FROM @ExcludeTableList WHERE ID=@Index	

	-- ����ű�
	SET	@Sqlstr = CONCAT( N'SELECT Name FROM ',@DBName,N'.dbo.sysobjects WHERE xtype=''U''')			
	INSERT INTO @DeleteTableName(TableName) EXEC(@Sqlstr)	

	--�����α�
	DECLARE MyCursor SCROLL CURSOR
	FOR SELECT TableName FROM @DeleteTableName

	--���α�
	OPEN MyCursor

	--�������
	DECLARE @TableName NVARCHAR(64)

	--��ȡ����
	FETCH NEXT FROM MyCursor INTO @TableName
	WHILE (@@FETCH_STATUS=0)
	BEGIN      
		-- ���˱���
		IF CHARINDEX(N'"'+@TableName+N'"',@TableList)=0
		BEGIN
			-- ����ű�
			SET	@Sqlstr = CONCAT( 'TRUNCATE TABLE ',@DBName,'.dbo.',@TableName)		

			-- ִ�нű�
			EXECUTE(@Sqlstr)	
		END 
	
		FETCH NEXT FROM MyCursor INTO @TableName
	END

	-- �������
	DELETE @DeleteTableName

	--�ر��α�
	CLOSE MyCursor
	DEALLOCATE MyCursor

	-- ��������
	SET @Index=@Index+1
END	
