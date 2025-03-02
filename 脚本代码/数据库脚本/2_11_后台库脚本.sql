USE [LHWebAdminDB]
GO
/****** Object:  Table [dbo].[AdminInfo]    Script Date: 2023/7/12 11:12:47 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AdminInfo](
	[UserID] [int] IDENTITY(1,1) NOT NULL,
	[UserName] [nvarchar](32) NOT NULL,
	[Password] [nvarchar](32) NOT NULL,
	[Avatar] [int] NOT NULL,
	[RoleID] [int] NOT NULL,
	[SiteIDList] [varchar](max) NOT NULL,
	[Mobile] [nvarchar](16) NULL,
	[Email] [nvarchar](256) NULL,
	[IsBand] [bit] NOT NULL,
	[BandIP] [varchar](15) NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[PreLoginIP] [varchar](15) NULL,
	[PreLoginDate] [datetime] NULL,
	[LastLoginIP] [varchar](15) NULL,
	[LastLoginDate] [datetime] NULL,
	[LoginTimes] [int] NOT NULL,
	[CollectIP] [varchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_AdminInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[AdminIPWhitelist]    Script Date: 2023/7/12 11:12:47 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AdminIPWhitelist](
	[IP] [nvarchar](15) NOT NULL,
	[CollectDateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_AdminIPWhitelist] PRIMARY KEY CLUSTERED 
(
	[IP] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ChannelAdminInfo]    Script Date: 2023/7/12 11:12:47 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ChannelAdminInfo](
	[UserID] [int] IDENTITY(1,1) NOT NULL,
	[ChannelID] [int] NOT NULL,
	[UserName] [nvarchar](32) NOT NULL,
	[Password] [nvarchar](32) NOT NULL,
	[Avatar] [int] NOT NULL,
	[RoleID] [int] NOT NULL,
	[SiteIDList] [varchar](max) NOT NULL,
	[Mobile] [nvarchar](16) NULL,
	[Email] [nvarchar](256) NULL,
	[IsBand] [bit] NOT NULL,
	[BandIP] [varchar](15) NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[PreLoginIP] [varchar](15) NULL,
	[PreLoginDate] [datetime] NULL,
	[LastLoginIP] [varchar](15) NULL,
	[LastLoginDate] [datetime] NULL,
	[LoginTimes] [int] NOT NULL,
	[CollectIP] [varchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_ChannelAdminInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[LogAccount]    Script Date: 2023/7/12 11:12:47 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[LogAccount](
	[LogID] [bigint] IDENTITY(1,1) NOT NULL,
	[LogContent] [nvarchar](1024) NULL,
	[LogType] [int] NULL,
	[LogLevel] [tinyint] NOT NULL,
	[ManagerID] [int] NOT NULL,
	[ManagerName] [nvarchar](32) NOT NULL,
	[RequestUrl] [nvarchar](256) NULL,
	[UserID] [nvarchar](max) NOT NULL,
	[Accounts] [nvarchar](max) NOT NULL,
	[StatusCode] [int] NOT NULL,
	[StatusDescribe] [nvarchar](512) NULL,
	[CostTime] [float] NOT NULL,
	[OperateIP] [varchar](39) NOT NULL,
	[Browser] [nvarchar](512) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_LogAccount] PRIMARY KEY CLUSTERED 
(
	[LogID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[LogException]    Script Date: 2023/7/12 11:12:47 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[LogException](
	[LogID] [int] IDENTITY(1,1) NOT NULL,
	[RequestUrl] [nvarchar](512) NOT NULL,
	[ExceptionMessage] [nvarchar](max) NOT NULL,
	[Browser] [nvarchar](512) NOT NULL,
	[RequstIP] [varchar](13) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_ErrorLog] PRIMARY KEY CLUSTERED 
(
	[LogID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[LogManage]    Script Date: 2023/7/12 11:12:47 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[LogManage](
	[LogID] [bigint] IDENTITY(1,1) NOT NULL,
	[LogContent] [nvarchar](1024) NOT NULL,
	[LogType] [int] NOT NULL,
	[LogLevel] [tinyint] NOT NULL,
	[ManagerID] [int] NOT NULL,
	[ManagerName] [nvarchar](32) NOT NULL,
	[RequestUrl] [nvarchar](256) NULL,
	[StatusCode] [int] NOT NULL,
	[StatusDescribe] [nvarchar](512) NULL,
	[CostTime] [float] NOT NULL,
	[OperateIP] [varchar](39) NOT NULL,
	[Browser] [nvarchar](512) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_LogSystems_LogID] PRIMARY KEY CLUSTERED 
(
	[LogID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[LogService]    Script Date: 2023/7/12 11:12:47 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[LogService](
	[LogID] [int] IDENTITY(1,1) NOT NULL,
	[LogContent] [nvarchar](1024) NOT NULL,
	[UserID] [int] NOT NULL,
	[Accounts] [nvarchar](32) NOT NULL,
	[SiteID] [int] NOT NULL,
	[ManagerID] [int] NOT NULL,
	[ManagerName] [nvarchar](32) NOT NULL,
	[OperateIP] [nvarchar](39) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_LogService] PRIMARY KEY CLUSTERED 
(
	[LogID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ModuleInfo]    Script Date: 2023/7/12 11:12:47 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ModuleInfo](
	[ModuleID] [int] NOT NULL,
	[ParentID] [int] NOT NULL,
	[ModuleName] [nvarchar](20) NOT NULL,
	[ModuleLink] [varchar](256) NOT NULL,
	[IsMenu] [bit] NOT NULL,
	[OrderNo] [int] NOT NULL,
	[Nullity] [bit] NOT NULL,
	[Description] [nvarchar](256) NOT NULL,
 CONSTRAINT [PK_ModuleInfo] PRIMARY KEY CLUSTERED 
(
	[ModuleID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[ModulePermission]    Script Date: 2023/7/12 11:12:47 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ModulePermission](
	[ModuleID] [int] NOT NULL,
	[PermissionValue] [bigint] NOT NULL,
	[PermissionName] [nvarchar](128) NOT NULL,
	[Nullity] [bit] NOT NULL,
 CONSTRAINT [PK_ModelPermission] PRIMARY KEY CLUSTERED 
(
	[ModuleID] ASC,
	[PermissionValue] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RoleInfo]    Script Date: 2023/7/12 11:12:47 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RoleInfo](
	[RoleID] [int] IDENTITY(1,1) NOT NULL,
	[RoleName] [nvarchar](32) NOT NULL,
	[Avatar] [int] NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[ModifyDate] [datetime] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[Description] [nvarchar](32) NULL,
 CONSTRAINT [PK_RoleInfo] PRIMARY KEY CLUSTERED 
(
	[RoleID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[RolePermission]    Script Date: 2023/7/12 11:12:47 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[RolePermission](
	[RoleID] [int] NOT NULL,
	[ModuleID] [int] NOT NULL,
	[ManagerPermission] [bigint] NOT NULL,
	[OperationPermission] [bigint] NOT NULL,
 CONSTRAINT [PK_RolePermission] PRIMARY KEY CLUSTERED 
(
	[RoleID] ASC,
	[ModuleID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
ALTER TABLE [dbo].[AdminInfo] ADD  CONSTRAINT [DF_AdminInfo_Avatar]  DEFAULT ((0)) FOR [Avatar]
GO
ALTER TABLE [dbo].[AdminInfo] ADD  CONSTRAINT [DF_AdminInfo_SiteIDList]  DEFAULT ((0)) FOR [SiteIDList]
GO
ALTER TABLE [dbo].[AdminInfo] ADD  CONSTRAINT [DF_AdminInfo_IsBand]  DEFAULT ((0)) FOR [IsBand]
GO
ALTER TABLE [dbo].[AdminInfo] ADD  CONSTRAINT [DF_AdminInfo_BandIP]  DEFAULT ('000.000.000.000') FOR [BandIP]
GO
ALTER TABLE [dbo].[AdminInfo] ADD  CONSTRAINT [DF_AdminInfo_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[AdminInfo] ADD  CONSTRAINT [DF_AdminInfo_PreLoginIP]  DEFAULT ('') FOR [PreLoginIP]
GO
ALTER TABLE [dbo].[AdminInfo] ADD  CONSTRAINT [DF_AdminInfo_LastLoginIP]  DEFAULT ('') FOR [LastLoginIP]
GO
ALTER TABLE [dbo].[AdminInfo] ADD  CONSTRAINT [DF_AdminInfo_LoginTimes]  DEFAULT ((0)) FOR [LoginTimes]
GO
ALTER TABLE [dbo].[AdminInfo] ADD  CONSTRAINT [DF_AdminInfo_InsertDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[AdminIPWhitelist] ADD  CONSTRAINT [DF_AdminIPWhitelist_CollectDateTime]  DEFAULT (getdate()) FOR [CollectDateTime]
GO
ALTER TABLE [dbo].[ChannelAdminInfo] ADD  CONSTRAINT [DF_ChannelAdminInfo_ChannelID]  DEFAULT ((1)) FOR [ChannelID]
GO
ALTER TABLE [dbo].[ChannelAdminInfo] ADD  CONSTRAINT [DF_ChannelAdminInfo_Avatar]  DEFAULT ((0)) FOR [Avatar]
GO
ALTER TABLE [dbo].[ChannelAdminInfo] ADD  CONSTRAINT [DF_ChannelAdminInfo_SiteIDList]  DEFAULT ((0)) FOR [SiteIDList]
GO
ALTER TABLE [dbo].[ChannelAdminInfo] ADD  CONSTRAINT [DF_ChannelAdminInfo_IsBand]  DEFAULT ((0)) FOR [IsBand]
GO
ALTER TABLE [dbo].[ChannelAdminInfo] ADD  CONSTRAINT [DF_ChannelAdminInfo_BandIP]  DEFAULT ('000.000.000.000') FOR [BandIP]
GO
ALTER TABLE [dbo].[ChannelAdminInfo] ADD  CONSTRAINT [DF_ChannelAdminInfo_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[ChannelAdminInfo] ADD  CONSTRAINT [DF_ChannelAdminInfo_PreLoginIP]  DEFAULT ('') FOR [PreLoginIP]
GO
ALTER TABLE [dbo].[ChannelAdminInfo] ADD  CONSTRAINT [DF_ChannelAdminInfo_LastLoginIP]  DEFAULT ('') FOR [LastLoginIP]
GO
ALTER TABLE [dbo].[ChannelAdminInfo] ADD  CONSTRAINT [DF_ChannelAdminInfo_LoginTimes]  DEFAULT ((0)) FOR [LoginTimes]
GO
ALTER TABLE [dbo].[ChannelAdminInfo] ADD  CONSTRAINT [DF_ChannelAdminInfo_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[LogAccount] ADD  CONSTRAINT [DF_LogAccount_LogLevel]  DEFAULT ((1)) FOR [LogLevel]
GO
ALTER TABLE [dbo].[LogAccount] ADD  CONSTRAINT [DF_LogAccount_ManagerID]  DEFAULT ((0)) FOR [ManagerID]
GO
ALTER TABLE [dbo].[LogAccount] ADD  CONSTRAINT [DF_LogAccount_StatusCode]  DEFAULT ((200)) FOR [StatusCode]
GO
ALTER TABLE [dbo].[LogAccount] ADD  CONSTRAINT [DF_LogAccount_StatusDescribe]  DEFAULT ('') FOR [StatusDescribe]
GO
ALTER TABLE [dbo].[LogAccount] ADD  CONSTRAINT [DF_LogAccount_CostTime]  DEFAULT ((0)) FOR [CostTime]
GO
ALTER TABLE [dbo].[LogAccount] ADD  CONSTRAINT [DF_LogAccount_OperateIP]  DEFAULT ('') FOR [OperateIP]
GO
ALTER TABLE [dbo].[LogAccount] ADD  CONSTRAINT [DF_LogAccount_Browser]  DEFAULT ('') FOR [Browser]
GO
ALTER TABLE [dbo].[LogAccount] ADD  CONSTRAINT [DF_LogAccount_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[LogException] ADD  CONSTRAINT [DF_ErrorLog_Type]  DEFAULT ((1)) FOR [RequestUrl]
GO
ALTER TABLE [dbo].[LogException] ADD  CONSTRAINT [DF_Table_1_Content]  DEFAULT ('') FOR [ExceptionMessage]
GO
ALTER TABLE [dbo].[LogException] ADD  CONSTRAINT [DF_LogError_OperateIP]  DEFAULT ('') FOR [RequstIP]
GO
ALTER TABLE [dbo].[LogException] ADD  CONSTRAINT [DF_ErrorLog_InsertDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[LogManage] ADD  CONSTRAINT [DF_SystemsLog_LogContent]  DEFAULT ('') FOR [LogContent]
GO
ALTER TABLE [dbo].[LogManage] ADD  CONSTRAINT [DF_SystemsLog_LogType]  DEFAULT ((0)) FOR [LogType]
GO
ALTER TABLE [dbo].[LogManage] ADD  CONSTRAINT [DF_LogManage_LogLevel]  DEFAULT ((1)) FOR [LogLevel]
GO
ALTER TABLE [dbo].[LogManage] ADD  CONSTRAINT [DF_LogManage_ManagerID]  DEFAULT ((0)) FOR [ManagerID]
GO
ALTER TABLE [dbo].[LogManage] ADD  CONSTRAINT [DF_LogManage_StatusCode]  DEFAULT ((200)) FOR [StatusCode]
GO
ALTER TABLE [dbo].[LogManage] ADD  CONSTRAINT [DF_LogManage_StatusDescribe]  DEFAULT ('') FOR [StatusDescribe]
GO
ALTER TABLE [dbo].[LogManage] ADD  CONSTRAINT [DF_LogManage_CostTime]  DEFAULT ((0)) FOR [CostTime]
GO
ALTER TABLE [dbo].[LogManage] ADD  CONSTRAINT [DF_Table_2_AdminIP]  DEFAULT ('') FOR [OperateIP]
GO
ALTER TABLE [dbo].[LogManage] ADD  CONSTRAINT [DF_LogManage_Browser]  DEFAULT ('') FOR [Browser]
GO
ALTER TABLE [dbo].[LogManage] ADD  CONSTRAINT [DF_SystemsLog_InsertDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[LogService] ADD  CONSTRAINT [DF_LogService_SiteID]  DEFAULT ((0)) FOR [SiteID]
GO
ALTER TABLE [dbo].[LogService] ADD  CONSTRAINT [DF_LogService_ManagerID]  DEFAULT ((0)) FOR [ManagerID]
GO
ALTER TABLE [dbo].[LogService] ADD  CONSTRAINT [DF_LogService_ManagerName]  DEFAULT ('') FOR [ManagerName]
GO
ALTER TABLE [dbo].[LogService] ADD  CONSTRAINT [DF_LogService_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[ModuleInfo] ADD  CONSTRAINT [DF_ModuleInfo_ParentID]  DEFAULT ((0)) FOR [ParentID]
GO
ALTER TABLE [dbo].[ModuleInfo] ADD  CONSTRAINT [DF_ModuleInfo_ModuleName]  DEFAULT ('') FOR [ModuleName]
GO
ALTER TABLE [dbo].[ModuleInfo] ADD  CONSTRAINT [DF_ModuleInfo_MoluleUrl]  DEFAULT ('') FOR [ModuleLink]
GO
ALTER TABLE [dbo].[ModuleInfo] ADD  CONSTRAINT [DF_ModuleInfo_IsMenu]  DEFAULT ((1)) FOR [IsMenu]
GO
ALTER TABLE [dbo].[ModuleInfo] ADD  CONSTRAINT [DF_ModuleInfo_OrderNo]  DEFAULT ((0)) FOR [OrderNo]
GO
ALTER TABLE [dbo].[ModuleInfo] ADD  CONSTRAINT [DF_ModuleInfo_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[ModuleInfo] ADD  CONSTRAINT [DF_ModuleInfo_Description]  DEFAULT ('') FOR [Description]
GO
ALTER TABLE [dbo].[ModulePermission] ADD  CONSTRAINT [DF_ModelPermission_PermissionName]  DEFAULT ('') FOR [PermissionName]
GO
ALTER TABLE [dbo].[ModulePermission] ADD  CONSTRAINT [DF_ModelPermission_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[RoleInfo] ADD  CONSTRAINT [DF_RoleInfo_FaceID]  DEFAULT ((0)) FOR [Avatar]
GO
ALTER TABLE [dbo].[RoleInfo] ADD  CONSTRAINT [DF_RoleInfo_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[RoleInfo] ADD  CONSTRAINT [DF_RoleInfo_ModifyDate]  DEFAULT (getdate()) FOR [ModifyDate]
GO
ALTER TABLE [dbo].[RoleInfo] ADD  CONSTRAINT [DF_SysRoleInfo_InsertDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[RoleInfo] ADD  CONSTRAINT [DF_RoleInfo_Description]  DEFAULT ('') FOR [Description]
GO
ALTER TABLE [dbo].[RolePermission] ADD  CONSTRAINT [DF_RolePermission_ManagerPermission]  DEFAULT ((0)) FOR [ManagerPermission]
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'网站后台管理员表ID，自增主键' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AdminInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理员账号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AdminInfo', @level2type=N'COLUMN',@level2name=N'UserName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'密码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AdminInfo', @level2type=N'COLUMN',@level2name=N'Password'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'头像ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AdminInfo', @level2type=N'COLUMN',@level2name=N'Avatar'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'角色ID ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AdminInfo', @level2type=N'COLUMN',@level2name=N'RoleID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'站点权限列表，多个用","分格，all表示具有所有站点权限。' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AdminInfo', @level2type=N'COLUMN',@level2name=N'SiteIDList'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'是否绑定IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AdminInfo', @level2type=N'COLUMN',@level2name=N'IsBand'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'绑定的IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AdminInfo', @level2type=N'COLUMN',@level2name=N'BandIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'冻结标识 0:启用，1:冻结' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AdminInfo', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上次登录IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AdminInfo', @level2type=N'COLUMN',@level2name=N'PreLoginIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上次登录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AdminInfo', @level2type=N'COLUMN',@level2name=N'PreLoginDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后登陆IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AdminInfo', @level2type=N'COLUMN',@level2name=N'LastLoginIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后登陆时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AdminInfo', @level2type=N'COLUMN',@level2name=N'LastLoginDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登陆次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AdminInfo', @level2type=N'COLUMN',@level2name=N'LoginTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AdminInfo', @level2type=N'COLUMN',@level2name=N'CollectIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AdminInfo', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AdminIPWhitelist', @level2type=N'COLUMN',@level2name=N'CollectDateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'网站后台管理员表ID，自增主键' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelAdminInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'渠道标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelAdminInfo', @level2type=N'COLUMN',@level2name=N'ChannelID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理员账号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelAdminInfo', @level2type=N'COLUMN',@level2name=N'UserName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'密码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelAdminInfo', @level2type=N'COLUMN',@level2name=N'Password'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'头像ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelAdminInfo', @level2type=N'COLUMN',@level2name=N'Avatar'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'角色ID ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelAdminInfo', @level2type=N'COLUMN',@level2name=N'RoleID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'站点权限列表，多个用","分格，all表示具有所有站点权限。' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelAdminInfo', @level2type=N'COLUMN',@level2name=N'SiteIDList'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'是否绑定IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelAdminInfo', @level2type=N'COLUMN',@level2name=N'IsBand'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'绑定的IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelAdminInfo', @level2type=N'COLUMN',@level2name=N'BandIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'冻结标识 0:启用，1:冻结' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelAdminInfo', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上次登录IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelAdminInfo', @level2type=N'COLUMN',@level2name=N'PreLoginIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上次登录时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelAdminInfo', @level2type=N'COLUMN',@level2name=N'PreLoginDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后登陆IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelAdminInfo', @level2type=N'COLUMN',@level2name=N'LastLoginIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后登陆时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelAdminInfo', @level2type=N'COLUMN',@level2name=N'LastLoginDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'登陆次数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelAdminInfo', @level2type=N'COLUMN',@level2name=N'LoginTimes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelAdminInfo', @level2type=N'COLUMN',@level2name=N'CollectIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ChannelAdminInfo', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日志ID，自增主键' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogAccount', @level2type=N'COLUMN',@level2name=N'LogID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日志内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogAccount', @level2type=N'COLUMN',@level2name=N'LogContent'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日志类型 暂未使用 预留字段' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogAccount', @level2type=N'COLUMN',@level2name=N'LogType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日志级别 1:一般 2:重要' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogAccount', @level2type=N'COLUMN',@level2name=N'LogLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理员ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogAccount', @level2type=N'COLUMN',@level2name=N'ManagerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作管理员' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogAccount', @level2type=N'COLUMN',@level2name=N'ManagerName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'请求地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogAccount', @level2type=N'COLUMN',@level2name=N'RequestUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'账户ID列表，多个用户用英文的逗号隔开' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogAccount', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'账号列表，冗余字段，多个用户用英文的逗号隔开' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogAccount', @level2type=N'COLUMN',@level2name=N'Accounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'状态码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogAccount', @level2type=N'COLUMN',@level2name=N'StatusCode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'状态描述' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogAccount', @level2type=N'COLUMN',@level2name=N'StatusDescribe'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'花费时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogAccount', @level2type=N'COLUMN',@level2name=N'CostTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作IP 兼容IP6' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogAccount', @level2type=N'COLUMN',@level2name=N'OperateIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'浏览器' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogAccount', @level2type=N'COLUMN',@level2name=N'Browser'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogAccount', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'系统错误日志ID，自增主键' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogException', @level2type=N'COLUMN',@level2name=N'LogID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日志类型 1：后台程序 2：前台程序' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogException', @level2type=N'COLUMN',@level2name=N'RequestUrl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'浏览器' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogException', @level2type=N'COLUMN',@level2name=N'Browser'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'请求IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogException', @level2type=N'COLUMN',@level2name=N'RequstIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'插入时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogException', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日志ID，自增主键' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogManage', @level2type=N'COLUMN',@level2name=N'LogID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日志内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogManage', @level2type=N'COLUMN',@level2name=N'LogContent'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日志类型 1:管理日志 2:安全日志 6:管理登录日志' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogManage', @level2type=N'COLUMN',@level2name=N'LogType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日志级别 1:一般 2:重要' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogManage', @level2type=N'COLUMN',@level2name=N'LogLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理员ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogManage', @level2type=N'COLUMN',@level2name=N'ManagerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作管理员' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogManage', @level2type=N'COLUMN',@level2name=N'ManagerName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'状态码' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogManage', @level2type=N'COLUMN',@level2name=N'StatusCode'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'状态描述' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogManage', @level2type=N'COLUMN',@level2name=N'StatusDescribe'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'花费时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogManage', @level2type=N'COLUMN',@level2name=N'CostTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作IP 兼容IP6' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogManage', @level2type=N'COLUMN',@level2name=N'OperateIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'浏览器' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogManage', @level2type=N'COLUMN',@level2name=N'Browser'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogManage', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'日志内容' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogService', @level2type=N'COLUMN',@level2name=N'LogContent'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogService', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'用户账号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogService', @level2type=N'COLUMN',@level2name=N'Accounts'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'站点ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogService', @level2type=N'COLUMN',@level2name=N'SiteID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理员ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogService', @level2type=N'COLUMN',@level2name=N'ManagerID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理员名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogService', @level2type=N'COLUMN',@level2name=N'ManagerName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作IP' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogService', @level2type=N'COLUMN',@level2name=N'OperateIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'LogService', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'模块标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ModuleInfo', @level2type=N'COLUMN',@level2name=N'ModuleID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'上级模块标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ModuleInfo', @level2type=N'COLUMN',@level2name=N'ParentID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'模块名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ModuleInfo', @level2type=N'COLUMN',@level2name=N'ModuleName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'模块链接地址' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ModuleInfo', @level2type=N'COLUMN',@level2name=N'ModuleLink'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'是否菜单 0:不是 ; 1:是' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ModuleInfo', @level2type=N'COLUMN',@level2name=N'IsMenu'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'排序数值' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ModuleInfo', @level2type=N'COLUMN',@level2name=N'OrderNo'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'是否启用 0: 启用 ; 1:禁用' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ModuleInfo', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'模块说明' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ModuleInfo', @level2type=N'COLUMN',@level2name=N'Description'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'模块标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ModulePermission', @level2type=N'COLUMN',@level2name=N'ModuleID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'权限ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ModulePermission', @level2type=N'COLUMN',@level2name=N'PermissionValue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'权限名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ModulePermission', @level2type=N'COLUMN',@level2name=N'PermissionName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'是否禁止 0:启动;1禁止' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ModulePermission', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'角色标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RoleInfo', @level2type=N'COLUMN',@level2name=N'RoleID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'角色名称' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RoleInfo', @level2type=N'COLUMN',@level2name=N'RoleName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'头像ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RoleInfo', @level2type=N'COLUMN',@level2name=N'Avatar'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'角色禁用状态 0:启用 1:禁用' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RoleInfo', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后修改时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RoleInfo', @level2type=N'COLUMN',@level2name=N'ModifyDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'创建时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RoleInfo', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'角色描述' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RoleInfo', @level2type=N'COLUMN',@level2name=N'Description'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'角色标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RolePermission', @level2type=N'COLUMN',@level2name=N'RoleID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'模块标识' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RolePermission', @level2type=N'COLUMN',@level2name=N'ModuleID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'管理权限' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RolePermission', @level2type=N'COLUMN',@level2name=N'ManagerPermission'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'操作权限' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'RolePermission', @level2type=N'COLUMN',@level2name=N'OperationPermission'
GO
