#include "StdAfx.h"
#include "Resource.h"
#include "PlatformResource.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CPlatformResource::CPlatformResource()
{
	//�ں˱���
	m_hInstance=GetModuleHandle(SKIN_RESOURCE_DLL_NAME);

	return;
}

//��������
CPlatformResource::~CPlatformResource()
{
}

//�ӿڲ�ѯ
VOID * __cdecl CPlatformResource::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IPlatformResource,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IPlatformResource,Guid,dwQueryVer);
	return NULL;
}

//ʵ�����
HINSTANCE __cdecl CPlatformResource::GetResInstance()
{
	return m_hInstance;
}

//��ȡԤ��
VOID __cdecl CPlatformResource::GetSkinPreview(CImage & ImagePreview)
{
	//������Դ
	ImagePreview.LoadFromResource(m_hInstance,IDB_SKIN_PREVIEW);

	return;
}

//������Դ
VOID * __cdecl CPlatformResource::GetSkinResource(REFGUID Guid, DWORD dwQueryVer)
{
	return m_SkinResource.QueryInterface(Guid,dwQueryVer);
}

//������Դ
VOID * __cdecl CPlatformResource::GetPlazaResource(REFGUID Guid, DWORD dwQueryVer)
{
	return m_PlazaResource.QueryInterface(Guid,dwQueryVer);
}

//�����Դ
VOID * __cdecl CPlatformResource::GetFrameResource(REFGUID Guid, DWORD dwQueryVer)
{
	return m_FrameResource.QueryInterface(Guid,dwQueryVer);
}

//////////////////////////////////////////////////////////////////////////

//��Դ����
CPlatformResource g_PlatformResource;

//�����������
extern "C" __declspec(dllexport) VOID * __cdecl CreatePlatformResource(const GUID & Guid, DWORD dwInterfaceVer)
{
	return g_PlatformResource.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////
