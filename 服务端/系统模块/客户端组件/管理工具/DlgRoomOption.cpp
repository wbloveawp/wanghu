#include "StdAfx.h"
#include "Resource.h"
#include "DlgRoomOption.h"
#include "DispatchManager.h"
//#include "DlgInformation.h"

//////////////////////////////////////////////////////////////////////////////////

//宏定义
#define COLOR_FRAME					RGB(239,249,255)					//填充颜色
#define COLOR_BORAD					RGB(170,170,170)					//边框颜色

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgRoomOption, CDlgOptionItem)
	//ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BT_MODIFY, OnBnClickedBtModify)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgRoomOption::CDlgRoomOption() : CDlgOptionItem(IDD_SERVER_OPTION)
{
	//设置变量
	ZeroMemory(&m_ModifyRule, sizeof(m_ModifyRule));
	return;
}

//析构函数
CDlgRoomOption::~CDlgRoomOption()
{
}

//控件绑定
VOID CDlgRoomOption::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

//配置函数
BOOL CDlgRoomOption::OnInitDialog()
{
	__super::OnInitDialog();

	//构造标题
	TCHAR szServerTitle[128] = TEXT("管理命令");
	//设置标题
	SetWindowText(szServerTitle);

	return TRUE;
}

//控件使能
VOID CDlgRoomOption::EnableControls(BOOL bEnabled)
{
	//使能设置
	GetDlgItem(IDC_BT_MODIFY)->EnableWindow(bEnabled);
}

//构造数据
WORD CDlgRoomOption::ConstructManagePacket(BYTE cbBuffer[], WORD wBufferSize)
{
	//参数校验
	ASSERT(wBufferSize >= sizeof(WORD) + sizeof(m_ModifyRule));
	if (wBufferSize < sizeof(WORD) + sizeof(m_ModifyRule)) return 0;

	//设置命令
	*(WORD*)cbBuffer = SUB_CMD_MODIFY_RULE;

	//拷贝数据
	CopyMemory(cbBuffer + sizeof(WORD), &m_ModifyRule, sizeof(m_ModifyRule));

	return sizeof(WORD) + sizeof(m_ModifyRule);
}

//编辑子项
VOID CDlgRoomOption::OnBnClickedBtModify()
{
	//变量定义
	DWORD dwTableRuleAdd = 0, dwTableRuleDel = 0;
	DWORD dwServerRuleAdd = 0, dwServerRuleDel = 0;

	//清理数据
	ZeroMemory(&m_ModifyRule, sizeof(m_ModifyRule));

	//机器占座
	if (((CButton*)GetDlgItem(IDC_ANDROID_SIMULATE))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_ANDROID_SIMULATE))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_ALLOW_ROBOT_SIMULATE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_ANDROID_SIMULATE))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_ALLOW_ROBOT_SIMULATE;
	}

	//机器陪打
	if (((CButton*)GetDlgItem(IDC_ANDROID_ATTEND))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_ANDROID_ATTEND))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_ALLOW_ROBOT_ATTEND;
		if (((CButton*)GetDlgItem(IDC_ALLOW_ANDROID_ATTEND))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_ALLOW_ROBOT_ATTEND;
	}

	//旁观游戏
	if (((CButton*)GetDlgItem(IDC_LOOKON))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_LOOKON))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_FORBID_LOOKON;
		if (((CButton*)GetDlgItem(IDC_ALLOW_LOOKON))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_FORBID_LOOKON;
	}

	//赠送低保
	if (((CButton*)GetDlgItem(IDC_BASEENSURE))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_BASEENSURE))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_ALLOW_LARGESS_SUBSIDY;
		if (((CButton*)GetDlgItem(IDC_ALLOW_BASEENSURE))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_ALLOW_LARGESS_SUBSIDY;
	}

	//强退起立
	if (((CButton*)GetDlgItem(IDC_FORCE_STANDUP))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_FORCE_STANDUP))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_FORBIT_FORCE_STANDUP;
		if (((CButton*)GetDlgItem(IDC_ALLOW_FORCE_STANDUP))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_FORBIT_FORCE_STANDUP;
	}

	//掉线起立
	if (((CButton*)GetDlgItem(IDC_OFFLINE_STANDUP))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_OFFLINE_STANDUP))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_FORBIT_OFFLINE_STANDUP;
		if (((CButton*)GetDlgItem(IDC_ALLOW_OFFLINE_STANDUP))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_FORBIT_OFFLINE_STANDUP;
	}

	//记录成绩
	if (((CButton*)GetDlgItem(IDC_RECORD_GAME_SCORE))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_RECORD_GAME_SCORE))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_RECORD_GAME_SCORE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_RECORD_GAME_SCORE))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_RECORD_GAME_SCORE;
	}

	//即时写分
	if (((CButton*)GetDlgItem(IDC_IMMEDIATE_WRITE_SCORE))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_IMMEDIATE_WRITE_SCORE))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_IMMEDIATE_WRITE_SCORE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_IMMEDIATE_WRITE_SCORE))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_IMMEDIATE_WRITE_SCORE;
	}

	//动态加入
	if (((CButton*)GetDlgItem(IDC_DYNAMIC_JOINGAME))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_DYNAMIC_JOINGAME))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_DYNAMIC_JOIN_GAME;
		if (((CButton*)GetDlgItem(IDC_ALLOW_DYNAMIC_JOINGAME))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_DYNAMIC_JOIN_GAME;
	}

	//语音聊天
	if (((CButton*)GetDlgItem(IDC_VOICE_CHAT))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_VOICE_CHAT))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_ALLOW_VOICE_CHAT;
		if (((CButton*)GetDlgItem(IDC_ALLOW_VOICE_CHAT))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_ALLOW_VOICE_CHAT;
	}

	//游戏录像
	if (((CButton*)GetDlgItem(IDC_GAME_VIDEO))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_GAME_VIDEO))->GetCheck() == TRUE)
			dwServerRuleDel |= SR_RECORD_GAME_VIDEO;
		if (((CButton*)GetDlgItem(IDC_ALLOW_GAME_VIDEO))->GetCheck() == TRUE)
			dwServerRuleAdd |= SR_RECORD_GAME_VIDEO;
	}

	//胜率设置
	if (((CButton*)GetDlgItem(IDC_WINRATE))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_WINRATE))->GetCheck() == TRUE)
			dwTableRuleDel |= TR_FORBID_WINRATE_SET;
		if (((CButton*)GetDlgItem(IDC_ALLOW_WINRATE))->GetCheck() == TRUE)
			dwTableRuleAdd |= TR_FORBID_WINRATE_SET;
	}

	//逃率设置
	if (((CButton*)GetDlgItem(IDC_FLEERATE))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_FLEERATE))->GetCheck() == TRUE)
			dwTableRuleDel |= TR_FORBID_FLEERATE_SET;
		if (((CButton*)GetDlgItem(IDC_ALLOW_FLEERATE))->GetCheck() == TRUE)
			dwTableRuleAdd |= TR_FORBID_FLEERATE_SET;
	}

	//地址设置
	if (((CButton*)GetDlgItem(IDC_IPSET))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_IPSET))->GetCheck() == TRUE)
			dwTableRuleDel |= TR_FORBID_IP_LIMIT;
		if (((CButton*)GetDlgItem(IDC_ALLOW_IPSET))->GetCheck() == TRUE)
			dwTableRuleAdd |= TR_FORBID_IP_LIMIT;
	}

	//成绩设置
	if (((CButton*)GetDlgItem(IDC_SCORELIMIT))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_SCORELIMIT))->GetCheck() == TRUE)
			dwTableRuleDel |= TR_FORBID_SCORE_LIMIT;
		if (((CButton*)GetDlgItem(IDC_ALLOW_SCORELIMIT))->GetCheck() == TRUE)
			dwTableRuleAdd |= TR_FORBID_SCORE_LIMIT;
	}

	//不受欢迎设置
	if (((CButton*)GetDlgItem(IDC_BLACK))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_BLACK))->GetCheck() == TRUE)
			dwTableRuleDel |= TR_FORBID_BLACKLIST_SET;
		if (((CButton*)GetDlgItem(IDC_ALLOW_BLACK))->GetCheck() == TRUE)
			dwTableRuleAdd |= TR_FORBID_BLACKLIST_SET;
	}

	//密码设置
	if (((CButton*)GetDlgItem(IDC_PASSWORD))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_PASSWORD))->GetCheck() == TRUE)
			dwTableRuleDel |= TR_FORBID_PASSWORD_SET;
		if (((CButton*)GetDlgItem(IDC_ALLOW_PASSWORD))->GetCheck() == TRUE)
			dwTableRuleAdd |= TR_FORBID_PASSWORD_SET;
	}

	//底分显示
	if (((CButton*)GetDlgItem(IDC_BASE_SCORE))->GetCheck() == TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_BASE_SCORE))->GetCheck() == TRUE)
			dwTableRuleDel |= TR_FORBID_SHOW_BASESCORE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_BASE_SCORE))->GetCheck() == TRUE)
			dwTableRuleAdd |= TR_FORBID_SHOW_BASESCORE;
	}


	//设置校验
	if ((dwTableRuleAdd | dwTableRuleDel | dwServerRuleAdd | dwServerRuleDel) == 0)
	{
		return;
	}

	//设置结构
	m_ModifyRule.dwTableRuleAdd = dwTableRuleAdd;
	m_ModifyRule.dwTableRuleDel = dwTableRuleDel;
	m_ModifyRule.dwServerRuleAdd = dwServerRuleAdd;
	m_ModifyRule.dwServerRuleDel = dwServerRuleDel;

	//发送事件
	CDispatchManager* pDispatchManager = CDispatchManager::GetInstance();
	if (pDispatchManager != NULL) pDispatchManager->SendEventInform(EVENT_COMMIT_MODIFY, 0L);

	//__super::OnOK();
}

//销毁消息
VOID CDlgRoomOption::OnDestroy()
{
	__super::OnDestroy();
}

//////////////////////////////////////////////////////////////////////////////////
