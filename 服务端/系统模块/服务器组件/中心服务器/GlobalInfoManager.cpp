#include "StdAfx.h"
#include "GlobalInfoManager.h"
#include "ServiceAttribManager.h"

//////////////////////////////////////////////////////////////////////////
// 
//构造函数
CGlobalServiceInfo::CGlobalServiceInfo()
{
	m_pNextServiceInfoPtr = NULL;

	//设置大小
	m_MapRouteItem.InitHashTable(11);
}

//析构函数
CGlobalServiceInfo::~CGlobalServiceInfo()
{

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

//查找路由
tagRouteItem* CGlobalServiceInfo::QueryRouteItem(DWORD dwRouteID)
{
	tagRouteItem RouteItem = {};
	if (m_MapRouteItem.Lookup(dwRouteID, RouteItem))
	{
		return &m_MapRouteItem[dwRouteID];
	}

	return NULL;
}

//查找路由
tagRouteItem* CGlobalServiceInfo::EnumRouteItem(POSITION& Position)
{
	//变量定义
	DWORD dwRouteID = 0L;
	tagRouteItem RouteItem = {};

	//获取对象
	if (Position == NULL) Position = m_MapRouteItem.GetStartPosition();
	if (Position != NULL) m_MapRouteItem.GetNextAssoc(Position, dwRouteID, RouteItem);

	return (dwRouteID!=0)?&m_MapRouteItem[dwRouteID]:NULL;	
}

//重置函数
VOID CGlobalServiceInfo::ResetData()
{
	__super::ResetData();

	m_pNextServiceInfoPtr = NULL;
	m_MapRouteItem.RemoveAll();
}


//////////////////////////////////////////////////////////////////////////
//构造函数
CGlobalServiceKind::CGlobalServiceKind()
{
	m_wServiceKind = 0;
	m_ServiceHash.CleanNode();
	m_pNextServiceKindPtr = NULL;
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

	//移除节点
	m_ServiceHash.RemoveNode(to_string(pGlobalServiceInfo->GetServiceID()));	
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

	//添加节点
	m_ServiceHash.AppendNode(CWHHashNode(to_string(pGlobalServiceInfo->GetServiceID()), 15));
}

//查找服务
WORD CGlobalServiceKind::LookupService(DWORD dwObjectID)
{
	//哈希类型
	if (m_wServiceMapKind == MAP_KIND_HASH)
	{
		//哈希处理
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

//分配服务
WORD CGlobalServiceKind::AssignService(DWORD dwObjectID, WORD wServiceID)
{
	//哈希类型
	if (m_wServiceMapKind == MAP_KIND_HASH)
	{
		return LookupService(dwObjectID);		
	}

	//路由类型
	if (m_wServiceMapKind == MAP_KIND_ROUTE)
	{
		if (wServiceID == INVALID_WORD)
		{
			//哈希处理
			CWHHashNode HashNode;
			if (m_ServiceHash.Lookup(to_string(dwObjectID), &HashNode))
			{
				wServiceID = atoi(HashNode.GetIdentify().c_str());
			}
		}		

		//查找服务
		auto nIndex = SearchService(wServiceID);
		if (nIndex != -1)
		{
			//构造路由
			tagRouteItem RouteItem = {};
			RouteItem.dwRouteID = dwObjectID;
			RouteItem.cbRouteState = ROUTE_STATE_NULL;

			//添加路由
			m_ArrayServiceInfo[nIndex]->AppendRouteItem(&RouteItem, 1);

			return wServiceID;
		}
	}	

	return INVALID_WORD;
}

//重置数据
VOID CGlobalServiceKind::ResetData()
{
	//设置变量
	m_wServiceKind = 0;
	m_ServiceHash.CleanNode();
	m_ArrayServiceInfo.RemoveAll();
	m_pNextServiceKindPtr = NULL;	
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
CGlobalUserInfo::CGlobalUserInfo()
{
	//用户属性
	m_dwUserID=0L;
	m_dwGameID=0L;
	m_cbGender=0;
	m_wFaceID=0;
	m_dwCustomID=0;
	m_szNickName[0]=0;

	//网络信息
	m_wGateID = 0;
	m_dwTokenID = 0;

	//服务信息
	m_pGlobalLogonInfo = NULL;
	m_pGlobalServerInfo = NULL;
	m_pGlobalMessageInfo = NULL;

	return;
}

//析构函数
CGlobalUserInfo::~CGlobalUserInfo()
{
}

//服务数量
WORD CGlobalUserInfo::BindServiceCount()
{
	//变量定义
	WORD wServiceCount = 0;

	//统计数量
	if (m_pGlobalLogonInfo) wServiceCount++;
	if (m_pGlobalServerInfo) wServiceCount++;
	if (m_pGlobalMessageInfo) wServiceCount++;	 

	return wServiceCount;
}

//绑定登录
VOID CGlobalUserInfo::BindLogonInfo(CGlobalLogonInfo* pGlobalLogonInfo)
{
	//参数校验
	if (m_pGlobalLogonInfo == pGlobalLogonInfo)
	{
		return;
	}

	//解绑服务
	if (m_pGlobalLogonInfo != NULL)
	{
		UnBindLogonInfo(m_pGlobalLogonInfo);
	}

	//映射用户
	pGlobalLogonInfo->m_MapUserID[m_dwUserID]= this;

	//设置服务
	m_pGlobalLogonInfo = pGlobalLogonInfo;
}

//绑定房间
VOID CGlobalUserInfo::BindServerInfo(CGlobalServerInfo* pGlobalServerInfo)
{
	//参数校验
	if (m_pGlobalServerInfo == pGlobalServerInfo)
	{
		return;
	}

	//解绑服务
	if (m_pGlobalServerInfo != NULL)
	{
		UnBindServerInfo(m_pGlobalServerInfo);
	}

	//映射用户
	pGlobalServerInfo->m_MapUserID[m_dwUserID] = this;

	//设置服务
	m_pGlobalServerInfo = pGlobalServerInfo;
}

//绑定消息
VOID CGlobalUserInfo::BindMessageInfo(CGlobalServiceInfo* pGlobalMessageInfo)
{
	//参数校验
	if (m_pGlobalMessageInfo == pGlobalMessageInfo)
	{
		return;
	}

	//解绑服务
	if (m_pGlobalMessageInfo != NULL)
	{
		UnBindMessageInfo(m_pGlobalMessageInfo);
	}

	//设置服务
	m_pGlobalMessageInfo = pGlobalMessageInfo;
}

//解绑登录
VOID CGlobalUserInfo::UnBindLogonInfo(CGlobalLogonInfo* pGlobalLogonInfo)
{
	//参数校验
	if (m_pGlobalLogonInfo != pGlobalLogonInfo)
	{
		return;
	}

	//移除用户
	m_pGlobalLogonInfo->m_MapUserID.RemoveKey(m_dwUserID);

	//设置服务
	m_pGlobalLogonInfo = NULL;
}

//解绑房间
VOID CGlobalUserInfo::UnBindServerInfo(CGlobalServerInfo* pGlobalServerInfo)
{
	//参数校验
	if (m_pGlobalServerInfo != pGlobalServerInfo)
	{
		return;
	}

	//移除用户
	m_pGlobalServerInfo->m_MapUserID.RemoveKey(m_dwUserID);

	//设置服务
	m_pGlobalServerInfo = NULL;
}


//绑定消息
VOID CGlobalUserInfo::UnBindMessageInfo(CGlobalServiceInfo* pGlobalMessageInfo)
{
	//参数校验
	if (m_pGlobalMessageInfo != pGlobalMessageInfo)
	{
		return;
	}

	//设置服务
	m_pGlobalMessageInfo = NULL;
}

//重置用户
VOID CGlobalUserInfo::ResetData()
{
	__super::ResetData();

	//解绑服务
	if (m_pGlobalLogonInfo!=NULL) UnBindLogonInfo(m_pGlobalLogonInfo);
	if (m_pGlobalServerInfo!=NULL) UnBindServerInfo(m_pGlobalServerInfo);
	if (m_pGlobalMessageInfo!=NULL) UnBindMessageInfo(m_pGlobalMessageInfo);

	//服务信息
	m_pGlobalLogonInfo = NULL;
	m_pGlobalServerInfo = NULL;
	m_pGlobalMessageInfo = NULL;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalLogonInfo::CGlobalLogonInfo()
{
}

//析构函数
CGlobalLogonInfo::~CGlobalLogonInfo()
{
}

//重置函数
VOID CGlobalLogonInfo::ResetData()
{
	__super::ResetData();

	//移除用户
	m_MapUserID.RemoveAll();
}

//////////////////////////////////////////////////////////////////////////
//构造函数
CGlobalGameStock::CGlobalGameStock()
{
	m_wKindID = 0;
	m_lJackpotScore = 0;
}

//析构函数
CGlobalGameStock::~CGlobalGameStock()
{
}

//重置函数
VOID CGlobalGameStock::ResetData()
{
	m_wKindID = 0;
	m_lJackpotScore = 0;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalMonitorInfo::CGlobalMonitorInfo()
{
	m_StockGameStock.InitHashTable(33);
}

//析构函数
CGlobalMonitorInfo::~CGlobalMonitorInfo()
{
}

//重置函数
VOID CGlobalMonitorInfo::ResetData()
{
	__super::ResetData();

	//移除子项
	m_StockGameStock.RemoveAllItem();
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalServerInfo::CGlobalServerInfo()
{
	//设置变量
	ZeroMemory(&m_GameServer,sizeof(m_GameServer));

	//设置质数
	m_MapUserID.InitHashTable(PRIME_SERVER_USER);

	return;
}

//析构函数
CGlobalServerInfo::~CGlobalServerInfo()
{
}

//寻找用户
CGlobalUserInfo * CGlobalServerInfo::SearchUserInfo(DWORD dwUserID)
{
	//搜索用户
	CGlobalUserInfo * pGlobalUserInfo=NULL;
	m_MapUserID.Lookup(dwUserID,pGlobalUserInfo);

	return pGlobalUserInfo;
}

//重置数据
VOID CGlobalServerInfo::ResetData()
{
	__super::ResetData();

	//移除用户
	m_MapUserID.RemoveAll();

	//设置变量
	ZeroMemory(&m_GameServer, sizeof(m_GameServer));
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalAgentInfo::CGlobalAgentInfo()
{
	//设置变量
	m_pNextAgentItemPtr = NULL;
	ZeroMemory(&m_AgentItem, sizeof(m_AgentItem));	
}

//析构函数
CGlobalAgentInfo::~CGlobalAgentInfo()
{
}

//删除端口
bool CGlobalAgentInfo::RemoveServicePort(WORD wServicePort)
{
	//重复判断
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

//插入端口
bool CGlobalAgentInfo::InsertServicePort(tagPortPair * pPortPair)
{
	//重复判断
	for (int i = 0; i < m_PortPairArray.GetCount(); i++)
	{
		if (m_PortPairArray[i].wForwardPort == pPortPair->wForwardPort)
		{
			return false;
		}
	}

	//添加网关
	m_PortPairArray.Add(*pPortPair);

	return true;
}

//重置数据
VOID CGlobalAgentInfo::ResetData()
{
	__super::ResetData();

	//移除用户
	m_PortPairArray.RemoveAll();

	//设置变量
	m_pNextAgentItemPtr = NULL;
	ZeroMemory(&m_AgentItem, sizeof(m_AgentItem));
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalInfoManager::CGlobalInfoManager()
{
	//存储变量
	m_pGlobalUserInfo = NULL;
	m_pGlobalServiceKind = NULL;
	ZeroMemory(m_pGlobalServiceInfo, sizeof(m_pGlobalServiceInfo));

	//设置索引
	m_MapUserID.InitHashTable(PRIME_PLATFORM_USER);

	//初始化
	for (INT_PTR i = 0; i < CountArray(m_MapServiceID); i++)
	{
		m_MapServiceID[i].InitHashTable(PRIME_CUSTOM);
	}

	//初始化
	for (INT_PTR i = 0; i < CountArray(m_MapServiceKind); i++)
	{
		m_MapServiceKind[i].InitHashTable(PRIME_CUSTOM);
	}

	return;
}

//析构函数
CGlobalInfoManager::~CGlobalInfoManager()
{
	//删除用户
	DWORD dwUserID = 0L;
	CGlobalUserInfo * pGlobalUserInfo = NULL;
	POSITION Position = m_MapUserID.GetStartPosition();
	while (Position != NULL)
	{
		m_MapUserID.GetNextAssoc(Position, dwUserID, pGlobalUserInfo);
		SafeDelete(pGlobalUserInfo);
	}
	m_MapUserID.RemoveAll();

	//删除服务
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

	//删除类型
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

	//删除用户
	while (m_pGlobalUserInfo != NULL)
	{
		pGlobalUserInfo = m_pGlobalUserInfo;
		m_pGlobalUserInfo = m_pGlobalUserInfo->m_pNextUserInfoPtr;
		SafeDelete(pGlobalUserInfo);
	}

	//删除类型
	while (m_pGlobalServiceKind != NULL)
	{
		pGlobalServiceKind = m_pGlobalServiceKind;
		m_pGlobalServiceKind = m_pGlobalServiceKind->m_pNextServiceKindPtr;
		SafeDelete(pGlobalServiceKind);
	}

	//删除服务
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

//重置数据
VOID CGlobalInfoManager::ResetData()
{
	//删除用户
	DWORD dwUserID = 0L;
	CGlobalUserInfo* pGlobalUserInfo = NULL;
	POSITION Position = m_MapUserID.GetStartPosition();

	//删除用户
	while (Position != NULL)
	{
		m_MapUserID.GetNextAssoc(Position, dwUserID, pGlobalUserInfo);
		if (pGlobalUserInfo != NULL) FreeGlobalUserInfo(pGlobalUserInfo);
	}

	//删除索引
	m_MapUserID.RemoveAll();

	//删除服务
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

	//删除类型
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

//服务数目
DWORD CGlobalInfoManager::GetServiceItemCount(WORD wServiceModule)
{
	//服务索引
	WORD wServiceIndex = wServiceModule - 1;
	if (wServiceIndex >= CountArray(m_MapServiceID)) return 0;

	return (DWORD)m_MapServiceID[wServiceIndex].GetCount();
}

//激活用户
CGlobalUserInfo* CGlobalInfoManager::ActiveUserInfo(tagGlobalUserInfo& GlobalUserInfo)
{
	//创建用户
	auto pGlobalUserInfo = CreateGlobalUserInfo();
	if (pGlobalUserInfo == NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置用户
	pGlobalUserInfo->m_dwUserID = GlobalUserInfo.dwUserID;
	pGlobalUserInfo->m_dwGameID = GlobalUserInfo.dwGameID;
	StringCchCopy(pGlobalUserInfo->m_szNickName, CountArray(pGlobalUserInfo->m_szNickName), GlobalUserInfo.szNickName);

	//用户资料
	pGlobalUserInfo->m_cbGender = GlobalUserInfo.cbGender;
	pGlobalUserInfo->m_wFaceID = GlobalUserInfo.wFaceID;
	pGlobalUserInfo->m_dwCustomID = GlobalUserInfo.dwCustomID;

	//状态信息
	pGlobalUserInfo->m_cbShutUpStatus = GlobalUserInfo.cbShutUpStatus;
	pGlobalUserInfo->m_dwShutUpOverTime = GlobalUserInfo.dwShutUpOverTime;

	//用户状态
	pGlobalUserInfo->m_cbIMUserStatus = GlobalUserInfo.cbIMUserStatus;
	pGlobalUserInfo->m_cbCBUserStatus = GlobalUserInfo.cbCBUserStatus;

	//网络信息
	pGlobalUserInfo->m_wGateID = GlobalUserInfo.SocketInfo.wGateID;
	pGlobalUserInfo->m_dwTokenID = GlobalUserInfo.SocketInfo.dwTokenID;

	//设置索引
	m_MapUserID[GlobalUserInfo.dwUserID] = pGlobalUserInfo;

	return pGlobalUserInfo;
}

//删除用户
bool CGlobalInfoManager::DeleteUserInfo(DWORD dwUserID)
{
	//寻找用户
	CGlobalUserInfo* pGlobalUserInfo = NULL;
	if (m_MapUserID.Lookup(dwUserID, pGlobalUserInfo) == FALSE)
	{
		ASSERT(FALSE);
		return false;
	}

	//移除用户
	m_MapUserID.RemoveKey(dwUserID);

	//回收对象
	FreeGlobalUserInfo(pGlobalUserInfo);

	return true;
}

//激活代理
bool CGlobalInfoManager::ActiveAgentInfo(DWORD dwSocketID, tagServiceItem & ServiceItem, tagAgentItem & AgentItem)
{
	//服务索引
	WORD wServiceIndex = SERVICE_MODULE_AGENT-1;
	if (wServiceIndex >= CountArray(m_MapServiceID)) return false;

	//寻找代理
	CGlobalServiceInfo* pGlobalServiceInfo = NULL;
	if (m_MapServiceID[wServiceIndex].Lookup(ServiceItem.wServiceID, pGlobalServiceInfo) == TRUE)
	{
		ASSERT(FALSE);
		return false;
	}

	//创建代理
	CGlobalAgentInfo * pGlobalAgentInfo = (CGlobalAgentInfo *)CreateGlobalServiceInfo(SERVICE_MODULE_AGENT);
	if (pGlobalAgentInfo == NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置代理
	pGlobalAgentInfo->m_dwSocketID = dwSocketID;
	pGlobalAgentInfo->m_AgentItem = AgentItem;
	pGlobalAgentInfo->m_ServiceItem = ServiceItem;

	//设置索引
	m_MapServiceID[wServiceIndex][ServiceItem.wServiceID] = pGlobalAgentInfo;

	return true;
}

//删除代理
bool CGlobalInfoManager::DeleteAgentInfo(WORD wAgentID)
{
	//服务索引
	WORD wServiceIndex = SERVICE_MODULE_AGENT-1;
	if (wServiceIndex >= CountArray(m_MapServiceID)) return false;

	//寻找房间
	CGlobalServiceInfo * pGlobalServiceInfo = NULL;
	if (m_MapServiceID[wServiceIndex].Lookup(wAgentID, pGlobalServiceInfo) == FALSE)
	{
		ASSERT(FALSE);
		return false;
	}

	//释放代理
	m_MapServiceID[wServiceIndex].RemoveKey(wAgentID);
	FreeGlobalServiceInfo(pGlobalServiceInfo);

	return true;
}

//激活房间
bool CGlobalInfoManager::ActiveServerInfo(DWORD dwSocketID, tagServiceItem & ServiceItem, tagGameServer & GameServer)
{
	//服务索引
	WORD wServiceIndex = SERVICE_MODULE_GAME-1;
	if (wServiceIndex >= CountArray(m_MapServiceID)) return false;

	//寻找房间
	CGlobalServiceInfo * pGlobalServiceInfo = NULL;
	if (m_MapServiceID[wServiceIndex].Lookup(ServiceItem.wServiceID, pGlobalServiceInfo)==TRUE)
	{
		ASSERT(FALSE);
		return false;
	}

	//创建房间
	CGlobalServerInfo * pGlobalServerInfo=(CGlobalServerInfo *)CreateGlobalServiceInfo(SERVICE_MODULE_GAME);
	if (pGlobalServerInfo==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置房间
	pGlobalServerInfo->m_dwSocketID= dwSocketID;
	pGlobalServerInfo->m_GameServer=GameServer;
	pGlobalServerInfo->m_ServiceItem=ServiceItem;

	//设置索引
	m_MapServiceID[wServiceIndex][ServiceItem.wServiceID]=pGlobalServerInfo;

	return true;
}

//删除房间
bool CGlobalInfoManager::DeleteServerInfo(WORD wServerID)
{
	//服务索引
	WORD wServiceIndex = SERVICE_MODULE_GAME-1;
	if (wServiceIndex >= CountArray(m_MapServiceID)) return false;

	//寻找房间
	CGlobalServiceInfo * pGlobalServiceInfo=NULL;
	if (m_MapServiceID[wServiceIndex].Lookup(wServerID,pGlobalServiceInfo)==FALSE)
	{
		ASSERT(FALSE);
		return false;
	}

	//变量定义
	DWORD dwUserKey=0L;
	CGlobalUserInfo * pGlobalUserInfo=NULL;
	CGlobalServerInfo * pGlobalServerInfo = (CGlobalServerInfo *)pGlobalServiceInfo;
	POSITION Position=pGlobalServerInfo->m_MapUserID.GetStartPosition();

	//删除用户
	while (Position!=NULL)
	{
		//获取数据
		pGlobalServerInfo->m_MapUserID.GetNextAssoc(Position,dwUserKey,pGlobalUserInfo);

		//解除绑定
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

	//释放房间
	m_MapServiceID[wServiceIndex].RemoveKey(wServerID);
	FreeGlobalServiceInfo(pGlobalServiceInfo);

	return true;
}

//删除登录
bool CGlobalInfoManager::DeleteLogonInfo(WORD wServiceID)
{
	//服务索引
	WORD wServiceIndex = SERVICE_MODULE_LOGON - 1;
	if (wServiceIndex >= CountArray(m_MapServiceID)) return false;

	//寻找房间
	CGlobalServiceInfo* pGlobalServiceInfo = NULL;
	if (m_MapServiceID[wServiceIndex].Lookup(wServiceID, pGlobalServiceInfo) == FALSE)
	{
		ASSERT(FALSE);
		return false;
	}

	//变量定义
	DWORD dwUserKey = 0L;
	CGlobalUserInfo* pGlobalUserInfo = NULL;
	CGlobalLogonInfo* pGlobalLogonInfo = (CGlobalLogonInfo*)pGlobalServiceInfo;
	POSITION Position = pGlobalLogonInfo->m_MapUserID.GetStartPosition();

	//删除用户
	while (Position != NULL)
	{
		//获取数据
		pGlobalLogonInfo->m_MapUserID.GetNextAssoc(Position, dwUserKey, pGlobalUserInfo);

		//解除绑定
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

	//释放房间
	m_MapServiceID[wServiceIndex].RemoveKey(wServiceID);
	FreeGlobalServiceInfo(pGlobalServiceInfo);

	return true;
}

//激活服务
bool CGlobalInfoManager::ActiveServiceInfo(DWORD dwSocketID, tagServiceItem & ServiceItem)
{
	//服务索引
	WORD wServiceIndex = ServiceItem.wServiceModule-1;
	if (wServiceIndex >= CountArray(m_MapServiceID)) return false;

	//寻找服务
	CGlobalServiceInfo * pGlobalServiceInfo = NULL;
	if (m_MapServiceID[wServiceIndex].Lookup(ServiceItem.wServiceID, pGlobalServiceInfo) == TRUE)
	{
		ASSERT(FALSE);
		return false;
	}

	//创建服务
	pGlobalServiceInfo = CreateGlobalServiceInfo(ServiceItem.wServiceModule);
	if (pGlobalServiceInfo == NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置房间
	pGlobalServiceInfo->m_dwSocketID = dwSocketID;
	pGlobalServiceInfo->m_ServiceItem = ServiceItem;

	//设置索引
	m_MapServiceID[wServiceIndex][ServiceItem.wServiceID] = pGlobalServiceInfo;

	//构造类型
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

	//添加服务
	if (pGlobalServiceKind != NULL)
	{
		pGlobalServiceKind->AppendService(pGlobalServiceInfo);
	}	

	return true;
}

//删除服务
bool CGlobalInfoManager::DeleteServiceInfo(WORD wServiceModule, WORD wServiceID)
{
	//服务索引
	WORD wServiceIndex = wServiceModule-1;
	if (wServiceIndex >= CountArray(m_MapServiceID)) return 0;

	//寻找房间
	CGlobalServiceInfo * pGlobalServiceInfo = NULL;
	if (m_MapServiceID[wServiceIndex].Lookup(wServiceID, pGlobalServiceInfo) == FALSE)
	{
		ASSERT(FALSE);
		return false;
	}

	//服务信息
	WORD wServiceKind = pGlobalServiceInfo->m_ServiceItem.wServiceKindID;

	//构造类型
	CGlobalServiceKind* pGlobalServiceKind = NULL;
	if (m_MapServiceKind[wServiceIndex].Lookup(wServiceKind, pGlobalServiceKind) == TRUE)
	{
		pGlobalServiceKind->RemoveService(pGlobalServiceInfo);
		if (pGlobalServiceKind->GetServiceCount() == 0)
		{
			m_MapServiceKind[wServiceIndex].RemoveKey(wServiceKind);
		}
	}

	//释放房间
	m_MapServiceID[wServiceIndex].RemoveKey(wServiceID);
	FreeGlobalServiceInfo(pGlobalServiceInfo);

	return true;
}

//寻找用户
CGlobalUserInfo * CGlobalInfoManager::SearchUserInfo(DWORD dwUserID)
{
	//寻找用户
	CGlobalUserInfo * pGlobalUserInfo=NULL;
	m_MapUserID.Lookup(dwUserID,pGlobalUserInfo);

	return pGlobalUserInfo;
}

//寻找服务
CGlobalServiceInfo * CGlobalInfoManager::SearchServiceInfo(WORD wServiceModule, WORD wServiceID)
{
	//服务索引
	WORD wServiceIndex = wServiceModule-1;
	if (wServiceIndex >= CountArray(m_MapServiceID)) return NULL;

	//寻找逻辑
	CGlobalServiceInfo * pGlobalServiceInfo = NULL;
	m_MapServiceID[wServiceIndex].Lookup(wServiceID, pGlobalServiceInfo);

	return pGlobalServiceInfo;
}

//枚举用户
CGlobalUserInfo * CGlobalInfoManager::EnumUserInfo(POSITION & Position)
{
	//变量定义
	DWORD dwUserID=0L;
	CGlobalUserInfo * pGlobalUserInfo=NULL;

	//获取对象
	if (Position==NULL) Position = m_MapUserID.GetStartPosition();
	if (Position!=NULL)	m_MapUserID.GetNextAssoc(Position,dwUserID,pGlobalUserInfo);

	return pGlobalUserInfo;
}

//枚举服务
CGlobalServiceInfo * CGlobalInfoManager::EnumServiceInfo(WORD wServiceModule, POSITION & Position)
{
	//服务索引
	WORD wServiceIndex = wServiceModule-1;
	if (wServiceIndex >= CountArray(m_MapServiceID)) return NULL;

	//变量定义
	WORD wServiceID = 0L;
	CGlobalServiceInfo * pGlobalServiceInfo = NULL;

	//获取对象
	if (Position == NULL) Position = m_MapServiceID[wServiceIndex].GetStartPosition();
	if (Position != NULL) m_MapServiceID[wServiceIndex].GetNextAssoc(Position, wServiceID, pGlobalServiceInfo);

	return pGlobalServiceInfo;
}

//映射服务
CGlobalServiceInfo* CGlobalInfoManager::LookupService(WORD wServiceModule, WORD wServiceKind, DWORD dwObjectID)
{
	//服务索引
	WORD wServiceIndex = wServiceModule-1;
	if (wServiceIndex >= CountArray(m_MapServiceKind)) return NULL;

	//查找节点
	CGlobalServiceKind* pGlobalServiceKind = NULL;
	if (m_MapServiceKind[wServiceIndex].Lookup(wServiceKind, pGlobalServiceKind)==TRUE)
	{
		//获取服务
		auto nServiceID = pGlobalServiceKind->LookupService(dwObjectID);

		//查找服务
		CGlobalServiceInfo* pGlobalServiceInfo = NULL;
		m_MapServiceID[wServiceIndex].Lookup(nServiceID, pGlobalServiceInfo);

		return pGlobalServiceInfo;
	}

	return NULL;
}

//分派服务
CGlobalServiceInfo* CGlobalInfoManager::AssignService(WORD wServiceModule, WORD wServiceKind, DWORD dwObjectID, WORD wServiceID)
{
	//服务索引
	WORD wServiceIndex = wServiceModule - 1;
	if (wServiceIndex >= CountArray(m_MapServiceKind)) return NULL;

	//查找节点
	CGlobalServiceKind* pGlobalServiceKind = NULL;
	if (m_MapServiceKind[wServiceIndex].Lookup(wServiceKind, pGlobalServiceKind) == TRUE)
	{
		//获取服务
		auto nServiceID = pGlobalServiceKind->AssignService(dwObjectID, wServiceID);

		//查找服务
		CGlobalServiceInfo* pGlobalServiceInfo = NULL;
		m_MapServiceID[wServiceIndex].Lookup(nServiceID, pGlobalServiceInfo);

		return pGlobalServiceInfo;
	}

	return NULL;
}

//创建用户
CGlobalUserInfo * CGlobalInfoManager::CreateGlobalUserInfo()
{
	//使用存储
	if (m_pGlobalUserInfo!=NULL)
	{
		CGlobalUserInfo * pGlobalUserInfo=m_pGlobalUserInfo;
		m_pGlobalUserInfo=m_pGlobalUserInfo->m_pNextUserInfoPtr;
		pGlobalUserInfo->m_pNextUserInfoPtr=NULL;
		return pGlobalUserInfo;
	}

	//创建对象
	try 
	{ 
		CGlobalUserInfo * pGlobalUserInfo=new CGlobalUserInfo;
		return pGlobalUserInfo;
	}
	catch (...) {}

	return NULL;
}

//创建类型
CGlobalServiceKind* CGlobalInfoManager::CreateGlobalServiceKind()
{
	//使用存储
	if (m_pGlobalServiceKind != NULL)
	{
		CGlobalServiceKind* pGlobalServiceKind = m_pGlobalServiceKind;
		m_pGlobalServiceKind = m_pGlobalServiceKind->m_pNextServiceKindPtr;
		pGlobalServiceKind->m_pNextServiceKindPtr = NULL;
		return pGlobalServiceKind;
	}

	//创建对象
	try
	{
		CGlobalServiceKind* pGlobalServiceKind = new CGlobalServiceKind;
		return pGlobalServiceKind;
	}
	catch (...) {}

	return NULL;
}

//创建服务
CGlobalServiceInfo * CGlobalInfoManager::CreateGlobalServiceInfo(WORD wServiceModule)
{
	//服务索引
	WORD wServiceIndex = wServiceModule-1;
	if (wServiceIndex >= CountArray(m_pGlobalServiceInfo)) return NULL;

	//使用存储
	if (m_pGlobalServiceInfo[wServiceIndex] != NULL)
	{
		CGlobalServiceInfo * pGlobalServiceInfo = m_pGlobalServiceInfo[wServiceIndex];
		m_pGlobalServiceInfo[wServiceIndex] = m_pGlobalServiceInfo[wServiceIndex]->m_pNextServiceInfoPtr;
		pGlobalServiceInfo->m_pNextServiceInfoPtr = NULL;
		return pGlobalServiceInfo;
	}

	//创建对象
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

//释放用户
bool CGlobalInfoManager::FreeGlobalUserInfo(CGlobalUserInfo * pGlobalUserInfo)
{
	//效验参数
	ASSERT(pGlobalUserInfo!=NULL);
	if (pGlobalUserInfo==NULL) return false;

	//重置对象
	pGlobalUserInfo->ResetData();

	//加入存储
	pGlobalUserInfo->m_pNextUserInfoPtr=m_pGlobalUserInfo;
	m_pGlobalUserInfo=pGlobalUserInfo;

	return true;
}

//释放类型
bool CGlobalInfoManager::FreeGlobalServiceKind(CGlobalServiceKind* pGlobalServiceKind)
{
	//效验参数
	ASSERT(pGlobalServiceKind != NULL);
	if (pGlobalServiceKind == NULL) return false;

	//重置对象
	pGlobalServiceKind->ResetData();

	//加入存储
	pGlobalServiceKind->m_pNextServiceKindPtr = m_pGlobalServiceKind;
	m_pGlobalServiceKind = pGlobalServiceKind;

	return true;
}

//释放服务
bool CGlobalInfoManager::FreeGlobalServiceInfo(CGlobalServiceInfo *pGlobalServiceInfo)
{
	//效验参数
	ASSERT (pGlobalServiceInfo != NULL);
	if (pGlobalServiceInfo == NULL) return false;

	//服务模块
	WORD wServiceModule = pGlobalServiceInfo->GetServiceModule();

	//服务索引
	WORD wServiceIndex = wServiceModule-1;
	if (wServiceIndex >= CountArray(m_pGlobalServiceInfo)) return false;

	//重置数据
	pGlobalServiceInfo->ResetData();
	
	//加入存储
	pGlobalServiceInfo->m_pNextServiceInfoPtr = m_pGlobalServiceInfo[wServiceIndex];
	m_pGlobalServiceInfo[wServiceIndex] = pGlobalServiceInfo;

	return true;
}

//////////////////////////////////////////////////////////////////////////
