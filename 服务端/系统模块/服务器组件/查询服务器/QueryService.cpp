#include "Stdafx.h"
#include <Afxdllx.h>
#include "QueryServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
static AFX_EXTENSION_MODULE	QueryServiceDLL={NULL,NULL};

//ȫ�ֺ���
extern "C" INT APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(QueryServiceDLL,hInstance)) return 0;
		new CDynLinkLibrary(QueryServiceDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(QueryServiceDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
