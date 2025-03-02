#ifndef FRAME_RESOURCE_HEAD_FILE
#define FRAME_RESOURCE_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//颜色定义
#define COLOR_CHAT_BK				RGB(229,249,255)					//聊天背景
#define COLOR_SERVER_TX				RGB(10,10,10)						//列表文字
#define COLOR_SERVER_BK				RGB(229,249,255)					//列表底色

//颜色定义
#define COLOR_SELECT_TX				RGB(0,0,0)							//字体颜色
#define COLOR_SELECT_BK				RGB(212,208,200)					//背景颜色
#define COLOR_NORMAL_TX				RGB(0,0,0)							//字体颜色
#define COLOR_NORMAL_BK				RGB(229,249,255)					//背景颜色

//颜色定义
#define COLOR_MYSELF_TX				RGB(29,56,117)						//字体颜色
#define COLOR_MYSELF_BK				RGB(199,229,245)					//背景颜色
#define COLOR_MASTER_TX				RGB(95,106,17)						//字体颜色
#define COLOR_MASTER_BK				RGB(225,237,173)					//背景颜色
#define COLOR_MEMBER_TX				RGB(114,45,3)						//字体颜色
#define COLOR_MEMBER_BK				RGB(251,236,166)					//背景颜色

//////////////////////////////////////////////////////////////////////////

//框架资源
class CFrameResource : public IFrameResource
{
	//函数定义
public:
	//构造函数
	CFrameResource();
	//析构函数
	virtual ~CFrameResource();

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
	//游戏控制
	virtual VOID __cdecl GetGameBarImage(tagGameBarImage & ResourceInfo);
	//游戏广告
	virtual VOID __cdecl GetGameBillImage(tagGameBillImage & ResourceInfo);
	//游戏聊天
	virtual VOID __cdecl GetGameChatImage(tagGameChatImage & ResourceInfo);
	//游戏框架
	virtual VOID __cdecl GetGameFrameImage(tagGameFrameImage & ResourceInfo);
	//游戏用户
	virtual VOID __cdecl GetGameUserInfoImage(tagGameUserInfoImage & ResourceInfo);
	//游戏列表
	virtual VOID __cdecl GetGameUserListImage(tagGameUserListImage & ResourceInfo);
	//喇叭资源
	virtual VOID __cdecl GetBugleImage(tagBugleImage &ResourceInfo);
};

//////////////////////////////////////////////////////////////////////////

#endif