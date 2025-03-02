#ifndef DOWN_LOAD_SOCKET_HEAD_FILE
#define DOWN_LOAD_SOCKET_HEAD_FILE

#pragma once

#include "WinSock2.h"
#include "UpdateServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//下载连接
class CDownLoadSocket : public CWnd
{
	//状态变量
protected:
	WORD							m_wRequestID;						//请求标识
	bool							m_bHeadValid;						//有效标志
	bool							m_bCloseByServer;					//关闭方式

	//数据包头
protected:
	WORD							m_wHeadSize;						//包头长度
	CHAR							m_szHeadData[1024];					//数据包头

	//内核变量
protected:
	SOCKET							m_hSocket;							//连接句柄
	BYTE							m_cbSocketStatus;					//网络状态
	IDownLoadSocketSink *			m_pIDownLoadSocketSink;				//回调接口

	//代理信息
protected:
	enProxyServerType				m_ProxyServerType;					//代理类型
	tagProxyServerInfo				m_ProxyServerInfo;					//代理信息

	//函数定义
public:
	//构造函数
	CDownLoadSocket();
	//析构函数
	virtual ~CDownLoadSocket();

	//信息接口
public:
	//获取状态
	BYTE GetSocketStatus() { return m_cbSocketStatus; }

	//配置函数
public:
	//设置接口
	bool SetDownLoadSocketSink(IUnknownEx * pIUnknownEx);

	//代理接口
public:
	//代理类型
	enProxyServerType GetProxyServerType() { return m_ProxyServerType; }
	//设置代理
	bool SetProxyServerInfo(enProxyServerType ProxyServerType, const tagProxyServerInfo & ProxyServerInfo);

	//操作接口
public:
	//关闭连接
	bool CloseSocket(bool bNotify);
	//连接操作
	bool Connect(LPCTSTR pszServerAddr, WORD wPort);
	//发送请求
	bool SendRequestData(VOID * const pData, WORD wDataSize);

	//辅助函数
protected:
	//连接代理
	bool ConnectProxyServer();
	//地址解释
	DWORD TranslateAddr(LPCTSTR pszServerAddr);

	//处理函数
protected:
	//网络读取
	LRESULT OnSocketNotifyRead(WPARAM wParam, LPARAM lParam);
	//网络关闭
	LRESULT OnSocketNotifyClose(WPARAM wParam, LPARAM lParam);
	//网络连接
	LRESULT OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam);

	//消息映射
protected:
	//网络消息
	LRESULT	OnSocketNotify(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif