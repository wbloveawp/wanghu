#pragma once


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

//////////////////////////////////////////////////////////////////////////
//组件头文件

#include "..\..\服务器组件\游戏服务\GameServiceHead.h"
#include "..\..\服务器组件\配置服务\ConfigServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//链接代码

#ifndef _DEBUG
		#pragma comment (lib,"../../链接库/CommonService.lib")
		#pragma comment (lib,"../../链接库/GameService.lib")
		#pragma comment (lib,"../../链接库/KernelEngine.lib")
		#pragma comment (lib,"../../链接库/ConfigService.lib")
#else
		#pragma comment (lib,"../../链接库/CommonServiceD.lib")
		#pragma comment (lib,"../../链接库/GameServiceD.lib")
		#pragma comment (lib,"../../链接库/KernelEngineD.lib")
		#pragma comment (lib,"../../链接库/ConfigServiceD.lib")
#endif

//////////////////////////////////////////////////////////////////////////



