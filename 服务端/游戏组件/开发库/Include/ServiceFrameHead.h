#ifndef SERVICE_FRAME_HEAD_HEAD_FILE
#define SERVICE_FRAME_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台定义
#include "Platform.h"
#include "RightDefine.h"

//消息定义
#include "CMD_Commom.h"
#include "CMD_Validate.h"
#include "CMD_Correspond.h"
#include "CMD_GameServer.h"

//平台文件
#include "CommonServiceHead.h"
#include "KernelEngineHead.h"

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
//枚举定义

//服务状态
enum enServiceStatus
{
	ServiceStatus_Stop,				//停止状态
	ServiceStatus_Config,			//配置状态
	ServiceStatus_Service,			//服务状态
	ServiceStatus_WillStop			//即将关闭
};

//////////////////////////////////////////////////////////////////////////

//状态接口
interface IServiceUnitsSink
{
	//接口定义
public:
	//服务状态
	virtual VOID OnServiceUnitsStatus(enServiceStatus ServiceStatus)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//导出文件
#include "UIControlSink.h"
#include "RichEditTrace.h"
#include "ValidateService.h"
#include "ServiceParameter.h"

//////////////////////////////////////////////////////////////////////////

#endif