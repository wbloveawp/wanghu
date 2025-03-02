#include "StdAfx.h"
#include "VirtualLinkManager.h"

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

BEGIN_MESSAGE_MAP(CVirtualLinkManager, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
// 
//���캯��
CSessionItem::CSessionItem()
{
	//���ñ���
	m_bActiveStatus = false;
	m_pVirtualLinkItem = NULL;
}

//��������
CSessionItem::~CSessionItem()
{
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CVirtualLinkItem::CVirtualLinkItem()
{
	//״̬����
	m_cbLinkStatus= LINK_STATUS_IDLE;

	//���ñ���	
	m_wServiceID=0;
	m_wServiceModule=0;
	m_pVirtualLinkManager=NULL;
	ZeroMemory(&m_LinkService, sizeof(m_LinkService));

	return;
}

//��������
CVirtualLinkItem::~CVirtualLinkItem()
{
}

//׼������
bool CVirtualLinkItem::OnEventSocketReady()
{
	if (m_pVirtualLinkManager != NULL)
	{
		m_pVirtualLinkManager->SendSocketData(MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_C_LINK_SERVICE, &m_LinkService, sizeof(m_LinkService));
	}

	return true;
}

//�ر��¼�
bool CVirtualLinkItem::OnEventVirtualShut(BYTE cbShutReason)
{
	for (INT_PTR i = 0; i < m_SessionItemArray.GetCount(); i++)
	{
		if (m_SessionItemArray[i]->m_bActiveStatus == true)
		{
			m_SessionItemArray[i]->m_bActiveStatus = false;
			m_SessionItemArray[i]->OnEventMissionShut(cbShutReason);			
		}
	}

	return true;
}

//�����¼�
bool CVirtualLinkItem::OnEventVirtualLink(LONG lErrorCode)
{
	for (INT_PTR i = 0; i < m_SessionItemArray.GetCount(); i++)
	{
		if (m_SessionItemArray[i]->m_bActiveStatus==true)
		{
			m_SessionItemArray[i]->OnEventMissionLink(lErrorCode);			
		}
	}

	return true;
}

//��ȡ�¼�
bool CVirtualLinkItem::OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	for (INT_PTR i = 0; i < m_SessionItemArray.GetCount(); i++)
	{
		if (m_SessionItemArray[i]->m_bActiveStatus == true)
		{
			m_SessionItemArray[i]->OnEventMissionRead(Command, pData, wDataSize);
		}
	}

	return true;
}

//��������
bool CVirtualLinkItem::InsertSessionItem(CSessionItem* pSessionItem)
{
	//��������
	for (INT_PTR i = 0; i < m_SessionItemArray.GetCount(); i++)
	{
		if (m_SessionItemArray[i] == pSessionItem)
		{
			//ASSERT(FALSE);
			return true;
		}
	}

	//��������
	pSessionItem->m_bActiveStatus = false;
	pSessionItem->m_pVirtualLinkItem = this;

	//��������
	m_SessionItemArray.Add(pSessionItem);

	return true;
}

//ɾ������
VOID CVirtualLinkItem::DeleteSessionItem()
{
	//ɾ������
	for (INT_PTR i = 0; i < m_SessionItemArray.GetCount(); i++)
	{
		//��������
		m_SessionItemArray[i]->m_bActiveStatus = false;
		m_SessionItemArray[i]->m_pVirtualLinkItem = NULL;
	}

	m_SessionItemArray.RemoveAll();
}

//ɾ������
bool CVirtualLinkItem::DeleteSessionItem(CSessionItem* pSessionItem)
{
	//ɾ������
	for (INT_PTR i = 0; i < m_SessionItemArray.GetCount(); i++)
	{
		if (m_SessionItemArray[i] == pSessionItem)
		{
			//ɾ������
			m_SessionItemArray.RemoveAt(i);

			//��������
			pSessionItem->m_bActiveStatus = false;
			pSessionItem->m_pVirtualLinkItem = NULL;

			return true;
		}
	}

	return false;
}

//��������
bool CVirtualLinkItem::SendVirtualData(WORD wMainCmdID, WORD wSubCmdID)
{
	//У��״̬
	ASSERT(m_wServiceModule != 0 && m_cbLinkStatus == LINK_STATUS_LINKED);
	if (m_wServiceModule == 0 || m_cbLinkStatus != LINK_STATUS_LINKED) return false;

	return m_pVirtualLinkManager->SendSocketData(MAKECMMD(wMainCmdID, m_wServiceModule), wSubCmdID, NULL, 0);
}

//��������
bool CVirtualLinkItem::SendVirtualData(WORD wMainCmdID, WORD wSubCmdID, VOID* const pData, WORD wDataSize)
{
	//У��״̬
	ASSERT(m_wServiceModule != 0 && m_cbLinkStatus == LINK_STATUS_LINKED);
	if (m_wServiceModule == 0 || m_cbLinkStatus != LINK_STATUS_LINKED) return false;

	return m_pVirtualLinkManager->SendSocketData(MAKECMMD(wMainCmdID, m_wServiceModule), wSubCmdID, pData, wDataSize);
}

//�ر�����
VOID CVirtualLinkItem::CloseVirtualLink()
{
	//����ṹ
	CMD_GG_C_DislinkService DislinkService;
	ZeroMemory(&DislinkService,sizeof(DislinkService));

	//���ñ���
	DislinkService.wServiceModule = m_wServiceModule;

	//��������
	m_pVirtualLinkManager->SendSocketData(MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_C_DISLINK_SERVICE, &DislinkService, sizeof(DislinkService));

	//����״̬
	m_cbLinkStatus = LINK_STATUS_IDLE;
}

//��������
bool CVirtualLinkItem::CreateVirtualLink(WORD wServiceID, DWORD dwUserID, LPCTSTR pszLogonToken, BYTE cbExtendData[], WORD wDataSize)
{
	//ģ����Ϣ
	m_LinkService.wServiceID = wServiceID;
	m_LinkService.wServiceModule = m_wServiceModule;

	//�û���Ϣ
	m_LinkService.dwUserID = dwUserID;

	//��¼����
	if (pszLogonToken != NULL)
	{
		StringCchCopy(m_LinkService.szLogonToken, CountArray(m_LinkService.szLogonToken), pszLogonToken);
	}	

	//��չ����
	if (wDataSize>0 && wDataSize<=CountArray(m_LinkService.cbLinkExtend))
	{
		CopyMemory(m_LinkService.cbLinkExtend, cbExtendData, wDataSize);
	}	

	//�ж�״̬
	return m_pVirtualLinkManager->ActiveVirtualLink(this);	
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CVirtualLinkManager::CVirtualLinkManager()
{
	//״̬����
	m_bSwitchDns = false;
	m_bTryDefault = false;
	m_bTryRedirect = false;
	m_wSwitchIndex = INVALID_WORD;

	//��ǰ����
	m_wCurrentPort = PORT_LOGON;
	m_dwCurrentServer = INADDR_NONE;

	//��ַ��Ϣ
	m_szCurrentServer[0] = 0;

	return;
}

//��������
CVirtualLinkManager::~CVirtualLinkManager()
{
	//�ر�����
	ConcludeVirtualLink();
}

//�ӿڲ�ѯ
VOID* __cdecl CVirtualLinkManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITCPSocketSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITCPSocketSink, Guid, dwQueryVer);
	return NULL;
}

//�����¼�
bool CVirtualLinkManager::OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode)
{
	//�ر�֪ͨ
	for (INT_PTR i = 0; i < m_VirtualLinkArray.GetCount(); i++)
	{
		//��ȡ����
		CVirtualLinkItem* pVirtualLinkItem = m_VirtualLinkArray[i];
		if (pVirtualLinkItem == NULL) continue;

		//�¼�����
		if (pVirtualLinkItem->m_cbLinkStatus == LINK_STATUS_WAIT)
		{
			if (nErrorCode == 0)
			{
				pVirtualLinkItem->OnEventSocketReady();
			}
			else
			{
				pVirtualLinkItem->OnEventVirtualLink(nErrorCode);
			}
		}
	}

	return true;
}

//�ر��¼�
bool CVirtualLinkManager::OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason)
{
	//�ر�֪ͨ
	for (INT_PTR i=0; i<m_VirtualLinkArray.GetCount(); i++)
	{
		//��ȡ����
		CVirtualLinkItem * pVirtualLinkItem=m_VirtualLinkArray[i];
		if (pVirtualLinkItem == NULL) continue;

		//�¼�����
		if (pVirtualLinkItem->m_cbLinkStatus == LINK_STATUS_LINKED)
		{
			//����״̬
			pVirtualLinkItem->m_wServiceID = 0;
			pVirtualLinkItem->m_cbLinkStatus = LINK_STATUS_IDLE;

			//�ر�֪ͨ
			pVirtualLinkItem->OnEventVirtualShut(cbShutReason);
		}
	}	

	return true;
}

//��ȡ�¼�
bool CVirtualLinkManager::OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID* pData, WORD wDataSize)
{
	//��ȡģ��
	WORD wServiceModule = HICMMD(Command.wMainCmdID);

	//�Ƴ�ģ��
	Command.wMainCmdID = LOCMMD(Command.wMainCmdID);

	//����ģ��
	if (wServiceModule == SERVICE_MODULE_GATE)
	{
		//���ӳɹ�
		if (Command.wSubCmdID == SUB_GG_S_LINK_SUCCESS)
		{
			//��ȡ����
			auto pLinkSuccess = (CMD_GG_S_LinkSuccess*)pData;		
			if (m_VirtualLinkArray.GetCount() >= pLinkSuccess->wServiceModule)
			{
				//��ȡ����
				auto pVirtualLinkItem = m_VirtualLinkArray.GetAt(pLinkSuccess->wServiceModule - 1);
				if (pVirtualLinkItem != NULL)
				{
					//����״̬
					pVirtualLinkItem->m_cbLinkStatus = LINK_STATUS_LINKED;
					pVirtualLinkItem->m_wServiceID = pLinkSuccess->wServiceID;

					//����֪ͨ
					pVirtualLinkItem->OnEventVirtualLink(0);
				}
			}

			return true;
		}
		
		//����ʧ��
		if (Command.wSubCmdID == SUB_GG_S_LINK_FAILURE)
		{
			//��ȡ����
			auto pLinkFailure = (CMD_GG_S_LinkFailure*)pData;
			if (m_VirtualLinkArray.GetCount() >= pLinkFailure->wServiceModule)
			{
				//��ȡ����
				auto pVirtualLinkItem = m_VirtualLinkArray.GetAt(pLinkFailure->wServiceModule - 1);
				if (pVirtualLinkItem != NULL)
				{
					//����֪ͨ
					pVirtualLinkItem->OnEventVirtualLink(pLinkFailure->lErrorCode);
				}
			}

			return true;
		}

		//���ӶϿ�
		if (Command.wSubCmdID == SUB_GG_S_LINK_SHUTDOWN)
		{
			//��ȡ����
			auto pLinkShutdown = (CMD_GG_S_LinkShutdown*)pData;
			if (m_VirtualLinkArray.GetCount() >= pLinkShutdown->wServiceModule)
			{
				//��ȡ����
				auto pVirtualLinkItem = m_VirtualLinkArray.GetAt(pLinkShutdown->wServiceModule - 1);
				if (pVirtualLinkItem != NULL)
				{
					//����״̬
					pVirtualLinkItem->m_wServiceID = 0;
					pVirtualLinkItem->m_cbLinkStatus = LINK_STATUS_IDLE;

					//�ر�֪ͨ
					pVirtualLinkItem->OnEventVirtualShut(SHUT_REASON_NORMAL);
				}
			}

			//ͳ������
			WORD wLinkCount = 0;
			for (int i = 0; i < m_VirtualLinkArray.GetCount(); i++)
			{
				if (m_VirtualLinkArray[i] == NULL) continue;
				if (m_VirtualLinkArray[i]->m_cbLinkStatus != LINK_STATUS_IDLE) wLinkCount++;
			}

			//�ر�����
			if (wLinkCount==0) ConcludeVirtualLink();

			return true;
		}

		return true;
	}

	//��ȡ����
	if (m_VirtualLinkArray.GetCount() >= wServiceModule)
	{
		auto pVirtualLinkItem = m_VirtualLinkArray.GetAt(wServiceModule - 1);
		if (pVirtualLinkItem != NULL && pVirtualLinkItem->m_cbLinkStatus == LINK_STATUS_LINKED)
		{
			pVirtualLinkItem->OnEventVirtualRead(Command, pData, wDataSize);
		}
	}

	return true;
}

//����״̬
BYTE CVirtualLinkManager::GetSocketStatus()
{
	//��������
	BYTE cbSocketStatus = SOCKET_STATUS_IDLE;

	//��ȡ״̬
	if (m_TCPSocketModule.GetInterface() != NULL)
	{
		cbSocketStatus = m_TCPSocketModule->GetSocketStatus();
	}

	return cbSocketStatus;
}

//���õ�ַ
VOID CVirtualLinkManager::SetCurrentServer(LPCTSTR pszCurrentServer)
{
	//���õ�ַ
	StringCchCopy(m_szCurrentServer, CountArray(m_szCurrentServer), pszCurrentServer);

	return;
}

//��������
bool CVirtualLinkManager::InsertVirtualLink(CVirtualLinkItem * pVirtualLinkItem)
{
	//У�����
	ASSERT (pVirtualLinkItem!=NULL && pVirtualLinkItem->GetServiceModule()>0);
	if (pVirtualLinkItem == NULL || pVirtualLinkItem->GetServiceModule() == 0) return false;

	//��ȡģ��
	WORD wServiceModule = pVirtualLinkItem->GetServiceModule();

	//������С
	if (wServiceModule > m_VirtualLinkArray.GetCount())
	{
		m_VirtualLinkArray.SetSize(wServiceModule);
	}
	
	//��������
	pVirtualLinkItem->m_cbLinkStatus= LINK_STATUS_IDLE;
	pVirtualLinkItem->m_pVirtualLinkManager=this;

	//��������
	m_VirtualLinkArray[wServiceModule-1]=pVirtualLinkItem;

	return true;
}

//ɾ������
bool CVirtualLinkManager::DeleteVirtualLink(CVirtualLinkItem * pVirtualLinkItem)
{
	//�ر�����
	if (pVirtualLinkItem->m_cbLinkStatus==LINK_STATUS_LINKED)
	{
		pVirtualLinkItem->CloseVirtualLink();
	}

	//��ȡģ��
	WORD wServiceModule = pVirtualLinkItem->GetServiceModule();
	if (wServiceModule > m_VirtualLinkArray.GetCount()) return false;

	//�Ƴ�����
	m_VirtualLinkArray.RemoveAt(wServiceModule-1);

	return true;
}

//�ر�����
bool CVirtualLinkManager::ConcludeVirtualLink()
{
	//�ر�����
	//ASSERT(m_TCPSocketModule.GetInterface() != NULL);
	if (m_TCPSocketModule.GetInterface() != NULL) m_TCPSocketModule->CloseSocket();

	return true;
}

//��������
bool CVirtualLinkManager::ActiveVirtualLink(CVirtualLinkItem* pVirtualLinkItem)
{
	//У�����
	ASSERT (pVirtualLinkItem!=NULL);
	if (pVirtualLinkItem == NULL) return false;

	//��ȡ״̬
	auto cbSocketStatus = GetSocketStatus();

	//����״̬
	if (cbSocketStatus ==SOCKET_STATUS_IDLE)
	{
		//�رմ���
		if (m_hWnd != NULL) DestroyWindow();

		//���ñ���
		m_bSwitchDns = false;
		m_bTryDefault = true;
		m_bTryRedirect = false;
		m_wSwitchIndex = INVALID_WORD;

		//����״̬
		pVirtualLinkItem->m_cbLinkStatus = LINK_STATUS_IDLE;

		//��ȡ��ַ
		if (m_szCurrentServer[0] == 0)
		{
			CGlobalServer GlobalServer;
			GlobalServer.GetLastServerName(m_szCurrentServer);
		}

		//���ñ���
		m_wCurrentPort = PORT_LOGON;

		//Ĭ�ϵ�ַ
		if ((GetServerInfo() == false) && (SwitchServerItem() == false))
		{
			ASSERT(FALSE);
			return false;
		}

		//��������
		if (m_hWnd == NULL)
		{
			CRect rcCreate(0, 0, 0, 0);
			Create(NULL, NULL, WS_CHILD, rcCreate, GetDesktopWindow(), 100);
		}

		//�������
		if (m_TCPSocketModule.GetInterface() == NULL)
		{
			if (m_TCPSocketModule.CreateInstance() == false) return false;
			if (m_TCPSocketModule->SetTCPSocketSink(QUERY_ME_INTERFACE(IUnknownEx)) == false) return false;
		}

		//��������
		if (m_TCPSocketModule->Connect(m_dwCurrentServer, m_wCurrentPort) != CONNECT_SUCCESS) return false;

		if (m_szCurrentServer[0] != 0)
		{
			CGlobalServer GlobalServer;
			GlobalServer.SetLastServerName(m_szCurrentServer);
		}		

		//����״̬
		pVirtualLinkItem->m_cbLinkStatus = LINK_STATUS_WAIT;
	}

	//����״̬
	if (cbSocketStatus == SOCKET_STATUS_CONNECT)
	{
		//����״̬
		pVirtualLinkItem->m_cbLinkStatus = LINK_STATUS_LINKED;

		//���Ӿ���
		pVirtualLinkItem->OnEventSocketReady();
	}	

	return true;
}

//���ͺ���
bool CVirtualLinkManager::SendSocketData(WORD wMainCmdID, WORD wSubCmdID)
{
	//״̬Ч��
	//ASSERT(GetSocketStatus() == SOCKET_STATUS_CONNECT);
	if (GetSocketStatus() != SOCKET_STATUS_CONNECT) return false;

	//��������
	ASSERT(m_TCPSocketModule.GetInterface() != NULL);
	if (m_TCPSocketModule.GetInterface() != NULL) m_TCPSocketModule->SendData(wMainCmdID, wSubCmdID);

	return true;
}

//���ͺ���
bool CVirtualLinkManager::SendSocketData(WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize)
{
	//״̬Ч��
	//ASSERT(GetSocketStatus() == SOCKET_STATUS_CONNECT);
	if (GetSocketStatus() != SOCKET_STATUS_CONNECT) return false;

	//��������
	ASSERT(m_TCPSocketModule.GetInterface() != NULL);
	if (m_TCPSocketModule.GetInterface() != NULL) m_TCPSocketModule->SendData(wMainCmdID, wSubCmdID, pData, wDataSize);

	return true;
}

//ʱ����Ϣ
VOID CVirtualLinkManager::OnTimer(UINT_PTR nIDEvent)
{
	//�ر�����
	if (nIDEvent==IDI_CLOSE_LINK)
	{
		//�رմ���
		DestroyWindow();

		//��ֹ����
		//PerformClose();

		return;
	}

	__super::OnTimer(nIDEvent);
}


//��ַ��Ϣ
bool CVirtualLinkManager::GetServerInfo()
{
	//Ч�����
	ASSERT(m_szCurrentServer[0] != 0);
	if (m_szCurrentServer[0] == 0) return false;

	//��������
	tagServerItem ServerItem;
	ZeroMemory(&ServerItem, sizeof(ServerItem));

	//���ñ���
	m_wCurrentPort = PORT_LOGON;
	m_dwCurrentServer = INADDR_NONE;

	//��ȡ��Ϣ
	CGlobalServer GlobalServer;
	CParameterGlobal* pParameterGlobal = CParameterGlobal::GetInstance();
	bool bSuccess = GlobalServer.GetServerItemInfo(m_szCurrentServer, pParameterGlobal->m_NetworkKind, ServerItem);

	//ת����ַ
	if (bSuccess == true)
	{
		//ʹ������
		if (m_bSwitchDns == true)
		{
			LPHOSTENT lpHost = gethostbyname(CT2CA(ServerItem.szServerHost));
			if (lpHost != NULL)
			{
				ServerItem.dwServerAddr = ((LPIN_ADDR)lpHost->h_addr)->s_addr;

				//���µ�ַ
				if (ServerItem.szServerHost[0] != 0)
					GlobalServer.WriteServerItemInfo(m_szCurrentServer, pParameterGlobal->m_NetworkKind, ServerItem);
			}
		}

		//ʹ�õ�ַ
		if ((m_bSwitchDns == false) && (ServerItem.dwServerAddr == INADDR_NONE))
		{
			CT2CA strServerDomain(ServerItem.szServerHost);
			ServerItem.dwServerAddr = inet_addr(strServerDomain);
		}
	}

	//���ַ���
	if (ServerItem.dwServerAddr == INADDR_NONE)
	{
		//��ַת��
		CT2CA CurrentServer(m_szCurrentServer);
		ServerItem.dwServerAddr = inet_addr(CurrentServer);

		//��������
		if (ServerItem.dwServerAddr == INADDR_NONE)
		{
			LPHOSTENT lpHost = gethostbyname(CurrentServer);
			if (lpHost != NULL) ServerItem.dwServerAddr = ((LPIN_ADDR)lpHost->h_addr)->s_addr;
		}
	}

	//�������
	if (ServerItem.dwServerAddr != INADDR_NONE)
	{
		//���ñ���
		m_wCurrentPort = ServerItem.wServerPort;
		m_dwCurrentServer = ServerItem.dwServerAddr;

		return true;
	}

	return false;
}

//�л�����
bool CVirtualLinkManager::SwitchServerItem()
{
	TCHAR szCurrentServer[LEN_SERVER] = TEXT("");
	StringCchCopy(szCurrentServer, CountArray(szCurrentServer), m_szCurrentServer);

	do
	{
		//��ȡ��Ϣ
		CGlobalServer GlobalServer;
		GlobalServer.GetServerItemName(++m_wSwitchIndex, m_szCurrentServer);

		//״̬�л�
		if ((m_szCurrentServer[0] == 0) && (m_bSwitchDns == false))
		{
			//m_bSwitchDns=true;
			//m_wSwitchIndex=INVALID_WORD;
			//continue;
			//��������
			break;
		}

		//����ж�
		if ((m_szCurrentServer[0] == 0) && (m_bSwitchDns == true)) break;

		//��ͬ�ж�
		if ((m_bSwitchDns == false) && (lstrcmp(m_szCurrentServer, szCurrentServer) == 0)) continue;

		//��ȡ��Ϣ
		if (GetServerInfo() == true) return true;

	} while (true);

	//����Ĭ��
	if (m_bTryDefault == false)
	{
		//���ñ��� 
		m_bTryDefault = true;
		StringCchCopy(m_szCurrentServer, CountArray(m_szCurrentServer), szLogonServer);

		//��ȡ��Ϣ
		if (GetServerInfo() == true) return true;
	}

	return false;
}

//��ַ����
DWORD CVirtualLinkManager::TranslateAddress(LPCTSTR szServerIP)
{
	//ת����ַ
	CT2CA ServerAddr(szServerIP);
	DWORD dwServerIP = inet_addr(ServerAddr);

	//��������
	if (dwServerIP == INADDR_NONE)
	{
		LPHOSTENT lpHost = gethostbyname(ServerAddr);
		if (lpHost == NULL) return INADDR_NONE;
		dwServerIP = ((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}

	return dwServerIP;
}

//////////////////////////////////////////////////////////////////////////