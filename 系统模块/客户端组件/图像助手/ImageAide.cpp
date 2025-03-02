#include "Stdafx.h"

//MFC �ļ�
#include <Afxdllx.h>
#include "ImageAideHead.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
static ULONG_PTR g_lGdiPlusToken=0L;
static AFX_EXTENSION_MODULE ImageAideDLL={NULL,NULL};

//////////////////////////////////////////////////////////////////////////

//DLL ����������
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
