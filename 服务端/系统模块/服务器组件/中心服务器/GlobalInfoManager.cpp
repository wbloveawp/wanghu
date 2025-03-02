#include "StdAfx.h"
#include "GlobalInfoManager.h"
#include "ServiceAttribManager.h"

//////////////////////////////////////////////////////////////////////////
// 
//���캯��
CGlobalServiceInfo::CGlobalServiceInfo()
{
	m_pNextServiceInfoPtr = NULL;

	//���ô�С
	m_MapRouteItem.InitHashTable(11);
}

//��������
CGlobalServiceInfo::~CGlobalServiceInfo()
{

}

//�Ƴ�·��
VOID CGlobalServiceInfo::RemoveRouteItem(DWORD dwRouteID)
{
	m_MapRouteItem.RemoveKey(dwRouteID);
}

//���·��
VOID CGlobalServiceInfo::AppendRouteItem(tagRouteItem RouteItem[], WORD wItemCount)
{
	for (int i = 0; i < wItemCount; i++)
	{
		m_MapRouteItem[RouteItem[i].dwRouteID] = RouteItem[i];
	}
}

//����·��
tagRouteItem* CGlobalServiceInfo::QueryRouteItem(DWORD dwRouteID)
{
	tagRouteItem RouteItem = {};
	if (m_MapRouteItem.Lookup(dwRouteID, RouteItem))
	{
		return &m_MapRouteItem[dwRouteID];
	}

	return NULL;
}

//����·��
tagRouteItem* CGlobalServiceInfo::EnumRouteItem(POSITION& Position)
{
	//��������
	DWORD dwRouteID = 0L;
	tagRouteItem RouteItem = {};

	//��ȡ����
	if (Position == NULL) Position = m_MapRouteItem.GetStartPosition();
	if (Position != NULL) m_MapRouteItem.GetNextAssoc(Position, dwRouteID, RouteItem);

	return (dwRouteID!=0)?&m_MapRouteItem[dwRouteID]:NULL;	
}

//���ú���
VOID CGlobalServiceInfo::ResetData()
{
	__super::ResetData();

	m_pNextServiceInfoPtr = NULL;
	m_MapRouteItem.RemoveAll();
}


//////////////////////////////////////////////////////////////////////////
//���캯��
CGlobalServiceKind::CGlobalServiceKind()
{
	m_wServiceKind = 0;
	m_ServiceHash.CleanNode();
	m_pNextServiceKindPtr = NULL;
}

//��������
CGlobalServiceKind::~CGlobalServiceKind()
{
}

//�Ƴ�����
VOID CGlobalServiceKind::RemoveService(CGlobalServiceInfo* pGlobalServiceInfo)
{
	//·������
	if (m_wServiceMapKind == MAP_KIND_ROUTE)
	{
		//���ҷ���
		auto nIndex = SearchService(pGlobalServiceInfo->GetServiceID());
		if (nIndex != -1)
		{
			m_ArrayServiceInfo.RemoveAt(nIndex);
		}
	}

	//�Ƴ��ڵ�
	m_ServiceHash.RemoveNode(to_string(pGlobalServiceInfo->GetServiceID()));	
}

//��ӷ���
VOID CGlobalServiceKind::AppendService(CGlobalServiceInfo* pGlobalServiceInfo)
{
	//·������
	if (m_wServiceMapKind == MAP_KIND_ROUTE)
	{
		//���ҷ���
		auto nIndex = SearchService(pGlobalServiceInfo->GetServiceID());
		if (nIndex != -1)
		{
			m_ArrayServiceInfo[nIndex] = pGlobalServiceInfo;
		}
		else
		{
			m_ArrayServiceInfo.Add(pGlobalServiceInfo);
		}
	}

	//��ӽڵ�
	m_ServiceHash.AppendNode(CWHHashNode(to_string(pGlobalServiceInfo->GetServiceID()), 15));
}

//���ҷ���
WORD CGlobalServiceKind::LookupService(DWORD dwObjectID)
{
	//��ϣ����
	if (m_wServiceMapKind == MAP_KIND_HASH)
	{
		//��ϣ����
		CWHHashNode HashNode;
		if (m_ServiceHash.Lookup(to_string(dwObjectID), &HashNode))
		{
			int nServiceID = atoi(HashNode.GetIdentify().c_str());
			return nServiceID;
		}
	}

	//·������
	if (m_wServiceMapKind == MAP_KIND_ROUTE)
	{
		//�������
		tagRouteItem RouteItem = {};

		//����·��
		for (int i = 0; i < m_ArrayServiceInfo.GetCount(); i++)
		{
			if (m_ArrayServiceInfo[i]->m_MapRouteItem.Lookup(dwObjectID, RouteItem) == TRUE)
			{
				return m_ArrayServiceInfo[i]->GetServiceID();
			}
		}
	}	

	return INVALID_WORD;
}

//�������
WORD CGlobalServiceKind::AssignService(DWORD dwObjectID, WORD wServiceID)
{
	//��ϣ����
	if (m_wServiceMapKind == MAP_KIND_HASH)
	{
		return LookupService(dwObjectID);		
	}

	//·������
	if (m_wServiceMapKind == MAP_KIND_ROUTE)
	{
		if (wServiceID == INVALID_WORD)
		{
			//��ϣ����
			CWHHashNode HashNode;
			if (m_ServiceHash.Lookup(to_string(dwObjectID), &HashNode))
			{
				wServiceID = atoi(HashNode.GetIdentify().c_str());
			}
		}		

		//���ҷ���
		auto nIndex = SearchService(wServiceID);
		if (nIndex != -1)
		{
			//����·��
			tagRouteItem RouteItem = {};
			RouteItem.dwRouteID = dwObjectID;
			RouteItem.cbRouteState = ROUTE_STATE_NULL;

			//���·��
			m_ArrayServiceInfo[nIndex]->AppendRouteItem(&RouteItem, 1);

			return wServiceID;
		}
	}	

	return INVALID_WORD;
}

//��������
VOID CGlobalServiceKind::ResetData()
{
	//���ñ���
	m_wServiceKind = 0;
	m_ServiceHash.CleanNode();
	m_ArrayServiceInfo.RemoveAll();
	m_pNextServiceKindPtr = NULL;	
}

//���ҷ���
INT CGlobalServiceKind::SearchService(WORD wServiceID)
{
	for (int i = 0; i < m_ArrayServiceInfo.GetCount(); i++)
	{
		if (m_ArrayServiceInfo[i]->GetServiceID() == wServiceID)
		{
			return i;
		}
	}

	return -1;
}


//////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalUserInfo::CGlobalUserInfo()
{
	//�û�����
	m_dwUserID=0L;
	m_dwGameID=0L;
	m_cbGender=0;
	m_wFaceID=0;
	m_dwCustomID=0;
	m_szNickName[0]=0;

	//������Ϣ
	m_wGateID = 0;
	m_dwTokenID = 0;

	//������Ϣ
	m_pGlobalLogonInfo = NULL;
	m_pGlobalServerInfo = NULL;
	m_pGlobalMessageInfo = NULL;

	return;
}

//��������
CGlobalUserInfo::~CGlobalUserInfo()
{
}

//��������
WORD CGlobalUserInfo::BindServiceCount()
{
	//��������
	WORD wServiceCount = 0;

	//ͳ������
	if (m_pGlobalLogonInfo) wServiceCount++;
	if (m_pGlobalServerInfo) wServiceCount++;
	if (m_pGlobalMessageInfo) wServiceCount++;	 

	return wServiceCount;
}

//�󶨵�¼
VOID CGlobalUserInfo::BindLogonInfo(CGlobalLogonInfo* pGlobalLogonInfo)
{
	//����У��
	if (m_pGlobalLogonInfo == pGlobalLogonInfo)
	{
		return;
	}

	//������
	if (m_pGlobalLogonInfo != NULL)
	{
		UnBindLogonInfo(m_pGlobalLogonInfo);
	}

	//ӳ���û�
	pGlobalLogonInfo->m_MapUserID[m_dwUserID]= this;

	//���÷���
	m_pGlobalLogonInfo = pGlobalLogonInfo;
}

//�󶨷���
VOID CGlobalUserInfo::BindServerInfo(CGlobalServerInfo* pGlobalServerInfo)
{
	//����У��
	if (m_pGlobalServerInfo == pGlobalServerInfo)
	{
		return;
	}

	//������
	if (m_pGlobalServerInfo != NULL)
	{
		UnBindServerInfo(m_pGlobalServerInfo);
	}

	//ӳ���û�
	pGlobalServerInfo->m_MapUserID[m_dwUserID] = this;

	//���÷���
	m_pGlobalServerInfo = pGlobalServerInfo;
}

//����Ϣ
VOID CGlobalUserInfo::BindMessageInfo(CGlobalServiceInfo* pGlobalMessageInfo)
{
	//����У��
	if (m_pGlobalMessageInfo == pGlobalMessageInfo)
	{
		return;
	}

	//������
	if (m_pGlobalMessageInfo != NULL)
	{
		UnBindMessageInfo(m_pGlobalMessageInfo);
	}

	//���÷���
	m_pGlobalMessageInfo = pGlobalMessageInfo;
}

//����¼
VOID CGlobalUserInfo::UnBindLogonInfo(CGlobalLogonInfo* pGlobalLogonInfo)
{
	//����У��
	if (m_pGlobalLogonInfo != pGlobalLogonInfo)
	{
		return;
	}

	//�Ƴ��û�
	m_pGlobalLogonInfo->m_MapUserID.RemoveKey(m_dwUserID);

	//���÷���
	m_pGlobalLogonInfo = NULL;
}

//��󷿼�
VOID CGlobalUserInfo::UnBindServerInfo(CGlobalServerInfo* pGlobalServerInfo)
{
	//����У��
	if (m_pGlobalServerInfo != pGlobalServerInfo)
	{
		return;
	}

	//�Ƴ��û�
	m_pGlobalServerInfo->m_MapUserID.RemoveKey(m_dwUserID);

	//���÷���
	m_pGlobalServerInfo = NULL;
}


//����Ϣ
VOID CGlobalUserInfo::UnBindMessageInfo(CGlobalServiceInfo* pGlobalMessageInfo)
{
	//����У��
	if (m_pGlobalMessageInfo != pGlobalMessageInfo)
	{
		return;
	}

	//���÷���
	m_pGlobalMessageInfo = NULL;
}

//�����û�
VOID CGlobalUserInfo::ResetData()
{
	__super::ResetData();

	//������
	if (m_pGlobalLogonInfo!=NULL) UnBindLogonInfo(m_pGlobalLogonInfo);
	if (m_pGlobalServerInfo!=NULL) UnBindServerInfo(m_pGlobalServerInfo);
	if (m_pGlobalMessageInfo!=NULL) UnBindMessageInfo(m_pGlobalMessageInfo);

	//������Ϣ
	m_pGlobalLogonInfo = NULL;
	m_pGlobalServerInfo = NULL;
	m_pGlobalMessageInfo = NULL;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalLogonInfo::CGlobalLogonInfo()
{
}

//��������
CGlobalLogonInfo::~CGlobalLogonInfo()
{
}

//���ú���
VOID CGlobalLogonInfo::ResetData()
{
	__super::ResetData();

	//�Ƴ��û�
	m_MapUserID.RemoveAll();
}

//////////////////////////////////////////////////////////////////////////
//���캯��
CGlobalGameStock::CGlobalGameStock()
{
	m_wKindID = 0;
	m_lJackpotScore = 0;
}

//��������
CGlobalGameStock::~CGlobalGameStock()
{
}

//���ú���
VOID CGlobalGameStock::ResetData()
{
	m_wKindID = 0;
	m_lJackpotScore = 0;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalMonitorInfo::CGlobalMonitorInfo()
{
	m_StockGameStock.InitHashTable(33);
}

//��������
CGlobalMonitorInfo::~CGlobalMonitorInfo()
{
}

//���ú���
VOID CGlobalMonitorInfo::ResetData()
{
	__super::ResetData();

	//�Ƴ�����
	m_StockGameStock.RemoveAllItem();
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalServerInfo::CGlobalServerInfo()
{
	//���ñ���
	ZeroMemory(&m_GameServer,sizeof(m_GameServer));

	//��������
	m_MapUserID.InitHashTable(PRIME_SERVER_USER);

	return;
}

//��������
CGlobalServerInfo::~CGlobalServerInfo()
{
}

//Ѱ���û�
CGlobalUserInfo * CGlobalServerInfo::SearchUserInfo(DWORD dwUserID)
{
	//�����û�
	CGlobalUserInfo * pGlobalUserInfo=NULL;
	m_MapUserID.Lookup(dwUserID,pGlobalUserInfo);

	return pGlobalUserInfo;
}

//��������
VOID CGlobalServerInfo::ResetData()
{
	__super::ResetData();

	//�Ƴ��û�
	m_MapUserID.RemoveAll();

	//���ñ���
	ZeroMemory(&m_GameServer, sizeof(m_GameServer));
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalAgentInfo::CGlobalAgentInfo()
{
	//���ñ���
	m_pNextAgentItemPtr = NULL;
	ZeroMemory(&m_AgentItem, sizeof(m_AgentItem));	
}

//��������
CGlobalAgentInfo::~CGlobalAgentInfo()
{
}

//ɾ���˿�
bool CGlobalAgentInfo::RemoveServicePort(WORD wServicePort)
{
	//�ظ��ж�
	for (int i = 0; i < m_PortPairArray.GetCount(); i++)
	{
		if (m_PortPairArray[i].wForwardPort == wServicePort)
		{
			m_PortPairArray.RemoveAt(i);

			return true;
		}
	}

	return false;
}

//����˿�
bool CGlobalAgentInfo::InsertServicePort(tagPortPair * pPortPair)
{
	//�ظ��ж�
	for (int i = 0; i < m_PortPairArray.GetCount(); i++)
	{
		if (m_PortPairArray[i].wForwardPort == pPortPair->wForwardPort)
		{
			return false;
		}
	}

	//�������
	m_PortPairArray.Add(*pPortPair);

	return true;
}

//��������
VOID CGlobalAgentInfo::ResetData()
{
	__super::ResetData();

	//�Ƴ��û�
	m_PortPairArray.RemoveAll();

	//���ñ���
	m_pNextAgentItemPtr = NULL;
	ZeroMemory(&m_AgentItem, sizeof(m_AgentItem));
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalInfoManager::CGlobalInfoManager()
{
	//�洢����
	m_pGlobalUserInfo = NULL;
	m_pGlobalServiceKind = NULL;
	ZeroMemory(m_pGlobalServiceInfo, sizeof(m_pGlobalServiceInfo));

	//��������
	m_MapUserID.InitHashTable(PRIME_PLATFORM_USER);

	//��ʼ��
	for (INT_PTR i = 0; i < CountArray(m_MapServiceID); i++)
	{
		m_MapServiceID[i].InitHashTable(PRIME_CUSTOM);
	}

	//��ʼ��
	for (INT_PTR i = 0; i < CountArray(m_MapServiceKind); i++)
	{
		m_MapServiceKind[i].InitHashTable(PRIME_CUSTOM);
	}

	return;
}

//��������
CGlobalInfoManager::~CGlobalInfoManager()
{
	//ɾ���û�
	DWORD dwUserID = 0L;
	CGlobalUserInfo * pGlobalUserInfo = NULL;
	POSITION Position = m_MapUserID.GetStartPosition();
	while (Position != NULL)
	{
		m_MapUserID.GetNextAssoc(Position, dwUserID, pGlobalUserInfo);
		SafeDelete(pGlobalUserInfo);
	}
	m_MapUserID.RemoveAll();

	//ɾ������
	WORD wServiceID = 0;
	CGlobalServiceInfo* pGlobalServiceInfo = NULL;
	for (INT_PTR i = 0; i < CountArray(m_MapServiceID); i++)
	{
		Position = m_MapServiceID[i].GetStartPosition();
		while (Position != NULL)
		{
			m_MapServiceID[i].GetNextAssoc(Position, wServiceID, pGlobalServiceInfo);
			SafeDelete(pGlobalServiceInfo);
		}
		m_MapServiceID[i].RemoveAll();
	}

	//ɾ������
	WORD wServiceKind = 0;
	CGlobalServiceKind* pGlobalServiceKind = NULL;
	for (INT_PTR i = 0; i < CountArray(m_MapServiceKind); i++)
	{
		Position = m_MapServiceKind[i].GetStartPosition();
		while (Position != NULL)
		{
			m_MapServiceKind[i].GetNextAssoc(Position, wServiceKind, pGlobalServiceKind);
			SafeDelete(pGlobalServiceKind);
		}
		m_MapServiceKind[i].RemoveAll();
	}

	//ɾ���û�
	while (m_pGlobalUserInfo != NULL)
	{
		pGlobalUserInfo = m_pGlobalUserInfo;
		m_pGlobalUserInfo = m_pGlobalUserInfo->m_pNextUserInfoPtr;
		SafeDelete(pGlobalUserInfo);
	}

	//ɾ������
	while (m_pGlobalServiceKind != NULL)
	{
		pGlobalServiceKind = m_pGlobalServiceKind;
		m_pGlobalServiceKind = m_pGlobalServiceKind->m_pNextServiceKindPtr;
		SafeDelete(pGlobalServiceKind);
	}

	//ɾ������
	for (int i = 0; i < CountArray(m_pGlobalServiceInfo); i++)
	{
		while (m_pGlobalServiceInfo[i] != NULL)
		{
			pGlobalServiceInfo = m_pGlobalServiceInfo[i];
			m_pGlobalServiceInfo[i] = m_pGlobalServiceInfo[i]->m_pNextServiceInfoPtr;
			SafeDelete(pGlobalServiceInfo);
		}
	}

	return;
}

//��������
VOID CGlobalInfoManager::ResetData()
{
	//ɾ���û�
	DWORD dwUserID = 0L;
	CGlobalUserInfo* pGlobalUserInfo = NULL;
	POSITION Position = m_MapUserID.GetStartPosition();

	//ɾ���û�
	while (Position != NULL)
	{
		m_MapUserID.GetNextAssoc(Position, dwUserID, pGlobalUserInfo);
		if (pGlobalUserInfo != NULL) FreeGlobalUserInfo(pGlobalUserInfo);
	}

	//ɾ������
	m_MapUserID.RemoveAll();

	//ɾ������
	WORD wServiceID = 0;
	CGlobalServiceInfo* pGlobalServiceInfo = NULL;
	for (INT_PTR i = 0; i < CountArray(m_MapServiceID); i++)
	{
		Position = m_MapServiceID[i].GetStartPosition();
		while (Position != NULL)
		{
			m_MapServiceID[i].GetNextAssoc(Position, wServiceID, pGlobalServiceInfo);
			if (pGlobalServiceInfo != NULL) FreeGlobalServiceInfo(pGlobalServiceInfo);
		}
		m_MapServiceID[i].RemoveAll();
	}

	//ɾ������
	WORD wServiceKind = 0;
	CGlobalServiceKind* pGlobalServiceKind = NULL;
	for (INT_PTR i = 0; i < CountArray(m_MapServiceKind); i++)
	{
		Position = m_MapServiceKind[i].GetStartPosition();
		while (Position != NULL)
		{
			m_MapServiceKind[i].GetNextAssoc(Position, wServiceKind, pGlobalServiceKind);
			if (pGlobalServiceKind != NULL) FreeGlobalServiceKind(pGlobalServiceKind);
		}
		m_MapServiceKind[i].RemoveAll();
	}

	return;
}

//������Ŀ
DWORD CGlobalInfoManager::GetServiceItemCount(WORD wServiceModule)
{
	//��������
	WORD wServiceIndex = wServiceModule - 1;
	if (wServiceIndex >= CountArray(m_MapServiceID)) return 0;

	return (DWORD)m_MapServiceID[wServiceIndex].GetCount();
}

//�����û�
CGlobalUserInfo* CGlobalInfoManager::ActiveUserInfo(tagGlobalUserInfo& GlobalUserInfo)
{
	//�����û�
	auto pGlobalUserInfo = CreateGlobalUserInfo();
	if (pGlobalUserInfo == NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//�����û�
	pGlobalUserInfo->m_dwUserID = GlobalUserInfo.dwUserID;
	pGlobalUserInfo->m_dwGameID = GlobalUserInfo.dwGameID;
	StringCchCopy(pGlobalUserInfo->m_szNickName, CountArray(pGlobalUserInfo->m_szNickName), GlobalUserInfo.szNickName);

	//�û�����
	pGlobalUserInfo->m_cbGender = GlobalUserInfo.cbGender;
	pGlobalUserInfo->m_wFaceID = GlobalUserInfo.wFaceID;
	pGlobalUserInfo->m_dwCustomID = GlobalUserInfo.dwCustomID;

	//״̬��Ϣ
	pGlobalUserInfo->m_cbShutUpStatus = GlobalUserInfo.cbShutUpStatus;
	pGlobalUserInfo->m_dwShutUpOverTime = GlobalUserInfo.dwShutUpOverTime;

	//�û�״̬
	pGlobalUserInfo->m_cbIMUserStatus = GlobalUserInfo.cbIMUserStatus;
	pGlobalUserInfo->m_cbCBUserStatus = GlobalUserInfo.cbCBUserStatus;

	//������Ϣ
	pGlobalUserInfo->m_wGateID = GlobalUserInfo.SocketInfo.wGateID;
	pGlobalUserInfo->m_dwTokenID = GlobalUserInfo.SocketInfo.dwTokenID;

	//��������
	m_MapUserID[GlobalUserInfo.dwUserID] = pGlobalUserInfo;

	return pGlobalUserInfo;
}

//ɾ���û�
bool CGlobalInfoManager::DeleteUserInfo(DWORD dwUserID)
{
	//Ѱ���û�
	CGlobalUserInfo* pGlobalUserInfo = NULL;
	if (m_MapUserID.Lookup(dwUserID, pGlobalUserInfo) == FALSE)
	{
		ASSERT(FALSE);
		return false;
	}

	//�Ƴ��û�
	m_MapUserID.RemoveKey(dwUserID);

	//���ն���
	FreeGlobalUserInfo(pGlobalUserInfo);

	return true;
}

//�������
bool CGlobalInfoManager::ActiveAgentInfo(DWORD dwSocketID, tagServiceItem & ServiceItem, tagAgentItem & AgentItem)
{
	//��������
	WORD wServiceIndex = SERVICE_MODULE_AGENT-1;
	if (wServiceIndex >= CountArray(m_MapServiceID)) return false;

	//Ѱ�Ҵ���
	CGlobalServiceInfo* pGlobalServiceInfo = NULL;
	if (m_MapServiceID[wServiceIndex].Lookup(ServiceItem.wServiceID, pGlobalServiceInfo) == TRUE)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	CGlobalAgentInfo * pGlobalAgentInfo = (CGlobalAgentInfo *)CreateGlobalServiceInfo(SERVICE_MODULE_AGENT);
	if (pGlobalAgentInfo == NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//���ô���
	pGlobalAgentInfo->m_dwSocketID = dwSocketID;
	pGlobalAgentInfo->m_AgentItem = AgentItem;
	pGlobalAgentInfo->m_ServiceItem = ServiceItem;

	//��������
	m_MapServiceID[wServiceIndex][ServiceItem.wServiceID] = pGlobalAgentInfo;

	return true;
}

//ɾ������
bool CGlobalInfoManager::DeleteAgentInfo(WORD wAgentID)
{
	//��������
	WORD wServiceIndex = SERVICE_MODULE_AGENT-1;
	if (wServiceIndex >= CountArray(m_MapServiceID)) return false;

	//Ѱ�ҷ���
	CGlobalServiceInfo * pGlobalServiceInfo = NULL;
	if (m_MapServiceID[wServiceIndex].Lookup(wAgentID, pGlobalServiceInfo) == FALSE)
	{
		ASSERT(FALSE);
		return false;
	}

	//�ͷŴ���
	m_MapServiceID[wServiceIndex].RemoveKey(wAgentID);
	FreeGlobalServiceInfo(pGlobalServiceInfo);

	return true;
}

//�����
bool CGlobalInfoManager::ActiveServerInfo(DWORD dwSocketID, tagServiceItem & ServiceItem, tagGameServer & GameServer)
{
	//��������
	WORD wServiceIndex = SERVICE_MODULE_GAME-1;
	if (wServiceIndex >= CountArray(m_MapServiceID)) return false;

	//Ѱ�ҷ���
	CGlobalServiceInfo * pGlobalServiceInfo = NULL;
	if (m_MapServiceID[wServiceIndex].Lookup(ServiceItem.wServiceID, pGlobalServiceInfo)==TRUE)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	CGlobalServerInfo * pGlobalServerInfo=(CGlobalServerInfo *)CreateGlobalServiceInfo(SERVICE_MODULE_GAME);
	if (pGlobalServerInfo==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//���÷���
	pGlobalServerInfo->m_dwSocketID= dwSocketID;
	pGlobalServerInfo->m_GameServer=GameServer;
	pGlobalServerInfo->m_ServiceItem=ServiceItem;

	//��������
	m_MapServiceID[wServiceIndex][ServiceItem.wServiceID]=pGlobalServerInfo;

	return true;
}

//ɾ������
bool CGlobalInfoManager::DeleteServerInfo(WORD wServerID)
{
	//��������
	WORD wServiceIndex = SERVICE_MODULE_GAME-1;
	if (wServiceIndex >= CountArray(m_MapServiceID)) return false;

	//Ѱ�ҷ���
	CGlobalServiceInfo * pGlobalServiceInfo=NULL;
	if (m_MapServiceID[wServiceIndex].Lookup(wServerID,pGlobalServiceInfo)==FALSE)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	DWORD dwUserKey=0L;
	CGlobalUserInfo * pGlobalUserInfo=NULL;
	CGlobalServerInfo * pGlobalServerInfo = (CGlobalServerInfo *)pGlobalServiceInfo;
	POSITION Position=pGlobalServerInfo->m_MapUserID.GetStartPosition();

	//ɾ���û�
	while (Position!=NULL)
	{
		//��ȡ����
		pGlobalServerInfo->m_MapUserID.GetNextAssoc(Position,dwUserKey,pGlobalUserInfo);

		//�����
		if (pGlobalUserInfo != NULL)
		{
			pGlobalUserInfo->UnBindServerInfo(pGlobalServerInfo);
			if (pGlobalUserInfo->BindServiceCount() == 0)
			{
				m_MapUserID.RemoveKey(pGlobalUserInfo->GetUserID());
				FreeGlobalUserInfo(pGlobalUserInfo);
			}
		}		
	}

	//�ͷŷ���
	m_MapServiceID[wServiceIndex].RemoveKey(wServerID);
	FreeGlobalServiceInfo(pGlobalServiceInfo);

	return true;
}

//ɾ����¼
bool CGlobalInfoManager::DeleteLogonInfo(WORD wServiceID)
{
	//��������
	WORD wServiceIndex = SERVICE_MODULE_LOGON - 1;
	if (wServiceIndex >= CountArray(m_MapServiceID)) return false;

	//Ѱ�ҷ���
	CGlobalServiceInfo* pGlobalServiceInfo = NULL;
	if (m_MapServiceID[wServiceIndex].Lookup(wServiceID, pGlobalServiceInfo) == FALSE)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	DWORD dwUserKey = 0L;
	CGlobalUserInfo* pGlobalUserInfo = NULL;
	CGlobalLogonInfo* pGlobalLogonInfo = (CGlobalLogonInfo*)pGlobalServiceInfo;
	POSITION Position = pGlobalLogonInfo->m_MapUserID.GetStartPosition();

	//ɾ���û�
	while (Position != NULL)
	{
		//��ȡ����
		pGlobalLogonInfo->m_MapUserID.GetNextAssoc(Position, dwUserKey, pGlobalUserInfo);

		//�����
		if (pGlobalUserInfo != NULL)
		{
			pGlobalUserInfo->UnBindLogonInfo(pGlobalLogonInfo);
			if (pGlobalUserInfo->BindServiceCount() == 0)
			{
				m_MapUserID.RemoveKey(pGlobalUserInfo->GetUserID());
				FreeGlobalUserInfo(pGlobalUserInfo);
			}
		}
	}

	//�ͷŷ���
	m_MapServiceID[wServiceIndex].RemoveKey(wServiceID);
	FreeGlobalServiceInfo(pGlobalServiceInfo);

	return true;
}

//�������
bool CGlobalInfoManager::ActiveServiceInfo(DWORD dwSocketID, tagServiceItem & ServiceItem)
{
	//��������
	WORD wServiceIndex = ServiceItem.wServiceModule-1;
	if (wServiceIndex >= CountArray(m_MapServiceID)) return false;

	//Ѱ�ҷ���
	CGlobalServiceInfo * pGlobalServiceInfo = NULL;
	if (m_MapServiceID[wServiceIndex].Lookup(ServiceItem.wServiceID, pGlobalServiceInfo) == TRUE)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	pGlobalServiceInfo = CreateGlobalServiceInfo(ServiceItem.wServiceModule);
	if (pGlobalServiceInfo == NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//���÷���
	pGlobalServiceInfo->m_dwSocketID = dwSocketID;
	pGlobalServiceInfo->m_ServiceItem = ServiceItem;

	//��������
	m_MapServiceID[wServiceIndex][ServiceItem.wServiceID] = pGlobalServiceInfo;

	//��������
	CGlobalServiceKind* pGlobalServiceKind = NULL;
	if (m_MapServiceKind[wServiceIndex].Lookup(ServiceItem.wServiceKindID, pGlobalServiceKind) == FALSE)
	{
		pGlobalServiceKind = CreateGlobalServiceKind();
		if (pGlobalServiceKind != NULL)
		{
			auto pServieAttribute = CServiceAttribManager::SearchServiceAttrib(ServiceItem.wServiceModule);
			if (pServieAttribute != NULL)
			{
				pGlobalServiceKind->m_wServiceKind = ServiceItem.wServiceKindID;
				pGlobalServiceKind->m_wServiceMapKind = pServieAttribute->wServiceMapKind;
				m_MapServiceKind[wServiceIndex][ServiceItem.wServiceKindID] = pGlobalServiceKind;
			}			
		}
	}

	//��ӷ���
	if (pGlobalServiceKind != NULL)
	{
		pGlobalServiceKind->AppendService(pGlobalServiceInfo);
	}	

	return true;
}

//ɾ������
bool CGlobalInfoManager::DeleteServiceInfo(WORD wServiceModule, WORD wServiceID)
{
	//��������
	WORD wServiceIndex = wServiceModule-1;
	if (wServiceIndex >= CountArray(m_MapServiceID)) return 0;

	//Ѱ�ҷ���
	CGlobalServiceInfo * pGlobalServiceInfo = NULL;
	if (m_MapServiceID[wServiceIndex].Lookup(wServiceID, pGlobalServiceInfo) == FALSE)
	{
		ASSERT(FALSE);
		return false;
	}

	//������Ϣ
	WORD wServiceKind = pGlobalServiceInfo->m_ServiceItem.wServiceKindID;

	//��������
	CGlobalServiceKind* pGlobalServiceKind = NULL;
	if (m_MapServiceKind[wServiceIndex].Lookup(wServiceKind, pGlobalServiceKind) == TRUE)
	{
		pGlobalServiceKind->RemoveService(pGlobalServiceInfo);
		if (pGlobalServiceKind->GetServiceCount() == 0)
		{
			m_MapServiceKind[wServiceIndex].RemoveKey(wServiceKind);
		}
	}

	//�ͷŷ���
	m_MapServiceID[wServiceIndex].RemoveKey(wServiceID);
	FreeGlobalServiceInfo(pGlobalServiceInfo);

	return true;
}

//Ѱ���û�
CGlobalUserInfo * CGlobalInfoManager::SearchUserInfo(DWORD dwUserID)
{
	//Ѱ���û�
	CGlobalUserInfo * pGlobalUserInfo=NULL;
	m_MapUserID.Lookup(dwUserID,pGlobalUserInfo);

	return pGlobalUserInfo;
}

//Ѱ�ҷ���
CGlobalServiceInfo * CGlobalInfoManager::SearchServiceInfo(WORD wServiceModule, WORD wServiceID)
{
	//��������
	WORD wServiceIndex = wServiceModule-1;
	if (wServiceIndex >= CountArray(m_MapServiceID)) return NULL;

	//Ѱ���߼�
	CGlobalServiceInfo * pGlobalServiceInfo = NULL;
	m_MapServiceID[wServiceIndex].Lookup(wServiceID, pGlobalServiceInfo);

	return pGlobalServiceInfo;
}

//ö���û�
CGlobalUserInfo * CGlobalInfoManager::EnumUserInfo(POSITION & Position)
{
	//��������
	DWORD dwUserID=0L;
	CGlobalUserInfo * pGlobalUserInfo=NULL;

	//��ȡ����
	if (Position==NULL) Position = m_MapUserID.GetStartPosition();
	if (Position!=NULL)	m_MapUserID.GetNextAssoc(Position,dwUserID,pGlobalUserInfo);

	return pGlobalUserInfo;
}

//ö�ٷ���
CGlobalServiceInfo * CGlobalInfoManager::EnumServiceInfo(WORD wServiceModule, POSITION & Position)
{
	//��������
	WORD wServiceIndex = wServiceModule-1;
	if (wServiceIndex >= CountArray(m_MapServiceID)) return NULL;

	//��������
	WORD wServiceID = 0L;
	CGlobalServiceInfo * pGlobalServiceInfo = NULL;

	//��ȡ����
	if (Position == NULL) Position = m_MapServiceID[wServiceIndex].GetStartPosition();
	if (Position != NULL) m_MapServiceID[wServiceIndex].GetNextAssoc(Position, wServiceID, pGlobalServiceInfo);

	return pGlobalServiceInfo;
}

//ӳ�����
CGlobalServiceInfo* CGlobalInfoManager::LookupService(WORD wServiceModule, WORD wServiceKind, DWORD dwObjectID)
{
	//��������
	WORD wServiceIndex = wServiceModule-1;
	if (wServiceIndex >= CountArray(m_MapServiceKind)) return NULL;

	//���ҽڵ�
	CGlobalServiceKind* pGlobalServiceKind = NULL;
	if (m_MapServiceKind[wServiceIndex].Lookup(wServiceKind, pGlobalServiceKind)==TRUE)
	{
		//��ȡ����
		auto nServiceID = pGlobalServiceKind->LookupService(dwObjectID);

		//���ҷ���
		CGlobalServiceInfo* pGlobalServiceInfo = NULL;
		m_MapServiceID[wServiceIndex].Lookup(nServiceID, pGlobalServiceInfo);

		return pGlobalServiceInfo;
	}

	return NULL;
}

//���ɷ���
CGlobalServiceInfo* CGlobalInfoManager::AssignService(WORD wServiceModule, WORD wServiceKind, DWORD dwObjectID, WORD wServiceID)
{
	//��������
	WORD wServiceIndex = wServiceModule - 1;
	if (wServiceIndex >= CountArray(m_MapServiceKind)) return NULL;

	//���ҽڵ�
	CGlobalServiceKind* pGlobalServiceKind = NULL;
	if (m_MapServiceKind[wServiceIndex].Lookup(wServiceKind, pGlobalServiceKind) == TRUE)
	{
		//��ȡ����
		auto nServiceID = pGlobalServiceKind->AssignService(dwObjectID, wServiceID);

		//���ҷ���
		CGlobalServiceInfo* pGlobalServiceInfo = NULL;
		m_MapServiceID[wServiceIndex].Lookup(nServiceID, pGlobalServiceInfo);

		return pGlobalServiceInfo;
	}

	return NULL;
}

//�����û�
CGlobalUserInfo * CGlobalInfoManager::CreateGlobalUserInfo()
{
	//ʹ�ô洢
	if (m_pGlobalUserInfo!=NULL)
	{
		CGlobalUserInfo * pGlobalUserInfo=m_pGlobalUserInfo;
		m_pGlobalUserInfo=m_pGlobalUserInfo->m_pNextUserInfoPtr;
		pGlobalUserInfo->m_pNextUserInfoPtr=NULL;
		return pGlobalUserInfo;
	}

	//��������
	try 
	{ 
		CGlobalUserInfo * pGlobalUserInfo=new CGlobalUserInfo;
		return pGlobalUserInfo;
	}
	catch (...) {}

	return NULL;
}

//��������
CGlobalServiceKind* CGlobalInfoManager::CreateGlobalServiceKind()
{
	//ʹ�ô洢
	if (m_pGlobalServiceKind != NULL)
	{
		CGlobalServiceKind* pGlobalServiceKind = m_pGlobalServiceKind;
		m_pGlobalServiceKind = m_pGlobalServiceKind->m_pNextServiceKindPtr;
		pGlobalServiceKind->m_pNextServiceKindPtr = NULL;
		return pGlobalServiceKind;
	}

	//��������
	try
	{
		CGlobalServiceKind* pGlobalServiceKind = new CGlobalServiceKind;
		return pGlobalServiceKind;
	}
	catch (...) {}

	return NULL;
}

//��������
CGlobalServiceInfo * CGlobalInfoManager::CreateGlobalServiceInfo(WORD wServiceModule)
{
	//��������
	WORD wServiceIndex = wServiceModule-1;
	if (wServiceIndex >= CountArray(m_pGlobalServiceInfo)) return NULL;

	//ʹ�ô洢
	if (m_pGlobalServiceInfo[wServiceIndex] != NULL)
	{
		CGlobalServiceInfo * pGlobalServiceInfo = m_pGlobalServiceInfo[wServiceIndex];
		m_pGlobalServiceInfo[wServiceIndex] = m_pGlobalServiceInfo[wServiceIndex]->m_pNextServiceInfoPtr;
		pGlobalServiceInfo->m_pNextServiceInfoPtr = NULL;
		return pGlobalServiceInfo;
	}

	//��������
	try
	{
		CGlobalServiceInfo * pGlobalServiceInfo = NULL;
		
		switch (wServiceModule)
		{
		case SERVICE_MODULE_LOGON:
		{
			pGlobalServiceInfo = new CGlobalLogonInfo;
			break;
		}
		case SERVICE_MODULE_GAME:
		{
			pGlobalServiceInfo = new CGlobalServerInfo;
			break;
		}
		case SERVICE_MODULE_AGENT:
		{
			pGlobalServiceInfo = new CGlobalAgentInfo;
			break;
		}	
		case SERVICE_MODULE_MONITOR:
		{
			pGlobalServiceInfo = new CGlobalMonitorInfo;
			break;
		}
		default:
		{
			pGlobalServiceInfo = new CGlobalServiceInfo;
			break;
		}
		}

		return pGlobalServiceInfo;
	}
	catch (...) {}

	return NULL;
}

//�ͷ��û�
bool CGlobalInfoManager::FreeGlobalUserInfo(CGlobalUserInfo * pGlobalUserInfo)
{
	//Ч�����
	ASSERT(pGlobalUserInfo!=NULL);
	if (pGlobalUserInfo==NULL) return false;

	//���ö���
	pGlobalUserInfo->ResetData();

	//����洢
	pGlobalUserInfo->m_pNextUserInfoPtr=m_pGlobalUserInfo;
	m_pGlobalUserInfo=pGlobalUserInfo;

	return true;
}

//�ͷ�����
bool CGlobalInfoManager::FreeGlobalServiceKind(CGlobalServiceKind* pGlobalServiceKind)
{
	//Ч�����
	ASSERT(pGlobalServiceKind != NULL);
	if (pGlobalServiceKind == NULL) return false;

	//���ö���
	pGlobalServiceKind->ResetData();

	//����洢
	pGlobalServiceKind->m_pNextServiceKindPtr = m_pGlobalServiceKind;
	m_pGlobalServiceKind = pGlobalServiceKind;

	return true;
}

//�ͷŷ���
bool CGlobalInfoManager::FreeGlobalServiceInfo(CGlobalServiceInfo *pGlobalServiceInfo)
{
	//Ч�����
	ASSERT (pGlobalServiceInfo != NULL);
	if (pGlobalServiceInfo == NULL) return false;

	//����ģ��
	WORD wServiceModule = pGlobalServiceInfo->GetServiceModule();

	//��������
	WORD wServiceIndex = wServiceModule-1;
	if (wServiceIndex >= CountArray(m_pGlobalServiceInfo)) return false;

	//��������
	pGlobalServiceInfo->ResetData();
	
	//����洢
	pGlobalServiceInfo->m_pNextServiceInfoPtr = m_pGlobalServiceInfo[wServiceIndex];
	m_pGlobalServiceInfo[wServiceIndex] = pGlobalServiceInfo;

	return true;
}

//////////////////////////////////////////////////////////////////////////
