#include "Stdafx.h"
#include <Afxdllx.h>
#include "ProcessTradeHead.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
static AFX_EXTENSION_MODULE ProcessTradeDLL={NULL,NULL};

//////////////////////////////////////////////////////////////////////////

//DLL 导出主函数
extern "C" INT APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(ProcessTradeDLL,hInstance)) return 0;
		new CDynLinkLibrary(ProcessTradeDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(ProcessTradeDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
