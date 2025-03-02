USE [LHAccountsDB]
GO
/****** Object:  Table [dbo].[AccountsAgentInfo]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsAgentInfo](
	[UserID] [int] NOT NULL,
	[AgentLevel] [tinyint] NOT NULL,
	[LowerBetAwardRate] [int] NOT NULL,
	[LowerPayAwardAmount] [bigint] NOT NULL,
	[LLowerBetAwardRate] [int] NOT NULL,
	[LLowerPayAwardAmount] [bigint] NOT NULL,
 CONSTRAINT [PK_AccountsAgentInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsAuthInfo]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsAuthInfo](
	[UserID] [int] NOT NULL,
	[CodeKind] [smallint] NOT NULL,
	[AuthTimes] [smallint] NOT NULL,
	[AcquireTimes] [smallint] NOT NULL,
	[LastAcquireDate] [datetime] NOT NULL,
	[LastMobileEmail] [nvarchar](64) NOT NULL,
 CONSTRAINT [PK_AccountsAuthInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC,
	[CodeKind] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsCommission]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsCommission](
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
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_UserBrokerageInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsDatum]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsDatum](
	[UserID] [int] NOT NULL,
	[Compellation] [nvarchar](16) NOT NULL,
	[Age] [tinyint] NOT NULL,
	[Area] [nvarchar](15) NOT NULL,
	[City] [nvarchar](15) NOT NULL,
	[Province] [nvarchar](15) NOT NULL,
	[QQ] [nvarchar](16) NOT NULL,
	[EMail] [nvarchar](32) NOT NULL,
	[SeatPhone] [nvarchar](32) NOT NULL,
	[MobilePhone] [nvarchar](21) NOT NULL,
	[DwellingPlace] [nvarchar](128) NOT NULL,
	[PostalCode] [nvarchar](8) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[UserNote] [nvarchar](256) NOT NULL,
 CONSTRAINT [PK_IndividualDatum_UserID] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsExternal]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsExternal](
	[UniqueID] [nvarchar](32) NOT NULL,
	[ExternalID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[PublicID] [nvarchar](32) NOT NULL,
	[NickName] [nvarchar](32) NOT NULL,
	[Gender] [tinyint] NOT NULL,
	[UserDatum] [nvarchar](512) NOT NULL,
	[UserFacePath] [nvarchar](256) NOT NULL,
	[LogonClientIP] [nvarchar](15) NOT NULL,
	[LogonDateTime] [datetime] NOT NULL,
	[SubscribeFlag] [tinyint] NOT NULL,
	[SubscribeTime] [datetime] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_AccountsWeChat_UniqueID] PRIMARY KEY CLUSTERED 
(
	[UniqueID] ASC,
	[ExternalID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsFace]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsFace](
	[UserID] [int] NOT NULL,
	[CustomID] [int] NOT NULL,
	[FacePath] [nvarchar](256) NOT NULL,
	[UpdateTime] [datetime] NOT NULL,
	[CollectTime] [datetime] NOT NULL,
 CONSTRAINT [PK_AccountsFace_ID] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsGoldInfo]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsGoldInfo](
	[UserID] [int] NOT NULL,
	[Score] [bigint] NOT NULL,
	[FillTimes] [int] NOT NULL,
	[FillAmount] [bigint] NOT NULL,
	[InsureScore] [bigint] NOT NULL,
	[GrantTimes] [int] NOT NULL,
	[GrantAmount] [bigint] NOT NULL,
 CONSTRAINT [PK_AccountsGoldInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsInfo]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsInfo](
	[UserID] [int] IDENTITY(1,1) NOT NULL,
	[GameID] [int] NOT NULL,
	[PasswordID] [int] NOT NULL,
	[Accounts] [nvarchar](31) NOT NULL,
	[RegAccounts] [nvarchar](31) NOT NULL,
	[NickName] [nvarchar](31) NOT NULL,
	[UnderWrite] [nvarchar](63) NOT NULL,
	[LogonPass] [nchar](32) NOT NULL,
	[InsurePass] [nchar](32) NOT NULL,
	[SecretKey] [nvarchar](32) NOT NULL,
	[EMail] [nvarchar](64) NULL,
	[MobilePhone] [nvarchar](21) NULL,
	[Gender] [tinyint] NOT NULL,
	[FaceID] [smallint] NOT NULL,
	[ActorID] [smallint] NOT NULL,
	[FaceFrameID] [smallint] NOT NULL,
	[ChatFrameID] [smallint] NOT NULL,
	[CardBackID] [smallint] NOT NULL,
	[TableBackID] [smallint] NOT NULL,
	[ProtectID] [int] NOT NULL,
	[CustomID] [int] NOT NULL,
	[ParentID] [int] NOT NULL,
	[UserRight] [int] NOT NULL,
	[MasterRight] [int] NOT NULL,
	[ServiceRight] [int] NOT NULL,
	[Experience] [int] NOT NULL,
	[LoveLiness] [int] NOT NULL,
	[GameOrder] [int] NOT NULL,
	[MasterOrder] [tinyint] NOT NULL,
	[MemberOrder] [tinyint] NOT NULL,
	[MemberPoint] [int] NOT NULL,
	[MemberOverTime] [int] NOT NULL,
	[WebLogonTimes] [int] NOT NULL,
	[GameLogonTimes] [int] NOT NULL,
	[PlayTimeCount] [int] NOT NULL,
	[OnlineTimeCount] [int] NOT NULL,
	[LogonIP] [nvarchar](15) NOT NULL,
	[LogonDate] [datetime] NOT NULL,
	[LogonMode] [smallint] NOT NULL,
	[LogonMobile] [nvarchar](15) NOT NULL,
	[LogonEMail] [nvarchar](32) NOT NULL,
	[LogonMachineID] [nvarchar](32) NOT NULL,
	[LogonDeviceKind] [int] NOT NULL,
	[RegisterIP] [nvarchar](15) NOT NULL,
	[RegisterDate] [datetime] NOT NULL,
	[RegisterMode] [smallint] NOT NULL,
	[RegisterMachineID] [nvarchar](32) NOT NULL,
	[RegisterDeviceKind] [int] NOT NULL,
	[RegisterPackageName] [nvarchar](64) NULL,
	[PlazaVersion] [int] NOT NULL,
	[MoorMachine] [tinyint] NOT NULL,
	[InfoPublic] [tinyint] NOT NULL,
	[IsRobot] [tinyint] NOT NULL,
	[IsValid] [tinyint] NOT NULL,
	[ValidUserDate] [datetime] NOT NULL,
	[MarketID] [smallint] NOT NULL,
	[ReleaseID] [int] NOT NULL,
	[LabelMask] [int] NOT NULL,
	[LikedNumber] [int] NOT NULL,
	[ActionMaskEver] [int] NOT NULL,
	[ActionMaskPerDay] [int] NOT NULL,
	[ActionMaskPerWeek] [int] NOT NULL,
	[ResetDateTime] [datetime] NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[SpreadChannelID] [int] NOT NULL,
	[AgentStatus] [bigint] NOT NULL,
	[AgentParentID] [int] NOT NULL,
 CONSTRAINT [PK_AccountsInfo_UserID] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsIngotInfo]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsIngotInfo](
	[UserID] [int] NOT NULL,
	[Score] [bigint] NOT NULL,
	[FillTimes] [int] NOT NULL,
	[FillAmount] [bigint] NOT NULL,
	[GrantTimes] [int] NOT NULL,
	[GrantAmount] [bigint] NOT NULL,
 CONSTRAINT [PK_AccountsIngotInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsInviteCode]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsInviteCode](
	[InviteCode] [int] NOT NULL,
	[UserID] [int] NOT NULL,
 CONSTRAINT [PK_AccountsInviteCode] PRIMARY KEY CLUSTERED 
(
	[InviteCode] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsMailAttach]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsMailAttach](
	[MailID] [bigint] NOT NULL,
	[GoodsID] [smallint] NOT NULL,
	[GoodsLevel] [smallint] NOT NULL,
	[GoodsCount] [int] NOT NULL,
	[GoodsIndate] [int] NOT NULL,
 CONSTRAINT [PK_AccountsMailAttach] PRIMARY KEY CLUSTERED 
(
	[MailID] ASC,
	[GoodsID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsMailBox]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsMailBox](
	[UserID] [int] NOT NULL,
	[MailID] [bigint] NOT NULL,
	[MailState] [tinyint] NOT NULL,
	[ExpireTime] [int] NOT NULL,
	[UpdateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_AccountsMailState] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC,
	[MailID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsMailInfo]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsMailInfo](
	[MailID] [bigint] IDENTITY(1,1) NOT NULL,
	[MailKind] [tinyint] NOT NULL,
	[MailType] [tinyint] NOT NULL,
	[MailTitle] [nvarchar](64) NOT NULL,
	[MailContent] [nvarchar](512) NOT NULL,
	[SendKind] [tinyint] NOT NULL,
	[SendTime] [int] NOT NULL,
	[ExpireTime] [int] NOT NULL,
	[CollectTime] [datetime] NOT NULL,
 CONSTRAINT [PK_AccountsMail] PRIMARY KEY CLUSTERED 
(
	[MailID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsMember]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsMember](
	[UserID] [int] NOT NULL,
	[MemberOrder] [tinyint] NOT NULL,
	[UserRight] [int] NOT NULL,
	[MemberOverDate] [datetime] NOT NULL,
 CONSTRAINT [PK_MemberInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC,
	[MemberOrder] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsOnlineInfo]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsOnlineInfo](
	[UserID] [int] NOT NULL,
	[GateID] [int] NOT NULL,
	[SocketID] [int] NOT NULL,
	[LogonID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[ServerID] [int] NOT NULL,
	[ServerKind] [smallint] NOT NULL,
	[InsertTime] [datetime] NOT NULL,
	[ModifyTime] [datetime] NOT NULL,
 CONSTRAINT [PK_AccountsOnlineInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsPack]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsPack](
	[UserID] [int] NOT NULL,
	[GoodsID] [int] NOT NULL,
	[GoodsKind] [tinyint] NOT NULL,
	[GoodsLevel] [smallint] NOT NULL,
	[GoodsCount] [int] NOT NULL,
	[GoodsIndate] [int] NOT NULL,
	[BindState] [tinyint] NOT NULL,
	[ExpireTime] [int] NOT NULL,
	[CollectTime] [datetime] NOT NULL,
 CONSTRAINT [PK_AccountsPack] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC,
	[GoodsID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsProtect]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsProtect](
	[ProtectID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[Question1] [nvarchar](32) NOT NULL,
	[Response1] [nvarchar](32) NOT NULL,
	[Question2] [nvarchar](32) NOT NULL,
	[Response2] [nvarchar](32) NOT NULL,
	[Question3] [nvarchar](32) NOT NULL,
	[Response3] [nvarchar](32) NOT NULL,
	[PassportID] [nvarchar](32) NOT NULL,
	[PassportType] [tinyint] NOT NULL,
	[SafeEmail] [nvarchar](32) NOT NULL,
	[CreateIP] [nvarchar](15) NOT NULL,
	[ModifyIP] [nvarchar](15) NOT NULL,
	[CreateDate] [datetime] NOT NULL,
	[ModifyDate] [datetime] NOT NULL,
 CONSTRAINT [PK_AccountsProtect] PRIMARY KEY CLUSTERED 
(
	[ProtectID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsShop]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsShop](
	[UserID] [int] NOT NULL,
	[ShopName] [nvarchar](30) NOT NULL,
	[ShopDescription] [nvarchar](30) NOT NULL,
	[LinkName1] [nvarchar](30) NOT NULL,
	[LinkContent1] [nvarchar](30) NOT NULL,
	[LinkName2] [nvarchar](30) NOT NULL,
	[LinkContent2] [nvarchar](30) NOT NULL,
	[SysShow] [tinyint] NOT NULL,
	[UserShow] [tinyint] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_AccountsShop_1] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsShopLink]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsShopLink](
	[LinkID] [int] IDENTITY(1,1) NOT NULL,
	[LinkName] [nvarchar](30) NOT NULL,
	[SortID] [int] NOT NULL,
	[Nullity] [tinyint] NOT NULL,
 CONSTRAINT [PK_AccountsShopLink] PRIMARY KEY CLUSTERED 
(
	[LinkID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsSpreadStat]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsSpreadStat](
	[UserID] [int] NOT NULL,
	[SpreadPayUser] [int] NOT NULL,
	[SparedPayAmount] [decimal](18, 2) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
	[SpreadViewCount] [int] NOT NULL,
	[SpreadLowerCount] [int] NOT NULL,
	[SpreadLLowerCount] [int] NOT NULL,
 CONSTRAINT [PK_AccountsCollect] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsStatus]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsStatus](
	[UserID] [int] NOT NULL,
	[ShutupStatus] [bit] NOT NULL,
	[ShutupOverTime] [int] NOT NULL,
	[ShutDownStatus] [bit] NOT NULL,
	[ShutDownOverTime] [int] NOT NULL,
	[LastShutupDateTime] [datetime] NULL,
	[LastShutDownDateTime] [datetime] NULL,
 CONSTRAINT [PK_AccountsStatus] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsSubsidyClaim]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsSubsidyClaim](
	[DateID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[ClaimTimes] [tinyint] NOT NULL,
	[ClaimGolds] [bigint] NOT NULL,
	[MachineID] [nvarchar](32) NOT NULL,
	[ModifyTime] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordUserBaseEnsure] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsTurntableLottery]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsTurntableLottery](
	[DateID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[MayLotteryTimes] [int] NOT NULL,
	[UseLotteryTimes] [int] NOT NULL,
	[PayLotteryTimes] [int] NOT NULL,
	[FreeLotteryTimes] [int] NOT NULL,
	[GameTimeLotteryTimes] [int] NOT NULL,
	[GameCountLotteryTimes] [int] NOT NULL,
	[GameExpendLotteryTimes] [int] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_AccountsTurntableInfo] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AccountsVisitor]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AccountsVisitor](
	[VisiteID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[MachineID] [nvarchar](32) NOT NULL,
	[MachineName] [nvarchar](32) NOT NULL,
	[PackageName] [nvarchar](64) NOT NULL,
	[LogonClientIP] [nvarchar](15) NOT NULL,
	[LogonDateTime] [datetime] NOT NULL,
	[CreateDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_AccountsVisite_VisiteID] PRIMARY KEY CLUSTERED 
(
	[VisiteID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AgentIncome]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AgentIncome](
	[AgentID] [int] NOT NULL,
	[Income] [decimal](18, 2) NOT NULL,
	[IncomeType] [tinyint] NOT NULL,
	[PayAmount] [decimal](18, 2) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
	[RecordID] [bigint] IDENTITY(1,1) NOT NULL,
	[AgentLevel] [int] NOT NULL,
	[ContributeAgentID] [int] NOT NULL,
	[ContributeUserID] [int] NOT NULL,
	[PayOrderID] [bigint] NOT NULL,
	[SettleWeeklD] [int] NOT NULL,
 CONSTRAINT [PK_AgentIncome] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AgentInfo]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AgentInfo](
	[UserID] [int] NOT NULL,
	[IncomeType] [tinyint] NOT NULL,
	[IncomePayRate] [decimal](18, 2) NOT NULL,
	[IncomePeopleAmount] [decimal](18, 2) NOT NULL,
	[AgentLevel] [int] NOT NULL,
	[AllowLowerStatus] [tinyint] NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_AgentInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AgentLogonRecord]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AgentLogonRecord](
	[RecordID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[DateID] [int] NOT NULL,
	[ClientIP] [nvarchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_AgentLogonRecord] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[BlackListAccounts]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[BlackListAccounts](
	[ListID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
 CONSTRAINT [PK_BlackListAccounts] PRIMARY KEY CLUSTERED 
(
	[ListID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[BlackListIP]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[BlackListIP](
	[ListID] [int] IDENTITY(1,1) NOT NULL,
	[IPAddress] [nvarchar](30) NOT NULL,
 CONSTRAINT [PK_BlackListIP] PRIMARY KEY CLUSTERED 
(
	[ListID] ASC,
	[IPAddress] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[BlackListKind]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[BlackListKind](
	[ListID] [int] IDENTITY(100,1) NOT NULL,
	[KindID] [int] NOT NULL,
	[StartDate] [datetime] NOT NULL,
	[EndDate] [datetime] NOT NULL,
	[ProcessState] [int] NOT NULL,
 CONSTRAINT [PK_BlackListKind] PRIMARY KEY CLUSTERED 
(
	[ListID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[BlackListPhone]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[BlackListPhone](
	[ListID] [int] IDENTITY(1,1) NOT NULL,
	[MobilePhone] [nvarchar](30) NOT NULL,
 CONSTRAINT [PK_BlackListPhone] PRIMARY KEY CLUSTERED 
(
	[ListID] ASC,
	[MobilePhone] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ChannelInfo]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ChannelInfo](
	[ChannelID] [int] NOT NULL,
	[ChannelName] [nvarchar](32) NOT NULL,
	[ChannelLable] [nvarchar](32) NOT NULL,
	[Accounts] [nvarchar](32) NOT NULL,
	[LoginPass] [nvarchar](32) NOT NULL,
	[ParentID] [int] NOT NULL,
	[ChannelRate] [decimal](18, 3) NOT NULL,
	[LastLoginDateTime] [datetime] NULL,
	[LastLoginIP] [nvarchar](15) NULL,
	[PreLoginIP] [nvarchar](15) NULL,
	[PreLoginDateTime] [datetime] NULL,
	[LoginCount] [int] NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_ChannelInfo] PRIMARY KEY CLUSTERED 
(
	[ChannelID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ChannelLoginRecord]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ChannelLoginRecord](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[ChannelID] [int] NOT NULL,
	[LoginIP] [nvarchar](15) NOT NULL,
	[LoginDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_ChannelLoginRecord] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ChannelPackage]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ChannelPackage](
	[MarketID] [int] NOT NULL,
	[ChannelID] [int] NOT NULL,
	[PackageName] [nvarchar](64) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
	[SpreadMarketID] [int] NOT NULL,
	[TypeID] [int] NOT NULL,
	[AndroidDownloadUrl] [nvarchar](256) NOT NULL,
 CONSTRAINT [PK_ChannelPackage] PRIMARY KEY CLUSTERED 
(
	[MarketID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ConfineAddress]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ConfineAddress](
	[ConfineID] [int] IDENTITY(1,1) NOT NULL,
	[AddrString] [nvarchar](15) NOT NULL,
	[EnjoinLogon] [bit] NOT NULL,
	[EnjoinRegister] [bit] NOT NULL,
	[EnjoinOverDate] [datetime] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[CollectNote] [nvarchar](32) NOT NULL,
	[MasterID] [int] NOT NULL,
 CONSTRAINT [PK_ConfineAddress] PRIMARY KEY CLUSTERED 
(
	[ConfineID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ConfineContent]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ConfineContent](
	[ConfineID] [int] IDENTITY(1,1) NOT NULL,
	[String] [nvarchar](31) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[MasterID] [int] NOT NULL,
 CONSTRAINT [PK_ConfineContent] PRIMARY KEY CLUSTERED 
(
	[ConfineID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ConfineMachine]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ConfineMachine](
	[ConfineID] [int] IDENTITY(1,1) NOT NULL,
	[MachineSerial] [nvarchar](32) NOT NULL,
	[EnjoinLogon] [bit] NOT NULL,
	[EnjoinRegister] [bit] NOT NULL,
	[EnjoinOverDate] [datetime] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[CollectNote] [nvarchar](32) NOT NULL,
	[MasterID] [int] NOT NULL,
 CONSTRAINT [PK_ConfineMachine] PRIMARY KEY CLUSTERED 
(
	[ConfineID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GameIdentifier]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GameIdentifier](
	[UserID] [int] IDENTITY(1,1) NOT NULL,
	[GameID] [int] NOT NULL,
	[IDLevel] [int] NOT NULL,
 CONSTRAINT [PK_GameIdentifier_UserID] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[MobileAuthInfo]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[MobileAuthInfo](
	[UserID] [int] NOT NULL,
	[AuthTimes] [smallint] NOT NULL,
	[AcquireTimes] [smallint] NOT NULL,
	[LastAcquireDate] [datetime] NOT NULL,
	[LastMobilePhone] [nvarchar](21) NOT NULL,
 CONSTRAINT [PK_MobileAuthInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[PendingMailUser]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[PendingMailUser](
	[MailID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
 CONSTRAINT [PK_PendingMailQueue] PRIMARY KEY CLUSTERED 
(
	[MailID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[PlazaVersionCollect]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[PlazaVersionCollect](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[DateID] [int] NOT NULL,
	[PlazaVersion] [int] NOT NULL,
	[LogonCount] [int] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_PlazaVersionCollect] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordAuthCode]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordAuthCode](
	[CodeKind] [tinyint] NOT NULL,
	[MobileEmail] [nvarchar](64) NOT NULL,
	[UserID] [int] NOT NULL,
	[AuthKind] [tinyint] NOT NULL,
	[AuthCode] [nchar](6) NOT NULL,
	[AuthCodeMD5] [nchar](32) NOT NULL,
	[AuthTimes] [smallint] NOT NULL,
	[AcquireTimes] [smallint] NOT NULL,
	[AcquireClientIP] [nvarchar](15) NOT NULL,
	[AcquireMachine] [nvarchar](32) NOT NULL,
	[LastAcquireDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordAuthCode] PRIMARY KEY CLUSTERED 
(
	[CodeKind] ASC,
	[MobileEmail] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordAuthTimes]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordAuthTimes](
	[DateID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[CodeKind] [tinyint] NOT NULL,
	[AuthKind] [tinyint] NOT NULL,
	[AuthTimes] [smallint] NOT NULL,
 CONSTRAINT [PK_RecordAuthTimes] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[UserID] ASC,
	[CodeKind] ASC,
	[AuthKind] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordBindEMail]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordBindEMail](
	[UserID] [int] NOT NULL,
	[EMail] [nvarchar](32) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordBindMobilePhone_copy1] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordBindMobilePhone]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordBindMobilePhone](
	[UserID] [int] NOT NULL,
	[MobilePhone] [nvarchar](21) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordBindMobilePhone] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordBindParent]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordBindParent](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[ParentID] [int] NOT NULL,
	[TypeID] [int] NOT NULL,
	[ClientIP] [nvarchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_AccountsBindInfo] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordGrantInfo]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordGrantInfo](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[PreScore] [bigint] NOT NULL,
	[PreInsureScore] [bigint] NOT NULL,
	[GrantScore] [bigint] NOT NULL,
	[GrantType] [int] NOT NULL,
	[GrantIP] [nvarchar](15) NOT NULL,
	[GrantDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordGrantInfo] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordInform]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordInform](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[InformUserID] [int] NOT NULL,
	[InformKindID] [tinyint] NOT NULL,
	[InformSource] [tinyint] NOT NULL,
	[InformMessage] [nvarchar](512) NOT NULL,
	[InformContent] [nvarchar](512) NOT NULL,
	[BeInformUserID] [int] NOT NULL,
	[BeInformGameID] [int] NOT NULL,
	[AttachmentUrl] [nvarchar](256) NOT NULL,
	[ClientIP] [nvarchar](15) NOT NULL,
	[CollectTime] [datetime] NOT NULL,
	[Status] [tinyint] NOT NULL,
	[HandleResult] [tinyint] NOT NULL,
	[PunishmentTime] [int] NOT NULL,
	[HandleRemark] [nvarchar](512) NOT NULL,
	[HandleMasterID] [int] NOT NULL,
	[HandleTime] [datetime] NULL,
 CONSTRAINT [PK_AccountsInform] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordLogonInfo]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordLogonInfo](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[DateID] [int] NOT NULL,
	[DeviceKind] [int] NOT NULL,
	[PlazaVersion] [int] NOT NULL,
	[ClientIP] [nvarchar](15) NOT NULL,
	[MachineID] [nvarchar](32) NOT NULL,
	[MachineOSID] [nvarchar](64) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordLogon_RecordID] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordNeedClearGold]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordNeedClearGold](
	[UserID] [int] NOT NULL,
	[WithdrawDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordNeedcClearGold] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordNeedDeductScore]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordNeedDeductScore](
	[UserID] [int] NOT NULL,
	[DeductScore] [bigint] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_BlaickListKindWinner_1] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordPresentClientIP]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordPresentClientIP](
	[ClientIP] [nvarchar](15) NOT NULL,
	[DateID] [int] NOT NULL,
	[PresentGold] [bigint] NOT NULL,
	[PresentIngot] [bigint] NOT NULL,
	[PresentCount] [int] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordPresentClientIP] PRIMARY KEY CLUSTERED 
(
	[ClientIP] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordPresentMachine]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordPresentMachine](
	[MachineID] [nvarchar](32) NOT NULL,
	[PresentGold] [bigint] NOT NULL,
	[PresentIngot] [int] NOT NULL,
	[PresentCount] [int] NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
	[DateID] [int] NOT NULL,
 CONSTRAINT [PK_RecordPresentMachine] PRIMARY KEY CLUSTERED 
(
	[MachineID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordSubsidyMachine]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordSubsidyMachine](
	[DateID] [int] NOT NULL,
	[MachineID] [nvarchar](32) NOT NULL,
	[PresentGold] [bigint] NOT NULL,
	[PresentCount] [int] NOT NULL,
	[UserIDString] [nvarchar](512) NOT NULL,
	[FirstGrantDate] [datetime] NOT NULL,
	[LastGrantDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordSubsidyMachine_1] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[MachineID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordTurntableClientIP]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordTurntableClientIP](
	[DateID] [int] NOT NULL,
	[ClientIP] [nvarchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordTurntableIPAddress] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[ClientIP] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RecordTurntableMachine]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RecordTurntableMachine](
	[DateID] [int] NOT NULL,
	[MachineID] [nvarchar](32) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_RecordTurntableMachine] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[MachineID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReserveIdentifier]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReserveIdentifier](
	[GameID] [int] NOT NULL,
	[IDLevel] [int] NOT NULL,
	[Distribute] [bit] NOT NULL,
 CONSTRAINT [PK_ReserveIdentifier_GameID] PRIMARY KEY CLUSTERED 
(
	[GameID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReserveIDSellInfo]    Script Date: 2023/7/12 11:03:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReserveIDSellInfo](
	[GameID] [int] NOT NULL,
	[TypeID] [int] NOT NULL,
	[SortID] [int] NOT NULL,
	[Price] [int] NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[ApplyUserID] [int] NULL,
	[ApplyDate] [datetime] NULL,
	[OperUserID] [int] NULL,
	[IPAddress] [nvarchar](15) NULL,
	[CollectDate] [datetime] NOT NULL,
	[CollectNote] [nvarchar](50) NOT NULL,
 CONSTRAINT [PK_ReserveIDSellInfo] PRIMARY KEY CLUSTERED 
(
	[GameID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ReserveIDTypeInfo]    Script Date: 2023/7/12 11:03:38 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReserveIDTypeInfo](
	[TypeID] [int] IDENTITY(1,1) NOT NULL,
	[TypeName] [nvarchar](30) NOT NULL,
	[SortID] [int] NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_ReserveIDTypeInfo] PRIMARY KEY CLUSTERED 
(
	[TypeID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RobotConfigure]    Script Date: 2023/7/12 11:03:38 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RobotConfigure](
	[BatchID] [int] IDENTITY(1,1) NOT NULL,
	[ServerID] [int] NOT NULL,
	[ServiceMode] [int] NOT NULL,
	[RobotCount] [int] NOT NULL,
	[EnterTime] [int] NOT NULL,
	[LeaveTime] [int] NOT NULL,
	[EnterMinInterval] [int] NOT NULL,
	[EnterMaxInterval] [int] NOT NULL,
	[LeaveMinInterval] [int] NOT NULL,
	[LeaveMaxInterval] [int] NOT NULL,
	[TakeMinScore] [bigint] NOT NULL,
	[TakeMaxScore] [bigint] NOT NULL,
	[SwitchMinInnings] [int] NOT NULL,
	[SwitchMaxInnings] [int] NOT NULL,
 CONSTRAINT [PK_AndroidConfigure_1] PRIMARY KEY CLUSTERED 
(
	[BatchID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RobotLockInfo]    Script Date: 2023/7/12 11:03:38 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RobotLockInfo](
	[UserID] [int] NOT NULL,
	[LogonPass] [nchar](32) NOT NULL,
	[RobotStatus] [tinyint] NOT NULL,
	[ServerID] [int] NOT NULL,
	[BatchID] [int] NOT NULL,
	[LockDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_AndroidLockInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[StreamGrantInfo]    Script Date: 2023/7/12 11:03:38 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[StreamGrantInfo](
	[DateID] [int] NOT NULL,
	[TypeID] [int] NOT NULL,
	[GrantScoreStat] [bigint] NOT NULL,
	[FirstGrantDate] [datetime] NOT NULL,
	[LastGrantDate] [datetime] NOT NULL,
 CONSTRAINT [PK_StreamGrantInfo_1] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[TypeID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[SystemStreamInfo]    Script Date: 2023/7/12 11:03:38 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[SystemStreamInfo](
	[DateID] [int] NOT NULL,
	[StationID] [int] NOT NULL,
	[WebLogonSuccess] [int] NOT NULL,
	[WebRegisterSuccess] [int] NOT NULL,
	[GameLogonSuccess] [int] NOT NULL,
	[GameRegisterSuccess] [int] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_SystemStreamInfo_ID] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[StationID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[SystemStreamLogon]    Script Date: 2023/7/12 11:03:38 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[SystemStreamLogon](
	[DateID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[FillCount] [smallint] NOT NULL,
	[FillAmount] [int] NOT NULL,
	[LogonCount] [int] NOT NULL,
	[OnlineTimeCount] [int] NOT NULL,
	[FirstDate] [datetime] NOT NULL,
	[LastDate] [datetime] NOT NULL,
 CONSTRAINT [PK_SystemStreamLogon] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[SystemStreamRegister]    Script Date: 2023/7/12 11:03:38 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[SystemStreamRegister](
	[DateID] [int] NOT NULL,
	[DeviceKind] [smallint] NOT NULL,
	[ExternalID] [int] NOT NULL,
	[TotalCount] [int] NOT NULL,
	[FirstDate] [datetime] NOT NULL,
	[LastDate] [datetime] NOT NULL,
 CONSTRAINT [PK_SystemStreamRegister_1] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[DeviceKind] ASC,
	[ExternalID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[TurntableLotteryHitStat]    Script Date: 2023/7/12 11:03:38 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[TurntableLotteryHitStat](
	[DateID] [int] NOT NULL,
	[ItemIndex] [int] NOT NULL,
	[HitTimes] [int] NOT NULL,
 CONSTRAINT [PK_RecordTurntableHitStat] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[ItemIndex] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[TurntableLotteryWriteList]    Script Date: 2023/7/12 11:03:38 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[TurntableLotteryWriteList](
	[UserID] [int] NOT NULL,
	[HitIndex] [int] NOT NULL,
	[Remark] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_TurntableLotteryWriteList] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[UserRegister]    Script Date: 2023/7/12 11:03:38 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[UserRegister](
	[DateID] [int] NOT NULL,
	[MarketID] [nchar](10) NOT NULL,
	[UserID] [int] NOT NULL,
 CONSTRAINT [PK_UserRegister] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[MarketID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
ALTER TABLE [dbo].[AccountsAgentInfo] ADD  CONSTRAINT [DF_AccountsAgentInfo_LowerBetAwardRate]  DEFAULT ((0)) FOR [LowerBetAwardRate]
GO
ALTER TABLE [dbo].[AccountsAgentInfo] ADD  CONSTRAINT [DF_AccountsAgentInfo_LowerPayAwardAmount]  DEFAULT ((0)) FOR [LowerPayAwardAmount]
GO
ALTER TABLE [dbo].[AccountsAgentInfo] ADD  CONSTRAINT [DF_Table_1_LowerBetAwardRate1]  DEFAULT ((0)) FOR [LLowerBetAwardRate]
GO
ALTER TABLE [dbo].[AccountsAgentInfo] ADD  CONSTRAINT [DF_AccountsAgentInfo_LLowerPayAwardAmount]  DEFAULT ((0)) FOR [LLowerPayAwardAmount]
GO
ALTER TABLE [dbo].[AccountsAuthInfo] ADD  CONSTRAINT [DF_AccountsAuthInfo_CodeKind]  DEFAULT ((0)) FOR [CodeKind]
GO
ALTER TABLE [dbo].[AccountsAuthInfo] ADD  CONSTRAINT [DF_AccountsAuthInfo_LastAuthDate]  DEFAULT (getdate()) FOR [LastAcquireDate]
GO
ALTER TABLE [dbo].[AccountsCommission] ADD  CONSTRAINT [DF_UserBrokerageInfo_Brokerage]  DEFAULT ((0)) FOR [Commission]
GO
ALTER TABLE [dbo].[AccountsCommission] ADD  CONSTRAINT [DF_AccountsCommission_TotalPayAmount]  DEFAULT ((0)) FOR [TotalPayAmount]
GO
ALTER TABLE [dbo].[AccountsCommission] ADD  CONSTRAINT [DF_AccountsCommission_TotalBetAmount]  DEFAULT ((0)) FOR [TotalBetAmount]
GO
ALTER TABLE [dbo].[AccountsCommission] ADD  CONSTRAINT [DF_AccountsCommission_TotalPayContribute]  DEFAULT ((0)) FOR [TotalPayContribute]
GO
ALTER TABLE [dbo].[AccountsCommission] ADD  CONSTRAINT [DF_AccountsCommission_TotalBetContribute]  DEFAULT ((0)) FOR [TotalBetContribute]
GO
ALTER TABLE [dbo].[AccountsCommission] ADD  CONSTRAINT [DF_GameBrokerage_LowerPayAmount]  DEFAULT ((0)) FOR [LowerPayAmount]
GO
ALTER TABLE [dbo].[AccountsCommission] ADD  CONSTRAINT [DF_GameBrokerage_LowerGameAmount]  DEFAULT ((0)) FOR [LowerBetAmount]
GO
ALTER TABLE [dbo].[AccountsCommission] ADD  CONSTRAINT [DF_GameBrokerage_LLowerPayAmount]  DEFAULT ((0)) FOR [LLowerPayAmount]
GO
ALTER TABLE [dbo].[AccountsCommission] ADD  CONSTRAINT [DF_GameBrokerage_LLowerGameAmount]  DEFAULT ((0)) FOR [LLowerBetAmount]
GO
ALTER TABLE [dbo].[AccountsCommission] ADD  CONSTRAINT [DF_Table_1_SpreadObtain]  DEFAULT ((0)) FOR [LowerPayContribute]
GO
ALTER TABLE [dbo].[AccountsCommission] ADD  CONSTRAINT [DF_Table_1_AnteObtain]  DEFAULT ((0)) FOR [LowerBetContribute]
GO
ALTER TABLE [dbo].[AccountsCommission] ADD  CONSTRAINT [DF_Table_1_IndirectSpreadObtain]  DEFAULT ((0)) FOR [LLowerPayContribute]
GO
ALTER TABLE [dbo].[AccountsCommission] ADD  CONSTRAINT [DF_Table_1_IndirectAnteObtain]  DEFAULT ((0)) FOR [LLowerBetContribute]
GO
ALTER TABLE [dbo].[AccountsCommission] ADD  CONSTRAINT [DF_AccountsCommission_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[AccountsDatum] ADD  CONSTRAINT [DF_IndividualDatum_Compellation]  DEFAULT ('') FOR [Compellation]
GO
ALTER TABLE [dbo].[AccountsDatum] ADD  CONSTRAINT [DF_IndividualDatum_Age]  DEFAULT ((0)) FOR [Age]
GO
ALTER TABLE [dbo].[AccountsDatum] ADD  CONSTRAINT [DF_IndividualDatum_Area]  DEFAULT ('') FOR [Area]
GO
ALTER TABLE [dbo].[AccountsDatum] ADD  CONSTRAINT [DF_IndividualDatum_City]  DEFAULT ('') FOR [City]
GO
ALTER TABLE [dbo].[AccountsDatum] ADD  CONSTRAINT [DF_IndividualDatum_Province]  DEFAULT ('') FOR [Province]
GO
ALTER TABLE [dbo].[AccountsDatum] ADD  CONSTRAINT [DF_IndividualDatum_SeatPhone]  DEFAULT ('') FOR [SeatPhone]
GO
ALTER TABLE [dbo].[AccountsDatum] ADD  CONSTRAINT [DF_IndividualDatum_MobilePhone]  DEFAULT ('') FOR [MobilePhone]
GO
ALTER TABLE [dbo].[AccountsDatum] ADD  CONSTRAINT [DF_IndividualDatum_DwellingPlace]  DEFAULT ('') FOR [DwellingPlace]
GO
ALTER TABLE [dbo].[AccountsDatum] ADD  CONSTRAINT [DF_IndividualDatum_PostalCode]  DEFAULT ('') FOR [PostalCode]
GO
ALTER TABLE [dbo].[AccountsDatum] ADD  CONSTRAINT [DF_IndividualDatum_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[AccountsDatum] ADD  CONSTRAINT [DF_IndividualDatum_UserNote]  DEFAULT ('') FOR [UserNote]
GO
ALTER TABLE [dbo].[AccountsExternal] ADD  CONSTRAINT [DF_AccountsThirdParty_PlatformID]  DEFAULT ((1)) FOR [ExternalID]
GO
ALTER TABLE [dbo].[AccountsExternal] ADD  CONSTRAINT [DF_AccountsWeChat_PublicID]  DEFAULT (N'') FOR [PublicID]
GO
ALTER TABLE [dbo].[AccountsExternal] ADD  CONSTRAINT [DF_AccountsThirdParty_UserDatum]  DEFAULT (N'') FOR [UserDatum]
GO
ALTER TABLE [dbo].[AccountsExternal] ADD  CONSTRAINT [DF_AccountsWeChat_HeadImageUrl]  DEFAULT (N'') FOR [UserFacePath]
GO
ALTER TABLE [dbo].[AccountsExternal] ADD  CONSTRAINT [DF_AccountsWeChat_LogonClientIP]  DEFAULT (N'') FOR [LogonClientIP]
GO
ALTER TABLE [dbo].[AccountsExternal] ADD  CONSTRAINT [DF_AccountsWeChat_LogonDateTime]  DEFAULT (getdate()) FOR [LogonDateTime]
GO
ALTER TABLE [dbo].[AccountsExternal] ADD  CONSTRAINT [DF_AccountsWeChat_Nullity_1]  DEFAULT ((1)) FOR [SubscribeFlag]
GO
ALTER TABLE [dbo].[AccountsExternal] ADD  CONSTRAINT [DF_AccountsWeChat_SubscribeTime]  DEFAULT (getdate()) FOR [SubscribeTime]
GO
ALTER TABLE [dbo].[AccountsExternal] ADD  CONSTRAINT [DF_AccountsWXInfo_CollectDate]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[AccountsFace] ADD  CONSTRAINT [DF_AccountsFace_CustomID]  DEFAULT ((0)) FOR [CustomID]
GO
ALTER TABLE [dbo].[AccountsFace] ADD  CONSTRAINT [DF_AccountsFace_FacePath]  DEFAULT ('') FOR [FacePath]
GO
ALTER TABLE [dbo].[AccountsFace] ADD  CONSTRAINT [DF_AccountsFace_CustomTime]  DEFAULT (getdate()) FOR [UpdateTime]
GO
ALTER TABLE [dbo].[AccountsGoldInfo] ADD  CONSTRAINT [DF_AccountsGoldInfo_Score]  DEFAULT ((0)) FOR [Score]
GO
ALTER TABLE [dbo].[AccountsGoldInfo] ADD  CONSTRAINT [DF_AccountsGoldInfo_FillTimes]  DEFAULT ((0)) FOR [FillTimes]
GO
ALTER TABLE [dbo].[AccountsGoldInfo] ADD  CONSTRAINT [DF_AccountsGoldInfo_FillAmount]  DEFAULT ((0)) FOR [FillAmount]
GO
ALTER TABLE [dbo].[AccountsGoldInfo] ADD  CONSTRAINT [DF_AccountsGoldInfo_InsureScore_1]  DEFAULT ((0)) FOR [InsureScore]
GO
ALTER TABLE [dbo].[AccountsGoldInfo] ADD  CONSTRAINT [DF_Table_1_GrantSysCount]  DEFAULT ((0)) FOR [GrantTimes]
GO
ALTER TABLE [dbo].[AccountsGoldInfo] ADD  CONSTRAINT [DF_AccountsGoldInfo_GrantCount1]  DEFAULT ((0)) FOR [GrantAmount]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_GameID]  DEFAULT ((0)) FOR [GameID]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_PasswordID]  DEFAULT ((0)) FOR [PasswordID]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_NickName]  DEFAULT (N'') FOR [NickName]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_UserAccounts_UnderWrite]  DEFAULT ('') FOR [UnderWrite]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_LogonToken]  DEFAULT (N'') FOR [SecretKey]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_MobilePhone1]  DEFAULT ('') FOR [EMail]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_MobilePhone]  DEFAULT ('') FOR [MobilePhone]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_UserAccounts_Gender]  DEFAULT (N'') FOR [Gender]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_UserAccounts_FaceID]  DEFAULT ((0)) FOR [FaceID]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_ActorID]  DEFAULT ((1)) FOR [ActorID]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_FrameID]  DEFAULT ((1)) FOR [FaceFrameID]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_ChatFrameID]  DEFAULT ((1)) FOR [ChatFrameID]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_CardBackID]  DEFAULT ((1)) FOR [CardBackID]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_GameBackID]  DEFAULT ((1)) FOR [TableBackID]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_UserAccounts_ProtectID]  DEFAULT ((0)) FOR [ProtectID]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_CustomID]  DEFAULT ((0)) FOR [CustomID]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_ParentID]  DEFAULT ((0)) FOR [ParentID]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_UserRight]  DEFAULT ((0)) FOR [UserRight]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_MasterRight]  DEFAULT ((0)) FOR [MasterRight]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_ServiceRight]  DEFAULT ((0)) FOR [ServiceRight]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_UserAccounts_Experience]  DEFAULT ((0)) FOR [Experience]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_LoveLiness]  DEFAULT ((0)) FOR [LoveLiness]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_GameLevel]  DEFAULT ((1)) FOR [GameOrder]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_MasterOrder]  DEFAULT ((0)) FOR [MasterOrder]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_UserAccounts_MemberOrder]  DEFAULT ((0)) FOR [MemberOrder]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_UserAccounts_MemberOverDate]  DEFAULT ((0)) FOR [MemberPoint]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_MemberSwitchDate]  DEFAULT ((0)) FOR [MemberOverTime]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_WebLogonTimes]  DEFAULT ((0)) FOR [WebLogonTimes]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_UserAccounts_AllLogonTimes]  DEFAULT ((0)) FOR [GameLogonTimes]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_PlayTimeCount]  DEFAULT ((0)) FOR [PlayTimeCount]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_OnLineTimeCount]  DEFAULT ((0)) FOR [OnlineTimeCount]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_UserAccounts_LastLogonDate]  DEFAULT (getdate()) FOR [LogonDate]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_LogonMode]  DEFAULT ((0)) FOR [LogonMode]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_LastLogonMobile]  DEFAULT ('') FOR [LogonMobile]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_LogonMobile1]  DEFAULT ('') FOR [LogonEMail]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_MachineSerial]  DEFAULT ('------------') FOR [LogonMachineID]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_RegisterDeviceKind1]  DEFAULT ((0)) FOR [LogonDeviceKind]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_UserAccounts_RegisterDate]  DEFAULT (getdate()) FOR [RegisterDate]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF__AccountsI__Regis__12BEA5E7]  DEFAULT ((0)) FOR [RegisterMode]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_RegisterMachine]  DEFAULT (N'------------') FOR [RegisterMachineID]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF__AccountsI__Regis__13B2CA20]  DEFAULT ((0)) FOR [RegisterDeviceKind]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_RegisterPackageName]  DEFAULT (N'') FOR [RegisterPackageName]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_PlazaVersion]  DEFAULT ((0)) FOR [PlazaVersion]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_MoorMachine]  DEFAULT ((0)) FOR [MoorMachine]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_InfoPublic]  DEFAULT ((0)) FOR [InfoPublic]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_IsAndroid]  DEFAULT ((0)) FOR [IsRobot]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_IsValid]  DEFAULT ((0)) FOR [IsValid]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_ValidUserDate]  DEFAULT ('1900-01-01') FOR [ValidUserDate]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_MarketID]  DEFAULT ((0)) FOR [MarketID]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_ReleaseID]  DEFAULT ((0)) FOR [ReleaseID]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_LabelMask]  DEFAULT ((0)) FOR [LabelMask]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_LikedNumber]  DEFAULT ((0)) FOR [LikedNumber]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_ActionMaskEver]  DEFAULT ((0)) FOR [ActionMaskEver]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_ActionMaskPerDay_1]  DEFAULT ((0)) FOR [ActionMaskPerDay]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_ActionMaskPerWeek]  DEFAULT ((0)) FOR [ActionMaskPerWeek]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_ResetDateTime]  DEFAULT (getdate()) FOR [ResetDateTime]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_UserAccounts_ServiceNullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_SpreadChannelID]  DEFAULT ((0)) FOR [SpreadChannelID]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_AgentStatus]  DEFAULT ((0)) FOR [AgentStatus]
GO
ALTER TABLE [dbo].[AccountsInfo] ADD  CONSTRAINT [DF_AccountsInfo_AgentParentID]  DEFAULT ((0)) FOR [AgentParentID]
GO
ALTER TABLE [dbo].[AccountsIngotInfo] ADD  CONSTRAINT [DF_AccountsWealth_Ingot]  DEFAULT ((0)) FOR [Score]
GO
ALTER TABLE [dbo].[AccountsIngotInfo] ADD  CONSTRAINT [DF_AccountsWealth_FillIngot]  DEFAULT ((0)) FOR [FillTimes]
GO
ALTER TABLE [dbo].[AccountsIngotInfo] ADD  CONSTRAINT [DF_AccountsIngotInfo_FillAmount]  DEFAULT ((0)) FOR [FillAmount]
GO
ALTER TABLE [dbo].[AccountsIngotInfo] ADD  CONSTRAINT [DF_AccountsWealth_FillIngot1]  DEFAULT ((0)) FOR [GrantTimes]
GO
ALTER TABLE [dbo].[AccountsIngotInfo] ADD  CONSTRAINT [DF_AccountsIngotInfo_GrantCount1]  DEFAULT ((0)) FOR [GrantAmount]
GO
ALTER TABLE [dbo].[AccountsMailAttach] ADD  CONSTRAINT [DF_AccountsMailAttach_GoodsLevel]  DEFAULT ((0)) FOR [GoodsLevel]
GO
ALTER TABLE [dbo].[AccountsMailInfo] ADD  CONSTRAINT [DF_AccountsMail_MailKind]  DEFAULT ((0)) FOR [MailKind]
GO
ALTER TABLE [dbo].[AccountsMailInfo] ADD  CONSTRAINT [DF_AccountsMailInfo_MailTypeID]  DEFAULT ((0)) FOR [MailType]
GO
ALTER TABLE [dbo].[AccountsMailInfo] ADD  CONSTRAINT [DF_AccountsMailInfo_SendKind]  DEFAULT ((2)) FOR [SendKind]
GO
ALTER TABLE [dbo].[AccountsMember] ADD  CONSTRAINT [DF_MemberInfo_UserRight]  DEFAULT ((0)) FOR [UserRight]
GO
ALTER TABLE [dbo].[AccountsMember] ADD  CONSTRAINT [DF_MemberInfo_MemberOverDate]  DEFAULT (getdate()) FOR [MemberOverDate]
GO
ALTER TABLE [dbo].[AccountsOnlineInfo] ADD  CONSTRAINT [DF_AccountsOnlineInfo_GateID]  DEFAULT ((0)) FOR [GateID]
GO
ALTER TABLE [dbo].[AccountsOnlineInfo] ADD  CONSTRAINT [DF_AccountsOnlineInfo_SocketID]  DEFAULT ((0)) FOR [SocketID]
GO
ALTER TABLE [dbo].[AccountsOnlineInfo] ADD  CONSTRAINT [DF_AccountsOnlineInfo_LogonID]  DEFAULT ((0)) FOR [LogonID]
GO
ALTER TABLE [dbo].[AccountsOnlineInfo] ADD  CONSTRAINT [DF_AccountsOnlineInfo_KindID]  DEFAULT ((0)) FOR [KindID]
GO
ALTER TABLE [dbo].[AccountsOnlineInfo] ADD  CONSTRAINT [DF_AccountsOnlineInfo_ServerID]  DEFAULT ((0)) FOR [ServerID]
GO
ALTER TABLE [dbo].[AccountsOnlineInfo] ADD  CONSTRAINT [DF_AccountsOnlineInfo_ServerKind]  DEFAULT ((0)) FOR [ServerKind]
GO
ALTER TABLE [dbo].[AccountsOnlineInfo] ADD  CONSTRAINT [DF_AccountsOnlineInfo_InsertTime]  DEFAULT (getdate()) FOR [InsertTime]
GO
ALTER TABLE [dbo].[AccountsOnlineInfo] ADD  CONSTRAINT [DF_AccountsOnlineInfo_ModifyTime]  DEFAULT (getdate()) FOR [ModifyTime]
GO
ALTER TABLE [dbo].[AccountsPack] ADD  CONSTRAINT [DF_AccountsPack_GoodID]  DEFAULT ((0)) FOR [GoodsID]
GO
ALTER TABLE [dbo].[AccountsPack] ADD  CONSTRAINT [DF_AccountsPack_GoodsKind]  DEFAULT ((0)) FOR [GoodsKind]
GO
ALTER TABLE [dbo].[AccountsPack] ADD  CONSTRAINT [DF_AccountsPack_GoodsIndate1]  DEFAULT ((1)) FOR [GoodsLevel]
GO
ALTER TABLE [dbo].[AccountsPack] ADD  CONSTRAINT [DF_AccountsPack_GoodCount]  DEFAULT ((0)) FOR [GoodsCount]
GO
ALTER TABLE [dbo].[AccountsPack] ADD  CONSTRAINT [DF_AccountsPack_GoodsIndate]  DEFAULT ((0)) FOR [GoodsIndate]
GO
ALTER TABLE [dbo].[AccountsPack] ADD  CONSTRAINT [DF_AccountsPack_BindState]  DEFAULT ((0)) FOR [BindState]
GO
ALTER TABLE [dbo].[AccountsPack] ADD  CONSTRAINT [DF_AccountsPack_ExpiryTimestamp]  DEFAULT ((0)) FOR [ExpireTime]
GO
ALTER TABLE [dbo].[AccountsPack] ADD  CONSTRAINT [DF_AccountsPack_CollectDateTime]  DEFAULT (getdate()) FOR [CollectTime]
GO
ALTER TABLE [dbo].[AccountsProtect] ADD  CONSTRAINT [DF_AccountsProtect_CreateDate]  DEFAULT (getdate()) FOR [CreateDate]
GO
ALTER TABLE [dbo].[AccountsProtect] ADD  CONSTRAINT [DF_AccountsProtect_ModifyDate]  DEFAULT (getdate()) FOR [ModifyDate]
GO
ALTER TABLE [dbo].[AccountsShop] ADD  CONSTRAINT [DF_AccountsShop_LinkName1]  DEFAULT ('') FOR [LinkName1]
GO
ALTER TABLE [dbo].[AccountsShop] ADD  CONSTRAINT [DF_AccountsShop_LinkContent1]  DEFAULT ('') FOR [LinkContent1]
GO
ALTER TABLE [dbo].[AccountsShop] ADD  CONSTRAINT [DF_AccountsShop_LinkName2]  DEFAULT ('') FOR [LinkName2]
GO
ALTER TABLE [dbo].[AccountsShop] ADD  CONSTRAINT [DF_AccountsShop_LinkContent2]  DEFAULT ('') FOR [LinkContent2]
GO
ALTER TABLE [dbo].[AccountsShop] ADD  CONSTRAINT [DF_AccountsShop_SysShow]  DEFAULT ((1)) FOR [SysShow]
GO
ALTER TABLE [dbo].[AccountsShop] ADD  CONSTRAINT [DF_AccountsShop_UserShow]  DEFAULT ((0)) FOR [UserShow]
GO
ALTER TABLE [dbo].[AccountsShop] ADD  CONSTRAINT [DF_AccountsShop_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[AccountsShopLink] ADD  CONSTRAINT [DF_AccountsShopLink_SortID]  DEFAULT ((0)) FOR [SortID]
GO
ALTER TABLE [dbo].[AccountsShopLink] ADD  CONSTRAINT [DF_AccountsShopLink_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[AccountsSpreadStat] ADD  CONSTRAINT [DF_AccountsSpreadStat_SpreadPayUser]  DEFAULT ((0)) FOR [SpreadPayUser]
GO
ALTER TABLE [dbo].[AccountsSpreadStat] ADD  CONSTRAINT [DF_AccountsSpreadStat_SparedPayAmount]  DEFAULT ((0)) FOR [SparedPayAmount]
GO
ALTER TABLE [dbo].[AccountsSpreadStat] ADD  CONSTRAINT [DF_AccountsSpreadStat_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[AccountsSpreadStat] ADD  CONSTRAINT [DF_AccountsSpreadStat_SpreadViewCount]  DEFAULT ((0)) FOR [SpreadViewCount]
GO
ALTER TABLE [dbo].[AccountsSpreadStat] ADD  CONSTRAINT [DF_AccountsSpreadStat_SpreadLowerCount]  DEFAULT ((0)) FOR [SpreadLowerCount]
GO
ALTER TABLE [dbo].[AccountsSpreadStat] ADD  CONSTRAINT [DF_AccountsSpreadStat_SpreadLLowerCount]  DEFAULT ((0)) FOR [SpreadLLowerCount]
GO
ALTER TABLE [dbo].[AccountsStatus] ADD  CONSTRAINT [DF_Table_1_ChatStatus]  DEFAULT ((0)) FOR [ShutupStatus]
GO
ALTER TABLE [dbo].[AccountsStatus] ADD  CONSTRAINT [DF_AccountsStatus_ShutupOverTime]  DEFAULT ((0)) FOR [ShutupOverTime]
GO
ALTER TABLE [dbo].[AccountsStatus] ADD  CONSTRAINT [DF_AccountsStatus_StunDownOverTime]  DEFAULT ((0)) FOR [ShutDownOverTime]
GO
ALTER TABLE [dbo].[AccountsSubsidyClaim] ADD  CONSTRAINT [DF_RecordUserBaseEnsure_TakeTimes]  DEFAULT ((0)) FOR [ClaimTimes]
GO
ALTER TABLE [dbo].[AccountsSubsidyClaim] ADD  CONSTRAINT [DF_RecordUserBaseEnsure_TakeGolds]  DEFAULT ((0)) FOR [ClaimGolds]
GO
ALTER TABLE [dbo].[AccountsSubsidyClaim] ADD  CONSTRAINT [DF_RecordUserBaseEnsure_ModifyTime]  DEFAULT (getdate()) FOR [ModifyTime]
GO
ALTER TABLE [dbo].[AccountsTurntableLottery] ADD  CONSTRAINT [DF_AccountsTurntableLottery_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[AccountsVisitor] ADD  CONSTRAINT [DF_AccountsVisitor_PackageName]  DEFAULT (N'') FOR [PackageName]
GO
ALTER TABLE [dbo].[AccountsVisitor] ADD  CONSTRAINT [DF_AccountsVisite_LogonDateTime]  DEFAULT (getdate()) FOR [LogonDateTime]
GO
ALTER TABLE [dbo].[AccountsVisitor] ADD  CONSTRAINT [DF_AccountsVisite_CreateDateTime]  DEFAULT (getdate()) FOR [CreateDateTime]
GO
ALTER TABLE [dbo].[AgentIncome] ADD  CONSTRAINT [DF_AgentIncome_AgentID]  DEFAULT ((0)) FOR [AgentID]
GO
ALTER TABLE [dbo].[AgentIncome] ADD  CONSTRAINT [DF_AgentIncome_Income]  DEFAULT ((0)) FOR [Income]
GO
ALTER TABLE [dbo].[AgentIncome] ADD  CONSTRAINT [DF_AgentIncome_PayAmount]  DEFAULT ((0)) FOR [PayAmount]
GO
ALTER TABLE [dbo].[AgentIncome] ADD  CONSTRAINT [DF_AgentIncome_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[AgentIncome] ADD  CONSTRAINT [DF_AgentIncome_AgentLevel]  DEFAULT ((1)) FOR [AgentLevel]
GO
ALTER TABLE [dbo].[AgentIncome] ADD  CONSTRAINT [DF_AgentIncome_ContributeAgentID]  DEFAULT ((0)) FOR [ContributeAgentID]
GO
ALTER TABLE [dbo].[AgentIncome] ADD  CONSTRAINT [DF_AgentIncome_PayOrderID]  DEFAULT ((0)) FOR [PayOrderID]
GO
ALTER TABLE [dbo].[AgentIncome] ADD  CONSTRAINT [DF_AgentIncome_SettleWeeklD]  DEFAULT ((0)) FOR [SettleWeeklD]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_IncomeType]  DEFAULT ((0)) FOR [IncomeType]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_IncomePayRate]  DEFAULT ((0)) FOR [IncomePayRate]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_IncomePeopleAmount]  DEFAULT ((0)) FOR [IncomePeopleAmount]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_AgentLevel]  DEFAULT ((1)) FOR [AgentLevel]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_AllowLowerStatus]  DEFAULT ((0)) FOR [AllowLowerStatus]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[AgentLogonRecord] ADD  CONSTRAINT [DF_AgentLogonRecord_DateID]  DEFAULT ((0)) FOR [DateID]
GO
ALTER TABLE [dbo].[AgentLogonRecord] ADD  CONSTRAINT [DF_AgentLogonRecord_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[BlackListAccounts] ADD  CONSTRAINT [DF_BlackListAccounts_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[BlackListKind] ADD  CONSTRAINT [DF_BlackListKind_ProcessState]  DEFAULT ((0)) FOR [ProcessState]
GO
ALTER TABLE [dbo].[ChannelInfo] ADD  CONSTRAINT [DF_ChannelInfo_NickName]  DEFAULT ('') FOR [ChannelName]
GO
ALTER TABLE [dbo].[ChannelInfo] ADD  CONSTRAINT [DF_ChannelInfo_ChannelLable]  DEFAULT ('') FOR [ChannelLable]
GO
ALTER TABLE [dbo].[ChannelInfo] ADD  CONSTRAINT [DF_ChannelInfo_Accounts]  DEFAULT ('') FOR [Accounts]
GO
ALTER TABLE [dbo].[ChannelInfo] ADD  CONSTRAINT [DF_ChannelInfo_LoginPass]  DEFAULT ('') FOR [LoginPass]
GO
ALTER TABLE [dbo].[ChannelInfo] ADD  CONSTRAINT [DF_ChannelInfo_ParentID]  DEFAULT ((0)) FOR [ParentID]
GO
ALTER TABLE [dbo].[ChannelInfo] ADD  CONSTRAINT [DF_ChannelInfo_ChannelRate]  DEFAULT ((0)) FOR [ChannelRate]
GO
ALTER TABLE [dbo].[ChannelInfo] ADD  CONSTRAINT [DF_ChannelInfo_LastLoginIP]  DEFAULT ('') FOR [LastLoginIP]
GO
ALTER TABLE [dbo].[ChannelInfo] ADD  CONSTRAINT [DF_ChannelInfo_LoginCount]  DEFAULT ((0)) FOR [LoginCount]
GO
ALTER TABLE [dbo].[ChannelInfo] ADD  CONSTRAINT [DF_ChannelInfo_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[ChannelInfo] ADD  CONSTRAINT [DF_ChannelInfo_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[ChannelLoginRecord] ADD  CONSTRAINT [DF_ChannelLoginRecord_LoginIP]  DEFAULT ('') FOR [LoginIP]
GO
ALTER TABLE [dbo].[ChannelLoginRecord] ADD  CONSTRAINT [DF_ChannelLoginRecord_LoginDateTime]  DEFAULT (getdate()) FOR [LoginDateTime]
GO
ALTER TABLE [dbo].[ChannelPackage] ADD  CONSTRAINT [DF_ChannelPackage_PackageName]  DEFAULT ('') FOR [PackageName]
GO
ALTER TABLE [dbo].[ChannelPackage] ADD  CONSTRAINT [DF_ChannelPackage_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[ChannelPackage] ADD  CONSTRAINT [DF_ChannelPackage_SpreadMarketID]  DEFAULT ((0)) FOR [SpreadMarketID]
GO
ALTER TABLE [dbo].[ChannelPackage] ADD  CONSTRAINT [DF_ChannelPackage_TypeID]  DEFAULT ((0)) FOR [TypeID]
GO
ALTER TABLE [dbo].[ChannelPackage] ADD  CONSTRAINT [DF_ChannelPackage_AndroidDownloadUrl]  DEFAULT ('') FOR [AndroidDownloadUrl]
GO
ALTER TABLE [dbo].[ConfineAddress] ADD  CONSTRAINT [DF_AddrConfineRule_EnjoinLogon]  DEFAULT ((0)) FOR [EnjoinLogon]
GO
ALTER TABLE [dbo].[ConfineAddress] ADD  CONSTRAINT [DF_AddrConfineRule_EnjoinRegister]  DEFAULT ((0)) FOR [EnjoinRegister]
GO
ALTER TABLE [dbo].[ConfineAddress] ADD  CONSTRAINT [DF_AddrConfineRule_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[ConfineAddress] ADD  CONSTRAINT [DF_AddrConfineRule_CollectNote]  DEFAULT ('') FOR [CollectNote]
GO
ALTER TABLE [dbo].[ConfineAddress] ADD  CONSTRAINT [DF_ConfineAddress_MasterID]  DEFAULT ((0)) FOR [MasterID]
GO
ALTER TABLE [dbo].[ConfineContent] ADD  CONSTRAINT [DF_ReserveCharacter_ModifyDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[ConfineContent] ADD  CONSTRAINT [DF_ConfineContent_MasterID]  DEFAULT ((0)) FOR [MasterID]
GO
ALTER TABLE [dbo].[ConfineMachine] ADD  CONSTRAINT [DF_MachineConfineRule_EnjoinLogon]  DEFAULT ((0)) FOR [EnjoinLogon]
GO
ALTER TABLE [dbo].[ConfineMachine] ADD  CONSTRAINT [DF_MachineConfineRule_EnjoinRegister]  DEFAULT ((0)) FOR [EnjoinRegister]
GO
ALTER TABLE [dbo].[ConfineMachine] ADD  CONSTRAINT [DF_MachineConfineRule_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[ConfineMachine] ADD  CONSTRAINT [DF_MachineConfineRule_CollectNote]  DEFAULT ('') FOR [CollectNote]
GO
ALTER TABLE [dbo].[ConfineMachine] ADD  CONSTRAINT [DF_ConfineMachine_MasterID]  DEFAULT ((0)) FOR [MasterID]
GO
ALTER TABLE [dbo].[GameIdentifier] ADD  CONSTRAINT [DF_GameIdentifiers_IDLevel]  DEFAULT ((0)) FOR [IDLevel]
GO
ALTER TABLE [dbo].[MobileAuthInfo] ADD  CONSTRAINT [DF_MobileAuthInfo_LastAuthDate]  DEFAULT (getdate()) FOR [LastAcquireDate]
GO
ALTER TABLE [dbo].[PlazaVersionCollect] ADD  CONSTRAINT [DF_PlazaVersionCollect_DateID]  DEFAULT ((0)) FOR [DateID]
GO
ALTER TABLE [dbo].[PlazaVersionCollect] ADD  CONSTRAINT [DF_PlazaVersionCollect_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordAuthCode] ADD  CONSTRAINT [DF_AccountValidateCode_MobilePhone]  DEFAULT ('') FOR [MobileEmail]
GO
ALTER TABLE [dbo].[RecordAuthCode] ADD  CONSTRAINT [DF_RecordAuthCode_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[RecordAuthCode] ADD  CONSTRAINT [DF_AccountValidateCode_CodeKind]  DEFAULT ((0)) FOR [AuthKind]
GO
ALTER TABLE [dbo].[RecordAuthCode] ADD  CONSTRAINT [DF_RecordAuthCode_AcquireTimes]  DEFAULT ((0)) FOR [AcquireTimes]
GO
ALTER TABLE [dbo].[RecordAuthCode] ADD  CONSTRAINT [DF_RecordAuthCode_AuthMachine]  DEFAULT (N'') FOR [AcquireMachine]
GO
ALTER TABLE [dbo].[RecordAuthCode] ADD  CONSTRAINT [DF_AccountValidateCode_ValidateDate_1]  DEFAULT (getdate()) FOR [LastAcquireDate]
GO
ALTER TABLE [dbo].[RecordBindEMail] ADD  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordBindMobilePhone] ADD  CONSTRAINT [DF_RecordBindMobilePhone_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordBindParent] ADD  CONSTRAINT [DF_AccountsBindInfo_ParentID]  DEFAULT ((0)) FOR [ParentID]
GO
ALTER TABLE [dbo].[RecordBindParent] ADD  CONSTRAINT [DF_AccountsBindInfo_TypeID]  DEFAULT ((0)) FOR [TypeID]
GO
ALTER TABLE [dbo].[RecordBindParent] ADD  CONSTRAINT [DF_AccountsBindInfo_ClientIP]  DEFAULT ('') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordBindParent] ADD  CONSTRAINT [DF_AccountsBindInfo_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordGrantInfo] ADD  CONSTRAINT [DF_RecordGrantInfo_GrantDate]  DEFAULT (getdate()) FOR [GrantDate]
GO
ALTER TABLE [dbo].[RecordInform] ADD  CONSTRAINT [DF_Table_1_UserID]  DEFAULT ((0)) FOR [InformUserID]
GO
ALTER TABLE [dbo].[RecordInform] ADD  CONSTRAINT [DF_AccountsInform_InformKindID]  DEFAULT ((0)) FOR [InformKindID]
GO
ALTER TABLE [dbo].[RecordInform] ADD  CONSTRAINT [DF_RecordInform_InformKindID1]  DEFAULT ((0)) FOR [InformSource]
GO
ALTER TABLE [dbo].[RecordInform] ADD  CONSTRAINT [DF_RecordInform_InformMessage]  DEFAULT ('') FOR [InformMessage]
GO
ALTER TABLE [dbo].[RecordInform] ADD  CONSTRAINT [DF_RecordInform_AttachmentUrl]  DEFAULT ('') FOR [AttachmentUrl]
GO
ALTER TABLE [dbo].[RecordInform] ADD  CONSTRAINT [DF_AccountsInform_ClientIP]  DEFAULT ('') FOR [ClientIP]
GO
ALTER TABLE [dbo].[RecordInform] ADD  CONSTRAINT [DF_AccountsInform_CollectTime]  DEFAULT (getdate()) FOR [CollectTime]
GO
ALTER TABLE [dbo].[RecordInform] ADD  CONSTRAINT [DF_AccountsInform_Stauts]  DEFAULT ((0)) FOR [Status]
GO
ALTER TABLE [dbo].[RecordInform] ADD  CONSTRAINT [DF_Table_1_Result]  DEFAULT ((0)) FOR [HandleResult]
GO
ALTER TABLE [dbo].[RecordInform] ADD  CONSTRAINT [DF_AccountsInform_PunishmentTime]  DEFAULT ((0)) FOR [PunishmentTime]
GO
ALTER TABLE [dbo].[RecordInform] ADD  CONSTRAINT [DF_AccountsInform_HandleRemark]  DEFAULT ('') FOR [HandleRemark]
GO
ALTER TABLE [dbo].[RecordInform] ADD  CONSTRAINT [DF_AccountsInform_HandleMasterID]  DEFAULT ((0)) FOR [HandleMasterID]
GO
ALTER TABLE [dbo].[RecordLogonInfo] ADD  CONSTRAINT [DF_RecordLogonInfo_DateID]  DEFAULT ((0)) FOR [DateID]
GO
ALTER TABLE [dbo].[RecordLogonInfo] ADD  CONSTRAINT [DF_RecordLogonInfo_MachineOSID]  DEFAULT ((0)) FOR [MachineOSID]
GO
ALTER TABLE [dbo].[RecordLogonInfo] ADD  CONSTRAINT [DF_RecordLogonInfo_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RecordNeedClearGold] ADD  CONSTRAINT [DF_RecordNeedcClearGold_WithdrawDateTime]  DEFAULT (getdate()) FOR [WithdrawDateTime]
GO
ALTER TABLE [dbo].[RecordNeedDeductScore] ADD  CONSTRAINT [DF_BlaickListKindWinner_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[RecordPresentClientIP] ADD  CONSTRAINT [DF_RecordPresentClientIP_DateID]  DEFAULT ((0)) FOR [DateID]
GO
ALTER TABLE [dbo].[RecordPresentClientIP] ADD  CONSTRAINT [DF_RecordPresentClientIP_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[RecordPresentMachine] ADD  CONSTRAINT [DF_RecordPresentMachine_PresentIngot]  DEFAULT ((0)) FOR [PresentIngot]
GO
ALTER TABLE [dbo].[RecordPresentMachine] ADD  CONSTRAINT [DF_RecordPresentMachine_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[RecordPresentMachine] ADD  DEFAULT ((0)) FOR [DateID]
GO
ALTER TABLE [dbo].[RecordSubsidyMachine] ADD  CONSTRAINT [DF_RecordSubsidyMachine_InsertDate]  DEFAULT (getdate()) FOR [FirstGrantDate]
GO
ALTER TABLE [dbo].[RecordSubsidyMachine] ADD  CONSTRAINT [DF_RecordSubsidyMachine_LastGrantDate]  DEFAULT (getdate()) FOR [LastGrantDate]
GO
ALTER TABLE [dbo].[RecordTurntableMachine] ADD  CONSTRAINT [DF_RecordTurntableMachine_FirstGrantDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[ReserveIdentifier] ADD  CONSTRAINT [DF_Table1_IDLevel]  DEFAULT ((0)) FOR [IDLevel]
GO
ALTER TABLE [dbo].[ReserveIdentifier] ADD  CONSTRAINT [DF_ReserveIdentifiers_Distribute]  DEFAULT ((0)) FOR [Distribute]
GO
ALTER TABLE [dbo].[ReserveIDSellInfo] ADD  CONSTRAINT [DF_ReserveIDSellInfo_SortID]  DEFAULT ((0)) FOR [SortID]
GO
ALTER TABLE [dbo].[ReserveIDSellInfo] ADD  CONSTRAINT [DF_ReserveIDSellInfo_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[ReserveIDSellInfo] ADD  CONSTRAINT [DF_ReserveIDSellInfo_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[ReserveIDSellInfo] ADD  CONSTRAINT [DF_ReserveIDSellInfo_CollectNote]  DEFAULT ('') FOR [CollectNote]
GO
ALTER TABLE [dbo].[ReserveIDTypeInfo] ADD  CONSTRAINT [DF_ReserveIDTypeInfo_TypeName]  DEFAULT ('') FOR [TypeName]
GO
ALTER TABLE [dbo].[ReserveIDTypeInfo] ADD  CONSTRAINT [DF_ReserveIDTypeInfo_SortID]  DEFAULT ((0)) FOR [SortID]
GO
ALTER TABLE [dbo].[ReserveIDTypeInfo] ADD  CONSTRAINT [DF_ReserveIDTypeInfo_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[ReserveIDTypeInfo] ADD  CONSTRAINT [DF_ReserveIDTypeInfo_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RobotLockInfo] ADD  CONSTRAINT [DF_AndroidLockInfo_LogonPass]  DEFAULT (N'') FOR [LogonPass]
GO
ALTER TABLE [dbo].[RobotLockInfo] ADD  CONSTRAINT [DF_AndroidLockInfo_AndroidStatus]  DEFAULT ((0)) FOR [RobotStatus]
GO
ALTER TABLE [dbo].[RobotLockInfo] ADD  CONSTRAINT [DF_AndroidLockInfo_ServerID]  DEFAULT ((0)) FOR [ServerID]
GO
ALTER TABLE [dbo].[RobotLockInfo] ADD  CONSTRAINT [DF_AndroidLockInfo_BatchID]  DEFAULT ((0)) FOR [BatchID]
GO
ALTER TABLE [dbo].[RobotLockInfo] ADD  CONSTRAINT [DF_AndroidLockInfo_LockDataTime]  DEFAULT (getdate()) FOR [LockDateTime]
GO
ALTER TABLE [dbo].[StreamGrantInfo] ADD  CONSTRAINT [DF_StreamGrantInfo_FirstGrantDate]  DEFAULT (getdate()) FOR [FirstGrantDate]
GO
ALTER TABLE [dbo].[StreamGrantInfo] ADD  CONSTRAINT [DF_StreamGrantInfo_LAstGrantDate]  DEFAULT (getdate()) FOR [LastGrantDate]
GO
ALTER TABLE [dbo].[SystemStreamInfo] ADD  CONSTRAINT [DF_SystemStreamInfo_StationID]  DEFAULT ((0)) FOR [StationID]
GO
ALTER TABLE [dbo].[SystemStreamInfo] ADD  CONSTRAINT [DF_SystemLogonInfo_WebLogonSuccess]  DEFAULT ((0)) FOR [WebLogonSuccess]
GO
ALTER TABLE [dbo].[SystemStreamInfo] ADD  CONSTRAINT [DF_SystemLogonInfo_WebRegisterSuccess]  DEFAULT ((0)) FOR [WebRegisterSuccess]
GO
ALTER TABLE [dbo].[SystemStreamInfo] ADD  CONSTRAINT [DF_TABLE1_LogonCount]  DEFAULT ((0)) FOR [GameLogonSuccess]
GO
ALTER TABLE [dbo].[SystemStreamInfo] ADD  CONSTRAINT [DF_TABLE1_RegisterCount]  DEFAULT ((0)) FOR [GameRegisterSuccess]
GO
ALTER TABLE [dbo].[SystemStreamInfo] ADD  CONSTRAINT [DF_TABLE1_RecordDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[SystemStreamLogon] ADD  CONSTRAINT [DF_SystemStreamLogon_FillCount]  DEFAULT ((0)) FOR [FillCount]
GO
ALTER TABLE [dbo].[SystemStreamLogon] ADD  CONSTRAINT [DF_SystemStreamLogon_FillAmount]  DEFAULT ((0)) FOR [FillAmount]
GO
ALTER TABLE [dbo].[SystemStreamLogon] ADD  CONSTRAINT [DF_SystemStreamLogon_LogonCount]  DEFAULT ((0)) FOR [LogonCount]
GO
ALTER TABLE [dbo].[SystemStreamLogon] ADD  CONSTRAINT [DF_SystemStreamLogon_OnlineTimeCount]  DEFAULT ((0)) FOR [OnlineTimeCount]
GO
ALTER TABLE [dbo].[SystemStreamLogon] ADD  CONSTRAINT [DF_SystemStreamLogon_FirstDate]  DEFAULT (getdate()) FOR [FirstDate]
GO
ALTER TABLE [dbo].[SystemStreamLogon] ADD  CONSTRAINT [DF_SystemStreamLogon_LastDate]  DEFAULT (getdate()) FOR [LastDate]
GO
ALTER TABLE [dbo].[SystemStreamRegister] ADD  CONSTRAINT [DF_SystemStreamRegister_ExternalID]  DEFAULT ((0)) FOR [ExternalID]
GO
ALTER TABLE [dbo].[SystemStreamRegister] ADD  CONSTRAINT [DF_SystemStreamRegister_RegisterCount]  DEFAULT ((0)) FOR [TotalCount]
GO
ALTER TABLE [dbo].[SystemStreamRegister] ADD  CONSTRAINT [DF_SystemStreamRegister_CollectDate]  DEFAULT (getdate()) FOR [FirstDate]
GO
ALTER TABLE [dbo].[SystemStreamRegister] ADD  CONSTRAINT [DF_SystemStreamRegister_LastDate]  DEFAULT (getdate()) FOR [LastDate]
GO
ALTER TABLE [dbo].[TurntableLotteryWriteList] ADD  CONSTRAINT [DF_TurntableLotteryWriteList_Remark]  DEFAULT ('') FOR [Remark]
GO
ALTER TABLE [dbo].[UserRegister] ADD  CONSTRAINT [DF_UserRegister_DateID]  DEFAULT ((0)) FOR [DateID]
GO
ALTER TABLE [dbo].[RecordLogonInfo]  WITH CHECK ADD  CONSTRAINT [FK_RecordLogonInfo_RecordLogonInfo] FOREIGN KEY([RecordID])
REFERENCES [dbo].[RecordLogonInfo] ([RecordID])
GO
ALTER TABLE [dbo].[RecordLogonInfo] CHECK CONSTRAINT [FK_RecordLogonInfo_RecordLogonInfo]
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsAgentInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'代理等级   0：普通 1：高级 2：自定义' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsAgentInfo', @level2type=N'COLUMN',@level2name=N'AgentLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下级收益百分比' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsAgentInfo', @level2type=N'COLUMN',@level2name=N'LowerBetAwardRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下级第一次充值时奖励' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsAgentInfo', @level2type=N'COLUMN',@level2name=N'LowerPayAwardAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下下级收益百分比' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsAgentInfo', @level2type=N'COLUMN',@level2name=N'LLowerBetAwardRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下下级第一次充值时奖励' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsAgentInfo', @level2type=N'COLUMN',@level2name=N'LLowerPayAwardAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsAuthInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'验证码类型(1、短信验证码  2、邮件验证码)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsAuthInfo', @level2type=N'COLUMN',@level2name=N'CodeKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'验证次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsAuthInfo', @level2type=N'COLUMN',@level2name=N'AuthTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'获取验证码次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsAuthInfo', @level2type=N'COLUMN',@level2name=N'AcquireTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后一次获取验证码时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsAuthInfo', @level2type=N'COLUMN',@level2name=N'LastAcquireDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后获取验证的手机或邮箱信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsAuthInfo', @level2type=N'COLUMN',@level2name=N'LastMobileEmail'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsCommission', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'玩家佣金余额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsCommission', @level2type=N'COLUMN',@level2name=N'Commission'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总支付金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsCommission', @level2type=N'COLUMN',@level2name=N'TotalPayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总押注金额 ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsCommission', @level2type=N'COLUMN',@level2name=N'TotalBetAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总充值佣金' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsCommission', @level2type=N'COLUMN',@level2name=N'TotalPayContribute'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总押注佣金' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsCommission', @level2type=N'COLUMN',@level2name=N'TotalBetContribute'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下级充值总金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsCommission', @level2type=N'COLUMN',@level2name=N'LowerPayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下级游戏获利总金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsCommission', @level2type=N'COLUMN',@level2name=N'LowerBetAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下下级充值总金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsCommission', @level2type=N'COLUMN',@level2name=N'LLowerPayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下下级游戏获利总金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsCommission', @level2type=N'COLUMN',@level2name=N'LLowerBetAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下级用户首充贡献' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsCommission', @level2type=N'COLUMN',@level2name=N'LowerPayContribute'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下级用户游戏获利贡献' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsCommission', @level2type=N'COLUMN',@level2name=N'LowerBetContribute'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下下级用户首充贡献' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsCommission', @level2type=N'COLUMN',@level2name=N'LLowerPayContribute'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下下级游戏获利贡献' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsCommission', @level2type=N'COLUMN',@level2name=N'LLowerBetContribute'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'更新时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsCommission', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsDatum', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'真实姓名' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsDatum', @level2type=N'COLUMN',@level2name=N'Compellation'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户年龄' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsDatum', @level2type=N'COLUMN',@level2name=N'Age'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'地区信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsDatum', @level2type=N'COLUMN',@level2name=N'Area'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'城市信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsDatum', @level2type=N'COLUMN',@level2name=N'City'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'省份信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsDatum', @level2type=N'COLUMN',@level2name=N'Province'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'QQ号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsDatum', @level2type=N'COLUMN',@level2name=N'QQ'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'电子邮件' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsDatum', @level2type=N'COLUMN',@level2name=N'EMail'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'固定电话' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsDatum', @level2type=N'COLUMN',@level2name=N'SeatPhone'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'手机号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsDatum', @level2type=N'COLUMN',@level2name=N'MobilePhone'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'详细住址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsDatum', @level2type=N'COLUMN',@level2name=N'DwellingPlace'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'邮政编码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsDatum', @level2type=N'COLUMN',@level2name=N'PostalCode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收集日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsDatum', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户备注' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsDatum', @level2type=N'COLUMN',@level2name=N'UserNote'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户Uin' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsExternal', @level2type=N'COLUMN',@level2name=N'UniqueID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'他方标识（1、官方   10、苹果账号  11、苹果gamecenter  20、谷歌账号  21、googleplay  22、 facebook  23、华为 ）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsExternal', @level2type=N'COLUMN',@level2name=N'ExternalID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsExternal', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'公共标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsExternal', @level2type=N'COLUMN',@level2name=N'PublicID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户昵称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsExternal', @level2type=N'COLUMN',@level2name=N'NickName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户性别' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsExternal', @level2type=N'COLUMN',@level2name=N'Gender'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户资料' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsExternal', @level2type=N'COLUMN',@level2name=N'UserDatum'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'头像地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsExternal', @level2type=N'COLUMN',@level2name=N'UserFacePath'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'注册地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsExternal', @level2type=N'COLUMN',@level2name=N'LogonClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsExternal', @level2type=N'COLUMN',@level2name=N'LogonDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'订阅标志' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsExternal', @level2type=N'COLUMN',@level2name=N'SubscribeFlag'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'订阅时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsExternal', @level2type=N'COLUMN',@level2name=N'SubscribeTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收集时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsExternal', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsFace', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'头像ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsFace', @level2type=N'COLUMN',@level2name=N'CustomID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'头像路径' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsFace', @level2type=N'COLUMN',@level2name=N'FacePath'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'更新时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsFace', @level2type=N'COLUMN',@level2name=N'UpdateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsFace', @level2type=N'COLUMN',@level2name=N'CollectTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsGoldInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'金币数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsGoldInfo', @level2type=N'COLUMN',@level2name=N'Score'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsGoldInfo', @level2type=N'COLUMN',@level2name=N'FillTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsGoldInfo', @level2type=N'COLUMN',@level2name=N'FillAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'银行金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsGoldInfo', @level2type=N'COLUMN',@level2name=N'InsureScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'系统赠送总次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsGoldInfo', @level2type=N'COLUMN',@level2name=N'GrantTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'系统赠送金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsGoldInfo', @level2type=N'COLUMN',@level2name=N'GrantAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'口令索引' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'PasswordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户帐号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'Accounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'注册帐号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'RegAccounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户昵称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'NickName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'个性签名' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'UnderWrite'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录密码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'LogonPass'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'安全密码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'InsurePass'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录令牌' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'SecretKey'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户邮箱' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'EMail'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户手机号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'MobilePhone'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户性别' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'Gender'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'头像标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'FaceID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'角色标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'ActorID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'头像框ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'FaceFrameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'聊天框标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'ChatFrameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'牌背标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'CardBackID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏背景标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'TableBackID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'密保标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'ProtectID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'自定义图像标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'CustomID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广员标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'ParentID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户权限' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'UserRight'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理权限' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'MasterRight'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'服务权限' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'ServiceRight'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'经验数值' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'Experience'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'魅力值' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'LoveLiness'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏等级' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'GameOrder'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理等级' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'MasterOrder'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'会员等级' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'MemberOrder'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'过期日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'MemberPoint'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'切换时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'MemberOverTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'网页登录次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'WebLogonTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏登录次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'GameLogonTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'PlayTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'在线时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'OnlineTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'LogonIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'LogonDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录方式(0：未定义（默认）1: 游客登录 2：账号登录 3：第三方登录  4: 手机号登录)  ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'LogonMode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录手机号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'LogonMobile'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录邮箱地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'LogonEMail'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录机器' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'LogonMachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'设备类型（0x01：PC  0x10：WEB  0x20：IPAD 0x40：IPHONE  0x80：ANDROID  0x0100：C++ 0x0200：U3D 0x0400：LUA 0x0800：H5)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'LogonDeviceKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'注册地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'RegisterIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'注册时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'RegisterDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'注册方式(0：未定义（默认）1: 游客注册  2：账号注册  3：邮箱注册  4：手机号注册  5: 第三方注册  10:网页注册  11：公众号注册  12：下载注册  )  ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'RegisterMode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'注册机器' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'RegisterMachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'设备类型（0x01：PC  0x10：WEB  0x20：IPAD 0x40：IPHONE  0x80：ANDROID  0x0100：C++ 0x0200：U3D 0x0400：LUA 0x0800：H5)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'RegisterDeviceKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'大厅版本' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'PlazaVersion'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'固定机器' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'MoorMachine'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'信息公开标志' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'InfoPublic'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器人标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'IsRobot'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'是否有效用户(0:无效,1:有效)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'IsValid'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'成为有效用户时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'ValidUserDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'MarketID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'被赞数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'LikedNumber'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'永久性行为掩码（0x0001、修改性别 0x0002、修改昵称 0x0004、修改头像）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'ActionMaskEver'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'每日行为掩码（）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'ActionMaskPerDay'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'每周行为掩码（）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'ActionMaskPerWeek'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁止服务' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广渠道ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'SpreadChannelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'代理身份状态 0：否 1：是' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'AgentStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'所属代理' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInfo', @level2type=N'COLUMN',@level2name=N'AgentParentID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsIngotInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'钻石数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsIngotInfo', @level2type=N'COLUMN',@level2name=N'Score'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsIngotInfo', @level2type=N'COLUMN',@level2name=N'FillTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值总金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsIngotInfo', @level2type=N'COLUMN',@level2name=N'FillAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsIngotInfo', @level2type=N'COLUMN',@level2name=N'GrantTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送总额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsIngotInfo', @level2type=N'COLUMN',@level2name=N'GrantAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'邀请码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInviteCode', @level2type=N'COLUMN',@level2name=N'InviteCode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsInviteCode', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'邮件标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMailAttach', @level2type=N'COLUMN',@level2name=N'MailID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMailAttach', @level2type=N'COLUMN',@level2name=N'GoodsID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品等级' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMailAttach', @level2type=N'COLUMN',@level2name=N'GoodsLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMailAttach', @level2type=N'COLUMN',@level2name=N'GoodsCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品有效期（0： 永久拥有  ）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMailAttach', @level2type=N'COLUMN',@level2name=N'GoodsIndate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMailBox', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'邮件标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMailBox', @level2type=N'COLUMN',@level2name=N'MailID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'处理状态（1、待处理  2、已查看  3、已领取  4、已删除）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMailBox', @level2type=N'COLUMN',@level2name=N'MailState'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'过期时间戳' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMailBox', @level2type=N'COLUMN',@level2name=N'ExpireTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'更新时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMailBox', @level2type=N'COLUMN',@level2name=N'UpdateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'邮件标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMailInfo', @level2type=N'COLUMN',@level2name=N'MailID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'邮件类型（1： 通知类型  2： 附件类型）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMailInfo', @level2type=N'COLUMN',@level2name=N'MailKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'邮件分类（1、推广奖励邮件  2、货币转账邮件  3、赠送礼物邮件 4、比赛奖励邮件  5、排位奖励邮件  11、系统赠送邮件 12、提现清零通知邮件）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMailInfo', @level2type=N'COLUMN',@level2name=N'MailType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'邮件标题' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMailInfo', @level2type=N'COLUMN',@level2name=N'MailTitle'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'邮件内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMailInfo', @level2type=N'COLUMN',@level2name=N'MailContent'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'发送类型（1： 全部用户  2： 部分用户）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMailInfo', @level2type=N'COLUMN',@level2name=N'SendKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'发送时间戳' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMailInfo', @level2type=N'COLUMN',@level2name=N'SendTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'过期时间戳' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMailInfo', @level2type=N'COLUMN',@level2name=N'ExpireTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMailInfo', @level2type=N'COLUMN',@level2name=N'CollectTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMember', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'会员标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMember', @level2type=N'COLUMN',@level2name=N'MemberOrder'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户权限' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMember', @level2type=N'COLUMN',@level2name=N'UserRight'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'会员期限' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsMember', @level2type=N'COLUMN',@level2name=N'MemberOverDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsOnlineInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'网关标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsOnlineInfo', @level2type=N'COLUMN',@level2name=N'GateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'网络标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsOnlineInfo', @level2type=N'COLUMN',@level2name=N'SocketID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsOnlineInfo', @level2type=N'COLUMN',@level2name=N'LogonID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsOnlineInfo', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsOnlineInfo', @level2type=N'COLUMN',@level2name=N'ServerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间类型（0x01、金币房 0x04、比赛房  0x08、公开房间  0x10、排位房间）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsOnlineInfo', @level2type=N'COLUMN',@level2name=N'ServerKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'插入时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsOnlineInfo', @level2type=N'COLUMN',@level2name=N'InsertTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修改时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsOnlineInfo', @level2type=N'COLUMN',@level2name=N'ModifyTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsPack', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsPack', @level2type=N'COLUMN',@level2name=N'GoodsID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsPack', @level2type=N'COLUMN',@level2name=N'GoodsKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品等级' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsPack', @level2type=N'COLUMN',@level2name=N'GoodsLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsPack', @level2type=N'COLUMN',@level2name=N'GoodsCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品有效期（0： 永久拥有  ）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsPack', @level2type=N'COLUMN',@level2name=N'GoodsIndate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'绑定状态' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsPack', @level2type=N'COLUMN',@level2name=N'BindState'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'物品过期时间戳' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsPack', @level2type=N'COLUMN',@level2name=N'ExpireTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsPack', @level2type=N'COLUMN',@level2name=N'CollectTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'密保标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsProtect', @level2type=N'COLUMN',@level2name=N'ProtectID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsProtect', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'问题一' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsProtect', @level2type=N'COLUMN',@level2name=N'Question1'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'答案一' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsProtect', @level2type=N'COLUMN',@level2name=N'Response1'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'问题二' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsProtect', @level2type=N'COLUMN',@level2name=N'Question2'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'答案二' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsProtect', @level2type=N'COLUMN',@level2name=N'Response2'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'问题三' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsProtect', @level2type=N'COLUMN',@level2name=N'Question3'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'答案三' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsProtect', @level2type=N'COLUMN',@level2name=N'Response3'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'证件号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsProtect', @level2type=N'COLUMN',@level2name=N'PassportID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'证件类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsProtect', @level2type=N'COLUMN',@level2name=N'PassportType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'安全邮箱' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsProtect', @level2type=N'COLUMN',@level2name=N'SafeEmail'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsProtect', @level2type=N'COLUMN',@level2name=N'CreateIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修改地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsProtect', @level2type=N'COLUMN',@level2name=N'ModifyIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsProtect', @level2type=N'COLUMN',@level2name=N'CreateDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修改日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsProtect', @level2type=N'COLUMN',@level2name=N'ModifyDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsShop', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商店名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsShop', @level2type=N'COLUMN',@level2name=N'ShopName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'商店描述' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsShop', @level2type=N'COLUMN',@level2name=N'ShopDescription'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'联系人1' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsShop', @level2type=N'COLUMN',@level2name=N'LinkName1'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'联系方式1' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsShop', @level2type=N'COLUMN',@level2name=N'LinkContent1'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'联系人2' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsShop', @level2type=N'COLUMN',@level2name=N'LinkName2'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'联系方式2' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsShop', @level2type=N'COLUMN',@level2name=N'LinkContent2'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'系统显示(0:显示,1:不显示，优先级大于用户显示)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsShop', @level2type=N'COLUMN',@level2name=N'SysShow'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户显示(0:显示,1:不显示)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsShop', @level2type=N'COLUMN',@level2name=N'UserShow'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsShop', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'联系方式' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsShopLink', @level2type=N'COLUMN',@level2name=N'LinkName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排序' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsShopLink', @level2type=N'COLUMN',@level2name=N'SortID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'冻结状态' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsShopLink', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsSpreadStat', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广支付用户数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsSpreadStat', @level2type=N'COLUMN',@level2name=N'SpreadPayUser'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广支付总金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsSpreadStat', @level2type=N'COLUMN',@level2name=N'SparedPayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广员点击数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsSpreadStat', @level2type=N'COLUMN',@level2name=N'SpreadViewCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广用户' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsSpreadStat', @level2type=N'COLUMN',@level2name=N'SpreadLowerCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广下下级用户数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsSpreadStat', @level2type=N'COLUMN',@level2name=N'SpreadLLowerCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsStatus', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁言状态 0：正常 1：禁言' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsStatus', @level2type=N'COLUMN',@level2name=N'ShutupStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁言过期时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsStatus', @level2type=N'COLUMN',@level2name=N'ShutupOverTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'封号状态' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsStatus', @level2type=N'COLUMN',@level2name=N'ShutDownStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'封号过期时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsStatus', @level2type=N'COLUMN',@level2name=N'ShutDownOverTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后操作禁言时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsStatus', @level2type=N'COLUMN',@level2name=N'LastShutupDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后操作封号时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsStatus', @level2type=N'COLUMN',@level2name=N'LastShutDownDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsSubsidyClaim', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsSubsidyClaim', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'领取次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsSubsidyClaim', @level2type=N'COLUMN',@level2name=N'ClaimTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'累计领取的金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsSubsidyClaim', @level2type=N'COLUMN',@level2name=N'ClaimGolds'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'查询机器' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsSubsidyClaim', @level2type=N'COLUMN',@level2name=N'MachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后修改时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsSubsidyClaim', @level2type=N'COLUMN',@level2name=N'ModifyTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsTurntableLottery', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsTurntableLottery', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'可抽奖总次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsTurntableLottery', @level2type=N'COLUMN',@level2name=N'MayLotteryTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'已使用抽奖次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsTurntableLottery', @level2type=N'COLUMN',@level2name=N'UseLotteryTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'来自充值的抽奖次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsTurntableLottery', @level2type=N'COLUMN',@level2name=N'PayLotteryTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'系统免费赠送的次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsTurntableLottery', @level2type=N'COLUMN',@level2name=N'FreeLotteryTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'来自游戏时长的抽奖次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsTurntableLottery', @level2type=N'COLUMN',@level2name=N'GameTimeLotteryTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'来自游戏局数的抽奖次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsTurntableLottery', @level2type=N'COLUMN',@level2name=N'GameCountLotteryTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'来自游戏消耗的抽奖次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsTurntableLottery', @level2type=N'COLUMN',@level2name=N'GameExpendLotteryTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsTurntableLottery', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游客标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsVisitor', @level2type=N'COLUMN',@level2name=N'VisiteID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsVisitor', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsVisitor', @level2type=N'COLUMN',@level2name=N'MachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'设备名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsVisitor', @level2type=N'COLUMN',@level2name=N'MachineName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'注册地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsVisitor', @level2type=N'COLUMN',@level2name=N'LogonClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsVisitor', @level2type=N'COLUMN',@level2name=N'LogonDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AccountsVisitor', @level2type=N'COLUMN',@level2name=N'CreateDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'代理ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentIncome', @level2type=N'COLUMN',@level2name=N'AgentID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收益' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentIncome', @level2type=N'COLUMN',@level2name=N'Income'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'分红类型 0：按充值 1：按人头' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentIncome', @level2type=N'COLUMN',@level2name=N'IncomeType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentIncome', @level2type=N'COLUMN',@level2name=N'PayAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentIncome', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentIncome', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'代理级别' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentIncome', @level2type=N'COLUMN',@level2name=N'AgentLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'贡献代理ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentIncome', @level2type=N'COLUMN',@level2name=N'ContributeAgentID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值用户' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentIncome', @level2type=N'COLUMN',@level2name=N'ContributeUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'支付订单' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentIncome', @level2type=N'COLUMN',@level2name=N'PayOrderID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'结算周ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentIncome', @level2type=N'COLUMN',@level2name=N'SettleWeeklD'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收益类型 0：按充值 1：按人头' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'IncomeType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值分成比例，按充值比例收益时' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'IncomePayRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广玩家奖励金额，按人头奖励时' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'IncomePeopleAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'代理级别' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'AgentLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'是否允许下级 0：不允许 1：允许' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'AllowLowerStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁用状态' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentLogonRecord', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentLogonRecord', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentLogonRecord', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentLogonRecord', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentLogonRecord', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'封禁状态 0-没封禁 1-已经把用户封禁了' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'BlackListKind', @level2type=N'COLUMN',@level2name=N'ProcessState'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelInfo', @level2type=N'COLUMN',@level2name=N'ChannelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelInfo', @level2type=N'COLUMN',@level2name=N'ChannelName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道标签' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelInfo', @level2type=N'COLUMN',@level2name=N'ChannelLable'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道帐号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelInfo', @level2type=N'COLUMN',@level2name=N'Accounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录密码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelInfo', @level2type=N'COLUMN',@level2name=N'LoginPass'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'父级ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelInfo', @level2type=N'COLUMN',@level2name=N'ParentID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道比例' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelInfo', @level2type=N'COLUMN',@level2name=N'ChannelRate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelInfo', @level2type=N'COLUMN',@level2name=N'LastLoginDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelInfo', @level2type=N'COLUMN',@level2name=N'LastLoginIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelInfo', @level2type=N'COLUMN',@level2name=N'PreLoginIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelInfo', @level2type=N'COLUMN',@level2name=N'PreLoginDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelInfo', @level2type=N'COLUMN',@level2name=N'LoginCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁用标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelInfo', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelInfo', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelLoginRecord', @level2type=N'COLUMN',@level2name=N'LoginIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelLoginRecord', @level2type=N'COLUMN',@level2name=N'LoginDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'包ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelPackage', @level2type=N'COLUMN',@level2name=N'MarketID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'包名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelPackage', @level2type=N'COLUMN',@level2name=N'PackageName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelPackage', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'父marketid' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelPackage', @level2type=N'COLUMN',@level2name=N'SpreadMarketID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'0---渠道包 1--裂变包' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelPackage', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'安卓下载地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelPackage', @level2type=N'COLUMN',@level2name=N'AndroidDownloadUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'限制标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineAddress', @level2type=N'COLUMN',@level2name=N'ConfineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'地址字符' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineAddress', @level2type=N'COLUMN',@level2name=N'AddrString'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'限制登录' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineAddress', @level2type=N'COLUMN',@level2name=N'EnjoinLogon'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'限制注册' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineAddress', @level2type=N'COLUMN',@level2name=N'EnjoinRegister'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'过期时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineAddress', @level2type=N'COLUMN',@level2name=N'EnjoinOverDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收集日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineAddress', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'输入备注' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineAddress', @level2type=N'COLUMN',@level2name=N'CollectNote'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'录入管理员' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineAddress', @level2type=N'COLUMN',@level2name=N'MasterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'限制标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineContent', @level2type=N'COLUMN',@level2name=N'ConfineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'保留字符' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineContent', @level2type=N'COLUMN',@level2name=N'String'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'录入日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineContent', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'录入管理员' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineContent', @level2type=N'COLUMN',@level2name=N'MasterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'限制标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineMachine', @level2type=N'COLUMN',@level2name=N'ConfineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineMachine', @level2type=N'COLUMN',@level2name=N'MachineSerial'
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
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'录入管理员' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ConfineMachine', @level2type=N'COLUMN',@level2name=N'MasterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameIdentifier', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameIdentifier', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'标识等级' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'GameIdentifier', @level2type=N'COLUMN',@level2name=N'IDLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MobileAuthInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'重置次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MobileAuthInfo', @level2type=N'COLUMN',@level2name=N'AuthTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'请求次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MobileAuthInfo', @level2type=N'COLUMN',@level2name=N'AcquireTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后请求时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MobileAuthInfo', @level2type=N'COLUMN',@level2name=N'LastAcquireDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后请求手机号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'MobileAuthInfo', @level2type=N'COLUMN',@level2name=N'LastMobilePhone'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'邮件标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PendingMailUser', @level2type=N'COLUMN',@level2name=N'MailID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PendingMailUser', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PlazaVersionCollect', @level2type=N'COLUMN',@level2name=N'ID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PlazaVersionCollect', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'大厅版本' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PlazaVersionCollect', @level2type=N'COLUMN',@level2name=N'PlazaVersion'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PlazaVersionCollect', @level2type=N'COLUMN',@level2name=N'LogonCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'PlazaVersionCollect', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'验证码类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAuthCode', @level2type=N'COLUMN',@level2name=N'CodeKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'手机或邮件信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAuthCode', @level2type=N'COLUMN',@level2name=N'MobileEmail'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAuthCode', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'验证类型（1：绑定手机 2：解绑手机  3: 重置密码 4: 验证码登陆  5:  手机注册）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAuthCode', @level2type=N'COLUMN',@level2name=N'AuthKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'当前验证码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAuthCode', @level2type=N'COLUMN',@level2name=N'AuthCode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'当前验证码MD5值' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAuthCode', @level2type=N'COLUMN',@level2name=N'AuthCodeMD5'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'验证次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAuthCode', @level2type=N'COLUMN',@level2name=N'AuthTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'请求次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAuthCode', @level2type=N'COLUMN',@level2name=N'AcquireTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'请求IP地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAuthCode', @level2type=N'COLUMN',@level2name=N'AcquireClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'请求机器码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAuthCode', @level2type=N'COLUMN',@level2name=N'AcquireMachine'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后请求时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAuthCode', @level2type=N'COLUMN',@level2name=N'LastAcquireDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAuthTimes', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAuthTimes', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'验证码类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAuthTimes', @level2type=N'COLUMN',@level2name=N'CodeKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'验证类型（1：绑定手机 2：解绑手机  3: 重置密码 4: 验证码登陆  5:  手机注册）' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAuthTimes', @level2type=N'COLUMN',@level2name=N'AuthKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'验证次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordAuthTimes', @level2type=N'COLUMN',@level2name=N'AuthTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBindEMail', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'邮箱信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBindEMail', @level2type=N'COLUMN',@level2name=N'EMail'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBindEMail', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBindMobilePhone', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'手机号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBindMobilePhone', @level2type=N'COLUMN',@level2name=N'MobilePhone'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBindMobilePhone', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBindParent', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'绑定操作的用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBindParent', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广人用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBindParent', @level2type=N'COLUMN',@level2name=N'ParentID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作类型(0:绑定,1:解绑)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBindParent', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'绑定地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBindParent', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'绑定时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordBindParent', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantInfo', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送前金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantInfo', @level2type=N'COLUMN',@level2name=N'PreScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送前银行' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantInfo', @level2type=N'COLUMN',@level2name=N'PreInsureScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantInfo', @level2type=N'COLUMN',@level2name=N'GrantScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送类型(1:注册,2:泡分,3.人为赠送,4.房间赠送,5:金币补贴,6:推广赠送)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantInfo', @level2type=N'COLUMN',@level2name=N'GrantType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantInfo', @level2type=N'COLUMN',@level2name=N'GrantIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordGrantInfo', @level2type=N'COLUMN',@level2name=N'GrantDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordInform', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'举报人UserID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordInform', @level2type=N'COLUMN',@level2name=N'InformUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'举报类型(1、Abusive words   2、Political or religious  3、Spam   4、Selling items  10、Others)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordInform', @level2type=N'COLUMN',@level2name=N'InformKindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'举报来源(0、个人界面   1、世界聊天  2、群聊   3、私聊)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordInform', @level2type=N'COLUMN',@level2name=N'InformSource'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'举报消息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordInform', @level2type=N'COLUMN',@level2name=N'InformMessage'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'举报内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordInform', @level2type=N'COLUMN',@level2name=N'InformContent'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'被举报人用户ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordInform', @level2type=N'COLUMN',@level2name=N'BeInformUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'被举报人GameID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordInform', @level2type=N'COLUMN',@level2name=N'BeInformGameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'附件Url' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordInform', @level2type=N'COLUMN',@level2name=N'AttachmentUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'客户端IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordInform', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'举报时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordInform', @level2type=N'COLUMN',@level2name=N'CollectTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'状态：0：待处理 1：已处理' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordInform', @level2type=N'COLUMN',@level2name=N'Status'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'处理结果 0：不惩罚 1：禁言 2：封号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordInform', @level2type=N'COLUMN',@level2name=N'HandleResult'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'惩罚时间，单位秒，0：永久' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordInform', @level2type=N'COLUMN',@level2name=N'PunishmentTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'处理备注' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordInform', @level2type=N'COLUMN',@level2name=N'HandleRemark'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'处理管理员ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordInform', @level2type=N'COLUMN',@level2name=N'HandleMasterID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'处理时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordInform', @level2type=N'COLUMN',@level2name=N'HandleTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordLogonInfo', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordLogonInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordLogonInfo', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'设备类型（0x01：PC  0x10：WEB  0x20：IPAD 0x40：IPHONE  0x80：ANDROID  0x0100：C++ 0x0200：U3D 0x0400：LUA 0x0800：H5)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordLogonInfo', @level2type=N'COLUMN',@level2name=N'DeviceKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'大厅版本' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordLogonInfo', @level2type=N'COLUMN',@level2name=N'PlazaVersion'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'客户端IP地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordLogonInfo', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordLogonInfo', @level2type=N'COLUMN',@level2name=N'MachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'设备系统版本' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordLogonInfo', @level2type=N'COLUMN',@level2name=N'MachineOSID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordLogonInfo', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'提现时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordNeedClearGold', @level2type=N'COLUMN',@level2name=N'WithdrawDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'客户端IP地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordPresentClientIP', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordPresentClientIP', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送金币总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordPresentClientIP', @level2type=N'COLUMN',@level2name=N'PresentGold'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送钻石总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordPresentClientIP', @level2type=N'COLUMN',@level2name=N'PresentIngot'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总赠送次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordPresentClientIP', @level2type=N'COLUMN',@level2name=N'PresentCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordPresentClientIP', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器码标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordPresentMachine', @level2type=N'COLUMN',@level2name=N'MachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送金币总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordPresentMachine', @level2type=N'COLUMN',@level2name=N'PresentGold'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送钻石总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordPresentMachine', @level2type=N'COLUMN',@level2name=N'PresentIngot'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总赠送次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordPresentMachine', @level2type=N'COLUMN',@level2name=N'PresentCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordPresentMachine', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordPresentMachine', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSubsidyMachine', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器码标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSubsidyMachine', @level2type=N'COLUMN',@level2name=N'MachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送金币总数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSubsidyMachine', @level2type=N'COLUMN',@level2name=N'PresentGold'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总赠送次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSubsidyMachine', @level2type=N'COLUMN',@level2name=N'PresentCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSubsidyMachine', @level2type=N'COLUMN',@level2name=N'UserIDString'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'初次赠送时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSubsidyMachine', @level2type=N'COLUMN',@level2name=N'FirstGrantDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后赠送时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordSubsidyMachine', @level2type=N'COLUMN',@level2name=N'LastGrantDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordTurntableClientIP', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'客户端IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordTurntableClientIP', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordTurntableClientIP', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordTurntableMachine', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器码标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordTurntableMachine', @level2type=N'COLUMN',@level2name=N'MachineID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RecordTurntableMachine', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReserveIdentifier', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'标识等级' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReserveIdentifier', @level2type=N'COLUMN',@level2name=N'IDLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'分配标志' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReserveIdentifier', @level2type=N'COLUMN',@level2name=N'Distribute'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReserveIDSellInfo', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReserveIDSellInfo', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排序标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReserveIDSellInfo', @level2type=N'COLUMN',@level2name=N'SortID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'价格' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReserveIDSellInfo', @level2type=N'COLUMN',@level2name=N'Price'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'冻结标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReserveIDSellInfo', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'购买人' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReserveIDSellInfo', @level2type=N'COLUMN',@level2name=N'ApplyUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'购买日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReserveIDSellInfo', @level2type=N'COLUMN',@level2name=N'ApplyDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作者用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReserveIDSellInfo', @level2type=N'COLUMN',@level2name=N'OperUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReserveIDSellInfo', @level2type=N'COLUMN',@level2name=N'IPAddress'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReserveIDSellInfo', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReserveIDSellInfo', @level2type=N'COLUMN',@level2name=N'CollectNote'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReserveIDTypeInfo', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类型名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReserveIDTypeInfo', @level2type=N'COLUMN',@level2name=N'TypeName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排序标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReserveIDTypeInfo', @level2type=N'COLUMN',@level2name=N'SortID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁用标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReserveIDTypeInfo', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReserveIDTypeInfo', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'批次标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RobotConfigure', @level2type=N'COLUMN',@level2name=N'BatchID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RobotConfigure', @level2type=N'COLUMN',@level2name=N'ServerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'服务模式' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RobotConfigure', @level2type=N'COLUMN',@level2name=N'ServiceMode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器人数量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RobotConfigure', @level2type=N'COLUMN',@level2name=N'RobotCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'进入时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RobotConfigure', @level2type=N'COLUMN',@level2name=N'EnterTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'离开时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RobotConfigure', @level2type=N'COLUMN',@level2name=N'LeaveTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'进入最小间隔' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RobotConfigure', @level2type=N'COLUMN',@level2name=N'EnterMinInterval'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'进入最大间隔' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RobotConfigure', @level2type=N'COLUMN',@level2name=N'EnterMaxInterval'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'离开最小间隔' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RobotConfigure', @level2type=N'COLUMN',@level2name=N'LeaveMinInterval'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'离开最大间隔' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RobotConfigure', @level2type=N'COLUMN',@level2name=N'LeaveMaxInterval'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最少携带分数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RobotConfigure', @level2type=N'COLUMN',@level2name=N'TakeMinScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最大携带分数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RobotConfigure', @level2type=N'COLUMN',@level2name=N'TakeMaxScore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最少换桌局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RobotConfigure', @level2type=N'COLUMN',@level2name=N'SwitchMinInnings'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最大换桌局数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RobotConfigure', @level2type=N'COLUMN',@level2name=N'SwitchMaxInnings'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RobotLockInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器密码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RobotLockInfo', @level2type=N'COLUMN',@level2name=N'LogonPass'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器人状态' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RobotLockInfo', @level2type=N'COLUMN',@level2name=N'RobotStatus'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'房间标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RobotLockInfo', @level2type=N'COLUMN',@level2name=N'ServerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'批次标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RobotLockInfo', @level2type=N'COLUMN',@level2name=N'BatchID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'插入日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RobotLockInfo', @level2type=N'COLUMN',@level2name=N'LockDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamGrantInfo', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送类?1:注册,2:泡分,3.人为赠?4.房间赠?5:金币补贴,6:推广赠?' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamGrantInfo', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赠送金币总量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamGrantInfo', @level2type=N'COLUMN',@level2name=N'GrantScoreStat'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'第一次赠送时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamGrantInfo', @level2type=N'COLUMN',@level2name=N'FirstGrantDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后一次赠送时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'StreamGrantInfo', @level2type=N'COLUMN',@level2name=N'LastGrantDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamInfo', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'站点标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamInfo', @level2type=N'COLUMN',@level2name=N'StationID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录成功' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamInfo', @level2type=N'COLUMN',@level2name=N'WebLogonSuccess'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'注册成功' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamInfo', @level2type=N'COLUMN',@level2name=N'WebRegisterSuccess'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录成功' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamInfo', @level2type=N'COLUMN',@level2name=N'GameLogonSuccess'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'注册成功' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamInfo', @level2type=N'COLUMN',@level2name=N'GameRegisterSuccess'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'录入时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamInfo', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamLogon', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamLogon', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamLogon', @level2type=N'COLUMN',@level2name=N'FillCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'充值金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamLogon', @level2type=N'COLUMN',@level2name=N'FillAmount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamLogon', @level2type=N'COLUMN',@level2name=N'LogonCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'在线游戏时长' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamLogon', @level2type=N'COLUMN',@level2name=N'OnlineTimeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'初次记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamLogon', @level2type=N'COLUMN',@level2name=N'FirstDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamLogon', @level2type=N'COLUMN',@level2name=N'LastDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamRegister', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'设备类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamRegister', @level2type=N'COLUMN',@level2name=N'DeviceKind'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'他方标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamRegister', @level2type=N'COLUMN',@level2name=N'ExternalID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'注册总量' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamRegister', @level2type=N'COLUMN',@level2name=N'TotalCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'初次记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamRegister', @level2type=N'COLUMN',@level2name=N'FirstDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'末次记录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStreamRegister', @level2type=N'COLUMN',@level2name=N'LastDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日期标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'TurntableLotteryHitStat', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'奖项索引' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'TurntableLotteryHitStat', @level2type=N'COLUMN',@level2name=N'ItemIndex'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'命中次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'TurntableLotteryHitStat', @level2type=N'COLUMN',@level2name=N'HitTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'命中索引' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'TurntableLotteryWriteList', @level2type=N'COLUMN',@level2name=N'HitIndex'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'TurntableLotteryWriteList', @level2type=N'COLUMN',@level2name=N'Remark'
GO
