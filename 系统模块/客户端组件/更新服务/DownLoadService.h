#ifndef DOWN_LOAD_SERVICE_HEAD_FILE
#define DOWN_LOAD_SERVICE_HEAD_FILE

#pragma once

#include "DlgDownLoad.h"
#include "DownLoadMission.h"
#include "UpdateServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//下载服务
class CDownLoadService : public IDownLoadService
{
	//友元定义
	friend class CDlgDownLoad;
	friend class CDownLoadMission;

	//变量定义
protected:
	DWORD							m_dwMissionID;						//任务标识

	//组件变量
protected:
	CDlgDownLoadArray				m_DlgDownLoadActive;				//下载窗口
	CDlgDownLoadArray				m_DlgDownLoadRelease;				//下载窗口
	CDownLoadMissionArray			m_DownLoadMissionActive;			//下载任务
	CDownLoadMissionArray			m_DownLoadMissionRelease;			//下载任务
	CDlgDownLoad *					m_pDlgDownLoad;						//下载组件

	IPlazaResource					*m_pPlazaResource;					//资源接口
	HINSTANCE						m_hInstance;						//资源句柄
	//静态变量
protected:
	static CDownLoadService *		m_pDownLoadService;					//下载服务

	//函数定义
public:
	//构造函数
	CDownLoadService();
	//析构函数
	virtual ~CDownLoadService();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { delete this; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//下载接口
public:
	//请求下载
	virtual DWORD __cdecl RequestDownLoad(const tagMissionRequest & MissionRequest);
	//请求下载
	virtual DWORD __cdecl RequestDownLoad(const tagDownLoadRequest & DownLoadRequest,DWORD dwPlazaStationID,DWORD dwUserStationID);
	//设置资源
	virtual	void __cdecl SetResourceInterface(IPlazaResource *pPlazaResource,HINSTANCE hInstance){m_pPlazaResource=pPlazaResource,m_hInstance=hInstance;}

	//释放函数
protected:
	//释放下载
	bool ReleaseDownLoad(CDlgDownLoad * pDlgDownLoad);
	//释放下载
	bool ReleaseDownLoad(CDownLoadMission * pDownLoadMission);

	//查找函数
protected:
	//查找下载
	CDlgDownLoad * SearchDownLoad(const tagDownLoadRequest & DownLoadRequest);
	//查找下载
	CDownLoadMission * SearchDownLoad(const tagMissionRequest & MissionRequest);

	//静态函数
public:
	//获取实例
	static CDownLoadService * GetInstance() { return m_pDownLoadService; }
};

//////////////////////////////////////////////////////////////////////////

#endif