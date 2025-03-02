#ifndef TRACE_SERVICE_HEAD_FILE
#define TRACE_SERVICE_HEAD_FILE

#pragma once

#include "KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////

//追踪服务
class KERNEL_ENGINE_CLASS CTraceService
{
	//函数定义
protected:
	//构造函数
	CTraceService();

	//服务配置
public:
	//设置服务
	static bool SetTraceService(IUnknownEx * pIUnknownEx);
	//获取服务
	static VOID * GetTraceService(const IID & Guid, DWORD dwQueryVer);
	//获取服务
	static VOID * GetTraceServiceManager(const IID & Guid, DWORD dwQueryVer);

	//状态管理
public:
	//追踪状态
	static bool IsEnableTrace(enTraceLevel TraceLevel);
	//追踪控制
	static bool EnableTrace(enTraceLevel TraceLevel, bool bEnableTrace);

	//功能函数
public:
	//追踪信息
	static bool TraceString(LPCTSTR pszFormat, enTraceLevel TraceLevel, ...);
};

//////////////////////////////////////////////////////////////////////////

#endif