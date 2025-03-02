#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//系统命令
#define DBR_GP_LOAD_CONFIG			1									//加载配置	
#define DBR_GP_UNLOCK_PLAYER		3									//解锁玩家
#define DBR_GP_REQUEST_WEBPAGE		4									//请求网页
#define DBR_GP_UPDATE_GATE_LIST		8									//更新网关
#define DBR_GP_ONLINE_COUNT_INFO	9									//在线信息
#define DBR_GP_UPDATE_SERVER_LIST	10									//更新房间
#define DBR_GP_UPDATE_RANKING_LIST  11									//更新排行

//////////////////////////////////////////////////////////////////////////////////
//更新类型

#define UPDATE_KIND_MODIFY			1									//修改类型 
#define UPDATE_KIND_DELETE			2									//删除类型

//////////////////////////////////////////////////////////////////////////

//解锁玩家
struct DBR_GP_UnlockPlayer
{
	DWORD							dwUserID;							//用户标识
};

//更新网关
struct DBR_UpdateGateList
{
	CHAR							szGateList[8192];					//网关列表
};

//请求网页
struct DBR_GP_RequestWebPage
{
	CHAR							szWebUrl[128];						//网页链接	
	CHAR							szPostData[2048];					//投递数据
};

//上报房间
struct DBR_GP_UpdateServerList
{
	BYTE							cbUpdateKind;						//更新类型	
	WCHAR							szServerList[4096];					//房间列表
};

//////////////////////////////////////////////////////////////////////////
//系统命令
#define DBO_GP_CONFIG_FINISH		101									//配置完成
#define DBO_GP_UNLOCK_PLAYER		103									//解锁玩家
#define DBO_GP_UPDATA_RANKING_LIST	106									//更新榜单

//////////////////////////////////////////////////////////////////////////

//解锁玩家
struct DBO_GP_UnlockPlayer
{
	DWORD							dwUserID;							//用户标识
};

//////////////////////////////////////////////////////////////////////////

#endif