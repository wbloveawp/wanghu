#pragma once

//////////////////////////////////////////////////////////////////////////////////
//MFC 定义


#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN         
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      

#define _AFX_ALL_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// MFC头文件
#include <afxwin.h>
#include <afxext.h>
#include <afxdisp.h>
#include <StrSafe.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>          
#endif

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>
#endif 

#include <afxcontrolbars.h>
//////////////////////////////////////////////////////////////////////////////////

//版本定义
#include "TargetVer.h"
//MFC 文件
#include <Nb30.h>
#include <AfxInet.h>

#include <map>
#include <string>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <unordered_map>


//////////////////////////////////////////////////////////////////////////
//包含文件

#include "..\..\全局定义\Macro.h"
#include "..\..\全局定义\Module.h"

//组件定义

#include "..\..\..\第三方库\sqlite\sqlite3.h"

//平台文件
#include "..\..\公共组件\公共服务\CommonServiceHead.h"
#include "..\..\客户端组件\网络服务\NetworkServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//链接代码

#ifndef _DEBUG
#pragma comment (lib,"../../链接库/CommonService.lib")
#pragma comment (lib,"../../链接库/NetworkService.lib")
#pragma comment (lib,"../../../第三方库/sqlite/sqlite3.lib")
#else
#pragma comment (lib,"../../链接库/CommonServiceD.lib")
#pragma comment (lib,"../../链接库/NetworkServiceD.lib")
#pragma comment (lib,"../../../第三方库/sqlite/sqlite3D.lib")
#endif

//////////////////////////////////////////////////////////////////////////////////

#pragma comment (lib,"Winmm.lib")


//////////////////////////////////////////////////////////////////////////
