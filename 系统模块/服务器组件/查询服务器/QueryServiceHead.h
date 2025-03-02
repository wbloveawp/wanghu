#ifndef SYNC_SERVICE_HEAD_HEAD_FILE
#define SYNC_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台定义
#include "..\..\全局定义\Platform.h"
#include "..\..\全局定义\RightDefine.h"

//消息定义
#include "..\..\消息定义\CMD_Commom.h"
#include "..\..\消息定义\CMD_CenterServer.h"
#include "..\..\消息定义\CMD_QueryServer.h"

//平台文件
#include "..\..\公共组件\公共服务\CommonServiceHead.h"
#include "..\..\服务器组件\服务框架\ServiceFrameHead.h"
#include "..\..\服务器组件\内核引擎\KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IChatServiceUnits INTERFACE_VERSION(1,1)
	static const GUID IID_IChatServiceUnits={0x807845d4,0x5a9c,0x4965,0xaf,0x99,0xd9,0x48,0xee,0x5f,0x7b,0x4c};
#else
	#define VER_IChatServiceUnits INTERFACE_VERSION(1,1)
	static const GUID IID_IChatServiceUnits={0x9fb08811,0xf9da,0x4f20,0xab,0xb0,0x27,0xfe,0x78,0x7c,0x0e,0xf1};
#endif

//服务单元
interface IChatServiceUnits : public IUnknownEx
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

	//服务配置
public:
	virtual bool _cdecl SetChatItem(tagMessageItem& MessageItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#endif