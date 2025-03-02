#include "StdAfx.h"
#include "GlobalUnits.h"
#include "ServerUserManager.h"	

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////
//��������
#define TASK_SYNC_INTERVAL			5									//ͬ�����

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CServerUserPack::CServerUserPack(CServerUserItem* pServerUserItem)
{
	//���ñ���
	m_dwRecordID = 0;
	m_bTransMode = false;

	//���±���
	m_dwUpdateTime = 0;
	m_wUpdateMaxCount = 15;
	m_wUpdateInterval = 120;

	//ָ�����	
	m_pIServerUserItemSink = NULL;
	m_pServerUserItem = pServerUserItem;

	//��ʼ���ֿ�
	m_StockGoodsHoldInfo.InitHashTable(31);
	m_StockGoodsHoldTemp.InitHashTable(31);
	m_StockGoodsVariation.InitHashTable(31);
}

//��������
CServerUserPack::~CServerUserPack()
{
	ResetUserPack();
}

//��ʼ����
VOID CServerUserPack::BeginTrans()
{
	ASSERT(m_bTransMode == false);

	//�ύ����
	if (m_bTransMode == true)
	{
		SubmitTrans();
	}

	//��ʼ����
	m_bTransMode = true;
}

//�ύ����
VOID CServerUserPack::SubmitTrans()
{
	m_bTransMode = false;

	//���֪ͨ
	if (m_pIServerUserItemSink && m_GoodsIDList.GetCount() > 0)
	{
		//���֪ͨ
		m_pIServerUserItemSink->OnEventUserItemPack(m_pServerUserItem, m_GoodsIDList.GetData(), (WORD)m_GoodsIDList.GetCount());

		//�����Ʒ
		m_GoodsIDList.RemoveAll();
	}
}

//�Ƴ���Ʒ
bool CServerUserPack::RemoveGoods(WORD wGoodsID, WORD wReason)
{
	//������Ʒ
	auto GameGoodsItemPtr = g_GameGoodsManager->SearchGoodsItem<CGameGoodsItemPtr>(wGoodsID);
	if (!GameGoodsItemPtr)
	{
		return false;
	}

	//������Ʒ
	auto pGoodsHoldInfo = m_StockGoodsHoldInfo.SearchStockItem(wGoodsID);
	if (pGoodsHoldInfo != NULL && pGoodsHoldInfo->dwGoodsCount > 0)
	{
		//��ȡ���
		auto pGoodsVariation = GetGoodsVariation();

		//�����Ʒ
		m_GoodsIDList.Add(pGoodsHoldInfo->wGoodsID);

		//�����Ϣ
		pGoodsVariation->wReason = wReason;
		pGoodsVariation->wGoodsID = wGoodsID;
		pGoodsVariation->wGoodsKind = GameGoodsItemPtr->GetGoodsKind();
		pGoodsVariation->dwBeforeCount = pGoodsHoldInfo->dwGoodsCount;
		pGoodsVariation->lVariationCount -= pGoodsHoldInfo->dwGoodsCount;
		GetLocalTime(&pGoodsVariation->VariationTime);

		//�Զ��ύ
		if (m_bTransMode == false) SubmitTrans();
	}

	//�Ƴ���Ʒ
	m_StockGoodsHoldInfo.RemoveStockItem(wGoodsID);

	return true;
}

//�Ƴ���Ʒ
bool CServerUserPack::RemoveGoods(WORD wGoodsID, DWORD dwGoodsCount, WORD wReason)
{
	//������Ʒ
	auto GameGoodsItemPtr = g_GameGoodsManager->SearchGoodsItem<CGameGoodsItemPtr>(wGoodsID);
	if (!GameGoodsItemPtr)
	{
		return false;
	}

	//������Ʒ
	auto pGoodsHoldInfo = m_StockGoodsHoldInfo.SearchStockItem(wGoodsID);
	if (pGoodsHoldInfo == NULL || pGoodsHoldInfo->dwGoodsCount < dwGoodsCount)
	{
		return false;
	}

	//��ȡ���
	auto pGoodsVariation = GetGoodsVariation();

	//�����Ʒ
	m_GoodsIDList.Add(pGoodsHoldInfo->wGoodsID);

	//�����Ϣ
	pGoodsVariation->wReason = wReason;
	pGoodsVariation->wGoodsID = wGoodsID;
	pGoodsVariation->wGoodsKind = GameGoodsItemPtr->GetGoodsKind();
	pGoodsVariation->dwBeforeCount = pGoodsHoldInfo->dwGoodsCount;
	pGoodsVariation->lVariationCount -= dwGoodsCount;
	GetLocalTime(&pGoodsVariation->VariationTime);

	//������Ʒ	
	pGoodsHoldInfo->dwGoodsCount -= dwGoodsCount;

	//�Զ��ύ
	if (m_bTransMode == false) SubmitTrans();

	return true;
}

//������Ʒ
bool CServerUserPack::UpdateGoods(WORD wGoodsID, WORD wGoodsLevel, DWORD dwIndate, WORD wReason)
{
	//������Ʒ
	auto pGoodsHoldInfo = SearchGoods(wGoodsID);
	if (pGoodsHoldInfo == NULL)
	{
		return false;
	}

	//��ȡ���
	auto pGoodsVariation = GetGoodsVariation();

	//�����Ʒ
	m_GoodsIDList.Add(pGoodsHoldInfo->wGoodsID);

	//�����Ϣ
	pGoodsVariation->wReason = wReason;
	pGoodsVariation->wGoodsID = wGoodsID;
	pGoodsVariation->wGoodsKind = pGoodsHoldInfo->wGoodsKind;
	GetLocalTime(&pGoodsVariation->VariationTime);

	//�������
	pGoodsVariation->lVariationCount = 0;

	//���õȼ�
	pGoodsHoldInfo->wGoodsLevel = wGoodsLevel;

	//У����Ч��
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

		//������Ч��
		pGoodsHoldInfo->dwGoodsIndate = dwIndate;
	}

	//�Զ��ύ
	if (m_bTransMode == false) SubmitTrans();

	return true;
}

//�����Ʒ
bool CServerUserPack::AppendGoods(WORD wGoodsID, DWORD dwGoodsCount, WORD wGoodsLevel, DWORD dwIndate, WORD wReason)
{
	//������Ʒ
	auto GameGoodsItemPtr = g_GameGoodsManager->SearchGoodsItem<CGameGoodsItemPtr>(wGoodsID);
	if (!GameGoodsItemPtr)
	{
		return false;
	}

	//�洢У��
	if ((GameGoodsItemPtr->GetCanStorePlace() & STORE_PLACE_PACK) == 0)
	{
		return false;
	}

	//������Ʒ
	auto pGoodsHoldInfo = GetGoodsHoldInfo(wGoodsID);
	if (pGoodsHoldInfo == NULL)
	{
		return false;
	}

	//��ʼ����Ʒ
	if (pGoodsHoldInfo->dwGoodsCount == 0)
	{
		pGoodsHoldInfo->wGoodsKind = GameGoodsItemPtr->GetGoodsKind();
	}

	//У������
	if (!GameGoodsItemPtr->GetHasIndate() && dwGoodsCount == 0)
	{
		return true;
	}

	//У����Ч��
	if (GameGoodsItemPtr->GetHasIndate() && dwIndate > 0)
	{
		if (pGoodsHoldInfo->dwGoodsCount > 0 && pGoodsHoldInfo->dwGoodsIndate == 0)
		{
			return true;
		}
	}

	//��ȡ���
	auto pGoodsVariation = GetGoodsVariation();

	//�����Ʒ
	m_GoodsIDList.Add(pGoodsHoldInfo->wGoodsID);

	//�����Ϣ
	pGoodsVariation->wReason = wReason;
	pGoodsVariation->wGoodsID = wGoodsID;
	pGoodsVariation->wGoodsKind = GameGoodsItemPtr->GetGoodsKind();
	GetLocalTime(&pGoodsVariation->VariationTime);

	//���õȼ�
	pGoodsHoldInfo->wGoodsLevel = wGoodsLevel;

	//У����Ч��
	if (dwIndate > 0)
	{
		//��������
		if (pGoodsHoldInfo->dwGoodsCount == 0)
		{
			pGoodsHoldInfo->dwGoodsCount = 1;
			pGoodsVariation->lVariationCount = 1;
			pGoodsVariation->dwBeforeCount = 0;
		}

		//������Ч��
		pGoodsHoldInfo->dwGoodsIndate += dwIndate;

		//���ù���ʱ��
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
			//�����¼			
			pGoodsVariation->dwBeforeCount = 0;
			pGoodsVariation->lVariationCount = 1;

			//�����Ʒ
			pGoodsHoldInfo->dwGoodsCount = 1;
			pGoodsHoldInfo->dwGoodsIndate = 0;
		}
		else
		{
			//�����¼
			pGoodsVariation->lVariationCount = dwGoodsCount;
			pGoodsVariation->dwBeforeCount = pGoodsHoldInfo->dwGoodsCount;

			//�����Ʒ
			pGoodsHoldInfo->dwGoodsIndate = 0;
			pGoodsHoldInfo->dwGoodsCount += dwGoodsCount;
		}
	}

	//ͬ����Ʒ
	if (wReason == GOODS_REASON_SYNC)
	{
		m_StockGoodsVariation.RemoveStockItem(m_dwRecordID);
	}

	//�Զ��ύ
	if (m_bTransMode == false) SubmitTrans();

	return true;
}

//�����Ʒ
WORD CServerUserPack::AppendGoods(tagGoodsHoldInfo GoodsHoldInfo[], WORD wGoodsCount)
{
	for (WORD i = 0; i < wGoodsCount; i++)
	{
		//������Ʒ
		auto pGoodsHoldInfo = GetGoodsHoldInfo(GoodsHoldInfo[i].wGoodsID);

		//��������
		CopyMemory(pGoodsHoldInfo, &GoodsHoldInfo[i], sizeof(tagGoodsHoldInfo));
	}

	return wGoodsCount;
}

//��ȡ����
tagGoodsHoldInfo* CServerUserPack::GetHeadGoodsItem()
{
	return m_StockGoodsHoldInfo.GetHeadStockItem();
}

//��ȡ����
tagGoodsHoldInfo* CServerUserPack::GetNextGoodsItem(tagGoodsHoldInfo* pGoodsHoldInfo)
{
	return m_StockGoodsHoldInfo.GetNextStockItem(pGoodsHoldInfo);
}

//������
VOID CServerUserPack::CleanVariation()
{
	m_dwRecordID = 0;
	m_dwUpdateTime = (DWORD)time(NULL);
	m_StockGoodsVariation.RemoveAllItem();
}

//�ύ���
bool CServerUserPack::IsSubmitVariation()
{
	return true;

	////У��ʱ����
	//auto dwCurrentTime = (DWORD)time(NULL);
	//if (dwCurrentTime >= m_dwUpdateTime + m_wUpdateInterval)
	//{
	//	return true;
	//}

	////У��������
	//if (m_StockGoodsVariation.GetItemCount() >= m_wUpdateMaxCount)
	//{
	//	return true;
	//}

	//return false;
}

//��ȡ��Ʒ
bool CServerUserPack::DistillVariationGoods(TCHAR szBuffer[], WORD wBufferCount)
{
	//��������
	WORD wGoodsCount = 0;
	CString strGoodsItem;
	CString strGoodsList;
	LPCTSTR pszGoodsFormart = TEXT("{\"GoodsID\":%d,\"GoodsKind\":%d,\"GoodsLevel\":%d,\"GoodsCount\":%u,\"GoodsIndate\":%u,\"ExpireTime\":%u}");

	//�Ƴ�����
	m_StockGoodsHoldTemp.RemoveAllItem();

	//��ȡͷ��
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

		//��һ����
		pVariationItem = m_StockGoodsVariation.GetNextStockItem(pVariationItem);
	}

	//��ȡͷ��	
	auto pGoodsHoldInfo = m_StockGoodsHoldTemp.GetHeadStockItem();
	while (pGoodsHoldInfo != NULL)
	{
		//�������
		if (wGoodsCount == 0) strGoodsList.Append(TEXT("["));

		//��ʽ����Ʒ
		strGoodsItem.Format(pszGoodsFormart, pGoodsHoldInfo->wGoodsID, pGoodsHoldInfo->wGoodsKind, pGoodsHoldInfo->wGoodsLevel, pGoodsHoldInfo->dwGoodsCount, pGoodsHoldInfo->dwGoodsIndate,
			pGoodsHoldInfo->dwExpireTime);

		//׷������
		strGoodsList.Append(strGoodsItem);
		strGoodsList.Append((wGoodsCount++ < m_StockGoodsHoldTemp.GetItemCount() - 1) ? TEXT(",") : TEXT("]"));


		//��һ����
		pGoodsHoldInfo = m_StockGoodsHoldTemp.GetNextStockItem(pGoodsHoldInfo);
	}

	//У�鳤��
	if (strGoodsList.GetLength() > wBufferCount)
	{
		return false;
	}

	//��������
	StringCchCopy(szBuffer, wBufferCount, strGoodsList.GetString());

	return true;
}

//��ȡ��¼
bool CServerUserPack::DistillVariationRecord(TCHAR szBuffer[], WORD wBufferCount)
{
	//��ʽ���ַ���
	LPCTSTR pszRecordFormart = TEXT("{\"GoodsID\":%d,\"GoodsKind\":%d,\"Reason\":%d,\"BeforeCount\":%u,\"VariationCount\":%ld,\"VariationTime\":\"%s\"}");

	//��������
	WORD wRecordCount = 0;
	CString strRecordItem;
	CString strRecordList;
	CString strVariationTime;

	//��ȡͷ��
	auto pVariationItem = m_StockGoodsVariation.GetHeadStockItem();
	while (pVariationItem != NULL)
	{
		//�������
		if (wRecordCount == 0) strRecordList.Append(TEXT("["));

		//��ʽ����Ʒ
		strVariationTime.Format(TEXT("%04d-%02d-%02d %02d:%02d:%02d"), pVariationItem->VariationTime.wYear, pVariationItem->VariationTime.wMonth, pVariationItem->VariationTime.wDay,
			pVariationItem->VariationTime.wHour, pVariationItem->VariationTime.wMinute, pVariationItem->VariationTime.wSecond);
		strRecordItem.Format(pszRecordFormart, pVariationItem->wGoodsID, pVariationItem->wGoodsKind, pVariationItem->wReason, pVariationItem->dwBeforeCount, pVariationItem->lVariationCount, strVariationTime);

		//׷����Ʒ
		strRecordList.Append(strRecordItem);
		strRecordList.Append((wRecordCount++ < m_StockGoodsVariation.GetItemCount() - 1) ? TEXT(",") : TEXT("]"));

		//��һ����
		pVariationItem = m_StockGoodsVariation.GetNextStockItem(pVariationItem);
	}

	//У�鳤��
	if (strRecordList.GetLength() > wBufferCount)
	{
		return false;
	}

	//��������
	StringCchCopy(szBuffer, wBufferCount, strRecordList.GetString());

	return true;
}

//������Ʒ
tagGoodsHoldInfo* CServerUserPack::SearchGoods(WORD wGoodsID)
{
	return m_StockGoodsHoldInfo.SearchStockItem(wGoodsID);
}

//���ñ���
VOID CServerUserPack::ResetUserPack()
{
	//���±���
	m_dwRecordID = 0;
	m_dwUpdateTime = (DWORD)time(NULL);

	//�Ƴ�����
	m_StockGoodsHoldInfo.RemoveAllItem();
	m_StockGoodsHoldTemp.RemoveAllItem();
	m_StockGoodsVariation.RemoveAllItem();
}

//������Ϣ
inline tagGoodsHoldInfo* CServerUserPack::GetGoodsHoldInfo(WORD wGoodsID)
{
	//��������	
	auto pGoodsHoldInfo = m_StockGoodsHoldInfo.SearchStockItem(wGoodsID);
	if (pGoodsHoldInfo == NULL)
	{
		//��������
		pGoodsHoldInfo = m_StockGoodsHoldInfo.CreateStockItem(wGoodsID);

		//��ʼ��
		if (pGoodsHoldInfo != NULL)
		{
			ZeroMemory(pGoodsHoldInfo, sizeof(tagGoodsHoldInfo));

			//���ñ���
			pGoodsHoldInfo->wGoodsID = wGoodsID;
		}
	}

	return pGoodsHoldInfo;
}

//��ȡ���
inline tagGoodsVariation* CServerUserPack::GetGoodsVariation()
{
	//���±�ʶ
	m_dwRecordID++;

	//��������	
	auto pGoodsVariotion = m_StockGoodsVariation.SearchStockItem(m_dwRecordID);
	if (pGoodsVariotion == NULL)
	{
		//��������
		pGoodsVariotion = m_StockGoodsVariation.CreateStockItem(m_dwRecordID);

		//��ʼ��
		if (pGoodsVariotion != NULL)
		{
			ZeroMemory(pGoodsVariotion, sizeof(tagGoodsVariation));
		}
	}

	return pGoodsVariotion;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CServerUserItem::CServerUserItem() : m_ServerUserPack(this)
{
	//״̬����
	m_dwLookonTime=0;
	m_wLastTableID=INVALID_WORD;
	m_wLastChairID=INVALID_WORD;	

	//Ȩ�ޱ���
	m_dwUserRight=0L;
	m_dwMasterRight=0L;

	//λ����Ϣ
	m_dGlobalPosLng=0.f;
	m_dGlobalPosLat=0.f;
	ZeroMemory(m_szPlaceName, sizeof(m_szPlaceName));

	//״̬����
	m_lNearInfo=0L;
	m_lNearCount=0L;
	m_dwLogonTime=0L;
	m_dwPlayTimeCount=0L;
	m_dwTotalPlayTimeCount = 0L;
	m_dwStartTimeCount = 0L;
	m_dwPlayTimeVariation = 0L;

	//�ڰ�����
	m_wAddupWinRate = 0;
	m_lAddupWinScore = 0;
	m_lAddupLoseScore = 0;
	m_pControlParameter = NULL;
	ZeroMemory(&m_UserControlItem, sizeof(m_UserControlItem));

	//���ֱ���
	m_lRevenue=0L;
	m_lFreeScore=0L;
	m_lLockScore=0L;	
	m_lFreezeScore=0L;	
	m_lLockServiceScore=0L;

	//��Ϸ����
	m_bMobileUser=false;
	m_bTrusteeUser=false;
	m_bRobotUser=false;
	m_bClientReady=false;
	m_bModifyScore=false;
	ZeroMemory(m_szPassword,sizeof(m_szPassword));
	ZeroMemory(m_szTmpPassword,sizeof(m_szTmpPassword));

	//������Ϣ
	m_lMatchNo=0;
	m_dwMatchID=0;	
	m_wSignupCount=0;
	m_dwTicketCount=0;
	m_dwSignUpTime=0;
	m_cbMatchStatus=MUS_NULL;

	//�ȼ���Ϣ
	m_cbGameOrder=0;
	m_cbMemberOrder=0;

	//״̬��Ϣ
	m_cbShutUpStatus = 0;
	m_dwShutUpOverTime = 0;

	//�ֻ�����	
	m_wDeskPos=0;
	m_wDeskCount=1;
	//m_wMobileUserRule=VIEW_MODE_PART|VIEW_INFO_LEVEL_1;

	//���丨��
	m_dwTokenID=0;
	m_dwClientIP=INADDR_ANY;		
	m_dwInoutIndex=INVALID_DWORD;
	m_wBindIndex=INVALID_WORD_INDEX;

	//�ͱ���Ϣ
	ZeroMemory(&m_SubsidyInfo, sizeof(m_SubsidyInfo));

	//������Ϣ
	m_wServerType = 0;

	//����ӿ�
	m_pIServerUserItemSink=NULL;

	//���Ա���
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	ZeroMemory(&m_UserRule,sizeof(m_UserRule));
	ZeroMemory(&m_ScoreFormer,sizeof(m_ScoreFormer));
	ZeroMemory(&m_ScoreVariation,sizeof(m_ScoreVariation));
	ZeroMemory(&m_RecordInfo,sizeof(m_RecordInfo));

	return;
}

//��������
CServerUserItem::~CServerUserItem()
{
}

//�ӿڲ�ѯ
VOID * CServerUserItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServerUserItem,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IServerUserItem,Guid,dwQueryVer);
	return NULL;
}

//���µ���
VOID CServerUserItem::UpdateMemberPoint(DWORD dwIncPoint)
{
	//���µ���
	m_UserInfo.dwMemberPoint += dwIncPoint;

	//��������
	if (dwIncPoint > 0)
	{
		BYTE cbLastOrder = m_cbMemberOrder;
		m_cbMemberOrder = g_GameLevelManager->CalcUserOrder(m_UserInfo.dwMemberPoint);

		//����֪ͨ
		if (m_cbMemberOrder > cbLastOrder)
		{
			//�¼�֪ͨ
			ASSERT(m_pIServerUserItemSink != NULL);
			if (m_pIServerUserItemSink != NULL) m_pIServerUserItemSink->OnEventUserUserLevel(this, cbLastOrder, m_cbMemberOrder);
		}
	}
}

//�û�ʤ��
WORD CServerUserItem::GetUserWinRate()
{
	//����ʤ��
	LONG lPlayCount=GetUserPlayCount();
	if (lPlayCount!=0L) return (WORD)(m_UserInfo.lWinCount*10000L/lPlayCount);

	return 0;
}

//�û�����
WORD CServerUserItem::GetUserLostRate()
{
	//��������
	LONG lPlayCount=GetUserPlayCount();
	if (lPlayCount!=0L) return (WORD)(m_UserInfo.lLostCount*10000L/lPlayCount);

	return 0;
}

//�û�����
WORD CServerUserItem::GetUserDrawRate()
{
	//�������
	LONG lPlayCount=GetUserPlayCount();
	if (lPlayCount!=0L) return (WORD)(m_UserInfo.lDrawCount*10000L/lPlayCount);

	return 0;
}

//�û�����
WORD CServerUserItem::GetUserFleeRate()
{
	//��������
	LONG lPlayCount=GetUserPlayCount();
	if (lPlayCount!=0L) return (WORD)(m_UserInfo.lFleeCount*10000L/lPlayCount);

	return 0;
}

//�Ա��ʺ�
bool CServerUserItem::ContrastAccounts(LPCTSTR pszAccounts)
{
	//Ч�����
	ASSERT(pszAccounts!=NULL);
	if (pszAccounts==NULL) return false;

	//���ȶԱ�
	INT nContrastLen=lstrlen(pszAccounts);
	INT nSourceLen=lstrlen(m_UserInfo.szAccounts);
	if (nContrastLen!=nSourceLen) return false;

	//�ַ��Ա�
	TCHAR wSourceChar=0,wContrastChar=0;
	for (INT i=0;i<nSourceLen;i++)
	{
		//��ȡ�ַ�
		wContrastChar=pszAccounts[i];
		wSourceChar=m_UserInfo.szAccounts[i];

		//ת���ַ�
		if ((wSourceChar>=L'A')&&(wSourceChar<=L'Z')) wSourceChar+=32;
		if ((wContrastChar>=L'A')&&(wContrastChar<=L'Z')) wContrastChar+=32;

		//�Ա��ַ�
		if (wSourceChar!=wContrastChar) return false;
	}

	return true;
}

//�Ա�����
bool CServerUserItem::ContrastPassword(LPCTSTR pszPassword)
{
	//Ч�����
	ASSERT(pszPassword!=NULL);
	if (pszPassword==NULL) return false;

	//����Ա�
	for (INT i=0;i<(CountArray(m_szPassword)-1);i++)
	{
		if (pszPassword[i]==0) return false;
		if (pszPassword[i]!=m_szPassword[i]) return false;
	}

	return true;
}

//�Ա���ʱ����
bool CServerUserItem::ContrastTempPassword(LPCTSTR pszPassword)
{
	//Ч�����
	ASSERT(pszPassword!=NULL);
	if (pszPassword==NULL) return false;

	return lstrcmp(m_szTmpPassword,pszPassword)==0;
}

//�Ա���������
bool CServerUserItem::ContrastTablePassword(LPCTSTR pszPassword)
{
	//Ч�����
	ASSERT(pszPassword!=NULL);
	if (pszPassword==NULL) return false;

	return lstrcmp(m_UserRule.szPassword,pszPassword)==0;
}

//��������
VOID CServerUserItem::SetClientReady(bool bClientReady)
{
	m_bClientReady = bClientReady; 
}

//����״̬
BYTE CServerUserItem::GetRegulateStatus(SCORE lScore)
{
	//����״̬
	if (m_UserControlItem.cbControlStatus != CONTROL_STATUS_NORMAL)
	{
		//��Ӯ����
		SCORE lTotalWinScore = m_UserControlItem.lTotalWinScore;
		SCORE lTotalLoseScore = m_UserControlItem.lTotalLoseScore;

		//��������
		if (lScore > 0) lTotalWinScore += lScore;
		if (lScore < 0) lTotalLoseScore -= lScore;

		//������
		if (m_UserControlItem.cbControlStatus == CONTROL_STATUS_WHITELIST)
		{
			//����ʤ��
			WORD wWinRate = static_cast<WORD>((lTotalWinScore / static_cast<double>(lTotalWinScore + lTotalLoseScore + 1)) * 100);

			//У��ʤ��
			if (wWinRate > m_UserControlItem.wControlWinRate)
			{
				//����ʤ�ʲ�
				auto wDifRate = m_UserControlItem.wControlWinRate - wWinRate;

				return CONTROL_STATUS_NORMAL;
			}
		}

		//������
		if (m_UserControlItem.cbControlStatus == CONTROL_STATUS_BLACKLIST)
		{
			//����ʤ��
			WORD wWinRate = static_cast<WORD>((lTotalWinScore / static_cast<double>(lTotalWinScore + lTotalLoseScore + 1)) * 100);

			//У��ʤ��
			if (wWinRate < m_UserControlItem.wControlWinRate)
			{
				//����ʤ�ʲ�
				auto wDifRate = m_UserControlItem.wControlWinRate - wWinRate;

				return CONTROL_STATUS_NORMAL;
			}
		}
	}

	return m_UserControlItem.cbControlStatus;
}

//Ԥ�ڱ��
bool CServerUserItem::IsExpectScoreChange(SCORE lScore)
{
	//У�����
	if (lScore == 0) return false;

	//��ȡ״̬
	BYTE cbControlStatus = GetControlStatus();
	BYTE cbBeforeStatus = GetRegulateStatus(0);
	BYTE cbAfterStatus = GetRegulateStatus(lScore);

	//״̬һ��
	if (cbAfterStatus == cbControlStatus)
	{
		//�������û�
		if (cbControlStatus == CONTROL_STATUS_WHITELIST)
		{
			if (lScore > 0) return true;
		}

		//�������û�
		if (cbControlStatus == CONTROL_STATUS_BLACKLIST)
		{
			if (lScore < 0) return true;
		}

		return false;		
	}
	
	//��ͨ״̬
	if (cbAfterStatus == cbBeforeStatus)
	{
		//�������û�
		if (cbControlStatus == CONTROL_STATUS_WHITELIST)
		{
			if (lScore <0) return true;
		}

		//�������û�
		if (cbControlStatus == CONTROL_STATUS_BLACKLIST)
		{
			if (lScore >0) return true;
		}		
	}
	else
	{
		//�������û�
		if (cbControlStatus == CONTROL_STATUS_WHITELIST)
		{
			if (rand() % 100 <= 50) return true;
		}

		//�������û�
		if (cbControlStatus == CONTROL_STATUS_BLACKLIST)
		{
			if (rand() % 100 <= 50) return true;
		}
	}	

	return false;
}

//��������
VOID CServerUserItem::ModifyUserScore(SCORE lModifyScore, const tagLockScoreInfo& LockScoreInfo)
{
	//���û���
	m_lFreeScore += lModifyScore;
	m_UserInfo.lScore += lModifyScore;

	//ͬ�����
	if (m_UserInfo.cbScoreKind == SCORE_KIND_GOLD)
	{
		m_UserInfo.lGold += lModifyScore;
	}
}

//����״̬
bool CServerUserItem::SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID)
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//�Ƚ�״̬
	if( m_UserInfo.cbUserStatus != cbUserStatus )
	{
		//������Ϸʱ��
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

			//������ֵ
			//m_UserInfo.lExperience+=(m_dwPlayTimeCount+59L)/60L;
			//m_ScoreVariation.lExperience+=(m_dwPlayTimeCount+59L)/60L;
			//m_RecordInfo.VariationScore.lExperience+=(m_dwPlayTimeCount+59L)/60L;

			m_dwStartTimeCount = 0;
			m_dwPlayTimeVariation = 0;
		}
	}

	//����״̬
	//m_wLastTableID=(m_UserInfo.wTableID==INVALID_WORD)?wTableID:m_UserInfo.wTableID;
	//m_wLastChairID=(m_UserInfo.wChairID==INVALID_WORD)?wChairID:m_UserInfo.wChairID;

	m_wLastTableID=m_UserInfo.wTableID;
	m_wLastChairID=m_UserInfo.wChairID;

	//���ñ���
	m_UserInfo.wTableID=wTableID;
	m_UserInfo.wChairID=wChairID;
	m_UserInfo.cbUserStatus=cbUserStatus;	

	//״̬����
	if ((cbUserStatus == US_OFFLINE) || (cbUserStatus == US_NULL))
	{
		m_dwTokenID = 0;	
		m_wBindIndex = INVALID_WORD_INDEX;
	}

	//����״̬
	ASSERT(m_pIServerUserItemSink!=NULL);
	m_pIServerUserItemSink->OnEventUserItemStatus(this,m_wLastTableID,m_wLastChairID);

	return true;
}

//д��Ƹ�
bool CServerUserItem::WriteUserWealth(WORD wMoneyID, SCORE lScore, WORD wReason)
{
	//Ч��״̬
	ASSERT((m_UserInfo.dwUserID != 0L));
	if (m_UserInfo.dwUserID == 0L) return false;

	//����ṹ
	tagWealthVariation WealthVariation;
	ZeroMemory(&WealthVariation, sizeof(WealthVariation));

	//���ñ���
	WealthVariation.wReason = wReason;
	WealthVariation.wMoneyID = wMoneyID;
	WealthVariation.lVariationScore = lScore;
	WealthVariation.cbSyncTarget = SYNC_TARGET_CLIENT | SYNC_TARGET_DATABASE;

	//���ʱ��
	GetLocalTime(&WealthVariation.VariationTime);

	//���ұ�ʶ
	switch (wMoneyID)
	{	
	case GOODS_ID_CARD:			//����
	{
		WealthVariation.lBefore = m_UserInfo.lUserCard;
		m_UserInfo.lUserCard += lScore;
		WealthVariation.lBalance = m_UserInfo.lUserCard;

		break;
	}
	case GOODS_ID_GOLD:			//�ƽ�
	{
		WealthVariation.lBefore = m_UserInfo.lGold;
		m_UserInfo.lGold += lScore;
		WealthVariation.lBalance = m_UserInfo.lGold;

		//�������
		if (m_UserInfo.cbScoreKind==SCORE_KIND_GOLD)
		{
			//���·���
			m_lFreeScore += lScore;
			m_lLockScore = m_lFreeScore;
			m_UserInfo.lScore += lScore;			
			m_RecordInfo.VariationScore.lScore += lScore;	

			//ͬ������
			WealthVariation.cbSyncTarget |= SYNC_TARGET_GAMESERVER;			
		}

		break;
	}	
	}

	//ͬ���Ƹ�
	if (wReason == GOODS_REASON_SYNC)
	{
		WealthVariation.cbSyncTarget &= ~SYNC_TARGET_DATABASE;
	}
	
	//�Ƹ����
	ASSERT(m_pIServerUserItemSink != NULL);
	if (m_pIServerUserItemSink != NULL) m_pIServerUserItemSink->OnEventUserItemWealth(this, WealthVariation);

	return true;
}

//д�����
bool CServerUserItem::WriteUserScore(SCORE lGameScore, SCORE lRevenue, SCORE lBetAmount, LONG lExperience, enScoreType ScoreType)
{
	/*CString s;
	s.Format(_T("д�����,lGameScore:%I64d, lRevenue:%I64d, lBetAmount::%I64d, lExperience:%d, ScoreType:%d"), lGameScore, lRevenue, lBetAmount, lExperience, ScoreType);
	CTraceService::TraceString(s, TraceLevel_Exception);*/
	//Ч��״̬
	ASSERT ((m_UserInfo.dwUserID!=0L)&&(ScoreType!=ScoreType_Null));
	//if ((m_UserInfo.dwUserID==0L)||(ScoreType==ScoreType_Null)) return false;//kk guan
	if (m_UserInfo.dwUserID == 0L)  return false;//kk jia
	

	//�޸�ʤ��
	switch (ScoreType)
	{
	case ScoreType_Win:		//ʤ��
		{
			//ͳ�ƾ���
			m_UserInfo.lWinCount++;
			m_ScoreVariation.lWinCount++;
			m_RecordInfo.VariationScore.lWinCount++;

			//��ʤ����
			if (++m_UserInfo.lWinStreak > m_UserInfo.lMaxWinStreak)
			{
				m_UserInfo.lMaxWinStreak = m_UserInfo.lWinStreak;
			}

			//ͬ������
			m_ScoreVariation.lWinStreak = m_UserInfo.lWinStreak;
			m_ScoreVariation.lMaxWinStreak = m_UserInfo.lMaxWinStreak;

			break;
		}
	case ScoreType_Lose:	//���
		{
			//ͳ�ƾ���
			m_UserInfo.lLostCount++;
			m_ScoreVariation.lLostCount++;
			m_RecordInfo.VariationScore.lLostCount++;

			//��ʤ����
			m_UserInfo.lWinStreak = 0;
			m_ScoreVariation.lWinStreak = m_UserInfo.lWinStreak;			

			break;
		}
	case ScoreType_Draw:	//�;�
		{
			//ͳ�ƾ���
			m_UserInfo.lDrawCount++;
			m_ScoreVariation.lDrawCount++;
			m_RecordInfo.VariationScore.lDrawCount++;
			break;
		}
	case ScoreType_Flee:	//�Ӿ�
		{
			//ͳ�ƾ���
			m_UserInfo.lFleeCount++;
			m_ScoreVariation.lFleeCount++;
			m_RecordInfo.VariationScore.lFleeCount++;

			//��ʤ����
			m_UserInfo.lWinStreak = 0;
			m_ScoreVariation.lWinStreak = m_UserInfo.lWinStreak;

			break;
		}
	}

	//ͳ����ע
	m_ScoreVariation.lBetAmount+=lBetAmount;
	m_RecordInfo.VariationScore.lBetAmount+=lBetAmount;

	//���û���
	m_bModifyScore=true;
	m_lFreeScore+=lGameScore;
	m_UserInfo.lScore+=lGameScore;
	m_ScoreVariation.lScore+=lGameScore;
	m_RecordInfo.VariationScore.lScore+=lGameScore;

	//���¾���
	m_UserInfo.lExperience += lExperience;
	m_ScoreVariation.lExperience += lExperience;
	m_RecordInfo.VariationScore.lExperience += lExperience;

	//ͬ�����
	if (m_UserInfo.cbScoreKind==SCORE_KIND_GOLD)
	{
		m_UserInfo.lGold += lGameScore;
	}

	//��λ����
	if (m_wServerType&GAME_GENRE_RANKING && m_UserInfo.lScore<0)
	{
		//������¼
		m_ScoreVariation.lScore -= m_UserInfo.lScore;
		m_RecordInfo.VariationScore.lScore -= m_UserInfo.lScore;

		//��������
		m_UserInfo.lScore = 0;
	}

	//���ӱ���
	m_lRevenue+=lRevenue;
	m_RecordInfo.lRevenue+=lRevenue;

	//����״̬
	UpdateControlStatus(lGameScore, lBetAmount);

	//����״̬
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemScore(this);	

	//��Ϸ�ȼ�
	if (lExperience > 0)
	{
		//��ȡ�ȼ�
		auto cbLastOrder = m_cbGameOrder;
		m_cbGameOrder = g_GameLevelManager->CalcGameOrder(m_UserInfo.lExperience);

		//����֪ͨ
		if (m_cbGameOrder > cbLastOrder)
		{
			ASSERT(m_pIServerUserItemSink != NULL);
			if (m_pIServerUserItemSink != NULL) m_pIServerUserItemSink->OnEventUserGameLevel(this, cbLastOrder, m_cbGameOrder);
		}
	}

	return true;
}

//������ʱ����
VOID CServerUserItem::SetTmpPassword( LPCTSTR lpszPassword )
{
	//Ч�����
	ASSERT(lpszPassword!=NULL);
	if (lpszPassword==NULL) return;

	StringCchCopy( m_szTmpPassword,CountArray(m_szTmpPassword), lpszPassword);

	return;
}

//�޸�Ȩ��
VOID CServerUserItem::ModifyUserRight( DWORD dwAddRight, DWORD dwRemoveRight )
{
	//���Ȩ��
	m_dwUserRight |= dwAddRight;

	//ɾ��Ȩ��
	m_dwUserRight &= ~dwRemoveRight;

	//����״̬
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemRight(this,dwAddRight,dwRemoveRight);

	return;
}

//���ò���
bool CServerUserItem::SetUserParameter(DWORD dwClientIP, WORD wBindIndex, DWORD dwTokenID, bool bRobotUser, bool bClientReady)
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//���ñ���
	m_dwTokenID=dwTokenID;
	m_wBindIndex=wBindIndex;
	m_dwClientIP=dwClientIP;
	m_bRobotUser=bRobotUser;
	m_bClientReady=bClientReady;

	return true;
}

//��ȡ���
bool CServerUserItem::DistillVariation(tagVariationScore & VariationScore, const tagLockScoreInfo & LockScoreInfo)
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//�޸��ж�
	if (m_bModifyScore==true)
	{
		//���ñ���
		VariationScore.lUnLockScore=0L;
		VariationScore.lRevenue=m_lRevenue;
		VariationScore.VariationScore=m_ScoreVariation;

		//��Ϸʱ��
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
			//��������
			if (m_ScoreVariation.lScore>0L)
			{
				//���ñ���
				m_lFreeScore=m_lLockScore;
				VariationScore.lUnLockScore=0L;

				//��������
				if (m_lLockScore<LockScoreInfo.lMaxLockScore+m_lFreezeScore)
				{
					//�������
					SCORE lWishAugment=LockScoreInfo.lMaxLockScore+m_lFreezeScore-m_lLockScore;
					SCORE lAllowAugment=__min(lWishAugment,m_ScoreVariation.lScore);

					//���ñ���
					m_lLockScore+=lAllowAugment;
					m_lFreeScore+=lAllowAugment;
					VariationScore.lUnLockScore-=lAllowAugment;
				}
			}
			else
			{
				//��������
				m_lLockScore=m_lFreeScore;
				VariationScore.lUnLockScore-=m_ScoreVariation.lScore;
			}
		}
	}
	else
	{
		//���ñ���
		ZeroMemory(&VariationScore,sizeof(VariationScore));

		//��ʤ��Ϣ
		VariationScore.VariationScore.lWinStreak = m_UserInfo.lWinStreak;
		VariationScore.VariationScore.lMaxWinStreak = m_UserInfo.lMaxWinStreak;
	}

	if (LockScoreInfo.lRestrictScore==0)
	{
		//�ͷ�����
		if (m_lLockScore>LockScoreInfo.lMaxLockScore+m_lFreezeScore)
		{
			SCORE lLastLockScore=m_lLockScore;
			SCORE lMaxLockScore = LockScoreInfo.lMaxLockScore+m_lFreezeScore;
			m_lFreeScore=lMaxLockScore;
			m_lLockScore=lMaxLockScore;
			VariationScore.lUnLockScore+=lLastLockScore-lMaxLockScore;
		}
	}

	//��ԭ����
	m_lRevenue=0L;
	m_bModifyScore=false;
	ZeroMemory(&m_ScoreVariation,sizeof(m_ScoreVariation));

	return true;
}

//���ýӿ�
VOID CServerUserItem::SetServerUserItemSink(IServerUserItemSink* pIServerUserItemSink)
{
	m_pIServerUserItemSink = pIServerUserItemSink;
	m_ServerUserPack.m_pIServerUserItemSink = pIServerUserItemSink;	
}

//��������
VOID CServerUserItem::ResetUserItem()
{
	//״̬����
	m_dwLookonTime=0;
	m_wLastTableID=INVALID_WORD;
	m_wLastChairID=INVALID_WORD;

	//Ȩ�ޱ���
	m_dwUserRight=0L;
	m_dwMasterRight=0L;

	//λ����Ϣ
	m_dGlobalPosLng=0.f;
	m_dGlobalPosLat=0.f;
	ZeroMemory(m_szPlaceName,sizeof(m_szPlaceName));

	//״̬����
	m_lNearInfo=0L;
	m_lNearCount=0L;
	m_dwLogonTime=0L;
	m_dwPlayTimeCount=0L;
	m_dwTotalPlayTimeCount = 0L;
	m_dwStartTimeCount = 0L;
	m_dwPlayTimeVariation = 0L;

	//���ֱ���
	m_lRevenue=0L;
	m_lFreeScore=0L;
	m_lLockScore=0L;	
	m_lFreezeScore = 0L;
	m_lLockServiceScore = 0L;

	//��Ϸ����
	m_bTrusteeUser=false;
	m_bRobotUser=false;
	m_bClientReady=false;
	m_bModifyScore=false;
	ZeroMemory(m_szPassword,sizeof(m_szPassword));
	ZeroMemory(m_szTmpPassword,sizeof(m_szTmpPassword));
	m_dwInoutIndex=INVALID_DWORD;

	//�ڰ�����
	m_wAddupWinRate = 0;
	m_lAddupWinScore = 0;
	m_lAddupLoseScore = 0;
	m_pControlParameter = NULL;
	ZeroMemory(&m_UserControlItem, sizeof(m_UserControlItem));

	//������Ϣ
	m_lMatchNo=0;
	m_dwMatchID=0;	
	m_wSignupCount=0;
	m_dwSignUpTime=0;
	m_dwTicketCount=0;
	m_cbMatchStatus=MUS_NULL;

	//�ȼ���Ϣ
	m_cbGameOrder = 0;
	m_cbMemberOrder = 0;

	//״̬��Ϣ
	m_cbShutUpStatus = 0;
	m_dwShutUpOverTime = 0;

	//�ͱ���Ϣ
	ZeroMemory(&m_SubsidyInfo, sizeof(m_SubsidyInfo));

	//������Ϣ
	m_wServerType = 0;

	//���丨��
	m_dwTokenID=0;
	m_dwClientIP=INADDR_ANY;
	m_wBindIndex=INVALID_WORD_INDEX;

	//�ֻ�����	
	m_wDeskPos=0;
	m_wDeskCount=1;
	//m_wMobileUserRule=VIEW_MODE_PART|VIEW_INFO_LEVEL_1;	

	//���Ա���
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	ZeroMemory(&m_UserRule,sizeof(m_UserRule));
	ZeroMemory(&m_ScoreFormer,sizeof(m_ScoreFormer));
	ZeroMemory(&m_ScoreVariation,sizeof(m_ScoreVariation));
	ZeroMemory(&m_RecordInfo,sizeof(m_RecordInfo));

	return;
}

//����״̬
VOID CServerUserItem::UpdateControlStatus(SCORE lScore, SCORE lBetAmount)
{
	//У�����
	//if (lScore == 0) return;
	
	//����Ӯ��
	auto lWinScore = static_cast<SCORE>((m_lAddupWinScore / static_cast<double>(m_lAddupWinScore + m_lAddupLoseScore + 1)) * m_pControlParameter->lBenchmarkValue);

	//����ʤ��
	if (lScore < 0)
	{
		m_lAddupLoseScore -= lScore;
		m_wAddupWinRate = static_cast<WORD>((lWinScore / static_cast<double>(m_pControlParameter->lBenchmarkValue - lScore)) * 100);
	}

	//����ʤ��
	if (lScore > 0)
	{
		m_lAddupWinScore += lScore;
		m_wAddupWinRate = static_cast<WORD>(((lWinScore + lScore) / static_cast<double>(m_pControlParameter->lBenchmarkValue)) * 100);
	}

	//У�鿪��
	if (m_pControlParameter == NULL || !m_pControlParameter->bControlEnabled) return;
	if (m_UserControlItem.cbControlStatus == CONTROL_STATUS_NORMAL && (m_pControlParameter->wSupportControlMode & CONTROL_MODE_AUTO) == 0) return;

	//��¼״̬
	BYTE cbLastControlStatus = m_UserControlItem.cbControlStatus;

	//��ͨ״̬
	if (m_UserControlItem.cbControlStatus == CONTROL_STATUS_NORMAL)
	{
		//У�����
		if (m_pControlParameter->wStartPlayCount < GetUserPlayCount())
		{
			//������
			if (m_wAddupWinRate <= m_pControlParameter->wInWhiteListRate)
			{
				m_UserControlItem.cbControlMode = CONTROL_MODE_AUTO;
				m_UserControlItem.cbControlKind = CONTROL_KIND_RATE;
				m_UserControlItem.cbControlStatus = CONTROL_STATUS_WHITELIST;
				m_UserControlItem.wControlWinRate = m_pControlParameter->wWLControlRate;
				m_UserControlItem.wTargetWinRate = m_pControlParameter->wOutWhiteListRate;
			}

			//������
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

		//ͳ�ƴ�����
		m_UserControlItem.lTotalBetAmount += lBetAmount;

		//ʤ�ʿ���
		if (m_UserControlItem.cbControlKind == CONTROL_KIND_RATE)
		{
			//������
			if (m_UserControlItem.cbControlStatus == CONTROL_STATUS_WHITELIST)
			{
				//У��ʤ��
				if (m_wAddupWinRate >= m_UserControlItem.wTargetWinRate)
				{
					m_UserControlItem.cbControlStatus = CONTROL_STATUS_NORMAL;
				}
			}

			//������
			if (m_UserControlItem.cbControlStatus == CONTROL_STATUS_BLACKLIST)
			{
				//У��ʤ��
				if (m_wAddupWinRate <= m_UserControlItem.wTargetWinRate)
				{
					m_UserControlItem.cbControlStatus = CONTROL_STATUS_NORMAL;
				}
			}
		}

		//ʱ�����
		if (m_UserControlItem.cbControlKind == CONTROL_KIND_TIME)
		{
			//��ǰʱ��
			DWORD dwCurrentTime = time(NULL);

			//У��ʱ��
			if (m_UserControlItem.dwControlStartTime + m_UserControlItem.dwControlDuration <= dwCurrentTime)
			{
				m_UserControlItem.cbControlStatus = CONTROL_STATUS_NORMAL;				
			}
		}

		//��������
		if (m_UserControlItem.cbControlKind == CONTROL_KIND_COUNT)
		{
			//У�����
			if (m_UserControlItem.wControlPlayCount <= m_UserControlItem.lTotalWinCount+ m_UserControlItem.lTotalLoseCount)
			{
				m_UserControlItem.cbControlStatus = CONTROL_STATUS_NORMAL;
			}
		}
	}

	//У��״̬
	if (m_UserControlItem.cbControlStatus != cbLastControlStatus)
	{
		if (m_UserControlItem.cbControlStatus == CONTROL_STATUS_NORMAL)
		{
			//����״̬
			ASSERT(m_pIServerUserItemSink != NULL);
			if (m_pIServerUserItemSink != NULL) m_pIServerUserItemSink->OnEventUserControlFinish(this);

			//��������
			ZeroMemory(&m_UserControlItem, sizeof(m_UserControlItem));
		}
		else
		{
			//����״̬
			ASSERT(m_pIServerUserItemSink != NULL);
			if (m_pIServerUserItemSink != NULL) m_pIServerUserItemSink->OnEventUserControlStart(this);
		}
	}
	else if (m_UserControlItem.cbControlStatus != CONTROL_STATUS_NORMAL && m_UserControlItem.cbControlKind == CONTROL_KIND_CUSTOM)
	{
		//����״̬
		ASSERT(m_pIServerUserItemSink != NULL);
		if (m_pIServerUserItemSink != NULL) m_pIServerUserItemSink->OnEventUserControlUpdate(this);
	}
}

//�������
bool CServerUserItem::FreezeUserScore( SCORE lScore, bool bNotify )
{
	ASSERT (m_lFreezeScore+lScore>=0);
	m_lFreezeScore += lScore;

	if (bNotify && m_pIServerUserItemSink)
		m_pIServerUserItemSink->OnEventUserFreezeScore(this,m_lFreezeScore);

	return true;
}

//�����
bool CServerUserItem::DetachBindStatus()
{
	//Ч��״̬
	ASSERT (m_UserInfo.dwUserID != 0L);
	if (m_UserInfo.dwUserID == 0L) return false;

	//�û�����
	m_bClientReady = false;

	//��������
	m_dwTokenID = 0;
	m_wBindIndex = INVALID_WORD_INDEX;

	return true;
}

//��ѯ��¼
bool CServerUserItem::QueryRecordInfo(tagVariationScore & RecordInfo)
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID != 0L);
	if (m_UserInfo.dwUserID == 0L) return false;

	//���ñ���
	RecordInfo = m_RecordInfo;

	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CServerUserManager::CServerUserManager()
{
	//����ӿ�
	m_pControlParameter=NULL;
	m_pGameServiceOption=NULL;
	m_pIServerUserItemSink=NULL;

	//��������
	m_UserIDMap.InitHashTable(PRIME_SERVER_USER);

	return;
}

//��������
CServerUserManager::~CServerUserManager()
{
	//�ͷ��û�
	for (INT_PTR i=0;i<m_UserItemStore.GetCount();i++) m_UserItemStore[i]->Release();
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++) m_UserItemArray[i]->Release();

	//ɾ������
	m_UserIDMap.RemoveAll();
	m_UserItemStore.RemoveAll();
	m_UserItemArray.RemoveAll();

	return;
}

//�ӿڲ�ѯ
VOID * CServerUserManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServerUserManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IServerUserManager,Guid,dwQueryVer);
	return NULL;
}

//���Ʋ���
VOID CServerUserManager::SetControlParamter(tagControlParameter* pControlParameter)
{
	m_pControlParameter = pControlParameter;
}

//�������
VOID CServerUserManager::SetServiceParameter(tagGameServiceOption* pGameServiceOption)
{
	//���ñ���
	m_pGameServiceOption = pGameServiceOption;
}

//���ýӿ�
bool CServerUserManager::SetServerUserItemSink(IUnknownEx * pIUnknownEx)
{
	//���ýӿ�
	if (pIUnknownEx!=NULL)
	{
		//��ѯ�ӿ�
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IServerUserItemSink)!=NULL);
		m_pIServerUserItemSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IServerUserItemSink);

		//�ɹ��ж�
		if (m_pIServerUserItemSink==NULL) return false;
	}
	else m_pIServerUserItemSink=NULL;

	return true;
}

//ö���û�
IServerUserItem * CServerUserManager::EnumUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserItemArray.GetCount()) return NULL;
	return m_UserItemArray[wEnumIndex];
}

//�����û�
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

//�����û�
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

//�����û�
IServerUserItem * CServerUserManager::ActiveUserItem(tagServerUserInfo & UserInfo, tagUserInfoPlus & UserInfoPlus, LPCTSTR pszPassword)
{
	//��������
	CServerUserItem * pServerUserItem=NULL;

	//��ȡָ��
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

	//���ýӿ�
	pServerUserItem->SetServerUserItemSink(m_pIServerUserItemSink);

	//���Ա���
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

	//Ȩ�ޱ���
	pServerUserItem->m_dwUserRight=UserInfoPlus.dwUserRight;
	pServerUserItem->m_dwMasterRight=UserInfoPlus.dwMasterRight;

	//���ֱ���
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

	//״̬����
	pServerUserItem->m_dwPlayTimeCount=0L;
	pServerUserItem->m_dwTotalPlayTimeCount = 0L;
	pServerUserItem->m_dwLogonTime=static_cast<DWORD>(time(NULL));

	//��ʶ����
	pServerUserItem->m_bClientReady=false;
	pServerUserItem->m_bModifyScore=false;
	pServerUserItem->m_bTrusteeUser=false;
	pServerUserItem->m_bRobotUser=UserInfoPlus.bRobotUser;
	pServerUserItem->m_bMobileUser=UserInfoPlus.bMobileUser;	

	//����ȼ�
	pServerUserItem->m_cbGameOrder = g_GameLevelManager->CalcGameOrder(pServerUserItem->GetExperience());
	pServerUserItem->m_cbMemberOrder = g_GameLevelManager->CalcUserOrder(pServerUserItem->GetMemberPoint());

	//������Ϣ
	pServerUserItem->m_wServerType = m_pGameServiceOption->wServerType;

	//λ�ñ���
	pServerUserItem->m_dGlobalPosLat=UserInfoPlus.dGlobalPosLat;
	pServerUserItem->m_dGlobalPosLng=UserInfoPlus.dGlobalPosLng;
	CopyMemory(pServerUserItem->m_szPlaceName, UserInfoPlus.szPlaceName,sizeof(pServerUserItem->m_szPlaceName));

	//״̬����
	pServerUserItem->m_cbShutUpStatus = UserInfoPlus.cbShutUpStatus;
	pServerUserItem->m_dwShutUpOverTime = UserInfoPlus.dwShutUpOverTime;

	//�ڰ�����
	pServerUserItem->m_pControlParameter = m_pControlParameter;
	pServerUserItem->m_wAddupWinRate = UserInfoPlus.wAddupWinRate;
	pServerUserItem->m_lAddupWinScore = UserInfoPlus.lAddupWinScore;
	pServerUserItem->m_lAddupLoseScore = UserInfoPlus.lAddupLoseScore;	
	CopyMemory(&pServerUserItem->m_UserControlItem, &UserInfoPlus.UserControlItem, sizeof(tagUserControlItem));

	//��������
	pServerUserItem->m_dwTokenID=UserInfoPlus.dwTokenID;
	pServerUserItem->m_wBindIndex=UserInfoPlus.wBindIndex;	
	pServerUserItem->m_dwClientIP=UserInfoPlus.dwClientIP;
	pServerUserItem->m_bRobotUser=UserInfoPlus.bRobotUser;
	StringCchCopy(pServerUserItem->m_szMachineID, LEN_MACHINE_ID, UserInfoPlus.szMachineID);
	StringCchCopy(pServerUserItem->m_szPassword,CountArray(pServerUserItem->m_szPassword), pszPassword);	
	pServerUserItem->m_dwInoutIndex=UserInfoPlus.dwInoutIndex;

	//�����û�
	m_UserItemArray.Add(pServerUserItem);
	m_UserIDMap[UserInfo.dwUserID]=pServerUserItem;

	return pServerUserItem;
}

//ɾ���û�
bool CServerUserManager::DeleteUserItem(IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserStatus()==US_NULL));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetUserStatus()!=US_NULL)) return false;

	//��������
	CServerUserItem * pTempUserItem=NULL;
	DWORD dwUserID=pIServerUserItem->GetUserID();

	//Ѱ�Ҷ���
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		//��ȡ�û�
		pTempUserItem=m_UserItemArray[i];
		if (pIServerUserItem!=pTempUserItem) continue;

		//���ö���
		pTempUserItem->ResetUserItem();

		//ɾ������
		m_UserItemArray.RemoveAt(i);
		m_UserIDMap.RemoveKey(dwUserID);
		m_UserItemStore.Add(pTempUserItem);

		return true;
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//ɾ���û�
bool CServerUserManager::DeleteUserItem()
{
	//�洢����
	m_UserItemStore.Append(m_UserItemArray);

	//ɾ������
	m_UserIDMap.RemoveAll();
	m_UserItemArray.RemoveAll();


	return true;
}
//////////////////////////////////////////////////////////////////////////
