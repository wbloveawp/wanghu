#include "StdAfx.h"
#include "GlobalUnits.h"
#include "ServerUserManager.h"	

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////
//常量定义
#define TASK_SYNC_INTERVAL			5									//同步间隔

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServerUserPack::CServerUserPack(CServerUserItem* pServerUserItem)
{
	//设置变量
	m_dwRecordID = 0;
	m_bTransMode = false;

	//更新变量
	m_dwUpdateTime = 0;
	m_wUpdateMaxCount = 15;
	m_wUpdateInterval = 120;

	//指针变量	
	m_pIServerUserItemSink = NULL;
	m_pServerUserItem = pServerUserItem;

	//初始化仓库
	m_StockGoodsHoldInfo.InitHashTable(31);
	m_StockGoodsHoldTemp.InitHashTable(31);
	m_StockGoodsVariation.InitHashTable(31);
}

//析构函数
CServerUserPack::~CServerUserPack()
{
	ResetUserPack();
}

//开始事务
VOID CServerUserPack::BeginTrans()
{
	ASSERT(m_bTransMode == false);

	//提交事务
	if (m_bTransMode == true)
	{
		SubmitTrans();
	}

	//开始事务
	m_bTransMode = true;
}

//提交事务
VOID CServerUserPack::SubmitTrans()
{
	m_bTransMode = false;

	//变更通知
	if (m_pIServerUserItemSink && m_GoodsIDList.GetCount() > 0)
	{
		//变更通知
		m_pIServerUserItemSink->OnEventUserItemPack(m_pServerUserItem, m_GoodsIDList.GetData(), (WORD)m_GoodsIDList.GetCount());

		//清除物品
		m_GoodsIDList.RemoveAll();
	}
}

//移除物品
bool CServerUserPack::RemoveGoods(WORD wGoodsID, WORD wReason)
{
	//查找物品
	auto GameGoodsItemPtr = g_GameGoodsManager->SearchGoodsItem<CGameGoodsItemPtr>(wGoodsID);
	if (!GameGoodsItemPtr)
	{
		return false;
	}

	//查找物品
	auto pGoodsHoldInfo = m_StockGoodsHoldInfo.SearchStockItem(wGoodsID);
	if (pGoodsHoldInfo != NULL && pGoodsHoldInfo->dwGoodsCount > 0)
	{
		//获取变更
		auto pGoodsVariation = GetGoodsVariation();

		//添加物品
		m_GoodsIDList.Add(pGoodsHoldInfo->wGoodsID);

		//变更信息
		pGoodsVariation->wReason = wReason;
		pGoodsVariation->wGoodsID = wGoodsID;
		pGoodsVariation->wGoodsKind = GameGoodsItemPtr->GetGoodsKind();
		pGoodsVariation->dwBeforeCount = pGoodsHoldInfo->dwGoodsCount;
		pGoodsVariation->lVariationCount -= pGoodsHoldInfo->dwGoodsCount;
		GetLocalTime(&pGoodsVariation->VariationTime);

		//自动提交
		if (m_bTransMode == false) SubmitTrans();
	}

	//移除物品
	m_StockGoodsHoldInfo.RemoveStockItem(wGoodsID);

	return true;
}

//移除物品
bool CServerUserPack::RemoveGoods(WORD wGoodsID, DWORD dwGoodsCount, WORD wReason)
{
	//查找物品
	auto GameGoodsItemPtr = g_GameGoodsManager->SearchGoodsItem<CGameGoodsItemPtr>(wGoodsID);
	if (!GameGoodsItemPtr)
	{
		return false;
	}

	//查找物品
	auto pGoodsHoldInfo = m_StockGoodsHoldInfo.SearchStockItem(wGoodsID);
	if (pGoodsHoldInfo == NULL || pGoodsHoldInfo->dwGoodsCount < dwGoodsCount)
	{
		return false;
	}

	//获取变更
	auto pGoodsVariation = GetGoodsVariation();

	//添加物品
	m_GoodsIDList.Add(pGoodsHoldInfo->wGoodsID);

	//变更信息
	pGoodsVariation->wReason = wReason;
	pGoodsVariation->wGoodsID = wGoodsID;
	pGoodsVariation->wGoodsKind = GameGoodsItemPtr->GetGoodsKind();
	pGoodsVariation->dwBeforeCount = pGoodsHoldInfo->dwGoodsCount;
	pGoodsVariation->lVariationCount -= dwGoodsCount;
	GetLocalTime(&pGoodsVariation->VariationTime);

	//更新物品	
	pGoodsHoldInfo->dwGoodsCount -= dwGoodsCount;

	//自动提交
	if (m_bTransMode == false) SubmitTrans();

	return true;
}

//更新物品
bool CServerUserPack::UpdateGoods(WORD wGoodsID, WORD wGoodsLevel, DWORD dwIndate, WORD wReason)
{
	//查找物品
	auto pGoodsHoldInfo = SearchGoods(wGoodsID);
	if (pGoodsHoldInfo == NULL)
	{
		return false;
	}

	//获取变更
	auto pGoodsVariation = GetGoodsVariation();

	//添加物品
	m_GoodsIDList.Add(pGoodsHoldInfo->wGoodsID);

	//变更信息
	pGoodsVariation->wReason = wReason;
	pGoodsVariation->wGoodsID = wGoodsID;
	pGoodsVariation->wGoodsKind = pGoodsHoldInfo->wGoodsKind;
	GetLocalTime(&pGoodsVariation->VariationTime);

	//变更数量
	pGoodsVariation->lVariationCount = 0;

	//设置等级
	pGoodsHoldInfo->wGoodsLevel = wGoodsLevel;

	//校验有效期
	if (pGoodsHoldInfo->dwGoodsIndate != dwIndate)
	{
		if (dwIndate > 0)
		{
			if (pGoodsHoldInfo->dwGoodsIndate > 0)
			{
				pGoodsHoldInfo->dwExpireTime += dwIndate;
				pGoodsHoldInfo->dwExpireTime -= pGoodsHoldInfo->dwGoodsIndate;
			}
		}
		else
		{
			if (pGoodsHoldInfo->dwGoodsIndate > 0)
			{
				pGoodsHoldInfo->dwExpireTime = 0;
			}
		}

		//更新有效期
		pGoodsHoldInfo->dwGoodsIndate = dwIndate;
	}

	//自动提交
	if (m_bTransMode == false) SubmitTrans();

	return true;
}

//添加物品
bool CServerUserPack::AppendGoods(WORD wGoodsID, DWORD dwGoodsCount, WORD wGoodsLevel, DWORD dwIndate, WORD wReason)
{
	//查找物品
	auto GameGoodsItemPtr = g_GameGoodsManager->SearchGoodsItem<CGameGoodsItemPtr>(wGoodsID);
	if (!GameGoodsItemPtr)
	{
		return false;
	}

	//存储校验
	if ((GameGoodsItemPtr->GetCanStorePlace() & STORE_PLACE_PACK) == 0)
	{
		return false;
	}

	//查找物品
	auto pGoodsHoldInfo = GetGoodsHoldInfo(wGoodsID);
	if (pGoodsHoldInfo == NULL)
	{
		return false;
	}

	//初始化物品
	if (pGoodsHoldInfo->dwGoodsCount == 0)
	{
		pGoodsHoldInfo->wGoodsKind = GameGoodsItemPtr->GetGoodsKind();
	}

	//校验数量
	if (!GameGoodsItemPtr->GetHasIndate() && dwGoodsCount == 0)
	{
		return true;
	}

	//校验有效期
	if (GameGoodsItemPtr->GetHasIndate() && dwIndate > 0)
	{
		if (pGoodsHoldInfo->dwGoodsCount > 0 && pGoodsHoldInfo->dwGoodsIndate == 0)
		{
			return true;
		}
	}

	//获取变更
	auto pGoodsVariation = GetGoodsVariation();

	//添加物品
	m_GoodsIDList.Add(pGoodsHoldInfo->wGoodsID);

	//变更信息
	pGoodsVariation->wReason = wReason;
	pGoodsVariation->wGoodsID = wGoodsID;
	pGoodsVariation->wGoodsKind = GameGoodsItemPtr->GetGoodsKind();
	GetLocalTime(&pGoodsVariation->VariationTime);

	//设置等级
	pGoodsHoldInfo->wGoodsLevel = wGoodsLevel;

	//校验有效期
	if (dwIndate > 0)
	{
		//设置数量
		if (pGoodsHoldInfo->dwGoodsCount == 0)
		{
			pGoodsHoldInfo->dwGoodsCount = 1;
			pGoodsVariation->lVariationCount = 1;
			pGoodsVariation->dwBeforeCount = 0;
		}

		//设置有效期
		pGoodsHoldInfo->dwGoodsIndate += dwIndate;

		//设置过期时间
		if (pGoodsHoldInfo->dwExpireTime > 0)
		{
			pGoodsHoldInfo->dwExpireTime += dwIndate;
		}
		else
		{
			pGoodsHoldInfo->dwExpireTime = (DWORD)time(NULL) + dwIndate;
		}
	}
	else if (dwIndate == 0)
	{
		if (GameGoodsItemPtr->GetHasIndate())
		{
			//变更记录			
			pGoodsVariation->dwBeforeCount = 0;
			pGoodsVariation->lVariationCount = 1;

			//变更物品
			pGoodsHoldInfo->dwGoodsCount = 1;
			pGoodsHoldInfo->dwGoodsIndate = 0;
		}
		else
		{
			//变更记录
			pGoodsVariation->lVariationCount = dwGoodsCount;
			pGoodsVariation->dwBeforeCount = pGoodsHoldInfo->dwGoodsCount;

			//变更物品
			pGoodsHoldInfo->dwGoodsIndate = 0;
			pGoodsHoldInfo->dwGoodsCount += dwGoodsCount;
		}
	}

	//同步物品
	if (wReason == GOODS_REASON_SYNC)
	{
		m_StockGoodsVariation.RemoveStockItem(m_dwRecordID);
	}

	//自动提交
	if (m_bTransMode == false) SubmitTrans();

	return true;
}

//添加物品
WORD CServerUserPack::AppendGoods(tagGoodsHoldInfo GoodsHoldInfo[], WORD wGoodsCount)
{
	for (WORD i = 0; i < wGoodsCount; i++)
	{
		//查找物品
		auto pGoodsHoldInfo = GetGoodsHoldInfo(GoodsHoldInfo[i].wGoodsID);

		//拷贝数据
		CopyMemory(pGoodsHoldInfo, &GoodsHoldInfo[i], sizeof(tagGoodsHoldInfo));
	}

	return wGoodsCount;
}

//获取对象
tagGoodsHoldInfo* CServerUserPack::GetHeadGoodsItem()
{
	return m_StockGoodsHoldInfo.GetHeadStockItem();
}

//获取对象
tagGoodsHoldInfo* CServerUserPack::GetNextGoodsItem(tagGoodsHoldInfo* pGoodsHoldInfo)
{
	return m_StockGoodsHoldInfo.GetNextStockItem(pGoodsHoldInfo);
}

//清除变更
VOID CServerUserPack::CleanVariation()
{
	m_dwRecordID = 0;
	m_dwUpdateTime = (DWORD)time(NULL);
	m_StockGoodsVariation.RemoveAllItem();
}

//提交变更
bool CServerUserPack::IsSubmitVariation()
{
	return true;

	////校验时间间隔
	//auto dwCurrentTime = (DWORD)time(NULL);
	//if (dwCurrentTime >= m_dwUpdateTime + m_wUpdateInterval)
	//{
	//	return true;
	//}

	////校验变更数量
	//if (m_StockGoodsVariation.GetItemCount() >= m_wUpdateMaxCount)
	//{
	//	return true;
	//}

	//return false;
}

//提取物品
bool CServerUserPack::DistillVariationGoods(TCHAR szBuffer[], WORD wBufferCount)
{
	//变量定义
	WORD wGoodsCount = 0;
	CString strGoodsItem;
	CString strGoodsList;
	LPCTSTR pszGoodsFormart = TEXT("{\"GoodsID\":%d,\"GoodsKind\":%d,\"GoodsLevel\":%d,\"GoodsCount\":%u,\"GoodsIndate\":%u,\"ExpireTime\":%u}");

	//移除对象
	m_StockGoodsHoldTemp.RemoveAllItem();

	//获取头部
	auto pVariationItem = m_StockGoodsVariation.GetHeadStockItem();
	while (pVariationItem != NULL)
	{
		if (m_StockGoodsHoldTemp.SearchStockItem(pVariationItem->wGoodsID) == NULL)
		{
			auto pGoodsHoldInfo = m_StockGoodsHoldTemp.CreateStockItem(pVariationItem->wGoodsID);
			if (pGoodsHoldInfo != NULL)
			{
				auto pSearchGooldInfo = SearchGoods(pVariationItem->wGoodsID);
				if (pSearchGooldInfo != NULL)
				{
					CopyMemory(pGoodsHoldInfo, pSearchGooldInfo, sizeof(tagGoodsHoldInfo));
				}
				else
				{
					m_StockGoodsHoldTemp.RemoveStockItem(pVariationItem->wGoodsID);
				}
			}
		}

		//下一子项
		pVariationItem = m_StockGoodsVariation.GetNextStockItem(pVariationItem);
	}

	//获取头部	
	auto pGoodsHoldInfo = m_StockGoodsHoldTemp.GetHeadStockItem();
	while (pGoodsHoldInfo != NULL)
	{
		//添加括号
		if (wGoodsCount == 0) strGoodsList.Append(TEXT("["));

		//格式化物品
		strGoodsItem.Format(pszGoodsFormart, pGoodsHoldInfo->wGoodsID, pGoodsHoldInfo->wGoodsKind, pGoodsHoldInfo->wGoodsLevel, pGoodsHoldInfo->dwGoodsCount, pGoodsHoldInfo->dwGoodsIndate,
			pGoodsHoldInfo->dwExpireTime);

		//追加数据
		strGoodsList.Append(strGoodsItem);
		strGoodsList.Append((wGoodsCount++ < m_StockGoodsHoldTemp.GetItemCount() - 1) ? TEXT(",") : TEXT("]"));


		//下一子项
		pGoodsHoldInfo = m_StockGoodsHoldTemp.GetNextStockItem(pGoodsHoldInfo);
	}

	//校验长度
	if (strGoodsList.GetLength() > wBufferCount)
	{
		return false;
	}

	//拷贝数据
	StringCchCopy(szBuffer, wBufferCount, strGoodsList.GetString());

	return true;
}

//提取记录
bool CServerUserPack::DistillVariationRecord(TCHAR szBuffer[], WORD wBufferCount)
{
	//格式化字符串
	LPCTSTR pszRecordFormart = TEXT("{\"GoodsID\":%d,\"GoodsKind\":%d,\"Reason\":%d,\"BeforeCount\":%u,\"VariationCount\":%ld,\"VariationTime\":\"%s\"}");

	//变量定义
	WORD wRecordCount = 0;
	CString strRecordItem;
	CString strRecordList;
	CString strVariationTime;

	//获取头部
	auto pVariationItem = m_StockGoodsVariation.GetHeadStockItem();
	while (pVariationItem != NULL)
	{
		//添加括号
		if (wRecordCount == 0) strRecordList.Append(TEXT("["));

		//格式化物品
		strVariationTime.Format(TEXT("%04d-%02d-%02d %02d:%02d:%02d"), pVariationItem->VariationTime.wYear, pVariationItem->VariationTime.wMonth, pVariationItem->VariationTime.wDay,
			pVariationItem->VariationTime.wHour, pVariationItem->VariationTime.wMinute, pVariationItem->VariationTime.wSecond);
		strRecordItem.Format(pszRecordFormart, pVariationItem->wGoodsID, pVariationItem->wGoodsKind, pVariationItem->wReason, pVariationItem->dwBeforeCount, pVariationItem->lVariationCount, strVariationTime);

		//追加物品
		strRecordList.Append(strRecordItem);
		strRecordList.Append((wRecordCount++ < m_StockGoodsVariation.GetItemCount() - 1) ? TEXT(",") : TEXT("]"));

		//下一子项
		pVariationItem = m_StockGoodsVariation.GetNextStockItem(pVariationItem);
	}

	//校验长度
	if (strRecordList.GetLength() > wBufferCount)
	{
		return false;
	}

	//拷贝数据
	StringCchCopy(szBuffer, wBufferCount, strRecordList.GetString());

	return true;
}

//查找物品
tagGoodsHoldInfo* CServerUserPack::SearchGoods(WORD wGoodsID)
{
	return m_StockGoodsHoldInfo.SearchStockItem(wGoodsID);
}

//重置背包
VOID CServerUserPack::ResetUserPack()
{
	//更新变量
	m_dwRecordID = 0;
	m_dwUpdateTime = (DWORD)time(NULL);

	//移除子项
	m_StockGoodsHoldInfo.RemoveAllItem();
	m_StockGoodsHoldTemp.RemoveAllItem();
	m_StockGoodsVariation.RemoveAllItem();
}

//持有信息
inline tagGoodsHoldInfo* CServerUserPack::GetGoodsHoldInfo(WORD wGoodsID)
{
	//构造主键	
	auto pGoodsHoldInfo = m_StockGoodsHoldInfo.SearchStockItem(wGoodsID);
	if (pGoodsHoldInfo == NULL)
	{
		//创建对象
		pGoodsHoldInfo = m_StockGoodsHoldInfo.CreateStockItem(wGoodsID);

		//初始化
		if (pGoodsHoldInfo != NULL)
		{
			ZeroMemory(pGoodsHoldInfo, sizeof(tagGoodsHoldInfo));

			//设置变量
			pGoodsHoldInfo->wGoodsID = wGoodsID;
		}
	}

	return pGoodsHoldInfo;
}

//获取变更
inline tagGoodsVariation* CServerUserPack::GetGoodsVariation()
{
	//更新标识
	m_dwRecordID++;

	//创建对象	
	auto pGoodsVariotion = m_StockGoodsVariation.SearchStockItem(m_dwRecordID);
	if (pGoodsVariotion == NULL)
	{
		//创建对象
		pGoodsVariotion = m_StockGoodsVariation.CreateStockItem(m_dwRecordID);

		//初始化
		if (pGoodsVariotion != NULL)
		{
			ZeroMemory(pGoodsVariotion, sizeof(tagGoodsVariation));
		}
	}

	return pGoodsVariotion;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CServerUserItem::CServerUserItem() : m_ServerUserPack(this)
{
	//状态变量
	m_dwLookonTime=0;
	m_wLastTableID=INVALID_WORD;
	m_wLastChairID=INVALID_WORD;	

	//权限变量
	m_dwUserRight=0L;
	m_dwMasterRight=0L;

	//位置信息
	m_dGlobalPosLng=0.f;
	m_dGlobalPosLat=0.f;
	ZeroMemory(m_szPlaceName, sizeof(m_szPlaceName));

	//状态变量
	m_lNearInfo=0L;
	m_lNearCount=0L;
	m_dwLogonTime=0L;
	m_dwPlayTimeCount=0L;
	m_dwTotalPlayTimeCount = 0L;
	m_dwStartTimeCount = 0L;
	m_dwPlayTimeVariation = 0L;

	//黑白名单
	m_wAddupWinRate = 0;
	m_lAddupWinScore = 0;
	m_lAddupLoseScore = 0;
	m_pControlParameter = NULL;
	ZeroMemory(&m_UserControlItem, sizeof(m_UserControlItem));

	//积分变量
	m_lRevenue=0L;
	m_lFreeScore=0L;
	m_lLockScore=0L;	
	m_lFreezeScore=0L;	
	m_lLockServiceScore=0L;

	//游戏辅助
	m_bMobileUser=false;
	m_bTrusteeUser=false;
	m_bRobotUser=false;
	m_bClientReady=false;
	m_bModifyScore=false;
	ZeroMemory(m_szPassword,sizeof(m_szPassword));
	ZeroMemory(m_szTmpPassword,sizeof(m_szTmpPassword));

	//比赛信息
	m_lMatchNo=0;
	m_dwMatchID=0;	
	m_wSignupCount=0;
	m_dwTicketCount=0;
	m_dwSignUpTime=0;
	m_cbMatchStatus=MUS_NULL;

	//等级信息
	m_cbGameOrder=0;
	m_cbMemberOrder=0;

	//状态信息
	m_cbShutUpStatus = 0;
	m_dwShutUpOverTime = 0;

	//手机定义	
	m_wDeskPos=0;
	m_wDeskCount=1;
	//m_wMobileUserRule=VIEW_MODE_PART|VIEW_INFO_LEVEL_1;

	//房间辅助
	m_dwTokenID=0;
	m_dwClientIP=INADDR_ANY;		
	m_dwInoutIndex=INVALID_DWORD;
	m_wBindIndex=INVALID_WORD_INDEX;

	//低保信息
	ZeroMemory(&m_SubsidyInfo, sizeof(m_SubsidyInfo));

	//配置信息
	m_wServerType = 0;

	//组件接口
	m_pIServerUserItemSink=NULL;

	//属性变量
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	ZeroMemory(&m_UserRule,sizeof(m_UserRule));
	ZeroMemory(&m_ScoreFormer,sizeof(m_ScoreFormer));
	ZeroMemory(&m_ScoreVariation,sizeof(m_ScoreVariation));
	ZeroMemory(&m_RecordInfo,sizeof(m_RecordInfo));

	return;
}

//析构函数
CServerUserItem::~CServerUserItem()
{
}

//接口查询
VOID * CServerUserItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServerUserItem,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IServerUserItem,Guid,dwQueryVer);
	return NULL;
}

//更新点数
VOID CServerUserItem::UpdateMemberPoint(DWORD dwIncPoint)
{
	//更新点数
	m_UserInfo.dwMemberPoint += dwIncPoint;

	//升级处理
	if (dwIncPoint > 0)
	{
		BYTE cbLastOrder = m_cbMemberOrder;
		m_cbMemberOrder = g_GameLevelManager->CalcUserOrder(m_UserInfo.dwMemberPoint);

		//升级通知
		if (m_cbMemberOrder > cbLastOrder)
		{
			//事件通知
			ASSERT(m_pIServerUserItemSink != NULL);
			if (m_pIServerUserItemSink != NULL) m_pIServerUserItemSink->OnEventUserUserLevel(this, cbLastOrder, m_cbMemberOrder);
		}
	}
}

//用户胜率
WORD CServerUserItem::GetUserWinRate()
{
	//计算胜率
	LONG lPlayCount=GetUserPlayCount();
	if (lPlayCount!=0L) return (WORD)(m_UserInfo.lWinCount*10000L/lPlayCount);

	return 0;
}

//用户输率
WORD CServerUserItem::GetUserLostRate()
{
	//计算输率
	LONG lPlayCount=GetUserPlayCount();
	if (lPlayCount!=0L) return (WORD)(m_UserInfo.lLostCount*10000L/lPlayCount);

	return 0;
}

//用户和率
WORD CServerUserItem::GetUserDrawRate()
{
	//计算和率
	LONG lPlayCount=GetUserPlayCount();
	if (lPlayCount!=0L) return (WORD)(m_UserInfo.lDrawCount*10000L/lPlayCount);

	return 0;
}

//用户逃率
WORD CServerUserItem::GetUserFleeRate()
{
	//计算逃率
	LONG lPlayCount=GetUserPlayCount();
	if (lPlayCount!=0L) return (WORD)(m_UserInfo.lFleeCount*10000L/lPlayCount);

	return 0;
}

//对比帐号
bool CServerUserItem::ContrastAccounts(LPCTSTR pszAccounts)
{
	//效验参数
	ASSERT(pszAccounts!=NULL);
	if (pszAccounts==NULL) return false;

	//长度对比
	INT nContrastLen=lstrlen(pszAccounts);
	INT nSourceLen=lstrlen(m_UserInfo.szAccounts);
	if (nContrastLen!=nSourceLen) return false;

	//字符对比
	TCHAR wSourceChar=0,wContrastChar=0;
	for (INT i=0;i<nSourceLen;i++)
	{
		//获取字符
		wContrastChar=pszAccounts[i];
		wSourceChar=m_UserInfo.szAccounts[i];

		//转换字符
		if ((wSourceChar>=L'A')&&(wSourceChar<=L'Z')) wSourceChar+=32;
		if ((wContrastChar>=L'A')&&(wContrastChar<=L'Z')) wContrastChar+=32;

		//对比字符
		if (wSourceChar!=wContrastChar) return false;
	}

	return true;
}

//对比密码
bool CServerUserItem::ContrastPassword(LPCTSTR pszPassword)
{
	//效验参数
	ASSERT(pszPassword!=NULL);
	if (pszPassword==NULL) return false;

	//密码对比
	for (INT i=0;i<(CountArray(m_szPassword)-1);i++)
	{
		if (pszPassword[i]==0) return false;
		if (pszPassword[i]!=m_szPassword[i]) return false;
	}

	return true;
}

//对比临时密码
bool CServerUserItem::ContrastTempPassword(LPCTSTR pszPassword)
{
	//效验参数
	ASSERT(pszPassword!=NULL);
	if (pszPassword==NULL) return false;

	return lstrcmp(m_szTmpPassword,pszPassword)==0;
}

//对比桌子密码
bool CServerUserItem::ContrastTablePassword(LPCTSTR pszPassword)
{
	//效验参数
	ASSERT(pszPassword!=NULL);
	if (pszPassword==NULL) return false;

	return lstrcmp(m_UserRule.szPassword,pszPassword)==0;
}

//设置连接
VOID CServerUserItem::SetClientReady(bool bClientReady)
{
	m_bClientReady = bClientReady; 
}

//调控状态
BYTE CServerUserItem::GetRegulateStatus(SCORE lScore)
{
	//控制状态
	if (m_UserControlItem.cbControlStatus != CONTROL_STATUS_NORMAL)
	{
		//输赢积分
		SCORE lTotalWinScore = m_UserControlItem.lTotalWinScore;
		SCORE lTotalLoseScore = m_UserControlItem.lTotalLoseScore;

		//调整变量
		if (lScore > 0) lTotalWinScore += lScore;
		if (lScore < 0) lTotalLoseScore -= lScore;

		//白名单
		if (m_UserControlItem.cbControlStatus == CONTROL_STATUS_WHITELIST)
		{
			//计算胜率
			WORD wWinRate = static_cast<WORD>((lTotalWinScore / static_cast<double>(lTotalWinScore + lTotalLoseScore + 1)) * 100);

			//校验胜率
			if (wWinRate > m_UserControlItem.wControlWinRate)
			{
				//计算胜率差
				auto wDifRate = m_UserControlItem.wControlWinRate - wWinRate;

				return CONTROL_STATUS_NORMAL;
			}
		}

		//黑名单
		if (m_UserControlItem.cbControlStatus == CONTROL_STATUS_BLACKLIST)
		{
			//计算胜率
			WORD wWinRate = static_cast<WORD>((lTotalWinScore / static_cast<double>(lTotalWinScore + lTotalLoseScore + 1)) * 100);

			//校验胜率
			if (wWinRate < m_UserControlItem.wControlWinRate)
			{
				//计算胜率差
				auto wDifRate = m_UserControlItem.wControlWinRate - wWinRate;

				return CONTROL_STATUS_NORMAL;
			}
		}
	}

	return m_UserControlItem.cbControlStatus;
}

//预期变更
bool CServerUserItem::IsExpectScoreChange(SCORE lScore)
{
	//校验参数
	if (lScore == 0) return false;

	//获取状态
	BYTE cbControlStatus = GetControlStatus();
	BYTE cbBeforeStatus = GetRegulateStatus(0);
	BYTE cbAfterStatus = GetRegulateStatus(lScore);

	//状态一致
	if (cbAfterStatus == cbControlStatus)
	{
		//白名单用户
		if (cbControlStatus == CONTROL_STATUS_WHITELIST)
		{
			if (lScore > 0) return true;
		}

		//黑名单用户
		if (cbControlStatus == CONTROL_STATUS_BLACKLIST)
		{
			if (lScore < 0) return true;
		}

		return false;		
	}
	
	//普通状态
	if (cbAfterStatus == cbBeforeStatus)
	{
		//白名单用户
		if (cbControlStatus == CONTROL_STATUS_WHITELIST)
		{
			if (lScore <0) return true;
		}

		//黑名单用户
		if (cbControlStatus == CONTROL_STATUS_BLACKLIST)
		{
			if (lScore >0) return true;
		}		
	}
	else
	{
		//白名单用户
		if (cbControlStatus == CONTROL_STATUS_WHITELIST)
		{
			if (rand() % 100 <= 50) return true;
		}

		//黑名单用户
		if (cbControlStatus == CONTROL_STATUS_BLACKLIST)
		{
			if (rand() % 100 <= 50) return true;
		}
	}	

	return false;
}

//比赛分数
VOID CServerUserItem::ModifyUserScore(SCORE lModifyScore, const tagLockScoreInfo& LockScoreInfo)
{
	//设置积分
	m_lFreeScore += lModifyScore;
	m_UserInfo.lScore += lModifyScore;

	//同步金币
	if (m_UserInfo.cbScoreKind == SCORE_KIND_GOLD)
	{
		m_UserInfo.lGold += lModifyScore;
	}
}

//设置状态
bool CServerUserItem::SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//比较状态
	if( m_UserInfo.cbUserStatus != cbUserStatus )
	{
		//设置游戏时长
		if( cbUserStatus == US_PLAYING )
		{
			m_dwStartTimeCount = (DWORD)time(NULL);
			m_dwPlayTimeVariation = m_dwStartTimeCount;
		}
		else if( m_UserInfo.cbUserStatus == US_PLAYING )
		{
			ASSERT(m_dwStartTimeCount!=0);	
			m_dwPlayTimeCount = (DWORD)time(NULL)-m_dwStartTimeCount;
			m_dwTotalPlayTimeCount += m_dwPlayTimeCount; 
			m_RecordInfo.dwPlayTimeCount+=m_dwPlayTimeCount;

			//经验数值
			//m_UserInfo.lExperience+=(m_dwPlayTimeCount+59L)/60L;
			//m_ScoreVariation.lExperience+=(m_dwPlayTimeCount+59L)/60L;
			//m_RecordInfo.VariationScore.lExperience+=(m_dwPlayTimeCount+59L)/60L;

			m_dwStartTimeCount = 0;
			m_dwPlayTimeVariation = 0;
		}
	}

	//设置状态
	//m_wLastTableID=(m_UserInfo.wTableID==INVALID_WORD)?wTableID:m_UserInfo.wTableID;
	//m_wLastChairID=(m_UserInfo.wChairID==INVALID_WORD)?wChairID:m_UserInfo.wChairID;

	m_wLastTableID=m_UserInfo.wTableID;
	m_wLastChairID=m_UserInfo.wChairID;

	//设置变量
	m_UserInfo.wTableID=wTableID;
	m_UserInfo.wChairID=wChairID;
	m_UserInfo.cbUserStatus=cbUserStatus;	

	//状态调整
	if ((cbUserStatus == US_OFFLINE) || (cbUserStatus == US_NULL))
	{
		m_dwTokenID = 0;	
		m_wBindIndex = INVALID_WORD_INDEX;
	}

	//发送状态
	ASSERT(m_pIServerUserItemSink!=NULL);
	m_pIServerUserItemSink->OnEventUserItemStatus(this,m_wLastTableID,m_wLastChairID);

	return true;
}

//写入财富
bool CServerUserItem::WriteUserWealth(WORD wMoneyID, SCORE lScore, WORD wReason)
{
	//效验状态
	ASSERT((m_UserInfo.dwUserID != 0L));
	if (m_UserInfo.dwUserID == 0L) return false;

	//构造结构
	tagWealthVariation WealthVariation;
	ZeroMemory(&WealthVariation, sizeof(WealthVariation));

	//设置变量
	WealthVariation.wReason = wReason;
	WealthVariation.wMoneyID = wMoneyID;
	WealthVariation.lVariationScore = lScore;
	WealthVariation.cbSyncTarget = SYNC_TARGET_CLIENT | SYNC_TARGET_DATABASE;

	//变更时间
	GetLocalTime(&WealthVariation.VariationTime);

	//货币标识
	switch (wMoneyID)
	{	
	case GOODS_ID_CARD:			//房卡
	{
		WealthVariation.lBefore = m_UserInfo.lUserCard;
		m_UserInfo.lUserCard += lScore;
		WealthVariation.lBalance = m_UserInfo.lUserCard;

		break;
	}
	case GOODS_ID_GOLD:			//黄金
	{
		WealthVariation.lBefore = m_UserInfo.lGold;
		m_UserInfo.lGold += lScore;
		WealthVariation.lBalance = m_UserInfo.lGold;

		//金币类型
		if (m_UserInfo.cbScoreKind==SCORE_KIND_GOLD)
		{
			//更新分数
			m_lFreeScore += lScore;
			m_lLockScore = m_lFreeScore;
			m_UserInfo.lScore += lScore;			
			m_RecordInfo.VariationScore.lScore += lScore;	

			//同步掩码
			WealthVariation.cbSyncTarget |= SYNC_TARGET_GAMESERVER;			
		}

		break;
	}	
	}

	//同步财富
	if (wReason == GOODS_REASON_SYNC)
	{
		WealthVariation.cbSyncTarget &= ~SYNC_TARGET_DATABASE;
	}
	
	//财富变更
	ASSERT(m_pIServerUserItemSink != NULL);
	if (m_pIServerUserItemSink != NULL) m_pIServerUserItemSink->OnEventUserItemWealth(this, WealthVariation);

	return true;
}

//写入积分
bool CServerUserItem::WriteUserScore(SCORE lGameScore, SCORE lRevenue, SCORE lBetAmount, LONG lExperience, enScoreType ScoreType)
{
	/*CString s;
	s.Format(_T("写入积分,lGameScore:%I64d, lRevenue:%I64d, lBetAmount::%I64d, lExperience:%d, ScoreType:%d"), lGameScore, lRevenue, lBetAmount, lExperience, ScoreType);
	CTraceService::TraceString(s, TraceLevel_Exception);*/
	//效验状态
	ASSERT ((m_UserInfo.dwUserID!=0L)&&(ScoreType!=ScoreType_Null));
	//if ((m_UserInfo.dwUserID==0L)||(ScoreType==ScoreType_Null)) return false;//kk guan
	if (m_UserInfo.dwUserID == 0L)  return false;//kk jia
	

	//修改胜负
	switch (ScoreType)
	{
	case ScoreType_Win:		//胜局
		{
			//统计局数
			m_UserInfo.lWinCount++;
			m_ScoreVariation.lWinCount++;
			m_RecordInfo.VariationScore.lWinCount++;

			//连胜处理
			if (++m_UserInfo.lWinStreak > m_UserInfo.lMaxWinStreak)
			{
				m_UserInfo.lMaxWinStreak = m_UserInfo.lWinStreak;
			}

			//同步变量
			m_ScoreVariation.lWinStreak = m_UserInfo.lWinStreak;
			m_ScoreVariation.lMaxWinStreak = m_UserInfo.lMaxWinStreak;

			break;
		}
	case ScoreType_Lose:	//输局
		{
			//统计局数
			m_UserInfo.lLostCount++;
			m_ScoreVariation.lLostCount++;
			m_RecordInfo.VariationScore.lLostCount++;

			//连胜处理
			m_UserInfo.lWinStreak = 0;
			m_ScoreVariation.lWinStreak = m_UserInfo.lWinStreak;			

			break;
		}
	case ScoreType_Draw:	//和局
		{
			//统计局数
			m_UserInfo.lDrawCount++;
			m_ScoreVariation.lDrawCount++;
			m_RecordInfo.VariationScore.lDrawCount++;
			break;
		}
	case ScoreType_Flee:	//逃局
		{
			//统计局数
			m_UserInfo.lFleeCount++;
			m_ScoreVariation.lFleeCount++;
			m_RecordInfo.VariationScore.lFleeCount++;

			//连胜处理
			m_UserInfo.lWinStreak = 0;
			m_ScoreVariation.lWinStreak = m_UserInfo.lWinStreak;

			break;
		}
	}

	//统计下注
	m_ScoreVariation.lBetAmount+=lBetAmount;
	m_RecordInfo.VariationScore.lBetAmount+=lBetAmount;

	//设置积分
	m_bModifyScore=true;
	m_lFreeScore+=lGameScore;
	m_UserInfo.lScore+=lGameScore;
	m_ScoreVariation.lScore+=lGameScore;
	m_RecordInfo.VariationScore.lScore+=lGameScore;

	//更新经验
	m_UserInfo.lExperience += lExperience;
	m_ScoreVariation.lExperience += lExperience;
	m_RecordInfo.VariationScore.lExperience += lExperience;

	//同步金币
	if (m_UserInfo.cbScoreKind==SCORE_KIND_GOLD)
	{
		m_UserInfo.lGold += lGameScore;
	}

	//排位房间
	if (m_wServerType&GAME_GENRE_RANKING && m_UserInfo.lScore<0)
	{
		//调整记录
		m_ScoreVariation.lScore -= m_UserInfo.lScore;
		m_RecordInfo.VariationScore.lScore -= m_UserInfo.lScore;

		//调整积分
		m_UserInfo.lScore = 0;
	}

	//附加变量
	m_lRevenue+=lRevenue;
	m_RecordInfo.lRevenue+=lRevenue;

	//更新状态
	UpdateControlStatus(lGameScore, lBetAmount);

	//发送状态
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemScore(this);	

	//游戏等级
	if (lExperience > 0)
	{
		//获取等级
		auto cbLastOrder = m_cbGameOrder;
		m_cbGameOrder = g_GameLevelManager->CalcGameOrder(m_UserInfo.lExperience);

		//升级通知
		if (m_cbGameOrder > cbLastOrder)
		{
			ASSERT(m_pIServerUserItemSink != NULL);
			if (m_pIServerUserItemSink != NULL) m_pIServerUserItemSink->OnEventUserGameLevel(this, cbLastOrder, m_cbGameOrder);
		}
	}

	return true;
}

//设置临时密码
VOID CServerUserItem::SetTmpPassword( LPCTSTR lpszPassword )
{
	//效验参数
	ASSERT(lpszPassword!=NULL);
	if (lpszPassword==NULL) return;

	StringCchCopy( m_szTmpPassword,CountArray(m_szTmpPassword), lpszPassword);

	return;
}

//修改权限
VOID CServerUserItem::ModifyUserRight( DWORD dwAddRight, DWORD dwRemoveRight )
{
	//添加权限
	m_dwUserRight |= dwAddRight;

	//删除权限
	m_dwUserRight &= ~dwRemoveRight;

	//发送状态
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemRight(this,dwAddRight,dwRemoveRight);

	return;
}

//设置参数
bool CServerUserItem::SetUserParameter(DWORD dwClientIP, WORD wBindIndex, DWORD dwTokenID, bool bRobotUser, bool bClientReady)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//设置变量
	m_dwTokenID=dwTokenID;
	m_wBindIndex=wBindIndex;
	m_dwClientIP=dwClientIP;
	m_bRobotUser=bRobotUser;
	m_bClientReady=bClientReady;

	return true;
}

//提取变更
bool CServerUserItem::DistillVariation(tagVariationScore & VariationScore, const tagLockScoreInfo & LockScoreInfo)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//修改判断
	if (m_bModifyScore==true)
	{
		//设置变量
		VariationScore.lUnLockScore=0L;
		VariationScore.lRevenue=m_lRevenue;
		VariationScore.VariationScore=m_ScoreVariation;

		//游戏时长
		if( m_dwPlayTimeVariation != 0 )
		{
			DWORD dwCurrentTime = (DWORD)time(NULL);
			VariationScore.dwPlayTimeCount = dwCurrentTime-m_dwPlayTimeVariation;
			m_dwPlayTimeVariation = dwCurrentTime;
		}

		if(LockScoreInfo.lRestrictScore>0)
		{
			m_lLockScore+=m_lFreeScore;
			m_lFreeScore=LockScoreInfo.lRestrictScore;
			m_lLockScore-=LockScoreInfo.lRestrictScore;
		}
		else
		{
			//设置锁定
			if (m_ScoreVariation.lScore>0L)
			{
				//设置变量
				m_lFreeScore=m_lLockScore;
				VariationScore.lUnLockScore=0L;

				//增加锁定
				if (m_lLockScore<LockScoreInfo.lMaxLockScore+m_lFreezeScore)
				{
					//申请计算
					SCORE lWishAugment=LockScoreInfo.lMaxLockScore+m_lFreezeScore-m_lLockScore;
					SCORE lAllowAugment=__min(lWishAugment,m_ScoreVariation.lScore);

					//设置变量
					m_lLockScore+=lAllowAugment;
					m_lFreeScore+=lAllowAugment;
					VariationScore.lUnLockScore-=lAllowAugment;
				}
			}
			else
			{
				//调整锁定
				m_lLockScore=m_lFreeScore;
				VariationScore.lUnLockScore-=m_ScoreVariation.lScore;
			}
		}
	}
	else
	{
		//设置变量
		ZeroMemory(&VariationScore,sizeof(VariationScore));

		//连胜信息
		VariationScore.VariationScore.lWinStreak = m_UserInfo.lWinStreak;
		VariationScore.VariationScore.lMaxWinStreak = m_UserInfo.lMaxWinStreak;
	}

	if (LockScoreInfo.lRestrictScore==0)
	{
		//释放锁定
		if (m_lLockScore>LockScoreInfo.lMaxLockScore+m_lFreezeScore)
		{
			SCORE lLastLockScore=m_lLockScore;
			SCORE lMaxLockScore = LockScoreInfo.lMaxLockScore+m_lFreezeScore;
			m_lFreeScore=lMaxLockScore;
			m_lLockScore=lMaxLockScore;
			VariationScore.lUnLockScore+=lLastLockScore-lMaxLockScore;
		}
	}

	//还原变量
	m_lRevenue=0L;
	m_bModifyScore=false;
	ZeroMemory(&m_ScoreVariation,sizeof(m_ScoreVariation));

	return true;
}

//设置接口
VOID CServerUserItem::SetServerUserItemSink(IServerUserItemSink* pIServerUserItemSink)
{
	m_pIServerUserItemSink = pIServerUserItemSink;
	m_ServerUserPack.m_pIServerUserItemSink = pIServerUserItemSink;	
}

//重置数据
VOID CServerUserItem::ResetUserItem()
{
	//状态变量
	m_dwLookonTime=0;
	m_wLastTableID=INVALID_WORD;
	m_wLastChairID=INVALID_WORD;

	//权限变量
	m_dwUserRight=0L;
	m_dwMasterRight=0L;

	//位置信息
	m_dGlobalPosLng=0.f;
	m_dGlobalPosLat=0.f;
	ZeroMemory(m_szPlaceName,sizeof(m_szPlaceName));

	//状态变量
	m_lNearInfo=0L;
	m_lNearCount=0L;
	m_dwLogonTime=0L;
	m_dwPlayTimeCount=0L;
	m_dwTotalPlayTimeCount = 0L;
	m_dwStartTimeCount = 0L;
	m_dwPlayTimeVariation = 0L;

	//积分变量
	m_lRevenue=0L;
	m_lFreeScore=0L;
	m_lLockScore=0L;	
	m_lFreezeScore = 0L;
	m_lLockServiceScore = 0L;

	//游戏辅助
	m_bTrusteeUser=false;
	m_bRobotUser=false;
	m_bClientReady=false;
	m_bModifyScore=false;
	ZeroMemory(m_szPassword,sizeof(m_szPassword));
	ZeroMemory(m_szTmpPassword,sizeof(m_szTmpPassword));
	m_dwInoutIndex=INVALID_DWORD;

	//黑白名单
	m_wAddupWinRate = 0;
	m_lAddupWinScore = 0;
	m_lAddupLoseScore = 0;
	m_pControlParameter = NULL;
	ZeroMemory(&m_UserControlItem, sizeof(m_UserControlItem));

	//比赛信息
	m_lMatchNo=0;
	m_dwMatchID=0;	
	m_wSignupCount=0;
	m_dwSignUpTime=0;
	m_dwTicketCount=0;
	m_cbMatchStatus=MUS_NULL;

	//等级信息
	m_cbGameOrder = 0;
	m_cbMemberOrder = 0;

	//状态信息
	m_cbShutUpStatus = 0;
	m_dwShutUpOverTime = 0;

	//低保信息
	ZeroMemory(&m_SubsidyInfo, sizeof(m_SubsidyInfo));

	//配置信息
	m_wServerType = 0;

	//房间辅助
	m_dwTokenID=0;
	m_dwClientIP=INADDR_ANY;
	m_wBindIndex=INVALID_WORD_INDEX;

	//手机定义	
	m_wDeskPos=0;
	m_wDeskCount=1;
	//m_wMobileUserRule=VIEW_MODE_PART|VIEW_INFO_LEVEL_1;	

	//属性变量
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	ZeroMemory(&m_UserRule,sizeof(m_UserRule));
	ZeroMemory(&m_ScoreFormer,sizeof(m_ScoreFormer));
	ZeroMemory(&m_ScoreVariation,sizeof(m_ScoreVariation));
	ZeroMemory(&m_RecordInfo,sizeof(m_RecordInfo));

	return;
}

//更新状态
VOID CServerUserItem::UpdateControlStatus(SCORE lScore, SCORE lBetAmount)
{
	//校验分数
	//if (lScore == 0) return;
	
	//计算赢分
	auto lWinScore = static_cast<SCORE>((m_lAddupWinScore / static_cast<double>(m_lAddupWinScore + m_lAddupLoseScore + 1)) * m_pControlParameter->lBenchmarkValue);

	//计算胜率
	if (lScore < 0)
	{
		m_lAddupLoseScore -= lScore;
		m_wAddupWinRate = static_cast<WORD>((lWinScore / static_cast<double>(m_pControlParameter->lBenchmarkValue - lScore)) * 100);
	}

	//计算胜率
	if (lScore > 0)
	{
		m_lAddupWinScore += lScore;
		m_wAddupWinRate = static_cast<WORD>(((lWinScore + lScore) / static_cast<double>(m_pControlParameter->lBenchmarkValue)) * 100);
	}

	//校验开关
	if (m_pControlParameter == NULL || !m_pControlParameter->bControlEnabled) return;
	if (m_UserControlItem.cbControlStatus == CONTROL_STATUS_NORMAL && (m_pControlParameter->wSupportControlMode & CONTROL_MODE_AUTO) == 0) return;

	//记录状态
	BYTE cbLastControlStatus = m_UserControlItem.cbControlStatus;

	//普通状态
	if (m_UserControlItem.cbControlStatus == CONTROL_STATUS_NORMAL)
	{
		//校验局数
		if (m_pControlParameter->wStartPlayCount < GetUserPlayCount())
		{
			//白名单
			if (m_wAddupWinRate <= m_pControlParameter->wInWhiteListRate)
			{
				m_UserControlItem.cbControlMode = CONTROL_MODE_AUTO;
				m_UserControlItem.cbControlKind = CONTROL_KIND_RATE;
				m_UserControlItem.cbControlStatus = CONTROL_STATUS_WHITELIST;
				m_UserControlItem.wControlWinRate = m_pControlParameter->wWLControlRate;
				m_UserControlItem.wTargetWinRate = m_pControlParameter->wOutWhiteListRate;
			}

			//黑名单
			if (m_wAddupWinRate >= m_pControlParameter->wInBlackListRate)
			{
				m_UserControlItem.cbControlMode = CONTROL_MODE_AUTO;
				m_UserControlItem.cbControlKind = CONTROL_KIND_RATE;
				m_UserControlItem.cbControlStatus = CONTROL_STATUS_BLACKLIST;
				m_UserControlItem.wControlWinRate = m_pControlParameter->wBLControlRate;
				m_UserControlItem.wTargetWinRate = m_pControlParameter->wOutBlackListRate;
			}
		}
	}
	else
	{
		if (lScore <= 0)
		{
			m_UserControlItem.lTotalLoseCount++;
			m_UserControlItem.lTotalLoseScore -= lScore;
		}
		else
		{
			m_UserControlItem.lTotalWinCount++;
			m_UserControlItem.lTotalWinScore += lScore;
		}

		//统计打码量
		m_UserControlItem.lTotalBetAmount += lBetAmount;

		//胜率控制
		if (m_UserControlItem.cbControlKind == CONTROL_KIND_RATE)
		{
			//白名单
			if (m_UserControlItem.cbControlStatus == CONTROL_STATUS_WHITELIST)
			{
				//校验胜率
				if (m_wAddupWinRate >= m_UserControlItem.wTargetWinRate)
				{
					m_UserControlItem.cbControlStatus = CONTROL_STATUS_NORMAL;
				}
			}

			//黑名单
			if (m_UserControlItem.cbControlStatus == CONTROL_STATUS_BLACKLIST)
			{
				//校验胜率
				if (m_wAddupWinRate <= m_UserControlItem.wTargetWinRate)
				{
					m_UserControlItem.cbControlStatus = CONTROL_STATUS_NORMAL;
				}
			}
		}

		//时间控制
		if (m_UserControlItem.cbControlKind == CONTROL_KIND_TIME)
		{
			//当前时间
			DWORD dwCurrentTime = time(NULL);

			//校验时间
			if (m_UserControlItem.dwControlStartTime + m_UserControlItem.dwControlDuration <= dwCurrentTime)
			{
				m_UserControlItem.cbControlStatus = CONTROL_STATUS_NORMAL;				
			}
		}

		//局数控制
		if (m_UserControlItem.cbControlKind == CONTROL_KIND_COUNT)
		{
			//校验局数
			if (m_UserControlItem.wControlPlayCount <= m_UserControlItem.lTotalWinCount+ m_UserControlItem.lTotalLoseCount)
			{
				m_UserControlItem.cbControlStatus = CONTROL_STATUS_NORMAL;
			}
		}
	}

	//校验状态
	if (m_UserControlItem.cbControlStatus != cbLastControlStatus)
	{
		if (m_UserControlItem.cbControlStatus == CONTROL_STATUS_NORMAL)
		{
			//发送状态
			ASSERT(m_pIServerUserItemSink != NULL);
			if (m_pIServerUserItemSink != NULL) m_pIServerUserItemSink->OnEventUserControlFinish(this);

			//清理数据
			ZeroMemory(&m_UserControlItem, sizeof(m_UserControlItem));
		}
		else
		{
			//发送状态
			ASSERT(m_pIServerUserItemSink != NULL);
			if (m_pIServerUserItemSink != NULL) m_pIServerUserItemSink->OnEventUserControlStart(this);
		}
	}
	else if (m_UserControlItem.cbControlStatus != CONTROL_STATUS_NORMAL && m_UserControlItem.cbControlKind == CONTROL_KIND_CUSTOM)
	{
		//发送状态
		ASSERT(m_pIServerUserItemSink != NULL);
		if (m_pIServerUserItemSink != NULL) m_pIServerUserItemSink->OnEventUserControlUpdate(this);
	}
}

//冻结积分
bool CServerUserItem::FreezeUserScore( SCORE lScore, bool bNotify )
{
	ASSERT (m_lFreezeScore+lScore>=0);
	m_lFreezeScore += lScore;

	if (bNotify && m_pIServerUserItemSink)
		m_pIServerUserItemSink->OnEventUserFreezeScore(this,m_lFreezeScore);

	return true;
}

//解除绑定
bool CServerUserItem::DetachBindStatus()
{
	//效验状态
	ASSERT (m_UserInfo.dwUserID != 0L);
	if (m_UserInfo.dwUserID == 0L) return false;

	//用户属性
	m_bClientReady = false;

	//连接属性
	m_dwTokenID = 0;
	m_wBindIndex = INVALID_WORD_INDEX;

	return true;
}

//查询记录
bool CServerUserItem::QueryRecordInfo(tagVariationScore & RecordInfo)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID != 0L);
	if (m_UserInfo.dwUserID == 0L) return false;

	//设置变量
	RecordInfo = m_RecordInfo;

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CServerUserManager::CServerUserManager()
{
	//组件接口
	m_pControlParameter=NULL;
	m_pGameServiceOption=NULL;
	m_pIServerUserItemSink=NULL;

	//设置质数
	m_UserIDMap.InitHashTable(PRIME_SERVER_USER);

	return;
}

//析构函数
CServerUserManager::~CServerUserManager()
{
	//释放用户
	for (INT_PTR i=0;i<m_UserItemStore.GetCount();i++) m_UserItemStore[i]->Release();
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++) m_UserItemArray[i]->Release();

	//删除数据
	m_UserIDMap.RemoveAll();
	m_UserItemStore.RemoveAll();
	m_UserItemArray.RemoveAll();

	return;
}

//接口查询
VOID * CServerUserManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServerUserManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IServerUserManager,Guid,dwQueryVer);
	return NULL;
}

//控制参数
VOID CServerUserManager::SetControlParamter(tagControlParameter* pControlParameter)
{
	m_pControlParameter = pControlParameter;
}

//服务参数
VOID CServerUserManager::SetServiceParameter(tagGameServiceOption* pGameServiceOption)
{
	//设置变量
	m_pGameServiceOption = pGameServiceOption;
}

//设置接口
bool CServerUserManager::SetServerUserItemSink(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if (pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IServerUserItemSink)!=NULL);
		m_pIServerUserItemSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IServerUserItemSink);

		//成功判断
		if (m_pIServerUserItemSink==NULL) return false;
	}
	else m_pIServerUserItemSink=NULL;

	return true;
}

//枚举用户
IServerUserItem * CServerUserManager::EnumUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserItemArray.GetCount()) return NULL;
	return m_UserItemArray[wEnumIndex];
}

//查找用户
IServerUserItem * CServerUserManager::SearchUserItem(DWORD dwUserID)
{
	CServerUserItem * pServerUserItem=NULL;
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		pServerUserItem=m_UserItemArray[i];
		if (pServerUserItem->m_UserInfo.dwUserID==dwUserID) return pServerUserItem;
	}

	return NULL;
}

//查找用户
IServerUserItem * CServerUserManager::SearchUserItem(LPCTSTR pszAccounts)
{
	CServerUserItem * pServerUserItem=NULL;
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		pServerUserItem=m_UserItemArray[i];
		if (pServerUserItem->ContrastAccounts(pszAccounts)==true) return pServerUserItem;
	}

	return NULL;
}

//激活用户
IServerUserItem * CServerUserManager::ActiveUserItem(tagServerUserInfo & UserInfo, tagUserInfoPlus & UserInfoPlus, LPCTSTR pszPassword)
{
	//变量定义
	CServerUserItem * pServerUserItem=NULL;

	//获取指针
	if (m_UserItemStore.GetCount()>0)
	{
		INT_PTR nItemPostion=m_UserItemStore.GetCount()-1;
		pServerUserItem=m_UserItemStore[nItemPostion];		
		m_UserItemStore.RemoveAt(nItemPostion);
		pServerUserItem->ResetUserItem();
	}
	else
	{
		try
		{
			pServerUserItem=new CServerUserItem;			
		}
		catch (...) { return NULL; }
	}

	//设置接口
	pServerUserItem->SetServerUserItemSink(m_pIServerUserItemSink);

	//属性变量
	pServerUserItem->m_ScoreFormer.lScore=UserInfo.lScore;
	pServerUserItem->m_ScoreFormer.cbScoreKind=UserInfo.cbScoreKind;
	pServerUserItem->m_ScoreFormer.lWinCount=UserInfo.lWinCount;
	pServerUserItem->m_ScoreFormer.lLostCount=UserInfo.lLostCount;
	pServerUserItem->m_ScoreFormer.lDrawCount=UserInfo.lDrawCount;
	pServerUserItem->m_ScoreFormer.lFleeCount=UserInfo.lFleeCount;
	pServerUserItem->m_ScoreFormer.lExperience=UserInfo.lExperience;
	pServerUserItem->m_ScoreFormer.lWinStreak=UserInfo.lWinStreak;
	pServerUserItem->m_ScoreFormer.lMaxWinStreak=UserInfo.lMaxWinStreak;
	CopyMemory(&pServerUserItem->m_UserInfo,&UserInfo,sizeof(UserInfo));
	ZeroMemory(&pServerUserItem->m_RecordInfo, sizeof(pServerUserItem->m_RecordInfo));
	ZeroMemory(&pServerUserItem->m_ScoreVariation,sizeof(pServerUserItem->m_ScoreVariation));	

	//权限变量
	pServerUserItem->m_dwUserRight=UserInfoPlus.dwUserRight;
	pServerUserItem->m_dwMasterRight=UserInfoPlus.dwMasterRight;

	//积分变量
	if (UserInfoPlus.lRestrictScore==0)
	{
		pServerUserItem->m_lFreeScore=UserInfoPlus.lFreeScore;
		pServerUserItem->m_lLockScore=UserInfoPlus.lFreeScore;
	}
	else
	{
		pServerUserItem->m_lFreeScore=UserInfoPlus.lRestrictScore;
		pServerUserItem->m_lLockScore=UserInfoPlus.lFreeScore-UserInfoPlus.lRestrictScore;
	}

	pServerUserItem->m_lFreezeScore = UserInfoPlus.lFreezeScore;
	pServerUserItem->m_lLockServiceScore = UserInfoPlus.lLockServiceScore;	

	//状态变量
	pServerUserItem->m_dwPlayTimeCount=0L;
	pServerUserItem->m_dwTotalPlayTimeCount = 0L;
	pServerUserItem->m_dwLogonTime=static_cast<DWORD>(time(NULL));

	//标识变量
	pServerUserItem->m_bClientReady=false;
	pServerUserItem->m_bModifyScore=false;
	pServerUserItem->m_bTrusteeUser=false;
	pServerUserItem->m_bRobotUser=UserInfoPlus.bRobotUser;
	pServerUserItem->m_bMobileUser=UserInfoPlus.bMobileUser;	

	//计算等级
	pServerUserItem->m_cbGameOrder = g_GameLevelManager->CalcGameOrder(pServerUserItem->GetExperience());
	pServerUserItem->m_cbMemberOrder = g_GameLevelManager->CalcUserOrder(pServerUserItem->GetMemberPoint());

	//配置信息
	pServerUserItem->m_wServerType = m_pGameServiceOption->wServerType;

	//位置变量
	pServerUserItem->m_dGlobalPosLat=UserInfoPlus.dGlobalPosLat;
	pServerUserItem->m_dGlobalPosLng=UserInfoPlus.dGlobalPosLng;
	CopyMemory(pServerUserItem->m_szPlaceName, UserInfoPlus.szPlaceName,sizeof(pServerUserItem->m_szPlaceName));

	//状态变量
	pServerUserItem->m_cbShutUpStatus = UserInfoPlus.cbShutUpStatus;
	pServerUserItem->m_dwShutUpOverTime = UserInfoPlus.dwShutUpOverTime;

	//黑白名单
	pServerUserItem->m_pControlParameter = m_pControlParameter;
	pServerUserItem->m_wAddupWinRate = UserInfoPlus.wAddupWinRate;
	pServerUserItem->m_lAddupWinScore = UserInfoPlus.lAddupWinScore;
	pServerUserItem->m_lAddupLoseScore = UserInfoPlus.lAddupLoseScore;	
	CopyMemory(&pServerUserItem->m_UserControlItem, &UserInfoPlus.UserControlItem, sizeof(tagUserControlItem));

	//辅助变量
	pServerUserItem->m_dwTokenID=UserInfoPlus.dwTokenID;
	pServerUserItem->m_wBindIndex=UserInfoPlus.wBindIndex;	
	pServerUserItem->m_dwClientIP=UserInfoPlus.dwClientIP;
	pServerUserItem->m_bRobotUser=UserInfoPlus.bRobotUser;
	StringCchCopy(pServerUserItem->m_szMachineID, LEN_MACHINE_ID, UserInfoPlus.szMachineID);
	StringCchCopy(pServerUserItem->m_szPassword,CountArray(pServerUserItem->m_szPassword), pszPassword);	
	pServerUserItem->m_dwInoutIndex=UserInfoPlus.dwInoutIndex;

	//插入用户
	m_UserItemArray.Add(pServerUserItem);
	m_UserIDMap[UserInfo.dwUserID]=pServerUserItem;

	return pServerUserItem;
}

//删除用户
bool CServerUserManager::DeleteUserItem(IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserStatus()==US_NULL));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetUserStatus()!=US_NULL)) return false;

	//变量定义
	CServerUserItem * pTempUserItem=NULL;
	DWORD dwUserID=pIServerUserItem->GetUserID();

	//寻找对象
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		//获取用户
		pTempUserItem=m_UserItemArray[i];
		if (pIServerUserItem!=pTempUserItem) continue;

		//重置对象
		pTempUserItem->ResetUserItem();

		//删除对象
		m_UserItemArray.RemoveAt(i);
		m_UserIDMap.RemoveKey(dwUserID);
		m_UserItemStore.Add(pTempUserItem);

		return true;
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//删除用户
bool CServerUserManager::DeleteUserItem()
{
	//存储对象
	m_UserItemStore.Append(m_UserItemArray);

	//删除对象
	m_UserIDMap.RemoveAll();
	m_UserItemArray.RemoveAll();


	return true;
}
//////////////////////////////////////////////////////////////////////////
