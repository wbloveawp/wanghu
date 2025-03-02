
USE master
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'LHAccountsDB')
DROP DATABASE [LHAccountsDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'LHGameGoldDB')
DROP DATABASE [LHGameGoldDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'LHGameScoreDB')
DROP DATABASE [LHGameScoreDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'LHGameBattleDB')
DROP DATABASE [LHGameBattleDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'LHBattleRecordDB')
DROP DATABASE [LHBattleRecordDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'LHGameMatchDB')
DROP DATABASE [LHGameMatchDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'LHGameRankingDB')
DROP DATABASE [LHGameRankingDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'LHGameRecordDB')
DROP DATABASE [LHGameRecordDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'LHPlatformDB')
DROP DATABASE [LHPlatformDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'LHInsureDB')
DROP DATABASE [LHInsureDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'LHNativeWebDB')
DROP DATABASE [LHNativeWebDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'LHWebAdminDB')
DROP DATABASE [LHWebAdminDB]
GO