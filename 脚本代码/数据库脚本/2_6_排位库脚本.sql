USE [LHGameRankingDB]
GO
/****** Object:  Table [dbo].[GameColumnItem]    Script Date: 2023/7/12 11:07:59 ******/
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
/****** Object:  Table [dbo].[GameScoreInfo]    Script Date: 2023/7/12 11:07:59 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameScoreInfo](
	[UserID] [int] NOT NULL,
	[Score] [bigint] NOT NULL,
	[LoveHeart] [int] NOT NULL,
	[WinCount] [int] NOT NULL,
	[LostCount] [int] NOT NULL,
	[DrawCount] [int] NOT NULL,
	[FleeCount] [int] NOT NULL,
	[WinStreak] [int] NOT NULL,
	[MaxWinStreak] [int] NOT NULL,
	[MainOrder] [smallint] NOT NULL,
	[SubOrder] [smallint] NOT NULL,
	[StarOrder] [smallint] NOT NULL,
	[LogonTimes] [int] NOT NULL,
	[PlayTimeCount] [int] NOT NULL,
	[OnLineTimeCount] [int] NOT NULL,
	[LogonIP] [nvarchar](15) NOT NULL,
	[LogonDate] [datetime] NOT NULL,
	[LogonMachineID] [nvarchar](32) NOT NULL,
	[RegisterIP] [nvarchar](15) NOT NULL,
	[RegisterDate] [datetime] NOT NULL,
	[RegisterMachineID] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_GameScoreInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordDrawInfo]    Script Date: 2023/7/12 11:07:59 ******/
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
	[RecordDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordDrawInfo_RecordID] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordDrawScore]    Script Date: 2023/7/12 11:07:59 ******/
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
	[RecordDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordGameScore]    Script Date: 2023/7/12 11:07:59 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordGameScore](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[KindID] [int] NOT NULL,
	[ServerID] [int] NOT NULL,
	[TableID] [int] NOT NULL,
	[PlayTimeCount] [int] NOT NULL,
	[UserID1] [int] NOT NULL,
	[GameScore1] [bigint] NOT NULL,
	[UserID2] [int] NOT NULL,
	[GameScore2] [bigint] NOT NULL,
	[UserID3] [int] NOT NULL,
	[GameScore3] [bigint] NOT NULL,
	[UserID4] [int] NOT NULL,
	[GameScore4] [bigint] NOT NULL,
	[UserID5] [int] NOT NULL,
	[GameScore5] [bigint] NOT NULL,
	[UserID6] [int] NOT NULL,
	[GameScore6] [bigint] NOT NULL,
	[UserID7] [int] NOT NULL,
	[GameScore7] [bigint] NOT NULL,
	[UserID8] [int] NOT NULL,
	[GameScore8] [bigint] NOT NULL,
	[VideoNumber] [nvarchar](21) NOT NULL,
	[RecordDate] [datetime] NOT NULL,
 CONSTRAINT [PK_GameScoreRecord_RecordID] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordRankHistory]    Script Date: 2023/7/12 11:07:59 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordRankHistory](
	[UserID] [int] NOT NULL,
	[SeasonID] [int] NOT NULL,
	[RankScore] [int] NOT NULL,
	[MainOrder] [smallint] NOT NULL,
	[SubOrder] [smallint] NOT NULL,
	[StarOrder] [smallint] NOT NULL,
	[CollectTime] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordRankHistory] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC,
	[SeasonID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordRankReward]    Script Date: 2023/7/12 11:07:59 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordRankReward](
	[UserID] [int] NOT NULL,
	[MainOrder] [smallint] NOT NULL,
	[MailID] [bigint] NOT NULL,
	[ExpireTime] [int] NOT NULL,
	[SendState] [tinyint] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordUserInout]    Script Date: 2023/7/12 11:07:59 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordUserInout](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[ServerID] [int] NOT NULL,
	[LockerScore] [int] NOT NULL,
	[EnterTime] [datetime] NOT NULL,
	[EnterScore] [bigint] NOT NULL,
	[EnterMachine] [nvarchar](32) NOT NULL,
	[EnterClientIP] [nvarchar](15) NOT NULL,
	[LeaveTime] [datetime] NULL,
	[LeaveReason] [int] NULL,
	[LeaveMachine] [nvarchar](32) NULL,
	[LeaveClientIP] [nvarchar](15) NULL,
	[Score] [bigint] NULL,
	[WinCount] [int] NULL,
	[LostCount] [int] NULL,
	[DrawCount] [int] NULL,
	[FleeCount] [int] NULL,
	[Experience] [int] NULL,
	[PlayTimeCount] [int] NULL,
	[OnLineTimeCount] [int] NULL,
 CONSTRAINT [PK_RecordUserInout_1] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReportUserScore]    Script Date: 2023/7/12 11:07:59 ******/
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
/****** Object:  Table [dbo].[StreamWasteInfo]    Script Date: 2023/7/12 11:07:59 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[StreamWasteInfo](
	[DateID] [int] NOT NULL,
	[TypeID] [int] NOT NULL,
	[WasteScoreStat] [bigint] NOT NULL,
	[FirstWasteDate] [datetime] NOT NULL,
	[LastWasteDate] [datetime] NOT NULL,
 CONSTRAINT [PK_StreamWasteInfo] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[TypeID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[SystemStatusInfo]    Script Date: 2023/7/12 11:07:59 ******/
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
/****** Object:  Table [dbo].[SystemStreamInfo]    Script Date: 2023/7/12 11:07:59 ******/
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
/****** Object:  Table [dbo].[UserRankingInfo]    Script Date: 2023/7/12 11:07:59 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[UserRankingInfo](
	[UserID] [int] NOT NULL,
	[RankID] [bigint] NOT NULL,
	[Score] [bigint] NOT NULL,
 CONSTRAINT [PK_UserRankingInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
ALTER TABLE [dbo].[GameColumnItem] ADD  CONSTRAINT [DF_GameColumnItem_SuportType]  DEFAULT ((0)) FOR [SuportType]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_Score]  DEFAULT ((501)) FOR [Score]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_LoveHeart]  DEFAULT ((3)) FOR [LoveHeart]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_WinCount]  DEFAULT ((0)) FOR [WinCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_LostCount]  DEFAULT ((0)) FOR [LostCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_DrawCount]  DEFAULT ((0)) FOR [DrawCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_FleeCount]  DEFAULT ((0)) FOR [FleeCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_MaxWinStreak1]  DEFAULT ((0)) FOR [WinStreak]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_MaxWinStreak]  DEFAULT ((0)) FOR [MaxWinStreak]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_MainOrder]  DEFAULT ((6)) FOR [MainOrder]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_SubOrder]  DEFAULT ((3)) FOR [SubOrder]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_StarOrder]  DEFAULT ((3)) FOR [StarOrder]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_AllLogonTimes]  DEFAULT ((0)) FOR [LogonTimes]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_PlayTimeCount_1]  DEFAULT ((0)) FOR [PlayTimeCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_OnLineTimeCount]  DEFAULT ((0)) FOR [OnLineTimeCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_LogonIP]  DEFAULT (N'') FOR [LogonIP]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_LastLogonDate]  DEFAULT (getdate()) FOR [LogonDate]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_LastLogonMachine]  DEFAULT ('------------') FOR [LogonMachineID]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_RegisterIP]  DEFAULT (N'') FOR [RegisterIP]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_RegisterDate]  DEFAULT (getdate()) FOR [RegisterDate]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_RegisterMachine]  DEFAULT (N'------------') FOR [RegisterMachineID]
GO
ALTER TABLE [dbo].[RecordDrawInfo] ADD  CONSTRAINT [DF_RecordDrawInfo_GameCourse]  DEFAULT ('') FOR [GameCourse]
GO
ALTER TABLE [dbo].[RecordDrawInfo] ADD  CONSTRAINT [DF_RecordDrawInfo_VideoNumber]  DEFAULT ('') FOR [VideoNumber]
GO
ALTER TABLE [dbo].[RecordDrawInfo] ADD  CONSTRAINT [DF_RecordDrawInfo_RecordDate]  DEFAULT (getdate()) FOR [RecordDate]
GO
ALTER TABLE [dbo].[RecordDrawScore] ADD  CONSTRAINT [DF_RecordDrawScore_VideoNumber]  DEFAULT (N'') FOR [VideoNumber]
GO
ALTER TABLE [dbo].[RecordDrawScore] ADD  CONSTRAINT [DF_RecordDrawScore_RecordDate]  DEFAULT (getdate()) FOR [RecordDate]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_GameScoreRecord_UserID1]  DEFAULT ((0)) FOR [UserID1]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_GameScoreRecord_GameScore1]  DEFAULT ((0)) FOR [GameScore1]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_GameScoreRecord_UserID2]  DEFAULT ((0)) FOR [UserID2]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_GameScoreRecord_GameScore2]  DEFAULT ((0)) FOR [GameScore2]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_GameScoreRecord_UserID3]  DEFAULT ((0)) FOR [UserID3]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_GameScoreRecord_GameScore3]  DEFAULT ((0)) FOR [GameScore3]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_GameScoreRecord_UserID4]  DEFAULT ((0)) FOR [UserID4]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_GameScoreRecord_GameScore4]  DEFAULT ((0)) FOR [GameScore4]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_GameScoreRecord_UserID5]  DEFAULT ((0)) FOR [UserID5]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_GameScoreRecord_GameScore5]  DEFAULT ((0)) FOR [GameScore5]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_GameScoreRecord_UserID6]  DEFAULT ((0)) FOR [UserID6]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_GameScoreRecord_GameScore6]  DEFAULT ((0)) FOR [GameScore6]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_GameScoreRecord_UserID7]  DEFAULT ((0)) FOR [UserID7]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_GameScoreRecord_GameScore7]  DEFAULT ((0)) FOR [GameScore7]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_GameScoreRecord_UserID8]  DEFAULT ((0)) FOR [UserID8]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_GameScoreRecord_GameScore8]  DEFAULT ((0)) FOR [GameScore8]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_RecordGameScore_VideoNumber]  DEFAULT (N'‘’') FOR [VideoNumber]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_GameScoreRecord_RecordDate]  DEFAULT (getdate()) FOR [RecordDate]
GO
ALTER TABLE [dbo].[RecordRankHistory] ADD  CONSTRAINT [DF_RecordRankHistory_RankMainOrder]  DEFAULT ((0)) FOR [MainOrder]
GO
ALTER TABLE [dbo].[RecordRankHistory] ADD  CONSTRAINT [DF_RecordRankHistory_SubOrder]  DEFAULT ((0)) FOR [SubOrder]
GO
ALTER TABLE [dbo].[RecordRankHistory] ADD  CONSTRAINT [DF_RecordRankHistory_StarOrder]  DEFAULT ((0)) FOR [StarOrder]
GO
ALTER TABLE [dbo].[RecordRankHistory] ADD  CONSTRAINT [DF_RecordRankInfo_CollectTime]  DEFAULT (getdate()) FOR [CollectTime]
GO
ALTER TABLE [dbo].[RecordRankReward] ADD  CONSTRAINT [DF_RecordRewardMail_MainOrder]  DEFAULT ((0)) FOR [MainOrder]
GO
ALTER TABLE [dbo].[RecordRankReward] ADD  CONSTRAINT [DF_RecordRewardMail_ExpireTime]  DEFAULT ((0)) FOR [ExpireTime]
GO
ALTER TABLE [dbo].[RecordUserInout] ADD  CONSTRAINT [DF_RecordUserInout_LockerScore]  DEFAULT ((0)) FOR [LockerScore]
GO
ALTER TABLE [dbo].[RecordUserInout] ADD  CONSTRAINT [DF_RecordUserInout_EnterTime]  DEFAULT (getdate()) FOR [EnterTime]
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
ALTER TABLE [dbo].[ReportUserScore] ADD  CONSTRAINT [DF_ReportUserScore_UserService]  DEFAULT ((0)) FOR [UserService]
GO
ALTER TABLE [dbo].[ReportUserScore] ADD  CONSTRAINT [DF_ReportUserScore_UserRevenue]  DEFAULT ((0)) FOR [UserRevenue]
GO
ALTER TABLE [dbo].[ReportUserScore] ADD  CONSTRAINT [DF_ReportUserScore_PlayTimeCount]  DEFAULT ((0)) FOR [PlayTimeCount]
GO
ALTER TABLE [dbo].[ReportUserScore] ADD  CONSTRAINT [DF_ReportUserScore_InsertDateTime]  DEFAULT (getdate()) FOR [InsertDateTime]
GO
ALTER TABLE [dbo].[ReportUserScore] ADD  CONSTRAINT [DF_ReportUserScore_ModifyDateTime]  DEFAULT (getdate()) FOR [ModifyDateTime]
GO
ALTER TABLE [dbo].[StreamWasteInfo] ADD  CONSTRAINT [DF_StreamWasteInfo_FirstWasteDate]  DEFAULT (getdate()) FOR [FirstWasteDate]
GO
ALTER TABLE [dbo].[StreamWasteInfo] ADD  CONSTRAINT [DF_StreamWasteInfo_LastWasteDate]  DEFAULT (getdate()) FOR [LastWasteDate]
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
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户积分（货币）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'Score'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'爱心值' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'LoveHeart'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'胜局数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'WinCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'输局数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'LostCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'和局数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'DrawCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'逃局数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'FleeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'连胜局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'WinStreak'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最大连胜' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'MaxWinStreak'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总登陆次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'LogonTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'PlayTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'在线时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'OnLineTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上次登陆 IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'LogonIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上次登陆时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'LogonDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录机器' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'LogonMachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'注册 IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'RegisterIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'注册时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'RegisterDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'注册机器' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'RegisterMachineID'
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
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'ServerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'桌子号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'TableID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'PlayTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'UserID1'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'GameScore1'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'UserID2'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'GameScore2'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'UserID3'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'GameScore3'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'UserID4'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'GameScore4'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'UserID5'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'GameScore5'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'UserID6'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'GameScore6'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'UserID7'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'GameScore7'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'UserID8'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'GameScore8'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'RecordDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'段位标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordRankReward', @level2type=N'COLUMN',@level2name=N'MainOrder'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'索引标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'ID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'ServerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'进入时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'EnterTime'
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
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变更积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'Score'
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
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserRankingInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排名ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserRankingInfo', @level2type=N'COLUMN',@level2name=N'RankID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排位分数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserRankingInfo', @level2type=N'COLUMN',@level2name=N'Score'
GO
