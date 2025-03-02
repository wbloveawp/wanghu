#include "StdAfx.h"
#include "MissionStock.h"
#include "PlatformFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////

//��������
#define IDI_AVTIVE_MISSION				100								//��������
#define TIME_AVTIVE_MISSION				600000L							//��������

//////////////////////////////////////////////////////////////////////////////////
//��̬����

CMissionStock * CMissionStock::m_pMissionStock=NULL;					//����ָ��

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMissionStock, CWnd)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CMissionStock::CMissionStock()
{
	//�洢����
	m_wDataSize=0;
	ZeroMemory(m_cbDataBuffer,sizeof(m_cbDataBuffer));

	//���ñ���
	m_StockAction=SAction_NULL;
	m_pIStockOperateCenter=NULL;	

	//���ö���
	if (m_pMissionStock==NULL) m_pMissionStock=this;

	return;
}

//��������
CMissionStock::~CMissionStock()
{
	//�ͷŶ���
	ASSERT(m_pMissionStock==this);
	if (m_pMissionStock==this) m_pMissionStock=NULL;

	return;
}

//�����¼�
bool CMissionStock::OnEventMissionLink(INT nErrorCode)
{
	//�����ж�
	if (nErrorCode!=0L)
	{
		//��������
		if (m_hWnd==NULL)
		{
			CRect rcCreate(0,0,0,0);
			Create(NULL,NULL,WS_CHILD,rcCreate,GetDesktopWindow(),100);
		}

		//���ÿؼ�
		if(m_pIStockOperateCenter!=NULL)
		{
			m_pIStockOperateCenter->OnEventEnableControls(TRUE);
		}

		//��ʾ��ʾ
		CInformation Information(CPlatformFrame::GetInstance());
		Information.ShowMessageBox(TEXT("��������������Ѿ��Ͽ�,�����µ�¼��"),MB_ICONERROR);
	}
	else
	{
		return true;
	}

	return true;
}

//�ر��¼�
bool CMissionStock::OnEventMissionShut(BYTE cbShutReason)
{
	//���ñ���
	m_wDataSize=0;
	m_StockAction=SAction_NULL;
	ZeroMemory(m_cbDataBuffer,sizeof(m_cbDataBuffer));

	//���ÿؼ�
	if(m_pIStockOperateCenter!=NULL)
	{
		m_pIStockOperateCenter->OnEventEnableControls(TRUE);
	}

	return true;
}

//��ȡ�¼�
bool CMissionStock::OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize)
{
	WORD wCmd = LOCMMD(Command.wMainCmdID);
	if (wCmd ==MDM_GP_STOCK_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_STOCK_OPERATE_RESULT:		//�������
			{
				//����У��
				ASSERT(wDataSize<=sizeof(CMD_GP_StockOperateResult));
				if(wDataSize>sizeof(CMD_GP_StockOperateResult)) return false;

				//��ȡ����
				auto pStockOperateResult = (CMD_GP_StockOperateResult*)pData;
				
				//�ӿڻص�
				if(m_pIStockOperateCenter!=NULL)
				{
					m_pIStockOperateCenter->OnEventEnableControls(TRUE);
					m_pIStockOperateCenter->OnEventStockOperateResult(pStockOperateResult->wSubCommdID, pStockOperateResult->wStockCount, pStockOperateResult->StockList);
				}

				break;
			}
		case SUB_GP_OPERATE_FAILURE:	//����ʧ��
			{
				//����У��
				ASSERT(sizeof(CMD_GP_OperateFailure)>=wDataSize);
				if(sizeof(CMD_GP_OperateFailure)<wDataSize) return false;

				//��ȡ����
				CMD_GP_OperateFailure * pOperateFailure = (CMD_GP_OperateFailure *)pData;

				//�ؼ�����
				if(m_pIStockOperateCenter!=NULL)
				{
					m_pIStockOperateCenter->OnEventEnableControls(TRUE);
				}

				//������ʾ
				CInformation Information;
				Information.ShowMessageBox(pOperateFailure->szDescribeString,MB_ICONERROR);

				break;
		}
		}
		
		CMissionManager* pMissionManager = GetMissionManager();
		pMissionManager->ConcludeMissionItem(this);
		return true;
	}
	else if (wCmd == MDM_GG_GATE_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
			case SUB_GG_S_LINK_SUCCESS:		//���������
			{
				//��ȡ����
				if (m_StockAction == SAction_QueryStockInfo)
				{
					//��������
					ASSERT(GetMissionManager() != NULL);
					GetMissionManager()->SendData(MAKECMMD(MDM_GP_STOCK_SERVICE, SERVICE_MODULE_LOGON), SUB_GP_QUERY_STOCK_INFO, m_cbDataBuffer, m_wDataSize);
				}

				//��Ӳ���
				if (m_StockAction == SAction_AppendStockInfo)
				{
					//��������
					ASSERT(GetMissionManager() != NULL);
					GetMissionManager()->SendData(MAKECMMD(MDM_GP_STOCK_SERVICE, SERVICE_MODULE_LOGON), SUB_GP_APPEND_STOCK_INFO, m_cbDataBuffer, m_wDataSize);
				}

				//�޸Ĳ���
				if (m_StockAction == SAction_ModifyStockInfo)
				{
					//��������
					ASSERT(GetMissionManager() != NULL);
					GetMissionManager()->SendData(MAKECMMD(MDM_GP_STOCK_SERVICE, SERVICE_MODULE_LOGON), SUB_GP_MODIFY_STOCK_INFO, m_cbDataBuffer, m_wDataSize);
				}

				//ɾ������
				if (m_StockAction == SAction_DeleteStockInfo)
				{
					//��������
					ASSERT(GetMissionManager() != NULL);
					GetMissionManager()->SendData(MAKECMMD(MDM_GP_STOCK_SERVICE, SERVICE_MODULE_LOGON), SUB_GP_DELETE_STOCK_INFO, m_cbDataBuffer, m_wDataSize);
				}

				//�������
				if (m_StockAction == SAction_AdjustStockScore)
				{
					//��������
					ASSERT(GetMissionManager() != NULL);
					GetMissionManager()->SendData(MAKECMMD(MDM_GP_STOCK_SERVICE, SERVICE_MODULE_LOGON), SUB_GP_ADJUST_STOCK_SCORE, m_cbDataBuffer, m_wDataSize);
				}

				//�����ʽ�
				if (m_StockAction == SAction_AdjustJackpotScore)
				{
					//��������
					ASSERT(GetMissionManager() != NULL);
					GetMissionManager()->SendData(MAKECMMD(MDM_GP_STOCK_SERVICE, SERVICE_MODULE_LOGON), SUB_GP_ADJUST_JACKPOT_SCORE, m_cbDataBuffer, m_wDataSize);
				}

				//���ñ���
				m_wDataSize = 0;
				m_StockAction = SAction_NULL;
				ZeroMemory(m_cbDataBuffer, sizeof(m_cbDataBuffer));

				return true;
			}
			case SUB_GG_S_LINK_FAILURE:
			{

			}
			case SUB_GG_S_LINK_SHUTDOWN:
			{

			}
		}
	}

	return true;
}

//���ýӿ�
VOID CMissionStock::SetStockOperateCenter(IStockOperateCenter * pIStockOperateCenter)
{
	//����У��
	ASSERT(pIStockOperateCenter !=NULL);

	//���ýӿ�
	m_pIStockOperateCenter= pIStockOperateCenter;
}

//��ȡ����
VOID CMissionStock::QueryStockInfo(LPCTSTR pszKindIDList)
{
	//��ȡ����
	auto pQueryParameter = (CMD_GP_QueryStockInfo*)m_cbDataBuffer;

	//���ñ���
	m_StockAction=SAction_QueryStockInfo;
	m_wDataSize=sizeof(CMD_GP_QueryStockInfo);
	StringCchCopy(pQueryParameter->szKindID, CountArray(pQueryParameter->szKindID) ,pszKindIDList);

	//��������
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);

	//���ÿؼ�
	if(m_pIStockOperateCenter!=NULL)
	{
		m_pIStockOperateCenter->OnEventEnableControls(FALSE);
	}

	return;
}

//ɾ������
VOID CMissionStock::DeleteStockInfo(WORD wStockID)
{
	//��ȡ����
	auto pDeleteStockInfo = (CMD_GP_DeleteStockInfo*)m_cbDataBuffer;

	//���ñ���
	pDeleteStockInfo->wStockID = wStockID;
	m_StockAction=SAction_DeleteStockInfo;
	m_wDataSize=sizeof(CMD_GP_DeleteStockInfo);

	//��������
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);

	//���ÿؼ�
	if(m_pIStockOperateCenter!=NULL)
	{
		m_pIStockOperateCenter->OnEventEnableControls(FALSE);
	}

	return;
}

//�޸Ĳ���
VOID CMissionStock::ModifyStockInfo(tagStockInfo* pStockInfo)
{
	//��ȡ����
	auto pModifyStockInfo = (CMD_GP_ModifyStockInfo*)m_cbDataBuffer;

	//���ñ���
	m_StockAction=SAction_ModifyStockInfo;
	m_wDataSize=sizeof(CMD_GP_ModifyStockInfo);
	CopyMemory(&pModifyStockInfo->StockInfo, pStockInfo,sizeof(pModifyStockInfo->StockInfo));

	//��������
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);

	//���ÿؼ�
	if(m_pIStockOperateCenter!=NULL)
	{
		m_pIStockOperateCenter->OnEventEnableControls(FALSE);
	}

	return;
}

//��Ӳ���
VOID CMissionStock::AppendStockInfo(tagStockInfo* pStockInfo)
{
	//��ȡ����
	auto pAppendStockInfo = (CMD_GP_AppendStockInfo*)m_cbDataBuffer;

	//���ñ���
	m_StockAction=SAction_AppendStockInfo;
	m_wDataSize=sizeof(CMD_GP_AppendStockInfo);
	CopyMemory(&pAppendStockInfo->StockInfo, pStockInfo,sizeof(pAppendStockInfo->StockInfo));

	//��������
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);

	//���ÿؼ�
	if(m_pIStockOperateCenter!=NULL)
	{
		m_pIStockOperateCenter->OnEventEnableControls(FALSE);
	}

	return;
}

//�������
VOID CMissionStock::AdjustStockScore(WORD wStockID, SCORE lChangeScore)
{
	//��ȡ����
	auto pAdjustStockScore = (CMD_GP_AdjustStockScore*)m_cbDataBuffer;

	//���ñ���
	m_StockAction = SAction_AdjustStockScore;
	m_wDataSize = sizeof(CMD_GP_AdjustStockScore);

	//���ñ���
	pAdjustStockScore->wStockID = wStockID;
	pAdjustStockScore->lChangeScore = lChangeScore;

	//��������
	CMissionManager* pMissionManager = GetMissionManager();
	if (pMissionManager != NULL) pMissionManager->AvtiveMissionItem(this, false);

	//���ÿؼ�
	if (m_pIStockOperateCenter != NULL)
	{
		m_pIStockOperateCenter->OnEventEnableControls(FALSE);
	}

	return;
}

//�����ʽ�
VOID CMissionStock::AdjustJackpotScore(WORD wStockID, WORD wLevelID, SCORE lChangeScore)
{
	//��ȡ����
	auto pAdjustJackpotScore = (CMD_GP_AdjustJackpotScore*)m_cbDataBuffer;

	//���ñ���
	m_StockAction = SAction_AdjustJackpotScore;
	m_wDataSize = sizeof(CMD_GP_AdjustJackpotScore);

	//���ñ���
	pAdjustJackpotScore->wStockID = wStockID;
	pAdjustJackpotScore->wLevelID = wLevelID;
	pAdjustJackpotScore->lChangeScore = lChangeScore;

	//��������
	CMissionManager* pMissionManager = GetMissionManager();
	if (pMissionManager != NULL) pMissionManager->AvtiveMissionItem(this, false);

	//���ÿؼ�
	if (m_pIStockOperateCenter != NULL)
	{
		m_pIStockOperateCenter->OnEventEnableControls(FALSE);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////
