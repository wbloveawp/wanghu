#include "Stdafx.h"
#include "ModuleBattle.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//ʱ�Ӷ���

#define	IDI_TIME_PULSE				1								//ʱ������

//////////////////////////////////////////////////////////////////////////////////////////////

//���캯��
CModuleBattle::CModuleBattle(IModuleItemSink* pIModuleItemSink):
	m_pIModuleItemSink(pIModuleItemSink)
{
	//����ģ��
	SetServiceModule(SERVICE_MODULE_BATTLE);
}

//��������
CModuleBattle::~CModuleBattle()
{
}

//�ر��¼�
bool CModuleBattle::OnEventVirtualShut()
{
	//���ñ���
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));

	//Ͷ����Ϣ	
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->PostWindowMessage(WM_BATTLE_LINK_SHUT, 0, 0);

	//������Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: ��Լս���������������ӶϿ���"), m_pIModuleItemSink->GetGlobalUserData()->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(255, 0, 0));

	return true;
}

//�����¼�
bool CModuleBattle::OnEventVirtualLink(LONG lErrorCode)
{
	//����ʧ��
	if (lErrorCode != 0)
	{
		//Ͷ����Ϣ	
		ASSERT(m_pIModuleItemSink != NULL);
		m_pIModuleItemSink->PostWindowMessage(WM_BATTLE_LINK_FAILURE, 0, 0);

		return true;
	}

	//��������
	if (m_BufferPacket.wMainCmdID != 0 && m_BufferPacket.wSubCmdID != 0)
	{
		SendVirtualData(m_BufferPacket.wMainCmdID, m_BufferPacket.wSubCmdID, m_BufferPacket.cbDataBuffer, m_BufferPacket.wDataSize);
	}

	return true;
}

//ɾ��ʱ��
bool CModuleBattle::KillModuleTimer(UINT nTimerID)
{
	//Ͷ����Ϣ
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->KillGameTimer(GetTimerStart() + nTimerID);

	return true;
}

//����ʱ��
bool CModuleBattle::SetModuleTimer(UINT nTimerID, UINT nElapse)
{
	//Ͷ����Ϣ
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->SetGameTimer(GetTimerStart() + nTimerID, nElapse);

	return true;
}

//ʱ���¼�
bool CModuleBattle::OnEventTimer(UINT uTimerID, WPARAM wBindParam)
{
	//ʱ������
	if (uTimerID == IDI_TIME_PULSE)
	{
		//��������
		DWORD dwTimeStamp = (DWORD)time(NULL);
		
		if (dwTimeStamp % 5 == 0)
		{
		
		}
	}

	return true;
}

//��ȡ�¼�
bool CModuleBattle::OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GB_BATTLE_SERVICE:			//�������
	{
		return OnSocketMainBattleService(Command.wSubCmdID, pData, wDataSize);
	}		
	}

	return true;
}

//��¼����
bool CModuleBattle::OnSocketMainBattleService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GB_S_TABLE_ITEM:		//�����Ա
	{
		return OnSocketSubTableItem(pData, wDataSize);
	}
	case SUB_GB_S_TABLE_PARAM:		//������Ϣ
	{
		return OnSocketSubTableParam(pData, wDataSize);
	}
	case SUB_GB_S_BATTLE_BASE_OPTION:	//����ɹ�
	{
		return OnSocketSubBattleBaseOption(pData, wDataSize);
	}
	case SUB_GB_S_BATTLE_GAME_OPTION:	//����ʧ��
	{
		return OnSocketSubBattleGameOption(pData, wDataSize);
	}
	case SUB_GB_S_BATTLE_OPTION_FINISH:		//��������
	{
		return OnSocketSubBattleOptionFinish(pData, wDataSize);
	}
	//case SUB_GB_S_OPERATE_SUCCESS:		//�����ɹ�
	//{
	//	return OnSocketSubOperateSuccess(pData, wDataSize);
	//}
	//case SUB_GB_S_OPERATE_FAILURE:		//����ʧ��
	//{
	//	return OnSocketSubOperateFailure(pData, wDataSize);
	//}
	}

	return true;
}
//��������
bool CModuleBattle::OnSocketSubTableItem(VOID* pData, WORD wDataSize)
{
	return true;
}

//�����б�
bool CModuleBattle::OnSocketSubTableList(VOID* pData, WORD wDataSize)
{
	return true;
}

//���Ӳ���
bool CModuleBattle::OnSocketSubTableParam(VOID* pData, WORD wDataSize)
{
	return true;
}

//���ӽ�ɢ
bool CModuleBattle::OnSocketSubTableDismiss(VOID* pData, WORD wDataSize)
{
	/*auto pOperateSuccess = (CMD_GB_S_OperateSuccess*)pData;

	CString strMessage;
	strMessage.Format(TEXT("�����ɹ���%s"), pOperateSuccess->szDescribeString);
	m_pIModuleItemSink->InsertString(strMessage, RGB(128, 128, 128));*/

	return true;
}

//�����ɹ�
bool CModuleBattle::OnSocketSubOperateSuccess(VOID* pData, WORD wDataSize)
{
	/*auto pOperateSuccess = (CMD_GB_S_OperateSuccess*)pData;

	CString strMessage;
	strMessage.Format(TEXT("�����ɹ���%s"), pOperateSuccess->szDescribeString);
	m_pIModuleItemSink->InsertString(strMessage,RGB(128, 128, 128));*/

	return true;
}

//����ʧ��
bool CModuleBattle::OnSocketSubOperateFailure(VOID* pData, WORD wDataSize)
{
	/*auto pOperateFailure = (CMD_GB_S_OperateFailure*)pData;

	CString strMessage;
	strMessage.Format(TEXT("����ʧ�ܣ�[%d]%s"), pOperateFailure->lErrorCode,pOperateFailure->szErrorMessage);
	m_pIModuleItemSink->InsertString(strMessage, RGB(128, 128, 128));*/

	return true;

}

//��������
bool CModuleBattle::OnSocketSubBattleBaseOption(VOID* pData, WORD wDataSize)
{
	auto pBaseOption = (tagBattleBaseOption*)pData;
	auto nOptionCount = wDataSize / sizeof(tagBattleBaseOption);

	for (int i = 0; i < nOptionCount; i++)
	{
		m_ArrayBaseOption.Add(*pBaseOption++);
	}

	m_pIModuleItemSink->InsertString(TEXT("�յ�Լս��������"), RGB(128, 128, 128));

	return true;
}

//��Ϸ����
bool CModuleBattle::OnSocketSubBattleGameOption(VOID* pData, WORD wDataSize)
{
	m_pIModuleItemSink->InsertString(TEXT("�յ�Լս��Ϸ����"), RGB(128, 128, 128));

	return true;
}

//�������
bool CModuleBattle::OnSocketSubBattleOptionFinish(VOID* pData, WORD wDataSize)
{
	m_pIModuleItemSink->InsertString(TEXT("Լս���ý������"), RGB(128, 128, 128));

	//��������
	if (m_ArrayBaseOption.GetCount() > 0)
	{
		CreateTable(&m_ArrayBaseOption[0]);
	}

	return true;
}

//�������
bool CModuleBattle::EnterService()
{
	//�û��ӿ�
	ASSERT(m_pIModuleItemSink != NULL);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//��������
	CMD_GB_C_QueryOption QueryOption = {};

	//���ñ���
	QueryOption.wKindID = INVALID_WORD;
	//QueryOption.cbEmployScene = 0;

	//У��״̬
	if (GetLinkStatus() == true)
	{
		SendVirtualData(MDM_GB_BATTLE_SERVICE, SUB_GB_C_QUERY_OPTION, &QueryOption, sizeof(QueryOption));
	}
	else
	{
		//��������
		m_BufferPacket.wMainCmdID = MDM_GB_BATTLE_SERVICE;
		m_BufferPacket.wSubCmdID = SUB_GB_C_QUERY_OPTION;
		m_BufferPacket.wDataSize = sizeof(QueryOption);
		CopyMemory(m_BufferPacket.cbDataBuffer,&QueryOption,sizeof(QueryOption));

		//��������
		CreateVirtualLink(INVALID_WORD, pGlobalUserData->dwUserID, pGlobalUserData->szLogonToken, NULL, 0);
	}

	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: ���ڲ�ѯԼս����..."), pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	return true;
}

//��������
VOID CModuleBattle::CreateTable(tagBattleBaseOption* pBattleBaseOption)
{
	//��������
	CMD_GB_C_CreateTable  CreateTable = {};

	//���ñ���
	StringCchCopy(CreateTable.szMachineID, sizeof(CreateTable.szMachineID), TEXT("jfkjslKKLFJLKfjiwejfLKJ1246546"));

	//Լս����-AA
	CreateTable.BattleConfig.wKindID = pBattleBaseOption->wKindID;
	CreateTable.BattleConfig.cbScoreKind = pBattleBaseOption->cbScoreKind;
	CreateTable.BattleConfig.cbCurrencyKind = CURRENCY_KIND_GOLD;
	CreateTable.BattleConfig.cbSettleKind = SETTLE_KIND_COUNT;
	CreateTable.BattleConfig.wPlayerCount = pBattleBaseOption->wPlayerCount;
	CreateTable.BattleConfig.wPayType = PAY_TYPE_USER;	
	CreateTable.BattleConfig.lBaseScore = pBattleBaseOption->lMaxBaseScore;
	CreateTable.BattleConfig.lEntryCost = pBattleBaseOption->lMinEntryCost;
	//CreateTable.BattleConfig.lRevenueRate = 0;
	//CreateTable.BattleConfig.cbAllowLookon = TRUE;
	CreateTable.BattleConfig.cbAllowPublic = TRUE;
	CreateTable.BattleConfig.cbAllowSameIP = TRUE;
	CreateTable.BattleConfig.wPlayCount = pBattleBaseOption->OptionItem[0].wPlayCount;

	//������Ϣ
	SendVirtualData(MDM_GB_BATTLE_SERVICE, SUB_GB_C_CREATE_TABLE, &CreateTable, sizeof(CreateTable));
}

//����ģ��
VOID CModuleBattle::ResetModule()
{
	//���ñ���
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));	
}
//////////////////////////////////////////////////////////////////////////////////////////////
