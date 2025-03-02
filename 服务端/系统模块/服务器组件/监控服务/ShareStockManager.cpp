#include "StdAfx.h"
#include "ShareStockManager.h"

////////////////////////////////////////////////////////////////////////
// 
#define INVALID_INDEX			-1

////////////////////////////////////////////////////////////////////////
//互斥对象名

//库存初始化
LPCTSTR g_pszMutexNameStockInit = TEXT("GlobalMutex_1D3AF3B1_42AD_4CE4_B1D9_36BF542772E0");

//库存类型
LPCTSTR g_pszMutexNameStockItem = TEXT("GlobalMutex_060E5C57_E9EC_4E22_B97A_1716D0CF8098_%d"); 

////////////////////////////////////////////////////////////////////////

//共享开始
#pragma data_seg("SharedStock") 

//共享库存数据
bool m_bStockInit = false;
DWORD m_dwVisitorCount = 0;

tagShareData m_ShareData = {};

//共享结束
#pragma data_seg() 

//////////////////////////////////////////////////////////////////////////////////
//链接选项
#pragma comment(linker, "/section:SharedStock,rws")

//////////////////////////////////////////////////////////////////////////////////
//构造函数
CShareMutex::CShareMutex()
{
	//设置变量
	m_hMutex = NULL;
	m_dwMutexNo = 0;
	m_nLockCount = 0;
	m_ulLockTickCount = 0;
}

//析构函数
CShareMutex::~CShareMutex()
{
	//关闭句柄
	CloseMutex();
}

//校验句柄
bool CShareMutex::IsMutexValid()
{
	return m_hMutex != NULL;
}

//关闭句柄
VOID CShareMutex::CloseMutex()
{
	//释放句柄
	if (m_hMutex != NULL)
	{
		//解锁
		if (m_nLockCount > 0) ReleaseMutex(m_hMutex);

		//关闭句柄
		CloseHandle(m_hMutex);
	}

	//设置变量
	m_hMutex = NULL;
	m_dwMutexNo = 0;
	m_nLockCount = 0;
	m_ulLockTickCount = 0;
}

//初始化
VOID CShareMutex::InitMutex(LPCTSTR pszMutexPrefix, DWORD dwMutexNo)
{
	//校验句柄
	ASSERT(m_hMutex==NULL);
	if (m_hMutex != NULL) CloseMutex();

	//构造名称
	TCHAR szMutexName[128];
	_sntprintf_s(szMutexName,CountArray(szMutexName),TEXT("%s_%d"), pszMutexPrefix, dwMutexNo);

	//创建对象
	m_dwMutexNo = dwMutexNo;
	m_hMutex = CreateMutex(NULL, FALSE, szMutexName);
}

//获取时间戳
ULONGLONG CShareMutex::GetLockTickCount()
{
	//锁定数据
	CThreadLock Section(m_CriticalSection);

	return m_ulLockTickCount;
}

//设置时间戳
VOID CShareMutex::SetLockTickCount(ULONGLONG ulTickCount)
{
	//锁定数据
	CThreadLock Section(m_CriticalSection);

	m_ulLockTickCount = ulTickCount;
}

//锁定对象
DWORD CShareMutex::Lock(DWORD dwMilliseconds)
{
	//校验句柄
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
	
//解锁对象
VOID CShareMutex::Unlock(WORD wStockKind,bool bForce)
{
	//CTraceService::TraceString(TEXT("Unlock MutextNo=%d"), TraceLevel_Debug, m_dwMutexNo);

	//校验句柄
	if (m_hMutex == NULL) return;

	//调整变量
	if (bForce == true && m_nLockCount != 1)
	{
		m_nLockCount = 1;
	}

	//解锁对象
	if (m_nLockCount>0 && --m_nLockCount==0)
	{
		static DWORD dwLockCount = 0;

		dwLockCount++;
		auto ulTickCount = GetTickCount64();
		if (ulTickCount > m_ulLockTickCount)
		{
			CTraceService::TraceString(TEXT("锁定次数: %d, 锁定时长：%dms,类型%d,MutexNo=%d"), TraceLevel_Exception, dwLockCount, ulTickCount - m_ulLockTickCount, wStockKind,m_dwMutexNo);
		}

		//重置时间戳
		SetLockTickCount(0);

		//解锁互斥
		bool bResult=ReleaseMutex(m_hMutex);
		if (bResult == false)
		{
			CTraceService::TraceString(TEXT("解锁失败：ErrorID=%d"), TraceLevel_Exception, GetLastError());
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////
//构造函数
CShareStockItem::CShareStockItem(DWORD dwUserLabel, CShareMutex* pShareMutex, tagShareStock* pShareStock)
{
	m_dwUserLabel = dwUserLabel;
	m_pShareStock = pShareStock;
	m_pShareMutex = pShareMutex;
}

//析构函数
CShareStockItem::~CShareStockItem()
{
	//解锁对象
	if (m_pShareMutex != NULL)
	{
		m_pShareMutex->Unlock(m_pShareStock->StockItem.wStockKind);
	}

	//设置变量
	m_pShareMutex = NULL;	
	m_pShareStock = NULL;
}

//库存抽水
bool CShareStockItem::StockShrinkAndUpdate(SCORE lBetScore, WORD wJackpotCount)
{
	//抽水变量
	SCORE lShrinkValue = 0;
	SCORE lShrinkTotalValue = 0;
	WORD wShrinkRatio = GetShrinkRatio();

	//计算抽水
	if (lBetScore > 0)
	{
		//库存抽水
		lShrinkValue = lBetScore * wShrinkRatio / 1000;
		lShrinkTotalValue += lShrinkValue;
		m_pShareStock->StockItem.lShrinkProfit += lShrinkValue;

		//彩池抽水
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

	//更新库存
	m_pShareStock->StockItem.lStockScore += (lBetScore - lShrinkTotalValue);

	return true;
}

//获取状态
WORD CShareStockItem::GetStockID()
{
	return m_pShareStock ? m_pShareStock->StockItem.wStockID : 0;
}
	
//库存类型
WORD CShareStockItem::GetStockKind()
{
	return m_pShareStock ? m_pShareStock->StockItem.wStockKind: 0;
}

//抽水比例
WORD CShareStockItem::GetShrinkRatio()
{
	if (m_pShareStock != NULL)
	{
		//获取对象
		auto pStockItem = &m_pShareStock->StockItem;
		auto pStockControl = &m_pShareStock->StockControl;

		//大R玩家
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
	
//当前库存
SCORE CShareStockItem::GetStockScore()
{
	return m_pShareStock ? m_pShareStock->StockItem.lStockScore : 0L;
}
	
//获取彩金
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

//库存信息
tagStockItem * CShareStockItem::GetStockItem()
{
	if (m_pShareStock == NULL)
	{
		return NULL;
	}

	return &m_pShareStock->StockItem;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CShareStockVisitor::CShareStockVisitor()
{
	//设置变量
	m_pStockList = NULL;
	m_wKindID = INVALID_WORD;		

	return;
}

//析构函数
CShareStockVisitor::~CShareStockVisitor()
{	
	//停止服务
	if (IsRuning())
	{
		ConcludeThread(INFINITY);
	}

	//重置对象
	ResetStockVisitor();
}

//重置对象
VOID CShareStockVisitor::ResetStockVisitor()
{
	if (m_pStockList != NULL)
	{
		//设置变量
		m_wKindID = 0;
		m_pStockList = NULL;

		//更新数量
		m_InitMutex.Lock();
		if (m_dwVisitorCount > 0) m_dwVisitorCount--;
		m_InitMutex.Unlock();

		//关闭句柄
		m_InitMutex.CloseMutex();
		m_MutexList.ListMutex.CloseMutex();

		//关闭对象
		for (int i = 0; i < CountArray(m_MutexList.StockMutext); i++)
		{
			m_MutexList.StockMutext[i].CloseMutex();
		}
	}
}

//初始化
bool CShareStockVisitor::InitStockVisitor(WORD wKindID)
{
	if (wKindID==0 || !GetStockInit()) return false;

	//查找游戏
	for (int i = 0; i < CountArray(m_ShareData.RoutineStock); i++)
	{
		if (m_ShareData.RoutineStock[i].wKindID == wKindID)
		{
			m_pStockList = &m_ShareData.RoutineStock[i];
			break;
		}
	}

	//退出判断
	if (m_pStockList == NULL) return false;

	//创建对象
	m_MutexList.ListMutex.InitMutex(g_pszMutexNameStockItem,m_pStockList->dwMutexNo);
	
	//创建对象
	for (int i = 0; i < CountArray(m_pStockList->ShareStock); i++)
	{
		m_MutexList.StockMutext[i].InitMutex(g_pszMutexNameStockItem, m_pStockList->ShareStock[i].dwMutexNo);
	}

	//设置变量
	m_wKindID = wKindID;		

	//更新数量
	m_InitMutex.Lock();
	m_dwVisitorCount++;
	m_InitMutex.Unlock();

	return true;
}

//查找库存
CShareStockItemPtr CShareStockVisitor::GetStockItem(WORD wStockKind, SCORE lBetAmount, DWORD dwUserLabel)
{
	//校验状态
	if (m_pStockList == NULL || m_pStockList->wKindID != m_wKindID)
	{
		return NULL;
	}

	//获取索引
	auto nStockIndex = GetStockIndex(wStockKind, lBetAmount);
	if (nStockIndex < 0)
	{
		return NULL;
	}

	//锁定函数
	m_MutexList.StockMutext[nStockIndex].Lock();

	//获取对象
	auto pShareMutex = &m_MutexList.StockMutext[nStockIndex];
	auto pShareStock = &m_pStockList->ShareStock[nStockIndex];
	auto pRoutineStockItem = &m_pStockList->ShareStock[nStockIndex].StockItem;

	//判断状态
	if (pShareStock->StockItem.cbStockState != STOCK_STATE_ENABLE)
	{
		//解锁
		m_MutexList.StockMutext[nStockIndex].Unlock();

		//上锁
		m_MutexList.ListMutex.Lock();

		//判断库存
		if (m_pStockList->pRelationStock == NULL)
		{
			m_MutexList.ListMutex.Unlock();

			return (pRoutineStockItem->cbStockState!=STOCK_STATE_ENABLE)?NULL:GetStockItem(wStockKind, lBetAmount, dwUserLabel);
		}

		//创建对象
		if (m_MutexList.RelationMutex.GetMutexNo()!= m_pStockList->pRelationStock->dwMutexNo)
		{
			m_MutexList.RelationMutex.CloseMutex();
			m_MutexList.RelationMutex.InitMutex(g_pszMutexNameStockItem,m_pStockList->pRelationStock->dwMutexNo);
		}

		//库存对象
		pShareStock = m_pStockList->pRelationStock;

		//解锁
		m_MutexList.ListMutex.Unlock();		
		
		//上锁
		m_MutexList.RelationMutex.Lock();

		//判断状态
		if (pShareStock->StockItem.cbStockState != STOCK_STATE_ENABLE)
		{
			m_MutexList.RelationMutex.Unlock();

			return (pRoutineStockItem->cbStockState!=STOCK_STATE_ENABLE)?NULL:GetStockItem(wStockKind, lBetAmount, dwUserLabel);
		}

		//互斥对象
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

//运行事件
bool CShareStockVisitor::OnEventThreadRun()
{
#ifndef _DEBUG

	//超时检测
	//for (int i=0; i<CountArray(m_MutexData); i++)
	//{
	//	//获取时间戳
	//	auto ulCurrTickCount = GetTickCount64();
	//	auto ulLockTickCount = m_StockMutex[i].GetLockTickCount();

	//	//超时校验
	//	if (ulLockTickCount > 0 && ulCurrTickCount - ulLockTickCount >= 1200)
	//	{
	//		//ExitProcess(0);			
	//	}
	//}
#endif

	//休眠
	Sleep(100);

	return true;
}

//初始化标志
bool CShareStockVisitor::GetStockInit()
{
	//校验句柄
	if (!m_InitMutex.IsMutexValid())
	{
		m_InitMutex.InitMutex(g_pszMutexNameStockInit, 1);
	}

	//锁定对象
	m_InitMutex.Lock();
	bool bStockInit = m_bStockInit;
	m_InitMutex.Unlock();

	return bStockInit;
}

//类型索引
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
//构造函数
CShareStockManager::CShareStockManager()
{
	//设置变量
	m_bManageState = false;
	m_pStockManagerSink = NULL;
	ZeroMemory(&m_SuperPlayerControl,sizeof(m_SuperPlayerControl));

	//初始化哈希表
	m_StockStockItem.InitHashTable(103);		
}

//析构函数
CShareStockManager::~CShareStockManager()
{
	CloseStockManager();
}

VOID CShareStockManager::StartStockManager(IStockManagerSink* pIStockManagerSink)
{
	//互斥编号
	DWORD dwMutexNo = 1;

	//校验句柄
	if (!m_InitMutex.IsMutexValid())
	{
		m_InitMutex.InitMutex(g_pszMutexNameStockInit, dwMutexNo++);
	}

	//锁定对象
	m_InitMutex.Lock();

	//创建对象
	if (m_bStockInit == false)
	{
		//设置编号		
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

		//设置变量
		m_bStockInit = true;		
	}

	//设置状态
	m_bManageState = true;
	m_lStockControlID = time(NULL)*1000;

	//查询接口
	m_pStockManagerSink = pIStockManagerSink;

	//解锁对象
	m_InitMutex.Unlock();
}

//重置对象
bool CShareStockManager::CloseStockManager()
{
	//校验句柄
	if (m_InitMutex.IsMutexValid())
	{
		//上锁
		m_InitMutex.Lock();

		//校验数量
		if (m_dwVisitorCount > 0)
		{
			m_InitMutex.Unlock();

			return false;
		}

		//设置状态
		m_bManageState = false;

		//关闭对象
		if (m_bStockInit = true)
		{
			//检测变更
			DetectStockChange();

			//检测状态
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

		//移除子项
		m_StockStockItem.RemoveAllItem();		

		//设置变量
		m_bStockInit = false;
		m_lStockControlID = 0;
		m_pStockManagerSink = NULL;
		ZeroMemory(&m_ShareData, sizeof(m_ShareData));
		ZeroMemory(&m_SuperPlayerControl, sizeof(m_SuperPlayerControl));

		//解锁
		m_InitMutex.Unlock();
		m_InitMutex.CloseMutex();
	}

	return true;
}

//时间脉冲
VOID CShareStockManager::OnEventTimerPulse()
{
	//检测状态
	DetectStockState();

	//检测变更
	DWORD dwCurrentTime = time(NULL);	
	if (dwCurrentTime % 5 == 0) DetectStockChange();
}

//添加库存
bool CShareStockManager::AppendStockItem(tagStockItem StockItem[], WORD wStockCount)
{
	//校验参数
	ASSERT (wStockCount > 0);
	if (wStockCount == 0) return false;

	//循环添加
	for (int i = 0; i < wStockCount; i++)
	{
		//获取对象
		auto pStockItem = &StockItem[i];

		//查找库存
		auto pSourceStockItem = m_StockStockItem.SearchStockItem(pStockItem->wStockID);
		if (pSourceStockItem != NULL) continue;

		//创建库存
		pSourceStockItem = m_StockStockItem.CreateStockItem(pStockItem->wStockID);
		if (pSourceStockItem == NULL) continue;

		//拷贝数据
		ZeroMemory(pSourceStockItem, sizeof(tagStockManagerItem));
		CopyMemory(&pSourceStockItem->StockItem, pStockItem, sizeof(tagStockItem));	
	}

	return true;
}

//删除库存
VOID CShareStockManager::DeleteStockItem(WORD wStockID)
{
	//查找库存
	auto pSourceStockItem = m_StockStockItem.SearchStockItem(wStockID);
	if (pSourceStockItem == NULL) return;

	//库存子项
	auto pStockItem = &pSourceStockItem->StockItem;

	//常规库存
	if (pStockItem->cbEnableMode == STOCK_ENABLE_MODE_AUTO)
	{
		//查找库存列表
		auto nListIndex = SearchRoutineStockList(pStockItem->wStockKind, pStockItem->wKindID);
		if (nListIndex != INVALID_INDEX)
		{
			//查找子项
			auto nItemIndex = SearchRoutineStockItem(nListIndex, pStockItem->wStockID);
			if (nItemIndex != INVALID_INDEX)
			{
				//上锁
				m_MutexData.RoutineMutex[nListIndex].StockMutext[nItemIndex].Lock();

				//拷贝数据
				ZeroMemory(&m_ShareData.RoutineStock[nListIndex].ShareStock[nItemIndex].StockItem, sizeof(tagStockItem));

				//解锁
				m_MutexData.RoutineMutex[nListIndex].StockMutext[nItemIndex].Unlock();
			}
		}		
	}
	else
	{
		//启用状态
		if (pStockItem->cbStockState == STOCK_STATE_ENABLE)
		{
			//分裂库存
			auto nItemIndex = SearchActivityStockItem(wStockID);
			if (nItemIndex != INVALID_INDEX)
			{
				FissionActivityStock(nItemIndex);

				//拷贝数据
				ZeroMemory(&m_ShareData.ActivityStock[nItemIndex].StockItem, sizeof(tagStockItem));
			}
		}
	}	

	//移除子项
	m_StockStockItem.RemoveStockItem(wStockID);
}

//修改库存
bool CShareStockManager::ModifyStockItem(tagStockItem* pStockItem)
{
	//查找库存
	auto pStockManagerItem = m_StockStockItem.SearchStockItem(pStockItem->wStockID);
	if (pStockManagerItem == NULL) return false;

	//变量定义
	CShareMutex* pShareMutext = NULL;
	tagStockItem* pShareStockItem = NULL;
	tagStockItem* pSourceStockItem = &pStockManagerItem->StockItem;

	//常规库存
	if (pStockItem->cbEnableMode == STOCK_ENABLE_MODE_AUTO)
	{
		//查找列表
		auto nListIndex = SearchRoutineStockList(pStockItem->wStockKind, pStockItem->wKindID);
		if (nListIndex != INVALID_INDEX)
		{
			//查找子项
			auto nItemIndex = SearchRoutineStockItem(nListIndex, pStockItem->wStockID);
			if (nItemIndex != INVALID_INDEX)
			{
				//上锁
				pShareMutext = &m_MutexData.RoutineMutex[nListIndex].StockMutext[nItemIndex];

				//共享库存
				pShareStockItem = &m_ShareData.RoutineStock[nListIndex].ShareStock[nItemIndex].StockItem;
			}
		}
	}
	else
	{
		//查找子项
		auto nItemIndex = SearchActivityStockItem(pStockItem->wStockID);
		if (nItemIndex != INVALID_INDEX)
		{
			//获取锁
			pShareMutext = &m_MutexData.ActivityMutex[nItemIndex];

			//共享库存
			pShareStockItem = &m_ShareData.ActivityStock[nItemIndex].StockItem;
		}
	}

	//原始库存
	if (pSourceStockItem != NULL)
	{
		//设置变量
		pSourceStockItem->lBetAmount = pStockItem->lBetAmount;
		pSourceStockItem->lExtraScore = pStockItem->lExtraScore;
		pSourceStockItem->wShrinkCount = pStockItem->wShrinkCount;
		CopyMemory(pSourceStockItem->ShrinkItem, pStockItem->ShrinkItem, sizeof(pSourceStockItem->ShrinkItem));

		//启用配置
		pSourceStockItem->cbEnableMode = pStockItem->cbEnableMode;
		pSourceStockItem->dwEnableEndTime = pStockItem->dwEnableEndTime;
		pSourceStockItem->dwEnableStartTime = pStockItem->dwEnableStartTime;

		//关联信息
		pSourceStockItem->wRelationCount = pStockItem->wRelationCount;
		CopyMemory(pSourceStockItem->RelationStockID, pStockItem->RelationStockID, sizeof(pSourceStockItem->RelationStockID));

		//奖池配置
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

	//共享库存
	if (pShareStockItem != NULL)
	{
		//上锁
		pShareMutext->Lock();

		//BET金额
		pShareStockItem->lBetAmount = pStockItem->lBetAmount;
		pShareStockItem->lExtraScore = pStockItem->lExtraScore;
		pShareStockItem->wShrinkCount = pStockItem->wShrinkCount;
		CopyMemory(pShareStockItem->ShrinkItem, pStockItem->ShrinkItem, sizeof(pShareStockItem->ShrinkItem));

		//启用配置
		pShareStockItem->cbEnableMode = pStockItem->cbEnableMode;
		pShareStockItem->dwEnableEndTime = pStockItem->dwEnableEndTime;
		pShareStockItem->dwEnableStartTime = pStockItem->dwEnableStartTime;

		//关联信息
		pShareStockItem->wRelationCount = pStockItem->wRelationCount;
		CopyMemory(pShareStockItem->RelationStockID, pStockItem->RelationStockID, sizeof(pShareStockItem->RelationStockID));

		//奖池配置
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

		//解锁
		pShareMutext->Unlock();
	}
	
	return true;
}

//调整库存值
bool CShareStockManager::AdjustStockScore(WORD wStockID, SCORE lChangeScore)
{
	//查找库存
	auto pStockManagerItem = m_StockStockItem.SearchStockItem(wStockID);
	if (pStockManagerItem == NULL) return false;

	//变量定义
	CShareMutex* pShareMutext = NULL;
	tagStockItem* pShareStockItem = NULL;
	tagStockItem* pSourceStockItem = &pStockManagerItem->StockItem;

	//常规库存
	if (pSourceStockItem->cbEnableMode == STOCK_ENABLE_MODE_AUTO)
	{
		//查找列表
		auto nListIndex = SearchRoutineStockList(pSourceStockItem->wStockKind, pSourceStockItem->wKindID);
		if (nListIndex != INVALID_INDEX)
		{
			//查找子项
			auto nItemIndex = SearchRoutineStockItem(nListIndex, pSourceStockItem->wStockID);
			if (nItemIndex != INVALID_INDEX)
			{
				//上锁
				pShareMutext = &m_MutexData.RoutineMutex[nListIndex].StockMutext[nItemIndex];

				//共享库存
				pShareStockItem = &m_ShareData.RoutineStock[nListIndex].ShareStock[nItemIndex].StockItem;
			}
		}
	}
	else
	{
		//查找子项
		auto nItemIndex = SearchActivityStockItem(pSourceStockItem->wStockID);
		if (nItemIndex != INVALID_INDEX)
		{
			//获取锁
			pShareMutext = &m_MutexData.ActivityMutex[nItemIndex];

			//共享库存
			pShareStockItem = &m_ShareData.ActivityStock[nItemIndex].StockItem;
		}
	}

	if (pShareStockItem != NULL)
	{
		//上锁
		pShareMutext->Lock();

		//更新分数
		pShareStockItem->lStockScore += lChangeScore;

		//变量定义
		tagStockVariationItem StockVariationItem = {};

		//库存变更
		StockVariationItem.wStockID = wStockID;
		StockVariationItem.lVariationScore = pShareStockItem->lStockScore - pSourceStockItem->lStockScore;
		StockVariationItem.lVariationProfit = pShareStockItem->lShrinkProfit - pSourceStockItem->lShrinkProfit;

		//更新库存
		pSourceStockItem->lStockScore = pShareStockItem->lStockScore;
		pSourceStockItem->lShrinkProfit = pShareStockItem->lShrinkProfit;

		//通知变更
		if (m_pStockManagerSink != NULL) m_pStockManagerSink->OnEventStockScoreUpdate(&StockVariationItem, 1);

		//解锁
		pShareMutext->Unlock();
	}
	else
	{
		//变量定义
		tagStockVariationItem StockVariationItem = {};

		//库存变更
		StockVariationItem.wStockID = wStockID;
		StockVariationItem.lVariationProfit = 0;
		StockVariationItem.lVariationScore += lChangeScore;		

		//更新库存
		pSourceStockItem->lStockScore += lChangeScore;		

		//通知变更
		if (m_pStockManagerSink != NULL) m_pStockManagerSink->OnEventStockScoreUpdate(&StockVariationItem, 1);
	}

	return true;
}

//调整彩金
bool CShareStockManager::AdjustJackpotScore(WORD wStockID, WORD wLevelID, SCORE lChangeScore)
{
	//校验参数
	ASSERT(wLevelID > 0 && lChangeScore != 0);
	if (wLevelID == 0 || lChangeScore == 0) return false;

	//查找库存
	auto pStockManagerItem = m_StockStockItem.SearchStockItem(wStockID);
	if (pStockManagerItem == NULL) return false;

	//校验等级
	if (wLevelID > pStockManagerItem->StockItem.wJackpotCount) return false;

	//变量定义
	CShareMutex* pShareMutext = NULL;
	tagStockItem* pShareStockItem = NULL;
	tagStockItem* pSourceStockItem = &pStockManagerItem->StockItem;

	//常规库存
	if (pSourceStockItem->cbEnableMode == STOCK_ENABLE_MODE_AUTO)
	{
		//查找列表
		auto nListIndex = SearchRoutineStockList(pSourceStockItem->wStockKind, pSourceStockItem->wKindID);
		if (nListIndex != INVALID_INDEX)
		{
			//查找子项
			auto nItemIndex = SearchRoutineStockItem(nListIndex, pSourceStockItem->wStockID);
			if (nItemIndex != INVALID_INDEX)
			{
				//上锁
				pShareMutext = &m_MutexData.RoutineMutex[nListIndex].StockMutext[nItemIndex];

				//共享库存
				pShareStockItem = &m_ShareData.RoutineStock[nListIndex].ShareStock[nItemIndex].StockItem;
			}
		}
	}
	else
	{
		//查找子项
		auto nItemIndex = SearchActivityStockItem(pSourceStockItem->wStockID);
		if (nItemIndex != INVALID_INDEX)
		{

			//获取锁
			pShareMutext = &m_MutexData.ActivityMutex[nItemIndex];

			//共享库存
			pShareStockItem = &m_ShareData.ActivityStock[nItemIndex].StockItem;
		}
	}
	
	if (pShareStockItem != NULL)
	{
		//上锁
		pShareMutext->Lock();

		//获取奖池
		auto pShareJackpotItem = &pShareStockItem->GameJackpotItem[wLevelID - 1];	
		auto pSrouceJackpotItem = &pSourceStockItem->GameJackpotItem[wLevelID - 1];

		//更新彩金
		pShareJackpotItem->lStockScore += lChangeScore;

		//变量定义
		tagJackpotVariationItem JackpotVariationItem = {};

		//彩金变更
		JackpotVariationItem.wLevelID = wLevelID;
		JackpotVariationItem.wStockID = wStockID;
		JackpotVariationItem.lVariationScore = pShareJackpotItem->lStockScore - pSrouceJackpotItem->lStockScore;

		//更新彩金
		pSrouceJackpotItem->lStockScore = pShareJackpotItem->lStockScore;

		//彩金变更
		if (m_pStockManagerSink != NULL) m_pStockManagerSink->OnEventJackpotScoreUpdate(&JackpotVariationItem, 1);

		//解锁
		pShareMutext->Unlock();
	}
	else
	{
		//获取奖池
		auto pSrouceJackpotItem = &pSourceStockItem->GameJackpotItem[wLevelID - 1];

		//变量定义
		tagJackpotVariationItem JackpotVariationItem = {};

		//彩金变更
		JackpotVariationItem.wLevelID = wLevelID;
		JackpotVariationItem.wStockID = wStockID;
		JackpotVariationItem.lVariationScore += lChangeScore;

		//更新彩金
		pSrouceJackpotItem->lStockScore += lChangeScore;

		//彩金变更
		if (m_pStockManagerSink != NULL) m_pStockManagerSink->OnEventJackpotScoreUpdate(&JackpotVariationItem, 1);
	}

	return true;
}

//统计彩金
bool CShareStockManager::CollectJackpotScore(WORD wStockKind, WORD wKindID,SCORE& lJackpotScore)
{
	//查找列表
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

//设置大R控制
VOID CShareStockManager::ApplySuperPlayerControl(tagSuperPlayerControl* pSuperPlayerControl)
{
	//拷贝数据
	CopyMemory(&m_SuperPlayerControl, pSuperPlayerControl,sizeof(m_SuperPlayerControl));

	//活动库存
	for (int i = 0; i < CountArray(m_ShareData.ActivityStock); i++)
	{
		if (m_ShareData.ActivityStock[i].StockItem.wStockID != 0)
		{
			m_MutexData.ActivityMutex[i].Lock();
			CopyMemory(&m_ShareData.ActivityStock[i].StockControl.SuperPlayerControl, &m_SuperPlayerControl, sizeof(m_SuperPlayerControl));
			m_MutexData.ActivityMutex[i].Unlock();
		}
	}

	//常规库存
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

//激活共享库存
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

//查找共享库存
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

//激活库存列表
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

//查找库存列表
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

//激活共享库存
int CShareStockManager::ActiveRoutineStockItem(int nListIndex, WORD wStockKind)
{
	//校验参数
	ASSERT(nListIndex < CountArray(m_ShareData.RoutineStock));
	if (nListIndex >= CountArray(m_ShareData.RoutineStock)) return INVALID_INDEX;

	//库存列表
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

//查找共享库存
int CShareStockManager::SearchRoutineStockItem(int nListIndex, WORD wStockID)
{
	//校验参数
	ASSERT(nListIndex < CountArray(m_ShareData.RoutineStock));
	if (nListIndex >= CountArray(m_ShareData.RoutineStock)) return INVALID_INDEX;

	//库存列表
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

//查找共享库存
int CShareStockManager::SearchRoutineStockItem(int nListIndex, WORD wStockKind, LONG lBetAmount)
{
	//校验参数
	ASSERT (nListIndex < CountArray(m_ShareData.RoutineStock));
	if (nListIndex >= CountArray(m_ShareData.RoutineStock)) return INVALID_INDEX;

	//库存列表
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

//检测状态
VOID CShareStockManager::DetectStockState()
{
	//当前时间
	DWORD dwCurrentTime = time(NULL);

	//变量定义
	tagStockItem* pStockItem = NULL;
	tagStockManagerItem* pStockManagerItem = m_StockStockItem.GetHeadStockItem();

	//变量定义
	WORD wStateCount = 0;
	tagStateVariationItem StateVariationList[SHARE_STOCK_COUNT] = {};

	//服务状态
	if (m_bManageState==true)
	{
		while (pStockManagerItem != NULL)
		{
			auto pStockItemCurrent = pStockManagerItem;
			auto pStockItem = &pStockManagerItem->StockItem;

			//自动模式
			if (pStockItem->cbEnableMode == STOCK_ENABLE_MODE_AUTO)
			{
				//未启用 
				if (m_bManageState && pStockItem->cbStockState == STOCK_STATE_DISABLE)
				{
					//变量定义
					bool bEnableStock = true;

					//校验库存
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
						//查找库存列表
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
							//查找子项
							auto nItemIndex = SearchRoutineStockItem(nListIndex, pStockItem->wStockKind, pStockItem->lBetAmount);
							if (nItemIndex == INVALID_INDEX)
							{
								nItemIndex = ActiveRoutineStockItem(nListIndex, pStockItem->wStockKind);
							}

							if (nItemIndex != INVALID_INDEX)
							{
								//上锁
								m_MutexData.RoutineMutex[nListIndex].StockMutext[nItemIndex].Lock();

								//设置状态
								pStockItem->cbStockState = STOCK_STATE_ENABLE;

								//拷贝数据
								CopyMemory(&m_ShareData.RoutineStock[nListIndex].ShareStock[nItemIndex].StockItem, pStockItem, sizeof(tagStockItem));
								CopyMemory(&m_ShareData.RoutineStock[nListIndex].ShareStock[nItemIndex].StockControl.SuperPlayerControl, &m_SuperPlayerControl, sizeof(m_SuperPlayerControl));

								//控制标识
								pStockManagerItem->lStockControlID = ++m_lStockControlID;

								//状态变更
								StateVariationList[wStateCount].wStockID = pStockItem->wStockID;
								StateVariationList[wStateCount].cbStockState = pStockItem->cbStockState;
								StateVariationList[wStateCount++].lStockControlID = pStockManagerItem->lStockControlID;

								//解锁
								m_MutexData.RoutineMutex[nListIndex].StockMutext[nItemIndex].Unlock();
							}
							else
							{
								CTraceService::TraceString(TEXT("游戏[%d]库存列表中找不到可用的空闲库存(StockID=%d)"),TraceLevel_Exception, pStockItem->wKindID, pStockItem->wStockID);
							}
						}
					}
				}
			}

			pStockManagerItem = m_StockStockItem.GetNextStockItem(pStockItemCurrent);
		}

		//通知变更
		if (m_pStockManagerSink != NULL)
		{
			//状态变更
			if (wStateCount > 0)
			{
				m_pStockManagerSink->OnEventStockStateUpdate(StateVariationList, wStateCount);				
			}			
		}
	}	
	
	//设置变量
	wStateCount = 0;
	pStockItem = NULL;
	pStockManagerItem = m_StockStockItem.GetHeadStockItem();

	//遍历源库存
	while (pStockManagerItem != NULL)
	{
		auto pStockItemCurrent = pStockManagerItem;
		auto pStockItem = &pStockManagerItem->StockItem;

		//手动模式
		if (pStockItem->cbEnableMode == STOCK_ENABLE_MODE_MANUAL)
		{
			//库存未启用
			if (pStockItem->cbStockState == STOCK_STATE_DISABLE)
			{
				//变量定义
				bool bEnableStock = true;

				//启用库存
				if (m_bManageState && dwCurrentTime >= pStockItem->dwEnableStartTime  && dwCurrentTime < pStockItem->dwEnableEndTime)
				{
					//遍历库存
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

					//与之关联的库存未启用或者未被关联
					if (bEnableStock)
					{
						//分配库存
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
							//合并库存
							MergeRoutineStock(nStockIndex);
						}
						else
						{
							CTraceService::TraceString(TEXT("库存[%d]启用失败，无法分配共享库存！"), TraceLevel_Exception, pStockItem->wStockID);
						}
					}
				}
			}

			//库存已启用
			if (pStockItem->cbStockState == STOCK_STATE_ENABLE)
			{
				//停用库存
				if (!m_bManageState || dwCurrentTime >= pStockItem->dwEnableEndTime)
				{
					//查找库存
					auto nStockIndex = SearchActivityStockItem(pStockItem->wStockID);
					if (nStockIndex != INVALID_INDEX)
					{
						//分裂库存
						FissionActivityStock(nStockIndex);
					}
					else
					{
						CTraceService::TraceString(TEXT("库存[%d]停用失败，未找到共享库存信息！"), TraceLevel_Exception, pStockItem->wStockID);
					}
				}
			}
		}

		//自动模式
		if (!m_bManageState && pStockItem->cbEnableMode == STOCK_ENABLE_MODE_AUTO)
		{
			//停用库存
			if (pStockItem->cbStockState == STOCK_STATE_ENABLE)
			{
				//设置状态
				pStockItem->cbStockState = STOCK_STATE_DISABLE;

				//查找库存列表
				auto nListIndex = SearchRoutineStockList(pStockItem->wStockKind, pStockItem->wKindID);
				if (nListIndex != INVALID_INDEX)
				{
					//查找子项
					auto nItemIndex = SearchRoutineStockItem(nListIndex, pStockItem->wStockKind, pStockItem->lBetAmount);
					if (nItemIndex == INVALID_INDEX)
					{
						//上锁
						m_MutexData.RoutineMutex[nListIndex].StockMutext[nItemIndex].Lock();

						//设置状态
						m_ShareData.RoutineStock[nListIndex].ShareStock[nItemIndex].StockItem.cbStockState = STOCK_STATE_DISABLE;

						//解锁
						m_MutexData.RoutineMutex[nListIndex].StockMutext[nItemIndex].Unlock();
					}
				}

				//状态变更
				StateVariationList[wStateCount].wStockID = pStockItem->wStockID;
				StateVariationList[wStateCount].cbStockState = STOCK_STATE_DISABLE;
				StateVariationList[wStateCount++].lStockControlID = pStockManagerItem->lStockControlID;			
			}			
		}

		pStockManagerItem = m_StockStockItem.GetNextStockItem(pStockItemCurrent);
	}

	//通知变更
	if (m_pStockManagerSink != NULL)
	{
		//状态变更
		if (wStateCount > 0) m_pStockManagerSink->OnEventStockStateUpdate(StateVariationList, wStateCount);
	}
}

//检测变化
VOID CShareStockManager::DetectStockChange()
{
	//变量定义
	WORD wStateCount = 0;
	tagStateVariationItem StateVariationList[SHARE_STOCK_COUNT] = {};

	//库存预警
	WORD wStockAlarmCount = 0;
	tagStockAlarmItem StockAlarmList[SHARE_STOCK_COUNT] = {};

	//库存变更
	WORD wStockCount = 0;
	tagStockVariationItem StockVariationList[SHARE_STOCK_COUNT] = {};	

	//奖池预警
	WORD wJackpotAlarmCount = 0;
	tagJackpotAlarmItem JackpotAlarmList[SHARE_STOCK_COUNT] = {};
	
	//奖池变更
	WORD wJackpotCount = 0;
	tagJackpotVariationItem JackpotVariationList[SHARE_STOCK_COUNT * 2] = {};

	//活动库存
	for (int i = 0; i < CountArray(m_ShareData.ActivityStock); i++)
	{
		//获取对象
		auto pShareStockItem = &m_ShareData.ActivityStock[i];
		if (pShareStockItem->StockItem.wStockID != 0)
		{
			auto pSourceStockItem = m_StockStockItem.SearchStockItem(pShareStockItem->StockItem.wStockID);
			if (pSourceStockItem == NULL)
			{
				CTraceService::TraceString(TEXT("未找到源库存%d"), TraceLevel_Exception, pShareStockItem->StockItem.wStockID);
				continue;
			}

			//报警标志
			bool bStockAlarm = false;

			//上锁
			m_MutexData.ActivityMutex[i].Lock();

			//校验库存
			if (pShareStockItem->StockItem.lStockScore != pSourceStockItem->StockItem.lStockScore || pShareStockItem->StockItem.lShrinkProfit != pSourceStockItem->StockItem.lShrinkProfit)
			{
				//库存变更
				StockVariationList[wStockCount].wStockID = pShareStockItem->StockItem.wStockID;
				StockVariationList[wStockCount].lVariationScore = pShareStockItem->StockItem.lStockScore - pSourceStockItem->StockItem.lStockScore;
				StockVariationList[wStockCount++].lVariationProfit = pShareStockItem->StockItem.lShrinkProfit - pSourceStockItem->StockItem.lShrinkProfit;

				//库存负分
				if (pShareStockItem->StockItem.lStockScore < 0)
				{
					bStockAlarm = true;
					StockAlarmList[wStockAlarmCount].wKindID = pShareStockItem->StockItem.wKindID;
					StockAlarmList[wStockAlarmCount].wStockID = pShareStockItem->StockItem.wStockID;
					StockAlarmList[wStockAlarmCount].lBetAmount = pShareStockItem->StockItem.lBetAmount;
					StockAlarmList[wStockAlarmCount++].lStockScore = pShareStockItem->StockItem.lStockScore;
				}

				//更新库存
				pSourceStockItem->StockItem.lStockScore = pShareStockItem->StockItem.lStockScore;
				pSourceStockItem->StockItem.lShrinkProfit = pShareStockItem->StockItem.lShrinkProfit;
			}			

			//校验彩金
			for (int j = 0; j < pShareStockItem->StockItem.wJackpotCount; j++)
			{
				//彩金变更
				JackpotVariationList[wJackpotCount].wStockID = pShareStockItem->StockItem.wStockID;
				JackpotVariationList[wJackpotCount].wLevelID = pShareStockItem->StockItem.GameJackpotItem[j].wLevelID;
				JackpotVariationList[wJackpotCount].lVariationScore = pShareStockItem->StockItem.GameJackpotItem[j].lStockScore - pSourceStockItem->StockItem.GameJackpotItem[j].lStockScore;
				
				//更新彩金
				pSourceStockItem->StockItem.GameJackpotItem[j].lStockScore = pShareStockItem->StockItem.GameJackpotItem[j].lStockScore;

				//校验变更
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

			//解锁
			m_MutexData.ActivityMutex[i].Unlock();

			//库存告负提前结束 
			if (bStockAlarm == true) pSourceStockItem->StockItem.dwEnableEndTime = time(NULL);
		}
	}

	//常规库存
	for (int i = 0; i < CountArray(m_ShareData.RoutineStock); i++)
	{
		//获取对象
		auto pShareStockList = &m_ShareData.RoutineStock[i];
		for (int j = 0; j < CountArray(pShareStockList->ShareStock); j++)
		{
			auto pShareStockItem = &pShareStockList->ShareStock[j];

			if (pShareStockItem->StockItem.wStockID != 0)
			{
				auto pSourceStockItem = m_StockStockItem.SearchStockItem(pShareStockItem->StockItem.wStockID);
				if (pSourceStockItem == NULL)
				{
					CTraceService::TraceString(TEXT("未找到源库存%d"), TraceLevel_Exception, pShareStockItem->StockItem.wStockID);
					continue;
				}

				//报警标志
				bool bStockAlarm = false;

				//上锁
				m_MutexData.RoutineMutex->StockMutext[j].Lock();

				//校验库存
				if (pShareStockItem->StockItem.lStockScore != pSourceStockItem->StockItem.lStockScore || pShareStockItem->StockItem.lShrinkProfit != pSourceStockItem->StockItem.lShrinkProfit)
				{
					//库存变更
					StockVariationList[wStockCount].wStockID = pShareStockItem->StockItem.wStockID;
					StockVariationList[wStockCount].lVariationScore = pShareStockItem->StockItem.lStockScore - pSourceStockItem->StockItem.lStockScore;
					StockVariationList[wStockCount++].lVariationProfit = pShareStockItem->StockItem.lShrinkProfit - pSourceStockItem->StockItem.lShrinkProfit;

					//库存负分
					if (pShareStockItem->StockItem.lStockScore < 0)
					{
						bStockAlarm = true;
						StockAlarmList[wStockAlarmCount].wKindID = pShareStockItem->StockItem.wKindID;
						StockAlarmList[wStockAlarmCount].wStockID = pShareStockItem->StockItem.wStockID;
						StockAlarmList[wStockAlarmCount].lBetAmount = pShareStockItem->StockItem.lBetAmount;
						StockAlarmList[wStockAlarmCount++].lStockScore = pShareStockItem->StockItem.lStockScore;
					}

					//更新库存
					pSourceStockItem->StockItem.lStockScore = pShareStockItem->StockItem.lStockScore;
					pSourceStockItem->StockItem.lShrinkProfit = pShareStockItem->StockItem.lShrinkProfit;
				}				

				//校验彩金
				for (int k = 0; k < pShareStockItem->StockItem.wJackpotCount; k++)
				{
					//彩金变更
					JackpotVariationList[wJackpotCount].wStockID = pShareStockItem->StockItem.wStockID;
					JackpotVariationList[wJackpotCount].wLevelID = pShareStockItem->StockItem.GameJackpotItem[k].wLevelID;
					JackpotVariationList[wJackpotCount].lVariationScore = pShareStockItem->StockItem.GameJackpotItem[k].lStockScore - pSourceStockItem->StockItem.GameJackpotItem[k].lStockScore;
					
					//更新彩金
					pSourceStockItem->StockItem.GameJackpotItem[k].lStockScore = pShareStockItem->StockItem.GameJackpotItem[k].lStockScore;

					//校验变更
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

				//库存告负停用库存
				if (bStockAlarm == true)
				{
					//设置状态
					pSourceStockItem->StockItem.cbStockState = STOCK_STATE_DISABLE;
					ZeroMemory(&pShareStockItem->StockItem,sizeof(pShareStockItem->StockItem));

					//状态变更
					StateVariationList[wStateCount].cbStockState = STOCK_STATE_DISABLE;
					StateVariationList[wStateCount].wStockID = pSourceStockItem->StockItem.wStockID;
					StateVariationList[wStateCount++].lStockControlID = pSourceStockItem->lStockControlID;
				}

				//解锁
				m_MutexData.RoutineMutex->StockMutext[j].Unlock();
			}
		}
	}

	//通知变更
	if (m_pStockManagerSink != NULL)
	{
		//库存变更
		if (wStockCount > 0) m_pStockManagerSink->OnEventStockScoreUpdate(StockVariationList, wStockCount);

		//彩金变更
		if (wJackpotCount > 0) m_pStockManagerSink->OnEventJackpotScoreUpdate(JackpotVariationList, wJackpotCount);

		//状态变更
		if (wStateCount > 0) m_pStockManagerSink->OnEventStockStateUpdate(StateVariationList, wStateCount);

		//库存预警
		if (wStockAlarmCount > 0) m_pStockManagerSink->OnEventStockScoreAlarm(StockAlarmList, wStockAlarmCount);

		//彩池预警
		if (wJackpotAlarmCount > 0) m_pStockManagerSink->OnEventJackpotScoreAlarm(JackpotAlarmList, wJackpotAlarmCount);		
	}
}

//合并库存
bool CShareStockManager::MergeRoutineStock(int nStockIndex)
{
	//校验参数
	ASSERT (nStockIndex!=INVALID_INDEX);
	if (nStockIndex == INVALID_INDEX) return false;

	//获取库存
	auto pShareStockItem = &m_ShareData.ActivityStock[nStockIndex];

	//查找库存
	auto pSourceStockItem = m_StockStockItem.SearchStockItem(pShareStockItem->StockItem.wStockID);
	if (pSourceStockItem == NULL) return false;

	//重置数据
	ZeroMemory(&pSourceStockItem->MergeRecord,sizeof(pSourceStockItem->MergeRecord));

	//变量定义
	WORD wStateCount = 0;	
	tagStateVariationItem StateVariationList[SHARE_STOCK_COUNT] = {0};

	//上锁
	m_MutexData.ActivityMutex[nStockIndex].Lock();

	//库存额外配额
	pShareStockItem->StockItem.lStockScore = pShareStockItem->StockItem.lExtraScore;

	//奖池额外配额
	for (int i = 0; i < pShareStockItem->StockItem.wJackpotCount; i++)
	{
		pShareStockItem->StockItem.GameJackpotItem[i].lStockScore = pShareStockItem->StockItem.GameJackpotItem[i].lExtraScore;
	}

	//更新状态
	pSourceStockItem->lStockControlID = ++m_lStockControlID;
	pShareStockItem->StockItem.cbStockState = STOCK_STATE_ENABLE;
	pSourceStockItem->StockItem.cbStockState = STOCK_STATE_ENABLE;

	//记录变更
	StateVariationList[wStateCount].wStockID = pShareStockItem->StockItem.wStockID;
	StateVariationList[wStateCount].lStockControlID = pSourceStockItem->lStockControlID;
	StateVariationList[wStateCount++].cbStockState = pShareStockItem->StockItem.cbStockState;
	
	//解锁
	m_MutexData.ActivityMutex[nStockIndex].Unlock();

	//获取对象
	auto pStockItem = &pSourceStockItem->StockItem;
	auto pMergeRecord = &pSourceStockItem->MergeRecord;

	//收集关联类型
	CollectRelationKindID(pMergeRecord, pStockItem);

	//逐个合并
	for (int i = 0; i < pMergeRecord->wKindCount; i++)
	{
		auto nListIndex = SearchRoutineStockList(pStockItem->wStockKind, pMergeRecord->wKindIDList[i]);
		if (nListIndex != INVALID_INDEX)
		{
			//获取对象
			auto pStockList = &m_ShareData.RoutineStock[nListIndex];
			auto pMutexList = &m_MutexData.RoutineMutex[nListIndex];

			//上锁		
			pMutexList->ListMutex.Lock();

			//关联库存
			pStockList->pRelationStock = pShareStockItem;			

			//统计库存和彩金
			for (int j = 0; j < CountArray(pStockList->ShareStock); j++)
			{
				//上锁
				pMutexList->StockMutext[j].Lock();				
				m_MutexData.ActivityMutex[nStockIndex].Lock();

				if (pStockList->ShareStock[j].StockItem.cbStockState==STOCK_STATE_ENABLE)
				{
					auto pTempStockItem = m_StockStockItem.SearchStockItem(pStockList->ShareStock[j].StockItem.wStockID);
					if (pTempStockItem != NULL)
					{

						//统计库存
						pMergeRecord->wCollectStockScore += pStockList->ShareStock[j].StockItem.lStockScore;
						pShareStockItem->StockItem.lStockScore += pStockList->ShareStock[j].StockItem.lStockScore;

						//统计彩金
						for (int k = 0; k < pShareStockItem->StockItem.wJackpotCount; k++)
						{
							pMergeRecord->wCollectJackpotScore[k] += pStockList->ShareStock[j].StockItem.GameJackpotItem[k].lStockScore;
							pShareStockItem->StockItem.GameJackpotItem[k].lStockScore += pStockList->ShareStock[j].StockItem.GameJackpotItem[k].lStockScore;
						}

						//修改状态
						pTempStockItem->StockItem.cbStockState = STOCK_STATE_LOCKED;
						pStockList->ShareStock[j].StockItem.cbStockState = STOCK_STATE_LOCKED;

						//状态变更
						StateVariationList[wStateCount].lStockControlID = pTempStockItem->lStockControlID;
						StateVariationList[wStateCount].wStockID = pStockList->ShareStock[j].StockItem.wStockID;						
						StateVariationList[wStateCount++].cbStockState = pStockList->ShareStock[j].StockItem.cbStockState;
					}
				}

				//解锁
				pMutexList->StockMutext[j].Unlock();
				m_MutexData.ActivityMutex[nStockIndex].Unlock();
			}

			//解锁
			pMutexList->ListMutex.Unlock();			
		}
	}

	//变更定义
	WORD wJackpotCount = 0;
	tagStockVariationItem StockVariationItem = {};
	tagJackpotVariationItem JackpotVariationItem[MAX_JACKPOT_COUNT] = {};

	//上锁
	m_MutexData.ActivityMutex[nStockIndex].Lock();

	//库存变更
	StockVariationItem.wStockID = pShareStockItem->StockItem.wStockID;
	StockVariationItem.lVariationScore = pShareStockItem->StockItem.lStockScore-pSourceStockItem->StockItem.lStockScore;

	//记录库存
	pSourceStockItem->StockItem.lStockScore = pShareStockItem->StockItem.lStockScore;

	//彩金数量
	wJackpotCount = pShareStockItem->StockItem.wJackpotCount;

	//统计彩金
	for (int i = 0; i < wJackpotCount; i++)
	{
		//彩金变更
		JackpotVariationItem[i].wStockID = pShareStockItem->StockItem.wStockID;
		JackpotVariationItem[i].wLevelID = pShareStockItem->StockItem.GameJackpotItem[i].wLevelID;
		JackpotVariationItem[i].lVariationScore = pShareStockItem->StockItem.GameJackpotItem[i].lStockScore-pSourceStockItem->StockItem.GameJackpotItem[i].lStockScore;

		//记录彩金
		pSourceStockItem->StockItem.GameJackpotItem[i].lStockScore = pShareStockItem->StockItem.GameJackpotItem[i].lStockScore;
	}	

	//解锁
	m_MutexData.ActivityMutex[nStockIndex].Unlock();

	//通知变更
	if (m_pStockManagerSink != NULL)
	{
		//分数变更
		m_pStockManagerSink->OnEventStockScoreUpdate(&StockVariationItem, 1);

		//彩金变更
		if (wJackpotCount>0) m_pStockManagerSink->OnEventJackpotScoreUpdate(JackpotVariationItem, wJackpotCount);

		//状态变更
		if (wStateCount > 0) m_pStockManagerSink->OnEventStockStateUpdate(StateVariationList, wStateCount);
	}	

	return true;
}

//分裂库存
bool CShareStockManager::FissionActivityStock(int nStockIndex)
{
	//校验参数
	ASSERT(nStockIndex != INVALID_INDEX);
	if (nStockIndex == INVALID_INDEX) return false;

	//获取库存
	auto pShareStockItem = &m_ShareData.ActivityStock[nStockIndex];

	//查找库存
	auto pSourceStockItem = m_StockStockItem.SearchStockItem(pShareStockItem->StockItem.wStockID);
	if (pSourceStockItem == NULL) return false;

	//获取对象
	auto pStockItem = &pSourceStockItem->StockItem;
	auto pMergeRecord = &pSourceStockItem->MergeRecord;

	//变量定义
	WORD wStateCount = 0,wStockCount = 0,wJackpotCount = 0;
	tagStateVariationItem StateVariationList[SHARE_STOCK_COUNT] = {};
	tagStockVariationItem StockVariationList[SHARE_STOCK_COUNT] = {};			
	tagJackpotVariationItem JackpotVariationList[SHARE_STOCK_COUNT*2] = {};

	//锁定库存
	m_MutexData.ActivityMutex[nStockIndex].Lock();

	//变量定义
	double dStockRatio=0.f;
	SCORE lStockScore=0;
	SCORE lShareStockScore=0;					
	SCORE lResidueStockScore = 0;
	SCORE lShareJackpotScore[MAX_JACKPOT_COUNT] = {0};
	SCORE lResidueJackpotScore[MAX_JACKPOT_COUNT] = { 0 };

	//记录初始分数
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
			//获取对象
			auto pStockList = &m_ShareData.RoutineStock[nListIndex];
			auto pMutexList = &m_MutexData.RoutineMutex[nListIndex];

			//上锁
			pMutexList->ListMutex.Lock();

			//统计库存和彩金
			for (int j = 0; j < CountArray(pStockList->ShareStock); j++)
			{
				//上锁
				pMutexList->StockMutext[j].Lock();

				if (pStockList->ShareStock[j].StockItem.cbStockState==STOCK_STATE_LOCKED)
				{
					//查找库存
					auto pTempStockItem = m_StockStockItem.SearchStockItem(pStockList->ShareStock[j].StockItem.wStockID);
					if (pTempStockItem != NULL)
					{
						//计算比列
						dStockRatio= pMergeRecord->wCollectStockScore?static_cast<double>(pStockList->ShareStock[j].StockItem.lStockScore / (double)(pMergeRecord->wCollectStockScore)):0;						

						//分裂库存
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

						//库存变更
						StockVariationList[wStockCount].wStockID= pStockList->ShareStock[j].StockItem.wStockID;
						StockVariationList[wStockCount++].lVariationScore = pStockList->ShareStock[j].StockItem.lStockScore - pTempStockItem->StockItem.lStockScore;
						pTempStockItem->StockItem.lStockScore = pStockList->ShareStock[j].StockItem.lStockScore;

						//分裂彩金
						for (int k = 0; k < pShareStockItem->StockItem.wJackpotCount; k++)
						{
							//计算比列
							dStockRatio = pMergeRecord->wCollectJackpotScore[k]?static_cast<double>(pStockList->ShareStock[j].StockItem.GameJackpotItem[k].lStockScore / (double)(pMergeRecord->wCollectJackpotScore[k])):0;

							//分裂彩金
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

							//彩金变更							
							JackpotVariationList[wJackpotCount].wStockID = pStockList->ShareStock[j].StockItem.wStockID;
							JackpotVariationList[wJackpotCount].wLevelID = pStockList->ShareStock[j].StockItem.GameJackpotItem[k].wLevelID;
							JackpotVariationList[wJackpotCount++].lVariationScore = pStockList->ShareStock[j].StockItem.GameJackpotItem[k].lStockScore - pTempStockItem->StockItem.GameJackpotItem[k].lStockScore;
							pTempStockItem->StockItem.GameJackpotItem[k].lStockScore = pStockList->ShareStock[j].StockItem.GameJackpotItem[k].lStockScore;
						}

						//修改状态
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
						
						//状态变更
						StateVariationList[wStateCount].lStockControlID = pTempStockItem->lStockControlID;
						StateVariationList[wStateCount].wStockID = pStockList->ShareStock[j].StockItem.wStockID;						
						StateVariationList[wStateCount++].cbStockState = pStockList->ShareStock[j].StockItem.cbStockState;
					}
				}

				//解锁
				pMutexList->StockMutext[j].Unlock();
			}

			//取消关联					
			pStockList->pRelationStock = NULL;

			//解锁
			pMutexList->ListMutex.Unlock();
		}
	}

	//更新状态
	pShareStockItem->StockItem.cbStockState = STOCK_STATE_DISABLE;
	pSourceStockItem->StockItem.cbStockState = STOCK_STATE_DISABLE;

	//状态变更
	StateVariationList[wStateCount].wStockID = pSourceStockItem->StockItem.wStockID;
	StateVariationList[wStateCount].lStockControlID = pSourceStockItem->lStockControlID;
	StateVariationList[wStateCount++].cbStockState = pSourceStockItem->StockItem.cbStockState;
	
	//库存变更	
	StockVariationList[wStockCount].wStockID = pSourceStockItem->StockItem.wStockID;
	StockVariationList[wStockCount++].lVariationScore = pShareStockItem->StockItem.lStockScore - pSourceStockItem->StockItem.lStockScore;
	
	//更新库存
	pSourceStockItem->StockItem.lStockScore = pShareStockItem->StockItem.lStockScore;

	//彩金变更
	for (int i = 0; i < pSourceStockItem->StockItem.wJackpotCount; i++)
	{
		//彩金变更
		JackpotVariationList[wJackpotCount].wStockID = pSourceStockItem->StockItem.wStockID;
		JackpotVariationList[wJackpotCount].wLevelID = pSourceStockItem->StockItem.GameJackpotItem[i].wLevelID;
		JackpotVariationList[wJackpotCount++].lVariationScore = pShareStockItem->StockItem.GameJackpotItem[i].lStockScore-pSourceStockItem->StockItem.GameJackpotItem[i].lStockScore;
		
		//更新彩金
		pSourceStockItem->StockItem.GameJackpotItem[i].lStockScore = pShareStockItem->StockItem.GameJackpotItem[i].lStockScore;
	}

	//解锁库存
	m_MutexData.ActivityMutex[nStockIndex].Unlock();

	//通知变更
	if (m_pStockManagerSink != NULL)
	{
		//分数变更
		if (wStockCount>0) m_pStockManagerSink->OnEventStockScoreUpdate(StockVariationList, wStockCount);

		//彩金变更
		if (wJackpotCount>0) m_pStockManagerSink->OnEventJackpotScoreUpdate(JackpotVariationList, wJackpotCount);

		//状态变更
		if (wStateCount > 0) m_pStockManagerSink->OnEventStockStateUpdate(StateVariationList, wStateCount);
	}

	return true;
}

//收集类型
bool CShareStockManager::CollectRelationKindID(tagStockMergeRecord* pStockMergeRecord, tagStockItem* pStockItem)
{
	//游戏库存
	if (pStockItem->wStockLevel == STOCK_LEVEL_GAME)
	{
		//查找类型
		for (int j = 0; j < pStockMergeRecord->wKindCount; j++)
		{
			if (pStockMergeRecord->wKindIDList[j] == pStockItem->wKindID)
			{
				return true;
			}
		}

		//添加类型
		pStockMergeRecord->wKindIDList[pStockMergeRecord->wKindCount++] = pStockItem->wKindID;

		return true;
	}

	for (int i = 0; i < pStockItem->wRelationCount; i++)
	{
		auto pTempStockItem = m_StockStockItem.SearchStockItem(pStockItem->RelationStockID[i]);
		if (pTempStockItem->StockItem.wStockLevel >= STOCK_LEVEL_GAME)
		{
			//分裂库存
			if (pTempStockItem->StockItem.cbStockState==STOCK_STATE_ENABLE)
			{
				auto nItemIndex = SearchActivityStockItem(pTempStockItem->StockItem.wStockID);
				if (nItemIndex != NULL) FissionActivityStock(nItemIndex);				
			}

			//添加库存
			pStockMergeRecord->wSublayerStockID[pStockMergeRecord->wStockCount++] = pTempStockItem->StockItem.wStockID;

			//递归调用			
			CollectRelationKindID(pStockMergeRecord, &pTempStockItem->StockItem);
		}		
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
