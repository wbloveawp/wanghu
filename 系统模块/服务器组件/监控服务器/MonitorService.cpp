#include "Stdafx.h"
#include <Afxdllx.h>

//////////////////////////////////////////////////////////////////////////

//静态变量
static AFX_EXTENSION_MODULE	MonitorServiceDLL={NULL,NULL};

//全局函数
extern "C" INT APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(MonitorServiceDLL,hInstance)) return 0;
		new CDynLinkLibrary(MonitorServiceDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(MonitorServiceDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
