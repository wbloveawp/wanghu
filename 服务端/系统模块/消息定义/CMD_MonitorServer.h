#ifndef CMD_MONITOR_SERVER_HEAD_FILE
#define CMD_MONITOR_SERVER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//注册服务
#define MDM_MS_REGISTER				10									//用户汇总

//////////////////////////////////////////////////////////////////////////////////

//服务注册
#define SUB_MS_C_REGISTER_WORLD		100									//注册世界服
#define SUB_MS_C_REGISTER_SERVER	101									//注册房间

//////////////////////////////////////////////////////////////////////////////////

//注册结果
#define SUB_MS_S_REGISTER_SUCCESS	300									//注册成功
#define SUB_MS_S_REGISTER_FAILURE	301									//注册失败

//////////////////////////////////////////////////////////////////////////////////

//注册世界服
struct CMD_MS_C_RegisterWorld
{
	WORD							wGameID;							//名称索引
	WORD							wKindID;							//名称索引
	WORD							wServiceID;							//服务索引
	WORD							wServicePort;						//服务端口
	TCHAR							szServerName[LEN_SERVER];			//服务名称
	TCHAR							szServerHost[LEN_DOMAIN];			//服务地址
};

//注册房间
struct CMD_MS_C_RegisterServer
{
	WORD							wGameID;							//名称索引
	WORD							wKindID;							//名称索引
	WORD							wServerID;							//房间索引
	WORD							wServerPort;						//房间端口
	TCHAR							szServerName[LEN_SERVER];			//房间名称
	TCHAR							szServerHost[LEN_DOMAIN];			//服务地址
};

//注册成功
struct CMD_MS_S_RegisterSuccess
{
	WORD							wServiceID;							//服务标识
};

//注册失败
struct CMD_MS_S_RegisterFailure
{
	DWORD							dwErrorCode;						//错误代码
	TCHAR							szDescribeString[128];				//错误消息
};

//////////////////////////////////////////////////////////////////////////////////

//服务命令
#define MDM_MS_MANAGE_SERVICE		11									//管理命令

//////////////////////////////////////////////////////////////////////////////////

#define SUB_MS_DISMISS_GAME			100									//解散游戏

//////////////////////////////////////////////////////////////////////////////////
//解散原因
#define DISMISS_REASON_STOCKLOW		1									//库存值低

//////////////////////////////////////////////////////////////////////////////////
//解散游戏
struct CMD_MS_S_DismissGame
{
	LONG							lReason;							//解散原因
	WORD							wTableID;							//桌子标识
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif