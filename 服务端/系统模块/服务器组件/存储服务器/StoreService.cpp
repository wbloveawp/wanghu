#include "Stdafx.h"
#include <Afxdllx.h>
#include "StoreServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
static AFX_EXTENSION_MODULE	StoreServiceDLL={NULL,NULL};

//ȫ�ֺ���
extern "C" INT APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(StoreServiceDLL,hInstance)) return 0;
		new CDynLinkLibrary(StoreServiceDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(StoreServiceDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
