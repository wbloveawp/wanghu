#include "StdAfx.h"
#include "GlobalServiceManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//���캯��
CGlobalBattleService::CGlobalBattleService()
{
	//��������
	m_pITCPSocketService = NULL;	
}

//��������
CGlobalBattleService::~CGlobalBattleService()
{
}

//��������
VOID CGlobalBattleService::ResetData()
{
	m_pITCPSocketService = NULL;	
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalServiceManager::CGlobalServiceManager()
{
	//���ñ���
	m_pIAttemperEngine = NULL;

	//��ʼ��
	m_StockBattleService.InitHashTable(11);	
}

//��������
CGlobalServiceManager::~CGlobalServiceManager()
{
	//ɾ������
	m_StockBattleService.DeleteAllItem();

	//�Ƴ�Ԫ��	
	m_SocketServiceActive.RemoveAll();
	m_SocketServiceBuffer.RemoveAll();

	return;
}

//��������
VOID CGlobalServiceManager::ResetData()
{
	//�Ƴ�����
	m_StockBattleService.RemoveAllItem();

	//ֹͣ����
	for (INT_PTR i = 0; i < m_SocketServiceActive.GetCount(); i++)
	{
		if (m_SocketServiceActive[i]->GetInterface())
		{
			m_SocketServiceActive[i]->GetInterface()->ConcludeService();
		}
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

//�������
CGlobalBattleService * CGlobalServiceManager::ActiveBattleService(DWORD dwServiceID, tagServiceItem & ServiceItem)
{
	//Ѱ����Ϣ
	CGlobalBattleService * pGlobalServiceItem = NULL;
	pGlobalServiceItem = m_StockBattleService.SearchStockItem(ServiceItem.wServiceID);
	if (pGlobalServiceItem!=NULL)
	{
		ASSERT(FALSE);
		return NULL;
	}

	//������Ϣ
	pGlobalServiceItem = m_StockBattleService.CreateStockItem(ServiceItem.wServiceID);
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

	//���ò���
	pGlobalServiceItem->m_ServiceItem = ServiceItem;

	return pGlobalServiceItem;
}

//ɾ������
bool CGlobalServiceManager::DeleteBattleService(WORD wServiceID)
{
	//��ѯ����
	auto pGlobalServiceItem = m_StockBattleService.SearchStockItem(wServiceID);
	if (pGlobalServiceItem == NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//������Ϣ
	WORD wServiceKind = pGlobalServiceItem->m_ServiceItem.wServiceKindID;

	//�Ƴ�����
	RemoveSocketService(pGlobalServiceItem->m_pITCPSocketService);

	//�Ƴ�����
	m_StockBattleService.RemoveStockItem(wServiceID);

	return true;
}

//Ѱ�ҷ���
CGlobalBattleService * CGlobalServiceManager::SearchBattleService(WORD wServiceID)
{
	return m_StockBattleService.SearchStockItem(wServiceID);	
}

//ö�ٷ���
CGlobalBattleService * CGlobalServiceManager::EnumBattleService(CGlobalBattleService* pGlobalBattleService)
{
	//��ȡ����
	if (pGlobalBattleService == NULL)
	{
		return m_StockBattleService.GetHeadStockItem();
	}
	else
	{
		return m_StockBattleService.GetNextStockItem(pGlobalBattleService);
	}
}

//�������
ITCPSocketService* CGlobalServiceManager::ActiveSocketService(DWORD dwServiceID)
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
bool CGlobalServiceManager::RemoveSocketService(ITCPSocketService* pITCPSocketService)
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
