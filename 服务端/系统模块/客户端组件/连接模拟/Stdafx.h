#pragma once

//////////////////////////////////////////////////////////////////////////////////
#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

//版本定义
#include "TargetVer.h"
//#define _AFXDLL
#define _AFX_ALL_WARNINGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////


//MFC 文件
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <StrSafe.h>
#include <AfxDisp.h>

//MFC 文件
#include <Nb30.h>
#include <AfxInet.h>

//////////////////////////////////////////////////////////////////////////
//平台文件


//////////////////////////////////////////////////////////////////////////
//链接代码

#pragma comment (lib,"Winmm.lib")
#pragma comment( lib,"strsafe.lib" )


#ifndef _DEBUG
#pragma comment (lib,"../../链接库/CommonService.lib")
#pragma comment (lib,"../../链接库/NetworkService.lib")
#else
#pragma comment (lib,"../../链接库/CommonServiceD.lib")
#pragma comment (lib,"../../链接库/NetworkServiceD.lib")
#endif


//////////////////////////////////////////////////////////////////////////
