#include "Stdafx.h"
#include "AfxDLLx.h"
#include "ClientControlsHead.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
static AFX_EXTENSION_MODULE ClientControlsDLL={NULL,NULL};

//////////////////////////////////////////////////////////////////////////

//DLL ������
extern "C" INT APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(ClientControlsDLL, hInstance)) return 0;
		new CDynLinkLibrary(ClientControlsDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(ClientControlsDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
