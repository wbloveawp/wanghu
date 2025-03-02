#include "StdAfx.h"
#include "GlobalInfoManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//���캯��
CGlobalServiceInfo::CGlobalServiceInfo()
{
	//��������
	m_wDataSize = 0;
	m_pITCPSocketService = NULL;
	ZeroMemory(m_cbExtendData, sizeof(m_cbExtendData));

	//���ô�С
	m_MapRouteItem.InitHashTable(11);
}

//��������
CGlobalServiceInfo::~CGlobalServiceInfo()
{
	ResetData();
}

//�Ƴ�·��
VOID CGlobalServiceInfo::RemoveRouteItem()
{
	m_MapRouteItem.RemoveAll();
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

//��������
VOID CGlobalServiceInfo::ResetData()
{
	//���ñ���
	m_wDataSize = 0;
	m_pITCPSocketService = NULL;
	ZeroMemory(m_cbExtendData,sizeof(m_cbExtendData));

	//�Ƴ�����
	m_MapRouteItem.RemoveAll();
}

//////////////////////////////////////////////////////////////////////////
//���캯��
CGlobalServiceKind::CGlobalServiceKind()
{
	m_wServiceKind = 0;
	m_wServiceMapKind = 0;
	m_ServiceHash.CleanNode();	
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

	//��ϣ����
	if (m_wServiceMapKind == MAP_KIND_HASH)
	{
		//�Ƴ��ڵ�
		m_ServiceHash.RemoveNode(to_string(pGlobalServiceInfo->GetServiceID()));
	}		
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

	//��ϣ����
	if (m_wServiceMapKind == MAP_KIND_HASH)
	{
		//��ӽڵ�
		m_ServiceHash.AppendNode(CWHHashNode(to_string(pGlobalServiceInfo->GetServiceID()), 15));
	}	
}

//���ҷ���
WORD CGlobalServiceKind::LookupService(DWORD dwObjectID)
{
	//��ϣ����
	if (m_wServiceMapKind == MAP_KIND_HASH)
	{
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

//��������
VOID CGlobalServiceKind::ResetData()
{
	m_wServiceKind = 0;
	m_ServiceHash.CleanNode();	
	m_ArrayServiceInfo.RemoveAll();
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
CGlobalInfoManager::CGlobalInfoManager()
{
	//���ñ���
	m_pIAttemperEngine = NULL;	
	m_pServiceAttribute = NULL;

	//��������
	for (INT_PTR i = 0; i < CountArray(m_StockServiceInfo); i++)
	{
		m_StockServiceInfo[i].InitHashTable(PRIME_CUSTOM);
	}

	//��������
	for (INT_PTR i = 0; i < CountArray(m_StockServiceKind); i++)
	{
		m_StockServiceKind[i].InitHashTable(3);
	}
}

//��������
CGlobalInfoManager::~CGlobalInfoManager()
{
	//ɾ������
	for (INT_PTR i = 0; i < CountArray(m_StockServiceInfo); i++)
	{
		m_StockServiceInfo[i].DeleteAllItem();
	}

	//ɾ������
	for (INT_PTR i = 0; i < CountArray(m_StockServiceKind); i++)
	{
		m_StockServiceKind[i].DeleteAllItem();
	}

	//�ͷŶ���
	m_SocketServiceBuffer.Append(m_SocketServiceActive);
	for (auto i = 0; i < m_SocketServiceBuffer.GetCount(); i++)
	{
		SafeDelete(m_SocketServiceBuffer[i]);
	}

	//�Ƴ�Ԫ��	
	m_SocketServiceActive.RemoveAll();
	m_SocketServiceBuffer.RemoveAll();

	return;
}

//��������
VOID CGlobalInfoManager::ResetData()
{
	//�Ƴ�����
	for (INT_PTR i = 0; i < CountArray(m_StockServiceInfo); i++)
	{
		//��������
		auto pMoveStockItem = m_StockServiceInfo[i].GetHeadStockItem();
		while (pMoveStockItem != NULL)
		{
			pMoveStockItem->ResetData();

			auto pCurrStockItem = pMoveStockItem;
			pMoveStockItem = m_StockServiceInfo[i].GetNextStockItem(pCurrStockItem);
		}

		//�Ƴ�����
		m_StockServiceInfo[i].RemoveAllItem();
	}

	//�Ƴ�����
	for (INT_PTR i = 0; i < CountArray(m_StockServiceKind); i++)
	{
		//��������
		auto pMoveStockItem = m_StockServiceKind[i].GetHeadStockItem();
		while (pMoveStockItem != NULL)
		{
			pMoveStockItem->ResetData();

			auto pCurrStockItem = pMoveStockItem;
			pMoveStockItem = m_StockServiceKind[i].GetNextStockItem(pCurrStockItem);						
		}

		//�Ƴ�����
		m_StockServiceKind[i].RemoveAllItem();
	}	

	//ֹͣ����
	for (INT_PTR i = 0; i < m_SocketServiceActive.GetCount(); i++)
	{
		if (m_SocketServiceActive[i]->GetInterface())
		{
			m_SocketServiceActive[i]->GetInterface()->ConcludeService();
		}
	}

	//�Ƴ�Ԫ��
	m_SocketServiceBuffer.Append(m_SocketServiceActive);
	m_SocketServiceActive.RemoveAll();

	return;
}

//������Ŀ
DWORD CGlobalInfoManager::GetServiceCount(WORD wServiceIndex)
{
	//��ȡ����
	ASSERT(wServiceIndex < CountArray(m_StockServiceInfo));
	if (wServiceIndex >= CountArray(m_StockServiceInfo)) return 0;

	return (DWORD)m_StockServiceInfo[wServiceIndex].GetItemCount(); 
}

//�������
CGlobalServiceInfo * CGlobalInfoManager::ActiveServiceInfo(WORD wServiceIndex, DWORD dwServiceID, tagServiceItem & ServiceItem)
{
	//��ȡ����
	ASSERT(wServiceIndex < CountArray(m_StockServiceInfo));
	if (wServiceIndex >= CountArray(m_StockServiceInfo)) return NULL;

	//Ѱ����Ϣ
	CGlobalServiceInfo * pGlobalServiceItem = NULL;
	pGlobalServiceItem = m_StockServiceInfo[wServiceIndex].SearchStockItem(ServiceItem.wServiceID);
	if (pGlobalServiceItem!=NULL)
	{
		ASSERT(FALSE);
		return NULL;
	}

	//������Ϣ
	pGlobalServiceItem = m_StockServiceInfo[wServiceIndex].CreateStockItem(ServiceItem.wServiceID);
	if (pGlobalServiceItem == NULL)
	{
		ASSERT(FALSE);
		return NULL;
	}

	//��������
	pGlobalServiceItem->m_pITCPSocketService = ActiveSocketService(dwServiceID);
	if (pGlobalServiceItem->m_pITCPSocketService == NULL)
	{
		ASSERT(FALSE);
		return NULL;
	}

	//����Ⱥ��
	pGlobalServiceItem->m_ServiceItem = ServiceItem;

	//��������
	auto pGlobalServiceKind = m_StockServiceKind[wServiceIndex].SearchStockItem(ServiceItem.wServiceKindID);
	if (pGlobalServiceKind ==NULL)
	{
		pGlobalServiceKind = m_StockServiceKind[wServiceIndex].CreateStockItem(ServiceItem.wServiceKindID);
		if (pGlobalServiceKind != NULL)
		{
			pGlobalServiceKind->m_wServiceKind = ServiceItem.wServiceKindID;
			pGlobalServiceKind->m_wServiceMapKind = m_pServiceAttribute[wServiceIndex].wServiceMapKind;
		}
	}

	//��ӷ���
	if (pGlobalServiceKind != NULL)
	{
		pGlobalServiceKind->AppendService(pGlobalServiceItem);
	}	

	return pGlobalServiceItem;
}

//ɾ������
bool CGlobalInfoManager::DeleteServiceItem(WORD wServiceIndex, WORD wServiceID)
{
	//��ȡ����
	ASSERT (wServiceIndex < CountArray(m_StockServiceInfo));
	if (wServiceIndex >= CountArray(m_StockServiceInfo)) return NULL;

	//��ѯ����
	auto pGlobalServiceItem = m_StockServiceInfo[wServiceIndex].SearchStockItem(wServiceID);
	if (pGlobalServiceItem == NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//������Ϣ
	WORD wServiceKind = pGlobalServiceItem->m_ServiceItem.wServiceKindID;

	//�Ƴ�����
	auto pGlobalServiceKind = m_StockServiceKind[wServiceIndex].SearchStockItem(wServiceKind);
	if (pGlobalServiceKind !=NULL)
	{
		pGlobalServiceKind->RemoveService(pGlobalServiceItem);
		if (pGlobalServiceKind->GetServiceCount() == 0)
		{
			m_StockServiceKind[wServiceIndex].RemoveStockItem(wServiceKind);
		}		
	}	

	//�Ƴ�����
	RemoveSocketService(pGlobalServiceItem->m_pITCPSocketService);

	//��������
	pGlobalServiceItem->ResetData();

	//�Ƴ�����
	m_StockServiceInfo[wServiceIndex].RemoveStockItem(wServiceID);

	return true;
}

//ӳ�����
CGlobalServiceInfo * CGlobalInfoManager::MapServiceInfo(WORD wServiceIndex, WORD wServiceKind, DWORD dwObjectID)
{
	//У������
	ASSERT (wServiceIndex < CountArray(m_StockServiceKind));
	if (wServiceIndex >= CountArray(m_StockServiceKind)) return NULL;

	//��������
	auto pStockServiceKind = m_StockServiceKind[wServiceIndex].SearchStockItem(wServiceKind);
	if (pStockServiceKind !=NULL)
	{		
		int nServiceID = pStockServiceKind->LookupService(dwObjectID);
		return SearchServiceInfo(wServiceIndex, nServiceID);		
	}	

	return NULL;
}

//Ѱ�ҷ���
CGlobalServiceInfo * CGlobalInfoManager::SearchServiceInfo(WORD wServiceIndex, WORD wServiceID)
{
	//��ȡ����
	ASSERT (wServiceIndex < CountArray(m_StockServiceInfo));
	if (wServiceIndex >= CountArray(m_StockServiceInfo)) return NULL;

	return m_StockServiceInfo[wServiceIndex].SearchStockItem(wServiceID);	
}

//��������
CGlobalServiceKind* CGlobalInfoManager::SearchServiceKind(WORD wServiceIndex, WORD wServiceKind)
{
	//��ȡ����
	ASSERT(wServiceIndex < CountArray(m_StockServiceKind));
	if (wServiceIndex >= CountArray(m_StockServiceKind)) return NULL;

	return m_StockServiceKind[wServiceIndex].SearchStockItem(wServiceKind);
}

//ö�ٷ���
CGlobalServiceInfo * CGlobalInfoManager::EnumServiceInfo(WORD wServiceIndex, CGlobalServiceInfo* pGlobalServiceItem)
{
	//��ȡ����
	ASSERT(wServiceIndex < CountArray(m_StockServiceInfo));
	if (wServiceIndex >= CountArray(m_StockServiceInfo)) return NULL;

	//��ȡ����
	if (pGlobalServiceItem == NULL)
	{
		return m_StockServiceInfo[wServiceIndex].GetHeadStockItem();
	}
	else
	{
		return m_StockServiceInfo[wServiceIndex].GetNextStockItem(pGlobalServiceItem);
	}
}

//������Ϣ
ITCPSocketService * CGlobalInfoManager::GetSocketService(WORD wServiceIndex, WORD wServiceID)
{
	//��������
	CGlobalServiceInfo * pGlobalServiceItem = SearchServiceInfo(wServiceIndex,wServiceID);
	if (pGlobalServiceItem != NULL && pGlobalServiceItem->m_pITCPSocketService != NULL)
	{
		return pGlobalServiceItem->m_pITCPSocketService;
	}

	return NULL;
}

//�������
ITCPSocketService* CGlobalInfoManager::ActiveSocketService(DWORD dwServiceID)
{
	//��������
	CTCPSocketServiceHelper* pTCPSocketService = NULL;

	//���һ���
	if (m_SocketServiceBuffer.GetCount() > 0)
	{
		//��ȡ����
		INT_PTR nItemCount = m_SocketServiceBuffer.GetCount();
		pTCPSocketService = m_SocketServiceBuffer[nItemCount - 1];
		m_SocketServiceBuffer.RemoveAt(nItemCount - 1);
	}

	//��������
	if (pTCPSocketService == NULL)
	{
		try
		{
			pTCPSocketService = new CTCPSocketServiceHelper;
			if (pTCPSocketService == NULL) throw TEXT("�ڴ治��,��������ʧ��!");
		}
		catch (...)
		{
			ASSERT(FALSE);
		}
	}

	//�����ӿ�
	if ((pTCPSocketService->GetInterface() == NULL) && (pTCPSocketService->CreateInstance() == false))
	{
		ASSERT(FALSE);
		SafeDelete(pTCPSocketService);

		return NULL;
	}

	//��ȡ�ӿ�
	ITCPSocketService* pITCPSocketService = pTCPSocketService->GetInterface();

	//���ñ�ʶ
	pITCPSocketService->SetServiceID(dwServiceID);

	//���ýӿ�
	if (pITCPSocketService->SetTCPSocketEvent(m_pIAttemperEngine) == false)
	{
		ASSERT(FALSE);
		SafeDelete(pTCPSocketService);

		return NULL;
	}

	//��������
	if (pITCPSocketService->StartService() == false)
	{
		ASSERT(FALSE);
		SafeDelete(pTCPSocketService);

		return NULL;
	}

	//��Ӷ���
	m_SocketServiceActive.Add(pTCPSocketService);

	return pITCPSocketService;
}

//�Ƴ�����
bool CGlobalInfoManager::RemoveSocketService(ITCPSocketService* pITCPSocketService)
{
	//����У��
	ASSERT(pITCPSocketService != NULL);
	if (pITCPSocketService == NULL) return false;

	//���Ҷ���
	for (INT_PTR i = 0; i < m_SocketServiceActive.GetCount(); i++)
	{
		if (m_SocketServiceActive[i]->GetInterface() == pITCPSocketService)
		{
			//ֹͣ����
			pITCPSocketService->ConcludeService();
			m_SocketServiceBuffer.Add(m_SocketServiceActive[i]);
			m_SocketServiceActive.RemoveAt(i);
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
