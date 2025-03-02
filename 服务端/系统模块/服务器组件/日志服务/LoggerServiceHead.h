#ifndef LOGGER_SERVICE_HEAD_FILE
#define LOGGER_SERVICE_HEAD_FILE

/////////////////////////////////////////////////////////////////////////////////
//MFC文件
#include <afxtempl.h>

//平台定义
#include "..\..\全局定义\Platform.h"

//平台文件
#include "..\..\公共组件\公共服务\CommonServiceHead.h"
#include "..\..\服务器组件\内核引擎\KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////////////
//导出定义

//导出定义
#ifndef LOGGER_SERVICE_CLASS
#ifdef LOGGER_SERVICE_DLL
#define LOGGER_SERVICE_CLASS _declspec(dllexport)
#else
#define LOGGER_SERVICE_CLASS _declspec(dllimport)
#endif
#endif

//模块定义
#ifndef _DEBUG
#define LOGGER_SERVICE_DLL_NAME	TEXT("LoggerService.dll")		//组件名字
#else
#define LOGGER_SERVICE_DLL_NAME	TEXT("LoggerServiceD.dll")		//组件名字
#endif

//////////////////////////////////////////////////////////////////////////////////
//接口说明


//////////////////////////////////////////////////////////////////////////////////

//包含文件
#ifndef LOGGER_SERVICE_DLL
	#include "Logger.h"
	#include "LoggerFile.h"
	#include "LoggerService.h"
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif