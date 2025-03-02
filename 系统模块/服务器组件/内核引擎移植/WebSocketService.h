#pragma once

#include"KernelEngineHead.h"
#include "Cipher.h"

class CWebSocketServiceThread : public CWHThread
{
	//内核变量
protected:
	HWND							m_hWnd;								//窗口句柄
	SOCKET							m_hSocket;							//连接句柄
	BYTE				            m_TCPSocketStatus;					//网络状态

																		//组件变量
protected:
	CWHDataQueue					m_DataQueue;						//请求队列
	CCriticalSection				m_CriticalSection;					//队列锁定

																		//接收变量
protected:
	WORD							m_wRecvSize;						//接收长度
	BYTE							m_cbRecvBuf[1024];					//接收缓冲

	bool							m_bFlag;

																		//缓冲变量
protected:
	bool							m_bNeedBuffer;						//缓冲状态
	DWORD							m_dwBufferData;						//缓冲数据
	DWORD							m_dwBufferSize;						//缓冲大小
	LPBYTE							m_pcbDataBuffer;					//缓冲数据

																		//计数变量
protected:
	DWORD							m_dwSendTickCount;					//发送时间
	DWORD							m_dwRecvTickCount;					//接收时间

																		//函数定义
public:
	//构造函数
	CWebSocketServiceThread();
	//析构函数
	virtual ~CWebSocketServiceThread();

	//重载函数
public:
	//运行函数
	virtual bool OnEventThreadRun();
	//开始事件
	virtual bool OnEventThreadStrat();
	//停止事件
	virtual bool OnEventThreadConclude();
	//停止线程
	virtual bool ConcludeThread(DWORD dwWaitSeconds);

	//请求函数
public:
	//投递请求
	bool PostThreadRequest(WORD wIdentifier, VOID * const pBuffer, WORD wDataSize);

	//控制函数
private:
	//关闭连接
	VOID PerformCloseSocket(BYTE cbShutReason);
	//连接服务器
	DWORD PerformConnect(DWORD dwServerIP, WORD wPort);
	//发送函数
	DWORD PerformSendData(WORD wMainCmdID, WORD wSubCmdID);
	//发送函数
	DWORD PerformSendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//辅助函数
private:
	//发送数据
	DWORD SendBuffer(VOID * pBuffer, WORD wSendSize);
	//缓冲数据
	VOID AmortizeBuffer(VOID * pData, WORD wDataSize);


	//处理函数
private:
	//网络消息
	LRESULT OnSocketNotify(WPARAM wParam, LPARAM lParam);
	//请求消息
	LRESULT OnServiceRequest(WPARAM wParam, LPARAM lParam);

	//辅助函数
private:
	//网络读取
	LRESULT OnSocketNotifyRead(WPARAM wParam, LPARAM lParam);
	//网络发送
	LRESULT OnSocketNotifyWrite(WPARAM wParam, LPARAM lParam);
	//网络关闭
	LRESULT OnSocketNotifyClose(WPARAM wParam, LPARAM lParam);
	//网络连接
	LRESULT OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam);
};

class CWebSocketService : public IWEBSocketService
{
public:
	CWebSocketService();
	~CWebSocketService();

public:
	WORD		m_wServiceID;
	bool		m_bService;

	IWEBSocketEvent					*m_pIWebSocketEvent;
	CWebSocketServiceThread			m_WebSocketServiceThread;
	//接口
public:
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//启动接口
public:
	virtual bool StartService();
	//停止服务
	virtual bool ConcludeService();
	//IWebSocketService 接口
public:
	//配置函数
	virtual bool SetServiceID(WORD wServiceID);
	//设置接口
	virtual bool SetWEBSocketEvent(IUnknownEx * pIUnknownEx);

	//功能接口
public:
	//关闭连接
	virtual bool CloseSocket(BYTE cbShutReason);
	//连接操作
	virtual bool ConnectServer(LPCTSTR pszURL, WORD wPort);
	//发送请求
	virtual bool SendRequestData(VOID * pData, WORD wDataSize);

	//连接消息
	bool OnEventWEBSocketLink(WORD wServiceID, WORD wRequestID, INT nErrorCode);
	//关闭消息
	bool OnEventWEBSocketShut(WORD wServiceID, WORD wRequestID, BYTE cbShutReason);
	//数据包流
	bool OnEventWEBSocketMain(WORD wServiceID, WORD wRequestID, VOID * pcbMailData, WORD wStreamSize);
	//数据包头
	bool OnEventWEBSocketHead(WORD wServiceID, WORD wRequestID, VOID * pcbHeadData, WORD wHeadSize, INT nStatusCode);
};

