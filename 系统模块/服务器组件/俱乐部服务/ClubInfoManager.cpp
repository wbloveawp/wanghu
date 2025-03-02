#include "StdAfx.h"
#include "ClubInfoManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////
//��̬��Ա
CClubTableArray CClubInfoItem::m_ClubTableBuffer;
CClubMemberArray CClubInfoItem::m_ClubMemberBuffer;
CKindSettingArray CClubInfoItem::m_KindSettingBuffer;
CBattleConfigArray CClubInfoItem::m_BattleConfigBuffer;
CApplyMessageArray CClubInfoItem::m_ApplyMessageBuffer;

//////////////////////////////////////////////////////////////////////////////////
//���캯��
CClubInfoItem::CClubInfoItem()
{
	//���ñ���
	m_dwActiveTime = 0;
	m_wOnlineCount = 0;
	m_bUnloadFlag = false;
	m_cbClubState=CLUB_STATE_OFFLINE;

	//��ʼ���ֵ�
	m_MapTableID.InitHashTable(13);
	m_MapMemberID.InitHashTable(503);
	m_MapMessageID.InitHashTable(53);

	//���ñ���	
	m_BattleConfigActive.RemoveAll();
	ZeroMemory(&m_ClubItem,sizeof(m_ClubItem));	
}

//��������
CClubInfoItem::~CClubInfoItem()
{
	//���ñ���	
	ResetClubItem();
}

//ö������
tagSocketInfo* CClubInfoItem::EnumEnterRequest(WORD wIndex)
{
	if (wIndex >= m_SocketInfoArray.GetCount())
	{
		return NULL;
	}

	return &m_SocketInfoArray[wIndex];
}

//��������
VOID CClubInfoItem::CacheEnterRequest(tagSocketInfo& SocketInfo)
{
	if (SearchEnterRequest(SocketInfo) == -1)
	{
		m_SocketInfoArray.Add(SocketInfo);
	}
}

//�Ƴ�����
VOID CClubInfoItem::RemoveEnterRequest(tagSocketInfo& SocketInfo)
{
	auto nIndex = SearchEnterRequest(SocketInfo);
	if (nIndex != -1)
	{
		m_SocketInfoArray.RemoveAt(nIndex);
	}
}

//�Ƴ���Ա
VOID CClubInfoItem::RemoveMember()
{
	//�����Ա
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

//�Ƴ���Ա
bool CClubInfoItem::RemoveMember(DWORD dwMemberID)
{
	//��ѯ��Ա
	tagClubMemberInfo * pClubMember = SearchMember(dwMemberID);
	if (pClubMember!=NULL)
	{
		//�Ƴ�Ԫ��
		m_MapMemberID.RemoveKey(dwMemberID);

		//�Ƴ�����
		RemoveClubMember(pClubMember);
	}

	return true;
}

//��ӳ�Ա
bool CClubInfoItem::AppendMember(tagClubMemberInfo & ClubMember)
{
	//���ҳ�Ա
	tagClubMemberInfo *pClubMember=SearchMember(ClubMember.dwMemberID);
	if (pClubMember!=NULL)
	{
		//������Ϣ
		CopyMemory(pClubMember,&ClubMember,sizeof(ClubMember));
	}
	else
	{
		//�����Ա
		pClubMember = ActiveClubMember();
		if (pClubMember == NULL) return false;

		//������Ϣ
		CopyMemory(pClubMember, &ClubMember, sizeof(ClubMember));

		//ӳ���Ա
		m_MapMemberID[ClubMember.dwMemberID] = pClubMember;
	}

	return true;
}

//ö�ٳ�Ա
tagClubMemberInfo * CClubInfoItem::EnumMember(POSITION & Position)
{
	//��������
	DWORD dwMemberID = 0L;
	tagClubMemberInfo * pClubMember = NULL;

	//��ȡ����
	if (Position == NULL) Position = m_MapMemberID.GetStartPosition();
	if (Position != NULL) m_MapMemberID.GetNextAssoc(Position, dwMemberID, pClubMember);

	return pClubMember;
}

//���ҳ�Ա
tagClubMemberInfo * CClubInfoItem::SearchMember(DWORD dwMemberID)
{
	//��������
	tagClubMemberInfo * pClubMember = NULL;
	if (m_MapMemberID.Lookup(dwMemberID, pClubMember))
	{
		return pClubMember;
	}

	return NULL;
}

//�Ƴ��淨
VOID CClubInfoItem::RemoveConfig()
{
	//Լս�淨
	for (INT_PTR i = 0; i < m_BattleConfigActive.GetCount(); i++)
	{
		RemoveBattleConfig(m_BattleConfigActive[i]);
	}
	m_BattleConfigActive.RemoveAll();
}

//�Ƴ��淨
bool CClubInfoItem::RemoveConfig(DWORD dwConfigID)
{
	//���Ҳ��Ƴ�
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

//����淨
bool CClubInfoItem::AppendConfig(tagClubBattleConfig & BattleConfig)
{
	//���ҳ�Ա
	auto  pIMBattleConfig=SearchConfig(BattleConfig.dwConfigID);
	if (pIMBattleConfig !=NULL)
	{
		//������Ϣ
		CopyMemory(pIMBattleConfig,&BattleConfig,sizeof(BattleConfig));
	}
	else
	{
		//�����淨
		pIMBattleConfig = ActiveBattleConfig();
		if (pIMBattleConfig == NULL) return false;

		//��������
		CopyMemory(pIMBattleConfig, &BattleConfig, sizeof(BattleConfig));

		//����淨
		m_BattleConfigActive.Add(pIMBattleConfig);
	}

	return true;
}

//ö���淨
tagClubBattleConfigEx * CClubInfoItem::EnumConfig(INT nIndex)
{
	if (nIndex>= m_BattleConfigActive.GetCount()) return NULL;
	return m_BattleConfigActive[nIndex];
}

//�����淨
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

//�Ƴ�����
VOID CClubInfoItem::RemoveKindSetting()
{
	//Լս�淨
	for (INT_PTR i = 0; i < m_KindSettingActive.GetCount(); i++)
	{
		RemoveKindSetting(m_KindSettingActive[i]);
	}
	m_KindSettingActive.RemoveAll();
}

//�Ƴ�����
VOID CClubInfoItem::RemoveKindSetting(WORD wKindID)
{
	//���Ҳ��Ƴ�
	for (INT_PTR i = 0; i < m_KindSettingActive.GetCount(); i++)
	{
		if (m_KindSettingActive[i]->wKindID == wKindID)
		{
			m_KindSettingActive.RemoveAt(i);
			return;
		}
	}	
}

//�������
bool CClubInfoItem::AppendKindSetting(tagClubKindSetting& KindSetting)
{
	//���ҳ�Ա
	auto  pKindSetting = SearchKindSetting(KindSetting.wKindID);
	if (pKindSetting != NULL)
	{
		//������Ϣ
		CopyMemory(pKindSetting, &KindSetting, sizeof(KindSetting));
	}
	else
	{
		//�����淨
		pKindSetting = ActiveKindSetting();
		if (pKindSetting == NULL) return false;

		//��������
		CopyMemory(pKindSetting, &KindSetting, sizeof(KindSetting));

		//����淨
		m_KindSettingActive.Add(pKindSetting);
	}

	return true;
}

//ö������
tagClubKindSetting* CClubInfoItem::EnumKindSetting(INT nIndex)
{
	if (nIndex >= m_KindSettingActive.GetCount()) return NULL;
	return m_KindSettingActive[nIndex];
}

//��������
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

//�Ƴ�����
VOID CClubInfoItem::RemoveClubTable()
{
	//��������
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

//�Ƴ�����
VOID CClubInfoItem::RemoveClubTable(DWORD dwBattleNum)
{	
	//��������
	auto pClubTableItem = SearchClubTable(dwBattleNum);
	if (pClubTableItem != NULL)
	{
		//�Ƴ�����
		m_MapTableID.RemoveKey(dwBattleNum);

		//��������
		RemoveClubTable(pClubTableItem);
	}
}

//�������
bool CClubInfoItem::AppendClubTable(tagClubTableItem& TableItem)
{
	//���ҳ�Ա
	auto pClubTableItem = SearchClubTable(TableItem.TableItem.dwBattleNum);
	if (pClubTableItem != NULL)
	{
		//������Ϣ
		CopyMemory(pClubTableItem, &TableItem, sizeof(TableItem));

		return false;
	}
	else
	{
		//��������
		pClubTableItem = ActiveClubTable();
		if (pClubTableItem == NULL) return false;

		//������Ϣ
		CopyMemory(pClubTableItem, &TableItem, sizeof(TableItem));

		//ӳ����Ϣ
		m_MapTableID[TableItem.TableItem.dwBattleNum] = pClubTableItem;		
	}

	return true;
}

//ö������
tagClubTableItem* CClubInfoItem::EnumClubTable(POSITION& Position)
{
	//��������
	DWORD dwTableID = 0L;
	tagClubTableItem * pClubTable = NULL;

	//��ȡ����
	if (Position == NULL) Position = m_MapTableID.GetStartPosition();
	if (Position != NULL) m_MapTableID.GetNextAssoc(Position, dwTableID, pClubTable);

	return pClubTable;
}

//��������
tagClubTableItem* CClubInfoItem::SearchClubTable(DWORD dwBattleNum)
{
	//��������
	tagClubTableItem * pClubTable = NULL;
	if (m_MapTableID.Lookup(dwBattleNum, pClubTable))
	{
		return pClubTable;
	}

	return NULL;
}

//�Ƴ���Ϣ
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

//�Ƴ���Ϣ
bool CClubInfoItem::RemoveMessage(DWORD dwMessageID)
{
	//��ѯ��Ա
	tagClubApplyMessage * pIMApplyMessage = SearchMessage(dwMessageID);
	if (pIMApplyMessage != NULL)
	{
		//�Ƴ�Ԫ��
		m_MapMessageID.RemoveKey(dwMessageID);

		//�Ƴ�����
		RemoveApplyMessage(pIMApplyMessage);
	}

	return true;
}

//�����Ϣ
bool CClubInfoItem::AppendMessage(tagClubApplyMessage & ApplyMessage)
{
	//���ҳ�Ա
	tagClubApplyMessage * pIMApplyMessage = SearchMessage(ApplyMessage.dwMessageID);
	if (pIMApplyMessage != NULL)
	{
		//������Ϣ
		CopyMemory(pIMApplyMessage, &ApplyMessage, sizeof(ApplyMessage));
	}
	else
	{
		//�����Ա
		pIMApplyMessage = ActiveApplyMessage();
		if (pIMApplyMessage == NULL) return false;

		//������Ϣ
		CopyMemory(pIMApplyMessage, &ApplyMessage, sizeof(ApplyMessage));

		//ӳ����Ϣ
		m_MapMessageID[ApplyMessage.dwMessageID] = pIMApplyMessage;
	}

	return true;
}

//ö����Ϣ
tagClubApplyMessage * CClubInfoItem::EnumMessage(POSITION & Position)
{
	//��������
	DWORD dwMessageID= 0L;
	tagClubApplyMessage * pIMApplyMessage= NULL;

	//��ȡ����
	if (Position == NULL) Position = m_MapMessageID.GetStartPosition();
	if (Position != NULL) m_MapMessageID.GetNextAssoc(Position, dwMessageID, pIMApplyMessage);

	return pIMApplyMessage;
}

//������Ϣ
tagClubApplyMessage * CClubInfoItem::SearchMessage(DWORD dwMessageID)
{
	//��������
	tagClubApplyMessage * pIMApplyMessage=NULL;
	if (m_MapMessageID.Lookup(dwMessageID, pIMApplyMessage))
	{
		return pIMApplyMessage;
	}

	return NULL;
}

//��������
tagClubTableItem* CClubInfoItem::ActiveClubTable()
{
	//��������
	tagClubTableItem* pClubTable= NULL;

	//���һ���
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

//�Ƴ�����
VOID CClubInfoItem::RemoveClubTable(tagClubTableItem* pClubTable)
{
	//��������
	ZeroMemory(pClubTable, sizeof(tagClubTableItem));

	//���뻺��
	m_ClubTableBuffer.Add(pClubTable);
}

//������Ա
tagClubMemberInfo * CClubInfoItem::ActiveClubMember()
{
	//��������
	tagClubMemberInfo * pClubMember = NULL;

	//���һ���
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

//�Ƴ���Ա
VOID CClubInfoItem::RemoveClubMember(tagClubMemberInfo * pClubMember)
{
	//��������
	ZeroMemory(pClubMember,sizeof(tagClubMemberInfo));

	//���뻺��
	m_ClubMemberBuffer.Add(pClubMember);
}

//�����淨
tagClubBattleConfigEx * CClubInfoItem::ActiveBattleConfig()
{
	//��������
	tagClubBattleConfigEx * pIMBattleConfig = NULL;

	//���һ���
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

//�Ƴ��淨
VOID CClubInfoItem::RemoveBattleConfig(tagClubBattleConfigEx* pIMBattleConfig)
{
	//��������
	ZeroMemory(pIMBattleConfig, sizeof(tagClubBattleConfigEx));

	//���뻺��
	m_BattleConfigBuffer.Add(pIMBattleConfig);
}

//��������
tagClubKindSetting* CClubInfoItem::ActiveKindSetting()
{
	//��������
	tagClubKindSetting * pKindSetting= NULL;

	//���һ���
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

//�Ƴ�����
VOID CClubInfoItem::RemoveKindSetting(tagClubKindSetting* pClubKindSetting)
{
	//��������
	ZeroMemory(pClubKindSetting, sizeof(pClubKindSetting));

	//���뻺��
	m_KindSettingBuffer.Add(pClubKindSetting);
}

//������Ϣ
tagClubApplyMessage * CClubInfoItem::ActiveApplyMessage()
{
	//��������
	tagClubApplyMessage * pIMApplyMessage = NULL;

	//���һ���
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

//�Ƴ���Ϣ
VOID CClubInfoItem::RemoveApplyMessage(tagClubApplyMessage * pIMApplyMessage)
{
	//��������
	ZeroMemory(pIMApplyMessage, sizeof(tagClubApplyMessage));

	//���뻺��
	m_ApplyMessageBuffer.Add(pIMApplyMessage);
}

//���ö���
void CClubInfoItem::ResetClubItem()
{
	//״̬����
	m_dwActiveTime = 0;
	m_wOnlineCount = 0;
	m_bUnloadFlag = false;
	m_cbClubState = CLUB_STATE_OFFLINE;

	//�Ƴ���Ա
	RemoveMember();

	//�Ƴ���Ϣ
	RemoveMessage();

	//�Ƴ��淨
	RemoveConfig();

	//�Ƴ�����
	RemoveClubTable();

	//�Ƴ�����
	RemoveKindSetting();

	//�Ƴ�����
	m_SocketInfoArray.RemoveAll();	

	//���ñ���		
	ZeroMemory(&m_ClubItem, sizeof(m_ClubItem));		
}

//��������
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

//��������
VOID CClubInfoItem::CleanStaticData()
{
	//��������
	for (INT_PTR i = 0; i < m_ClubTableBuffer.GetCount(); i++)
	{
		SafeDelete(m_ClubTableBuffer[i]);
	}

	//��ݳ�Ա
	for (INT_PTR i = 0; i < m_ClubMemberBuffer.GetCount(); i++)
	{
		SafeDelete(m_ClubMemberBuffer[i]);
	}

	//Լս�淨
	for (INT_PTR i = 0; i < m_BattleConfigBuffer.GetCount(); i++)
	{
		SafeDelete(m_BattleConfigBuffer[i]);
	}

	//������Ϣ
	for (INT_PTR i = 0; i < m_ApplyMessageBuffer.GetCount(); i++)
	{
		SafeDelete(m_ApplyMessageBuffer[i]);
	}
	
	//��������
	for (INT_PTR i = 0; i < m_KindSettingBuffer.GetCount(); i++)
	{
		SafeDelete(m_KindSettingBuffer[i]);
	}

	//�Ƴ�Ԫ��
	m_ClubTableBuffer.RemoveAll();
	m_ClubMemberBuffer.RemoveAll();
	m_KindSettingBuffer.RemoveAll();
	m_BattleConfigBuffer.RemoveAll();
	m_ApplyMessageBuffer.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////////
//���캯��
CClubInfoManager::CClubInfoManager()
{
	//��������
	m_StockClubID.InitHashTable(1003L);
}

//��������
CClubInfoManager::~CClubInfoManager()
{
	//��������
	CClubInfoItem::CleanStaticData();

	//ɾ������
	m_StockClubID.DeleteAllItem();

	return;
}

//ö�ٲ���
CClubInfoItem * CClubInfoManager::EnumClubItem(CClubInfoItem* pClubInfoItem)
{
	//��������
	if (pClubInfoItem == NULL)
	{
		return m_StockClubID.GetHeadStockItem();
	}

	return m_StockClubID.GetNextStockItem(pClubInfoItem);
}

//���Ҳ���
CClubInfoItem * CClubInfoManager::SearchClubItem(DWORD dwClubID)
{
	return m_StockClubID.SearchStockItem(dwClubID);
}

//�Ƴ�����
VOID CClubInfoManager::RemoveClubItem()
{
	//��������
	auto pClubInfoItem = m_StockClubID.GetHeadStockItem();
	while (pClubInfoItem != NULL)
	{
		pClubInfoItem->ResetClubItem();

		auto pClubInfoLast = pClubInfoItem;
		pClubInfoItem=m_StockClubID.GetNextStockItem(pClubInfoLast);
	}

	//�Ƴ�Ԫ��
	m_StockClubID.RemoveAllItem();
}

//�Ƴ�����
VOID CClubInfoManager::RemoveClubItem(DWORD dwClubID)
{
	//���Ҳ���
	auto pClubInfoItem = m_StockClubID.SearchStockItem(dwClubID);
	if (pClubInfoItem) pClubInfoItem->ResetClubItem();

	//�Ƴ�����
	m_StockClubID.RemoveStockItem(dwClubID);
}

//�������
CClubInfoItem* CClubInfoManager::ActiveClubItem(tagClubItem & ClubItem)
{
	//��������
	auto pClubInfoItem= m_StockClubID.SearchStockItem(ClubItem.ClubProperty.dwClubID);
	if (pClubInfoItem == NULL)
	{
		pClubInfoItem = m_StockClubID.CreateStockItem(ClubItem.ClubProperty.dwClubID);
	}	

	//��������
	if (pClubInfoItem != NULL)
	{
		//����״̬
		pClubInfoItem->m_wOnlineCount = 0;
		pClubInfoItem->m_bUnloadFlag = false;
		pClubInfoItem->m_dwActiveTime = (DWORD)time(NULL);		
		pClubInfoItem->m_cbClubState = CLUB_STATE_OFFLINE;

		//��������
		CopyMemory(&pClubInfoItem->m_ClubItem, &ClubItem, sizeof(ClubItem));
	}

	return pClubInfoItem;
}

//////////////////////////////////////////////////////////////////////////////////
