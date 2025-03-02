#ifndef PLATFORM_RESOURCE_HEAD_FILE
#define PLATFORM_RESOURCE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "SkinResource.h"
#include "PlazaResource.h"
#include "FrameResource.h"

//////////////////////////////////////////////////////////////////////////

//平台资源
class CPlatformResource : public IPlatformResource
{
	//内核变量
protected:
	HINSTANCE						m_hInstance;						//资源句柄

	//资源对象
protected:
	CSkinResource					m_SkinResource;						//界面资源
	CPlazaResource					m_PlazaResource;					//大厅资源
	CFrameResource					m_FrameResource;					//框架资源

	//函数定义
public:
	//构造函数
	CPlatformResource();
	//析构函数
	virtual ~CPlatformResource();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { return; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//内核接口
public:
	//资源句柄
	virtual HINSTANCE __cdecl GetResInstance();
	//获取预览
	virtual VOID __cdecl GetSkinPreview(CImage & ImagePreview);

	//界面资源
public:
	//界面资源
	virtual VOID * __cdecl GetSkinResource(REFGUID Guid, DWORD dwQueryVer);
	//大厅资源
	virtual VOID * __cdecl GetPlazaResource(REFGUID Guid, DWORD dwQueryVer);
	//框架资源
	virtual VOID * __cdecl GetFrameResource(REFGUID Guid, DWORD dwQueryVer);
};

//////////////////////////////////////////////////////////////////////////

//全局变量
extern CPlatformResource g_PlatformResource;

//////////////////////////////////////////////////////////////////////////

#endif