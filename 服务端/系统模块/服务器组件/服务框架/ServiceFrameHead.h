#ifndef SERVICE_FRAME_HEAD_HEAD_FILE
#define SERVICE_FRAME_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台定义
#include "..\..\全局定义\Platform.h"
#include "..\..\全局定义\RightDefine.h"

//消息定义
#include "..\..\消息定义\CMD_Commom.h"
#include "..\..\消息定义\CMD_Validate.h"
#include "..\..\消息定义\CMD_CenterServer.h"
#include "..\..\消息定义\CMD_GameServer.h"
#include "..\..\消息定义\IPC_MonitorServer.h"

//平台文件
#include "..\..\公共组件\公共服务\CommonServiceHead.h"
#include "..\..\公共组件\进程通讯\ProcessTradeHead.h"
#include "..\..\服务器组件\内核引擎\KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////
//导出定义

//导出定义
#ifndef SERVICE_FRAME_CLASS
	#ifdef SERVICE_FRAME_DLL
		#define SERVICE_FRAME_CLASS _declspec(dllexport)
	#else
		#define SERVICE_FRAME_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define SERVICE_FRAME_DLL_NAME	TEXT("ServiceFrame.dll")			//组件 DLL 名字
#else
	#define SERVICE_FRAME_DLL_NAME	TEXT("ServiceFrameD.dll")			//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////

//状态接口
interface IServiceUnitsSink
{
	//接口定义
public:
	//服务状态
	virtual VOID OnServiceUnitsStatus(WORD ServiceStatus)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//导出文件
#ifndef SERVICE_FRAME_DLL
#include "UIControlSink.h"
#include "MonitorClient.h"
#include "RichEditTrace.h"
#include "ServiceParameter.h"
#include "GlobalInfoItem.h"
#include "GlobalItemManager.h"
#endif
//////////////////////////////////////////////////////////////////////////

#endif