#pragma once

//////////////////////////////////////////////////////////////////////////////////
//MFC ����

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#define _AFX_ALL_WARNINGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

//////////////////////////////////////////////////////////////////////////////////

//�汾����
#include "TargetVer.h"

//MFC �ļ�
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxcMn.h>
#include <StrSafe.h>
#include <AfxDisp.h>
#include <AfxDtctl.h>
#include <AfxControlBars.h>

//////////////////////////////////////////////////////////////////////////

//�����ļ�
#include <iptypes.h>
#include <ntddndis.h>
#include <winioctl.h>
#include <ipifcons.h>
#include <iphlpapi.h>


//////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "Dbghelp.lib")

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
