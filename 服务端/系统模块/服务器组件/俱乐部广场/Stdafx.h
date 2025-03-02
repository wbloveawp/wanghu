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

#include "targetver.h"

//MFC 文件
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxcMn.h>
#include <StrSafe.h>
#include <AfxDisp.h>
#include <AfxDtctl.h>
#include <AfxControlBars.h>

//////////////////////////////////////////////////////////////////////////
//包含文件

//平台定义
#include "..\..\全局定义\Platform.h"
#include "..\..\消息定义\CMD_CenterServer.h"

//组件定义
#include "..\..\公共组件\公共服务\CommonServiceHead.h"
#include "..\..\服务器组件\服务框架\ServiceFrameHead.h"
#include "..\..\服务器组件\内核引擎\KernelEngineHead.h"
#include "..\..\服务器组件\模块管理\ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////
//链接代码

#ifndef _DEBUG
	#pragma comment (lib,"../../链接库/KernelEngine.lib")
	#pragma comment (lib,"../../链接库/ServiceFrame.lib")
	#pragma comment (lib,"../../链接库/CommonService.lib")	
	#pragma comment (lib,"../../链接库/ModuleManager.lib")
#else
	#pragma comment (lib,"../../链接库/KernelEngineD.lib")
	#pragma comment (lib,"../../链接库/ServiceFrameD.lib")
	#pragma comment (lib,"../../链接库/CommonServiceD.lib")	
	#pragma comment (lib,"../../链接库/ModuleManagerD.lib")
#endif

//////////////////////////////////////////////////////////////////////////
