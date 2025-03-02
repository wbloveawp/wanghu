#include "StdAfx.h"
#include "GlobalServiceManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//构造函数
CGlobalBattleService::CGlobalBattleService()
{
	//服务属性
	m_pITCPSocketService = NULL;	
}

//析构函数
CGlobalBattleService::~CGlobalBattleService()
{
}

//重置数据
VOID CGlobalBattleService::ResetData()
{
	m_pITCPSocketService = NULL;	
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalServiceManager::CGlobalServiceManager()
{
	//配置变量
	m_pIAttemperEngine = NULL;

	//初始化
	m_StockBattleService.InitHashTable(11);	
}

//析构函数
CGlobalServiceManager::~CGlobalServiceManager()
{
	//删除子项
	m_StockBattleService.DeleteAllItem();

	//移除元素	
	m_SocketServiceActive.RemoveAll();
	m_SocketServiceBuffer.RemoveAll();

	return;
}

//重置数据
VOID CGlobalServiceManager::ResetData()
{
	//移除子项
	m_StockBattleService.RemoveAllItem();

	//停止服务
	for (INT_PTR i = 0; i < m_SocketServiceActive.GetCount(); i++)
	{
		if (m_SocketServiceActive[i]->GetInterface())
		{
			m_SocketServiceActive[i]->GetInterface()->ConcludeService();
		}
	}

	//释放对象
	m_SocketServiceBuffer.Append(m_SocketServiceActive);
	for (auto i = 0; i < m_SocketServiceBuffer.GetCount(); i++)
	{
		SafeDelete(m_SocketServiceBuffer[i]);
	}

	//移除元素	
	m_SocketServiceActive.RemoveAll();
	m_SocketServiceBuffer.RemoveAll();
	

	return;
}

//激活服务
CGlobalBattleService * CGlobalServiceManager::ActiveBattleService(DWORD dwServiceID, tagServiceItem & ServiceItem)
{
	//寻找消息
	CGlobalBattleService * pGlobalServiceItem = NULL;
	pGlobalServiceItem = m_StockBattleService.SearchStockItem(ServiceItem.wServiceID);
	if (pGlobalServiceItem!=NULL)
	{
		ASSERT(FALSE);
		return NULL;
	}

	//创建消息
	pGlobalServiceItem = m_StockBattleService.CreateStockItem(ServiceItem.wServiceID);
	if (pGlobalServiceItem == NULL)
	{
		ASSERT(FALSE);
		return NULL;
	}

	//创建网络
	pGlobalServiceItem->m_pITCPSocketService = ActiveSocketService(dwServiceID);
	if (pGlobalServiceItem->m_pITCPSocketService == NULL)
	{
		ASSERT(FALSE);
		return NULL;
	}

	//设置茶社
	pGlobalServiceItem->m_ServiceItem = ServiceItem;

	return pGlobalServiceItem;
}

//删除服务
bool CGlobalServiceManager::DeleteBattleService(WORD wServiceID)
{
	//查询服务
	auto pGlobalServiceItem = m_StockBattleService.SearchStockItem(wServiceID);
	if (pGlobalServiceItem == NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//服务信息
	WORD wServiceKind = pGlobalServiceItem->m_ServiceItem.wServiceKindID;

	//移除网络
	RemoveSocketService(pGlobalServiceItem->m_pITCPSocketService);

	//移除服务
	m_StockBattleService.RemoveStockItem(wServiceID);

	return true;
}

//寻找服务
CGlobalBattleService * CGlobalServiceManager::SearchBattleService(WORD wServiceID)
{
	return m_StockBattleService.SearchStockItem(wServiceID);	
}

//枚举服务
CGlobalBattleService * CGlobalServiceManager::EnumBattleService(CGlobalBattleService* pGlobalBattleService)
{
	//获取对象
	if (pGlobalBattleService == NULL)
	{
		return m_StockBattleService.GetHeadStockItem();
	}
	else
	{
		return m_StockBattleService.GetNextStockItem(pGlobalBattleService);
	}
}

//激活服务
ITCPSocketService* CGlobalServiceManager::ActiveSocketService(DWORD dwServiceID)
{
	//变量定义
	CTCPSocketServiceHelper* pTCPSocketService = NULL;

	//查找缓存
	if (m_SocketServiceBuffer.GetCount() > 0)
	{
		//获取缓存
		INT_PTR nItemCount = m_SocketServiceBuffer.GetCount();
		pTCPSocketService = m_SocketServiceBuffer[nItemCount - 1];
		m_SocketServiceBuffer.RemoveAt(nItemCount - 1);
	}

	//创建对象
	if (pTCPSocketService == NULL)
	{
		try
		{
			pTCPSocketService = new CTCPSocketServiceHelper;
			if (pTCPSocketService == NULL) throw TEXT("内存不足,创建对象失败!");
		}
		catch (...)
		{
			ASSERT(FALSE);
		}
	}

	//创建接口
	if ((pTCPSocketService->GetInterface() == NULL) && (pTCPSocketService->CreateInstance() == false))
	{
		ASSERT(FALSE);
		SafeDelete(pTCPSocketService);

		return NULL;
	}

	//获取接口
	ITCPSocketService* pITCPSocketService = pTCPSocketService->GetInterface();

	//设置标识
	pITCPSocketService->SetServiceID(dwServiceID);

	//设置接口
	if (pITCPSocketService->SetTCPSocketEvent(m_pIAttemperEngine) == false)
	{
		ASSERT(FALSE);
		SafeDelete(pTCPSocketService);

		return NULL;
	}

	//启动服务
	if (pITCPSocketService->StartService() == false)
	{
		ASSERT(FALSE);
		SafeDelete(pTCPSocketService);

		return NULL;
	}

	//添加对象
	m_SocketServiceActive.Add(pTCPSocketService);

	return pITCPSocketService;
}

//移除服务
bool CGlobalServiceManager::RemoveSocketService(ITCPSocketService* pITCPSocketService)
{
	//参数校验
	ASSERT(pITCPSocketService != NULL);
	if (pITCPSocketService == NULL) return false;

	//查找对象
	for (INT_PTR i = 0; i < m_SocketServiceActive.GetCount(); i++)
	{
		if (m_SocketServiceActive[i]->GetInterface() == pITCPSocketService)
		{
			//停止服务
			pITCPSocketService->ConcludeService();
			m_SocketServiceBuffer.Add(m_SocketServiceActive[i]);
			m_SocketServiceActive.RemoveAt(i);
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
