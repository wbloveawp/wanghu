#ifndef CLIENT_DEBUG_HEAD_FILE
#define CLIENT_DEBUG_HEAD_FILE
#pragma once

#include <afxtempl.h>

//////////////////////////////////////////////////////////////////////////////////////
// 
//平台头文件
#include "Platform.h"           
#include "ClientServiceHead.h"                 

//////////////////////////////////////////////////////////////////////////////////////

#define CreateInterface( Hinstance, InterfacePoint, InterfaceName, CallDllName, CallFunctionName )		\
	InterfacePoint = NULL;																					\
	Hinstance = LoadLibrary(_T(CallDllName));															\
	if ( Hinstance )																						\
{																										\
	typedef void * (*CREATE)(); 																		\
	CREATE pFunction = (CREATE)GetProcAddress( Hinstance, CallFunctionName ); 							\
	if ( pFunction )																					\
	{																									\
	InterfacePoint = static_cast< InterfaceName * >( pFunction() );									\
	}																									\
}																										



#define ReleaseInterface(Hinstance, InterfacePoint)	\
	if( InterfacePoint )									\
{														\
	InterfacePoint->Release();							\
	InterfacePoint = NULL;								\
}														\
	if( Hinstance )											\
{														\
	FreeLibrary(Hinstance);								\
	Hinstance = NULL;									\
}	

///////////////////////////////////////////////////////////////////////////////////////
// 命令定义
//请求命令
#define SUB_C_GET_SERVER_USER						100					//获取用户

//应答命令
#define SUB_S_SERVER_USER_LIST						1000				//用户列表

///////////////////////////////////////////////////////////////////////////////////////

//回调接口
interface IClientDebugCallback
{
	//发送数据
	virtual bool SendDebugData(WORD nMessageID, WORD wTableID, void * pData, WORD wDataSize) = NULL;	
};

///////////////////////////////////////////////////////////////////////////////////////
// 
// 客户端接口
interface IClientDebug
{
	//控制接口
public:
	//释放接口
	virtual void Release() = NULL;
	//创建函数
	virtual bool Create(CWnd * pParentWnd, IClientDebugCallback * pIClientDebugCallback) = NULL;
	//显示窗口
	virtual bool ShowWindow(bool bShow) = NULL;

	//消息事件
public:
	//消息函数
	virtual bool OnEventDebugMessage(WORD nMessageID, WORD wTableID, void * pData, WORD nSize) = NULL;

	//用户事件
public:
	//用户进入
	virtual VOID OnEventUserEnter(IClientUserItem * pIClientUserItem){};
	//用户离开
	virtual VOID OnEventUserLeave(IClientUserItem * pIClientUserItem){};
	//用户状态
	virtual VOID OnEventUserStatus(IClientUserItem *pIClientUserItem){};
};

//////////////////////////////////////////////////////////////////////////////////

#endif
