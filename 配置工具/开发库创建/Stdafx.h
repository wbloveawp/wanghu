#pragma once

//////////////////////////////////////////////////////////////////////////
//MFC 文件

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
#include <afxSock.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>          
#endif

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>
#endif 

#include <afxcontrolbars.h>

//////////////////////////////////////////////////////////////////////////////////

//平台文件
#include "..\..\系统模块\全局定义\Platform.h"
#include "..\..\系统模块\客户端组件\界面控件\SkinControlsHead.h"

//////////////////////////////////////////////////////////////////////////////////
//链接代码

#ifndef _DEBUG
	#pragma comment (lib,"../../系统模块/链接库/SkinControls.lib")
#else
	#pragma comment (lib,"../../系统模块/链接库/SkinControlsD.lib")
#endif

//////////////////////////////////////////////////////////////////////////////////
