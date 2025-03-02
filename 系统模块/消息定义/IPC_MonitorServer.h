#ifndef IPC_MONITOR_SERVER_HEAD_FILE
#define IPC_MONITOR_SERVER_HEAD_FILE


//////////////////////////////////////////////////////////////////////////
//类名定义
#define MONITOR_CLASS_NAME			 TEXT("WND_MONITOR_#65478")

//////////////////////////////////////////////////////////////////////////
//监控消息

#define IPC_CMD_MS_MONITOR			10									//监控消息

#define IPC_SUB_MS_PROCESS_START	1									//进程启动
#define IPC_SUB_MS_PROCESS_CLOSE	2									//进程关闭
#define IPC_SUB_MS_SERVICE_READY	3									//服务准备
#define IPC_SUB_MS_SERVICE_STATUS	4									//服务状态
#define IPC_SUB_MS_DATABASE_EXCEPTION 5									//数据库异常

#define IPC_SUB_MS_STOP_SERVICE		100									//停止服务
#define IPC_SUB_MS_START_SERVICE	101									//启动服务
#define IPC_SUB_MS_CLOSE_PROCESS	102									//关闭进程

//////////////////////////////////////////////////////////////////////////
//进程启动
struct CMD_MS_ProcessStart
{
	DWORD							dwProcessID;						//进程标识	
};

//进程关闭
struct CMD_MS_ProcessClose
{
	DWORD							dwProcessID;						//进程标识	
};

//服务准备
struct CMD_MS_ServiceReady
{
	DWORD							dwProcessID;						//进程标识
	WORD							wServiceID;							//服务标识
	WORD							wServiceModule;						//服务模块
	WORD							wServiceStatus;						//服务状态
	TCHAR							szServiceName[32];					//服务名称
};

//服务状态
struct CMD_MS_ServiceStatus
{
	DWORD							dwProcessID;						//进程标识
	WORD							wServiceStatus;						//服务状态
};

//数据库异常
struct CMD_MS_DataBaseException
{
	DWORD							dwProcessID;						//进程标识
	TCHAR							szExceptionInfo[256];				//异常信息
};

//////////////////////////////////////////////////////////////////////////

#endif