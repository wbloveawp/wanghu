#ifndef BATTLE_SERVICE_HEAD_HEAD_FILE
#define BATTLE_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//MFC文件
#include <afxtempl.h>

//平台定义
#include "..\..\全局定义\Platform.h"

//消息定义
#include "..\..\消息定义\CMD_Commom.h"
#include "..\..\消息定义\CMD_CenterServer.h"
#include "..\..\消息定义\CMD_BattleServer.h"
#include "..\..\消息定义\CMD_GameServer.h"

//平台文件
#include "..\..\公共组件\公共服务\CommonServiceHead.h"
#include "..\..\服务器组件\游戏服务\GameServiceHead.h"
#include "..\..\服务器组件\内核引擎\KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////////////
//导出定义

//导出定义
#ifndef BATTLE_SERVICE_CLASS
	#ifdef  BATTLE_SERVICE_DLL
		#define BATTLE_SERVICE_CLASS _declspec(dllexport)
	#else
		#define BATTLE_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define BATTLE_SERVICE_DLL_NAME	TEXT("BattleService.dll")			//组件名字
#else
	#define BATTLE_SERVICE_DLL_NAME	TEXT("BattleServiceD.dll")			//组件名字
#endif

//////////////////////////////////////////////////////////////////////////////////

#ifndef BATTLE_SERVICE_DLL

	#define BATTLE_MODULE_HELPER
	
	DECLARE_MODULE_HELPER(BattleServiceManager, ExtendServiceManager, BATTLE_SERVICE_DLL_NAME,"CreateBattleServiceManager")

#endif

//////////////////////////////////////////////////////////////////////////////////

#endif