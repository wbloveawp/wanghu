#ifndef CONFIG_SERVICE_HEAD_FILE
#define CONFIG_SERVICE_HEAD_FILE

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
#ifndef CONFIG_SERVICE_CLASS
#ifdef CONFIG_SERVICE_DLL
#define CONFIG_SERVICE_CLASS _declspec(dllexport)
#else
#define CONFIG_SERVICE_CLASS _declspec(dllimport)
#endif
#endif

//模块定义
#ifndef _DEBUG
#define CONFIG_SERVICE_DLL_NAME	TEXT("ConfigService.dll")		//组件名字
#else
#define CONFIG_SERVICE_DLL_NAME	TEXT("ConfigServiceD.dll")		//组件名字
#endif

//////////////////////////////////////////////////////////////////////////////////
//接口说明


//////////////////////////////////////////////////////////////////////////////////

//包含文件
#ifndef CONFIG_SERVICE_DLL
	#include "GameGoodsManager.h"
	#include "GameLevelManager.h"
	#include "GlobalOptionManager.h"
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif