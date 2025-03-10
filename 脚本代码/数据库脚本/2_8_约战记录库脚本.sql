USE [LHBattleRecordDB]
GO
/****** Object:  Table [dbo].[RecordBattleInfo]    Script Date: 2023/7/20 16:19:55 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordBattleInfo](
	[RecordID] [bigint] NOT NULL,
	[MappedNum] [int] NOT NULL,
	[DateID] [int] NOT NULL,
	[BattleFinish] [tinyint] NOT NULL,
	[ClubID] [bigint] NOT NULL,
	[KindID] [smallint] NOT NULL,
	[ServerID] [smallint] NOT NULL,
	[ScoreKind] [tinyint] NOT NULL,
	[OwnerID] [int] NOT NULL,
	[SettleKind] [tinyint] NOT NULL,
	[PlayCount] [smallint] NOT NULL,
	[PlayTime] [int] NOT NULL,
	[CreateTime] [int] NOT NULL,
	[BaseScore] [int] NOT NULL,
	[PayType] [tinyint] NOT NULL,
	[PayUserID] [int] NOT NULL,
	[ConsumeCount] [int] NOT NULL,
	[CurrencyKind] [tinyint] NOT NULL,
	[GameOption] [nvarchar](128) NOT NULL,
	[UserCount] [smallint] NOT NULL,
	[UserScoreString] [nvarchar](4000) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordBattleInfo_RecordID] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordBattleUser]    Script Date: 2023/7/20 16:19:55 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordBattleUser](
	[RecordID] [bigint] NOT NULL,
	[UserID] [int] NOT NULL,
	[DateID] [int] NOT NULL,
	[ClubID] [int] NOT NULL,
	[MappedNum] [int] NOT NULL,
	[CollectDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordCardChange]    Script Date: 2023/7/20 16:19:55 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordCardChange](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[SrcUserID] [int] NOT NULL,
	[SrcGameID] [int] NOT NULL,
	[SrcCard] [int] NOT NULL,
	[ClubID] [int] NOT NULL,
	[DstUserID] [int] NOT NULL,
	[DstGameID] [int] NOT NULL,
	[DstCard] [int] NOT NULL,
	[CardStationID] [int] NOT NULL,
	[TradeCard] [int] NOT NULL,
	[TradeType] [int] NOT NULL,
	[ClientIP] [nvarchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[CollectNote] [nvarchar](200) NOT NULL,
 CONSTRAINT [PK_RecordCardChange] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordCheckIn]    Script Date: 2023/7/20 16:19:55 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordCheckIn](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[GameID] [int] NOT NULL,
	[StationID] [int] NOT NULL,
	[PresentIngot] [bigint] NOT NULL,
	[MachineID] [nvarchar](32) NOT NULL,
	[IPAddress] [nvarchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordCheckIn] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordGrantClubScore]    Script Date: 2023/7/20 16:19:55 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordGrantClubScore](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[StationID] [int] NOT NULL,
	[MasterID] [int] NOT NULL,
	[ClientIP] [nvarchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[UserID] [int] NOT NULL,
	[CurClubScore] [bigint] NOT NULL,
	[AddClubScore] [bigint] NOT NULL,
	[Reason] [nvarchar](200) NOT NULL,
 CONSTRAINT [PK_RecordGrantClubScore] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordGrantIngot]    Script Date: 2023/7/20 16:19:55 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordGrantIngot](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[StationID] [int] NOT NULL,
	[CurIngot] [bigint] NOT NULL,
	[AddIngot] [bigint] NOT NULL,
	[AddAmount] [decimal](18, 2) NOT NULL,
	[MasterID] [int] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[ClientIP] [varchar](15) NOT NULL,
	[Reason] [nvarchar](200) NOT NULL,
 CONSTRAINT [PK_RecordGrantIngot] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordClubCowryChange]    Script Date: 2023/7/20 16:19:55 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordClubCowryChange](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[DateID] [int] NOT NULL,
	[ClubID] [int] NOT NULL,
	[SrcUserID] [int] NOT NULL,
	[SrcGameID] [int] NOT NULL,
	[SrcClubCowry] [bigint] NOT NULL,
	[SrcUserType] [tinyint] NOT NULL,
	[DstUserID] [int] NOT NULL,
	[DstGameID] [int] NOT NULL,
	[DstClubCowry] [bigint] NOT NULL,
	[DstUserType] [tinyint] NOT NULL,
	[TradeType] [int] NOT NULL,
	[TradeClubCowry] [bigint] NOT NULL,
	[ClientIP] [nvarchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[CollectNote] [nvarchar](200) NOT NULL,
 CONSTRAINT [PK_RecordClubCowryChange] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordClubIngotChange]    Script Date: 2023/7/20 16:19:55 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordClubIngotChange](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[ClubID] [bigint] NOT NULL,
	[StationID] [int] NOT NULL,
	[BeforeIngot] [bigint] NOT NULL,
	[TradeType] [tinyint] NOT NULL,
	[TradeIngot] [int] NOT NULL,
	[ClientIP] [nvarchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordClubIngotChange] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordClubRevenue]    Script Date: 2023/7/20 16:19:55 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordClubRevenue](
	[DateID] [int] NOT NULL,
	[ClubID] [bigint] NOT NULL,
	[UserID] [int] NOT NULL,
	[ParentID] [int] NOT NULL,
	[ParentNickname] [nvarchar](32) NOT NULL,
	[ParentGameID] [int] NOT NULL,
	[WinScore] [bigint] NOT NULL,
	[LostScore] [bigint] NOT NULL,
	[Revenue] [bigint] NOT NULL,
	[RevenueRate] [decimal](18, 2) NOT NULL,
	[RealRate] [decimal](18, 2) NOT NULL,
	[LevelID] [int] NOT NULL,
	[IncomeScore] [bigint] NOT NULL,
	[IsBalance] [tinyint] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordClubRevenue] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[ClubID] ASC,
	[UserID] ASC,
	[ParentID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordClubScoreChange]    Script Date: 2023/7/20 16:19:55 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordClubScoreChange](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[DateID] [int] NOT NULL,
	[ClubID] [bigint] NOT NULL,
	[SrcUserID] [int] NOT NULL,
	[SrcGameID] [int] NOT NULL,
	[SrcClubScore] [bigint] NOT NULL,
	[SrcUserType] [tinyint] NOT NULL,
	[DstUserID] [int] NOT NULL,
	[DstGameID] [int] NOT NULL,
	[DstClubScore] [bigint] NOT NULL,
	[DstUserType] [tinyint] NOT NULL,
	[TradeType] [int] NOT NULL,
	[TradeClubScore] [bigint] NOT NULL,
	[BalanceMark] [tinyint] NOT NULL,
	[ClientIP] [nvarchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[CollectNote] [nvarchar](200) NOT NULL,
 CONSTRAINT [PK_RecordClubScoreChange] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordIngotChange]    Script Date: 2023/7/20 16:19:55 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordIngotChange](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[SrcUserID] [int] NOT NULL,
	[SrcGameID] [int] NOT NULL,
	[SrcIngot] [bigint] NOT NULL,
	[DstUserID] [int] NOT NULL,
	[DstGameID] [int] NOT NULL,
	[DstIngot] [int] NOT NULL,
	[StationID] [int] NOT NULL,
	[ClubID] [int] NOT NULL,
	[TradeIngot] [bigint] NOT NULL,
	[TypeID] [int] NOT NULL,
	[ClientIP] [nvarchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[CollectNote] [nvarchar](200) NOT NULL,
 CONSTRAINT [PK_RecordIngotChange] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordPartnerRevenue]    Script Date: 2023/7/20 16:19:55 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordPartnerRevenue](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[DateID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[ClubID] [int] NOT NULL,
	[SupplyUserID] [int] NOT NULL,
	[SupplyRevenue] [int] NOT NULL,
	[ExtractRate] [smallint] NOT NULL,
	[ExtractRevenue] [int] NOT NULL,
	[MappedNum] [int] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordPartnerRevenueDetail] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordVideoInfo]    Script Date: 2023/7/20 16:19:55 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordVideoInfo](
	[VideoNumber] [nvarchar](21) NOT NULL,
	[KindID] [int] NOT NULL,
	[MappedNum] [int] NOT NULL,
	[BattleRecordID] [bigint] NOT NULL,
	[VideoCreateTime] [datetime] NOT NULL,
	[UserCount] [int] NOT NULL,
	[PlayBackCode] [int] NOT NULL,
	[RecordDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordVideoInfo] PRIMARY KEY CLUSTERED 
(
	[VideoNumber] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordVideoUser]    Script Date: 2023/7/20 16:19:55 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordVideoUser](
	[VideoNumber] [nvarchar](21) NOT NULL,
	[UserID] [int] NOT NULL,
	[MappedNum] [int] NOT NULL,
	[RecordDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordVideoUser_1] PRIMARY KEY CLUSTERED 
(
	[VideoNumber] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordBattleInfo_CollectDate]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordBattleInfo_CollectDate] ON [dbo].[RecordBattleInfo]
(
	[CollectDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordBattleInfo_DateID]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordBattleInfo_DateID] ON [dbo].[RecordBattleInfo]
(
	[DateID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordBattleInfo_ClubID]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordBattleInfo_ClubID] ON [dbo].[RecordBattleInfo]
(
	[ClubID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordBattleInfo_KindID]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordBattleInfo_KindID] ON [dbo].[RecordBattleInfo]
(
	[KindID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordBattleUser_DGU]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordBattleUser_DGU] ON [dbo].[RecordBattleUser]
(
	[DateID] ASC,
	[ClubID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordCheckIn_CollectDate]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordCheckIn_CollectDate] ON [dbo].[RecordCheckIn]
(
	[CollectDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON
GO
/****** Object:  Index [IX_RecordCheckIn_MachineID]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordCheckIn_MachineID] ON [dbo].[RecordCheckIn]
(
	[MachineID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordCheckIn_UserID]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordCheckIn_UserID] ON [dbo].[RecordCheckIn]
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordGrantIngot_CollectDate]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordGrantIngot_CollectDate] ON [dbo].[RecordGrantIngot]
(
	[CollectDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordGrantIngot_StationID]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordGrantIngot_StationID] ON [dbo].[RecordGrantIngot]
(
	[StationID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordGrantIngot_UserID]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordGrantIngot_UserID] ON [dbo].[RecordGrantIngot]
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordClubCowryChange_CollectDate]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordClubCowryChange_CollectDate] ON [dbo].[RecordClubCowryChange]
(
	[CollectDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordClubCowryChange_DstUserID]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordClubCowryChange_DstUserID] ON [dbo].[RecordClubCowryChange]
(
	[DstUserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordClubCowryChange_ClubID]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordClubCowryChange_ClubID] ON [dbo].[RecordClubCowryChange]
(
	[ClubID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordIngotChange_SrcUserID]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordIngotChange_SrcUserID] ON [dbo].[RecordIngotChange]
(
	[SrcUserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordPartnerRevenue_CollectDate]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordPartnerRevenue_CollectDate] ON [dbo].[RecordPartnerRevenue]
(
	[CollectDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordPartnerRevenue_DateID]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordPartnerRevenue_DateID] ON [dbo].[RecordPartnerRevenue]
(
	[DateID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordPartnerRevenue_ClubID]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordPartnerRevenue_ClubID] ON [dbo].[RecordPartnerRevenue]
(
	[ClubID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordPartnerRevenue_UserID]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordPartnerRevenue_UserID] ON [dbo].[RecordPartnerRevenue]
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordVideoInfo_MappedNum]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordVideoInfo_MappedNum] ON [dbo].[RecordVideoInfo]
(
	[MappedNum] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordVideoInfo_PlayBackCode]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordVideoInfo_PlayBackCode] ON [dbo].[RecordVideoInfo]
(
	[PlayBackCode] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordVideoInfo_RecordDate]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordVideoInfo_RecordDate] ON [dbo].[RecordVideoInfo]
(
	[RecordDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordVideoInfo_RecordID]    Script Date: 2023/7/20 16:19:55 ******/
CREATE NONCLUSTERED INDEX [IX_RecordVideoInfo_RecordID] ON [dbo].[RecordVideoInfo]
(
	[BattleRecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
ALTER TABLE [dbo].[RecordBattleInfo] ADD  CONSTRAINT [DF_RecordBattleInfo_DateID]  DEFAULT ((0)) FOR [DateID]
GO
ALTER TABLE [dbo].[RecordBattleInfo] ADD  CONSTRAINT [DF_RecordBattleInfo_BattleFinish]  DEFAULT ((0)) FOR [BattleFinish]
GO
ALTER TABLE [dbo].[RecordBattleInfo] ADD  CONSTRAINT [DF_RecordBattleInfo_ClubID]  DEFAULT ((0)) FOR [ClubID]
GO
ALTER TABLE [dbo].[RecordBattleInfo] ADD  CONSTRAINT [DF_RecordBattleInfo_ServerType]  DEFAULT ((1)) FOR [ScoreKind]
GO
ALTER TABLE [dbo].[RecordBattleInfo] ADD  CONSTRAINT [DF_RecordBattleInfo_CreateTime]  DEFAULT ((0)) FOR [CreateTime]
GO
ALTER TABLE [dbo].[RecordBattleInfo] ADD  CONSTRAINT [DF_RecordBattleInfo_PayType]  DEFAULT ((0)) FOR [PayType]
GO
ALTER TABLE [dbo].[RecordBattleInfo] ADD  CONSTRAINT [DF_RecordBattleInfo_PayUserID]  DEFAULT ((0)) FOR [PayUserID]
GO
ALTER TABLE [dbo].[RecordBattleInfo] ADD  CONSTRAINT [DF_RecordBattleInfo_ConsumeCount]  DEFAULT ((0)) FOR [ConsumeCount]
GO
ALTER TABLE [dbo].[RecordBattleInfo] ADD  CONSTRAINT [DF_RecordBattleInfo_CurrencyKind]  DEFAULT ((0)) FOR [CurrencyKind]
GO
ALTER TABLE [dbo].[RecordBattleInfo] ADD  CONSTRAINT [DF_RecordBattleInfo_GameOption]  DEFAULT (N'') FOR [GameOption]
GO
ALTER TABLE [dbo].[RecordBattleInfo] ADD  CONSTRAINT [DF_RecordBattleInfo_UserScoreString]  DEFAULT ('') FOR [UserScoreString]
GO
ALTER TABLE [dbo].[RecordBattleInfo] ADD  CONSTRAINT [DF_RecordBattleInfo_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordBattleUser] ADD  CONSTRAINT [DF_RecordBattleUser_DateID]  DEFAULT ((0)) FOR [DateID]
GO
ALTER TABLE [dbo].[RecordBattleUser] ADD  CONSTRAINT [DF_RecordBattleUser_ClubID]  DEFAULT ((0)) FOR [ClubID]
GO
ALTER TABLE [dbo].[RecordCardChange] ADD  CONSTRAINT [DF_RecordCardChange_SrcUserID]  DEFAULT ((0)) FOR [SrcUserID]
GO
ALTER TABLE [dbo].[RecordCardChange] ADD  CONSTRAINT [DF_RecordCardChange_SrcGameID]  DEFAULT ((0)) FOR [SrcGameID]
GO
ALTER TABLE [dbo].[RecordCardChange] ADD  CONSTRAINT [DF_RecordCardChange_SrcCard]  DEFAULT ((0)) FOR [SrcCard]
GO
ALTER TABLE [dbo].[RecordCardChange] ADD  CONSTRAINT [DF_RecordCardChange_ClubID]  DEFAULT ((0)) FOR [ClubID]
GO
ALTER TABLE [dbo].[RecordCardChange] ADD  CONSTRAINT [DF_RecordCardChange_DstUserID]  DEFAULT ((0)) FOR [DstUserID]
GO
ALTER TABLE [dbo].[RecordCardChange] ADD  CONSTRAINT [DF_RecordCardChange_DstGameID]  DEFAULT ((0)) FOR [DstGameID]
GO
ALTER TABLE [dbo].[RecordCardChange] ADD  CONSTRAINT [DF_RecordCardChange_DstCard]  DEFAULT ((0)) FOR [DstCard]
GO
ALTER TABLE [dbo].[RecordCardChange] ADD  CONSTRAINT [DF_RecordCardChange_StationID]  DEFAULT ((0)) FOR [CardStationID]
GO
ALTER TABLE [dbo].[RecordCardChange] ADD  CONSTRAINT [DF_RecordCardChange_TradeCard]  DEFAULT ((0)) FOR [TradeCard]
GO
ALTER TABLE [dbo].[RecordCardChange] ADD  CONSTRAINT [DF_RecordCardChange_TradeType]  DEFAULT ((0)) FOR [TradeType]
GO
ALTER TABLE [dbo].[RecordCardChange] ADD  CONSTRAINT [DF_RecordCardChange_ClientIP]  DEFAULT (N'‘’') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordCardChange] ADD  CONSTRAINT [DF_RecordCardChange_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordCardChange] ADD  CONSTRAINT [DF_RecordCardChange_CollectNote]  DEFAULT (N'‘’') FOR [CollectNote]
GO
ALTER TABLE [dbo].[RecordCheckIn] ADD  CONSTRAINT [DF_RecordCheckIn_MachineID]  DEFAULT ('') FOR [MachineID]
GO
ALTER TABLE [dbo].[RecordCheckIn] ADD  CONSTRAINT [DF_RecordCheckIn_IPAddress]  DEFAULT ('') FOR [IPAddress]
GO
ALTER TABLE [dbo].[RecordCheckIn] ADD  CONSTRAINT [DF_RecordCheckIn_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordGrantClubScore] ADD  CONSTRAINT [DF_RecordGrantClubScore_ClientIP]  DEFAULT (N'000.000.000.000') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordGrantClubScore] ADD  CONSTRAINT [DF_RecordGrantClubScore_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordGrantClubScore] ADD  CONSTRAINT [DF_RecordGrantClubScore_Reason]  DEFAULT ('') FOR [Reason]
GO
ALTER TABLE [dbo].[RecordGrantIngot] ADD  CONSTRAINT [DF_RecordGrantIngot_CurIngot]  DEFAULT ((0)) FOR [CurIngot]
GO
ALTER TABLE [dbo].[RecordGrantIngot] ADD  CONSTRAINT [DF_RecordGrantIngot_AddIngot]  DEFAULT ((0)) FOR [AddIngot]
GO
ALTER TABLE [dbo].[RecordGrantIngot] ADD  CONSTRAINT [DF_RecordGrantIngot_AddAmount]  DEFAULT ((0)) FOR [AddAmount]
GO
ALTER TABLE [dbo].[RecordGrantIngot] ADD  CONSTRAINT [DF_RecordGrantIngot_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordGrantIngot] ADD  CONSTRAINT [DF_RecordGrantIngot_ClientIP]  DEFAULT (N'000.000.000.000') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordGrantIngot] ADD  CONSTRAINT [DF_RecordGrantIngot_Reason]  DEFAULT ('') FOR [Reason]
GO
ALTER TABLE [dbo].[RecordClubCowryChange] ADD  CONSTRAINT [DF_RecordClubCowryChange_DateID]  DEFAULT ((0)) FOR [DateID]
GO
ALTER TABLE [dbo].[RecordClubCowryChange] ADD  CONSTRAINT [DF_RecordClubCowryChange_SrcUserType]  DEFAULT ((0)) FOR [SrcUserType]
GO
ALTER TABLE [dbo].[RecordClubCowryChange] ADD  CONSTRAINT [DF_RecordClubCowryChange_DstUserID]  DEFAULT ((0)) FOR [DstUserID]
GO
ALTER TABLE [dbo].[RecordClubCowryChange] ADD  CONSTRAINT [DF_RecordClubCowryChange_DstGameID]  DEFAULT ((0)) FOR [DstGameID]
GO
ALTER TABLE [dbo].[RecordClubCowryChange] ADD  CONSTRAINT [DF_RecordClubCowryChange_DstClubCowry]  DEFAULT ((0)) FOR [DstClubCowry]
GO
ALTER TABLE [dbo].[RecordClubCowryChange] ADD  CONSTRAINT [DF_RecordClubCowryChange_DstUserType]  DEFAULT ((0)) FOR [DstUserType]
GO
ALTER TABLE [dbo].[RecordClubCowryChange] ADD  CONSTRAINT [DF_RecordClubCowryChange_ClientIP]  DEFAULT ('') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordClubCowryChange] ADD  CONSTRAINT [DF_RecordClubCowryChange_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordClubCowryChange] ADD  CONSTRAINT [DF_RecordClubCowryChange_CollectNote]  DEFAULT ('') FOR [CollectNote]
GO
ALTER TABLE [dbo].[RecordClubIngotChange] ADD  CONSTRAINT [DF_RecordClubIngotChange_ClubID]  DEFAULT ((0)) FOR [ClubID]
GO
ALTER TABLE [dbo].[RecordClubIngotChange] ADD  CONSTRAINT [DF_RecordClubIngotChange_StationID]  DEFAULT ((0)) FOR [StationID]
GO
ALTER TABLE [dbo].[RecordClubIngotChange] ADD  CONSTRAINT [DF_RecordClubIngotChange_BeforeIngot]  DEFAULT ((0)) FOR [BeforeIngot]
GO
ALTER TABLE [dbo].[RecordClubIngotChange] ADD  CONSTRAINT [DF_RecordClubIngotChange_TradeType]  DEFAULT ((0)) FOR [TradeType]
GO
ALTER TABLE [dbo].[RecordClubIngotChange] ADD  CONSTRAINT [DF_RecordClubIngotChange_TradeIngot]  DEFAULT ((0)) FOR [TradeIngot]
GO
ALTER TABLE [dbo].[RecordClubIngotChange] ADD  CONSTRAINT [DF_RecordClubIngotChange_ClientIP]  DEFAULT ('') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordClubIngotChange] ADD  CONSTRAINT [DF_RecordClubIngotChange_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordClubRevenue] ADD  CONSTRAINT [DF_RecordClubRevenue_IsBalance]  DEFAULT ((0)) FOR [IsBalance]
GO
ALTER TABLE [dbo].[RecordClubRevenue] ADD  CONSTRAINT [DF_RecordClubRevenue_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordClubScoreChange] ADD  CONSTRAINT [DF_RecordClubScoreChange_DateID]  DEFAULT ((0)) FOR [DateID]
GO
ALTER TABLE [dbo].[RecordClubScoreChange] ADD  CONSTRAINT [DF_RecordClubScoreChange_ClubID]  DEFAULT ((0)) FOR [ClubID]
GO
ALTER TABLE [dbo].[RecordClubScoreChange] ADD  CONSTRAINT [DF_RecordClubScoreChange_SrcUserType]  DEFAULT ((0)) FOR [SrcUserType]
GO
ALTER TABLE [dbo].[RecordClubScoreChange] ADD  CONSTRAINT [DF_RecordClubScoreChange_DstUserID]  DEFAULT ((0)) FOR [DstUserID]
GO
ALTER TABLE [dbo].[RecordClubScoreChange] ADD  CONSTRAINT [DF_RecordClubScoreChange_DstGameID]  DEFAULT ((0)) FOR [DstGameID]
GO
ALTER TABLE [dbo].[RecordClubScoreChange] ADD  CONSTRAINT [DF_RecordClubScoreChange_DstClubScore]  DEFAULT ((0)) FOR [DstClubScore]
GO
ALTER TABLE [dbo].[RecordClubScoreChange] ADD  CONSTRAINT [DF_RecordClubScoreChange_DstUserType]  DEFAULT ((0)) FOR [DstUserType]
GO
ALTER TABLE [dbo].[RecordClubScoreChange] ADD  CONSTRAINT [DF_RecordClubScoreChange_BalanceMark]  DEFAULT ((0)) FOR [BalanceMark]
GO
ALTER TABLE [dbo].[RecordClubScoreChange] ADD  CONSTRAINT [DF_RecordClubScoreChange_ClientIP]  DEFAULT ('') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordClubScoreChange] ADD  CONSTRAINT [DF_RecordClubScoreChange_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordClubScoreChange] ADD  CONSTRAINT [DF_RecordClubScoreChange_CollectNote]  DEFAULT ('') FOR [CollectNote]
GO
ALTER TABLE [dbo].[RecordIngotChange] ADD  CONSTRAINT [DF_RecordIngotChange_SrcUserID]  DEFAULT ((0)) FOR [SrcUserID]
GO
ALTER TABLE [dbo].[RecordIngotChange] ADD  CONSTRAINT [DF_RecordIngotChange_SrcGameID]  DEFAULT ((0)) FOR [SrcGameID]
GO
ALTER TABLE [dbo].[RecordIngotChange] ADD  CONSTRAINT [DF_RecordIngotChange_SrcIngot]  DEFAULT ((0)) FOR [SrcIngot]
GO
ALTER TABLE [dbo].[RecordIngotChange] ADD  CONSTRAINT [DF_RecordIngotChange_DstUserID]  DEFAULT ((0)) FOR [DstUserID]
GO
ALTER TABLE [dbo].[RecordIngotChange] ADD  CONSTRAINT [DF_RecordIngotChange_DstGameID]  DEFAULT ((0)) FOR [DstGameID]
GO
ALTER TABLE [dbo].[RecordIngotChange] ADD  CONSTRAINT [DF_RecordIngotChange_DstIngot]  DEFAULT ((0)) FOR [DstIngot]
GO
ALTER TABLE [dbo].[RecordIngotChange] ADD  CONSTRAINT [DF_RecordIngotChange_StationID]  DEFAULT ((0)) FOR [StationID]
GO
ALTER TABLE [dbo].[RecordIngotChange] ADD  CONSTRAINT [DF_RecordIngotChange_ClubID]  DEFAULT ((0)) FOR [ClubID]
GO
ALTER TABLE [dbo].[RecordIngotChange] ADD  CONSTRAINT [DF_RecordIngotChange_TradeIngot]  DEFAULT ((0)) FOR [TradeIngot]
GO
ALTER TABLE [dbo].[RecordIngotChange] ADD  CONSTRAINT [DF_RecordIngotChange_TypeID]  DEFAULT ((0)) FOR [TypeID]
GO
ALTER TABLE [dbo].[RecordIngotChange] ADD  CONSTRAINT [DF_RecordIngotChange_ClientIP]  DEFAULT ('') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordIngotChange] ADD  CONSTRAINT [DF_RecordIngotChange_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordIngotChange] ADD  CONSTRAINT [DF_RecordIngotChange_CollectNote]  DEFAULT ('') FOR [CollectNote]
GO
ALTER TABLE [dbo].[RecordPartnerRevenue] ADD  CONSTRAINT [DF_RecordPartnerRevenue_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordVideoInfo] ADD  CONSTRAINT [DF_RecordVideoInfo_KindID]  DEFAULT ((0)) FOR [KindID]
GO
ALTER TABLE [dbo].[RecordVideoInfo] ADD  CONSTRAINT [DF_RecordVideoInfo_MappedNum]  DEFAULT ((0)) FOR [MappedNum]
GO
ALTER TABLE [dbo].[RecordVideoInfo] ADD  CONSTRAINT [DF_RecordVideoInfo_BattleID]  DEFAULT ((0)) FOR [BattleRecordID]
GO
ALTER TABLE [dbo].[RecordVideoInfo] ADD  CONSTRAINT [DF_RecordVideoInfo_VideoCreateTime]  DEFAULT (getdate()) FOR [VideoCreateTime]
GO
ALTER TABLE [dbo].[RecordVideoInfo] ADD  CONSTRAINT [DF_RecordVideoInfo_UserCount]  DEFAULT ((0)) FOR [UserCount]
GO
ALTER TABLE [dbo].[RecordVideoInfo] ADD  CONSTRAINT [DF_RecordVideoInfo_PlayBackCode]  DEFAULT ((0)) FOR [PlayBackCode]
GO
ALTER TABLE [dbo].[RecordVideoInfo] ADD  CONSTRAINT [DF_RecordVideoInfo_RecordDate]  DEFAULT (getdate()) FOR [RecordDate]
GO
ALTER TABLE [dbo].[RecordVideoUser] ADD  CONSTRAINT [DF_RecordVideoUser_MappedNum]  DEFAULT ((0)) FOR [MappedNum]
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'映射编号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBattleInfo', @level2type=N'COLUMN',@level2name=N'MappedNum'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'0 未完成  1 已完成' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBattleInfo', @level2type=N'COLUMN',@level2name=N'BattleFinish'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'群组标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBattleInfo', @level2type=N'COLUMN',@level2name=N'ClubID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBattleInfo', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBattleInfo', @level2type=N'COLUMN',@level2name=N'ServerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'分数类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBattleInfo', @level2type=N'COLUMN',@level2name=N'ScoreKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'桌主标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBattleInfo', @level2type=N'COLUMN',@level2name=N'OwnerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'解散方式' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBattleInfo', @level2type=N'COLUMN',@level2name=N'SettleKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBattleInfo', @level2type=N'COLUMN',@level2name=N'PlayCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏时长' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBattleInfo', @level2type=N'COLUMN',@level2name=N'PlayTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'基础底分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBattleInfo', @level2type=N'COLUMN',@level2name=N'BaseScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'货币类型(1：钻石 2：房卡)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBattleInfo', @level2type=N'COLUMN',@level2name=N'CurrencyKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数据大小' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBattleInfo', @level2type=N'COLUMN',@level2name=N'UserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCardChange', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'源用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCardChange', @level2type=N'COLUMN',@level2name=N'SrcUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'源用户ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCardChange', @level2type=N'COLUMN',@level2name=N'SrcGameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'源用户房卡' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCardChange', @level2type=N'COLUMN',@level2name=N'SrcCard'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'茶馆标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCardChange', @level2type=N'COLUMN',@level2name=N'ClubID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCardChange', @level2type=N'COLUMN',@level2name=N'DstUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标用户ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCardChange', @level2type=N'COLUMN',@level2name=N'DstGameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标用户房卡' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCardChange', @level2type=N'COLUMN',@level2name=N'DstCard'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'平台标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCardChange', @level2type=N'COLUMN',@level2name=N'CardStationID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变化房卡' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCardChange', @level2type=N'COLUMN',@level2name=N'TradeCard'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'1.购买房卡,2:赠送房卡,3:开房费用,4:开房退还,5:系统赠送,6:后台赠送,7:转盘抽奖,9:签到赠送 11:茶馆存入,12:茶馆取出,14:创建茶馆 15:转盘抽奖' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCardChange', @level2type=N'COLUMN',@level2name=N'TradeType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCardChange', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCardChange', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作备注' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCardChange', @level2type=N'COLUMN',@level2name=N'CollectNote'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCheckIn', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCheckIn', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCheckIn', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'站点标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCheckIn', @level2type=N'COLUMN',@level2name=N'StationID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送钻石' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCheckIn', @level2type=N'COLUMN',@level2name=N'PresentIngot'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCheckIn', @level2type=N'COLUMN',@level2name=N'MachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCheckIn', @level2type=N'COLUMN',@level2name=N'IPAddress'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCheckIn', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantIngot', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantIngot', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'站点标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantIngot', @level2type=N'COLUMN',@level2name=N'StationID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'当前钻石' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantIngot', @level2type=N'COLUMN',@level2name=N'CurIngot'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送钻石' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantIngot', @level2type=N'COLUMN',@level2name=N'AddIngot'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记账金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantIngot', @level2type=N'COLUMN',@level2name=N'AddAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantIngot', @level2type=N'COLUMN',@level2name=N'MasterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantIngot', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantIngot', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送原因' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantIngot', @level2type=N'COLUMN',@level2name=N'Reason'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubCowryChange', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubCowryChange', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'茶馆标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubCowryChange', @level2type=N'COLUMN',@level2name=N'ClubID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'源用户ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubCowryChange', @level2type=N'COLUMN',@level2name=N'SrcUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'源游戏ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubCowryChange', @level2type=N'COLUMN',@level2name=N'SrcGameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'源贝壳数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubCowryChange', @level2type=N'COLUMN',@level2name=N'SrcClubCowry'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'成员类型（0：普通成员 1：合伙人助手 2：合伙人 3：管理员 4：老板）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubCowryChange', @level2type=N'COLUMN',@level2name=N'SrcUserType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标用户ID ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubCowryChange', @level2type=N'COLUMN',@level2name=N'DstUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标游戏ID ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubCowryChange', @level2type=N'COLUMN',@level2name=N'DstGameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标贝壳数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubCowryChange', @level2type=N'COLUMN',@level2name=N'DstClubCowry'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'成员类型（0：普通成员 1：合伙人助手 2：合伙人 3：管理员 4：老板）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubCowryChange', @level2type=N'COLUMN',@level2name=N'DstUserType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变化类型(2:上分,3:下分)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubCowryChange', @level2type=N'COLUMN',@level2name=N'TradeType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变化贝壳' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubCowryChange', @level2type=N'COLUMN',@level2name=N'TradeClubCowry'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubCowryChange', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubCowryChange', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubCowryChange', @level2type=N'COLUMN',@level2name=N'CollectNote'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubIngotChange', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'茶馆标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubIngotChange', @level2type=N'COLUMN',@level2name=N'ClubID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'茶馆站点标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubIngotChange', @level2type=N'COLUMN',@level2name=N'StationID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'交易前钻石' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubIngotChange', @level2type=N'COLUMN',@level2name=N'BeforeIngot'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'交易类型（1  存入钻石  2 取出钻石  3  开房消耗  4 退房返还）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubIngotChange', @level2type=N'COLUMN',@level2name=N'TradeType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'交易钻石' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubIngotChange', @level2type=N'COLUMN',@level2name=N'TradeIngot'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubIngotChange', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubIngotChange', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubRevenue', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'群组标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubRevenue', @level2type=N'COLUMN',@level2name=N'ClubID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubRevenue', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上级标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubRevenue', @level2type=N'COLUMN',@level2name=N'ParentID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上级昵称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubRevenue', @level2type=N'COLUMN',@level2name=N'ParentNickname'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上级游戏ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubRevenue', @level2type=N'COLUMN',@level2name=N'ParentGameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总赢' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubRevenue', @level2type=N'COLUMN',@level2name=N'WinScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总输' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubRevenue', @level2type=N'COLUMN',@level2name=N'LostScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'提供税收' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubRevenue', @level2type=N'COLUMN',@level2name=N'Revenue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'税收比例' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubRevenue', @level2type=N'COLUMN',@level2name=N'RevenueRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'真实比例' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubRevenue', @level2type=N'COLUMN',@level2name=N'RealRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'代理级别' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubRevenue', @level2type=N'COLUMN',@level2name=N'LevelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收入积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubRevenue', @level2type=N'COLUMN',@level2name=N'IncomeScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'0-未结算，1-已结算' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubRevenue', @level2type=N'COLUMN',@level2name=N'IsBalance'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubRevenue', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubScoreChange', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'茶馆标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubScoreChange', @level2type=N'COLUMN',@level2name=N'ClubID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'源用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubScoreChange', @level2type=N'COLUMN',@level2name=N'SrcUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'源游戏ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubScoreChange', @level2type=N'COLUMN',@level2name=N'SrcGameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'源茶馆分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubScoreChange', @level2type=N'COLUMN',@level2name=N'SrcClubScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'成员类型（0：普通成员 1：合伙人助手 2：合伙人 3：管理员 4：老板）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubScoreChange', @level2type=N'COLUMN',@level2name=N'SrcUserType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubScoreChange', @level2type=N'COLUMN',@level2name=N'DstUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标游戏ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubScoreChange', @level2type=N'COLUMN',@level2name=N'DstGameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标茶馆分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubScoreChange', @level2type=N'COLUMN',@level2name=N'DstClubScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'成员类型（0：普通成员 1：合伙人助手 2：合伙人 3：管理员 4：老板）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubScoreChange', @level2type=N'COLUMN',@level2name=N'DstUserType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变化类型(1:充值积分,2:上分,3:下分,4:抽税收入,5:系统赠送,6:后台赠送,7:兑换支出)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubScoreChange', @level2type=N'COLUMN',@level2name=N'TradeType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变化积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubScoreChange', @level2type=N'COLUMN',@level2name=N'TradeClubScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'结算标志（0： 未结算 1： 已结算）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubScoreChange', @level2type=N'COLUMN',@level2name=N'BalanceMark'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变化地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubScoreChange', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变化日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubScoreChange', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubScoreChange', @level2type=N'COLUMN',@level2name=N'CollectNote'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordIngotChange', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'原始钻石' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordIngotChange', @level2type=N'COLUMN',@level2name=N'SrcIngot'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'茶馆标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordIngotChange', @level2type=N'COLUMN',@level2name=N'ClubID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变化钻石' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordIngotChange', @level2type=N'COLUMN',@level2name=N'TradeIngot'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'1.获取钻石,2:兑换金币,3:开房费用,4:开房退还,5:系统赠送,6:后台赠送,7:分享送钻,8:推广赠送,9:升级赠送,10:转盘签到,11:茶馆存入,12:茶馆取出,13:购买二维码,14:购买道具,15:代理赠送' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordIngotChange', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordIngotChange', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordIngotChange', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'约战记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordVideoInfo', @level2type=N'COLUMN',@level2name=N'BattleRecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordVideoInfo', @level2type=N'COLUMN',@level2name=N'VideoCreateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'回放码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordVideoInfo', @level2type=N'COLUMN',@level2name=N'PlayBackCode'
GO
