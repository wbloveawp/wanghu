#include "StdAfx.h"
#include "Resource.h"
#include "DownLoadService.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
CDownLoadService *		CDownLoadService::m_pDownLoadService=NULL;		//下载服务

//////////////////////////////////////////////////////////////////////////

//构造函数
CDownLoadService::CDownLoadService()
{
	//设置变量
	m_dwMissionID=0L;
	m_pDlgDownLoad=NULL;
	m_pPlazaResource=NULL;
	m_hInstance=NULL;

	//设置变量
	ASSERT(m_pDownLoadService==NULL);
	if (m_pDownLoadService==NULL) m_pDownLoadService=this;

	return;
}

//析构函数
CDownLoadService::~CDownLoadService()
{
	//设置变量
	ASSERT(m_pDownLoadService==this);
	if (m_pDownLoadService==this) m_pDownLoadService=NULL;

// 	//清理下载
// 	CDlgDownLoad * pDlgDownLoad=NULL;
// 	for (INT_PTR i=0;i<m_DlgDownLoadActive.GetCount();i++)
// 	{
// 		pDlgDownLoad=m_DlgDownLoadActive[i];
// 		pDlgDownLoad->StopDownLoad();
// 		pDlgDownLoad->DestroyWindow();
// 		SafeDelete(pDlgDownLoad);
// 	}
// 	for (INT_PTR i=0;i<m_DlgDownLoadRelease.GetCount();i++)
// 	{
// 		pDlgDownLoad=m_DlgDownLoadRelease[i];
// 		SafeDelete(pDlgDownLoad);
// 	}
// 
// 	//清理数组
// 	m_DlgDownLoadActive.RemoveAll();
// 	m_DlgDownLoadRelease.RemoveAll();
// 
// 	//清理任务
// 	CDownLoadMission * pDownLoadMission=NULL;
// 	for (INT_PTR i=0;i<m_DownLoadMissionActive.GetCount();i++)
// 	{
// 		pDownLoadMission=m_DownLoadMissionActive[i];
// 		pDownLoadMission->StopDownLoad();
// 		SafeDelete(pDownLoadMission);
// 	}
// 	for (INT_PTR i=0;i<m_DownLoadMissionRelease.GetCount();i++)
// 	{
// 		pDownLoadMission=m_DownLoadMissionRelease[i];
// 		SafeDelete(pDownLoadMission);
// 	}

	//清理数组
	m_DownLoadMissionActive.RemoveAll();
	m_DownLoadMissionRelease.RemoveAll();

	return;
}

//接口查询
VOID * __cdecl CDownLoadService::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDownLoadService,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDownLoadService,Guid,dwQueryVer);
	return NULL;
}

//请求下载
DWORD __cdecl CDownLoadService::RequestDownLoad(const tagMissionRequest & MissionRequest)
{
	//寻找下载
	CDownLoadMission * pDownLoadMission=SearchDownLoad(MissionRequest);
	if (pDownLoadMission!=NULL) return pDownLoadMission->GetMissionID();

	//创建下载
	INT_PTR nReleaseCount=m_DownLoadMissionRelease.GetCount();
	if (nReleaseCount>0)
	{
		pDownLoadMission=m_DownLoadMissionRelease[nReleaseCount-1];
		m_DownLoadMissionRelease.RemoveAt(nReleaseCount-1);
	}
	else
	{
		try { pDownLoadMission=new CDownLoadMission; }
		catch (...) { return 0L; }
	}

	//错误效验
	ASSERT(pDownLoadMission!=NULL);
	if (pDownLoadMission==NULL) return 0L;

	//插入数组
	pDownLoadMission->SetRelease(true);
	m_DownLoadMissionActive.Add(pDownLoadMission);

	//配置下载
	m_dwMissionID++;
	pDownLoadMission->StartDownLoad(m_dwMissionID,MissionRequest);

	return pDownLoadMission->GetMissionID();
}

//请求下载
DWORD __cdecl CDownLoadService::RequestDownLoad(const tagDownLoadRequest & DownLoadRequest,DWORD dwPlazaStationID,DWORD dwUserStationID)
{
// 	//寻找下载
// 	CDlgDownLoad * pDlgDownLoad=SearchDownLoad(DownLoadRequest);
// 
// 	//存在判断
// 	if (pDlgDownLoad!=NULL) 
// 	{
// 		//显示窗口
// 		if (pDlgDownLoad->IsWindowVisible())
// 		{
// 			pDlgDownLoad->ShowWindow(SW_RESTORE);
// 			pDlgDownLoad->BringWindowToTop();
// 		}
// 
// 		return pDlgDownLoad->m_DownLoadMission.GetMissionID();
// 	}
// 
// 	//创建下载
// 	INT_PTR nReleaseCount=m_DlgDownLoadRelease.GetCount();
// 	if (nReleaseCount>0)
// 	{
// 		pDlgDownLoad=m_DlgDownLoadRelease[nReleaseCount-1];
// 		m_DlgDownLoadRelease.RemoveAt(nReleaseCount-1);
// 	}
// 	else
// 	{
// 		try { pDlgDownLoad=new CDlgDownLoad; }
// 		catch (...) { return 0L; }
// 	}
// 
// 	//错误效验
// 	ASSERT(pDlgDownLoad!=NULL);
// 	if (pDlgDownLoad==NULL) return 0L;
// 
// 	//插入数组
// 	pDlgDownLoad->SetRelease(true);
// 	m_DlgDownLoadActive.Add(pDlgDownLoad);
// 
// 	//配置下载
// 	m_dwMissionID++;
// 	pDlgDownLoad->StartDownLoad(m_dwMissionID,DownLoadRequest);
// 
// 	return pDlgDownLoad->m_DownLoadMission.GetMissionID();

	//创建对象
	if (m_pDlgDownLoad==NULL)
	{
		m_pDlgDownLoad=new CDlgDownLoad;
		m_pDlgDownLoad->SetResourceInterface(m_pPlazaResource,m_hInstance);
	}

	//创建窗口
	if (m_pDlgDownLoad->m_hWnd==NULL)
	{
		m_pDlgDownLoad->Create(IDD_DLG_DOWNLOAD,CWnd::GetDesktopWindow());
	}

	//下载游戏
	m_pDlgDownLoad->DownLoadClient(DownLoadRequest.szDescribe,DownLoadRequest.wKindID,DownLoadRequest.wServerID,dwPlazaStationID,dwUserStationID,DownLoadRequest.bSilent);
	return 0;
}

//释放下载
bool CDownLoadService::ReleaseDownLoad(CDlgDownLoad * pDlgDownLoad)
{
	//寻找下载

	// add by humorly 2018/8/1
	// make the var i outside loop
	INT_PTR i = 0;

	for (i=0;i<m_DlgDownLoadActive.GetCount();i++)
	{
		if (pDlgDownLoad==m_DlgDownLoadActive[i]) break;
	}

	//错误效验
	ASSERT(i!=m_DlgDownLoadActive.GetCount());
	if (i==m_DlgDownLoadActive.GetCount()) return false;

	//清理下载
//	pDlgDownLoad->SetRelease(false);
	m_DlgDownLoadActive.RemoveAt(i);
	m_DlgDownLoadRelease.Add(pDlgDownLoad);

	return true;
}

//释放下载
bool CDownLoadService::ReleaseDownLoad(CDownLoadMission * pDownLoadMission)
{
	//寻找下载

	// add by humorly 2018/8/1
	// make the var i outside loop
	INT_PTR i = 0;
	for (i=0;i<m_DownLoadMissionActive.GetCount();i++)
	{
		if (pDownLoadMission==m_DownLoadMissionActive[i]) break;
	}

	//错误效验
	ASSERT(i!=m_DownLoadMissionActive.GetCount());
	if (i==m_DownLoadMissionActive.GetCount()) return false;

	//清理下载
	pDownLoadMission->SetRelease(false);
	m_DownLoadMissionActive.RemoveAt(i);
	m_DownLoadMissionRelease.Add(pDownLoadMission);

	return true;
}

//查找下载
CDlgDownLoad * CDownLoadService::SearchDownLoad(const tagDownLoadRequest & DownLoadRequest)
{
	//查找下载
// 	for (INT_PTR i=0;i<m_DlgDownLoadActive.GetCount();i++)
// 	{
// 		CDlgDownLoad * pDlgDownLoad=m_DlgDownLoadActive[i];
// 		if (pDlgDownLoad->CompareRequest(DownLoadRequest)==true) return pDlgDownLoad;
// 	}

	return NULL;
}

//查找下载
CDownLoadMission * CDownLoadService::SearchDownLoad(const tagMissionRequest & MissionRequest)
{
	//查找下载
	for (INT_PTR i=0;i<m_DownLoadMissionActive.GetCount();i++)
	{
		CDownLoadMission * pDownLoadMission=m_DownLoadMissionActive[i];
		if (pDownLoadMission->CompareRequest(MissionRequest)==true) return pDownLoadMission;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(DownLoadService);

//////////////////////////////////////////////////////////////////////////
