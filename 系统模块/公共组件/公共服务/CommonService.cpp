#include "Stdafx.h"
#include "AfxDllx.h"
#include "CommonServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
static AFX_EXTENSION_MODULE CommonServiceDLL={NULL,NULL};

//////////////////////////////////////////////////////////////////////////

//导出函数
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(CommonServiceDLL, hInstance)) return 0;
		new CDynLinkLibrary(CommonServiceDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(CommonServiceDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
