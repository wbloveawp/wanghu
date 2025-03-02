#ifndef SKIN_RESOURCE_HEAD_FILE
#define SKIN_RESOURCE_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//资源管理类
class CSkinResource : public ISkinResource
{
	//函数定义
public:
	//构造函数
	CSkinResource();
	//析构函数
	virtual ~CSkinResource();

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

	//特殊资源
public:
	//创建字体
	virtual bool __cdecl GetFontResource(CFont & Font);
	//获取信息
	virtual bool __cdecl GetEncircleResource(tagSkinEncircleResource & SkinEncircleResource);

	//界面资源
public:
	//获取信息
	virtual bool __cdecl GetMenuResource(tagSkinMenuResource & ResourceInfo);
	//获取信息
	virtual bool __cdecl GetEditResource(tagSkinEditResource & ResourceInfo);
	//获取信息
	virtual bool __cdecl GetDialogResource(tagSkinDialogResource & ResourceInfo);
	//获取信息
	virtual bool __cdecl GetButtonResource(tagSkinButtonResource & ResourceInfo);
	//获取信息
	virtual bool __cdecl GetComboBoxResource(tagSkinComboBoxResource & ResourceInfo);
	//获取信息
	virtual bool __cdecl GetSplitterResource(tagSkinSplitterResource & ResourceInfo);
	//获取信息
	virtual bool __cdecl GetListCtrlResource(tagSkinListCtrlResource & ResourceInfo);
	//获取信息
	virtual bool __cdecl GetHyperLinkResource(tagSkinHyperLinkResource & ResourceInfo);
	//获取信息
	virtual bool __cdecl GetTabControlResource(tagSkinTabControlResource & ResourceInfo);
	//获取信息
	virtual	bool __cdecl GetScrollResource(tagSkinScrollResource &ResourceInfo);
};

//////////////////////////////////////////////////////////////////////////

#endif