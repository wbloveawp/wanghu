#ifndef PROCESS_TRADE_HEAD_FILE
#define PROCESS_TRADE_HEAD_FILE

#pragma once

#include "ProcessTradeHead.h"

//////////////////////////////////////////////////////////////////////////

//进程通讯
class CProcessTrade : public CWnd, public IProcessTrade
{
	//组件变量
protected:
	CWHDataQueue					m_DataQueue;						//数据队列
	IProcessTradeSink *				m_pIProcessTradeSink;				//回调接口

	//函数定义
public:
	//构造函数
	CProcessTrade();
	//析构函数
	virtual ~CProcessTrade();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { delete this; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//配置接口
public:
	//获取句柄
	virtual HWND __cdecl GetTradeKernelWnd();
	//创建通讯
	virtual bool __cdecl SetProcessTradeSink(IUnknownEx * pIUnknownEx);

	//连接管理
public:
	//关闭通讯
	virtual bool __cdecl CloseProcessTrade();
	//创建连接
	virtual bool __cdecl CreateProcessTrade(LPCTSTR pszClassName);

	//发送接口
public:
	//发送数据
	virtual bool __cdecl SendData(HWND hWndServer, WORD wMainCmdID, WORD wSubCmdID);
	//发送数据
	virtual bool __cdecl SendData(HWND hWndServer, WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize);
	//执行命令
	virtual bool __cdecl TransferCommand(HWND hWndServer, WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize);

	//消息映射
protected:
	//异步数据
	LRESULT	OnAsynchronismData(WPARAM wParam, LPARAM lParam);
	//IPC 消息
	BOOL OnCopyData(CWnd * pWnd, COPYDATASTRUCT * pCopyDataStruct);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif