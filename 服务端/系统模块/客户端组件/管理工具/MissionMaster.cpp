#include "StdAfx.h"
#include "MissionMaster.h"
#include "PlatformFrame.h"

//////////////////////////////////////////////////////////////////////////////////

//��������
#define IDI_AVTIVE_MISSION				100								//��������
#define TIME_AVTIVE_MISSION				600000L							//��������

//////////////////////////////////////////////////////////////////////////////////
//��̬����

CMissionMaster * CMissionMaster::m_pMissionMaster=NULL;					//����ָ��

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMissionMaster, CWnd)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CMissionMaster::CMissionMaster()
{
	//�洢����
	m_wDataSize=0;
	ZeroMemory(m_cbDataBuffer,sizeof(m_cbDataBuffer));

	//���ñ���
	m_pIManagerOperateCenter=NULL;	

	//���ö���
	ASSERT(m_pMissionMaster==NULL);
	if (m_pMissionMaster==NULL) m_pMissionMaster=this;

	return;
}

//��������
CMissionMaster::~CMissionMaster()
{
	//�ͷŶ���
	ASSERT(m_pMissionMaster==this);
	if (m_pMissionMaster==this) m_pMissionMaster=NULL;

	return;
}

//�����¼�
bool CMissionMaster::OnEventMissionLink(INT nErrorCode)
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
		if(m_pIManagerOperateCenter!=NULL)
		{
			m_pIManagerOperateCenter->OnEventEnableControls(TRUE);
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
bool CMissionMaster::OnEventMissionShut(LONG cbShutReason)
{
	//���ñ���
	m_wDataSize=0;
	ZeroMemory(m_cbDataBuffer,sizeof(m_cbDataBuffer));

	//���ÿؼ�
	m_pIManagerOperateCenter->OnEventEnableControls(TRUE);	

	return true;
}

//��ȡ�¼�
bool CMissionMaster::OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize)
{
	WORD wCmd = LOCMMD(Command.wMainCmdID);
	if (wCmd==MDM_GP_MANAGER_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_MANAGE_SUCCESS:
			{
				//���ÿؼ�
				m_pIManagerOperateCenter->OnEventEnableControls(TRUE);	

				//�ر�����
				CMissionManager * pMissionManager=GetMissionManager();
				if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this);

				//��Ϣ��ʾ
				CInformation Information;
				Information.ShowMessageBox(TEXT("�����ɹ���"));
			}
		}
	}
	else if (wCmd == MDM_GG_GATE_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
			case SUB_GG_S_LINK_SUCCESS:		//���������
			{
				//��������
				ASSERT(GetMissionManager() != NULL);
				GetMissionManager()->SendData(MAKECMMD(MDM_GP_MANAGER_SERVICE, SERVICE_MODULE_LOGON), 0, m_cbDataBuffer, m_wDataSize);

				//���ñ���
				m_wDataSize = 0;
				ZeroMemory(m_cbDataBuffer, sizeof(m_cbDataBuffer));
			}
		}
	}

	return true;
}

//���ýӿ�
VOID CMissionMaster::SetManagerOperateCenter(IManagerOperateCenter * pIManagerOperateCenter)
{
	//����У��
	ASSERT(pIManagerOperateCenter!=NULL);

	//���ýӿ�
	m_pIManagerOperateCenter=pIManagerOperateCenter;
}

//��������
bool CMissionMaster::ActiveMission()
{
	//״̬�ж�
	ASSERT (m_pIManagerOperateCenter!=NULL);
	if (m_pIManagerOperateCenter==NULL) return false;

	//��������
	m_wDataSize=m_pIManagerOperateCenter->ConstructDataPacket(m_cbDataBuffer,sizeof(m_cbDataBuffer));
	if (m_wDataSize==0) return false;

	//��������
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);

	//���ÿؼ�
	m_pIManagerOperateCenter->OnEventEnableControls(FALSE);	

	return true;
}
//////////////////////////////////////////////////////////////////////////////////
