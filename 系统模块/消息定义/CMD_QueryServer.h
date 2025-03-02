#ifndef CMD_SYNC_SERVER_HEAD_FILE
#define CMD_SYNC_SERVER_HEAD_FILE

#pragma pack(1)


//////////////////////////////////////////////////////////////////////////
//命令定义

//消息命令
#define MDM_AQ_QUERY_SERVICE			10									//查询命令

//////////////////////////////////////////////////////////////////////////

//请求命令
#define SUB_AQ_C_SYNC_USERINFO			10									//同步信息
#define SUB_AQ_C_QUERY_USERINFO			11									//查询信息
#define SUB_AQ_C_QUERY_BLACKLIST		12									//查询黑名单
#define SUB_AQ_C_QUERY_FRIENDLIST		13									//好友列表
#define SUB_AQ_C_QUERY_FRIENDINFO		14									//好友信息
#define SUB_AQ_C_QUERY_FRIENDEXTRAS		15									//附加信息

//请求命令
#define SUB_AQ_C_BUILD_ACCESSTOKEN		20									//生成令牌

//应答命令
#define SUB_AQ_S_USERINFO_LIST			100									//信息列表
#define SUB_AQ_S_FRIENDINFO_LIST		101									//好友列表
#define SUB_AQ_S_FRIENDEXTRAS_LIST		102									//附加列表

//应答命令
#define SUB_AQ_S_SYNC_USERINFO_RESULT	110									//同步结果
#define SUB_AQ_S_QUERY_USERINFO_RESULT	111									//查询结果
#define SUB_AQ_S_QUERY_BLACKLIST_RESULT 112									//查询结果
#define SUB_AQ_S_QUERY_FRIENDLIST_RESULT 113								//查询结果
#define SUB_AQ_S_QUERY_FRIENDINFO_RESULT 114								//查询结果
#define SUB_AQ_S_QUERY_FRIENDEXTRAS_RESULT 115								//查询结果

//应答命令
#define SUB_AQ_S_BUILD_ACCESSTOKEN_RESULT 120									//令牌结果	

/////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif