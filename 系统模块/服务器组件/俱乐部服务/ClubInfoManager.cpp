#include "StdAfx.h"
#include "ClubInfoManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////
//静态成员
CClubTableArray CClubInfoItem::m_ClubTableBuffer;
CClubMemberArray CClubInfoItem::m_ClubMemberBuffer;
CKindSettingArray CClubInfoItem::m_KindSettingBuffer;
CBattleConfigArray CClubInfoItem::m_BattleConfigBuffer;
CApplyMessageArray CClubInfoItem::m_ApplyMessageBuffer;

//////////////////////////////////////////////////////////////////////////////////
//构造函数
CClubInfoItem::CClubInfoItem()
{
	//设置变量
	m_dwActiveTime = 0;
	m_wOnlineCount = 0;
	m_bUnloadFlag = false;
	m_cbClubState=CLUB_STATE_OFFLINE;

	//初始化字典
	m_MapTableID.InitHashTable(13);
	m_MapMemberID.InitHashTable(503);
	m_MapMessageID.InitHashTable(53);

	//设置变量	
	m_BattleConfigActive.RemoveAll();
	ZeroMemory(&m_ClubItem,sizeof(m_ClubItem));	
}

//析构函数
CClubInfoItem::~CClubInfoItem()
{
	//设置变量	
	ResetClubItem();
}

//枚举请求
tagSocketInfo* CClubInfoItem::EnumEnterRequest(WORD wIndex)
{
	if (wIndex >= m_SocketInfoArray.GetCount())
	{
		return NULL;
	}

	return &m_SocketInfoArray[wIndex];
}

//缓存请求
VOID CClubInfoItem::CacheEnterRequest(tagSocketInfo& SocketInfo)
{
	if (SearchEnterRequest(SocketInfo) == -1)
	{
		m_SocketInfoArray.Add(SocketInfo);
	}
}

//移除请求
VOID CClubInfoItem::RemoveEnterRequest(tagSocketInfo& SocketInfo)
{
	auto nIndex = SearchEnterRequest(SocketInfo);
	if (nIndex != -1)
	{
		m_SocketInfoArray.RemoveAt(nIndex);
	}
}

//移除成员
VOID CClubInfoItem::RemoveMember()
{
	//茶社成员
	DWORD dwMemberID = 0L;
	tagClubMemberInfo* pClubMember = NULL;
	auto Position = m_MapMemberID.GetStartPosition();
	while (Position != NULL)
	{
		m_MapMemberID.GetNextAssoc(Position, dwMemberID, pClubMember);
		RemoveClubMember(pClubMember);
	}
	m_MapMemberID.RemoveAll();
}

//移除成员
bool CClubInfoItem::RemoveMember(DWORD dwMemberID)
{
	//查询成员
	tagClubMemberInfo * pClubMember = SearchMember(dwMemberID);
	if (pClubMember!=NULL)
	{
		//移除元素
		m_MapMemberID.RemoveKey(dwMemberID);

		//移除对象
		RemoveClubMember(pClubMember);
	}

	return true;
}

//添加成员
bool CClubInfoItem::AppendMember(tagClubMemberInfo & ClubMember)
{
	//查找成员
	tagClubMemberInfo *pClubMember=SearchMember(ClubMember.dwMemberID);
	if (pClubMember!=NULL)
	{
		//更新信息
		CopyMemory(pClubMember,&ClubMember,sizeof(ClubMember));
	}
	else
	{
		//激活成员
		pClubMember = ActiveClubMember();
		if (pClubMember == NULL) return false;

		//拷贝信息
		CopyMemory(pClubMember, &ClubMember, sizeof(ClubMember));

		//映射成员
		m_MapMemberID[ClubMember.dwMemberID] = pClubMember;
	}

	return true;
}

//枚举成员
tagClubMemberInfo * CClubInfoItem::EnumMember(POSITION & Position)
{
	//变量定义
	DWORD dwMemberID = 0L;
	tagClubMemberInfo * pClubMember = NULL;

	//获取对象
	if (Position == NULL) Position = m_MapMemberID.GetStartPosition();
	if (Position != NULL) m_MapMemberID.GetNextAssoc(Position, dwMemberID, pClubMember);

	return pClubMember;
}

//查找成员
tagClubMemberInfo * CClubInfoItem::SearchMember(DWORD dwMemberID)
{
	//变量定义
	tagClubMemberInfo * pClubMember = NULL;
	if (m_MapMemberID.Lookup(dwMemberID, pClubMember))
	{
		return pClubMember;
	}

	return NULL;
}

//移除玩法
VOID CClubInfoItem::RemoveConfig()
{
	//约战玩法
	for (INT_PTR i = 0; i < m_BattleConfigActive.GetCount(); i++)
	{
		RemoveBattleConfig(m_BattleConfigActive[i]);
	}
	m_BattleConfigActive.RemoveAll();
}

//移除玩法
bool CClubInfoItem::RemoveConfig(DWORD dwConfigID)
{
	//查找并移除
	for (INT_PTR i=0; i<m_BattleConfigActive.GetCount(); i++)
	{
		if (m_BattleConfigActive[i]->dwConfigID == dwConfigID)
		{
			m_BattleConfigActive.RemoveAt(i);
			return true;
		}
	}

	return false;
}

//添加玩法
bool CClubInfoItem::AppendConfig(tagClubBattleConfig & BattleConfig)
{
	//查找成员
	auto  pIMBattleConfig=SearchConfig(BattleConfig.dwConfigID);
	if (pIMBattleConfig !=NULL)
	{
		//更新信息
		CopyMemory(pIMBattleConfig,&BattleConfig,sizeof(BattleConfig));
	}
	else
	{
		//激活玩法
		pIMBattleConfig = ActiveBattleConfig();
		if (pIMBattleConfig == NULL) return false;

		//拷贝数据
		CopyMemory(pIMBattleConfig, &BattleConfig, sizeof(BattleConfig));

		//添加玩法
		m_BattleConfigActive.Add(pIMBattleConfig);
	}

	return true;
}

//枚举玩法
tagClubBattleConfigEx * CClubInfoItem::EnumConfig(INT nIndex)
{
	if (nIndex>= m_BattleConfigActive.GetCount()) return NULL;
	return m_BattleConfigActive[nIndex];
}

//查找玩法
tagClubBattleConfigEx * CClubInfoItem::SearchConfig(DWORD dwConfigID)
{
	for (INT_PTR i=0; i< m_BattleConfigActive.GetCount(); i++)
	{
		if (m_BattleConfigActive[i]->dwConfigID== dwConfigID)
		{
			return m_BattleConfigActive[i];
		}
	}

	return NULL;
}

//移除桌子
VOID CClubInfoItem::RemoveKindSetting()
{
	//约战玩法
	for (INT_PTR i = 0; i < m_KindSettingActive.GetCount(); i++)
	{
		RemoveKindSetting(m_KindSettingActive[i]);
	}
	m_KindSettingActive.RemoveAll();
}

//移除桌子
VOID CClubInfoItem::RemoveKindSetting(WORD wKindID)
{
	//查找并移除
	for (INT_PTR i = 0; i < m_KindSettingActive.GetCount(); i++)
	{
		if (m_KindSettingActive[i]->wKindID == wKindID)
		{
			m_KindSettingActive.RemoveAt(i);
			return;
		}
	}	
}

//添加设置
bool CClubInfoItem::AppendKindSetting(tagClubKindSetting& KindSetting)
{
	//查找成员
	auto  pKindSetting = SearchKindSetting(KindSetting.wKindID);
	if (pKindSetting != NULL)
	{
		//更新信息
		CopyMemory(pKindSetting, &KindSetting, sizeof(KindSetting));
	}
	else
	{
		//激活玩法
		pKindSetting = ActiveKindSetting();
		if (pKindSetting == NULL) return false;

		//拷贝数据
		CopyMemory(pKindSetting, &KindSetting, sizeof(KindSetting));

		//添加玩法
		m_KindSettingActive.Add(pKindSetting);
	}

	return true;
}

//枚举设置
tagClubKindSetting* CClubInfoItem::EnumKindSetting(INT nIndex)
{
	if (nIndex >= m_KindSettingActive.GetCount()) return NULL;
	return m_KindSettingActive[nIndex];
}

//查找桌子
tagClubKindSetting* CClubInfoItem::SearchKindSetting(WORD wKindID)
{
	for (INT_PTR i = 0; i < m_KindSettingActive.GetCount(); i++)
	{
		if (m_KindSettingActive[i]->wKindID == wKindID)
		{
			return m_KindSettingActive[i];
		}
	}

	return NULL;
}

//移除桌子
VOID CClubInfoItem::RemoveClubTable()
{
	//茶社桌子
	DWORD dwTableID = 0L;
	tagClubTableItem* pClubTable = NULL;
	auto Position = m_MapTableID.GetStartPosition();
	while (Position != NULL)
	{
		m_MapTableID.GetNextAssoc(Position, dwTableID, pClubTable);
		RemoveClubTable(pClubTable);
	}
	m_MapTableID.RemoveAll();
}

//移除桌子
VOID CClubInfoItem::RemoveClubTable(DWORD dwBattleNum)
{	
	//查找桌子
	auto pClubTableItem = SearchClubTable(dwBattleNum);
	if (pClubTableItem != NULL)
	{
		//移除桌子
		m_MapTableID.RemoveKey(dwBattleNum);

		//回收桌子
		RemoveClubTable(pClubTableItem);
	}
}

//添加桌子
bool CClubInfoItem::AppendClubTable(tagClubTableItem& TableItem)
{
	//查找成员
	auto pClubTableItem = SearchClubTable(TableItem.TableItem.dwBattleNum);
	if (pClubTableItem != NULL)
	{
		//更新信息
		CopyMemory(pClubTableItem, &TableItem, sizeof(TableItem));

		return false;
	}
	else
	{
		//激活桌子
		pClubTableItem = ActiveClubTable();
		if (pClubTableItem == NULL) return false;

		//拷贝信息
		CopyMemory(pClubTableItem, &TableItem, sizeof(TableItem));

		//映射消息
		m_MapTableID[TableItem.TableItem.dwBattleNum] = pClubTableItem;		
	}

	return true;
}

//枚举桌子
tagClubTableItem* CClubInfoItem::EnumClubTable(POSITION& Position)
{
	//变量定义
	DWORD dwTableID = 0L;
	tagClubTableItem * pClubTable = NULL;

	//获取对象
	if (Position == NULL) Position = m_MapTableID.GetStartPosition();
	if (Position != NULL) m_MapTableID.GetNextAssoc(Position, dwTableID, pClubTable);

	return pClubTable;
}

//查找桌子
tagClubTableItem* CClubInfoItem::SearchClubTable(DWORD dwBattleNum)
{
	//变量定义
	tagClubTableItem * pClubTable = NULL;
	if (m_MapTableID.Lookup(dwBattleNum, pClubTable))
	{
		return pClubTable;
	}

	return NULL;
}

//移除消息
VOID CClubInfoItem::RemoveMessage()
{
	DWORD dwMessageID = 0L;
	tagClubApplyMessage* pIMApplyMessage = NULL;
	auto Position = m_MapMessageID.GetStartPosition();
	while (Position != NULL)
	{
		m_MapMessageID.GetNextAssoc(Position, dwMessageID, pIMApplyMessage);
		RemoveApplyMessage(pIMApplyMessage);
	}
	m_MapMessageID.RemoveAll();
}

//移除消息
bool CClubInfoItem::RemoveMessage(DWORD dwMessageID)
{
	//查询成员
	tagClubApplyMessage * pIMApplyMessage = SearchMessage(dwMessageID);
	if (pIMApplyMessage != NULL)
	{
		//移除元素
		m_MapMessageID.RemoveKey(dwMessageID);

		//移除对象
		RemoveApplyMessage(pIMApplyMessage);
	}

	return true;
}

//添加消息
bool CClubInfoItem::AppendMessage(tagClubApplyMessage & ApplyMessage)
{
	//查找成员
	tagClubApplyMessage * pIMApplyMessage = SearchMessage(ApplyMessage.dwMessageID);
	if (pIMApplyMessage != NULL)
	{
		//更新信息
		CopyMemory(pIMApplyMessage, &ApplyMessage, sizeof(ApplyMessage));
	}
	else
	{
		//激活成员
		pIMApplyMessage = ActiveApplyMessage();
		if (pIMApplyMessage == NULL) return false;

		//拷贝信息
		CopyMemory(pIMApplyMessage, &ApplyMessage, sizeof(ApplyMessage));

		//映射消息
		m_MapMessageID[ApplyMessage.dwMessageID] = pIMApplyMessage;
	}

	return true;
}

//枚举消息
tagClubApplyMessage * CClubInfoItem::EnumMessage(POSITION & Position)
{
	//变量定义
	DWORD dwMessageID= 0L;
	tagClubApplyMessage * pIMApplyMessage= NULL;

	//获取对象
	if (Position == NULL) Position = m_MapMessageID.GetStartPosition();
	if (Position != NULL) m_MapMessageID.GetNextAssoc(Position, dwMessageID, pIMApplyMessage);

	return pIMApplyMessage;
}

//查找消息
tagClubApplyMessage * CClubInfoItem::SearchMessage(DWORD dwMessageID)
{
	//变量定义
	tagClubApplyMessage * pIMApplyMessage=NULL;
	if (m_MapMessageID.Lookup(dwMessageID, pIMApplyMessage))
	{
		return pIMApplyMessage;
	}

	return NULL;
}

//创建桌子
tagClubTableItem* CClubInfoItem::ActiveClubTable()
{
	//变量定义
	tagClubTableItem* pClubTable= NULL;

	//查找缓冲
	if (m_ClubTableBuffer.GetCount() > 0)
	{
		INT_PTR nItemCount = m_ClubTableBuffer.GetCount();
		pClubTable = m_ClubTableBuffer[nItemCount - 1];
		m_ClubTableBuffer.RemoveAt(nItemCount - 1);
		return pClubTable;
	}

	try
	{
		pClubTable = new tagClubTableItem;
		ZeroMemory(pClubTable, sizeof(tagClubTableItem));
		return pClubTable;
	}
	catch (...)
	{
		ASSERT(FALSE);
		SafeDelete(pClubTable);
	}

	return NULL;
}

//移除桌子
VOID CClubInfoItem::RemoveClubTable(tagClubTableItem* pClubTable)
{
	//清理数据
	ZeroMemory(pClubTable, sizeof(tagClubTableItem));

	//加入缓存
	m_ClubTableBuffer.Add(pClubTable);
}

//创建成员
tagClubMemberInfo * CClubInfoItem::ActiveClubMember()
{
	//变量定义
	tagClubMemberInfo * pClubMember = NULL;

	//查找缓冲
	if (m_ClubMemberBuffer.GetCount() > 0)
	{
		INT_PTR nItemCount=m_ClubMemberBuffer.GetCount();
		pClubMember = m_ClubMemberBuffer[nItemCount - 1];
		m_ClubMemberBuffer.RemoveAt(nItemCount - 1);
		return pClubMember;
	}

	try
	{
		pClubMember = new tagClubMemberInfo;
		ZeroMemory(pClubMember,sizeof(tagClubMemberInfo));
		return pClubMember;
	}
	catch (...)
	{
		ASSERT (FALSE);
		SafeDelete(pClubMember);
	}

	return NULL;
}

//移除成员
VOID CClubInfoItem::RemoveClubMember(tagClubMemberInfo * pClubMember)
{
	//清理数据
	ZeroMemory(pClubMember,sizeof(tagClubMemberInfo));

	//加入缓存
	m_ClubMemberBuffer.Add(pClubMember);
}

//创建玩法
tagClubBattleConfigEx * CClubInfoItem::ActiveBattleConfig()
{
	//变量定义
	tagClubBattleConfigEx * pIMBattleConfig = NULL;

	//查找缓冲
	if (m_BattleConfigBuffer.GetCount() > 0)
	{
		INT_PTR nItemCount = m_BattleConfigBuffer.GetCount();
		pIMBattleConfig = m_BattleConfigBuffer[nItemCount - 1];
		m_BattleConfigBuffer.RemoveAt(nItemCount - 1);
		return pIMBattleConfig;
	}

	try
	{
		pIMBattleConfig = new tagClubBattleConfigEx;
		ZeroMemory(pIMBattleConfig, sizeof(tagClubBattleConfigEx));
		return pIMBattleConfig;
	}
	catch (...)
	{
		ASSERT(FALSE);
		SafeDelete(pIMBattleConfig);
	}

	return NULL;
}

//移除玩法
VOID CClubInfoItem::RemoveBattleConfig(tagClubBattleConfigEx* pIMBattleConfig)
{
	//清理数据
	ZeroMemory(pIMBattleConfig, sizeof(tagClubBattleConfigEx));

	//加入缓存
	m_BattleConfigBuffer.Add(pIMBattleConfig);
}

//创建设置
tagClubKindSetting* CClubInfoItem::ActiveKindSetting()
{
	//变量定义
	tagClubKindSetting * pKindSetting= NULL;

	//查找缓冲
	if (m_KindSettingBuffer.GetCount() > 0)
	{
		INT_PTR nItemCount = m_KindSettingBuffer.GetCount();
		pKindSetting = m_KindSettingBuffer[nItemCount - 1];
		m_KindSettingBuffer.RemoveAt(nItemCount - 1);
		return pKindSetting;
	}

	try
	{
		pKindSetting = new tagClubKindSetting;
		ZeroMemory(pKindSetting, sizeof(tagClubKindSetting));
		return pKindSetting;
	}
	catch (...)
	{
		ASSERT(FALSE);
		SafeDelete(pKindSetting);
	}

	return NULL;
}

//移除设置
VOID CClubInfoItem::RemoveKindSetting(tagClubKindSetting* pClubKindSetting)
{
	//清理数据
	ZeroMemory(pClubKindSetting, sizeof(pClubKindSetting));

	//加入缓存
	m_KindSettingBuffer.Add(pClubKindSetting);
}

//创建消息
tagClubApplyMessage * CClubInfoItem::ActiveApplyMessage()
{
	//变量定义
	tagClubApplyMessage * pIMApplyMessage = NULL;

	//查找缓冲
	if (m_ApplyMessageBuffer.GetCount() > 0)
	{
		INT_PTR nItemCount = m_ApplyMessageBuffer.GetCount();
		pIMApplyMessage = m_ApplyMessageBuffer[nItemCount - 1];
		m_ApplyMessageBuffer.RemoveAt(nItemCount - 1);
		return pIMApplyMessage;
	}

	try
	{
		pIMApplyMessage = new tagClubApplyMessage;
		ZeroMemory(pIMApplyMessage, sizeof(tagClubApplyMessage));
		return pIMApplyMessage;
	}
	catch (...)
	{
		ASSERT(FALSE);
		SafeDelete(pIMApplyMessage);
	}

	return NULL;
}

//移除消息
VOID CClubInfoItem::RemoveApplyMessage(tagClubApplyMessage * pIMApplyMessage)
{
	//清理数据
	ZeroMemory(pIMApplyMessage, sizeof(tagClubApplyMessage));

	//加入缓存
	m_ApplyMessageBuffer.Add(pIMApplyMessage);
}

//重置对象
void CClubInfoItem::ResetClubItem()
{
	//状态变量
	m_dwActiveTime = 0;
	m_wOnlineCount = 0;
	m_bUnloadFlag = false;
	m_cbClubState = CLUB_STATE_OFFLINE;

	//移除成员
	RemoveMember();

	//移除消息
	RemoveMessage();

	//移除玩法
	RemoveConfig();

	//移除桌子
	RemoveClubTable();

	//移除设置
	RemoveKindSetting();

	//移除数据
	m_SocketInfoArray.RemoveAll();	

	//设置变量		
	ZeroMemory(&m_ClubItem, sizeof(m_ClubItem));		
}

//查找请求
INT CClubInfoItem::SearchEnterRequest(tagSocketInfo& SocketInfo)
{
	for (int i = 0; i < m_SocketInfoArray.GetCount(); i++)
	{
		if (m_SocketInfoArray[i].wGateID == SocketInfo.wGateID && m_SocketInfoArray[i].dwTokenID == SocketInfo.dwTokenID)
		{
			return i;
		}
	}

	return -1;
}

//清理数据
VOID CClubInfoItem::CleanStaticData()
{
	//茶社桌子
	for (INT_PTR i = 0; i < m_ClubTableBuffer.GetCount(); i++)
	{
		SafeDelete(m_ClubTableBuffer[i]);
	}

	//茶馆成员
	for (INT_PTR i = 0; i < m_ClubMemberBuffer.GetCount(); i++)
	{
		SafeDelete(m_ClubMemberBuffer[i]);
	}

	//约战玩法
	for (INT_PTR i = 0; i < m_BattleConfigBuffer.GetCount(); i++)
	{
		SafeDelete(m_BattleConfigBuffer[i]);
	}

	//申请消息
	for (INT_PTR i = 0; i < m_ApplyMessageBuffer.GetCount(); i++)
	{
		SafeDelete(m_ApplyMessageBuffer[i]);
	}
	
	//类型设置
	for (INT_PTR i = 0; i < m_KindSettingBuffer.GetCount(); i++)
	{
		SafeDelete(m_KindSettingBuffer[i]);
	}

	//移除元素
	m_ClubTableBuffer.RemoveAll();
	m_ClubMemberBuffer.RemoveAll();
	m_KindSettingBuffer.RemoveAll();
	m_BattleConfigBuffer.RemoveAll();
	m_ApplyMessageBuffer.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////////
//构造函数
CClubInfoManager::CClubInfoManager()
{
	//设置质数
	m_StockClubID.InitHashTable(1003L);
}

//析构函数
CClubInfoManager::~CClubInfoManager()
{
	//清理数据
	CClubInfoItem::CleanStaticData();

	//删除子项
	m_StockClubID.DeleteAllItem();

	return;
}

//枚举茶社
CClubInfoItem * CClubInfoManager::EnumClubItem(CClubInfoItem* pClubInfoItem)
{
	//变量定义
	if (pClubInfoItem == NULL)
	{
		return m_StockClubID.GetHeadStockItem();
	}

	return m_StockClubID.GetNextStockItem(pClubInfoItem);
}

//查找茶社
CClubInfoItem * CClubInfoManager::SearchClubItem(DWORD dwClubID)
{
	return m_StockClubID.SearchStockItem(dwClubID);
}

//移除茶社
VOID CClubInfoManager::RemoveClubItem()
{
	//遍历茶社
	auto pClubInfoItem = m_StockClubID.GetHeadStockItem();
	while (pClubInfoItem != NULL)
	{
		pClubInfoItem->ResetClubItem();

		auto pClubInfoLast = pClubInfoItem;
		pClubInfoItem=m_StockClubID.GetNextStockItem(pClubInfoLast);
	}

	//移除元素
	m_StockClubID.RemoveAllItem();
}

//移除茶社
VOID CClubInfoManager::RemoveClubItem(DWORD dwClubID)
{
	//查找茶社
	auto pClubInfoItem = m_StockClubID.SearchStockItem(dwClubID);
	if (pClubInfoItem) pClubInfoItem->ResetClubItem();

	//移除茶社
	m_StockClubID.RemoveStockItem(dwClubID);
}

//插入茶社
CClubInfoItem* CClubInfoManager::ActiveClubItem(tagClubItem & ClubItem)
{
	//创建子项
	auto pClubInfoItem= m_StockClubID.SearchStockItem(ClubItem.ClubProperty.dwClubID);
	if (pClubInfoItem == NULL)
	{
		pClubInfoItem = m_StockClubID.CreateStockItem(ClubItem.ClubProperty.dwClubID);
	}	

	//拷贝数据
	if (pClubInfoItem != NULL)
	{
		//设置状态
		pClubInfoItem->m_wOnlineCount = 0;
		pClubInfoItem->m_bUnloadFlag = false;
		pClubInfoItem->m_dwActiveTime = (DWORD)time(NULL);		
		pClubInfoItem->m_cbClubState = CLUB_STATE_OFFLINE;

		//拷贝数据
		CopyMemory(&pClubInfoItem->m_ClubItem, &ClubItem, sizeof(ClubItem));
	}

	return pClubInfoItem;
}

//////////////////////////////////////////////////////////////////////////////////
