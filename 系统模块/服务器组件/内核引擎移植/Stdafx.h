#pragma once

#if _MSC_VER>=1600
#define _CRT_SECURE_NO_WARNINGS
#else  
#endif 
//////////////////////////////////////////////////////////////////////////

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#include "targetver.h"


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

#define _AFX_ALL_WARNINGS

#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxDisp.h>
#include <AfxDtctl.h>

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <AfxCmn.h>
#endif

//////////////////////////////////////////////////////////////////////////////////

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../Á´½Ó¿â/Ansi/ServiceCore.lib")
#else
#pragma comment (lib,"../../Á´½Ó¿â/Unicode/ServiceCore.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../Á´½Ó¿â/Ansi/ServiceCoreD.lib")
#else
#pragma comment (lib,"../../Á´½Ó¿â/Unicode/ServiceCoreD.lib")
#endif
#endif

#pragma comment (lib,"Dbghelp.lib")


//////////////////////////www.foxpower.com////////////////////////////QQ398472227
