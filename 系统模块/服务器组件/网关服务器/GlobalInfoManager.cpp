#include "StdAfx.h"
#include "GlobalInfoManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//构造函数
CGlobalServiceInfo::CGlobalServiceInfo()
{
	//服务属性
	m_wDataSize = 0;
	m_pITCPSocketService = NULL;
	ZeroMemory(m_cbExtendData, sizeof(m_cbExtendData));

	//设置大小
	m_MapRouteItem.InitHashTable(11);
}

//析构函数
CGlobalServiceInfo::~CGlobalServiceInfo()
{
	ResetData();
}

//移除路由
VOID CGlobalServiceInfo::RemoveRouteItem()
{
	m_MapRouteItem.RemoveAll();
}

//移除路由
VOID CGlobalServiceInfo::RemoveRouteItem(DWORD dwRouteID)
{
	m_MapRouteItem.RemoveKey(dwRouteID);
}

//添加路由
VOID CGlobalServiceInfo::AppendRouteItem(tagRouteItem RouteItem[], WORD wItemCount)
{
	for (int i = 0; i < wItemCount; i++)
	{
		m_MapRouteItem[RouteItem[i].dwRouteID] = RouteItem[i];
	}
}

//重置数据
VOID CGlobalServiceInfo::ResetData()
{
	//设置变量
	m_wDataSize = 0;
	m_pITCPSocketService = NULL;
	ZeroMemory(m_cbExtendData,sizeof(m_cbExtendData));

	//移除数据
	m_MapRouteItem.RemoveAll();
}

//////////////////////////////////////////////////////////////////////////
//构造函数
CGlobalServiceKind::CGlobalServiceKind()
{
	m_wServiceKind = 0;
	m_wServiceMapKind = 0;
	m_ServiceHash.CleanNode();	
}

//析构函数
CGlobalServiceKind::~CGlobalServiceKind()
{
}

//移除服务
VOID CGlobalServiceKind::RemoveService(CGlobalServiceInfo* pGlobalServiceInfo)
{
	//路由类型
	if (m_wServiceMapKind == MAP_KIND_ROUTE)
	{
		//查找服务
		auto nIndex = SearchService(pGlobalServiceInfo->GetServiceID());
		if (nIndex != -1)
		{
			m_ArrayServiceInfo.RemoveAt(nIndex);
		}
	}

	//哈希类型
	if (m_wServiceMapKind == MAP_KIND_HASH)
	{
		//移除节点
		m_ServiceHash.RemoveNode(to_string(pGlobalServiceInfo->GetServiceID()));
	}		
}

//添加服务
VOID CGlobalServiceKind::AppendService(CGlobalServiceInfo* pGlobalServiceInfo)
{
	//路由类型
	if (m_wServiceMapKind == MAP_KIND_ROUTE)
	{
		//查找服务
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

	//哈希类型
	if (m_wServiceMapKind == MAP_KIND_HASH)
	{
		//添加节点
		m_ServiceHash.AppendNode(CWHHashNode(to_string(pGlobalServiceInfo->GetServiceID()), 15));
	}	
}

//查找服务
WORD CGlobalServiceKind::LookupService(DWORD dwObjectID)
{
	//哈希类型
	if (m_wServiceMapKind == MAP_KIND_HASH)
	{
		CWHHashNode HashNode;
		if (m_ServiceHash.Lookup(to_string(dwObjectID), &HashNode))
		{
			int nServiceID = atoi(HashNode.GetIdentify().c_str());
			return nServiceID;
		}
	}

	//路由类型
	if (m_wServiceMapKind == MAP_KIND_ROUTE)
	{
		//构造对象
		tagRouteItem RouteItem = {};

		//查找路由
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

//重置数据
VOID CGlobalServiceKind::ResetData()
{
	m_wServiceKind = 0;
	m_ServiceHash.CleanNode();	
	m_ArrayServiceInfo.RemoveAll();
}

//查找服务
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

//构造函数
CGlobalInfoManager::CGlobalInfoManager()
{
	//配置变量
	m_pIAttemperEngine = NULL;	
	m_pServiceAttribute = NULL;

	//设置索引
	for (INT_PTR i = 0; i < CountArray(m_StockServiceInfo); i++)
	{
		m_StockServiceInfo[i].InitHashTable(PRIME_CUSTOM);
	}

	//设置索引
	for (INT_PTR i = 0; i < CountArray(m_StockServiceKind); i++)
	{
		m_StockServiceKind[i].InitHashTable(3);
	}
}

//析构函数
CGlobalInfoManager::~CGlobalInfoManager()
{
	//删除服务
	for (INT_PTR i = 0; i < CountArray(m_StockServiceInfo); i++)
	{
		m_StockServiceInfo[i].DeleteAllItem();
	}

	//删除类型
	for (INT_PTR i = 0; i < CountArray(m_StockServiceKind); i++)
	{
		m_StockServiceKind[i].DeleteAllItem();
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

//重置数据
VOID CGlobalInfoManager::ResetData()
{
	//移除服务
	for (INT_PTR i = 0; i < CountArray(m_StockServiceInfo); i++)
	{
		//重置数据
		auto pMoveStockItem = m_StockServiceInfo[i].GetHeadStockItem();
		while (pMoveStockItem != NULL)
		{
			pMoveStockItem->ResetData();

			auto pCurrStockItem = pMoveStockItem;
			pMoveStockItem = m_StockServiceInfo[i].GetNextStockItem(pCurrStockItem);
		}

		//移除子项
		m_StockServiceInfo[i].RemoveAllItem();
	}

	//移除类型
	for (INT_PTR i = 0; i < CountArray(m_StockServiceKind); i++)
	{
		//重置数据
		auto pMoveStockItem = m_StockServiceKind[i].GetHeadStockItem();
		while (pMoveStockItem != NULL)
		{
			pMoveStockItem->ResetData();

			auto pCurrStockItem = pMoveStockItem;
			pMoveStockItem = m_StockServiceKind[i].GetNextStockItem(pCurrStockItem);						
		}

		//移除子项
		m_StockServiceKind[i].RemoveAllItem();
	}	

	//停止服务
	for (INT_PTR i = 0; i < m_SocketServiceActive.GetCount(); i++)
	{
		if (m_SocketServiceActive[i]->GetInterface())
		{
			m_SocketServiceActive[i]->GetInterface()->ConcludeService();
		}
	}

	//移除元素
	m_SocketServiceBuffer.Append(m_SocketServiceActive);
	m_SocketServiceActive.RemoveAll();

	return;
}

//服务数目
DWORD CGlobalInfoManager::GetServiceCount(WORD wServiceIndex)
{
	//获取索引
	ASSERT(wServiceIndex < CountArray(m_StockServiceInfo));
	if (wServiceIndex >= CountArray(m_StockServiceInfo)) return 0;

	return (DWORD)m_StockServiceInfo[wServiceIndex].GetItemCount(); 
}

//激活服务
CGlobalServiceInfo * CGlobalInfoManager::ActiveServiceInfo(WORD wServiceIndex, DWORD dwServiceID, tagServiceItem & ServiceItem)
{
	//获取索引
	ASSERT(wServiceIndex < CountArray(m_StockServiceInfo));
	if (wServiceIndex >= CountArray(m_StockServiceInfo)) return NULL;

	//寻找消息
	CGlobalServiceInfo * pGlobalServiceItem = NULL;
	pGlobalServiceItem = m_StockServiceInfo[wServiceIndex].SearchStockItem(ServiceItem.wServiceID);
	if (pGlobalServiceItem!=NULL)
	{
		ASSERT(FALSE);
		return NULL;
	}

	//创建消息
	pGlobalServiceItem = m_StockServiceInfo[wServiceIndex].CreateStockItem(ServiceItem.wServiceID);
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

	//设置群组
	pGlobalServiceItem->m_ServiceItem = ServiceItem;

	//查找类型
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

	//添加服务
	if (pGlobalServiceKind != NULL)
	{
		pGlobalServiceKind->AppendService(pGlobalServiceItem);
	}	

	return pGlobalServiceItem;
}

//删除服务
bool CGlobalInfoManager::DeleteServiceItem(WORD wServiceIndex, WORD wServiceID)
{
	//获取索引
	ASSERT (wServiceIndex < CountArray(m_StockServiceInfo));
	if (wServiceIndex >= CountArray(m_StockServiceInfo)) return NULL;

	//查询服务
	auto pGlobalServiceItem = m_StockServiceInfo[wServiceIndex].SearchStockItem(wServiceID);
	if (pGlobalServiceItem == NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//服务信息
	WORD wServiceKind = pGlobalServiceItem->m_ServiceItem.wServiceKindID;

	//移除服务
	auto pGlobalServiceKind = m_StockServiceKind[wServiceIndex].SearchStockItem(wServiceKind);
	if (pGlobalServiceKind !=NULL)
	{
		pGlobalServiceKind->RemoveService(pGlobalServiceItem);
		if (pGlobalServiceKind->GetServiceCount() == 0)
		{
			m_StockServiceKind[wServiceIndex].RemoveStockItem(wServiceKind);
		}		
	}	

	//移除网络
	RemoveSocketService(pGlobalServiceItem->m_pITCPSocketService);

	//重置数据
	pGlobalServiceItem->ResetData();

	//移除服务
	m_StockServiceInfo[wServiceIndex].RemoveStockItem(wServiceID);

	return true;
}

//映射服务
CGlobalServiceInfo * CGlobalInfoManager::MapServiceInfo(WORD wServiceIndex, WORD wServiceKind, DWORD dwObjectID)
{
	//校验索引
	ASSERT (wServiceIndex < CountArray(m_StockServiceKind));
	if (wServiceIndex >= CountArray(m_StockServiceKind)) return NULL;

	//查找类型
	auto pStockServiceKind = m_StockServiceKind[wServiceIndex].SearchStockItem(wServiceKind);
	if (pStockServiceKind !=NULL)
	{		
		int nServiceID = pStockServiceKind->LookupService(dwObjectID);
		return SearchServiceInfo(wServiceIndex, nServiceID);		
	}	

	return NULL;
}

//寻找服务
CGlobalServiceInfo * CGlobalInfoManager::SearchServiceInfo(WORD wServiceIndex, WORD wServiceID)
{
	//获取索引
	ASSERT (wServiceIndex < CountArray(m_StockServiceInfo));
	if (wServiceIndex >= CountArray(m_StockServiceInfo)) return NULL;

	return m_StockServiceInfo[wServiceIndex].SearchStockItem(wServiceID);	
}

//服务类型
CGlobalServiceKind* CGlobalInfoManager::SearchServiceKind(WORD wServiceIndex, WORD wServiceKind)
{
	//获取索引
	ASSERT(wServiceIndex < CountArray(m_StockServiceKind));
	if (wServiceIndex >= CountArray(m_StockServiceKind)) return NULL;

	return m_StockServiceKind[wServiceIndex].SearchStockItem(wServiceKind);
}

//枚举服务
CGlobalServiceInfo * CGlobalInfoManager::EnumServiceInfo(WORD wServiceIndex, CGlobalServiceInfo* pGlobalServiceItem)
{
	//获取索引
	ASSERT(wServiceIndex < CountArray(m_StockServiceInfo));
	if (wServiceIndex >= CountArray(m_StockServiceInfo)) return NULL;

	//获取对象
	if (pGlobalServiceItem == NULL)
	{
		return m_StockServiceInfo[wServiceIndex].GetHeadStockItem();
	}
	else
	{
		return m_StockServiceInfo[wServiceIndex].GetNextStockItem(pGlobalServiceItem);
	}
}

//网络信息
ITCPSocketService * CGlobalInfoManager::GetSocketService(WORD wServiceIndex, WORD wServiceID)
{
	//查找子项
	CGlobalServiceInfo * pGlobalServiceItem = SearchServiceInfo(wServiceIndex,wServiceID);
	if (pGlobalServiceItem != NULL && pGlobalServiceItem->m_pITCPSocketService != NULL)
	{
		return pGlobalServiceItem->m_pITCPSocketService;
	}

	return NULL;
}

//激活服务
ITCPSocketService* CGlobalInfoManager::ActiveSocketService(DWORD dwServiceID)
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
bool CGlobalInfoManager::RemoveSocketService(ITCPSocketService* pITCPSocketService)
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
