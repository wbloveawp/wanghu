USE [LHPlatformDB]
GO
SET IDENTITY_INSERT [dbo].[DataBaseList] ON 

INSERT [dbo].[DataBaseList] ([DBListID], [DBInfoID], [DBName], [DBGenre], [DBAliasName], [Information]) VALUES (1, 1, N'LHAccountsDB', 0, N'LHAccountsDB', N'帐号数据库')
INSERT [dbo].[DataBaseList] ([DBListID], [DBInfoID], [DBName], [DBGenre], [DBAliasName], [Information]) VALUES (2, 1, N'LHGameGoldDB', 1, N'LHGameGoldDB', N'财富数据库')
INSERT [dbo].[DataBaseList] ([DBListID], [DBInfoID], [DBName], [DBGenre], [DBAliasName], [Information]) VALUES (3, 1, N'LHGameRecordDB', 0, N'LHGameRecordDB', N'记录数据库')
INSERT [dbo].[DataBaseList] ([DBListID], [DBInfoID], [DBName], [DBGenre], [DBAliasName], [Information]) VALUES (4, 1, N'LHInsureDB', 0, N'LHInsureDB', N'分站数据库')
INSERT [dbo].[DataBaseList] ([DBListID], [DBInfoID], [DBName], [DBGenre], [DBAliasName], [Information]) VALUES (5, 1, N'LHGameMatchDB', 1, N'LHGameMatchDB', N'比赛数据库')
INSERT [dbo].[DataBaseList] ([DBListID], [DBInfoID], [DBName], [DBGenre], [DBAliasName], [Information]) VALUES (6, 1, N'LHGameBattleDB', 1, N'LHGameBattleDB', N'约战数据库')
INSERT [dbo].[DataBaseList] ([DBListID], [DBInfoID], [DBName], [DBGenre], [DBAliasName], [Information]) VALUES (7, 1, N'LHBattleRecordDB', 0, N'LHBattleRecordDB', N'约战记录库')
INSERT [dbo].[DataBaseList] ([DBListID], [DBInfoID], [DBName], [DBGenre], [DBAliasName], [Information]) VALUES (8, 1, N'LHGameRankingDB', 1, N'LHGameRankingDB', N'排位数据库')
SET IDENTITY_INSERT [dbo].[DataBaseList] OFF
GO
