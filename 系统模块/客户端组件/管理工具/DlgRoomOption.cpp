#include "StdAfx.h"
#include "Resource.h"
#include "DlgRoomOption.h"
#include "DispatchManager.h"
//#include "DlgInformation.h"

//////////////////////////////////////////////////////////////////////////////////

//�궨��
#define COLOR_FRAME					RGB(239,249,255)					//�����ɫ
#define COLOR_BORAD					RGB(170,170,170)					//�߿���ɫ

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgRoomOption, CDlgOptionItem)
	//ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BT_MODIFY, OnBnClickedBtModify)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgRoomOption::CDlgRoomOption() : CDlgOptionItem(IDD_SERVER_OPTION)
{
	//���ñ���
	ZeroMemory(&m_ModifyRule, sizeof(m_ModifyRule));
	return;
}

//��������
CDlgRoomOption::~CDlgRoomOption()
{
}

//�ؼ���
VOID CDlgRoomOption::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

//���ú���
BOOL CDlgRoomOption::OnInitDialog()
{
	__super::OnInitDialog();

	//�������
	TCHAR szServerTitle[128] = TEXT("��������");
	//���ñ���
	SetWindowText(szServerTitle);

	return TRUE;
}

//�ؼ�ʹ��
VOID CDlgRoomOption::EnableControls(BOOL bEnabled)
{
	//ʹ������
	GetDlgItem(IDC_BT_MODIFY)->EnableWindow(bEnabled);
}

//��������
WORD CDlgRoomOption::ConstructManagePacket(BYTE cbBuffer[], WORD wBufferSize)
{
	//����У��
	ASSERT(wBufferSize >= sizeof(WORD) + sizeof(m_ModifyRule));
	if (wBufferSize < sizeof(WORD) + sizeof(m_ModifyRule)) return 0;

	//��������
	*(WORD*)cbBuffer = SUB_CMD_MODIFY_RULE;

	//��������
	CopyMemory(cbBuffer + sizeof(WORD), &m_ModifyRule, sizeof(m_ModifyRule));

	return sizeof(WORD) + sizeof(m_ModifyRule);
}

//�༭����
VOID CDlgRoomOption::OnBnClickedBtModify()
{
	//��������
	DWORD dwTableRuleAdd = 0, dwTableRuleDel = 0;
	DWORD dwServerRuleAdd = 0, dwServerRuleDel = 0;

	//��������
	ZeroMemory(&m_ModifyRule, sizeof(m_ModifyRule));

	//����ռ��
	if (((CButton*)GetDlgItem(IDC_ANDROID_SIMULATE))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_ANDROID_SIMULATE))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_ALLOW_ROBOT_SIMULATE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_ANDROID_SIMULATE))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_ALLOW_ROBOT_SIMULATE;
	}

	//�������
	if (((CButton*)GetDlgItem(IDC_ANDROID_ATTEND))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_ANDROID_ATTEND))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_ALLOW_ROBOT_ATTEND;
		if (((CButton*)GetDlgItem(IDC_ALLOW_ANDROID_ATTEND))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_ALLOW_ROBOT_ATTEND;
	}

	//�Թ���Ϸ
	if (((CButton*)GetDlgItem(IDC_LOOKON))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_LOOKON))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_FORBID_LOOKON;
		if (((CButton*)GetDlgItem(IDC_ALLOW_LOOKON))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_FORBID_LOOKON;
	}

	//���͵ͱ�
	if (((CButton*)GetDlgItem(IDC_BASEENSURE))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_BASEENSURE))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_ALLOW_LARGESS_SUBSIDY;
		if (((CButton*)GetDlgItem(IDC_ALLOW_BASEENSURE))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_ALLOW_LARGESS_SUBSIDY;
	}

	//ǿ������
	if (((CButton*)GetDlgItem(IDC_FORCE_STANDUP))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_FORCE_STANDUP))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_FORBIT_FORCE_STANDUP;
		if (((CButton*)GetDlgItem(IDC_ALLOW_FORCE_STANDUP))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_FORBIT_FORCE_STANDUP;
	}

	//��������
	if (((CButton*)GetDlgItem(IDC_OFFLINE_STANDUP))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_OFFLINE_STANDUP))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_FORBIT_OFFLINE_STANDUP;
		if (((CButton*)GetDlgItem(IDC_ALLOW_OFFLINE_STANDUP))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_FORBIT_OFFLINE_STANDUP;
	}

	//��¼�ɼ�
	if (((CButton*)GetDlgItem(IDC_RECORD_GAME_SCORE))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_RECORD_GAME_SCORE))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_RECORD_GAME_SCORE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_RECORD_GAME_SCORE))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_RECORD_GAME_SCORE;
	}

	//��ʱд��
	if (((CButton*)GetDlgItem(IDC_IMMEDIATE_WRITE_SCORE))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_IMMEDIATE_WRITE_SCORE))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_IMMEDIATE_WRITE_SCORE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_IMMEDIATE_WRITE_SCORE))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_IMMEDIATE_WRITE_SCORE;
	}

	//��̬����
	if (((CButton*)GetDlgItem(IDC_DYNAMIC_JOINGAME))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_DYNAMIC_JOINGAME))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_DYNAMIC_JOIN_GAME;
		if (((CButton*)GetDlgItem(IDC_ALLOW_DYNAMIC_JOINGAME))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_DYNAMIC_JOIN_GAME;
	}

	//��������
	if (((CButton*)GetDlgItem(IDC_VOICE_CHAT))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_VOICE_CHAT))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_ALLOW_VOICE_CHAT;
		if (((CButton*)GetDlgItem(IDC_ALLOW_VOICE_CHAT))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_ALLOW_VOICE_CHAT;
	}

	//��Ϸ¼��
	if (((CButton*)GetDlgItem(IDC_GAME_VIDEO))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_GAME_VIDEO))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_RECORD_GAME_VIDEO;
		if (((CButton*)GetDlgItem(IDC_ALLOW_GAME_VIDEO))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_RECORD_GAME_VIDEO;
	}

	//ʤ������
	if (((CButton*)GetDlgItem(IDC_WINRATE))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_WINRATE))->GetCheck() == TRUE)
			dwTableRuleDel |= TR_FORBID_WINRATE_SET;
		if (((CButton*)GetDlgItem(IDC_ALLOW_WINRATE))->GetCheck() == TRUE)
			dwTableRuleAdd |= TR_FORBID_WINRATE_SET;
	}

	//��������
	if (((CButton*)GetDlgItem(IDC_FLEERATE))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_FLEERATE))->GetCheck() == TRUE)
			dwTableRuleDel |= TR_FORBID_FLEERATE_SET;
		if (((CButton*)GetDlgItem(IDC_ALLOW_FLEERATE))->GetCheck() == TRUE)
			dwTableRuleAdd |= TR_FORBID_FLEERATE_SET;
	}

	//��ַ����
	if (((CButton*)GetDlgItem(IDC_IPSET))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_IPSET))->GetCheck() == TRUE)
			dwTableRuleDel |= TR_FORBID_IP_LIMIT;
		if (((CButton*)GetDlgItem(IDC_ALLOW_IPSET))->GetCheck() == TRUE)
			dwTableRuleAdd |= TR_FORBID_IP_LIMIT;
	}

	//�ɼ�����
	if (((CButton*)GetDlgItem(IDC_SCORELIMIT))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_SCORELIMIT))->GetCheck() == TRUE)
			dwTableRuleDel |= TR_FORBID_SCORE_LIMIT;
		if (((CButton*)GetDlgItem(IDC_ALLOW_SCORELIMIT))->GetCheck() == TRUE)
			dwTableRuleAdd |= TR_FORBID_SCORE_LIMIT;
	}

	//���ܻ�ӭ����
	if (((CButton*)GetDlgItem(IDC_BLACK))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_BLACK))->GetCheck() == TRUE)
			dwTableRuleDel |= TR_FORBID_BLACKLIST_SET;
		if (((CButton*)GetDlgItem(IDC_ALLOW_BLACK))->GetCheck() == TRUE)
			dwTableRuleAdd |= TR_FORBID_BLACKLIST_SET;
	}

	//��������
	if (((CButton*)GetDlgItem(IDC_PASSWORD))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_PASSWORD))->GetCheck() == TRUE)
			dwTableRuleDel |= TR_FORBID_PASSWORD_SET;
		if (((CButton*)GetDlgItem(IDC_ALLOW_PASSWORD))->GetCheck() == TRUE)
			dwTableRuleAdd |= TR_FORBID_PASSWORD_SET;
	}

	//�׷���ʾ
	if (((CButton*)GetDlgItem(IDC_BASE_SCORE))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_BASE_SCORE))->GetCheck() == TRUE)
			dwTableRuleDel |= TR_FORBID_SHOW_BASESCORE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_BASE_SCORE))->GetCheck() == TRUE)
			dwTableRuleAdd |= TR_FORBID_SHOW_BASESCORE;
	}


	//����У��
	if ((dwTableRuleAdd | dwTableRuleDel | dwServerRuleAdd | dwServerRuleDel) == 0)
	{
		return;
	}

	//���ýṹ
	m_ModifyRule.dwTableRuleAdd = dwTableRuleAdd;
	m_ModifyRule.dwTableRuleDel = dwTableRuleDel;
	m_ModifyRule.dwServerRuleAdd = dwServerRuleAdd;
	m_ModifyRule.dwServerRuleDel = dwServerRuleDel;

	//�����¼�
	CDispatchManager* pDispatchManager = CDispatchManager::GetInstance();
	if (pDispatchManager != NULL) pDispatchManager->SendEventInform(EVENT_COMMIT_MODIFY, 0L);

	//__super::OnOK();
}

//������Ϣ
VOID CDlgRoomOption::OnDestroy()
{
	__super::OnDestroy();
}

//////////////////////////////////////////////////////////////////////////////////
