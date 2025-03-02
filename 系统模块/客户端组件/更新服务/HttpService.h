#ifndef HTTP_SERVICE_HEAD_FILE
#define HTTP_SERVICE_HEAD_FILE

#pragma once

#include "Wininet.h"
#include "DownLoadSocket.h"
#include "HttpHeadTranslate.h"
#include "DownLoadMission.h"
#include "UpdateServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//HTTP服务
class CHttpService : public IHttpService, public IDownLoadSocketSink
{
	//缓冲变量
protected:
	LPVOID							m_lpvBuffer;						//缓冲地址
	DWORD							m_dwBufSize;						//缓冲大小

	//任务信息
protected:
	enDownLoadStatus				m_DownLoadStatus;					//下载状态
	tagRequestInfo					m_RequestInfo;						//请求信息

	//文件信息
protected:
	DWORD							m_dwDownLoadSize;					//下载大小
	DWORD							m_dwTotalFileSize;					//文件大小

	//组件变量
protected:
	CDownLoadSocket					m_DownLoadSocket;					//下载连接
	CHttpHeadTranslate				m_HttpHeadTranslate;				//协议翻译
	IHttpServiceSink				*m_pIHttpServiceSink;				//服务回调

	//函数定义
public:
	//构造函数
	CHttpService();
	//析构函数
	virtual ~CHttpService();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { delete this; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//配置接口
public:
	//设置接口
	virtual bool __cdecl SetHttpServiceSink(IHttpServiceSink * pSink);

	//控制接口
public:
	//连接请求
	virtual bool __cdecl ConnectServer(LPCTSTR lpURL);
	//停止服务
	virtual VOID __cdecl StopService();

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
	//发送请求
	bool SendHttpRequest(const tagRequestInfo & RequestInfo);
	//获取信息
	INTERNET_SCHEME GetDownLoadInfo(LPCTSTR pszDownUrl, tagRequestInfo & RequestInfo);
};

//////////////////////////////////////////////////////////////////////////

#endif