#include "StdAfx.h"
#include "Resource.h"
#include "DlgManagerCmd.h"
#include "DispatchManager.h"
//#include "DlgInformation.h"

//////////////////////////////////////////////////////////////////////////////////

//�궨��
#define COLOR_FRAME					RGB(239,249,255)					//�����ɫ
#define COLOR_BORAD					RGB(170,170,170)					//�߿���ɫ

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgManagerCmd, CDlgOptionItem)
	//ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BT_EXECUTE_CMD, OnBnClickedBtExecuteCmd)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgManagerCmd::CDlgManagerCmd() : CDlgOptionItem(IDD_CUSTOM_OPTION)
{
	//���ñ���
	ZeroMemory(&m_CommandCode, sizeof(m_CommandCode));
	return;
}

//��������
CDlgManagerCmd::~CDlgManagerCmd()
{
}

//�ؼ���
VOID CDlgManagerCmd::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

//���ú���
BOOL CDlgManagerCmd::OnInitDialog()
{
	__super::OnInitDialog();

	//�������
	TCHAR szServerTitle[128] = TEXT("��������");
	//���ñ���
	SetWindowText(szServerTitle);

	return TRUE;
}

//�ؼ�ʹ��
VOID CDlgManagerCmd::EnableControls(BOOL bEnabled)
{
	//ʹ������
	GetDlgItem(IDC_BT_EXECUTE_CMD)->EnableWindow(bEnabled);
}

//��������
WORD CDlgManagerCmd::ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize)
{
	//����У��
	ASSERT(wBufferSize >= sizeof(WORD) + sizeof(m_CommandCode));
	if (wBufferSize < sizeof(WORD) + sizeof(m_CommandCode)) return 0;

	//��������
	*(WORD*)cbBuffer = SUB_CMD_EXECUTE_CMD;

	//��������
	CopyMemory(cbBuffer + sizeof(WORD), &m_CommandCode, sizeof(m_CommandCode));

	return sizeof(WORD) + sizeof(m_CommandCode);
}

//�༭����
VOID CDlgManagerCmd::OnBnClickedBtExecuteCmd()
{
	//��������
	DWORD dwCommandCode = 0;

	//������Ϸ�汾
	if (((CButton*)GetDlgItem(IDC_LOAD_GAME_VERSION))->GetCheck() == TRUE)
	{
		dwCommandCode |= CMD_LOAD_GAME_VERSION;
	}
	else
	{
		dwCommandCode &= ~CMD_LOAD_GAME_VERSION;
	}

	//����ƽ̨�汾
	if (((CButton*)GetDlgItem(IDC_LOAD_PLAZA_VERSION))->GetCheck() == TRUE)
	{
		dwCommandCode |= CMD_LOAD_PLAZA_VERSION;
	}
	else
	{
		dwCommandCode &= ~CMD_LOAD_PLAZA_VERSION;
	}

	//���ؿ�������
	if (((CButton*)GetDlgItem(IDC_LOAD_CREATE_OPTION))->GetCheck() == TRUE)
	{
		dwCommandCode |= CMD_LOAD_CREATE_OPTION;
	}
	else
	{
		dwCommandCode &= ~CMD_LOAD_CREATE_OPTION;
	}

	//���ر�������
	if (((CButton*)GetDlgItem(IDC_LOAD_MATCH_OPTION))->GetCheck() == TRUE)
	{
		dwCommandCode |= CMD_LOAD_MATCH_OPTION;
	}
	else
	{
		dwCommandCode &= ~CMD_LOAD_MATCH_OPTION;
	}

	//���������ַ
	if (((CButton*)GetDlgItem(IDC_LOAD_ALLOW_ADDRESS))->GetCheck() == TRUE)
	{
		dwCommandCode |= CMD_LOAD_ALLOW_ADDRESS;
	}
	else
	{
		dwCommandCode &= ~CMD_LOAD_ALLOW_ADDRESS;
	}

	//�������дʻ�
	if (((CButton*)GetDlgItem(IDC_LOAD_FORBID_WORDS))->GetCheck() == TRUE)
	{
		dwCommandCode |= CMD_LOAD_FORBID_WORDS;
	}
	else
	{
		dwCommandCode &= ~CMD_LOAD_FORBID_WORDS;
	}

	//���ز������
	if (((CButton*)GetDlgItem(IDC_LOAD_GROUP_OPTION))->GetCheck() == TRUE)
	{
		dwCommandCode |= CMD_LOAD_GROUP_OPTION;
	}
	else
	{
		dwCommandCode &= ~CMD_LOAD_GROUP_OPTION;
	}

	//��ӡ������Ϣ
	if (((CButton*)GetDlgItem(IDC_OUTPUT_DEBUG_INFO))->GetCheck() == TRUE)
	{
		dwCommandCode |= CMD_OUTPUT_DEBUG_INFO;
	}
	else
	{
		dwCommandCode &= ~CMD_OUTPUT_DEBUG_INFO;
	}

	//Լս����
	if (((CButton*)GetDlgItem(IDC_BATTLE_SERVICE))->GetCheck() == TRUE)
	{
		//ֹͣ����
		if (((CButton*)GetDlgItem(IDC_STOP_BATTLE_SERVICE))->GetCheck() == TRUE)
		{
			dwCommandCode |= CMD_STOP_BATTLE_SERVICE;
			dwCommandCode &= ~CMD_START_BATTLE_SERVICE;
		}

		//��������
		if (((CButton*)GetDlgItem(IDC_START_BATTLE_SERVICE))->GetCheck() == TRUE)
		{
			dwCommandCode |= CMD_START_BATTLE_SERVICE;
			dwCommandCode &= ~CMD_STOP_BATTLE_SERVICE;
		}
	}

	//��ݷ���
	if (((CButton*)GetDlgItem(IDC_GROUP_SERVICE))->GetCheck() == TRUE)
	{
		//ֹͣ����
		if (((CButton*)GetDlgItem(IDC_STOP_GROUP_SERVICE))->GetCheck() == TRUE)
		{
			dwCommandCode |= CMD_STOP_GROUP_SERVICE;
			dwCommandCode &= ~CMD_START_GROUP_SERVICE;
		}

		//��������
		if (((CButton*)GetDlgItem(IDC_START_GROUP_SERVICE))->GetCheck() == TRUE)
		{
			dwCommandCode |= CMD_START_GROUP_SERVICE;
			dwCommandCode &= ~CMD_STOP_GROUP_SERVICE;
		}
	}

	//��������
	if (((CButton*)GetDlgItem(IDC_MATCH_SERVICE))->GetCheck() == TRUE)
	{
		//ֹͣ����
		if (((CButton*)GetDlgItem(IDC_STOP_MATCH_SERVICE))->GetCheck() == TRUE)
		{
			dwCommandCode |= CMD_STOP_MATCH_SERVICE;
			dwCommandCode &= ~CMD_START_MATCH_SERVICE;
		}

		//��������
		if (((CButton*)GetDlgItem(IDC_START_MATCH_SERVICE))->GetCheck() == TRUE)
		{
			dwCommandCode |= CMD_START_MATCH_SERVICE;
			dwCommandCode &= ~CMD_STOP_MATCH_SERVICE;
		}
	}

	//�����ж�
	if (dwCommandCode == 0)
	{
		AfxMessageBox(TEXT("��ѡ����Ҫִ�е����"));
		return;
	}

	//���ýṹ
	m_CommandCode.dwCommandCode = dwCommandCode;

	//�����¼�
	CDispatchManager* pDispatchManager = CDispatchManager::GetInstance();
	if (pDispatchManager != NULL) pDispatchManager->SendEventInform(EVENT_COMMIT_MODIFY, 0L);

	//__super::OnOK();
}

//////////////////////////////////////////////////////////////////////////////////
