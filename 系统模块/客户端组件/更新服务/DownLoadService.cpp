#include "StdAfx.h"
#include "Resource.h"
#include "DownLoadService.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
CDownLoadService *		CDownLoadService::m_pDownLoadService=NULL;		//���ط���

//////////////////////////////////////////////////////////////////////////

//���캯��
CDownLoadService::CDownLoadService()
{
	//���ñ���
	m_dwMissionID=0L;
	m_pDlgDownLoad=NULL;
	m_pPlazaResource=NULL;
	m_hInstance=NULL;

	//���ñ���
	ASSERT(m_pDownLoadService==NULL);
	if (m_pDownLoadService==NULL) m_pDownLoadService=this;

	return;
}

//��������
CDownLoadService::~CDownLoadService()
{
	//���ñ���
	ASSERT(m_pDownLoadService==this);
	if (m_pDownLoadService==this) m_pDownLoadService=NULL;

// 	//��������
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
// 	//��������
// 	m_DlgDownLoadActive.RemoveAll();
// 	m_DlgDownLoadRelease.RemoveAll();
// 
// 	//��������
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

	//��������
	m_DownLoadMissionActive.RemoveAll();
	m_DownLoadMissionRelease.RemoveAll();

	return;
}

//�ӿڲ�ѯ
VOID * __cdecl CDownLoadService::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDownLoadService,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDownLoadService,Guid,dwQueryVer);
	return NULL;
}

//��������
DWORD __cdecl CDownLoadService::RequestDownLoad(const tagMissionRequest & MissionRequest)
{
	//Ѱ������
	CDownLoadMission * pDownLoadMission=SearchDownLoad(MissionRequest);
	if (pDownLoadMission!=NULL) return pDownLoadMission->GetMissionID();

	//��������
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

	//����Ч��
	ASSERT(pDownLoadMission!=NULL);
	if (pDownLoadMission==NULL) return 0L;

	//��������
	pDownLoadMission->SetRelease(true);
	m_DownLoadMissionActive.Add(pDownLoadMission);

	//��������
	m_dwMissionID++;
	pDownLoadMission->StartDownLoad(m_dwMissionID,MissionRequest);

	return pDownLoadMission->GetMissionID();
}

//��������
DWORD __cdecl CDownLoadService::RequestDownLoad(const tagDownLoadRequest & DownLoadRequest,DWORD dwPlazaStationID,DWORD dwUserStationID)
{
// 	//Ѱ������
// 	CDlgDownLoad * pDlgDownLoad=SearchDownLoad(DownLoadRequest);
// 
// 	//�����ж�
// 	if (pDlgDownLoad!=NULL) 
// 	{
// 		//��ʾ����
// 		if (pDlgDownLoad->IsWindowVisible())
// 		{
// 			pDlgDownLoad->ShowWindow(SW_RESTORE);
// 			pDlgDownLoad->BringWindowToTop();
// 		}
// 
// 		return pDlgDownLoad->m_DownLoadMission.GetMissionID();
// 	}
// 
// 	//��������
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
// 	//����Ч��
// 	ASSERT(pDlgDownLoad!=NULL);
// 	if (pDlgDownLoad==NULL) return 0L;
// 
// 	//��������
// 	pDlgDownLoad->SetRelease(true);
// 	m_DlgDownLoadActive.Add(pDlgDownLoad);
// 
// 	//��������
// 	m_dwMissionID++;
// 	pDlgDownLoad->StartDownLoad(m_dwMissionID,DownLoadRequest);
// 
// 	return pDlgDownLoad->m_DownLoadMission.GetMissionID();

	//��������
	if (m_pDlgDownLoad==NULL)
	{
		m_pDlgDownLoad=new CDlgDownLoad;
		m_pDlgDownLoad->SetResourceInterface(m_pPlazaResource,m_hInstance);
	}

	//��������
	if (m_pDlgDownLoad->m_hWnd==NULL)
	{
		m_pDlgDownLoad->Create(IDD_DLG_DOWNLOAD,CWnd::GetDesktopWindow());
	}

	//������Ϸ
	m_pDlgDownLoad->DownLoadClient(DownLoadRequest.szDescribe,DownLoadRequest.wKindID,DownLoadRequest.wServerID,dwPlazaStationID,dwUserStationID,DownLoadRequest.bSilent);
	return 0;
}

//�ͷ�����
bool CDownLoadService::ReleaseDownLoad(CDlgDownLoad * pDlgDownLoad)
{
	//Ѱ������

	// add by humorly 2018/8/1
	// make the var i outside loop
	INT_PTR i = 0;

	for (i=0;i<m_DlgDownLoadActive.GetCount();i++)
	{
		if (pDlgDownLoad==m_DlgDownLoadActive[i]) break;
	}

	//����Ч��
	ASSERT(i!=m_DlgDownLoadActive.GetCount());
	if (i==m_DlgDownLoadActive.GetCount()) return false;

	//��������
//	pDlgDownLoad->SetRelease(false);
	m_DlgDownLoadActive.RemoveAt(i);
	m_DlgDownLoadRelease.Add(pDlgDownLoad);

	return true;
}

//�ͷ�����
bool CDownLoadService::ReleaseDownLoad(CDownLoadMission * pDownLoadMission)
{
	//Ѱ������

	// add by humorly 2018/8/1
	// make the var i outside loop
	INT_PTR i = 0;
	for (i=0;i<m_DownLoadMissionActive.GetCount();i++)
	{
		if (pDownLoadMission==m_DownLoadMissionActive[i]) break;
	}

	//����Ч��
	ASSERT(i!=m_DownLoadMissionActive.GetCount());
	if (i==m_DownLoadMissionActive.GetCount()) return false;

	//��������
	pDownLoadMission->SetRelease(false);
	m_DownLoadMissionActive.RemoveAt(i);
	m_DownLoadMissionRelease.Add(pDownLoadMission);

	return true;
}

//��������
CDlgDownLoad * CDownLoadService::SearchDownLoad(const tagDownLoadRequest & DownLoadRequest)
{
	//��������
// 	for (INT_PTR i=0;i<m_DlgDownLoadActive.GetCount();i++)
// 	{
// 		CDlgDownLoad * pDlgDownLoad=m_DlgDownLoadActive[i];
// 		if (pDlgDownLoad->CompareRequest(DownLoadRequest)==true) return pDlgDownLoad;
// 	}

	return NULL;
}

//��������
CDownLoadMission * CDownLoadService::SearchDownLoad(const tagMissionRequest & MissionRequest)
{
	//��������
	for (INT_PTR i=0;i<m_DownLoadMissionActive.GetCount();i++)
	{
		CDownLoadMission * pDownLoadMission=m_DownLoadMissionActive[i];
		if (pDownLoadMission->CompareRequest(MissionRequest)==true) return pDownLoadMission;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(DownLoadService);

//////////////////////////////////////////////////////////////////////////
