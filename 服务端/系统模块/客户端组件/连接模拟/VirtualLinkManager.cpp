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

//���캯��
CVirtualLinkItem::CVirtualLinkItem()
{
	//״̬����
	m_bLinkStatus=false;

	//���ñ���	
	m_wServiceID=0;
	m_wServiceModule=0;
	m_pVirtualLinkManager=NULL;

	return;
}

//��������
CVirtualLinkItem::~CVirtualLinkItem()
{
}

//��������
bool CVirtualLinkItem::SendVirtualData(WORD wMainCmdID, WORD wSubCmdID)
{
	//У��״̬
	ASSERT (m_wServiceID!=0 && m_wServiceModule!=0);
	if (m_wServiceID == 0 || m_wServiceModule==0) return false;

	return m_pVirtualLinkManager->SendSocketData(MAKECMMD(wMainCmdID, m_wServiceModule), wSubCmdID, NULL, 0);
}

//��������
bool CVirtualLinkItem::SendVirtualData(WORD wMainCmdID, WORD wSubCmdID, VOID* const pData, WORD wDataSize)
{
	//У��״̬
	ASSERT(m_wServiceID != 0 && m_wServiceModule != 0);
	if (m_wServiceID == 0 || m_wServiceModule == 0) return false;

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
	m_bLinkStatus = false;
}

//��������
bool CVirtualLinkItem::CreateVirtualLink(WORD wServiceID, DWORD dwUserID, LPCTSTR pszLogonToken, BYTE cbExtendData[], WORD wDataSize)
{
	//����ṹ
	CMD_GG_C_LinkService LinkService;
	ZeroMemory(&LinkService,sizeof(LinkService));

	//ģ����Ϣ
	LinkService.wServiceID = wServiceID;
	LinkService.wServiceModule = m_wServiceModule;

	//�û���Ϣ
	LinkService.dwUserID = dwUserID;

	//��¼����
	if (pszLogonToken != NULL)
	{
		StringCchCopy(LinkService.szLogonToken, CountArray(LinkService.szLogonToken), pszLogonToken);
	}	

	//��չ����
	if (wDataSize>0 && wDataSize<=CountArray(LinkService.cbLinkExtend))
	{
		CopyMemory(LinkService.cbLinkExtend, cbExtendData, wDataSize);
	}	

	return m_pVirtualLinkManager->SendSocketData(MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_C_LINK_SERVICE, &LinkService, sizeof(LinkService));
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CVirtualLinkManager::CVirtualLinkManager()
{
	//������Ϣ
	m_wServerPort=0;
	m_dwServerAddr=INADDR_NONE;

	return;
}

//��������
CVirtualLinkManager::~CVirtualLinkManager()
{
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
	//�¼�֪ͨ
	ASSERT(m_pIVirtualManagerSink != NULL);
	m_pIVirtualManagerSink->OnEventTCPSocketLink(nErrorCode);

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
		if (pVirtualLinkItem->m_bLinkStatus ==true)
		{
			pVirtualLinkItem->m_bLinkStatus =false;
			pVirtualLinkItem->OnEventVirtualShut();
		}
	}

	//�¼�֪ͨ
	ASSERT(m_pIVirtualManagerSink != NULL);
	m_pIVirtualManagerSink->OnEventTCPSocketShut(cbShutReason);

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

			//��ȡ����
			auto pVirtualLinkItem = m_VirtualLinkArray.GetAt(pLinkSuccess->wServiceModule-1);
			if (pVirtualLinkItem != NULL)
			{
				//����״̬
				pVirtualLinkItem->m_bLinkStatus = true;
				pVirtualLinkItem->m_wServiceID = pLinkSuccess->wServiceID;

				//����֪ͨ
				pVirtualLinkItem->OnEventVirtualLink(0);
			}

			return true;
		}
		
		//����ʧ��
		if (Command.wSubCmdID == SUB_GG_S_LINK_FAILURE)
		{
			//��ȡ����
			auto pLinkFailure = (CMD_GG_S_LinkFailure*)pData;

			//��ȡ����
			auto pVirtualLinkItem = m_VirtualLinkArray.GetAt(pLinkFailure->wServiceModule-1);
			if (pVirtualLinkItem != NULL)
			{
				//����֪ͨ
				pVirtualLinkItem->OnEventVirtualLink(pLinkFailure->lErrorCode);
			}

			return true;
		}

		//���ӶϿ�
		if (Command.wSubCmdID == SUB_GG_S_LINK_SHUTDOWN)
		{
			//��ȡ����
			auto pLinkShutdown = (CMD_GG_S_LinkShutdown*)pData;

			//��ȡ����
			auto pVirtualLinkItem = m_VirtualLinkArray.GetAt(pLinkShutdown->wServiceModule-1);
			if (pVirtualLinkItem != NULL)
			{
				//����״̬
				pVirtualLinkItem->m_bLinkStatus = false;
				
				//�ر�֪ͨ
				pVirtualLinkItem->OnEventVirtualShut();

				//���ñ�ʶ
				pVirtualLinkItem->m_wServiceID = 0;
			}

			return true;
		}

		return true;
	}

	//��ȡ����
	auto pVirtualLinkItem = m_VirtualLinkArray.GetAt(wServiceModule-1);
	if (pVirtualLinkItem != NULL && pVirtualLinkItem->m_bLinkStatus==true)
	{
		pVirtualLinkItem->OnEventVirtualRead(Command,pData,wDataSize);
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
	pVirtualLinkItem->m_bLinkStatus=false;
	pVirtualLinkItem->m_pVirtualLinkManager=this;

	//��������
	m_VirtualLinkArray[wServiceModule-1]=pVirtualLinkItem;

	return true;
}

//ɾ������
bool CVirtualLinkManager::DeleteVirtualLink(CVirtualLinkItem * pVirtualLinkItem)
{
	//�ر�����
	if (pVirtualLinkItem->m_bLinkStatus==true)
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

//���ýӿ�
VOID CVirtualLinkManager::SetVirtualManagerSink(IVirtualManagerSink* pIVirtualManagerSink)
{
	m_pIVirtualManagerSink = pIVirtualManagerSink;
}

//�ж�����
bool CVirtualLinkManager::PerformClose()
{
	//�ر�����
	ASSERT(m_TCPSocketModule.GetInterface() != NULL);
	if (m_TCPSocketModule.GetInterface() != NULL) m_TCPSocketModule->CloseSocket();

	return true;
}

//��������
bool CVirtualLinkManager::PerformConnect(LPCTSTR pszServerAddr, WORD wServerPort)
{
	//״̬Ч��
	ASSERT(pszServerAddr != NULL);
	if (pszServerAddr == NULL) return false;

	//���ñ���
	m_wServerPort = wServerPort;
	m_dwServerAddr = TranslateAddress(pszServerAddr);

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
	if (m_TCPSocketModule->Connect(m_dwServerAddr, m_wServerPort) != CONNECT_SUCCESS) return false;

	return true;
}

//���ͺ���
bool CVirtualLinkManager::SendSocketData(WORD wMainCmdID, WORD wSubCmdID)
{
	//״̬Ч��
	ASSERT(GetSocketStatus() == SOCKET_STATUS_CONNECT);
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
	ASSERT(GetSocketStatus() == SOCKET_STATUS_CONNECT);
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
		PerformClose();

		return;
	}

	__super::OnTimer(nIDEvent);
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