#pragma once

//////////////////////////////////////////////////////////////////////////////////
//MFC 定义

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#define _AFX_ALL_WARNINGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

//////////////////////////////////////////////////////////////////////////////////

//版本定义
#include "TargetVer.h"

//MFC 文件
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxcMn.h>
#include <StrSafe.h>
#include <AfxDisp.h>
#include <AfxDtctl.h>
#include <AfxControlBars.h>


//////////////////////////////////////////////////////////////////////////

//引入文件
#include "..\..\全局定义\Condition.h"
#include "..\..\全局定义\Platform.h"
#include "..\..\消息定义\CMD_Validate.h"
#include "..\..\消息定义\CMD_LogonServer.h"

//基础模块
#include "..\..\服务器组件\配置服务\ConfigServiceHead.h"
#include "..\..\服务器组件\模块管理\ModuleManagerHead.h"
#include "..\..\服务器组件\日志服务\LoggerServiceHead.h"
#include "..\..\服务器组件\监控服务\MonitorServiceHead.h"

//录像服务
#ifdef VIDEO_SERVICE_MODULE
	#include "..\..\服务器组件\录像服务\VideoServiceHead.h"
#endif

//比赛服务
#ifdef MATCH_SERVICE_MODULE
	#include "..\..\服务器组件\比赛服务\MatchServiceHead.h"
#endif

//约战服务
#ifdef BATTLE_SERVICE_MODULE
	#include "..\..\服务器组件\约战服务\BattleServiceHead.h"
#endif

//排位服务
#ifdef RANKING_SERVICE_MODULE
	#include "..\..\服务器组件\排位服务\RankingServiceHead.h"
#endif

//////////////////////////////////////////////////////////////////////////
//链接代码

#ifndef _DEBUG
	#pragma comment (lib,"../../链接库/KernelEngine.lib")	
	#pragma comment (lib,"../../链接库/CommonService.lib")
	#pragma comment (lib,"../../链接库/ConfigService.lib")
	#pragma comment (lib,"../../链接库/ServiceFrame.lib")
	#pragma comment (lib,"../../链接库/ProcessTrade.lib")
	#pragma comment (lib,"../../链接库/ModuleManager.lib")
	#pragma comment (lib,"../../链接库/MonitorService.lib")
	#pragma comment (lib,"../../链接库/LoggerService.lib")	
#else
	#pragma comment (lib,"../../链接库/KernelEngineD.lib")
	#pragma comment (lib,"../../链接库/ServiceFrameD.lib")
	#pragma comment (lib,"../../链接库/ProcessTradeD.lib")
	#pragma comment (lib,"../../链接库/ConfigServiceD.lib")
	#pragma comment (lib,"../../链接库/CommonServiceD.lib")
	#pragma comment (lib,"../../链接库/ModuleManagerD.lib")
	#pragma comment (lib,"../../链接库/MonitorServiceD.lib")
	#pragma comment (lib,"../../链接库/LoggerServiceD.lib")	
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#if defined _M_IX86
		#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#elif defined _M_X64
		#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#else
		#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#endif
#endif

//////////////////////////////////////////////////////////////////////////