#include "StdAfx.h"
#include "MatchListControl.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CMatchListControl::CMatchListControl()
{
}

//析构函数
CMatchListControl::~CMatchListControl()
{
}

//配置列表
VOID CMatchListControl::InitListControl()
{
	//变量定义
	INT nColIndex=0;

	//配置列表
	InsertColumn(nColIndex++,TEXT("比赛标识"),LVCFMT_LEFT,60);			
	InsertColumn(nColIndex++,TEXT("比赛名称"),LVCFMT_LEFT,100);
	InsertColumn(nColIndex++,TEXT("比赛类型"), LVCFMT_LEFT,80);
	InsertColumn(nColIndex++,TEXT("模块名称"), LVCFMT_LEFT,80);
	InsertColumn(nColIndex++,TEXT("报名方式"),LVCFMT_LEFT,100);
	InsertColumn(nColIndex++,TEXT("报名费用"),LVCFMT_LEFT,100);

	return;
}

//子项排序
INT CMatchListControl::SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex)
{
	//变量定义
	tagMatchOption * pMatchOption1=(tagMatchOption *)lParam1;
	tagMatchOption * pMatchOption2=(tagMatchOption *)lParam2;

	//子项排序
	switch (wColumnIndex)
	{
	case 0:		//比赛标识
	{
		return pMatchOption1->dwMatchID>pMatchOption2->dwMatchID?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
	}	
	case 1:		//比赛名称
	{
		return lstrcmp(pMatchOption1->szMatchName,pMatchOption2->szMatchName);
	}
	case 2:		//比赛类型
	{
		return pMatchOption1->cbMatchType>pMatchOption2->cbMatchType?SORT_POSITION_AFTER:SORT_POSITION_FRONT;			
	}
	case 3:		//模块名称
	{
		return lstrcmp(pMatchOption1->szModuleName, pMatchOption2->szModuleName);
	}
	case 4:		//报名方式
	{
		return pMatchOption1->cbSignupMode<pMatchOption2->cbSignupMode?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
	}
	case 5:		//比赛费用
	{
		return pMatchOption1->lFeeAmount>pMatchOption2->lFeeAmount ?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
	}
	}

	return 0;
}

//获取颜色
VOID CMatchListControl::GetListItemColor(INT nItemID,LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor)
{
	//变量定义
	ASSERT(lItemParam!=NULL);

	//设置颜色
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

//插入列表
bool CMatchListControl::InsertMatchOption(tagMatchOption * pMatchOption)
{
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchOption;

	//存在判断
	INT nInsertItem=FindItem(&FindInfo);
	if (nInsertItem!=LB_ERR) return true;

	//插入列表
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		if (i==0)
		{
			//插入首项
			INT nIndex=GetInsertIndex(pMatchOption);
			LPCTSTR pszDescribe=GetDescribeString(pMatchOption,i);
			nInsertItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,pszDescribe,0,0,0,(LPARAM)pMatchOption);
		}
		else
		{
			//字符子项
			SetItem(nInsertItem,i,LVIF_TEXT,GetDescribeString(pMatchOption,i),0,0,0,0);
		}
	}

	return true;
}

//更新列表
bool CMatchListControl::UpdateMatchOption(tagMatchOption * pMatchOption)
{
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchOption;

	//存在判断
	INT nUpdateItem=FindItem(&FindInfo);
	if (nUpdateItem==LB_ERR) return true;

	//插入列表
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		//字符子项
		SetItem(nUpdateItem,i,LVIF_TEXT,GetDescribeString(pMatchOption,i),0,0,0,0);
	}

	//更新子项
	RedrawItems(nUpdateItem,nUpdateItem);

	return true;
}

//插入索引
WORD CMatchListControl::GetInsertIndex(tagMatchOption * pMatchOption)
{
	//变量定义
	INT nItemCount=GetItemCount();
	tagMatchOption * pGameMatchTemp=NULL;

	//获取位置
	for (INT i=0;i<nItemCount;i++)
	{
		//获取数据
		pGameMatchTemp=(tagMatchOption *)GetItemData(i);

		//比赛名称
		if (lstrcmp(pMatchOption->szMatchName,pGameMatchTemp->szMatchName)<0)
		{
			return i;
		}
	}

	return nItemCount;
}

//描述字符
LPCTSTR CMatchListControl::GetDescribeString(tagMatchOption * pMatchOption, WORD wColumnIndex)
{
	//变量定义
	static TCHAR szDescribe[128]=TEXT("");

	//构造字符
	switch (wColumnIndex)
	{
	case 0:		//比赛标识
	{
		_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%d"),pMatchOption->dwMatchID);
		return szDescribe;
	}		
	case 1:		//比赛名称
	{
		return CW2CT(pMatchOption->szMatchName);
	}
	case 2:		//比赛类型
	{
		//定时赛
		if (pMatchOption->cbMatchType & MATCH_TYPE_LOCKTIME)
		{
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("定时赛"));
		}

		//即时赛
		if (pMatchOption->cbMatchType & MATCH_TYPE_IMMEDIATE)
		{
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("即时赛"));
		}

		//锦标赛
		if (pMatchOption->cbMatchType & MATCH_TYPE_TOURNAMENT)
		{
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("锦标赛"));
		}

		return szDescribe;
	}
	case 3:		//模块名称
	{
		return pMatchOption->szModuleName;
	}
	case 4:		//报名方式
	{
		//变量定义			
		CString strSignupMode;

		//缴纳费用
		if (pMatchOption->cbSignupMode&SIGNUP_MODE_SIGNUP_FEE)
		{
			strSignupMode.Append(TEXT("缴纳费用"));
		}

		//使用比赛券
		if (pMatchOption->cbSignupMode & SIGNUP_MODE_MATCH_TICKET)
		{
			if (strSignupMode.IsEmpty() == false) strSignupMode.Append(TEXT("、"));
			strSignupMode.Append(TEXT("使用比赛券"));
		}

		//朋友圈分享
		/*if (pMatchOption->cbSignupMode&SIGNUP_MODE_WECHAT_SHARE)
		{
			if (strSignupMode.IsEmpty()==false) strSignupMode.Append(TEXT("、"));
			strSignupMode.Append(TEXT("朋友圈分享"));
		}*/

		StringCchCopy(szDescribe, CountArray(szDescribe), strSignupMode);

		return szDescribe;
	}
	case 5:		//比赛费用
	{
		//变量定义			
		CString strSignupMode;

		//缴纳费用
		if (pMatchOption->cbSignupMode & SIGNUP_MODE_SIGNUP_FEE)
		{
			//费用类型
			LPCTSTR pszFeeName = TEXT("");
			if (pMatchOption->wFeeMoneyID == GOODS_ID_GOLD) pszFeeName = TEXT("金币");
			//if (pMatchOption->wFeeMoneyID == GOODS_ID_INGOT) pszFeeName = TEXT("钻石");

			strSignupMode.AppendFormat(TEXT("%I64d%s"),pMatchOption->lFeeAmount, pszFeeName);
		}

		//使用比赛券
		if (pMatchOption->cbSignupMode & SIGNUP_MODE_MATCH_TICKET)
		{
			if (strSignupMode.IsEmpty() == false) strSignupMode.Append(TEXT("、"));
			strSignupMode.AppendFormat(TEXT("%d张比赛券"), pMatchOption->wUseTicketCount);
		}

		//朋友圈分享
	/*	if (pMatchOption->cbSignupMode & SIGNUP_MODE_WECHAT_SHARE)
		{
			if (strSignupMode.IsEmpty() == false) strSignupMode.Append(TEXT("、"));
			strSignupMode.Append(TEXT("朋友圈分享"));
		}*/

		StringCchCopy(szDescribe, CountArray(szDescribe), strSignupMode);

		return szDescribe;
	}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////


//构造函数
CRewardListControl::CRewardListControl()
{
}

//析构函数
CRewardListControl::~CRewardListControl()
{
}

//配置列表
VOID CRewardListControl::InitListControl()
{
	//变量定义
	INT nColIndex=0;

	//配置列表
	InsertColumn(nColIndex++,TEXT("名  次"),LVCFMT_LEFT,80);
	InsertColumn(nColIndex++,TEXT("实物奖励"),LVCFMT_LEFT,160);
	InsertColumn(nColIndex++,TEXT("物品奖励"),LVCFMT_LEFT,160);	

	return;
}

//子项排序
INT CRewardListControl::SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex)
{
	//变量定义
	tagMatchRewardItem * pMatchRewardItem1=(tagMatchRewardItem *)lParam1;
	tagMatchRewardItem * pMatchRewardItem2=(tagMatchRewardItem *)lParam2;

	//子项排序
	switch (wColumnIndex)
	{
	case 0:		//奖励名称
		{
			return pMatchRewardItem1->wStartRankID>pMatchRewardItem2->wStartRankID;
		}
	case 1:		//实物奖励
		{
			return lstrlen(pMatchRewardItem1->szRewardEntity)>lstrlen(pMatchRewardItem2->szRewardEntity);			
		}
	case 2:		//物品奖励
		{
			return lstrlen(pMatchRewardItem1->szRewardGoods)>lstrlen(pMatchRewardItem2->szRewardGoods);
		}
	}

	return 0;
}

//获取颜色
VOID CRewardListControl::GetListItemColor(INT nItemID,LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor)
{
	//变量定义
	ASSERT(lItemParam!=NULL);

	//设置颜色
	ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(0,0,128):CR_NORMAL_BK;
	ListItemColor.rcTextColor=(uItemStatus&ODS_SELECTED)?RGB(255,255,255):RGB(0,0,0);

	return;
}

//插入列表
bool CRewardListControl::InsertMatchReward(tagMatchRewardItem * pMatchRewardItem)
{
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchRewardItem;

	//存在判断
	INT nInsertItem=FindItem(&FindInfo);
	if (nInsertItem!=LB_ERR) return true;

	//插入列表
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		if (i==0)
		{
			//插入首项
			INT nIndex=GetInsertIndex(pMatchRewardItem);
			LPCTSTR pszDescribe=GetDescribeString(pMatchRewardItem,i);
			nInsertItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,pszDescribe,0,0,0,(LPARAM)pMatchRewardItem);
		}
		else
		{
			//字符子项
			SetItem(nInsertItem,i,LVIF_TEXT,GetDescribeString(pMatchRewardItem,i),0,0,0,0);
		}
	}

	return true;
}

//更新列表
bool CRewardListControl::UpdateMatchReward(tagMatchRewardItem * pMatchRewardItem)
{
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchRewardItem;

	//存在判断
	INT nUpdateItem=FindItem(&FindInfo);
	if (nUpdateItem==LB_ERR) return true;

	//插入列表
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		//字符子项
		SetItem(nUpdateItem,i,LVIF_TEXT,GetDescribeString(pMatchRewardItem,i),0,0,0,0);
	}

	//更新子项
	RedrawItems(nUpdateItem,nUpdateItem);

	return true;
}

//删除列表
bool CRewardListControl::DeleteMatchReward(tagMatchRewardItem * pMatchRewardItem)
{
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchRewardItem;

	//存在判断
	INT nDeleteItem=FindItem(&FindInfo);
	if (nDeleteItem==LB_ERR) return true;

	//删除子项
	DeleteItem(nDeleteItem);

	return true;
}

//插入索引
WORD CRewardListControl::GetInsertIndex(tagMatchRewardItem * pMatchRewardItem)
{
	//变量定义
	INT nItemCount=GetItemCount();
	tagMatchRewardItem * pMatchRewardTemp=NULL;

	//获取位置
	for (INT i=0;i<nItemCount;i++)
	{
		//获取数据
		pMatchRewardTemp=(tagMatchRewardItem *)GetItemData(i);

		//比赛名称
		if (pMatchRewardItem->wStartRankID < pMatchRewardTemp->wStartRankID)
		{
			return i;
		}
	}

	return nItemCount;
}


//描述字符
LPCTSTR CRewardListControl::GetDescribeString(tagMatchRewardItem * pMatchRewardItem, WORD wColumnIndex)
{
	//变量定义
	static TCHAR szDescribe[512]=TEXT("");

	//构造字符
	switch (wColumnIndex)
	{
	case 0:		//比赛名次
		{
			if (pMatchRewardItem->wStartRankID==pMatchRewardItem->wEndRankID)
			{
				_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("第 %d 名"),pMatchRewardItem->wStartRankID);
			}
			else
			{
				_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("第 %d - %d 名"),pMatchRewardItem->wStartRankID,pMatchRewardItem->wEndRankID);
			}

			return szDescribe;
		}
	case 1:		//奖励金币
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%s"),pMatchRewardItem->szRewardEntity);

			return szDescribe;
		}
	case 2:		//奖励钻石
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%s"),pMatchRewardItem->szRewardGoods);

			return szDescribe;
		}
	}

	return NULL;
}


//////////////////////////////////////////////////////////////////////////////////


//构造函数
CRoundListControl::CRoundListControl()
{
}

//析构函数
CRoundListControl::~CRoundListControl()
{
}

//配置列表
VOID CRoundListControl::InitListControl()
{
	//变量定义
	INT nColIndex=0;

	//配置列表
	InsertColumn(nColIndex++,TEXT("轮次标识"),LVCFMT_LEFT,80);
	InsertColumn(nColIndex++,TEXT("晋级方式"),LVCFMT_LEFT,80);
	InsertColumn(nColIndex++,TEXT("晋级人数"),LVCFMT_LEFT,80);
	InsertColumn(nColIndex++,TEXT("游戏局数"),LVCFMT_LEFT,80);	
	InsertColumn(nColIndex++,TEXT("初始底分"),LVCFMT_LEFT,80);	

	return;
}

//子项排序
INT CRoundListControl::SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex)
{
	//变量定义
	tagMatchRoundItem * pMatchRoundItem1=(tagMatchRoundItem *)lParam1;
	tagMatchRoundItem * pMatchRoundItem2=(tagMatchRoundItem *)lParam2;

	//子项排序
	switch (wColumnIndex)
	{
	case 0:		//轮次标识
		{
			return pMatchRoundItem1->wRoundID>pMatchRoundItem2->wRoundID;
		}
	case 1:		//晋级方式
		{
			return pMatchRoundItem1->cbPromoteMode>pMatchRoundItem2->cbPromoteMode;			
		}
	case 2:		//晋级人数
		{
			return pMatchRoundItem1->wPromoteCount>pMatchRoundItem2->wPromoteCount;			
		}
	case 3:		//游戏局数
		{
			return pMatchRoundItem1->wPlayCount>pMatchRoundItem2->wPlayCount;
		}
	case 4:		//初始底分
		{
			return pMatchRoundItem1->lInitBaseScore>pMatchRoundItem2->lInitBaseScore;
		}
	}

	return 0;
}

//获取颜色
VOID CRoundListControl::GetListItemColor(INT nItemID,LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor)
{
	//变量定义
	ASSERT(lItemParam!=NULL);

	//设置颜色
	ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(0,0,128):CR_NORMAL_BK;
	ListItemColor.rcTextColor=(uItemStatus&ODS_SELECTED)?RGB(255,255,255):RGB(0,0,0);

	return;
}

//插入列表
bool CRoundListControl::InsertMatchRound(tagMatchRoundItem * pMatchRoundItem)
{
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchRoundItem;

	//存在判断
	INT nInsertItem=FindItem(&FindInfo);
	if (nInsertItem!=LB_ERR) return true;

	//插入列表
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		if (i==0)
		{
			//插入首项
			INT nIndex=GetInsertIndex(pMatchRoundItem);
			LPCTSTR pszDescribe=GetDescribeString(pMatchRoundItem,i);
			nInsertItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,pszDescribe,0,0,0,(LPARAM)pMatchRoundItem);
		}
		else
		{
			//字符子项
			SetItem(nInsertItem,i,LVIF_TEXT,GetDescribeString(pMatchRoundItem,i),0,0,0,0);
		}
	}

	return true;
}

//更新列表
bool CRoundListControl::UpdateMatchRound(tagMatchRoundItem * pMatchRoundItem)
{
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchRoundItem;

	//存在判断
	INT nUpdateItem=FindItem(&FindInfo);
	if (nUpdateItem==LB_ERR) return true;

	//插入列表
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		//字符子项
		SetItem(nUpdateItem,i,LVIF_TEXT,GetDescribeString(pMatchRoundItem,i),0,0,0,0);
	}

	//更新子项
	RedrawItems(nUpdateItem,nUpdateItem);

	return true;
}

//删除列表
bool CRoundListControl::DeleteMatchRound(tagMatchRoundItem * pMatchRoundItem)
{
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchRoundItem;

	//存在判断
	INT nDeleteItem=FindItem(&FindInfo);
	if (nDeleteItem==LB_ERR) return true;

	//删除子项
	DeleteItem(nDeleteItem);

	return true;
}

//插入索引
WORD CRoundListControl::GetInsertIndex(tagMatchRoundItem * pMatchRoundItem)
{
	//变量定义
	INT nItemCount=GetItemCount();
	tagMatchRoundItem * pMatchRoundTemp=NULL;

	//获取位置
	for (INT i=0;i<nItemCount;i++)
	{
		//获取数据
		pMatchRoundTemp=(tagMatchRoundItem *)GetItemData(i);

		//比赛名称
		if (pMatchRoundItem->wRoundID < pMatchRoundTemp->wRoundID)
		{
			return i;
		}
	}

	return nItemCount;
}


//描述字符
LPCTSTR CRoundListControl::GetDescribeString(tagMatchRoundItem * pMatchRoundItem, WORD wColumnIndex)
{
	//变量定义
	static TCHAR szDescribe[128]=TEXT("");

	//构造字符
	switch (wColumnIndex)
	{
	case 0:		//轮次标识
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("第 %d 轮"),pMatchRoundItem->wRoundID);			

			return szDescribe;
		}
	case 1:		//晋级方式
		{
			LPCTSTR szPromoteMode[]={TEXT("打立出局"),TEXT("定局积分"),TEXT("按桌晋级")};
			if (pMatchRoundItem->cbPromoteMode<CountArray(szPromoteMode))
			{
				_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%s"),szPromoteMode[pMatchRoundItem->cbPromoteMode]);
			}

			return szDescribe;
		}
	case 2:		//晋级人数
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%d"),pMatchRoundItem->wPromoteCount);

			return szDescribe;
		}
	case 3:		//游戏局数
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%d"),pMatchRoundItem->wPlayCount);

			return szDescribe;
		}
	case 4:		//初始底分
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%d"),pMatchRoundItem->lInitBaseScore);

			return szDescribe;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////
