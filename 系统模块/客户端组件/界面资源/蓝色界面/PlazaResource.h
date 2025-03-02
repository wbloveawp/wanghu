#ifndef PLAZA_RESOURCE_HEAD_FILE
#define PLAZA_RESOURCE_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//大厅资源
class CPlazaResource : public IPlazaResource
{
	//函数定义
public:
	//构造函数
	CPlazaResource();
	//析构函数
	virtual ~CPlazaResource();

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

	//资源信息
public:
	//大厅聊天
	virtual VOID __cdecl GetPlazaChatImage(tagPlazaChatImage & ResourceInfo);
	//大厅桌子
	virtual VOID __cdecl GetPlazaTableImage(tagPlazaTableImage & ResourceInfo);
	//大厅框架
	virtual VOID __cdecl GetPlazaFrameImage(tagPlazaFrameImage & ResourceInfo);
	//大厅列表
	virtual VOID __cdecl GetPlazaServerImage(tagPlazaServerImage & ResourceInfo);
	//大厅浏览
	virtual VOID __cdecl GetPlazaBrowserImage(tagPlazaBrowserImage & ResourceInfo);
	//大厅服务
	virtual VOID __cdecl GetPlazaServiceImage(tagPlazaServiceImage & ResourceInfo);
	//大厅私聊
	virtual VOID __cdecl GetPlazaWhisperImage(tagPlazaWhisperImage & ResourceInfo);
	//大厅用户
	virtual VOID __cdecl GetPlazaUserListImage(tagPlazaUserListImage & ResourceInfo);
	//大厅用户
	virtual VOID __cdecl GetPlazaUserInfoImage(tagPlazaUserInfoImage & ResourceInfo);
	//大厅设置
	virtual VOID __cdecl GetPlazaCollocateImage(tagPlazaCollocateImage & ResourceInfo);
	//安全设置
	virtual VOID __cdecl GetPlazaSafePromptImage(tagPlazaSafePromptImage & ResourceInfo);
	//喇叭资源
	virtual VOID __cdecl GetBugleImage(tagBugleImage &ResourceInfo);
	//下载资源
	virtual VOID __cdecl GetDownloadImage(tagDownLoadImage &ResourceInfo);
};

//////////////////////////////////////////////////////////////////////////

#endif