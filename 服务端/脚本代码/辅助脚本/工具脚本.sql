SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[OutputTable]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[OutputTable]
GO

CREATE PROCEDURE [dbo].[OutputTable] @tablename SYSNAME
AS
  BEGIN
      SET nocount ON

      DECLARE @sqlstr VARCHAR(max)
      DECLARE @sqlstr1 VARCHAR(max)
      DECLARE @sqlstr2 VARCHAR(max)

      SELECT @sqlstr = 'SELECT ''INSERT ' + @tablename
      SELECT @sqlstr1 = ''
      SELECT @sqlstr2 = ' ('
      SELECT @sqlstr1 = ' VALUES ( ''+'
      SELECT @sqlstr1 = @sqlstr1 + col + '+'',''+',
             @sqlstr2 = @sqlstr2 + name + ','
      FROM   (SELECT CASE
                       -- when a.xtype =173 then 'case when '+a.name+' is null then ''NULL'' else '+'convert(varchar('+convert(varchar(4),a.length*2+2)+'),'+a.name +')'+' end'  
                       WHEN a.xtype = 104 THEN 'case when ' + a.name + ' is null then ''NULL'' else ' + 'convert(varchar(1),' + a.name + ')' + ' end'
                       WHEN a.xtype = 175 THEN 'case when ' + a.name + ' is null then ''NULL'' else ' + '''''''''+' + 'replace(' + a.name + ','''''''','''''''''''')' + '+''''''''' + ' end'
                       WHEN a.xtype = 61  THEN 'case when ' + a.name + ' is null then ''NULL'' else ' + '''''''''+' + 'convert(varchar(23),' + a.name + ',121)' + '+''''''''' + ' end'
                       WHEN a.xtype = 106 THEN 'case when ' + a.name + ' is null then ''NULL'' else ' + 'convert(varchar(' + CONVERT(VARCHAR(4), a.xprec+2) + '),' + a.name + ')' + ' end'
                       WHEN a.xtype = 62  THEN 'case when ' + a.name + ' is null then ''NULL'' else ' + 'convert(varchar(23),' + a.name + ',2)' + ' end'
                       WHEN a.xtype = 56  THEN 'case when ' + a.name + ' is null then ''NULL'' else ' + 'convert(varchar(11),' + a.name + ')' + ' end'
					   WHEN a.xtype = 127 THEN 'case when ' + a.name + ' is null then ''NULL'' else ' + 'convert(varchar(21),' + a.name + ')' + ' end'
                       WHEN a.xtype = 60  THEN 'case when ' + a.name + ' is null then ''NULL'' else ' + 'convert(varchar(22),' + a.name + ')' + ' end'
                       WHEN a.xtype = 239 THEN 'case when ' + a.name + ' is null then ''NULL'' else ' + '''''''''+' + 'replace(' + a.name + ','''''''','''''''''''')' + '+''''''''' + ' end'
                       WHEN a.xtype = 108 THEN 'case when ' + a.name + ' is null then ''NULL'' else ' + 'convert(varchar(' + CONVERT(VARCHAR(4), a.xprec+2) + '),' + a.name + ')' + ' end'
                       WHEN a.xtype = 231 THEN 'case when ' + a.name + ' is null then ''NULL'' else ' + '''''''''+' + 'replace(' + a.name + ','''''''','''''''''''')' + '+''''''''' + ' end'
                       WHEN a.xtype = 59  THEN 'case when ' + a.name + ' is null then ''NULL'' else ' + 'convert(varchar(23),' + a.name + ',2)' + ' end'
                       WHEN a.xtype = 58  THEN 'case when ' + a.name + ' is null then ''NULL'' else ' + '''''''''+' + 'convert(varchar(23),' + a.name + ',121)' + '+''''''''' + ' end'
                       WHEN a.xtype = 52  THEN 'case when ' + a.name + ' is null then ''NULL'' else ' + 'convert(varchar(12),' + a.name + ')' + ' end'
                       WHEN a.xtype = 122 THEN 'case when ' + a.name + ' is null then ''NULL'' else ' + 'convert(varchar(22),' + a.name + ')' + ' end'
                       WHEN a.xtype = 48  THEN 'case when ' + a.name + ' is null then ''NULL'' else ' + 'convert(varchar(6),' + a.name + ')' + ' end'
                       -- when a.xtype =165 then 'case when '+a.name+' is null then ''NULL'' else '+'convert(varchar('+convert(varchar(4),a.length*2+2)+'),'+a.name +')'+' end'  
                       WHEN a.xtype = 167 THEN 'case when ' + a.name + ' is null then ''NULL'' else ' + '''''''''+' + 'replace(' + a.name + ','''''''','''''''''''')' + '+''''''''' + ' end'
                       ELSE '''NULL'''
                     END AS col,
                     a.colid,
                     a.name
              FROM   syscolumns a
              WHERE  a.id = Object_id(@tablename)
                     AND a.xtype <> 189
                     AND a.xtype <> 34
                     AND a.xtype <> 35
                     AND a.xtype <> 36)t
      ORDER  BY colid

      SELECT @sqlstr = @sqlstr + LEFT(@sqlstr2, Len(@sqlstr2)-1)
                       + ') ' + LEFT(@sqlstr1, Len(@sqlstr1)-3)
                       + ')'' FROM ' + @tablename

      -- print @sqlstr  
      EXEC( @sqlstr)

      SET nocount OFF
  END