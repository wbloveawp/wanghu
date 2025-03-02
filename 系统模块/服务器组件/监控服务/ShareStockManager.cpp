#include "StdAfx.h"
#include "ShareStockManager.h"

////////////////////////////////////////////////////////////////////////
// 
#define INVALID_INDEX			-1

////////////////////////////////////////////////////////////////////////
//���������

//����ʼ��
LPCTSTR g_pszMutexNameStockInit = TEXT("GlobalMutex_1D3AF3B1_42AD_4CE4_B1D9_36BF542772E0");

//�������
LPCTSTR g_pszMutexNameStockItem = TEXT("GlobalMutex_060E5C57_E9EC_4E22_B97A_1716D0CF8098_%d"); 

////////////////////////////////////////////////////////////////////////

//����ʼ
#pragma data_seg("SharedStock") 

//����������
bool m_bStockInit = false;
DWORD m_dwVisitorCount = 0;

tagShareData m_ShareData = {};

//�������
#pragma data_seg() 

//////////////////////////////////////////////////////////////////////////////////
//����ѡ��
#pragma comment(linker, "/section:SharedStock,rws")

//////////////////////////////////////////////////////////////////////////////////
//���캯��
CShareMutex::CShareMutex()
{
	//���ñ���
	m_hMutex = NULL;
	m_dwMutexNo = 0;
	m_nLockCount = 0;
	m_ulLockTickCount = 0;
}

//��������
CShareMutex::~CShareMutex()
{
	//�رվ��
	CloseMutex();
}

//У����
bool CShareMutex::IsMutexValid()
{
	return m_hMutex != NULL;
}

//�رվ��
VOID CShareMutex::CloseMutex()
{
	//�ͷž��
	if (m_hMutex != NULL)
	{
		//����
		if (m_nLockCount > 0) ReleaseMutex(m_hMutex);

		//�رվ��
		CloseHandle(m_hMutex);
	}

	//���ñ���
	m_hMutex = NULL;
	m_dwMutexNo = 0;
	m_nLockCount = 0;
	m_ulLockTickCount = 0;
}

//��ʼ��
VOID CShareMutex::InitMutex(LPCTSTR pszMutexPrefix, DWORD dwMutexNo)
{
	//У����
	ASSERT(m_hMutex==NULL);
	if (m_hMutex != NULL) CloseMutex();

	//��������
	TCHAR szMutexName[128];
	_sntprintf_s(szMutexName,CountArray(szMutexName),TEXT("%s_%d"), pszMutexPrefix, dwMutexNo);

	//��������
	m_dwMutexNo = dwMutexNo;
	m_hMutex = CreateMutex(NULL, FALSE, szMutexName);
}

//��ȡʱ���
ULONGLONG CShareMutex::GetLockTickCount()
{
	//��������
	CThreadLock Section(m_CriticalSection);

	return m_ulLockTickCount;
}

//����ʱ���
VOID CShareMutex::SetLockTickCount(ULONGLONG ulTickCount)
{
	//��������
	CThreadLock Section(m_CriticalSection);

	m_ulLockTickCount = ulTickCount;
}

//��������
DWORD CShareMutex::Lock(DWORD dwMilliseconds)
{
	//У����
	if (m_hMutex == NULL) return 0;

	if (++m_nLockCount == 1)
	{
		DWORD dwResult=WaitForSingleObject(m_hMutex, dwMilliseconds);
		if (dwResult == WAIT_OBJECT_0)
		{
			SetLockTickCount(GetTickCount64());
		}
	}

	return WAIT_OBJECT_0;
}
	
//��������
VOID CShareMutex::Unlock(WORD wStockKind,bool bForce)
{
	//CTraceService::TraceString(TEXT("Unlock MutextNo=%d"), TraceLevel_Debug, m_dwMutexNo);

	//У����
	if (m_hMutex == NULL) return;

	//��������
	if (bForce == true && m_nLockCount != 1)
	{
		m_nLockCount = 1;
	}

	//��������
	if (m_nLockCount>0 && --m_nLockCount==0)
	{
		static DWORD dwLockCount = 0;

		dwLockCount++;
		auto ulTickCount = GetTickCount64();
		if (ulTickCount > m_ulLockTickCount)
		{
			CTraceService::TraceString(TEXT("��������: %d, ����ʱ����%dms,����%d,MutexNo=%d"), TraceLevel_Exception, dwLockCount, ulTickCount - m_ulLockTickCount, wStockKind,m_dwMutexNo);
		}

		//����ʱ���
		SetLockTickCount(0);

		//��������
		bool bResult=ReleaseMutex(m_hMutex);
		if (bResult == false)
		{
			CTraceService::TraceString(TEXT("����ʧ�ܣ�ErrorID=%d"), TraceLevel_Exception, GetLastError());
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////
//���캯��
CShareStockItem::CShareStockItem(DWORD dwUserLabel, CShareMutex* pShareMutex, tagShareStock* pShareStock)
{
	m_dwUserLabel = dwUserLabel;
	m_pShareStock = pShareStock;
	m_pShareMutex = pShareMutex;
}

//��������
CShareStockItem::~CShareStockItem()
{
	//��������
	if (m_pShareMutex != NULL)
	{
		m_pShareMutex->Unlock(m_pShareStock->StockItem.wStockKind);
	}

	//���ñ���
	m_pShareMutex = NULL;	
	m_pShareStock = NULL;
}

//����ˮ
bool CShareStockItem::StockShrinkAndUpdate(SCORE lBetScore, WORD wJackpotCount)
{
	//��ˮ����
	SCORE lShrinkValue = 0;
	SCORE lShrinkTotalValue = 0;
	WORD wShrinkRatio = GetShrinkRatio();

	//�����ˮ
	if (lBetScore > 0)
	{
		//����ˮ
		lShrinkValue = lBetScore * wShrinkRatio / 1000;
		lShrinkTotalValue += lShrinkValue;
		m_pShareStock->StockItem.lShrinkProfit += lShrinkValue;

		//�ʳس�ˮ
		for (WORD wLevelID = 1; wLevelID <= wJackpotCount; wLevelID++)
		{
			auto pJackpotItem = GetJackpotItem(wLevelID);
			if (pJackpotItem != NULL)
			{
				lShrinkValue = (lBetScore * pJackpotItem->wShrinkRatio / 1000);
				lShrinkTotalValue += lShrinkValue;
				pJackpotItem->lStockScore += lShrinkValue;
			}
		}
	}	

	//���¿��
	m_pShareStock->StockItem.lStockScore += (lBetScore - lShrinkTotalValue);

	return true;
}

//��ȡ״̬
WORD CShareStockItem::GetStockID()
{
	return m_pShareStock ? m_pShareStock->StockItem.wStockID : 0;
}
	
//�������
WORD CShareStockItem::GetStockKind()
{
	return m_pShareStock ? m_pShareStock->StockItem.wStockKind: 0;
}

//��ˮ����
WORD CShareStockItem::GetShrinkRatio()
{
	if (m_pShareStock != NULL)
	{
		//��ȡ����
		auto pStockItem = &m_pShareStock->StockItem;
		auto pStockControl = &m_pShareStock->StockControl;

		//��R���
		if ((m_dwUserLabel&UL_SUPPER_PLAYER)!=0 && pStockControl->SuperPlayerControl.bEanbleState)
		{
			for (int i = pStockControl->SuperPlayerControl.wShrinkCount - 1; i >= 0; i--)
			{
				if (pStockItem->lShrinkProfit >= pStockItem->ShrinkItem[i].lShrinkProfit)
				{
					return pStockControl->SuperPlayerControl.ShrinkItem[i].wShrinkRatio;
				}
			}
		}
		else
		{
			for (int i = pStockItem->wShrinkCount - 1; i >= 0; i--)
			{
				if (pStockItem->lShrinkProfit >= pStockItem->ShrinkItem[i].lShrinkProfit)
				{
					return pStockItem->ShrinkItem[i].wShrinkRatio;
				}
			}
		}
	}	

	return 0L;
}
	
//��ǰ���
SCORE CShareStockItem::GetStockScore()
{
	return m_pShareStock ? m_pShareStock->StockItem.lStockScore : 0L;
}
	
//��ȡ�ʽ�
tagJackpotItem* CShareStockItem::GetJackpotItem(WORD wLevelID)
{
	if (m_pShareStock != NULL && wLevelID <= m_pShareStock->StockItem.wJackpotCount)
	{
		for (int i = 0; i < m_pShareStock->StockItem.wJackpotCount; i++)
		{
			if (m_pShareStock->StockItem.GameJackpotItem[i].wLevelID == wLevelID)
			{
				return &m_pShareStock->StockItem.GameJackpotItem[i];
			}
		}
	}

	return NULL;
}

//�����Ϣ
tagStockItem * CShareStockItem::GetStockItem()
{
	if (m_pShareStock == NULL)
	{
		return NULL;
	}

	return &m_pShareStock->StockItem;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CShareStockVisitor::CShareStockVisitor()
{
	//���ñ���
	m_pStockList = NULL;
	m_wKindID = INVALID_WORD;		

	return;
}

//��������
CShareStockVisitor::~CShareStockVisitor()
{	
	//ֹͣ����
	if (IsRuning())
	{
		ConcludeThread(INFINITY);
	}

	//���ö���
	ResetStockVisitor();
}

//���ö���
VOID CShareStockVisitor::ResetStockVisitor()
{
	if (m_pStockList != NULL)
	{
		//���ñ���
		m_wKindID = 0;
		m_pStockList = NULL;

		//��������
		m_InitMutex.Lock();
		if (m_dwVisitorCount > 0) m_dwVisitorCount--;
		m_InitMutex.Unlock();

		//�رվ��
		m_InitMutex.CloseMutex();
		m_MutexList.ListMutex.CloseMutex();

		//�رն���
		for (int i = 0; i < CountArray(m_MutexList.StockMutext); i++)
		{
			m_MutexList.StockMutext[i].CloseMutex();
		}
	}
}

//��ʼ��
bool CShareStockVisitor::InitStockVisitor(WORD wKindID)
{
	if (wKindID==0 || !GetStockInit()) return false;

	//������Ϸ
	for (int i = 0; i < CountArray(m_ShareData.RoutineStock); i++)
	{
		if (m_ShareData.RoutineStock[i].wKindID == wKindID)
		{
			m_pStockList = &m_ShareData.RoutineStock[i];
			break;
		}
	}

	//�˳��ж�
	if (m_pStockList == NULL) return false;

	//��������
	m_MutexList.ListMutex.InitMutex(g_pszMutexNameStockItem,m_pStockList->dwMutexNo);
	
	//��������
	for (int i = 0; i < CountArray(m_pStockList->ShareStock); i++)
	{
		m_MutexList.StockMutext[i].InitMutex(g_pszMutexNameStockItem, m_pStockList->ShareStock[i].dwMutexNo);
	}

	//���ñ���
	m_wKindID = wKindID;		

	//��������
	m_InitMutex.Lock();
	m_dwVisitorCount++;
	m_InitMutex.Unlock();

	return true;
}

//���ҿ��
CShareStockItemPtr CShareStockVisitor::GetStockItem(WORD wStockKind, SCORE lBetAmount, DWORD dwUserLabel)
{
	//У��״̬
	if (m_pStockList == NULL || m_pStockList->wKindID != m_wKindID)
	{
		return NULL;
	}

	//��ȡ����
	auto nStockIndex = GetStockIndex(wStockKind, lBetAmount);
	if (nStockIndex < 0)
	{
		return NULL;
	}

	//��������
	m_MutexList.StockMutext[nStockIndex].Lock();

	//��ȡ����
	auto pShareMutex = &m_MutexList.StockMutext[nStockIndex];
	auto pShareStock = &m_pStockList->ShareStock[nStockIndex];
	auto pRoutineStockItem = &m_pStockList->ShareStock[nStockIndex].StockItem;

	//�ж�״̬
	if (pShareStock->StockItem.cbStockState != STOCK_STATE_ENABLE)
	{
		//����
		m_MutexList.StockMutext[nStockIndex].Unlock();

		//����
		m_MutexList.ListMutex.Lock();

		//�жϿ��
		if (m_pStockList->pRelationStock == NULL)
		{
			m_MutexList.ListMutex.Unlock();

			return (pRoutineStockItem->cbStockState!=STOCK_STATE_ENABLE)?NULL:GetStockItem(wStockKind, lBetAmount, dwUserLabel);
		}

		//��������
		if (m_MutexList.RelationMutex.GetMutexNo()!= m_pStockList->pRelationStock->dwMutexNo)
		{
			m_MutexList.RelationMutex.CloseMutex();
			m_MutexList.RelationMutex.InitMutex(g_pszMutexNameStockItem,m_pStockList->pRelationStock->dwMutexNo);
		}

		//������
		pShareStock = m_pStockList->pRelationStock;

		//����
		m_MutexList.ListMutex.Unlock();		
		
		//����
		m_MutexList.RelationMutex.Lock();

		//�ж�״̬
		if (pShareStock->StockItem.cbStockState != STOCK_STATE_ENABLE)
		{
			m_MutexList.RelationMutex.Unlock();

			return (pRoutineStockItem->cbStockState!=STOCK_STATE_ENABLE)?NULL:GetStockItem(wStockKind, lBetAmount, dwUserLabel);
		}

		//�������
		pShareMutex = &m_MutexList.RelationMutex;
	}

	try
	{
		CShareStockItemPtr ShareStockItemPtr = new CShareStockItem(dwUserLabel, pShareMutex, pShareStock);

		return ShareStockItemPtr;
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return NULL;
}

//�����¼�
bool CShareStockVisitor::OnEventThreadRun()
{
#ifndef _DEBUG

	//��ʱ���
	//for (int i=0; i<CountArray(m_MutexData); i++)
	//{
	//	//��ȡʱ���
	//	auto ulCurrTickCount = GetTickCount64();
	//	auto ulLockTickCount = m_StockMutex[i].GetLockTickCount();

	//	//��ʱУ��
	//	if (ulLockTickCount > 0 && ulCurrTickCount - ulLockTickCount >= 1200)
	//	{
	//		//ExitProcess(0);			
	//	}
	//}
#endif

	//����
	Sleep(100);

	return true;
}

//��ʼ����־
bool CShareStockVisitor::GetStockInit()
{
	//У����
	if (!m_InitMutex.IsMutexValid())
	{
		m_InitMutex.InitMutex(g_pszMutexNameStockInit, 1);
	}

	//��������
	m_InitMutex.Lock();
	bool bStockInit = m_bStockInit;
	m_InitMutex.Unlock();

	return bStockInit;
}

//��������
int CShareStockVisitor::GetStockIndex(WORD wStockKind, LONG lBetAmount)
{
	for (int i = 0; i < CountArray(m_pStockList->ShareStock); i++)
	{
		if (m_pStockList->ShareStock[i].StockItem.lBetAmount == lBetAmount)
		{
			return i;
		}
	}

	return INVALID_INDEX;
}

//////////////////////////////////////////////////////////////////////////////////
//���캯��
CShareStockManager::CShareStockManager()
{
	//���ñ���
	m_bManageState = false;
	m_pStockManagerSink = NULL;
	ZeroMemory(&m_SuperPlayerControl,sizeof(m_SuperPlayerControl));

	//��ʼ����ϣ��
	m_StockStockItem.InitHashTable(103);		
}

//��������
CShareStockManager::~CShareStockManager()
{
	CloseStockManager();
}

VOID CShareStockManager::StartStockManager(IStockManagerSink* pIStockManagerSink)
{
	//������
	DWORD dwMutexNo = 1;

	//У����
	if (!m_InitMutex.IsMutexValid())
	{
		m_InitMutex.InitMutex(g_pszMutexNameStockInit, dwMutexNo++);
	}

	//��������
	m_InitMutex.Lock();

	//��������
	if (m_bStockInit == false)
	{
		//���ñ��		
		ZeroMemory(&m_ShareData, sizeof(m_ShareData));

		for (int i = 0; i < CountArray(m_ShareData.ActivityStock); i++)
		{
			m_ShareData.ActivityStock[i].dwMutexNo = dwMutexNo++;
			m_MutexData.ActivityMutex[i].InitMutex(g_pszMutexNameStockItem, m_ShareData.ActivityStock[i].dwMutexNo);
		}

		for (int i = 0; i < CountArray(m_ShareData.RoutineStock); i++)
		{
			auto pStockList = &m_ShareData.RoutineStock[i];
			pStockList->dwMutexNo = dwMutexNo++;
			m_MutexData.RoutineMutex[i].ListMutex.InitMutex(g_pszMutexNameStockItem,pStockList->dwMutexNo);

			for (int j = 0; j < CountArray(pStockList->ShareStock); j++)
			{
				pStockList->ShareStock[j].dwMutexNo = dwMutexNo++;
				m_MutexData.RoutineMutex[i].StockMutext[j].InitMutex(g_pszMutexNameStockItem, pStockList->ShareStock[j].dwMutexNo);
			}
		}

		//���ñ���
		m_bStockInit = true;		
	}

	//����״̬
	m_bManageState = true;
	m_lStockControlID = time(NULL)*1000;

	//��ѯ�ӿ�
	m_pStockManagerSink = pIStockManagerSink;

	//��������
	m_InitMutex.Unlock();
}

//���ö���
bool CShareStockManager::CloseStockManager()
{
	//У����
	if (m_InitMutex.IsMutexValid())
	{
		//����
		m_InitMutex.Lock();

		//У������
		if (m_dwVisitorCount > 0)
		{
			m_InitMutex.Unlock();

			return false;
		}

		//����״̬
		m_bManageState = false;

		//�رն���
		if (m_bStockInit = true)
		{
			//�����
			DetectStockChange();

			//���״̬
			DetectStockState();

			for (int i = 0; i < CountArray(m_MutexData.ActivityMutex); i++)
			{
				m_MutexData.ActivityMutex[i].CloseMutex();
			}

			for (int i = 0; i < CountArray(m_MutexData.RoutineMutex); i++)
			{
				m_MutexData.RoutineMutex[i].ListMutex.CloseMutex();

				for (int j = 0; j < CountArray(m_MutexData.RoutineMutex[i].StockMutext); j++)
				{
					m_MutexData.RoutineMutex[i].StockMutext[j].CloseMutex();
				}
			}
		}

		//�Ƴ�����
		m_StockStockItem.RemoveAllItem();		

		//���ñ���
		m_bStockInit = false;
		m_lStockControlID = 0;
		m_pStockManagerSink = NULL;
		ZeroMemory(&m_ShareData, sizeof(m_ShareData));
		ZeroMemory(&m_SuperPlayerControl, sizeof(m_SuperPlayerControl));

		//����
		m_InitMutex.Unlock();
		m_InitMutex.CloseMutex();
	}

	return true;
}

//ʱ������
VOID CShareStockManager::OnEventTimerPulse()
{
	//���״̬
	DetectStockState();

	//�����
	DWORD dwCurrentTime = time(NULL);	
	if (dwCurrentTime % 5 == 0) DetectStockChange();
}

//��ӿ��
bool CShareStockManager::AppendStockItem(tagStockItem StockItem[], WORD wStockCount)
{
	//У�����
	ASSERT (wStockCount > 0);
	if (wStockCount == 0) return false;

	//ѭ�����
	for (int i = 0; i < wStockCount; i++)
	{
		//��ȡ����
		auto pStockItem = &StockItem[i];

		//���ҿ��
		auto pSourceStockItem = m_StockStockItem.SearchStockItem(pStockItem->wStockID);
		if (pSourceStockItem != NULL) continue;

		//�������
		pSourceStockItem = m_StockStockItem.CreateStockItem(pStockItem->wStockID);
		if (pSourceStockItem == NULL) continue;

		//��������
		ZeroMemory(pSourceStockItem, sizeof(tagStockManagerItem));
		CopyMemory(&pSourceStockItem->StockItem, pStockItem, sizeof(tagStockItem));	
	}

	return true;
}

//ɾ�����
VOID CShareStockManager::DeleteStockItem(WORD wStockID)
{
	//���ҿ��
	auto pSourceStockItem = m_StockStockItem.SearchStockItem(wStockID);
	if (pSourceStockItem == NULL) return;

	//�������
	auto pStockItem = &pSourceStockItem->StockItem;

	//������
	if (pStockItem->cbEnableMode == STOCK_ENABLE_MODE_AUTO)
	{
		//���ҿ���б�
		auto nListIndex = SearchRoutineStockList(pStockItem->wStockKind, pStockItem->wKindID);
		if (nListIndex != INVALID_INDEX)
		{
			//��������
			auto nItemIndex = SearchRoutineStockItem(nListIndex, pStockItem->wStockID);
			if (nItemIndex != INVALID_INDEX)
			{
				//����
				m_MutexData.RoutineMutex[nListIndex].StockMutext[nItemIndex].Lock();

				//��������
				ZeroMemory(&m_ShareData.RoutineStock[nListIndex].ShareStock[nItemIndex].StockItem, sizeof(tagStockItem));

				//����
				m_MutexData.RoutineMutex[nListIndex].StockMutext[nItemIndex].Unlock();
			}
		}		
	}
	else
	{
		//����״̬
		if (pStockItem->cbStockState == STOCK_STATE_ENABLE)
		{
			//���ѿ��
			auto nItemIndex = SearchActivityStockItem(wStockID);
			if (nItemIndex != INVALID_INDEX)
			{
				FissionActivityStock(nItemIndex);

				//��������
				ZeroMemory(&m_ShareData.ActivityStock[nItemIndex].StockItem, sizeof(tagStockItem));
			}
		}
	}	

	//�Ƴ�����
	m_StockStockItem.RemoveStockItem(wStockID);
}

//�޸Ŀ��
bool CShareStockManager::ModifyStockItem(tagStockItem* pStockItem)
{
	//���ҿ��
	auto pStockManagerItem = m_StockStockItem.SearchStockItem(pStockItem->wStockID);
	if (pStockManagerItem == NULL) return false;

	//��������
	CShareMutex* pShareMutext = NULL;
	tagStockItem* pShareStockItem = NULL;
	tagStockItem* pSourceStockItem = &pStockManagerItem->StockItem;

	//������
	if (pStockItem->cbEnableMode == STOCK_ENABLE_MODE_AUTO)
	{
		//�����б�
		auto nListIndex = SearchRoutineStockList(pStockItem->wStockKind, pStockItem->wKindID);
		if (nListIndex != INVALID_INDEX)
		{
			//��������
			auto nItemIndex = SearchRoutineStockItem(nListIndex, pStockItem->wStockID);
			if (nItemIndex != INVALID_INDEX)
			{
				//����
				pShareMutext = &m_MutexData.RoutineMutex[nListIndex].StockMutext[nItemIndex];

				//������
				pShareStockItem = &m_ShareData.RoutineStock[nListIndex].ShareStock[nItemIndex].StockItem;
			}
		}
	}
	else
	{
		//��������
		auto nItemIndex = SearchActivityStockItem(pStockItem->wStockID);
		if (nItemIndex != INVALID_INDEX)
		{
			//��ȡ��
			pShareMutext = &m_MutexData.ActivityMutex[nItemIndex];

			//������
			pShareStockItem = &m_ShareData.ActivityStock[nItemIndex].StockItem;
		}
	}

	//ԭʼ���
	if (pSourceStockItem != NULL)
	{
		//���ñ���
		pSourceStockItem->lBetAmount = pStockItem->lBetAmount;
		pSourceStockItem->lExtraScore = pStockItem->lExtraScore;
		pSourceStockItem->wShrinkCount = pStockItem->wShrinkCount;
		CopyMemory(pSourceStockItem->ShrinkItem, pStockItem->ShrinkItem, sizeof(pSourceStockItem->ShrinkItem));

		//��������
		pSourceStockItem->cbEnableMode = pStockItem->cbEnableMode;
		pSourceStockItem->dwEnableEndTime = pStockItem->dwEnableEndTime;
		pSourceStockItem->dwEnableStartTime = pStockItem->dwEnableStartTime;

		//������Ϣ
		pSourceStockItem->wRelationCount = pStockItem->wRelationCount;
		CopyMemory(pSourceStockItem->RelationStockID, pStockItem->RelationStockID, sizeof(pSourceStockItem->RelationStockID));

		//��������
		pSourceStockItem->wJackpotCount = pStockItem->wJackpotCount;
		for (int i = 0; i < CountArray(pSourceStockItem->GameJackpotItem); i++)
		{
			if (i < pStockItem->wJackpotCount)
			{
				if (pSourceStockItem->GameJackpotItem[i].wLevelID == 0)
				{
					CopyMemory(&pSourceStockItem->GameJackpotItem[i], &pStockItem->GameJackpotItem[i],sizeof(pSourceStockItem->GameJackpotItem[i]));
				}
				else
				{
					pSourceStockItem->GameJackpotItem[i].wPayoutProb = pStockItem->GameJackpotItem[i].wPayoutProb;
					pSourceStockItem->GameJackpotItem[i].wShrinkRatio = pStockItem->GameJackpotItem[i].wShrinkRatio;
					pSourceStockItem->GameJackpotItem[i].lExtraScore = pStockItem->GameJackpotItem[i].lExtraScore;
					pSourceStockItem->GameJackpotItem[i].lPayoutLimit = pStockItem->GameJackpotItem[i].lPayoutLimit;
					pSourceStockItem->GameJackpotItem[i].lPayoutOrigin = pStockItem->GameJackpotItem[i].lPayoutOrigin;
					pSourceStockItem->GameJackpotItem[i].lVirtualScore = pStockItem->GameJackpotItem[i].lVirtualScore;
				}
			}			
			else
			{
				ZeroMemory(&pSourceStockItem->GameJackpotItem[i], sizeof(pSourceStockItem->GameJackpotItem[i]));
			}
		}
	}

	//������
	if (pShareStockItem != NULL)
	{
		//����
		pShareMutext->Lock();

		//BET���
		pShareStockItem->lBetAmount = pStockItem->lBetAmount;
		pShareStockItem->lExtraScore = pStockItem->lExtraScore;
		pShareStockItem->wShrinkCount = pStockItem->wShrinkCount;
		CopyMemory(pShareStockItem->ShrinkItem, pStockItem->ShrinkItem, sizeof(pShareStockItem->ShrinkItem));

		//��������
		pShareStockItem->cbEnableMode = pStockItem->cbEnableMode;
		pShareStockItem->dwEnableEndTime = pStockItem->dwEnableEndTime;
		pShareStockItem->dwEnableStartTime = pStockItem->dwEnableStartTime;

		//������Ϣ
		pShareStockItem->wRelationCount = pStockItem->wRelationCount;
		CopyMemory(pShareStockItem->RelationStockID, pStockItem->RelationStockID, sizeof(pShareStockItem->RelationStockID));

		//��������
		pShareStockItem->wJackpotCount = pStockItem->wJackpotCount;
		for (int i = 0; i < CountArray(pShareStockItem->GameJackpotItem); i++)
		{
			if (i < pStockItem->wJackpotCount)
			{
				if (pShareStockItem->GameJackpotItem[i].wLevelID == 0)
				{
					CopyMemory(&pShareStockItem->GameJackpotItem[i], &pStockItem->GameJackpotItem[i], sizeof(pShareStockItem->GameJackpotItem[i]));
				}
				else
				{
					pShareStockItem->GameJackpotItem[i].wPayoutProb = pStockItem->GameJackpotItem[i].wPayoutProb;
					pShareStockItem->GameJackpotItem[i].wShrinkRatio = pStockItem->GameJackpotItem[i].wShrinkRatio;
					pShareStockItem->GameJackpotItem[i].lExtraScore = pStockItem->GameJackpotItem[i].lExtraScore;
					pShareStockItem->GameJackpotItem[i].lPayoutLimit = pStockItem->GameJackpotItem[i].lPayoutLimit;
					pShareStockItem->GameJackpotItem[i].lPayoutOrigin = pStockItem->GameJackpotItem[i].lPayoutOrigin;
					pShareStockItem->GameJackpotItem[i].lVirtualScore = pStockItem->GameJackpotItem[i].lVirtualScore;
				}
			}
			else
			{
				ZeroMemory(&pShareStockItem->GameJackpotItem[i],sizeof(pShareStockItem->GameJackpotItem[i]));
			}
		}

		//����
		pShareMutext->Unlock();
	}
	
	return true;
}

//�������ֵ
bool CShareStockManager::AdjustStockScore(WORD wStockID, SCORE lChangeScore)
{
	//���ҿ��
	auto pStockManagerItem = m_StockStockItem.SearchStockItem(wStockID);
	if (pStockManagerItem == NULL) return false;

	//��������
	CShareMutex* pShareMutext = NULL;
	tagStockItem* pShareStockItem = NULL;
	tagStockItem* pSourceStockItem = &pStockManagerItem->StockItem;

	//������
	if (pSourceStockItem->cbEnableMode == STOCK_ENABLE_MODE_AUTO)
	{
		//�����б�
		auto nListIndex = SearchRoutineStockList(pSourceStockItem->wStockKind, pSourceStockItem->wKindID);
		if (nListIndex != INVALID_INDEX)
		{
			//��������
			auto nItemIndex = SearchRoutineStockItem(nListIndex, pSourceStockItem->wStockID);
			if (nItemIndex != INVALID_INDEX)
			{
				//����
				pShareMutext = &m_MutexData.RoutineMutex[nListIndex].StockMutext[nItemIndex];

				//������
				pShareStockItem = &m_ShareData.RoutineStock[nListIndex].ShareStock[nItemIndex].StockItem;
			}
		}
	}
	else
	{
		//��������
		auto nItemIndex = SearchActivityStockItem(pSourceStockItem->wStockID);
		if (nItemIndex != INVALID_INDEX)
		{
			//��ȡ��
			pShareMutext = &m_MutexData.ActivityMutex[nItemIndex];

			//������
			pShareStockItem = &m_ShareData.ActivityStock[nItemIndex].StockItem;
		}
	}

	if (pShareStockItem != NULL)
	{
		//����
		pShareMutext->Lock();

		//���·���
		pShareStockItem->lStockScore += lChangeScore;

		//��������
		tagStockVariationItem StockVariationItem = {};

		//�����
		StockVariationItem.wStockID = wStockID;
		StockVariationItem.lVariationScore = pShareStockItem->lStockScore - pSourceStockItem->lStockScore;
		StockVariationItem.lVariationProfit = pShareStockItem->lShrinkProfit - pSourceStockItem->lShrinkProfit;

		//���¿��
		pSourceStockItem->lStockScore = pShareStockItem->lStockScore;
		pSourceStockItem->lShrinkProfit = pShareStockItem->lShrinkProfit;

		//֪ͨ���
		if (m_pStockManagerSink != NULL) m_pStockManagerSink->OnEventStockScoreUpdate(&StockVariationItem, 1);

		//����
		pShareMutext->Unlock();
	}
	else
	{
		//��������
		tagStockVariationItem StockVariationItem = {};

		//�����
		StockVariationItem.wStockID = wStockID;
		StockVariationItem.lVariationProfit = 0;
		StockVariationItem.lVariationScore += lChangeScore;		

		//���¿��
		pSourceStockItem->lStockScore += lChangeScore;		

		//֪ͨ���
		if (m_pStockManagerSink != NULL) m_pStockManagerSink->OnEventStockScoreUpdate(&StockVariationItem, 1);
	}

	return true;
}

//�����ʽ�
bool CShareStockManager::AdjustJackpotScore(WORD wStockID, WORD wLevelID, SCORE lChangeScore)
{
	//У�����
	ASSERT(wLevelID > 0 && lChangeScore != 0);
	if (wLevelID == 0 || lChangeScore == 0) return false;

	//���ҿ��
	auto pStockManagerItem = m_StockStockItem.SearchStockItem(wStockID);
	if (pStockManagerItem == NULL) return false;

	//У��ȼ�
	if (wLevelID > pStockManagerItem->StockItem.wJackpotCount) return false;

	//��������
	CShareMutex* pShareMutext = NULL;
	tagStockItem* pShareStockItem = NULL;
	tagStockItem* pSourceStockItem = &pStockManagerItem->StockItem;

	//������
	if (pSourceStockItem->cbEnableMode == STOCK_ENABLE_MODE_AUTO)
	{
		//�����б�
		auto nListIndex = SearchRoutineStockList(pSourceStockItem->wStockKind, pSourceStockItem->wKindID);
		if (nListIndex != INVALID_INDEX)
		{
			//��������
			auto nItemIndex = SearchRoutineStockItem(nListIndex, pSourceStockItem->wStockID);
			if (nItemIndex != INVALID_INDEX)
			{
				//����
				pShareMutext = &m_MutexData.RoutineMutex[nListIndex].StockMutext[nItemIndex];

				//������
				pShareStockItem = &m_ShareData.RoutineStock[nListIndex].ShareStock[nItemIndex].StockItem;
			}
		}
	}
	else
	{
		//��������
		auto nItemIndex = SearchActivityStockItem(pSourceStockItem->wStockID);
		if (nItemIndex != INVALID_INDEX)
		{

			//��ȡ��
			pShareMutext = &m_MutexData.ActivityMutex[nItemIndex];

			//������
			pShareStockItem = &m_ShareData.ActivityStock[nItemIndex].StockItem;
		}
	}
	
	if (pShareStockItem != NULL)
	{
		//����
		pShareMutext->Lock();

		//��ȡ����
		auto pShareJackpotItem = &pShareStockItem->GameJackpotItem[wLevelID - 1];	
		auto pSrouceJackpotItem = &pSourceStockItem->GameJackpotItem[wLevelID - 1];

		//���²ʽ�
		pShareJackpotItem->lStockScore += lChangeScore;

		//��������
		tagJackpotVariationItem JackpotVariationItem = {};

		//�ʽ���
		JackpotVariationItem.wLevelID = wLevelID;
		JackpotVariationItem.wStockID = wStockID;
		JackpotVariationItem.lVariationScore = pShareJackpotItem->lStockScore - pSrouceJackpotItem->lStockScore;

		//���²ʽ�
		pSrouceJackpotItem->lStockScore = pShareJackpotItem->lStockScore;

		//�ʽ���
		if (m_pStockManagerSink != NULL) m_pStockManagerSink->OnEventJackpotScoreUpdate(&JackpotVariationItem, 1);

		//����
		pShareMutext->Unlock();
	}
	else
	{
		//��ȡ����
		auto pSrouceJackpotItem = &pSourceStockItem->GameJackpotItem[wLevelID - 1];

		//��������
		tagJackpotVariationItem JackpotVariationItem = {};

		//�ʽ���
		JackpotVariationItem.wLevelID = wLevelID;
		JackpotVariationItem.wStockID = wStockID;
		JackpotVariationItem.lVariationScore += lChangeScore;

		//���²ʽ�
		pSrouceJackpotItem->lStockScore += lChangeScore;

		//�ʽ���
		if (m_pStockManagerSink != NULL) m_pStockManagerSink->OnEventJackpotScoreUpdate(&JackpotVariationItem, 1);
	}

	return true;
}

//ͳ�Ʋʽ�
bool CShareStockManager::CollectJackpotScore(WORD wStockKind, WORD wKindID,SCORE& lJackpotScore)
{
	//�����б�
	auto nListIndex = SearchRoutineStockList(wStockKind, wKindID);
	if (nListIndex == INVALID_INDEX) return false;

	if (m_ShareData.RoutineStock[nListIndex].pRelationStock != NULL)
	{
		for (int i = 0; i < m_ShareData.RoutineStock[nListIndex].pRelationStock->StockItem.wJackpotCount; i++)
		{
			lJackpotScore += m_ShareData.RoutineStock[nListIndex].pRelationStock->StockItem.GameJackpotItem[i].lStockScore;
			lJackpotScore += m_ShareData.RoutineStock[nListIndex].pRelationStock->StockItem.GameJackpotItem[i].lVirtualScore;
		}
	}
	else
	{
		for (int i = 0; i < CountArray(m_ShareData.RoutineStock[nListIndex].ShareStock); i++)
		{
			for (int j = 0; j < m_ShareData.RoutineStock[nListIndex].ShareStock[i].StockItem.wJackpotCount; j++)
			{
				lJackpotScore += m_ShareData.RoutineStock[nListIndex].ShareStock[i].StockItem.GameJackpotItem[j].lStockScore;
				lJackpotScore += m_ShareData.RoutineStock[nListIndex].ShareStock[i].StockItem.GameJackpotItem[j].lVirtualScore;
			}
		}			
	}			

	return true;
}

//���ô�R����
VOID CShareStockManager::ApplySuperPlayerControl(tagSuperPlayerControl* pSuperPlayerControl)
{
	//��������
	CopyMemory(&m_SuperPlayerControl, pSuperPlayerControl,sizeof(m_SuperPlayerControl));

	//����
	for (int i = 0; i < CountArray(m_ShareData.ActivityStock); i++)
	{
		if (m_ShareData.ActivityStock[i].StockItem.wStockID != 0)
		{
			m_MutexData.ActivityMutex[i].Lock();
			CopyMemory(&m_ShareData.ActivityStock[i].StockControl.SuperPlayerControl, &m_SuperPlayerControl, sizeof(m_SuperPlayerControl));
			m_MutexData.ActivityMutex[i].Unlock();
		}
	}

	//������
	for (int i = 0; i < CountArray(m_ShareData.RoutineStock); i++)
	{
		for (int j = 0; j < CountArray(m_ShareData.RoutineStock[i].ShareStock); j++)
		{
			if (m_ShareData.RoutineStock[i].ShareStock[j].StockItem.wStockID != 0)
			{
				m_MutexData.RoutineMutex[i].StockMutext[j].Lock();
				CopyMemory(&m_ShareData.RoutineStock[i].ShareStock[j].StockControl.SuperPlayerControl, &m_SuperPlayerControl, sizeof(m_SuperPlayerControl));
				m_MutexData.RoutineMutex[i].StockMutext[j].Unlock();
			}
		}
	}
}

//�������
int CShareStockManager::ActiveActivityStockItem()
{
	for (int i = 0; i < CountArray(m_ShareData.ActivityStock); i++)
	{
		if (m_ShareData.ActivityStock[i].StockItem.wStockID == 0)
		{
			return i;
		}
	}

	return INVALID_INDEX;
}

//���ҹ�����
int CShareStockManager::SearchActivityStockItem(WORD wStockID)
{
	for (int i = 0; i < CountArray(m_ShareData.ActivityStock); i++)
	{
		if (m_ShareData.ActivityStock[i].StockItem.wStockID == wStockID)
		{
			return i;
		}
	}

	return INVALID_INDEX;
}

//�������б�
int CShareStockManager::ActiveRoutineStockList(WORD wStockKind)
{
	for (int i = 0; i < CountArray(m_ShareData.RoutineStock); i++)
	{
		if (m_ShareData.RoutineStock[i].wKindID == 0)
		{
			return i;
		}
	}

	return INVALID_INDEX;
}

//���ҿ���б�
int CShareStockManager::SearchRoutineStockList(WORD wStockKind, WORD wKindID)
{
	for (int i = 0; i < CountArray(m_ShareData.RoutineStock); i++)
	{
		if (m_ShareData.RoutineStock[i].wKindID == wKindID)
		{
			return i;
		}
	}

	return INVALID_INDEX;
}

//�������
int CShareStockManager::ActiveRoutineStockItem(int nListIndex, WORD wStockKind)
{
	//У�����
	ASSERT(nListIndex < CountArray(m_ShareData.RoutineStock));
	if (nListIndex >= CountArray(m_ShareData.RoutineStock)) return INVALID_INDEX;

	//����б�
	auto pStockList = &m_ShareData.RoutineStock[nListIndex];
	
	for (int i = 0; i < CountArray(pStockList->ShareStock); i++)
	{
		if (pStockList->ShareStock[i].StockItem.wStockID == 0)
		{
			return i;
		}
	}

	return INVALID_INDEX;
}

//���ҹ�����
int CShareStockManager::SearchRoutineStockItem(int nListIndex, WORD wStockID)
{
	//У�����
	ASSERT(nListIndex < CountArray(m_ShareData.RoutineStock));
	if (nListIndex >= CountArray(m_ShareData.RoutineStock)) return INVALID_INDEX;

	//����б�
	auto pStockList = &m_ShareData.RoutineStock[nListIndex];

	for (int i = 0; i < CountArray(pStockList->ShareStock); i++)
	{
		if (pStockList->ShareStock[i].StockItem.wStockID == wStockID)
		{
			return i;
		}
	}

	return INVALID_INDEX;
}

//���ҹ�����
int CShareStockManager::SearchRoutineStockItem(int nListIndex, WORD wStockKind, LONG lBetAmount)
{
	//У�����
	ASSERT (nListIndex < CountArray(m_ShareData.RoutineStock));
	if (nListIndex >= CountArray(m_ShareData.RoutineStock)) return INVALID_INDEX;

	//����б�
	auto pStockList = &m_ShareData.RoutineStock[nListIndex];

	for (int i = 0; i < CountArray(pStockList->ShareStock); i++)
	{
		if (pStockList->ShareStock[i].StockItem.lBetAmount == lBetAmount)
		{
			return i;
		}
	}

	return INVALID_INDEX;
}

//���״̬
VOID CShareStockManager::DetectStockState()
{
	//��ǰʱ��
	DWORD dwCurrentTime = time(NULL);

	//��������
	tagStockItem* pStockItem = NULL;
	tagStockManagerItem* pStockManagerItem = m_StockStockItem.GetHeadStockItem();

	//��������
	WORD wStateCount = 0;
	tagStateVariationItem StateVariationList[SHARE_STOCK_COUNT] = {};

	//����״̬
	if (m_bManageState==true)
	{
		while (pStockManagerItem != NULL)
		{
			auto pStockItemCurrent = pStockManagerItem;
			auto pStockItem = &pStockManagerItem->StockItem;

			//�Զ�ģʽ
			if (pStockItem->cbEnableMode == STOCK_ENABLE_MODE_AUTO)
			{
				//δ���� 
				if (m_bManageState && pStockItem->cbStockState == STOCK_STATE_DISABLE)
				{
					//��������
					bool bEnableStock = true;

					//У����
					if (pStockItem->lStockScore < 0) bEnableStock = false;
					for (int i = 0; i < pStockItem->wJackpotCount; i++)
					{
						if (pStockItem->GameJackpotItem[i].lStockScore < 0)
						{
							bEnableStock = false;
							break;
						}
					}

					if (bEnableStock == true)
					{
						//���ҿ���б�
						auto nListIndex = SearchRoutineStockList(pStockItem->wStockKind, pStockItem->wKindID);
						if (nListIndex == INVALID_INDEX)
						{
							nListIndex = ActiveRoutineStockList(pStockItem->wStockKind);
							if (nListIndex != INVALID_INDEX)
							{
								m_ShareData.RoutineStock[nListIndex].wKindID = pStockItem->wKindID;
							}
						}

						if (nListIndex != INVALID_INDEX && m_ShareData.RoutineStock[nListIndex].pRelationStock == NULL)
						{
							//��������
							auto nItemIndex = SearchRoutineStockItem(nListIndex, pStockItem->wStockKind, pStockItem->lBetAmount);
							if (nItemIndex == INVALID_INDEX)
							{
								nItemIndex = ActiveRoutineStockItem(nListIndex, pStockItem->wStockKind);
							}

							if (nItemIndex != INVALID_INDEX)
							{
								//����
								m_MutexData.RoutineMutex[nListIndex].StockMutext[nItemIndex].Lock();

								//����״̬
								pStockItem->cbStockState = STOCK_STATE_ENABLE;

								//��������
								CopyMemory(&m_ShareData.RoutineStock[nListIndex].ShareStock[nItemIndex].StockItem, pStockItem, sizeof(tagStockItem));
								CopyMemory(&m_ShareData.RoutineStock[nListIndex].ShareStock[nItemIndex].StockControl.SuperPlayerControl, &m_SuperPlayerControl, sizeof(m_SuperPlayerControl));

								//���Ʊ�ʶ
								pStockManagerItem->lStockControlID = ++m_lStockControlID;

								//״̬���
								StateVariationList[wStateCount].wStockID = pStockItem->wStockID;
								StateVariationList[wStateCount].cbStockState = pStockItem->cbStockState;
								StateVariationList[wStateCount++].lStockControlID = pStockManagerItem->lStockControlID;

								//����
								m_MutexData.RoutineMutex[nListIndex].StockMutext[nItemIndex].Unlock();
							}
							else
							{
								CTraceService::TraceString(TEXT("��Ϸ[%d]����б����Ҳ������õĿ��п��(StockID=%d)"),TraceLevel_Exception, pStockItem->wKindID, pStockItem->wStockID);
							}
						}
					}
				}
			}

			pStockManagerItem = m_StockStockItem.GetNextStockItem(pStockItemCurrent);
		}

		//֪ͨ���
		if (m_pStockManagerSink != NULL)
		{
			//״̬���
			if (wStateCount > 0)
			{
				m_pStockManagerSink->OnEventStockStateUpdate(StateVariationList, wStateCount);				
			}			
		}
	}	
	
	//���ñ���
	wStateCount = 0;
	pStockItem = NULL;
	pStockManagerItem = m_StockStockItem.GetHeadStockItem();

	//����Դ���
	while (pStockManagerItem != NULL)
	{
		auto pStockItemCurrent = pStockManagerItem;
		auto pStockItem = &pStockManagerItem->StockItem;

		//�ֶ�ģʽ
		if (pStockItem->cbEnableMode == STOCK_ENABLE_MODE_MANUAL)
		{
			//���δ����
			if (pStockItem->cbStockState == STOCK_STATE_DISABLE)
			{
				//��������
				bool bEnableStock = true;

				//���ÿ��
				if (m_bManageState && dwCurrentTime >= pStockItem->dwEnableStartTime  && dwCurrentTime < pStockItem->dwEnableEndTime)
				{
					//�������
					for (int i = 0; i < CountArray(m_ShareData.ActivityStock); i++)
					{
						auto pShareStockItem = &m_ShareData.ActivityStock[i].StockItem;
						if (pShareStockItem->wStockID != 0 && pShareStockItem->cbStockState==STOCK_STATE_ENABLE && pShareStockItem->wStockLevel>pStockItem->wStockLevel)
						{
							auto pSourceStockItem = m_StockStockItem.SearchStockItem(pShareStockItem->wStockID);

							for (int j = 0; j < pSourceStockItem->MergeRecord.wStockCount; j++)
							{
								if (pSourceStockItem->MergeRecord.wSublayerStockID[j] == pStockItem->wStockID)
								{
									bEnableStock = false;
									break;
								}
							}

							if (!bEnableStock) break;
						}
					}

					//��֮�����Ŀ��δ���û���δ������
					if (bEnableStock)
					{
						//������
						auto nStockIndex = SearchActivityStockItem(pStockItem->wStockID);
						if (nStockIndex == INVALID_INDEX)
						{
							nStockIndex = ActiveActivityStockItem();
							if (nStockIndex != INVALID_INDEX)
							{
								CopyMemory(&m_ShareData.ActivityStock[nStockIndex].StockItem, pStockItem,sizeof(tagStockItem));
							}							
						}
						if (nStockIndex != INVALID_INDEX)
						{
							//�ϲ����
							MergeRoutineStock(nStockIndex);
						}
						else
						{
							CTraceService::TraceString(TEXT("���[%d]����ʧ�ܣ��޷����乲���棡"), TraceLevel_Exception, pStockItem->wStockID);
						}
					}
				}
			}

			//���������
			if (pStockItem->cbStockState == STOCK_STATE_ENABLE)
			{
				//ͣ�ÿ��
				if (!m_bManageState || dwCurrentTime >= pStockItem->dwEnableEndTime)
				{
					//���ҿ��
					auto nStockIndex = SearchActivityStockItem(pStockItem->wStockID);
					if (nStockIndex != INVALID_INDEX)
					{
						//���ѿ��
						FissionActivityStock(nStockIndex);
					}
					else
					{
						CTraceService::TraceString(TEXT("���[%d]ͣ��ʧ�ܣ�δ�ҵ���������Ϣ��"), TraceLevel_Exception, pStockItem->wStockID);
					}
				}
			}
		}

		//�Զ�ģʽ
		if (!m_bManageState && pStockItem->cbEnableMode == STOCK_ENABLE_MODE_AUTO)
		{
			//ͣ�ÿ��
			if (pStockItem->cbStockState == STOCK_STATE_ENABLE)
			{
				//����״̬
				pStockItem->cbStockState = STOCK_STATE_DISABLE;

				//���ҿ���б�
				auto nListIndex = SearchRoutineStockList(pStockItem->wStockKind, pStockItem->wKindID);
				if (nListIndex != INVALID_INDEX)
				{
					//��������
					auto nItemIndex = SearchRoutineStockItem(nListIndex, pStockItem->wStockKind, pStockItem->lBetAmount);
					if (nItemIndex == INVALID_INDEX)
					{
						//����
						m_MutexData.RoutineMutex[nListIndex].StockMutext[nItemIndex].Lock();

						//����״̬
						m_ShareData.RoutineStock[nListIndex].ShareStock[nItemIndex].StockItem.cbStockState = STOCK_STATE_DISABLE;

						//����
						m_MutexData.RoutineMutex[nListIndex].StockMutext[nItemIndex].Unlock();
					}
				}

				//״̬���
				StateVariationList[wStateCount].wStockID = pStockItem->wStockID;
				StateVariationList[wStateCount].cbStockState = STOCK_STATE_DISABLE;
				StateVariationList[wStateCount++].lStockControlID = pStockManagerItem->lStockControlID;			
			}			
		}

		pStockManagerItem = m_StockStockItem.GetNextStockItem(pStockItemCurrent);
	}

	//֪ͨ���
	if (m_pStockManagerSink != NULL)
	{
		//״̬���
		if (wStateCount > 0) m_pStockManagerSink->OnEventStockStateUpdate(StateVariationList, wStateCount);
	}
}

//���仯
VOID CShareStockManager::DetectStockChange()
{
	//��������
	WORD wStateCount = 0;
	tagStateVariationItem StateVariationList[SHARE_STOCK_COUNT] = {};

	//���Ԥ��
	WORD wStockAlarmCount = 0;
	tagStockAlarmItem StockAlarmList[SHARE_STOCK_COUNT] = {};

	//�����
	WORD wStockCount = 0;
	tagStockVariationItem StockVariationList[SHARE_STOCK_COUNT] = {};	

	//����Ԥ��
	WORD wJackpotAlarmCount = 0;
	tagJackpotAlarmItem JackpotAlarmList[SHARE_STOCK_COUNT] = {};
	
	//���ر��
	WORD wJackpotCount = 0;
	tagJackpotVariationItem JackpotVariationList[SHARE_STOCK_COUNT * 2] = {};

	//����
	for (int i = 0; i < CountArray(m_ShareData.ActivityStock); i++)
	{
		//��ȡ����
		auto pShareStockItem = &m_ShareData.ActivityStock[i];
		if (pShareStockItem->StockItem.wStockID != 0)
		{
			auto pSourceStockItem = m_StockStockItem.SearchStockItem(pShareStockItem->StockItem.wStockID);
			if (pSourceStockItem == NULL)
			{
				CTraceService::TraceString(TEXT("δ�ҵ�Դ���%d"), TraceLevel_Exception, pShareStockItem->StockItem.wStockID);
				continue;
			}

			//������־
			bool bStockAlarm = false;

			//����
			m_MutexData.ActivityMutex[i].Lock();

			//У����
			if (pShareStockItem->StockItem.lStockScore != pSourceStockItem->StockItem.lStockScore || pShareStockItem->StockItem.lShrinkProfit != pSourceStockItem->StockItem.lShrinkProfit)
			{
				//�����
				StockVariationList[wStockCount].wStockID = pShareStockItem->StockItem.wStockID;
				StockVariationList[wStockCount].lVariationScore = pShareStockItem->StockItem.lStockScore - pSourceStockItem->StockItem.lStockScore;
				StockVariationList[wStockCount++].lVariationProfit = pShareStockItem->StockItem.lShrinkProfit - pSourceStockItem->StockItem.lShrinkProfit;

				//��渺��
				if (pShareStockItem->StockItem.lStockScore < 0)
				{
					bStockAlarm = true;
					StockAlarmList[wStockAlarmCount].wKindID = pShareStockItem->StockItem.wKindID;
					StockAlarmList[wStockAlarmCount].wStockID = pShareStockItem->StockItem.wStockID;
					StockAlarmList[wStockAlarmCount].lBetAmount = pShareStockItem->StockItem.lBetAmount;
					StockAlarmList[wStockAlarmCount++].lStockScore = pShareStockItem->StockItem.lStockScore;
				}

				//���¿��
				pSourceStockItem->StockItem.lStockScore = pShareStockItem->StockItem.lStockScore;
				pSourceStockItem->StockItem.lShrinkProfit = pShareStockItem->StockItem.lShrinkProfit;
			}			

			//У��ʽ�
			for (int j = 0; j < pShareStockItem->StockItem.wJackpotCount; j++)
			{
				//�ʽ���
				JackpotVariationList[wJackpotCount].wStockID = pShareStockItem->StockItem.wStockID;
				JackpotVariationList[wJackpotCount].wLevelID = pShareStockItem->StockItem.GameJackpotItem[j].wLevelID;
				JackpotVariationList[wJackpotCount].lVariationScore = pShareStockItem->StockItem.GameJackpotItem[j].lStockScore - pSourceStockItem->StockItem.GameJackpotItem[j].lStockScore;
				
				//���²ʽ�
				pSourceStockItem->StockItem.GameJackpotItem[j].lStockScore = pShareStockItem->StockItem.GameJackpotItem[j].lStockScore;

				//У����
				if (abs(JackpotVariationList[wJackpotCount].lVariationScore) != 0)
				{
					wJackpotCount++;

					if (pShareStockItem->StockItem.GameJackpotItem[j].lStockScore < 0)
					{
						bStockAlarm = true;
						JackpotAlarmList[wJackpotAlarmCount].wKindID = pShareStockItem->StockItem.wKindID;
						JackpotAlarmList[wJackpotAlarmCount].wStockID = pShareStockItem->StockItem.wStockID;
						JackpotAlarmList[wJackpotAlarmCount].lBetAmount = pShareStockItem->StockItem.lBetAmount;
						JackpotAlarmList[wJackpotAlarmCount].wLevelID = pShareStockItem->StockItem.GameJackpotItem[j].wLevelID;
						JackpotAlarmList[wJackpotAlarmCount++].lStockScore = pShareStockItem->StockItem.GameJackpotItem[j].lStockScore;
					}
				}
			}	

			//����
			m_MutexData.ActivityMutex[i].Unlock();

			//���渺��ǰ���� 
			if (bStockAlarm == true) pSourceStockItem->StockItem.dwEnableEndTime = time(NULL);
		}
	}

	//������
	for (int i = 0; i < CountArray(m_ShareData.RoutineStock); i++)
	{
		//��ȡ����
		auto pShareStockList = &m_ShareData.RoutineStock[i];
		for (int j = 0; j < CountArray(pShareStockList->ShareStock); j++)
		{
			auto pShareStockItem = &pShareStockList->ShareStock[j];

			if (pShareStockItem->StockItem.wStockID != 0)
			{
				auto pSourceStockItem = m_StockStockItem.SearchStockItem(pShareStockItem->StockItem.wStockID);
				if (pSourceStockItem == NULL)
				{
					CTraceService::TraceString(TEXT("δ�ҵ�Դ���%d"), TraceLevel_Exception, pShareStockItem->StockItem.wStockID);
					continue;
				}

				//������־
				bool bStockAlarm = false;

				//����
				m_MutexData.RoutineMutex->StockMutext[j].Lock();

				//У����
				if (pShareStockItem->StockItem.lStockScore != pSourceStockItem->StockItem.lStockScore || pShareStockItem->StockItem.lShrinkProfit != pSourceStockItem->StockItem.lShrinkProfit)
				{
					//�����
					StockVariationList[wStockCount].wStockID = pShareStockItem->StockItem.wStockID;
					StockVariationList[wStockCount].lVariationScore = pShareStockItem->StockItem.lStockScore - pSourceStockItem->StockItem.lStockScore;
					StockVariationList[wStockCount++].lVariationProfit = pShareStockItem->StockItem.lShrinkProfit - pSourceStockItem->StockItem.lShrinkProfit;

					//��渺��
					if (pShareStockItem->StockItem.lStockScore < 0)
					{
						bStockAlarm = true;
						StockAlarmList[wStockAlarmCount].wKindID = pShareStockItem->StockItem.wKindID;
						StockAlarmList[wStockAlarmCount].wStockID = pShareStockItem->StockItem.wStockID;
						StockAlarmList[wStockAlarmCount].lBetAmount = pShareStockItem->StockItem.lBetAmount;
						StockAlarmList[wStockAlarmCount++].lStockScore = pShareStockItem->StockItem.lStockScore;
					}

					//���¿��
					pSourceStockItem->StockItem.lStockScore = pShareStockItem->StockItem.lStockScore;
					pSourceStockItem->StockItem.lShrinkProfit = pShareStockItem->StockItem.lShrinkProfit;
				}				

				//У��ʽ�
				for (int k = 0; k < pShareStockItem->StockItem.wJackpotCount; k++)
				{
					//�ʽ���
					JackpotVariationList[wJackpotCount].wStockID = pShareStockItem->StockItem.wStockID;
					JackpotVariationList[wJackpotCount].wLevelID = pShareStockItem->StockItem.GameJackpotItem[k].wLevelID;
					JackpotVariationList[wJackpotCount].lVariationScore = pShareStockItem->StockItem.GameJackpotItem[k].lStockScore - pSourceStockItem->StockItem.GameJackpotItem[k].lStockScore;
					
					//���²ʽ�
					pSourceStockItem->StockItem.GameJackpotItem[k].lStockScore = pShareStockItem->StockItem.GameJackpotItem[k].lStockScore;

					//У����
					if (abs(JackpotVariationList[wJackpotCount].lVariationScore) != 0)
					{
						wJackpotCount++;

						if (pShareStockItem->StockItem.GameJackpotItem[k].lStockScore < 0)
						{
							bStockAlarm = true;
							JackpotAlarmList[wJackpotAlarmCount].wKindID = pShareStockItem->StockItem.wKindID;
							JackpotAlarmList[wJackpotAlarmCount].wStockID = pShareStockItem->StockItem.wStockID;
							JackpotAlarmList[wJackpotAlarmCount].lBetAmount = pShareStockItem->StockItem.lBetAmount;
							JackpotAlarmList[wJackpotAlarmCount].wLevelID = pShareStockItem->StockItem.GameJackpotItem[k].wLevelID;
							JackpotAlarmList[wJackpotAlarmCount++].lStockScore = pShareStockItem->StockItem.GameJackpotItem[k].lStockScore;
						}
					}
				}

				//���渺ͣ�ÿ��
				if (bStockAlarm == true)
				{
					//����״̬
					pSourceStockItem->StockItem.cbStockState = STOCK_STATE_DISABLE;
					ZeroMemory(&pShareStockItem->StockItem,sizeof(pShareStockItem->StockItem));

					//״̬���
					StateVariationList[wStateCount].cbStockState = STOCK_STATE_DISABLE;
					StateVariationList[wStateCount].wStockID = pSourceStockItem->StockItem.wStockID;
					StateVariationList[wStateCount++].lStockControlID = pSourceStockItem->lStockControlID;
				}

				//����
				m_MutexData.RoutineMutex->StockMutext[j].Unlock();
			}
		}
	}

	//֪ͨ���
	if (m_pStockManagerSink != NULL)
	{
		//�����
		if (wStockCount > 0) m_pStockManagerSink->OnEventStockScoreUpdate(StockVariationList, wStockCount);

		//�ʽ���
		if (wJackpotCount > 0) m_pStockManagerSink->OnEventJackpotScoreUpdate(JackpotVariationList, wJackpotCount);

		//״̬���
		if (wStateCount > 0) m_pStockManagerSink->OnEventStockStateUpdate(StateVariationList, wStateCount);

		//���Ԥ��
		if (wStockAlarmCount > 0) m_pStockManagerSink->OnEventStockScoreAlarm(StockAlarmList, wStockAlarmCount);

		//�ʳ�Ԥ��
		if (wJackpotAlarmCount > 0) m_pStockManagerSink->OnEventJackpotScoreAlarm(JackpotAlarmList, wJackpotAlarmCount);		
	}
}

//�ϲ����
bool CShareStockManager::MergeRoutineStock(int nStockIndex)
{
	//У�����
	ASSERT (nStockIndex!=INVALID_INDEX);
	if (nStockIndex == INVALID_INDEX) return false;

	//��ȡ���
	auto pShareStockItem = &m_ShareData.ActivityStock[nStockIndex];

	//���ҿ��
	auto pSourceStockItem = m_StockStockItem.SearchStockItem(pShareStockItem->StockItem.wStockID);
	if (pSourceStockItem == NULL) return false;

	//��������
	ZeroMemory(&pSourceStockItem->MergeRecord,sizeof(pSourceStockItem->MergeRecord));

	//��������
	WORD wStateCount = 0;	
	tagStateVariationItem StateVariationList[SHARE_STOCK_COUNT] = {0};

	//����
	m_MutexData.ActivityMutex[nStockIndex].Lock();

	//���������
	pShareStockItem->StockItem.lStockScore = pShareStockItem->StockItem.lExtraScore;

	//���ض������
	for (int i = 0; i < pShareStockItem->StockItem.wJackpotCount; i++)
	{
		pShareStockItem->StockItem.GameJackpotItem[i].lStockScore = pShareStockItem->StockItem.GameJackpotItem[i].lExtraScore;
	}

	//����״̬
	pSourceStockItem->lStockControlID = ++m_lStockControlID;
	pShareStockItem->StockItem.cbStockState = STOCK_STATE_ENABLE;
	pSourceStockItem->StockItem.cbStockState = STOCK_STATE_ENABLE;

	//��¼���
	StateVariationList[wStateCount].wStockID = pShareStockItem->StockItem.wStockID;
	StateVariationList[wStateCount].lStockControlID = pSourceStockItem->lStockControlID;
	StateVariationList[wStateCount++].cbStockState = pShareStockItem->StockItem.cbStockState;
	
	//����
	m_MutexData.ActivityMutex[nStockIndex].Unlock();

	//��ȡ����
	auto pStockItem = &pSourceStockItem->StockItem;
	auto pMergeRecord = &pSourceStockItem->MergeRecord;

	//�ռ���������
	CollectRelationKindID(pMergeRecord, pStockItem);

	//����ϲ�
	for (int i = 0; i < pMergeRecord->wKindCount; i++)
	{
		auto nListIndex = SearchRoutineStockList(pStockItem->wStockKind, pMergeRecord->wKindIDList[i]);
		if (nListIndex != INVALID_INDEX)
		{
			//��ȡ����
			auto pStockList = &m_ShareData.RoutineStock[nListIndex];
			auto pMutexList = &m_MutexData.RoutineMutex[nListIndex];

			//����		
			pMutexList->ListMutex.Lock();

			//�������
			pStockList->pRelationStock = pShareStockItem;			

			//ͳ�ƿ��Ͳʽ�
			for (int j = 0; j < CountArray(pStockList->ShareStock); j++)
			{
				//����
				pMutexList->StockMutext[j].Lock();				
				m_MutexData.ActivityMutex[nStockIndex].Lock();

				if (pStockList->ShareStock[j].StockItem.cbStockState==STOCK_STATE_ENABLE)
				{
					auto pTempStockItem = m_StockStockItem.SearchStockItem(pStockList->ShareStock[j].StockItem.wStockID);
					if (pTempStockItem != NULL)
					{

						//ͳ�ƿ��
						pMergeRecord->wCollectStockScore += pStockList->ShareStock[j].StockItem.lStockScore;
						pShareStockItem->StockItem.lStockScore += pStockList->ShareStock[j].StockItem.lStockScore;

						//ͳ�Ʋʽ�
						for (int k = 0; k < pShareStockItem->StockItem.wJackpotCount; k++)
						{
							pMergeRecord->wCollectJackpotScore[k] += pStockList->ShareStock[j].StockItem.GameJackpotItem[k].lStockScore;
							pShareStockItem->StockItem.GameJackpotItem[k].lStockScore += pStockList->ShareStock[j].StockItem.GameJackpotItem[k].lStockScore;
						}

						//�޸�״̬
						pTempStockItem->StockItem.cbStockState = STOCK_STATE_LOCKED;
						pStockList->ShareStock[j].StockItem.cbStockState = STOCK_STATE_LOCKED;

						//״̬���
						StateVariationList[wStateCount].lStockControlID = pTempStockItem->lStockControlID;
						StateVariationList[wStateCount].wStockID = pStockList->ShareStock[j].StockItem.wStockID;						
						StateVariationList[wStateCount++].cbStockState = pStockList->ShareStock[j].StockItem.cbStockState;
					}
				}

				//����
				pMutexList->StockMutext[j].Unlock();
				m_MutexData.ActivityMutex[nStockIndex].Unlock();
			}

			//����
			pMutexList->ListMutex.Unlock();			
		}
	}

	//�������
	WORD wJackpotCount = 0;
	tagStockVariationItem StockVariationItem = {};
	tagJackpotVariationItem JackpotVariationItem[MAX_JACKPOT_COUNT] = {};

	//����
	m_MutexData.ActivityMutex[nStockIndex].Lock();

	//�����
	StockVariationItem.wStockID = pShareStockItem->StockItem.wStockID;
	StockVariationItem.lVariationScore = pShareStockItem->StockItem.lStockScore-pSourceStockItem->StockItem.lStockScore;

	//��¼���
	pSourceStockItem->StockItem.lStockScore = pShareStockItem->StockItem.lStockScore;

	//�ʽ�����
	wJackpotCount = pShareStockItem->StockItem.wJackpotCount;

	//ͳ�Ʋʽ�
	for (int i = 0; i < wJackpotCount; i++)
	{
		//�ʽ���
		JackpotVariationItem[i].wStockID = pShareStockItem->StockItem.wStockID;
		JackpotVariationItem[i].wLevelID = pShareStockItem->StockItem.GameJackpotItem[i].wLevelID;
		JackpotVariationItem[i].lVariationScore = pShareStockItem->StockItem.GameJackpotItem[i].lStockScore-pSourceStockItem->StockItem.GameJackpotItem[i].lStockScore;

		//��¼�ʽ�
		pSourceStockItem->StockItem.GameJackpotItem[i].lStockScore = pShareStockItem->StockItem.GameJackpotItem[i].lStockScore;
	}	

	//����
	m_MutexData.ActivityMutex[nStockIndex].Unlock();

	//֪ͨ���
	if (m_pStockManagerSink != NULL)
	{
		//�������
		m_pStockManagerSink->OnEventStockScoreUpdate(&StockVariationItem, 1);

		//�ʽ���
		if (wJackpotCount>0) m_pStockManagerSink->OnEventJackpotScoreUpdate(JackpotVariationItem, wJackpotCount);

		//״̬���
		if (wStateCount > 0) m_pStockManagerSink->OnEventStockStateUpdate(StateVariationList, wStateCount);
	}	

	return true;
}

//���ѿ��
bool CShareStockManager::FissionActivityStock(int nStockIndex)
{
	//У�����
	ASSERT(nStockIndex != INVALID_INDEX);
	if (nStockIndex == INVALID_INDEX) return false;

	//��ȡ���
	auto pShareStockItem = &m_ShareData.ActivityStock[nStockIndex];

	//���ҿ��
	auto pSourceStockItem = m_StockStockItem.SearchStockItem(pShareStockItem->StockItem.wStockID);
	if (pSourceStockItem == NULL) return false;

	//��ȡ����
	auto pStockItem = &pSourceStockItem->StockItem;
	auto pMergeRecord = &pSourceStockItem->MergeRecord;

	//��������
	WORD wStateCount = 0,wStockCount = 0,wJackpotCount = 0;
	tagStateVariationItem StateVariationList[SHARE_STOCK_COUNT] = {};
	tagStockVariationItem StockVariationList[SHARE_STOCK_COUNT] = {};			
	tagJackpotVariationItem JackpotVariationList[SHARE_STOCK_COUNT*2] = {};

	//�������
	m_MutexData.ActivityMutex[nStockIndex].Lock();

	//��������
	double dStockRatio=0.f;
	SCORE lStockScore=0;
	SCORE lShareStockScore=0;					
	SCORE lResidueStockScore = 0;
	SCORE lShareJackpotScore[MAX_JACKPOT_COUNT] = {0};
	SCORE lResidueJackpotScore[MAX_JACKPOT_COUNT] = { 0 };

	//��¼��ʼ����
	lShareStockScore = __max(pShareStockItem->StockItem.lStockScore,0);
	lResidueStockScore = lShareStockScore;
	for (int i = 0; i < pShareStockItem->StockItem.wJackpotCount; i++)
	{
		lShareJackpotScore[i] = __max(pShareStockItem->StockItem.GameJackpotItem[i].lStockScore,0);
		lResidueJackpotScore[i] = lShareJackpotScore[i];
	}

	for (int i = 0; i < pMergeRecord->wKindCount; i++)
	{
		auto nListIndex = SearchRoutineStockList(pStockItem->wStockKind, pMergeRecord->wKindIDList[i]);
		if (nListIndex != INVALID_INDEX)
		{
			//��ȡ����
			auto pStockList = &m_ShareData.RoutineStock[nListIndex];
			auto pMutexList = &m_MutexData.RoutineMutex[nListIndex];

			//����
			pMutexList->ListMutex.Lock();

			//ͳ�ƿ��Ͳʽ�
			for (int j = 0; j < CountArray(pStockList->ShareStock); j++)
			{
				//����
				pMutexList->StockMutext[j].Lock();

				if (pStockList->ShareStock[j].StockItem.cbStockState==STOCK_STATE_LOCKED)
				{
					//���ҿ��
					auto pTempStockItem = m_StockStockItem.SearchStockItem(pStockList->ShareStock[j].StockItem.wStockID);
					if (pTempStockItem != NULL)
					{
						//�������
						dStockRatio= pMergeRecord->wCollectStockScore?static_cast<double>(pStockList->ShareStock[j].StockItem.lStockScore / (double)(pMergeRecord->wCollectStockScore)):0;						

						//���ѿ��
						lStockScore = static_cast<SCORE>(lShareStockScore * dStockRatio);
						if (lResidueStockScore >= lStockScore)
						{
							lResidueStockScore -= lStockScore;
							pStockList->ShareStock[j].StockItem.lStockScore = lStockScore;							
						}
						else
						{
							pStockList->ShareStock[j].StockItem.lStockScore = lResidueStockScore;
							lResidueStockScore = 0;
						}

						//�����
						StockVariationList[wStockCount].wStockID= pStockList->ShareStock[j].StockItem.wStockID;
						StockVariationList[wStockCount++].lVariationScore = pStockList->ShareStock[j].StockItem.lStockScore - pTempStockItem->StockItem.lStockScore;
						pTempStockItem->StockItem.lStockScore = pStockList->ShareStock[j].StockItem.lStockScore;

						//���Ѳʽ�
						for (int k = 0; k < pShareStockItem->StockItem.wJackpotCount; k++)
						{
							//�������
							dStockRatio = pMergeRecord->wCollectJackpotScore[k]?static_cast<double>(pStockList->ShareStock[j].StockItem.GameJackpotItem[k].lStockScore / (double)(pMergeRecord->wCollectJackpotScore[k])):0;

							//���Ѳʽ�
							lStockScore = static_cast<SCORE>(lShareJackpotScore[k] * dStockRatio);
							if (lResidueJackpotScore[k] >= lStockScore)
							{
								lResidueJackpotScore[k] -= lStockScore;
								pStockList->ShareStock[j].StockItem.GameJackpotItem[k].lStockScore = lStockScore;
							}
							else
							{
								pStockList->ShareStock[j].StockItem.GameJackpotItem[k].lStockScore = lResidueJackpotScore[k];
								lResidueJackpotScore[k] = 0;
							}							

							//�ʽ���							
							JackpotVariationList[wJackpotCount].wStockID = pStockList->ShareStock[j].StockItem.wStockID;
							JackpotVariationList[wJackpotCount].wLevelID = pStockList->ShareStock[j].StockItem.GameJackpotItem[k].wLevelID;
							JackpotVariationList[wJackpotCount++].lVariationScore = pStockList->ShareStock[j].StockItem.GameJackpotItem[k].lStockScore - pTempStockItem->StockItem.GameJackpotItem[k].lStockScore;
							pTempStockItem->StockItem.GameJackpotItem[k].lStockScore = pStockList->ShareStock[j].StockItem.GameJackpotItem[k].lStockScore;
						}

						//�޸�״̬
						if (m_bManageState)
						{
							pTempStockItem->lStockControlID = ++m_lStockControlID;
							pTempStockItem->StockItem.cbStockState = STOCK_STATE_ENABLE;
							pStockList->ShareStock[j].StockItem.cbStockState = STOCK_STATE_ENABLE;
						}
						else
						{
							pTempStockItem->lStockControlID = 0;
							pTempStockItem->StockItem.cbStockState = STOCK_STATE_DISABLE;
							pStockList->ShareStock[j].StockItem.cbStockState = STOCK_STATE_DISABLE;
						}
						
						//״̬���
						StateVariationList[wStateCount].lStockControlID = pTempStockItem->lStockControlID;
						StateVariationList[wStateCount].wStockID = pStockList->ShareStock[j].StockItem.wStockID;						
						StateVariationList[wStateCount++].cbStockState = pStockList->ShareStock[j].StockItem.cbStockState;
					}
				}

				//����
				pMutexList->StockMutext[j].Unlock();
			}

			//ȡ������					
			pStockList->pRelationStock = NULL;

			//����
			pMutexList->ListMutex.Unlock();
		}
	}

	//����״̬
	pShareStockItem->StockItem.cbStockState = STOCK_STATE_DISABLE;
	pSourceStockItem->StockItem.cbStockState = STOCK_STATE_DISABLE;

	//״̬���
	StateVariationList[wStateCount].wStockID = pSourceStockItem->StockItem.wStockID;
	StateVariationList[wStateCount].lStockControlID = pSourceStockItem->lStockControlID;
	StateVariationList[wStateCount++].cbStockState = pSourceStockItem->StockItem.cbStockState;
	
	//�����	
	StockVariationList[wStockCount].wStockID = pSourceStockItem->StockItem.wStockID;
	StockVariationList[wStockCount++].lVariationScore = pShareStockItem->StockItem.lStockScore - pSourceStockItem->StockItem.lStockScore;
	
	//���¿��
	pSourceStockItem->StockItem.lStockScore = pShareStockItem->StockItem.lStockScore;

	//�ʽ���
	for (int i = 0; i < pSourceStockItem->StockItem.wJackpotCount; i++)
	{
		//�ʽ���
		JackpotVariationList[wJackpotCount].wStockID = pSourceStockItem->StockItem.wStockID;
		JackpotVariationList[wJackpotCount].wLevelID = pSourceStockItem->StockItem.GameJackpotItem[i].wLevelID;
		JackpotVariationList[wJackpotCount++].lVariationScore = pShareStockItem->StockItem.GameJackpotItem[i].lStockScore-pSourceStockItem->StockItem.GameJackpotItem[i].lStockScore;
		
		//���²ʽ�
		pSourceStockItem->StockItem.GameJackpotItem[i].lStockScore = pShareStockItem->StockItem.GameJackpotItem[i].lStockScore;
	}

	//�������
	m_MutexData.ActivityMutex[nStockIndex].Unlock();

	//֪ͨ���
	if (m_pStockManagerSink != NULL)
	{
		//�������
		if (wStockCount>0) m_pStockManagerSink->OnEventStockScoreUpdate(StockVariationList, wStockCount);

		//�ʽ���
		if (wJackpotCount>0) m_pStockManagerSink->OnEventJackpotScoreUpdate(JackpotVariationList, wJackpotCount);

		//״̬���
		if (wStateCount > 0) m_pStockManagerSink->OnEventStockStateUpdate(StateVariationList, wStateCount);
	}

	return true;
}

//�ռ�����
bool CShareStockManager::CollectRelationKindID(tagStockMergeRecord* pStockMergeRecord, tagStockItem* pStockItem)
{
	//��Ϸ���
	if (pStockItem->wStockLevel == STOCK_LEVEL_GAME)
	{
		//��������
		for (int j = 0; j < pStockMergeRecord->wKindCount; j++)
		{
			if (pStockMergeRecord->wKindIDList[j] == pStockItem->wKindID)
			{
				return true;
			}
		}

		//�������
		pStockMergeRecord->wKindIDList[pStockMergeRecord->wKindCount++] = pStockItem->wKindID;

		return true;
	}

	for (int i = 0; i < pStockItem->wRelationCount; i++)
	{
		auto pTempStockItem = m_StockStockItem.SearchStockItem(pStockItem->RelationStockID[i]);
		if (pTempStockItem->StockItem.wStockLevel >= STOCK_LEVEL_GAME)
		{
			//���ѿ��
			if (pTempStockItem->StockItem.cbStockState==STOCK_STATE_ENABLE)
			{
				auto nItemIndex = SearchActivityStockItem(pTempStockItem->StockItem.wStockID);
				if (nItemIndex != NULL) FissionActivityStock(nItemIndex);				
			}

			//��ӿ��
			pStockMergeRecord->wSublayerStockID[pStockMergeRecord->wStockCount++] = pTempStockItem->StockItem.wStockID;

			//�ݹ����			
			CollectRelationKindID(pStockMergeRecord, &pTempStockItem->StockItem);
		}		
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
