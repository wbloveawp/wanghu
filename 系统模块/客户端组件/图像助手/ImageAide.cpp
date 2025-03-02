#include "Stdafx.h"

//MFC 文件
#include <Afxdllx.h>
#include "ImageAideHead.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
static ULONG_PTR g_lGdiPlusToken=0L;
static AFX_EXTENSION_MODULE ImageAideDLL={NULL,NULL};

//////////////////////////////////////////////////////////////////////////

//DLL 导出主函数
extern "C" INT APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(ImageAideDLL,hInstance)) return 0;
		new CDynLinkLibrary(ImageAideDLL);

		GdiplusStartupInput StartupInput;
		GdiplusStartup(&g_lGdiPlusToken,&StartupInput,NULL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		GdiplusShutdown(g_lGdiPlusToken);
		AfxTermExtensionModule(ImageAideDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
