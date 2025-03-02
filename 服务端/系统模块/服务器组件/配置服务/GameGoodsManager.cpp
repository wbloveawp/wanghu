#include "StdAfx.h"
#include "GameGoodsManager.h"

using namespace WHJson;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CGameGoodsItem::CGameGoodsItem()
{
	m_wGoodsID = 0;
	m_wGoodsKind = 0;
	m_wCanTradeType = 0;
	m_wCanStorePlace = 0;
	m_szGoodsName[0] = 0;	
	m_LastGoodsItemPtr = NULL;
	m_NextGoodsItemPtr = NULL;	
}

//��������
CGameGoodsItem::~CGameGoodsItem()
{
}

//������Ʒ
bool CGameGoodsItem::ConstructGoods(tagGoodsInfo* pGoodsInfo)
{
	//У�����
	ASSERT (pGoodsInfo!=NULL);
	if (pGoodsInfo == NULL) return false;

	//���ñ���
	m_wGoodsID = pGoodsInfo->wGoodsID;
	m_wGoodsKind = pGoodsInfo->wGoodsKind;
	m_bHasIndate = pGoodsInfo->bHasIndate;
	m_wCanTradeType = pGoodsInfo->wCanTradeType;
	m_wCanStorePlace = pGoodsInfo->wCanStorePlace;
	StringCchCopy(m_szGoodsName, CountArray(m_szGoodsName), pGoodsInfo->szGoodsName);

	//���ռ۸�
	if ((m_wCanTradeType&TRADE_TYPE_RECYCLE)!=0)
	{
		return false;
	}	

	return ParseGoodsOption(pGoodsInfo->szGoodsOption);
}

//��������
bool CGameGoodsItem::ParseGoodsOption(LPCTSTR pszGoodsOption)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameGoodsManager::CGameGoodsManager()
{
	m_MapGoodsKind.InitHashTable(17);
	m_MapGoodsItem.InitHashTable(101);
}

//��������
CGameGoodsManager::~CGameGoodsManager()
{
	ResetData();
}

//��������
VOID CGameGoodsManager::ResetData()
{
	m_MapGoodsKind.RemoveAll();
	m_MapGoodsItem.RemoveAll();	
}

//�Ƴ���Ʒ
VOID CGameGoodsManager::RemoveGameGoods(WORD wGoodsID)
{
	//��ѯ����
	CGameGoodsItemPtr GameGoodsItemPtr;
	if (m_MapGoodsItem.Lookup(wGoodsID, GameGoodsItemPtr))
	{
		if (GameGoodsItemPtr->m_LastGoodsItemPtr)
		{
			GameGoodsItemPtr->m_LastGoodsItemPtr->m_NextGoodsItemPtr = GameGoodsItemPtr->m_NextGoodsItemPtr;
		}
		else
		{
			if (GameGoodsItemPtr->m_NextGoodsItemPtr)
			{
				GameGoodsItemPtr->m_NextGoodsItemPtr->m_LastGoodsItemPtr = NULL;
				m_MapGoodsKind[GameGoodsItemPtr->m_wGoodsKind] = GameGoodsItemPtr->m_NextGoodsItemPtr;
			}
			else
			{
				m_MapGoodsKind.RemoveKey(GameGoodsItemPtr->m_wGoodsKind);
			}
		}
	}	
}

//�����Ʒ
WORD CGameGoodsManager::AppendGameGoods(tagGoodsInfo GoodsInfo[], WORD wGoodsCount)
{
	for (WORD i = 0; i < wGoodsCount; i++)
	{
		tagGoodsInfo* pGoodsInfo = &GoodsInfo[i];

		try
		{			
			auto GameGoodsItemPtr = CreateGameGoods(pGoodsInfo->wGoodsKind);
			if (GameGoodsItemPtr)
			{
				//������Ʒ
				GameGoodsItemPtr->ConstructGoods(pGoodsInfo);

				//�����ֵ�
				m_MapGoodsItem[pGoodsInfo->wGoodsID] = GameGoodsItemPtr;

				//��ѯ����
				CGameGoodsItemPtr GameGoodsKindPtr;
				m_MapGoodsKind.Lookup(pGoodsInfo->wGoodsKind, GameGoodsKindPtr);

				//������Ʒ
				if (GameGoodsKindPtr)
				{
					GameGoodsItemPtr->m_LastGoodsItemPtr = NULL;
					GameGoodsItemPtr->m_NextGoodsItemPtr = GameGoodsKindPtr.get();
					GameGoodsKindPtr->m_LastGoodsItemPtr = GameGoodsItemPtr.get();
				}

				m_MapGoodsKind[pGoodsInfo->wGoodsKind] = GameGoodsItemPtr;
			}
		}
		catch (LPCTSTR pszException)
		{
			CTraceService::TraceString(TEXT("��Ʒ[%s]����ʧ��:%s"), TraceLevel_Exception, pGoodsInfo->szGoodsName, pszException);

			return i;
		}
		catch (...)
		{
			CTraceService::TraceString(TEXT("��Ʒ[%s]����ʧ��:δ֪�쳣"), TraceLevel_Exception, pGoodsInfo->szGoodsName);

			return i;
		}
	}

	return wGoodsCount;
}

//������Ʒ
bool CGameGoodsManager::IsMoneyGoods(WORD wGoodsID)
{
	if (wGoodsID== GOODS_ID_GOLD || 
		wGoodsID== GOODS_ID_CARD)		
	{
		return true;
	}

	return false;
}

//���л���Ʒ
bool CGameGoodsManager::SerializeToJson(tagGoodsItem& GoodsItem, TCHAR szBuffer[], WORD wBufferCount)
{
	//��ʽ���ַ���
	CString strGoodsItem;
	LPCTSTR pszTaskItem = TEXT("{\"GoodsID\":%d,\"Level\":%d,\"Count\":%u,\"Indate\":%u}");

	//��ʽ����Ʒ
	strGoodsItem.Format(pszTaskItem, GoodsItem.wGoodsID, GoodsItem.wGoodsLevel,GoodsItem.dwGoodsCount, GoodsItem.dwGoodsIndate);

	//У�鳤��
	if (strGoodsItem.GetLength() > wBufferCount)
	{
		return false;
	}

	//��������
	StringCchCopy(szBuffer, wBufferCount, strGoodsItem.GetString());

	return true;
}

//���л���Ʒ
bool CGameGoodsManager::SerializeToJson(tagGoodsItem GoodsItem[], WORD wItemCount, TCHAR szBuffer[], WORD wBufferCount)
{
	//��ʽ���ַ���
	LPCTSTR pszTaskItem = TEXT("{\"GoodsID\":%d,\"Level\":%d,\"Count\":%u,\"Indate\":%u}");

	//��������
	CString strGoodsItem;
	CString strGoodsList;

	//���������б�
	for (auto i = 0; i < wItemCount; i++)
	{
		//�������
		if (i == 0) strGoodsList.Append(TEXT("["));

		//��ʽ����Ʒ
		strGoodsItem.Format(pszTaskItem, GoodsItem[i].wGoodsID, GoodsItem[i].wGoodsLevel, GoodsItem[i].dwGoodsCount, GoodsItem[i].dwGoodsIndate);

		//�������
		strGoodsList.Append(strGoodsItem);
		strGoodsList.Append((i<wItemCount-1)?TEXT(","):TEXT("]"));
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

//������Ʒ
bool CGameGoodsManager::ParseFromJson(LPCTSTR pszGoodsItem, tagGoodsItem& GoodsItem)
{
	//����У��
	if (pszGoodsItem == NULL || lstrlen(pszGoodsItem) == 0)
	{
		return false;
	}

	try
	{
		//��ȡ����
		auto JsonValuePtr = CWHJson::getValue(string(CT2CA(pszGoodsItem)));
		if (JsonValuePtr->getType() != eJsonTypeObj)
		{
			return false;
		}

		//ת������
		auto JsonValueObjPtr = CJsonValueObjPtr::dynamicCast(JsonValuePtr);

		//Ĭ������
		GoodsItem.wGoodsLevel = 1;

		//��ȡ����
		CWHJsonInput::readJson(GoodsItem.wGoodsID, JsonValueObjPtr->value["GoodsID"], true);		
		CWHJsonInput::readJson(GoodsItem.dwGoodsCount, JsonValueObjPtr->value["Count"], true);
		CWHJsonInput::readJson(GoodsItem.wGoodsLevel, JsonValueObjPtr->value["Level"], false);
		CWHJsonInput::readJson(GoodsItem.dwGoodsIndate, JsonValueObjPtr->value["Indate"], false);		

		return true;
	}
	catch (...)
	{
		ASSERT(false);
	}

	return false;
}

//������Ʒ
bool CGameGoodsManager::ParseFromJson(LPCTSTR pszGoodsList, CGoodsItemArray& MoneyArray, CGoodsItemArray& GoodsArray)
{
	//����У��
	if (pszGoodsList == NULL || lstrlen(pszGoodsList) == 0)
	{
		return false;
	}

	try
	{
		//��ȡ����
		auto JsonValuePtr = CWHJson::getValue(string(CT2CA(pszGoodsList)));
		if (JsonValuePtr->getType() != eJsonTypeArray)
		{
			return false;
		}

		//ת������
		auto JsonValueArrayPtr = CJsonValueArrayPtr::dynamicCast(JsonValuePtr);

		//��������
		tagGoodsItem GoodsItem;
		ZeroMemory(&GoodsItem,sizeof(GoodsItem));

		//ת������	
		for (auto ItemPtr : JsonValueArrayPtr->value)
		{
			auto RewardItemPtr = CJsonValueObjPtr::dynamicCast(ItemPtr);
			if (RewardItemPtr)
			{
				//���ñ���
				GoodsItem.wGoodsLevel = 1;
				GoodsItem.dwGoodsIndate = 0;

				//��ȡ����
				CWHJsonInput::readJson(GoodsItem.wGoodsID, RewardItemPtr->value["GoodsID"], true);
				CWHJsonInput::readJson(GoodsItem.dwGoodsCount, RewardItemPtr->value["Count"], true);
				CWHJsonInput::readJson(GoodsItem.wGoodsLevel, RewardItemPtr->value["Level"], false);
				CWHJsonInput::readJson(GoodsItem.dwGoodsIndate, RewardItemPtr->value["Indate"], false);

				//��ӽ���
				if (IsMoneyGoods(GoodsItem.wGoodsID))
				{
					MoneyArray.Add(GoodsItem);
				}
				else
				{
					GoodsArray.Add(GoodsItem);
				}
			}
		}

		return true;
	}
	catch (...)
	{
		ASSERT(false);
	}

	return false;
}

//������Ʒ
CGameGoodsItemPtr CGameGoodsManager::CreateGameGoods(WORD wGoodsKind)
{
	try
	{
		//��������
		CGameGoodsItemPtr GameGoodsItemPtr(NULL);

		switch (wGoodsKind)
		{
		default:
		{
			GameGoodsItemPtr = new CGameGoodsItem();
			break;
		}
		}

		return GameGoodsItemPtr;
	}
	catch (...)
	{
		ASSERT (FALSE);
	}	

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////
