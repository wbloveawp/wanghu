#include "StdAfx.h"
#include "MissionRobot.h"
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

CMissionRobot * CMissionRobot::m_pMissionRobot=NULL;						//����ָ��

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMissionRobot, CWnd)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CMissionRobot::CMissionRobot()
{
	//�洢����
	m_wDataSize=0;
	ZeroMemory(m_cbDataBuffer,sizeof(m_cbDataBuffer));

	//���ñ���
	m_RobotAction = RAction_NULL;
	m_pIRobotOperateCenter=NULL;	

	//���ö���
	if (m_pMissionRobot==NULL) m_pMissionRobot=this;

	return;
}

//��������
CMissionRobot::~CMissionRobot()
{
	//�ͷŶ���
	ASSERT(m_pMissionRobot==this);
	if (m_pMissionRobot==this) m_pMissionRobot=NULL;

	return;
}

//�����¼�
bool CMissionRobot::OnEventMissionLink(INT nErrorCode)
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
		if(m_pIRobotOperateCenter!=NULL)
		{
			m_pIRobotOperateCenter->OnEventEnableControls(TRUE);
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
bool CMissionRobot::OnEventMissionShut(BYTE cbShutReason)
{
	//���ñ���
	m_wDataSize=0;
	m_RobotAction=RAction_NULL;
	ZeroMemory(m_cbDataBuffer,sizeof(m_cbDataBuffer));

	//���ÿؼ�
	if(m_pIRobotOperateCenter!=NULL)
	{
		m_pIRobotOperateCenter->OnEventEnableControls(TRUE);
	}

	return true;
}

//��ȡ�¼�
bool CMissionRobot::OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize)
{
	WORD wCmd = LOCMMD(Command.wMainCmdID);
	if (wCmd ==MDM_GP_ROBOT_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_ROBOT_OPERATE_RESULT:		//�������
			{
				//����У��
				ASSERT(wDataSize<=sizeof(CMD_GP_RobotOperateResult));
				if(wDataSize>sizeof(CMD_GP_RobotOperateResult)) return false;

				//��ȡ����
				auto pRobotOperateResult = (CMD_GP_RobotOperateResult*)pData;
				
				//�ӿڻص�
				if(m_pIRobotOperateCenter!=NULL)
				{
					m_pIRobotOperateCenter->OnEventEnableControls(TRUE);
					m_pIRobotOperateCenter->OnEventRobotOperateResult(pRobotOperateResult->wSubCommdID, pRobotOperateResult->wParameterCount, (pRobotOperateResult->RobotParameter));
				}

				break;
			}
		case SUB_GP_OPERATE_FAILURE:		//����ʧ��
			{
				//����У��
				ASSERT(sizeof(CMD_GP_OperateFailure)>=wDataSize);
				if(sizeof(CMD_GP_OperateFailure)<wDataSize) return false;

				//��ȡ����
				CMD_GP_OperateFailure * pOperateFailure = (CMD_GP_OperateFailure *)pData;

				//�ؼ�����
				if(m_pIRobotOperateCenter!=NULL)
				{
					m_pIRobotOperateCenter->OnEventEnableControls(TRUE);
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
				if (m_RobotAction == RAction_QueryParameter)
				{
					//��������
					ASSERT(GetMissionManager() != NULL);
					GetMissionManager()->SendData(MAKECMMD(MDM_GP_ROBOT_SERVICE, SERVICE_MODULE_LOGON), SUB_GP_QUERY_PARAMETER, m_cbDataBuffer, m_wDataSize);
				}

				//��Ӳ���
				if (m_RobotAction == RAction_AppendParameter)
				{
					//��������
					ASSERT(GetMissionManager() != NULL);
					GetMissionManager()->SendData(MAKECMMD(MDM_GP_ROBOT_SERVICE, SERVICE_MODULE_LOGON), SUB_GP_APPEND_PARAMETER, m_cbDataBuffer, m_wDataSize);
				}

				//�޸Ĳ���
				if (m_RobotAction == RAction_ModifyParameter)
				{
					//��������
					ASSERT(GetMissionManager() != NULL);
					GetMissionManager()->SendData(MAKECMMD(MDM_GP_ROBOT_SERVICE, SERVICE_MODULE_LOGON), SUB_GP_MODIFY_PARAMETER, m_cbDataBuffer, m_wDataSize);
				}

				//ɾ������
				if (m_RobotAction == RAction_DeleteParameter)
				{
					//��������
					ASSERT(GetMissionManager() != NULL);
					GetMissionManager()->SendData(MAKECMMD(MDM_GP_ROBOT_SERVICE, SERVICE_MODULE_LOGON), SUB_GP_DELETE_PARAMETER, m_cbDataBuffer, m_wDataSize);
				}

				//���ñ���
				m_wDataSize = 0;
				m_RobotAction = RAction_NULL;
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
VOID CMissionRobot::SetRobotOperateCenter(IRobotOperateCenter * pIRobotOperateCenter)
{
	//����У��
	ASSERT(pIRobotOperateCenter!=NULL);

	//���ýӿ�
	m_pIRobotOperateCenter=pIRobotOperateCenter;
}

//��ȡ����
VOID CMissionRobot::GetRobotParameter(WORD wServerID)
{
	//��ȡ����
	auto pGetParameter = (CMD_GP_QueryParameter *)m_cbDataBuffer;

	//���ñ���
	pGetParameter->wServerID=wServerID;
	m_RobotAction=RAction_QueryParameter;
	m_wDataSize=sizeof(CMD_GP_QueryParameter);

	//��������
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);

	//���ÿؼ�
	if(m_pIRobotOperateCenter!=NULL)
	{
		m_pIRobotOperateCenter->OnEventEnableControls(FALSE);
	}

	return;
}

//ɾ������
VOID CMissionRobot::DeleteRobotParameter(WORD wServerID, DWORD dwBatchID)
{
	//��ȡ����
	CMD_GP_DeleteParameter * pDeleteParameter = (CMD_GP_DeleteParameter *)m_cbDataBuffer;

	//���ñ���
	pDeleteParameter->wServerID=wServerID;
	pDeleteParameter->dwBatchID = dwBatchID;
	m_RobotAction=RAction_DeleteParameter;
	m_wDataSize=sizeof(CMD_GP_DeleteParameter);

	//��������
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);

	//���ÿؼ�
	if(m_pIRobotOperateCenter!=NULL)
	{
		m_pIRobotOperateCenter->OnEventEnableControls(FALSE);
	}

	return;
}

//�޸Ĳ���
VOID CMissionRobot::ModifyRobotParameter(WORD wServerID,tagRobotParameter * pRobotParameter)
{
	//��ȡ����
	CMD_GP_ModifyParameter * pModifyParameter = (CMD_GP_ModifyParameter *)m_cbDataBuffer;

	//���ñ���
	m_RobotAction=RAction_ModifyParameter;
	m_wDataSize=sizeof(CMD_GP_ModifyParameter);
	pModifyParameter->wServerID=wServerID;
	CopyMemory(&pModifyParameter->RobotParameter,pRobotParameter,sizeof(tagRobotParameter));

	//��������
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);

	//���ÿؼ�
	if(m_pIRobotOperateCenter!=NULL)
	{
		m_pIRobotOperateCenter->OnEventEnableControls(FALSE);
	}

	return;
}

//��Ӳ���
VOID CMissionRobot::AddRobotParameter(WORD wServerID,tagRobotParameter* pRobotParameter)
{
	//��ȡ����
	auto pAddParameter = (CMD_GP_AppendParameter *)m_cbDataBuffer;

	//���ñ���
	pAddParameter->wServerID=wServerID;
	m_RobotAction=RAction_AppendParameter;
	m_wDataSize=sizeof(CMD_GP_AppendParameter);
	CopyMemory(&pAddParameter->RobotParameter,pRobotParameter,sizeof(tagRobotParameter));

	//��������
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);

	//���ÿؼ�
	if(m_pIRobotOperateCenter!=NULL)
	{
		m_pIRobotOperateCenter->OnEventEnableControls(FALSE);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////
