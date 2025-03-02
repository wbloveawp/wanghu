USE [LHGameMatchDB]
GO
/****** Object:  Table [dbo].[GameColumnItem]    Script Date: 2023/7/12 11:06:52 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameColumnItem](
	[SortID] [int] NOT NULL,
	[ColumnName] [nvarchar](15) NOT NULL,
	[ColumnWidth] [tinyint] NOT NULL,
	[DataDescribe] [tinyint] NOT NULL,
	[SuportType] [smallint] NOT NULL,
 CONSTRAINT [PK_GameColumnItem_SortID] PRIMARY KEY CLUSTERED 
(
	[SortID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameScoreInfo]    Script Date: 2023/7/12 11:06:52 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameScoreInfo](
	[UserID] [int] NOT NULL,
	[UserRight] [int] NOT NULL,
	[MasterRight] [int] NOT NULL,
	[MasterOrder] [tinyint] NOT NULL,
	[LastLogonIP] [nvarchar](15) NOT NULL,
	[LastLogonDate] [datetime] NOT NULL,
	[LastLogonMachine] [nvarchar](32) NOT NULL,
	[AllLogonTimes] [int] NOT NULL,
	[PlayTimeCount] [int] NOT NULL,
	[OnLineTimeCount] [int] NOT NULL,
 CONSTRAINT [PK_GameScore] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[MatchOption]    Script Date: 2023/7/12 11:06:52 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[MatchOption](
	[MatchID] [int] IDENTITY(1,1) NOT NULL,
	[MatchStatus] [tinyint] NOT NULL,
	[GroupID] [bigint] NOT NULL,
	[KindID] [int] NOT NULL,
	[ModuleName] [nvarchar](32) NOT NULL,
	[MatchName] [nvarchar](50) NOT NULL,
	[MatchType] [tinyint] NOT NULL,
	[ShowType] [tinyint] NOT NULL,
	[SignupCount] [smallint] NOT NULL,
	[SignupMode] [tinyint] NOT NULL,
	[FeeAmount] [int] NOT NULL,
	[FeeMoneyID] [smallint] NOT NULL,
	[UseTicketCount] [int] NOT NULL,
	[BuyTicketCount] [int] NOT NULL,
	[BuyTicketOption] [int] NOT NULL,
	[BuyTicketAmount] [int] NOT NULL,
	[BuyTicketMoneyID] [smallint] NOT NULL,
	[JoinCondition] [tinyint] NOT NULL,
	[MemberOrder] [tinyint] NOT NULL,
	[Experience] [int] NOT NULL,
	[MatchDistribute] [varbinary](64) NOT NULL,
	[MatchReward] [varbinary](max) NOT NULL,
	[RewardCount] [smallint] NOT NULL,
	[MatchRound] [varbinary](512) NOT NULL,
	[RoundCount] [smallint] NOT NULL,
	[MatchRule] [varbinary](512) NOT NULL,
	[ServiceMachine] [nchar](32) NOT NULL,
	[Nullity] [bit] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_MatchOption] PRIMARY KEY CLUSTERED 
(
	[MatchID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[MatchReviveInfo]    Script Date: 2023/7/12 11:06:52 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[MatchReviveInfo](
	[UserID] [int] NOT NULL,
	[MatchID] [int] NOT NULL,
	[MatchNo] [bigint] NOT NULL,
	[FeeMoneyID] [tinyint] NOT NULL,
	[FeeAmount] [int] NOT NULL,
	[ReviveTimes] [tinyint] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_MatchReviveInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC,
	[MatchID] ASC,
	[MatchNo] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[MatchScoreInfo]    Script Date: 2023/7/12 11:06:52 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[MatchScoreInfo](
	[UserID] [int] NOT NULL,
	[MatchID] [int] NOT NULL,
	[MatchNo] [bigint] NOT NULL,
	[GameID] [int] NOT NULL,
	[ServerID] [int] NOT NULL,
	[RankID] [smallint] NOT NULL,
	[Score] [bigint] NOT NULL,
	[WinCount] [int] NOT NULL,
	[LostCount] [int] NOT NULL,
	[DrawCount] [int] NOT NULL,
	[FleeCount] [int] NOT NULL,
	[WinStreak] [int] NOT NULL,
	[MaxWinStreak] [int] NOT NULL,
	[MatchRight] [int] NOT NULL,
	[PlayTimeCount] [int] NOT NULL,
	[SignupTime] [datetime] NOT NULL,
	[SignupCount] [smallint] NOT NULL,
	[SignupRecordID] [int] NOT NULL,
 CONSTRAINT [PK_MatchScoreInfo_1] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC,
	[MatchID] ASC,
	[MatchNo] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordDrawInfo]    Script Date: 2023/7/12 11:06:52 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordDrawInfo](
	[RecordID] [bigint] NOT NULL,
	[KindID] [int] NOT NULL,
	[ServerID] [int] NOT NULL,
	[TableID] [int] NOT NULL,
	[UserCount] [smallint] NOT NULL,
	[WasteCount] [bigint] NOT NULL,
	[RevenueCount] [bigint] NOT NULL,
	[PlayTimeCount] [int] NOT NULL,
	[GameCourse] [nvarchar](1024) NOT NULL,
	[VideoNumber] [nvarchar](21) NOT NULL,
	[MatchNo] [bigint] NOT NULL,
	[RecordDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordDrawInfo_RecordID] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordDrawScore]    Script Date: 2023/7/12 11:06:52 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordDrawScore](
	[RecordID] [bigint] NOT NULL,
	[UserID] [int] NOT NULL,
	[ChairID] [smallint] NOT NULL,
	[Score] [bigint] NOT NULL,
	[Revenue] [bigint] NOT NULL,
	[VideoNumber] [nvarchar](21) NOT NULL,
	[MatchNo] [bigint] NOT NULL,
	[RecordDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordDrawScore] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordFirstBuyTicket]    Script Date: 2023/7/12 11:06:52 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordFirstBuyTicket](
	[UserID] [int] NOT NULL,
	[ParentID] [int] NOT NULL,
	[MatchID] [int] NOT NULL,
	[MatchNo] [bigint] NOT NULL,
	[MatchName] [nvarchar](32) NOT NULL,
	[MatchTime] [datetime] NOT NULL,
	[BuyAmount] [int] NOT NULL,
	[BuyMoneyID] [smallint] NOT NULL,
	[CollectTime] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordMatchTicket] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordUserInout]    Script Date: 2023/7/12 11:06:52 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordUserInout](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[ServerID] [int] NOT NULL,
	[LockerScore] [bigint] NOT NULL,
	[EnterTime] [datetime] NOT NULL,
	[EnterGold] [bigint] NOT NULL,
	[EnterScore] [bigint] NOT NULL,
	[EnterMachine] [nvarchar](32) NOT NULL,
	[EnterClientIP] [nvarchar](15) NOT NULL,
	[LeaveTime] [datetime] NULL,
	[LeaveReason] [int] NULL,
	[LeaveMachine] [nvarchar](32) NULL,
	[LeaveClientIP] [nvarchar](15) NULL,
	[Gold] [bigint] NULL,
	[Score] [bigint] NULL,
	[Revenue] [bigint] NULL,
	[WinCount] [int] NULL,
	[LostCount] [int] NULL,
	[DrawCount] [int] NULL,
	[FleeCount] [int] NULL,
	[Experience] [int] NULL,
	[LoveLiness] [int] NULL,
	[PlayTimeCount] [int] NULL,
	[OnLineTimeCount] [int] NULL,
 CONSTRAINT [PK_RecordUserInout_1] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReportUserScore]    Script Date: 2023/7/12 11:06:52 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReportUserScore](
	[DateID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[GainScore] [bigint] NOT NULL,
	[LoseScore] [bigint] NULL,
	[WinCount] [int] NOT NULL,
	[LostCount] [int] NOT NULL,
	[DrawCount] [int] NOT NULL,
	[FleeCount] [int] NOT NULL,
	[UserService] [bigint] NOT NULL,
	[UserRevenue] [bigint] NOT NULL,
	[PlayTimeCount] [int] NOT NULL,
	[InsertDateTime] [datetime] NOT NULL,
	[ModifyDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_ReportUserScore] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[StreamMatchFeeInfo]    Script Date: 2023/7/12 11:06:52 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[StreamMatchFeeInfo](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[MatchID] [int] NOT NULL,
	[MatchNo] [bigint] NOT NULL,
	[MatchType] [smallint] NOT NULL,
	[ServerID] [int] NOT NULL,
	[SignupMode] [smallint] NOT NULL,
	[FeeMoneyID] [smallint] NOT NULL,
	[FeeAmount] [bigint] NOT NULL,
	[UseTicketCount] [int] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_StreamMatchFeeInfo] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[StreamMatchHistory]    Script Date: 2023/7/12 11:06:52 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[StreamMatchHistory](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[GameID] [int] NOT NULL,
	[MatchID] [int] NOT NULL,
	[MatchNo] [bigint] NOT NULL,
	[MatchType] [tinyint] NOT NULL,
	[ServerID] [int] NOT NULL,
	[RankID] [int] NOT NULL,
	[MatchScore] [int] NOT NULL,
	[IsPromoted] [tinyint] NOT NULL,
	[RewardEntity] [nvarchar](16) NOT NULL,
	[RewardGoods] [nvarchar](256) NOT NULL,
	[WinCount] [int] NOT NULL,
	[LostCount] [int] NOT NULL,
	[DrawCount] [int] NOT NULL,
	[FleeCount] [int] NOT NULL,
	[WinStreak] [int] NOT NULL,
	[MaxWinStreak] [int] NOT NULL,
	[MatchStartTime] [datetime] NOT NULL,
	[MatchFinishTime] [datetime] NOT NULL,
	[PlayTimeCount] [int] NOT NULL,
	[RecordDate] [datetime] NOT NULL,
 CONSTRAINT [PK_StreamMatchHistory] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[StreamMatchSession]    Script Date: 2023/7/12 11:06:52 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[StreamMatchSession](
	[MatchID] [int] NOT NULL,
	[MatchNo] [bigint] NOT NULL,
	[SingupUserCount] [int] NOT NULL,
	[MatchUserCount] [int] NOT NULL,
	[AvgMatchTimes] [int] NOT NULL,
	[TotalMatchTimes] [int] NOT NULL,
	[ConsumeIngot] [bigint] NOT NULL,
	[MatchStartTime] [datetime] NOT NULL,
	[MatchFinishTime] [datetime] NOT NULL,
	[CollectTime] [datetime] NOT NULL,
 CONSTRAINT [PK_StreamMatchSesstion] PRIMARY KEY CLUSTERED 
(
	[MatchID] ASC,
	[MatchNo] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[StreamMatchTicket]    Script Date: 2023/7/12 11:06:52 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[StreamMatchTicket](
	[UserID] [int] NOT NULL,
	[MatchID] [int] NOT NULL,
	[MatchNo] [bigint] NOT NULL,
	[BuyTimes] [int] NOT NULL,
	[BuyAmount] [int] NOT NULL,
	[BuyMoneyID] [smallint] NOT NULL,
	[BuyTicketCount] [int] NOT NULL,
	[UseTicketCount] [int] NOT NULL,
	[UpdateTime] [datetime] NOT NULL,
	[CollectTime] [datetime] NOT NULL,
 CONSTRAINT [PK_MatchTicketInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC,
	[MatchID] ASC,
	[MatchNo] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[SystemStatusInfo]    Script Date: 2023/7/12 11:06:52 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[SystemStatusInfo](
	[StatusName] [nvarchar](32) NOT NULL,
	[StatusValue] [int] NOT NULL,
	[StatusString] [nvarchar](512) NOT NULL,
 CONSTRAINT [PK_SystemStatusInfo] PRIMARY KEY CLUSTERED 
(
	[StatusName] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[SystemStreamInfo]    Script Date: 2023/7/12 11:06:52 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[SystemStreamInfo](
	[DateID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[ServerID] [int] NOT NULL,
	[LogonCount] [int] NOT NULL,
	[LogOutCount] [int] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_SystemStreamInfo] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[KindID] ASC,
	[ServerID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
ALTER TABLE [dbo].[GameColumnItem] ADD  CONSTRAINT [DF_GameColumnItem_SuportType]  DEFAULT ((0)) FOR [SuportType]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_UserRight]  DEFAULT ((0)) FOR [UserRight]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_MasterRight]  DEFAULT ((0)) FOR [MasterRight]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_MasterOrder]  DEFAULT ((0)) FOR [MasterOrder]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_LastLogonDate]  DEFAULT (getdate()) FOR [LastLogonDate]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_LastLogonMachine]  DEFAULT (N'') FOR [LastLogonMachine]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_AllLogonTimes]  DEFAULT ((0)) FOR [AllLogonTimes]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_PlayTimeCount_1]  DEFAULT ((0)) FOR [PlayTimeCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_OnLineTimeCount]  DEFAULT ((0)) FOR [OnLineTimeCount]
GO
ALTER TABLE [dbo].[MatchOption] ADD  CONSTRAINT [DF_MatchPublic_Status]  DEFAULT ((0)) FOR [MatchStatus]
GO
ALTER TABLE [dbo].[MatchOption] ADD  CONSTRAINT [DF_MatchPublic_GroupID]  DEFAULT ((0)) FOR [GroupID]
GO
ALTER TABLE [dbo].[MatchOption] ADD  CONSTRAINT [DF_MatchPublic_KindID]  DEFAULT ((0)) FOR [KindID]
GO
ALTER TABLE [dbo].[MatchOption] ADD  CONSTRAINT [DF_MatchPublic_ModuleName]  DEFAULT ('') FOR [ModuleName]
GO
ALTER TABLE [dbo].[MatchOption] ADD  CONSTRAINT [DF_MatchOption_ShowType]  DEFAULT ((0)) FOR [ShowType]
GO
ALTER TABLE [dbo].[MatchOption] ADD  CONSTRAINT [DF_MatchPublic_SignupCount]  DEFAULT ((0)) FOR [SignupCount]
GO
ALTER TABLE [dbo].[MatchOption] ADD  CONSTRAINT [DF_MatchPublic_FeeType]  DEFAULT ((0)) FOR [FeeMoneyID]
GO
ALTER TABLE [dbo].[MatchOption] ADD  CONSTRAINT [DF_MatchOption_BuyTicketMoneyID1]  DEFAULT ((0)) FOR [BuyTicketOption]
GO
ALTER TABLE [dbo].[MatchOption] ADD  CONSTRAINT [DF_MatchPublic_TicketAmount]  DEFAULT ((0)) FOR [BuyTicketAmount]
GO
ALTER TABLE [dbo].[MatchOption] ADD  CONSTRAINT [DF_MatchPublic_TicketMoneyID]  DEFAULT ((0)) FOR [BuyTicketMoneyID]
GO
ALTER TABLE [dbo].[MatchOption] ADD  CONSTRAINT [DF_MatchPublic_MemberOrder]  DEFAULT ((0)) FOR [MemberOrder]
GO
ALTER TABLE [dbo].[MatchOption] ADD  CONSTRAINT [DF_MatchPublic_Experience]  DEFAULT ((0)) FOR [Experience]
GO
ALTER TABLE [dbo].[MatchOption] ADD  CONSTRAINT [DF_MatchOption_MatchDistribute]  DEFAULT ((0)) FOR [MatchDistribute]
GO
ALTER TABLE [dbo].[MatchOption] ADD  CONSTRAINT [DF_MatchPublic_MatchReward]  DEFAULT (NULL) FOR [MatchReward]
GO
ALTER TABLE [dbo].[MatchOption] ADD  CONSTRAINT [DF_MatchPublic_RewardCount]  DEFAULT ((0)) FOR [RewardCount]
GO
ALTER TABLE [dbo].[MatchOption] ADD  CONSTRAINT [DF_MatchPublic_RoundCount]  DEFAULT ((0)) FOR [RoundCount]
GO
ALTER TABLE [dbo].[MatchOption] ADD  CONSTRAINT [DF_MatchPublic_ServiceMachine]  DEFAULT (N'') FOR [ServiceMachine]
GO
ALTER TABLE [dbo].[MatchOption] ADD  CONSTRAINT [DF_MatchPublic_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[MatchReviveInfo] ADD  CONSTRAINT [DF_MatchReviveInfo_MatchNO]  DEFAULT ((0)) FOR [MatchNo]
GO
ALTER TABLE [dbo].[MatchReviveInfo] ADD  CONSTRAINT [DF_MatchReviveInfo_FeeType]  DEFAULT ((0)) FOR [FeeMoneyID]
GO
ALTER TABLE [dbo].[MatchReviveInfo] ADD  CONSTRAINT [DF_MatchReviveInfo_ReviveFee]  DEFAULT ((0)) FOR [FeeAmount]
GO
ALTER TABLE [dbo].[MatchReviveInfo] ADD  CONSTRAINT [DF_MatchReviveInfo_ReviveTimes]  DEFAULT ((0)) FOR [ReviveTimes]
GO
ALTER TABLE [dbo].[MatchReviveInfo] ADD  CONSTRAINT [DF_MatchReviveInfo_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[MatchScoreInfo] ADD  CONSTRAINT [DF_MatchScoreInfo_GameID]  DEFAULT ((0)) FOR [GameID]
GO
ALTER TABLE [dbo].[MatchScoreInfo] ADD  CONSTRAINT [DF_MatchScoreInfo_ServerID]  DEFAULT ((0)) FOR [ServerID]
GO
ALTER TABLE [dbo].[MatchScoreInfo] ADD  CONSTRAINT [DF_MatchScoreInfo_RankID]  DEFAULT ((0)) FOR [RankID]
GO
ALTER TABLE [dbo].[MatchScoreInfo] ADD  CONSTRAINT [DF_MatchScoreInfo_Score]  DEFAULT ((0)) FOR [Score]
GO
ALTER TABLE [dbo].[MatchScoreInfo] ADD  CONSTRAINT [DF_MatchScoreInfo_WinCount]  DEFAULT ((0)) FOR [WinCount]
GO
ALTER TABLE [dbo].[MatchScoreInfo] ADD  CONSTRAINT [DF_MatchScoreInfo_LostCount]  DEFAULT ((0)) FOR [LostCount]
GO
ALTER TABLE [dbo].[MatchScoreInfo] ADD  CONSTRAINT [DF_MatchScoreInfo_DrawCount]  DEFAULT ((0)) FOR [DrawCount]
GO
ALTER TABLE [dbo].[MatchScoreInfo] ADD  CONSTRAINT [DF_MatchScoreInfo_FleeCount]  DEFAULT ((0)) FOR [FleeCount]
GO
ALTER TABLE [dbo].[MatchScoreInfo] ADD  CONSTRAINT [DF_MatchScoreInfo_WinStreak]  DEFAULT ((0)) FOR [WinStreak]
GO
ALTER TABLE [dbo].[MatchScoreInfo] ADD  CONSTRAINT [DF_MatchScoreInfo_MaxWinStreak]  DEFAULT ((0)) FOR [MaxWinStreak]
GO
ALTER TABLE [dbo].[MatchScoreInfo] ADD  CONSTRAINT [DF_MatchScoreInfo_UserRight]  DEFAULT ((0)) FOR [MatchRight]
GO
ALTER TABLE [dbo].[MatchScoreInfo] ADD  CONSTRAINT [DF_MatchScoreInfo_PlayTimeCount]  DEFAULT ((0)) FOR [PlayTimeCount]
GO
ALTER TABLE [dbo].[MatchScoreInfo] ADD  CONSTRAINT [DF_MatchScoreInfo_UpdateTime]  DEFAULT (getdate()) FOR [SignupTime]
GO
ALTER TABLE [dbo].[MatchScoreInfo] ADD  CONSTRAINT [DF_MatchScoreInfo_SignupCount]  DEFAULT ((0)) FOR [SignupCount]
GO
ALTER TABLE [dbo].[MatchScoreInfo] ADD  CONSTRAINT [DF_MatchScoreInfo_RelationFeeID]  DEFAULT ((0)) FOR [SignupRecordID]
GO
ALTER TABLE [dbo].[RecordDrawInfo] ADD  CONSTRAINT [DF_RecordDrawInfo_GameCourse]  DEFAULT ('') FOR [GameCourse]
GO
ALTER TABLE [dbo].[RecordDrawInfo] ADD  CONSTRAINT [DF_RecordDrawInfo_VideoNumber]  DEFAULT ('') FOR [VideoNumber]
GO
ALTER TABLE [dbo].[RecordDrawInfo] ADD  CONSTRAINT [DF_RecordDrawInfo_MatchNo]  DEFAULT ((0)) FOR [MatchNo]
GO
ALTER TABLE [dbo].[RecordDrawInfo] ADD  CONSTRAINT [DF_RecordDrawInfo_RecordDate]  DEFAULT (getdate()) FOR [RecordDate]
GO
ALTER TABLE [dbo].[RecordDrawScore] ADD  CONSTRAINT [DF_RecordDrawScore_VideoNumber]  DEFAULT (N'') FOR [VideoNumber]
GO
ALTER TABLE [dbo].[RecordDrawScore] ADD  CONSTRAINT [DF_RecordDrawScore_MatchNo]  DEFAULT ((0)) FOR [MatchNo]
GO
ALTER TABLE [dbo].[RecordDrawScore] ADD  CONSTRAINT [DF_RecordDrawScore_RecordDate]  DEFAULT (getdate()) FOR [RecordDate]
GO
ALTER TABLE [dbo].[RecordUserInout] ADD  CONSTRAINT [DF_RecordUserInout_FreeScore]  DEFAULT ((0)) FOR [LockerScore]
GO
ALTER TABLE [dbo].[RecordUserInout] ADD  CONSTRAINT [DF_RecordUserInout_EnterTime]  DEFAULT (getdate()) FOR [EnterTime]
GO
ALTER TABLE [dbo].[RecordUserInout] ADD  CONSTRAINT [DF_RecordUserInout_EnterGold]  DEFAULT ((0)) FOR [EnterGold]
GO
ALTER TABLE [dbo].[ReportUserScore] ADD  CONSTRAINT [DF_ReportUserScore_GainScore]  DEFAULT ((0)) FOR [GainScore]
GO
ALTER TABLE [dbo].[ReportUserScore] ADD  CONSTRAINT [DF_ReportUserScore_LoseScore]  DEFAULT ((0)) FOR [LoseScore]
GO
ALTER TABLE [dbo].[ReportUserScore] ADD  CONSTRAINT [DF_ReportUserScore_WinCount]  DEFAULT ((0)) FOR [WinCount]
GO
ALTER TABLE [dbo].[ReportUserScore] ADD  CONSTRAINT [DF_ReportUserScore_LostCount]  DEFAULT ((0)) FOR [LostCount]
GO
ALTER TABLE [dbo].[ReportUserScore] ADD  CONSTRAINT [DF_ReportUserScore_DrawCount]  DEFAULT ((0)) FOR [DrawCount]
GO
ALTER TABLE [dbo].[ReportUserScore] ADD  CONSTRAINT [DF_ReportUserScore_FleeCount]  DEFAULT ((0)) FOR [FleeCount]
GO
ALTER TABLE [dbo].[ReportUserScore] ADD  CONSTRAINT [DF_ReportUserScore_UserService_1]  DEFAULT ((0)) FOR [UserService]
GO
ALTER TABLE [dbo].[ReportUserScore] ADD  CONSTRAINT [DF_ReportUserScore_UserRevenue]  DEFAULT ((0)) FOR [UserRevenue]
GO
ALTER TABLE [dbo].[ReportUserScore] ADD  CONSTRAINT [DF_ReportUserScore_PlayTimeCount]  DEFAULT ((0)) FOR [PlayTimeCount]
GO
ALTER TABLE [dbo].[ReportUserScore] ADD  CONSTRAINT [DF_ReportUserScore_InsertDateTime]  DEFAULT (getdate()) FOR [InsertDateTime]
GO
ALTER TABLE [dbo].[ReportUserScore] ADD  CONSTRAINT [DF_ReportUserScore_ModifyDateTime]  DEFAULT (getdate()) FOR [ModifyDateTime]
GO
ALTER TABLE [dbo].[StreamMatchFeeInfo] ADD  CONSTRAINT [DF_StreamMatchFeeInfo_MacthNo]  DEFAULT ((0)) FOR [MatchNo]
GO
ALTER TABLE [dbo].[StreamMatchFeeInfo] ADD  CONSTRAINT [DF_StreamMatchFeeInfo_MatchType]  DEFAULT ((0)) FOR [MatchType]
GO
ALTER TABLE [dbo].[StreamMatchFeeInfo] ADD  CONSTRAINT [DF_StreamMatchFeeInfo_FeeType]  DEFAULT ((0)) FOR [FeeMoneyID]
GO
ALTER TABLE [dbo].[StreamMatchFeeInfo] ADD  CONSTRAINT [DF_StreamMatchFeeInfo_UseTicketCount]  DEFAULT ((0)) FOR [UseTicketCount]
GO
ALTER TABLE [dbo].[StreamMatchFeeInfo] ADD  CONSTRAINT [DF_StreamMatchFeeInfo_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[StreamMatchHistory] ADD  CONSTRAINT [DF_StreamMatchHistory_GameID]  DEFAULT ((0)) FOR [GameID]
GO
ALTER TABLE [dbo].[StreamMatchHistory] ADD  CONSTRAINT [DF_StreamMatchHistory_TypeID]  DEFAULT ((0)) FOR [MatchNo]
GO
ALTER TABLE [dbo].[StreamMatchHistory] ADD  CONSTRAINT [DF_StreamMatchHistory_MatchType]  DEFAULT ((0)) FOR [MatchType]
GO
ALTER TABLE [dbo].[StreamMatchHistory] ADD  CONSTRAINT [DF_StreamMatchHistory_IsPromoted]  DEFAULT ((0)) FOR [IsPromoted]
GO
ALTER TABLE [dbo].[StreamMatchHistory] ADD  CONSTRAINT [DF_StreamMatchHistory_RewardEntity]  DEFAULT ('') FOR [RewardEntity]
GO
ALTER TABLE [dbo].[StreamMatchHistory] ADD  CONSTRAINT [DF_StreamMatchHistory_RewardGold]  DEFAULT ('') FOR [RewardGoods]
GO
ALTER TABLE [dbo].[StreamMatchHistory] ADD  CONSTRAINT [DF_StreamMatchHistory_WinCount]  DEFAULT ((0)) FOR [WinCount]
GO
ALTER TABLE [dbo].[StreamMatchHistory] ADD  CONSTRAINT [DF_StreamMatchHistory_LostCount]  DEFAULT ((0)) FOR [LostCount]
GO
ALTER TABLE [dbo].[StreamMatchHistory] ADD  CONSTRAINT [DF_StreamMatchHistory_DrawCount]  DEFAULT ((0)) FOR [DrawCount]
GO
ALTER TABLE [dbo].[StreamMatchHistory] ADD  CONSTRAINT [DF_StreamMatchHistory_FleeCount]  DEFAULT ((0)) FOR [FleeCount]
GO
ALTER TABLE [dbo].[StreamMatchHistory] ADD  CONSTRAINT [DF_StreamMatchHistory_WinStreak_1]  DEFAULT ((0)) FOR [WinStreak]
GO
ALTER TABLE [dbo].[StreamMatchHistory] ADD  CONSTRAINT [DF_StreamMatchHistory_WinStreak1]  DEFAULT ((0)) FOR [MaxWinStreak]
GO
ALTER TABLE [dbo].[StreamMatchHistory] ADD  CONSTRAINT [DF_StreamMatchHistory_FristStartTime]  DEFAULT (getdate()) FOR [MatchStartTime]
GO
ALTER TABLE [dbo].[StreamMatchHistory] ADD  CONSTRAINT [DF_StreamMatchHistory_LastEndTime]  DEFAULT (getdate()) FOR [MatchFinishTime]
GO
ALTER TABLE [dbo].[StreamMatchHistory] ADD  CONSTRAINT [DF_StreamMatchHistory_PlayDureation]  DEFAULT ((0)) FOR [PlayTimeCount]
GO
ALTER TABLE [dbo].[StreamMatchHistory] ADD  CONSTRAINT [DF_StreamMatchHistory_RecordDate]  DEFAULT (getdate()) FOR [RecordDate]
GO
ALTER TABLE [dbo].[StreamMatchSession] ADD  CONSTRAINT [DF_StreamMatchSesstion_MatchStartTime]  DEFAULT (getdate()) FOR [MatchStartTime]
GO
ALTER TABLE [dbo].[StreamMatchSession] ADD  CONSTRAINT [DF_StreamMatchSesstion_MatchFinishTime]  DEFAULT (getdate()) FOR [MatchFinishTime]
GO
ALTER TABLE [dbo].[StreamMatchSession] ADD  CONSTRAINT [DF_StreamMatchSesstion_CollectTime]  DEFAULT (getdate()) FOR [CollectTime]
GO
ALTER TABLE [dbo].[SystemStatusInfo] ADD  CONSTRAINT [DF_SystemStatusInfo_StatusValue]  DEFAULT ((0)) FOR [StatusValue]
GO
ALTER TABLE [dbo].[SystemStatusInfo] ADD  CONSTRAINT [DF_SystemStatusInfo_StatusString]  DEFAULT ('') FOR [StatusString]
GO
ALTER TABLE [dbo].[SystemStreamInfo] ADD  CONSTRAINT [DF_TABLE1_LogonCount]  DEFAULT ((0)) FOR [LogonCount]
GO
ALTER TABLE [dbo].[SystemStreamInfo] ADD  CONSTRAINT [DF_GameEnterInfo_LogonCount1]  DEFAULT ((0)) FOR [LogOutCount]
GO
ALTER TABLE [dbo].[SystemStreamInfo] ADD  CONSTRAINT [DF_TABLE1_RecordDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排列标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameColumnItem', @level2type=N'COLUMN',@level2name=N'SortID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'列头名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameColumnItem', @level2type=N'COLUMN',@level2name=N'ColumnName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'列表宽度' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameColumnItem', @level2type=N'COLUMN',@level2name=N'ColumnWidth'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数据描述' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameColumnItem', @level2type=N'COLUMN',@level2name=N'DataDescribe'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户 ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户权限' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'UserRight'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理权限' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'MasterRight'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理等级' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'MasterOrder'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上次登陆 IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'LastLogonIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上次登陆时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'LastLogonDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总登陆次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'AllLogonTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'PlayTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'在线时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'OnLineTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchOption', @level2type=N'COLUMN',@level2name=N'MatchID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛状态（0x00： 为空闲   0x02：比赛中  0x08： 比赛结束）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchOption', @level2type=N'COLUMN',@level2name=N'MatchStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchOption', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchOption', @level2type=N'COLUMN',@level2name=N'MatchName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛类型 1: 定时赛 2:即时赛 4: 锦标赛' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchOption', @level2type=N'COLUMN',@level2name=N'MatchType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'报名次数（0、表示不限制）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchOption', @level2type=N'COLUMN',@level2name=N'SignupCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'报名方式  1:扣除费用  2、扣除比赛券  3:朋友圈分享免费报名' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchOption', @level2type=N'COLUMN',@level2name=N'SignupMode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'报名费用金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchOption', @level2type=N'COLUMN',@level2name=N'FeeAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'报名费用货币标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchOption', @level2type=N'COLUMN',@level2name=N'FeeMoneyID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'使用比赛券数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchOption', @level2type=N'COLUMN',@level2name=N'UseTicketCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'购买比赛券数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchOption', @level2type=N'COLUMN',@level2name=N'BuyTicketCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'购票选项（0x01、重置比赛分）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchOption', @level2type=N'COLUMN',@level2name=N'BuyTicketOption'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'购买比赛券金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchOption', @level2type=N'COLUMN',@level2name=N'BuyTicketAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'购买比赛券的货币标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchOption', @level2type=N'COLUMN',@level2name=N'BuyTicketMoneyID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'参赛条件  (1 表示会员等级 2表示经验值 4表示比赛玩家)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchOption', @level2type=N'COLUMN',@level2name=N'JoinCondition'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'会员等级' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchOption', @level2type=N'COLUMN',@level2name=N'MemberOrder'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'玩家经验' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchOption', @level2type=N'COLUMN',@level2name=N'Experience'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchOption', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'费用类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchReviveInfo', @level2type=N'COLUMN',@level2name=N'FeeMoneyID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'复活费用' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchReviveInfo', @level2type=N'COLUMN',@level2name=N'FeeAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'复活次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchReviveInfo', @level2type=N'COLUMN',@level2name=N'ReviveTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchScoreInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchScoreInfo', @level2type=N'COLUMN',@level2name=N'MatchID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'场次编号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchScoreInfo', @level2type=N'COLUMN',@level2name=N'MatchNo'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchScoreInfo', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchScoreInfo', @level2type=N'COLUMN',@level2name=N'ServerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'名次标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchScoreInfo', @level2type=N'COLUMN',@level2name=N'RankID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛分数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchScoreInfo', @level2type=N'COLUMN',@level2name=N'Score'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'胜利局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchScoreInfo', @level2type=N'COLUMN',@level2name=N'WinCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'失败局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchScoreInfo', @level2type=N'COLUMN',@level2name=N'LostCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'打平局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchScoreInfo', @level2type=N'COLUMN',@level2name=N'DrawCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'逃跑局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchScoreInfo', @level2type=N'COLUMN',@level2name=N'FleeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'连胜局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchScoreInfo', @level2type=N'COLUMN',@level2name=N'WinStreak'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛权限' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchScoreInfo', @level2type=N'COLUMN',@level2name=N'MatchRight'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏时长' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchScoreInfo', @level2type=N'COLUMN',@level2name=N'PlayTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'报名时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchScoreInfo', @level2type=N'COLUMN',@level2name=N'SignupTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'报名次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchScoreInfo', @level2type=N'COLUMN',@level2name=N'SignupCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'关联标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MatchScoreInfo', @level2type=N'COLUMN',@level2name=N'SignupRecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawInfo', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawInfo', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawInfo', @level2type=N'COLUMN',@level2name=N'ServerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'桌子号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawInfo', @level2type=N'COLUMN',@level2name=N'TableID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'损耗数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawInfo', @level2type=N'COLUMN',@level2name=N'WasteCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'税收数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawInfo', @level2type=N'COLUMN',@level2name=N'RevenueCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawInfo', @level2type=N'COLUMN',@level2name=N'PlayTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏过程' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawInfo', @level2type=N'COLUMN',@level2name=N'GameCourse'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'录像编号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawInfo', @level2type=N'COLUMN',@level2name=N'VideoNumber'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawInfo', @level2type=N'COLUMN',@level2name=N'RecordDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'索引标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'ID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'ServerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'锁定金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'LockerScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'进入时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'EnterTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'进入金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'EnterGold'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'进入积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'EnterScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'进入机器' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'EnterMachine'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'EnterClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'离开时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'LeaveTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'离开原因' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'LeaveReason'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'离开机器' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'LeaveMachine'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'LeaveClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变更金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'Gold'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变更积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'Score'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变更税收' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'Revenue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'胜局变更' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'WinCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'输局变更' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'LostCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'和局变更' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'DrawCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'逃局变更' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'FleeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'经验变更' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'Experience'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'魅力变更' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'LoveLiness'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'PlayTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'在线时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'OnLineTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'胜利积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserScore', @level2type=N'COLUMN',@level2name=N'GainScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'失败积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserScore', @level2type=N'COLUMN',@level2name=N'LoseScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'胜利局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserScore', @level2type=N'COLUMN',@level2name=N'WinCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'失败局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserScore', @level2type=N'COLUMN',@level2name=N'LostCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'和局总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserScore', @level2type=N'COLUMN',@level2name=N'DrawCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'逃跑局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserScore', @level2type=N'COLUMN',@level2name=N'FleeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户税费' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserScore', @level2type=N'COLUMN',@level2name=N'UserRevenue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏时长' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserScore', @level2type=N'COLUMN',@level2name=N'PlayTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'写入时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserScore', @level2type=N'COLUMN',@level2name=N'InsertDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修改时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserScore', @level2type=N'COLUMN',@level2name=N'ModifyDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchFeeInfo', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchFeeInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchFeeInfo', @level2type=N'COLUMN',@level2name=N'MatchID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛序号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchFeeInfo', @level2type=N'COLUMN',@level2name=N'MatchNo'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchFeeInfo', @level2type=N'COLUMN',@level2name=N'ServerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'1；扣费  2：分享' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchFeeInfo', @level2type=N'COLUMN',@level2name=N'SignupMode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'费用类型（0：金币   1： 奖牌 2：钻石）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchFeeInfo', @level2type=N'COLUMN',@level2name=N'FeeMoneyID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'报名费' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchFeeInfo', @level2type=N'COLUMN',@level2name=N'FeeAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赛券数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchFeeInfo', @level2type=N'COLUMN',@level2name=N'UseTicketCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchFeeInfo', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchHistory', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchHistory', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchHistory', @level2type=N'COLUMN',@level2name=N'MatchID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛场次' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchHistory', @level2type=N'COLUMN',@level2name=N'MatchNo'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchHistory', @level2type=N'COLUMN',@level2name=N'MatchType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchHistory', @level2type=N'COLUMN',@level2name=N'ServerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛名次' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchHistory', @level2type=N'COLUMN',@level2name=N'RankID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛得分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchHistory', @level2type=N'COLUMN',@level2name=N'MatchScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'晋级标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchHistory', @level2type=N'COLUMN',@level2name=N'IsPromoted'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'奖励金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchHistory', @level2type=N'COLUMN',@level2name=N'RewardGoods'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赢的局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchHistory', @level2type=N'COLUMN',@level2name=N'WinCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'输的局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchHistory', @level2type=N'COLUMN',@level2name=N'LostCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'平的局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchHistory', @level2type=N'COLUMN',@level2name=N'DrawCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'逃跑局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchHistory', @level2type=N'COLUMN',@level2name=N'FleeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'连胜局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchHistory', @level2type=N'COLUMN',@level2name=N'WinStreak'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'连胜局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchHistory', @level2type=N'COLUMN',@level2name=N'MaxWinStreak'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛第一局开始时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchHistory', @level2type=N'COLUMN',@level2name=N'MatchStartTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛最后一句结束时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchHistory', @level2type=N'COLUMN',@level2name=N'MatchFinishTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏时长 单位:秒' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchHistory', @level2type=N'COLUMN',@level2name=N'PlayTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'录入时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchHistory', @level2type=N'COLUMN',@level2name=N'RecordDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'报名用户数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchSession', @level2type=N'COLUMN',@level2name=N'SingupUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'参赛用户数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchSession', @level2type=N'COLUMN',@level2name=N'MatchUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'人均参赛次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchSession', @level2type=N'COLUMN',@level2name=N'AvgMatchTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总参赛次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchSession', @level2type=N'COLUMN',@level2name=N'TotalMatchTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'sp消耗数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchSession', @level2type=N'COLUMN',@level2name=N'ConsumeIngot'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛开始时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchSession', @level2type=N'COLUMN',@level2name=N'MatchStartTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛结束时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchSession', @level2type=N'COLUMN',@level2name=N'MatchFinishTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'购买次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchTicket', @level2type=N'COLUMN',@level2name=N'BuyTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'购买赛券花费的金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchTicket', @level2type=N'COLUMN',@level2name=N'BuyAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'购买赛券使用的货币标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchTicket', @level2type=N'COLUMN',@level2name=N'BuyMoneyID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'购买的比赛券总数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchTicket', @level2type=N'COLUMN',@level2name=N'BuyTicketCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'购买的比赛券总数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchTicket', @level2type=N'COLUMN',@level2name=N'UseTicketCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'更新时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamMatchTicket', @level2type=N'COLUMN',@level2name=N'UpdateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'状态名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStatusInfo', @level2type=N'COLUMN',@level2name=N'StatusName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'状态数值' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStatusInfo', @level2type=N'COLUMN',@level2name=N'StatusValue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'状态字符' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStatusInfo', @level2type=N'COLUMN',@level2name=N'StatusString'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamInfo', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamInfo', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamInfo', @level2type=N'COLUMN',@level2name=N'ServerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'进入数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamInfo', @level2type=N'COLUMN',@level2name=N'LogonCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'离开数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamInfo', @level2type=N'COLUMN',@level2name=N'LogOutCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'录入时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamInfo', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
