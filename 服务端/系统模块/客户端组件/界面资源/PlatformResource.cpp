#include "StdAfx.h"
#include "Resource.h"
#include "PlatformResource.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CPlatformResource::CPlatformResource()
{
	//内核变量
	m_hInstance=GetModuleHandle(SKIN_RESOURCE_DLL_NAME);

	return;
}

//析构函数
CPlatformResource::~CPlatformResource()
{
}

//接口查询
VOID * __cdecl CPlatformResource::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IPlatformResource,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IPlatformResource,Guid,dwQueryVer);
	return NULL;
}

//实例句柄
HINSTANCE __cdecl CPlatformResource::GetResInstance()
{
	return m_hInstance;
}

//获取预览
VOID __cdecl CPlatformResource::GetSkinPreview(CImage & ImagePreview)
{
	//加载资源
	ImagePreview.LoadFromResource(m_hInstance,IDB_SKIN_PREVIEW);

	return;
}

//界面资源
VOID * __cdecl CPlatformResource::GetSkinResource(REFGUID Guid, DWORD dwQueryVer)
{
	return m_SkinResource.QueryInterface(Guid,dwQueryVer);
}

//大厅资源
VOID * __cdecl CPlatformResource::GetPlazaResource(REFGUID Guid, DWORD dwQueryVer)
{
	return m_PlazaResource.QueryInterface(Guid,dwQueryVer);
}

//框架资源
VOID * __cdecl CPlatformResource::GetFrameResource(REFGUID Guid, DWORD dwQueryVer)
{
	return m_FrameResource.QueryInterface(Guid,dwQueryVer);
}

//////////////////////////////////////////////////////////////////////////

//资源变量
CPlatformResource g_PlatformResource;

//组件创建函数
extern "C" __declspec(dllexport) VOID * __cdecl CreatePlatformResource(const GUID & Guid, DWORD dwInterfaceVer)
{
	return g_PlatformResource.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////
