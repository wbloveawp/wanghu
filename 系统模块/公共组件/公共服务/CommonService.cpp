#include "Stdafx.h"
#include "AfxDllx.h"
#include "CommonServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
static AFX_EXTENSION_MODULE CommonServiceDLL={NULL,NULL};

//////////////////////////////////////////////////////////////////////////

//��������
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
