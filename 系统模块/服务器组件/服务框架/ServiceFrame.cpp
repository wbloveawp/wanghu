#include "Stdafx.h"
#include <Afxdllx.h>
#include "ServiceFrameHead.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
static AFX_EXTENSION_MODULE	ServiceFrameDLL={NULL,NULL};

//ȫ�ֺ���
extern "C" INT APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(ServiceFrameDLL,hInstance)) return 0;
		new CDynLinkLibrary(ServiceFrameDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(ServiceFrameDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
