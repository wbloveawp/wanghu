#pragma once

//////////////////////////////////////////////////////////////////////////////////

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0400
#endif

#define _ATL_ATTRIBUTES
#define _AFX_ALL_WARNINGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

//////////////////////////////////////////////////////////////////////////////////

//MFC 文件
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <AfxDisp.h>
#include <StrSafe.h>
//////////////////////////////////////////////////////////////////////////////////

//编译环境
//#include "..\..\..\SDKCondition.h"
#include "..\..\..\..\系统模块\全局定义\Macro.h"
//////////////////////////////////////////////////////////////////////////////////
#ifdef SDK_CONDITION

//////////////////////////////////////////////////////////////////////////////////
//开发环境

//平台头文件
#include "..\..\..\开发库\Include\GameServiceHead.h"
//
//#ifndef _DEBUG
#pragma comment (lib,"../../../开发库/lib/CommonService.lib")
//#else
#pragma comment (lib,"../../../开发库/lib/CommonServiceD.lib")
//#endif

//////////////////////////////////////////////////////////////////////////////////

#else

//////////////////////////////////////////////////////////////////////////////////
//系统环境

//平台环境
#include "..\..\..\..\系统模块\服务器组件\游戏服务\GameServiceHead.h"
#ifndef _DEBUG

#pragma comment (lib,"../../../../系统模块/链接库/CommonService.lib")
#pragma comment (lib,"../../../../第三方库/sqlite/sqlite3.lib")
#else

#pragma comment (lib,"../../../../系统模块/链接库/CommonServiceD.lib")
#pragma comment (lib,"../../../../第三方库/sqlite/sqlite3D.lib")
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif
