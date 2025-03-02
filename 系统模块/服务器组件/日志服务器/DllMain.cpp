#include "Stdafx.h"
#include <Afxdllx.h>

//////////////////////////////////////////////////////////////////////////

//静态变量
static AFX_EXTENSION_MODULE	LoggerServiceDLL={NULL,NULL};

//全局函数
extern "C" INT APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(LoggerServiceDLL,hInstance)) return 0;
		new CDynLinkLibrary(LoggerServiceDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(LoggerServiceDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
