#include "StdAfx.h"
#include "Resource.h"
#include "DispatchManager.h"
#include "DlgManagerOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//��������
#define MIN_INOUTINTERVAL			30*60								//��С���
#define MAX_INOUTINTERVAL			12*3600								//�����

//////////////////////////////////////////////////////////////////////////
//��Ϣӳ��

BEGIN_MESSAGE_MAP(CDlgOptionItem, CDialog)
	ON_WM_SIZE()	
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerOption,CDlgOptionItem)

	//ϵͳ��Ϣ
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BT_MODIFY, OnBnClickedBtModify)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgCustomOption,CDlgOptionItem)	
	ON_BN_CLICKED(IDC_BT_EXECUTE_CMD, OnBnClickedBtExecuteCmd)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgOptionItem::CDlgOptionItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
}

//��������
CDlgOptionItem::~CDlgOptionItem()
{
}

//��ʾ����
VOID CDlgOptionItem::ShowViewItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//��������
	if (m_hWnd==NULL) CreateViewItem(rcRect,pParentWnd);

	//��ʾ����
	ShowWindow(SW_SHOW);
}

//��������
VOID CDlgOptionItem::CreateViewItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//��������
	BOOL bSuccess=Create(m_lpszTemplateName,pParentWnd);
	SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);
}

//λ����Ϣ
VOID CDlgOptionItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerOption::CDlgServerOption() : CDlgOptionItem(IDD_SERVER_OPTION)
{
}

//��������
CDlgServerOption::~CDlgServerOption()
{
}

//��ʼ����
BOOL CDlgServerOption::OnInitDialog()
{
	__super::OnInitDialog();

	//��ʼ���ؼ�
	InitControls();

	return TRUE;  
}

//��������
VOID CDlgServerOption::DoDataExchange(CDataExchange* pDX)
{

	__super::DoDataExchange(pDX);
}

//�ؼ�ʹ��
VOID CDlgServerOption::EnableControls(BOOL bEnabled)
{
	//ʹ������
	GetDlgItem(IDC_BT_MODIFY)->EnableWindow(bEnabled);
}

//��������
WORD CDlgServerOption::ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize)
{
	//����У��
	ASSERT (wBufferSize>=sizeof(WORD)+sizeof(m_ModifyRule));
	if (wBufferSize<sizeof(WORD)+sizeof(m_ModifyRule)) return 0;

	//��������
	*(WORD *)cbBuffer=SUB_CMD_MODIFY_RULE;

	//��������
	CopyMemory(cbBuffer+sizeof(WORD),&m_ModifyRule,sizeof(m_ModifyRule));	

	return sizeof(WORD)+sizeof(m_ModifyRule);
}

//��ʼ���ؼ�
VOID CDlgServerOption::InitControls()
{	
}

//�༭����
VOID CDlgServerOption::OnBnClickedBtModify()
{
	//��������
	DWORD dwTableRuleAdd=0,dwTableRuleDel=0;
	DWORD dwServerRuleAdd=0,dwServerRuleDel=0;

	//��������
	ZeroMemory(&m_ModifyRule,sizeof(m_ModifyRule));

	//����ռ��
	if (((CButton*)GetDlgItem(IDC_ANDROID_SIMULATE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_ANDROID_SIMULATE))->GetCheck()==TRUE)
			dwServerRuleDel|= SR_ALLOW_ROBOT_SIMULATE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_ANDROID_SIMULATE))->GetCheck()==TRUE)
			dwServerRuleAdd|= SR_ALLOW_ROBOT_SIMULATE;
	}

	//�������
	if (((CButton*)GetDlgItem(IDC_ANDROID_ATTEND))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_ANDROID_ATTEND))->GetCheck()==TRUE)
			dwServerRuleDel|= SR_ALLOW_ROBOT_ATTEND;
		if (((CButton*)GetDlgItem(IDC_ALLOW_ANDROID_ATTEND))->GetCheck()==TRUE)
			dwServerRuleAdd|= SR_ALLOW_ROBOT_ATTEND;
	}

	//�Թ���Ϸ
	if (((CButton*)GetDlgItem(IDC_LOOKON))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_LOOKON))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_FORBID_LOOKON;
		if (((CButton*)GetDlgItem(IDC_ALLOW_LOOKON))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_FORBID_LOOKON;		
	}

	//���͵ͱ�
	if (((CButton*)GetDlgItem(IDC_BASEENSURE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_BASEENSURE))->GetCheck()==TRUE)
			dwServerRuleDel|= SR_ALLOW_LARGESS_SUBSIDY;
		if (((CButton*)GetDlgItem(IDC_ALLOW_BASEENSURE))->GetCheck()==TRUE)
			dwServerRuleAdd|= SR_ALLOW_LARGESS_SUBSIDY;
	}

	//ǿ������
	if (((CButton*)GetDlgItem(IDC_FORCE_STANDUP))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_FORCE_STANDUP))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_FORBIT_FORCE_STANDUP;
		if (((CButton*)GetDlgItem(IDC_ALLOW_FORCE_STANDUP))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_FORBIT_FORCE_STANDUP;		
	}

	//��������
	if (((CButton*)GetDlgItem(IDC_OFFLINE_STANDUP))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_OFFLINE_STANDUP))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_FORBIT_OFFLINE_STANDUP;
		if (((CButton*)GetDlgItem(IDC_ALLOW_OFFLINE_STANDUP))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_FORBIT_OFFLINE_STANDUP;		
	}

	//��¼�ɼ�
	if (((CButton*)GetDlgItem(IDC_RECORD_GAME_SCORE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_RECORD_GAME_SCORE))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_RECORD_GAME_SCORE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_RECORD_GAME_SCORE))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_RECORD_GAME_SCORE;		
	}

	//��ʱд��
	if (((CButton*)GetDlgItem(IDC_IMMEDIATE_WRITE_SCORE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_IMMEDIATE_WRITE_SCORE))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_IMMEDIATE_WRITE_SCORE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_IMMEDIATE_WRITE_SCORE))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_IMMEDIATE_WRITE_SCORE;		
	}

	//��̬����
	if (((CButton*)GetDlgItem(IDC_DYNAMIC_JOINGAME))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_DYNAMIC_JOINGAME))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_DYNAMIC_JOIN_GAME;
		if (((CButton*)GetDlgItem(IDC_ALLOW_DYNAMIC_JOINGAME))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_DYNAMIC_JOIN_GAME;		
	}

	//��������
	if (((CButton*)GetDlgItem(IDC_VOICE_CHAT))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_VOICE_CHAT))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_ALLOW_VOICE_CHAT;
		if (((CButton*)GetDlgItem(IDC_ALLOW_VOICE_CHAT))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_ALLOW_VOICE_CHAT;		
	}

	//��Ϸ¼��
	if (((CButton*)GetDlgItem(IDC_GAME_VIDEO))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_GAME_VIDEO))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_RECORD_GAME_VIDEO;
		if (((CButton*)GetDlgItem(IDC_ALLOW_GAME_VIDEO))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_RECORD_GAME_VIDEO;		
	}

	//ʤ������
	if (((CButton*)GetDlgItem(IDC_WINRATE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_WINRATE))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_WINRATE_SET;
		if (((CButton*)GetDlgItem(IDC_ALLOW_WINRATE))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_WINRATE_SET;		
	}

	//��������
	if (((CButton*)GetDlgItem(IDC_FLEERATE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_FLEERATE))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_FLEERATE_SET;
		if (((CButton*)GetDlgItem(IDC_ALLOW_FLEERATE))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_FLEERATE_SET;		
	}

	//��ַ����
	if (((CButton*)GetDlgItem(IDC_IPSET))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_IPSET))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_IP_LIMIT;
		if (((CButton*)GetDlgItem(IDC_ALLOW_IPSET))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_IP_LIMIT;		
	}

	//�ɼ�����
	if (((CButton*)GetDlgItem(IDC_SCORELIMIT))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_SCORELIMIT))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_SCORE_LIMIT;
		if (((CButton*)GetDlgItem(IDC_ALLOW_SCORELIMIT))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_SCORE_LIMIT;		
	}

	//���ܻ�ӭ����
	if (((CButton*)GetDlgItem(IDC_BLACK))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_BLACK))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_BLACKLIST_SET;
		if (((CButton*)GetDlgItem(IDC_ALLOW_BLACK))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_BLACKLIST_SET;		
	}

	//��������
	if (((CButton*)GetDlgItem(IDC_PASSWORD))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_PASSWORD))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_PASSWORD_SET;
		if (((CButton*)GetDlgItem(IDC_ALLOW_PASSWORD))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_PASSWORD_SET;		
	}	

	//�׷���ʾ
	if (((CButton*)GetDlgItem(IDC_BASE_SCORE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_BASE_SCORE))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_SHOW_BASESCORE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_BASE_SCORE))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_SHOW_BASESCORE;		
	}	
	

	//����У��
	if ((dwTableRuleAdd|dwTableRuleDel|dwServerRuleAdd|dwServerRuleDel)==0)
	{
		return;
	}

	//���ýṹ
	m_ModifyRule.dwTableRuleAdd=dwTableRuleAdd;
	m_ModifyRule.dwTableRuleDel=dwTableRuleDel;
	m_ModifyRule.dwServerRuleAdd=dwServerRuleAdd;
	m_ModifyRule.dwServerRuleDel=dwServerRuleDel;	

	//�����¼�
	CDispatchManager * pDispatchManager=CDispatchManager::GetInstance();
	if (pDispatchManager!=NULL) pDispatchManager->SendEventInform(EVENT_COMMIT_MODIFY,0L);
}

//������Ϣ
VOID CDlgServerOption::OnDestroy()
{
	__super::OnDestroy();
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgCustomOption::CDlgCustomOption() : CDlgOptionItem(IDD_CUSTOM_OPTION)
{
	//���ñ���
	ZeroMemory(&m_CommandCode,sizeof(m_CommandCode));
}

//��������
CDlgCustomOption::~CDlgCustomOption()
{
}

//��ʼ����
BOOL CDlgCustomOption::OnInitDialog()
{
	__super::OnInitDialog();

	return TRUE;
}

//��������
VOID CDlgCustomOption::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}

//�ؼ�ʹ��
VOID CDlgCustomOption::EnableControls(BOOL bEnabled)
{
	//ʹ������
	GetDlgItem(IDC_BT_EXECUTE_CMD)->EnableWindow(bEnabled);
}

//��������
WORD CDlgCustomOption::ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize)
{
	//����У��
	ASSERT (wBufferSize>=sizeof(WORD)+sizeof(m_CommandCode));
	if (wBufferSize<sizeof(WORD)+sizeof(m_CommandCode)) return 0;

	//��������
	*(WORD *)cbBuffer=SUB_CMD_EXECUTE_CMD;

	//��������
	CopyMemory(cbBuffer+sizeof(WORD),&m_CommandCode,sizeof(m_CommandCode));	

	return sizeof(WORD)+sizeof(m_CommandCode);
}

//�༭����
VOID CDlgCustomOption::OnBnClickedBtExecuteCmd()
{	
	//��������
	DWORD dwCommandCode=0;

	//������Ϸ�汾
	if (((CButton*)GetDlgItem(IDC_LOAD_GAME_VERSION))->GetCheck()==TRUE)
	{
		dwCommandCode|=CMD_LOAD_GAME_VERSION;
	}
	else
	{
		dwCommandCode&=~CMD_LOAD_GAME_VERSION;
	}

	//����ƽ̨�汾
	if (((CButton*)GetDlgItem(IDC_LOAD_PLAZA_VERSION))->GetCheck()==TRUE)
	{
		dwCommandCode|=CMD_LOAD_PLAZA_VERSION;
	}
	else
	{
		dwCommandCode&=~CMD_LOAD_PLAZA_VERSION;
	}

	//���ؿ�������
	if (((CButton*)GetDlgItem(IDC_LOAD_CREATE_OPTION))->GetCheck()==TRUE)
	{
		dwCommandCode|=CMD_LOAD_CREATE_OPTION;
	}
	else
	{
		dwCommandCode&=~CMD_LOAD_CREATE_OPTION;
	}

	//���ر�������
	if (((CButton*)GetDlgItem(IDC_LOAD_MATCH_OPTION))->GetCheck()==TRUE)
	{
		dwCommandCode|=CMD_LOAD_MATCH_OPTION;
	}
	else
	{
		dwCommandCode&=~CMD_LOAD_MATCH_OPTION;
	}

	//���������ַ
	if (((CButton*)GetDlgItem(IDC_LOAD_ALLOW_ADDRESS))->GetCheck()==TRUE)
	{
		dwCommandCode|=CMD_LOAD_ALLOW_ADDRESS;
	}
	else
	{
		dwCommandCode&=~CMD_LOAD_ALLOW_ADDRESS;
	}

	//�������дʻ�
	if (((CButton*)GetDlgItem(IDC_LOAD_FORBID_WORDS))->GetCheck()==TRUE)
	{
		dwCommandCode|=CMD_LOAD_FORBID_WORDS;
	}
	else
	{
		dwCommandCode&=~CMD_LOAD_FORBID_WORDS;
	}

	//���ز������
	if (((CButton*)GetDlgItem(IDC_LOAD_GROUP_OPTION))->GetCheck()==TRUE)
	{
		dwCommandCode|=CMD_LOAD_GROUP_OPTION;
	}
	else
	{
		dwCommandCode&=~CMD_LOAD_GROUP_OPTION;
	}

	//��ӡ������Ϣ
	if (((CButton*)GetDlgItem(IDC_OUTPUT_DEBUG_INFO))->GetCheck()==TRUE)
	{
		dwCommandCode|=CMD_OUTPUT_DEBUG_INFO;
	}
	else
	{
		dwCommandCode&=~CMD_OUTPUT_DEBUG_INFO;
	}	

	//Լս����
	if (((CButton*)GetDlgItem(IDC_BATTLE_SERVICE))->GetCheck()==TRUE)
	{
		//ֹͣ����
		if (((CButton*)GetDlgItem(IDC_STOP_BATTLE_SERVICE))->GetCheck()==TRUE)
		{
			dwCommandCode|=CMD_STOP_BATTLE_SERVICE;
			dwCommandCode&=~CMD_START_BATTLE_SERVICE;
		}

		//��������
		if (((CButton*)GetDlgItem(IDC_START_BATTLE_SERVICE))->GetCheck()==TRUE)
		{
			dwCommandCode|=CMD_START_BATTLE_SERVICE;
			dwCommandCode&=~CMD_STOP_BATTLE_SERVICE;
		}
	}

	//��ݷ���
	if (((CButton*)GetDlgItem(IDC_GROUP_SERVICE))->GetCheck()==TRUE)
	{
		//ֹͣ����
		if (((CButton*)GetDlgItem(IDC_STOP_GROUP_SERVICE))->GetCheck()==TRUE)
		{
			dwCommandCode|=CMD_STOP_GROUP_SERVICE;
			dwCommandCode&=~CMD_START_GROUP_SERVICE;
		}

		//��������
		if (((CButton*)GetDlgItem(IDC_START_GROUP_SERVICE))->GetCheck()==TRUE)
		{
			dwCommandCode|=CMD_START_GROUP_SERVICE;
			dwCommandCode&=~CMD_STOP_GROUP_SERVICE;
		}
	}

	//��������
	if (((CButton*)GetDlgItem(IDC_MATCH_SERVICE))->GetCheck()==TRUE)
	{
		//ֹͣ����
		if (((CButton*)GetDlgItem(IDC_STOP_MATCH_SERVICE))->GetCheck()==TRUE)
		{
			dwCommandCode|=CMD_STOP_MATCH_SERVICE;
			dwCommandCode&=~CMD_START_MATCH_SERVICE;
		}

		//��������
		if (((CButton*)GetDlgItem(IDC_START_MATCH_SERVICE))->GetCheck()==TRUE)
		{
			dwCommandCode|=CMD_START_MATCH_SERVICE;
			dwCommandCode&=~CMD_STOP_MATCH_SERVICE;
		}
	}

	//�����ж�
	if (dwCommandCode==0)
	{
		AfxMessageBox(TEXT("��ѡ����Ҫִ�е����"));
		return;
	}
	
	//���ýṹ
	m_CommandCode.dwCommandCode=dwCommandCode;

	//�����¼�
	CDispatchManager * pDispatchManager=CDispatchManager::GetInstance();
	if (pDispatchManager!=NULL) pDispatchManager->SendEventInform(EVENT_COMMIT_MODIFY,0L);
}

//////////////////////////////////////////////////////////////////////////