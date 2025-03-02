#include "StdAfx.h"
#include "MatchListControl.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CMatchListControl::CMatchListControl()
{
}

//��������
CMatchListControl::~CMatchListControl()
{
}

//�����б�
VOID CMatchListControl::InitListControl()
{
	//��������
	INT nColIndex=0;

	//�����б�
	InsertColumn(nColIndex++,TEXT("������ʶ"),LVCFMT_LEFT,60);			
	InsertColumn(nColIndex++,TEXT("��������"),LVCFMT_LEFT,100);
	InsertColumn(nColIndex++,TEXT("��������"), LVCFMT_LEFT,80);
	InsertColumn(nColIndex++,TEXT("ģ������"), LVCFMT_LEFT,80);
	InsertColumn(nColIndex++,TEXT("������ʽ"),LVCFMT_LEFT,100);
	InsertColumn(nColIndex++,TEXT("��������"),LVCFMT_LEFT,100);

	return;
}

//��������
INT CMatchListControl::SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex)
{
	//��������
	tagMatchOption * pMatchOption1=(tagMatchOption *)lParam1;
	tagMatchOption * pMatchOption2=(tagMatchOption *)lParam2;

	//��������
	switch (wColumnIndex)
	{
	case 0:		//������ʶ
	{
		return pMatchOption1->dwMatchID>pMatchOption2->dwMatchID?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
	}	
	case 1:		//��������
	{
		return lstrcmp(pMatchOption1->szMatchName,pMatchOption2->szMatchName);
	}
	case 2:		//��������
	{
		return pMatchOption1->cbMatchType>pMatchOption2->cbMatchType?SORT_POSITION_AFTER:SORT_POSITION_FRONT;			
	}
	case 3:		//ģ������
	{
		return lstrcmp(pMatchOption1->szModuleName, pMatchOption2->szModuleName);
	}
	case 4:		//������ʽ
	{
		return pMatchOption1->cbSignupMode<pMatchOption2->cbSignupMode?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
	}
	case 5:		//��������
	{
		return pMatchOption1->lFeeAmount>pMatchOption2->lFeeAmount ?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
	}
	}

	return 0;
}

//��ȡ��ɫ
VOID CMatchListControl::GetListItemColor(INT nItemID,LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor)
{
	//��������
	ASSERT(lItemParam!=NULL);

	//������ɫ
	//if (nItemID%2==0)
	{
		ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(0,0,128):CR_NORMAL_BK;
		ListItemColor.rcTextColor=(uItemStatus&ODS_SELECTED)?RGB(255,255,255):RGB(0,0,0);
	}
	/*else
	{
		ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(0,0,128):RGB(185,185,185);
		ListItemColor.rcTextColor=(uItemStatus&ODS_SELECTED)?RGB(0,0,0):RGB(255,255,255);
	}*/

	return;
}

//�����б�
bool CMatchListControl::InsertMatchOption(tagMatchOption * pMatchOption)
{
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchOption;

	//�����ж�
	INT nInsertItem=FindItem(&FindInfo);
	if (nInsertItem!=LB_ERR) return true;

	//�����б�
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		if (i==0)
		{
			//��������
			INT nIndex=GetInsertIndex(pMatchOption);
			LPCTSTR pszDescribe=GetDescribeString(pMatchOption,i);
			nInsertItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,pszDescribe,0,0,0,(LPARAM)pMatchOption);
		}
		else
		{
			//�ַ�����
			SetItem(nInsertItem,i,LVIF_TEXT,GetDescribeString(pMatchOption,i),0,0,0,0);
		}
	}

	return true;
}

//�����б�
bool CMatchListControl::UpdateMatchOption(tagMatchOption * pMatchOption)
{
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchOption;

	//�����ж�
	INT nUpdateItem=FindItem(&FindInfo);
	if (nUpdateItem==LB_ERR) return true;

	//�����б�
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		//�ַ�����
		SetItem(nUpdateItem,i,LVIF_TEXT,GetDescribeString(pMatchOption,i),0,0,0,0);
	}

	//��������
	RedrawItems(nUpdateItem,nUpdateItem);

	return true;
}

//��������
WORD CMatchListControl::GetInsertIndex(tagMatchOption * pMatchOption)
{
	//��������
	INT nItemCount=GetItemCount();
	tagMatchOption * pGameMatchTemp=NULL;

	//��ȡλ��
	for (INT i=0;i<nItemCount;i++)
	{
		//��ȡ����
		pGameMatchTemp=(tagMatchOption *)GetItemData(i);

		//��������
		if (lstrcmp(pMatchOption->szMatchName,pGameMatchTemp->szMatchName)<0)
		{
			return i;
		}
	}

	return nItemCount;
}

//�����ַ�
LPCTSTR CMatchListControl::GetDescribeString(tagMatchOption * pMatchOption, WORD wColumnIndex)
{
	//��������
	static TCHAR szDescribe[128]=TEXT("");

	//�����ַ�
	switch (wColumnIndex)
	{
	case 0:		//������ʶ
	{
		_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%d"),pMatchOption->dwMatchID);
		return szDescribe;
	}		
	case 1:		//��������
	{
		return CW2CT(pMatchOption->szMatchName);
	}
	case 2:		//��������
	{
		//��ʱ��
		if (pMatchOption->cbMatchType & MATCH_TYPE_LOCKTIME)
		{
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("��ʱ��"));
		}

		//��ʱ��
		if (pMatchOption->cbMatchType & MATCH_TYPE_IMMEDIATE)
		{
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("��ʱ��"));
		}

		//������
		if (pMatchOption->cbMatchType & MATCH_TYPE_TOURNAMENT)
		{
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("������"));
		}

		return szDescribe;
	}
	case 3:		//ģ������
	{
		return pMatchOption->szModuleName;
	}
	case 4:		//������ʽ
	{
		//��������			
		CString strSignupMode;

		//���ɷ���
		if (pMatchOption->cbSignupMode&SIGNUP_MODE_SIGNUP_FEE)
		{
			strSignupMode.Append(TEXT("���ɷ���"));
		}

		//ʹ�ñ���ȯ
		if (pMatchOption->cbSignupMode & SIGNUP_MODE_MATCH_TICKET)
		{
			if (strSignupMode.IsEmpty() == false) strSignupMode.Append(TEXT("��"));
			strSignupMode.Append(TEXT("ʹ�ñ���ȯ"));
		}

		//����Ȧ����
		/*if (pMatchOption->cbSignupMode&SIGNUP_MODE_WECHAT_SHARE)
		{
			if (strSignupMode.IsEmpty()==false) strSignupMode.Append(TEXT("��"));
			strSignupMode.Append(TEXT("����Ȧ����"));
		}*/

		StringCchCopy(szDescribe, CountArray(szDescribe), strSignupMode);

		return szDescribe;
	}
	case 5:		//��������
	{
		//��������			
		CString strSignupMode;

		//���ɷ���
		if (pMatchOption->cbSignupMode & SIGNUP_MODE_SIGNUP_FEE)
		{
			//��������
			LPCTSTR pszFeeName = TEXT("");
			if (pMatchOption->wFeeMoneyID == GOODS_ID_GOLD) pszFeeName = TEXT("���");
			//if (pMatchOption->wFeeMoneyID == GOODS_ID_INGOT) pszFeeName = TEXT("��ʯ");

			strSignupMode.AppendFormat(TEXT("%I64d%s"),pMatchOption->lFeeAmount, pszFeeName);
		}

		//ʹ�ñ���ȯ
		if (pMatchOption->cbSignupMode & SIGNUP_MODE_MATCH_TICKET)
		{
			if (strSignupMode.IsEmpty() == false) strSignupMode.Append(TEXT("��"));
			strSignupMode.AppendFormat(TEXT("%d�ű���ȯ"), pMatchOption->wUseTicketCount);
		}

		//����Ȧ����
	/*	if (pMatchOption->cbSignupMode & SIGNUP_MODE_WECHAT_SHARE)
		{
			if (strSignupMode.IsEmpty() == false) strSignupMode.Append(TEXT("��"));
			strSignupMode.Append(TEXT("����Ȧ����"));
		}*/

		StringCchCopy(szDescribe, CountArray(szDescribe), strSignupMode);

		return szDescribe;
	}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////


//���캯��
CRewardListControl::CRewardListControl()
{
}

//��������
CRewardListControl::~CRewardListControl()
{
}

//�����б�
VOID CRewardListControl::InitListControl()
{
	//��������
	INT nColIndex=0;

	//�����б�
	InsertColumn(nColIndex++,TEXT("��  ��"),LVCFMT_LEFT,80);
	InsertColumn(nColIndex++,TEXT("ʵ�ｱ��"),LVCFMT_LEFT,160);
	InsertColumn(nColIndex++,TEXT("��Ʒ����"),LVCFMT_LEFT,160);	

	return;
}

//��������
INT CRewardListControl::SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex)
{
	//��������
	tagMatchRewardItem * pMatchRewardItem1=(tagMatchRewardItem *)lParam1;
	tagMatchRewardItem * pMatchRewardItem2=(tagMatchRewardItem *)lParam2;

	//��������
	switch (wColumnIndex)
	{
	case 0:		//��������
		{
			return pMatchRewardItem1->wStartRankID>pMatchRewardItem2->wStartRankID;
		}
	case 1:		//ʵ�ｱ��
		{
			return lstrlen(pMatchRewardItem1->szRewardEntity)>lstrlen(pMatchRewardItem2->szRewardEntity);			
		}
	case 2:		//��Ʒ����
		{
			return lstrlen(pMatchRewardItem1->szRewardGoods)>lstrlen(pMatchRewardItem2->szRewardGoods);
		}
	}

	return 0;
}

//��ȡ��ɫ
VOID CRewardListControl::GetListItemColor(INT nItemID,LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor)
{
	//��������
	ASSERT(lItemParam!=NULL);

	//������ɫ
	ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(0,0,128):CR_NORMAL_BK;
	ListItemColor.rcTextColor=(uItemStatus&ODS_SELECTED)?RGB(255,255,255):RGB(0,0,0);

	return;
}

//�����б�
bool CRewardListControl::InsertMatchReward(tagMatchRewardItem * pMatchRewardItem)
{
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchRewardItem;

	//�����ж�
	INT nInsertItem=FindItem(&FindInfo);
	if (nInsertItem!=LB_ERR) return true;

	//�����б�
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		if (i==0)
		{
			//��������
			INT nIndex=GetInsertIndex(pMatchRewardItem);
			LPCTSTR pszDescribe=GetDescribeString(pMatchRewardItem,i);
			nInsertItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,pszDescribe,0,0,0,(LPARAM)pMatchRewardItem);
		}
		else
		{
			//�ַ�����
			SetItem(nInsertItem,i,LVIF_TEXT,GetDescribeString(pMatchRewardItem,i),0,0,0,0);
		}
	}

	return true;
}

//�����б�
bool CRewardListControl::UpdateMatchReward(tagMatchRewardItem * pMatchRewardItem)
{
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchRewardItem;

	//�����ж�
	INT nUpdateItem=FindItem(&FindInfo);
	if (nUpdateItem==LB_ERR) return true;

	//�����б�
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		//�ַ�����
		SetItem(nUpdateItem,i,LVIF_TEXT,GetDescribeString(pMatchRewardItem,i),0,0,0,0);
	}

	//��������
	RedrawItems(nUpdateItem,nUpdateItem);

	return true;
}

//ɾ���б�
bool CRewardListControl::DeleteMatchReward(tagMatchRewardItem * pMatchRewardItem)
{
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchRewardItem;

	//�����ж�
	INT nDeleteItem=FindItem(&FindInfo);
	if (nDeleteItem==LB_ERR) return true;

	//ɾ������
	DeleteItem(nDeleteItem);

	return true;
}

//��������
WORD CRewardListControl::GetInsertIndex(tagMatchRewardItem * pMatchRewardItem)
{
	//��������
	INT nItemCount=GetItemCount();
	tagMatchRewardItem * pMatchRewardTemp=NULL;

	//��ȡλ��
	for (INT i=0;i<nItemCount;i++)
	{
		//��ȡ����
		pMatchRewardTemp=(tagMatchRewardItem *)GetItemData(i);

		//��������
		if (pMatchRewardItem->wStartRankID < pMatchRewardTemp->wStartRankID)
		{
			return i;
		}
	}

	return nItemCount;
}


//�����ַ�
LPCTSTR CRewardListControl::GetDescribeString(tagMatchRewardItem * pMatchRewardItem, WORD wColumnIndex)
{
	//��������
	static TCHAR szDescribe[512]=TEXT("");

	//�����ַ�
	switch (wColumnIndex)
	{
	case 0:		//��������
		{
			if (pMatchRewardItem->wStartRankID==pMatchRewardItem->wEndRankID)
			{
				_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("�� %d ��"),pMatchRewardItem->wStartRankID);
			}
			else
			{
				_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("�� %d - %d ��"),pMatchRewardItem->wStartRankID,pMatchRewardItem->wEndRankID);
			}

			return szDescribe;
		}
	case 1:		//�������
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%s"),pMatchRewardItem->szRewardEntity);

			return szDescribe;
		}
	case 2:		//������ʯ
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%s"),pMatchRewardItem->szRewardGoods);

			return szDescribe;
		}
	}

	return NULL;
}


//////////////////////////////////////////////////////////////////////////////////


//���캯��
CRoundListControl::CRoundListControl()
{
}

//��������
CRoundListControl::~CRoundListControl()
{
}

//�����б�
VOID CRoundListControl::InitListControl()
{
	//��������
	INT nColIndex=0;

	//�����б�
	InsertColumn(nColIndex++,TEXT("�ִα�ʶ"),LVCFMT_LEFT,80);
	InsertColumn(nColIndex++,TEXT("������ʽ"),LVCFMT_LEFT,80);
	InsertColumn(nColIndex++,TEXT("��������"),LVCFMT_LEFT,80);
	InsertColumn(nColIndex++,TEXT("��Ϸ����"),LVCFMT_LEFT,80);	
	InsertColumn(nColIndex++,TEXT("��ʼ�׷�"),LVCFMT_LEFT,80);	

	return;
}

//��������
INT CRoundListControl::SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex)
{
	//��������
	tagMatchRoundItem * pMatchRoundItem1=(tagMatchRoundItem *)lParam1;
	tagMatchRoundItem * pMatchRoundItem2=(tagMatchRoundItem *)lParam2;

	//��������
	switch (wColumnIndex)
	{
	case 0:		//�ִα�ʶ
		{
			return pMatchRoundItem1->wRoundID>pMatchRoundItem2->wRoundID;
		}
	case 1:		//������ʽ
		{
			return pMatchRoundItem1->cbPromoteMode>pMatchRoundItem2->cbPromoteMode;			
		}
	case 2:		//��������
		{
			return pMatchRoundItem1->wPromoteCount>pMatchRoundItem2->wPromoteCount;			
		}
	case 3:		//��Ϸ����
		{
			return pMatchRoundItem1->wPlayCount>pMatchRoundItem2->wPlayCount;
		}
	case 4:		//��ʼ�׷�
		{
			return pMatchRoundItem1->lInitBaseScore>pMatchRoundItem2->lInitBaseScore;
		}
	}

	return 0;
}

//��ȡ��ɫ
VOID CRoundListControl::GetListItemColor(INT nItemID,LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor)
{
	//��������
	ASSERT(lItemParam!=NULL);

	//������ɫ
	ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(0,0,128):CR_NORMAL_BK;
	ListItemColor.rcTextColor=(uItemStatus&ODS_SELECTED)?RGB(255,255,255):RGB(0,0,0);

	return;
}

//�����б�
bool CRoundListControl::InsertMatchRound(tagMatchRoundItem * pMatchRoundItem)
{
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchRoundItem;

	//�����ж�
	INT nInsertItem=FindItem(&FindInfo);
	if (nInsertItem!=LB_ERR) return true;

	//�����б�
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		if (i==0)
		{
			//��������
			INT nIndex=GetInsertIndex(pMatchRoundItem);
			LPCTSTR pszDescribe=GetDescribeString(pMatchRoundItem,i);
			nInsertItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,pszDescribe,0,0,0,(LPARAM)pMatchRoundItem);
		}
		else
		{
			//�ַ�����
			SetItem(nInsertItem,i,LVIF_TEXT,GetDescribeString(pMatchRoundItem,i),0,0,0,0);
		}
	}

	return true;
}

//�����б�
bool CRoundListControl::UpdateMatchRound(tagMatchRoundItem * pMatchRoundItem)
{
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchRoundItem;

	//�����ж�
	INT nUpdateItem=FindItem(&FindInfo);
	if (nUpdateItem==LB_ERR) return true;

	//�����б�
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		//�ַ�����
		SetItem(nUpdateItem,i,LVIF_TEXT,GetDescribeString(pMatchRoundItem,i),0,0,0,0);
	}

	//��������
	RedrawItems(nUpdateItem,nUpdateItem);

	return true;
}

//ɾ���б�
bool CRoundListControl::DeleteMatchRound(tagMatchRoundItem * pMatchRoundItem)
{
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchRoundItem;

	//�����ж�
	INT nDeleteItem=FindItem(&FindInfo);
	if (nDeleteItem==LB_ERR) return true;

	//ɾ������
	DeleteItem(nDeleteItem);

	return true;
}

//��������
WORD CRoundListControl::GetInsertIndex(tagMatchRoundItem * pMatchRoundItem)
{
	//��������
	INT nItemCount=GetItemCount();
	tagMatchRoundItem * pMatchRoundTemp=NULL;

	//��ȡλ��
	for (INT i=0;i<nItemCount;i++)
	{
		//��ȡ����
		pMatchRoundTemp=(tagMatchRoundItem *)GetItemData(i);

		//��������
		if (pMatchRoundItem->wRoundID < pMatchRoundTemp->wRoundID)
		{
			return i;
		}
	}

	return nItemCount;
}


//�����ַ�
LPCTSTR CRoundListControl::GetDescribeString(tagMatchRoundItem * pMatchRoundItem, WORD wColumnIndex)
{
	//��������
	static TCHAR szDescribe[128]=TEXT("");

	//�����ַ�
	switch (wColumnIndex)
	{
	case 0:		//�ִα�ʶ
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("�� %d ��"),pMatchRoundItem->wRoundID);			

			return szDescribe;
		}
	case 1:		//������ʽ
		{
			LPCTSTR szPromoteMode[]={TEXT("��������"),TEXT("���ֻ���"),TEXT("��������")};
			if (pMatchRoundItem->cbPromoteMode<CountArray(szPromoteMode))
			{
				_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%s"),szPromoteMode[pMatchRoundItem->cbPromoteMode]);
			}

			return szDescribe;
		}
	case 2:		//��������
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%d"),pMatchRoundItem->wPromoteCount);

			return szDescribe;
		}
	case 3:		//��Ϸ����
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%d"),pMatchRoundItem->wPlayCount);

			return szDescribe;
		}
	case 4:		//��ʼ�׷�
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%d"),pMatchRoundItem->lInitBaseScore);

			return szDescribe;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////
