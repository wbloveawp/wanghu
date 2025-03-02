#ifndef DOWN_LOAD_MISSION_HEAD_FILE
#define DOWN_LOAD_MISSION_HEAD_FILE

#pragma once

#include "Wininet.h"
#include "DownLoadSocket.h"
#include "HttpHeadTranslate.h"
#include "UpdateServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//类型说明

//类说明
class CDownLoadMission;

//数组说明
typedef CWHArray<CDownLoadMission *> CDownLoadMissionArray;		//任务数组

//////////////////////////////////////////////////////////////////////////
//结构定义

//请求信息
struct tagRequestInfo
{
	WORD							wHostPort;							//连接端口
	TCHAR							szHostName[128];					//服务器名
	TCHAR							szRequestPath[256];					//请求路径
};

//////////////////////////////////////////////////////////////////////////

//下载任务
class CDownLoadMission : public IDownLoadMission, public IDownLoadSocketSink
{
	//友元定义
	friend class CDownLoadService;

	//任务信息
protected:
	DWORD							m_dwMissionID;						//任务标识
	enDownLoadStatus				m_DownLoadStatus;					//下载状态

	//文件信息
protected:
	DWORD							m_dwDownLoadSize;					//下载大小
	DWORD							m_dwTotalFileSize;					//文件大小
	LPCTSTR							m_pszFileEntityTag;					//文件标识

	//内核信息
protected:
	bool							m_bRelease;							//释放释放
	tagRequestInfo					m_RequestInfo;						//请求信息
	tagMissionRequest				m_MissionRequest;					//下载请求
	IDownLoadMissionSink *			m_pIDownLoadMissionSink;			//回调接口

	//组件变量
protected:
	CFile							m_BufferFile;						//缓冲文件
	CDownLoadSocket					m_DownLoadSocket;					//下载连接
	CHttpHeadTranslate				m_HttpHeadTranslate;				//协议翻译

	//函数定义
public:
	//构造函数
	CDownLoadMission();
	//析构函数
	virtual ~CDownLoadMission();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { delete this; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//配置接口
public:
	//设置接口
	virtual bool __cdecl SetDownLoadMissionSink(IUnknownEx * pIUnknownEx);

	//任务信息
public:
	//任务标识
	virtual DWORD __cdecl GetMissionID() { return m_dwMissionID; }
	//下载状态
	virtual enDownLoadStatus __cdecl GetDownLoadStatus() { return m_DownLoadStatus; }

	//文件信息
public:
	//下载大小
	virtual DWORD __cdecl GetDownLoadSize() { return m_dwDownLoadSize; }
	//文件大小
	virtual DWORD __cdecl GetTotalFileSize() { return m_dwTotalFileSize; }
	//文件标识
	virtual LPCTSTR __cdecl GetFileEntityTag() { return m_pszFileEntityTag; }

	//控制接口
public:
	//结束下载
	virtual bool __cdecl StopDownLoad();
	//开始下载
	virtual bool __cdecl StartDownLoad(DWORD dwMissionID, const tagMissionRequest & MissionRequest);

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

	//功能函数
public:
	//设置释放
	bool SetRelease(bool bRelease);
	//对比请求
	bool CompareRequest(const tagMissionRequest & MissionRequest);

	//辅助函数
protected:
	//请求连接
	bool ConnectServer(LPCTSTR pszDownLoadUrl);
	//发送请求
	bool SendHttpRequest(const tagRequestInfo & RequestInfo);
	//创建文件
	bool CreateBufferFile(const tagMissionRequest & MissionRequest);
	//获取信息
	INTERNET_SCHEME GetDownLoadInfo(LPCTSTR pszDownUrl, tagRequestInfo & RequestInfo);
};

//////////////////////////////////////////////////////////////////////////

#endif