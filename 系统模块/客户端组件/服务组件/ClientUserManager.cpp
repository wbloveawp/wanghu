#include "StdAfx.h"
#include "ClientUserManager.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

CClientUserItemArray CPlazaUserManager::m_UserItemStorage;				//存储数组

//////////////////////////////////////////////////////////////////////////

//构造函数
CClientUserItem::CClientUserItem()
{
	//设置变量
	m_cbCompanion=CP_NORMAL;
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	ZeroMemory(m_szUserNote,sizeof(m_szUserNote));

	return;
}

//析构函数
CClientUserItem::~CClientUserItem()
{
}

//接口查询
VOID * __cdecl CClientUserItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IClientUserItem,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IClientUserItem,Guid,dwQueryVer);
	return NULL;
}

//重要等级
LONG __cdecl CClientUserItem::GetImportOrder()
{
	//构造等级
	INT nOrder=0;
	if (m_cbCompanion==CP_FRIEND) nOrder+=1000;
	if (m_UserInfo.dwMemberPoint!=0) nOrder+=m_UserInfo.dwMemberPoint*100L;
	if (m_UserInfo.cbMasterOrder!=0) nOrder+=m_UserInfo.cbMasterOrder*10000L;

	return nOrder;
}

//经验数值
LONG __cdecl CClientUserItem::GetUserExperience()
{
	return m_UserInfo.lExperience;
}

//获取等级
WORD __cdecl CClientUserItem::GetExperienceLevel()
{
	//没有等级
	if (m_UserInfo.lExperience<=0L) return 0;

	//变量定义
	LONG lIncrease=0L;
	LONG lLevelValue=0L;

	//等级计算

	// add by humorly 2018/8/1
	// make the value useful outside loop
	WORD wUserLevel = 1;
	for (wUserLevel=1;wUserLevel<60L;wUserLevel++)
	{
		lIncrease+=wUserLevel*30L;
		lLevelValue=lLevelValue+lIncrease;
		if (m_UserInfo.lExperience<lLevelValue) break;
	}

	return wUserLevel;
}

//等级信息
WORD __cdecl CClientUserItem::GetExperienceIndex(BYTE cbOrderIndex[], WORD wMaxCount)
{
	//变量定义
	WORD wIndexCount=0;
	WORD wLevelVaule[]={1L,5L,10L};

	//获取等级
	WORD wUserLevel=GetExperienceLevel();

	//设置等级
	for (WORD i=0;i<wMaxCount;i++)
	{
		//获取索引
		for (INT j=0;j<CountArray(wLevelVaule);j++)
		{
			if (wUserLevel>=wLevelVaule[CountArray(wLevelVaule)-j-1])
			{
				//设置变量
				wUserLevel-=wLevelVaule[CountArray(wLevelVaule)-j-1];
				cbOrderIndex[wIndexCount++]=CountArray(wLevelVaule)-j-1;

				break;
			}
		}

		//终止判断
		if (wUserLevel==0L) break;
	}

	return wIndexCount;
}

//用户胜率
float __cdecl CClientUserItem::GetUserWinRate()
{
	LONG lPlayCount=GetUserPlayCount();
	if (lPlayCount!=0L) return (float)(m_UserInfo.lWinCount*100.0f/(float)lPlayCount);

	return 0.0f;
}

//用户输率
float __cdecl CClientUserItem::GetUserLostRate()
{
	LONG lPlayCount=GetUserPlayCount();
	if (lPlayCount!=0L) return (float)(m_UserInfo.lLostCount*100.0f/(float)lPlayCount);

	return 0.0f;
}

//用户和率
float __cdecl CClientUserItem::GetUserDrawRate()
{
	LONG lPlayCount=GetUserPlayCount();
	if (lPlayCount!=0L) return (float)(m_UserInfo.lDrawCount*100.0f/(float)lPlayCount);

	return 0.0f;
}

//用户逃率
float __cdecl CClientUserItem::GetUserFleeRate()
{
	LONG lPlayCount=GetUserPlayCount();
	if (lPlayCount!=0L) return (float)(m_UserInfo.lFleeCount*100.0f/(float)lPlayCount);

	return 0.0f;
}

//设置备注
VOID __cdecl CClientUserItem::SetUserNoteInfo(LPCWSTR pszUserNote)
{
	//效验参数
	ASSERT(pszUserNote!=NULL);
	if (pszUserNote==NULL) return;

	//设置备注
	lstrcpynW(m_szUserNote,pszUserNote,CountArray(m_szUserNote));

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CPlazaUserManager::CPlazaUserManager()
{
	//组件接口
	m_pIUserManagerSink=NULL;
	m_pIUserInformation=NULL;

	return;
}

//析构函数
CPlazaUserManager::~CPlazaUserManager()
{
	//存储用户
	m_UserItemStorage.Append(m_UserItemActive);

	//释放资源
	for (INT_PTR i=0;i<m_UserItemStorage.GetCount();i++)
	{
		if (m_UserItemStorage[i]!=NULL) m_UserItemStorage[i]->Release();
	}

	//清理用户
	m_UserItemActive.RemoveAll();
	m_UserItemStorage.RemoveAll();

	return;
}

//接口查询
VOID * __cdecl CPlazaUserManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IPlazaUserManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IPlazaUserManager,Guid,dwQueryVer);
	return NULL;
}

//设置接口
bool __cdecl CPlazaUserManager::SetUserInformation(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if (pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IUserInformation)!=NULL);
		m_pIUserInformation=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IUserInformation);

		//成功判断
		if (m_pIUserInformation==NULL) return false;
	}
	else m_pIUserInformation=NULL;

	return true;
}

//设置接口
bool __cdecl CPlazaUserManager::SetUserManagerSink(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if (pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IUserManagerSink)!=NULL);
		m_pIUserManagerSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IUserManagerSink);

		//成功判断
		if (m_pIUserManagerSink==NULL) return false;
	}
	else m_pIUserManagerSink=NULL;

	return true;
}

//增加用户
IClientUserItem * __cdecl CPlazaUserManager::ActiveUserItem(const tagClientUserInfo& UserInfo, tagCustomFaceInfo & CustomFaceInfo)
{
	//变量定义
	CClientUserItem * pClientUserItem=NULL;
	INT_PTR nStorageCount=m_UserItemStorage.GetCount();

	//获取用户
	if (nStorageCount>0)
	{
		pClientUserItem=m_UserItemStorage[nStorageCount-1];
		m_UserItemStorage.RemoveAt(nStorageCount-1);
	}
	else
	{
		pClientUserItem=new CClientUserItem;
		if (pClientUserItem==NULL) return NULL;
	}

	//插入用户
	ASSERT(pClientUserItem!=NULL);
	m_UserItemActive.Add(pClientUserItem);

	//信息查找
	//ASSERT(m_pIUserInformation!=NULL);
	//tagCompanionInfo * pCompanionInfo=m_pIUserInformation->SearchCompanionInfo(UserInfo.dwUserID);

	////关系数据
	//if (pCompanionInfo!=NULL)
	//{
	//	pClientUserItem->m_cbCompanion=pCompanionInfo->cbCompanion;
	//	lstrcpynW(pClientUserItem->m_szUserNote,pCompanionInfo->szUserNote,CountArray(pClientUserItem->m_szUserNote));
	//}
	//else 
	{
		pClientUserItem->m_szUserNote[0]=0;
		pClientUserItem->m_cbCompanion=CP_NORMAL; 
	}

	//设置数据
	CopyMemory(&pClientUserItem->m_UserInfo,&UserInfo,sizeof(tagClientUserInfo));
	CopyMemory(&pClientUserItem->m_CustomFaceInfo,&CustomFaceInfo,sizeof(CustomFaceInfo));

	//更新通知
	ASSERT(m_pIUserManagerSink!=NULL);
	if (m_pIUserManagerSink!=NULL) m_pIUserManagerSink->OnUserItemAcitve(pClientUserItem);

	//信息更新
	//if (pCompanionInfo!=NULL)
	//{
	//	//一致判断
	//	bool bSameData=(UserInfo.dwGameID==pCompanionInfo->dwGameID);
	//	if ((bSameData==true)&&(wcscmp(UserInfo.szAccounts,pCompanionInfo->szAccounts)!=0)) bSameData=false;

	//	//更新处理
	//	if (bSameData==false) m_pIUserInformation->InsertCompanionInfo(pClientUserItem,pCompanionInfo->cbCompanion);
	//}

	return pClientUserItem;
}

//删除用户
bool __cdecl CPlazaUserManager::DeleteUserItem(IClientUserItem * pIClientUserItem)
{
	//查找用户
	CClientUserItem * pUserItemActive=NULL;
	for (INT_PTR i=0;i<m_UserItemActive.GetCount();i++)
	{
		pUserItemActive=m_UserItemActive[i];
		if (pIClientUserItem==pUserItemActive)
		{
			//删除用户
			m_UserItemActive.RemoveAt(i);
			m_UserItemStorage.Add(pUserItemActive);

			//删除通知
			ASSERT(m_pIUserManagerSink!=NULL);
			m_pIUserManagerSink->OnUserItemDelete(pUserItemActive);

			//设置数据
			pUserItemActive->m_cbCompanion=CP_NORMAL;
			ZeroMemory(&pUserItemActive->m_UserInfo,sizeof(tagClientUserInfo));

			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//更新积分
bool __cdecl CPlazaUserManager::UpdateUserItemScore(IClientUserItem * pIClientUserItem, tagClientUserScore * const pUserScore)
{
	//效验参数
	ASSERT(pUserScore!=NULL);
	ASSERT(pIClientUserItem!=NULL);

	//获取用户
	tagClientUserInfo* pUserInfo=pIClientUserItem->GetUserInfo();

	//以往数据
	tagClientUserScore UserScore;
	UserScore.lScore=pUserInfo->lScore;
	//UserScore.cbScoreKind= pUserInfo->cbScoreKind;
	UserScore.lWinCount=pUserInfo->lWinCount;
	UserScore.lLostCount=pUserInfo->lLostCount;
	UserScore.lDrawCount=pUserInfo->lDrawCount;
	UserScore.lFleeCount=pUserInfo->lFleeCount;
	UserScore.lExperience=pUserInfo->lExperience;

	//设置数据
	pUserInfo->lScore=pUserScore->lScore;
	//pUserInfo->cbScoreKind= pUserScore->cbScoreKind;
	pUserInfo->lWinCount=pUserScore->lWinCount;
	pUserInfo->lLostCount=pUserScore->lLostCount;
	pUserInfo->lDrawCount=pUserScore->lDrawCount;
	pUserInfo->lFleeCount=pUserScore->lFleeCount;
	pUserInfo->lExperience=pUserScore->lExperience;

	//通知更新
	ASSERT(m_pIUserManagerSink!=NULL);
	m_pIUserManagerSink->OnUserItemUpdate(pIClientUserItem,UserScore);

	return true;
}

//更新状态
bool __cdecl CPlazaUserManager::UpdateUserItemStatus(IClientUserItem * pIClientUserItem, tagUserStatus * const pUserStatus)
{
	//效验参数
	ASSERT(pUserStatus!=NULL);
	ASSERT(pIClientUserItem!=NULL);

	//获取用户
	tagClientUserInfo* pUserInfo=pIClientUserItem->GetUserInfo();

	//以往数据
	tagUserStatus UserStatus;
	UserStatus.wTableID=pUserInfo->wTableID;
	UserStatus.wChairID=pUserInfo->wChairID;
	UserStatus.cbUserStatus=pUserInfo->cbUserStatus;

	//设置数据
	pUserInfo->wTableID=pUserStatus->wTableID;
	pUserInfo->wChairID=pUserStatus->wChairID;
	pUserInfo->cbUserStatus=pUserStatus->cbUserStatus;

	//通知更新
	ASSERT(m_pIUserManagerSink!=NULL);
	m_pIUserManagerSink->OnUserItemUpdate(pIClientUserItem,UserStatus);

	return true;
}

////更新段位
//bool __cdecl CPlazaUserManager::UpdateUserItemSegment(IClientUserItem * pIClientUserItem, tagUserSegment * const pUserSegment)
//{
//	//效验参数
//	ASSERT(pUserSegment!=NULL);
//	ASSERT(pIClientUserItem!=NULL);
//
//	//获取用户
//	tagClientUserInfo* pUserInfo=pIClientUserItem->GetUserInfo();
//
//	//以往数据
//	tagUserSegment UserSegment;
//	UserSegment.cbSegmentMode=pUserInfo->cbSegmentMode;
//	UserSegment.cbGameSegment=pUserInfo->cbGameSegment;
//	UserSegment.cbMasterSegment=pUserInfo->cbMasterSegment;
//
//	//设置数据
//	pUserInfo->cbSegmentMode=pUserSegment->cbSegmentMode;
//	pUserInfo->cbGameSegment=pUserSegment->cbGameSegment;
//	pUserInfo->cbMasterSegment=pUserSegment->cbMasterSegment;
//
//	//通知更新
//	ASSERT(m_pIUserManagerSink!=NULL);
//	m_pIUserManagerSink->OnUserItemUpdate(pIClientUserItem,UserSegment);
//
//	return true;
//}

//更新头像
bool CPlazaUserManager::UpdateUserCustomFace(IClientUserItem * pIClientUserItem, DWORD dwCustomID, tagCustomFaceInfo & CustomFaceInfo)
{
	//获取用户
	tagClientUserInfo* pUserInfo=pIClientUserItem->GetUserInfo();
	tagCustomFaceInfo * pCustomFaceInfo=pIClientUserItem->GetCustomFaceInfo();

	//设置变量
	pUserInfo->dwCustomID=dwCustomID;
	CopyMemory(pCustomFaceInfo,&CustomFaceInfo,sizeof(CustomFaceInfo));

	//通知更新
	ASSERT(m_pIUserManagerSink!=NULL);
	m_pIUserManagerSink->OnUserFaceUpdate(pIClientUserItem);

	return true;
}


//枚举用户
IClientUserItem * __cdecl CPlazaUserManager::EnumUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserItemActive.GetCount()) return NULL;
	return m_UserItemActive[wEnumIndex];
}

//查找用户
IClientUserItem * __cdecl CPlazaUserManager::SearchUserByUserID(DWORD dwUserID)
{
	//用户搜索
	for (INT_PTR i=0;i<m_UserItemActive.GetCount();i++)
	{
		CClientUserItem * pClientUserItem=m_UserItemActive[i];
		if (pClientUserItem->m_UserInfo.dwUserID==dwUserID) return pClientUserItem;
	}

	return NULL;
}

//查找用户
IClientUserItem * __cdecl CPlazaUserManager::SearchUserByGameID(DWORD dwGameID)
{
	//用户搜索
	for (INT_PTR i=0;i<m_UserItemActive.GetCount();i++)
	{
		CClientUserItem * pClientUserItem=m_UserItemActive[i];
		if (pClientUserItem->m_UserInfo.dwGameID==dwGameID) return pClientUserItem;
	}

	return NULL;
}

//查找用户
IClientUserItem * __cdecl CPlazaUserManager::SearchUserByAccounts(LPCTSTR pszAccounts)
{
	//变量定义
	CT2CW strAccounts(pszAccounts);

	//用户搜索
	for (INT_PTR i=0;i<m_UserItemActive.GetCount();i++)
	{
		CClientUserItem * pClientUserItem=m_UserItemActive[i];
		LPCWSTR pszTempAccounts=pClientUserItem->GetAccounts();
		if (wcscmp((LPCWSTR)strAccounts,pszTempAccounts)==0) return pClientUserItem;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameUserManager::CGameUserManager()
{
	//组件接口
	m_pIUserManagerSink=NULL;

	//用户数据
	ZeroMemory(m_pTableUserItem,sizeof(m_pTableUserItem));

	return;
}

//析构函数
CGameUserManager::~CGameUserManager()
{
	//旁观用户
	m_UserItemStorage.Append(m_UserItemLookon);

	//游戏用户
	for (WORD i=0;i<CountArray(m_pTableUserItem);i++)
	{
		if (m_pTableUserItem[i]!=NULL) m_UserItemStorage.Add(m_pTableUserItem[i]);
	}

	//释放资源
	for (INT_PTR i=0;i<m_UserItemStorage.GetCount();i++)
	{
		if (m_UserItemStorage[i]!=NULL) m_UserItemStorage[i]->Release();
	}

	//清理用户
	m_UserItemLookon.RemoveAll();

	return;
}

//接口查询
VOID * __cdecl CGameUserManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameUserManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameUserManager,Guid,dwQueryVer);
	return NULL;
}

//设置接口
bool __cdecl CGameUserManager::SetUserManagerSink(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if (pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IUserManagerSink)!=NULL);
		m_pIUserManagerSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IUserManagerSink);

		//成功判断
		if (m_pIUserManagerSink==NULL) return false;
	}
	else m_pIUserManagerSink=NULL;

	return true;
}

//接口查询
VOID * __cdecl CGameUserManager::GetUserManagerSink(REFGUID Guid, DWORD dwQueryVer)
{
	if (m_pIUserManagerSink==NULL) return NULL;
	return m_pIUserManagerSink->QueryInterface(Guid,dwQueryVer);
}

//增加用户
IClientUserItem * __cdecl CGameUserManager::ActiveUserItem(const tagClientUserInfo& UserInfo, tagCustomFaceInfo & CustomFaceInfo, BYTE cbCompanion, LPCWSTR pszUserNote)
{
	//效验状态
	ASSERT(UserInfo.wChairID<MAX_CHAIR);
	if (UserInfo.wChairID>=MAX_CHAIR) return NULL;

	//变量定义
	CClientUserItem * pClientUserItem=NULL;
	INT_PTR nStorageCount=m_UserItemStorage.GetCount();

	//获取用户
	if (nStorageCount>0)
	{
		pClientUserItem=m_UserItemStorage[nStorageCount-1];
		ASSERT(pClientUserItem!=NULL);
		m_UserItemStorage.RemoveAt(nStorageCount-1);
	}
	else
	{
		pClientUserItem=new CClientUserItem;
		ASSERT(pClientUserItem!=NULL);
		if (pClientUserItem==NULL) return false;
	}

	//设置用户
	if (UserInfo.cbUserStatus==US_LOOKON) m_UserItemLookon.Add(pClientUserItem);
	else m_pTableUserItem[UserInfo.wChairID]=pClientUserItem;

	//用户信息
	CopyMemory(&pClientUserItem->m_UserInfo,&UserInfo,sizeof(tagClientUserInfo));
	CopyMemory(&pClientUserItem->m_CustomFaceInfo,&CustomFaceInfo,sizeof(CustomFaceInfo));

	//附加信息
	pClientUserItem->m_cbCompanion=cbCompanion;
	lstrcpynW(pClientUserItem->m_szUserNote,pszUserNote,CountArray(pClientUserItem->m_szUserNote));

	//更新通知
	ASSERT(m_pIUserManagerSink!=NULL);
	if (m_pIUserManagerSink!=NULL) m_pIUserManagerSink->OnUserItemAcitve(pClientUserItem);

	return pClientUserItem;
}

//删除用户
bool __cdecl CGameUserManager::DeleteUserItem(IClientUserItem * pIClientUserItem)
{
	//效验状态
	ASSERT((pIClientUserItem!=NULL)&&(pIClientUserItem->GetChairID()<MAX_CHAIR));
	if ((pIClientUserItem==NULL)||(pIClientUserItem->GetChairID()>=MAX_CHAIR)) return false;

	//游戏用户
	if (pIClientUserItem->GetUserStatus()!=US_LOOKON)
	{
		//变量定义
		WORD wChairID=pIClientUserItem->GetChairID();
		CClientUserItem * pUserItemRemove=(CClientUserItem *)pIClientUserItem;

		//效验用户
		ASSERT(m_pTableUserItem[pIClientUserItem->GetChairID()]==pIClientUserItem);
		if (m_pTableUserItem[pIClientUserItem->GetChairID()]!=pIClientUserItem) return false;

		//删除用户
		m_pTableUserItem[wChairID]=NULL;
		m_UserItemStorage.Add(pUserItemRemove);

		//删除通知
		ASSERT(m_pIUserManagerSink!=NULL);
		m_pIUserManagerSink->OnUserItemDelete(pUserItemRemove);

		//设置数据
		pUserItemRemove->m_cbCompanion=CP_NORMAL;
		ZeroMemory(&pUserItemRemove->m_UserInfo,sizeof(tagClientUserInfo));

		return true;
	}

	//旁观用户
	CClientUserItem * pUserItemActive=NULL;
	for (INT_PTR i=0;i<m_UserItemLookon.GetCount();i++)
	{
		pUserItemActive=m_UserItemLookon[i];
		if (pIClientUserItem==pUserItemActive)
		{
			//删除用户
			m_UserItemLookon.RemoveAt(i);
			m_UserItemStorage.Add(pUserItemActive);

			//删除通知
			ASSERT(m_pIUserManagerSink!=NULL);
			m_pIUserManagerSink->OnUserItemDelete(pUserItemActive);

			//设置数据
			pUserItemActive->m_cbCompanion=CP_NORMAL;
			ZeroMemory(&pUserItemActive->m_UserInfo,sizeof(tagClientUserInfo));

			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//重置用户
bool __cdecl CGameUserManager::DeleteAllUserItem()
{
	//存储用户
	for (WORD i=0;i<MAX_CHAIR;i++)
	{
		if (m_pTableUserItem[i]!=NULL) m_UserItemStorage.Add(m_pTableUserItem[i]);
	}
	m_UserItemStorage.Append(m_UserItemLookon);

	//设置变量
	m_UserItemLookon.RemoveAll();
	ZeroMemory(m_pTableUserItem,sizeof(m_pTableUserItem));

	return true;
}

//更新积分
bool __cdecl CGameUserManager::UpdateUserItemScore(IClientUserItem * pIClientUserItem, tagClientUserScore * const pUserScore)
{
	//效验参数
	ASSERT(pUserScore!=NULL);
	ASSERT(pIClientUserItem!=NULL);

	//获取用户
	tagClientUserInfo* pUserInfo=pIClientUserItem->GetUserInfo();

	//以往数据
	tagClientUserScore UserScore;
	UserScore.lScore=pUserInfo->lScore;
	//UserScore.cbScoreKind=pUserInfo->cbScoreKind;
	UserScore.lWinCount=pUserInfo->lWinCount;
	UserScore.lLostCount=pUserInfo->lLostCount;
	UserScore.lDrawCount=pUserInfo->lDrawCount;
	UserScore.lFleeCount=pUserInfo->lFleeCount;
	UserScore.lExperience=pUserInfo->lExperience;

	//设置数据
	pUserInfo->lScore=pUserScore->lScore;
	//pUserInfo->cbScoreKind=pUserScore->cbScoreKind;
	pUserInfo->lWinCount=pUserScore->lWinCount;
	pUserInfo->lLostCount=pUserScore->lLostCount;
	pUserInfo->lDrawCount=pUserScore->lDrawCount;
	pUserInfo->lFleeCount=pUserScore->lFleeCount;
	pUserInfo->lExperience=pUserScore->lExperience;

	//通知更新
	ASSERT(m_pIUserManagerSink!=NULL);
	m_pIUserManagerSink->OnUserItemUpdate(pIClientUserItem,UserScore);

	return true;
}

//更新状态
bool __cdecl CGameUserManager::UpdateUserItemStatus(IClientUserItem * pIClientUserItem, tagUserStatus * const pUserStatus)
{
	//效验参数
	ASSERT(pUserStatus!=NULL);
	ASSERT(pIClientUserItem!=NULL);

	//获取用户
	tagClientUserInfo* pUserInfo=pIClientUserItem->GetUserInfo();

	//以往数据
	tagUserStatus UserStatus;
	UserStatus.wTableID=pUserInfo->wTableID;
	UserStatus.wChairID=pUserInfo->wChairID;
	UserStatus.cbUserStatus=pUserInfo->cbUserStatus;

	//设置数据
	pUserInfo->wTableID=pUserStatus->wTableID;
	pUserInfo->wChairID=pUserStatus->wChairID;
	pUserInfo->cbUserStatus=pUserStatus->cbUserStatus;

	//通知更新
	ASSERT(m_pIUserManagerSink!=NULL);
	m_pIUserManagerSink->OnUserItemUpdate(pIClientUserItem,UserStatus);

	return true;
}

////更新段位
//bool __cdecl CGameUserManager::UpdateUserItemSegment(IClientUserItem * pIClientUserItem, tagUserSegment * const pUserSegment)
//{
//	//效验参数
//	ASSERT(pUserSegment!=NULL);
//	ASSERT(pIClientUserItem!=NULL);
//
//	//获取用户
//	tagClientUserInfo * pUserInfo=pIClientUserItem->GetUserInfo();
//
//	//以往数据
//	tagUserSegment UserSegment;
//	UserSegment.cbSegmentMode=pUserInfo->cbSegmentMode;
//	UserSegment.cbGameSegment=pUserInfo->cbGameSegment;
//	UserSegment.cbMasterSegment=pUserInfo->cbMasterSegment;
//
//	//设置数据
//	pUserInfo->cbSegmentMode=pUserSegment->cbSegmentMode;
//	pUserInfo->cbGameSegment=pUserSegment->cbGameSegment;
//	pUserInfo->cbMasterSegment=pUserSegment->cbMasterSegment;
//
//	//通知更新
//	ASSERT(m_pIUserManagerSink!=NULL);
//	m_pIUserManagerSink->OnUserItemUpdate(pIClientUserItem,UserSegment);
//
//	return true;
//}

//更新头像
bool CGameUserManager::UpdateUserCustomFace(IClientUserItem * pIClientUserItem, DWORD dwCustomID, tagCustomFaceInfo & CustomFaceInfo)
{
	//获取用户
	tagClientUserInfo* pUserInfo=pIClientUserItem->GetUserInfo();
	tagCustomFaceInfo * pCustomFaceInfo=pIClientUserItem->GetCustomFaceInfo();

	//设置变量
	pUserInfo->dwCustomID=dwCustomID;
	CopyMemory(&pCustomFaceInfo,&CustomFaceInfo,sizeof(CustomFaceInfo));

	//通知更新
	ASSERT(m_pIUserManagerSink!=NULL);
	m_pIUserManagerSink->OnUserFaceUpdate(pIClientUserItem);

	return true;
}


//游戏用户
IClientUserItem * __cdecl CGameUserManager::GetTableUserItem(WORD wChariID)
{
	//效验参数
	ASSERT(wChariID<CountArray(m_pTableUserItem));
	if (wChariID>=CountArray(m_pTableUserItem)) return NULL;

	return m_pTableUserItem[wChariID];
}

//旁观用户
IClientUserItem * __cdecl CGameUserManager::EnumLookonUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserItemLookon.GetCount()) return NULL;
	return m_UserItemLookon[wEnumIndex];
}

//查找用户
IClientUserItem * __cdecl CGameUserManager::SearchUserByUserID(DWORD dwUserID)
{
	//游戏用户
	for (WORD i=0;i<MAX_CHAIR;i++)
	{
		CClientUserItem * pClientUserItem=m_pTableUserItem[i];
		if ((pClientUserItem!=NULL)&&(pClientUserItem->GetUserID()==dwUserID)) return pClientUserItem;
	}

	//旁观用户
	for (INT_PTR i=0;i<m_UserItemLookon.GetCount();i++)
	{
		CClientUserItem * pClientUserItem=m_UserItemLookon[i];
		if (pClientUserItem->GetUserID()==dwUserID) return pClientUserItem;
	}

	return NULL;
}

//查找用户
IClientUserItem * __cdecl CGameUserManager::SearchUserByGameID(DWORD dwGameID)
{
	//游戏用户
	for (WORD i=0;i<MAX_CHAIR;i++)
	{
		CClientUserItem * pClientUserItem=m_pTableUserItem[i];
		if ((pClientUserItem!=NULL)&&(pClientUserItem->GetGameID()==dwGameID)) return pClientUserItem;
	}

	//旁观用户
	for (INT_PTR i=0;i<m_UserItemLookon.GetCount();i++)
	{
		CClientUserItem * pClientUserItem=m_UserItemLookon[i];
		if (pClientUserItem->GetGameID()==dwGameID) return pClientUserItem;
	}

	return NULL;
}

//查找用户
IClientUserItem * __cdecl CGameUserManager::SearchUserByAccounts(LPCTSTR pszAccounts)
{
	//变量定义
	CT2CW strAccounts(pszAccounts);

	//游戏用户
	for (WORD i=0;i<MAX_CHAIR;i++)
	{
		//获取用户
		CClientUserItem * pClientUserItem=m_pTableUserItem[i];

		//用户判断
		if (pClientUserItem!=NULL)
		{
			LPCWSTR pszTempAccounts=pClientUserItem->GetAccounts();
			if (wcscmp((LPCWSTR)strAccounts,pszTempAccounts)==0) return pClientUserItem;
		}
	}

	//旁观用户
	for (INT_PTR i=0;i<m_UserItemLookon.GetCount();i++)
	{
		//获取用户
		CClientUserItem * pClientUserItem=m_UserItemLookon[i];

		//用户判断
		if (pClientUserItem!=NULL)
		{
			LPCWSTR pszTempAccounts=pClientUserItem->GetAccounts();
			if (wcscmp((LPCWSTR)strAccounts,pszTempAccounts)==0) return pClientUserItem;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(ClientUserItem);
DECLARE_CREATE_MODULE(GameUserManager);
DECLARE_CREATE_MODULE(PlazaUserManager);

//////////////////////////////////////////////////////////////////////////
