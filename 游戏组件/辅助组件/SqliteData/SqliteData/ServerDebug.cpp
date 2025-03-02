// ServerDebug.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include <afxdllx.h>
#include "SqliteData.h"

static AFX_EXTENSION_MODULE ServerDebugDLL = { NULL, NULL };

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// ���ʹ�� lpReserved���뽫���Ƴ�
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("LuxuryFruitSqliteData.DLL ���ڳ�ʼ����\n");
		if (!AfxInitExtensionModule(ServerDebugDLL, hInstance)) return 0;
		new CDynLinkLibrary(ServerDebugDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("LuxuryFruitSqliteData.DLL ������ֹ��\n");
		AfxTermExtensionModule(ServerDebugDLL);
	}
	return 1;   // ȷ��
}

//����������
extern "C" __declspec(dllexport) void * __cdecl CreateSqliteData()
{
	//��������
	CSqliteDataItemSink* pServerDebug = NULL;
	try
	{
		pServerDebug = new CSqliteDataItemSink();
		if (pServerDebug == NULL) 
			throw TEXT("����ʧ��");

		return pServerDebug;
	}
	catch (...) {}

	//�������
	SafeDelete(pServerDebug);
	return NULL;
}