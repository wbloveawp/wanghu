#include "Stdafx.h"
#include "AfxDLLx.h"
#include "PlazaControlsHead.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
static AFX_EXTENSION_MODULE PlazaControlDLL={NULL,NULL};

//////////////////////////////////////////////////////////////////////////

//DLL ������
extern "C" INT APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(PlazaControlDLL, hInstance)) return 0;
		new CDynLinkLibrary(PlazaControlDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(PlazaControlDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
