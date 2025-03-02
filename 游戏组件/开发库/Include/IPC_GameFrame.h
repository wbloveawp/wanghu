#ifndef IPC_GAME_FRAME_HEAD_FILE
#define IPC_GAME_FRAME_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//网络消息

#define IPC_CMD_GF_SOCKET			1									//网络消息

#define IPC_SUB_GF_SOCKET_SEND		1									//网络发送
#define IPC_SUB_GF_SOCKET_RECV		2									//网络接收

//网络发送
struct IPC_GF_SocketSend
{
	CMD_Command						CommandInfo;						//命令信息
	BYTE							cbBuffer[SOCKET_PACKET];			//数据缓冲
};

//网络接收
struct IPC_GF_SocketRecv
{
	CMD_Command						CommandInfo;						//命令信息
	BYTE							cbBuffer[SOCKET_PACKET];			//数据缓冲
};

//////////////////////////////////////////////////////////////////////////
//控制消息

#define IPC_CMD_GF_CONTROL			2									//控制消息

#define IPC_SUB_GF_CLIENT_READY		1									//准备就绪
#define IPC_SUB_GF_CLIENT_CLOSE		2									//进程关闭

#define IPC_SUB_GF_CLOSE_PROCESS	100									//关闭进程
#define IPC_SUB_GF_ACTIVE_PROCESS	101									//激活进程

#define IPC_SUB_GF_BOSS_COME		200									//老板来了
#define IPC_SUB_GF_BOSS_LEFT		201									//老板走了

//////////////////////////////////////////////////////////////////////////
//配置消息

#define IPC_CMD_GF_CONFIG			3									//配置消息

#define IPC_SUB_GF_LEVEL_INFO		100									//等级信息
#define IPC_SUB_GF_MEMBER_INFO		101									//会员信息
#define IPC_SUB_GF_COLUMN_INFO		102									//列表信息
#define IPC_SUB_GF_SERVER_INFO		103									//房间信息
#define IPC_SUB_GF_CONFIG_FINISH	104									//配置完成
#define IPC_SUB_GF_MASTER_INFO		105									//会员信息
#define IPC_SUB_GF_SERVER_RULE		106									//房间规则
#define IPC_SUB_GF_USER_RIGHT		107									//玩家权限

//房间信息
struct IPC_GF_ServerInfo
{
	//用户信息
	WORD							wTableID;							//桌子号码
	WORD							wChairID;							//椅子号码
	DWORD							dwUserID;							//用户 I D

	//用户权限
	DWORD							dwUserRight;						//用户权限
	DWORD							dwMasterRight;						//管理权限

	//站点标识
	DWORD							dwUserStationID;					//用户站点
	DWORD							dwPlazaStationID;					//大厅站点
	TCHAR							szPlatformName[LEN_SERVER];			//房间名称

	//房间信息
	WORD							wKindID;							//房间类型
	WORD							wServerID;							//房间标识
	WORD							wServerType;						//房间类型
	DWORD							dwServerRule;						//房间规则
	TCHAR							szServerName[LEN_SERVER];			//房间名称
};

//等级信息
struct IPC_GF_LevelInfo
{
	BYTE							cbItemCount;						//列表数目
	tagLevelItem					LevelItem[64];						//列表描述
};

//列表配置
struct IPC_GF_ColumnInfo
{
	BYTE							cbColumnCount;						//列表数目
	tagColumnItem					ColumnItem[32];						//列表描述
};

//会员信息
struct IPC_GF_MemberInfo
{
	BYTE							cbMemberCount;						//会员数目
	tagMemberItem					MemberItem[22];						//等级描述
};

//管理员信息
struct IPC_GF_MasterInfo
{
	BYTE							cbMasterCount;						//会员数目
	tagMasterItem					MasterItem[4];						//等级描述
};

//房间规则
struct IPC_GF_ServerRule
{
	DWORD							dwServerRule;						//房间规则
};

//玩家权限
struct IPC_GF_UserRight
{
	DWORD							dwUserRight;						//玩家权限
};

//////////////////////////////////////////////////////////////////////////
//用户消息

#define IPC_CMD_GF_USER_INFO		4									//用户消息

#define IPC_SUB_GF_USER_ENTER		100									//用户进入
#define IPC_SUB_GF_USER_SCORE		101									//用户分数
#define IPC_SUB_GF_USER_STATUS		102									//用户状态
#define IPC_SUB_GF_USER_SEGMENT		103									//用户段位
#define IPC_SUB_GF_CUSTOM_FACE		103									//自定头像

//用户信息
struct IPC_GF_UserInfo
{
	BYTE							cbCompanion;						//用户关系
	tagUserInfoHead					UserInfoHead;						//用户信息
};

//用户积分
struct IPC_GF_UserScore
{
	DWORD							dwUserID;							//用户标识
	tagUserScore					UserScore;							//用户积分
};

//用户状态
struct IPC_GF_UserStatus
{
	DWORD							dwUserID;							//用户标识
	tagUserStatus					UserStatus;							//用户状态
};

//用户段位
struct IPC_GF_UserSegment
{
	DWORD							dwUserID;							//用户标识
	tagUserSegment					UserSegment;						//用户段位
};

//用户头像
struct IPC_GF_CustomFace
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwCustomID;							//自定标识
	tagCustomFaceInfo				CustomFaceInfo;						//自定头像
};

//////////////////////////////////////////////////////////////////////////

#endif