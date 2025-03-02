#include "Stdafx.h"
#include <Afxdllx.h>
#include "CenterServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
static AFX_EXTENSION_MODULE	CenterServiceDLL={NULL,NULL};

//全局函数
extern "C" INT APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(CenterServiceDLL,hInstance)) return 0;
		new CDynLinkLibrary(CenterServiceDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(CenterServiceDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
