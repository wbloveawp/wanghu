#ifndef PROCESS_TRADE_HEAD_HEAD_FILE
#define PROCESS_TRADE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台文件
#include "Platform.h"           

//类头文件
#include "CommonServiceHead.h"                

//////////////////////////////////////////////////////////////////////////
//公共定义

//导出定义
#ifndef PROCESS_TRADE_CLASS
	#ifdef  PROCESS_TRADE_DLL
		#define PROCESS_TRADE_CLASS _declspec(dllexport)
	#else
		#define PROCESS_TRADE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define PROCESS_TRADE_DLL_NAME	TEXT("ProcessTrade.dll")			//组件 DLL 名字
#else
	#define PROCESS_TRADE_DLL_NAME	TEXT("ProcessTradeD.dll")			//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IProcessTrade INTERFACE_VERSION(1,1)
	static const GUID IID_IProcessTrade={0xc4e4c3b5,0x6204,0x4ec5,0x008c,0xf1,0xe8,0x30,0x6e,0xf1,0xad,0xf7};
#else
	#define VER_IProcessTrade INTERFACE_VERSION(1,1)
	static const GUID IID_IProcessTrade={0xeb98b2ba,0xfbb6,0x46bb,0x008f,0x95,0x78,0x45,0xb2,0x60,0x63,0x90};
#endif

//进程通讯
interface IProcessTrade : public IUnknownEx
{
	//配置接口
public:
	//获取句柄
	virtual HWND __cdecl GetTradeKernelWnd()=NULL;
	//配置组件
	virtual bool __cdecl SetProcessTradeSink(IUnknownEx * pIUnknownEx)=NULL;

	//连接管理
public:
	//关闭通讯
	virtual bool __cdecl CloseProcessTrade()=NULL;
	//创建连接
	virtual bool __cdecl CreateProcessTrade(LPCTSTR pszClassName)=NULL;

	//发送接口
public:
	//发送数据
	virtual bool __cdecl SendData(HWND hWndServer, WORD wMainCmdID, WORD wSubCmdID)=NULL;
	//发送数据
	virtual bool __cdecl SendData(HWND hWndServer, WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize)=NULL;
	//执行命令
	virtual bool __cdecl TransferCommand(HWND hWndServer, WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IProcessTradeSink INTERFACE_VERSION(1,1)
	static const GUID IID_IProcessTradeSink={0x29ff40d6,0xbe04,0x4ecc,0x00b0,0x06,0x86,0x91,0x18,0xa3,0xed,0xdd};
#else
	#define VER_IProcessTradeSink INTERFACE_VERSION(1,1)
	static const GUID IID_IProcessTradeSink={0x0505dc10,0x3e03,0x4d4f,0x0080,0xe3,0x7a,0xc3,0x88,0x9c,0x58,0xda};
#endif

//数据接口
interface IProcessTradeSink : public IUnknownEx
{
	//进程数据
	virtual bool __cdecl OnProcessTradeData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, HWND hWndSend)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IIPCRecvDataSink INTERFACE_VERSION(1,1)
	static const GUID IID_IIPCRecvDataSink={0x653995d6,0xa5f6,0x435a,0x009f,0xb1,0xb8,0xff,0xd4,0x1d,0xfd,0x52};
#else
	#define VER_IIPCRecvDataSink INTERFACE_VERSION(1,1)
	static const GUID IID_IIPCRecvDataSink={0x06711045,0x5e25,0x4ed6,0x0082,0xfc,0xc2,0xf9,0x19,0x7e,0x2f,0x0b};
#endif

//数据接口
interface IIPCRecvDataSink : public IUnknownEx
{
	//同步数据
	virtual bool __cdecl OnImmediateData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, HWND hWndSend)=NULL;
	//异步数据
	virtual bool __cdecl OnAsynchronismData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, HWND hWndSend)=NULL;
};

//////////////////////////////////////////////////////////////////////////
//组件辅助类

DECLARE_MODULE_HELPER(ProcessTrade, ProcessTrade, PROCESS_TRADE_DLL_NAME,"CreateProcessTrade")

//////////////////////////////////////////////////////////////////////////
//导出文件

#ifndef PROCESS_TRADE_DLL
	#include "IPCDataControl.h"
#endif

//////////////////////////////////////////////////////////////////////////

#endif