USE [LHPlatformDB]
GO
/****** Object:  Table [dbo].[AllowAddress]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AllowAddress](
	[StationID] [int] NOT NULL,
	[IPAddress] [nvarchar](15) NOT NULL,
	[Nullity] [bit] NOT NULL,
	[InvalidDate] [datetime] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[CollectNote] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_AllowAddress] PRIMARY KEY CLUSTERED 
(
	[StationID] ASC,
	[IPAddress] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AllowMachine]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AllowMachine](
	[MachineID] [nvarchar](12) NOT NULL,
	[Nullity] [bit] NOT NULL,
	[InvalidDate] [datetime] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[CollectNote] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_AllowMachine] PRIMARY KEY CLUSTERED 
(
	[MachineID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[DataBaseInfo]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[DataBaseInfo](
	[DBInfoID] [int] IDENTITY(1,1) NOT NULL,
	[DBAddr] [nvarchar](15) NOT NULL,
	[DBPort] [int] NOT NULL,
	[DBUser] [nvarchar](512) NOT NULL,
	[DBPassword] [nvarchar](512) NOT NULL,
	[MachineID] [nvarchar](12) NOT NULL,
	[MachineName] [nvarchar](32) NOT NULL,
	[Information] [nvarchar](128) NOT NULL,
 CONSTRAINT [PK_DataBaseInfo_DBInfoID] PRIMARY KEY CLUSTERED 
(
	[DBInfoID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[DataBaseList]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[DataBaseList](
	[DBListID] [int] IDENTITY(1,1) NOT NULL,
	[DBInfoID] [int] NOT NULL,
	[DBName] [nvarchar](32) NOT NULL,
	[DBGenre] [int] NOT NULL,
	[DBAliasName] [nvarchar](32) NOT NULL,
	[Information] [nvarchar](128) NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ExceptAddress]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ExceptAddress](
	[IPAddress] [nvarchar](16) NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[Describe] [nvarchar](128) NOT NULL,
	[CollectTime] [datetime] NOT NULL,
 CONSTRAINT [PK_UpdateExceptAddress] PRIMARY KEY CLUSTERED 
(
	[IPAddress] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ExceptMachine]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ExceptMachine](
	[MachineID] [nvarchar](32) NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[Describe] [nvarchar](128) NOT NULL,
	[CollectTime] [datetime] NOT NULL,
 CONSTRAINT [PK_UpdateExceptMachine] PRIMARY KEY CLUSTERED 
(
	[MachineID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameConfigBasic]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameConfigBasic](
	[KindID] [int] NOT NULL,
	[KindName] [nvarchar](32) NOT NULL,
	[ConfigValue] [nvarchar](max) NOT NULL,
 CONSTRAINT [PK_GameBasicConfig] PRIMARY KEY CLUSTERED 
(
	[KindID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameConfigItem]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameConfigItem](
	[ConfigName] [nvarchar](32) NOT NULL,
	[ConfigMask] [bigint] NOT NULL,
	[ServiceMask] [int] NOT NULL,
 CONSTRAINT [PK_GameConfigInfo] PRIMARY KEY CLUSTERED 
(
	[ConfigName] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameConfigWeight]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameConfigWeight](
	[KindID] [int] NOT NULL,
	[TypeID] [tinyint] NOT NULL,
	[MinTimes] [int] NOT NULL,
	[MaxTimes] [int] NOT NULL,
	[TimesKind] [tinyint] NOT NULL,
	[WeightValue] [int] NOT NULL,
	[WeightRatio]  AS (CONVERT([decimal](18,3),CONVERT([float],[WeightValue])/[dbo].[LHF_SumTimesWeight]([KindID],[TypeID],[TimesKind]))),
 CONSTRAINT [PK_GameWeightConfig] PRIMARY KEY CLUSTERED 
(
	[KindID] ASC,
	[TypeID] ASC,
	[MinTimes] ASC,
	[MaxTimes] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameExpLevel]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameExpLevel](
	[LevelID] [smallint] NOT NULL,
	[NeedPoint] [int] NOT NULL,
	[SpanPoint] [int] NOT NULL,
	[Rewards] [nvarchar](1024) NOT NULL,
 CONSTRAINT [PK_GameOrderConfig] PRIMARY KEY CLUSTERED 
(
	[LevelID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameGiftInfo]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameGiftInfo](
	[GiftID] [int] IDENTITY(1,1) NOT NULL,
	[GiftName] [nvarchar](32) NOT NULL,
	[GoodsID] [int] NOT NULL,
	[Price] [int] NOT NULL,
	[MoneyID] [int] NOT NULL,
	[LoveLiness] [int] NOT NULL,
	[SoldCount] [int] NOT NULL,
	[SortID] [int] NOT NULL,
	[SendType] [tinyint] NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[Describe] [nvarchar](max) NOT NULL,
	[ModifyDateTime] [datetime] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_GameGiftInfo] PRIMARY KEY CLUSTERED 
(
	[GiftID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameGoodsInfo]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameGoodsInfo](
	[GoodsID] [int] NOT NULL,
	[OrderID] [int] NOT NULL,
	[ImageID] [int] NOT NULL,
	[HasIndate] [tinyint] NOT NULL,
	[GoodsKind] [tinyint] NOT NULL,
	[GoodsName] [nvarchar](64) NOT NULL,
	[CanTradeType] [int] NOT NULL,
	[CanStorePlace] [int] NOT NULL,
	[GoodsOption] [nvarchar](2048) NOT NULL,
	[GoodsDescribe] [nvarchar](256) NOT NULL,
 CONSTRAINT [PK_GameGoodsInfo] PRIMARY KEY CLUSTERED 
(
	[GoodsID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameJackpotInfo]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameJackpotInfo](
	[StockID] [int] NOT NULL,
	[LevelID] [tinyint] NOT NULL,
	[ExtraScore] [bigint] NOT NULL,
	[StockScore] [bigint] NOT NULL,
	[VirtualScore] [bigint] NOT NULL,
	[ShrinkRatio] [smallint] NOT NULL,
	[PayoutProb] [smallint] NOT NULL,
	[PayoutLimit] [bigint] NOT NULL,
	[PayoutOrigin] [bigint] NOT NULL,
	[FirstGold] [bigint] NOT NULL,
 CONSTRAINT [PK_GameJackpotInfo] PRIMARY KEY CLUSTERED 
(
	[StockID] ASC,
	[LevelID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameKindItem]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameKindItem](
	[KindID] [int] NOT NULL,
	[TypeID] [int] NOT NULL,
	[JoinID] [int] NOT NULL,
	[SortID] [int] NOT NULL,
	[ModuleID] [int] NOT NULL,
	[KindName] [nvarchar](32) NOT NULL,
	[ProcessName] [nvarchar](32) NOT NULL,
	[DBListIndex] [int] NOT NULL,
	[SupportMask] [int] NOT NULL,
	[GameRuleUrl] [nvarchar](256) NOT NULL,
	[DownLoadUrl] [nvarchar](256) NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[KindClass] [smallint] NOT NULL,
 CONSTRAINT [PK_GameKindItem_1] PRIMARY KEY CLUSTERED 
(
	[KindID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameModuleInfo]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameModuleInfo](
	[ModuleID] [int] NOT NULL,
	[DBListID] [int] NOT NULL,
	[ModuleName] [nvarchar](31) NOT NULL,
	[SupportType] [int] NOT NULL,
	[ServerVersion] [int] NOT NULL,
	[ClientVersion] [int] NOT NULL,
	[ClientExeName] [nvarchar](31) NOT NULL,
	[ServerDLLName] [nvarchar](31) NOT NULL,
	[AndroidDLLName] [nvarchar](31) NOT NULL,
 CONSTRAINT [PK_GameModuleInfo_ModuleID] PRIMARY KEY NONCLUSTERED 
(
	[ModuleID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameNodeItem]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameNodeItem](
	[NodeID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[JoinID] [int] NOT NULL,
	[SortID] [int] NOT NULL,
	[NodeName] [nvarchar](32) NOT NULL,
	[Nullity] [tinyint] NOT NULL,
 CONSTRAINT [PK_GameNodeItem] PRIMARY KEY CLUSTERED 
(
	[NodeID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GamePageItem]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GamePageItem](
	[PageID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[NodeID] [int] NOT NULL,
	[SortID] [int] NOT NULL,
	[OperateType] [int] NOT NULL,
	[DisplayName] [nvarchar](32) NOT NULL,
	[ResponseUrl] [nvarchar](256) NOT NULL,
	[Nullity] [tinyint] NOT NULL,
 CONSTRAINT [PK_GameCustomItem_CustomID] PRIMARY KEY CLUSTERED 
(
	[PageID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameRankLevel]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameRankLevel](
	[LevelID] [int] NOT NULL,
	[NeedScore] [int] NOT NULL,
	[SpanScore] [int] NOT NULL,
	[MainOrder] [smallint] NOT NULL,
	[SubOrder] [smallint] NOT NULL,
	[StarOrder] [smallint] NOT NULL,
	[LevelName] [nvarchar](64) NOT NULL,
 CONSTRAINT [PK_GameRankingLevel_1] PRIMARY KEY CLUSTERED 
(
	[LevelID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameServerInfo]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameServerInfo](
	[ServerID] [int] IDENTITY(1,1) NOT NULL,
	[ServerName] [nvarchar](31) NOT NULL,
	[KindID] [int] NOT NULL,
	[NodeID] [int] NOT NULL,
	[SortID] [int] NOT NULL,
	[VisibleMask] [int] NOT NULL,
	[ModuleID] [int] NOT NULL,
	[TableCount] [int] NOT NULL,
	[ServerType] [int] NOT NULL,
	[ServerPort] [int] NOT NULL,
	[ServerLevel] [int] NOT NULL,
	[MatchType] [tinyint] NOT NULL,
	[CellScore] [bigint] NOT NULL,
	[ServiceScore] [int] NOT NULL,
	[ExpGainRate] [int] NOT NULL,
	[DBListID] [int] NOT NULL,
	[RestrictScore] [bigint] NOT NULL,
	[MinTableScore] [bigint] NOT NULL,
	[MinEnterScore] [bigint] NOT NULL,
	[MaxEnterScore] [bigint] NOT NULL,
	[MinEnterMember] [int] NOT NULL,
	[MaxEnterMember] [int] NOT NULL,
	[DistributeMode] [tinyint] NOT NULL,
	[DistributeRule] [tinyint] NOT NULL,
	[MinPlayerCount] [smallint] NOT NULL,
	[MinDistributeUser] [smallint] NOT NULL,
	[MinPartakeGameUser] [smallint] NOT NULL,
	[MaxPartakeGameUser] [smallint] NOT NULL,
	[DistributeTimeSpace] [smallint] NOT NULL,
	[MaxPlayer] [int] NOT NULL,
	[ServerRule] [int] NOT NULL,
	[CustomRule] [varbinary](2048) NULL,
	[AttachUserRight] [int] NOT NULL,
	[ServiceAddr] [nvarchar](32) NOT NULL,
	[ServiceDomain] [nvarchar](64) NOT NULL,
	[ServiceMachine] [nvarchar](32) NOT NULL,
	[LessStartPlayer] [int] NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[ServerNote] [nvarchar](64) NOT NULL,
	[CreateDateTime] [datetime] NOT NULL,
	[ModifyDateTime] [datetime] NOT NULL,
	[BattleRule] [int] NOT NULL,
 CONSTRAINT [PK_GameServerInfo_ServerID] PRIMARY KEY CLUSTERED 
(
	[ServerID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameSiftItem]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameSiftItem](
	[ServerID] [int] NOT NULL,
	[SortID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[NodeID] [int] NOT NULL,
	[SiftMask] [int] NOT NULL,
	[ServerName] [nvarchar](31) NOT NULL,
	[Nullity] [tinyint] NOT NULL,
 CONSTRAINT [PK_GameFilterItem_ServerID] PRIMARY KEY CLUSTERED 
(
	[ServerID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameStockInfo]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameStockInfo](
	[StockID] [int] IDENTITY(1,1) NOT NULL,
	[KindID] [smallint] NOT NULL,
	[BetAmount] [int] NOT NULL,
	[StockKind] [int] NOT NULL,
	[StockLevel] [int] NOT NULL,
	[StockName] [nvarchar](50) NOT NULL,
	[ExtraScore] [bigint] NOT NULL,
	[StockScore] [bigint] NOT NULL,
	[StockState] [tinyint] NOT NULL,
	[EnableMode] [tinyint] NOT NULL,
	[EnableEndTime] [bigint] NOT NULL,
	[EnableStartTime] [bigint] NOT NULL,
	[ShrinkProfit] [bigint] NOT NULL,
	[ShrinkOption] [nvarchar](512) NOT NULL,
	[RelationIDList] [nvarchar](128) NOT NULL,
	[StockControlID] [bigint] NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[Describe] [nvarchar](128) NOT NULL,
	[FirstStock] [bigint] NOT NULL,
	[FirstGold] [bigint] NOT NULL,
	[AllAddSubtractStock] [bigint] NOT NULL,
	[AllAddSubtractGold] [bigint] NOT NULL,
 CONSTRAINT [PK_GameStockInfo] PRIMARY KEY CLUSTERED 
(
	[StockID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameTypeItem]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameTypeItem](
	[TypeID] [int] NOT NULL,
	[JoinID] [int] NOT NULL,
	[SortID] [int] NOT NULL,
	[TypeName] [nvarchar](32) NOT NULL,
	[Nullity] [tinyint] NOT NULL,
 CONSTRAINT [PK_GameTypeItem_TypeID] PRIMARY KEY CLUSTERED 
(
	[TypeID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameUserLevel]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameUserLevel](
	[LevelID] [smallint] NOT NULL,
	[NeedPoint] [int] NOT NULL,
	[SpanPoint] [int] NOT NULL,
	[LevelName] [nvarchar](16) NOT NULL,
	[Rewards] [nvarchar](1024) NOT NULL,
 CONSTRAINT [PK_MemberOrder] PRIMARY KEY CLUSTERED 
(
	[LevelID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameVersionInfo]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameVersionInfo](
	[ModuleID] [int] NOT NULL,
	[DeviceKind] [smallint] NOT NULL,
	[Version] [int] NOT NULL,
	[IsCurrent] [tinyint] NOT NULL,
	[Remarks] [nvarchar](64) NULL,
 CONSTRAINT [PK_GameVersionInfo] PRIMARY KEY CLUSTERED 
(
	[ModuleID] ASC,
	[DeviceKind] ASC,
	[Version] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[OnLineServerInfo]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[OnLineServerInfo](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[MachineSerial] [nvarchar](12) NOT NULL,
	[MachineServer] [nvarchar](32) NOT NULL,
	[InsertDateTime] [datetime] NOT NULL,
	[OnLineCountSum] [int] NOT NULL,
	[OnLineCountKind] [nvarchar](2048) NOT NULL,
 CONSTRAINT [PK_OnLineServerInfo_ID] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[OnLineStreamInfo]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[OnLineStreamInfo](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[MarketID] [int] NOT NULL,
	[OnLineUserCount] [int] NOT NULL,
	[OnLinePlazaCount] [int] NOT NULL,
	[OnLineServerCount] [int] NOT NULL,
	[OnLineGoldServerCount] [int] NOT NULL,
	[OnLineMatchServerCount] [int] NOT NULL,
	[OnLineBattleServerCount] [int] NOT NULL,
	[OnLineRankingServerCount] [int] NOT NULL,
	[CountDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_OnLineStreamInfo] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[PlatformConfig]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[PlatformConfig](
	[PlatformID] [int] NOT NULL,
	[PlatformName] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_ThirdPlatform] PRIMARY KEY CLUSTERED 
(
	[PlatformID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ResourceVersion]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ResourceVersion](
	[ResourceID] [int] NOT NULL,
	[Version] [int] NOT NULL,
	[Description] [nvarchar](32) NULL,
 CONSTRAINT [PK_ResourceVersion_1] PRIMARY KEY CLUSTERED 
(
	[ResourceID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[SystemOptionInfo]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[SystemOptionInfo](
	[OptionName] [nvarchar](50) NOT NULL,
	[OptionValue] [nvarchar](max) NOT NULL,
	[OptionState] [tinyint] NOT NULL,
	[OptionDescribe] [nvarchar](512) NOT NULL,
 CONSTRAINT [PK_SystemOptionInfo] PRIMARY KEY CLUSTERED 
(
	[OptionName] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[SystemStatusInfo]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[SystemStatusInfo](
	[StatusKind] [nvarchar](32) NOT NULL,
	[StatusName] [nvarchar](32) NOT NULL,
	[StatusValue] [nvarchar](max) NOT NULL,
	[StatusString] [nvarchar](512) NOT NULL,
 CONSTRAINT [PK_SystemStatusInfo_1] PRIMARY KEY CLUSTERED 
(
	[StatusKind] ASC,
	[StatusName] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[TurntableAwardConfig]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[TurntableAwardConfig](
	[ItemIndex] [tinyint] NOT NULL,
	[ItemWeight] [int] NOT NULL,
	[AwardName] [nvarchar](64) NOT NULL,
	[GoodsID] [int] NOT NULL,
	[GoodsCount] [bigint] NOT NULL,
	[GoodsLevel] [int] NOT NULL,
	[GoodsIndate] [int] NOT NULL,
	[DailyLimitPlaces] [int] NOT NULL,
 CONSTRAINT [PK_TurntableConfig] PRIMARY KEY CLUSTERED 
(
	[ItemIndex] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[UIReferrer]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[UIReferrer](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[Ip] [nvarchar](16) NOT NULL,
	[MachineID] [nvarchar](32) NOT NULL,
	[Referrer] [nvarchar](512) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_UIReferrer] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[UIShieldAccount]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[UIShieldAccount](
	[UserID] [int] NOT NULL,
	[GameID] [int] NOT NULL,
	[ShieldMask] [int] NOT NULL,
	[Describe] [nvarchar](128) NOT NULL,
	[Nullity] [tinyint] NOT NULL,
 CONSTRAINT [PK_UIShieldAccount] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[UIShieldAddress]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[UIShieldAddress](
	[Start_IPAddress] [bigint] NOT NULL,
	[Finish_IPAddress] [bigint] NOT NULL,
	[Start_IPAddress_String] [nvarchar](16) NOT NULL,
	[Finish_IPAddress_String] [nvarchar](16) NOT NULL,
	[ShieldMask] [int] NOT NULL,
	[Describe] [nvarchar](128) NOT NULL,
	[Nullity] [tinyint] NOT NULL,
 CONSTRAINT [PK_UIShieldAddress_1] PRIMARY KEY CLUSTERED 
(
	[Start_IPAddress] ASC,
	[Finish_IPAddress] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[UIShieldMachine]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[UIShieldMachine](
	[MachineID] [nvarchar](32) NOT NULL,
	[ShieldMask] [int] NOT NULL,
	[Describe] [nvarchar](128) NOT NULL,
	[Nullity] [tinyint] NOT NULL,
 CONSTRAINT [PK_UIShieldMachine] PRIMARY KEY CLUSTERED 
(
	[MachineID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[UIWhiteListAddress]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[UIWhiteListAddress](
	[Address] [nvarchar](16) NOT NULL,
	[ExpireTime] [datetime] NOT NULL,
 CONSTRAINT [PK_UIWhiteListAddress] PRIMARY KEY CLUSTERED 
(
	[Address] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[UIWhiteListMachine]    Script Date: 2023/7/12 11:12:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[UIWhiteListMachine](
	[MachineID] [nvarchar](32) NOT NULL,
	[WhiteListType] [tinyint] NOT NULL,
	[Ip] [nvarchar](16) NOT NULL,
	[Referrer] [nvarchar](256) NOT NULL,
 CONSTRAINT [PK_UIWhiteListMachine] PRIMARY KEY CLUSTERED 
(
	[MachineID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
ALTER TABLE [dbo].[AllowAddress] ADD  CONSTRAINT [DF_Table_1_EnjoinLogon]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[AllowAddress] ADD  CONSTRAINT [DF_AllowAddress_InvalidDate]  DEFAULT (getdate()+(1095)) FOR [InvalidDate]
GO
ALTER TABLE [dbo].[AllowAddress] ADD  CONSTRAINT [DF_AllowAddress_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[AllowAddress] ADD  CONSTRAINT [DF_AllowAddress_CollectNote]  DEFAULT ('') FOR [CollectNote]
GO
ALTER TABLE [dbo].[AllowMachine] ADD  CONSTRAINT [DF_MachineAllow_AllowLogon]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[AllowMachine] ADD  CONSTRAINT [DF_MachineAllow_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[AllowMachine] ADD  CONSTRAINT [DF_MachineAllow_CollectNote]  DEFAULT ('') FOR [CollectNote]
GO
ALTER TABLE [dbo].[DataBaseInfo] ADD  CONSTRAINT [DF_TABLE1_DataBasePort]  DEFAULT ((1433)) FOR [DBPort]
GO
ALTER TABLE [dbo].[DataBaseInfo] ADD  CONSTRAINT [DF_DataBaseInfo_MachineID]  DEFAULT (N'') FOR [MachineID]
GO
ALTER TABLE [dbo].[DataBaseInfo] ADD  CONSTRAINT [DF_SQLConnectInfo_NoteInfo]  DEFAULT (N'N('')') FOR [Information]
GO
ALTER TABLE [dbo].[DataBaseList] ADD  CONSTRAINT [DF_DataBaseList_DBGenre]  DEFAULT ((0)) FOR [DBGenre]
GO
ALTER TABLE [dbo].[DataBaseList] ADD  CONSTRAINT [DF_DataBaseList_Information1]  DEFAULT ('') FOR [Information]
GO
ALTER TABLE [dbo].[GameConfigItem] ADD  CONSTRAINT [DF_GameConfigInfo_ApplyServiceMask]  DEFAULT ((0)) FOR [ServiceMask]
GO
ALTER TABLE [dbo].[GameConfigWeight] ADD  CONSTRAINT [DF_GameConfigWeight_TimesKind]  DEFAULT ((0)) FOR [TimesKind]
GO
ALTER TABLE [dbo].[GameExpLevel] ADD  CONSTRAINT [DF_GameExpLevel_NeedPoint]  DEFAULT ((0)) FOR [NeedPoint]
GO
ALTER TABLE [dbo].[GameExpLevel] ADD  CONSTRAINT [DF_GameExpLevel_SpanPoint]  DEFAULT ((0)) FOR [SpanPoint]
GO
ALTER TABLE [dbo].[GameExpLevel] ADD  CONSTRAINT [DF_GameExpLevel_Rewards]  DEFAULT (N'') FOR [Rewards]
GO
ALTER TABLE [dbo].[GameGiftInfo] ADD  CONSTRAINT [DF__GameGiftI__Price__0816FA42]  DEFAULT ((0)) FOR [Price]
GO
ALTER TABLE [dbo].[GameGiftInfo] ADD  CONSTRAINT [DF_GameGiftInfo_MoneyID]  DEFAULT ((0)) FOR [MoneyID]
GO
ALTER TABLE [dbo].[GameGiftInfo] ADD  CONSTRAINT [DF__GameGiftI__LoveL__090B1E7B]  DEFAULT ((0)) FOR [LoveLiness]
GO
ALTER TABLE [dbo].[GameGiftInfo] ADD  CONSTRAINT [DF_GameGiftInfo_SoldCount]  DEFAULT ((0)) FOR [SoldCount]
GO
ALTER TABLE [dbo].[GameGiftInfo] ADD  CONSTRAINT [DF_GameGiftInfo_SortID]  DEFAULT ((0)) FOR [SortID]
GO
ALTER TABLE [dbo].[GameGiftInfo] ADD  CONSTRAINT [DF_GameGiftInfo_SendType]  DEFAULT ((0)) FOR [SendType]
GO
ALTER TABLE [dbo].[GameGiftInfo] ADD  CONSTRAINT [DF__GameGiftI__Nulli__09FF42B4]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[GameGiftInfo] ADD  CONSTRAINT [DF_GameGiftInfo_Describe]  DEFAULT ('') FOR [Describe]
GO
ALTER TABLE [dbo].[GameGiftInfo] ADD  CONSTRAINT [DF_GameGiftInfo_ModifyDateTime]  DEFAULT (getdate()) FOR [ModifyDateTime]
GO
ALTER TABLE [dbo].[GameGiftInfo] ADD  CONSTRAINT [DF_GameGiftInfo_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[GameGoodsInfo] ADD  CONSTRAINT [DF_GameGoodsInfo_ImageID]  DEFAULT ((0)) FOR [ImageID]
GO
ALTER TABLE [dbo].[GameGoodsInfo] ADD  CONSTRAINT [DF_GameGoods_HasIndate]  DEFAULT ((0)) FOR [HasIndate]
GO
ALTER TABLE [dbo].[GameGoodsInfo] ADD  CONSTRAINT [DF_GameGoods_CanRecycle]  DEFAULT ((0)) FOR [CanTradeType]
GO
ALTER TABLE [dbo].[GameGoodsInfo] ADD  CONSTRAINT [DF_GameGoods_CanStorePlace]  DEFAULT ((0)) FOR [CanStorePlace]
GO
ALTER TABLE [dbo].[GameGoodsInfo] ADD  CONSTRAINT [DF_GameGoods_GoodsOption]  DEFAULT ('') FOR [GoodsOption]
GO
ALTER TABLE [dbo].[GameGoodsInfo] ADD  CONSTRAINT [DF_GameGoods_GoodsEffect]  DEFAULT ('') FOR [GoodsDescribe]
GO
ALTER TABLE [dbo].[GameJackpotInfo] ADD  CONSTRAINT [DF_GameJackpotInfo_ExtraScore]  DEFAULT ((0)) FOR [ExtraScore]
GO
ALTER TABLE [dbo].[GameJackpotInfo] ADD  CONSTRAINT [DF_GameJackpotInfo_StockScore]  DEFAULT ((0)) FOR [StockScore]
GO
ALTER TABLE [dbo].[GameJackpotInfo] ADD  CONSTRAINT [DF_GameJackpotInfo_VirtualScore]  DEFAULT ((0)) FOR [VirtualScore]
GO
ALTER TABLE [dbo].[GameJackpotInfo] ADD  CONSTRAINT [DF_GameJackpotInfo_ShrinkRatio]  DEFAULT ((0)) FOR [ShrinkRatio]
GO
ALTER TABLE [dbo].[GameJackpotInfo] ADD  CONSTRAINT [DF_GameJackpotInfo_PayoutLimit]  DEFAULT ((0)) FOR [PayoutLimit]
GO
ALTER TABLE [dbo].[GameJackpotInfo] ADD  CONSTRAINT [DF_GameJackpotInfo_PayoutOrigin]  DEFAULT ((0)) FOR [PayoutOrigin]
GO
ALTER TABLE [dbo].[GameJackpotInfo] ADD  CONSTRAINT [DF_GameJackpotInfo_FirstGold]  DEFAULT ((0)) FOR [FirstGold]
GO
ALTER TABLE [dbo].[GameKindItem] ADD  CONSTRAINT [DF_GameKindItem_JoinID]  DEFAULT ((0)) FOR [JoinID]
GO
ALTER TABLE [dbo].[GameKindItem] ADD  CONSTRAINT [DF_GameKindItem_SortID]  DEFAULT ((1000)) FOR [SortID]
GO
ALTER TABLE [dbo].[GameKindItem] ADD  CONSTRAINT [DF_GameKindItem_DBListIndex]  DEFAULT ((0)) FOR [DBListIndex]
GO
ALTER TABLE [dbo].[GameKindItem] ADD  CONSTRAINT [DF_GameKindItem_SupportMask]  DEFAULT ((0)) FOR [SupportMask]
GO
ALTER TABLE [dbo].[GameKindItem] ADD  CONSTRAINT [DF_GameKindItem_ResponseUrl]  DEFAULT (N'') FOR [GameRuleUrl]
GO
ALTER TABLE [dbo].[GameKindItem] ADD  CONSTRAINT [DF_GameKindItem_DownLoadUrl]  DEFAULT ('') FOR [DownLoadUrl]
GO
ALTER TABLE [dbo].[GameKindItem] ADD  CONSTRAINT [DF_GameKindItem_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[GameKindItem] ADD  CONSTRAINT [DF_GameKindItem_KindClass_1]  DEFAULT ((1)) FOR [KindClass]
GO
ALTER TABLE [dbo].[GameModuleInfo] ADD  CONSTRAINT [DF_GameModuleInfo_ModuleVersion]  DEFAULT ((0)) FOR [ServerVersion]
GO
ALTER TABLE [dbo].[GameModuleInfo] ADD  CONSTRAINT [DF_GameModuleInfo_ClientVersion]  DEFAULT ((0)) FOR [ClientVersion]
GO
ALTER TABLE [dbo].[GameModuleInfo] ADD  CONSTRAINT [DF_GameModuleInfo_AndroidDLLName]  DEFAULT ('') FOR [AndroidDLLName]
GO
ALTER TABLE [dbo].[GameNodeItem] ADD  CONSTRAINT [DF_GameStationItem_JoinID]  DEFAULT ((0)) FOR [JoinID]
GO
ALTER TABLE [dbo].[GameNodeItem] ADD  CONSTRAINT [DF_GameStationItem_SortID]  DEFAULT ((1000)) FOR [SortID]
GO
ALTER TABLE [dbo].[GameNodeItem] ADD  CONSTRAINT [DF_GameStationItem_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[GamePageItem] ADD  CONSTRAINT [DF_GamePageItem_ResponseUrl]  DEFAULT (N'') FOR [ResponseUrl]
GO
ALTER TABLE [dbo].[GamePageItem] ADD  CONSTRAINT [DF_GameCustomItem_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[GameRankLevel] ADD  CONSTRAINT [DF_GameRankingLevel_InitScore]  DEFAULT ((0)) FOR [NeedScore]
GO
ALTER TABLE [dbo].[GameRankLevel] ADD  CONSTRAINT [DF_GameRankingLevel_RankName]  DEFAULT (N'') FOR [LevelName]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_VisibleMask]  DEFAULT ((0)) FOR [VisibleMask]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameRoomInfo_TableCount]  DEFAULT ((60)) FOR [TableCount]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_ServerPort]  DEFAULT ((0)) FOR [ServerPort]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_ServerLevel]  DEFAULT ((0)) FOR [ServerLevel]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_MatchType]  DEFAULT ((0)) FOR [MatchType]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_CellScore]  DEFAULT ((0)) FOR [CellScore]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_ExperienceGain]  DEFAULT ((0)) FOR [ExpGainRate]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_RestrictScore]  DEFAULT ((0)) FOR [RestrictScore]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_MinTableScore]  DEFAULT ((0)) FOR [MinTableScore]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_MinEnterScore]  DEFAULT ((0)) FOR [MinEnterScore]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_MaxEnterScore]  DEFAULT ((0)) FOR [MaxEnterScore]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_MinEnterMember]  DEFAULT ((0)) FOR [MinEnterMember]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_MaxEnterMember]  DEFAULT ((0)) FOR [MaxEnterMember]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_DistributeMode]  DEFAULT ((0)) FOR [DistributeMode]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_DistributeRule]  DEFAULT ((0)) FOR [DistributeRule]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_MinPlayerCount]  DEFAULT ((0)) FOR [MinPlayerCount]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_MinDistributeUser]  DEFAULT ((0)) FOR [MinDistributeUser]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_MinPartakeGameUser]  DEFAULT ((0)) FOR [MinPartakeGameUser]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_MaxPartakeGameUser]  DEFAULT ((0)) FOR [MaxPartakeGameUser]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_DistributeTimeSpace]  DEFAULT ((0)) FOR [DistributeTimeSpace]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_UserRight]  DEFAULT ((0)) FOR [AttachUserRight]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_ServiceAddr]  DEFAULT ('') FOR [ServiceAddr]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_ServiceDomain]  DEFAULT ('') FOR [ServiceDomain]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_MachineSerial]  DEFAULT ('') FOR [ServiceMachine]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_LessStartPlayer]  DEFAULT ((0)) FOR [LessStartPlayer]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_ServerNote]  DEFAULT ('') FOR [ServerNote]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_CreateDataTime]  DEFAULT (getdate()) FOR [CreateDateTime]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF_GameServerInfo_ModifyDateTime]  DEFAULT (getdate()) FOR [ModifyDateTime]
GO
ALTER TABLE [dbo].[GameServerInfo] ADD  CONSTRAINT [DF__GameServe__Battl__7F6BDA51]  DEFAULT ((0)) FOR [BattleRule]
GO
ALTER TABLE [dbo].[GameSiftItem] ADD  CONSTRAINT [DF_GameRoomItem_SortID]  DEFAULT ((1000)) FOR [SortID]
GO
ALTER TABLE [dbo].[GameSiftItem] ADD  CONSTRAINT [DF_GameFilterItem_KindID]  DEFAULT ((0)) FOR [KindID]
GO
ALTER TABLE [dbo].[GameSiftItem] ADD  CONSTRAINT [DF_GameRoomItem_Nullity]  DEFAULT ((0)) FOR [SiftMask]
GO
ALTER TABLE [dbo].[GameSiftItem] ADD  CONSTRAINT [DF_GameRoomItem_Nullity_1]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[GameStockInfo] ADD  CONSTRAINT [DF_StockBaseInfo_KindID]  DEFAULT ((0)) FOR [KindID]
GO
ALTER TABLE [dbo].[GameStockInfo] ADD  CONSTRAINT [DF_StockBaseInfo_BetAmount]  DEFAULT ((0)) FOR [BetAmount]
GO
ALTER TABLE [dbo].[GameStockInfo] ADD  CONSTRAINT [DF_GameStockInfo_StockLevel]  DEFAULT ((1)) FOR [StockLevel]
GO
ALTER TABLE [dbo].[GameStockInfo] ADD  CONSTRAINT [DF_StockBaseInfo_StockName]  DEFAULT (N'') FOR [StockName]
GO
ALTER TABLE [dbo].[GameStockInfo] ADD  CONSTRAINT [DF_GameStockInfo_CurrentScore]  DEFAULT ((0)) FOR [StockScore]
GO
ALTER TABLE [dbo].[GameStockInfo] ADD  CONSTRAINT [DF_StockBaseInfo_StockState]  DEFAULT ((0)) FOR [StockState]
GO
ALTER TABLE [dbo].[GameStockInfo] ADD  CONSTRAINT [DF_StockBaseInfo_EnableMode]  DEFAULT ((0)) FOR [EnableMode]
GO
ALTER TABLE [dbo].[GameStockInfo] ADD  CONSTRAINT [DF_StockBaseInfo_ShrinkRatio]  DEFAULT ((0)) FOR [ShrinkProfit]
GO
ALTER TABLE [dbo].[GameStockInfo] ADD  CONSTRAINT [DF_StockBaseInfo_StockShrinkProfit]  DEFAULT (N'[{"ShrinkProfit":0,"ShrinkRatio":3}]') FOR [ShrinkOption]
GO
ALTER TABLE [dbo].[GameStockInfo] ADD  CONSTRAINT [DF_StockBaseInfo_RelationID]  DEFAULT (N'') FOR [RelationIDList]
GO
ALTER TABLE [dbo].[GameStockInfo] ADD  CONSTRAINT [DF_GameStockInfo_StateChangeID]  DEFAULT ((0)) FOR [StockControlID]
GO
ALTER TABLE [dbo].[GameStockInfo] ADD  CONSTRAINT [DF_StockBaseInfo_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[GameStockInfo] ADD  CONSTRAINT [DF_StockBaseInfo_Describe]  DEFAULT ('') FOR [Describe]
GO
ALTER TABLE [dbo].[GameStockInfo] ADD  CONSTRAINT [DF_GameStockInfo_FirstStock]  DEFAULT ((0)) FOR [FirstStock]
GO
ALTER TABLE [dbo].[GameStockInfo] ADD  CONSTRAINT [DF_GameStockInfo_FirstGold]  DEFAULT ((0)) FOR [FirstGold]
GO
ALTER TABLE [dbo].[GameStockInfo] ADD  CONSTRAINT [DF_GameStockInfo_AllAddSubtract]  DEFAULT ((0)) FOR [AllAddSubtractStock]
GO
ALTER TABLE [dbo].[GameStockInfo] ADD  CONSTRAINT [DF_GameStockInfo_AllAddSubtractGold]  DEFAULT ((0)) FOR [AllAddSubtractGold]
GO
ALTER TABLE [dbo].[GameTypeItem] ADD  CONSTRAINT [DF_GameTypeItem_JoinID]  DEFAULT ((0)) FOR [JoinID]
GO
ALTER TABLE [dbo].[GameTypeItem] ADD  CONSTRAINT [DF_GameTypeItem_SortID]  DEFAULT ((1000)) FOR [SortID]
GO
ALTER TABLE [dbo].[GameTypeItem] ADD  CONSTRAINT [DF_GameTypeItem_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[GameUserLevel] ADD  CONSTRAINT [DF_GameUserLevel_SpanPoint]  DEFAULT ((0)) FOR [SpanPoint]
GO
ALTER TABLE [dbo].[GameUserLevel] ADD  CONSTRAINT [DF_MemberOrderConfig_OrderName]  DEFAULT (N'') FOR [LevelName]
GO
ALTER TABLE [dbo].[GameUserLevel] ADD  CONSTRAINT [DF_GameUserLevel_Rewards]  DEFAULT (N'') FOR [Rewards]
GO
ALTER TABLE [dbo].[GameVersionInfo] ADD  CONSTRAINT [DF_GameVersionInfo_ClientVersion]  DEFAULT ((0)) FOR [Version]
GO
ALTER TABLE [dbo].[GameVersionInfo] ADD  CONSTRAINT [DF_GameVersionInfo_IsSpecialVersion]  DEFAULT ((0)) FOR [IsCurrent]
GO
ALTER TABLE [dbo].[GameVersionInfo] ADD  CONSTRAINT [DF_GameVersionInfo_Remarks]  DEFAULT (N'') FOR [Remarks]
GO
ALTER TABLE [dbo].[OnLineServerInfo] ADD  CONSTRAINT [DF_OnLineServerInfo_InsertDateTime]  DEFAULT (getdate()) FOR [InsertDateTime]
GO
ALTER TABLE [dbo].[OnLineServerInfo] ADD  CONSTRAINT [DF_OnLineServerInfo_OnLineCountInfo]  DEFAULT (N'') FOR [OnLineCountKind]
GO
ALTER TABLE [dbo].[OnLineStreamInfo] ADD  CONSTRAINT [DF_OnLineStreamInfo_MarketID]  DEFAULT ((0)) FOR [MarketID]
GO
ALTER TABLE [dbo].[OnLineStreamInfo] ADD  CONSTRAINT [DF_OnLineStreamInfo_OnLineGoldUserCount]  DEFAULT ((0)) FOR [OnLineGoldServerCount]
GO
ALTER TABLE [dbo].[OnLineStreamInfo] ADD  CONSTRAINT [DF_OnLineStreamInfo_OnLineGoldServerCount1_1]  DEFAULT ((0)) FOR [OnLineMatchServerCount]
GO
ALTER TABLE [dbo].[OnLineStreamInfo] ADD  CONSTRAINT [DF_OnLineStreamInfo_OnLineGoldServerCount1]  DEFAULT ((0)) FOR [OnLineBattleServerCount]
GO
ALTER TABLE [dbo].[OnLineStreamInfo] ADD  CONSTRAINT [DF_OnLineStreamInfo_OnLineBattleServerCount1]  DEFAULT ((0)) FOR [OnLineRankingServerCount]
GO
ALTER TABLE [dbo].[OnLineStreamInfo] ADD  CONSTRAINT [DF_OnLineStreamInfo_CountDate]  DEFAULT (getdate()) FOR [CountDateTime]
GO
ALTER TABLE [dbo].[PlatformConfig] ADD  CONSTRAINT [DF_ThirdPlatform_PlatformID]  DEFAULT ((0)) FOR [PlatformID]
GO
ALTER TABLE [dbo].[ResourceVersion] ADD  CONSTRAINT [DF_ResourceVersion_ResourceID]  DEFAULT ((0)) FOR [ResourceID]
GO
ALTER TABLE [dbo].[ResourceVersion] ADD  CONSTRAINT [DF_ResourceVersion_Version]  DEFAULT ((1)) FOR [Version]
GO
ALTER TABLE [dbo].[SystemStatusInfo] ADD  CONSTRAINT [DF_SystemStatusInfo_StatusValue]  DEFAULT ((0)) FOR [StatusValue]
GO
ALTER TABLE [dbo].[SystemStatusInfo] ADD  CONSTRAINT [DF_SystemStatusInfo_StatusString]  DEFAULT ('') FOR [StatusString]
GO
ALTER TABLE [dbo].[TurntableAwardConfig] ADD  CONSTRAINT [DF_TurntableConfig_HitRate]  DEFAULT ((0)) FOR [ItemWeight]
GO
ALTER TABLE [dbo].[TurntableAwardConfig] ADD  CONSTRAINT [DF_TurntableConfig_DailyLimitPlaces]  DEFAULT ((0)) FOR [DailyLimitPlaces]
GO
ALTER TABLE [dbo].[UIReferrer] ADD  CONSTRAINT [DF_UIReferrer_Ip]  DEFAULT ('') FOR [Ip]
GO
ALTER TABLE [dbo].[UIReferrer] ADD  CONSTRAINT [DF_UIReferrer_MachineID]  DEFAULT ('') FOR [MachineID]
GO
ALTER TABLE [dbo].[UIReferrer] ADD  CONSTRAINT [DF_UIReferrer_Referrer]  DEFAULT ('') FOR [Referrer]
GO
ALTER TABLE [dbo].[UIReferrer] ADD  CONSTRAINT [DF_UIReferrer_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[UIShieldMachine] ADD  CONSTRAINT [DF_UIShieldMachine_ShieldMask]  DEFAULT ((0)) FOR [ShieldMask]
GO
ALTER TABLE [dbo].[UIShieldMachine] ADD  CONSTRAINT [DF_UIShieldMachine_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[UIWhiteListAddress] ADD  CONSTRAINT [DF_UIWhiteListAddress_ExpireTime]  DEFAULT (getdate()) FOR [ExpireTime]
GO
ALTER TABLE [dbo].[UIWhiteListMachine] ADD  CONSTRAINT [DF_UIWhiteListMachine_WhiteListType]  DEFAULT ((0)) FOR [WhiteListType]
GO
ALTER TABLE [dbo].[UIWhiteListMachine] ADD  CONSTRAINT [DF_UIWhiteListMachine_Ip]  DEFAULT ('') FOR [Ip]
GO
ALTER TABLE [dbo].[UIWhiteListMachine] ADD  CONSTRAINT [DF_UIWhiteListMachine_Referrer]  DEFAULT ('') FOR [Referrer]
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'站点标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AllowAddress', @level2type=N'COLUMN',@level2name=N'StationID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'IP地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AllowAddress', @level2type=N'COLUMN',@level2name=N'IPAddress'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁止标志' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AllowAddress', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'过期时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AllowAddress', @level2type=N'COLUMN',@level2name=N'InvalidDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收集日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AllowAddress', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AllowAddress', @level2type=N'COLUMN',@level2name=N'CollectNote'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AllowMachine', @level2type=N'COLUMN',@level2name=N'MachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁止标志' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AllowMachine', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'过期时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AllowMachine', @level2type=N'COLUMN',@level2name=N'InvalidDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收集日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AllowMachine', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AllowMachine', @level2type=N'COLUMN',@level2name=N'CollectNote'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数据库索引' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'DataBaseInfo', @level2type=N'COLUMN',@level2name=N'DBInfoID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数据库IP地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'DataBaseInfo', @level2type=N'COLUMN',@level2name=N'DBAddr'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数据库端口' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'DataBaseInfo', @level2type=N'COLUMN',@level2name=N'DBPort'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数据库用户' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'DataBaseInfo', @level2type=N'COLUMN',@level2name=N'DBUser'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数据库密码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'DataBaseInfo', @level2type=N'COLUMN',@level2name=N'DBPassword'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'DataBaseInfo', @level2type=N'COLUMN',@level2name=N'MachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'DataBaseInfo', @level2type=N'COLUMN',@level2name=N'MachineName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'DataBaseInfo', @level2type=N'COLUMN',@level2name=N'Information'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数据库列表标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'DataBaseList', @level2type=N'COLUMN',@level2name=N'DBListID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数据库信息索引' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'DataBaseList', @level2type=N'COLUMN',@level2name=N'DBInfoID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数据库库名' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'DataBaseList', @level2type=N'COLUMN',@level2name=N'DBName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数据类型(0:主数据库,1:游戏数据库)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'DataBaseList', @level2type=N'COLUMN',@level2name=N'DBGenre'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数据库别名' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'DataBaseList', @level2type=N'COLUMN',@level2name=N'DBAliasName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'DataBaseList', @level2type=N'COLUMN',@level2name=N'Information'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'IP地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ExceptAddress', @level2type=N'COLUMN',@level2name=N'IPAddress'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'可用标识（0、可用 1、不可用）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ExceptAddress', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'地址描述' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ExceptAddress', @level2type=N'COLUMN',@level2name=N'Describe'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'更新时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ExceptAddress', @level2type=N'COLUMN',@level2name=N'CollectTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ExceptMachine', @level2type=N'COLUMN',@level2name=N'MachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁用标识（0、可用 1、不可用）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ExceptMachine', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'地址描述' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ExceptMachine', @level2type=N'COLUMN',@level2name=N'Describe'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ExceptMachine', @level2type=N'COLUMN',@level2name=N'CollectTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'配置名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameConfigItem', @level2type=N'COLUMN',@level2name=N'ConfigName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'配置掩码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameConfigItem', @level2type=N'COLUMN',@level2name=N'ConfigMask'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'服务掩码（0x0002、游戏服务  0x0004、登录服务）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameConfigItem', @level2type=N'COLUMN',@level2name=N'ServiceMask'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameConfigWeight', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型标识（0、常规类型  1、白名单类型 2、黑名单类型）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameConfigWeight', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最小倍数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameConfigWeight', @level2type=N'COLUMN',@level2name=N'MinTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最大倍数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameConfigWeight', @level2type=N'COLUMN',@level2name=N'MaxTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'倍数标记（0、输分倍数  1、赢分倍数）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameConfigWeight', @level2type=N'COLUMN',@level2name=N'TimesKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'权重值' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameConfigWeight', @level2type=N'COLUMN',@level2name=N'WeightValue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'等级标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameExpLevel', @level2type=N'COLUMN',@level2name=N'LevelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'需要达到的点数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameExpLevel', @level2type=N'COLUMN',@level2name=N'NeedPoint'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'当前级别跨越的点数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameExpLevel', @level2type=N'COLUMN',@level2name=N'SpanPoint'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'奖励配置' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameExpLevel', @level2type=N'COLUMN',@level2name=N'Rewards'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'礼物标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGiftInfo', @level2type=N'COLUMN',@level2name=N'GiftID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'礼物名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGiftInfo', @level2type=N'COLUMN',@level2name=N'GiftName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGiftInfo', @level2type=N'COLUMN',@level2name=N'GoodsID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'价格' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGiftInfo', @level2type=N'COLUMN',@level2name=N'Price'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'货币ID 0：钻石 1：金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGiftInfo', @level2type=N'COLUMN',@level2name=N'MoneyID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送魅力值' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGiftInfo', @level2type=N'COLUMN',@level2name=N'LoveLiness'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'销售总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGiftInfo', @level2type=N'COLUMN',@level2name=N'SoldCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排序标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGiftInfo', @level2type=N'COLUMN',@level2name=N'SortID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送类型(0:全局赠送 1:私人赠送)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGiftInfo', @level2type=N'COLUMN',@level2name=N'SendType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁止服务' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGiftInfo', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'描述信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGiftInfo', @level2type=N'COLUMN',@level2name=N'Describe'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修改时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGiftInfo', @level2type=N'COLUMN',@level2name=N'ModifyDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGiftInfo', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGoodsInfo', @level2type=N'COLUMN',@level2name=N'GoodsID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排序标识（客户端用）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGoodsInfo', @level2type=N'COLUMN',@level2name=N'OrderID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'图片标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGoodsInfo', @level2type=N'COLUMN',@level2name=N'ImageID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'是否具有有效期（0： 永久拥有 1：具有有效期）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGoodsInfo', @level2type=N'COLUMN',@level2name=N'HasIndate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品类型(1: 钻石 2：金币 3：赛券  4：爱心   10 ：游戏道具  11： 比赛黄金消耗抵消道具  12： 会员点数  13：会员时长  14：角色  15： 头像框  16： 聊天框  17： 牌背  18： 桌子背景 ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGoodsInfo', @level2type=N'COLUMN',@level2name=N'GoodsKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGoodsInfo', @level2type=N'COLUMN',@level2name=N'GoodsName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'可交易类型（0x01： 可购买  0x02： 可回收  0x04: 可兑换 ）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGoodsInfo', @level2type=N'COLUMN',@level2name=N'CanTradeType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'可存放的地方（0x01:  背包  0x02:  商城 0x04:  兑换商城 0x08:  后台赠送 0x10:  邮件）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGoodsInfo', @level2type=N'COLUMN',@level2name=N'CanStorePlace'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品属性' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGoodsInfo', @level2type=N'COLUMN',@level2name=N'GoodsOption'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品描述' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameGoodsInfo', @level2type=N'COLUMN',@level2name=N'GoodsDescribe'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'额外配额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameJackpotInfo', @level2type=N'COLUMN',@level2name=N'ExtraScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'当前彩金' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameJackpotInfo', @level2type=N'COLUMN',@level2name=N'StockScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'虚拟彩金' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameJackpotInfo', @level2type=N'COLUMN',@level2name=N'VirtualScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'奖池抽水配置' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameJackpotInfo', @level2type=N'COLUMN',@level2name=N'ShrinkRatio'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'派彩概率' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameJackpotInfo', @level2type=N'COLUMN',@level2name=N'PayoutProb'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'个人下注派彩限制' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameJackpotInfo', @level2type=N'COLUMN',@level2name=N'PayoutLimit'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'派彩起点' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameJackpotInfo', @level2type=N'COLUMN',@level2name=N'PayoutOrigin'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'初始彩金' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameJackpotInfo', @level2type=N'COLUMN',@level2name=N'FirstGold'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameKindItem', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'种类标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameKindItem', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'挂接标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameKindItem', @level2type=N'COLUMN',@level2name=N'JoinID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排序标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameKindItem', @level2type=N'COLUMN',@level2name=N'SortID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'模块标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameKindItem', @level2type=N'COLUMN',@level2name=N'ModuleID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameKindItem', @level2type=N'COLUMN',@level2name=N'KindName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'进程名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameKindItem', @level2type=N'COLUMN',@level2name=N'ProcessName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数据库列表索引' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameKindItem', @level2type=N'COLUMN',@level2name=N'DBListIndex'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'客户端支持掩码（0x01 为PC 0x02 为手机）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameKindItem', @level2type=N'COLUMN',@level2name=N'SupportMask'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'跳转地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameKindItem', @level2type=N'COLUMN',@level2name=N'GameRuleUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下载地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameKindItem', @level2type=N'COLUMN',@level2name=N'DownLoadUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'无效标志' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameKindItem', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'1-slot  2-poker  3-mahjong 4-Hundred 5-Casual ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameKindItem', @level2type=N'COLUMN',@level2name=N'KindClass'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameModuleInfo', @level2type=N'COLUMN',@level2name=N'ModuleID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数据索引' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameModuleInfo', @level2type=N'COLUMN',@level2name=N'DBListID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameModuleInfo', @level2type=N'COLUMN',@level2name=N'ModuleName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'支持类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameModuleInfo', @level2type=N'COLUMN',@level2name=N'SupportType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'服务器版本' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameModuleInfo', @level2type=N'COLUMN',@level2name=N'ServerVersion'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'客户端版本' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameModuleInfo', @level2type=N'COLUMN',@level2name=N'ClientVersion'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'客户端名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameModuleInfo', @level2type=N'COLUMN',@level2name=N'ClientExeName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'服务端名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameModuleInfo', @level2type=N'COLUMN',@level2name=N'ServerDLLName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'服务端名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameModuleInfo', @level2type=N'COLUMN',@level2name=N'AndroidDLLName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'节点标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameNodeItem', @level2type=N'COLUMN',@level2name=N'NodeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameNodeItem', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'挂接标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameNodeItem', @level2type=N'COLUMN',@level2name=N'JoinID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排序标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameNodeItem', @level2type=N'COLUMN',@level2name=N'SortID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'节点名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameNodeItem', @level2type=N'COLUMN',@level2name=N'NodeName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁用标志' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameNodeItem', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'页面标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GamePageItem', @level2type=N'COLUMN',@level2name=N'PageID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GamePageItem', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'节点索引' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GamePageItem', @level2type=N'COLUMN',@level2name=N'NodeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排序标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GamePageItem', @level2type=N'COLUMN',@level2name=N'SortID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GamePageItem', @level2type=N'COLUMN',@level2name=N'OperateType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'显示名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GamePageItem', @level2type=N'COLUMN',@level2name=N'DisplayName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'跳转地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GamePageItem', @level2type=N'COLUMN',@level2name=N'ResponseUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁用标志' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GamePageItem', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'等级标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameRankLevel', @level2type=N'COLUMN',@level2name=N'LevelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'起始分数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameRankLevel', @level2type=N'COLUMN',@level2name=N'NeedScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'积分跨度' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameRankLevel', @level2type=N'COLUMN',@level2name=N'SpanScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'段位' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameRankLevel', @level2type=N'COLUMN',@level2name=N'MainOrder'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'级别' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameRankLevel', @level2type=N'COLUMN',@level2name=N'SubOrder'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'星级' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameRankLevel', @level2type=N'COLUMN',@level2name=N'StarOrder'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'等级名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameRankLevel', @level2type=N'COLUMN',@level2name=N'LevelName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'ServerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'ServerName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'挂接类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'挂接节点' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'NodeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排列标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'SortID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'显示掩码（PC 手机 H5）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'VisibleMask'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'模块标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'ModuleID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'桌子数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'TableCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'ServerType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'服务端口' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'ServerPort'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间等级' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'ServerLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'MatchType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'单位积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'CellScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'服务费' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'ServiceScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'经验值增益比例（百分比）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'ExpGainRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数据库列表索引' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'DBListID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'限制积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'RestrictScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最低积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'MinTableScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最小进入分数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'MinEnterScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最大进入分数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'MaxEnterScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最小进入会员等级' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'MinEnterMember'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最大进入会员等级' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'MaxEnterMember'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'分组模式' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'DistributeMode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'分组规则' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'DistributeRule'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最少玩家' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'MinPlayerCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最小分组人数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'MinDistributeUser'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最小游戏人数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'MinPartakeGameUser'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最大游戏人数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'MaxPartakeGameUser'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'分组间隔' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'DistributeTimeSpace'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最大数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'MaxPlayer'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间规则' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'ServerRule'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'服务定制规则' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'CustomRule'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'附加用户权限' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'AttachUserRight'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'服务地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'ServiceAddr'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间服务域名' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'ServiceDomain'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'运行机器' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'ServiceMachine'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最少开始玩家数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'LessStartPlayer'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁用标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注说明' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'ServerNote'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'CreateDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修改时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'ModifyDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'约战规则' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameServerInfo', @level2type=N'COLUMN',@level2name=N'BattleRule'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameSiftItem', @level2type=N'COLUMN',@level2name=N'ServerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排序标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameSiftItem', @level2type=N'COLUMN',@level2name=N'SortID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameSiftItem', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标节点' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameSiftItem', @level2type=N'COLUMN',@level2name=N'NodeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'过滤掩码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameSiftItem', @level2type=N'COLUMN',@level2name=N'SiftMask'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameSiftItem', @level2type=N'COLUMN',@level2name=N'ServerName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁用标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameSiftItem', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'库存标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'StockID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下注金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'BetAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'库存类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'StockKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'库存级别（1、下注金额库存  2、游戏库存  3、机组库存   4、活动库存）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'StockLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'库存名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'StockName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'库存额外配额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'ExtraScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'当前库存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'StockScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'库存状态（0、未启用状态  1、启用状态 2、锁定状态）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'StockState'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'启用方式（0、自动启用  1、时间控制）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'EnableMode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'启用结束时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'EnableEndTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'启用开始时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'EnableStartTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'抽水利润' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'ShrinkProfit'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'抽水配置' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'ShrinkOption'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'关联库存ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'RelationIDList'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'库存控制标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'StockControlID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁用标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'说明信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'Describe'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'初始库存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'FirstStock'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'初始彩金' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'FirstGold'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'累计加减库存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameStockInfo', @level2type=N'COLUMN',@level2name=N'AllAddSubtractStock'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameTypeItem', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'挂接标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameTypeItem', @level2type=N'COLUMN',@level2name=N'JoinID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排序标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameTypeItem', @level2type=N'COLUMN',@level2name=N'SortID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameTypeItem', @level2type=N'COLUMN',@level2name=N'TypeName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'无效标志' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameTypeItem', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameTypeItem'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'等级标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameUserLevel', @level2type=N'COLUMN',@level2name=N'LevelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'需要达到的点数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameUserLevel', @level2type=N'COLUMN',@level2name=N'NeedPoint'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'当前级别跨越的点数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameUserLevel', @level2type=N'COLUMN',@level2name=N'SpanPoint'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'等级名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameUserLevel', @level2type=N'COLUMN',@level2name=N'LevelName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'奖励配置' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameUserLevel', @level2type=N'COLUMN',@level2name=N'Rewards'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'模块标识（0表示为平台广场）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameVersionInfo', @level2type=N'COLUMN',@level2name=N'ModuleID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'设备类型（0x00：电脑端  0x10:安卓端  0x40: 苹果端）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameVersionInfo', @level2type=N'COLUMN',@level2name=N'DeviceKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'终端版本' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameVersionInfo', @level2type=N'COLUMN',@level2name=N'Version'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'是否当前版本（0 ：否  1：是）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameVersionInfo', @level2type=N'COLUMN',@level2name=N'IsCurrent'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注说明' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameVersionInfo', @level2type=N'COLUMN',@level2name=N'Remarks'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'OnLineServerInfo', @level2type=N'COLUMN',@level2name=N'ID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'OnLineServerInfo', @level2type=N'COLUMN',@level2name=N'MachineSerial'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'OnLineServerInfo', @level2type=N'COLUMN',@level2name=N'MachineServer'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'插入时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'OnLineServerInfo', @level2type=N'COLUMN',@level2name=N'InsertDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总在线数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'OnLineServerInfo', @level2type=N'COLUMN',@level2name=N'OnLineCountSum'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'在线信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'OnLineServerInfo', @level2type=N'COLUMN',@level2name=N'OnLineCountKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'OnLineStreamInfo', @level2type=N'COLUMN',@level2name=N'ID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'在线用户总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'OnLineStreamInfo', @level2type=N'COLUMN',@level2name=N'OnLineUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'大厅在线用户总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'OnLineStreamInfo', @level2type=N'COLUMN',@level2name=N'OnLinePlazaCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间在线用户总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'OnLineStreamInfo', @level2type=N'COLUMN',@level2name=N'OnLineServerCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'金币房间在线总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'OnLineStreamInfo', @level2type=N'COLUMN',@level2name=N'OnLineGoldServerCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛房间在线用户总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'OnLineStreamInfo', @level2type=N'COLUMN',@level2name=N'OnLineMatchServerCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'约战用户在线总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'OnLineStreamInfo', @level2type=N'COLUMN',@level2name=N'OnLineBattleServerCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排位用户在线总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'OnLineStreamInfo', @level2type=N'COLUMN',@level2name=N'OnLineRankingServerCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'统计时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'OnLineStreamInfo', @level2type=N'COLUMN',@level2name=N'CountDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'平台标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PlatformConfig', @level2type=N'COLUMN',@level2name=N'PlatformID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'平台名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PlatformConfig', @level2type=N'COLUMN',@level2name=N'PlatformName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'资源标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ResourceVersion', @level2type=N'COLUMN',@level2name=N'ResourceID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'版本号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ResourceVersion', @level2type=N'COLUMN',@level2name=N'Version'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'描述信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ResourceVersion', @level2type=N'COLUMN',@level2name=N'Description'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'配置名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemOptionInfo', @level2type=N'COLUMN',@level2name=N'OptionName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'配置值' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemOptionInfo', @level2type=N'COLUMN',@level2name=N'OptionValue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'配置状态' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemOptionInfo', @level2type=N'COLUMN',@level2name=N'OptionState'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'配置解释' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemOptionInfo', @level2type=N'COLUMN',@level2name=N'OptionDescribe'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'配置类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStatusInfo', @level2type=N'COLUMN',@level2name=N'StatusKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'状态名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStatusInfo', @level2type=N'COLUMN',@level2name=N'StatusName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'状态数值' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStatusInfo', @level2type=N'COLUMN',@level2name=N'StatusValue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'状态字符' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStatusInfo', @level2type=N'COLUMN',@level2name=N'StatusString'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'区域标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'TurntableAwardConfig', @level2type=N'COLUMN',@level2name=N'ItemIndex'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'奖品名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'TurntableAwardConfig', @level2type=N'COLUMN',@level2name=N'AwardName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'TurntableAwardConfig', @level2type=N'COLUMN',@level2name=N'GoodsID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'TurntableAwardConfig', @level2type=N'COLUMN',@level2name=N'GoodsCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品等级' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'TurntableAwardConfig', @level2type=N'COLUMN',@level2name=N'GoodsLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品有效期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'TurntableAwardConfig', @level2type=N'COLUMN',@level2name=N'GoodsIndate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'每日限定名额（0、 不限）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'TurntableAwardConfig', @level2type=N'COLUMN',@level2name=N'DailyLimitPlaces'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UIReferrer', @level2type=N'COLUMN',@level2name=N'MachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推荐参数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UIReferrer', @level2type=N'COLUMN',@level2name=N'Referrer'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UIReferrer', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UIShieldAccount', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'开始IP地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UIShieldAddress', @level2type=N'COLUMN',@level2name=N'Start_IPAddress'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'结束IP地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UIShieldAddress', @level2type=N'COLUMN',@level2name=N'Finish_IPAddress'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'开始IP地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UIShieldAddress', @level2type=N'COLUMN',@level2name=N'Start_IPAddress_String'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'结束IP地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UIShieldAddress', @level2type=N'COLUMN',@level2name=N'Finish_IPAddress_String'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'显示掩码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UIShieldAddress', @level2type=N'COLUMN',@level2name=N'ShieldMask'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UIShieldAddress', @level2type=N'COLUMN',@level2name=N'Describe'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁用标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UIShieldAddress', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UIShieldMachine', @level2type=N'COLUMN',@level2name=N'MachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'显示掩码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UIShieldMachine', @level2type=N'COLUMN',@level2name=N'ShieldMask'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UIShieldMachine', @level2type=N'COLUMN',@level2name=N'Describe'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁用标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UIShieldMachine', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'过期时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UIWhiteListAddress', @level2type=N'COLUMN',@level2name=N'ExpireTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UIWhiteListMachine', @level2type=N'COLUMN',@level2name=N'MachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'白名单类型 0：IP 1：根据来源参数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UIWhiteListMachine', @level2type=N'COLUMN',@level2name=N'WhiteListType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'IP参数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UIWhiteListMachine', @level2type=N'COLUMN',@level2name=N'Ip'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'来源参数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UIWhiteListMachine', @level2type=N'COLUMN',@level2name=N'Referrer'
GO
