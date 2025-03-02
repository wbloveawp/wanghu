#ifndef CENTER_SERVICE_HEAD_HEAD_FILE
#define CENTER_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台定义
#include "..\..\全局定义\Platform.h"
#include "..\..\全局定义\RightDefine.h"

//消息定义
#include "..\..\消息定义\CMD_Commom.h"
#include "..\..\消息定义\CMD_CenterServer.h"

//平台文件
#include "..\..\公共组件\公共服务\CommonServiceHead.h"
#include "..\..\服务器组件\服务框架\ServiceFrameHead.h"
#include "..\..\服务器组件\内核引擎\KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////
//导出定义

//模块定义
#ifndef _DEBUG
	#define CENTER_SERVICE_DLL_NAME	TEXT("CenterService.dll")	//组件 DLL 名字
#else
	#define CENTER_SERVICE_DLL_NAME	TEXT("CenterServiceD.dll")	//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ICenterServiceUnits INTERFACE_VERSION(1,1)
	static const GUID IID_ICenterServiceUnits={0x807845d4,0x5a9c,0x4965,0xaf,0x99,0xd9,0x48,0xee,0x5f,0x7b,0x4c};
#else
	#define VER_ICenterServiceUnits INTERFACE_VERSION(1,1)
	static const GUID IID_ICenterServiceUnits={0x9fb08811,0xf9da,0x4f20,0xab,0xb0,0x27,0xfe,0x78,0x7c,0x0e,0xf1};
#endif

//服务单元
interface ICenterServiceUnits : public IUnknownEx
{
	//信息函数
public:
	//获取状态
	virtual WORD GetServiceStatus()=NULL;

	//服务控制
public:
	//启动服务
	virtual bool StartService()=NULL;
	//停止服务
	virtual bool ConcludeService()=NULL;
	//设置接口
	virtual bool SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//服务单元
DECLARE_MODULE_HELPER(CenterServiceUnits, CenterServiceUnits, CENTER_SERVICE_DLL_NAME,"CreateServiceUnits")

//////////////////////////////////////////////////////////////////////////

#endif