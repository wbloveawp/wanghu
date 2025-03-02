USE [LHInsureDB]
GO
/****** Object:  Table [dbo].[AccountsBank]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsBank](
	[UserID] [int] NOT NULL,
	[WithdrawChannelID] [int] NOT NULL,
	[PayChannelID] [int] NOT NULL,
	[Name] [nvarchar](64) NOT NULL,
	[BankCode] [nvarchar](64) NOT NULL,
	[BankName] [nvarchar](64) NOT NULL,
	[BankNo] [nvarchar](64) NOT NULL,
	[MobileNo] [nvarchar](64) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
	[PixNo] [nvarchar](64) NOT NULL,
	[PayEamil] [nvarchar](128) NOT NULL,
	[PixType] [tinyint] NOT NULL,
	[Cpf] [nvarchar](64) NOT NULL,
	[PayMobileNo] [nvarchar](128) NOT NULL,
	[PayCpf] [nvarchar](128) NOT NULL,
 CONSTRAINT [PK_AccountsBank] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsPay]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsPay](
	[UserID] [int] NOT NULL,
	[PayCount] [int] NOT NULL,
	[PayAmount] [decimal](18, 2) NOT NULL,
	[LastPayAmount] [decimal](18, 2) NOT NULL,
	[LastPayDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_AccountsPay_UserID] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsWithdraw]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsWithdraw](
	[UserID] [int] NOT NULL,
	[ResetBetAmount] [bigint] NOT NULL,
	[ResetGrantAmount] [bigint] NOT NULL,
	[WithdrawNeedBetAmount] [bigint] NOT NULL,
	[ModifyTime] [datetime] NOT NULL,
	[ResetType] [tinyint] NOT NULL,
 CONSTRAINT [PK_AccountsWithdraw] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[BillChannel]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[BillChannel](
	[MonthID] [int] NOT NULL,
	[ChannelID] [int] NOT NULL,
	[ChannelName] [nvarchar](32) NOT NULL,
	[ChannelRate] [decimal](18, 3) NOT NULL,
	[PlatformRate] [decimal](18, 3) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_BillChannel] PRIMARY KEY CLUSTERED 
(
	[MonthID] ASC,
	[ChannelID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[BillIncomeExpenditure]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[BillIncomeExpenditure](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[MonthID] [int] NOT NULL,
	[TypeID] [int] NOT NULL,
	[RecordName] [nvarchar](32) NOT NULL,
	[Amount] [decimal](18, 2) NOT NULL,
	[Remark] [nvarchar](256) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_AccountIncomeExpenditure] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[BillRecordMonth]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[BillRecordMonth](
	[MonthID] [int] NOT NULL,
	[BillStatus] [tinyint] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_BillRecordMonth] PRIMARY KEY CLUSTERED 
(
	[MonthID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ChannelConfig]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ChannelConfig](
	[ChannelID] [int] NOT NULL,
	[ChannelName] [nvarchar](32) NOT NULL,
	[ChannelRate] [decimal](18, 3) NOT NULL,
	[PlatformID] [int] NOT NULL,
	[PlatformRate] [decimal](18, 3) NOT NULL,
	[SpreadChannelID] [int] NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[MerchantID] [nvarchar](512) NULL,
	[MerchantAppID] [nvarchar](512) NULL,
	[MerchantPublicKey] [nvarchar](max) NULL,
	[MerchantPrivateKey] [nvarchar](max) NULL,
	[ExtendInfo] [nvarchar](512) NULL,
	[WeightValue] [int] NOT NULL,
	[DayLimitAmount] [int] NOT NULL,
 CONSTRAINT [PK_ChannelConfig] PRIMARY KEY CLUSTERED 
(
	[ChannelID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ChannelWithdrawConfig]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ChannelWithdrawConfig](
	[ChannelID] [int] NOT NULL,
	[ChannelName] [nvarchar](32) NOT NULL,
	[ChannelRate] [decimal](18, 3) NOT NULL,
	[FiexdFee] [decimal](18, 2) NOT NULL,
	[CheckSwitch] [int] NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[SpreadChannelID] [int] NOT NULL,
	[WeightValue] [int] NOT NULL,
 CONSTRAINT [PK_ChannelWithdrawConfig] PRIMARY KEY CLUSTERED 
(
	[ChannelID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[EMallCommodity]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[EMallCommodity](
	[CommodityID] [int] NOT NULL,
	[CommodityName] [nvarchar](32) NOT NULL,
	[CommodityTypeID] [int] NOT NULL,
	[KindID] [tinyint] NOT NULL,
	[GoodsID] [int] NOT NULL,
	[GoodsCount] [int] NOT NULL,
	[GoodsIndate] [int] NOT NULL,
	[Price] [int] NOT NULL,
	[MoneyID] [int] NOT NULL,
	[RecommendStatus] [tinyint] NOT NULL,
	[CommodityStatus] [tinyint] NOT NULL,
	[FillAddressStatus] [tinyint] NOT NULL,
	[VipLimitMin] [tinyint] NOT NULL,
	[Inventory] [int] NOT NULL,
	[SoldCount] [int] NOT NULL,
	[SortID] [int] NOT NULL,
	[Describe] [nvarchar](max) NOT NULL,
	[ModifyDateTime] [datetime] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_MallCommodity] PRIMARY KEY CLUSTERED 
(
	[CommodityID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[EMallCommodityType]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[EMallCommodityType](
	[CommodityTypeID] [int] IDENTITY(1,1) NOT NULL,
	[TypeName] [nvarchar](32) NOT NULL,
	[SortID] [int] NOT NULL,
	[AllowDeleteStatus] [tinyint] NOT NULL,
	[Describe] [nvarchar](32) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
	[Nullity] [tinyint] NOT NULL,
 CONSTRAINT [PK_MallCommodityType] PRIMARY KEY CLUSTERED 
(
	[CommodityTypeID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[EMallOrder]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[EMallOrder](
	[OrderID] [int] IDENTITY(100000,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[Accounts] [nvarchar](32) NOT NULL,
	[CommodityID] [int] NOT NULL,
	[CommodityName] [nvarchar](32) NOT NULL,
	[CommodityTypeID] [int] NOT NULL,
	[CommodityCount] [int] NOT NULL,
	[Price] [int] NOT NULL,
	[OrderAmount] [int] NOT NULL,
	[GoodsID] [int] NOT NULL,
	[GoodsCount] [int] NOT NULL,
	[OrderStatus] [tinyint] NOT NULL,
	[Mobile] [nvarchar](16) NULL,
	[Consignee] [nvarchar](32) NULL,
	[Province] [nvarchar](32) NULL,
	[City] [nvarchar](32) NULL,
	[Area] [nvarchar](32) NULL,
	[DetailedAddress] [nvarchar](128) NULL,
	[Remark] [nvarchar](64) NULL,
	[DeliveryDateTime] [datetime] NULL,
	[CollectDateTime] [datetime] NOT NULL,
	[PayAccounts] [nvarchar](32) NULL,
	[RefundStatus] [tinyint] NOT NULL,
	[RefundDescribe] [nvarchar](64) NOT NULL,
 CONSTRAINT [PK_MallOrder] PRIMARY KEY CLUSTERED 
(
	[OrderID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GiftbagCode]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GiftbagCode](
	[GiftbagCodeID] [nvarchar](16) NOT NULL,
	[Count] [int] NOT NULL,
	[UsedCount] [int] NOT NULL,
	[LimitNum] [int] NOT NULL,
	[PlatformID] [int] NOT NULL,
	[Status] [tinyint] NOT NULL,
	[StartTime] [datetime] NOT NULL,
	[ExpiresTime] [datetime] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_GiftbagCode] PRIMARY KEY CLUSTERED 
(
	[GiftbagCodeID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GiftbagCodeExGoods]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GiftbagCodeExGoods](
	[RecordID] [int] NOT NULL,
	[GoodsID] [bigint] NOT NULL,
	[GoodsNumber] [int] NOT NULL,
	[Indate] [int] NOT NULL,
 CONSTRAINT [PK_GiftbagCodeExGoods] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC,
	[GoodsID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GiftbagCodeExRecord]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GiftbagCodeExRecord](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[GiftbagCodeID] [nvarchar](16) NOT NULL,
	[UserID] [int] NOT NULL,
	[Accounts] [nvarchar](32) NOT NULL,
	[GoodsArrivalStatus] [int] NOT NULL,
	[GoodsArrivalTime] [datetime] NULL,
	[ClientIP] [nvarchar](15) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_GiftbagCodeRecord] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GiftbagCodeGoods]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GiftbagCodeGoods](
	[GiftbagCodeID] [nvarchar](16) NOT NULL,
	[GoodsID] [bigint] NOT NULL,
	[BindState] [tinyint] NOT NULL,
	[GoodsNumber] [bigint] NOT NULL,
	[Indate] [int] NOT NULL,
 CONSTRAINT [PK_GiftbagCodeGoods] PRIMARY KEY CLUSTERED 
(
	[GiftbagCodeID] ASC,
	[GoodsID] ASC,
	[BindState] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GiftbagConfig]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GiftbagConfig](
	[GiftbagID] [int] NOT NULL,
	[GiftbagName] [nvarchar](32) NOT NULL,
	[OriginalPrice] [int] NOT NULL,
	[DiscountedPrice] [int] NOT NULL,
	[ShowArea] [int] NOT NULL,
	[Kind] [tinyint] NOT NULL,
	[SortID] [int] NOT NULL,
	[NumberLimit] [tinyint] NOT NULL,
	[VipLimitMin] [tinyint] NOT NULL,
	[VipLimitMax] [tinyint] NOT NULL,
	[CannonTimesLimitMin] [int] NOT NULL,
	[CannonTimesLimitMax] [int] NOT NULL,
	[ContinuousPayInterval] [int] NOT NULL,
	[GoldDoubleRule] [nvarchar](max) NOT NULL,
	[PurchaseID] [nvarchar](128) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_BuyGiftbagConfig] PRIMARY KEY CLUSTERED 
(
	[GiftbagID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GiftbagGoods]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GiftbagGoods](
	[GiftbagID] [int] NOT NULL,
	[GroupIndex] [int] NOT NULL,
	[GoodsID] [int] NOT NULL,
	[GoodsNumber] [bigint] NOT NULL,
	[Indate] [int] NOT NULL,
 CONSTRAINT [PK_GiftbagGoods] PRIMARY KEY CLUSTERED 
(
	[GiftbagID] ASC,
	[GroupIndex] ASC,
	[GoodsID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GlobalAppInfo]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GlobalAppInfo](
	[ProductID] [int] IDENTITY(1,1) NOT NULL,
	[StationID] [int] NOT NULL,
	[Price] [int] NOT NULL,
	[PresentCount] [bigint] NOT NULL,
	[ExtraCount] [bigint] NOT NULL,
	[IsTop] [bit] NOT NULL,
	[SortID] [int] NOT NULL,
	[Nullity] [tinyint] NOT NULL,
 CONSTRAINT [PK_GlobalAppInfo_1] PRIMARY KEY CLUSTERED 
(
	[ProductID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GlobalIngotInfo]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GlobalIngotInfo](
	[IngotID] [int] IDENTITY(1,1) NOT NULL,
	[ProductName] [nvarchar](100) NOT NULL,
	[Description] [nvarchar](100) NOT NULL,
	[Price] [bigint] NOT NULL,
	[PresentScore] [bigint] NOT NULL,
	[AttachScore] [bigint] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_GlobalIngotInfo] PRIMARY KEY CLUSTERED 
(
	[IngotID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[MallGoods]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[MallGoods](
	[MallgoodsID] [int] NOT NULL,
	[MallgoodsName] [nvarchar](32) NOT NULL,
	[KindID] [tinyint] NOT NULL,
	[OriginalPrice] [decimal](18, 2) NOT NULL,
	[DiscountedPrice] [decimal](18, 2) NOT NULL,
	[MoneyID] [int] NOT NULL,
	[NumberLimit] [tinyint] NOT NULL,
	[GoodsInfo] [nvarchar](max) NOT NULL,
	[SortID] [int] NOT NULL,
	[Label] [nvarchar](16) NULL,
	[DoubleStatus] [tinyint] NOT NULL,
	[RecommendStatus] [tinyint] NOT NULL,
	[VipLimitMin] [tinyint] NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
	[PurchaseID] [nvarchar](128) NOT NULL,
	[DisDailyInventory] [int] NOT NULL,
	[DisEachDailyLimit] [int] NOT NULL,
	[DisDailyRemainInventory] [int] NOT NULL,
 CONSTRAINT [PK_GoodsBuyConfig] PRIMARY KEY CLUSTERED 
(
	[MallgoodsID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[MallGoodsBuyStat]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[MallGoodsBuyStat](
	[UserID] [int] NOT NULL,
	[MallgoodsID] [int] NOT NULL,
	[MallgoodsBuyCount] [int] NOT NULL,
	[SumAmount] [decimal](18, 2) NOT NULL,
	[LastBuyDateTime] [datetime] NOT NULL,
	[TodayBuyCount] [int] NOT NULL,
 CONSTRAINT [PK_MallGoodsBuyStat] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC,
	[MallgoodsID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[MallGoodsPresent]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[MallGoodsPresent](
	[MallgoodsID] [int] NOT NULL,
	[VipLevel] [tinyint] NOT NULL,
	[PresentRate] [decimal](18, 2) NOT NULL,
 CONSTRAINT [PK_GoodsBuyPresent] PRIMARY KEY CLUSTERED 
(
	[MallgoodsID] ASC,
	[VipLevel] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Notice]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Notice](
	[NoticeID] [int] IDENTITY(1,1) NOT NULL,
	[TypeID] [int] NOT NULL,
	[ContentType] [int] NOT NULL,
	[NoticeTitle] [nvarchar](200) NOT NULL,
	[NoticeContent] [ntext] NOT NULL,
	[ViewPlace] [int] NOT NULL,
	[StartDateTime] [datetime] NOT NULL,
	[EndDateTime] [datetime] NOT NULL,
	[TopStatus] [tinyint] NOT NULL,
	[ShowPlatformID] [nvarchar](max) NOT NULL,
	[ExclusiveStatus] [tinyint] NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[ModifyDateTime] [datetime] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_Notice] PRIMARY KEY CLUSTERED 
(
	[NoticeID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[PayOrder]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[PayOrder](
	[OrderID] [bigint] NOT NULL,
	[OrderKind] [tinyint] NOT NULL,
	[OrderAmount] [decimal](18, 2) NOT NULL,
	[PayAmount] [decimal](18, 2) NOT NULL,
	[UserID] [int] NOT NULL,
	[Accounts] [nvarchar](32) NOT NULL,
	[OrderState] [tinyint] NOT NULL,
	[ChannelID] [int] NOT NULL,
	[ChannelRate] [decimal](18, 3) NOT NULL,
	[ChannelAmount] [decimal](18, 3) NOT NULL,
	[PlatformID] [int] NOT NULL,
	[PlatformRate] [decimal](18, 3) NOT NULL,
	[PlatformAmount] [decimal](18, 3) NOT NULL,
	[CommodityID] [int] NOT NULL,
	[CommodityName] [nvarchar](32) NOT NULL,
	[CompanyAmount] [decimal](18, 3) NOT NULL,
	[DoubleStatus] [tinyint] NOT NULL,
	[GiftbagKind] [tinyint] NOT NULL,
	[GiftbagExtraGoldMultiple] [int] NOT NULL,
	[GoodsArrivalStatus] [tinyint] NOT NULL,
	[PayDateTime] [datetime] NULL,
	[CollectDateTime] [datetime] NOT NULL,
	[CreateClientIP] [nvarchar](15) NULL,
	[CallbackClientIP] [nvarchar](15) NULL,
	[IpayOrderID] [nvarchar](64) NULL,
	[OrderStatusChangeWay] [tinyint] NOT NULL,
	[SpreadChannelID] [int] NOT NULL,
	[MarketID] [int] NOT NULL,
	[SceneID] [int] NOT NULL,
 CONSTRAINT [PK_PayOrder] PRIMARY KEY CLUSTERED 
(
	[OrderID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[PayOrderGoods]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[PayOrderGoods](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[OrderID] [bigint] NOT NULL,
	[GoodsID] [int] NOT NULL,
	[GoodsNumber] [bigint] NOT NULL,
	[Indate] [int] NOT NULL,
	[AwardIdent] [tinyint] NOT NULL,
	[ExtendProperty] [nvarchar](max) NOT NULL,
 CONSTRAINT [PK_PayOrderGoods] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[PayRecord]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[PayRecord](
	[OrderID] [bigint] NOT NULL,
	[OrderKind] [tinyint] NOT NULL,
	[OrderAmount] [decimal](18, 2) NOT NULL,
	[PayAmount] [decimal](18, 2) NOT NULL,
	[UserID] [int] NOT NULL,
	[Accounts] [nvarchar](32) NOT NULL,
	[ChannelID] [int] NOT NULL,
	[ChannelRate] [decimal](18, 3) NOT NULL,
	[ChannelAmount] [decimal](18, 3) NOT NULL,
	[PlatformID] [int] NOT NULL,
	[PlatformRate] [decimal](18, 3) NOT NULL,
	[PlatformAmount] [decimal](18, 3) NOT NULL,
	[CommodityID] [int] NOT NULL,
	[CommodityName] [nvarchar](32) NOT NULL,
	[CompanyAmount] [decimal](18, 2) NOT NULL,
	[GoodsArrivalStatus] [tinyint] NOT NULL,
	[DoubleStatus] [tinyint] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
	[PayDateTime] [datetime] NOT NULL,
	[CreateClientIP] [nvarchar](16) NULL,
	[CallbackClientIP] [nvarchar](16) NULL,
	[IpayOrderID] [nvarchar](64) NULL,
	[OrderStatusChangeWay] [tinyint] NOT NULL,
	[SpreadChannelID] [int] NOT NULL,
	[SpreadChannelRate] [decimal](18, 3) NOT NULL,
	[SpreadChannelAmount] [decimal](18, 2) NOT NULL,
	[SpreadUserID] [int] NOT NULL,
	[SpreadRate] [decimal](18, 3) NOT NULL,
	[SpreadDivideAmount] [decimal](18, 2) NOT NULL,
	[SpreadDivideScore] [bigint] NOT NULL,
	[MarketID] [int] NOT NULL,
	[SceneID] [int] NOT NULL,
 CONSTRAINT [PK_PayRecord] PRIMARY KEY CLUSTERED 
(
	[OrderID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReportDayCommodityPay]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReportDayCommodityPay](
	[DateID] [int] NOT NULL,
	[CommodityID] [int] NOT NULL,
	[OrderKind] [int] NOT NULL,
	[PaySumAmount] [decimal](18, 2) NOT NULL,
	[PayCount] [int] NOT NULL,
	[LastPayDateTime] [datetime] NOT NULL,
	[LastPayAmount] [decimal](18, 2) NOT NULL,
 CONSTRAINT [PK_ReportDayCommodityPay] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[CommodityID] ASC,
	[OrderKind] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReportDayPay]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReportDayPay](
	[DateID] [int] NOT NULL,
	[ChannelID] [int] NOT NULL,
	[PlatformID] [int] NOT NULL,
	[PayAmount] [decimal](18, 2) NOT NULL,
	[PayCount] [int] NOT NULL,
	[PayUserCount] [int] NOT NULL,
	[ChannelAmount] [decimal](18, 3) NULL,
	[PlatformAmount] [decimal](18, 3) NULL,
	[StatDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_ReportDayPay_1] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[ChannelID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[WithdrawOrder]    Script Date: 2023/7/12 11:11:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[WithdrawOrder](
	[OrderID] [bigint] NOT NULL,
	[UserID] [int] NOT NULL,
	[ChannelID] [int] NOT NULL,
	[ChannelFee] [decimal](18, 2) NOT NULL,
	[ApplyType] [tinyint] NOT NULL,
	[ApplyAmount] [decimal](18, 2) NOT NULL,
	[TransferType] [tinyint] NOT NULL,
	[TransferAmount] [decimal](18, 2) NOT NULL,
	[BeforeScore] [bigint] NOT NULL,
	[Name] [nvarchar](64) NOT NULL,
	[BankCode] [nvarchar](64) NOT NULL,
	[BankName] [nvarchar](64) NOT NULL,
	[BankNo] [nvarchar](64) NOT NULL,
	[MobileNo] [nvarchar](64) NOT NULL,
	[CheckKind] [tinyint] NOT NULL,
	[CheckDateTime] [datetime] NULL,
	[OrderState] [tinyint] NOT NULL,
	[ClientIP] [nvarchar](15) NOT NULL,
	[ChannelOrderID] [nvarchar](32) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
	[PayDateTime] [datetime] NULL,
	[CollectNote] [nvarchar](512) NOT NULL,
	[SystemFee] [decimal](18, 2) NOT NULL,
	[PixNo] [nvarchar](64) NOT NULL,
	[PixType] [tinyint] NOT NULL,
	[Cpf] [nvarchar](64) NOT NULL,
	[ConsumeBetAmount] [bigint] NOT NULL,
	[ManualAuditReason] [nvarchar](256) NOT NULL,
	[TransferFailReason] [nvarchar](128) NOT NULL,
	[MarketID] [int] NOT NULL,
	[SpreadChannelID] [int] NOT NULL,
 CONSTRAINT [PK_WithdrawOrder] PRIMARY KEY CLUSTERED 
(
	[OrderID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
ALTER TABLE [dbo].[AccountsBank] ADD  CONSTRAINT [DF_AccountsBank_WithdrawChannelID]  DEFAULT ((0)) FOR [WithdrawChannelID]
GO
ALTER TABLE [dbo].[AccountsBank] ADD  CONSTRAINT [DF_AccountsBank_PayChannelID]  DEFAULT ((0)) FOR [PayChannelID]
GO
ALTER TABLE [dbo].[AccountsBank] ADD  CONSTRAINT [DF_AccountsBank_Name]  DEFAULT ('') FOR [Name]
GO
ALTER TABLE [dbo].[AccountsBank] ADD  CONSTRAINT [DF_AccountsBank_BankName]  DEFAULT ('') FOR [BankCode]
GO
ALTER TABLE [dbo].[AccountsBank] ADD  CONSTRAINT [DF_AccountsBank_BankName_1]  DEFAULT ('') FOR [BankName]
GO
ALTER TABLE [dbo].[AccountsBank] ADD  CONSTRAINT [DF_AccountsBank_BankCode]  DEFAULT ('') FOR [BankNo]
GO
ALTER TABLE [dbo].[AccountsBank] ADD  CONSTRAINT [DF_AccountsBank_Mobile]  DEFAULT ('') FOR [MobileNo]
GO
ALTER TABLE [dbo].[AccountsBank] ADD  CONSTRAINT [DF_AccountsBank_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[AccountsBank] ADD  CONSTRAINT [DF_AccountsBank_PixNo]  DEFAULT ('') FOR [PixNo]
GO
ALTER TABLE [dbo].[AccountsBank] ADD  CONSTRAINT [DF_AccountsBank_PayEamil]  DEFAULT ('') FOR [PayEamil]
GO
ALTER TABLE [dbo].[AccountsBank] ADD  CONSTRAINT [DF_AccountsBank_PixType]  DEFAULT ((1)) FOR [PixType]
GO
ALTER TABLE [dbo].[AccountsBank] ADD  CONSTRAINT [DF_AccountsBank_Cpf]  DEFAULT ('') FOR [Cpf]
GO
ALTER TABLE [dbo].[AccountsBank] ADD  CONSTRAINT [DF_AccountsBank_PayMobileNo]  DEFAULT ('') FOR [PayMobileNo]
GO
ALTER TABLE [dbo].[AccountsBank] ADD  CONSTRAINT [DF_AccountsBank_PayCpf]  DEFAULT ('') FOR [PayCpf]
GO
ALTER TABLE [dbo].[AccountsPay] ADD  CONSTRAINT [DF_AccountsPay_PayCurrency]  DEFAULT ((0)) FOR [PayCount]
GO
ALTER TABLE [dbo].[AccountsPay] ADD  CONSTRAINT [DF_AccountsPay_Currency]  DEFAULT ((0)) FOR [PayAmount]
GO
ALTER TABLE [dbo].[AccountsPay] ADD  CONSTRAINT [DF_AccountsPay_LastPayAmount]  DEFAULT ((0)) FOR [LastPayAmount]
GO
ALTER TABLE [dbo].[AccountsPay] ADD  CONSTRAINT [DF_AccountsPay_LastPayDateTime]  DEFAULT (getdate()) FOR [LastPayDateTime]
GO
ALTER TABLE [dbo].[AccountsWithdraw] ADD  CONSTRAINT [DF_AccountsWithdraw_ResetBetAmount]  DEFAULT ((0)) FOR [ResetBetAmount]
GO
ALTER TABLE [dbo].[AccountsWithdraw] ADD  CONSTRAINT [DF_AccountsWithdraw_ResetGrantAmount]  DEFAULT ((0)) FOR [ResetGrantAmount]
GO
ALTER TABLE [dbo].[AccountsWithdraw] ADD  CONSTRAINT [DF_AccountsWithdraw_WithdrawNeedBetAmount]  DEFAULT ((0)) FOR [WithdrawNeedBetAmount]
GO
ALTER TABLE [dbo].[AccountsWithdraw] ADD  CONSTRAINT [DF_AccountsWithdraw_ModifyTime]  DEFAULT (getdate()) FOR [ModifyTime]
GO
ALTER TABLE [dbo].[AccountsWithdraw] ADD  CONSTRAINT [DF_AccountsWithdraw_ResetType]  DEFAULT ((0)) FOR [ResetType]
GO
ALTER TABLE [dbo].[BillChannel] ADD  CONSTRAINT [DF_BillChannel_ChannelName]  DEFAULT ('') FOR [ChannelName]
GO
ALTER TABLE [dbo].[BillChannel] ADD  CONSTRAINT [DF_BillChannel_ChannelRate]  DEFAULT ((0)) FOR [ChannelRate]
GO
ALTER TABLE [dbo].[BillChannel] ADD  CONSTRAINT [DF_BillChannel_PlatformRate]  DEFAULT ((0)) FOR [PlatformRate]
GO
ALTER TABLE [dbo].[BillChannel] ADD  CONSTRAINT [DF_BillChannel_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[BillIncomeExpenditure] ADD  CONSTRAINT [DF_AccountIncomeExpenditure_MonthID]  DEFAULT ((0)) FOR [MonthID]
GO
ALTER TABLE [dbo].[BillIncomeExpenditure] ADD  CONSTRAINT [DF_AccountIncomeExpenditure_TypeID]  DEFAULT ((1)) FOR [TypeID]
GO
ALTER TABLE [dbo].[BillIncomeExpenditure] ADD  CONSTRAINT [DF_AccountIncomeExpenditure_RecordName]  DEFAULT ('') FOR [RecordName]
GO
ALTER TABLE [dbo].[BillIncomeExpenditure] ADD  CONSTRAINT [DF_AccountIncomeExpenditure_Amount]  DEFAULT ((0)) FOR [Amount]
GO
ALTER TABLE [dbo].[BillIncomeExpenditure] ADD  CONSTRAINT [DF_AccountIncomeExpenditure_Remark]  DEFAULT ('') FOR [Remark]
GO
ALTER TABLE [dbo].[BillIncomeExpenditure] ADD  CONSTRAINT [DF_AccountIncomeExpenditure_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[BillRecordMonth] ADD  CONSTRAINT [DF_BillRecordMonth_BillStatus]  DEFAULT ((0)) FOR [BillStatus]
GO
ALTER TABLE [dbo].[BillRecordMonth] ADD  CONSTRAINT [DF_BillRecordMonth_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[ChannelConfig] ADD  CONSTRAINT [DF_ChannelConfig_ChannelName]  DEFAULT ('') FOR [ChannelName]
GO
ALTER TABLE [dbo].[ChannelConfig] ADD  CONSTRAINT [DF_ChannelConfig_ChannelRate]  DEFAULT ((0)) FOR [ChannelRate]
GO
ALTER TABLE [dbo].[ChannelConfig] ADD  CONSTRAINT [DF_ChannelConfig_PlatformID]  DEFAULT ((1)) FOR [PlatformID]
GO
ALTER TABLE [dbo].[ChannelConfig] ADD  CONSTRAINT [DF_ChannelConfig_PlatformRate]  DEFAULT ((0)) FOR [PlatformRate]
GO
ALTER TABLE [dbo].[ChannelConfig] ADD  CONSTRAINT [DF_ChannelConfig_SpreadChannelID]  DEFAULT ((1)) FOR [SpreadChannelID]
GO
ALTER TABLE [dbo].[ChannelConfig] ADD  CONSTRAINT [DF_ChannelConfig_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[ChannelConfig] ADD  CONSTRAINT [DF__ChannelCo__Exten__1DDB52D8]  DEFAULT ('') FOR [ExtendInfo]
GO
ALTER TABLE [dbo].[ChannelConfig] ADD  CONSTRAINT [DF__ChannelCo__Weigh__56DEC60A]  DEFAULT ((0)) FOR [WeightValue]
GO
ALTER TABLE [dbo].[ChannelConfig] ADD  CONSTRAINT [DF__ChannelCo__DayLi__625078B6]  DEFAULT ((0)) FOR [DayLimitAmount]
GO
ALTER TABLE [dbo].[ChannelWithdrawConfig] ADD  CONSTRAINT [DF_ChannelWithdrawConfig_ChannelName]  DEFAULT ('') FOR [ChannelName]
GO
ALTER TABLE [dbo].[ChannelWithdrawConfig] ADD  CONSTRAINT [DF_ChannelWithdrawConfig_ChannelRate]  DEFAULT ((0)) FOR [ChannelRate]
GO
ALTER TABLE [dbo].[ChannelWithdrawConfig] ADD  CONSTRAINT [DF_ChannelWithdrawConfig_FiexFee]  DEFAULT ((0)) FOR [FiexdFee]
GO
ALTER TABLE [dbo].[ChannelWithdrawConfig] ADD  CONSTRAINT [DF_ChannelWithdrawConfig_CheckSwitch]  DEFAULT ((0)) FOR [CheckSwitch]
GO
ALTER TABLE [dbo].[ChannelWithdrawConfig] ADD  CONSTRAINT [DF_ChannelWithdrawConfig_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[ChannelWithdrawConfig] ADD  CONSTRAINT [DF_ChannelWithdrawConfig_SpreadChannelID]  DEFAULT ((1)) FOR [SpreadChannelID]
GO
ALTER TABLE [dbo].[ChannelWithdrawConfig] ADD  CONSTRAINT [DF_ChannelWithdrawConfig_WeightValue]  DEFAULT ((0)) FOR [WeightValue]
GO
ALTER TABLE [dbo].[EMallCommodity] ADD  CONSTRAINT [DF_MallCommodity_TypeID]  DEFAULT ((0)) FOR [CommodityTypeID]
GO
ALTER TABLE [dbo].[EMallCommodity] ADD  CONSTRAINT [DF_EMallCommodity_KindID]  DEFAULT ((1)) FOR [KindID]
GO
ALTER TABLE [dbo].[EMallCommodity] ADD  CONSTRAINT [DF_EMallCommodity_GoodsID]  DEFAULT ((0)) FOR [GoodsID]
GO
ALTER TABLE [dbo].[EMallCommodity] ADD  CONSTRAINT [DF_EMallCommodity_GoodsCount]  DEFAULT ((0)) FOR [GoodsCount]
GO
ALTER TABLE [dbo].[EMallCommodity] ADD  CONSTRAINT [DF_EMallCommodity_GoodsIndate]  DEFAULT ((0)) FOR [GoodsIndate]
GO
ALTER TABLE [dbo].[EMallCommodity] ADD  CONSTRAINT [DF_MallCommodity_Price]  DEFAULT ((0)) FOR [Price]
GO
ALTER TABLE [dbo].[EMallCommodity] ADD  CONSTRAINT [DF_EMallCommodity_MoneyID]  DEFAULT ((0)) FOR [MoneyID]
GO
ALTER TABLE [dbo].[EMallCommodity] ADD  CONSTRAINT [DF_MallCommodity_RecommendStatus]  DEFAULT ((0)) FOR [RecommendStatus]
GO
ALTER TABLE [dbo].[EMallCommodity] ADD  CONSTRAINT [DF_MallCommodity_CommodityStatus]  DEFAULT ((0)) FOR [CommodityStatus]
GO
ALTER TABLE [dbo].[EMallCommodity] ADD  CONSTRAINT [DF_EMallCommodity_FillAddressStatus]  DEFAULT ((0)) FOR [FillAddressStatus]
GO
ALTER TABLE [dbo].[EMallCommodity] ADD  CONSTRAINT [DF_MallCommodity_VipLimitMin]  DEFAULT ((0)) FOR [VipLimitMin]
GO
ALTER TABLE [dbo].[EMallCommodity] ADD  CONSTRAINT [DF_MallCommodity_Inventory]  DEFAULT ((0)) FOR [Inventory]
GO
ALTER TABLE [dbo].[EMallCommodity] ADD  CONSTRAINT [DF_MallCommodity_SalesVolume]  DEFAULT ((0)) FOR [SoldCount]
GO
ALTER TABLE [dbo].[EMallCommodity] ADD  CONSTRAINT [DF_MallCommodity_SortID]  DEFAULT ((0)) FOR [SortID]
GO
ALTER TABLE [dbo].[EMallCommodity] ADD  CONSTRAINT [DF_MallCommodity_Description]  DEFAULT ('') FOR [Describe]
GO
ALTER TABLE [dbo].[EMallCommodity] ADD  CONSTRAINT [DF_MallCommodity_ModifyDate]  DEFAULT (getdate()) FOR [ModifyDateTime]
GO
ALTER TABLE [dbo].[EMallCommodity] ADD  CONSTRAINT [DF_MallCommodity_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[EMallCommodityType] ADD  CONSTRAINT [DF_MallCommodityType_SortID]  DEFAULT ((0)) FOR [SortID]
GO
ALTER TABLE [dbo].[EMallCommodityType] ADD  CONSTRAINT [DF_EMallCommodityType_AllowDeleteStatus]  DEFAULT ((1)) FOR [AllowDeleteStatus]
GO
ALTER TABLE [dbo].[EMallCommodityType] ADD  CONSTRAINT [DF_MallCommodityType_Describe]  DEFAULT ('') FOR [Describe]
GO
ALTER TABLE [dbo].[EMallCommodityType] ADD  CONSTRAINT [DF_MallCommodityType_CollectDate]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[EMallCommodityType] ADD  CONSTRAINT [DF__EMallComm__Nulli__0A338187]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[EMallOrder] ADD  CONSTRAINT [DF_MallOrder_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[EMallOrder] ADD  CONSTRAINT [DF_EMallOrder_Accounts]  DEFAULT ('') FOR [Accounts]
GO
ALTER TABLE [dbo].[EMallOrder] ADD  CONSTRAINT [DF_EMallOrder_CommodityName]  DEFAULT ('') FOR [CommodityName]
GO
ALTER TABLE [dbo].[EMallOrder] ADD  CONSTRAINT [DF_MallOrder_TypeID]  DEFAULT ((0)) FOR [CommodityTypeID]
GO
ALTER TABLE [dbo].[EMallOrder] ADD  CONSTRAINT [DF_MallOrder_CommodityCount]  DEFAULT ((0)) FOR [CommodityCount]
GO
ALTER TABLE [dbo].[EMallOrder] ADD  CONSTRAINT [DF_EMallOrder_Prive]  DEFAULT ((0)) FOR [Price]
GO
ALTER TABLE [dbo].[EMallOrder] ADD  CONSTRAINT [DF_MallOrder_Amount]  DEFAULT ((0)) FOR [OrderAmount]
GO
ALTER TABLE [dbo].[EMallOrder] ADD  CONSTRAINT [DF_MallOrder_GoodsID]  DEFAULT ((0)) FOR [GoodsID]
GO
ALTER TABLE [dbo].[EMallOrder] ADD  CONSTRAINT [DF_EMallOrder_GoodsCount]  DEFAULT ((0)) FOR [GoodsCount]
GO
ALTER TABLE [dbo].[EMallOrder] ADD  CONSTRAINT [DF_MallOrder_OrderStatus]  DEFAULT ((0)) FOR [OrderStatus]
GO
ALTER TABLE [dbo].[EMallOrder] ADD  CONSTRAINT [DF_MallOrder_Mobile]  DEFAULT ('') FOR [Mobile]
GO
ALTER TABLE [dbo].[EMallOrder] ADD  CONSTRAINT [DF_MallOrder_Province]  DEFAULT ('') FOR [Province]
GO
ALTER TABLE [dbo].[EMallOrder] ADD  CONSTRAINT [DF_MallOrder_City]  DEFAULT ('') FOR [City]
GO
ALTER TABLE [dbo].[EMallOrder] ADD  CONSTRAINT [DF_MallOrder_Area]  DEFAULT ('') FOR [Area]
GO
ALTER TABLE [dbo].[EMallOrder] ADD  CONSTRAINT [DF_MallOrder_Address]  DEFAULT ('') FOR [DetailedAddress]
GO
ALTER TABLE [dbo].[EMallOrder] ADD  CONSTRAINT [DF_EMallOrder_ExpressNumber]  DEFAULT ('') FOR [Remark]
GO
ALTER TABLE [dbo].[EMallOrder] ADD  CONSTRAINT [DF_MallOrder_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[EMallOrder] ADD  CONSTRAINT [DF__EMallOrde__PayAc__601E1781]  DEFAULT ('') FOR [PayAccounts]
GO
ALTER TABLE [dbo].[EMallOrder] ADD  CONSTRAINT [DF__EMallOrde__Refun__7DAE7A68]  DEFAULT ((0)) FOR [RefundStatus]
GO
ALTER TABLE [dbo].[EMallOrder] ADD  CONSTRAINT [DF__EMallOrde__Refun__7EA29EA1]  DEFAULT ('') FOR [RefundDescribe]
GO
ALTER TABLE [dbo].[GiftbagCode] ADD  CONSTRAINT [DF_Table_1_AllowUseNum]  DEFAULT ((0)) FOR [Count]
GO
ALTER TABLE [dbo].[GiftbagCode] ADD  CONSTRAINT [DF_GiftbagCode_UsedCount]  DEFAULT ((0)) FOR [UsedCount]
GO
ALTER TABLE [dbo].[GiftbagCode] ADD  CONSTRAINT [DF_GiftbagCode_LimitNum]  DEFAULT ((0)) FOR [LimitNum]
GO
ALTER TABLE [dbo].[GiftbagCode] ADD  CONSTRAINT [DF_GiftbagCode_PlatformID]  DEFAULT ((0)) FOR [PlatformID]
GO
ALTER TABLE [dbo].[GiftbagCode] ADD  CONSTRAINT [DF_GiftbagCode_Status]  DEFAULT ((0)) FOR [Status]
GO
ALTER TABLE [dbo].[GiftbagCode] ADD  CONSTRAINT [DF_GiftbagCode_StartTime]  DEFAULT (getdate()) FOR [StartTime]
GO
ALTER TABLE [dbo].[GiftbagCode] ADD  CONSTRAINT [DF_GiftbagCode_ExpiresTime]  DEFAULT (getdate()) FOR [ExpiresTime]
GO
ALTER TABLE [dbo].[GiftbagCode] ADD  CONSTRAINT [DF_GiftbagCode_CollectTimeDate]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[GiftbagCodeExGoods] ADD  CONSTRAINT [DF_GiftbagCodeExGoods_GoodsNumber]  DEFAULT ((0)) FOR [GoodsNumber]
GO
ALTER TABLE [dbo].[GiftbagCodeExGoods] ADD  CONSTRAINT [DF_GiftbagCodeExGoods_Indate]  DEFAULT ((0)) FOR [Indate]
GO
ALTER TABLE [dbo].[GiftbagCodeExRecord] ADD  CONSTRAINT [DF_GiftbagCodeRecord_GoodsArrivalStatus]  DEFAULT ((0)) FOR [GoodsArrivalStatus]
GO
ALTER TABLE [dbo].[GiftbagCodeExRecord] ADD  CONSTRAINT [DF_GiftbagCodeRecord_ClientIP]  DEFAULT ('') FOR [ClientIP]
GO
ALTER TABLE [dbo].[GiftbagCodeExRecord] ADD  CONSTRAINT [DF_GiftbagCodeRecord_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[GiftbagCodeGoods] ADD  CONSTRAINT [DF_GiftbagCodeGoods_BindState]  DEFAULT ((0)) FOR [BindState]
GO
ALTER TABLE [dbo].[GiftbagCodeGoods] ADD  CONSTRAINT [DF_GiftbagCodeGoods_Indate]  DEFAULT ((0)) FOR [Indate]
GO
ALTER TABLE [dbo].[GiftbagConfig] ADD  CONSTRAINT [DF_BuyGiftbagConfig_GiftbagName]  DEFAULT ('') FOR [GiftbagName]
GO
ALTER TABLE [dbo].[GiftbagConfig] ADD  CONSTRAINT [DF_GiftbagConfig_OriginalPrice]  DEFAULT ((0)) FOR [OriginalPrice]
GO
ALTER TABLE [dbo].[GiftbagConfig] ADD  CONSTRAINT [DF_BuyGiftbagConfig_Price]  DEFAULT ((0)) FOR [DiscountedPrice]
GO
ALTER TABLE [dbo].[GiftbagConfig] ADD  CONSTRAINT [DF_GiftbagConfig_ShowArea]  DEFAULT ((1)) FOR [ShowArea]
GO
ALTER TABLE [dbo].[GiftbagConfig] ADD  CONSTRAINT [DF_GiftbagConfig_KindID]  DEFAULT ((1)) FOR [Kind]
GO
ALTER TABLE [dbo].[GiftbagConfig] ADD  CONSTRAINT [DF_BuyGiftbagConfig_SortID]  DEFAULT ((0)) FOR [SortID]
GO
ALTER TABLE [dbo].[GiftbagConfig] ADD  CONSTRAINT [DF_GiftbagConfig_VipLimitMin]  DEFAULT ((0)) FOR [VipLimitMin]
GO
ALTER TABLE [dbo].[GiftbagConfig] ADD  CONSTRAINT [DF_GiftbagConfig_VipLimitMax]  DEFAULT ((0)) FOR [VipLimitMax]
GO
ALTER TABLE [dbo].[GiftbagConfig] ADD  CONSTRAINT [DF_GiftbagConfig_CannonTimesLimitMin]  DEFAULT ((0)) FOR [CannonTimesLimitMin]
GO
ALTER TABLE [dbo].[GiftbagConfig] ADD  CONSTRAINT [DF_GiftbagConfig_CannonTimesLimitMax]  DEFAULT ((0)) FOR [CannonTimesLimitMax]
GO
ALTER TABLE [dbo].[GiftbagConfig] ADD  CONSTRAINT [DF_GiftbagConfig_ContinuousPayInterval]  DEFAULT ((0)) FOR [ContinuousPayInterval]
GO
ALTER TABLE [dbo].[GiftbagConfig] ADD  CONSTRAINT [DF_GiftbagConfig_DoubleRules]  DEFAULT ('') FOR [GoldDoubleRule]
GO
ALTER TABLE [dbo].[GiftbagConfig] ADD  CONSTRAINT [DF__GiftbagCo__Purch__2453463D]  DEFAULT ('') FOR [PurchaseID]
GO
ALTER TABLE [dbo].[GiftbagConfig] ADD  CONSTRAINT [DF_BuyGiftbagConfig_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[GiftbagGoods] ADD  CONSTRAINT [DF_GiftbagGoods_GoodsIndex]  DEFAULT ((0)) FOR [GroupIndex]
GO
ALTER TABLE [dbo].[GiftbagGoods] ADD  CONSTRAINT [DF_GiftbagGoods_GoodsNumber]  DEFAULT ((0)) FOR [GoodsNumber]
GO
ALTER TABLE [dbo].[GiftbagGoods] ADD  CONSTRAINT [DF_GiftbagGoods_Indate]  DEFAULT ((0)) FOR [Indate]
GO
ALTER TABLE [dbo].[GlobalAppInfo] ADD  CONSTRAINT [DF_GlobalAppInfo_StationID]  DEFAULT ((0)) FOR [StationID]
GO
ALTER TABLE [dbo].[GlobalAppInfo] ADD  CONSTRAINT [DF_GlobalAppInfo_Price]  DEFAULT ((0)) FOR [Price]
GO
ALTER TABLE [dbo].[GlobalAppInfo] ADD  CONSTRAINT [DF_GlobalAppInfo_IsTop]  DEFAULT ((0)) FOR [IsTop]
GO
ALTER TABLE [dbo].[GlobalAppInfo] ADD  CONSTRAINT [DF_GlobalAppInfo_SortID]  DEFAULT ((0)) FOR [SortID]
GO
ALTER TABLE [dbo].[GlobalAppInfo] ADD  CONSTRAINT [DF_GlobalAppInfo_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[GlobalIngotInfo] ADD  CONSTRAINT [DF_GlobalIngotInfo_ProductName]  DEFAULT ('') FOR [ProductName]
GO
ALTER TABLE [dbo].[GlobalIngotInfo] ADD  CONSTRAINT [DF_GlobalIngotInfo_Description]  DEFAULT ('') FOR [Description]
GO
ALTER TABLE [dbo].[GlobalIngotInfo] ADD  CONSTRAINT [DF_GlobalIngotInfo_Price]  DEFAULT ((0)) FOR [Price]
GO
ALTER TABLE [dbo].[GlobalIngotInfo] ADD  CONSTRAINT [DF_GlobalIngotInfo_PresentScore]  DEFAULT ((0)) FOR [PresentScore]
GO
ALTER TABLE [dbo].[GlobalIngotInfo] ADD  CONSTRAINT [DF_GlobalIngotInfo_AttachScore]  DEFAULT ((0)) FOR [AttachScore]
GO
ALTER TABLE [dbo].[GlobalIngotInfo] ADD  CONSTRAINT [DF_GlobalIngotInfo_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[MallGoods] ADD  CONSTRAINT [DF_GoodsBuyConfig_ConfigName]  DEFAULT ('') FOR [MallgoodsName]
GO
ALTER TABLE [dbo].[MallGoods] ADD  CONSTRAINT [DF_MallGoods_KindID]  DEFAULT ((1)) FOR [KindID]
GO
ALTER TABLE [dbo].[MallGoods] ADD  CONSTRAINT [DF_GoodsBuyConfig_OriginalPrice]  DEFAULT ((0)) FOR [OriginalPrice]
GO
ALTER TABLE [dbo].[MallGoods] ADD  CONSTRAINT [DF_GoodsBuyConfig_DiscountedPrice]  DEFAULT ((0)) FOR [DiscountedPrice]
GO
ALTER TABLE [dbo].[MallGoods] ADD  CONSTRAINT [DF_MallGoods_CurrenyKind]  DEFAULT ((0)) FOR [MoneyID]
GO
ALTER TABLE [dbo].[MallGoods] ADD  CONSTRAINT [DF_MallGoods_NumberLimit]  DEFAULT ((0)) FOR [NumberLimit]
GO
ALTER TABLE [dbo].[MallGoods] ADD  CONSTRAINT [DF_MallGoods_GoodsInfo]  DEFAULT ('') FOR [GoodsInfo]
GO
ALTER TABLE [dbo].[MallGoods] ADD  CONSTRAINT [DF_BuyGoodsConfig_SortID]  DEFAULT ((0)) FOR [SortID]
GO
ALTER TABLE [dbo].[MallGoods] ADD  CONSTRAINT [DF_MallGoods_Label]  DEFAULT ('') FOR [Label]
GO
ALTER TABLE [dbo].[MallGoods] ADD  CONSTRAINT [DF_GoodsBuyConfig_IsFristDouble]  DEFAULT ((0)) FOR [DoubleStatus]
GO
ALTER TABLE [dbo].[MallGoods] ADD  CONSTRAINT [DF_MallGoods_RecommendationStatus]  DEFAULT ((0)) FOR [RecommendStatus]
GO
ALTER TABLE [dbo].[MallGoods] ADD  CONSTRAINT [DF_MallGoods_VipLimitMin]  DEFAULT ((0)) FOR [VipLimitMin]
GO
ALTER TABLE [dbo].[MallGoods] ADD  CONSTRAINT [DF_MallGoods_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[MallGoods] ADD  CONSTRAINT [DF_GoodsBuyConfig_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[MallGoods] ADD  CONSTRAINT [DF__MallGoods__Purch__25476A76]  DEFAULT ('') FOR [PurchaseID]
GO
ALTER TABLE [dbo].[MallGoods] ADD  CONSTRAINT [DF_MallGoods_DisDailyInventory]  DEFAULT ((0)) FOR [DisDailyInventory]
GO
ALTER TABLE [dbo].[MallGoods] ADD  CONSTRAINT [DF_MallGoods_DisEachDailyLimit]  DEFAULT ((0)) FOR [DisEachDailyLimit]
GO
ALTER TABLE [dbo].[MallGoods] ADD  CONSTRAINT [DF_MallGoods_DisDailyRemainInventory]  DEFAULT ((0)) FOR [DisDailyRemainInventory]
GO
ALTER TABLE [dbo].[MallGoodsBuyStat] ADD  CONSTRAINT [DF_MallGoodsBuyStat_BuyCount]  DEFAULT ((0)) FOR [MallgoodsBuyCount]
GO
ALTER TABLE [dbo].[MallGoodsBuyStat] ADD  CONSTRAINT [DF_MallGoodsBuyStat_PayAmount]  DEFAULT ((0)) FOR [SumAmount]
GO
ALTER TABLE [dbo].[MallGoodsBuyStat] ADD  CONSTRAINT [DF_MallGoodsBuyStat_TodayBuyCount]  DEFAULT ((0)) FOR [TodayBuyCount]
GO
ALTER TABLE [dbo].[MallGoodsPresent] ADD  CONSTRAINT [DF_GoodsBuyPresent_PresentRate]  DEFAULT ((0)) FOR [PresentRate]
GO
ALTER TABLE [dbo].[Notice] ADD  CONSTRAINT [DF_Notice_TypeID]  DEFAULT ((1)) FOR [TypeID]
GO
ALTER TABLE [dbo].[Notice] ADD  CONSTRAINT [DF_Notice_ContentType]  DEFAULT ((1)) FOR [ContentType]
GO
ALTER TABLE [dbo].[Notice] ADD  CONSTRAINT [DF_Notice_NoticeTitle]  DEFAULT ('') FOR [NoticeTitle]
GO
ALTER TABLE [dbo].[Notice] ADD  CONSTRAINT [DF_Notice_NoticeContent]  DEFAULT ('') FOR [NoticeContent]
GO
ALTER TABLE [dbo].[Notice] ADD  CONSTRAINT [DF_Notice_ViewPlace]  DEFAULT ((0)) FOR [ViewPlace]
GO
ALTER TABLE [dbo].[Notice] ADD  CONSTRAINT [DF_Notice_StartTime]  DEFAULT (getdate()) FOR [StartDateTime]
GO
ALTER TABLE [dbo].[Notice] ADD  CONSTRAINT [DF_Notice_EndTime]  DEFAULT (getdate()) FOR [EndDateTime]
GO
ALTER TABLE [dbo].[Notice] ADD  CONSTRAINT [DF_Notice_TopStatus]  DEFAULT ((0)) FOR [TopStatus]
GO
ALTER TABLE [dbo].[Notice] ADD  CONSTRAINT [DF_Notice_ShowPlatformID]  DEFAULT ('') FOR [ShowPlatformID]
GO
ALTER TABLE [dbo].[Notice] ADD  CONSTRAINT [DF_Notice_ExclusiveStatus]  DEFAULT ((0)) FOR [ExclusiveStatus]
GO
ALTER TABLE [dbo].[Notice] ADD  CONSTRAINT [DF_Notice_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[Notice] ADD  CONSTRAINT [DF_Notice_ModifyDateTime]  DEFAULT (getdate()) FOR [ModifyDateTime]
GO
ALTER TABLE [dbo].[Notice] ADD  CONSTRAINT [DF_Notice_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF_PayOrder_Amount]  DEFAULT ((0)) FOR [OrderAmount]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF_PayOrder_PayAmount]  DEFAULT ((0)) FOR [PayAmount]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF_PayOrder_Accounts]  DEFAULT ('') FOR [Accounts]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF_PayOrder_OrderState]  DEFAULT ((0)) FOR [OrderState]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF_PayOrder_ChannelRate]  DEFAULT ((0)) FOR [ChannelRate]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF_PayOrder_ChannelAmount]  DEFAULT ((0)) FOR [ChannelAmount]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF_PayOrder_PlatformID]  DEFAULT ((1)) FOR [PlatformID]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF_PayOrder_PlatformRate]  DEFAULT ((0)) FOR [PlatformRate]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF_PayOrder_ChannelAmount1]  DEFAULT ((0)) FOR [PlatformAmount]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF_PayOrder_CommodityID]  DEFAULT ((0)) FOR [CommodityID]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF_PayOrder_CommodityName]  DEFAULT ((0)) FOR [CommodityName]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF_PayOrder_CompanyAmount]  DEFAULT ((0)) FOR [CompanyAmount]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF_PayOrder_IsDouble]  DEFAULT ((0)) FOR [DoubleStatus]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF_PayOrder_GiftbagKind]  DEFAULT ((0)) FOR [GiftbagKind]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF_PayOrder_GiveGoldMultiple]  DEFAULT ((0)) FOR [GiftbagExtraGoldMultiple]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF_PayOrder_GoodsArrivalStatus]  DEFAULT ((0)) FOR [GoodsArrivalStatus]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF_PayOrder_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF__PayOrder__Create__25D17A5B]  DEFAULT ('') FOR [CreateClientIP]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF__PayOrder__Callba__26C59E94]  DEFAULT ('') FOR [CallbackClientIP]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF__PayOrder__IpayOr__080C0D4A]  DEFAULT ('') FOR [IpayOrderID]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF__PayOrder__OrderS__617C500E]  DEFAULT ((0)) FOR [OrderStatusChangeWay]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF_PayOrder_SpreadChannelID]  DEFAULT ((1)) FOR [SpreadChannelID]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF_PayOrder_MarketID]  DEFAULT ((1)) FOR [MarketID]
GO
ALTER TABLE [dbo].[PayOrder] ADD  CONSTRAINT [DF_PayOrder_SceneID]  DEFAULT ((0)) FOR [SceneID]
GO
ALTER TABLE [dbo].[PayOrderGoods] ADD  CONSTRAINT [DF_PayOrderGoods_GoodsNumber]  DEFAULT ((0)) FOR [GoodsNumber]
GO
ALTER TABLE [dbo].[PayOrderGoods] ADD  CONSTRAINT [DF_PayOrderGoods_Indate]  DEFAULT ((0)) FOR [Indate]
GO
ALTER TABLE [dbo].[PayOrderGoods] ADD  CONSTRAINT [DF_PayOrderGoods_AwardIdent]  DEFAULT ((0)) FOR [AwardIdent]
GO
ALTER TABLE [dbo].[PayOrderGoods] ADD  CONSTRAINT [DF_PayOrderGoods_ExtendProperty]  DEFAULT ('') FOR [ExtendProperty]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF_PayRecord_Accounts]  DEFAULT ('') FOR [Accounts]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF_PayRecord_ChannelRate]  DEFAULT ((0)) FOR [ChannelRate]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF_PayRecord_ChannelAmount]  DEFAULT ((0)) FOR [ChannelAmount]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF_PayRecord_PlatformID]  DEFAULT ((1)) FOR [PlatformID]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF_PayRecord_PlatformRate]  DEFAULT ((0)) FOR [PlatformRate]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF_PayRecord_PlatformAmount]  DEFAULT ((0)) FOR [PlatformAmount]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF_PayRecord_CompanyAmount]  DEFAULT ((0)) FOR [CompanyAmount]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF_PayRecord_GoodsArrivalStatus]  DEFAULT ((0)) FOR [GoodsArrivalStatus]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF_PayRecord_IsFirstDoubel]  DEFAULT ((0)) FOR [DoubleStatus]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF__PayRecord__Creat__27B9C2CD]  DEFAULT ('') FOR [CreateClientIP]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF__PayRecord__Callb__28ADE706]  DEFAULT ('') FOR [CallbackClientIP]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF__PayRecord__IpayO__09003183]  DEFAULT ('') FOR [IpayOrderID]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF__PayRecord__Order__62707447]  DEFAULT ((0)) FOR [OrderStatusChangeWay]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF__PayRecord__Sprea__5A8539BC]  DEFAULT ((0)) FOR [SpreadChannelID]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF__PayRecord__Sprea__5B795DF5]  DEFAULT ((0)) FOR [SpreadChannelRate]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF__PayRecord__Sprea__5C6D822E]  DEFAULT ((0)) FOR [SpreadChannelAmount]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF_PayRecord_SpreadUserID]  DEFAULT ((0)) FOR [SpreadUserID]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF_PayRecord_SpreadRate]  DEFAULT ((0)) FOR [SpreadRate]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF_PayRecord_SpreadDivideAmount]  DEFAULT ((0)) FOR [SpreadDivideAmount]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF_PayRecord_SpreadDivideScore]  DEFAULT ((0)) FOR [SpreadDivideScore]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF_PayRecord_MarketID]  DEFAULT ((1)) FOR [MarketID]
GO
ALTER TABLE [dbo].[PayRecord] ADD  CONSTRAINT [DF_PayRecord_SceneID]  DEFAULT ((0)) FOR [SceneID]
GO
ALTER TABLE [dbo].[ReportDayCommodityPay] ADD  CONSTRAINT [DF_ReportDayCommodityPay_PaySumAmount]  DEFAULT ((0)) FOR [PaySumAmount]
GO
ALTER TABLE [dbo].[ReportDayCommodityPay] ADD  CONSTRAINT [DF_ReportDayCommodityPay_LastPayDateTime]  DEFAULT (getdate()) FOR [LastPayDateTime]
GO
ALTER TABLE [dbo].[ReportDayPay] ADD  CONSTRAINT [DF_ReportDayPay_PlatformID]  DEFAULT ((1)) FOR [PlatformID]
GO
ALTER TABLE [dbo].[ReportDayPay] ADD  CONSTRAINT [DF_ReportDayPay_PayNumber]  DEFAULT ((0)) FOR [PayCount]
GO
ALTER TABLE [dbo].[ReportDayPay] ADD  CONSTRAINT [DF_ReportDayPay_PayUserCount]  DEFAULT ((0)) FOR [PayUserCount]
GO
ALTER TABLE [dbo].[ReportDayPay] ADD  CONSTRAINT [DF_ReportDayPay_ChannelAmount]  DEFAULT ((0)) FOR [ChannelAmount]
GO
ALTER TABLE [dbo].[ReportDayPay] ADD  CONSTRAINT [DF_ReportDayPay_PlatformAmount]  DEFAULT ((0)) FOR [PlatformAmount]
GO
ALTER TABLE [dbo].[ReportDayPay] ADD  CONSTRAINT [DF_ReportDayPay_StatDateTime]  DEFAULT (getdate()) FOR [StatDateTime]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_ChannelID]  DEFAULT ((0)) FOR [ChannelID]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_ChannelFee]  DEFAULT ((0)) FOR [ChannelFee]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_ApplyType]  DEFAULT ((0)) FOR [ApplyType]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_ApplyAmount]  DEFAULT ((0)) FOR [ApplyAmount]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_TransferType]  DEFAULT ((0)) FOR [TransferType]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_TransferAmount]  DEFAULT ((0)) FOR [TransferAmount]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_BeforeScore]  DEFAULT ((0)) FOR [BeforeScore]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_Name]  DEFAULT ('') FOR [Name]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_BankCode]  DEFAULT ('') FOR [BankCode]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_BankName]  DEFAULT ('') FOR [BankName]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_BankNo]  DEFAULT ('') FOR [BankNo]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_MobileNo]  DEFAULT ('') FOR [MobileNo]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_CheckKind]  DEFAULT ((0)) FOR [CheckKind]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_OrderState]  DEFAULT ((0)) FOR [OrderState]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_ClientIP]  DEFAULT ('') FOR [ClientIP]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_ChannelOrderID]  DEFAULT ('') FOR [ChannelOrderID]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_CollectNote]  DEFAULT ('') FOR [CollectNote]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_SystemFee]  DEFAULT ((0)) FOR [SystemFee]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_PixNo]  DEFAULT ('') FOR [PixNo]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_PixType]  DEFAULT ((1)) FOR [PixType]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_Cpf]  DEFAULT ('') FOR [Cpf]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_ConsumeBetAmount]  DEFAULT ((0)) FOR [ConsumeBetAmount]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_ManualAuditReason]  DEFAULT ('') FOR [ManualAuditReason]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_TransferFailReason]  DEFAULT ('') FOR [TransferFailReason]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_MarketID]  DEFAULT ((1)) FOR [MarketID]
GO
ALTER TABLE [dbo].[WithdrawOrder] ADD  CONSTRAINT [DF_WithdrawOrder_SpreadChannelID]  DEFAULT ((1)) FOR [SpreadChannelID]
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsBank', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'兑换渠道' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsBank', @level2type=N'COLUMN',@level2name=N'WithdrawChannelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'支付渠道ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsBank', @level2type=N'COLUMN',@level2name=N'PayChannelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收款人姓名' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsBank', @level2type=N'COLUMN',@level2name=N'Name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'银行代码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsBank', @level2type=N'COLUMN',@level2name=N'BankCode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'银行名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsBank', @level2type=N'COLUMN',@level2name=N'BankName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'银行帐号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsBank', @level2type=N'COLUMN',@level2name=N'BankNo'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'手机号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsBank', @level2type=N'COLUMN',@level2name=N'MobileNo'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsBank', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Pix账户' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsBank', @level2type=N'COLUMN',@level2name=N'PixNo'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'支付EMAIL' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsBank', @level2type=N'COLUMN',@level2name=N'PayEamil'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Pxi类型  1：CPF 2：CNPJ 3：EMAIL 4：PHONE 5：EVP 6：RANDOMKEY' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsBank', @level2type=N'COLUMN',@level2name=N'PixType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'CPF' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsBank', @level2type=N'COLUMN',@level2name=N'Cpf'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'支付电话' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsBank', @level2type=N'COLUMN',@level2name=N'PayMobileNo'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsBank', @level2type=N'COLUMN',@level2name=N'PayCpf'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsPay', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsPay', @level2type=N'COLUMN',@level2name=N'PayCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsPay', @level2type=N'COLUMN',@level2name=N'PayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后支付金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsPay', @level2type=N'COLUMN',@level2name=N'LastPayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后支付时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsPay', @level2type=N'COLUMN',@level2name=N'LastPayDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'重置金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsWithdraw', @level2type=N'COLUMN',@level2name=N'ResetBetAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'重置打码量类型 1：提现 2：充值' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsWithdraw', @level2type=N'COLUMN',@level2name=N'ResetType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'月份ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BillChannel', @level2type=N'COLUMN',@level2name=N'MonthID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BillChannel', @level2type=N'COLUMN',@level2name=N'ChannelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BillChannel', @level2type=N'COLUMN',@level2name=N'ChannelName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道分成' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BillChannel', @level2type=N'COLUMN',@level2name=N'ChannelRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'平台分成' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BillChannel', @level2type=N'COLUMN',@level2name=N'PlatformRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BillChannel', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BillIncomeExpenditure', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'月份ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BillIncomeExpenditure', @level2type=N'COLUMN',@level2name=N'MonthID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型ID 1：硬件成本 2：其他成本 3：其他收入' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BillIncomeExpenditure', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BillIncomeExpenditure', @level2type=N'COLUMN',@level2name=N'RecordName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BillIncomeExpenditure', @level2type=N'COLUMN',@level2name=N'Amount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BillIncomeExpenditure', @level2type=N'COLUMN',@level2name=N'Remark'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BillIncomeExpenditure', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'帐目状态' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BillRecordMonth', @level2type=N'COLUMN',@level2name=N'BillStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BillRecordMonth', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelConfig', @level2type=N'COLUMN',@level2name=N'ChannelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelConfig', @level2type=N'COLUMN',@level2name=N'ChannelName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道分成' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelConfig', @level2type=N'COLUMN',@level2name=N'ChannelRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道所属平台ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelConfig', @level2type=N'COLUMN',@level2name=N'PlatformID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'平台分成' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelConfig', @level2type=N'COLUMN',@level2name=N'PlatformRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广渠道ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelConfig', @level2type=N'COLUMN',@level2name=N'SpreadChannelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁用状态 0：正常 1：禁用' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelConfig', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商户ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelConfig', @level2type=N'COLUMN',@level2name=N'MerchantID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'APPID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelConfig', @level2type=N'COLUMN',@level2name=N'MerchantAppID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商户公钥' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelConfig', @level2type=N'COLUMN',@level2name=N'MerchantPublicKey'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商户私钥' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelConfig', @level2type=N'COLUMN',@level2name=N'MerchantPrivateKey'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'权重值' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelConfig', @level2type=N'COLUMN',@level2name=N'WeightValue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日限制金额 0：不限制' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelConfig', @level2type=N'COLUMN',@level2name=N'DayLimitAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelWithdrawConfig', @level2type=N'COLUMN',@level2name=N'ChannelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelWithdrawConfig', @level2type=N'COLUMN',@level2name=N'ChannelName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道分成' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelWithdrawConfig', @level2type=N'COLUMN',@level2name=N'ChannelRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'固定手续费' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelWithdrawConfig', @level2type=N'COLUMN',@level2name=N'FiexdFee'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'自动审核开关 0-关闭 1-开启' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelWithdrawConfig', @level2type=N'COLUMN',@level2name=N'CheckSwitch'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁用状态 0：正常 1：禁用' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelWithdrawConfig', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广渠道ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelWithdrawConfig', @level2type=N'COLUMN',@level2name=N'SpreadChannelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'权重值' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelWithdrawConfig', @level2type=N'COLUMN',@level2name=N'WeightValue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商品ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodity', @level2type=N'COLUMN',@level2name=N'CommodityID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商品名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodity', @level2type=N'COLUMN',@level2name=N'CommodityName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商品ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodity', @level2type=N'COLUMN',@level2name=N'CommodityTypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商城类型 1：金币商城 2：会员商城 3：男人商城 4：女人商城 5：好友商城 6：牌背 7：头像框 8：聊天框 9：背景' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodity', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'有效道具ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodity', @level2type=N'COLUMN',@level2name=N'GoodsID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'道具总数（仅游戏道具有效）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodity', @level2type=N'COLUMN',@level2name=N'GoodsCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品有效期秒（0： 永久拥有  ）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodity', @level2type=N'COLUMN',@level2name=N'GoodsIndate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'价格' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodity', @level2type=N'COLUMN',@level2name=N'Price'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'货币ID 1：钻石 2：金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodity', @level2type=N'COLUMN',@level2name=N'MoneyID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推荐状态 0：不推荐 1：推荐' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodity', @level2type=N'COLUMN',@level2name=N'RecommendStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上架状态 0：未上架 1：已上架' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodity', @level2type=N'COLUMN',@level2name=N'CommodityStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'需要填写地址状态 0：无需 1：需要' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodity', @level2type=N'COLUMN',@level2name=N'FillAddressStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'兑换最小VIP等级' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodity', @level2type=N'COLUMN',@level2name=N'VipLimitMin'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'库存数量（-1 无库存）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodity', @level2type=N'COLUMN',@level2name=N'Inventory'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'销售总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodity', @level2type=N'COLUMN',@level2name=N'SoldCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排序ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodity', @level2type=N'COLUMN',@level2name=N'SortID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'描述信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodity', @level2type=N'COLUMN',@level2name=N'Describe'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修改时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodity', @level2type=N'COLUMN',@level2name=N'ModifyDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodity', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商品类型ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodityType', @level2type=N'COLUMN',@level2name=N'CommodityTypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodityType', @level2type=N'COLUMN',@level2name=N'TypeName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'允许删除状态 0：不允许 1：允许' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodityType', @level2type=N'COLUMN',@level2name=N'AllowDeleteStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'描述信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodityType', @level2type=N'COLUMN',@level2name=N'Describe'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallCommodityType', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallOrder', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户帐号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallOrder', @level2type=N'COLUMN',@level2name=N'Accounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商品ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallOrder', @level2type=N'COLUMN',@level2name=N'CommodityID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商品名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallOrder', @level2type=N'COLUMN',@level2name=N'CommodityName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商品类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallOrder', @level2type=N'COLUMN',@level2name=N'CommodityTypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallOrder', @level2type=N'COLUMN',@level2name=N'CommodityCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'订单金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallOrder', @level2type=N'COLUMN',@level2name=N'OrderAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallOrder', @level2type=N'COLUMN',@level2name=N'GoodsID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商品数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallOrder', @level2type=N'COLUMN',@level2name=N'GoodsCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'订单状态 0：未发货 1：已发货' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallOrder', @level2type=N'COLUMN',@level2name=N'OrderStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'手机号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallOrder', @level2type=N'COLUMN',@level2name=N'Mobile'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收货人' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallOrder', @level2type=N'COLUMN',@level2name=N'Consignee'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收货地址：省份' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallOrder', @level2type=N'COLUMN',@level2name=N'Province'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收获地址：城市' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallOrder', @level2type=N'COLUMN',@level2name=N'City'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收获地址：地区' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallOrder', @level2type=N'COLUMN',@level2name=N'Area'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收获地址：详细地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallOrder', @level2type=N'COLUMN',@level2name=N'DetailedAddress'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallOrder', @level2type=N'COLUMN',@level2name=N'Remark'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'发货时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallOrder', @level2type=N'COLUMN',@level2name=N'DeliveryDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallOrder', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'退款状态' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'EMallOrder', @level2type=N'COLUMN',@level2name=N'RefundStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'礼包码ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCode', @level2type=N'COLUMN',@level2name=N'GiftbagCodeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'礼包数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCode', @level2type=N'COLUMN',@level2name=N'Count'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'已使用数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCode', @level2type=N'COLUMN',@level2name=N'UsedCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'单个用户使用次数，0表示不限制' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCode', @level2type=N'COLUMN',@level2name=N'LimitNum'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道限制，0表示不限制' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCode', @level2type=N'COLUMN',@level2name=N'PlatformID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'礼包卡状态' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCode', @level2type=N'COLUMN',@level2name=N'Status'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'开始时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCode', @level2type=N'COLUMN',@level2name=N'StartTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'过期时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCode', @level2type=N'COLUMN',@level2name=N'ExpiresTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCode', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCodeExGoods', @level2type=N'COLUMN',@level2name=N'GoodsID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCodeExGoods', @level2type=N'COLUMN',@level2name=N'GoodsNumber'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'有效期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCodeExGoods', @level2type=N'COLUMN',@level2name=N'Indate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCodeExRecord', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'礼包码ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCodeExRecord', @level2type=N'COLUMN',@level2name=N'GiftbagCodeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCodeExRecord', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品到账状态 0：未到帐 1：已到帐' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCodeExRecord', @level2type=N'COLUMN',@level2name=N'GoodsArrivalStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品到帐时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCodeExRecord', @level2type=N'COLUMN',@level2name=N'GoodsArrivalTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'客户端IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCodeExRecord', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCodeExRecord', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'礼包ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCodeGoods', @level2type=N'COLUMN',@level2name=N'GiftbagCodeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCodeGoods', @level2type=N'COLUMN',@level2name=N'GoodsID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCodeGoods', @level2type=N'COLUMN',@level2name=N'GoodsNumber'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'有效期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagCodeGoods', @level2type=N'COLUMN',@level2name=N'Indate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'礼包ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagConfig', @level2type=N'COLUMN',@level2name=N'GiftbagID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'礼包名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagConfig', @level2type=N'COLUMN',@level2name=N'GiftbagName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'原价' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagConfig', @level2type=N'COLUMN',@level2name=N'OriginalPrice'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'折后价格' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagConfig', @level2type=N'COLUMN',@level2name=N'DiscountedPrice'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'显示区域' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagConfig', @level2type=N'COLUMN',@level2name=N'ShowArea'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'礼包类型 1：普通礼包 2：充值连充' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagConfig', @level2type=N'COLUMN',@level2name=N'Kind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排序ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagConfig', @level2type=N'COLUMN',@level2name=N'SortID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数量限制  0：不限制 1：每日一次 2：每周一次 3：永久一次' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagConfig', @level2type=N'COLUMN',@level2name=N'NumberLimit'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'VIP最小限制 0：不限制' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagConfig', @level2type=N'COLUMN',@level2name=N'VipLimitMin'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'VIP最大限制 0：不限制' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagConfig', @level2type=N'COLUMN',@level2name=N'VipLimitMax'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'炮倍最小限制 0：不限制' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagConfig', @level2type=N'COLUMN',@level2name=N'CannonTimesLimitMin'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'炮倍最大限制 0：不限制' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagConfig', @level2type=N'COLUMN',@level2name=N'CannonTimesLimitMax'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'连充间隔' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagConfig', @level2type=N'COLUMN',@level2name=N'ContinuousPayInterval'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'翻倍规则' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagConfig', @level2type=N'COLUMN',@level2name=N'GoldDoubleRule'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'苹果ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagConfig', @level2type=N'COLUMN',@level2name=N'PurchaseID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagConfig', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagGoods', @level2type=N'COLUMN',@level2name=N'GoodsID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagGoods', @level2type=N'COLUMN',@level2name=N'GoodsNumber'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'有效期 0：永久 ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GiftbagGoods', @level2type=N'COLUMN',@level2name=N'Indate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商品标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GlobalAppInfo', @level2type=N'COLUMN',@level2name=N'ProductID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商品价格' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GlobalAppInfo', @level2type=N'COLUMN',@level2name=N'Price'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GlobalAppInfo', @level2type=N'COLUMN',@level2name=N'PresentCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'额外金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GlobalAppInfo', @level2type=N'COLUMN',@level2name=N'ExtraCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'是否首页显示(0：不，1：是)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GlobalAppInfo', @level2type=N'COLUMN',@level2name=N'IsTop'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'产品名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GlobalIngotInfo', @level2type=N'COLUMN',@level2name=N'ProductName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'描述' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GlobalIngotInfo', @level2type=N'COLUMN',@level2name=N'Description'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GlobalIngotInfo', @level2type=N'COLUMN',@level2name=N'PresentScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'附加金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GlobalIngotInfo', @level2type=N'COLUMN',@level2name=N'AttachScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GlobalIngotInfo', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'配置ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoods', @level2type=N'COLUMN',@level2name=N'MallgoodsID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'配置名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoods', @level2type=N'COLUMN',@level2name=N'MallgoodsName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商城类型 1：金币商城 2：钻石商城 3：会员商城  4：礼包商城  5：特惠礼包' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoods', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商品原价' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoods', @level2type=N'COLUMN',@level2name=N'OriginalPrice'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商品折后价' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoods', @level2type=N'COLUMN',@level2name=N'DiscountedPrice'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'货币ID 0：货币 1：钻石' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoods', @level2type=N'COLUMN',@level2name=N'MoneyID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数量限制  0：不限制 1：每日一次 2：每周一次 3：永久一次' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoods', @level2type=N'COLUMN',@level2name=N'NumberLimit'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoods', @level2type=N'COLUMN',@level2name=N'GoodsInfo'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排序ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoods', @level2type=N'COLUMN',@level2name=N'SortID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商品角标' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoods', @level2type=N'COLUMN',@level2name=N'Label'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'首充是否翻倍 0：不翻倍 1：首充翻倍 2：每日翻倍' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoods', @level2type=N'COLUMN',@level2name=N'DoubleStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推荐状态 0：不推荐 1：推荐' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoods', @level2type=N'COLUMN',@level2name=N'RecommendStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'VIP最小限制 0：不限制' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoods', @level2type=N'COLUMN',@level2name=N'VipLimitMin'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁用状态' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoods', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoods', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'支付ID（苹果、华为、谷歌等）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoods', @level2type=N'COLUMN',@level2name=N'PurchaseID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'每日特惠礼包属性，每日库存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoods', @level2type=N'COLUMN',@level2name=N'DisDailyInventory'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'每日特惠礼包属性，每人每日次数限制' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoods', @level2type=N'COLUMN',@level2name=N'DisEachDailyLimit'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'每日优惠，当日剩余库存' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoods', @level2type=N'COLUMN',@level2name=N'DisDailyRemainInventory'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoodsBuyStat', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商城商品ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoodsBuyStat', @level2type=N'COLUMN',@level2name=N'MallgoodsID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商城商品购买数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoodsBuyStat', @level2type=N'COLUMN',@level2name=N'MallgoodsBuyCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoodsBuyStat', @level2type=N'COLUMN',@level2name=N'SumAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后购买时间（按订单时间算）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoodsBuyStat', @level2type=N'COLUMN',@level2name=N'LastBuyDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'今日购买次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoodsBuyStat', @level2type=N'COLUMN',@level2name=N'TodayBuyCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送比例' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MallGoodsPresent', @level2type=N'COLUMN',@level2name=N'PresentRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'公告ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'Notice', @level2type=N'COLUMN',@level2name=N'NoticeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型ID 1：登录公告 2：大厅公告 3：跑马灯' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'Notice', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'内容类型（1：图片，2：文字）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'Notice', @level2type=N'COLUMN',@level2name=N'ContentType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'公告标题' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'Notice', @level2type=N'COLUMN',@level2name=N'NoticeTitle'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'文字内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'Notice', @level2type=N'COLUMN',@level2name=N'NoticeContent'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'显示区域' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'Notice', @level2type=N'COLUMN',@level2name=N'ViewPlace'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'开始时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'Notice', @level2type=N'COLUMN',@level2name=N'StartDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'结束时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'Notice', @level2type=N'COLUMN',@level2name=N'EndDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'置顶状态' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'Notice', @level2type=N'COLUMN',@level2name=N'TopStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'独享状态' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'Notice', @level2type=N'COLUMN',@level2name=N'ExclusiveStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁止服务' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'Notice', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'更新时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'Notice', @level2type=N'COLUMN',@level2name=N'ModifyDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'Notice', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'订单标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'OrderID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'订单类型 1：商城  2：礼包' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'OrderKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'订单金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'OrderAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'实际支付金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'PayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户帐号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'Accounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'订单状态 0：待支付 1：已支付，物品' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'OrderState'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'ChannelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道分成' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'ChannelRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'ChannelAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'平台ID 1：官方' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'PlatformID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'平台分成' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'PlatformRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'平台金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'PlatformAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商品ID（物品ID或者礼包ID）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'CommodityID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商品名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'CommodityName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'公司收入' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'CompanyAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'是否首充翻倍 充值成功回调之后才处理这个字段 0：不翻倍 1：翻倍' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'DoubleStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'礼包类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'GiftbagKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'半盲礼包额外赠送倍数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'GiftbagExtraGoldMultiple'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品到账状态 0：未到帐 1：到帐' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'GoodsArrivalStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'支付时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'PayDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广渠道ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'SpreadChannelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'包ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'MarketID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'1-登录充值弹框 2-大厅商城 3-游戏商城 4- 游戏中的充值弹框 ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrder', @level2type=N'COLUMN',@level2name=N'SceneID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrderGoods', @level2type=N'COLUMN',@level2name=N'GoodsNumber'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'有效期 0：永久' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrderGoods', @level2type=N'COLUMN',@level2name=N'Indate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'奖励标识 0：不属奖励 1：首充翻倍 2：支付宝额外赠送 3：半盲盒额外赠送多倍' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrderGoods', @level2type=N'COLUMN',@level2name=N'AwardIdent'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'扩展属性' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayOrderGoods', @level2type=N'COLUMN',@level2name=N'ExtendProperty'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'订单ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'OrderID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'订单类型 1：商城  2：礼包' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'OrderKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'订单金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'OrderAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'实际支付金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'PayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户帐号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'Accounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道比例' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'ChannelRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'ChannelAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'平台ID 1：官方' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'PlatformID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'平台分成' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'PlatformRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'平台金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'PlatformAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商品ID（物品ID或者礼包ID）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'CommodityID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商品名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'CommodityName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'公司收入' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'CompanyAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品到帐状态 0：未到账 1：已到账' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'GoodsArrivalStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'是否首充翻倍 充值成功回调之后才处理这个字段 0：不翻倍 1：翻倍' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'DoubleStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'支付金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'PayDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广渠道' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'SpreadChannelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广渠道分成比例' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'SpreadChannelRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广渠道分成金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'SpreadChannelAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广用户ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'SpreadUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广比例' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'SpreadRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广分成金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'SpreadDivideAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广分成金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'SpreadDivideScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'包ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'MarketID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'1-登录充值弹框 2-大厅商城 3-游戏商城 4- 游戏中的充值弹框 ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PayRecord', @level2type=N'COLUMN',@level2name=N'SceneID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'订单类型 1：金币 2：钻石 3：会员  4：礼包' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayCommodityPay', @level2type=N'COLUMN',@level2name=N'OrderKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总支付金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayCommodityPay', @level2type=N'COLUMN',@level2name=N'PaySumAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'支付总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayCommodityPay', @level2type=N'COLUMN',@level2name=N'PayCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后支付时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayCommodityPay', @level2type=N'COLUMN',@level2name=N'LastPayDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后支付金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayCommodityPay', @level2type=N'COLUMN',@level2name=N'LastPayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'平台ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayPay', @level2type=N'COLUMN',@level2name=N'PlatformID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'支付金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayPay', @level2type=N'COLUMN',@level2name=N'PayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'支付笔数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayPay', @level2type=N'COLUMN',@level2name=N'PayCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'支付用户总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayPay', @level2type=N'COLUMN',@level2name=N'PayUserCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道分成' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayPay', @level2type=N'COLUMN',@level2name=N'ChannelAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'平台分成' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayPay', @level2type=N'COLUMN',@level2name=N'PlatformAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReportDayPay', @level2type=N'COLUMN',@level2name=N'StatDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'订单号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'OrderID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'通道ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'ChannelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'动态手续费' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'ChannelFee'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'提现类型（0：金币提现，1：佣金提现）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'ApplyType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'提现金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'ApplyAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'转账类型（0：转账到银行，1：转账到电子钱包）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'TransferType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'转账金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'TransferAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'兑换前数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'BeforeScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收款账号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'Name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'银行编码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'BankCode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'银行名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'BankName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'银行账号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'BankNo'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'手机号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'MobileNo'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'审核方式 0：人工审核 1：机器自动审核' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'CheckKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'审核时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'CheckDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'订单状态 0：待审核 1：已审核，2：扣留，3：退款  4：转账完成  5:转账失败' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'OrderState'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'客户端IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'第三方渠道订单号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'ChannelOrderID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'支付时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'PayDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'CollectNote'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'系统手续费' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'SystemFee'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Pix账户' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'PixNo'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'税类型 1:CPF 2:CPNJ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'PixType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Cpf' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'Cpf'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'消耗下注金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'ConsumeBetAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'人工审核原因' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'ManualAuditReason'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'转账失败原因' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'TransferFailReason'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'包ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'MarketID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广渠道ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WithdrawOrder', @level2type=N'COLUMN',@level2name=N'SpreadChannelID'
GO
