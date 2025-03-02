USE [LHNativeWebDB]
GO
/****** Object:  Table [dbo].[AllowAddress]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AllowAddress](
	[IPAddress] [nvarchar](15) NOT NULL,
	[AllowLogon] [bit] NOT NULL,
	[InvalidDate] [datetime] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[CollectNote] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_AllowAddress_IPAddress] PRIMARY KEY CLUSTERED 
(
	[IPAddress] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ApplyMatchInfo]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ApplyMatchInfo](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[MatchID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[GameID] [int] NOT NULL,
	[NickName] [nvarchar](31) NOT NULL,
	[RealName] [nvarchar](15) NOT NULL,
	[MobilePhone] [nvarchar](11) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_ApplyMatchInfo] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ApplyMatchRedPack]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ApplyMatchRedPack](
	[CodeID] [int] NOT NULL,
	[MatchID] [int] NOT NULL,
	[Amount] [decimal](18, 2) NOT NULL,
	[CreateDate] [datetime] NOT NULL,
	[UserID] [int] NOT NULL,
	[RecevieDate] [datetime] NULL,
	[RecevieIP] [nvarchar](15) NOT NULL,
 CONSTRAINT [PK_ApplyMatchRedPack] PRIMARY KEY CLUSTERED 
(
	[CodeID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[CacheChartsEvents]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[CacheChartsEvents](
	[CacheID] [int] IDENTITY(1,1) NOT NULL,
	[CurrencyID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[ModuleID] [int] NOT NULL,
	[TypeID] [int] NOT NULL,
	[KindName] [nvarchar](31) NOT NULL,
	[TopN] [int] NOT NULL,
	[UpdateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_CacheChartsEvents] PRIMARY KEY CLUSTERED 
(
	[CacheID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[CacheChartsGold]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[CacheChartsGold](
	[ChartID] [int] NOT NULL,
	[CurrencyID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[GameID] [int] NOT NULL,
	[Accounts] [nvarchar](31) NOT NULL,
	[Score] [bigint] NOT NULL,
	[WinCount] [int] NOT NULL,
	[LostCount] [int] NOT NULL,
	[FleeCount] [int] NOT NULL,
	[DrawCount] [int] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[CacheChartsScore]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[CacheChartsScore](
	[ChartID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[StationID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[GameID] [int] NOT NULL,
	[Accounts] [nvarchar](31) NOT NULL,
	[Score] [bigint] NOT NULL,
	[WinCount] [int] NOT NULL,
	[LostCount] [int] NOT NULL,
	[FleeCount] [int] NOT NULL,
	[DrawCount] [int] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ScheduledEventsInfo]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ScheduledEventsInfo](
	[EventID] [int] IDENTITY(1,1) NOT NULL,
	[Key] [nvarchar](32) NOT NULL,
	[TimeOfDay] [int] NOT NULL,
	[Minutes] [int] NOT NULL,
	[ScheduleType] [varchar](1024) NOT NULL,
	[Enabled] [bit] NOT NULL,
	[IsSystemEvent] [bit] NOT NULL,
 CONSTRAINT [PK_ScheduledEventsInfo] PRIMARY KEY CLUSTERED 
(
	[Key] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ScheduledEventsLogs]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ScheduledEventsLogs](
	[ScheduleID] [int] IDENTITY(1,1) NOT NULL,
	[Key] [varchar](32) NOT NULL,
	[LastExecuted] [datetime] NOT NULL,
	[ServerName] [varchar](128) NOT NULL,
 CONSTRAINT [PK_ScheduledEventsLogs] PRIMARY KEY CLUSTERED 
(
	[ScheduleID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[SystemExtendCenter]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[SystemExtendCenter](
	[ExtendID] [int] IDENTITY(1,1) NOT NULL,
	[StationID] [int] NOT NULL,
	[AppName] [nvarchar](32) NOT NULL,
	[AppUrl] [nvarchar](256) NOT NULL,
	[APIKey] [nchar](32) NOT NULL,
	[SecretKey] [nchar](32) NOT NULL,
	[IPAddresses] [nvarchar](256) NOT NULL,
	[Nullity] [tinyint] NOT NULL,
 CONSTRAINT [PK_SystemExtendCenter] PRIMARY KEY CLUSTERED 
(
	[ExtendID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[SystemModuleInfo]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[SystemModuleInfo](
	[ModuleID] [int] NOT NULL,
	[ParentID] [int] NOT NULL,
	[Title] [nvarchar](32) NOT NULL,
	[Imgurl] [nvarchar](256) NOT NULL,
	[Linkurl] [nvarchar](256) NOT NULL,
	[SortID] [int] NOT NULL,
	[FrameID] [varchar](16) NOT NULL,
	[Nullity] [tinyint] NOT NULL,
 CONSTRAINT [PK_SystemModuleInfo] PRIMARY KEY CLUSTERED 
(
	[ModuleID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[SystemModulePermission]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[SystemModulePermission](
	[ModuleID] [int] NOT NULL,
	[PermissionID] [int] NOT NULL,
	[PermissionName] [nvarchar](32) NOT NULL,
	[PermissionValue] [int] NOT NULL,
 CONSTRAINT [PK_SystemModulePermission] PRIMARY KEY CLUSTERED 
(
	[ModuleID] ASC,
	[PermissionID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[SystemRecordInfo]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[SystemRecordInfo](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[StationID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[EventType] [int] NOT NULL,
	[EventLevel] [tinyint] NOT NULL,
	[EventDetail] [nvarchar](128) NOT NULL,
	[EventUrl] [nvarchar](256) NOT NULL,
	[EventIP] [nvarchar](15) NOT NULL,
	[EventDate] [datetime] NOT NULL,
 CONSTRAINT [PK_SystemRecordInfo] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[SystemStatusInfo]    Script Date: 2023/7/12 11:11:41 ******/
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
/****** Object:  Table [dbo].[UserAccountsInfo]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[UserAccountsInfo](
	[UserID] [int] IDENTITY(1,1) NOT FOR REPLICATION NOT NULL,
	[RoleID] [int] NOT NULL,
	[StationID] [int] NOT NULL,
	[Accounts] [nvarchar](31) NOT NULL,
	[LogonPass] [nvarchar](32) NOT NULL,
	[Realname] [nvarchar](32) NOT NULL,
	[LinkEmail] [nvarchar](128) NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[LogonTimes] [int] NOT NULL,
	[RegisterIP] [nvarchar](15) NOT NULL,
	[LastLogonIP] [nvarchar](15) NOT NULL,
	[RegisterDate] [datetime] NOT NULL,
	[LastLogonDate] [datetime] NOT NULL,
 CONSTRAINT [PK_UserAccountsInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[UserRolesInfo]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[UserRolesInfo](
	[RoleID] [int] IDENTITY(1,1) NOT NULL,
	[Rolename] [nvarchar](32) NOT NULL,
	[Nullity] [tinyint] NOT NULL,
 CONSTRAINT [PK_UserRolesInfo] PRIMARY KEY CLUSTERED 
(
	[RoleID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[UserRolesPermission]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[UserRolesPermission](
	[RoleID] [int] NOT NULL,
	[ModuleID] [int] NOT NULL,
	[PermissionValue] [int] NOT NULL,
 CONSTRAINT [PK_UserRolesPermission] PRIMARY KEY CLUSTERED 
(
	[RoleID] ASC,
	[ModuleID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[WebsiteChannel]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[WebsiteChannel](
	[ChannelID] [int] IDENTITY(1,1) NOT NULL,
	[Token] [nvarchar](16) NOT NULL,
	[Title] [nvarchar](32) NOT NULL,
	[LogoUrl] [varchar](256) NOT NULL,
	[SeoTitle] [nvarchar](64) NOT NULL,
	[SeoHead] [nvarchar](512) NOT NULL,
	[SeoKeymeta] [nvarchar](256) NOT NULL,
	[SeoDescmeta] [nvarchar](512) NOT NULL,
	[Themename] [nvarchar](32) NOT NULL,
	[Themepath] [varchar](256) NOT NULL,
	[Templetpath] [varchar](256) NOT NULL,
	[CachedPath] [varchar](256) NOT NULL,
	[IndexPagename] [nvarchar](32) NOT NULL,
	[PageSize] [int] NOT NULL,
	[SortID] [int] NOT NULL,
	[IsLink] [tinyint] NOT NULL,
	[LinkUrl] [varchar](256) NOT NULL,
	[IsLock] [tinyint] NOT NULL,
	[IsVisible] [tinyint] NOT NULL,
	[IsHtml] [tinyint] NOT NULL,
 CONSTRAINT [PK_WebsiteChannel] PRIMARY KEY CLUSTERED 
(
	[ChannelID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[WebsiteChannelDigest]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[WebsiteChannelDigest](
	[DigestID] [int] IDENTITY(1,1) NOT NULL,
	[ChannelID] [int] NOT NULL,
	[Token] [nvarchar](16) NOT NULL,
	[DigestContent] [nvarchar](4000) NOT NULL,
	[LoadPosition] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_WebsiteChannelDigest] PRIMARY KEY CLUSTERED 
(
	[DigestID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[WebsiteEmailConfigInfo]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[WebsiteEmailConfigInfo](
	[Smtp] [nvarchar](512) NOT NULL,
	[Port] [int] NOT NULL,
	[Sysemail] [nvarchar](64) NOT NULL,
	[Username] [nvarchar](32) NOT NULL,
	[Password] [nvarchar](512) NOT NULL,
	[Emailcontent] [nvarchar](2048) NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[WebsiteGameInfo]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[WebsiteGameInfo](
	[GameID] [int] IDENTITY(1,1) NOT NULL,
	[ModuleID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[TypeID] [int] NOT NULL,
	[KindName] [nvarchar](32) NOT NULL,
	[KindNameAlias] [nvarchar](32) NOT NULL,
	[KindIntro] [nvarchar](1024) NOT NULL,
	[MaxVersion] [nvarchar](32) NOT NULL,
	[Platforms] [nvarchar](128) NOT NULL,
	[InstalledSize] [nvarchar](16) NOT NULL,
	[InstallName1] [varchar](512) NOT NULL,
	[InstallName2] [varchar](512) NOT NULL,
	[InstallName3] [varchar](512) NOT NULL,
	[IsPlaza] [tinyint] NOT NULL,
	[ImgInstalledUrl1] [varchar](512) NOT NULL,
	[ImgInstalledUrl2] [varchar](512) NOT NULL,
	[ImgRuleUrl1] [varchar](512) NOT NULL,
	[ImgRuleUrl2] [varchar](512) NOT NULL,
	[ImgSpreadUrl1] [varchar](512) NOT NULL,
	[ImgSpreadUrl2] [varchar](512) NOT NULL,
	[Keywords] [nvarchar](256) NOT NULL,
	[Description] [nvarchar](512) NOT NULL,
	[HelpIntro] [nvarchar](4000) NOT NULL,
	[HelpRule] [nvarchar](max) NOT NULL,
	[HelpGrade] [nvarchar](4000) NOT NULL,
	[HelpContent1] [nvarchar](4000) NOT NULL,
	[HelpContent2] [nvarchar](4000) NOT NULL,
	[HelpContent3] [nvarchar](4000) NOT NULL,
	[JoinSpread] [bit] NOT NULL,
	[JoinIntro] [bit] NOT NULL,
	[Nullity] [bit] NOT NULL,
	[IssueDate] [datetime] NOT NULL,
	[ModifyDate] [datetime] NOT NULL,
 CONSTRAINT [PK_WebsiteGameInfo] PRIMARY KEY CLUSTERED 
(
	[GameID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[WebsiteGameInstall]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[WebsiteGameInstall](
	[KindID] [int] NOT NULL,
	[ProcessName] [nvarchar](32) NOT NULL,
	[InstallImgUrl] [nvarchar](256) NOT NULL,
	[InstallUrl] [nvarchar](256) NOT NULL,
	[InstallToolTips] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_WebsiteGameInstall] PRIMARY KEY CLUSTERED 
(
	[KindID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[WebsiteGameNominate]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[WebsiteGameNominate](
	[NominateID] [int] IDENTITY(1,1) NOT NULL,
	[TypeID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[KindName] [nvarchar](32) NOT NULL,
	[LogoUrl] [nvarchar](256) NOT NULL,
	[SpreadUrl] [nvarchar](256) NOT NULL,
	[InstallToolTips] [nvarchar](32) NOT NULL,
	[InstallUrl] [nvarchar](256) NOT NULL,
	[ProcessName] [nvarchar](32) NOT NULL,
	[RuleUrl] [nvarchar](256) NOT NULL,
	[SortID] [int] NOT NULL,
 CONSTRAINT [PK_WebsiteGameNominate] PRIMARY KEY CLUSTERED 
(
	[NominateID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[WebsiteGeneralInfo]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[WebsiteGeneralInfo](
	[GeneralID] [int] IDENTITY(1,1) NOT NULL,
	[WebTitle] [nvarchar](64) NOT NULL,
	[WebUrl] [nvarchar](128) NOT NULL,
	[SeoTitle] [nvarchar](512) NOT NULL,
	[SeoKeywords] [nvarchar](512) NOT NULL,
	[SeoDescription] [nvarchar](512) NOT NULL,
	[SeoHead] [nvarchar](1024) NOT NULL,
	[CookieDomain] [nvarchar](32) NOT NULL,
	[CookieKeepTime] [int] NOT NULL,
	[CookieEncryptKey] [nvarchar](32) NOT NULL,
	[AutoLogonPlaza] [tinyint] NOT NULL,
	[MaxUserfaceID] [int] NOT NULL,
	[UserfaceUrl] [nvarchar](512) NOT NULL,
	[ThemeName] [nvarchar](16) NOT NULL,
	[ThemePath] [nvarchar](512) NOT NULL,
	[LogLevel] [tinyint] NOT NULL,
	[IcpInfo] [nvarchar](1024) NOT NULL,
	[WebStat] [nvarchar](1024) NOT NULL,
	[ScheduleEnabled] [bit] NOT NULL,
	[TimerMinutesInterval] [int] NOT NULL,
	[DownloadUrl1] [nvarchar](512) NOT NULL,
	[DownloadUrl2] [nvarchar](512) NOT NULL,
	[DownloadUrl3] [nvarchar](512) NOT NULL,
 CONSTRAINT [PK_WebsiteGeneralInfo] PRIMARY KEY CLUSTERED 
(
	[GeneralID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[WebsiteIssueContent]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[WebsiteIssueContent](
	[IssueID] [int] IDENTITY(1,1) NOT NULL,
	[IssueType] [int] NOT NULL,
	[IssueTitle] [nvarchar](512) NOT NULL,
	[IssueContent] [ntext] NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[ModifyDate] [datetime] NOT NULL,
 CONSTRAINT [PK_GameIssueInfo] PRIMARY KEY CLUSTERED 
(
	[IssueID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[WebsiteNewsContent]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[WebsiteNewsContent](
	[NewsID] [int] IDENTITY(1,1) NOT NULL,
	[PopID] [int] NOT NULL,
	[Subject] [nvarchar](64) NOT NULL,
	[Subject1] [nvarchar](64) NOT NULL,
	[OnTop] [tinyint] NOT NULL,
	[OnTopAll] [tinyint] NOT NULL,
	[IsElite] [tinyint] NOT NULL,
	[IsHot] [tinyint] NOT NULL,
	[IsLock] [tinyint] NOT NULL,
	[IsDelete] [tinyint] NOT NULL,
	[IsGlobal] [tinyint] NOT NULL,
	[IsLinks] [tinyint] NOT NULL,
	[LinkUrl] [nvarchar](256) NOT NULL,
	[Body] [ntext] NOT NULL,
	[FormattedBody] [ntext] NOT NULL,
	[Keywords] [nvarchar](64) NOT NULL,
	[Brief] [nvarchar](256) NOT NULL,
	[HighLight] [varchar](512) NOT NULL,
	[Icon] [int] NOT NULL,
	[ClassID] [tinyint] NOT NULL,
	[DeviceID] [int] NOT NULL,
	[KindID] [int] NOT NULL,
	[Author] [nvarchar](31) NOT NULL,
	[UserID] [int] NOT NULL,
	[IssueIP] [nchar](15) NOT NULL,
	[LastModifyIP] [nchar](15) NOT NULL,
	[IssueDate] [datetime] NOT NULL,
	[LastModifyDate] [datetime] NOT NULL,
	[OverDate] [datetime] NULL,
 CONSTRAINT [PK_WebsiteNewsContent] PRIMARY KEY CLUSTERED 
(
	[NewsID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[WebsiteNoticeContent]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[WebsiteNoticeContent](
	[NoticeID] [int] IDENTITY(1,1) NOT NULL,
	[Subject] [nvarchar](64) NOT NULL,
	[IsLink] [tinyint] NOT NULL,
	[LinkUrl] [varchar](256) NOT NULL,
	[Body] [nvarchar](max) NOT NULL,
	[Location] [varchar](128) NOT NULL,
	[Width] [int] NOT NULL,
	[Height] [int] NOT NULL,
	[StartDate] [datetime] NOT NULL,
	[OverDate] [datetime] NOT NULL,
	[Nullity] [tinyint] NOT NULL,
 CONSTRAINT [PK_News_PopWindows] PRIMARY KEY CLUSTERED 
(
	[NoticeID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[WebsiteSidebar]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[WebsiteSidebar](
	[BarID] [int] IDENTITY(1,1) NOT NULL,
	[ChannelID] [int] NOT NULL,
	[Token] [nvarchar](16) NOT NULL,
	[Title] [nvarchar](32) NOT NULL,
	[BarContent] [nvarchar](2048) NOT NULL,
	[LogoUrl] [varchar](256) NOT NULL,
	[LoadPosition] [nvarchar](32) NOT NULL,
	[IsLink] [tinyint] NOT NULL,
	[LinkUrl] [varchar](256) NOT NULL,
	[IsLock] [tinyint] NOT NULL,
 CONSTRAINT [PK_WebsiteSidebar] PRIMARY KEY CLUSTERED 
(
	[BarID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[WebsiteTemplates]    Script Date: 2023/7/12 11:11:41 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[WebsiteTemplates](
	[TMLID] [int] IDENTITY(1,1) NOT NULL,
	[Token] [nvarchar](16) NOT NULL,
	[Note] [nvarchar](64) NOT NULL,
	[TMLContent1] [nvarchar](max) NOT NULL,
	[TMLContent2] [nvarchar](max) NOT NULL,
	[TMLContent3] [nvarchar](max) NOT NULL,
	[IssueDate] [datetime] NOT NULL,
	[ModifyDate] [datetime] NOT NULL,
 CONSTRAINT [PK_WebsiteTemplates] PRIMARY KEY CLUSTERED 
(
	[TMLID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
ALTER TABLE [dbo].[AllowAddress] ADD  CONSTRAINT [DF_Table_1_EnjoinLogon]  DEFAULT ((0)) FOR [AllowLogon]
GO
ALTER TABLE [dbo].[AllowAddress] ADD  CONSTRAINT [DF_AllowAddress_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[AllowAddress] ADD  CONSTRAINT [DF_AllowAddress_CollectNote]  DEFAULT ('') FOR [CollectNote]
GO
ALTER TABLE [dbo].[ApplyMatchInfo] ADD  CONSTRAINT [DF_ApplyMatchInfo_MatchID]  DEFAULT ((0)) FOR [MatchID]
GO
ALTER TABLE [dbo].[ApplyMatchInfo] ADD  CONSTRAINT [DF_ApplyMatchInfo_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[ApplyMatchInfo] ADD  CONSTRAINT [DF_ApplyMatchInfo_GameID]  DEFAULT ((0)) FOR [GameID]
GO
ALTER TABLE [dbo].[ApplyMatchInfo] ADD  CONSTRAINT [DF_ApplyMatchInfo_NickName]  DEFAULT ('') FOR [NickName]
GO
ALTER TABLE [dbo].[ApplyMatchInfo] ADD  CONSTRAINT [DF_ApplyMatchInfo_RealName]  DEFAULT ('') FOR [RealName]
GO
ALTER TABLE [dbo].[ApplyMatchInfo] ADD  CONSTRAINT [DF_ApplyMatchInfo_MobilePhone]  DEFAULT ('') FOR [MobilePhone]
GO
ALTER TABLE [dbo].[ApplyMatchInfo] ADD  CONSTRAINT [DF_ApplyMatchInfo_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[ApplyMatchRedPack] ADD  CONSTRAINT [DF_ApplyMatchRedPack_CodeID]  DEFAULT ((0)) FOR [CodeID]
GO
ALTER TABLE [dbo].[ApplyMatchRedPack] ADD  CONSTRAINT [DF_ApplyMatchRedPack_MatchID]  DEFAULT ((0)) FOR [MatchID]
GO
ALTER TABLE [dbo].[ApplyMatchRedPack] ADD  CONSTRAINT [DF_ApplyMatchRedPack_Amount]  DEFAULT ((0)) FOR [Amount]
GO
ALTER TABLE [dbo].[ApplyMatchRedPack] ADD  CONSTRAINT [DF_ApplyMatchRedPack_CreateDate]  DEFAULT (getdate()) FOR [CreateDate]
GO
ALTER TABLE [dbo].[ApplyMatchRedPack] ADD  CONSTRAINT [DF_ApplyMatchRedPack_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[ApplyMatchRedPack] ADD  CONSTRAINT [DF_ApplyMatchRedPack_RecevieIP]  DEFAULT ('') FOR [RecevieIP]
GO
ALTER TABLE [dbo].[CacheChartsEvents] ADD  CONSTRAINT [DF_CacheChartsEvents_CurrencyID]  DEFAULT ((0)) FOR [CurrencyID]
GO
ALTER TABLE [dbo].[CacheChartsEvents] ADD  CONSTRAINT [DF_CacheChartsEvents_KindID]  DEFAULT ((0)) FOR [KindID]
GO
ALTER TABLE [dbo].[CacheChartsEvents] ADD  CONSTRAINT [DF_CacheChartsEvents_ModuleID]  DEFAULT ((0)) FOR [ModuleID]
GO
ALTER TABLE [dbo].[CacheChartsEvents] ADD  CONSTRAINT [DF_CacheChartsEvents_TypeID]  DEFAULT ((0)) FOR [TypeID]
GO
ALTER TABLE [dbo].[CacheChartsEvents] ADD  CONSTRAINT [DF_CacheChartsEvents_KindName]  DEFAULT (N'') FOR [KindName]
GO
ALTER TABLE [dbo].[CacheChartsEvents] ADD  CONSTRAINT [DF_CacheChartsEvents_TopN]  DEFAULT ((100)) FOR [TopN]
GO
ALTER TABLE [dbo].[CacheChartsEvents] ADD  CONSTRAINT [DF_CacheChartsEvents_UpdateTime]  DEFAULT (getdate()) FOR [UpdateTime]
GO
ALTER TABLE [dbo].[CacheChartsGold] ADD  CONSTRAINT [DF_CacheChartsGold_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[CacheChartsGold] ADD  CONSTRAINT [DF_CacheChartsGold_GameID]  DEFAULT ((0)) FOR [GameID]
GO
ALTER TABLE [dbo].[CacheChartsGold] ADD  CONSTRAINT [DF_CacheChartsGold_Score]  DEFAULT ((0)) FOR [Score]
GO
ALTER TABLE [dbo].[CacheChartsGold] ADD  CONSTRAINT [DF_CacheChartsGold_WinCount]  DEFAULT ((0)) FOR [WinCount]
GO
ALTER TABLE [dbo].[CacheChartsGold] ADD  CONSTRAINT [DF_CacheChartsGold_LostCount]  DEFAULT ((0)) FOR [LostCount]
GO
ALTER TABLE [dbo].[CacheChartsGold] ADD  CONSTRAINT [DF_CacheChartsGold_FleeCount]  DEFAULT ((0)) FOR [FleeCount]
GO
ALTER TABLE [dbo].[CacheChartsGold] ADD  CONSTRAINT [DF_CacheChartsGold_DrawCount]  DEFAULT ((0)) FOR [DrawCount]
GO
ALTER TABLE [dbo].[CacheChartsScore] ADD  CONSTRAINT [DF_CacheChartsScore_StationID]  DEFAULT ((0)) FOR [StationID]
GO
ALTER TABLE [dbo].[CacheChartsScore] ADD  CONSTRAINT [DF_CacheChartsScore_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[CacheChartsScore] ADD  CONSTRAINT [DF_CacheChartsScore_GameID]  DEFAULT ((0)) FOR [GameID]
GO
ALTER TABLE [dbo].[CacheChartsScore] ADD  CONSTRAINT [DF_CacheChartsScore_Score]  DEFAULT ((0)) FOR [Score]
GO
ALTER TABLE [dbo].[CacheChartsScore] ADD  CONSTRAINT [DF_CacheChartsScore_WinCount]  DEFAULT ((0)) FOR [WinCount]
GO
ALTER TABLE [dbo].[CacheChartsScore] ADD  CONSTRAINT [DF_CacheChartsScore_LostCount]  DEFAULT ((0)) FOR [LostCount]
GO
ALTER TABLE [dbo].[CacheChartsScore] ADD  CONSTRAINT [DF_CacheChartsScore_FleeCount]  DEFAULT ((0)) FOR [FleeCount]
GO
ALTER TABLE [dbo].[CacheChartsScore] ADD  CONSTRAINT [DF_CacheChartsScore_DrawCount]  DEFAULT ((0)) FOR [DrawCount]
GO
ALTER TABLE [dbo].[ScheduledEventsInfo] ADD  CONSTRAINT [DF_ScheduledEventsInfo_TimeOfDay]  DEFAULT ((-1)) FOR [TimeOfDay]
GO
ALTER TABLE [dbo].[ScheduledEventsInfo] ADD  CONSTRAINT [DF_ScheduledEventsInfo_Minutes]  DEFAULT ((60)) FOR [Minutes]
GO
ALTER TABLE [dbo].[ScheduledEventsInfo] ADD  CONSTRAINT [DF_ScheduledEventsInfo_ScheduleType]  DEFAULT ('') FOR [ScheduleType]
GO
ALTER TABLE [dbo].[ScheduledEventsInfo] ADD  CONSTRAINT [DF_ScheduledEventsInfo_Enabled]  DEFAULT ((0)) FOR [Enabled]
GO
ALTER TABLE [dbo].[ScheduledEventsInfo] ADD  CONSTRAINT [DF_ScheduledEventsInfo_IsSystemEvent]  DEFAULT ((0)) FOR [IsSystemEvent]
GO
ALTER TABLE [dbo].[ScheduledEventsLogs] ADD  CONSTRAINT [DF_ScheduledEvents_LastExecuted]  DEFAULT (getdate()) FOR [LastExecuted]
GO
ALTER TABLE [dbo].[SystemExtendCenter] ADD  CONSTRAINT [DF_SystemExtendCenter_IPAddresses]  DEFAULT (N'127.0.0.1') FOR [IPAddresses]
GO
ALTER TABLE [dbo].[SystemExtendCenter] ADD  CONSTRAINT [DF_SystemExtendCenter_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[SystemModuleInfo] ADD  CONSTRAINT [DF_SystemModuleInfo_ParentID]  DEFAULT ((0)) FOR [ParentID]
GO
ALTER TABLE [dbo].[SystemModuleInfo] ADD  CONSTRAINT [DF_SystemModuleInfo_Imgurl]  DEFAULT (N'') FOR [Imgurl]
GO
ALTER TABLE [dbo].[SystemModuleInfo] ADD  CONSTRAINT [DF_SystemModuleInfo_Linkurl]  DEFAULT (N'') FOR [Linkurl]
GO
ALTER TABLE [dbo].[SystemModuleInfo] ADD  CONSTRAINT [DF_SystemModuleInfo_SortID]  DEFAULT ((0)) FOR [SortID]
GO
ALTER TABLE [dbo].[SystemModuleInfo] ADD  CONSTRAINT [DF_SystemModuleInfo_FrameID]  DEFAULT (N'') FOR [FrameID]
GO
ALTER TABLE [dbo].[SystemModuleInfo] ADD  CONSTRAINT [DF_SystemModuleInfo_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[SystemModulePermission] ADD  CONSTRAINT [DF_SystemModulePermission_PermissionValue]  DEFAULT ((0)) FOR [PermissionValue]
GO
ALTER TABLE [dbo].[SystemRecordInfo] ADD  CONSTRAINT [DF_SystemRecordInfo_EventLevel]  DEFAULT ((0)) FOR [EventLevel]
GO
ALTER TABLE [dbo].[SystemRecordInfo] ADD  CONSTRAINT [DF_SystemRecordInfo_EventDetail]  DEFAULT (N'') FOR [EventDetail]
GO
ALTER TABLE [dbo].[SystemRecordInfo] ADD  CONSTRAINT [DF_SystemRecordInfo_EventUrl]  DEFAULT (N'') FOR [EventUrl]
GO
ALTER TABLE [dbo].[SystemRecordInfo] ADD  CONSTRAINT [DF_SystemRecordInfo_EventIP]  DEFAULT (N'0.0.0.0') FOR [EventIP]
GO
ALTER TABLE [dbo].[SystemRecordInfo] ADD  CONSTRAINT [DF_SystemRecordInfo_EventDate]  DEFAULT (getdate()) FOR [EventDate]
GO
ALTER TABLE [dbo].[SystemStatusInfo] ADD  CONSTRAINT [DF_SystemStatusInfo_StatusValue]  DEFAULT ((0)) FOR [StatusValue]
GO
ALTER TABLE [dbo].[SystemStatusInfo] ADD  CONSTRAINT [DF_SystemStatusInfo_StatusString]  DEFAULT ('') FOR [StatusString]
GO
ALTER TABLE [dbo].[UserAccountsInfo] ADD  CONSTRAINT [DF_UserAccountsInfo_RoleID]  DEFAULT ((0)) FOR [RoleID]
GO
ALTER TABLE [dbo].[UserAccountsInfo] ADD  CONSTRAINT [DF_UserAccountsInfo_LogonTimes]  DEFAULT ((0)) FOR [LogonTimes]
GO
ALTER TABLE [dbo].[UserAccountsInfo] ADD  CONSTRAINT [DF_UserAccountsInfo_RegisterIP]  DEFAULT (N'0.0.0.0') FOR [RegisterIP]
GO
ALTER TABLE [dbo].[UserAccountsInfo] ADD  CONSTRAINT [DF_UserAccountsInfo_LastLogonIP]  DEFAULT (N'0.0.0.0') FOR [LastLogonIP]
GO
ALTER TABLE [dbo].[UserAccountsInfo] ADD  CONSTRAINT [DF_UserAccountsInfo_RegisterDate]  DEFAULT (getdate()) FOR [RegisterDate]
GO
ALTER TABLE [dbo].[UserAccountsInfo] ADD  CONSTRAINT [DF_UserAccountsInfo_LastLogonDate]  DEFAULT (getdate()) FOR [LastLogonDate]
GO
ALTER TABLE [dbo].[UserRolesInfo] ADD  CONSTRAINT [DF_UserRolesInfo_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[UserRolesPermission] ADD  CONSTRAINT [DF_UserRolesPermission_PermissionValue]  DEFAULT ((0)) FOR [PermissionValue]
GO
ALTER TABLE [dbo].[WebsiteChannel] ADD  CONSTRAINT [DF_Website_Channel_ChannelIndex]  DEFAULT (N'') FOR [Token]
GO
ALTER TABLE [dbo].[WebsiteChannel] ADD  CONSTRAINT [DF_Website_Channel_ChannelLogoUrl]  DEFAULT ('') FOR [LogoUrl]
GO
ALTER TABLE [dbo].[WebsiteChannel] ADD  CONSTRAINT [DF_Website_Channel_SeoTitle]  DEFAULT ('') FOR [SeoTitle]
GO
ALTER TABLE [dbo].[WebsiteChannel] ADD  CONSTRAINT [DF_Website_Channel_SeoHead]  DEFAULT ('') FOR [SeoHead]
GO
ALTER TABLE [dbo].[WebsiteChannel] ADD  CONSTRAINT [DF_Website_Channel_SeoKeymeta]  DEFAULT ('') FOR [SeoKeymeta]
GO
ALTER TABLE [dbo].[WebsiteChannel] ADD  CONSTRAINT [DF_Website_Channel_SeoDescmeta]  DEFAULT ('') FOR [SeoDescmeta]
GO
ALTER TABLE [dbo].[WebsiteChannel] ADD  CONSTRAINT [DF_WebsiteChannel_Themename]  DEFAULT (N'') FOR [Themename]
GO
ALTER TABLE [dbo].[WebsiteChannel] ADD  CONSTRAINT [DF_Website_Channel_Themepath]  DEFAULT ('') FOR [Themepath]
GO
ALTER TABLE [dbo].[WebsiteChannel] ADD  CONSTRAINT [DF_Website_Channel_Templetpath]  DEFAULT ('') FOR [Templetpath]
GO
ALTER TABLE [dbo].[WebsiteChannel] ADD  CONSTRAINT [DF_Website_Channel_CachePath]  DEFAULT ('') FOR [CachedPath]
GO
ALTER TABLE [dbo].[WebsiteChannel] ADD  CONSTRAINT [DF_Website_Channel_IndexPagename]  DEFAULT ('') FOR [IndexPagename]
GO
ALTER TABLE [dbo].[WebsiteChannel] ADD  CONSTRAINT [DF_WebsiteChannel_PageSize]  DEFAULT ((0)) FOR [PageSize]
GO
ALTER TABLE [dbo].[WebsiteChannel] ADD  CONSTRAINT [DF_Website_Channel_SortID]  DEFAULT ((0)) FOR [SortID]
GO
ALTER TABLE [dbo].[WebsiteChannel] ADD  CONSTRAINT [DF_Website_Channel_IsLink]  DEFAULT ((0)) FOR [IsLink]
GO
ALTER TABLE [dbo].[WebsiteChannel] ADD  CONSTRAINT [DF_Website_Channel_LinkUrl]  DEFAULT (N'') FOR [LinkUrl]
GO
ALTER TABLE [dbo].[WebsiteChannel] ADD  CONSTRAINT [DF_Website_Channel_IsLocked]  DEFAULT ((1)) FOR [IsLock]
GO
ALTER TABLE [dbo].[WebsiteChannel] ADD  CONSTRAINT [DF_Website_Channel_IsVisiable]  DEFAULT ((0)) FOR [IsVisible]
GO
ALTER TABLE [dbo].[WebsiteChannel] ADD  CONSTRAINT [DF_Website_Channel_IsHtml]  DEFAULT ((0)) FOR [IsHtml]
GO
ALTER TABLE [dbo].[WebsiteChannelDigest] ADD  CONSTRAINT [DF_Website_ChannelDigest_DigestToken]  DEFAULT (N'') FOR [Token]
GO
ALTER TABLE [dbo].[WebsiteChannelDigest] ADD  CONSTRAINT [DF_Website_ChannelDigest_DigestContent]  DEFAULT (N'') FOR [DigestContent]
GO
ALTER TABLE [dbo].[WebsiteChannelDigest] ADD  CONSTRAINT [DF_Website_ChannelDigest_DigestPosition]  DEFAULT (N'') FOR [LoadPosition]
GO
ALTER TABLE [dbo].[WebsiteEmailConfigInfo] ADD  CONSTRAINT [DF_uc_EmailConfigInfo_port]  DEFAULT ((25)) FOR [Port]
GO
ALTER TABLE [dbo].[WebsiteEmailConfigInfo] ADD  CONSTRAINT [DF_uc_EmailConfigInfo_Sysemail]  DEFAULT (N'') FOR [Sysemail]
GO
ALTER TABLE [dbo].[WebsiteEmailConfigInfo] ADD  CONSTRAINT [DF_uc_EmailConfigInfo_Emailcontent]  DEFAULT (N'') FOR [Emailcontent]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_KindNameAlias]  DEFAULT (N'') FOR [KindNameAlias]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_KindIntro]  DEFAULT (N'') FOR [KindIntro]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_MaxVersion]  DEFAULT (N'0.0.0.0') FOR [MaxVersion]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_Platforms]  DEFAULT (N'98/NT/2000/XP/2003/VISTA/Win7') FOR [Platforms]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_InstalledSize]  DEFAULT (N'0 M') FOR [InstalledSize]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_DownloadUrl1]  DEFAULT (N'') FOR [InstallName1]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_DownloadUrl2]  DEFAULT (N'') FOR [InstallName2]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_DownloadUrl3]  DEFAULT (N'') FOR [InstallName3]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_IsPlaza]  DEFAULT ((0)) FOR [IsPlaza]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_ImgInstalledUrl1]  DEFAULT (N'') FOR [ImgInstalledUrl1]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_ImgInstalledUrl2]  DEFAULT (N'') FOR [ImgInstalledUrl2]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_ImgRuleUrl1]  DEFAULT (N'') FOR [ImgRuleUrl1]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_ImgRuleUrl2]  DEFAULT (N'') FOR [ImgRuleUrl2]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_ImgSpreadUrl1]  DEFAULT (N'') FOR [ImgSpreadUrl1]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_ImgSpreadUrl2]  DEFAULT (N'') FOR [ImgSpreadUrl2]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_Keywords]  DEFAULT (N'') FOR [Keywords]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_Description]  DEFAULT (N'') FOR [Description]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_HelpIntro]  DEFAULT (N'') FOR [HelpIntro]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_HelpRule]  DEFAULT (N'') FOR [HelpRule]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_HelpGrade]  DEFAULT (N'') FOR [HelpGrade]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_HelpContent1]  DEFAULT (N'') FOR [HelpContent1]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_HelpContent2]  DEFAULT (N'') FOR [HelpContent2]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_HelpContent3]  DEFAULT (N'') FOR [HelpContent3]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_JoinSpread]  DEFAULT ((0)) FOR [JoinSpread]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_JoinIntro]  DEFAULT ((0)) FOR [JoinIntro]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_IssueDate]  DEFAULT (getdate()) FOR [IssueDate]
GO
ALTER TABLE [dbo].[WebsiteGameInfo] ADD  CONSTRAINT [DF_WebsiteGameInfo_ModifyDate]  DEFAULT (getdate()) FOR [ModifyDate]
GO
ALTER TABLE [dbo].[WebsiteGameInstall] ADD  CONSTRAINT [DF_WebsiteGameInstall_InstallImg]  DEFAULT (N'') FOR [InstallImgUrl]
GO
ALTER TABLE [dbo].[WebsiteGameInstall] ADD  CONSTRAINT [DF_WebsiteGameInstall_InstallToolTips]  DEFAULT (N'') FOR [InstallToolTips]
GO
ALTER TABLE [dbo].[WebsiteGameNominate] ADD  CONSTRAINT [DF_Website_SpreadGame_TypeID]  DEFAULT ((0)) FOR [TypeID]
GO
ALTER TABLE [dbo].[WebsiteGameNominate] ADD  CONSTRAINT [DF_Website_SpreadGame_KindID]  DEFAULT ((0)) FOR [KindID]
GO
ALTER TABLE [dbo].[WebsiteGameNominate] ADD  CONSTRAINT [DF_Website_SpreadGame_KindName]  DEFAULT (N'') FOR [KindName]
GO
ALTER TABLE [dbo].[WebsiteGameNominate] ADD  CONSTRAINT [DF_Website_SpreadGame_KindLogoPicUrl]  DEFAULT ('') FOR [LogoUrl]
GO
ALTER TABLE [dbo].[WebsiteGameNominate] ADD  CONSTRAINT [DF_Website_SpreadGame_KindSpreadUrl]  DEFAULT ('') FOR [SpreadUrl]
GO
ALTER TABLE [dbo].[WebsiteGameNominate] ADD  CONSTRAINT [DF_Website_SpreadGame_KindInstallAlt]  DEFAULT ('') FOR [InstallToolTips]
GO
ALTER TABLE [dbo].[WebsiteGameNominate] ADD  CONSTRAINT [DF_Website_SpreadGame_KindInstallUrl]  DEFAULT (N'') FOR [InstallUrl]
GO
ALTER TABLE [dbo].[WebsiteGameNominate] ADD  CONSTRAINT [DF_Website_SpreadGame_KindProcessName]  DEFAULT (N'') FOR [ProcessName]
GO
ALTER TABLE [dbo].[WebsiteGameNominate] ADD  CONSTRAINT [DF_Website_SpreadGame_KindRuleUrl]  DEFAULT (N'') FOR [RuleUrl]
GO
ALTER TABLE [dbo].[WebsiteGameNominate] ADD  CONSTRAINT [DF_Website_SpreadGame_SpreadSortID]  DEFAULT ((0)) FOR [SortID]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_Website_GeneralInfo_WebTitle]  DEFAULT (N'GameWeb') FOR [WebTitle]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_WebsiteGeneralInfo_Weburl]  DEFAULT (N'') FOR [WebUrl]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_Website_GeneralInfo_SeoTitle]  DEFAULT (N'') FOR [SeoTitle]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_Website_GeneralInfo_SeoKeywords]  DEFAULT (N'') FOR [SeoKeywords]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_Website_GeneralInfo_SeoDescription]  DEFAULT (N'') FOR [SeoDescription]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_Website_GeneralInfo_SeoHead]  DEFAULT (N'') FOR [SeoHead]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_Website_GeneralInfo_CookieDomain]  DEFAULT (N'') FOR [CookieDomain]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_Website_GeneralInfo_CookieKeepTime]  DEFAULT ((0)) FOR [CookieKeepTime]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_Website_GeneralInfo_EncryptKey]  DEFAULT ((123456789)) FOR [CookieEncryptKey]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_Website_GeneralInfo_PlazaAutoLogon]  DEFAULT ((1)) FOR [AutoLogonPlaza]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_Website_GeneralInfo_MaxUserfaceID]  DEFAULT ((0)) FOR [MaxUserfaceID]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_Website_GeneralInfo_UserfaceUrl]  DEFAULT (N'http://service.foxuc.com/avatars/') FOR [UserfaceUrl]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_Website_GeneralInfo_ThemeName]  DEFAULT (N'default') FOR [ThemeName]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_Website_GeneralInfo_ThemePath]  DEFAULT (N'/Themes/default/') FOR [ThemePath]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_Website_GeneralInfo_EnableLog]  DEFAULT ((0)) FOR [LogLevel]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_Website_GeneralInfo_Icp]  DEFAULT (N'') FOR [IcpInfo]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_Website_GeneralInfo_WebStat]  DEFAULT (N'') FOR [WebStat]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_WebsiteGeneralInfo_ScheduleEnabled]  DEFAULT ((1)) FOR [ScheduleEnabled]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_WebsiteGeneralInfo_TimerMinutesInterval]  DEFAULT ((1)) FOR [TimerMinutesInterval]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_WebsiteGeneralInfo_DownloadUrl]  DEFAULT (N'') FOR [DownloadUrl1]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_WebsiteGeneralInfo_DownloadUrl11]  DEFAULT (N'') FOR [DownloadUrl2]
GO
ALTER TABLE [dbo].[WebsiteGeneralInfo] ADD  CONSTRAINT [DF_WebsiteGeneralInfo_DownloadUrl21]  DEFAULT (N'') FOR [DownloadUrl3]
GO
ALTER TABLE [dbo].[WebsiteIssueContent] ADD  CONSTRAINT [DF_WebsiteIssueContent_IssueType]  DEFAULT ((0)) FOR [IssueType]
GO
ALTER TABLE [dbo].[WebsiteIssueContent] ADD  CONSTRAINT [DF_GameIssueInfo_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[WebsiteIssueContent] ADD  CONSTRAINT [DF_GameIssueInfo_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[WebsiteIssueContent] ADD  CONSTRAINT [DF_GameIssueInfo_ModifyDate]  DEFAULT (getdate()) FOR [ModifyDate]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContents_PopID]  DEFAULT ((0)) FOR [PopID]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContent_Subject]  DEFAULT (N'') FOR [Subject]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContent_Subject1]  DEFAULT (N'') FOR [Subject1]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContent_IsTop]  DEFAULT ((0)) FOR [OnTop]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContent_IsTopAll]  DEFAULT ((0)) FOR [OnTopAll]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContent_Elite]  DEFAULT ((0)) FOR [IsElite]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContents_IsHot]  DEFAULT ((0)) FOR [IsHot]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContent_IsLock]  DEFAULT ((1)) FOR [IsLock]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContent_IsDelete]  DEFAULT ((0)) FOR [IsDelete]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContents_IsGlobe]  DEFAULT ((0)) FOR [IsGlobal]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContent_IsLinks]  DEFAULT ((0)) FOR [IsLinks]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContent_LinkUrl]  DEFAULT (N'') FOR [LinkUrl]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContent_Body]  DEFAULT ('') FOR [Body]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContent_FormattedBody]  DEFAULT (N'') FOR [FormattedBody]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContent_Keywords]  DEFAULT (N'') FOR [Keywords]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContent_Brief]  DEFAULT ('') FOR [Brief]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContent_SubjectColor]  DEFAULT (N'') FOR [HighLight]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContent_Icon]  DEFAULT ((0)) FOR [Icon]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContent_Type]  DEFAULT ((1)) FOR [ClassID]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContent_DeviceID]  DEFAULT ((0)) FOR [DeviceID]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContent_KindID]  DEFAULT ((0)) FOR [KindID]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContent_IPAddress]  DEFAULT (N'000.000.000.000') FOR [IssueIP]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContents_LastUpdateIP]  DEFAULT ('000.000.000.000') FOR [LastModifyIP]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContents_IssueDate]  DEFAULT (getdate()) FOR [IssueDate]
GO
ALTER TABLE [dbo].[WebsiteNewsContent] ADD  CONSTRAINT [DF_WebsiteNewsContent_BullentinDate]  DEFAULT (getdate()) FOR [LastModifyDate]
GO
ALTER TABLE [dbo].[WebsiteNoticeContent] ADD  CONSTRAINT [DF_News_PoppuWin_PopTitle]  DEFAULT (N'') FOR [Subject]
GO
ALTER TABLE [dbo].[WebsiteNoticeContent] ADD  CONSTRAINT [DF_WebsiteNewsPopup_IsLink]  DEFAULT ((0)) FOR [IsLink]
GO
ALTER TABLE [dbo].[WebsiteNoticeContent] ADD  CONSTRAINT [DF_WebsiteNewsPopup_PopUrl]  DEFAULT (N'') FOR [LinkUrl]
GO
ALTER TABLE [dbo].[WebsiteNoticeContent] ADD  CONSTRAINT [DF_WebsiteNewsPopup_PopContent]  DEFAULT (N'') FOR [Body]
GO
ALTER TABLE [dbo].[WebsiteNoticeContent] ADD  CONSTRAINT [DF_News_PoppuWin_PopLocation]  DEFAULT ('') FOR [Location]
GO
ALTER TABLE [dbo].[WebsiteNoticeContent] ADD  CONSTRAINT [DF_News_PoppuWin_Width]  DEFAULT ((0)) FOR [Width]
GO
ALTER TABLE [dbo].[WebsiteNoticeContent] ADD  CONSTRAINT [DF_News_PoppuWin_Height]  DEFAULT ((0)) FOR [Height]
GO
ALTER TABLE [dbo].[WebsiteNoticeContent] ADD  CONSTRAINT [DF_WebsiteNewsPopup_StartDate]  DEFAULT (getdate()) FOR [StartDate]
GO
ALTER TABLE [dbo].[WebsiteNoticeContent] ADD  CONSTRAINT [DF_News_PoppuWin_OverDate]  DEFAULT (getdate()) FOR [OverDate]
GO
ALTER TABLE [dbo].[WebsiteNoticeContent] ADD  CONSTRAINT [DF_News_PoppuWin_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[WebsiteSidebar] ADD  CONSTRAINT [DF_Website_Sidebar_ChannelID]  DEFAULT ((0)) FOR [ChannelID]
GO
ALTER TABLE [dbo].[WebsiteSidebar] ADD  CONSTRAINT [DF_Website_Sidebar_BarTitle]  DEFAULT (N'') FOR [Title]
GO
ALTER TABLE [dbo].[WebsiteSidebar] ADD  CONSTRAINT [DF_Website_Sidebar_BarContent]  DEFAULT (N'') FOR [BarContent]
GO
ALTER TABLE [dbo].[WebsiteSidebar] ADD  CONSTRAINT [DF_Website_Sidebar_BarLogoUrl]  DEFAULT (N'') FOR [LogoUrl]
GO
ALTER TABLE [dbo].[WebsiteSidebar] ADD  CONSTRAINT [DF_Website_Sidebar_LoadPosition]  DEFAULT (N'') FOR [LoadPosition]
GO
ALTER TABLE [dbo].[WebsiteSidebar] ADD  CONSTRAINT [DF_Website_Sidebar_IsLink]  DEFAULT ((0)) FOR [IsLink]
GO
ALTER TABLE [dbo].[WebsiteSidebar] ADD  CONSTRAINT [DF_Website_Sidebar_LinkUrl]  DEFAULT (N'') FOR [LinkUrl]
GO
ALTER TABLE [dbo].[WebsiteSidebar] ADD  CONSTRAINT [DF_Website_Sidebar_IsLocked]  DEFAULT ((0)) FOR [IsLock]
GO
ALTER TABLE [dbo].[WebsiteTemplates] ADD  CONSTRAINT [DF_WebsiteTemplates_Note]  DEFAULT (N'') FOR [Note]
GO
ALTER TABLE [dbo].[WebsiteTemplates] ADD  CONSTRAINT [DF_WebsiteTemplates_TMLContent1]  DEFAULT (N'') FOR [TMLContent1]
GO
ALTER TABLE [dbo].[WebsiteTemplates] ADD  CONSTRAINT [DF_WebsiteTemplates_TMLContent2]  DEFAULT (N'') FOR [TMLContent2]
GO
ALTER TABLE [dbo].[WebsiteTemplates] ADD  CONSTRAINT [DF_WebsiteTemplates_TMLContent]  DEFAULT (N'') FOR [TMLContent3]
GO
ALTER TABLE [dbo].[WebsiteTemplates] ADD  CONSTRAINT [DF_WebsiteTemplates_IssueDate]  DEFAULT (getdate()) FOR [IssueDate]
GO
ALTER TABLE [dbo].[WebsiteTemplates] ADD  CONSTRAINT [DF_WebsiteTemplates_ModifyDate]  DEFAULT (getdate()) FOR [ModifyDate]
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'地址字符' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AllowAddress', @level2type=N'COLUMN',@level2name=N'IPAddress'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'允许登录' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AllowAddress', @level2type=N'COLUMN',@level2name=N'AllowLogon'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'过期时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AllowAddress', @level2type=N'COLUMN',@level2name=N'InvalidDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'收集日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AllowAddress', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AllowAddress', @level2type=N'COLUMN',@level2name=N'CollectNote'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'序列标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ApplyMatchInfo', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ApplyMatchInfo', @level2type=N'COLUMN',@level2name=N'MatchID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ApplyMatchInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ApplyMatchInfo', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户昵称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ApplyMatchInfo', @level2type=N'COLUMN',@level2name=N'NickName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'真实姓名' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ApplyMatchInfo', @level2type=N'COLUMN',@level2name=N'RealName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'手机号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ApplyMatchInfo', @level2type=N'COLUMN',@level2name=N'MobilePhone'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'报名时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ApplyMatchInfo', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'领取红包验证码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ApplyMatchRedPack', @level2type=N'COLUMN',@level2name=N'CodeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'比赛标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ApplyMatchRedPack', @level2type=N'COLUMN',@level2name=N'MatchID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'红包金额' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ApplyMatchRedPack', @level2type=N'COLUMN',@level2name=N'Amount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建验证码时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ApplyMatchRedPack', @level2type=N'COLUMN',@level2name=N'CreateDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'领取用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ApplyMatchRedPack', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'领取地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ApplyMatchRedPack', @level2type=N'COLUMN',@level2name=N'RecevieIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'缓存标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsEvents', @level2type=N'COLUMN',@level2name=N'CacheID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'币种标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsEvents', @level2type=N'COLUMN',@level2name=N'CurrencyID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'种类标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsEvents', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsEvents', @level2type=N'COLUMN',@level2name=N'ModuleID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类别标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsEvents', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsEvents', @level2type=N'COLUMN',@level2name=N'KindName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Top N' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsEvents', @level2type=N'COLUMN',@level2name=N'TopN'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'更新时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsEvents', @level2type=N'COLUMN',@level2name=N'UpdateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'名次标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsGold', @level2type=N'COLUMN',@level2name=N'ChartID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'币种标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsGold', @level2type=N'COLUMN',@level2name=N'CurrencyID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsGold', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsGold', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户帐号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsGold', @level2type=N'COLUMN',@level2name=N'Accounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户金币' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsGold', @level2type=N'COLUMN',@level2name=N'Score'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赢局数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsGold', @level2type=N'COLUMN',@level2name=N'WinCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'输局数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsGold', @level2type=N'COLUMN',@level2name=N'LostCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'逃局数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsGold', @level2type=N'COLUMN',@level2name=N'FleeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'和局数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsGold', @level2type=N'COLUMN',@level2name=N'DrawCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'名次标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsScore', @level2type=N'COLUMN',@level2name=N'ChartID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏种类' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsScore', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'站点标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsScore', @level2type=N'COLUMN',@level2name=N'StationID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsScore', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsScore', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户帐号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsScore', @level2type=N'COLUMN',@level2name=N'Accounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户积分' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsScore', @level2type=N'COLUMN',@level2name=N'Score'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赢局数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsScore', @level2type=N'COLUMN',@level2name=N'WinCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'输局数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsScore', @level2type=N'COLUMN',@level2name=N'LostCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'逃局数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsScore', @level2type=N'COLUMN',@level2name=N'FleeCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'和局数目' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'CacheChartsScore', @level2type=N'COLUMN',@level2name=N'DrawCount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'事件标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ScheduledEventsInfo', @level2type=N'COLUMN',@level2name=N'EventID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'事件标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ScheduledEventsInfo', @level2type=N'COLUMN',@level2name=N'Key'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'每日执行时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ScheduledEventsInfo', @level2type=N'COLUMN',@level2name=N'TimeOfDay'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'每次执行的周期时间(分钟)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ScheduledEventsInfo', @level2type=N'COLUMN',@level2name=N'Minutes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'实现了 IEvent 的类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ScheduledEventsInfo', @level2type=N'COLUMN',@level2name=N'ScheduleType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'是否启用此任务' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ScheduledEventsInfo', @level2type=N'COLUMN',@level2name=N'Enabled'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'是否是系统级任务(如果是, 则不允许关闭)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ScheduledEventsInfo', @level2type=N'COLUMN',@level2name=N'IsSystemEvent'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'任务标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ScheduledEventsLogs', @level2type=N'COLUMN',@level2name=N'ScheduleID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'任务键值' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ScheduledEventsLogs', @level2type=N'COLUMN',@level2name=N'Key'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上次执行时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ScheduledEventsLogs', @level2type=N'COLUMN',@level2name=N'LastExecuted'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'机器名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ScheduledEventsLogs', @level2type=N'COLUMN',@level2name=N'ServerName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'整合标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemExtendCenter', @level2type=N'COLUMN',@level2name=N'ExtendID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'站点标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemExtendCenter', @level2type=N'COLUMN',@level2name=N'StationID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'应用名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemExtendCenter', @level2type=N'COLUMN',@level2name=N'AppName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'应用地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemExtendCenter', @level2type=N'COLUMN',@level2name=N'AppUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'接口代码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemExtendCenter', @level2type=N'COLUMN',@level2name=N'APIKey'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'整合密钥' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemExtendCenter', @level2type=N'COLUMN',@level2name=N'SecretKey'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'地址列表' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemExtendCenter', @level2type=N'COLUMN',@level2name=N'IPAddresses'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁用标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemExtendCenter', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'模块标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemModuleInfo', @level2type=N'COLUMN',@level2name=N'ModuleID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上级标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemModuleInfo', @level2type=N'COLUMN',@level2name=N'ParentID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'模块名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemModuleInfo', @level2type=N'COLUMN',@level2name=N'Title'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'图标地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemModuleInfo', @level2type=N'COLUMN',@level2name=N'Imgurl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'链接地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemModuleInfo', @level2type=N'COLUMN',@level2name=N'Linkurl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排序标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemModuleInfo', @level2type=N'COLUMN',@level2name=N'SortID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'框架标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemModuleInfo', @level2type=N'COLUMN',@level2name=N'FrameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁用标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemModuleInfo', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'模块标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemModulePermission', @level2type=N'COLUMN',@level2name=N'ModuleID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'权限标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemModulePermission', @level2type=N'COLUMN',@level2name=N'PermissionID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'权限名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemModulePermission', @level2type=N'COLUMN',@level2name=N'PermissionName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'权限代码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemModulePermission', @level2type=N'COLUMN',@level2name=N'PermissionValue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'记录标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemRecordInfo', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'站点标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemRecordInfo', @level2type=N'COLUMN',@level2name=N'StationID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemRecordInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'事件类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemRecordInfo', @level2type=N'COLUMN',@level2name=N'EventType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'时间级别' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemRecordInfo', @level2type=N'COLUMN',@level2name=N'EventLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'时间描述' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemRecordInfo', @level2type=N'COLUMN',@level2name=N'EventDetail'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'事件地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemRecordInfo', @level2type=N'COLUMN',@level2name=N'EventUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'事件地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemRecordInfo', @level2type=N'COLUMN',@level2name=N'EventIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'事件时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemRecordInfo', @level2type=N'COLUMN',@level2name=N'EventDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'状态名字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStatusInfo', @level2type=N'COLUMN',@level2name=N'StatusName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'状态数值' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStatusInfo', @level2type=N'COLUMN',@level2name=N'StatusValue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'状态字符' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStatusInfo', @level2type=N'COLUMN',@level2name=N'StatusString'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserAccountsInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'所属角色' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserAccountsInfo', @level2type=N'COLUMN',@level2name=N'RoleID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'站点标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserAccountsInfo', @level2type=N'COLUMN',@level2name=N'StationID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户帐号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserAccountsInfo', @level2type=N'COLUMN',@level2name=N'Accounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录密码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserAccountsInfo', @level2type=N'COLUMN',@level2name=N'LogonPass'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'真实姓名' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserAccountsInfo', @level2type=N'COLUMN',@level2name=N'Realname'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'联系邮箱' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserAccountsInfo', @level2type=N'COLUMN',@level2name=N'LinkEmail'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁用标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserAccountsInfo', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登录次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserAccountsInfo', @level2type=N'COLUMN',@level2name=N'LogonTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'注册地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserAccountsInfo', @level2type=N'COLUMN',@level2name=N'RegisterIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上次登录地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserAccountsInfo', @level2type=N'COLUMN',@level2name=N'LastLogonIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'注册时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserAccountsInfo', @level2type=N'COLUMN',@level2name=N'RegisterDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上次登录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserAccountsInfo', @level2type=N'COLUMN',@level2name=N'LastLogonDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'角色标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserRolesInfo', @level2type=N'COLUMN',@level2name=N'RoleID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'角色名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserRolesInfo', @level2type=N'COLUMN',@level2name=N'Rolename'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'禁用标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserRolesInfo', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'角色标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserRolesPermission', @level2type=N'COLUMN',@level2name=N'RoleID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'模块标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserRolesPermission', @level2type=N'COLUMN',@level2name=N'ModuleID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'模块权限' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'UserRolesPermission', @level2type=N'COLUMN',@level2name=N'PermissionValue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'频道编号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannel', @level2type=N'COLUMN',@level2name=N'ChannelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'调用标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannel', @level2type=N'COLUMN',@level2name=N'Token'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'频道名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannel', @level2type=N'COLUMN',@level2name=N'Title'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'频道Logo地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannel', @level2type=N'COLUMN',@level2name=N'LogoUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'附加标题' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannel', @level2type=N'COLUMN',@level2name=N'SeoTitle'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'附加头部' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannel', @level2type=N'COLUMN',@level2name=N'SeoHead'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Meta关键字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannel', @level2type=N'COLUMN',@level2name=N'SeoKeymeta'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Meta描述' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannel', @level2type=N'COLUMN',@level2name=N'SeoDescmeta'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'皮肤名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannel', @level2type=N'COLUMN',@level2name=N'Themename'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'皮肤路径' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannel', @level2type=N'COLUMN',@level2name=N'Themepath'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'模板路径' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannel', @level2type=N'COLUMN',@level2name=N'Templetpath'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'缓存路径' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannel', @level2type=N'COLUMN',@level2name=N'CachedPath'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'频道首页' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannel', @level2type=N'COLUMN',@level2name=N'IndexPagename'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'页面大小' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannel', @level2type=N'COLUMN',@level2name=N'PageSize'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排序标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannel', @level2type=N'COLUMN',@level2name=N'SortID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'外部链接 0 非外部链接; 1 外部链接' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannel', @level2type=N'COLUMN',@level2name=N'IsLink'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'链接地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannel', @level2type=N'COLUMN',@level2name=N'LinkUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'锁定标识  0 有效; 1 锁定' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannel', @level2type=N'COLUMN',@level2name=N'IsLock'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'可视标识  0 可视;1 隐藏 可视作为菜单项使用,隐藏作为后台调用代码使用' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannel', @level2type=N'COLUMN',@level2name=N'IsVisible'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'静态文件 0:动态页面 1:静态页面' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannel', @level2type=N'COLUMN',@level2name=N'IsHtml'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'摘要标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannelDigest', @level2type=N'COLUMN',@level2name=N'DigestID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'频道标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannelDigest', @level2type=N'COLUMN',@level2name=N'ChannelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'调用标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannelDigest', @level2type=N'COLUMN',@level2name=N'Token'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'摘要内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannelDigest', @level2type=N'COLUMN',@level2name=N'DigestContent'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'加载位置' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteChannelDigest', @level2type=N'COLUMN',@level2name=N'LoadPosition'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'smtp 地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteEmailConfigInfo', @level2type=N'COLUMN',@level2name=N'Smtp'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'端口号码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteEmailConfigInfo', @level2type=N'COLUMN',@level2name=N'Port'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'系统邮件地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteEmailConfigInfo', @level2type=N'COLUMN',@level2name=N'Sysemail'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'邮件帐号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteEmailConfigInfo', @level2type=N'COLUMN',@level2name=N'Username'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'邮件密码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteEmailConfigInfo', @level2type=N'COLUMN',@level2name=N'Password'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'邮件内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteEmailConfigInfo', @level2type=N'COLUMN',@level2name=N'Emailcontent'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'GameID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'模块标识(默认不可更改)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'ModuleID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'类别标识(可以定制)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'分类标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'KindName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏别名(名字辅助描述)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'KindNameAlias'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏摘要(简短描述)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'KindIntro'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'版本号码(8.10.0.1)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'MaxVersion'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'应用平台' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'Platforms'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'软件大小' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'InstalledSize'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'安装包名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'InstallName1'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'安装包名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'InstallName2'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'安装包名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'InstallName3'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'是否大厅(0 单款游戏,1 独立大厅,2 完整大厅)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'IsPlaza'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'安装包图片' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'ImgInstalledUrl1'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'安装包图片' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'ImgInstalledUrl2'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'规则截图' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'ImgRuleUrl1'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'规则截图' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'ImgRuleUrl2'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推荐缩略图' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'ImgSpreadUrl1'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推荐缩略图' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'ImgSpreadUrl2'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'页面关键词' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'Keywords'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'页面描述' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'Description'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏介绍' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'HelpIntro'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏规则' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'HelpRule'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'积分等级' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'HelpGrade'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'补充内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'HelpContent1'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'补充内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'HelpContent2'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'补充内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'HelpContent3'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'加入推荐(0 不加入,1 加入)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'JoinSpread'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'加入介绍(0 不加入,1 加入)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'JoinIntro'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'有效标识(0 有效; 1 无效)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'发布日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'IssueDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修订日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInfo', @level2type=N'COLUMN',@level2name=N'ModifyDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInstall', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'进程名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInstall', @level2type=N'COLUMN',@level2name=N'ProcessName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'图标路径' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInstall', @level2type=N'COLUMN',@level2name=N'InstallImgUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下载路径' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInstall', @level2type=N'COLUMN',@level2name=N'InstallUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下载提示' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameInstall', @level2type=N'COLUMN',@level2name=N'InstallToolTips'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameNominate', @level2type=N'COLUMN',@level2name=N'NominateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏类别' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameNominate', @level2type=N'COLUMN',@level2name=N'TypeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameNominate', @level2type=N'COLUMN',@level2name=N'KindID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameNominate', @level2type=N'COLUMN',@level2name=N'KindName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏Logo' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameNominate', @level2type=N'COLUMN',@level2name=N'LogoUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'推广地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameNominate', @level2type=N'COLUMN',@level2name=N'SpreadUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下载提示' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameNominate', @level2type=N'COLUMN',@level2name=N'InstallToolTips'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下载地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameNominate', @level2type=N'COLUMN',@level2name=N'InstallUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'进程名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameNominate', @level2type=N'COLUMN',@level2name=N'ProcessName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏规则' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameNominate', @level2type=N'COLUMN',@level2name=N'RuleUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排序标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGameNominate', @level2type=N'COLUMN',@level2name=N'SortID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'系统标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGeneralInfo', @level2type=N'COLUMN',@level2name=N'GeneralID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'网站标题' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGeneralInfo', @level2type=N'COLUMN',@level2name=N'WebTitle'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'网站域名' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGeneralInfo', @level2type=N'COLUMN',@level2name=N'WebUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'标题附加字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGeneralInfo', @level2type=N'COLUMN',@level2name=N'SeoTitle'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Meta Keywords' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGeneralInfo', @level2type=N'COLUMN',@level2name=N'SeoKeywords'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Meta Description' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGeneralInfo', @level2type=N'COLUMN',@level2name=N'SeoDescription'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'其他头部信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGeneralInfo', @level2type=N'COLUMN',@level2name=N'SeoHead'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'身份验证Cookie域名' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGeneralInfo', @level2type=N'COLUMN',@level2name=N'CookieDomain'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'cookie保存时限;0 浏览器进程保存 单位(秒)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGeneralInfo', @level2type=N'COLUMN',@level2name=N'CookieKeepTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'加密 key' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGeneralInfo', @level2type=N'COLUMN',@level2name=N'CookieEncryptKey'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'游戏大厅同步登录' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGeneralInfo', @level2type=N'COLUMN',@level2name=N'AutoLogonPlaza'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'头像最大标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGeneralInfo', @level2type=N'COLUMN',@level2name=N'MaxUserfaceID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'头像路径' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGeneralInfo', @level2type=N'COLUMN',@level2name=N'UserfaceUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'皮肤名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGeneralInfo', @level2type=N'COLUMN',@level2name=N'ThemeName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'皮肤路径' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGeneralInfo', @level2type=N'COLUMN',@level2name=N'ThemePath'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'启用日志记录级别' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGeneralInfo', @level2type=N'COLUMN',@level2name=N'LogLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'icp备案' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGeneralInfo', @level2type=N'COLUMN',@level2name=N'IcpInfo'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'网站统计代码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteGeneralInfo', @level2type=N'COLUMN',@level2name=N'WebStat'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'问题标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteIssueContent', @level2type=N'COLUMN',@level2name=N'IssueID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'问题类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteIssueContent', @level2type=N'COLUMN',@level2name=N'IssueType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'问题标题' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteIssueContent', @level2type=N'COLUMN',@level2name=N'IssueTitle'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'问题内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteIssueContent', @level2type=N'COLUMN',@level2name=N'IssueContent'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'是否禁止' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteIssueContent', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'新增日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteIssueContent', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'更新日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteIssueContent', @level2type=N'COLUMN',@level2name=N'ModifyDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'公告标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'NewsID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'弹出窗口' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'PopID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'新闻主标题' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'Subject'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'新闻副标题' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'Subject1'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'置顶标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'OnTop'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'总置顶标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'OnTopAll'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'精华标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'IsElite'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'热点标识 (0, 1　热点)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'IsHot'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'锁定标识(0 锁定 1 发布)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'IsLock'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'删除标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'IsDelete'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'全局标识 (0:仅本站点;1:所有从站点)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'IsGlobal'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'外部链接标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'IsLinks'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'外部链接地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'LinkUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'公告内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'Body'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'带格式公告内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'FormattedBody'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'关键字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'Keywords'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'摘要' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'Brief'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'标题颜色' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'HighLight'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'使用图标' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'Icon'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'新闻类别 (维护消息 1; 系统消息 2，游戏下载3)' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'ClassID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'设备类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'DeviceID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'发布人' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'Author'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'发布人标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'发布地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'IssueIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后更新地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'LastModifyIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'发布时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'IssueDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'更新时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'LastModifyDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'过期时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNewsContent', @level2type=N'COLUMN',@level2name=N'OverDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'窗口标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNoticeContent', @level2type=N'COLUMN',@level2name=N'NoticeID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'窗口标题' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNoticeContent', @level2type=N'COLUMN',@level2name=N'Subject'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'外部链接' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNoticeContent', @level2type=N'COLUMN',@level2name=N'IsLink'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'窗口地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNoticeContent', @level2type=N'COLUMN',@level2name=N'LinkUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'窗口内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNoticeContent', @level2type=N'COLUMN',@level2name=N'Body'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'弹出位置' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNoticeContent', @level2type=N'COLUMN',@level2name=N'Location'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'窗口宽度' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNoticeContent', @level2type=N'COLUMN',@level2name=N'Width'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'窗口高度' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNoticeContent', @level2type=N'COLUMN',@level2name=N'Height'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'开始时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNoticeContent', @level2type=N'COLUMN',@level2name=N'StartDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'结束时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNoticeContent', @level2type=N'COLUMN',@level2name=N'OverDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'无效标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteNoticeContent', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'侧栏编号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteSidebar', @level2type=N'COLUMN',@level2name=N'BarID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'频道标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteSidebar', @level2type=N'COLUMN',@level2name=N'ChannelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'调用标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteSidebar', @level2type=N'COLUMN',@level2name=N'Token'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'侧栏标题' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteSidebar', @level2type=N'COLUMN',@level2name=N'Title'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'侧栏内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteSidebar', @level2type=N'COLUMN',@level2name=N'BarContent'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Logo地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteSidebar', @level2type=N'COLUMN',@level2name=N'LogoUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'加载位置' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteSidebar', @level2type=N'COLUMN',@level2name=N'LoadPosition'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'是否链接 0 不是; 1 是' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteSidebar', @level2type=N'COLUMN',@level2name=N'IsLink'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'链接地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteSidebar', @level2type=N'COLUMN',@level2name=N'LinkUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'锁定标识  0 有效; 1 锁定' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteSidebar', @level2type=N'COLUMN',@level2name=N'IsLock'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'模板标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteTemplates', @level2type=N'COLUMN',@level2name=N'TMLID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'调用标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteTemplates', @level2type=N'COLUMN',@level2name=N'Token'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'备注信息' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteTemplates', @level2type=N'COLUMN',@level2name=N'Note'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'模板内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteTemplates', @level2type=N'COLUMN',@level2name=N'TMLContent1'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'模板内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteTemplates', @level2type=N'COLUMN',@level2name=N'TMLContent2'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'模板内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteTemplates', @level2type=N'COLUMN',@level2name=N'TMLContent3'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteTemplates', @level2type=N'COLUMN',@level2name=N'IssueDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'修订日期' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'WebsiteTemplates', @level2type=N'COLUMN',@level2name=N'ModifyDate'
GO
