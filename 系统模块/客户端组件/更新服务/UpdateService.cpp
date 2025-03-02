#include "Stdafx.h"
#include "AfxDLLx.h"
#include "WinSock2.h"

static AFX_EXTENSION_MODULE UpdateServiceDLL={NULL,NULL};

//////////////////////////////////////////////////////////////////////////

//DLL 主函数
extern "C" INT APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(UpdateServiceDLL, hInstance)) return 0;
		new CDynLinkLibrary(UpdateServiceDLL);

		//初始化网络
		WSADATA WSAData;
		WORD wVersionRequested=MAKEWORD(1,1);
		if (WSAStartup(wVersionRequested,&WSAData)!=0) return 0;
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(UpdateServiceDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
