#include "StdAfx.h"
#include "GlobalInfoItem.h"

//////////////////////////////////////////////////////////////////////////


//���캯��
CGlobalUserItem::CGlobalUserItem()
{
	//�û�����
	m_dwUserID=0;
	m_wGateID=0;
	m_dwTokenID=0;
	m_dwGameID=0;
	m_cbGender=0;
	m_wFaceID=0;
	m_dwCustomID=0;
	m_szNickName[0]=0;

	//�û�״̬
	m_cbIMUserStatus = US_IM_OFFLINE;
	m_cbCBUserStatus = US_CB_OFFLINE;

	//״̬��Ϣ
	m_cbShutUpStatus = 0;
	m_dwShutUpOverTime = 0;

	return;
}

//��������
CGlobalUserItem::~CGlobalUserItem()
{
}

//��������
VOID CGlobalUserItem::ResetData()
{
	//�û�����
	m_dwUserID = 0;
	m_wGateID = 0;
	m_dwTokenID = 0;
	m_dwGameID = 0;
	m_cbGender = 0;
	m_wFaceID = 0;
	m_dwCustomID = 0;
	m_szNickName[0] = 0;

	//�û�״̬
	m_cbIMUserStatus = US_IM_OFFLINE;
	m_cbCBUserStatus = US_CB_OFFLINE;

	//״̬��Ϣ
	m_cbShutUpStatus = 0;
	m_dwShutUpOverTime = 0;
}

//////////////////////////////////////////////////////////////////////////
// 
//���캯��
CGlobalServiceItem::CGlobalServiceItem()
{
	//��������
	m_dwSocketID = 0;
	m_wOnlineCount = 0;	
	ZeroMemory(&m_ServiceItem, sizeof(m_ServiceItem));
}

//��������
CGlobalServiceItem::~CGlobalServiceItem()
{
}

//��������
VOID CGlobalServiceItem::ResetData()
{
	//��������
	m_dwSocketID = 0;
	m_wOnlineCount = 0;	
	ZeroMemory(&m_ServiceItem, sizeof(m_ServiceItem));
}

//////////////////////////////////////////////////////////////////////////
//���캯��
CGlobalGateItem::CGlobalGateItem()
{
	//��ʼ��
	m_StockTokenID.InitHashTable(1013);
}

//��������
CGlobalGateItem::~CGlobalGateItem()
{
	//��������
	ResetData();
}

//�������
tagTokenParameter* CGlobalGateItem::ActiveTokenID(DWORD dwTokenID)
{
	//��������
	auto pTokenParameter = m_StockTokenID.SearchStockItem(dwTokenID);
	if (pTokenParameter != NULL)
	{
		return pTokenParameter;
	}

	//��������
	pTokenParameter = m_StockTokenID.CreateStockItem(dwTokenID);
	if (pTokenParameter != NULL)
	{
		ZeroMemory(pTokenParameter,sizeof(tagTokenParameter));

		pTokenParameter->pBindUserItem=NULL;
		pTokenParameter->dwTokenID = dwTokenID;
		pTokenParameter->dwActiveTime = (DWORD)time(NULL);
	}

	return pTokenParameter;
}

//�Ƴ�����
VOID CGlobalGateItem::RemoveTokenID()
{
	m_TokenIDArray.RemoveAll();
	m_StockTokenID.RemoveAllItem();
}

//�Ƴ�����
VOID CGlobalGateItem::RemoveTokenID(DWORD dwTokenID)
{
	auto pTokenParameter = m_StockTokenID.RemoveStockItem(dwTokenID);
	if (pTokenParameter != NULL)
	{
		ZeroMemory(pTokenParameter,sizeof(tagTokenParameter));
	}
}

//��ѯ����
tagTokenParameter* CGlobalGateItem::SearchTokenID(DWORD dwTokenID)
{
	return m_StockTokenID.SearchStockItem(dwTokenID);
}

//��ѯ����
tagTokenParameter* CGlobalGateItem::EnumTokenID(tagTokenParameter* pTokenParameter)
{
	if (pTokenParameter == NULL)
	{
		return m_StockTokenID.GetHeadStockItem();
	}

	return m_StockTokenID.GetNextStockItem(pTokenParameter);
}

//��������
VOID CGlobalGateItem::ResetData()
{
	__super::ResetData();

	//�Ƴ�����
	m_TokenIDArray.RemoveAll();
	m_StockTokenID.RemoveAllItem();
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalServerItem::CGlobalServerItem()
{
	//���ñ���
	m_dwSocketID = 0;
	ZeroMemory(&m_GameServer, sizeof(m_GameServer));

	return;
}

//��������
CGlobalServerItem::~CGlobalServerItem()
{
}

//��������
VOID CGlobalServerItem::ResetData()
{
	//���ñ���
	m_dwSocketID = 0;
	ZeroMemory(&m_GameServer, sizeof(m_GameServer));
}

//////////////////////////////////////////////////////////////////////////
