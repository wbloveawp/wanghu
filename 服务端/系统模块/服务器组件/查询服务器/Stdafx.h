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
//包含文件

//平台定义
#include "..\..\全局定义\Platform.h"
#include "..\..\消息定义\CMD_Validate.h"
#include "..\..\消息定义\CMD_CenterServer.h"
#include "..\..\消息定义\CMD_GateServer.h"
#include "..\..\消息定义\CMD_QueryServer.h"

//消息协议
#include "..\..\协议定义\message\Struct.pb.h"
#include "..\..\协议定义\message\QueryServer.pb.h"

//组件定义
#include "..\..\公共组件\公共服务\CommonServiceHead.h"
#include "..\..\服务器组件\配置服务\ConfigServiceHead.h"
#include "..\..\服务器组件\服务框架\ServiceFrameHead.h"
#include "..\..\服务器组件\内核引擎\KernelEngineHead.h"
#include "..\..\服务器组件\模块管理\ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////
//链接代码

#ifndef _DEBUG
	#pragma comment (lib,"../../链接库/KernelEngine.lib")
	#pragma comment (lib,"../../链接库/ServiceFrame.lib")
	#pragma comment (lib,"../../链接库/ConfigService.lib")
	#pragma comment (lib,"../../链接库/CommonService.lib")
	#pragma comment (lib,"../../链接库/ModuleManager.lib")
#else
	#pragma comment (lib,"../../链接库/KernelEngineD.lib")
	#pragma comment (lib,"../../链接库/ServiceFrameD.lib")
	#pragma comment (lib,"../../链接库/ConfigServiceD.lib")
	#pragma comment (lib,"../../链接库/CommonServiceD.lib")
	#pragma comment (lib,"../../链接库/ModuleManagerD.lib")
#endif

//第三方库
#ifndef _DEBUG
	#pragma comment (lib,"../../../第三方库/redis/lib/release/redis.lib")	
	#pragma comment (lib,"../../../第三方库/redis/lib/release/hiredis.lib")	
	#pragma comment (lib,"../../../第三方库/protobuf/lib/release/libprotoc.lib")
	#pragma comment (lib,"../../../第三方库/protobuf/lib/release/libprotobuf.lib")
#else
	#pragma comment (lib,"../../../第三方库/redis/lib/debug/redis.lib")	
	#pragma comment (lib,"../../../第三方库/redis/lib/debug/hiredis.lib")	
	#pragma comment (lib,"../../../第三方库/protobuf/lib/debug/libprotoc.lib")
	#pragma comment (lib,"../../../第三方库/protobuf/lib/debug/libprotobuf.lib")
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
