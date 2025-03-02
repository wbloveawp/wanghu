#include "Stdafx.h"
#include <Afxdllx.h>
#include "MessageServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
static AFX_EXTENSION_MODULE	MessageServiceDLL={NULL,NULL};

//全局函数
extern "C" INT APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(MessageServiceDLL,hInstance)) return 0;
		new CDynLinkLibrary(MessageServiceDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(MessageServiceDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
