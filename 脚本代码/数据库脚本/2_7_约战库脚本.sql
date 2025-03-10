USE [LHGameBattleDB]
GO
/****** Object:  Table [dbo].[AccountsControl]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsControl](
	[UserID] [int] NOT NULL,
	[ClubID] [int] NOT NULL,
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
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_AccountsControl] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC,
	[ClubID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AndroidActionTemplate]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AndroidActionTemplate](
	[TemplateID] [int] IDENTITY(1,1) NOT NULL,
	[GameKindID] [smallint] NOT NULL,
	[TemplateName] [nvarchar](32) NOT NULL,
	[TemplateRemarks] [nvarchar](64) NOT NULL,
	[TemplateData] [varbinary](512) NOT NULL,
 CONSTRAINT [PK_AndroidActionMode] PRIMARY KEY CLUSTERED 
(
	[TemplateID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AndroidBatchConfig]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AndroidBatchConfig](
	[BatchID] [int] IDENTITY(1,1) NOT NULL,
	[ClubID] [int] NOT NULL,
	[EnterTime] [int] NOT NULL,
	[LeaveTime] [int] NOT NULL,
 CONSTRAINT [PK_AndroidConfigure_1] PRIMARY KEY CLUSTERED 
(
	[BatchID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AndroidBatchMember]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AndroidBatchMember](
	[UserID] [int] NOT NULL,
	[BatchID] [int] NOT NULL,
	[EnterTime] [int] NOT NULL,
	[LeaveTime] [int] NOT NULL,
	[GameKindID1] [smallint] NOT NULL,
	[ActionTemplateID1] [smallint] NOT NULL,
	[GameKindID2] [smallint] NOT NULL,
	[ActionTemplateID2] [smallint] NOT NULL,
	[GameKindID3] [smallint] NOT NULL,
	[ActionTemplateID3] [smallint] NOT NULL,
	[SupplyTemplateID] [smallint] NOT NULL,
 CONSTRAINT [PK_AndroidLockInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC,
	[BatchID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AndroidSupplyTemplate]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AndroidSupplyTemplate](
	[TemplateID] [int] IDENTITY(1,1) NOT NULL,
	[TemplateName] [nvarchar](32) NOT NULL,
	[TemplateRemarks] [nvarchar](64) NOT NULL,
	[TemplateData] [varbinary](256) NOT NULL,
 CONSTRAINT [PK_AndroidSupplyTemplate] PRIMARY KEY CLUSTERED 
(
	[TemplateID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[BattleOption]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[BattleOption](
	[OptionID] [int] IDENTITY(1,1) NOT NULL,
	[StationID] [int] NOT NULL,
	[KindID] [smallint] NOT NULL,
	[ScoreKind] [tinyint] NOT NULL,
	[PlayerCount] [smallint] NOT NULL,
	[EmployScene] [tinyint] NOT NULL,
	[VideoIngot_User] [smallint] NOT NULL,
	[VideoRoomCard_User] [smallint] NOT NULL,
	[VideoIngot_Owner] [smallint] NOT NULL,
	[VideoRoomCard_Owner] [smallint] NOT NULL,
	[MinBaseScore] [int] NOT NULL,
	[MaxBaseScore] [int] NOT NULL,
	[SuportPayType] [smallint] NOT NULL,
	[ItemCount] [tinyint] NOT NULL,
	[OptionMask] [smallint] NOT NULL,
	[Nullity] [tinyint] NOT NULL,
 CONSTRAINT [PK_BattleOption] PRIMARY KEY CLUSTERED 
(
	[OptionID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[BattleOptionCustom]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[BattleOptionCustom](
	[StationID] [int] NOT NULL,
	[KindID] [smallint] NOT NULL,
	[EmployScene] [tinyint] NOT NULL,
	[CustomOption] [nvarchar](4000) NOT NULL,
 CONSTRAINT [PK_BattleOptionCustom_1] PRIMARY KEY CLUSTERED 
(
	[StationID] ASC,
	[KindID] ASC,
	[EmployScene] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[BattleOptionItem]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[BattleOptionItem](
	[OptionID] [int] NOT NULL,
	[ItemIndex] [tinyint] NOT NULL,
	[PlayTime] [int] NOT NULL,
	[PlayCount] [smallint] NOT NULL,
	[Ingot_User] [smallint] NOT NULL,
	[Ingot_Owner] [smallint] NOT NULL,
	[RoomCard_User] [smallint] NOT NULL,
	[RoomCard_Owner] [smallint] NOT NULL,
 CONSTRAINT [PK_BattleOptionItem] PRIMARY KEY CLUSTERED 
(
	[OptionID] ASC,
	[ItemIndex] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[BattleReward]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[BattleReward](
	[OptionID] [int] NOT NULL,
	[ConsumeIndex] [smallint] NOT NULL,
	[RankIDStart] [smallint] NOT NULL,
	[RankIDEnd] [smallint] NOT NULL,
	[RewardMedal] [bigint] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[BattleUserLocker]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[BattleUserLocker](
	[UserID] [int] NOT NULL,
	[KindID] [smallint] NOT NULL,
	[ServerID] [smallint] NOT NULL,
	[ScoreKind] [tinyint] NOT NULL,
	[MappedNum] [int] NOT NULL,
	[BattleStatus] [tinyint] NOT NULL,
	[OnlineStatus] [tinyint] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_BattleUserLocker] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ConfineAddress]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ConfineAddress](
	[AddrString] [nvarchar](15) NOT NULL,
	[EnjoinLogon] [bit] NOT NULL,
	[EnjoinOverDate] [datetime] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[CollectNote] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_ConfineAddress_AddrString] PRIMARY KEY CLUSTERED 
(
	[AddrString] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ConfineMachine]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ConfineMachine](
	[MachineSerial] [nvarchar](12) NOT NULL,
	[EnjoinLogon] [bit] NOT NULL,
	[EnjoinRegister] [bit] NOT NULL,
	[EnjoinOverDate] [datetime] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[CollectNote] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_MachineConfineRule_MachineSerial] PRIMARY KEY CLUSTERED 
(
	[MachineSerial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameColumnItem]    Script Date: 2023/7/20 16:19:15 ******/
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
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameClubScoreInfo]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameClubScoreInfo](
	[UserID] [int] NOT NULL,
	[ClubScore] [bigint] NOT NULL,
	[FillCount] [bigint] NOT NULL,
	[RevenueCount] [bigint] NOT NULL,
	[ExchangeCount] [bigint] NOT NULL,
	[RemainCount] [bigint] NOT NULL,
	[PresentCount] [bigint] NOT NULL,
 CONSTRAINT [PK_GameClubScoreInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameIngotInfo]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameIngotInfo](
	[UserID] [int] NOT NULL,
	[StationID] [int] NOT NULL,
	[Ingot] [bigint] NOT NULL,
	[FillCount] [bigint] NOT NULL,
 CONSTRAINT [PK_GameIngotInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC,
	[StationID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameScoreInfo]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameScoreInfo](
	[UserID] [int] NOT NULL,
	[CurrencyID] [int] NOT NULL,
	[StationID] [int] NOT NULL,
	[WinCount] [int] NOT NULL,
	[LostCount] [int] NOT NULL,
	[DrawCount] [int] NOT NULL,
	[FleeCount] [int] NOT NULL,
	[UserRight] [int] NOT NULL,
	[MasterRight] [int] NOT NULL,
	[MasterOrder] [tinyint] NOT NULL,
	[AllLogonTimes] [int] NOT NULL,
	[PlayTimeCount] [int] NOT NULL,
	[OnLineTimeCount] [int] NOT NULL,
	[LastLogonIP] [nvarchar](15) NOT NULL,
	[LastLogonDate] [datetime] NOT NULL,
	[LastLogonMachine] [nvarchar](32) NOT NULL,
	[RegisterIP] [nvarchar](15) NOT NULL,
	[RegisterDate] [datetime] NOT NULL,
	[RegisterMachine] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_GameScore_Index] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC,
	[CurrencyID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ClubMember_0]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ClubMember_0](
	[ClubID] [int] NOT NULL,
	[MemberID] [int] NOT NULL,
	[ParentID] [int] NOT NULL,
	[MemberType] [tinyint] NOT NULL,
	[MemberRate] [smallint] NOT NULL,
	[MemberStatus] [tinyint] NOT NULL,
	[MemberFace] [nvarchar](200) NOT NULL,
	[IsAndroid] [tinyint] NOT NULL,
	[ClubScore] [int] NOT NULL,
	[ClubCowry] [int] NOT NULL,
	[CreateCount] [int] NOT NULL,
	[BattleCount] [int] NOT NULL,
	[RevenueCount] [int] NOT NULL,
	[AddupWinRate] [smallint] NOT NULL,
	[AddupWinScore] [int] NOT NULL,
	[AddupLoseScore] [int] NOT NULL,
	[JoinDateTime] [datetime] NOT NULL,
	[LastBattleTime] [datetime] NOT NULL,
 CONSTRAINT [PK__ClubMem__8455F7B926E9437F] PRIMARY KEY CLUSTERED 
(
	[ClubID] ASC,
	[MemberID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ClubOption]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ClubOption](
	[StationID] [int] NOT NULL,
	[OptionName] [nvarchar](32) NOT NULL,
	[OptionValue] [int] NOT NULL,
	[OptionDescribe] [nvarchar](128) NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ClubProperty]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ClubProperty](
	[ClubID] [int] NOT NULL,
	[StationID] [int] NOT NULL,
	[CreaterID] [int] NOT NULL,
	[CreaterGameID] [int] NOT NULL,
	[CreaterNickName] [nvarchar](32) NOT NULL,
	[ClubName] [nvarchar](32) NOT NULL,
	[ClubLevel] [tinyint] NOT NULL,
	[ClubStatus] [tinyint] NOT NULL,
	[ClubIntroduce] [nvarchar](128) NOT NULL,
	[ClubRate] [smallint] NOT NULL,
	[PayrollerID] [int] NOT NULL,
	[MemberCount] [smallint] NOT NULL,
	[MaxMemberCount] [smallint] NOT NULL,
	[CreateDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_ClubProperty_1] PRIMARY KEY CLUSTERED 
(
	[ClubID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ClubRules]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ClubRules](
	[ClubID] [bigint] NOT NULL,
	[PayType] [tinyint] NOT NULL,
	[CurrencyKind] [tinyint] NOT NULL,
	[RevenueRate] [smallint] NOT NULL,
	[BattleMask] [smallint] NOT NULL,
	[ScoreRight] [tinyint] NOT NULL,
	[ScoreAdjust] [tinyint] NOT NULL,
	[SwitchOption] [smallint] NOT NULL,
	[AutoCleanup] [tinyint] NOT NULL,
 CONSTRAINT [PK_ClubRules] PRIMARY KEY CLUSTERED 
(
	[ClubID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ClubUserInfo]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ClubUserInfo](
	[UserID] [int] NOT NULL,
	[ClubIDArray] [nvarchar](512) NOT NULL,
 CONSTRAINT [PK_ClubUserInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ClubWealth]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ClubWealth](
	[ClubID] [bigint] NOT NULL,
	[Ingot] [bigint] NOT NULL,
	[RoomCard] [bigint] NOT NULL,
 CONSTRAINT [PK_ClubWealth] PRIMARY KEY CLUSTERED 
(
	[ClubID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[NormalClubID]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[NormalClubID](
	[ClubID] [int] NOT NULL,
	[Distribute] [tinyint] NOT NULL,
 CONSTRAINT [PK_NormalClubID] PRIMARY KEY CLUSTERED 
(
	[ClubID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordDrawInfo]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordDrawInfo](
	[RecordID] [bigint] NOT NULL,
	[KindID] [int] NOT NULL,
	[ServerID] [int] NOT NULL,
	[ScoreKind] [tinyint] NOT NULL,
	[TableID] [int] NOT NULL,
	[UserCount] [smallint] NOT NULL,
	[WasteCount] [bigint] NOT NULL,
	[RevenueCount] [bigint] NOT NULL,
	[PlayTimeCount] [int] NOT NULL,
	[GameCourse] [nvarchar](1024) NOT NULL,
	[BattleRecordID] [bigint] NOT NULL,
	[VideoNumber] [nvarchar](21) NOT NULL,
	[RecordDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordDrawInfo_RecordID] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordDrawScore]    Script Date: 2023/7/20 16:19:15 ******/
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
	[BattleRecordID] [bigint] NOT NULL,
	[VideoNumber] [nvarchar](21) NOT NULL,
	[RecordDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordGameScore]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordGameScore](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[KindID] [int] NOT NULL,
	[ServerID] [int] NOT NULL,
	[ScoreKind] [tinyint] NOT NULL,
	[TableID] [int] NOT NULL,
	[WasteCount] [bigint] NOT NULL,
	[RevenueCount] [bigint] NOT NULL,
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
	[GameCourse] [nvarchar](1024) NOT NULL,
	[BattleRecordID] [bigint] NOT NULL,
	[VideoNumber] [nvarchar](21) NOT NULL,
	[RecordDate] [datetime] NOT NULL,
 CONSTRAINT [PK_GameScoreRecord_RecordID] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordClubApply]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordClubApply](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[ClubID] [bigint] NOT NULL,
	[ClubName] [nvarchar](32) NOT NULL,
	[ApplyerID] [int] NOT NULL,
	[ApplyerGameID] [int] NOT NULL,
	[ApplyerGender] [tinyint] NOT NULL,
	[ApplyerFaceID] [int] NOT NULL,
	[ApplyerCustomID] [int] NOT NULL,
	[ApplyerNickName] [nvarchar](32) NOT NULL,
	[ApplyType] [tinyint] NOT NULL,
	[ApplyStatus] [tinyint] NOT NULL,
	[CreaterID] [int] NOT NULL,
	[ResponderID] [int] NOT NULL,
	[DeleteMask] [tinyint] NOT NULL,
	[Remarks] [nvarchar](64) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_ClubApplyRecord_1] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordClubInvite]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordClubInvite](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[RequestUserID] [int] NOT NULL,
	[RequestNickName] [nvarchar](32) NOT NULL,
	[RequestClubID] [int] NOT NULL,
	[RequestClubName] [nvarchar](32) NOT NULL,
	[RespondUserID] [int] NOT NULL,
	[RespondNickName] [nvarchar](32) NOT NULL,
	[RespondCode] [tinyint] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_ClubInviteRecord] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordClubMerge]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordClubMerge](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[RequestClubID] [int] NOT NULL,
	[RequestCreaterID] [int] NOT NULL,
	[RequestNickName] [nvarchar](32) NOT NULL,
	[RequestClubName] [nvarchar](32) NOT NULL,
	[RespondClubID] [int] NOT NULL,
	[RespondCreaterID] [int] NOT NULL,
	[RespondClubName] [nvarchar](32) NOT NULL,
	[RespondCode] [tinyint] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_ClubMergeRecord] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordClubNotice]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordClubNotice](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[ClubID] [int] NOT NULL,
	[NoticeContent] [nvarchar](64) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordClubNotice] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordClubPayroll]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordClubPayroll](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[RequestClubID] [int] NOT NULL,
	[RequestCreaterID] [int] NOT NULL,
	[RequestNickName] [nvarchar](32) NOT NULL,
	[RequestClubName] [nvarchar](32) NOT NULL,
	[RespondUserID] [int] NOT NULL,
	[RespondCode] [tinyint] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_ClubPayrollRecord] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordUserInout]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordUserInout](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[ServerID] [int] NOT NULL,
	[StationID] [int] NOT NULL,
	[CurrencyID] [int] NOT NULL,
	[DeviceType] [smallint] NOT NULL,
	[EnterTime] [datetime] NOT NULL,
	[EnterGold] [bigint] NOT NULL,
	[EnterScore] [bigint] NOT NULL,
	[EnterMachine] [nvarchar](32) NOT NULL,
	[EnterClientIP] [nvarchar](15) NOT NULL,
	[LeaveTime] [datetime] NULL,
	[LeaveReason] [int] NULL,
	[LeaveMachine] [nvarchar](32) NULL,
	[LeaveClientIP] [nvarchar](15) NULL,
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
	[ClubID] [bigint] NOT NULL,
	[ScoreKind] [tinyint] NOT NULL,
	[MappedNum] [int] NOT NULL,
 CONSTRAINT [PK_RecordUserInout_1] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RequestWriteRecord]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RequestWriteRecord](
	[UserID] [int] NOT NULL,
	[ServerID] [int] NOT NULL,
	[RequestID] [int] NOT NULL,
	[RequestDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RequestWriteRecord] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC,
	[ServerID] ASC,
	[RequestID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReserveClubID]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReserveClubID](
	[ClubID] [int] NOT NULL,
	[LevelID] [tinyint] NOT NULL,
	[Distribute] [tinyint] NOT NULL,
 CONSTRAINT [PK_ReserveClubID] PRIMARY KEY CLUSTERED 
(
	[ClubID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[StreamBattleInfo]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[StreamBattleInfo](
	[DateID] [int] NOT NULL,
	[StationID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[BattleCount] [bigint] NOT NULL,
	[BattleFinishCount] [bigint] NOT NULL,
	[IngotCount] [bigint] NOT NULL,
	[CardCount] [bigint] NOT NULL,
 CONSTRAINT [PK_StreamBattleInfo] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[StationID] ASC,
	[KindID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[StreamBattleKindInfo]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[StreamBattleKindInfo](
	[DateID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[BattleCount] [bigint] NOT NULL,
	[WinCount] [int] NOT NULL,
	[LostCount] [int] NOT NULL,
	[PlayTimeCount] [bigint] NOT NULL,
 CONSTRAINT [PK_StreamBattleKindInfo] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[UserID] ASC,
	[KindID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[StreamClubBalance]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[StreamClubBalance](
	[DateID] [int] NOT NULL,
	[ClubID] [bigint] NOT NULL,
	[UserID] [int] NOT NULL,
	[GStationID] [int] NOT NULL,
	[WinCount] [int] NOT NULL,
	[WinScore] [bigint] NOT NULL,
	[LostCount] [int] NOT NULL,
	[LostScore] [bigint] NOT NULL,
	[IncreaseScore] [bigint] NOT NULL,
	[DecreaseScore] [bigint] NOT NULL,
	[IncreaseCowry] [bigint] NOT NULL,
	[DecreaseCowry] [bigint] NOT NULL,
	[TradeIn] [bigint] NOT NULL,
	[TradeOut] [bigint] NOT NULL,
	[Revenue] [bigint] NOT NULL,
	[RevenueCowry] [bigint] NOT NULL,
	[BattleCount] [int] NOT NULL,
	[BalanceCount] [int] NOT NULL,
	[IngotCount] [bigint] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_IMClubBalance] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[ClubID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[StreamClubBattle]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[StreamClubBattle](
	[DateID] [int] NOT NULL,
	[ClubID] [bigint] NOT NULL,
	[BattleCount] [bigint] NOT NULL,
	[BattleFinishCount] [bigint] NOT NULL,
	[IncreaseScore] [bigint] NOT NULL,
	[DecreaseScore] [bigint] NOT NULL,
	[IncreaseCowry] [bigint] NOT NULL,
	[DecreaseCowry] [bigint] NOT NULL,
	[Revenue] [bigint] NOT NULL,
	[RevenueCowry] [bigint] NOT NULL,
	[IngotCount] [bigint] NOT NULL,
 CONSTRAINT [PK_StreamClubBattle] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[ClubID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[StreamClubPartner]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[StreamClubPartner](
	[DateID] [int] NOT NULL,
	[ClubID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[SupplyUserID] [int] NOT NULL,
	[SupplyRevenue] [int] NOT NULL,
	[ExtractRevenue] [int] NOT NULL,
	[WinScore] [int] NOT NULL,
	[LostScore] [int] NOT NULL,
	[WinCount] [int] NOT NULL,
	[LostCount] [int] NOT NULL,
	[BattleCount] [int] NOT NULL,
 CONSTRAINT [PK_StreamClubPartner] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[ClubID] ASC,
	[UserID] ASC,
	[SupplyUserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[StreamClubUser]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[StreamClubUser](
	[DateID] [int] NOT NULL,
	[ClubID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[WinCount] [int] NOT NULL,
	[WinScore] [bigint] NOT NULL,
	[LostCount] [int] NOT NULL,
	[LostScore] [bigint] NOT NULL,
	[BattleCount] [int] NOT NULL,
	[LastBattleTime] [datetime] NOT NULL,
 CONSTRAINT [PK_StreamClubUser] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[ClubID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[SystemStatusInfo]    Script Date: 2023/7/20 16:19:15 ******/
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
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[SystemStreamInfo]    Script Date: 2023/7/20 16:19:15 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[SystemStreamInfo](
	[DateID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[ServerID] [int] NOT NULL,
	[StationID] [int] NOT NULL,
	[LogonCount] [int] NOT NULL,
	[LogOutCount] [int] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_SystemStreamInfo_ID] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[KindID] ASC,
	[ServerID] ASC,
	[StationID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Index [IX_ClubOption]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_ClubOption] ON [dbo].[ClubOption]
(
	[StationID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_ClubProperty_CreaterID]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_ClubProperty_CreaterID] ON [dbo].[ClubProperty]
(
	[CreaterID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON
GO
/****** Object:  Index [IX_ClubProperty_ClubName]    Script Date: 2023/7/20 16:19:15 ******/
CREATE UNIQUE NONCLUSTERED INDEX [IX_ClubProperty_ClubName] ON [dbo].[ClubProperty]
(
	[ClubName] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordDrawInfo_BattleRecordID]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_RecordDrawInfo_BattleRecordID] ON [dbo].[RecordDrawInfo]
(
	[BattleRecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordDrawInfo_ServerType]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_RecordDrawInfo_ServerType] ON [dbo].[RecordDrawInfo]
(
	[ScoreKind] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON
GO
/****** Object:  Index [IX_RecordDrawInfo_VideoNumber]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_RecordDrawInfo_VideoNumber] ON [dbo].[RecordDrawInfo]
(
	[VideoNumber] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordGameScore_BattleRecordID]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_RecordGameScore_BattleRecordID] ON [dbo].[RecordGameScore]
(
	[BattleRecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordGameScore_ServerType]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_RecordGameScore_ServerType] ON [dbo].[RecordGameScore]
(
	[ScoreKind] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON
GO
/****** Object:  Index [IX_RecordGameScore_VideoNumber]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_RecordGameScore_VideoNumber] ON [dbo].[RecordGameScore]
(
	[VideoNumber] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_ClubApplyRecord_ApplyerID]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_ClubApplyRecord_ApplyerID] ON [dbo].[RecordClubApply]
(
	[ApplyerID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_ClubApplyRecord_CreaterID]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_ClubApplyRecord_CreaterID] ON [dbo].[RecordClubApply]
(
	[CreaterID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_ClubApplyRecord_ClubID]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_ClubApplyRecord_ClubID] ON [dbo].[RecordClubApply]
(
	[ClubID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_ClubApplyRecord_ViewMask]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_ClubApplyRecord_ViewMask] ON [dbo].[RecordClubApply]
(
	[DeleteMask] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_ClubInviteRecord_RequestClubID]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_ClubInviteRecord_RequestClubID] ON [dbo].[RecordClubInvite]
(
	[RequestClubID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_ClubInviteRecord_RespondUserID]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_ClubInviteRecord_RespondUserID] ON [dbo].[RecordClubInvite]
(
	[RespondUserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_ClubMergeRecord_RequestClubID]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_ClubMergeRecord_RequestClubID] ON [dbo].[RecordClubMerge]
(
	[RequestClubID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_ClubMergeRecord_RespondCreaterID]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_ClubMergeRecord_RespondCreaterID] ON [dbo].[RecordClubMerge]
(
	[RespondCreaterID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_ClubMergeRecord_RespondClubID]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_ClubMergeRecord_RespondClubID] ON [dbo].[RecordClubMerge]
(
	[RespondClubID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_RecordClubNotice_UserID]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_RecordClubNotice_UserID] ON [dbo].[RecordClubNotice]
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_ClubPayrollRecord_RequestClubID]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_ClubPayrollRecord_RequestClubID] ON [dbo].[RecordClubPayroll]
(
	[RequestClubID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_ClubPayrollRecord_RespondUserID]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_ClubPayrollRecord_RespondUserID] ON [dbo].[RecordClubPayroll]
(
	[RespondUserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_StreamClubUser_LostCount]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_StreamClubUser_LostCount] ON [dbo].[StreamClubUser]
(
	[LostCount] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_StreamClubUser_LostScore]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_StreamClubUser_LostScore] ON [dbo].[StreamClubUser]
(
	[LostScore] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_StreamClubUser_WinCount]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_StreamClubUser_WinCount] ON [dbo].[StreamClubUser]
(
	[WinCount] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_StreamClubUser_WinScore]    Script Date: 2023/7/20 16:19:15 ******/
CREATE NONCLUSTERED INDEX [IX_StreamClubUser_WinScore] ON [dbo].[StreamClubUser]
(
	[WinScore] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
ALTER TABLE [dbo].[AccountsControl] ADD  CONSTRAINT [DF_AccountsControl_ControlKind]  DEFAULT ((0)) FOR [ControlKind]
GO
ALTER TABLE [dbo].[AccountsControl] ADD  CONSTRAINT [DF_AccountsControl_ControlMode]  DEFAULT ((0)) FOR [ControlMode]
GO
ALTER TABLE [dbo].[AccountsControl] ADD  CONSTRAINT [DF_AccountsControl_ControlStatus]  DEFAULT ((0)) FOR [ControlStatus]
GO
ALTER TABLE [dbo].[AccountsControl] ADD  CONSTRAINT [DF_AccountsControl_TargetWinRate]  DEFAULT ((0)) FOR [TargetWinRate]
GO
ALTER TABLE [dbo].[AccountsControl] ADD  CONSTRAINT [DF_AccountsControl_ControlRate]  DEFAULT ((0)) FOR [ControlWinRate]
GO
ALTER TABLE [dbo].[AccountsControl] ADD  CONSTRAINT [DF_AccountsControl_SourceWinRate]  DEFAULT ((0)) FOR [InitailWinRate]
GO
ALTER TABLE [dbo].[AccountsControl] ADD  CONSTRAINT [DF_AccountsControl_InitailWinScore]  DEFAULT ((0)) FOR [InitailWinScore]
GO
ALTER TABLE [dbo].[AccountsControl] ADD  CONSTRAINT [DF_AccountsControl_InitailLoseScore]  DEFAULT ((0)) FOR [InitailLoseScore]
GO
ALTER TABLE [dbo].[AccountsControl] ADD  CONSTRAINT [DF_AccountsControl_WinCount]  DEFAULT ((0)) FOR [TotalWinCount]
GO
ALTER TABLE [dbo].[AccountsControl] ADD  CONSTRAINT [DF_AccountsControl_LoseCount]  DEFAULT ((0)) FOR [TotalLoseCount]
GO
ALTER TABLE [dbo].[AccountsControl] ADD  CONSTRAINT [DF_Table_1_TotalWinScore]  DEFAULT ((0)) FOR [TotalWinScore]
GO
ALTER TABLE [dbo].[AccountsControl] ADD  CONSTRAINT [DF_Table_1_TotalLoseScore]  DEFAULT ((0)) FOR [TotalLoseScore]
GO
ALTER TABLE [dbo].[AccountsControl] ADD  CONSTRAINT [DF_AccountsControl_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[AndroidBatchConfig] ADD  CONSTRAINT [DF_AndroidConfigure_ClubID]  DEFAULT ((0)) FOR [ClubID]
GO
ALTER TABLE [dbo].[AndroidBatchMember] ADD  CONSTRAINT [DF_AndroidLockInfo_BatchID]  DEFAULT ((0)) FOR [BatchID]
GO
ALTER TABLE [dbo].[AndroidBatchMember] ADD  CONSTRAINT [DF_AndroidAccounts_EnterTime]  DEFAULT ((0)) FOR [EnterTime]
GO
ALTER TABLE [dbo].[AndroidBatchMember] ADD  CONSTRAINT [DF_AndroidAccounts_LeaveTime]  DEFAULT ((0)) FOR [LeaveTime]
GO
ALTER TABLE [dbo].[BattleOption] ADD  CONSTRAINT [DF_BattleOption_StationID]  DEFAULT ((8000)) FOR [StationID]
GO
ALTER TABLE [dbo].[BattleOption] ADD  CONSTRAINT [DF_BattleOption_SettleCurrency]  DEFAULT ((1)) FOR [ScoreKind]
GO
ALTER TABLE [dbo].[BattleOption] ADD  CONSTRAINT [DF_BattleConfig_PlayerCount]  DEFAULT ((0)) FOR [PlayerCount]
GO
ALTER TABLE [dbo].[BattleOption] ADD  CONSTRAINT [DF_BattleOption_UseScene]  DEFAULT ((0)) FOR [EmployScene]
GO
ALTER TABLE [dbo].[BattleOption] ADD  CONSTRAINT [DF_BattleConfig_VideoIngot]  DEFAULT ((0)) FOR [VideoIngot_User]
GO
ALTER TABLE [dbo].[BattleOption] ADD  CONSTRAINT [DF_BattleConfig_VideoRoomCard]  DEFAULT ((0)) FOR [VideoRoomCard_User]
GO
ALTER TABLE [dbo].[BattleOption] ADD  CONSTRAINT [DF_BattleConfig_VideoIngot_User1]  DEFAULT ((0)) FOR [VideoIngot_Owner]
GO
ALTER TABLE [dbo].[BattleOption] ADD  CONSTRAINT [DF_BattleConfig_VideoRoomCard_User1]  DEFAULT ((0)) FOR [VideoRoomCard_Owner]
GO
ALTER TABLE [dbo].[BattleOption] ADD  CONSTRAINT [DF_BattleConfig_SuportPayType]  DEFAULT (0x01) FOR [SuportPayType]
GO
ALTER TABLE [dbo].[BattleOptionCustom] ADD  CONSTRAINT [DF_BattleOptionCustom_StationID]  DEFAULT ((8000)) FOR [StationID]
GO
ALTER TABLE [dbo].[BattleOptionCustom] ADD  CONSTRAINT [DF_BattleOptionCustom_EmployScene]  DEFAULT ((0)) FOR [EmployScene]
GO
ALTER TABLE [dbo].[BattleOptionItem] ADD  CONSTRAINT [DF_BattleOptionItem_OptionID]  DEFAULT ((0)) FOR [OptionID]
GO
ALTER TABLE [dbo].[BattleOptionItem] ADD  CONSTRAINT [DF_BattleConfig_Option_OptionIndex]  DEFAULT ((0)) FOR [ItemIndex]
GO
ALTER TABLE [dbo].[BattleOptionItem] ADD  CONSTRAINT [DF_Table_1_Ingot1_Owner]  DEFAULT ((0)) FOR [Ingot_Owner]
GO
ALTER TABLE [dbo].[BattleOptionItem] ADD  CONSTRAINT [DF_Table_1_RoomCard1_User]  DEFAULT ((0)) FOR [RoomCard_User]
GO
ALTER TABLE [dbo].[BattleOptionItem] ADD  CONSTRAINT [DF_Table_1_RoomCard1_Owner]  DEFAULT ((0)) FOR [RoomCard_Owner]
GO
ALTER TABLE [dbo].[BattleReward] ADD  CONSTRAINT [DF_BattleReward_OptionID]  DEFAULT ((0)) FOR [OptionID]
GO
ALTER TABLE [dbo].[BattleReward] ADD  CONSTRAINT [DF_PrivateTableReward_ConsumeIndex]  DEFAULT ((0)) FOR [ConsumeIndex]
GO
ALTER TABLE [dbo].[BattleUserLocker] ADD  CONSTRAINT [DF_BattleUserLocker_ScoreKind]  DEFAULT ((0)) FOR [ScoreKind]
GO
ALTER TABLE [dbo].[BattleUserLocker] ADD  CONSTRAINT [DF_BattleUserLocker_MappedNum]  DEFAULT ((0)) FOR [MappedNum]
GO
ALTER TABLE [dbo].[BattleUserLocker] ADD  CONSTRAINT [DF_BattleUserLocker_BattleStatus]  DEFAULT ((0)) FOR [BattleStatus]
GO
ALTER TABLE [dbo].[BattleUserLocker] ADD  CONSTRAINT [DF_BattleUserLocker_OnlineStatus]  DEFAULT ((0)) FOR [OnlineStatus]
GO
ALTER TABLE [dbo].[BattleUserLocker] ADD  CONSTRAINT [DF_BattleUserLocker_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[ConfineAddress] ADD  CONSTRAINT [DF_AddrConfineRule_EnjoinLogon]  DEFAULT ((0)) FOR [EnjoinLogon]
GO
ALTER TABLE [dbo].[ConfineAddress] ADD  CONSTRAINT [DF_AddrConfineRule_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[ConfineAddress] ADD  CONSTRAINT [DF_AddrConfineRule_CollectNote]  DEFAULT ('') FOR [CollectNote]
GO
ALTER TABLE [dbo].[ConfineMachine] ADD  CONSTRAINT [DF_MachineConfineRule_EnjoinLogon]  DEFAULT ((0)) FOR [EnjoinLogon]
GO
ALTER TABLE [dbo].[ConfineMachine] ADD  CONSTRAINT [DF_MachineConfineRule_EnjoinRegister]  DEFAULT ((0)) FOR [EnjoinRegister]
GO
ALTER TABLE [dbo].[ConfineMachine] ADD  CONSTRAINT [DF_MachineConfineRule_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[ConfineMachine] ADD  CONSTRAINT [DF_MachineConfineRule_CollectNote]  DEFAULT ('') FOR [CollectNote]
GO
ALTER TABLE [dbo].[GameColumnItem] ADD  CONSTRAINT [DF_GameColumnItem_SuportType_1]  DEFAULT ((0)) FOR [SuportType]
GO
ALTER TABLE [dbo].[GameClubScoreInfo] ADD  CONSTRAINT [DF_GameClubScoreInfo_FillCount]  DEFAULT ((0)) FOR [FillCount]
GO
ALTER TABLE [dbo].[GameClubScoreInfo] ADD  CONSTRAINT [DF_GameClubScoreInfo_RevenueCount]  DEFAULT ((0)) FOR [RevenueCount]
GO
ALTER TABLE [dbo].[GameClubScoreInfo] ADD  CONSTRAINT [DF_GameClubScoreInfo_ExchangeCount]  DEFAULT ((0)) FOR [ExchangeCount]
GO
ALTER TABLE [dbo].[GameClubScoreInfo] ADD  CONSTRAINT [DF_GameClubScoreInfo_RemainCount]  DEFAULT ((0)) FOR [RemainCount]
GO
ALTER TABLE [dbo].[GameClubScoreInfo] ADD  CONSTRAINT [DF_GameClubScoreInfo_PresentCount]  DEFAULT ((0)) FOR [PresentCount]
GO
ALTER TABLE [dbo].[GameIngotInfo] ADD  CONSTRAINT [DF_GameIngotInfo_StationID]  DEFAULT ((0)) FOR [StationID]
GO
ALTER TABLE [dbo].[GameIngotInfo] ADD  CONSTRAINT [DF_GameIngotInfo_Ingot]  DEFAULT ((0)) FOR [Ingot]
GO
ALTER TABLE [dbo].[GameIngotInfo] ADD  CONSTRAINT [DF_GameIngotInfo_FillCount]  DEFAULT ((0)) FOR [FillCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_UserStationID]  DEFAULT ((0)) FOR [StationID]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_WinCount]  DEFAULT ((0)) FOR [WinCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_LostCount]  DEFAULT ((0)) FOR [LostCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_DrawCount]  DEFAULT ((0)) FOR [DrawCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_FleeCount]  DEFAULT ((0)) FOR [FleeCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_UserRight]  DEFAULT ((0)) FOR [UserRight]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_MasterRight]  DEFAULT ((0)) FOR [MasterRight]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_MasterOrder]  DEFAULT ((0)) FOR [MasterOrder]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_AllLogonTimes]  DEFAULT ((0)) FOR [AllLogonTimes]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_PlayTimeCount_1]  DEFAULT ((0)) FOR [PlayTimeCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_OnLineTimeCount]  DEFAULT ((0)) FOR [OnLineTimeCount]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_LastLogonDate]  DEFAULT (getdate()) FOR [LastLogonDate]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_LastLogonMachine]  DEFAULT ('------------') FOR [LastLogonMachine]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScore_RegisterDate]  DEFAULT (getdate()) FOR [RegisterDate]
GO
ALTER TABLE [dbo].[GameScoreInfo] ADD  CONSTRAINT [DF_GameScoreInfo_RegisterMachine]  DEFAULT (N'------------') FOR [RegisterMachine]
GO
ALTER TABLE [dbo].[ClubMember_0] ADD  CONSTRAINT [DF__ClubMemb__Paren__6C1B70C0]  DEFAULT ((0)) FOR [ParentID]
GO
ALTER TABLE [dbo].[ClubMember_0] ADD  CONSTRAINT [DF__ClubMemb__Membe__6D0F94F9]  DEFAULT ((0)) FOR [MemberType]
GO
ALTER TABLE [dbo].[ClubMember_0] ADD  CONSTRAINT [DF__ClubMemb__Membe__6E03B932]  DEFAULT ((0)) FOR [MemberRate]
GO
ALTER TABLE [dbo].[ClubMember_0] ADD  CONSTRAINT [DF__ClubMemb__Membe__6EF7DD6B]  DEFAULT ((0)) FOR [MemberStatus]
GO
ALTER TABLE [dbo].[ClubMember_0] ADD  CONSTRAINT [DF__ClubMemb__Membe__6FEC01A4]  DEFAULT ('') FOR [MemberFace]
GO
ALTER TABLE [dbo].[ClubMember_0] ADD  CONSTRAINT [DF__ClubMemb__IsAnd__70E025DD]  DEFAULT ((0)) FOR [IsAndroid]
GO
ALTER TABLE [dbo].[ClubMember_0] ADD  CONSTRAINT [DF__ClubMemb__Club__71D44A16]  DEFAULT ((0)) FOR [ClubScore]
GO
ALTER TABLE [dbo].[ClubMember_0] ADD  CONSTRAINT [DF_ClubMember_0_RedIngot]  DEFAULT ((0)) FOR [ClubCowry]
GO
ALTER TABLE [dbo].[ClubMember_0] ADD  CONSTRAINT [DF__ClubMemb__Creat__72C86E4F]  DEFAULT ((0)) FOR [CreateCount]
GO
ALTER TABLE [dbo].[ClubMember_0] ADD  CONSTRAINT [DF__ClubMemb__Battl__73BC9288]  DEFAULT ((0)) FOR [BattleCount]
GO
ALTER TABLE [dbo].[ClubMember_0] ADD  CONSTRAINT [DF__ClubMemb__Reven__74B0B6C1]  DEFAULT ((0)) FOR [RevenueCount]
GO
ALTER TABLE [dbo].[ClubMember_0] ADD  CONSTRAINT [DF__ClubMemb__Addup__75A4DAFA]  DEFAULT ((0)) FOR [AddupWinRate]
GO
ALTER TABLE [dbo].[ClubMember_0] ADD  CONSTRAINT [DF__ClubMemb__Addup__7698FF33]  DEFAULT ((0)) FOR [AddupWinScore]
GO
ALTER TABLE [dbo].[ClubMember_0] ADD  CONSTRAINT [DF__ClubMemb__Addup__778D236C]  DEFAULT ((0)) FOR [AddupLoseScore]
GO
ALTER TABLE [dbo].[ClubMember_0] ADD  CONSTRAINT [DF__ClubMemb__JoinD__788147A5]  DEFAULT (getdate()) FOR [JoinDateTime]
GO
ALTER TABLE [dbo].[ClubMember_0] ADD  CONSTRAINT [DF__ClubMemb__LastB__79756BDE]  DEFAULT (getdate()) FOR [LastBattleTime]
GO
ALTER TABLE [dbo].[ClubProperty] ADD  CONSTRAINT [DF_ClubProperty_StationID]  DEFAULT ((2000)) FOR [StationID]
GO
ALTER TABLE [dbo].[ClubProperty] ADD  CONSTRAINT [DF_ClubProperty_CreaterNickName]  DEFAULT ('') FOR [CreaterNickName]
GO
ALTER TABLE [dbo].[ClubProperty] ADD  CONSTRAINT [DF_ClubProperty_ClubLevel]  DEFAULT ((0)) FOR [ClubLevel]
GO
ALTER TABLE [dbo].[ClubProperty] ADD  CONSTRAINT [DF_ClubProperty_ClubStatus]  DEFAULT ((1)) FOR [ClubStatus]
GO
ALTER TABLE [dbo].[ClubProperty] ADD  CONSTRAINT [DF_ClubProperty_ClubIntroduce]  DEFAULT ('') FOR [ClubIntroduce]
GO
ALTER TABLE [dbo].[ClubProperty] ADD  CONSTRAINT [DF_ClubProperty_ClubRate]  DEFAULT ((600)) FOR [ClubRate]
GO
ALTER TABLE [dbo].[ClubProperty] ADD  CONSTRAINT [DF_ClubProperty_PayrollerID]  DEFAULT ((0)) FOR [PayrollerID]
GO
ALTER TABLE [dbo].[ClubProperty] ADD  CONSTRAINT [DF_ClubProperty_MemberCount]  DEFAULT ((0)) FOR [MemberCount]
GO
ALTER TABLE [dbo].[ClubProperty] ADD  CONSTRAINT [DF_ClubProperty_MaxMemberCount]  DEFAULT ((0)) FOR [MaxMemberCount]
GO
ALTER TABLE [dbo].[ClubProperty] ADD  CONSTRAINT [DF_ClubProperty_CreateDateTime]  DEFAULT (getdate()) FOR [CreateDateTime]
GO
ALTER TABLE [dbo].[ClubRules] ADD  CONSTRAINT [DF_ClubRules_PayType]  DEFAULT ((0)) FOR [PayType]
GO
ALTER TABLE [dbo].[ClubRules] ADD  CONSTRAINT [DF_ClubRules_RevenueRate]  DEFAULT ((0)) FOR [RevenueRate]
GO
ALTER TABLE [dbo].[ClubRules] ADD  CONSTRAINT [DF_ClubRules_BattleMask]  DEFAULT ((0)) FOR [BattleMask]
GO
ALTER TABLE [dbo].[ClubRules] ADD  CONSTRAINT [DF_ClubRules_ScoreRight]  DEFAULT ((0)) FOR [ScoreRight]
GO
ALTER TABLE [dbo].[ClubRules] ADD  CONSTRAINT [DF_ClubRules_ScoreAdjust]  DEFAULT ((0)) FOR [ScoreAdjust]
GO
ALTER TABLE [dbo].[ClubRules] ADD  CONSTRAINT [DF_ClubRules_SwitchOption]  DEFAULT ((15)) FOR [SwitchOption]
GO
ALTER TABLE [dbo].[ClubRules] ADD  CONSTRAINT [DF_ClubRules_AutoCleanup]  DEFAULT ((0)) FOR [AutoCleanup]
GO
ALTER TABLE [dbo].[ClubUserInfo] ADD  CONSTRAINT [DF_ClubUserInfo_ClubIDArray]  DEFAULT (N';') FOR [ClubIDArray]
GO
ALTER TABLE [dbo].[NormalClubID] ADD  CONSTRAINT [DF_NormalClubID_Distribute]  DEFAULT ((0)) FOR [Distribute]
GO
ALTER TABLE [dbo].[RecordDrawInfo] ADD  CONSTRAINT [DF_RecordDrawInfo_ServerType]  DEFAULT ((0)) FOR [ScoreKind]
GO
ALTER TABLE [dbo].[RecordDrawInfo] ADD  CONSTRAINT [DF_RecordDrawInfo_GameCourse]  DEFAULT ('') FOR [GameCourse]
GO
ALTER TABLE [dbo].[RecordDrawInfo] ADD  CONSTRAINT [DF_RecordDrawInfo_BattleRecordID]  DEFAULT ((0)) FOR [BattleRecordID]
GO
ALTER TABLE [dbo].[RecordDrawInfo] ADD  CONSTRAINT [DF_RecordDrawInfo_VideoNumber]  DEFAULT ('') FOR [VideoNumber]
GO
ALTER TABLE [dbo].[RecordDrawInfo] ADD  CONSTRAINT [DF_RecordDrawInfo_RecordDate]  DEFAULT (getdate()) FOR [RecordDate]
GO
ALTER TABLE [dbo].[RecordDrawScore] ADD  CONSTRAINT [DF_RecordDrawScore_BattleRecordID]  DEFAULT ((0)) FOR [BattleRecordID]
GO
ALTER TABLE [dbo].[RecordDrawScore] ADD  CONSTRAINT [DF_RecordDrawScore_VideoNumber]  DEFAULT ('') FOR [VideoNumber]
GO
ALTER TABLE [dbo].[RecordDrawScore] ADD  CONSTRAINT [DF_RecordDrawScore_RecordDate]  DEFAULT (getdate()) FOR [RecordDate]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_RecordGameScore_ServerType]  DEFAULT ((0)) FOR [ScoreKind]
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
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_RecordGameScore_GameCourse]  DEFAULT ('') FOR [GameCourse]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_RecordGameScore_BattleRecordID]  DEFAULT ((0)) FOR [BattleRecordID]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_RecordGameScore_VideoNumber]  DEFAULT ('') FOR [VideoNumber]
GO
ALTER TABLE [dbo].[RecordGameScore] ADD  CONSTRAINT [DF_GameScoreRecord_RecordDate]  DEFAULT (getdate()) FOR [RecordDate]
GO
ALTER TABLE [dbo].[RecordClubApply] ADD  CONSTRAINT [DF_ClubApplyRecord_ApplyerGender]  DEFAULT ((0)) FOR [ApplyerGender]
GO
ALTER TABLE [dbo].[RecordClubApply] ADD  CONSTRAINT [DF_ClubApplyRecord_ApplyerFaceID]  DEFAULT ((0)) FOR [ApplyerFaceID]
GO
ALTER TABLE [dbo].[RecordClubApply] ADD  CONSTRAINT [DF_ClubApplyRecord_ApplyerNickName]  DEFAULT ('') FOR [ApplyerNickName]
GO
ALTER TABLE [dbo].[RecordClubApply] ADD  CONSTRAINT [DF_ClubApplyRecord_ApplyType]  DEFAULT ((0)) FOR [ApplyType]
GO
ALTER TABLE [dbo].[RecordClubApply] ADD  CONSTRAINT [DF_ClubApplyRecord_ResponderID]  DEFAULT ((0)) FOR [ResponderID]
GO
ALTER TABLE [dbo].[RecordClubApply] ADD  CONSTRAINT [DF_ClubApplyRecord_ViewMask]  DEFAULT ((0)) FOR [DeleteMask]
GO
ALTER TABLE [dbo].[RecordClubApply] ADD  CONSTRAINT [DF_ClubApplyRecord_Remarks]  DEFAULT ('') FOR [Remarks]
GO
ALTER TABLE [dbo].[RecordClubApply] ADD  CONSTRAINT [DF_ClubApplyRecord_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordClubInvite] ADD  CONSTRAINT [DF_ClubInviteRecord_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordClubMerge] ADD  CONSTRAINT [DF_ClubMergeRecord_ClubName]  DEFAULT ('') FOR [RequestClubName]
GO
ALTER TABLE [dbo].[RecordClubMerge] ADD  CONSTRAINT [DF_ClubMergeRecord_MergeClubName]  DEFAULT ('') FOR [RespondClubName]
GO
ALTER TABLE [dbo].[RecordClubMerge] ADD  CONSTRAINT [DF_ClubMergeRecord_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordClubNotice] ADD  CONSTRAINT [DF_RecordClubNotice_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordClubPayroll] ADD  CONSTRAINT [DF_ClubPayrollRecord_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordUserInout] ADD  CONSTRAINT [DF_RecordUserInout_StationID]  DEFAULT ((0)) FOR [StationID]
GO
ALTER TABLE [dbo].[RecordUserInout] ADD  CONSTRAINT [DF_RecordUserInout_CurrencyID]  DEFAULT ((0)) FOR [CurrencyID]
GO
ALTER TABLE [dbo].[RecordUserInout] ADD  CONSTRAINT [DF_RecordUserInout_DeviceType]  DEFAULT ((0)) FOR [DeviceType]
GO
ALTER TABLE [dbo].[RecordUserInout] ADD  CONSTRAINT [DF_RecordUserInout_EnterTime]  DEFAULT (getdate()) FOR [EnterTime]
GO
ALTER TABLE [dbo].[RecordUserInout] ADD  CONSTRAINT [DF_RecordUserInout_EnterGold]  DEFAULT ((0)) FOR [EnterGold]
GO
ALTER TABLE [dbo].[RecordUserInout] ADD  CONSTRAINT [DF_RecordUserInout_ClubID]  DEFAULT ((0)) FOR [ClubID]
GO
ALTER TABLE [dbo].[RequestWriteRecord] ADD  CONSTRAINT [DF_RequestWriteScore_RequestDate]  DEFAULT (getdate()) FOR [RequestDate]
GO
ALTER TABLE [dbo].[ReserveClubID] ADD  CONSTRAINT [DF_ReserveClubID_Distribute]  DEFAULT ((0)) FOR [Distribute]
GO
ALTER TABLE [dbo].[StreamBattleInfo] ADD  CONSTRAINT [DF_StreamBattleInfo_BattleCount]  DEFAULT ((0)) FOR [BattleCount]
GO
ALTER TABLE [dbo].[StreamBattleInfo] ADD  CONSTRAINT [DF_StreamBattleInfo_BattleCount1]  DEFAULT ((0)) FOR [BattleFinishCount]
GO
ALTER TABLE [dbo].[StreamBattleInfo] ADD  CONSTRAINT [DF_StreamBattleInfo_IngotCount]  DEFAULT ((0)) FOR [IngotCount]
GO
ALTER TABLE [dbo].[StreamBattleInfo] ADD  CONSTRAINT [DF_StreamBattleInfo_CardCount]  DEFAULT ((0)) FOR [CardCount]
GO
ALTER TABLE [dbo].[StreamBattleKindInfo] ADD  CONSTRAINT [DF_StreamBattleKindInfo_BattleCount]  DEFAULT ((0)) FOR [BattleCount]
GO
ALTER TABLE [dbo].[StreamBattleKindInfo] ADD  CONSTRAINT [DF_StreamBattleKindInfo_WinCount]  DEFAULT ((0)) FOR [WinCount]
GO
ALTER TABLE [dbo].[StreamBattleKindInfo] ADD  CONSTRAINT [DF_StreamBattleKindInfo_LostCount]  DEFAULT ((0)) FOR [LostCount]
GO
ALTER TABLE [dbo].[StreamBattleKindInfo] ADD  CONSTRAINT [DF_StreamBattleKindInfo_PlayTimeCount]  DEFAULT ((0)) FOR [PlayTimeCount]
GO
ALTER TABLE [dbo].[StreamClubBalance] ADD  CONSTRAINT [DF_IMClubBalance_DateID]  DEFAULT ((0)) FOR [DateID]
GO
ALTER TABLE [dbo].[StreamClubBalance] ADD  CONSTRAINT [DF_IMClubBalance_ClubID]  DEFAULT ((0)) FOR [ClubID]
GO
ALTER TABLE [dbo].[StreamClubBalance] ADD  CONSTRAINT [DF_IMClubBalance_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[StreamClubBalance] ADD  CONSTRAINT [DF_StreamClubBalance_GStationID]  DEFAULT ((0)) FOR [GStationID]
GO
ALTER TABLE [dbo].[StreamClubBalance] ADD  CONSTRAINT [DF_StreamClubBalance_WinCount]  DEFAULT ((0)) FOR [WinCount]
GO
ALTER TABLE [dbo].[StreamClubBalance] ADD  CONSTRAINT [DF_StreamClubBalance_WinScore]  DEFAULT ((0)) FOR [WinScore]
GO
ALTER TABLE [dbo].[StreamClubBalance] ADD  CONSTRAINT [DF_StreamClubBalance_LostCount]  DEFAULT ((0)) FOR [LostCount]
GO
ALTER TABLE [dbo].[StreamClubBalance] ADD  CONSTRAINT [DF_StreamClubBalance_LostScore]  DEFAULT ((0)) FOR [LostScore]
GO
ALTER TABLE [dbo].[StreamClubBalance] ADD  CONSTRAINT [DF_StreamClubBalance_IncreaseScore]  DEFAULT ((0)) FOR [IncreaseScore]
GO
ALTER TABLE [dbo].[StreamClubBalance] ADD  CONSTRAINT [DF_StreamClubBalance_DecreaseScore]  DEFAULT ((0)) FOR [DecreaseScore]
GO
ALTER TABLE [dbo].[StreamClubBalance] ADD  CONSTRAINT [DF_StreamClubBalance_IncreaseCowry]  DEFAULT ((0)) FOR [IncreaseCowry]
GO
ALTER TABLE [dbo].[StreamClubBalance] ADD  CONSTRAINT [DF_StreamClubBalance_DecreaseCowry]  DEFAULT ((0)) FOR [DecreaseCowry]
GO
ALTER TABLE [dbo].[StreamClubBalance] ADD  CONSTRAINT [DF_StreamClubBalance_TradeIn]  DEFAULT ((0)) FOR [TradeIn]
GO
ALTER TABLE [dbo].[StreamClubBalance] ADD  CONSTRAINT [DF_StreamClubBalance_TradeOut]  DEFAULT ((0)) FOR [TradeOut]
GO
ALTER TABLE [dbo].[StreamClubBalance] ADD  CONSTRAINT [DF_StreamClubBalance_Revenue]  DEFAULT ((0)) FOR [Revenue]
GO
ALTER TABLE [dbo].[StreamClubBalance] ADD  CONSTRAINT [DF_StreamClubBalance_RevenueCowry]  DEFAULT ((0)) FOR [RevenueCowry]
GO
ALTER TABLE [dbo].[StreamClubBalance] ADD  CONSTRAINT [DF_IMClubBalance_BattleCount]  DEFAULT ((0)) FOR [BattleCount]
GO
ALTER TABLE [dbo].[StreamClubBalance] ADD  CONSTRAINT [DF_IMClubBalance_BalanceCount]  DEFAULT ((0)) FOR [BalanceCount]
GO
ALTER TABLE [dbo].[StreamClubBalance] ADD  CONSTRAINT [DF_StreamClubBalance_IngotCount]  DEFAULT ((0)) FOR [IngotCount]
GO
ALTER TABLE [dbo].[StreamClubBalance] ADD  CONSTRAINT [DF_IMClubBalance_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[StreamClubBattle] ADD  CONSTRAINT [DF_StreamClubBattle_DateID]  DEFAULT ((0)) FOR [DateID]
GO
ALTER TABLE [dbo].[StreamClubBattle] ADD  CONSTRAINT [DF_StreamClubBattle_ClubID]  DEFAULT ((0)) FOR [ClubID]
GO
ALTER TABLE [dbo].[StreamClubBattle] ADD  CONSTRAINT [DF_StreamClubBattle_BattleCount]  DEFAULT ((0)) FOR [BattleCount]
GO
ALTER TABLE [dbo].[StreamClubBattle] ADD  CONSTRAINT [DF_StreamClubBattle_BattleFinishCount]  DEFAULT ((0)) FOR [BattleFinishCount]
GO
ALTER TABLE [dbo].[StreamClubBattle] ADD  CONSTRAINT [DF_StreamClubBattle_IncreaseScore]  DEFAULT ((0)) FOR [IncreaseScore]
GO
ALTER TABLE [dbo].[StreamClubBattle] ADD  CONSTRAINT [DF_StreamClubBattle_DecreaseScore]  DEFAULT ((0)) FOR [DecreaseScore]
GO
ALTER TABLE [dbo].[StreamClubBattle] ADD  CONSTRAINT [DF_StreamClubBattle_IncreaseCowry]  DEFAULT ((0)) FOR [IncreaseCowry]
GO
ALTER TABLE [dbo].[StreamClubBattle] ADD  CONSTRAINT [DF_StreamClubBattle_DecreaseCowry]  DEFAULT ((0)) FOR [DecreaseCowry]
GO
ALTER TABLE [dbo].[StreamClubBattle] ADD  CONSTRAINT [DF_StreamClubBattle_Revenue]  DEFAULT ((0)) FOR [Revenue]
GO
ALTER TABLE [dbo].[StreamClubBattle] ADD  CONSTRAINT [DF_StreamClubBattle_RevenueCowry]  DEFAULT ((0)) FOR [RevenueCowry]
GO
ALTER TABLE [dbo].[StreamClubBattle] ADD  CONSTRAINT [DF_StreamClubBattle_IngotCount]  DEFAULT ((0)) FOR [IngotCount]
GO
ALTER TABLE [dbo].[StreamClubPartner] ADD  CONSTRAINT [DF_StreamClubPartner_SupplyRevenue]  DEFAULT ((0)) FOR [SupplyRevenue]
GO
ALTER TABLE [dbo].[StreamClubPartner] ADD  CONSTRAINT [DF_StreamClubPartner_ExtractRevenue]  DEFAULT ((0)) FOR [ExtractRevenue]
GO
ALTER TABLE [dbo].[StreamClubPartner] ADD  CONSTRAINT [DF_StreamClubPartner_WinScore]  DEFAULT ((0)) FOR [WinScore]
GO
ALTER TABLE [dbo].[StreamClubPartner] ADD  CONSTRAINT [DF_StreamClubPartner_LostScore]  DEFAULT ((0)) FOR [LostScore]
GO
ALTER TABLE [dbo].[StreamClubPartner] ADD  CONSTRAINT [DF_StreamClubPartner_WinCount]  DEFAULT ((0)) FOR [WinCount]
GO
ALTER TABLE [dbo].[StreamClubPartner] ADD  CONSTRAINT [DF_StreamClubPartner_LostCount]  DEFAULT ((0)) FOR [LostCount]
GO
ALTER TABLE [dbo].[StreamClubPartner] ADD  CONSTRAINT [DF_StreamClubPartner_BattleCount]  DEFAULT ((0)) FOR [BattleCount]
GO
ALTER TABLE [dbo].[StreamClubUser] ADD  CONSTRAINT [DF_StreamClubUser_DateID]  DEFAULT ((0)) FOR [DateID]
GO
ALTER TABLE [dbo].[StreamClubUser] ADD  CONSTRAINT [DF_StreamClubUser_WinCount]  DEFAULT ((0)) FOR [WinCount]
GO
ALTER TABLE [dbo].[StreamClubUser] ADD  CONSTRAINT [DF_StreamClubUser_WinScore]  DEFAULT ((0)) FOR [WinScore]
GO
ALTER TABLE [dbo].[StreamClubUser] ADD  CONSTRAINT [DF_StreamClubUser_LostCount]  DEFAULT ((0)) FOR [LostCount]
GO
ALTER TABLE [dbo].[StreamClubUser] ADD  CONSTRAINT [DF_StreamClubUser_LostScore]  DEFAULT ((0)) FOR [LostScore]
GO
ALTER TABLE [dbo].[StreamClubUser] ADD  CONSTRAINT [DF_StreamClubUser_BattleCount]  DEFAULT ((0)) FOR [BattleCount]
GO
ALTER TABLE [dbo].[StreamClubUser] ADD  CONSTRAINT [DF_StreamClubUser_LastBattleTime]  DEFAULT (getdate()) FOR [LastBattleTime]
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
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'控制类型（0、胜率控制  1、局数控制 2、时间控制）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsControl', @level2type=N'COLUMN',@level2name=N'ControlKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'控制模式（1：自动模式 2：手动模式）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsControl', @level2type=N'COLUMN',@level2name=N'ControlMode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'控制状态（0：常规状态 1：白名单  2：黑名单）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsControl', @level2type=N'COLUMN',@level2name=N'ControlStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标胜率' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsControl', @level2type=N'COLUMN',@level2name=N'TargetWinRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'控制状态下的胜率' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsControl', @level2type=N'COLUMN',@level2name=N'ControlWinRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赢局' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsControl', @level2type=N'COLUMN',@level2name=N'TotalWinCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'输局' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsControl', @level2type=N'COLUMN',@level2name=N'TotalLoseCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'累计赢分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsControl', @level2type=N'COLUMN',@level2name=N'TotalWinScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'累计输分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsControl', @level2type=N'COLUMN',@level2name=N'TotalLoseScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'批次标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AndroidBatchConfig', @level2type=N'COLUMN',@level2name=N'BatchID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'群组标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AndroidBatchConfig', @level2type=N'COLUMN',@level2name=N'ClubID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'进入时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AndroidBatchConfig', @level2type=N'COLUMN',@level2name=N'EnterTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'离开时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AndroidBatchConfig', @level2type=N'COLUMN',@level2name=N'LeaveTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AndroidBatchMember', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'批次标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AndroidBatchMember', @level2type=N'COLUMN',@level2name=N'BatchID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AndroidBatchMember', @level2type=N'COLUMN',@level2name=N'GameKindID1'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'行为模板' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AndroidBatchMember', @level2type=N'COLUMN',@level2name=N'ActionTemplateID1'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'补给模板' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AndroidBatchMember', @level2type=N'COLUMN',@level2name=N'SupplyTemplateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleOption', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'分数类型(1: 金币 2:积分 3：茶馆分)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleOption', @level2type=N'COLUMN',@level2name=N'ScoreKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏人数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleOption', @level2type=N'COLUMN',@level2name=N'PlayerCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'使用场景（0：馆外使用 1：馆内使用）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleOption', @level2type=N'COLUMN',@level2name=N'EmployScene'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'视频费用' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleOption', @level2type=N'COLUMN',@level2name=N'VideoIngot_User'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'视频费用' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleOption', @level2type=N'COLUMN',@level2name=N'VideoRoomCard_User'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'视频费用' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleOption', @level2type=N'COLUMN',@level2name=N'VideoIngot_Owner'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'视频费用' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleOption', @level2type=N'COLUMN',@level2name=N'VideoRoomCard_Owner'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最小底分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleOption', @level2type=N'COLUMN',@level2name=N'MinBaseScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最大底分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleOption', @level2type=N'COLUMN',@level2name=N'MaxBaseScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'支付方式（0x01: 房主支付 0x02: AA支付）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleOption', @level2type=N'COLUMN',@level2name=N'SuportPayType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'选项数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleOption', @level2type=N'COLUMN',@level2name=N'ItemCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'选项掩码（0x01 表示游戏时长可选  0x02表示游戏局数可选  0x04表示支持钻石  0x08表示支持房卡 0x10表示支持视频)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleOption', @level2type=N'COLUMN',@level2name=N'OptionMask'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁用标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleOption', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'使用场景(0: 馆外使用 1: 馆内使用)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleOptionCustom', @level2type=N'COLUMN',@level2name=N'EmployScene'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'配置标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleOptionItem', @level2type=N'COLUMN',@level2name=N'OptionID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏时长' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleOptionItem', @level2type=N'COLUMN',@level2name=N'PlayTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleOptionItem', @level2type=N'COLUMN',@level2name=N'PlayCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'消耗钻石数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleOptionItem', @level2type=N'COLUMN',@level2name=N'Ingot_User'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'配置标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleReward', @level2type=N'COLUMN',@level2name=N'OptionID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'消费索引' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleReward', @level2type=N'COLUMN',@level2name=N'ConsumeIndex'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'开始名次' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleReward', @level2type=N'COLUMN',@level2name=N'RankIDStart'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'结束名次' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleReward', @level2type=N'COLUMN',@level2name=N'RankIDEnd'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'奖牌奖励' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleReward', @level2type=N'COLUMN',@level2name=N'RewardMedal'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'分数类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleUserLocker', @level2type=N'COLUMN',@level2name=N'ScoreKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'约战状态（0：未开始  1：开始）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleUserLocker', @level2type=N'COLUMN',@level2name=N'BattleStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'在线状态（0：不在线 1：在线）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BattleUserLocker', @level2type=N'COLUMN',@level2name=N'OnlineStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'地址字符' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineAddress', @level2type=N'COLUMN',@level2name=N'AddrString'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'限制登陆' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineAddress', @level2type=N'COLUMN',@level2name=N'EnjoinLogon'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'过期时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineAddress', @level2type=N'COLUMN',@level2name=N'EnjoinOverDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收集日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineAddress', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'输入备注' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineAddress', @level2type=N'COLUMN',@level2name=N'CollectNote'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器序列' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineMachine', @level2type=N'COLUMN',@level2name=N'MachineSerial'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'限制登录' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineMachine', @level2type=N'COLUMN',@level2name=N'EnjoinLogon'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'限制注册' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineMachine', @level2type=N'COLUMN',@level2name=N'EnjoinRegister'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'过期时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineMachine', @level2type=N'COLUMN',@level2name=N'EnjoinOverDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收集日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineMachine', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'输入备注' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineMachine', @level2type=N'COLUMN',@level2name=N'CollectNote'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排列标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameColumnItem', @level2type=N'COLUMN',@level2name=N'SortID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'列头名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameColumnItem', @level2type=N'COLUMN',@level2name=N'ColumnName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'列表宽度' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameColumnItem', @level2type=N'COLUMN',@level2name=N'ColumnWidth'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数据描述' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameColumnItem', @level2type=N'COLUMN',@level2name=N'DataDescribe'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'值为房间类型和桌子模式的组合   房间类型（金币类型: 0x0001   点值类型： 0x0002  比赛类型： 0x0004  约战类型：0x0008  福利类型： 0x0010）  
桌子模式（分配模式：0x1000 普通模式： 0x2000）

' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameColumnItem', @level2type=N'COLUMN',@level2name=N'SuportType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'茶馆分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameClubScoreInfo', @level2type=N'COLUMN',@level2name=N'ClubScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值总额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameClubScoreInfo', @level2type=N'COLUMN',@level2name=N'FillCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'税收总额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameClubScoreInfo', @level2type=N'COLUMN',@level2name=N'RevenueCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'兑换总额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameClubScoreInfo', @level2type=N'COLUMN',@level2name=N'ExchangeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'保留总额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameClubScoreInfo', @level2type=N'COLUMN',@level2name=N'RemainCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送总额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameClubScoreInfo', @level2type=N'COLUMN',@level2name=N'PresentCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'钻石' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameIngotInfo', @level2type=N'COLUMN',@level2name=N'Ingot'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameIngotInfo', @level2type=N'COLUMN',@level2name=N'FillCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户 ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'币种标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'CurrencyID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户站点' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'StationID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'胜局数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'WinCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'输局数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'LostCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'和局数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'DrawCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'逃局数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'FleeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户权限' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'UserRight'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理权限' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'MasterRight'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理等级' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'MasterOrder'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总登陆次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'AllLogonTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'PlayTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'在线时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'OnLineTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上次登陆 IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'LastLogonIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上次登陆时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'LastLogonDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录机器' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'LastLogonMachine'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'注册 IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'RegisterIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'注册时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'RegisterDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'注册机器' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameScoreInfo', @level2type=N'COLUMN',@level2name=N'RegisterMachine'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'茶馆钻石' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubMember_0', @level2type=N'COLUMN',@level2name=N'ClubCowry'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'群组标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubProperty', @level2type=N'COLUMN',@level2name=N'ClubID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'站点标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubProperty', @level2type=N'COLUMN',@level2name=N'StationID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建者标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubProperty', @level2type=N'COLUMN',@level2name=N'CreaterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubProperty', @level2type=N'COLUMN',@level2name=N'CreaterGameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'群主昵称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubProperty', @level2type=N'COLUMN',@level2name=N'CreaterNickName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'群组名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubProperty', @level2type=N'COLUMN',@level2name=N'ClubName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'群组级别（0：普通群组 1：比赛群组）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubProperty', @level2type=N'COLUMN',@level2name=N'ClubLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'群组状态（1：可用 2：不可用）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubProperty', @level2type=N'COLUMN',@level2name=N'ClubStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'群组公告' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubProperty', @level2type=N'COLUMN',@level2name=N'ClubIntroduce'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'馆主税率' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubProperty', @level2type=N'COLUMN',@level2name=N'ClubRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'代付者标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubProperty', @level2type=N'COLUMN',@level2name=N'PayrollerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'成员数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubProperty', @level2type=N'COLUMN',@level2name=N'MemberCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最大成员数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubProperty', @level2type=N'COLUMN',@level2name=N'MaxMemberCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubProperty', @level2type=N'COLUMN',@level2name=N'CreateDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'支付类型（0x01：房主支付 0x02：AA支付）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubRules', @level2type=N'COLUMN',@level2name=N'PayType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'货币类型（0：钻石 1：房卡）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubRules', @level2type=N'COLUMN',@level2name=N'CurrencyKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'约战税收' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubRules', @level2type=N'COLUMN',@level2name=N'RevenueRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'约战掩码（0x01: 允许馆员开房）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubRules', @level2type=N'COLUMN',@level2name=N'BattleMask'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'积分权限：0 为管理员 1为合伙人 2为大联盟' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubRules', @level2type=N'COLUMN',@level2name=N'ScoreRight'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'积分调整' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubRules', @level2type=N'COLUMN',@level2name=N'ScoreAdjust'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'开关选项掩码（1： 标识审核开 2： 标识隐私开 4： 标识排行榜开 8：标识打烊开 16:标识游戏列表开  32:  标识贝壳开）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ClubRules', @level2type=N'COLUMN',@level2name=N'SwitchOption'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawInfo', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawInfo', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawInfo', @level2type=N'COLUMN',@level2name=N'ServerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'分数类型(1: 金币 2:积分 3：茶馆分)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawInfo', @level2type=N'COLUMN',@level2name=N'ScoreKind'
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
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'录像编号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawScore', @level2type=N'COLUMN',@level2name=N'VideoNumber'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'ServerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'分数类型(1: 金币 2:积分 3：茶馆分)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'ScoreKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'桌子号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'TableID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'损耗数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'WasteCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'税收数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'RevenueCount'
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
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏过程' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'GameCourse'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'录像编号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'VideoNumber'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameScore', @level2type=N'COLUMN',@level2name=N'RecordDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'茶馆标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubApply', @level2type=N'COLUMN',@level2name=N'ClubID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'茶馆名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubApply', @level2type=N'COLUMN',@level2name=N'ClubName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'申请者标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubApply', @level2type=N'COLUMN',@level2name=N'ApplyerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'头像ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubApply', @level2type=N'COLUMN',@level2name=N'ApplyerFaceID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'头像标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubApply', @level2type=N'COLUMN',@level2name=N'ApplyerCustomID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'申请者昵称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubApply', @level2type=N'COLUMN',@level2name=N'ApplyerNickName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'申请状态（0：等待审核 1：同意 2：拒绝 3: 撤销）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubApply', @level2type=N'COLUMN',@level2name=N'ApplyStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建者标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubApply', @level2type=N'COLUMN',@level2name=N'CreaterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'响应玩家标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubApply', @level2type=N'COLUMN',@level2name=N'ResponderID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'删除掩码（0：未删除 1：申请者删除 2：馆主删除）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubApply', @level2type=N'COLUMN',@level2name=N'DeleteMask'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubApply', @level2type=N'COLUMN',@level2name=N'Remarks'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录状态（0： 等待 1：同意  2： 拒绝）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubInvite', @level2type=N'COLUMN',@level2name=N'RespondCode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubMerge', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'请求合并茶馆标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubMerge', @level2type=N'COLUMN',@level2name=N'RequestClubID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'请求合并馆主标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubMerge', @level2type=N'COLUMN',@level2name=N'RequestCreaterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'被合并茶馆标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubMerge', @level2type=N'COLUMN',@level2name=N'RespondClubID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'被合并馆主标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubMerge', @level2type=N'COLUMN',@level2name=N'RespondCreaterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'响应代码（0： 未响应  1：同意  2： 拒绝  3： 完成 ）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubMerge', @level2type=N'COLUMN',@level2name=N'RespondCode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录状态（0： 等待 1：同意  2： 拒绝）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClubPayroll', @level2type=N'COLUMN',@level2name=N'RespondCode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'索引标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'ID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'ServerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'站点标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'StationID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'币种标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'CurrencyID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'设备类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'DeviceType'
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
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'茶馆标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'ClubID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'分数类型(1: 金币 2:积分 3：茶馆分)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'ScoreKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'约战房号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserInout', @level2type=N'COLUMN',@level2name=N'MappedNum'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'时间标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBalance', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'茶馆标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBalance', @level2type=N'COLUMN',@level2name=N'ClubID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBalance', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'茶馆站点标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBalance', @level2type=N'COLUMN',@level2name=N'GStationID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'大赢家次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBalance', @level2type=N'COLUMN',@level2name=N'WinCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBalance', @level2type=N'COLUMN',@level2name=N'IncreaseScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBalance', @level2type=N'COLUMN',@level2name=N'DecreaseScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上贝壳' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBalance', @level2type=N'COLUMN',@level2name=N'IncreaseCowry'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下贝壳' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBalance', @level2type=N'COLUMN',@level2name=N'DecreaseCowry'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'转入' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBalance', @level2type=N'COLUMN',@level2name=N'TradeIn'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'转出' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBalance', @level2type=N'COLUMN',@level2name=N'TradeOut'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'税收' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBalance', @level2type=N'COLUMN',@level2name=N'Revenue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'税收' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBalance', @level2type=N'COLUMN',@level2name=N'RevenueCowry'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'约战次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBalance', @level2type=N'COLUMN',@level2name=N'BattleCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'结算次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBalance', @level2type=N'COLUMN',@level2name=N'BalanceCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'钻石消耗' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBalance', @level2type=N'COLUMN',@level2name=N'IngotCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBalance', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBattle', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'茶馆标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBattle', @level2type=N'COLUMN',@level2name=N'ClubID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'开房次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBattle', @level2type=N'COLUMN',@level2name=N'BattleCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'完成约战次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBattle', @level2type=N'COLUMN',@level2name=N'BattleFinishCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBattle', @level2type=N'COLUMN',@level2name=N'IncreaseScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBattle', @level2type=N'COLUMN',@level2name=N'DecreaseScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上贝壳' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBattle', @level2type=N'COLUMN',@level2name=N'IncreaseCowry'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下贝壳' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBattle', @level2type=N'COLUMN',@level2name=N'DecreaseCowry'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'税收' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBattle', @level2type=N'COLUMN',@level2name=N'Revenue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'税收' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBattle', @level2type=N'COLUMN',@level2name=N'RevenueCowry'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'消耗钻石' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubBattle', @level2type=N'COLUMN',@level2name=N'IngotCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubPartner', @level2type=N'COLUMN',@level2name=N'WinCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'俱乐部标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubUser', @level2type=N'COLUMN',@level2name=N'ClubID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubUser', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赢局' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubUser', @level2type=N'COLUMN',@level2name=N'WinCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赢分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubUser', @level2type=N'COLUMN',@level2name=N'WinScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'输局' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubUser', @level2type=N'COLUMN',@level2name=N'LostCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'输分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamClubUser', @level2type=N'COLUMN',@level2name=N'LostScore'
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
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'站点标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamInfo', @level2type=N'COLUMN',@level2name=N'StationID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'进入数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamInfo', @level2type=N'COLUMN',@level2name=N'LogonCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'离开数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamInfo', @level2type=N'COLUMN',@level2name=N'LogOutCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'录入时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamInfo', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
