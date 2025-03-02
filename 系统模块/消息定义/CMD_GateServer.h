#ifndef CMD_GATE_HEAD_FILE
#define CMD_GATE_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//命令定义

//网关服务
#define MDM_GG_GATE_SERVICE				1								//网关服务

//链接服务
#define SUB_GG_C_LINK_SERVICE			1								//链接服务
#define SUB_GG_C_DISLINK_SERVICE		2								//断开服务

//链接结果
#define SUB_GG_S_LINK_SUCCESS			100								//链接成功
#define SUB_GG_S_LINK_FAILURE			101								//链接失败
#define SUB_GG_S_LINK_SHUTDOWN			102								//链接关闭

//////////////////////////////////////////////////////////////////////////////////
//扩展定义

//服务哈希
struct tagServiceHash
{
	DWORD							dwHashKeyID;						//散列标识
	DWORD							dwHashParam;						//散列参数	
	BYTE							cbExtendData[8];					//扩展数据
};

//房间服务扩展
struct tagLinkExtend_Server
{
	WORD							wKindID;							//类型标识
	WORD							wServerLevel;						//房间级别
	WORD							wServerType;						//房间类别
};

//服务路由扩展
struct tagLinkExtend_Route
{
	DWORD							dwRouteKeyID;						//路由标识	
};

//////////////////////////////////////////////////////////////////////////////////
//链接服务
struct CMD_GG_C_LinkService
{
	//服务信息
	WORD						    wServiceID;							//服务标识
	WORD							wServiceModule;						//服务模块	

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonToken[LEN_LOGON_TOKEN];		//登录令牌

	//扩展信息
	BYTE							cbLinkExtend[LEN_LINK_EXTEND];		//链接扩展
};

//断开服务
struct CMD_GG_C_DislinkService
{
	//服务信息
	WORD							wServiceModule;						//服务模块	
};

//链接成功
struct CMD_GG_S_LinkSuccess
{
	//服务信息
	WORD						    wServiceID;							//服务标识
	WORD							wServiceModule;						//服务模块
};

//链接失败
struct CMD_GG_S_LinkFailure
{
	//服务信息
	WORD						    wServiceID;							//服务标识
	WORD							wServiceModule;						//服务模块

	//错误信息
	LONG							lErrorCode;							//错误代码
	TCHAR							szErrorDescribe[128];				//错误描述
};

//链接关闭
struct CMD_GG_S_LinkShutdown
{
	//服务信息
	WORD						    wServiceID;							//服务标识
	WORD							wServiceModule;						//服务模块
};

////////////////////////////////////////////////////////////////////////////////////
#pragma pack()

#endif