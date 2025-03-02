#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//请求命令
#define DBR_GR_SYNC_USERINFO			10									//同步信息
#define DBR_GR_QUERY_USERINFO			11									//查询信息
#define DBR_GR_QUERY_BLACKLIST			12									//查询黑名单
#define DBR_GR_QUERY_FRIENDLIST			13									//好友列表
#define DBR_GR_QUERY_FRIENDINFO			14									//好友信息
#define DBR_GR_QUERY_FRIENDEXTRAS		15									//附加信息

//请求命令
#define DBR_GR_BUILD_ACCESSTOKEN		20									//请求令牌

//////////////////////////////////////////////////////////////////////////
//输出信息

//应答命令
#define DBO_GR_USERINFO_LIST			100									//信息列表
#define DBO_GR_FRIENDINFO_LIST			101									//好友列表
#define DBO_GR_FRIENDEXTRAS_LIST		102									//附加列表

//应答命令
#define DBO_GR_SYNC_USERINFO_RESULT		110									//同步结果
#define DBO_GR_QUERY_USERINFO_RESULT	111									//查询结果
#define DBO_GR_QUERY_BLACKLIST_RESULT	112									//查询结果
#define DBO_GR_QUERY_FRIENDLIST_RESULT	113									//查询结果
#define DBO_GR_QUERY_FRIENDINFO_RESULT  114									//查询结果
#define DBO_GR_QUERY_FRIENDEXTRAS_RESULT 115								//查询结果

//应答命令
#define DBO_GR_BUILD_ACCESSTOKEN_RESULT	120									//令牌结果

//////////////////////////////////////////////////////////////////////////////////

#endif