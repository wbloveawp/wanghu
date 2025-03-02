#include "StdAfx.h"
#include "ServiceManager.h"

#pragma warning (disable : 4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//�ر�����
#define IDI_CLOSE_LINK					100								//��������

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CServiceManager, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
//��̬��Ա

CServiceManager					*CServiceManager::m_pServiceManager = NULL;

//////////////////////////////////////////////////////////////////////////
//���캯��
CServiceManager::CServiceManager()
{
	//״̬����
	m_bSwitchDns=false;
	m_bTryDefault=false;
	m_bTryRedirect = false;
	m_wSwitchIndex=INVALID_WORD;

	//��ǰ����
	m_wCurrentPort=PORT_LOGON;
	m_dwCurrentServer=INADDR_NONE;

	//��ַ��Ϣ
	m_szCurrentServer[0]=0;

	//�������
	m_MissionSocket.SetMissionSocketSink(this);

	ASSERT(m_pServiceManager==NULL);
	if( m_pServiceManager == NULL )
	{
		m_pServiceManager = this;
	}

	return;
}

//��������
CServiceManager::~CServiceManager()
{
}

//�ӿڲ�ѯ
VOID * __cdecl CServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IRedirectServiceSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IRedirectServiceSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CServiceManager::OnEventMissionLink(INT nErrorCode)
{
	//�л�����
	if ((nErrorCode==WSAETIMEDOUT)||(nErrorCode==WSAECONNREFUSED)||(nErrorCode==WSAEADDRNOTAVAIL))
	{
		//�л�����
		if (!SwitchServerItem())
		{
			//�ض���
			if( RedirectionServer() ) return true;
		}
		else
		{
			m_MissionSocket.SetConnectInfo(m_dwCurrentServer,m_wCurrentPort);
			return m_MissionSocket.PerformConnect();
		}
	}

	//����֪ͨ
	for (INT_PTR i=0;i<m_MissionItemArray.GetCount();i++)
	{
		//�¼�����
		if (m_MissionItemArray[i]->IsActiveStatus()==true)
		{
			//����״̬
			if (nErrorCode!=0L)
			{
				m_MissionItemArray[i]->SetActiveStatus(false);
			}

			//�¼�֪ͨ
			m_MissionItemArray[i]->OnEventMissionLink(nErrorCode);
		}
	}

	return true;
}

//�ر��¼�
bool CServiceManager::OnEventMissionShut(BYTE cbShutReason)
{
	//�ر�֪ͨ
	for (INT_PTR i=0;i<m_MissionItemArray.GetCount();i++)
	{
		//�¼�����
		if (m_MissionItemArray[i]->IsActiveStatus()==true)
		{
			m_MissionItemArray[i]->SetActiveStatus(false);
			m_MissionItemArray[i]->OnEventMissionShut(cbShutReason);
		}
	}

	return true;
}

//��ȡ�¼�
bool CServiceManager::OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize)
{
	//����֪ͨ
	for (INT_PTR i=0;i<m_MissionItemArray.GetCount();i++)
	{
		if( m_MissionItemArray[i]->IsActiveStatus() )
			m_MissionItemArray[i]->OnEventMissionRead(Command,pData,wDataSize);
	}

	return true;
}

////�������
//bool __cdecl CServiceManager::OnRedirectComplete(LPCTSTR pszServerIP, int nParam)
//{
//	//���ñ���
//	lstrcpyn( m_szCurrentServer,pszServerIP,CountArray(m_szCurrentServer) );
//
//	//��ȡ��ַ
//	bool bSucc = GetServerInfo();
//
//	//���õ�ַ
//	if( bSucc )
//		m_MissionSocket.SetConnectInfo(m_dwCurrentServer,m_wCurrentPort);
//
//	//��������
//	if ( !bSucc || m_MissionSocket.PerformConnect()==false )
//	{
//		//�ر�֪ͨ
//		for (INT_PTR i=0;i<m_MissionItemArray.GetCount();i++)
//		{
//			//�¼�����
//			if (m_MissionItemArray[i]->IsActiveStatus())
//			{
//				m_MissionItemArray[i]->SetActiveStatus(false);
//				m_MissionItemArray[i]->OnEventMissionShut(SHUT_REASON_EXCEPTION);
//			}
//		}
//
//		return true;
//	}
//
//	//��������¼
//	if( m_szCurrentServer[0] != 0 )
//	{
//		CGlobalService GlobalService;
//		GlobalService.SetLastServerName(m_szCurrentServer);
//	}
//
//	return true;
//}
//
////�����쳣
//bool __cdecl CServiceManager::OnRedirectException(enDownLoadError DownLoadError)
//{
//	//�ر�֪ͨ
//	for (INT_PTR i=0;i<m_MissionItemArray.GetCount();i++)
//	{
//		//�¼�����
//		if (m_MissionItemArray[i]->IsActiveStatus())
//		{
//			m_MissionItemArray[i]->SetActiveStatus(false);
//			m_MissionItemArray[i]->OnEventMissionShut(SHUT_REASON_EXCEPTION);
//		}
//	}
//
//	return true;
//}

//���õ�ַ
VOID CServiceManager::SetCurrentServer(LPCTSTR pszCurrentServer)
{
	//���õ�ַ
	lstrcpyn( m_szCurrentServer,pszCurrentServer,CountArray(m_szCurrentServer) );

	return;
}

//��������
bool CServiceManager::InsertMissionItem(IServiceSocketSink * pMissionItem)
{
	//��������
	for (INT_PTR i=0;i<m_MissionItemArray.GetCount();i++)
	{
		if (m_MissionItemArray[i]==pMissionItem)
		{
			ASSERT(FALSE);
			return true;
		}
	}

	//��������
	pMissionItem->SetActiveStatus(false);
	pMissionItem->SetServiceManager(this);

	//��������
	m_MissionItemArray.Add(pMissionItem);

	return true;
}

//ɾ������
bool CServiceManager::DeleteMissionItem(IServiceSocketSink * pMissionItem)
{
	//��ֹ����
	if (pMissionItem->IsActiveStatus()==true)
	{
		ConcludeMissionItem(pMissionItem);
	}

	//ɾ������
	for (INT_PTR i=0;i<m_MissionItemArray.GetCount();i++)
	{
		if (m_MissionItemArray[i]==pMissionItem)
		{
			//ɾ������
			m_MissionItemArray.RemoveAt(i);

			//��������
			pMissionItem->SetActiveStatus(false);
			pMissionItem->SetServiceManager(NULL);

			return true;
		}
	}

	return false;
}

//��������
bool CServiceManager::AvtiveMissionItem(IServiceSocketSink * pMissionItem, bool bContinue)
{
	//Ч��״̬
	ASSERT(pMissionItem->GetServiceManager()==this);
	if (pMissionItem->GetServiceManager()!=this) return false;

	//�رմ���
	if (m_hWnd!=NULL) DestroyWindow();

	//��������
	pMissionItem->SetActiveStatus(true);

	//���Ӵ���
	switch (m_MissionSocket.GetSocketStatus())
	{
	case SOCKET_STATUS_IDLE:	//����״̬
		{
			if( bContinue==false )
			{
				//���ñ���
				m_bSwitchDns=false;
				m_bTryDefault=true;						//������Ĭ������
				m_bTryRedirect = false;
				m_wSwitchIndex=INVALID_WORD;

				//��ȡ��ַ
				if (m_szCurrentServer[0]==0)
				{
					CGlobalService GlobalService;
					GlobalService.GetLastServerName(m_szCurrentServer);
				}

				//���ñ���
				m_wCurrentPort=PORT_LOGON;

				//Ĭ�ϵ�ַ
				if ((GetServerInfo()==false)&&(SwitchServerItem()==false))
				{
					ASSERT(FALSE);
					pMissionItem->SetActiveStatus(false);
					return false;
				}
			}
			else
			{
				//�л�����
				if (SwitchServerItem()==false)
				{
					//�����ض���
					if( RedirectionServer() ) return true;

					pMissionItem->SetActiveStatus(false);
					return false;
				}
			}

			m_MissionSocket.SetConnectInfo(m_dwCurrentServer,m_wCurrentPort);
			//��������
			if (m_MissionSocket.PerformConnect()==false)
			{
				//��������
				pMissionItem->SetActiveStatus(false);

				return false;
			}

			if( m_szCurrentServer[0] != 0 )
			{
				CGlobalService GlobalService;
				GlobalService.SetLastServerName(m_szCurrentServer);
			}

			break;
		}
	case SOCKET_STATUS_CONNECT:	//����״̬
		{
			//�¼�֪ͨ
			pMissionItem->OnEventMissionLink(0L);

			break;
		}
	}

	return true;
}

//��ֹ����
bool CServiceManager::ConcludeMissionItem(IServiceSocketSink * pMissionItem)
{
	//Ч��״̬
	ASSERT(pMissionItem->IsActiveStatus()==true);
	if (pMissionItem->IsActiveStatus()==false) return false;

	//��������
	pMissionItem->SetActiveStatus(false);

	//��������
	UINT uActiveCount=0;
	BYTE cbSocketStatus=m_MissionSocket.GetSocketStatus();

	//��������
	for (INT_PTR i=0;i<m_MissionItemArray.GetCount();i++)
	{
		if (m_MissionItemArray[i]->IsActiveStatus()==true) uActiveCount++;
	}

	//��ֹ����
	if ((uActiveCount==0)&&(cbSocketStatus!=SOCKET_STATUS_IDLE))
	{
		//��������
		CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();

		//�ر��ж�
		if (pParameterGlobal->m_wIntermitTime>0)
		{
			//��������
			if (m_hWnd==NULL)
			{
				CRect rcCreate(0,0,0,0);
				Create(NULL,NULL,WS_CHILD,rcCreate,GetDesktopWindow(),100);
			}

			//����ʱ��
			SetTimer(IDI_CLOSE_LINK,pParameterGlobal->m_wIntermitTime*1000L,NULL);
		}
		else
		{
			//�ر�����
			m_MissionSocket.PerformClose();
		}
	}

	return false;
}

//���ͺ���
bool CServiceManager::SendData(WORD wMainCmdID, WORD wSubCmdID)
{
	//��������
	m_MissionSocket.PerformSendData(wMainCmdID,wSubCmdID);

	return true;
}

//���ͺ���
bool CServiceManager::SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize)
{
	//��������
	m_MissionSocket.PerformSendData(wMainCmdID,wSubCmdID,pData,wDataSize);

	return true;
}

//ʱ����Ϣ
VOID CServiceManager::OnTimer(UINT_PTR nIDEvent)
{
	//�ر�����
	if (nIDEvent==IDI_CLOSE_LINK)
	{
		//�رմ���
		DestroyWindow();

		//��ֹ����
		m_MissionSocket.PerformClose();

		return;
	}

	__super::OnTimer(nIDEvent);
}

//��ַ��Ϣ
bool CServiceManager::GetServerInfo()
{
	//Ч�����
	ASSERT(m_szCurrentServer[0]!=0);
	if (m_szCurrentServer[0]==0) return false;

	//��������
	tagServerItem ServerItem;
	ZeroMemory(&ServerItem,sizeof(ServerItem));

	//���ñ���
	m_wCurrentPort=PORT_LOGON;
	m_dwCurrentServer=INADDR_NONE;

	//��ȡ��Ϣ
	CGlobalService GlobalService;
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();
	bool bSuccess=GlobalService.GetServerItemInfo(m_szCurrentServer,pParameterGlobal->m_NetworkKind,ServerItem);

	//ת����ַ
	if (bSuccess==true)
	{
		//ʹ������
		if (m_bSwitchDns==true)
		{
			LPHOSTENT lpHost=gethostbyname(CT2CA(ServerItem.szServerHost));
			if (lpHost!=NULL) 
			{
				ServerItem.dwServerAddr=((LPIN_ADDR)lpHost->h_addr)->s_addr;

				//���µ�ַ
				if( ServerItem.szServerHost[0] != 0 )
					GlobalService.WriteServerItemInfo(m_szCurrentServer,pParameterGlobal->m_NetworkKind,ServerItem);
			}
		}

		//ʹ�õ�ַ
		if ((m_bSwitchDns==false)&&(ServerItem.dwServerAddr==INADDR_NONE))
		{
			CT2CA strServerDomain(ServerItem.szServerHost);
			ServerItem.dwServerAddr=inet_addr(strServerDomain);
		}
	}

	//���ַ���
	if (ServerItem.dwServerAddr==INADDR_NONE)
	{
		//��ַת��
		CT2CA CurrentServer(m_szCurrentServer);
		ServerItem.dwServerAddr=inet_addr(CurrentServer);

		//��������
		if (ServerItem.dwServerAddr==INADDR_NONE)
		{
			LPHOSTENT lpHost=gethostbyname(CurrentServer);
			if (lpHost!=NULL) ServerItem.dwServerAddr=((LPIN_ADDR)lpHost->h_addr)->s_addr;
		}
	}

	//�������
	if (ServerItem.dwServerAddr!=INADDR_NONE)
	{
		//���ñ���
		m_wCurrentPort=ServerItem.wServerPort;
		m_dwCurrentServer=ServerItem.dwServerAddr;

		return true;
	}

	return false;
}

//�л�����
bool CServiceManager::SwitchServerItem()
{
	TCHAR szCurrentServer[LEN_SERVER] = TEXT("");
	lstrcpyn( szCurrentServer,m_szCurrentServer,CountArray(szCurrentServer) );

	do
	{
		//��ȡ��Ϣ
		CGlobalService GlobalService;
		GlobalService.GetServerItemName(++m_wSwitchIndex,m_szCurrentServer);

		//״̬�л�
		if ((m_szCurrentServer[0]==0)&&(m_bSwitchDns==false))
		{
			//m_bSwitchDns=true;
			//m_wSwitchIndex=INVALID_WORD;
			//continue;
			//��������
			break;
		}

		//����ж�
		if ((m_szCurrentServer[0]==0)&&(m_bSwitchDns==true)) break;

		//��ͬ�ж�
		if ((m_bSwitchDns==false)&&(lstrcmp(m_szCurrentServer,szCurrentServer)==0)) continue;

		//��ȡ��Ϣ
		if (GetServerInfo()==true) return true;

	} while (true);

	//����Ĭ��
	if (m_bTryDefault==false)
	{
		//���ñ���
		m_bTryDefault=true;
		lstrcpyn(m_szCurrentServer,szLogonServer,CountArray(m_szCurrentServer));

		//��ȡ��Ϣ
		if (GetServerInfo()==true) return true;
	}

	return false;
}

//���¶���
bool CServiceManager::RedirectionServer()
{
	if( m_bTryRedirect ) return false;

	//���ñ���
	m_bTryRedirect = true;

	//��ʼ���������
	if (m_RedirectServiceHelper.GetInterface()==NULL)
	{
		if( !m_RedirectServiceHelper.CreateInstance() ) return false;

		m_RedirectServiceHelper->SetRedirectServiceSink(QUERY_ME_INTERFACE(IUnknownEx));
	}

	//�ض����ַ
	TCHAR szUrl[MAX_PATH] = {};
	CGlobalStation * pGlobalStation=CGlobalStation::GetInstance();
	pGlobalStation->GetRedirectionServer(2,szUrl,CountArray(szUrl));

	return m_RedirectServiceHelper->RequestRedirect(szUrl);
}

//////////////////////////////////////////////////////////////////////////