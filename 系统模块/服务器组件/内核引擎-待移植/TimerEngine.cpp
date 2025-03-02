#include "StdAfx.h"
#include "TimerEngine.h"
#include "TraceServiceManager.h"

//////////////////////////////////////////////////////////////////////////

//�궨��
#define NO_TIME_LEAVE				DWORD(-1)							//����Ӧʱ��

//////////////////////////////////////////////////////////////////////////

//���캯��
CTimerThread::CTimerThread()
{
	//���ñ���
	m_dwLastTickCount=0L;
	m_dwTimerSpace=TIME_CELL;
	m_nTimerSpaceFix = 0;
	//����ӿ�
	m_pTimerEngine=NULL;

	return;
}

//��������
CTimerThread::~CTimerThread()
{
}

//���ú���
bool CTimerThread::InitThread(CTimerEngine * pTimerEngine, DWORD dwTimerSpace)
{
	//Ч�����
	ASSERT(dwTimerSpace>=10L);
	ASSERT(pTimerEngine!=NULL);
	if (pTimerEngine==NULL) return false;

	//���ñ���
	m_dwLastTickCount=0L;
	m_dwTimerSpace=dwTimerSpace;
	m_pTimerEngine=pTimerEngine;

	return true;
}

//���к���
bool CTimerThread::OnEventThreadRun()
{
	//Ч�����
	ASSERT(m_pTimerEngine!=NULL);

	//��ȡʱ��
	DWORD dwTimerSpace=m_dwTimerSpace;
	DWORD dwNowTickCount=GetTickCount();

	//�ȴ�����
	if ((m_dwLastTickCount!=0L)&&(dwNowTickCount>m_dwLastTickCount))
	{
		DWORD dwHandleTickCount=dwNowTickCount-m_dwLastTickCount;
		dwTimerSpace=(dwTimerSpace>dwHandleTickCount)?(dwTimerSpace-dwHandleTickCount):0L;
	}

	int nDiff = dwTimerSpace - m_nTimerSpaceFix;
	if (nDiff> 0)
	{
		dwTimerSpace = dwTimerSpace - m_nTimerSpaceFix;
		m_nTimerSpaceFix = 0;
	}
	else
	{
		m_nTimerSpaceFix -= dwTimerSpace;
		dwTimerSpace = 0;
	}
	
	if (dwTimerSpace > 0)
	{
		DWORD dwSleepBeginTickCount = GetTickCount();
		//��ʱ����
		Sleep(dwTimerSpace);
		DWORD dwSleepEndTickCount = GetTickCount();
		DWORD dwSleepDiff = 0;
		if (dwSleepEndTickCount > dwSleepBeginTickCount) dwSleepDiff = dwSleepEndTickCount - dwSleepBeginTickCount;

		m_nTimerSpaceFix += (dwSleepDiff - dwTimerSpace);
	}

	m_dwLastTickCount = GetTickCount();

	//ʱ�䴦��
	m_pTimerEngine->OnTimerThreadSink();

	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CTimerEngine::CTimerEngine()
{
	//״̬����
	m_bService=false;
	m_dwTimePass=0L;
	m_dwTimeLeave=NO_TIME_LEAVE;

	//���ñ���
	m_dwTimerSpace= TIME_CELL;
	m_pITimerEngineEvent=NULL;

	return;
}

//��������
CTimerEngine::~CTimerEngine()
{
	//ֹͣ����
	ConcludeService();

	//�����ڴ�
	tagTimerItem * pTimerItem=NULL;
	for (INT_PTR i=0;i<m_TimerItemFree.GetCount();i++)
	{
		pTimerItem=m_TimerItemFree[i];
		ASSERT(pTimerItem!=NULL);
		SafeDelete(pTimerItem);
	}
	for (INT_PTR i = 0; i<m_TimerItemActive.GetCount(); i++)
	{
		pTimerItem=m_TimerItemActive[i];
		ASSERT(pTimerItem!=NULL);
		SafeDelete(pTimerItem);
	}
	m_TimerItemFree.RemoveAll();
	m_TimerItemActive.RemoveAll();

	return;
}

//�ӿڲ�ѯ
VOID * CTimerEngine::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITimerEngine,Guid,dwQueryVer);
	QUERYINTERFACE(IServiceModule,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITimerEngine,Guid,dwQueryVer);
	return NULL;
}

//���ö�ʱ��
bool CTimerEngine::SetTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	//������Դ
	CThreadLock ThreadLock(m_CriticalSection);

	//Ч�����
	ASSERT(dwRepeat>0L);
	if (dwRepeat == 0 || dwElapse == 0) return false;
	dwElapse=(dwElapse+m_dwTimerSpace-1)/m_dwTimerSpace*m_dwTimerSpace;

	//���Ҷ�ʱ��
	bool bTimerExist=false;
	tagTimerItem * pTimerItem=NULL;
	for (INT_PTR i=0;i<m_TimerItemActive.GetCount();i++)
	{
		pTimerItem=m_TimerItemActive[i];
		if (pTimerItem->dwTimerID==dwTimerID) 
		{
			bTimerExist=true;
			break;
		}
	}

	//������ʱ��
	if (bTimerExist==false)
	{
		INT_PTR nFreeCount=m_TimerItemFree.GetCount();
		if (nFreeCount>0)
		{
			pTimerItem=m_TimerItemFree[nFreeCount-1];
			ASSERT(pTimerItem!=NULL);
			m_TimerItemFree.RemoveAt(nFreeCount-1);
		}
		else
		{
			try
			{
				pTimerItem=new tagTimerItem;
				ASSERT(pTimerItem!=NULL);
				if (pTimerItem==NULL) return false;
			}
			catch (...) { return false; }
		}
	}

	//���ò���
	ASSERT(pTimerItem!=NULL);
	pTimerItem->dwElapse=dwElapse;
	pTimerItem->dwTimerID=dwTimerID;
	pTimerItem->dwRepeatTimes=dwRepeat;
	pTimerItem->dwBindParameter=dwBindParameter;
	pTimerItem->dwTimeLeave=dwElapse+m_dwTimePass;

	//���ʱ��
	for (INT_PTR i = 0; i < m_TimerItemActive.GetCount(); i++)
	{
		m_dwTimeLeave = __min(m_dwTimeLeave, m_TimerItemActive[i]->dwTimeLeave);
	}
	m_dwTimeLeave=__min(m_dwTimeLeave,dwElapse);
	if (bTimerExist==false) m_TimerItemActive.Add(pTimerItem);

	return false;
}

//���ö�ʱ����Ԫ
void CTimerEngine::SetTimeCell(DWORD dwTimerCell)
{
	m_dwTimerSpace = dwTimerCell;
}

//ɾ����ʱ��
bool CTimerEngine::KillTimer(DWORD dwTimerID)
{
	//������Դ
	CThreadLock ThreadLock(m_CriticalSection);

	//���Ҷ�ʱ��
	for (INT_PTR i=0;i<m_TimerItemActive.GetCount();i++)
	{
		//��ȡ����
		tagTimerItem * pTimerItem=m_TimerItemActive[i];
		if (pTimerItem->dwTimerID!=dwTimerID) continue;

		//���ö���
		m_TimerItemActive.RemoveAt(i);
		m_TimerItemFree.Add(pTimerItem);

		//ֹͣ����
		if (m_TimerItemActive.GetCount()==0)
		{
			m_dwTimePass=0L;
			m_dwTimeLeave=NO_TIME_LEAVE;
		}

		return true;
	}

	return false;
}

//ɾ����ʱ��
bool CTimerEngine::KillAllTimer()
{
	//������Դ
	CThreadLock ThreadLock(m_CriticalSection);

	//ɾ����ʱ��
	m_TimerItemFree.Append(m_TimerItemActive);
	m_TimerItemActive.RemoveAll();

	//���ñ���
	m_dwTimePass=0L;
	m_dwTimeLeave=NO_TIME_LEAVE;

	return true;
}

//��ʼ����
bool CTimerEngine::StartService()
{
	//״̬Ч��
	ASSERT(m_bService==false);
	if (m_bService==true) return false;

	//���ñ���
	m_dwTimePass=0L;
	m_dwTimeLeave=NO_TIME_LEAVE;
	if (m_TimerThread.InitThread(this,m_dwTimerSpace)==false) return false;

	//��������
	if (m_TimerThread.StartThread()==false) return false;

	//���ñ���
	m_bService=true;

	return true;
}

//ֹͣ����
bool CTimerEngine::ConcludeService()
{
	//���ñ���
	m_bService=false;

	//ֹͣ�߳�
	m_TimerThread.ConcludeThread(INFINITE);

	//ɾ����ʱ��
	KillAllTimer();

	return true;
}

//���ýӿ�
bool CTimerEngine::SetTimerEngineEvent(IUnknownEx * pIUnknownEx)
{
	//״̬Ч��
	ASSERT(m_bService==false);
	if (m_bService==true) return false;

	//���ýӿ�
	if (pIUnknownEx!=NULL)
	{
		m_pITimerEngineEvent=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITimerEngineEvent);

		//�ɹ��ж�
		if (m_pITimerEngineEvent == NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}
	else
	{
		m_pITimerEngineEvent = NULL;
	}

    return true;
}

//��ʱ��֪ͨ
VOID CTimerEngine::OnTimerThreadSink()
{
	//������Դ
	CThreadLock ThreadLock(m_CriticalSection);

	//����ʱ��
	if (m_dwTimeLeave==NO_TIME_LEAVE)
	{
		ASSERT(m_TimerItemActive.GetCount()==0);
		return;
	}

	//����ʱ��
	m_dwTimePass+=m_dwTimerSpace;
	m_dwTimeLeave-=m_dwTimerSpace;

	//��ѯ��ʱ��
	if (m_dwTimeLeave==0)
	{
		//��������
		bool bKillTimer=false;
		DWORD dwTimeLeave=NO_TIME_LEAVE;

		//ʱ������
		for (INT_PTR i=0;i<m_TimerItemActive.GetCount();)
		{
			//��ȡ����
			tagTimerItem * pTimerItem=m_TimerItemActive[i];
			if (NULL == pTimerItem) break;
			ASSERT(pTimerItem->dwTimeLeave>=m_dwTimePass);

			//���ñ���
			bKillTimer=false;
			if (pTimerItem->dwTimeLeave<=m_dwTimePass)
			{
				pTimerItem->dwTimeLeave = 0;
			}
			else
			{
				pTimerItem->dwTimeLeave -= m_dwTimePass;
			}
			//֪ͨ�ж�
			if (pTimerItem->dwTimeLeave==0L)
			{
				//����֪ͨ
				ASSERT(m_pITimerEngineEvent!=NULL);
				m_pITimerEngineEvent->OnEventTimer(pTimerItem->dwTimerID,pTimerItem->dwBindParameter);

				//���ô���
				if (pTimerItem->dwRepeatTimes!=TIMES_INFINITY)
				{
					if (pTimerItem->dwRepeatTimes==1L)
					{
						bKillTimer=true;
						m_TimerItemActive.RemoveAt(i);
						m_TimerItemFree.Add(pTimerItem);
					}
					else pTimerItem->dwRepeatTimes--;
				}

				//����ʱ��
				if (bKillTimer==false) pTimerItem->dwTimeLeave=pTimerItem->dwElapse;
			}

			//��������
			if (bKillTimer==false) 
			{
				i++;
				dwTimeLeave=__min(dwTimeLeave,pTimerItem->dwTimeLeave);
				ASSERT(dwTimeLeave%m_dwTimerSpace==0);
			}
		}

		//������Ӧ
		m_dwTimePass=0L;
		m_dwTimeLeave=dwTimeLeave;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(TimerEngine);

//////////////////////////////////////////////////////////////////////////
