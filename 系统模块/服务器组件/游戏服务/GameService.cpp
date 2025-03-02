#include "Stdafx.h"
#include <Afxdllx.h>
#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
static AFX_EXTENSION_MODULE	GameServiceDLL={NULL,NULL};

//全局函数
extern "C" INT APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
{
		if (!AfxInitExtensionModule(GameServiceDLL,hInstance)) return 0;
		new CDynLinkLibrary(GameServiceDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
{
		AfxTermExtensionModule(GameServiceDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
