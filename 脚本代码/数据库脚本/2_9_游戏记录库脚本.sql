USE [LHGameRecordDB]
GO
/****** Object:  Table [dbo].[AccountsLossReport]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsLossReport](
	[ReportID] [int] IDENTITY(1,1) NOT NULL,
	[ReportNo] [nvarchar](32) NOT NULL,
	[ReportedStationID] [int] NOT NULL,
	[ReportedType] [int] NOT NULL,
	[UserStationID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[GameID] [int] NOT NULL,
	[Accounts] [nvarchar](31) NOT NULL,
	[UserName] [nvarchar](31) NOT NULL,
	[QQMsn] [nvarchar](63) NOT NULL,
	[LinkPhone] [nvarchar](16) NOT NULL,
	[LinkEmail] [nvarchar](60) NOT NULL,
	[OldAccounts1] [nvarchar](31) NOT NULL,
	[OldAccounts2] [nvarchar](31) NOT NULL,
	[OldPass1] [nchar](32) NOT NULL,
	[OldPass2] [nchar](32) NOT NULL,
	[PassportType] [tinyint] NOT NULL,
	[PassportID] [nvarchar](32) NOT NULL,
	[OldQuestion] [nvarchar](32) NOT NULL,
	[OldResponse] [nvarchar](32) NOT NULL,
	[OldEmail] [nvarchar](63) NOT NULL,
	[SuppInfo] [nvarchar](512) NOT NULL,
	[ProcessStatus] [tinyint] NOT NULL,
	[SendCount] [int] NOT NULL,
	[SolveDate] [datetime] NOT NULL,
	[OverDate] [datetime] NOT NULL,
	[ReportIP] [nvarchar](50) NOT NULL,
	[ReportDate] [datetime] NOT NULL,
 CONSTRAINT [PK_AccountsLossReport] PRIMARY KEY CLUSTERED 
(
	[ReportID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[CacheChartsBattleCount]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[CacheChartsBattleCount](
	[DateID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[ChartID] [int] NOT NULL,
	[GameID] [int] NOT NULL,
	[Accounts] [nvarchar](31) NOT NULL,
	[NickName] [nvarchar](31) NOT NULL,
	[FaceID] [int] NOT NULL,
	[CustomID] [int] NOT NULL,
	[Gender] [int] NOT NULL,
	[BattleCount] [int] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_CacheChartsBattleCount] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[KindID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[CacheChartsCommission]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[CacheChartsCommission](
	[DateID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[ChartID] [int] NOT NULL,
	[GameID] [int] NOT NULL,
	[Accounts] [nvarchar](32) NOT NULL,
	[Nickname] [nvarchar](32) NOT NULL,
	[FaceID] [int] NOT NULL,
	[CustomID] [int] NOT NULL,
	[Gender] [int] NOT NULL,
	[FirstPay] [bigint] NOT NULL,
	[TotalBet] [bigint] NOT NULL,
	[TotalCommission] [bigint] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_CacheChartsCommission_1] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[CacheChartsMedal]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[CacheChartsMedal](
	[ChartID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[GameID] [int] NOT NULL,
	[Accounts] [nvarchar](31) NOT NULL,
	[NickName] [nvarchar](31) NOT NULL,
	[FaceID] [int] NOT NULL,
	[CustomID] [int] NOT NULL,
	[Gender] [int] NOT NULL,
	[UserMedal] [int] NOT NULL,
	[CollectDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RankingListRange]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RankingListRange](
	[TypeID] [tinyint] NOT NULL,
	[DateID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[GameID] [int] NOT NULL,
	[NickName] [nvarchar](32) NOT NULL,
	[Gender] [tinyint] NOT NULL,
	[FaceID] [smallint] NOT NULL,
	[CustomID] [int] NOT NULL,
	[ActorID] [smallint] NOT NULL,
	[FaceFrameID] [smallint] NOT NULL,
	[VipPoint] [int] NOT NULL,
	[RankID] [int] NOT NULL,
	[Score] [bigint] NOT NULL,
 CONSTRAINT [PK_RankingListRange] PRIMARY KEY CLUSTERED 
(
	[TypeID] ASC,
	[DateID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RankingListVersion]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RankingListVersion](
	[TypeID] [int] NOT NULL,
	[ModifyDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_RankingListVersion] PRIMARY KEY CLUSTERED 
(
	[TypeID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordAccountsExpend]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordAccountsExpend](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[ReAccounts] [nvarchar](31) NOT NULL,
	[ClientIP] [nvarchar](15) NOT NULL,
	[Cost] [int] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordAccountsExpend] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordBindMachine]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordBindMachine](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[MachineSerial] [nvarchar](32) NOT NULL,
	[MoorMachine] [tinyint] NOT NULL,
	[IsGamePlaza] [tinyint] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordBindMachine] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordBuyQrcode]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordBuyQrcode](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[BuyType] [tinyint] NOT NULL,
	[BuyPrice] [decimal](18, 2) NOT NULL,
	[QrTicket] [nvarchar](300) NOT NULL,
	[ClientIP] [nvarchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordBuyQrcode] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordChangeGold]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordChangeGold](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[GameID] [int] NOT NULL,
	[TypeID] [tinyint] NOT NULL,
	[BeforeGold] [bigint] NOT NULL,
	[ChangeGold] [bigint] NOT NULL,
	[CollectClientIP] [nvarchar](15) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordChangeGold] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordChangeGoods]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordChangeGoods](
	[RecordID] [bigint] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[GoodsID] [int] NOT NULL,
	[GoodsKind] [tinyint] NOT NULL,
	[TypeID] [tinyint] NOT NULL,
	[RelationID] [int] NOT NULL,
	[BeforeCount] [int] NOT NULL,
	[ChangeCount] [int] NOT NULL,
	[CollectClientIP] [nvarchar](15) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
	[CollectNote] [nvarchar](200) NOT NULL,
 CONSTRAINT [PK_RecordChangeGoods] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordChangeIngot]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordChangeIngot](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[GameID] [int] NOT NULL,
	[TypeID] [tinyint] NOT NULL,
	[BeforeIngot] [bigint] NOT NULL,
	[ChangeIngot] [int] NOT NULL,
	[CollectClientIP] [nvarchar](15) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordChangeIngot] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordChangeLoveHeart]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordChangeLoveHeart](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[GameID] [int] NOT NULL,
	[TypeID] [tinyint] NOT NULL,
	[BeforeLoveHeart] [bigint] NOT NULL,
	[ChangeLoveHeart] [bigint] NOT NULL,
	[CollectClientIP] [nvarchar](15) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordChangeLoveHeart] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordCheckIn]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordCheckIn](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[PresentScore] [int] NOT NULL,
	[LxCount] [int] NOT NULL,
	[LxScore] [int] NOT NULL,
	[MachineID] [nvarchar](32) NOT NULL,
	[IPAddress] [nvarchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordCheckIn] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordClearFlee]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordClearFlee](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[CurFlee] [int] NOT NULL,
	[ClearType] [tinyint] NOT NULL,
	[ClientIP] [varchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordClearFlee] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordClearScore]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordClearScore](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[CurScore] [bigint] NOT NULL,
	[ClearType] [tinyint] NOT NULL,
	[ClientIP] [varchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordClearScore] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordDeductGold]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordDeductGold](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[CurGold] [bigint] NOT NULL,
	[DeductGold] [bigint] NOT NULL,
	[MasterID] [int] NOT NULL,
	[Reason] [nvarchar](120) NOT NULL,
	[CollectClientIP] [nvarchar](15) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordDeductGold] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordDrawCommission]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordDrawCommission](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[NickName] [nvarchar](32) NOT NULL,
	[ServiceFee] [int] NOT NULL,
	[DrawAmount] [bigint] NOT NULL,
	[BeforeCommission] [bigint] NOT NULL,
	[CollectClientIP] [nvarchar](15) NOT NULL,
	[CollectMachineID] [nvarchar](32) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordChangeCommision] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordFeedback]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordFeedback](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[FeedbackTitle] [nvarchar](200) NOT NULL,
	[FeedbackContent] [nvarchar](2000) NOT NULL,
	[FeedbackDate] [datetime] NOT NULL,
	[FeedbackIP] [nvarchar](15) NOT NULL,
	[ViewCount] [int] NOT NULL,
	[RevertStatus] [tinyint] NOT NULL,
	[RevertAccounts] [nvarchar](32) NULL,
	[RevertContent] [nvarchar](2000) NULL,
	[RevertDate] [datetime] NULL,
 CONSTRAINT [PK_RecordFeedback] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordGameExpend]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordGameExpend](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[ClearScore] [int] NOT NULL,
	[ClearFlee] [int] NOT NULL,
	[ClientIP] [varchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordGameExpend] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordGrantClearScore]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordGrantClearScore](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[MasterID] [int] NOT NULL,
	[ClientIP] [varchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[UserID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[CurScore] [bigint] NOT NULL,
	[Reason] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_RecordGrantClearScore] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordGrantExperience]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordGrantExperience](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[MasterID] [int] NOT NULL,
	[ClientIP] [varchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[UserID] [int] NOT NULL,
	[CurExperience] [int] NOT NULL,
	[AddExperience] [int] NOT NULL,
	[Reason] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_RecordGrantExperience] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordGrantFlee]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordGrantFlee](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[MasterID] [int] NOT NULL,
	[ClientIP] [varchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[UserID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[CurFlee] [int] NOT NULL,
	[AddFlee] [int] NOT NULL,
	[Reason] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_RecordGrantFlee] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordGrantGameID]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordGrantGameID](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[GameID] [int] NOT NULL,
	[ReGameID] [int] NOT NULL,
	[IDLevel] [int] NOT NULL,
	[ClientIP] [varchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordGrantGameID] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordGrantGold]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordGrantGold](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[CurGold] [bigint] NOT NULL,
	[AddGold] [bigint] NOT NULL,
	[MasterID] [int] NOT NULL,
	[Reason] [nvarchar](120) NOT NULL,
	[CollectClientIP] [nvarchar](15) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordGrantGold] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordGrantGoods]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordGrantGoods](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[GoodsID] [bigint] NOT NULL,
	[CurGoodsCount] [bigint] NOT NULL,
	[AddGoodsCount] [int] NOT NULL,
	[MasterID] [int] NOT NULL,
	[Reason] [nvarchar](120) NOT NULL,
	[CollectClientIP] [nvarchar](15) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordGrantGoods] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordGrantIngot]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordGrantIngot](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[CurIngot] [bigint] NOT NULL,
	[AddIngot] [int] NOT NULL,
	[MasterID] [int] NOT NULL,
	[Reason] [nvarchar](120) NOT NULL,
	[CollectClientIP] [nvarchar](15) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordGrantIngot] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordGrantLoveHeart]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordGrantLoveHeart](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[CurLoveHeart] [bigint] NOT NULL,
	[AddLoveHeart] [bigint] NOT NULL,
	[MasterID] [int] NOT NULL,
	[Reason] [nvarchar](120) NOT NULL,
	[CollectClientIP] [nvarchar](15) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordGrantLoveHeart] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordGrantVip]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordGrantVip](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[CurVipOrder] [int] NOT NULL,
	[AfterVipOrder] [int] NOT NULL,
	[MasterID] [int] NOT NULL,
	[Reason] [nvarchar](120) NOT NULL,
	[CollectClientIP] [nvarchar](15) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordGrantVip] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordModifyPassword]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordModifyPassword](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[TypeID] [tinyint] NOT NULL,
	[OldPassword] [nvarchar](50) NOT NULL,
	[NewPassword] [nvarchar](50) NOT NULL,
	[ChannelID] [tinyint] NOT NULL,
	[MasterID] [int] NOT NULL,
	[CollectClientIP] [nvarchar](15) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordModifyPassword] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordSendPresent]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordSendPresent](
	[PresentID] [tinyint] NOT NULL,
	[RcvUserID] [int] NOT NULL,
	[SendUserID] [int] NOT NULL,
	[LovelinessRcv] [int] NOT NULL,
	[LovelinessSend] [int] NOT NULL,
	[PresentPrice] [int] NOT NULL,
	[PresentCount] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[ServerID] [int] NOT NULL,
	[SendTime] [datetime] NOT NULL,
	[ClientIP] [nvarchar](15) NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordServiceRemarks]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordServiceRemarks](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[Remarks] [nvarchar](512) NOT NULL,
	[CreateDate] [datetime] NOT NULL,
	[CreateBy] [int] NOT NULL,
	[UpdateCount] [int] NOT NULL,
	[UpdateDate] [datetime] NOT NULL,
	[UpdateBy] [int] NOT NULL,
 CONSTRAINT [PK_RecordServiceRemarks] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordSpreadRewards]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordSpreadRewards](
	[RecordID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[SubUserID] [int] NOT NULL,
	[Gold] [int] NULL,
	[Ingot] [int] NULL,
	[MachineSerial] [nvarchar](32) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = OFF, ALLOW_PAGE_LOCKS = OFF, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordStockAddReduce]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordStockAddReduce](
	[ID] [int] NOT NULL,
	[StockID] [int] NOT NULL,
	[BetAmount] [bigint] NOT NULL,
	[TypeID] [int] NOT NULL,
	[OperationAmount] [bigint] NOT NULL,
	[OperatorUserID] [int] NOT NULL,
	[Remark] [nvarchar](50) NOT NULL,
	[DateTime] [datetime] NULL,
 CONSTRAINT [PK_RecordStockAddReduce] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordStockControl]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordStockControl](
	[ID] [bigint] NOT NULL,
	[StockID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[BetAmount] [int] NOT NULL,
	[StockName] [nvarchar](50) NOT NULL,
	[StockLevel] [int] NOT NULL,
	[StartTime] [bigint] NOT NULL,
	[EndTime] [bigint] NOT NULL,
	[FirstStock] [bigint] NOT NULL,
	[StockScore] [bigint] NOT NULL,
	[ExtraScore] [bigint] NOT NULL,
	[FirstShrinkProfit] [bigint] NOT NULL,
	[ShrinkProfit] [bigint] NOT NULL,
	[ShrinkOption] [nvarchar](512) NOT NULL,
	[FirstGold] [bigint] NOT NULL,
	[GoldScore] [bigint] NOT NULL,
	[ShrinkRatio] [smallint] NOT NULL,
	[TotalAddReduceStock] [bigint] NOT NULL,
	[TotalAddReduceGold] [bigint] NOT NULL,
	[RelationIDList] [nvarchar](128) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
	[JackpotInfo] [nvarchar](1024) NOT NULL,
 CONSTRAINT [PK_RecordStockControl] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordStockModify]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordStockModify](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[StockID] [int] NOT NULL,
	[BeforeStock] [bigint] NOT NULL,
	[ChangeStock] [bigint] NOT NULL,
	[MasterID] [int] NOT NULL,
	[ClientIP] [nvarchar](15) NOT NULL,
	[Remark] [nvarchar](128) NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordStockModify] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordTurntableLottery]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordTurntableLottery](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[NickName] [nvarchar](32) NOT NULL,
	[HitIndex] [tinyint] NOT NULL,
	[AwardName] [nvarchar](64) NOT NULL,
	[GoodsID] [int] NOT NULL,
	[GoodsCount] [int] NOT NULL,
	[GoodsLevel] [smallint] NOT NULL,
	[GoodsIndate] [int] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordTurntableLottery] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordUnderwriteExpend]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordUnderwriteExpend](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[ReUnderwrite] [nvarchar](63) NOT NULL,
	[ClientIP] [nvarchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordUnderwriteExpend] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordUserGift]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordUserGift](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[TargetUserID] [int] NOT NULL,
	[GoodsID] [int] NOT NULL,
	[GoodsCount] [int] NOT NULL,
	[LoveLiness] [int] NOT NULL,
	[SendType] [tinyint] NOT NULL,
	[LoadStatus] [tinyint] NOT NULL,
	[CollectClientIP] [nvarchar](15) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK__RecordUs__FBDF78C9013E24C5] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = OFF, ALLOW_PAGE_LOCKS = OFF, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordUserManagement]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordUserManagement](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[MasterID] [int] NOT NULL,
	[MasterAccounts] [nvarchar](31) NOT NULL,
	[MasterIP] [varchar](15) NOT NULL,
	[MasterDate] [datetime] NOT NULL,
	[UserID] [int] NOT NULL,
	[GameID] [int] NOT NULL,
	[UserAccounts] [nvarchar](31) NOT NULL,
	[Action] [int] NOT NULL,
	[Reason] [nvarchar](256) NOT NULL,
 CONSTRAINT [PK_RecordUserManagement] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordUserNullity]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordUserNullity](
	[RecordID] [int] IDENTITY(100,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[Remark] [nvarchar](80) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordUserNullity] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordUserRedPack]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordUserRedPack](
	[CodeID] [int] NOT NULL,
	[Amount] [decimal](18, 2) NOT NULL,
	[StatusID] [tinyint] NOT NULL,
	[OverTime] [datetime] NOT NULL,
	[UserID] [int] NOT NULL,
	[RecevieDate] [datetime] NOT NULL,
	[RecevieIP] [nvarchar](15) NOT NULL,
 CONSTRAINT [PK_RecordUserRedPack_1] PRIMARY KEY CLUSTERED 
(
	[CodeID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordVideoInfo]    Script Date: 2023/7/12 11:10:14 ******/
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
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordVideoUser]    Script Date: 2023/7/12 11:10:14 ******/
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
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordWealthTransfer]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordWealthTransfer](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[GameID] [int] NOT NULL,
	[NickName] [nvarchar](32) NOT NULL,
	[TargetUserID] [int] NOT NULL,
	[TargetGameID] [int] NOT NULL,
	[TargetNickName] [nvarchar](32) NOT NULL,
	[ServiceFee] [bigint] NOT NULL,
	[TransferAmount] [bigint] NOT NULL,
	[CollectClientIP] [nvarchar](15) NOT NULL,
	[CollectMachineID] [nvarchar](32) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordWealthTransfer] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReportDay]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReportDay](
	[DateID] [int] NOT NULL,
	[UserCount] [int] NOT NULL,
	[PayUserCount] [int] NOT NULL,
	[PayTwiceUserCount] [int] NOT NULL,
	[TodayLoginUserCount] [int] NOT NULL,
	[TodayRegUserCount] [int] NOT NULL,
	[TodayNewVipCount] [int] NOT NULL,
	[TodayPayUserCount] [int] NOT NULL,
	[TodayPayMaxAmount] [decimal](18, 2) NOT NULL,
	[TodayDayActiveUser] [int] NOT NULL,
	[TodayPlayCommonUserCount] [int] NOT NULL,
	[TodayPlayVipUserCount] [int] NOT NULL,
	[TodayBattleCreateRoomCount] [int] NOT NULL,
	[TodaySumOnlineTime] [bigint] NOT NULL,
	[OneDayRemainRate] [decimal](18, 4) NOT NULL,
	[TwoDayRemainRate] [decimal](18, 4) NOT NULL,
	[ThreeDayRemainRate] [decimal](18, 4) NOT NULL,
	[FourDayRemainRate] [decimal](18, 4) NOT NULL,
	[FiveDayRemainRate] [decimal](18, 4) NOT NULL,
	[ServenDayRemainRate] [decimal](18, 4) NOT NULL,
	[FiftenDayRemainRate] [decimal](18, 4) NOT NULL,
	[ThirtyDayRemainRate] [decimal](18, 4) NOT NULL,
	[ReportDateTime] [datetime] NOT NULL,
	[TodayRegPayUserCount] [int] NOT NULL,
	[TotalPlayers] [int] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
	[TodaySpreadCommission] [decimal](18, 2) NOT NULL,
	[TodayGoldWithdrawAmount] [decimal](18, 2) NOT NULL,
	[TodayNewPayAmount] [decimal](18, 2) NOT NULL,
	[TodayNewPayUserCount] [int] NOT NULL,
	[TodayNewLoginUserCount] [int] NOT NULL,
	[TodayNewPayCount] [int] NOT NULL,
	[OldTodayPayUserCount] [int] NOT NULL,
	[TodayPayAmount] [decimal](18, 2) NOT NULL,
	[TodayGoldWithdrawSystemFee] [decimal](18, 2) NOT NULL,
	[TodayGoldWithdrawChannelFee] [decimal](18, 2) NOT NULL,
	[TodayGoldWithdrawUserCount] [int] NOT NULL,
 CONSTRAINT [PK_ReportDay] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReportDayCommission]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReportDayCommission](
	[DateID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[Commission] [bigint] NOT NULL,
	[TotalPayAmount] [bigint] NOT NULL,
	[TotalBetAmount] [bigint] NOT NULL,
	[TotalPayContribute] [bigint] NOT NULL,
	[TotalBetContribute] [bigint] NOT NULL,
	[LowerPayAmount] [bigint] NOT NULL,
	[LowerBetAmount] [bigint] NOT NULL,
	[LLowerPayAmount] [bigint] NOT NULL,
	[LLowerBetAmount] [bigint] NOT NULL,
	[LowerPayContribute] [bigint] NOT NULL,
	[LowerBetContribute] [bigint] NOT NULL,
	[LLowerPayContribute] [bigint] NOT NULL,
	[LLowerBetContribute] [bigint] NOT NULL,
 CONSTRAINT [PK_ReportDayCommission_1] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReportDayMarket]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReportDayMarket](
	[DateID] [int] NOT NULL,
	[MarketID] [int] NOT NULL,
	[SpreadChannelID] [int] NOT NULL,
	[UserCount] [int] NOT NULL,
	[PayUserCount] [int] NOT NULL,
	[PayTwiceUserCount] [int] NOT NULL,
	[TodayPayAmount] [decimal](18, 2) NOT NULL,
	[TodayPayCount] [int] NOT NULL,
	[TodayLoginUserCount] [int] NOT NULL,
	[TodayRegUserCount] [int] NOT NULL,
	[TodayNewVipCount] [int] NOT NULL,
	[TodayPayUserCount] [int] NOT NULL,
	[TodayPayMaxAmount] [decimal](18, 2) NOT NULL,
	[TodaySpreadCommission] [decimal](18, 2) NOT NULL,
	[TodayGoldWithdrawAmount] [decimal](18, 2) NOT NULL,
	[TodayGoldWithdrawSystemFee] [decimal](18, 2) NOT NULL,
	[TodayGoldWithdrawChannelFee] [decimal](18, 2) NOT NULL,
	[TodayGoldWithdrawUserCount] [int] NOT NULL,
	[TodayDayActiveUser] [int] NOT NULL,
	[TodayPlayCommonUserCount] [int] NOT NULL,
	[TodayPlayVipUserCount] [int] NOT NULL,
	[TodayBattleCreateRoomCount] [int] NOT NULL,
	[TodaySumOnlineTime] [bigint] NOT NULL,
	[TodayNewPayAmount] [decimal](18, 2) NOT NULL,
	[TodayNewPayUserCount] [int] NOT NULL,
	[TodayNewLoginUserCount] [int] NOT NULL,
	[TodayNewPayCount] [int] NOT NULL,
	[OneDayRemainRate] [decimal](18, 4) NOT NULL,
	[TwoDayRemainRate] [decimal](18, 4) NOT NULL,
	[ThreeDayRemainRate] [decimal](18, 4) NOT NULL,
	[FourDayRemainRate] [decimal](18, 4) NOT NULL,
	[FiveDayRemainRate] [decimal](18, 4) NOT NULL,
	[ServenDayRemainRate] [decimal](18, 4) NOT NULL,
	[FiftenDayRemainRate] [decimal](18, 4) NOT NULL,
	[ThirtyDayRemainRate] [decimal](18, 4) NOT NULL,
	[ReportDateTime] [datetime] NOT NULL,
	[TodayRegPayUserCount] [int] NOT NULL,
	[TotalPlayers] [int] NOT NULL,
	[OldTodayPayUserCount] [int] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
	[TodayAllPayAmount] [decimal](18, 2) NOT NULL,
	[TodayAllPayCount] [int] NOT NULL,
	[TodayAllPayUserCount] [int] NOT NULL,
	[TodayNewAllPayAmount] [decimal](18, 2) NOT NULL,
	[TodayNewAllPayUserCount] [int] NOT NULL,
	[TodayNewAllPayCount] [int] NOT NULL,
	[TodayRegMachineCount] [int] NOT NULL,
	[TodayRegPayMachineCount] [int] NOT NULL,
	[TodayLoginMachineCount] [int] NOT NULL,
	[TodayNewFirstPayAmount] [decimal](18, 2) NOT NULL,
	[TodayNewTwoPayCount] [int] NOT NULL,
	[TodayNewTwoPayAmount] [decimal](18, 2) NOT NULL,
 CONSTRAINT [PK_ReportDayMarket] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[MarketID] ASC,
	[SpreadChannelID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReportDayUser]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReportDayUser](
	[DateID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[PayCount] [int] NOT NULL,
	[PayAmount] [decimal](18, 2) NOT NULL,
	[LastPayAmount] [decimal](18, 2) NOT NULL,
	[LastPayDate] [datetime] NOT NULL,
	[Experience] [int] NOT NULL,
	[Loveliness] [int] NOT NULL,
	[SpreadViewCount] [int] NOT NULL,
	[ReportDateTime] [datetime] NOT NULL,
	[SpreadLowerCount] [int] NOT NULL,
	[SpreadLLowerCount] [int] NOT NULL,
 CONSTRAINT [PK_ReportDayUser] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReportGameData]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReportGameData](
	[DateID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[GameNumber] [int] NOT NULL,
	[UserService] [bigint] NOT NULL,
	[UserRevenue] [bigint] NOT NULL,
	[PlayTimeCount] [int] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
	[PlayUserCount] [int] NOT NULL,
	[UserBetAmount] [bigint] NOT NULL,
 CONSTRAINT [PK_ReportGameData] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[KindID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReportGoods]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReportGoods](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[DateID] [int] NOT NULL,
	[GoodsID] [int] NOT NULL,
	[GoodsCount] [bigint] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_ReportGoods_1] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReportGoodsOpCn]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReportGoodsOpCn](
	[DateID] [int] NOT NULL,
	[GoodsID] [int] NOT NULL,
	[ChangeTypeID] [int] NOT NULL,
	[OutputCount] [bigint] NOT NULL,
	[ConsumeCount] [bigint] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_ReportOpCnGoods] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[GoodsID] ASC,
	[ChangeTypeID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReportStock]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReportStock](
	[DateID] [int] NOT NULL,
	[StockID] [int] NOT NULL,
	[StockStart] [bigint] NOT NULL,
	[StockChange] [bigint] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_ReportStock] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[StockID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReportTerminal]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReportTerminal](
	[DateID] [int] NOT NULL,
	[TerminalID] [int] NOT NULL,
	[UserCount] [int] NOT NULL,
 CONSTRAINT [PK_ReportDayTerminal] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[TerminalID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReportUserLTV]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReportUserLTV](
	[DateID] [int] NOT NULL,
	[MarketID] [int] NOT NULL,
	[SpreadChannelID] [int] NOT NULL,
	[UserCount] [int] NOT NULL,
	[LTVOne] [decimal](18, 2) NOT NULL,
	[LTVTwo] [decimal](18, 2) NOT NULL,
	[LTVThree] [decimal](18, 2) NOT NULL,
	[LTVFour] [decimal](18, 2) NOT NULL,
	[LTVFive] [decimal](18, 2) NOT NULL,
	[LTVSix] [decimal](18, 2) NOT NULL,
	[LTVSeven] [decimal](18, 2) NOT NULL,
	[LTVEight] [decimal](18, 2) NOT NULL,
	[LTVNine] [decimal](18, 2) NOT NULL,
	[LTVTen] [decimal](18, 2) NOT NULL,
	[LTVEleven] [decimal](18, 2) NOT NULL,
	[LTVTwelve] [decimal](18, 2) NOT NULL,
	[LTVThirteen] [decimal](18, 2) NOT NULL,
	[LTVFourteen] [decimal](18, 2) NOT NULL,
	[LTVFifteen] [decimal](18, 2) NOT NULL,
	[LTVThirty] [decimal](18, 2) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_ReportUserLTV] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[MarketID] ASC,
	[SpreadChannelID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReportUserValue]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReportUserValue](
	[DateID] [int] NOT NULL,
	[MarketID] [int] NOT NULL,
	[SpreadChanneID] [int] NOT NULL,
	[UserCount] [int] NOT NULL,
	[OneDayUserPayAmount] [decimal](18, 2) NOT NULL,
	[OneDayUserWithdrawAmount] [decimal](18, 2) NOT NULL,
	[TwoDayUserPayAmount] [decimal](18, 2) NOT NULL,
	[TwoDayUserWithdrawAmount] [decimal](18, 2) NOT NULL,
	[ThreeDayUserPayAmount] [decimal](18, 2) NOT NULL,
	[ThreeDayUserWithdrawAmount] [decimal](18, 2) NOT NULL,
	[FourDayUserPayAmount] [decimal](18, 2) NOT NULL,
	[FourDayUserWithdrawAmount] [decimal](18, 2) NOT NULL,
	[FiveDayUserPayAmount] [decimal](18, 2) NOT NULL,
	[FiveDayUserWithdrawAmount] [decimal](18, 2) NOT NULL,
	[SevenDayUserPayAmount] [decimal](18, 2) NOT NULL,
	[SevenDayUserWithdrawAmount] [decimal](18, 2) NOT NULL,
	[FiftenDayUserPayAmount] [decimal](18, 2) NOT NULL,
	[FiftenDayUserWithdrawAmount] [decimal](18, 2) NOT NULL,
	[ThirtyDayUserPayAmount] [decimal](18, 2) NOT NULL,
	[ThirtyDayUserWithdrawAmount] [decimal](18, 2) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_ReportUserValue_1] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[SpreadChanneID] ASC,
	[MarketID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReportVip]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReportVip](
	[DateID] [int] NOT NULL,
	[VipOrder] [int] NOT NULL,
	[UserCount] [int] NOT NULL,
	[NewUserCount] [int] NOT NULL,
 CONSTRAINT [PK_ReportVip] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[VipOrder] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[StreamGameDownInfo]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[StreamGameDownInfo](
	[DateID] [int] NOT NULL,
	[ReleaseID] [int] NOT NULL,
	[DownCount] [int] NOT NULL,
	[RegisterCount] [int] NOT NULL,
	[FirstCollectDate] [datetime] NOT NULL,
	[LastCollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_StreamGameDownInfo_1] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[ReleaseID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[WithdrawUserLTV]    Script Date: 2023/7/12 11:10:14 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[WithdrawUserLTV](
	[DateID] [int] NOT NULL,
	[MarketID] [int] NOT NULL,
	[SpreadChannelID] [int] NOT NULL,
	[UserCount] [int] NOT NULL,
	[LTVOne] [decimal](18, 2) NOT NULL,
	[LTVTwo] [decimal](18, 2) NOT NULL,
	[LTVThree] [decimal](18, 2) NOT NULL,
	[LTVFour] [decimal](18, 2) NOT NULL,
	[LTVFive] [decimal](18, 2) NOT NULL,
	[LTVSix] [decimal](18, 2) NOT NULL,
	[LTVSeven] [decimal](18, 2) NOT NULL,
	[LTVEight] [decimal](18, 2) NOT NULL,
	[LTVNine] [decimal](18, 2) NOT NULL,
	[LTVTen] [decimal](18, 2) NOT NULL,
	[LTVEleven] [decimal](18, 2) NOT NULL,
	[LTVTwelve] [decimal](18, 2) NOT NULL,
	[LTVThirteen] [decimal](18, 2) NOT NULL,
	[LTVFourteen] [decimal](18, 2) NOT NULL,
	[LTVFifteen] [decimal](18, 2) NOT NULL,
	[LTVThirty] [decimal](18, 2) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_WithdrawUserLTV] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[MarketID] ASC,
	[SpreadChannelID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_StationID]  DEFAULT ((0)) FOR [ReportedStationID]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_ReportedType]  DEFAULT ((0)) FOR [ReportedType]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_UserStationID]  DEFAULT ((0)) FOR [UserStationID]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_GameID]  DEFAULT ((0)) FOR [GameID]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_Account]  DEFAULT (N'') FOR [Accounts]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_RealName]  DEFAULT (N'') FOR [UserName]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_SafeQQ]  DEFAULT (N'') FOR [QQMsn]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_Tel]  DEFAULT (N'') FOR [LinkPhone]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_OldAccounts1]  DEFAULT ('') FOR [OldAccounts1]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_OldAccounts2]  DEFAULT ('') FOR [OldAccounts2]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_OldLogonPass1]  DEFAULT ('') FOR [OldPass1]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_OldLogonPass2]  DEFAULT ('') FOR [OldPass2]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_PassportType]  DEFAULT ((0)) FOR [PassportType]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_PassportID]  DEFAULT ('') FOR [PassportID]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_OldQuestion]  DEFAULT (N'') FOR [OldQuestion]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_OldAnswer]  DEFAULT (N'') FOR [OldResponse]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_OldEmail]  DEFAULT (N'') FOR [OldEmail]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_Other]  DEFAULT (N'') FOR [SuppInfo]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_ProcessStatus]  DEFAULT ((0)) FOR [ProcessStatus]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_SendCount]  DEFAULT ((0)) FOR [SendCount]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_FoundDate]  DEFAULT (((1980)-(1))-(1)) FOR [SolveDate]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_OverDate]  DEFAULT (((1980)-(1))-(1)) FOR [OverDate]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_ClientIP]  DEFAULT ('000.000.000.000') FOR [ReportIP]
GO
ALTER TABLE [dbo].[AccountsLossReport] ADD  CONSTRAINT [DF_AccountsLossReport_CollectDate]  DEFAULT (getdate()) FOR [ReportDate]
GO
ALTER TABLE [dbo].[CacheChartsBattleCount] ADD  CONSTRAINT [DF_CacheChartsBattleCount_KindID]  DEFAULT ((0)) FOR [KindID]
GO
ALTER TABLE [dbo].[CacheChartsBattleCount] ADD  CONSTRAINT [DF_CacheChartsBattleCount_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[CacheChartsBattleCount] ADD  CONSTRAINT [DF_CacheChartsBattleCount_ChartID]  DEFAULT ((0)) FOR [ChartID]
GO
ALTER TABLE [dbo].[CacheChartsBattleCount] ADD  CONSTRAINT [DF_CacheChartsBattleCount_GameID]  DEFAULT ((0)) FOR [GameID]
GO
ALTER TABLE [dbo].[CacheChartsBattleCount] ADD  CONSTRAINT [DF_CacheChartsBattleCount_FaceID]  DEFAULT ((0)) FOR [FaceID]
GO
ALTER TABLE [dbo].[CacheChartsBattleCount] ADD  CONSTRAINT [DF_CacheChartsBattleCount_CustomID]  DEFAULT ((0)) FOR [CustomID]
GO
ALTER TABLE [dbo].[CacheChartsBattleCount] ADD  CONSTRAINT [DF_CacheChartsBattleCount_Gender]  DEFAULT ((0)) FOR [Gender]
GO
ALTER TABLE [dbo].[CacheChartsBattleCount] ADD  CONSTRAINT [DF_CacheChartsBattleCount_BattleCount]  DEFAULT ((0)) FOR [BattleCount]
GO
ALTER TABLE [dbo].[CacheChartsBattleCount] ADD  CONSTRAINT [DF_CacheChartsBattleCount_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[CacheChartsCommission] ADD  CONSTRAINT [DF_CacheChartsCommission_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[CacheChartsMedal] ADD  CONSTRAINT [DF_CacheChartsMedal_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[CacheChartsMedal] ADD  CONSTRAINT [DF_CacheChartsMedal_GameID]  DEFAULT ((0)) FOR [GameID]
GO
ALTER TABLE [dbo].[CacheChartsMedal] ADD  CONSTRAINT [DF_CacheChartsMedal_FaceID]  DEFAULT ((0)) FOR [FaceID]
GO
ALTER TABLE [dbo].[CacheChartsMedal] ADD  CONSTRAINT [DF_CacheChartsMedal_CustomID]  DEFAULT ((0)) FOR [CustomID]
GO
ALTER TABLE [dbo].[CacheChartsMedal] ADD  CONSTRAINT [DF_CacheChartsMedal_Gender]  DEFAULT ((0)) FOR [Gender]
GO
ALTER TABLE [dbo].[CacheChartsMedal] ADD  CONSTRAINT [DF_CacheChartsMedal_UserMedal]  DEFAULT ((0)) FOR [UserMedal]
GO
ALTER TABLE [dbo].[CacheChartsMedal] ADD  CONSTRAINT [DF_CacheChartsMedal_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RankingListRange] ADD  CONSTRAINT [RankingListRange_TypeID]  DEFAULT ((0)) FOR [TypeID]
GO
ALTER TABLE [dbo].[RankingListRange] ADD  CONSTRAINT [DF_RankingListRange_DateID]  DEFAULT ((0)) FOR [DateID]
GO
ALTER TABLE [dbo].[RankingListRange] ADD  CONSTRAINT [RankingListRange_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[RankingListRange] ADD  CONSTRAINT [RankingListRange_NickName]  DEFAULT ('') FOR [NickName]
GO
ALTER TABLE [dbo].[RankingListRange] ADD  CONSTRAINT [RankingListRangeRange_Gender]  DEFAULT ((0)) FOR [Gender]
GO
ALTER TABLE [dbo].[RankingListRange] ADD  CONSTRAINT [RankingListRange_FaceID]  DEFAULT ((0)) FOR [FaceID]
GO
ALTER TABLE [dbo].[RankingListRange] ADD  CONSTRAINT [RankingListRange_CustomID]  DEFAULT ((0)) FOR [CustomID]
GO
ALTER TABLE [dbo].[RankingListRange] ADD  CONSTRAINT [RankingListRangeRange_ActorID]  DEFAULT ((1)) FOR [ActorID]
GO
ALTER TABLE [dbo].[RankingListRange] ADD  CONSTRAINT [RankingListRangeRange_FaceFrameID]  DEFAULT ((1)) FOR [FaceFrameID]
GO
ALTER TABLE [dbo].[RankingListRange] ADD  CONSTRAINT [RankingListRange_RankID]  DEFAULT ((0)) FOR [RankID]
GO
ALTER TABLE [dbo].[RankingListRange] ADD  CONSTRAINT [RankingListRange_RankValue]  DEFAULT ((0)) FOR [Score]
GO
ALTER TABLE [dbo].[RankingListVersion] ADD  CONSTRAINT [DF_RankingListVersion_ModifyDateTime]  DEFAULT (getdate()) FOR [ModifyDateTime]
GO
ALTER TABLE [dbo].[RecordAccountsExpend] ADD  CONSTRAINT [DF_RecordAccountsExpend_ClientIP]  DEFAULT (N'000.000.000.000') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordAccountsExpend] ADD  CONSTRAINT [DF_RecordAccountsExpend_Cost]  DEFAULT ((0)) FOR [Cost]
GO
ALTER TABLE [dbo].[RecordAccountsExpend] ADD  CONSTRAINT [DF_RecordAccountsExpend_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordBindMachine] ADD  CONSTRAINT [DF_RecordBindMachine_MoorMachine]  DEFAULT ((0)) FOR [MoorMachine]
GO
ALTER TABLE [dbo].[RecordBindMachine] ADD  CONSTRAINT [DF_RecordBindMachine_IsClient]  DEFAULT ((0)) FOR [IsGamePlaza]
GO
ALTER TABLE [dbo].[RecordBindMachine] ADD  CONSTRAINT [DF_RecordBindMachine_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordBuyQrcode] ADD  CONSTRAINT [DF_RecordBuyQrcode_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[RecordBuyQrcode] ADD  CONSTRAINT [DF_RecordBuyQrcode_BuyType]  DEFAULT ((0)) FOR [BuyType]
GO
ALTER TABLE [dbo].[RecordBuyQrcode] ADD  CONSTRAINT [DF_RecordBuyQrcode_BuyPrice]  DEFAULT ((0)) FOR [BuyPrice]
GO
ALTER TABLE [dbo].[RecordBuyQrcode] ADD  CONSTRAINT [DF_RecordBuyQrcode_QrcodeTicket]  DEFAULT ('') FOR [QrTicket]
GO
ALTER TABLE [dbo].[RecordBuyQrcode] ADD  CONSTRAINT [DF_RecordBuyQrcode_ClientIP]  DEFAULT ('') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordBuyQrcode] ADD  CONSTRAINT [DF_RecordBuyQrcode_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordChangeGold] ADD  CONSTRAINT [DF_RecordChangeGold_GameID]  DEFAULT ((0)) FOR [GameID]
GO
ALTER TABLE [dbo].[RecordChangeGold] ADD  CONSTRAINT [DF_RecordChangeGold_CollectClientIP]  DEFAULT ((0)) FOR [CollectClientIP]
GO
ALTER TABLE [dbo].[RecordChangeGold] ADD  CONSTRAINT [DF_RecordChangeGold_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[RecordChangeGoods] ADD  CONSTRAINT [DF_RecordChangeGoods_ChangeType]  DEFAULT ((0)) FOR [TypeID]
GO
ALTER TABLE [dbo].[RecordChangeGoods] ADD  CONSTRAINT [DF_RecordChangeGoods_RelationID]  DEFAULT ((0)) FOR [RelationID]
GO
ALTER TABLE [dbo].[RecordChangeGoods] ADD  CONSTRAINT [DF_RecordChangeGoods_CollectClientIP]  DEFAULT (N'') FOR [CollectClientIP]
GO
ALTER TABLE [dbo].[RecordChangeGoods] ADD  CONSTRAINT [DF_RecordChangeGoods_CollectNote]  DEFAULT ('') FOR [CollectNote]
GO
ALTER TABLE [dbo].[RecordChangeIngot] ADD  CONSTRAINT [DF_RecordChangeIngot_GameID]  DEFAULT ((0)) FOR [GameID]
GO
ALTER TABLE [dbo].[RecordChangeIngot] ADD  CONSTRAINT [DF_RecordChangeIngot_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[RecordChangeLoveHeart] ADD  CONSTRAINT [DF_RecordChangeLoveHeart_GameID]  DEFAULT ((0)) FOR [GameID]
GO
ALTER TABLE [dbo].[RecordChangeLoveHeart] ADD  CONSTRAINT [DF_RecordChangeLoveHeart_CollectClientIP]  DEFAULT ((0)) FOR [CollectClientIP]
GO
ALTER TABLE [dbo].[RecordChangeLoveHeart] ADD  CONSTRAINT [DF_RecordChangeLoveHeart_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[RecordCheckIn] ADD  CONSTRAINT [DF_RecordCheckIn_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[RecordCheckIn] ADD  CONSTRAINT [DF_RecordCheckIn_PresentScore]  DEFAULT ((0)) FOR [PresentScore]
GO
ALTER TABLE [dbo].[RecordCheckIn] ADD  CONSTRAINT [DF_RecordCheckIn_LxCount]  DEFAULT ((0)) FOR [LxCount]
GO
ALTER TABLE [dbo].[RecordCheckIn] ADD  CONSTRAINT [DF_RecordCheckIn_LxScore]  DEFAULT ((0)) FOR [LxScore]
GO
ALTER TABLE [dbo].[RecordCheckIn] ADD  CONSTRAINT [DF_RecordCheckIn_MachineID]  DEFAULT ('') FOR [MachineID]
GO
ALTER TABLE [dbo].[RecordCheckIn] ADD  CONSTRAINT [DF_RecordCheckIn_IPAddress]  DEFAULT ('') FOR [IPAddress]
GO
ALTER TABLE [dbo].[RecordCheckIn] ADD  CONSTRAINT [DF_RecordCheckIn_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordClearFlee] ADD  CONSTRAINT [DF_RecordClearFlee_CurFlee]  DEFAULT ((0)) FOR [CurFlee]
GO
ALTER TABLE [dbo].[RecordClearFlee] ADD  CONSTRAINT [DF_RecordClearFlee_ClearType]  DEFAULT ((0)) FOR [ClearType]
GO
ALTER TABLE [dbo].[RecordClearFlee] ADD  CONSTRAINT [DF_RecordClearFlee_ClientIP]  DEFAULT ('000.000.000.000') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordClearFlee] ADD  CONSTRAINT [DF_RecordClearFlee_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordClearScore] ADD  CONSTRAINT [DF_RecordClearScore_CurScore]  DEFAULT ((0)) FOR [CurScore]
GO
ALTER TABLE [dbo].[RecordClearScore] ADD  CONSTRAINT [DF_RecordClearScore_ClearType]  DEFAULT ((0)) FOR [ClearType]
GO
ALTER TABLE [dbo].[RecordClearScore] ADD  CONSTRAINT [DF_RecordClearScore_ClientIP]  DEFAULT ('000.000.000.000') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordClearScore] ADD  CONSTRAINT [DF_RecordClearScore_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordDeductGold] ADD  CONSTRAINT [DF_RecordDeductGold_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[RecordDeductGold] ADD  CONSTRAINT [DF_RecordDeductGold_CurGold]  DEFAULT ((0)) FOR [CurGold]
GO
ALTER TABLE [dbo].[RecordDeductGold] ADD  CONSTRAINT [DF_RecordDeductGold_DeductGold]  DEFAULT ((0)) FOR [DeductGold]
GO
ALTER TABLE [dbo].[RecordDeductGold] ADD  CONSTRAINT [DF_RecordDeductGold_MasterID]  DEFAULT ((0)) FOR [MasterID]
GO
ALTER TABLE [dbo].[RecordDeductGold] ADD  CONSTRAINT [DF_RecordDeductGold_CollectClientIP]  DEFAULT ('') FOR [CollectClientIP]
GO
ALTER TABLE [dbo].[RecordDeductGold] ADD  CONSTRAINT [DF_RecordDeductGold_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[RecordDrawCommission] ADD  CONSTRAINT [DF_RecordChangeCommision_GameID]  DEFAULT ((0)) FOR [NickName]
GO
ALTER TABLE [dbo].[RecordDrawCommission] ADD  CONSTRAINT [DF_RecordChangeCommision_CollectClientIP]  DEFAULT ((0)) FOR [CollectClientIP]
GO
ALTER TABLE [dbo].[RecordDrawCommission] ADD  CONSTRAINT [DF_RecordDrawCommision_CollectClientIP1]  DEFAULT ((0)) FOR [CollectMachineID]
GO
ALTER TABLE [dbo].[RecordDrawCommission] ADD  CONSTRAINT [DF_RecordChangeCommision_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[RecordFeedback] ADD  CONSTRAINT [DF_RecordFeedback_FeedbackDate]  DEFAULT (getdate()) FOR [FeedbackDate]
GO
ALTER TABLE [dbo].[RecordFeedback] ADD  CONSTRAINT [DF_RecordFeedback_FeedbackIP]  DEFAULT ('') FOR [FeedbackIP]
GO
ALTER TABLE [dbo].[RecordFeedback] ADD  CONSTRAINT [DF_RecordFeedback_ViewCount]  DEFAULT ((0)) FOR [ViewCount]
GO
ALTER TABLE [dbo].[RecordFeedback] ADD  CONSTRAINT [DF_RecordFeedback_RevertStatus]  DEFAULT ((0)) FOR [RevertStatus]
GO
ALTER TABLE [dbo].[RecordGameExpend] ADD  CONSTRAINT [DF_RecordUserExpend_ClearScore]  DEFAULT ((0)) FOR [ClearScore]
GO
ALTER TABLE [dbo].[RecordGameExpend] ADD  CONSTRAINT [DF_RecordUserExpend_ClearFlee]  DEFAULT ((0)) FOR [ClearFlee]
GO
ALTER TABLE [dbo].[RecordGameExpend] ADD  CONSTRAINT [DF_RecordUserExpend_ClientIP]  DEFAULT ('000.000.000.000') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordGameExpend] ADD  CONSTRAINT [DF_RecordUserExpend_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordGrantClearScore] ADD  CONSTRAINT [DF_RecordGrantClearScore_ClientIP]  DEFAULT ('000.000.000.000') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordGrantClearScore] ADD  CONSTRAINT [DF_RecordGrantClearScore_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordGrantClearScore] ADD  CONSTRAINT [DF_RecordGrantClearScore_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[RecordGrantClearScore] ADD  CONSTRAINT [DF_RecordGrantClearScore_CurScore]  DEFAULT ((0)) FOR [CurScore]
GO
ALTER TABLE [dbo].[RecordGrantClearScore] ADD  CONSTRAINT [DF_RecordGrantClearScore_Reason]  DEFAULT ('') FOR [Reason]
GO
ALTER TABLE [dbo].[RecordGrantExperience] ADD  CONSTRAINT [DF_RecordGrantExperience_ClientIP]  DEFAULT ('000.000.000.000') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordGrantExperience] ADD  CONSTRAINT [DF_RecordGrantExperience_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordGrantExperience] ADD  CONSTRAINT [DF_RecordGrantExperience_CurExperience]  DEFAULT ((0)) FOR [CurExperience]
GO
ALTER TABLE [dbo].[RecordGrantExperience] ADD  CONSTRAINT [DF_RecordGrantExperience_AddExperience]  DEFAULT ((0)) FOR [AddExperience]
GO
ALTER TABLE [dbo].[RecordGrantExperience] ADD  CONSTRAINT [DF_RecordGrantExperience_Reason]  DEFAULT (N'') FOR [Reason]
GO
ALTER TABLE [dbo].[RecordGrantFlee] ADD  CONSTRAINT [DF_RecordGrantFlee_ClientIP]  DEFAULT ('000.000.000.000') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordGrantFlee] ADD  CONSTRAINT [DF_RecordGrantFlee_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordGrantFlee] ADD  CONSTRAINT [DF_RecordGrantFlee_CurFlee]  DEFAULT ((0)) FOR [CurFlee]
GO
ALTER TABLE [dbo].[RecordGrantFlee] ADD  CONSTRAINT [DF_RecordGrantFlee_AddFlee]  DEFAULT ((0)) FOR [AddFlee]
GO
ALTER TABLE [dbo].[RecordGrantFlee] ADD  CONSTRAINT [DF_RecordGrantFlee_Reason]  DEFAULT (N'') FOR [Reason]
GO
ALTER TABLE [dbo].[RecordGrantGameID] ADD  CONSTRAINT [DF_RecordGrantGameID_IDLevel]  DEFAULT ((0)) FOR [IDLevel]
GO
ALTER TABLE [dbo].[RecordGrantGameID] ADD  CONSTRAINT [DF_RecordGrantGameID_ClientIP]  DEFAULT ('000.000.000.000') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordGrantGameID] ADD  CONSTRAINT [DF_RecordGrantGameID_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordGrantGold] ADD  CONSTRAINT [DF_RecordGrantGold_CollectClientIP]  DEFAULT ('') FOR [CollectClientIP]
GO
ALTER TABLE [dbo].[RecordGrantGold] ADD  CONSTRAINT [DF_RecordGrantGold_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[RecordGrantGoods] ADD  CONSTRAINT [DF_RecordGrantGoods_CollectClientIP]  DEFAULT ('') FOR [CollectClientIP]
GO
ALTER TABLE [dbo].[RecordGrantGoods] ADD  CONSTRAINT [DF_RecordGrantGoods_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[RecordGrantIngot] ADD  CONSTRAINT [DF_RecordGrantIngot_CollectClientIP]  DEFAULT ('') FOR [CollectClientIP]
GO
ALTER TABLE [dbo].[RecordGrantIngot] ADD  CONSTRAINT [DF_RecordGrantIngot_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[RecordGrantLoveHeart] ADD  CONSTRAINT [DF_RecordGrantLoveHeart_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[RecordGrantLoveHeart] ADD  CONSTRAINT [DF_Table_1_CurIngot]  DEFAULT ((0)) FOR [CurLoveHeart]
GO
ALTER TABLE [dbo].[RecordGrantLoveHeart] ADD  CONSTRAINT [DF_RecordGrantLoveHeart_AddLoveHeart]  DEFAULT ((0)) FOR [AddLoveHeart]
GO
ALTER TABLE [dbo].[RecordGrantLoveHeart] ADD  CONSTRAINT [DF_RecordGrantLoveHeart_MasterID]  DEFAULT ((0)) FOR [MasterID]
GO
ALTER TABLE [dbo].[RecordGrantLoveHeart] ADD  CONSTRAINT [DF_RecordGrantLoveHeart_CollectClientIP]  DEFAULT ('') FOR [CollectClientIP]
GO
ALTER TABLE [dbo].[RecordGrantLoveHeart] ADD  CONSTRAINT [DF_RecordGrantLoveHeart_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[RecordGrantVip] ADD  CONSTRAINT [DF_RecordGrantVip_MasterID]  DEFAULT ((0)) FOR [MasterID]
GO
ALTER TABLE [dbo].[RecordGrantVip] ADD  CONSTRAINT [DF_RecordGrantVip_CollectClientIP]  DEFAULT ('') FOR [CollectClientIP]
GO
ALTER TABLE [dbo].[RecordGrantVip] ADD  CONSTRAINT [DF_RecordGrantVip_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[RecordModifyPassword] ADD  CONSTRAINT [DF_RecordModifyPassword_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[RecordSendPresent] ADD  CONSTRAINT [DF_RecordSendPresent_SendTime]  DEFAULT (getdate()) FOR [SendTime]
GO
ALTER TABLE [dbo].[RecordServiceRemarks] ADD  CONSTRAINT [DF_RecordServiceRemarks_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[RecordServiceRemarks] ADD  CONSTRAINT [DF_RecordServiceRemarks_Remarks]  DEFAULT (N'N('')') FOR [Remarks]
GO
ALTER TABLE [dbo].[RecordServiceRemarks] ADD  CONSTRAINT [DF_RecordServiceRemarks_CreateDate]  DEFAULT (getdate()) FOR [CreateDate]
GO
ALTER TABLE [dbo].[RecordServiceRemarks] ADD  CONSTRAINT [DF_RecordServiceRemarks_UpdateCount]  DEFAULT ((0)) FOR [UpdateCount]
GO
ALTER TABLE [dbo].[RecordServiceRemarks] ADD  CONSTRAINT [DF_RecordServiceRemarks_UpdateDate]  DEFAULT (getdate()) FOR [UpdateDate]
GO
ALTER TABLE [dbo].[RecordSpreadRewards] ADD  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordStockAddReduce] ADD  CONSTRAINT [DF_RecordStockAddReduce_BetAmount]  DEFAULT ((0)) FOR [BetAmount]
GO
ALTER TABLE [dbo].[RecordStockAddReduce] ADD  CONSTRAINT [DF_RecordStockAddReduce_TypeID]  DEFAULT ((0)) FOR [TypeID]
GO
ALTER TABLE [dbo].[RecordStockAddReduce] ADD  CONSTRAINT [DF_RecordStockAddReduce_OperationAmount]  DEFAULT ((0)) FOR [OperationAmount]
GO
ALTER TABLE [dbo].[RecordStockAddReduce] ADD  CONSTRAINT [DF_RecordStockAddReduce_Remark]  DEFAULT (N'') FOR [Remark]
GO
ALTER TABLE [dbo].[RecordStockControl] ADD  CONSTRAINT [DF_RecordStockControl_BetAmount]  DEFAULT ((0)) FOR [BetAmount]
GO
ALTER TABLE [dbo].[RecordStockControl] ADD  CONSTRAINT [DF_RecordStockControl_StockName]  DEFAULT (N'') FOR [StockName]
GO
ALTER TABLE [dbo].[RecordStockControl] ADD  CONSTRAINT [DF_RecordStockControl_StockLevel]  DEFAULT ((1)) FOR [StockLevel]
GO
ALTER TABLE [dbo].[RecordStockControl] ADD  CONSTRAINT [DF_RecordStockControl_FirstStock]  DEFAULT ((0)) FOR [FirstStock]
GO
ALTER TABLE [dbo].[RecordStockControl] ADD  CONSTRAINT [DF_RecordStockControl_StockScore]  DEFAULT ((0)) FOR [StockScore]
GO
ALTER TABLE [dbo].[RecordStockControl] ADD  CONSTRAINT [DF_RecordStockControl_ExtraScore]  DEFAULT ((0)) FOR [ExtraScore]
GO
ALTER TABLE [dbo].[RecordStockControl] ADD  CONSTRAINT [DF_RecordStockControl_FirstShrinkProfit]  DEFAULT ((0)) FOR [FirstShrinkProfit]
GO
ALTER TABLE [dbo].[RecordStockControl] ADD  CONSTRAINT [DF_RecordStockControl_ShrinkProfit]  DEFAULT ((0)) FOR [ShrinkProfit]
GO
ALTER TABLE [dbo].[RecordStockControl] ADD  CONSTRAINT [DF_RecordStockControl_ShrinkOption]  DEFAULT (N'') FOR [ShrinkOption]
GO
ALTER TABLE [dbo].[RecordStockControl] ADD  CONSTRAINT [DF_RecordStockControl_FirstGold]  DEFAULT ((0)) FOR [FirstGold]
GO
ALTER TABLE [dbo].[RecordStockControl] ADD  CONSTRAINT [DF_RecordStockControl_GoldScore]  DEFAULT ((0)) FOR [GoldScore]
GO
ALTER TABLE [dbo].[RecordStockControl] ADD  CONSTRAINT [DF_RecordStockControl_ShrinkRatio]  DEFAULT ((0)) FOR [ShrinkRatio]
GO
ALTER TABLE [dbo].[RecordStockControl] ADD  CONSTRAINT [DF_RecordStockControl_TotalAddReduceStock]  DEFAULT ((0)) FOR [TotalAddReduceStock]
GO
ALTER TABLE [dbo].[RecordStockControl] ADD  CONSTRAINT [DF_RecordStockControl_TotalAddReduceGold]  DEFAULT ((0)) FOR [TotalAddReduceGold]
GO
ALTER TABLE [dbo].[RecordStockControl] ADD  CONSTRAINT [DF_RecordStockControl_RelationIDList]  DEFAULT (N'') FOR [RelationIDList]
GO
ALTER TABLE [dbo].[RecordStockControl] ADD  CONSTRAINT [DF_RecordStockControl_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[RecordStockControl] ADD  CONSTRAINT [DF_RecordStockControl_JackpotInfo]  DEFAULT (N'') FOR [JackpotInfo]
GO
ALTER TABLE [dbo].[RecordStockModify] ADD  CONSTRAINT [DF_RecordStockModify_StockID]  DEFAULT ((0)) FOR [StockID]
GO
ALTER TABLE [dbo].[RecordStockModify] ADD  CONSTRAINT [DF_Table_1_CurScore]  DEFAULT ((0)) FOR [BeforeStock]
GO
ALTER TABLE [dbo].[RecordStockModify] ADD  CONSTRAINT [DF_RecordStockModify_ChangeStock]  DEFAULT ((0)) FOR [ChangeStock]
GO
ALTER TABLE [dbo].[RecordStockModify] ADD  CONSTRAINT [DF_RecordStockModify_MasterID]  DEFAULT ((0)) FOR [MasterID]
GO
ALTER TABLE [dbo].[RecordStockModify] ADD  CONSTRAINT [DF_RecordStockModify_ClientIP]  DEFAULT ('') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordStockModify] ADD  CONSTRAINT [DF_RecordStockModify_Remark]  DEFAULT ('') FOR [Remark]
GO
ALTER TABLE [dbo].[RecordStockModify] ADD  CONSTRAINT [DF_RecordStockModify_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordUnderwriteExpend] ADD  CONSTRAINT [DF_RecordUnderwriteExpend_ReUnderwrite]  DEFAULT (N'') FOR [ReUnderwrite]
GO
ALTER TABLE [dbo].[RecordUnderwriteExpend] ADD  CONSTRAINT [DF_RecordUnderwriteExpend_ClientIP]  DEFAULT (N'000.000.000.000') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordUnderwriteExpend] ADD  CONSTRAINT [DF_RecordUnderwriteExpend_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordUserGift] ADD  CONSTRAINT [DF_RecordUserGift_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[RecordUserGift] ADD  CONSTRAINT [DF_RecordUserGift_TargetUserID]  DEFAULT ((0)) FOR [TargetUserID]
GO
ALTER TABLE [dbo].[RecordUserGift] ADD  CONSTRAINT [DF_RecordUserGift_GoodsID]  DEFAULT ((0)) FOR [GoodsID]
GO
ALTER TABLE [dbo].[RecordUserGift] ADD  CONSTRAINT [DF_RecordUserGift_GoodsCount]  DEFAULT ((0)) FOR [GoodsCount]
GO
ALTER TABLE [dbo].[RecordUserGift] ADD  CONSTRAINT [DF_RecordUserGift_LoveLiness]  DEFAULT ((0)) FOR [LoveLiness]
GO
ALTER TABLE [dbo].[RecordUserGift] ADD  CONSTRAINT [DF_RecordUserGift_SendType]  DEFAULT ((1)) FOR [SendType]
GO
ALTER TABLE [dbo].[RecordUserGift] ADD  CONSTRAINT [DF_RecordUserGift_LoadStatus]  DEFAULT ((0)) FOR [LoadStatus]
GO
ALTER TABLE [dbo].[RecordUserGift] ADD  CONSTRAINT [DF__RecordUse__Colle__4C0C31DC]  DEFAULT ('') FOR [CollectClientIP]
GO
ALTER TABLE [dbo].[RecordUserGift] ADD  CONSTRAINT [DF__RecordUse__Colle__4D005615]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[RecordUserManagement] ADD  CONSTRAINT [DF_RecordUserManagement_MasterID]  DEFAULT ((0)) FOR [MasterID]
GO
ALTER TABLE [dbo].[RecordUserManagement] ADD  CONSTRAINT [DF_RecordUserManagement_MasterAccounts]  DEFAULT (N'') FOR [MasterAccounts]
GO
ALTER TABLE [dbo].[RecordUserManagement] ADD  CONSTRAINT [DF_RecordUserManagement_MasterIP]  DEFAULT ('000.000.000.000') FOR [MasterIP]
GO
ALTER TABLE [dbo].[RecordUserManagement] ADD  CONSTRAINT [DF_RecordUserManagement_MasterDate]  DEFAULT (getdate()) FOR [MasterDate]
GO
ALTER TABLE [dbo].[RecordUserManagement] ADD  CONSTRAINT [DF_RecordUserManagement_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[RecordUserManagement] ADD  CONSTRAINT [DF_RecordUserManagement_GameID]  DEFAULT ((0)) FOR [GameID]
GO
ALTER TABLE [dbo].[RecordUserManagement] ADD  CONSTRAINT [DF_RecordUserManagement_UserAccounts]  DEFAULT (N'') FOR [UserAccounts]
GO
ALTER TABLE [dbo].[RecordUserManagement] ADD  CONSTRAINT [DF_RecordUserManagement_Action]  DEFAULT ((0)) FOR [Action]
GO
ALTER TABLE [dbo].[RecordUserManagement] ADD  CONSTRAINT [DF_RecordUserManagement_Reason]  DEFAULT (N'') FOR [Reason]
GO
ALTER TABLE [dbo].[RecordUserRedPack] ADD  CONSTRAINT [DF_RecordUserRedPack_Amount]  DEFAULT ((0)) FOR [Amount]
GO
ALTER TABLE [dbo].[RecordUserRedPack] ADD  CONSTRAINT [DF_RecordUserRedPack_StatusID]  DEFAULT ((0)) FOR [StatusID]
GO
ALTER TABLE [dbo].[RecordUserRedPack] ADD  CONSTRAINT [DF_RecordUserRedPack_OverTime]  DEFAULT (getdate()) FOR [OverTime]
GO
ALTER TABLE [dbo].[RecordUserRedPack] ADD  CONSTRAINT [DF_RecordUserRedPack_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[RecordUserRedPack] ADD  CONSTRAINT [DF_RecordUserRedPack_RecevieDate]  DEFAULT (getdate()) FOR [RecevieDate]
GO
ALTER TABLE [dbo].[RecordUserRedPack] ADD  CONSTRAINT [DF_RecordUserRedPack_RecevieIP]  DEFAULT ('') FOR [RecevieIP]
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
ALTER TABLE [dbo].[RecordWealthTransfer] ADD  CONSTRAINT [DF_RecordWealthTransfer_GameID]  DEFAULT ((0)) FOR [GameID]
GO
ALTER TABLE [dbo].[RecordWealthTransfer] ADD  CONSTRAINT [DF_RecordWealthTransfer_TargetGameID]  DEFAULT ((0)) FOR [TargetGameID]
GO
ALTER TABLE [dbo].[RecordWealthTransfer] ADD  CONSTRAINT [DF_RecordWealthTransfer_ServiceFee]  DEFAULT ((0)) FOR [ServiceFee]
GO
ALTER TABLE [dbo].[RecordWealthTransfer] ADD  CONSTRAINT [DF_RecordWealthTransfer_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_StatisticalDataDay_AccountNum]  DEFAULT ((0)) FOR [UserCount]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_StatisticalDataDay_PayUserCount]  DEFAULT ((0)) FOR [PayUserCount]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_PayTwiceUserCount]  DEFAULT ((0)) FOR [PayTwiceUserCount]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDataDay_LoginUser]  DEFAULT ((0)) FOR [TodayLoginUserCount]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDataDay_RegUserCount]  DEFAULT ((0)) FOR [TodayRegUserCount]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDataDay_NewVipCount]  DEFAULT ((0)) FOR [TodayNewVipCount]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_TodayPayUserCount]  DEFAULT ((0)) FOR [TodayPayUserCount]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_TodayPayMaxAmount]  DEFAULT ((0)) FOR [TodayPayMaxAmount]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportUserGlobal_DayActiveNumber]  DEFAULT ((0)) FOR [TodayDayActiveUser]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_TodayPlayGameUserCount]  DEFAULT ((0)) FOR [TodayPlayCommonUserCount]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_TodayPlayVipUserCount]  DEFAULT ((0)) FOR [TodayPlayVipUserCount]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_TodayBattleCreateRoomCount]  DEFAULT ((0)) FOR [TodayBattleCreateRoomCount]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_TodaySumOnlineTime]  DEFAULT ((0)) FOR [TodaySumOnlineTime]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_OneDayRemainRate]  DEFAULT ((0)) FOR [OneDayRemainRate]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_OneDayRemainRate1]  DEFAULT ((0)) FOR [TwoDayRemainRate]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_TwoDayRemainRate1]  DEFAULT ((0)) FOR [ThreeDayRemainRate]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_ThreeDayRemainRate1]  DEFAULT ((0)) FOR [FourDayRemainRate]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_FiveDayRemainRate]  DEFAULT ((0)) FOR [FiveDayRemainRate]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_FiveDayRemainRate1]  DEFAULT ((0)) FOR [ServenDayRemainRate]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_FiftenDayRemainRate]  DEFAULT ((0)) FOR [FiftenDayRemainRate]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_FiftenDayRemainRate1]  DEFAULT ((0)) FOR [ThirtyDayRemainRate]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_ReportDateTime]  DEFAULT (getdate()) FOR [ReportDateTime]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF__ReportDay__Today__5FF32EF8]  DEFAULT ((0)) FOR [TodayRegPayUserCount]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_TotalPlayGameUserCount]  DEFAULT ((0)) FOR [TotalPlayers]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_TodaySpreadCommission]  DEFAULT ((0)) FOR [TodaySpreadCommission]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_TodayGoldWithdrawAmount]  DEFAULT ((0)) FOR [TodayGoldWithdrawAmount]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_TodayNewPayAmount]  DEFAULT ((0)) FOR [TodayNewPayAmount]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_TodayNewPayUserCount]  DEFAULT ((0)) FOR [TodayNewPayUserCount]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_TodayNewLoginUserCount]  DEFAULT ((0)) FOR [TodayNewLoginUserCount]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_TodayNewPayCount]  DEFAULT ((0)) FOR [TodayNewPayCount]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_OldTodayPayUserCount]  DEFAULT ((0)) FOR [OldTodayPayUserCount]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_TodayPayAmount]  DEFAULT ((0)) FOR [TodayPayAmount]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_TodayGoldWithdrawSystemFee]  DEFAULT ((0)) FOR [TodayGoldWithdrawSystemFee]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_TodayGoldWithdrawChannelFee]  DEFAULT ((0)) FOR [TodayGoldWithdrawChannelFee]
GO
ALTER TABLE [dbo].[ReportDay] ADD  CONSTRAINT [DF_ReportDay_TodayGoldWithdrawUserCount]  DEFAULT ((0)) FOR [TodayGoldWithdrawUserCount]
GO
ALTER TABLE [dbo].[ReportDayCommission] ADD  CONSTRAINT [DF_ReportDayCommission_TotalPayAmount]  DEFAULT ((0)) FOR [TotalPayAmount]
GO
ALTER TABLE [dbo].[ReportDayCommission] ADD  CONSTRAINT [DF_ReportDayCommission_TotalBetAmount]  DEFAULT ((0)) FOR [TotalBetAmount]
GO
ALTER TABLE [dbo].[ReportDayCommission] ADD  CONSTRAINT [DF_ReportDayCommission_TotalPayContribute]  DEFAULT ((0)) FOR [TotalPayContribute]
GO
ALTER TABLE [dbo].[ReportDayCommission] ADD  CONSTRAINT [DF_ReportDayCommission_TotalBetContribute]  DEFAULT ((0)) FOR [TotalBetContribute]
GO
ALTER TABLE [dbo].[ReportDayCommission] ADD  CONSTRAINT [DF_ReportDayCommission_LowerPayCommission1]  DEFAULT ((0)) FOR [LowerPayAmount]
GO
ALTER TABLE [dbo].[ReportDayCommission] ADD  CONSTRAINT [DF_ReportDayCommission_LowerPayAmount1]  DEFAULT ((0)) FOR [LowerBetAmount]
GO
ALTER TABLE [dbo].[ReportDayCommission] ADD  CONSTRAINT [DF_ReportDayCommission_LowerPayAmount1_1]  DEFAULT ((0)) FOR [LLowerPayAmount]
GO
ALTER TABLE [dbo].[ReportDayCommission] ADD  CONSTRAINT [DF_ReportDayCommission_LLowerPayAmount1]  DEFAULT ((0)) FOR [LLowerBetAmount]
GO
ALTER TABLE [dbo].[ReportDayCommission] ADD  CONSTRAINT [DF__StreamCom__Lower__33F4B129]  DEFAULT ((0)) FOR [LowerPayContribute]
GO
ALTER TABLE [dbo].[ReportDayCommission] ADD  CONSTRAINT [DF__StreamCom__Lower__34E8D562]  DEFAULT ((0)) FOR [LowerBetContribute]
GO
ALTER TABLE [dbo].[ReportDayCommission] ADD  CONSTRAINT [DF__StreamCom__LLowe__35DCF99B]  DEFAULT ((0)) FOR [LLowerPayContribute]
GO
ALTER TABLE [dbo].[ReportDayCommission] ADD  CONSTRAINT [DF__StreamCom__LLowe__36D11DD4]  DEFAULT ((0)) FOR [LLowerBetContribute]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_MarketID]  DEFAULT ((0)) FOR [MarketID]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_UserCount]  DEFAULT ((0)) FOR [UserCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_PayUserCount]  DEFAULT ((0)) FOR [PayUserCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_PayTwiceUserCount]  DEFAULT ((0)) FOR [PayTwiceUserCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayPayAmount]  DEFAULT ((0)) FOR [TodayPayAmount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayPayCount]  DEFAULT ((0)) FOR [TodayPayCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayLoginUserCount]  DEFAULT ((0)) FOR [TodayLoginUserCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayRegUserCount]  DEFAULT ((0)) FOR [TodayRegUserCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayNewVipCount]  DEFAULT ((0)) FOR [TodayNewVipCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayPayUserCount]  DEFAULT ((0)) FOR [TodayPayUserCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayPayMaxAmount]  DEFAULT ((0)) FOR [TodayPayMaxAmount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodaySpreadCommission]  DEFAULT ((0)) FOR [TodaySpreadCommission]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayGoldWithdrawAmount]  DEFAULT ((0)) FOR [TodayGoldWithdrawAmount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayGoldWithdrawSystemFee]  DEFAULT ((0)) FOR [TodayGoldWithdrawSystemFee]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayGoldWithdrawChannelFee]  DEFAULT ((0)) FOR [TodayGoldWithdrawChannelFee]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayGoldWithdrawUserCount]  DEFAULT ((0)) FOR [TodayGoldWithdrawUserCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayDayActiveUser]  DEFAULT ((0)) FOR [TodayDayActiveUser]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayPlayCommonUserCount]  DEFAULT ((0)) FOR [TodayPlayCommonUserCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayPlayVipUserCount]  DEFAULT ((0)) FOR [TodayPlayVipUserCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayBattleCreateRoomCount]  DEFAULT ((0)) FOR [TodayBattleCreateRoomCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodaySumOnlineTime]  DEFAULT ((0)) FOR [TodaySumOnlineTime]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayNewPayAmount]  DEFAULT ((0)) FOR [TodayNewPayAmount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayNewPayUserCount]  DEFAULT ((0)) FOR [TodayNewPayUserCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayNewLoginUserCount]  DEFAULT ((0)) FOR [TodayNewLoginUserCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayNewPayCount]  DEFAULT ((0)) FOR [TodayNewPayCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_OneDayRemainRate]  DEFAULT ((0)) FOR [OneDayRemainRate]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TwoDayRemainRate]  DEFAULT ((0)) FOR [TwoDayRemainRate]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_ThreeDayRemainRate]  DEFAULT ((0)) FOR [ThreeDayRemainRate]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_FourDayRemainRate]  DEFAULT ((0)) FOR [FourDayRemainRate]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_FiveDayRemainRate]  DEFAULT ((0)) FOR [FiveDayRemainRate]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_ServenDayRemainRate]  DEFAULT ((0)) FOR [ServenDayRemainRate]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_FiftenDayRemainRate]  DEFAULT ((0)) FOR [FiftenDayRemainRate]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_ThirtyDayRemainRate]  DEFAULT ((0)) FOR [ThirtyDayRemainRate]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_ReportDateTime]  DEFAULT (getdate()) FOR [ReportDateTime]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayRegPayUserCount]  DEFAULT ((0)) FOR [TodayRegPayUserCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TotalPlayers]  DEFAULT ((0)) FOR [TotalPlayers]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_OldTodayPayUserCount]  DEFAULT ((0)) FOR [OldTodayPayUserCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayAllPayAmount]  DEFAULT ((0)) FOR [TodayAllPayAmount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayAllPayCount]  DEFAULT ((0)) FOR [TodayAllPayCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayAllPayUserCount]  DEFAULT ((0)) FOR [TodayAllPayUserCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayNewAllPayAmount]  DEFAULT ((0)) FOR [TodayNewAllPayAmount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayNewAllPayUserCount]  DEFAULT ((0)) FOR [TodayNewAllPayUserCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayNewAllPayCount]  DEFAULT ((0)) FOR [TodayNewAllPayCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayRegMachineCount]  DEFAULT ((0)) FOR [TodayRegMachineCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayRegPayMachineCount]  DEFAULT ((0)) FOR [TodayRegPayMachineCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayLoginMachineCount]  DEFAULT ((0)) FOR [TodayLoginMachineCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayNewFirstPayAmount]  DEFAULT ((0)) FOR [TodayNewFirstPayAmount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayNewTwoPayCount]  DEFAULT ((0)) FOR [TodayNewTwoPayCount]
GO
ALTER TABLE [dbo].[ReportDayMarket] ADD  CONSTRAINT [DF_ReportDayMarket_TodayNewTwoPayAmount]  DEFAULT ((0)) FOR [TodayNewTwoPayAmount]
GO
ALTER TABLE [dbo].[ReportDayUser] ADD  CONSTRAINT [DF_ReportDayUser_PayCount]  DEFAULT ((0)) FOR [PayCount]
GO
ALTER TABLE [dbo].[ReportDayUser] ADD  CONSTRAINT [DF_ReportDayUser_PayAmount]  DEFAULT ((0)) FOR [PayAmount]
GO
ALTER TABLE [dbo].[ReportDayUser] ADD  CONSTRAINT [DF_ReportDayUser_LastPayAmount]  DEFAULT ((0)) FOR [LastPayAmount]
GO
ALTER TABLE [dbo].[ReportDayUser] ADD  CONSTRAINT [DF_ReportDayUser_LastPayDate]  DEFAULT ('1900-01-01') FOR [LastPayDate]
GO
ALTER TABLE [dbo].[ReportDayUser] ADD  CONSTRAINT [DF_ReportDayUser_Experience]  DEFAULT ((0)) FOR [Experience]
GO
ALTER TABLE [dbo].[ReportDayUser] ADD  CONSTRAINT [DF_ReportDayUser_Loveliness]  DEFAULT ((0)) FOR [Loveliness]
GO
ALTER TABLE [dbo].[ReportDayUser] ADD  CONSTRAINT [DF_ReportDayUser_SpreadViewCount]  DEFAULT ((0)) FOR [SpreadViewCount]
GO
ALTER TABLE [dbo].[ReportDayUser] ADD  CONSTRAINT [DF_ReportDayUser_ReportDateTime]  DEFAULT (getdate()) FOR [ReportDateTime]
GO
ALTER TABLE [dbo].[ReportDayUser] ADD  CONSTRAINT [DF_ReportDayUser_SpreadLowerCount]  DEFAULT ((0)) FOR [SpreadLowerCount]
GO
ALTER TABLE [dbo].[ReportDayUser] ADD  CONSTRAINT [DF_ReportDayUser_SpreadLLowerCount]  DEFAULT ((0)) FOR [SpreadLLowerCount]
GO
ALTER TABLE [dbo].[ReportGameData] ADD  CONSTRAINT [DF_ReportGameData_GameNumber]  DEFAULT ((0)) FOR [GameNumber]
GO
ALTER TABLE [dbo].[ReportGameData] ADD  CONSTRAINT [DF_ReportGameData_UserService]  DEFAULT ((0)) FOR [UserService]
GO
ALTER TABLE [dbo].[ReportGameData] ADD  CONSTRAINT [DF_ReportGameData_UserRevenue]  DEFAULT ((0)) FOR [UserRevenue]
GO
ALTER TABLE [dbo].[ReportGameData] ADD  CONSTRAINT [DF_ReportGameData_PlayTimeCount]  DEFAULT ((0)) FOR [PlayTimeCount]
GO
ALTER TABLE [dbo].[ReportGameData] ADD  CONSTRAINT [DF_ReportGameData_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[ReportGameData] ADD  CONSTRAINT [DF_ReportGameData_PlayUserCount]  DEFAULT ((0)) FOR [PlayUserCount]
GO
ALTER TABLE [dbo].[ReportGameData] ADD  CONSTRAINT [DF_ReportGameData_UserBetAmount]  DEFAULT ((0)) FOR [UserBetAmount]
GO
ALTER TABLE [dbo].[ReportGoods] ADD  CONSTRAINT [DF_ReportGoods_GoodsID]  DEFAULT ((0)) FOR [GoodsID]
GO
ALTER TABLE [dbo].[ReportGoodsOpCn] ADD  CONSTRAINT [DF_ReportGoodsOutputConsumption_DateID]  DEFAULT ((0)) FOR [DateID]
GO
ALTER TABLE [dbo].[ReportGoodsOpCn] ADD  CONSTRAINT [DF_ReportGoodsOutputConsumption_GoodsID]  DEFAULT ((0)) FOR [GoodsID]
GO
ALTER TABLE [dbo].[ReportGoodsOpCn] ADD  CONSTRAINT [DF_Table_1_TypeID]  DEFAULT ((0)) FOR [ChangeTypeID]
GO
ALTER TABLE [dbo].[ReportGoodsOpCn] ADD  CONSTRAINT [DF_ReportGoodsOutputConsumption_GoodsCount]  DEFAULT ((0)) FOR [OutputCount]
GO
ALTER TABLE [dbo].[ReportGoodsOpCn] ADD  CONSTRAINT [DF_ReportGoodsOutputConsumption_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[ReportStock] ADD  CONSTRAINT [DF_Table_1_StartStock]  DEFAULT ((0)) FOR [StockStart]
GO
ALTER TABLE [dbo].[ReportStock] ADD  CONSTRAINT [DF_ReportStock_StockEnd]  DEFAULT ((0)) FOR [StockChange]
GO
ALTER TABLE [dbo].[ReportStock] ADD  CONSTRAINT [DF_ReportStock_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[ReportUserLTV] ADD  CONSTRAINT [DF_ReportUserLTV_MarketID]  DEFAULT ((0)) FOR [MarketID]
GO
ALTER TABLE [dbo].[ReportUserLTV] ADD  CONSTRAINT [DF_ReportUserLTV_UserCount]  DEFAULT ((0)) FOR [UserCount]
GO
ALTER TABLE [dbo].[ReportUserLTV] ADD  CONSTRAINT [DF_ReportUserLTV_LTVOne]  DEFAULT ((0)) FOR [LTVOne]
GO
ALTER TABLE [dbo].[ReportUserLTV] ADD  CONSTRAINT [DF_Table_1_LTVOne1]  DEFAULT ((0)) FOR [LTVTwo]
GO
ALTER TABLE [dbo].[ReportUserLTV] ADD  CONSTRAINT [DF_Table_1_LTVOne2]  DEFAULT ((0)) FOR [LTVThree]
GO
ALTER TABLE [dbo].[ReportUserLTV] ADD  CONSTRAINT [DF_Table_1_LTVOne3]  DEFAULT ((0)) FOR [LTVFour]
GO
ALTER TABLE [dbo].[ReportUserLTV] ADD  CONSTRAINT [DF_Table_1_LTVOne4]  DEFAULT ((0)) FOR [LTVFive]
GO
ALTER TABLE [dbo].[ReportUserLTV] ADD  CONSTRAINT [DF_Table_1_LTVOne5]  DEFAULT ((0)) FOR [LTVSix]
GO
ALTER TABLE [dbo].[ReportUserLTV] ADD  CONSTRAINT [DF_Table_1_LTVOne6]  DEFAULT ((0)) FOR [LTVSeven]
GO
ALTER TABLE [dbo].[ReportUserLTV] ADD  CONSTRAINT [DF_Table_1_LTVOne7]  DEFAULT ((0)) FOR [LTVEight]
GO
ALTER TABLE [dbo].[ReportUserLTV] ADD  CONSTRAINT [DF_Table_1_LTVOne8]  DEFAULT ((0)) FOR [LTVNine]
GO
ALTER TABLE [dbo].[ReportUserLTV] ADD  CONSTRAINT [DF_Table_1_LTVOne9]  DEFAULT ((0)) FOR [LTVTen]
GO
ALTER TABLE [dbo].[ReportUserLTV] ADD  CONSTRAINT [DF_Table_1_LTVOne10]  DEFAULT ((0)) FOR [LTVEleven]
GO
ALTER TABLE [dbo].[ReportUserLTV] ADD  CONSTRAINT [DF_Table_1_LTVOne11]  DEFAULT ((0)) FOR [LTVTwelve]
GO
ALTER TABLE [dbo].[ReportUserLTV] ADD  CONSTRAINT [DF_Table_1_LTVOne12]  DEFAULT ((0)) FOR [LTVThirteen]
GO
ALTER TABLE [dbo].[ReportUserLTV] ADD  CONSTRAINT [DF_Table_1_LTVOne13]  DEFAULT ((0)) FOR [LTVFourteen]
GO
ALTER TABLE [dbo].[ReportUserLTV] ADD  CONSTRAINT [DF_Table_1_LTVOne14]  DEFAULT ((0)) FOR [LTVFifteen]
GO
ALTER TABLE [dbo].[ReportUserLTV] ADD  CONSTRAINT [DF_Table_1_LTVOne15]  DEFAULT ((0)) FOR [LTVThirty]
GO
ALTER TABLE [dbo].[ReportUserLTV] ADD  CONSTRAINT [DF_ReportUserLTV_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[ReportVip] ADD  CONSTRAINT [DF_ReportVip_NewUserCount]  DEFAULT ((0)) FOR [NewUserCount]
GO
ALTER TABLE [dbo].[StreamGameDownInfo] ADD  CONSTRAINT [DF_StreamGameDownInfo_DownCount]  DEFAULT ((0)) FOR [DownCount]
GO
ALTER TABLE [dbo].[StreamGameDownInfo] ADD  CONSTRAINT [DF_StreamGameDownInfo_RegisterCount]  DEFAULT ((0)) FOR [RegisterCount]
GO
ALTER TABLE [dbo].[StreamGameDownInfo] ADD  CONSTRAINT [DF_StreamGameDownInfo_FirstCollectDate]  DEFAULT (getdate()) FOR [FirstCollectDate]
GO
ALTER TABLE [dbo].[StreamGameDownInfo] ADD  CONSTRAINT [DF_StreamGameDownInfo_LastCollectDate]  DEFAULT (getdate()) FOR [LastCollectDate]
GO
ALTER TABLE [dbo].[WithdrawUserLTV] ADD  CONSTRAINT [DF_WithdrawUserLTV_MarketID]  DEFAULT ((0)) FOR [MarketID]
GO
ALTER TABLE [dbo].[WithdrawUserLTV] ADD  CONSTRAINT [DF_WithdrawUserLTV_UserCount]  DEFAULT ((0)) FOR [UserCount]
GO
ALTER TABLE [dbo].[WithdrawUserLTV] ADD  CONSTRAINT [DF_WithdrawUserLTV_LTVOne]  DEFAULT ((0)) FOR [LTVOne]
GO
ALTER TABLE [dbo].[WithdrawUserLTV] ADD  CONSTRAINT [DF_WithdrawUserLTV_LTVTwo]  DEFAULT ((0)) FOR [LTVTwo]
GO
ALTER TABLE [dbo].[WithdrawUserLTV] ADD  CONSTRAINT [DF_WithdrawUserLTV_LTVThree]  DEFAULT ((0)) FOR [LTVThree]
GO
ALTER TABLE [dbo].[WithdrawUserLTV] ADD  CONSTRAINT [DF_WithdrawUserLTV_LTVFour]  DEFAULT ((0)) FOR [LTVFour]
GO
ALTER TABLE [dbo].[WithdrawUserLTV] ADD  CONSTRAINT [DF_WithdrawUserLTV_LTVFive]  DEFAULT ((0)) FOR [LTVFive]
GO
ALTER TABLE [dbo].[WithdrawUserLTV] ADD  CONSTRAINT [DF_WithdrawUserLTV_LTVSix]  DEFAULT ((0)) FOR [LTVSix]
GO
ALTER TABLE [dbo].[WithdrawUserLTV] ADD  CONSTRAINT [DF_WithdrawUserLTV_LTVSeven]  DEFAULT ((0)) FOR [LTVSeven]
GO
ALTER TABLE [dbo].[WithdrawUserLTV] ADD  CONSTRAINT [DF_WithdrawUserLTV_LTVTEight]  DEFAULT ((0)) FOR [LTVEight]
GO
ALTER TABLE [dbo].[WithdrawUserLTV] ADD  CONSTRAINT [DF_WithdrawUserLTV_LTVNine]  DEFAULT ((0)) FOR [LTVNine]
GO
ALTER TABLE [dbo].[WithdrawUserLTV] ADD  CONSTRAINT [DF_WithdrawUserLTV_LTVTen]  DEFAULT ((0)) FOR [LTVTen]
GO
ALTER TABLE [dbo].[WithdrawUserLTV] ADD  CONSTRAINT [DF_WithdrawUserLTV_LTVEleven]  DEFAULT ((0)) FOR [LTVEleven]
GO
ALTER TABLE [dbo].[WithdrawUserLTV] ADD  CONSTRAINT [DF_WithdrawUserLTV_LTVTwelve]  DEFAULT ((0)) FOR [LTVTwelve]
GO
ALTER TABLE [dbo].[WithdrawUserLTV] ADD  CONSTRAINT [DF_WithdrawUserLTV_LTVThirteen]  DEFAULT ((0)) FOR [LTVThirteen]
GO
ALTER TABLE [dbo].[WithdrawUserLTV] ADD  CONSTRAINT [DF_WithdrawUserLTV_LTVFourteen]  DEFAULT ((0)) FOR [LTVFourteen]
GO
ALTER TABLE [dbo].[WithdrawUserLTV] ADD  CONSTRAINT [DF_WithdrawUserLTV_LTVFifteen]  DEFAULT ((0)) FOR [LTVFifteen]
GO
ALTER TABLE [dbo].[WithdrawUserLTV] ADD  CONSTRAINT [DF_WithdrawUserLTV_LTVThirty]  DEFAULT ((0)) FOR [LTVThirty]
GO
ALTER TABLE [dbo].[WithdrawUserLTV] ADD  CONSTRAINT [DF_WithdrawUserLTV_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'挂失标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'ReportID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'申述单号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'ReportNo'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'申述站点' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'ReportedStationID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'申述类型(0:登录密码,1:银行密码)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'ReportedType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户站点' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'UserStationID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录帐号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'Accounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户姓名' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'UserName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'QQ或MSN' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'QQMsn'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'联系电话' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'LinkPhone'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'联系邮箱' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'LinkEmail'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'历史帐号1' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'OldAccounts1'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'历史帐号2' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'OldAccounts2'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'历史密码1' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'OldPass1'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'历史密码2' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'OldPass2'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'证件类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'PassportType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'证件号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'PassportID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'密保问题' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'OldQuestion'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'密保答案' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'OldResponse'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'安全邮箱' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'OldEmail'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'补充材料' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'SuppInfo'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'处理状态(0:未处理,1:发送成功邮件,2:发送失败邮件,3:更新密码成功)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'ProcessStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'发送次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'SendCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'解决日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'SolveDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户处理时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'OverDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'申述地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'ReportIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'申述日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsLossReport', @level2type=N'COLUMN',@level2name=N'ReportDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsBattleCount', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsBattleCount', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsBattleCount', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsBattleCount', @level2type=N'COLUMN',@level2name=N'ChartID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsBattleCount', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏账号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsBattleCount', @level2type=N'COLUMN',@level2name=N'Accounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'玩家昵称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsBattleCount', @level2type=N'COLUMN',@level2name=N'NickName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'头像标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsBattleCount', @level2type=N'COLUMN',@level2name=N'FaceID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'自定义头像标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsBattleCount', @level2type=N'COLUMN',@level2name=N'CustomID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'性别' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsBattleCount', @level2type=N'COLUMN',@level2name=N'Gender'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'约战次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsBattleCount', @level2type=N'COLUMN',@level2name=N'BattleCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录机器' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsBattleCount', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsCommission', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsCommission', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsCommission', @level2type=N'COLUMN',@level2name=N'ChartID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsCommission', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户账号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsCommission', @level2type=N'COLUMN',@level2name=N'Accounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户昵称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsCommission', @level2type=N'COLUMN',@level2name=N'Nickname'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'头像标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsCommission', @level2type=N'COLUMN',@level2name=N'FaceID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'自定义头像标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsCommission', @level2type=N'COLUMN',@level2name=N'CustomID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户性别' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsCommission', @level2type=N'COLUMN',@level2name=N'Gender'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'初次充值金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsCommission', @level2type=N'COLUMN',@level2name=N'FirstPay'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下注金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsCommission', @level2type=N'COLUMN',@level2name=N'TotalBet'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏上级收益' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsCommission', @level2type=N'COLUMN',@level2name=N'TotalCommission'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsCommission', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsMedal', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsMedal', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户账号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsMedal', @level2type=N'COLUMN',@level2name=N'Accounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户昵称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsMedal', @level2type=N'COLUMN',@level2name=N'NickName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'头像标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsMedal', @level2type=N'COLUMN',@level2name=N'FaceID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'自定义头像标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsMedal', @level2type=N'COLUMN',@level2name=N'CustomID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户性别' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsMedal', @level2type=N'COLUMN',@level2name=N'Gender'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户奖牌' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsMedal', @level2type=N'COLUMN',@level2name=N'UserMedal'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsMedal', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排行榜类型（1、充值排行 2、黄金排行  3、经验排行  4、魅力排行  5、约战排行）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RankingListRange', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RankingListRange', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RankingListRange', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RankingListRange', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户昵称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RankingListRange', @level2type=N'COLUMN',@level2name=N'NickName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'性别' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RankingListRange', @level2type=N'COLUMN',@level2name=N'Gender'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'头像标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RankingListRange', @level2type=N'COLUMN',@level2name=N'FaceID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'自定义头像标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RankingListRange', @level2type=N'COLUMN',@level2name=N'CustomID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'角色标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RankingListRange', @level2type=N'COLUMN',@level2name=N'ActorID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'头像框ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RankingListRange', @level2type=N'COLUMN',@level2name=N'FaceFrameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'会员点数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RankingListRange', @level2type=N'COLUMN',@level2name=N'VipPoint'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排行名次' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RankingListRange', @level2type=N'COLUMN',@level2name=N'RankID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排行值' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RankingListRange', @level2type=N'COLUMN',@level2name=N'Score'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排行榜类型（1、黄金排行  2、等级排行  3、魅力排行  4、公开房排行）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RankingListVersion', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修改时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RankingListVersion', @level2type=N'COLUMN',@level2name=N'ModifyDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAccountsExpend', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAccountsExpend', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户帐号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAccountsExpend', @level2type=N'COLUMN',@level2name=N'ReAccounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修改地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAccountsExpend', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'支持数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAccountsExpend', @level2type=N'COLUMN',@level2name=N'Cost'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修改日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAccountsExpend', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBindMachine', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBindMachine', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBindMachine', @level2type=N'COLUMN',@level2name=N'MachineSerial'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'绑定标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBindMachine', @level2type=N'COLUMN',@level2name=N'MoorMachine'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏大厅操作标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBindMachine', @level2type=N'COLUMN',@level2name=N'IsGamePlaza'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBuyQrcode', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBuyQrcode', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'购买固定二维码支付类型（1、钻石）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBuyQrcode', @level2type=N'COLUMN',@level2name=N'BuyType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'购买价格' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBuyQrcode', @level2type=N'COLUMN',@level2name=N'BuyPrice'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'固定二维码票据' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBuyQrcode', @level2type=N'COLUMN',@level2name=N'QrTicket'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'购买地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBuyQrcode', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'购买时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBuyQrcode', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeGold', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeGold', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeGold', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作类型（同步物品：0，注册赠送：1，游戏升级奖励：2，VIP升级奖励：3，兑换礼包码：13，领取低保：15，购买礼物：16，修改昵称：17，修改头像：18，转盘抽奖：19，货币转账：20，取出佣金：21，提现余额清理：79，领取邮件附件：80，领取推广奖励邮件：81，领取后台赠送邮件：84，后台赠送：100，用户签到：101，用户充值：102，提现：103，提现失败退款：104，扣除金币：106）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeGold', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作前金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeGold', @level2type=N'COLUMN',@level2name=N'BeforeGold'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变更金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeGold', @level2type=N'COLUMN',@level2name=N'ChangeGold'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeGold', @level2type=N'COLUMN',@level2name=N'CollectClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeGold', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeGoods', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeGoods', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeGoods', @level2type=N'COLUMN',@level2name=N'GoodsID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeGoods', @level2type=N'COLUMN',@level2name=N'GoodsKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变更类型（1：货币购买 2：礼包赠送  3： 物品兑换  4：系统赠送  5：注册赠送  6：签到奖励  7：  游戏掉落  8： 任务奖励  9：活动奖励  10： 漂流瓶退回  11：升级奖励  12：邮件附件   20： 游戏中使用 21：系统回收  22：投放漂流瓶)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeGoods', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'关联标识（ChangeWay=兑换  关联兑换记录标识）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeGoods', @level2type=N'COLUMN',@level2name=N'RelationID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变更前数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeGoods', @level2type=N'COLUMN',@level2name=N'BeforeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变更数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeGoods', @level2type=N'COLUMN',@level2name=N'ChangeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'客户端IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeGoods', @level2type=N'COLUMN',@level2name=N'CollectClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变更时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeGoods', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeGoods', @level2type=N'COLUMN',@level2name=N'CollectNote'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeIngot', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeIngot', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeIngot', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作类型（1、注册赠送）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeIngot', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作前钻石' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeIngot', @level2type=N'COLUMN',@level2name=N'BeforeIngot'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变化钻石' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeIngot', @level2type=N'COLUMN',@level2name=N'ChangeIngot'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeIngot', @level2type=N'COLUMN',@level2name=N'CollectClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeIngot', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeLoveHeart', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeLoveHeart', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeLoveHeart', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作类型（1、商城购买  2、排位失败消耗  3、排位升级奖励 ）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeLoveHeart', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变化前爱心' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeLoveHeart', @level2type=N'COLUMN',@level2name=N'BeforeLoveHeart'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变化爱心' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeLoveHeart', @level2type=N'COLUMN',@level2name=N'ChangeLoveHeart'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeLoveHeart', @level2type=N'COLUMN',@level2name=N'CollectClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordChangeLoveHeart', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCheckIn', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCheckIn', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCheckIn', @level2type=N'COLUMN',@level2name=N'PresentScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'连续次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCheckIn', @level2type=N'COLUMN',@level2name=N'LxCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'连续金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCheckIn', @level2type=N'COLUMN',@level2name=N'LxScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCheckIn', @level2type=N'COLUMN',@level2name=N'MachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'签到地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCheckIn', @level2type=N'COLUMN',@level2name=N'IPAddress'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'签到时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordCheckIn', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClearFlee', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClearFlee', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClearFlee', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'当前逃率' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClearFlee', @level2type=N'COLUMN',@level2name=N'CurFlee'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'清零方式' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClearFlee', @level2type=N'COLUMN',@level2name=N'ClearType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'清零地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClearFlee', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'清零日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClearFlee', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClearScore', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClearScore', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClearScore', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'当前积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClearScore', @level2type=N'COLUMN',@level2name=N'CurScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'清零方式' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClearScore', @level2type=N'COLUMN',@level2name=N'ClearType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'客户端IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClearScore', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'清零日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordClearScore', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDeductGold', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'扣除数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDeductGold', @level2type=N'COLUMN',@level2name=N'DeductGold'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理员ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDeductGold', @level2type=N'COLUMN',@level2name=N'MasterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作理由' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDeductGold', @level2type=N'COLUMN',@level2name=N'Reason'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDeductGold', @level2type=N'COLUMN',@level2name=N'CollectClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDeductGold', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawCommission', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawCommission', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户昵称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawCommission', @level2type=N'COLUMN',@level2name=N'NickName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'系统抽取服务费' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawCommission', @level2type=N'COLUMN',@level2name=N'ServiceFee'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'提取佣金' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawCommission', @level2type=N'COLUMN',@level2name=N'DrawAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'提取前佣金余额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawCommission', @level2type=N'COLUMN',@level2name=N'BeforeCommission'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawCommission', @level2type=N'COLUMN',@level2name=N'CollectClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawCommission', @level2type=N'COLUMN',@level2name=N'CollectMachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordDrawCommission', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordFeedback', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'反馈人' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordFeedback', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'反馈标题' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordFeedback', @level2type=N'COLUMN',@level2name=N'FeedbackTitle'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'反馈内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordFeedback', @level2type=N'COLUMN',@level2name=N'FeedbackContent'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'反馈日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordFeedback', @level2type=N'COLUMN',@level2name=N'FeedbackDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'反馈地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordFeedback', @level2type=N'COLUMN',@level2name=N'FeedbackIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'浏览次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordFeedback', @level2type=N'COLUMN',@level2name=N'ViewCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'回复状态（0：未回复，1：已回复）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordFeedback', @level2type=N'COLUMN',@level2name=N'RevertStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'回复人' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordFeedback', @level2type=N'COLUMN',@level2name=N'RevertAccounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'回复内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordFeedback', @level2type=N'COLUMN',@level2name=N'RevertContent'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'回复日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordFeedback', @level2type=N'COLUMN',@level2name=N'RevertDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameExpend', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameExpend', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameExpend', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'负分次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameExpend', @level2type=N'COLUMN',@level2name=N'ClearScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'逃率次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameExpend', @level2type=N'COLUMN',@level2name=N'ClearFlee'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'客户端IP地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameExpend', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGameExpend', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantClearScore', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理员ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantClearScore', @level2type=N'COLUMN',@level2name=N'MasterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'客户端IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantClearScore', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收集日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantClearScore', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'玩家标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantClearScore', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantClearScore', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'当前积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantClearScore', @level2type=N'COLUMN',@level2name=N'CurScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'清零负分原因' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantClearScore', @level2type=N'COLUMN',@level2name=N'Reason'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantExperience', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理员标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantExperience', @level2type=N'COLUMN',@level2name=N'MasterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠予地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantExperience', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠予时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantExperience', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantExperience', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'当前经验' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantExperience', @level2type=N'COLUMN',@level2name=N'CurExperience'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'补加经验' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantExperience', @level2type=N'COLUMN',@level2name=N'AddExperience'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠予原因' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantExperience', @level2type=N'COLUMN',@level2name=N'Reason'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantFlee', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantFlee', @level2type=N'COLUMN',@level2name=N'MasterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'服务地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantFlee', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantFlee', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'玩家标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantFlee', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantFlee', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'之前逃率' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantFlee', @level2type=N'COLUMN',@level2name=N'CurFlee'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'累加逃率' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantFlee', @level2type=N'COLUMN',@level2name=N'AddFlee'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'理由' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantFlee', @level2type=N'COLUMN',@level2name=N'Reason'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGameID', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGameID', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'原游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGameID', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠予标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGameID', @level2type=N'COLUMN',@level2name=N'ReGameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'ID级别' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGameID', @level2type=N'COLUMN',@level2name=N'IDLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠予地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGameID', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠予时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGameID', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGold', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGold', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'当前金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGold', @level2type=N'COLUMN',@level2name=N'CurGold'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'增加金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGold', @level2type=N'COLUMN',@level2name=N'AddGold'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理员标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGold', @level2type=N'COLUMN',@level2name=N'MasterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作理由' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGold', @level2type=N'COLUMN',@level2name=N'Reason'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGold', @level2type=N'COLUMN',@level2name=N'CollectClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGold', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGoods', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGoods', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGoods', @level2type=N'COLUMN',@level2name=N'GoodsID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送前数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGoods', @level2type=N'COLUMN',@level2name=N'CurGoodsCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGoods', @level2type=N'COLUMN',@level2name=N'AddGoodsCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理员标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGoods', @level2type=N'COLUMN',@level2name=N'MasterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送原因' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGoods', @level2type=N'COLUMN',@level2name=N'Reason'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGoods', @level2type=N'COLUMN',@level2name=N'CollectClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantGoods', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantIngot', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantIngot', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送前钻石' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantIngot', @level2type=N'COLUMN',@level2name=N'CurIngot'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送钻石' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantIngot', @level2type=N'COLUMN',@level2name=N'AddIngot'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理员标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantIngot', @level2type=N'COLUMN',@level2name=N'MasterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送原因' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantIngot', @level2type=N'COLUMN',@level2name=N'Reason'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantIngot', @level2type=N'COLUMN',@level2name=N'CollectClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantIngot', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantLoveHeart', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantLoveHeart', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'当前爱心' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantLoveHeart', @level2type=N'COLUMN',@level2name=N'CurLoveHeart'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'爱心变更数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantLoveHeart', @level2type=N'COLUMN',@level2name=N'AddLoveHeart'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理员ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantLoveHeart', @level2type=N'COLUMN',@level2name=N'MasterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送原因' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantLoveHeart', @level2type=N'COLUMN',@level2name=N'Reason'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantLoveHeart', @level2type=N'COLUMN',@level2name=N'CollectClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantLoveHeart', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantVip', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantVip', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'当前VIP级别' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantVip', @level2type=N'COLUMN',@level2name=N'CurVipOrder'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送之后VIP级别' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantVip', @level2type=N'COLUMN',@level2name=N'AfterVipOrder'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理员ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantVip', @level2type=N'COLUMN',@level2name=N'MasterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送原因' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantVip', @level2type=N'COLUMN',@level2name=N'Reason'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'客户端IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantVip', @level2type=N'COLUMN',@level2name=N'CollectClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantVip', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordModifyPassword', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordModifyPassword', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'密码类型 1:登陆密码 2:保险柜密码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordModifyPassword', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'旧的密码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordModifyPassword', @level2type=N'COLUMN',@level2name=N'OldPassword'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'新密码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordModifyPassword', @level2type=N'COLUMN',@level2name=N'NewPassword'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修改渠道（1、后台  2、网页  3、手机端  4、H5端  5、电脑端 ）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordModifyPassword', @level2type=N'COLUMN',@level2name=N'ChannelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理员ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordModifyPassword', @level2type=N'COLUMN',@level2name=N'MasterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordModifyPassword', @level2type=N'COLUMN',@level2name=N'CollectClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordModifyPassword', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'礼物标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSendPresent', @level2type=N'COLUMN',@level2name=N'PresentID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'玩家标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSendPresent', @level2type=N'COLUMN',@level2name=N'RcvUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'玩家标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSendPresent', @level2type=N'COLUMN',@level2name=N'SendUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'魅力数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSendPresent', @level2type=N'COLUMN',@level2name=N'LovelinessRcv'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'魅力数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSendPresent', @level2type=N'COLUMN',@level2name=N'LovelinessSend'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'礼物价钱' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSendPresent', @level2type=N'COLUMN',@level2name=N'PresentPrice'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'礼物数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSendPresent', @level2type=N'COLUMN',@level2name=N'PresentCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSendPresent', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSendPresent', @level2type=N'COLUMN',@level2name=N'ServerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSendPresent', @level2type=N'COLUMN',@level2name=N'SendTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'玩家地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSendPresent', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordServiceRemarks', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordServiceRemarks', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordServiceRemarks', @level2type=N'COLUMN',@level2name=N'Remarks'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordServiceRemarks', @level2type=N'COLUMN',@level2name=N'CreateDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建人' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordServiceRemarks', @level2type=N'COLUMN',@level2name=N'CreateBy'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修改次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordServiceRemarks', @level2type=N'COLUMN',@level2name=N'UpdateCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修改时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordServiceRemarks', @level2type=N'COLUMN',@level2name=N'UpdateDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修改人' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordServiceRemarks', @level2type=N'COLUMN',@level2name=N'UpdateBy'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSpreadRewards', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSpreadRewards', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下级用户' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSpreadRewards', @level2type=N'COLUMN',@level2name=N'SubUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'奖励金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSpreadRewards', @level2type=N'COLUMN',@level2name=N'Gold'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'奖励钻石' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSpreadRewards', @level2type=N'COLUMN',@level2name=N'Ingot'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下级机器码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSpreadRewards', @level2type=N'COLUMN',@level2name=N'MachineSerial'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSpreadRewards', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockAddReduce', @level2type=N'COLUMN',@level2name=N'ID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'库存ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockAddReduce', @level2type=N'COLUMN',@level2name=N'StockID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'增加前金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockAddReduce', @level2type=N'COLUMN',@level2name=N'BetAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'  操作类型 1 增加库存 2减少库存 3增加彩金 4.减少彩金' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockAddReduce', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockAddReduce', @level2type=N'COLUMN',@level2name=N'OperationAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作人id' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockAddReduce', @level2type=N'COLUMN',@level2name=N'OperatorUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'库存ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockControl', @level2type=N'COLUMN',@level2name=N'StockID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下注金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockControl', @level2type=N'COLUMN',@level2name=N'BetAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'库存名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockControl', @level2type=N'COLUMN',@level2name=N'StockName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'库存级别（1、下注金额库存  2、游戏库存  3、机组库存   4、活动库存）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockControl', @level2type=N'COLUMN',@level2name=N'StockLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'初始库存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockControl', @level2type=N'COLUMN',@level2name=N'FirstStock'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最终库存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockControl', @level2type=N'COLUMN',@level2name=N'StockScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'库存额外配额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockControl', @level2type=N'COLUMN',@level2name=N'ExtraScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'初始抽水利润' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockControl', @level2type=N'COLUMN',@level2name=N'FirstShrinkProfit'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'抽水利润' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockControl', @level2type=N'COLUMN',@level2name=N'ShrinkProfit'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'抽水配置' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockControl', @level2type=N'COLUMN',@level2name=N'ShrinkOption'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'初始彩金' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockControl', @level2type=N'COLUMN',@level2name=N'FirstGold'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最终彩金' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockControl', @level2type=N'COLUMN',@level2name=N'GoldScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'彩金抽水点数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockControl', @level2type=N'COLUMN',@level2name=N'ShrinkRatio'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'累计加减库存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockControl', @level2type=N'COLUMN',@level2name=N'TotalAddReduceStock'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'累计加减彩金' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockControl', @level2type=N'COLUMN',@level2name=N'TotalAddReduceGold'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'关联库存ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockControl', @level2type=N'COLUMN',@level2name=N'RelationIDList'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockModify', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'库存标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockModify', @level2type=N'COLUMN',@level2name=N'StockID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'当前库存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockModify', @level2type=N'COLUMN',@level2name=N'BeforeStock'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'变化库存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockModify', @level2type=N'COLUMN',@level2name=N'ChangeStock'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理员ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockModify', @level2type=N'COLUMN',@level2name=N'MasterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'客户端IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockModify', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修改备注' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockModify', @level2type=N'COLUMN',@level2name=N'Remark'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordStockModify', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordTurntableLottery', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordTurntableLottery', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户昵称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordTurntableLottery', @level2type=N'COLUMN',@level2name=N'NickName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'命中索引' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordTurntableLottery', @level2type=N'COLUMN',@level2name=N'HitIndex'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'奖品名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordTurntableLottery', @level2type=N'COLUMN',@level2name=N'AwardName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'奖品的物品标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordTurntableLottery', @level2type=N'COLUMN',@level2name=N'GoodsID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'奖品数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordTurntableLottery', @level2type=N'COLUMN',@level2name=N'GoodsCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品等级' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordTurntableLottery', @level2type=N'COLUMN',@level2name=N'GoodsLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'奖品有效期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordTurntableLottery', @level2type=N'COLUMN',@level2name=N'GoodsIndate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordTurntableLottery', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUnderwriteExpend', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUnderwriteExpend', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'个性签名' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUnderwriteExpend', @level2type=N'COLUMN',@level2name=N'ReUnderwrite'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修改地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUnderwriteExpend', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修改日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUnderwriteExpend', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserGift', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserGift', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标用户' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserGift', @level2type=N'COLUMN',@level2name=N'TargetUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'礼物标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserGift', @level2type=N'COLUMN',@level2name=N'GoodsID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'礼物数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserGift', @level2type=N'COLUMN',@level2name=N'GoodsCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'增加魅力值' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserGift', @level2type=N'COLUMN',@level2name=N'LoveLiness'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'发送类型(1:大厅礼物 2:游戏礼物)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserGift', @level2type=N'COLUMN',@level2name=N'SendType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'加载状态（0、未加载  2、已加载）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserGift', @level2type=N'COLUMN',@level2name=N'LoadStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserGift', @level2type=N'COLUMN',@level2name=N'CollectClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserGift', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserManagement', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理员标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserManagement', @level2type=N'COLUMN',@level2name=N'MasterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理员帐号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserManagement', @level2type=N'COLUMN',@level2name=N'MasterAccounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserManagement', @level2type=N'COLUMN',@level2name=N'MasterIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserManagement', @level2type=N'COLUMN',@level2name=N'MasterDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'玩家标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserManagement', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserManagement', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'玩家帐号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserManagement', @level2type=N'COLUMN',@level2name=N'UserAccounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'执行动作' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserManagement', @level2type=N'COLUMN',@level2name=N'Action'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作原因' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserManagement', @level2type=N'COLUMN',@level2name=N'Reason'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'红包验证码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserRedPack', @level2type=N'COLUMN',@level2name=N'CodeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'红包金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserRedPack', @level2type=N'COLUMN',@level2name=N'Amount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'红包状态（0 未领取  1 已领取）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserRedPack', @level2type=N'COLUMN',@level2name=N'StatusID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'红包有效期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserRedPack', @level2type=N'COLUMN',@level2name=N'OverTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'领取用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserRedPack', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'领取时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserRedPack', @level2type=N'COLUMN',@level2name=N'RecevieDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'领取地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordUserRedPack', @level2type=N'COLUMN',@level2name=N'RecevieIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'录像号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordVideoInfo', @level2type=N'COLUMN',@level2name=N'VideoNumber'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordVideoInfo', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'约战房号（邀请码）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordVideoInfo', @level2type=N'COLUMN',@level2name=N'MappedNum'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'约战记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordVideoInfo', @level2type=N'COLUMN',@level2name=N'BattleRecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordVideoInfo', @level2type=N'COLUMN',@level2name=N'VideoCreateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'玩家人数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordVideoInfo', @level2type=N'COLUMN',@level2name=N'UserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'回放码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordVideoInfo', @level2type=N'COLUMN',@level2name=N'PlayBackCode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordVideoInfo', @level2type=N'COLUMN',@level2name=N'RecordDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'录像号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordVideoUser', @level2type=N'COLUMN',@level2name=N'VideoNumber'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordVideoUser', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'约战房号（邀请码）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordVideoUser', @level2type=N'COLUMN',@level2name=N'MappedNum'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordVideoUser', @level2type=N'COLUMN',@level2name=N'RecordDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordWealthTransfer', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordWealthTransfer', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordWealthTransfer', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户昵称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordWealthTransfer', @level2type=N'COLUMN',@level2name=N'NickName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordWealthTransfer', @level2type=N'COLUMN',@level2name=N'TargetUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标用户游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordWealthTransfer', @level2type=N'COLUMN',@level2name=N'TargetGameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'目标用户昵称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordWealthTransfer', @level2type=N'COLUMN',@level2name=N'TargetNickName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'服务费用' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordWealthTransfer', @level2type=N'COLUMN',@level2name=N'ServiceFee'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'转账金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordWealthTransfer', @level2type=N'COLUMN',@level2name=N'TransferAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'客户端IP地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordWealthTransfer', @level2type=N'COLUMN',@level2name=N'CollectClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordWealthTransfer', @level2type=N'COLUMN',@level2name=N'CollectMachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordWealthTransfer', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'UserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值用户总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'PayUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值两次人数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'PayTwiceUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日登录用户数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodayLoginUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日注册用户数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodayRegUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日新增VIP总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodayNewVipCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日充值用户总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodayPayUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日最高支付金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodayPayMaxAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日活跃用户数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodayDayActiveUser'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏玩家数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodayPlayCommonUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'VIP玩家数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodayPlayVipUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建好友房次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodayBattleCreateRoomCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总在线时长' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodaySumOnlineTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'1日留存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'OneDayRemainRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'2日留存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TwoDayRemainRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'3日留存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'ThreeDayRemainRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'4日留存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'FourDayRemainRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'5日留存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'FiveDayRemainRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'7日留存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'ServenDayRemainRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'15日留存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'FiftenDayRemainRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'30日留存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'ThirtyDayRemainRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'报表时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'ReportDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日注册且充值用户总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodayRegPayUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'参与过游戏人数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TotalPlayers'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收集时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日推广佣金' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodaySpreadCommission'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日提现金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodayGoldWithdrawAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日新客支付金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodayNewPayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日新客支付人数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodayNewPayUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日新客登录人数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodayNewLoginUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日新客充值次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodayNewPayCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'OldTodayPayUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodayPayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日提现系统手续费' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodayGoldWithdrawSystemFee'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日提现渠道手续费' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodayGoldWithdrawChannelFee'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日提现人数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDay', @level2type=N'COLUMN',@level2name=N'TodayGoldWithdrawUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayCommission', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayCommission', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'玩家佣金余额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayCommission', @level2type=N'COLUMN',@level2name=N'Commission'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总支付金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayCommission', @level2type=N'COLUMN',@level2name=N'TotalPayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总押注金额 ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayCommission', @level2type=N'COLUMN',@level2name=N'TotalBetAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总充值佣金' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayCommission', @level2type=N'COLUMN',@level2name=N'TotalPayContribute'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总押注佣金' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayCommission', @level2type=N'COLUMN',@level2name=N'TotalBetContribute'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下级充值总金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayCommission', @level2type=N'COLUMN',@level2name=N'LowerPayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下级游戏获利总金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayCommission', @level2type=N'COLUMN',@level2name=N'LowerBetAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下下级充值总金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayCommission', @level2type=N'COLUMN',@level2name=N'LLowerPayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下下级游戏获利总金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayCommission', @level2type=N'COLUMN',@level2name=N'LLowerBetAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下级用户首充贡献' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayCommission', @level2type=N'COLUMN',@level2name=N'LowerPayContribute'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下级用户游戏获利贡献' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayCommission', @level2type=N'COLUMN',@level2name=N'LowerBetContribute'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下下级用户首充贡献' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayCommission', @level2type=N'COLUMN',@level2name=N'LLowerPayContribute'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下下级游戏获利贡献' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayCommission', @level2type=N'COLUMN',@level2name=N'LLowerBetContribute'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'包ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'MarketID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'UserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值用户总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'PayUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值两次人数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'PayTwiceUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayPayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日支付订单数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayPayCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日登录用户数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayLoginUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日注册用户数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayRegUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日新增VIP总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayNewVipCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日充值用户总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayPayUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日最高支付金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayPayMaxAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日推广佣金' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodaySpreadCommission'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日提现金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayGoldWithdrawAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日提现系统手续费' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayGoldWithdrawSystemFee'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日提现渠道手续费' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayGoldWithdrawChannelFee'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日提现人数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayGoldWithdrawUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日活跃用户数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayDayActiveUser'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏玩家数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayPlayCommonUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'VIP玩家数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayPlayVipUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建好友房次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayBattleCreateRoomCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总在线时长' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodaySumOnlineTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日新客支付金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayNewPayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日新客支付人数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayNewPayUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日新客登录人数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayNewLoginUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日新客充值次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayNewPayCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'1日留存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'OneDayRemainRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'2日留存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TwoDayRemainRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'3日留存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'ThreeDayRemainRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'4日留存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'FourDayRemainRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'5日留存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'FiveDayRemainRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'7日留存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'ServenDayRemainRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'15日留存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'FiftenDayRemainRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'30日留存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'ThirtyDayRemainRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'报表时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'ReportDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日注册且充值用户总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayRegPayUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'参与过游戏人数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TotalPlayers'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收集时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总充值金额（包括失败情况）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayAllPayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日总支付订单数（包括失败情况）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayAllPayCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日总充值用户总数（包括失败情况）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayAllPayUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日新客总支付金额（包括失败情况）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayNewAllPayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日总新客支付人数（包括失败情况）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayNewAllPayUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日新客总充值次数（包括失败情况）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayNewAllPayCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日注册机器数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayRegMachineCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日注册且充值机器总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayRegPayMachineCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日登录机器数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayLoginMachineCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日新客首充金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayNewFirstPayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日新客二次充值人数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayNewTwoPayCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日新客二次充值金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayMarket', @level2type=N'COLUMN',@level2name=N'TodayNewTwoPayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayUser', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayUser', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值笔数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayUser', @level2type=N'COLUMN',@level2name=N'PayCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayUser', @level2type=N'COLUMN',@level2name=N'PayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后充值金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayUser', @level2type=N'COLUMN',@level2name=N'LastPayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后充值时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayUser', @level2type=N'COLUMN',@level2name=N'LastPayDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'获取经验值' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayUser', @level2type=N'COLUMN',@level2name=N'Experience'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'获取魅力值' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayUser', @level2type=N'COLUMN',@level2name=N'Loveliness'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广链接点击数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayUser', @level2type=N'COLUMN',@level2name=N'SpreadViewCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后报告时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayUser', @level2type=N'COLUMN',@level2name=N'ReportDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广用户' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayUser', @level2type=N'COLUMN',@level2name=N'SpreadLowerCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广下下级数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayUser', @level2type=N'COLUMN',@level2name=N'SpreadLLowerCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportGameData', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportGameData', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportGameData', @level2type=N'COLUMN',@level2name=N'GameNumber'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'服务费用' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportGameData', @level2type=N'COLUMN',@level2name=N'UserService'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户税收' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportGameData', @level2type=N'COLUMN',@level2name=N'UserRevenue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏时长' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportGameData', @level2type=N'COLUMN',@level2name=N'PlayTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'统计时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportGameData', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'参加游戏玩家数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportGameData', @level2type=N'COLUMN',@level2name=N'PlayUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户下注金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportGameData', @level2type=N'COLUMN',@level2name=N'UserBetAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportGoods', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportGoods', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportGoods', @level2type=N'COLUMN',@level2name=N'GoodsID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportGoods', @level2type=N'COLUMN',@level2name=N'GoodsCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'统计时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportGoods', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportGoodsOpCn', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportGoodsOpCn', @level2type=N'COLUMN',@level2name=N'GoodsID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'发产出或者消耗的变化类型ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportGoodsOpCn', @level2type=N'COLUMN',@level2name=N'ChangeTypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'产出数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportGoodsOpCn', @level2type=N'COLUMN',@level2name=N'OutputCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'消耗数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportGoodsOpCn', @level2type=N'COLUMN',@level2name=N'ConsumeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportGoodsOpCn', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportStock', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'库存标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportStock', @level2type=N'COLUMN',@level2name=N'StockID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'开始库存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportStock', @level2type=N'COLUMN',@level2name=N'StockStart'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'结束库存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportStock', @level2type=N'COLUMN',@level2name=N'StockChange'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportStock', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportTerminal', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'终端标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportTerminal', @level2type=N'COLUMN',@level2name=N'TerminalID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportTerminal', @level2type=N'COLUMN',@level2name=N'UserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserLTV', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'包ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserLTV', @level2type=N'COLUMN',@level2name=N'MarketID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserLTV', @level2type=N'COLUMN',@level2name=N'UserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserValue', @level2type=N'COLUMN',@level2name=N'MarketID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'新增用户数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportUserValue', @level2type=N'COLUMN',@level2name=N'UserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportVip', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'会员订单' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportVip', @level2type=N'COLUMN',@level2name=N'VipOrder'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'玩家人数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportVip', @level2type=N'COLUMN',@level2name=N'UserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'新增会员总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportVip', @level2type=N'COLUMN',@level2name=N'NewUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamGameDownInfo', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawUserLTV', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'包ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawUserLTV', @level2type=N'COLUMN',@level2name=N'MarketID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawUserLTV', @level2type=N'COLUMN',@level2name=N'UserCount'
GO
