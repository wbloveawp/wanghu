USE [LHGameGoldDB]
GO
/****** Object:  Table [dbo].[CheckInItem]    Script Date: 2023/7/12 11:05:58 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[CheckInItem](
	[ItemIndex] [int] NOT NULL,
	[ItemCount] [int] NOT NULL,
	[Remark] [nvarchar](50) NOT NULL,
 CONSTRAINT [PK_CheckInItem_1] PRIMARY KEY CLUSTERED 
(
	[ItemIndex] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameColumnItem]    Script Date: 2023/7/12 11:05:58 ******/
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
/****** Object:  Table [dbo].[GameControlInfo]    Script Date: 2023/7/12 11:05:58 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameControlInfo](
	[UserID] [int] NOT NULL,
	[ControlKind] [tinyint] NOT NULL,
	[ControlMode] [tinyint] NOT NULL,
	[ControlStatus] [tinyint] NOT NULL,
	[TargetWinRate] [smallint] NOT NULL,
	[ControlWinRate] [smallint] NOT NULL,
	[InitailWinRate] [smallint] NOT NULL,
	[InitailWinScore] [bigint] NOT NULL,
	[InitailLoseScore] [bigint] NOT NULL,
	[TotalWinCount] [int] NOT NULL,
	[TotalLoseCount] [int] NOT NULL,
	[TotalWinScore] [bigint] NOT NULL,
	[TotalLoseScore] [bigint] NOT NULL,
	[TotalBetAmount] [bigint] NOT NULL,
	[ControlDuration] [int] NOT NULL,
	[ControlPlayCount] [int] NOT NULL,
	[ControlStartTime] [datetime] NOT NULL,
	[OperaterMasterID] [int] NOT NULL,
	[OperateRemark] [nvarchar](128) NOT NULL,
 CONSTRAINT [PK_[GameControlInfo_1] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameScoreInfo]    Script Date: 2023/7/12 11:05:58 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameScoreInfo](
	[UserID] [int] NOT NULL,
	[BetAmount] [bigint] NOT NULL,
	[WinScore] [bigint] NOT NULL,
	[LostScore] [bigint] NOT NULL,
	[LockerScore] [bigint] NOT NULL,
	[WinCount] [int] NOT NULL,
	[LostCount] [int] NOT NULL,
	[DrawCount] [int] NOT NULL,
	[FleeCount] [int] NOT NULL,
	[WinStreak] [int] NOT NULL,
	[MaxWinStreak] [int] NOT NULL,
	[WasteCount] [bigint] NOT NULL,
	[ServiceCount] [bigint] NOT NULL,
	[LogonTimes] [int] NOT NULL,
	[PlayTimeCount] [int] NOT NULL,
	[OnLineTimeCount] [int] NOT NULL,
	[AddupWinRate] [smallint] NOT NULL,
	[AddupWinScore] [bigint] NOT NULL,
	[AddupLoseScore] [bigint] NOT NULL,
	[RegisterIP] [nvarchar](15) NOT NULL,
	[RegisterDate] [datetime] NOT NULL,
	[RegisterMachineID] [nvarchar](32) NOT NULL,
	[LogonIP] [nvarchar](15) NOT NULL,
	[LogonDate] [datetime] NOT NULL,
	[LogonMachineID] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_GameScoreInfo_1] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordDrawInfo]    Script Date: 2023/7/12 11:05:58 ******/
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
/****** Object:  Table [dbo].[RecordDrawScore]    Script Date: 2023/7/12 11:05:58 ******/
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
	[BetAmount] [bigint] NOT NULL,
	[BeforeScore] [bigint] NOT NULL,
	[VideoNumber] [nvarchar](21) NOT NULL,
	[RecordDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordDrawScore] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordGameControl]    Script Date: 2023/7/12 11:05:58 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordGameControl](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[ControlKind] [tinyint] NOT NULL,
	[ControlMode] [tinyint] NOT NULL,
	[ControlStatus] [tinyint] NOT NULL,
	[TargetWinRate] [smallint] NOT NULL,
	[ControlWinRate] [smallint] NOT NULL,
	[InitailWinRate] [smallint] NOT NULL,
	[InitailWinScore] [int] NOT NULL,
	[InitailLoseScore] [int] NOT NULL,
	[TotalWinCount] [int] NOT NULL,
	[TotalLoseCount] [int] NOT NULL,
	[TotalWinScore] [int] NOT NULL,
	[TotalLoseScore] [int] NOT NULL,
	[TotalBetAmount] [bigint] NOT NULL,
	[ControlDuration] [int] NOT NULL,
	[ControlPlayCount] [int] NOT NULL,
	[ControlStartTime] [datetime] NOT NULL,
	[ControlFinishTime] [datetime] NOT NULL,
	[OperaterUserID] [int] NOT NULL,
	[OperateRemark] [nvarchar](128) NOT NULL,
 CONSTRAINT [PK_[RecordGameControl_1] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordUserInout]    Script Date: 2023/7/12 11:05:58 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordUserInout](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[ServerID] [int] NOT NULL,
	[DeviceType] [smallint] NOT NULL,
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
/****** Object:  Table [dbo].[RecordUserInsure]    Script Date: 2023/7/12 11:05:58 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordUserInsure](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[SrcUserID] [int] NOT NULL,
	[SrcGameID] [int] NOT NULL,
	[SrcAccounts] [nvarchar](31) NOT NULL,
	[SrcScore] [bigint] NOT NULL,
	[SrcInsure] [bigint] NOT NULL,
	[SrcLockerScore] [bigint] NOT NULL,
	[DstUserID] [int] NOT NULL,
	[DstGameID] [int] NOT NULL,
	[DstAccounts] [nvarchar](31) NOT NULL,
	[DstScore] [bigint] NOT NULL,
	[DstInsure] [bigint] NOT NULL,
	[DstLockerScore] [bigint] NOT NULL,
	[TradeScore] [bigint] NOT NULL,
	[Revenue] [bigint] NOT NULL,
	[RevenueScale] [real] NOT NULL,
	[MinTradeScore] [bigint] NOT NULL,
	[TradeType] [tinyint] NOT NULL,
	[ClientIP] [nvarchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[CollectNote] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_RecordInsureUser] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReportUserCommission]    Script Date: 2023/7/12 11:05:58 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReportUserCommission](
	[DateID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[ParentID] [int] NOT NULL,
	[PParentID] [int] NOT NULL,
	[ParentPayAmount] [bigint] NOT NULL,
	[PParentPayAmount] [bigint] NOT NULL,
	[ParentBetAmount] [bigint] NOT NULL,
	[PParentBetAmount] [bigint] NOT NULL,
	[ParentPayContribute] [bigint] NOT NULL,
	[PParentPayContribute] [bigint] NOT NULL,
	[ParentBetContribute] [bigint] NOT NULL,
	[PParentBetContribute] [bigint] NOT NULL,
	[BalanceState] [tinyint] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordUserCommission] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReportUserKind]    Script Date: 2023/7/12 11:05:58 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReportUserKind](
	[DateID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[GainScore] [bigint] NOT NULL,
	[LoseScore] [bigint] NOT NULL,
	[WinCount] [int] NOT NULL,
	[LostCount] [int] NOT NULL,
	[DrawCount] [int] NOT NULL,
	[FleeCount] [int] NOT NULL,
	[UserService] [bigint] NOT NULL,
	[UserRevenue] [bigint] NOT NULL,
	[UserBetAmount] [bigint] NOT NULL,
	[PlayTimeCount] [int] NOT NULL,
	[InsertDateTime] [datetime] NOT NULL,
	[ModifyDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_ReportUserKind] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[UserID] ASC,
	[KindID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReportUserScore]    Script Date: 2023/7/12 11:05:58 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReportUserScore](
	[DateID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[GainScore] [bigint] NOT NULL,
	[LoseScore] [bigint] NOT NULL,
	[WinCount] [int] NOT NULL,
	[LostCount] [int] NOT NULL,
	[DrawCount] [int] NOT NULL,
	[FleeCount] [int] NOT NULL,
	[UserService] [bigint] NOT NULL,
	[UserRevenue] [bigint] NOT NULL,
	[UserBetAmount] [bigint] NOT NULL,
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
/****** Object:  Table [dbo].[StreamWasteInfo]    Script Date: 2023/7/12 11:05:58 ******/
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
/****** Object:  Table [dbo].[SystemStatusInfo]    Script Date: 2023/7/12 11:05:58 ******/
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
/****** Object:  Table [dbo].[SystemStreamInfo]    Script Date: 2023/7/12 11:05:58 ******/
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
ALTER TABLE [dbo].[CheckInItem] ADD  CONSTRAINT [DF_CheckInItem_ItemCount]  DEFAULT ((0)) FOR [ItemCount]
GO
ALTER TABLE [dbo].[CheckInItem] ADD  CONSTRAINT [DF_CheckInItem_Remark]  DEFAULT ('') FOR [Remark]
GO
ALTER TABLE [dbo].[GameColumnItem] ADD  CONSTRAINT [DF_GameColumnItem_SuportType_1]  DEFAULT ((0)) FOR [SuportType]
GO
ALTER TABLE [dbo].[GameControlInfo] ADD  CONSTRAINT [DF_GameControlInfo_ControlKind]  DEFAULT ((0)) FOR [ControlKind]
GO
ALTER TABLE [dbo].[GameControlInfo] ADD  CONSTRAINT [DF_GameControlInfo_ControlMode]  DEFAULT ((0)) FOR [ControlMode]
GO
ALTER TABLE [dbo].[GameControlInfo] ADD  CONSTRAINT [DF_GameControlInfo_ControlStatus]  DEFAULT ((0)) FOR [ControlStatus]
GO
ALTER TABLE [dbo].[GameControlInfo] ADD  CONSTRAINT [DF_GameControlInfo_TargetWinRate]  DEFAULT ((0)) FOR [TargetWinRate]
GO
ALTER TABLE [dbo].[GameControlInfo] ADD  CONSTRAINT [DF_GameControlInfo_ControlRate]  DEFAULT ((0)) FOR [ControlWinRate]
GO
ALTER TABLE [dbo].[GameControlInfo] ADD  CONSTRAINT [DF_GameControlInfo_SourceWinRate]  DEFAULT ((0)) FOR [InitailWinRate]
GO
ALTER TABLE [dbo].[GameControlInfo] ADD  CONSTRAINT [DF_GameControlInfo_InitailWinScore]  DEFAULT ((0)) FOR [InitailWinScore]
GO
ALTER TABLE [dbo].[GameControlInfo] ADD  CONSTRAINT [DF_GameControlInfo_InitailLoseScore]  DEFAULT ((0)) FOR [InitailLoseScore]
GO
ALTER TABLE [dbo].[GameControlInfo] ADD  CONSTRAINT [DF_GameControlInfo_WinCount]  DEFAULT ((0)) FOR [TotalWinCount]
GO
ALTER TABLE [dbo].[GameControlInfo] ADD  CONSTRAINT [DF_GameControlInfo_LoseCount]  DEFAULT ((0)) FOR [TotalLoseCount]
GO
ALTER TABLE [dbo].[GameControlInfo] ADD  CONSTRAINT [DF_Table_1_TotalWinScore]  DEFAULT ((0)) FOR [TotalWinScore]
GO
ALTER TABLE [dbo].[GameControlInfo] ADD  CONSTRAINT [DF_Table_1_TotalLoseScore]  DEFAULT ((0)) FOR [TotalLoseScore]
GO
ALTER TABLE [dbo].[GameControlInfo] ADD  CONSTRAINT [DF_GameControlInfo_TotalBetAmount]  DEFAULT ((0)) FOR [TotalBetAmount]
GO
ALTER TABLE [dbo].[GameControlInfo] ADD  CONSTRAINT [DF_GameControlInfo_ControlDuration]  DEFAULT ((0)) FOR [ControlDuration]
GO
ALTER TABLE [dbo].[GameControlInfo] ADD  CONSTRAINT [DF_GameControlInfo_ControlPlayCount]  DEFAULT ((0)) FOR [ControlPlayCount]
GO
ALTER TABLE [dbo].[GameControlInfo] ADD  CONSTRAINT [DF_GameControlInfo_CollectDate]  DEFAULT (getdate()) FOR [ControlStartTime]
GO
ALTER TABLE [dbo].[GameControlInfo] ADD  CONSTRAINT [DF_GameControlInfo_OperaterUserID]  DEFAULT ((0)) FOR [OperaterMasterID]
GO
ALTER TABLE [dbo].[GameControlInfo] ADD  CONSTRAINT [DF_GameControlInfo_OperateRemark]  DEFAULT ('') FOR [OperateRemark]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_BetAmount]  DEFAULT ((0)) FOR [BetAmount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_WinScore_1]  DEFAULT ((0)) FOR [WinScore]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_LostScore_1]  DEFAULT ((0)) FOR [LostScore]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_LockedScore]  DEFAULT ((0)) FOR [LockerScore]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_WinCount]  DEFAULT ((0)) FOR [WinCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_LostCount]  DEFAULT ((0)) FOR [LostCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_DrawCount]  DEFAULT ((0)) FOR [DrawCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_FleeCount]  DEFAULT ((0)) FOR [FleeCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_WinStreak]  DEFAULT ((0)) FOR [WinStreak]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_MaxWinStreak_1]  DEFAULT ((0)) FOR [MaxWinStreak]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_WasteCount]  DEFAULT ((0)) FOR [WasteCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_ServiceCount]  DEFAULT ((0)) FOR [ServiceCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_AllLogonTimes]  DEFAULT ((0)) FOR [LogonTimes]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_PlayTimeCount_1]  DEFAULT ((0)) FOR [PlayTimeCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_OnLineTimeCount]  DEFAULT ((0)) FOR [OnLineTimeCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_AddupWinRate]  DEFAULT ((0)) FOR [AddupWinRate]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_AddupWinScore]  DEFAULT ((0)) FOR [AddupWinScore]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_AddupLoseScore]  DEFAULT ((0)) FOR [AddupLoseScore]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_RegisteIP]  DEFAULT (N'') FOR [RegisterIP]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_LastLogonDate1]  DEFAULT (getdate()) FOR [RegisterDate]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_LastLogonMachine1]  DEFAULT ('------------') FOR [RegisterMachineID]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_LogonIP]  DEFAULT (N'') FOR [LogonIP]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_LastLogonDate]  DEFAULT (getdate()) FOR [LogonDate]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_LastLogonMachine]  DEFAULT ('------------') FOR [LogonMachineID]
GO
ALTER TABLE [dbo].[RecordDrawInfo] ADD  CONSTRAINT [DF_RecordDrawInfo_GameCourse]  DEFAULT ('') FOR [GameCourse]
GO
ALTER TABLE [dbo].[RecordDrawInfo] ADD  CONSTRAINT [DF_RecordDrawInfo_VideoNumber]  DEFAULT ('') FOR [VideoNumber]
GO
ALTER TABLE [dbo].[RecordDrawInfo] ADD  CONSTRAINT [DF_RecordDrawInfo_RecordDate]  DEFAULT (getdate()) FOR [RecordDate]
GO
ALTER TABLE [dbo].[RecordDrawScore] ADD  CONSTRAINT [DF_RecordDrawScore_BetAmount]  DEFAULT ((0)) FOR [BetAmount]
GO
ALTER TABLE [dbo].[RecordDrawScore] ADD  CONSTRAINT [DF_RecordDrawScore_DeforeScore]  DEFAULT ((0)) FOR [BeforeScore]
GO
ALTER TABLE [dbo].[RecordDrawScore] ADD  CONSTRAINT [DF_RecordDrawScore_VideoNumber]  DEFAULT (N'') FOR [VideoNumber]
GO
ALTER TABLE [dbo].[RecordDrawScore] ADD  CONSTRAINT [DF_RecordDrawScore_RecordDate]  DEFAULT (getdate()) FOR [RecordDate]
GO
ALTER TABLE [dbo].[RecordGameControl] ADD  CONSTRAINT [DF_RecordGameControl_ControlKind]  DEFAULT ((0)) FOR [ControlKind]
GO
ALTER TABLE [dbo].[RecordGameControl] ADD  CONSTRAINT [DF_RecordGameControl_ControlMode]  DEFAULT ((0)) FOR [ControlMode]
GO
ALTER TABLE [dbo].[RecordGameControl] ADD  CONSTRAINT [DF_RecordGameControl_ControlStatus]  DEFAULT ((0)) FOR [ControlStatus]
GO
ALTER TABLE [dbo].[RecordGameControl] ADD  CONSTRAINT [DF_RecordGameControl_TargetWinRate]  DEFAULT ((0)) FOR [TargetWinRate]
GO
ALTER TABLE [dbo].[RecordGameControl] ADD  CONSTRAINT [DF_RecordGameControl_ControlRate]  DEFAULT ((0)) FOR [ControlWinRate]
GO
ALTER TABLE [dbo].[RecordGameControl] ADD  CONSTRAINT [DF_RecordGameControl_SourceWinRate]  DEFAULT ((0)) FOR [InitailWinRate]
GO
ALTER TABLE [dbo].[RecordGameControl] ADD  CONSTRAINT [DF_RecordGameControl_InitailWinScore]  DEFAULT ((0)) FOR [InitailWinScore]
GO
ALTER TABLE [dbo].[RecordGameControl] ADD  CONSTRAINT [DF_RecordGameControl_InitailLoseScore]  DEFAULT ((0)) FOR [InitailLoseScore]
GO
ALTER TABLE [dbo].[RecordGameControl] ADD  CONSTRAINT [DF_RecordGameControl_WinCount]  DEFAULT ((0)) FOR [TotalWinCount]
GO
ALTER TABLE [dbo].[RecordGameControl] ADD  CONSTRAINT [DF_RecordGameControl_LoseCount]  DEFAULT ((0)) FOR [TotalLoseCount]
GO
ALTER TABLE [dbo].[RecordGameControl] ADD  CONSTRAINT [DF_RecordGameControl_1_TotalWinScore]  DEFAULT ((0)) FOR [TotalWinScore]
GO
ALTER TABLE [dbo].[RecordGameControl] ADD  CONSTRAINT [DF_RecordGameControl_1_TotalLoseScore]  DEFAULT ((0)) FOR [TotalLoseScore]
GO
ALTER TABLE [dbo].[RecordGameControl] ADD  CONSTRAINT [DF_RecordGameControl_TotalBetAmount]  DEFAULT ((0)) FOR [TotalBetAmount]
GO
ALTER TABLE [dbo].[RecordGameControl] ADD  CONSTRAINT [DF_RecordGameControl_ControlDuration]  DEFAULT ((0)) FOR [ControlDuration]
GO
ALTER TABLE [dbo].[RecordGameControl] ADD  CONSTRAINT [DF_RecordGameControl_ControlPlayCount]  DEFAULT ((0)) FOR [ControlPlayCount]
GO
ALTER TABLE [dbo].[RecordGameControl] ADD  CONSTRAINT [DF_RecordGameControl_CollectDate]  DEFAULT (getdate()) FOR [ControlStartTime]
GO
ALTER TABLE [dbo].[RecordGameControl] ADD  CONSTRAINT [DF_RecordGameControl_ControlFinishTime]  DEFAULT (getdate()) FOR [ControlFinishTime]
GO
ALTER TABLE [dbo].[RecordUserInout] ADD  CONSTRAINT [DF_RecordUserInout_DeviceType]  DEFAULT ((0)) FOR [DeviceType]
GO
ALTER TABLE [dbo].[RecordUserInout] ADD  CONSTRAINT [DF_RecordUserInout_FreeScore]  DEFAULT ((0)) FOR [LockerScore]
GO
ALTER TABLE [dbo].[RecordUserInout] ADD  CONSTRAINT [DF_RecordUserInout_EnterTime]  DEFAULT (getdate()) FOR [EnterTime]
GO
ALTER TABLE [dbo].[RecordUserInout] ADD  CONSTRAINT [DF_RecordUserInout_EnterGold]  DEFAULT ((0)) FOR [EnterGold]
GO
ALTER TABLE [dbo].[RecordUserInsure] ADD  CONSTRAINT [DF_RecordInsureUser_SourceUserID_1]  DEFAULT ((0)) FOR [SrcUserID]
GO
ALTER TABLE [dbo].[RecordUserInsure] ADD  CONSTRAINT [DF_RecordInsureUser_SourceUserID]  DEFAULT ((0)) FOR [SrcGameID]
GO
ALTER TABLE [dbo].[RecordUserInsure] ADD  CONSTRAINT [DF_RecordInsureUser_SourceAccounts]  DEFAULT (N'') FOR [SrcAccounts]
GO
ALTER TABLE [dbo].[RecordUserInsure] ADD  CONSTRAINT [DF_RecordInsureUser_SourceScore]  DEFAULT ((0)) FOR [SrcScore]
GO
ALTER TABLE [dbo].[RecordUserInsure] ADD  CONSTRAINT [DF_RecordInsureUser_CurrentScore]  DEFAULT ((0)) FOR [SrcInsure]
GO
ALTER TABLE [dbo].[RecordUserInsure] ADD  CONSTRAINT [DF_RecordInsureUser_SourceLockerScore]  DEFAULT ((0)) FOR [SrcLockerScore]
GO
ALTER TABLE [dbo].[RecordUserInsure] ADD  CONSTRAINT [DF_RecordInsureUser_TargetUserID_1]  DEFAULT ((0)) FOR [DstUserID]
GO
ALTER TABLE [dbo].[RecordUserInsure] ADD  CONSTRAINT [DF_RecordInsureUser_TargetUserID]  DEFAULT ((0)) FOR [DstGameID]
GO
ALTER TABLE [dbo].[RecordUserInsure] ADD  CONSTRAINT [DF_RecordInsureUser_TargetAccounts]  DEFAULT (N'') FOR [DstAccounts]
GO
ALTER TABLE [dbo].[RecordUserInsure] ADD  CONSTRAINT [DF_RecordInsureUser_TargetScore]  DEFAULT ((0)) FOR [DstScore]
GO
ALTER TABLE [dbo].[RecordUserInsure] ADD  CONSTRAINT [DF_RecordInsureUser_TargetInsure]  DEFAULT ((0)) FOR [DstInsure]
GO
ALTER TABLE [dbo].[RecordUserInsure] ADD  CONSTRAINT [DF_RecordUserInsure_DstLockerScore]  DEFAULT ((0)) FOR [DstLockerScore]
GO
ALTER TABLE [dbo].[RecordUserInsure] ADD  CONSTRAINT [DF_RecordInsureUser_TransferScore]  DEFAULT ((0)) FOR [TradeScore]
GO
ALTER TABLE [dbo].[RecordUserInsure] ADD  CONSTRAINT [DF_RecordInsureUser_Taxation]  DEFAULT ((0)) FOR [Revenue]
GO
ALTER TABLE [dbo].[RecordUserInsure] ADD  CONSTRAINT [DF_RecordInsureUser_RevenueRate]  DEFAULT ((0)) FOR [RevenueScale]
GO
ALTER TABLE [dbo].[RecordUserInsure] ADD  CONSTRAINT [DF_RecordInsureUser_MinSwapScore]  DEFAULT ((0)) FOR [MinTradeScore]
GO
ALTER TABLE [dbo].[RecordUserInsure] ADD  CONSTRAINT [DF_RecordInsureUser_TradeType]  DEFAULT ((0)) FOR [TradeType]
GO
ALTER TABLE [dbo].[RecordUserInsure] ADD  CONSTRAINT [DF_RecordInsureUser_ClientIP]  DEFAULT ('000.000.000.000') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordUserInsure] ADD  CONSTRAINT [DF_RecordInsureUser_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordUserInsure] ADD  CONSTRAINT [DF_RecordInsureUser_CollectNote]  DEFAULT (N'') FOR [CollectNote]
GO
ALTER TABLE [dbo].[ReportUserCommission] ADD  CONSTRAINT [DF_StreamCommissionInfo_ParentID]  DEFAULT ((0)) FOR [ParentID]
GO
ALTER TABLE [dbo].[ReportUserCommission] ADD  CONSTRAINT [DF_StreamCommissionInfo_PParentID]  DEFAULT ((0)) FOR [PParentID]
GO
ALTER TABLE [dbo].[ReportUserCommission] ADD  CONSTRAINT [DF_ReportUserCommission_PayAmount]  DEFAULT ((0)) FOR [ParentPayAmount]
GO
ALTER TABLE [dbo].[ReportUserCommission] ADD  CONSTRAINT [DF_ReportUserCommission_ParentPayAmount1]  DEFAULT ((0)) FOR [PParentPayAmount]
GO
ALTER TABLE [dbo].[ReportUserCommission] ADD  CONSTRAINT [DF_ReportUserCommission_BetAmount]  DEFAULT ((0)) FOR [ParentBetAmount]
GO
ALTER TABLE [dbo].[ReportUserCommission] ADD  CONSTRAINT [DF_ReportUserCommission_ParentBetAmount1]  DEFAULT ((0)) FOR [PParentBetAmount]
GO
ALTER TABLE [dbo].[ReportUserCommission] ADD  CONSTRAINT [DF_StreamCommissionInfo_LowerGameContribute]  DEFAULT ((0)) FOR [ParentPayContribute]
GO
ALTER TABLE [dbo].[ReportUserCommission] ADD  CONSTRAINT [DF_StreamCommissionInfo_LLowerGameContribute]  DEFAULT ((0)) FOR [PParentPayContribute]
GO
ALTER TABLE [dbo].[ReportUserCommission] ADD  CONSTRAINT [DF_ReportUserCommission_ParentPayContribute1]  DEFAULT ((0)) FOR [ParentBetContribute]
GO
ALTER TABLE [dbo].[ReportUserCommission] ADD  CONSTRAINT [DF_ReportUserCommission_PParentPayContribute1]  DEFAULT ((0)) FOR [PParentBetContribute]
GO
ALTER TABLE [dbo].[ReportUserCommission] ADD  CONSTRAINT [DF_ReportUserCommission_BalanceState_1]  DEFAULT ((0)) FOR [BalanceState]
GO
ALTER TABLE [dbo].[ReportUserCommission] ADD  CONSTRAINT [DF_StreamCommissionInfo_ModifyDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[ReportUserKind] ADD  CONSTRAINT [DF_ReportUserKind_GainScore]  DEFAULT ((0)) FOR [GainScore]
GO
ALTER TABLE [dbo].[ReportUserKind] ADD  CONSTRAINT [DF_ReportUserKind_LoseScore]  DEFAULT ((0)) FOR [LoseScore]
GO
ALTER TABLE [dbo].[ReportUserKind] ADD  CONSTRAINT [DF_ReportUserKind_WinCount]  DEFAULT ((0)) FOR [WinCount]
GO
ALTER TABLE [dbo].[ReportUserKind] ADD  CONSTRAINT [DF_ReportUserKind_LostCount]  DEFAULT ((0)) FOR [LostCount]
GO
ALTER TABLE [dbo].[ReportUserKind] ADD  CONSTRAINT [DF_ReportUserKind_DrawCount]  DEFAULT ((0)) FOR [DrawCount]
GO
ALTER TABLE [dbo].[ReportUserKind] ADD  CONSTRAINT [DF_ReportUserKind_FleeCount]  DEFAULT ((0)) FOR [FleeCount]
GO
ALTER TABLE [dbo].[ReportUserKind] ADD  CONSTRAINT [DF_ReportUserKind_UserService]  DEFAULT ((0)) FOR [UserService]
GO
ALTER TABLE [dbo].[ReportUserKind] ADD  CONSTRAINT [DF_ReportUserKind_UserRevenue]  DEFAULT ((0)) FOR [UserRevenue]
GO
ALTER TABLE [dbo].[ReportUserKind] ADD  CONSTRAINT [DF_ReportUserKind_UserBetAmount]  DEFAULT ((0)) FOR [UserBetAmount]
GO
ALTER TABLE [dbo].[ReportUserKind] ADD  CONSTRAINT [DF_ReportUserKind_PlayTimeCount]  DEFAULT ((0)) FOR [PlayTimeCount]
GO
ALTER TABLE [dbo].[ReportUserKind] ADD  CONSTRAINT [DF_ReportUserKind_InsertDateTime]  DEFAULT (getdate()) FOR [InsertDateTime]
GO
ALTER TABLE [dbo].[ReportUserKind] ADD  CONSTRAINT [DF_ReportUserKind_ModifyDateTime]  DEFAULT (getdate()) FOR [ModifyDateTime]
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
ALTER TABLE [dbo].[ReportUserScore] ADD  CONSTRAINT [DF_ReportUserScore_UserBetAmount]  DEFAULT ((0)) FOR [UserBetAmount]
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
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'签到标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CheckInItem', @level2type=N'COLUMN',@level2name=N'ItemIndex'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CheckInItem', @level2type=N'COLUMN',@level2name=N'ItemCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CheckInItem', @level2type=N'COLUMN',@level2name=N'Remark'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排列标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameColumnItem', @level2type=N'COLUMN',@level2name=N'SortID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'列头名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameColumnItem', @level2type=N'COLUMN',@level2name=N'ColumnName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'列表宽度' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameColumnItem', @level2type=N'COLUMN',@level2name=N'ColumnWidth'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数据描述' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameColumnItem', @level2type=N'COLUMN',@level2name=N'DataDescribe'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'值为房间类型和桌子模式的组合   房间类型（金币类型: 0x0001   点值类型： 0x0002  比赛类型： 0x0004  训练类型：0x0008 积分金币：0x0010 福利类型： 0x0020）  
桌子模式（创建模式：0x1000  分配模式：0x2000 普通模式： 0x4000）

' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameColumnItem', @level2type=N'COLUMN',@level2name=N'SuportType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameControlInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'控制类型（0、胜率控制  1、时间控制 2、局数控制）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameControlInfo', @level2type=N'COLUMN',@level2name=N'ControlKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'控制模式（1：自动模式 2：手动模式）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameControlInfo', @level2type=N'COLUMN',@level2name=N'ControlMode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'控制状态（0：常规状态 1：白名单  2：黑名单）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameControlInfo', @level2type=N'COLUMN',@level2name=N'ControlStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标胜率' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameControlInfo', @level2type=N'COLUMN',@level2name=N'TargetWinRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'控制状态下的胜率' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameControlInfo', @level2type=N'COLUMN',@level2name=N'ControlWinRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'开始前胜率' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameControlInfo', @level2type=N'COLUMN',@level2name=N'InitailWinRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'开始前累计赢分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameControlInfo', @level2type=N'COLUMN',@level2name=N'InitailWinScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'开始前累计输分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameControlInfo', @level2type=N'COLUMN',@level2name=N'InitailLoseScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赢局' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameControlInfo', @level2type=N'COLUMN',@level2name=N'TotalWinCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'输局' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameControlInfo', @level2type=N'COLUMN',@level2name=N'TotalLoseCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'累计赢分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameControlInfo', @level2type=N'COLUMN',@level2name=N'TotalWinScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'累计输分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameControlInfo', @level2type=N'COLUMN',@level2name=N'TotalLoseScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'持续时长' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameControlInfo', @level2type=N'COLUMN',@level2name=N'ControlDuration'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'控制游戏局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameControlInfo', @level2type=N'COLUMN',@level2name=N'ControlPlayCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'控制开始时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameControlInfo', @level2type=N'COLUMN',@level2name=N'ControlStartTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作者标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameControlInfo', @level2type=N'COLUMN',@level2name=N'OperaterMasterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作备注' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameControlInfo', @level2type=N'COLUMN',@level2name=N'OperateRemark'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户 ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下注金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'BetAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赢取金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'WinScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'输出金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'LostScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'锁定数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'LockerScore'
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
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'损耗总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'WasteCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'服务费总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'ServiceCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总登陆次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'LogonTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'PlayTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'在线时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'OnLineTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'累计胜率' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'AddupWinRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'累计赢分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'AddupWinScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'累计输分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'AddupLoseScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上次登陆 IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'RegisterIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上次登陆时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'RegisterDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录机器' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'RegisterMachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上次登陆 IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'LogonIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上次登陆时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'LogonDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录机器' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'LogonMachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawInfo', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawInfo', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawInfo', @level2type=N'COLUMN',@level2name=N'ServerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'桌子号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawInfo', @level2type=N'COLUMN',@level2name=N'TableID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'玩家数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawInfo', @level2type=N'COLUMN',@level2name=N'UserCount'
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
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawScore', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawScore', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'椅子标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawScore', @level2type=N'COLUMN',@level2name=N'ChairID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'输赢分数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawScore', @level2type=N'COLUMN',@level2name=N'Score'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'税收' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawScore', @level2type=N'COLUMN',@level2name=N'Revenue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下注金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawScore', @level2type=N'COLUMN',@level2name=N'BetAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'写入前分数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawScore', @level2type=N'COLUMN',@level2name=N'BeforeScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'录像标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawScore', @level2type=N'COLUMN',@level2name=N'VideoNumber'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawScore', @level2type=N'COLUMN',@level2name=N'RecordDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameControl', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameControl', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'控制类型（0、胜率控制  1、局数控制 2、时间控制）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameControl', @level2type=N'COLUMN',@level2name=N'ControlKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'控制模式（1：自动模式 2：手动模式）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameControl', @level2type=N'COLUMN',@level2name=N'ControlMode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'控制状态（0：常规状态 1：白名单  2：黑名单）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameControl', @level2type=N'COLUMN',@level2name=N'ControlStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标胜率' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameControl', @level2type=N'COLUMN',@level2name=N'TargetWinRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'控制状态下的胜率' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameControl', @level2type=N'COLUMN',@level2name=N'ControlWinRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'开始前胜率' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameControl', @level2type=N'COLUMN',@level2name=N'InitailWinRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'开始前赢分统计' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameControl', @level2type=N'COLUMN',@level2name=N'InitailWinScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'开始前输分统计' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameControl', @level2type=N'COLUMN',@level2name=N'InitailLoseScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赢局' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameControl', @level2type=N'COLUMN',@level2name=N'TotalWinCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'输局' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameControl', @level2type=N'COLUMN',@level2name=N'TotalLoseCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'累计赢分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameControl', @level2type=N'COLUMN',@level2name=N'TotalWinScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'累计输分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameControl', @level2type=N'COLUMN',@level2name=N'TotalLoseScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'持续时长' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameControl', @level2type=N'COLUMN',@level2name=N'ControlDuration'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'控制游戏局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameControl', @level2type=N'COLUMN',@level2name=N'ControlPlayCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'开始时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameControl', @level2type=N'COLUMN',@level2name=N'ControlStartTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'结束时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameControl', @level2type=N'COLUMN',@level2name=N'ControlFinishTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作者标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameControl', @level2type=N'COLUMN',@level2name=N'OperaterUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作备注' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameControl', @level2type=N'COLUMN',@level2name=N'OperateRemark'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'索引标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'ID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'ServerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'设备类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'DeviceType'
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
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'流水标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'源用户ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'SrcUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'源用户游戏ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'SrcGameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'源用户账号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'SrcAccounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'源用户现金金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'SrcScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'源用户银行金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'SrcInsure'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'锁定数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'SrcLockerScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标用户ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'DstUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标用户游戏ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'DstGameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标用户账号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'DstAccounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标用户现金' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'DstScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标用户银行金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'DstInsure'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'锁定数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'DstLockerScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'转出金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'TradeScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'交易场所税收' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'Revenue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'税收比率' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'RevenueScale'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最少数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'MinTradeScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'交易类别: 存 1,取 2,转 3' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'TradeType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'连接地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'交易时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户备注' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInsure', @level2type=N'COLUMN',@level2name=N'CollectNote'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserCommission', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'贡献玩家' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserCommission', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上级' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserCommission', @level2type=N'COLUMN',@level2name=N'ParentID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上上级' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserCommission', @level2type=N'COLUMN',@level2name=N'PParentID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上级充值获利总计' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserCommission', @level2type=N'COLUMN',@level2name=N'ParentPayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上上级充值获利总计' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserCommission', @level2type=N'COLUMN',@level2name=N'PParentPayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上级游戏获利总计' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserCommission', @level2type=N'COLUMN',@level2name=N'ParentBetAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上上级游戏获利总计' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserCommission', @level2type=N'COLUMN',@level2name=N'PParentBetAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上级佣金' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserCommission', @level2type=N'COLUMN',@level2name=N'ParentPayContribute'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上上级佣金' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserCommission', @level2type=N'COLUMN',@level2name=N'PParentPayContribute'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上级佣金' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserCommission', @level2type=N'COLUMN',@level2name=N'ParentBetContribute'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上上级佣金' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserCommission', @level2type=N'COLUMN',@level2name=N'PParentBetContribute'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'结算状态' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserCommission', @level2type=N'COLUMN',@level2name=N'BalanceState'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserCommission', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserKind', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserKind', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserKind', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'胜利积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserKind', @level2type=N'COLUMN',@level2name=N'GainScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'失败积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserKind', @level2type=N'COLUMN',@level2name=N'LoseScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'胜利局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserKind', @level2type=N'COLUMN',@level2name=N'WinCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'失败局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserKind', @level2type=N'COLUMN',@level2name=N'LostCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'和局总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserKind', @level2type=N'COLUMN',@level2name=N'DrawCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'逃跑局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserKind', @level2type=N'COLUMN',@level2name=N'FleeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'貌似没什么用' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserKind', @level2type=N'COLUMN',@level2name=N'UserService'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户税费' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserKind', @level2type=N'COLUMN',@level2name=N'UserRevenue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下注金额总计' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserKind', @level2type=N'COLUMN',@level2name=N'UserBetAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏时长' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserKind', @level2type=N'COLUMN',@level2name=N'PlayTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'写入时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserKind', @level2type=N'COLUMN',@level2name=N'InsertDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修改时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserKind', @level2type=N'COLUMN',@level2name=N'ModifyDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserScore', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserScore', @level2type=N'COLUMN',@level2name=N'UserID'
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
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'貌似没什么用' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserScore', @level2type=N'COLUMN',@level2name=N'UserService'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户税费' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserScore', @level2type=N'COLUMN',@level2name=N'UserRevenue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下注金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserScore', @level2type=N'COLUMN',@level2name=N'UserBetAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏时长' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserScore', @level2type=N'COLUMN',@level2name=N'PlayTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'写入时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserScore', @level2type=N'COLUMN',@level2name=N'InsertDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修改时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserScore', @level2type=N'COLUMN',@level2name=N'ModifyDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamWasteInfo', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'损耗标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamWasteInfo', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'损耗数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamWasteInfo', @level2type=N'COLUMN',@level2name=N'WasteScoreStat'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'初次记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamWasteInfo', @level2type=N'COLUMN',@level2name=N'FirstWasteDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'末次记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamWasteInfo', @level2type=N'COLUMN',@level2name=N'LastWasteDate'
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
