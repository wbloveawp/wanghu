#include "Stdafx.h"
#include <Afxdllx.h>
#include "QueryServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
static AFX_EXTENSION_MODULE	QueryServiceDLL={NULL,NULL};

//全局函数
extern "C" INT APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(QueryServiceDLL,hInstance)) return 0;
		new CDynLinkLibrary(QueryServiceDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(QueryServiceDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
