#include "Stdafx.h"
#include <Afxdllx.h>
#include "ClubServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
static AFX_EXTENSION_MODULE	CorrespondServiceDLL={NULL,NULL};

//全局函数
extern "C" INT APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(CorrespondServiceDLL,hInstance)) return 0;
		new CDynLinkLibrary(CorrespondServiceDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(CorrespondServiceDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
