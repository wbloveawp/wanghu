#include "StdAfx.h"
#include "AfxTempl.h"
#include "BattleTableFrame.h"
#include "BattleServiceManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//��̬��Ա
CBattleServiceManager * CBattleServiceManager::m_pBattleServiceManager=NULL;

////////////////////////////////////////////////////////////////////////

//���캯��
CBattleServiceManager::CBattleServiceManager(void)
{
	//״̬����
	m_bIsService=false;
	ZeroMemory(&m_ExtendServiceParameter, sizeof(m_ExtendServiceParameter));

	//���ñ���
	if (m_pBattleServiceManager==NULL) m_pBattleServiceManager=this;
}

//��������
CBattleServiceManager::~CBattleServiceManager(void)
{	
	//���ñ���
	if (m_pBattleServiceManager==this) m_pBattleServiceManager=NULL;
}

//ֹͣ����
bool CBattleServiceManager::ConcludeService()
{
	//״̬�ж�
	ASSERT(m_bIsService==true);

	//����״̬
	m_bIsService=false;
	ZeroMemory(&m_ExtendServiceParameter, sizeof(m_ExtendServiceParameter));

	return true;
}

//��������
bool CBattleServiceManager::StartService()
{
	//״̬�ж�
	ASSERT(m_bIsService==false);
	if(m_bIsService==true) return false;

	//����״̬
	m_bIsService=true;	

	return true;
}
//�ӿڲ�ѯ
void *  CBattleServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IExtendServiceManager,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IExtendServiceManager,Guid,dwQueryVer);	
	return NULL;
}

//��ʼ�����ӿ��
VOID* CBattleServiceManager::CreateExtendTableFrame()
{
	//��������
	CTableFrameBattle * pTableFrameItem = NULL;

	try
	{
		//��������
		pTableFrameItem = new CTableFrameBattle();
		if (pTableFrameItem == NULL) throw TEXT("����ʧ��");

		//�������
		m_TableFrameArray.Add(pTableFrameItem);

		return pTableFrameItem;
	}
	catch (...) {}

	//ɾ������
	SafeDelete(pTableFrameItem);

	return NULL;	
}

//��������
VOID* CBattleServiceManager::CreateExtendDataBaseSink(REFGUID Guid, DWORD dwQueryVer)
{
	//��������
	CExtendDataBaseSink* pExtendDataBaseSink = NULL;

	try
	{
		//��������
		pExtendDataBaseSink = new CExtendDataBaseSink();
		if (pExtendDataBaseSink == NULL) throw TEXT("����ʧ��");

		//��ѯ�ӿ�
		VOID* pObject = pExtendDataBaseSink->QueryInterface(Guid, dwQueryVer);
		if (pObject == NULL) throw TEXT("�ӿڲ�ѯʧ��");

		return pObject;
	}
	catch (...) {}

	//ɾ������
	SafeDelete(pExtendDataBaseSink);

	return NULL;
}

//��ʼ���ӿ�
bool CBattleServiceManager::InitServiceManager(tagExtendServiceParameter& ExtendServiceParameter)
{
	//��������
	m_ExtendServiceParameter=ExtendServiceParameter;

	return true;
}

//ʱ���¼�
bool CBattleServiceManager::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//��������
	BYTE cbTimerIndex=dwTimerID-IDI_EXTEND_MODULE_START;

	return false;
}

//���ݿ��¼�
bool CBattleServiceManager::OnEventDataBase(WORD wRequestID, VOID * pData, WORD wDataSize,DWORD dwContextID, DWORD dwTokenID)
{
	switch (wRequestID)
	{
	case DBO_GR_DEDUCT_BATTLE_COST: //�۷ѽ��
	{
		//Ч�����
		ASSERT (wDataSize <= sizeof(DBO_GR_DeductBattleCost));
		if (wDataSize > sizeof(DBO_GR_DeductBattleCost)) return false;

		//��ȡ����
		auto pDeductBattleCost = (DBO_GR_DeductBattleCost*)pData;

		//�ж�����
		ASSERT (pDeductBattleCost->wTableID < m_TableFrameArray.GetCount());
		if (pDeductBattleCost->wTableID >= m_TableFrameArray.GetCount()) return true;

		//��������
		auto pTableFrame = (CTableFrameBattle*)m_TableFrameArray[pDeductBattleCost->wTableID];
		if (pTableFrame == NULL) return false;

		//��������
		tagBattleTableItemEx TableItem;
		pTableFrame->GetBattleTableItem(TableItem);

		//����֧��
		if (TableItem.BattleConfig.wPayType == PAY_TYPE_OWNER)
		{
			//��������
			CMD_BS_S_CreateTable CreateTable;
			ZeroMemory(&CreateTable, sizeof(CreateTable));

			//������Ϣ	
			CreateTable.wTableID = TableItem.wTableID;
			CreateTable.dwBattleNum = TableItem.dwBattleNum;
			CreateTable.dwCreateTime = TableItem.dwCreateTime;

			//�Ƹ���Ϣ
			CreateTable.dwPayUserID = pDeductBattleCost->dwPayUserID;
			CreateTable.lUserGold = pDeductBattleCost->lUserGold;
			CreateTable.lUserCard = pDeductBattleCost->lUserCard;

			//�����Ϣ
			CreateTable.lResultCode = pDeductBattleCost->lResultCode;
			lstrcpynW(CreateTable.szErrorMessage, pDeductBattleCost->szErrorMessage, CountArray(CreateTable.szErrorMessage));

			//������Ϣ
			WORD wStringSize = CountStringBufferW(CreateTable.szErrorMessage);
			WORD wSendSize = sizeof(CreateTable) - sizeof(CreateTable.szErrorMessage) + wStringSize;
			m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_CREATE_TABLE, &CreateTable, wSendSize, 0);
		}

		//�Ǵ�Ӯ��֧��
		if (TableItem.BattleConfig.wPayType != PAY_TYPE_WINNER)
		{
			//�۷ѽ��
			pTableFrame->DeductCostResult(pDeductBattleCost->lResultCode == 0, pDeductBattleCost->dwPayUserID, pDeductBattleCost->dwFailUserID);

			//��ɢ����
			if (pDeductBattleCost->lResultCode != 0) pTableFrame->DismissBattleTable(false);
		}
	}
	case DBO_GR_RETURN_BATTLE_COST: //�˻����
	{
		//Ч�����
		ASSERT(wDataSize <= sizeof(DBO_GR_ReturnBattleCost));
		if (wDataSize > sizeof(DBO_GR_ReturnBattleCost)) return false;

		//��ȡ����
		auto pReturnBattleCost = (DBO_GR_ReturnBattleCost*)pData;	

		return true;
	}	
	}

	return false;
}

//�����¼�
bool CBattleServiceManager::OnEventSocketRead(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_BS_C_CREATE_TABLE:		 //��������
	{
		//����У��
		ASSERT(sizeof(CMD_BS_C_CreateTable) == wDataSize);
		if (sizeof(CMD_BS_C_CreateTable) != wDataSize) return false;

		//��ȡ����
		CMD_BS_C_CreateTable* pCreateTable = (CMD_BS_C_CreateTable*)pData;

		//��������			
		tagBattleTableItemEx TableItem;
		CTableFrameBattle* pFindTableFrame = NULL;

		//���ҿ���	
		for (WORD i = 0; i < m_ExtendServiceParameter.pGameServiceOption->wTableCount; i++)
		{
			//��ȡ����
			auto pTableFrame = (CTableFrameBattle*)m_TableFrameArray[i];

			//�ж�״̬
			if (pTableFrame->IsGameStarted()) continue;
			if (pTableFrame->GetSitUserCount() > 0) continue;

			//��ȡ��Ϣ				
			pTableFrame->GetBattleTableItem(TableItem);
			if (TableItem.dwBattleNum != 0 || TableItem.dwOwnerID != 0) continue;

			//���ñ���
			pFindTableFrame = pTableFrame;
			break;
		}

		//δ�ҵ�����
		if (pFindTableFrame == NULL)
		{
			//��������
			CMD_BS_S_CreateTable CreateBattleTable;
			ZeroMemory(&CreateBattleTable, sizeof(CreateBattleTable));

			//����ṹ				
			CreateBattleTable.wTableID = INVALID_TABLE;
			CreateBattleTable.lResultCode = BATTLE_CODE_TABLE_UNFOUND;
			CreateBattleTable.dwBattleNum = pCreateTable->dwBattleNum;

			//������Ϣ
			StringCchCopy(CreateBattleTable.szErrorMessage, CountArray(CreateBattleTable.szErrorMessage), CWHService::FormatMessage(MIM_SERVER_0043, TEXT("��Ǹ��û�ҵ����õķ���, ���Ժ����ԣ�")));

			//������Ϣ
			WORD wStringSize = CountStringBuffer(CreateBattleTable.szErrorMessage);
			WORD wPacketSize = sizeof(CreateBattleTable) - sizeof(CreateBattleTable.szErrorMessage) + wStringSize;
			m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_CREATE_TABLE, &CreateBattleTable, wPacketSize, 0);

			return true;
		}

		//��������
		tagBattleCreateParam CreateParam;
		ZeroMemory(&CreateParam, sizeof(CreateParam));

		//������Ϣ			
		CreateParam.dwOwnerID = pCreateTable->dwUserID;
		CreateParam.dwBattleNum = pCreateTable->dwBattleNum;
		StringCchCopy(CreateParam.szPassword, CountArray(CreateParam.szPassword), pCreateTable->szPassword);

		//Ⱥ����Ϣ
		CreateParam.dwClubID = pCreateTable->dwClubID;
		CreateParam.wSwitchOption = pCreateTable->wSwitchOption;
		CreateParam.wClubServiceID = pCreateTable->wClubServiceID;
		CreateParam.dwClubCreaterID = pCreateTable->dwClubCreaterID;		

		//������Ϣ
		CreateParam.dwCreateTime = (DWORD)time(NULL);
		CreateParam.dwConfigID = pCreateTable->dwConfigID;
		CopyMemory(&CreateParam.BattleConfig, &pCreateTable->BattleConfig, sizeof(CreateParam.BattleConfig));

		//��������
		if (pFindTableFrame->CreateBattleTable(CreateParam) == false)
		{
			//��������
			CMD_BS_S_CreateTable CreateBattleTable;
			ZeroMemory(&CreateBattleTable, sizeof(CreateBattleTable));

			//����ṹ				
			CreateBattleTable.wTableID = INVALID_TABLE;
			CreateBattleTable.lResultCode = BATTLE_CODE_CREATE_FAILED;
			CreateBattleTable.dwBattleNum = pCreateTable->dwBattleNum;

			//������Ϣ
			StringCchCopy(CreateBattleTable.szErrorMessage, CountArray(CreateBattleTable.szErrorMessage), CWHService::FormatMessage(MIM_SERVER_0044, TEXT("��Ǹ�����䴴��ʧ��, ���Ժ����ԣ�")));

			//������Ϣ
			WORD wStringSize = CountStringBuffer(CreateBattleTable.szErrorMessage);
			WORD wPacketSize = sizeof(CreateBattleTable) - sizeof(CreateBattleTable.szErrorMessage) + wStringSize;
			m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_CREATE_TABLE, &CreateBattleTable, wPacketSize, 0);

			return true;
		}

		//����֧��
		if (pCreateTable->BattleConfig.wPayType == PAY_TYPE_OWNER)
		{
			//����ṹ
			DBR_GR_DeductBattleCost DeductBattleCost;
			ZeroMemory(&DeductBattleCost, sizeof(DeductBattleCost));

			//�û���Ϣ	
			DeductBattleCost.dwUserID = pCreateTable->dwUserID;
			DeductBattleCost.dwClubID = pCreateTable->dwClubID;
			DeductBattleCost.wTableID = pFindTableFrame->GetTableID();
			DeductBattleCost.wPayType = pCreateTable->BattleConfig.wPayType;
			DeductBattleCost.lConsumeCount = pCreateTable->BattleConfig.lConsumeCount;
			DeductBattleCost.cbCurrencyKind = pCreateTable->BattleConfig.cbCurrencyKind;
			
			//Ͷ������
			m_ExtendServiceParameter.pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DEDUCT_BATTLE_COST, DeductBattleCost.dwUserID, 0L, 0L, &DeductBattleCost, sizeof(DeductBattleCost));
		}
		else
		{
			//��ȡ������Ϣ			
			pFindTableFrame->GetBattleTableItem(TableItem);

			//��������
			CMD_BS_S_CreateTable CreateBattleTable;
			ZeroMemory(&CreateBattleTable, sizeof(CreateBattleTable));

			//������Ϣ	
			CreateBattleTable.wTableID = TableItem.wTableID;
			CreateBattleTable.dwBattleNum = TableItem.dwBattleNum;
			CreateBattleTable.dwCreateTime = TableItem.dwCreateTime;

			//������Ϣ
			WORD wStringSize = CountStringBuffer(CreateBattleTable.szErrorMessage);
			WORD wPacketSize = sizeof(CreateBattleTable) - sizeof(CreateBattleTable.szErrorMessage) + wStringSize;
			m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_CREATE_TABLE, &CreateBattleTable, wPacketSize, 0);
		}

		return true;
	}
	case SUB_BS_C_MODIFY_TABLE:		 //�޸�����
	{
		//����У��
		ASSERT(sizeof(CMD_BS_C_ModifyTable) == wDataSize);
		if (sizeof(CMD_BS_C_ModifyTable) != wDataSize) return false;

		//��ȡ����
		CMD_BS_C_ModifyTable* pModifyTable = (CMD_BS_C_ModifyTable*)pData;

		//У������
		if (pModifyTable->wTableID >= m_TableFrameArray.GetCount())
		{
			//��������
			CMD_BS_S_ModifyTable ModifyBattleTable;
			ZeroMemory(&ModifyBattleTable, sizeof(ModifyBattleTable));

			//������Ϣ
			ModifyBattleTable.cbResultCode = BATTLE_CODE_MODIFY_FAILED;
			CopyMemory(&ModifyBattleTable.SocketInfo, &pModifyTable->SocketInfo, sizeof(ModifyBattleTable.SocketInfo));
			StringCchCopy(ModifyBattleTable.szErrorMessage, CountArray(ModifyBattleTable.szErrorMessage), CWHService::FormatMessage(MIM_SERVER_0045, L"��Ǹ��ϵͳδ��ѯ��������Ϣ,���ø���ʧ�ܣ�"));

			//������Ϣ
			WORD wStringSize = CountStringBufferW(ModifyBattleTable.szErrorMessage);
			WORD wPacketSize = sizeof(ModifyBattleTable) - sizeof(ModifyBattleTable.szErrorMessage) + wStringSize;
			m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_MODIFY_TABLE, &ModifyBattleTable, wPacketSize, 0);
		}

		//��ȡ����
		auto pTableFrame = (CTableFrameBattle*)m_TableFrameArray[pModifyTable->wTableID];

		//��������
		tagBattleModifyParam ModifyParam;
		ZeroMemory(&ModifyParam, sizeof(ModifyParam));

		//������Ϣ
		ModifyParam.dwUserID = pModifyTable->dwUserID;
		ModifyParam.dwBattleNum = pModifyTable->dwBattleNum;

		//��������
		CopyMemory(&ModifyParam.BattleConfig, &pModifyTable->BattleConfig, sizeof(ModifyParam.BattleConfig));

		//�޸�����
		if (pTableFrame->ModifyBattleTable(ModifyParam) == false)
		{
			//��������
			CMD_BS_S_ModifyTable ModifyBattleTable;
			ZeroMemory(&ModifyBattleTable, sizeof(ModifyBattleTable));

			//������Ϣ
			CopyMemory(&ModifyBattleTable.SocketInfo, &pModifyTable->SocketInfo, sizeof(ModifyBattleTable.SocketInfo));

			//������Ϣ
			ModifyBattleTable.cbResultCode = BATTLE_CODE_MODIFY_FAILED;
			_snwprintf_s(ModifyBattleTable.szErrorMessage, CountArray(ModifyBattleTable.szErrorMessage), CWHService::FormatMessage(MIM_SERVER_0046, L"��Ǹ�����ø���ʧ�ܣ�"));

			//������Ϣ
			WORD wStringSize = CountStringBufferW(ModifyBattleTable.szErrorMessage);
			WORD wPacketSize = sizeof(ModifyBattleTable) - sizeof(ModifyBattleTable.szErrorMessage) + wStringSize;
			m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_MODIFY_TABLE, &ModifyBattleTable, wPacketSize, 0);

			return true;
		}

		//������Ϣ		
		tagBattleTableItemEx TableItem;
		pTableFrame->GetBattleTableItem(TableItem);

		//��������
		CMD_BS_S_ModifyTable ModifyBattleTable;
		ZeroMemory(&ModifyBattleTable, sizeof(ModifyBattleTable));

		//������Ϣ
		ModifyBattleTable.dwUserID = pModifyTable->dwUserID;
		ModifyBattleTable.dwBattleNum = pModifyTable->dwBattleNum;
		CopyMemory(&ModifyBattleTable.BattleConfig, &TableItem.BattleConfig, sizeof(ModifyBattleTable.BattleConfig));

		//������Ϣ
		CopyMemory(&ModifyBattleTable.SocketInfo, &pModifyTable->SocketInfo, sizeof(ModifyBattleTable.SocketInfo));

		//������Ϣ
		WORD wPacketSize = sizeof(ModifyBattleTable) - sizeof(ModifyBattleTable.szErrorMessage);
		m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_MODIFY_TABLE, &ModifyBattleTable, wPacketSize, 0);

		return true;
	}
	case SUB_BS_C_BATTLE_OPTION:	 //Լսѡ��
	{
		ASSERT(wDataSize == sizeof(CMD_BS_C_BattleOption));
		if (wDataSize != sizeof(CMD_BS_C_BattleOption)) return false;

		//��ȡ����
		CMD_BS_C_BattleOption* pBattleOption = (CMD_BS_C_BattleOption*)pData;

		//��������	
		for (WORD i = 0; i < m_ExtendServiceParameter.pGameServiceOption->wTableCount; i++)
		{
			//��ȡ����
			auto pTableFrame = (CTableFrameBattle*)m_TableFrameArray[i];
			pTableFrame->SetBattleService(pBattleOption->bBattleService);
		}

		return true;
	}
	case SUB_BS_C_SYNCHRO_TABLE:	 //ͬ������
	{
		//����У��
		ASSERT(wDataSize % sizeof(tagBattleTableItemEx) == 0);
		if (wDataSize % sizeof(tagBattleTableItemEx) != 0) return false;

		//��������
		WORD wTableCount = wDataSize / sizeof(tagBattleTableItemEx);

		//��������
		tagBattleTableItemEx TableItem;
		CTableFrameBattle* pTableFrame = NULL;

		//��������
		if (wTableCount == 0)
		{
			//��������
			WORD wPacketSize = 0;
			BYTE cbBuffer[SOCKET_BUFFER];

			//��������	
			for (WORD i = 0; i < m_ExtendServiceParameter.pGameServiceOption->wTableCount; i++)
			{
				//��ȡ����
				pTableFrame = (CTableFrameBattle*)m_TableFrameArray[i];
				if (pTableFrame->GetRelationParameter()->dwRelationNum == 0) continue;

				//�����ж�
				if (wPacketSize + sizeof(tagBattleTableItemEx) > sizeof(cbBuffer))
				{
					//��������
					m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_TABLE_SYNCHRO, cbBuffer, wPacketSize, 0);
					wPacketSize = 0;
				}

				//������Ϣ
				pTableFrame->GetBattleTableItem(TableItem);

				//��ȡ����
				tagBattleTableItemEx* pTableItem = (tagBattleTableItemEx*)(cbBuffer + wPacketSize);
				CopyMemory(pTableItem, &TableItem, sizeof(tagBattleTableItemEx));

				//λ��ƫ��
				wPacketSize += sizeof(tagBattleTableItemEx);
			}

			//ʣ�෢��
			if (wPacketSize > 0)
			{
				//��������
				m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_TABLE_SYNCHRO, cbBuffer, wPacketSize, 0);
			}

			//��������	
			for (WORD i = 0; i < m_ExtendServiceParameter.pGameServiceOption->wTableCount; i++)
			{
				//��ȡ����
				pTableFrame = (CTableFrameBattle*)m_TableFrameArray[i];
				if (pTableFrame->GetRelationParameter()->dwRelationNum == 0) continue;

				//������Ϣ
				pTableFrame->NotifyTableParam();
				pTableFrame->NotifyTableUserList();
			}

			//ͬ�����
			m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_SYNCHRO_FINISH, NULL, 0, 0);
		}
		else
		{
			//��������
			tagBattleCreateParam CreateParam;
			tagBattleTableItemEx* pTableItem = (tagBattleTableItemEx*)pData;
			
			//�������
			for (WORD i = 0; i < wTableCount; i++)
			{
				//�����ж�
				ASSERT(pTableItem->wTableID < m_TableFrameArray.GetCount());
				if (pTableItem->wTableID >= m_TableFrameArray.GetCount()) continue;

				//��ȡ����
				pTableFrame = (CTableFrameBattle*)m_TableFrameArray[pTableItem->wTableID];

				//����ṹ					
				ZeroMemory(&CreateParam, sizeof(CreateParam));

				//������Ϣ
				CreateParam.dwOwnerID = pTableItem->dwOwnerID;
				CreateParam.dwBattleNum = pTableItem->dwBattleNum;
				StringCchCopy(CreateParam.szPassword, CountArray(CreateParam.szPassword), pTableItem->szPassword);

				//������Ϣ
				CreateParam.dwClubID = pTableItem->dwClubID;
				CreateParam.wSwitchOption = pTableItem->wSwitchOption;
				CreateParam.wClubServiceID = pTableItem->wClubServiceID;
				CreateParam.dwClubCreaterID = pTableItem->dwClubCreaterID;

				//������Ϣ
				CreateParam.dwConfigID = pTableItem->dwConfigID;
				CreateParam.dwCreateTime = pTableItem->dwCreateTime;				
				CopyMemory(&CreateParam.BattleConfig, &pTableItem->BattleConfig, sizeof(CreateParam.BattleConfig));

				//У��״̬
				if (m_ExtendServiceParameter.pIMainServiceFrame->GetServiceStatus() != SERVICE_STATUS_SERVICE)
				{
					//��������
					if (pTableFrame->CreateBattleTable(CreateParam) == false)
					{
						//��������
						CMD_BS_S_TableDismiss TableDismiss;
						ZeroMemory(&TableDismiss, sizeof(TableDismiss));

						//������Ϣ	
						TableDismiss.wTableID = pTableItem->wTableID;
						TableDismiss.dwBattleNum = pTableItem->dwBattleNum;
						TableDismiss.wServerID = m_ExtendServiceParameter.pGameServiceOption->wServerID;

						//��������
						m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_TABLE_DISMISS, &TableDismiss, sizeof(TableDismiss), 0);
					}

					//������Ϣ
					pTableFrame->NotifyTableParam();
					pTableFrame->NotifyTableUserList();

					CTraceService::TraceString(TEXT("BattleNum=%d,Password=%ls,ClubID=%d,ConfigID=%d"), TraceLevel_Normal, pTableItem->dwBattleNum, pTableItem->szPassword, pTableItem->dwClubID,pTableItem->dwConfigID);
				}
				else
				{
					//������Ϣ																		
					pTableFrame->GetBattleTableItem(TableItem);

					//�����ѽ�ɢ
					if (TableItem.dwBattleNum == 0 || TableItem.dwBattleNum != CreateParam.dwBattleNum)
					{
						//��������
						CMD_BS_S_TableDismiss TableDismiss;
						ZeroMemory(&TableDismiss, sizeof(TableDismiss));

						//������Ϣ	
						TableDismiss.wTableID = TableItem.wTableID;
						TableDismiss.dwBattleNum = CreateParam.dwBattleNum;
						TableDismiss.wServerID = m_ExtendServiceParameter.pGameServiceOption->wServerID;

						//��������
						m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_TABLE_DISMISS, &TableDismiss, sizeof(TableDismiss), 0);
					}
					else
					{
						//Լս״̬
						if (pTableFrame->GetBattleState() == BATTLE_STATE_PLAY)
						{
							//������Ϣ
							pTableFrame->NotifyTableParam();
							pTableFrame->NotifyTableUserList();
						}
					}
				}

				//ָ�����
				++pTableItem;
			}
		}

		return true;
	}
	case SUB_BS_C_DISMISS_TABLE:	 //��ɢ����
	{
		//У�����
		ASSERT(wDataSize == sizeof(CMD_BS_C_DismissTable));
		if (wDataSize != sizeof(CMD_BS_C_DismissTable)) return false;

		//��ȡ����
		CMD_BS_C_DismissTable* pDismissTable = (CMD_BS_C_DismissTable*)pData;
		if (pDismissTable->wTableID >= m_TableFrameArray.GetCount()) return false;

		//������Ϣ
		auto pSocketInfo = &pDismissTable->SocketInfo;

		//��ȡ����
		auto pTableFrame = (CTableFrameBattle*)m_TableFrameArray[pDismissTable->wTableID];
		if (pTableFrame->GetRelationParameter()->dwRelationNum != pDismissTable->dwBattleNum)
		{
			//����ṹ
			CMD_BS_S_DismissTable DismissBattleTable;
			ZeroMemory(&DismissBattleTable, sizeof(DismissBattleTable));

			//���ñ���
			DismissBattleTable.cbResultCode = 1;
			DismissBattleTable.dwBattleNum = pDismissTable->dwBattleNum;
			CopyMemory(&DismissBattleTable.SocketInfo, pSocketInfo, sizeof(DismissBattleTable.SocketInfo));
			StringCchCopy(DismissBattleTable.szDescribeString, CountArray(DismissBattleTable.szDescribeString), CWHService::FormatMessage(MIM_SERVER_0047, TEXT("�����ѱ���ɢ��")));

			//��������
			m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_DISMISS_TABLE, &DismissBattleTable, sizeof(DismissBattleTable), 0);

			return true;
		}		

		//��ɢ����
		pTableFrame->DismissBattleTable(pDismissTable->dwUserID);

		//���˽�ɢ
		if (pSocketInfo->dwTokenID != 0 && pSocketInfo->wGateID != 0)
		{
			//����ṹ
			CMD_BS_S_DismissTable DismissBattleTable;
			ZeroMemory(&DismissBattleTable, sizeof(DismissBattleTable));

			//���ñ���			
			DismissBattleTable.dwBattleNum = pDismissTable->dwBattleNum;
			CopyMemory(&DismissBattleTable.SocketInfo, pSocketInfo, sizeof(DismissBattleTable.SocketInfo));			

			//��������
			m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_DISMISS_TABLE, &DismissBattleTable, sizeof(DismissBattleTable), 0);
		}

		return true;
	}
	case SUB_BS_C_MODIFY_CLUB_SCORE:	//��ѯ����
	{
		//У�����
		ASSERT (wDataSize==sizeof(CMD_BS_C_ModifyClubScore));
		if (wDataSize != sizeof(CMD_BS_C_ModifyClubScore)) return false;

		//��ȡ����
		auto pModifyClubScore = (CMD_BS_C_ModifyClubScore*)pData;

		//�����û�
		auto pIServerUserItem = m_ExtendServiceParameter.pIServerUserManager->SearchUserItem(pModifyClubScore->dwUserID);
		if (pIServerUserItem != NULL && pIServerUserItem->GetScoreKind() == SCORE_KIND_CLUB)
		{
			//������Ϣ
			tagLockScoreInfo LockScoreInfo;
			ZeroMemory(&LockScoreInfo, sizeof(LockScoreInfo));
			LockScoreInfo.lMaxLockScore = m_ExtendServiceParameter.pGameServiceOption->lMaxLockScore;
			LockScoreInfo.lLessLockScore = m_ExtendServiceParameter.pGameServiceOption->lLessLockScore;
			LockScoreInfo.lRestrictScore = m_ExtendServiceParameter.pGameServiceOption->lRestrictScore;

			//�޸Ļ���
			pIServerUserItem->ModifyUserScore(pModifyClubScore->lChangeScore, LockScoreInfo);

			//���ͷ���
			m_ExtendServiceParameter.pIMainServiceFrame->SendUserScore(pIServerUserItem);

			//���·���
			OnEventUserItemScore(pIServerUserItem);
		}	

		return true;
	}
	}

	return true;
}

//������Ϣ
bool CBattleServiceManager::OnEventNetworkRead(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID,DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_DISMISS_TABLE:			//��ɢ����
	{
		//У�����
		ASSERT(wDataSize == sizeof(CMD_GR_DismissTable));
		if (wDataSize != sizeof(CMD_GR_DismissTable)) return false;

		//��ȡ����
		CMD_GR_DismissTable * pDismissTable=(CMD_GR_DismissTable *)pData;
		if (pDismissTable->wTableID>=m_TableFrameArray.GetCount()) return false;

		//��ȡ����
		auto pTableFrame= (CTableFrameBattle*)m_TableFrameArray[pDismissTable->wTableID];
		if (pTableFrame->GetRelationParameter()->dwRelationNum != pDismissTable->dwBattleNum) return false;

		//������Ϣ
		tagBattleTableItemEx TableItem;
		pTableFrame->GetBattleTableItem(TableItem);

		//�û���Ϣ
		DWORD dwUserID = pIServerUserItem->GetUserID();

		//��������
		CMD_GR_DismissResult DismissBattleTable;
		ZeroMemory(&DismissBattleTable, sizeof(DismissBattleTable));

		//У�����
		if (dwUserID != TableItem.dwOwnerID)
		{
			//����ṹ
			DismissBattleTable.cbResultCode = FALSE;
			StringCchCopy(DismissBattleTable.szDescribeString, CountArray(DismissBattleTable.szDescribeString), CWHService::FormatMessage(MIM_SERVER_0049,TEXT("��û��Ȩ�޽�ɢ����!")));
		}
		else
		{
			//Լս����
			if (pTableFrame->DismissBattleTable(dwUserID) == true)
			{
				//����ṹ
				DismissBattleTable.cbResultCode = TRUE;
				StringCchCopy(DismissBattleTable.szDescribeString, CountArray(DismissBattleTable.szDescribeString), CWHService::FormatMessage(MIM_SERVER_0050, TEXT("�����ɢ�ɹ�!")) );
			}
			else
			{
				//����ṹ
				DismissBattleTable.cbResultCode = FALSE;
				StringCchCopy(DismissBattleTable.szDescribeString, CountArray(DismissBattleTable.szDescribeString), CWHService::FormatMessage(MIM_SERVER_0051, TEXT("�����ɢʧ��!")));
			}
		}

		//��������	
		WORD wStringSize=CountStringBuffer(DismissBattleTable.szDescribeString);
		WORD wSendSize=sizeof(DismissBattleTable)-sizeof(DismissBattleTable.szDescribeString)+wStringSize;
		m_ExtendServiceParameter.pIMainServiceFrame->SendDataToUser(pIServerUserItem,MDM_GR_BATTLE, SUB_GR_DISMISS_RESULT,&DismissBattleTable,wSendSize);

		return true;
	}
	}
	return false;
}

//�û���¼
bool CBattleServiceManager::OnEventUserLogon(IServerUserItem* pIServerUserItem)
{
	return true;
}

//�û��ǳ�
bool CBattleServiceManager::OnEventUserLogout(IServerUserItem* pIServerUserItem)
{
	return true;
}

//�û�����
bool CBattleServiceManager::OnEventUserItemScore(IServerUserItem* pIServerUserItem)
{
	//����У��
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem == NULL) return false;

	//�������
	if (pIServerUserItem->GetScoreKind() == SCORE_KIND_CLUB)
	{
		WORD wTableID = pIServerUserItem->GetTableID();
		if (wTableID < m_TableFrameArray.GetCount())
		{
			//��ȡ����
			auto pTableFrame = (CTableFrameBattle*)m_TableFrameArray[wTableID];

			//Լս��Ϣ
			tagBattleTableItemEx TableItem;
			pTableFrame->GetBattleTableItem(TableItem);

			//����ṹ
			CMD_BS_S_ClubScore ClubScore = {};

			//���ñ���
			ClubScore.dwClubID = TableItem.dwClubID;
			ClubScore.dwBattleNum = TableItem.dwBattleNum;
			ClubScore.dwUserID = pIServerUserItem->GetUserID();			
			ClubScore.lClubScore = pIServerUserItem->GetUserScore();

			//��������
			m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_CLUB_SCORE, &ClubScore, sizeof(ClubScore), 0);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(BattleServiceManager);

////////////////////////////////////////////////////////////////////////
