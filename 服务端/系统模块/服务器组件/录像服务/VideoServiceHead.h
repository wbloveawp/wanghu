#ifndef VIDEO_SERVICE_HEAD_FILE
#define VIDEO_SERVICE_HEAD_FILE

/////////////////////////////////////////////////////////////////////////////////
//MFC文件
#include <afxtempl.h>

//平台定义
#include "..\..\全局定义\Platform.h"

//平台文件
#include "..\..\公共组件\公共服务\CommonServiceHead.h"
#include "..\..\服务器组件\游戏服务\GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//导出定义

//导出定义
#ifndef VIDEO_SERVICE_CLASS
	#ifdef VIDEO_SERVICE_DLL
		#define VIDEO_SERVICE_CLASS _declspec(dllexport)
	#else
		#define VIDEO_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define VIDEO_SERVICE_DLL_NAME	TEXT("VideoService.dll")			//组件名字
#else
	#define VIDEO_SERVICE_DLL_NAME	TEXT("VideoServiceD.dll")			//组件名字
#endif


//录像参数
struct tagGameVideoParameter
{
	//配置变量
	tagGameServiceAttrib *			pGameServiceAttrib;					//桌子属性
	tagGameServiceOption *			pGameServiceOption;					//配置参数
};

//////////////////////////////////////////////////////////////////////////////////

//接口声明
interface IGameVideoItem;
interface IGameVideoManager;
interface VER_IGameVideoItemSink;

//////////////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_IGameVideoItem INTERFACE_VERSION(1,1)
static const GUID IID_IGameVideoItem={0x5d8433e7,0x6c57,0x462b,0x8c,0xfe,0x84,0xb7,0xbd,0x97,0x83,0xfa};
#else
#define VER_IGameVideoItem INTERFACE_VERSION(1,1)
static const GUID IID_IGameVideoItem={0xf1ff8ad,0x7ea6,0x4097,0x8c,0x89,0xa2,0xcd,0x1e,0x84,0x56,0x97};
#endif

//游戏录像
interface IGameVideoItem : public IUnknownEx
{
	//控制接口
public:
	//开始录像
	virtual bool StartVideo()=NULL;	
	//停止录像
	virtual bool ConcludeVideo()=NULL;	

	//数据接口
public:
	//压缩大小
	virtual DWORD GetCompressSize()=NULL;
	//压缩数据
	virtual LPBYTE GetCompressBuffer()=NULL;

	//信息接口
public:
	//桌子标识
	virtual WORD _cdecl GetTableID()=NULL;
	//录像编号
	virtual LPCTSTR GetVideoNumber()=NULL;
	
	//功能接口
public:	
	//插入用户
	virtual bool InsertVideoUser(IServerUserItem * pIServerUserItem)=NULL;
	//插入数据
	virtual bool InsertVideoData(WORD wChairID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;	
};

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IGameVideoManager INTERFACE_VERSION(1,1)
static const GUID IID_IGameVideoManager={0xe08ed997,0xbbcb,0x4f45,0x96,0x8c,0x69,0x39,0xf4,0xcb,0xec,0x5a};
#else
#define VER_IGameVideoManager INTERFACE_VERSION(1,1)
static const GUID IID_IGameVideoManager={0x559cc859,0xbc7f,0x4c6a,0xa5,0x4b,0xab,0x38,0xd0,0xf0,0x98,0xe3};
#endif

//录像管理
interface IGameVideoManager : public IUnknownEx
{
	//配置函数
public:	
	//设置接口
	virtual bool SetGameVideoItemSink(IUnknownEx * pIUnknownEx)=NULL;
	//配置组件
	virtual void InitVideoManager(tagGameVideoParameter & GameVideoParameter)=NULL;

	//管理函数
public:
	//激活录像
	virtual IGameVideoItem * ActiveVideoItem(WORD wTableID)=NULL;
	//删除录像
	virtual bool DeleteVideoItem(IGameVideoItem * pIGameVideoItem)=NULL;	
};


//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IGameVideoItemSink INTERFACE_VERSION(1,1)
static const GUID IID_IGameVideoItemSink={0x2584ef2a,0xe303,0x476f,0xbc,0xa7,0x9f,0xeb,0xcf,0x28,0xe9,0x54};
#else
#define VER_IGameVideoItemSink INTERFACE_VERSION(1,1)
static const GUID IID_IGameVideoItemSink={0x710114e,0xfda3,0x4c30,0x8a,0x4c,0x31,0x1,0x65,0x45,0x42,0x7f};
#endif

//录像回调
interface IGameVideoItemSink :public IUnknownEx
{
	//录像接口
public:
	//录像开始
	virtual void OnEventVideoStart(IGameVideoItem * pIGameVideoItem)=NULL;
	//录像停止
	virtual void OnEventVideoConclude(IGameVideoItem * pIGameVideoItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifndef VIDEO_SERVICE_DLL

	#define	VIDEO_MODULE_HELPER
	
	DECLARE_MODULE_HELPER(GameVideoManager, GameVideoManager, VIDEO_SERVICE_DLL_NAME,"CreateGameVideoManager")

#endif

//////////////////////////////////////////////////////////////////////////////////

#endif