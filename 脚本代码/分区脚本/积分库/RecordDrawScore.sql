
--------------------------------------------------------------------------------------------------------------------------------------

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GetFileLocation]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[GetFileLocation]
GO

-- ��ȡ�ļ�·��
CREATE FUNCTION [dbo].[GetFileLocation](@PortNumber SMALLINT)
RETURNS NVARCHAR(50)    --����ֵ����
AS
BEGIN
	RETURN CASE (@PortNumber%2)
			 WHEN 0 THEN 'E:\���ݿ�\Ϻ�׻���\LHGameScoreDB\'
			 WHEN 1 THEN 'D:\���ݿ�\Ϻ�׻���\LHGameScoreDB\'
			 ELSE 'D:\���ݿ�\Ϻ�׻���\LHGameScoreDB\'
		   END
END

GO
----------------------------------------------------------------------------------------------------------------------------------------

-- ��������Ϣ
DECLARE @TableName NVARCHAR(50)
DECLARE @ColumnName NVARCHAR(50)
DECLARE @DataBaseName NVARCHAR(50)

-- ������洢��Ϣ
DECLARE @FileSize NVARCHAR(50)
DECLARE @FileGrowth NVARCHAR(50)

-- ��������
DECLARE @Index INT
DECLARE @sql NVARCHAR(MAX)
DECLARE @PartCount INT
DECLARE @PartNumber NVARCHAR(50)

-- ��������Ϣ
SET @ColumnName='RecordDate'
SET @TableName='RecordDrawScore'
SET @DataBaseName='LHGameScoreDB'

-- �洢��Ϣ
SET @FileSize = '30MB'
SET @FileGrowth = '10%'

-- ���ñ���
SET @PartCount=12
SET @PartNumber=''

------------------------------------------------------------�����ļ���-----------------------------------------------------------------
PRINT CHAR(13)+'�����ļ���'

SET @Index=1
WHILE @Index<=@PartCount
BEGIN
    SET @PartNumber=RIGHT('0'+CONVERT(NVARCHAR,@Index),2)
    SET @sql='ALTER DATABASE ['+@DataBaseName +']
			 ADD FILEGROUP [FG_'+@TableName+'_'+@ColumnName+'_'+@PartNumber+']'
    PRINT @sql+CHAR(13)
	EXECUTE(@sql);
    SET @Index=@Index+1
END

------------------------------------------------------------�����ļ���-----------------------------------------------------------------


--------------------------------------------------------------�����ļ�-----------------------------------------------------------------
PRINT CHAR(13)+'�����ļ�'

SET @Index=1
WHILE @Index<=@PartCount
BEGIN
    SET @PartNumber=RIGHT('0'+CONVERT(NVARCHAR,@Index),2)
    SET @sql='ALTER DATABASE ['+@DataBaseName +']	ADD FILE
			 (NAME = N''FG_'+@TableName+'_'+@ColumnName+'_'+@PartNumber+'_data'',FILENAME = N'''+dbo.GetFileLocation(@Index)+'FG_'+@TableName+'_'+@ColumnName+'_'+@PartNumber+'_data.ndf'',
			 SIZE = '+@FileSize+', FILEGROWTH = '+@FileGrowth+' ) TO FILEGROUP [FG_'+@TableName+'_'+@ColumnName+'_'+@PartNumber+'];'
    PRINT @sql + CHAR(13)
	EXECUTE(@sql);
    SET @Index=@Index+1
END

--------------------------------------------------------------�����ļ�-----------------------------------------------------------------


--------------------------------------------------------------������������-----------------------------------------------------------------
PRINT CHAR(13)+'������������'
DECLARE @FunValueStr NVARCHAR(MAX) 

--SET @Index = 1
--SET @FunValueStr = ''
--WHILE @Index < @PartCount
--BEGIN
--    SET @FunValueStr = @FunValueStr + convert(NVARCHAR(50),(@Index*@FunValue)) + ','
--    SET @Index=@Index+1
--END
--SET @FunValueStr = substring(@FunValueStr,1,len(@FunValueStr)-1)

SET @FunValueStr='''2019-06-01 00:00:00'',''2019-07-01 00:00:00'',''2019-08-01 00:00:00'',''2019-09-01 00:00:00'',''2019-10-01 00:00:00'',''2019-11-01 00:00:00'',''2019-12-01 00:00:00'',
				  ''2020-01-01 00:00:00'',''2020-02-01 00:00:00'',''2020-03-01 00:00:00'',''2020-04-01 00:00:00'''

SET @sql= 'USE '+@DataBaseName+' '
SET @sql+='CREATE PARTITION FUNCTION Fun_'+@TableName+'_'+@ColumnName+'(DATETIME) AS RANGE RIGHT FOR VALUES('+@FunValueStr+')'
PRINT @sql + CHAR(13)
EXECUTE(@sql);

--------------------------------------------------------------������������-----------------------------------------------------------------


--------------------------------------------------------------������������-----------------------------------------------------------------
PRINT CHAR(13)+'������������'
DECLARE @FileGroupStr NVARCHAR(MAX) 

SET @Index=1
SET @FileGroupStr=''
WHILE @Index<=@PartCount
BEGIN
    SET @PartNumber=RIGHT('0'+CONVERT(NVARCHAR,@Index),2)
    SET @FileGroupStr=@FileGroupStr+'[FG_'+@TableName+'_'+@ColumnName+'_'+@PartNumber+'],'
    SET @Index=@Index+1
END

SET @FileGroupStr = SUBSTRING(@FileGroupStr,1,LEN(@FileGroupStr)-1)

SET @sql= 'USE '+@DataBaseName+' '
SET @sql+='CREATE PARTITION SCHEME Sch_'+@TableName+'_'+@ColumnName+' AS PARTITION Fun_'+@TableName+'_'+@ColumnName+' 
           TO('+@FileGroupStr+')'
PRINT @sql + CHAR(13)
EXECUTE(@sql);

--------------------------------------------------------------������������-----------------------------------------------------------------

--------------------------------------------------------------����������-----------------------------------------------------------------
DECLARE @PartScheme NVARCHAR(60) 
SET @PartScheme='Sch_'+@TableName+'_'+@ColumnName+'('+@ColumnName+')'

-- ����������
SET @sql= 'USE '+@DataBaseName+' '
SET @sql+='CREATE TABLE '+@TableName+'
		(
		   [RecordID] [bigint] NOT NULL,
		   [UserID] [int] NOT NULL,
		   [ChairID] [smallint] NOT NULL,
		   [Score] [bigint] NOT NULL,
		   [Revenue] [bigint] NOT NULL,
		   [VideoNumber] [NVARCHAR](21) NOT NULL,
		   [RecordDate] [datetime] NOT NULL,
		)
		ON '+@PartScheme

PRINT @sql + CHAR(13)
EXECUTE(@sql);	

-- ����������RecordID+RecordDate��
SET @sql= 'USE '+@DataBaseName+' '
SET @sql+='CREATE UNIQUE NONCLUSTERED INDEX UIX_'+@TableName+'_RecordID ON '+@TableName+'
		 (
			RecordID ASC,
			UserID ASC,
			RecordDate ASC
		  ) ON '+@PartScheme

PRINT @sql + CHAR(13)
EXECUTE(@sql);

--------------------------------------------------------------����������-----------------------------------------------------------------

-----------------------------------------------------------------------------------------------------------------------------------------

-- ɾ������
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GetFileLocation]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[GetFileLocation]
GO

-----------------------------------------------------------------------------------------------------------------------------------------
