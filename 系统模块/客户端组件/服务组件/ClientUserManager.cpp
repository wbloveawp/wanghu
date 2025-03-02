#include "StdAfx.h"
#include "ClientUserManager.h"

//////////////////////////////////////////////////////////////////////////
//��̬����

CClientUserItemArray CPlazaUserManager::m_UserItemStorage;				//�洢����

//////////////////////////////////////////////////////////////////////////

//���캯��
CClientUserItem::CClientUserItem()
{
	//���ñ���
	m_cbCompanion=CP_NORMAL;
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	ZeroMemory(m_szUserNote,sizeof(m_szUserNote));

	return;
}

//��������
CClientUserItem::~CClientUserItem()
{
}

//�ӿڲ�ѯ
VOID * __cdecl CClientUserItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IClientUserItem,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IClientUserItem,Guid,dwQueryVer);
	return NULL;
}

//��Ҫ�ȼ�
LONG __cdecl CClientUserItem::GetImportOrder()
{
	//����ȼ�
	INT nOrder=0;
	if (m_cbCompanion==CP_FRIEND) nOrder+=1000;
	if (m_UserInfo.dwMemberPoint!=0) nOrder+=m_UserInfo.dwMemberPoint*100L;
	if (m_UserInfo.cbMasterOrder!=0) nOrder+=m_UserInfo.cbMasterOrder*10000L;

	return nOrder;
}

//������ֵ
LONG __cdecl CClientUserItem::GetUserExperience()
{
	return m_UserInfo.lExperience;
}

//��ȡ�ȼ�
WORD __cdecl CClientUserItem::GetExperienceLevel()
{
	//û�еȼ�
	if (m_UserInfo.lExperience<=0L) return 0;

	//��������
	LONG lIncrease=0L;
	LONG lLevelValue=0L;

	//�ȼ�����

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

//�ȼ���Ϣ
WORD __cdecl CClientUserItem::GetExperienceIndex(BYTE cbOrderIndex[], WORD wMaxCount)
{
	//��������
	WORD wIndexCount=0;
	WORD wLevelVaule[]={1L,5L,10L};

	//��ȡ�ȼ�
	WORD wUserLevel=GetExperienceLevel();

	//���õȼ�
	for (WORD i=0;i<wMaxCount;i++)
	{
		//��ȡ����
		for (INT j=0;j<CountArray(wLevelVaule);j++)
		{
			if (wUserLevel>=wLevelVaule[CountArray(wLevelVaule)-j-1])
			{
				//���ñ���
				wUserLevel-=wLevelVaule[CountArray(wLevelVaule)-j-1];
				cbOrderIndex[wIndexCount++]=CountArray(wLevelVaule)-j-1;

				break;
			}
		}

		//��ֹ�ж�
		if (wUserLevel==0L) break;
	}

	return wIndexCount;
}

//�û�ʤ��
float __cdecl CClientUserItem::GetUserWinRate()
{
	LONG lPlayCount=GetUserPlayCount();
	if (lPlayCount!=0L) return (float)(m_UserInfo.lWinCount*100.0f/(float)lPlayCount);

	return 0.0f;
}

//�û�����
float __cdecl CClientUserItem::GetUserLostRate()
{
	LONG lPlayCount=GetUserPlayCount();
	if (lPlayCount!=0L) return (float)(m_UserInfo.lLostCount*100.0f/(float)lPlayCount);

	return 0.0f;
}

//�û�����
float __cdecl CClientUserItem::GetUserDrawRate()
{
	LONG lPlayCount=GetUserPlayCount();
	if (lPlayCount!=0L) return (float)(m_UserInfo.lDrawCount*100.0f/(float)lPlayCount);

	return 0.0f;
}

//�û�����
float __cdecl CClientUserItem::GetUserFleeRate()
{
	LONG lPlayCount=GetUserPlayCount();
	if (lPlayCount!=0L) return (float)(m_UserInfo.lFleeCount*100.0f/(float)lPlayCount);

	return 0.0f;
}

//���ñ�ע
VOID __cdecl CClientUserItem::SetUserNoteInfo(LPCWSTR pszUserNote)
{
	//Ч�����
	ASSERT(pszUserNote!=NULL);
	if (pszUserNote==NULL) return;

	//���ñ�ע
	lstrcpynW(m_szUserNote,pszUserNote,CountArray(m_szUserNote));

	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CPlazaUserManager::CPlazaUserManager()
{
	//����ӿ�
	m_pIUserManagerSink=NULL;
	m_pIUserInformation=NULL;

	return;
}

//��������
CPlazaUserManager::~CPlazaUserManager()
{
	//�洢�û�
	m_UserItemStorage.Append(m_UserItemActive);

	//�ͷ���Դ
	for (INT_PTR i=0;i<m_UserItemStorage.GetCount();i++)
	{
		if (m_UserItemStorage[i]!=NULL) m_UserItemStorage[i]->Release();
	}

	//�����û�
	m_UserItemActive.RemoveAll();
	m_UserItemStorage.RemoveAll();

	return;
}

//�ӿڲ�ѯ
VOID * __cdecl CPlazaUserManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IPlazaUserManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IPlazaUserManager,Guid,dwQueryVer);
	return NULL;
}

//���ýӿ�
bool __cdecl CPlazaUserManager::SetUserInformation(IUnknownEx * pIUnknownEx)
{
	//���ýӿ�
	if (pIUnknownEx!=NULL)
	{
		//��ѯ�ӿ�
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IUserInformation)!=NULL);
		m_pIUserInformation=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IUserInformation);

		//�ɹ��ж�
		if (m_pIUserInformation==NULL) return false;
	}
	else m_pIUserInformation=NULL;

	return true;
}

//���ýӿ�
bool __cdecl CPlazaUserManager::SetUserManagerSink(IUnknownEx * pIUnknownEx)
{
	//���ýӿ�
	if (pIUnknownEx!=NULL)
	{
		//��ѯ�ӿ�
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IUserManagerSink)!=NULL);
		m_pIUserManagerSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IUserManagerSink);

		//�ɹ��ж�
		if (m_pIUserManagerSink==NULL) return false;
	}
	else m_pIUserManagerSink=NULL;

	return true;
}

//�����û�
IClientUserItem * __cdecl CPlazaUserManager::ActiveUserItem(const tagClientUserInfo& UserInfo, tagCustomFaceInfo & CustomFaceInfo)
{
	//��������
	CClientUserItem * pClientUserItem=NULL;
	INT_PTR nStorageCount=m_UserItemStorage.GetCount();

	//��ȡ�û�
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

	//�����û�
	ASSERT(pClientUserItem!=NULL);
	m_UserItemActive.Add(pClientUserItem);

	//��Ϣ����
	//ASSERT(m_pIUserInformation!=NULL);
	//tagCompanionInfo * pCompanionInfo=m_pIUserInformation->SearchCompanionInfo(UserInfo.dwUserID);

	////��ϵ����
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

	//��������
	CopyMemory(&pClientUserItem->m_UserInfo,&UserInfo,sizeof(tagClientUserInfo));
	CopyMemory(&pClientUserItem->m_CustomFaceInfo,&CustomFaceInfo,sizeof(CustomFaceInfo));

	//����֪ͨ
	ASSERT(m_pIUserManagerSink!=NULL);
	if (m_pIUserManagerSink!=NULL) m_pIUserManagerSink->OnUserItemAcitve(pClientUserItem);

	//��Ϣ����
	//if (pCompanionInfo!=NULL)
	//{
	//	//һ���ж�
	//	bool bSameData=(UserInfo.dwGameID==pCompanionInfo->dwGameID);
	//	if ((bSameData==true)&&(wcscmp(UserInfo.szAccounts,pCompanionInfo->szAccounts)!=0)) bSameData=false;

	//	//���´���
	//	if (bSameData==false) m_pIUserInformation->InsertCompanionInfo(pClientUserItem,pCompanionInfo->cbCompanion);
	//}

	return pClientUserItem;
}

//ɾ���û�
bool __cdecl CPlazaUserManager::DeleteUserItem(IClientUserItem * pIClientUserItem)
{
	//�����û�
	CClientUserItem * pUserItemActive=NULL;
	for (INT_PTR i=0;i<m_UserItemActive.GetCount();i++)
	{
		pUserItemActive=m_UserItemActive[i];
		if (pIClientUserItem==pUserItemActive)
		{
			//ɾ���û�
			m_UserItemActive.RemoveAt(i);
			m_UserItemStorage.Add(pUserItemActive);

			//ɾ��֪ͨ
			ASSERT(m_pIUserManagerSink!=NULL);
			m_pIUserManagerSink->OnUserItemDelete(pUserItemActive);

			//��������
			pUserItemActive->m_cbCompanion=CP_NORMAL;
			ZeroMemory(&pUserItemActive->m_UserInfo,sizeof(tagClientUserInfo));

			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//���»���
bool __cdecl CPlazaUserManager::UpdateUserItemScore(IClientUserItem * pIClientUserItem, tagClientUserScore * const pUserScore)
{
	//Ч�����
	ASSERT(pUserScore!=NULL);
	ASSERT(pIClientUserItem!=NULL);

	//��ȡ�û�
	tagClientUserInfo* pUserInfo=pIClientUserItem->GetUserInfo();

	//��������
	tagClientUserScore UserScore;
	UserScore.lScore=pUserInfo->lScore;
	//UserScore.cbScoreKind= pUserInfo->cbScoreKind;
	UserScore.lWinCount=pUserInfo->lWinCount;
	UserScore.lLostCount=pUserInfo->lLostCount;
	UserScore.lDrawCount=pUserInfo->lDrawCount;
	UserScore.lFleeCount=pUserInfo->lFleeCount;
	UserScore.lExperience=pUserInfo->lExperience;

	//��������
	pUserInfo->lScore=pUserScore->lScore;
	//pUserInfo->cbScoreKind= pUserScore->cbScoreKind;
	pUserInfo->lWinCount=pUserScore->lWinCount;
	pUserInfo->lLostCount=pUserScore->lLostCount;
	pUserInfo->lDrawCount=pUserScore->lDrawCount;
	pUserInfo->lFleeCount=pUserScore->lFleeCount;
	pUserInfo->lExperience=pUserScore->lExperience;

	//֪ͨ����
	ASSERT(m_pIUserManagerSink!=NULL);
	m_pIUserManagerSink->OnUserItemUpdate(pIClientUserItem,UserScore);

	return true;
}

//����״̬
bool __cdecl CPlazaUserManager::UpdateUserItemStatus(IClientUserItem * pIClientUserItem, tagUserStatus * const pUserStatus)
{
	//Ч�����
	ASSERT(pUserStatus!=NULL);
	ASSERT(pIClientUserItem!=NULL);

	//��ȡ�û�
	tagClientUserInfo* pUserInfo=pIClientUserItem->GetUserInfo();

	//��������
	tagUserStatus UserStatus;
	UserStatus.wTableID=pUserInfo->wTableID;
	UserStatus.wChairID=pUserInfo->wChairID;
	UserStatus.cbUserStatus=pUserInfo->cbUserStatus;

	//��������
	pUserInfo->wTableID=pUserStatus->wTableID;
	pUserInfo->wChairID=pUserStatus->wChairID;
	pUserInfo->cbUserStatus=pUserStatus->cbUserStatus;

	//֪ͨ����
	ASSERT(m_pIUserManagerSink!=NULL);
	m_pIUserManagerSink->OnUserItemUpdate(pIClientUserItem,UserStatus);

	return true;
}

////���¶�λ
//bool __cdecl CPlazaUserManager::UpdateUserItemSegment(IClientUserItem * pIClientUserItem, tagUserSegment * const pUserSegment)
//{
//	//Ч�����
//	ASSERT(pUserSegment!=NULL);
//	ASSERT(pIClientUserItem!=NULL);
//
//	//��ȡ�û�
//	tagClientUserInfo* pUserInfo=pIClientUserItem->GetUserInfo();
//
//	//��������
//	tagUserSegment UserSegment;
//	UserSegment.cbSegmentMode=pUserInfo->cbSegmentMode;
//	UserSegment.cbGameSegment=pUserInfo->cbGameSegment;
//	UserSegment.cbMasterSegment=pUserInfo->cbMasterSegment;
//
//	//��������
//	pUserInfo->cbSegmentMode=pUserSegment->cbSegmentMode;
//	pUserInfo->cbGameSegment=pUserSegment->cbGameSegment;
//	pUserInfo->cbMasterSegment=pUserSegment->cbMasterSegment;
//
//	//֪ͨ����
//	ASSERT(m_pIUserManagerSink!=NULL);
//	m_pIUserManagerSink->OnUserItemUpdate(pIClientUserItem,UserSegment);
//
//	return true;
//}

//����ͷ��
bool CPlazaUserManager::UpdateUserCustomFace(IClientUserItem * pIClientUserItem, DWORD dwCustomID, tagCustomFaceInfo & CustomFaceInfo)
{
	//��ȡ�û�
	tagClientUserInfo* pUserInfo=pIClientUserItem->GetUserInfo();
	tagCustomFaceInfo * pCustomFaceInfo=pIClientUserItem->GetCustomFaceInfo();

	//���ñ���
	pUserInfo->dwCustomID=dwCustomID;
	CopyMemory(pCustomFaceInfo,&CustomFaceInfo,sizeof(CustomFaceInfo));

	//֪ͨ����
	ASSERT(m_pIUserManagerSink!=NULL);
	m_pIUserManagerSink->OnUserFaceUpdate(pIClientUserItem);

	return true;
}


//ö���û�
IClientUserItem * __cdecl CPlazaUserManager::EnumUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserItemActive.GetCount()) return NULL;
	return m_UserItemActive[wEnumIndex];
}

//�����û�
IClientUserItem * __cdecl CPlazaUserManager::SearchUserByUserID(DWORD dwUserID)
{
	//�û�����
	for (INT_PTR i=0;i<m_UserItemActive.GetCount();i++)
	{
		CClientUserItem * pClientUserItem=m_UserItemActive[i];
		if (pClientUserItem->m_UserInfo.dwUserID==dwUserID) return pClientUserItem;
	}

	return NULL;
}

//�����û�
IClientUserItem * __cdecl CPlazaUserManager::SearchUserByGameID(DWORD dwGameID)
{
	//�û�����
	for (INT_PTR i=0;i<m_UserItemActive.GetCount();i++)
	{
		CClientUserItem * pClientUserItem=m_UserItemActive[i];
		if (pClientUserItem->m_UserInfo.dwGameID==dwGameID) return pClientUserItem;
	}

	return NULL;
}

//�����û�
IClientUserItem * __cdecl CPlazaUserManager::SearchUserByAccounts(LPCTSTR pszAccounts)
{
	//��������
	CT2CW strAccounts(pszAccounts);

	//�û�����
	for (INT_PTR i=0;i<m_UserItemActive.GetCount();i++)
	{
		CClientUserItem * pClientUserItem=m_UserItemActive[i];
		LPCWSTR pszTempAccounts=pClientUserItem->GetAccounts();
		if (wcscmp((LPCWSTR)strAccounts,pszTempAccounts)==0) return pClientUserItem;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameUserManager::CGameUserManager()
{
	//����ӿ�
	m_pIUserManagerSink=NULL;

	//�û�����
	ZeroMemory(m_pTableUserItem,sizeof(m_pTableUserItem));

	return;
}

//��������
CGameUserManager::~CGameUserManager()
{
	//�Թ��û�
	m_UserItemStorage.Append(m_UserItemLookon);

	//��Ϸ�û�
	for (WORD i=0;i<CountArray(m_pTableUserItem);i++)
	{
		if (m_pTableUserItem[i]!=NULL) m_UserItemStorage.Add(m_pTableUserItem[i]);
	}

	//�ͷ���Դ
	for (INT_PTR i=0;i<m_UserItemStorage.GetCount();i++)
	{
		if (m_UserItemStorage[i]!=NULL) m_UserItemStorage[i]->Release();
	}

	//�����û�
	m_UserItemLookon.RemoveAll();

	return;
}

//�ӿڲ�ѯ
VOID * __cdecl CGameUserManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameUserManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameUserManager,Guid,dwQueryVer);
	return NULL;
}

//���ýӿ�
bool __cdecl CGameUserManager::SetUserManagerSink(IUnknownEx * pIUnknownEx)
{
	//���ýӿ�
	if (pIUnknownEx!=NULL)
	{
		//��ѯ�ӿ�
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IUserManagerSink)!=NULL);
		m_pIUserManagerSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IUserManagerSink);

		//�ɹ��ж�
		if (m_pIUserManagerSink==NULL) return false;
	}
	else m_pIUserManagerSink=NULL;

	return true;
}

//�ӿڲ�ѯ
VOID * __cdecl CGameUserManager::GetUserManagerSink(REFGUID Guid, DWORD dwQueryVer)
{
	if (m_pIUserManagerSink==NULL) return NULL;
	return m_pIUserManagerSink->QueryInterface(Guid,dwQueryVer);
}

//�����û�
IClientUserItem * __cdecl CGameUserManager::ActiveUserItem(const tagClientUserInfo& UserInfo, tagCustomFaceInfo & CustomFaceInfo, BYTE cbCompanion, LPCWSTR pszUserNote)
{
	//Ч��״̬
	ASSERT(UserInfo.wChairID<MAX_CHAIR);
	if (UserInfo.wChairID>=MAX_CHAIR) return NULL;

	//��������
	CClientUserItem * pClientUserItem=NULL;
	INT_PTR nStorageCount=m_UserItemStorage.GetCount();

	//��ȡ�û�
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

	//�����û�
	if (UserInfo.cbUserStatus==US_LOOKON) m_UserItemLookon.Add(pClientUserItem);
	else m_pTableUserItem[UserInfo.wChairID]=pClientUserItem;

	//�û���Ϣ
	CopyMemory(&pClientUserItem->m_UserInfo,&UserInfo,sizeof(tagClientUserInfo));
	CopyMemory(&pClientUserItem->m_CustomFaceInfo,&CustomFaceInfo,sizeof(CustomFaceInfo));

	//������Ϣ
	pClientUserItem->m_cbCompanion=cbCompanion;
	lstrcpynW(pClientUserItem->m_szUserNote,pszUserNote,CountArray(pClientUserItem->m_szUserNote));

	//����֪ͨ
	ASSERT(m_pIUserManagerSink!=NULL);
	if (m_pIUserManagerSink!=NULL) m_pIUserManagerSink->OnUserItemAcitve(pClientUserItem);

	return pClientUserItem;
}

//ɾ���û�
bool __cdecl CGameUserManager::DeleteUserItem(IClientUserItem * pIClientUserItem)
{
	//Ч��״̬
	ASSERT((pIClientUserItem!=NULL)&&(pIClientUserItem->GetChairID()<MAX_CHAIR));
	if ((pIClientUserItem==NULL)||(pIClientUserItem->GetChairID()>=MAX_CHAIR)) return false;

	//��Ϸ�û�
	if (pIClientUserItem->GetUserStatus()!=US_LOOKON)
	{
		//��������
		WORD wChairID=pIClientUserItem->GetChairID();
		CClientUserItem * pUserItemRemove=(CClientUserItem *)pIClientUserItem;

		//Ч���û�
		ASSERT(m_pTableUserItem[pIClientUserItem->GetChairID()]==pIClientUserItem);
		if (m_pTableUserItem[pIClientUserItem->GetChairID()]!=pIClientUserItem) return false;

		//ɾ���û�
		m_pTableUserItem[wChairID]=NULL;
		m_UserItemStorage.Add(pUserItemRemove);

		//ɾ��֪ͨ
		ASSERT(m_pIUserManagerSink!=NULL);
		m_pIUserManagerSink->OnUserItemDelete(pUserItemRemove);

		//��������
		pUserItemRemove->m_cbCompanion=CP_NORMAL;
		ZeroMemory(&pUserItemRemove->m_UserInfo,sizeof(tagClientUserInfo));

		return true;
	}

	//�Թ��û�
	CClientUserItem * pUserItemActive=NULL;
	for (INT_PTR i=0;i<m_UserItemLookon.GetCount();i++)
	{
		pUserItemActive=m_UserItemLookon[i];
		if (pIClientUserItem==pUserItemActive)
		{
			//ɾ���û�
			m_UserItemLookon.RemoveAt(i);
			m_UserItemStorage.Add(pUserItemActive);

			//ɾ��֪ͨ
			ASSERT(m_pIUserManagerSink!=NULL);
			m_pIUserManagerSink->OnUserItemDelete(pUserItemActive);

			//��������
			pUserItemActive->m_cbCompanion=CP_NORMAL;
			ZeroMemory(&pUserItemActive->m_UserInfo,sizeof(tagClientUserInfo));

			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//�����û�
bool __cdecl CGameUserManager::DeleteAllUserItem()
{
	//�洢�û�
	for (WORD i=0;i<MAX_CHAIR;i++)
	{
		if (m_pTableUserItem[i]!=NULL) m_UserItemStorage.Add(m_pTableUserItem[i]);
	}
	m_UserItemStorage.Append(m_UserItemLookon);

	//���ñ���
	m_UserItemLookon.RemoveAll();
	ZeroMemory(m_pTableUserItem,sizeof(m_pTableUserItem));

	return true;
}

//���»���
bool __cdecl CGameUserManager::UpdateUserItemScore(IClientUserItem * pIClientUserItem, tagClientUserScore * const pUserScore)
{
	//Ч�����
	ASSERT(pUserScore!=NULL);
	ASSERT(pIClientUserItem!=NULL);

	//��ȡ�û�
	tagClientUserInfo* pUserInfo=pIClientUserItem->GetUserInfo();

	//��������
	tagClientUserScore UserScore;
	UserScore.lScore=pUserInfo->lScore;
	//UserScore.cbScoreKind=pUserInfo->cbScoreKind;
	UserScore.lWinCount=pUserInfo->lWinCount;
	UserScore.lLostCount=pUserInfo->lLostCount;
	UserScore.lDrawCount=pUserInfo->lDrawCount;
	UserScore.lFleeCount=pUserInfo->lFleeCount;
	UserScore.lExperience=pUserInfo->lExperience;

	//��������
	pUserInfo->lScore=pUserScore->lScore;
	//pUserInfo->cbScoreKind=pUserScore->cbScoreKind;
	pUserInfo->lWinCount=pUserScore->lWinCount;
	pUserInfo->lLostCount=pUserScore->lLostCount;
	pUserInfo->lDrawCount=pUserScore->lDrawCount;
	pUserInfo->lFleeCount=pUserScore->lFleeCount;
	pUserInfo->lExperience=pUserScore->lExperience;

	//֪ͨ����
	ASSERT(m_pIUserManagerSink!=NULL);
	m_pIUserManagerSink->OnUserItemUpdate(pIClientUserItem,UserScore);

	return true;
}

//����״̬
bool __cdecl CGameUserManager::UpdateUserItemStatus(IClientUserItem * pIClientUserItem, tagUserStatus * const pUserStatus)
{
	//Ч�����
	ASSERT(pUserStatus!=NULL);
	ASSERT(pIClientUserItem!=NULL);

	//��ȡ�û�
	tagClientUserInfo* pUserInfo=pIClientUserItem->GetUserInfo();

	//��������
	tagUserStatus UserStatus;
	UserStatus.wTableID=pUserInfo->wTableID;
	UserStatus.wChairID=pUserInfo->wChairID;
	UserStatus.cbUserStatus=pUserInfo->cbUserStatus;

	//��������
	pUserInfo->wTableID=pUserStatus->wTableID;
	pUserInfo->wChairID=pUserStatus->wChairID;
	pUserInfo->cbUserStatus=pUserStatus->cbUserStatus;

	//֪ͨ����
	ASSERT(m_pIUserManagerSink!=NULL);
	m_pIUserManagerSink->OnUserItemUpdate(pIClientUserItem,UserStatus);

	return true;
}

////���¶�λ
//bool __cdecl CGameUserManager::UpdateUserItemSegment(IClientUserItem * pIClientUserItem, tagUserSegment * const pUserSegment)
//{
//	//Ч�����
//	ASSERT(pUserSegment!=NULL);
//	ASSERT(pIClientUserItem!=NULL);
//
//	//��ȡ�û�
//	tagClientUserInfo * pUserInfo=pIClientUserItem->GetUserInfo();
//
//	//��������
//	tagUserSegment UserSegment;
//	UserSegment.cbSegmentMode=pUserInfo->cbSegmentMode;
//	UserSegment.cbGameSegment=pUserInfo->cbGameSegment;
//	UserSegment.cbMasterSegment=pUserInfo->cbMasterSegment;
//
//	//��������
//	pUserInfo->cbSegmentMode=pUserSegment->cbSegmentMode;
//	pUserInfo->cbGameSegment=pUserSegment->cbGameSegment;
//	pUserInfo->cbMasterSegment=pUserSegment->cbMasterSegment;
//
//	//֪ͨ����
//	ASSERT(m_pIUserManagerSink!=NULL);
//	m_pIUserManagerSink->OnUserItemUpdate(pIClientUserItem,UserSegment);
//
//	return true;
//}

//����ͷ��
bool CGameUserManager::UpdateUserCustomFace(IClientUserItem * pIClientUserItem, DWORD dwCustomID, tagCustomFaceInfo & CustomFaceInfo)
{
	//��ȡ�û�
	tagClientUserInfo* pUserInfo=pIClientUserItem->GetUserInfo();
	tagCustomFaceInfo * pCustomFaceInfo=pIClientUserItem->GetCustomFaceInfo();

	//���ñ���
	pUserInfo->dwCustomID=dwCustomID;
	CopyMemory(&pCustomFaceInfo,&CustomFaceInfo,sizeof(CustomFaceInfo));

	//֪ͨ����
	ASSERT(m_pIUserManagerSink!=NULL);
	m_pIUserManagerSink->OnUserFaceUpdate(pIClientUserItem);

	return true;
}


//��Ϸ�û�
IClientUserItem * __cdecl CGameUserManager::GetTableUserItem(WORD wChariID)
{
	//Ч�����
	ASSERT(wChariID<CountArray(m_pTableUserItem));
	if (wChariID>=CountArray(m_pTableUserItem)) return NULL;

	return m_pTableUserItem[wChariID];
}

//�Թ��û�
IClientUserItem * __cdecl CGameUserManager::EnumLookonUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserItemLookon.GetCount()) return NULL;
	return m_UserItemLookon[wEnumIndex];
}

//�����û�
IClientUserItem * __cdecl CGameUserManager::SearchUserByUserID(DWORD dwUserID)
{
	//��Ϸ�û�
	for (WORD i=0;i<MAX_CHAIR;i++)
	{
		CClientUserItem * pClientUserItem=m_pTableUserItem[i];
		if ((pClientUserItem!=NULL)&&(pClientUserItem->GetUserID()==dwUserID)) return pClientUserItem;
	}

	//�Թ��û�
	for (INT_PTR i=0;i<m_UserItemLookon.GetCount();i++)
	{
		CClientUserItem * pClientUserItem=m_UserItemLookon[i];
		if (pClientUserItem->GetUserID()==dwUserID) return pClientUserItem;
	}

	return NULL;
}

//�����û�
IClientUserItem * __cdecl CGameUserManager::SearchUserByGameID(DWORD dwGameID)
{
	//��Ϸ�û�
	for (WORD i=0;i<MAX_CHAIR;i++)
	{
		CClientUserItem * pClientUserItem=m_pTableUserItem[i];
		if ((pClientUserItem!=NULL)&&(pClientUserItem->GetGameID()==dwGameID)) return pClientUserItem;
	}

	//�Թ��û�
	for (INT_PTR i=0;i<m_UserItemLookon.GetCount();i++)
	{
		CClientUserItem * pClientUserItem=m_UserItemLookon[i];
		if (pClientUserItem->GetGameID()==dwGameID) return pClientUserItem;
	}

	return NULL;
}

//�����û�
IClientUserItem * __cdecl CGameUserManager::SearchUserByAccounts(LPCTSTR pszAccounts)
{
	//��������
	CT2CW strAccounts(pszAccounts);

	//��Ϸ�û�
	for (WORD i=0;i<MAX_CHAIR;i++)
	{
		//��ȡ�û�
		CClientUserItem * pClientUserItem=m_pTableUserItem[i];

		//�û��ж�
		if (pClientUserItem!=NULL)
		{
			LPCWSTR pszTempAccounts=pClientUserItem->GetAccounts();
			if (wcscmp((LPCWSTR)strAccounts,pszTempAccounts)==0) return pClientUserItem;
		}
	}

	//�Թ��û�
	for (INT_PTR i=0;i<m_UserItemLookon.GetCount();i++)
	{
		//��ȡ�û�
		CClientUserItem * pClientUserItem=m_UserItemLookon[i];

		//�û��ж�
		if (pClientUserItem!=NULL)
		{
			LPCWSTR pszTempAccounts=pClientUserItem->GetAccounts();
			if (wcscmp((LPCWSTR)strAccounts,pszTempAccounts)==0) return pClientUserItem;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(ClientUserItem);
DECLARE_CREATE_MODULE(GameUserManager);
DECLARE_CREATE_MODULE(PlazaUserManager);

//////////////////////////////////////////////////////////////////////////
