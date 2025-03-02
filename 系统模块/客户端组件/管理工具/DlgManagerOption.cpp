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

//常量定义
#define MIN_INOUTINTERVAL			30*60								//最小间隔
#define MAX_INOUTINTERVAL			12*3600								//最大间隔

//////////////////////////////////////////////////////////////////////////
//消息映射

BEGIN_MESSAGE_MAP(CDlgOptionItem, CDialog)
	ON_WM_SIZE()	
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerOption,CDlgOptionItem)

	//系统消息
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BT_MODIFY, OnBnClickedBtModify)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgCustomOption,CDlgOptionItem)	
	ON_BN_CLICKED(IDC_BT_EXECUTE_CMD, OnBnClickedBtExecuteCmd)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgOptionItem::CDlgOptionItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
}

//析构函数
CDlgOptionItem::~CDlgOptionItem()
{
}

//显示子项
VOID CDlgOptionItem::ShowViewItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//创建窗口
	if (m_hWnd==NULL) CreateViewItem(rcRect,pParentWnd);

	//显示窗口
	ShowWindow(SW_SHOW);
}

//创建子项
VOID CDlgOptionItem::CreateViewItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//创建窗口
	BOOL bSuccess=Create(m_lpszTemplateName,pParentWnd);
	SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);
}

//位置消息
VOID CDlgOptionItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgServerOption::CDlgServerOption() : CDlgOptionItem(IDD_SERVER_OPTION)
{
}

//析构函数
CDlgServerOption::~CDlgServerOption()
{
}

//初始窗口
BOOL CDlgServerOption::OnInitDialog()
{
	__super::OnInitDialog();

	//初始化控件
	InitControls();

	return TRUE;  
}

//交换数据
VOID CDlgServerOption::DoDataExchange(CDataExchange* pDX)
{

	__super::DoDataExchange(pDX);
}

//控件使能
VOID CDlgServerOption::EnableControls(BOOL bEnabled)
{
	//使能设置
	GetDlgItem(IDC_BT_MODIFY)->EnableWindow(bEnabled);
}

//构造数据
WORD CDlgServerOption::ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize)
{
	//参数校验
	ASSERT (wBufferSize>=sizeof(WORD)+sizeof(m_ModifyRule));
	if (wBufferSize<sizeof(WORD)+sizeof(m_ModifyRule)) return 0;

	//设置命令
	*(WORD *)cbBuffer=SUB_CMD_MODIFY_RULE;

	//拷贝数据
	CopyMemory(cbBuffer+sizeof(WORD),&m_ModifyRule,sizeof(m_ModifyRule));	

	return sizeof(WORD)+sizeof(m_ModifyRule);
}

//初始化控件
VOID CDlgServerOption::InitControls()
{	
}

//编辑子项
VOID CDlgServerOption::OnBnClickedBtModify()
{
	//变量定义
	DWORD dwTableRuleAdd=0,dwTableRuleDel=0;
	DWORD dwServerRuleAdd=0,dwServerRuleDel=0;

	//清理数据
	ZeroMemory(&m_ModifyRule,sizeof(m_ModifyRule));

	//机器占座
	if (((CButton*)GetDlgItem(IDC_ANDROID_SIMULATE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_ANDROID_SIMULATE))->GetCheck()==TRUE)
			dwServerRuleDel|= SR_ALLOW_ROBOT_SIMULATE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_ANDROID_SIMULATE))->GetCheck()==TRUE)
			dwServerRuleAdd|= SR_ALLOW_ROBOT_SIMULATE;
	}

	//机器陪打
	if (((CButton*)GetDlgItem(IDC_ANDROID_ATTEND))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_ANDROID_ATTEND))->GetCheck()==TRUE)
			dwServerRuleDel|= SR_ALLOW_ROBOT_ATTEND;
		if (((CButton*)GetDlgItem(IDC_ALLOW_ANDROID_ATTEND))->GetCheck()==TRUE)
			dwServerRuleAdd|= SR_ALLOW_ROBOT_ATTEND;
	}

	//旁观游戏
	if (((CButton*)GetDlgItem(IDC_LOOKON))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_LOOKON))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_FORBID_LOOKON;
		if (((CButton*)GetDlgItem(IDC_ALLOW_LOOKON))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_FORBID_LOOKON;		
	}

	//赠送低保
	if (((CButton*)GetDlgItem(IDC_BASEENSURE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_BASEENSURE))->GetCheck()==TRUE)
			dwServerRuleDel|= SR_ALLOW_LARGESS_SUBSIDY;
		if (((CButton*)GetDlgItem(IDC_ALLOW_BASEENSURE))->GetCheck()==TRUE)
			dwServerRuleAdd|= SR_ALLOW_LARGESS_SUBSIDY;
	}

	//强退起立
	if (((CButton*)GetDlgItem(IDC_FORCE_STANDUP))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_FORCE_STANDUP))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_FORBIT_FORCE_STANDUP;
		if (((CButton*)GetDlgItem(IDC_ALLOW_FORCE_STANDUP))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_FORBIT_FORCE_STANDUP;		
	}

	//掉线起立
	if (((CButton*)GetDlgItem(IDC_OFFLINE_STANDUP))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_OFFLINE_STANDUP))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_FORBIT_OFFLINE_STANDUP;
		if (((CButton*)GetDlgItem(IDC_ALLOW_OFFLINE_STANDUP))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_FORBIT_OFFLINE_STANDUP;		
	}

	//记录成绩
	if (((CButton*)GetDlgItem(IDC_RECORD_GAME_SCORE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_RECORD_GAME_SCORE))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_RECORD_GAME_SCORE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_RECORD_GAME_SCORE))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_RECORD_GAME_SCORE;		
	}

	//即时写分
	if (((CButton*)GetDlgItem(IDC_IMMEDIATE_WRITE_SCORE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_IMMEDIATE_WRITE_SCORE))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_IMMEDIATE_WRITE_SCORE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_IMMEDIATE_WRITE_SCORE))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_IMMEDIATE_WRITE_SCORE;		
	}

	//动态加入
	if (((CButton*)GetDlgItem(IDC_DYNAMIC_JOINGAME))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_DYNAMIC_JOINGAME))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_DYNAMIC_JOIN_GAME;
		if (((CButton*)GetDlgItem(IDC_ALLOW_DYNAMIC_JOINGAME))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_DYNAMIC_JOIN_GAME;		
	}

	//语音聊天
	if (((CButton*)GetDlgItem(IDC_VOICE_CHAT))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_VOICE_CHAT))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_ALLOW_VOICE_CHAT;
		if (((CButton*)GetDlgItem(IDC_ALLOW_VOICE_CHAT))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_ALLOW_VOICE_CHAT;		
	}

	//游戏录像
	if (((CButton*)GetDlgItem(IDC_GAME_VIDEO))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_GAME_VIDEO))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_RECORD_GAME_VIDEO;
		if (((CButton*)GetDlgItem(IDC_ALLOW_GAME_VIDEO))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_RECORD_GAME_VIDEO;		
	}

	//胜率设置
	if (((CButton*)GetDlgItem(IDC_WINRATE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_WINRATE))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_WINRATE_SET;
		if (((CButton*)GetDlgItem(IDC_ALLOW_WINRATE))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_WINRATE_SET;		
	}

	//逃率设置
	if (((CButton*)GetDlgItem(IDC_FLEERATE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_FLEERATE))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_FLEERATE_SET;
		if (((CButton*)GetDlgItem(IDC_ALLOW_FLEERATE))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_FLEERATE_SET;		
	}

	//地址设置
	if (((CButton*)GetDlgItem(IDC_IPSET))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_IPSET))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_IP_LIMIT;
		if (((CButton*)GetDlgItem(IDC_ALLOW_IPSET))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_IP_LIMIT;		
	}

	//成绩设置
	if (((CButton*)GetDlgItem(IDC_SCORELIMIT))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_SCORELIMIT))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_SCORE_LIMIT;
		if (((CButton*)GetDlgItem(IDC_ALLOW_SCORELIMIT))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_SCORE_LIMIT;		
	}

	//不受欢迎设置
	if (((CButton*)GetDlgItem(IDC_BLACK))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_BLACK))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_BLACKLIST_SET;
		if (((CButton*)GetDlgItem(IDC_ALLOW_BLACK))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_BLACKLIST_SET;		
	}

	//密码设置
	if (((CButton*)GetDlgItem(IDC_PASSWORD))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_PASSWORD))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_PASSWORD_SET;
		if (((CButton*)GetDlgItem(IDC_ALLOW_PASSWORD))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_PASSWORD_SET;		
	}	

	//底分显示
	if (((CButton*)GetDlgItem(IDC_BASE_SCORE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_BASE_SCORE))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_SHOW_BASESCORE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_BASE_SCORE))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_SHOW_BASESCORE;		
	}	
	

	//设置校验
	if ((dwTableRuleAdd|dwTableRuleDel|dwServerRuleAdd|dwServerRuleDel)==0)
	{
		return;
	}

	//设置结构
	m_ModifyRule.dwTableRuleAdd=dwTableRuleAdd;
	m_ModifyRule.dwTableRuleDel=dwTableRuleDel;
	m_ModifyRule.dwServerRuleAdd=dwServerRuleAdd;
	m_ModifyRule.dwServerRuleDel=dwServerRuleDel;	

	//发送事件
	CDispatchManager * pDispatchManager=CDispatchManager::GetInstance();
	if (pDispatchManager!=NULL) pDispatchManager->SendEventInform(EVENT_COMMIT_MODIFY,0L);
}

//销毁消息
VOID CDlgServerOption::OnDestroy()
{
	__super::OnDestroy();
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgCustomOption::CDlgCustomOption() : CDlgOptionItem(IDD_CUSTOM_OPTION)
{
	//设置变量
	ZeroMemory(&m_CommandCode,sizeof(m_CommandCode));
}

//析构函数
CDlgCustomOption::~CDlgCustomOption()
{
}

//初始窗口
BOOL CDlgCustomOption::OnInitDialog()
{
	__super::OnInitDialog();

	return TRUE;
}

//交换数据
VOID CDlgCustomOption::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}

//控件使能
VOID CDlgCustomOption::EnableControls(BOOL bEnabled)
{
	//使能设置
	GetDlgItem(IDC_BT_EXECUTE_CMD)->EnableWindow(bEnabled);
}

//构造数据
WORD CDlgCustomOption::ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize)
{
	//参数校验
	ASSERT (wBufferSize>=sizeof(WORD)+sizeof(m_CommandCode));
	if (wBufferSize<sizeof(WORD)+sizeof(m_CommandCode)) return 0;

	//设置命令
	*(WORD *)cbBuffer=SUB_CMD_EXECUTE_CMD;

	//拷贝数据
	CopyMemory(cbBuffer+sizeof(WORD),&m_CommandCode,sizeof(m_CommandCode));	

	return sizeof(WORD)+sizeof(m_CommandCode);
}

//编辑子项
VOID CDlgCustomOption::OnBnClickedBtExecuteCmd()
{	
	//变量定义
	DWORD dwCommandCode=0;

	//加载游戏版本
	if (((CButton*)GetDlgItem(IDC_LOAD_GAME_VERSION))->GetCheck()==TRUE)
	{
		dwCommandCode|=CMD_LOAD_GAME_VERSION;
	}
	else
	{
		dwCommandCode&=~CMD_LOAD_GAME_VERSION;
	}

	//加载平台版本
	if (((CButton*)GetDlgItem(IDC_LOAD_PLAZA_VERSION))->GetCheck()==TRUE)
	{
		dwCommandCode|=CMD_LOAD_PLAZA_VERSION;
	}
	else
	{
		dwCommandCode&=~CMD_LOAD_PLAZA_VERSION;
	}

	//加载开房配置
	if (((CButton*)GetDlgItem(IDC_LOAD_CREATE_OPTION))->GetCheck()==TRUE)
	{
		dwCommandCode|=CMD_LOAD_CREATE_OPTION;
	}
	else
	{
		dwCommandCode&=~CMD_LOAD_CREATE_OPTION;
	}

	//加载比赛配置
	if (((CButton*)GetDlgItem(IDC_LOAD_MATCH_OPTION))->GetCheck()==TRUE)
	{
		dwCommandCode|=CMD_LOAD_MATCH_OPTION;
	}
	else
	{
		dwCommandCode&=~CMD_LOAD_MATCH_OPTION;
	}

	//加载允许地址
	if (((CButton*)GetDlgItem(IDC_LOAD_ALLOW_ADDRESS))->GetCheck()==TRUE)
	{
		dwCommandCode|=CMD_LOAD_ALLOW_ADDRESS;
	}
	else
	{
		dwCommandCode&=~CMD_LOAD_ALLOW_ADDRESS;
	}

	//加载敏感词汇
	if (((CButton*)GetDlgItem(IDC_LOAD_FORBID_WORDS))->GetCheck()==TRUE)
	{
		dwCommandCode|=CMD_LOAD_FORBID_WORDS;
	}
	else
	{
		dwCommandCode&=~CMD_LOAD_FORBID_WORDS;
	}

	//加载茶馆配置
	if (((CButton*)GetDlgItem(IDC_LOAD_GROUP_OPTION))->GetCheck()==TRUE)
	{
		dwCommandCode|=CMD_LOAD_GROUP_OPTION;
	}
	else
	{
		dwCommandCode&=~CMD_LOAD_GROUP_OPTION;
	}

	//打印调试信息
	if (((CButton*)GetDlgItem(IDC_OUTPUT_DEBUG_INFO))->GetCheck()==TRUE)
	{
		dwCommandCode|=CMD_OUTPUT_DEBUG_INFO;
	}
	else
	{
		dwCommandCode&=~CMD_OUTPUT_DEBUG_INFO;
	}	

	//约战服务
	if (((CButton*)GetDlgItem(IDC_BATTLE_SERVICE))->GetCheck()==TRUE)
	{
		//停止服务
		if (((CButton*)GetDlgItem(IDC_STOP_BATTLE_SERVICE))->GetCheck()==TRUE)
		{
			dwCommandCode|=CMD_STOP_BATTLE_SERVICE;
			dwCommandCode&=~CMD_START_BATTLE_SERVICE;
		}

		//启动服务
		if (((CButton*)GetDlgItem(IDC_START_BATTLE_SERVICE))->GetCheck()==TRUE)
		{
			dwCommandCode|=CMD_START_BATTLE_SERVICE;
			dwCommandCode&=~CMD_STOP_BATTLE_SERVICE;
		}
	}

	//茶馆服务
	if (((CButton*)GetDlgItem(IDC_GROUP_SERVICE))->GetCheck()==TRUE)
	{
		//停止服务
		if (((CButton*)GetDlgItem(IDC_STOP_GROUP_SERVICE))->GetCheck()==TRUE)
		{
			dwCommandCode|=CMD_STOP_GROUP_SERVICE;
			dwCommandCode&=~CMD_START_GROUP_SERVICE;
		}

		//启动服务
		if (((CButton*)GetDlgItem(IDC_START_GROUP_SERVICE))->GetCheck()==TRUE)
		{
			dwCommandCode|=CMD_START_GROUP_SERVICE;
			dwCommandCode&=~CMD_STOP_GROUP_SERVICE;
		}
	}

	//比赛服务
	if (((CButton*)GetDlgItem(IDC_MATCH_SERVICE))->GetCheck()==TRUE)
	{
		//停止服务
		if (((CButton*)GetDlgItem(IDC_STOP_MATCH_SERVICE))->GetCheck()==TRUE)
		{
			dwCommandCode|=CMD_STOP_MATCH_SERVICE;
			dwCommandCode&=~CMD_START_MATCH_SERVICE;
		}

		//启动服务
		if (((CButton*)GetDlgItem(IDC_START_MATCH_SERVICE))->GetCheck()==TRUE)
		{
			dwCommandCode|=CMD_START_MATCH_SERVICE;
			dwCommandCode&=~CMD_STOP_MATCH_SERVICE;
		}
	}

	//配置判断
	if (dwCommandCode==0)
	{
		AfxMessageBox(TEXT("请选择您要执行的命令！"));
		return;
	}
	
	//设置结构
	m_CommandCode.dwCommandCode=dwCommandCode;

	//发送事件
	CDispatchManager * pDispatchManager=CDispatchManager::GetInstance();
	if (pDispatchManager!=NULL) pDispatchManager->SendEventInform(EVENT_COMMIT_MODIFY,0L);
}

//////////////////////////////////////////////////////////////////////////