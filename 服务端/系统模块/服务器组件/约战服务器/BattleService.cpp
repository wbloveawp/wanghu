#include "Stdafx.h"
#include <Afxdllx.h>
#include "BattleServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
static AFX_EXTENSION_MODULE	BattleServiceDLL={NULL,NULL};

//全局函数
extern "C" INT APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(BattleServiceDLL,hInstance)) return 0;
		new CDynLinkLibrary(BattleServiceDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(BattleServiceDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
