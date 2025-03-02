#ifndef REDIRECT_SERVICE_HEAD_FILE
#define REDIRECT_SERVICE_HEAD_FILE

#pragma once

#include "Wininet.h"
#include "DownLoadSocket.h"
#include "HttpHeadTranslate.h"
#include "DownLoadMission.h"
#include "UpdateServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//重定向服务
class CRedirectService : public IRedirectService, public IDownLoadSocketSink
{
	//地址变量
protected:
	TCHAR							m_szRedirectServer[LEN_SERVER];		//定向地址

	//任务信息
protected:
	enDownLoadStatus				m_DownLoadStatus;					//下载状态
	tagRequestInfo					m_RequestInfo;						//请求信息

	//文件信息
protected:
	DWORD							m_dwDownLoadSize;					//下载大小
	DWORD							m_dwTotalFileSize;					//文件大小
	CHAR							m_szDownLoadBuffer[1024];			//文件缓冲

	//组件变量
protected:
	CDownLoadSocket					m_DownLoadSocket;					//下载连接
	CHttpHeadTranslate				m_HttpHeadTranslate;				//协议翻译
	IRedirectServiceSink			*m_pIRedirectServiceSink;			//定向回调

	//函数定义
public:
	//构造函数
	CRedirectService();
	//析构函数
	virtual ~CRedirectService();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { delete this; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//配置接口
public:
	//设置接口
	virtual bool __cdecl SetRedirectServiceSink(IUnknownEx * pIUnknownEx);

	//控制接口
public:
	//请求定向
	virtual bool __cdecl RequestRedirect(LPCTSTR lpURL);
	//停止定向
	virtual VOID __cdecl StopRedirect();

	//查询接口
public:
	//获取定向地址
	virtual LPCTSTR __cdecl GetRedirectServer() { return m_szRedirectServer; }

	//状态接口
public:
	//连接消息
	virtual bool __cdecl OnSocketConnect(INT nErrorCode);
	//关闭消息
	virtual bool __cdecl OnSocketClose(bool bCloseByServer);

	//数据接口
public:
	//数据包流
	virtual VOID __cdecl OnHttpDataStream(const VOID * pStreamData, WORD wStreamSize);
	//数据包头
	virtual VOID __cdecl OnHttpDataHead(const VOID * pcbHeadData, WORD wHeadSize, INT nStatusCode);

	//辅助函数
protected:
	//请求连接
	bool ConnectServer(LPCTSTR pszDownLoadUrl);
	//发送请求
	bool SendHttpRequest(const tagRequestInfo & RequestInfo);
	//获取信息
	INTERNET_SCHEME GetDownLoadInfo(LPCTSTR pszDownUrl, tagRequestInfo & RequestInfo);
};

//////////////////////////////////////////////////////////////////////////

#endif