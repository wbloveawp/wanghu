#include "StdAfx.h"
#include "MissionManager.h"

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

BEGIN_MESSAGE_MAP(CMissionManager, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CMissionSocket::CMissionSocket()
{
	//��ǰ����
	m_wCurrentPort=PORT_LOGON;
	m_dwCurrentServer=INADDR_NONE;

	//�ӿڱ���
	m_pIMissionSocketSink=NULL;

	return;
}

//��������
CMissionSocket::~CMissionSocket()
{
}

//�ӿڲ�ѯ
VOID * __cdecl CMissionSocket::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITCPSocketSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITCPSocketSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool __cdecl CMissionSocket::OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode)
{
	//����֪ͨ
	ASSERT(m_pIMissionSocketSink!=NULL);
	if (m_pIMissionSocketSink!=NULL) m_pIMissionSocketSink->OnEventMissionLink(nErrorCode);

	return true;
}

//�ر��¼�
bool __cdecl CMissionSocket::OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason)
{
	//�ر�֪ͨ
	ASSERT(m_pIMissionSocketSink!=NULL);
	if (m_pIMissionSocketSink!=NULL) m_pIMissionSocketSink->OnEventMissionShut(cbShutReason);

	return true;
}

//��ȡ�¼�
bool __cdecl CMissionSocket::OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID * pData, WORD wDataSize)
{
	//��ȡ֪ͨ
	ASSERT(m_pIMissionSocketSink!=NULL);
	if (m_pIMissionSocketSink!=NULL) m_pIMissionSocketSink->OnEventMissionRead(Command,pData,wDataSize);

	return true;
}

//����״̬
BYTE CMissionSocket::GetSocketStatus()
{
	//��������
	BYTE cbSocketStatus=SOCKET_STATUS_IDLE;

	//��ȡ״̬
	if (m_TCPSocketModule.GetInterface()!=NULL)
	{
		cbSocketStatus=m_TCPSocketModule->GetSocketStatus();
	}

	return cbSocketStatus;
}

//����������Ϣ
VOID CMissionSocket::SetConnectInfo( DWORD dwServerIP, WORD wPort )
{
	m_dwCurrentServer = dwServerIP;
	m_wCurrentPort = wPort;
}

//�ж�����
bool CMissionSocket::PerformClose()
{
	//�ر�����
	ASSERT(m_TCPSocketModule.GetInterface()!=NULL);
	if (m_TCPSocketModule.GetInterface()!=NULL) m_TCPSocketModule->CloseSocket();

	return true;
}

//��������
bool CMissionSocket::PerformConnect()
{
	//״̬Ч��
	ASSERT(m_dwCurrentServer!=INADDR_NONE);
	if( m_dwCurrentServer == INADDR_NONE ) return false;

	//�������
	if (m_TCPSocketModule.GetInterface()==NULL)
	{
		if (m_TCPSocketModule.CreateInstance()==false) return false;
		if (m_TCPSocketModule->SetTCPSocketSink(QUERY_ME_INTERFACE(IUnknownEx))==false) return false;
	}

	//���ô���
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();
	enProxyServerType ProxyServerType=pParameterGlobal->m_ProxyServerType;
	tagProxyServerInfo & ProxyServerInfo=pParameterGlobal->m_ProxyServerInfo;
	if (m_TCPSocketModule->SetProxyServerInfo(ProxyServerType,ProxyServerInfo)==false) return false;

	//��������
	if (m_TCPSocketModule->Connect(m_dwCurrentServer,m_wCurrentPort)!=CONNECT_SUCCESS) return false;

	return true;
}

//���ͺ���
bool CMissionSocket::PerformSendData(WORD wMainCmdID, WORD wSubCmdID)
{
	//״̬Ч��
	ASSERT(GetSocketStatus()==SOCKET_STATUS_CONNECT);
	if (GetSocketStatus()!=SOCKET_STATUS_CONNECT) return false;

	//��������
	ASSERT(m_TCPSocketModule.GetInterface()!=NULL);
	if (m_TCPSocketModule.GetInterface()!=NULL) m_TCPSocketModule->SendData(wMainCmdID,wSubCmdID);

	return true;
}

//���ͺ���
bool CMissionSocket::PerformSendData(WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize)
{
	//״̬Ч��
	ASSERT(GetSocketStatus()==SOCKET_STATUS_CONNECT);
	if (GetSocketStatus()!=SOCKET_STATUS_CONNECT) return false;

	//��������
	ASSERT(m_TCPSocketModule.GetInterface()!=NULL);
	if (m_TCPSocketModule.GetInterface()!=NULL) m_TCPSocketModule->SendData(wMainCmdID,wSubCmdID,pData,wDataSize);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CMissionItem::CMissionItem()
{
	//���ñ���
	m_bActiveStatus=false;

	return;
}

//��������
CMissionItem::~CMissionItem()
{
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CMissionManager::CMissionManager()
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
	
	return;
}

//��������
CMissionManager::~CMissionManager()
{
}

//�ӿڲ�ѯ
VOID * __cdecl CMissionManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IRedirectServiceSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IRedirectServiceSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CMissionManager::OnEventMissionLink(INT nErrorCode)
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
		//��ȡ����
		CMissionItem * pMissionItem=m_MissionItemArray[i];

		//�¼�����
		if (pMissionItem->m_bActiveStatus==true)
		{
			//����״̬
			if (nErrorCode!=0L)
			{
				pMissionItem->m_bActiveStatus=false;
			}
			else
			{
				//������������
				CreateVirtualLink();
			}

			//�¼�֪ͨ
			pMissionItem->OnEventMissionLink(nErrorCode);
		}
	}

	return true;
}

//�ر��¼�
bool CMissionManager::OnEventMissionShut(BYTE cbShutReason)
{
	//�ر�֪ͨ
	for (INT_PTR i=0;i<m_MissionItemArray.GetCount();i++)
	{
		//��ȡ����
		CMissionItem * pMissionItem=m_MissionItemArray[i];

		//�¼�����
		if (pMissionItem->m_bActiveStatus==true)
		{
			pMissionItem->m_bActiveStatus=false;
			pMissionItem->OnEventMissionShut(cbShutReason);
		}
	}

	return true;
}

//��ȡ�¼�
bool CMissionManager::OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize)
{
	//����֪ͨ
	for (INT_PTR i=0;i<m_MissionItemArray.GetCount();i++)
	{
		CMissionItem * pMissionItem=m_MissionItemArray[i];
		if (pMissionItem->m_bActiveStatus == true)
			pMissionItem->OnEventMissionRead(Command, pData, wDataSize);
	}

	return true;
}

////�������
//bool __cdecl CMissionManager::OnRedirectComplete(LPCTSTR pszServerIP, int nParam)
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
//			//��ȡ����
//			CMissionItem * pMissionItem=m_MissionItemArray[i];
//
//			//�¼�����
//			if (pMissionItem->m_bActiveStatus==true)
//			{
//				pMissionItem->m_bActiveStatus=false;
//				pMissionItem->OnEventMissionShut(SHUT_REASON_EXCEPTION);
//			}
//		}
//
//		return true;
//	}
//
//	//��������¼
//	if( m_szCurrentServer[0] != 0 )
//	{
//		CGlobalServer GlobalServer;
//		GlobalServer.SetLastServerName(m_szCurrentServer);
//	}
//
//	return true;
//}
//
////�����쳣
//bool __cdecl CMissionManager::OnRedirectException(enDownLoadError DownLoadError)
//{
//	//�ر�֪ͨ
//	for (INT_PTR i=0;i<m_MissionItemArray.GetCount();i++)
//	{
//		//��ȡ����
//		CMissionItem * pMissionItem=m_MissionItemArray[i];
//
//		//�¼�����
//		if (pMissionItem->m_bActiveStatus==true)
//		{
//			pMissionItem->m_bActiveStatus=false;
//			pMissionItem->OnEventMissionShut(SHUT_REASON_EXCEPTION);
//		}
//	}
//
//	return true;
//}

//���õ�ַ
VOID CMissionManager::SetCurrentServer(LPCTSTR pszCurrentServer)
{
	//���õ�ַ
	lstrcpyn( m_szCurrentServer,pszCurrentServer,CountArray(m_szCurrentServer) );

	return;
}

//��������
bool CMissionManager::InsertMissionItem(CMissionItem * pMissionItem)
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
	pMissionItem->m_bActiveStatus=false;
	pMissionItem->m_pMissionManager=this;

	//��������
	m_MissionItemArray.Add(pMissionItem);

	return true;
}

//ɾ������
bool CMissionManager::DeleteMissionItem(CMissionItem * pMissionItem)
{
	//��ֹ����
	if (pMissionItem->m_bActiveStatus==true)
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
			pMissionItem->m_bActiveStatus=false;
			pMissionItem->m_pMissionManager=NULL;

			return true;
		}
	}

	return false;
}

//��������
bool CMissionManager::AvtiveMissionItem(CMissionItem * pMissionItem, bool bContinue)
{
	//Ч��״̬
	ASSERT(pMissionItem->m_pMissionManager==this);
	if (pMissionItem->m_pMissionManager!=this) return false;

	//�رմ���
	if (m_hWnd!=NULL) DestroyWindow();

	//��������
	pMissionItem->m_bActiveStatus=true;

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
					CGlobalServer GlobalServer;
					GlobalServer.GetLastServerName(m_szCurrentServer);
				}

				//���ñ���
				m_wCurrentPort=PORT_LOGON;

				//Ĭ�ϵ�ַ
				if ((GetServerInfo()==false)&&(SwitchServerItem()==false))
				{
					ASSERT(FALSE);
					pMissionItem->m_bActiveStatus = false;
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

					pMissionItem->m_bActiveStatus = false;
					return false;
				}
			}

			m_MissionSocket.SetConnectInfo(m_dwCurrentServer,m_wCurrentPort);
			//��������
			if (m_MissionSocket.PerformConnect()==false)
			{
				//��������
				pMissionItem->m_bActiveStatus=false;

				return false;
			}

			if( m_szCurrentServer[0] != 0 )
			{
				CGlobalServer GlobalServer;
				GlobalServer.SetLastServerName(m_szCurrentServer);
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
bool CMissionManager::ConcludeMissionItem(CMissionItem * pMissionItem)
{
	//Ч��״̬
	ASSERT(pMissionItem->m_bActiveStatus==true);
	if (pMissionItem->m_bActiveStatus==false) return false;

	//��������
	pMissionItem->m_bActiveStatus=false;

	//��������
	UINT uActiveCount=0;
	BYTE cbSocketStatus=m_MissionSocket.GetSocketStatus();

	//��������
	for (INT_PTR i=0;i<m_MissionItemArray.GetCount();i++)
	{
		CMissionItem * pMissionItem=m_MissionItemArray[i];
		if (pMissionItem->m_bActiveStatus==true) uActiveCount++;
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
bool CMissionManager::SendData(WORD wMainCmdID, WORD wSubCmdID)
{
	//��������
	m_MissionSocket.PerformSendData(wMainCmdID,wSubCmdID);

	return true;
}

//���ͺ���
bool CMissionManager::SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize)
{
	//��������
	m_MissionSocket.PerformSendData(wMainCmdID,wSubCmdID,pData,wDataSize);

	return true;
}

//ʱ����Ϣ
VOID CMissionManager::OnTimer(UINT_PTR nIDEvent)
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
bool CMissionManager::GetServerInfo()
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
	CGlobalServer GlobalServer;
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();
	bool bSuccess=GlobalServer.GetServerItemInfo(m_szCurrentServer,pParameterGlobal->m_NetworkKind,ServerItem);

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
					GlobalServer.WriteServerItemInfo(m_szCurrentServer,pParameterGlobal->m_NetworkKind,ServerItem);
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
		m_wCurrentPort= ServerItem.wServerPort;
		m_dwCurrentServer=ServerItem.dwServerAddr;

		return true;
	}

	return false;
}

//�л�����
bool CMissionManager::SwitchServerItem()
{
	TCHAR szCurrentServer[LEN_SERVER] = TEXT("");
	lstrcpyn( szCurrentServer,m_szCurrentServer,CountArray(szCurrentServer) );

	do
	{
		//��ȡ��Ϣ
		CGlobalServer GlobalServer;
		GlobalServer.GetServerItemName(++m_wSwitchIndex,m_szCurrentServer);

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
bool CMissionManager::RedirectionServer()
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
	pGlobalStation->GetRedirectionServer(1,szUrl,CountArray(szUrl));

	return m_RedirectServiceHelper->RequestRedirect(szUrl);
}

//������������
void CMissionManager::CreateVirtualLink()
{
	CMD_GG_C_LinkService linkService;
	ZeroMemory(&linkService, sizeof(linkService));

	//linkService.dwStationID = 1000;
	linkService.wServiceID = INVALID_WORD;
	linkService.wServiceModule = SERVICE_MODULE_LOGON;

	//��������
	SendData(MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_C_LINK_SERVICE, &linkService, sizeof(linkService));
}

//////////////////////////////////////////////////////////////////////////