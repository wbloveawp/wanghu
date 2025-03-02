#ifndef PLAZA_CONTROL_HEAD_HEAD_FILE
#define PLAZA_CONTROL_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//平台文件

//平台文件
#include "..\..\全局定义\Platform.h"

//类头文件
#include "..\..\客户端组件\界面控件\SkinControlsHead.h"
#include "..\..\客户端组件\平台数据\PlatformDataHead.h"
#include "..\..\客户端组件\游戏控件\ClientControlsHead.h"

//组件文件
#include "..\..\客户端组件\界面资源\SkinResourceHead.h"
#include "..\..\客户端组件\服务组件\ClientServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//公共宏定义

//导出定义
#ifndef PLAZA_CONTROL_CLASS
	#ifdef  PLAZA_CONTROL_DLL
		#define PLAZA_CONTROL_CLASS _declspec(dllexport)
	#else
		#define PLAZA_CONTROL_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define PLAZA_CONTROL_DLL_NAME	TEXT("PlazaControl.dll")			//组件 DLL 名字
#else
	#define PLAZA_CONTROL_DLL_NAME	TEXT("PlazaControlD.dll")			//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IProcessManager INTERFACE_VERSION(1,1)
	static const GUID IID_IProcessManager={0x44958a10,0x6e7e,0x44ab,0xaa,0xb1,0x48,0xca,0x8a,0xc2,0xe4,0x04};
#else
	#define VER_IProcessManager INTERFACE_VERSION(1,1)
	static const GUID IID_IProcessManager={0xfbbd9807,0x60c9,0x4665,0xb8,0x8e,0xe1,0x10,0x72,0x0f,0xe0,0x82};
#endif

//进程管理
interface IProcessManager : public IUnknownEx
{
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IProcessManagerSink INTERFACE_VERSION(1,1)
	static const GUID IID_IProcessManagerSink={0xe2460d28,0xdf88,0x4572,0x96,0x13,0x54,0x6d,0x77,0xc6,0x5d,0xb0};
#else
	#define VER_IProcessManagerSink INTERFACE_VERSION(1,1)
	static const GUID IID_IProcessManagerSink={0xc503a1ee,0x2012,0x42f7,0xa1,0x5c,0x44,0xff,0x1c,0xa0,0x3f,0xdd};
#endif

//游戏管理
interface IProcessManagerSink : public IUnknownEx
{
	//进程错误
	virtual bool __cdecl OnGameProcessMistake()=NULL;
	//进程启动
	virtual bool __cdecl OnGameProcessCreate(bool bAlreadyExist)=NULL;
	//进程关闭
	virtual bool __cdecl OnGameProcessConclude(DWORD dwExitCode)=NULL;
	//进程数据
	virtual bool __cdecl OnProcessData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//导出文件
#ifndef PLAZA_CONTROL_DLL
	#include "ServerListManager.h"
#endif

//////////////////////////////////////////////////////////////////////////

#endif