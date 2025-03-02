#include "StdAfx.h"
#include "Resource.h"
#include "DlgManagerCmd.h"
#include "DispatchManager.h"
//#include "DlgInformation.h"

//////////////////////////////////////////////////////////////////////////////////

//宏定义
#define COLOR_FRAME					RGB(239,249,255)					//填充颜色
#define COLOR_BORAD					RGB(170,170,170)					//边框颜色

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgManagerCmd, CDlgOptionItem)
	//ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BT_EXECUTE_CMD, OnBnClickedBtExecuteCmd)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgManagerCmd::CDlgManagerCmd() : CDlgOptionItem(IDD_CUSTOM_OPTION)
{
	//设置变量
	ZeroMemory(&m_CommandCode, sizeof(m_CommandCode));
	return;
}

//析构函数
CDlgManagerCmd::~CDlgManagerCmd()
{
}

//控件绑定
VOID CDlgManagerCmd::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

//配置函数
BOOL CDlgManagerCmd::OnInitDialog()
{
	__super::OnInitDialog();

	//构造标题
	TCHAR szServerTitle[128] = TEXT("管理命令");
	//设置标题
	SetWindowText(szServerTitle);

	return TRUE;
}

//控件使能
VOID CDlgManagerCmd::EnableControls(BOOL bEnabled)
{
	//使能设置
	GetDlgItem(IDC_BT_EXECUTE_CMD)->EnableWindow(bEnabled);
}

//构造数据
WORD CDlgManagerCmd::ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize)
{
	//参数校验
	ASSERT(wBufferSize >= sizeof(WORD) + sizeof(m_CommandCode));
	if (wBufferSize < sizeof(WORD) + sizeof(m_CommandCode)) return 0;

	//设置命令
	*(WORD*)cbBuffer = SUB_CMD_EXECUTE_CMD;

	//拷贝数据
	CopyMemory(cbBuffer + sizeof(WORD), &m_CommandCode, sizeof(m_CommandCode));

	return sizeof(WORD) + sizeof(m_CommandCode);
}

//编辑子项
VOID CDlgManagerCmd::OnBnClickedBtExecuteCmd()
{
	//变量定义
	DWORD dwCommandCode = 0;

	//加载游戏版本
	if (((CButton*)GetDlgItem(IDC_LOAD_GAME_VERSION))->GetCheck() == TRUE)
	{
		dwCommandCode |= CMD_LOAD_GAME_VERSION;
	}
	else
	{
		dwCommandCode &= ~CMD_LOAD_GAME_VERSION;
	}

	//加载平台版本
	if (((CButton*)GetDlgItem(IDC_LOAD_PLAZA_VERSION))->GetCheck() == TRUE)
	{
		dwCommandCode |= CMD_LOAD_PLAZA_VERSION;
	}
	else
	{
		dwCommandCode &= ~CMD_LOAD_PLAZA_VERSION;
	}

	//加载开房配置
	if (((CButton*)GetDlgItem(IDC_LOAD_CREATE_OPTION))->GetCheck() == TRUE)
	{
		dwCommandCode |= CMD_LOAD_CREATE_OPTION;
	}
	else
	{
		dwCommandCode &= ~CMD_LOAD_CREATE_OPTION;
	}

	//加载比赛配置
	if (((CButton*)GetDlgItem(IDC_LOAD_MATCH_OPTION))->GetCheck() == TRUE)
	{
		dwCommandCode |= CMD_LOAD_MATCH_OPTION;
	}
	else
	{
		dwCommandCode &= ~CMD_LOAD_MATCH_OPTION;
	}

	//加载允许地址
	if (((CButton*)GetDlgItem(IDC_LOAD_ALLOW_ADDRESS))->GetCheck() == TRUE)
	{
		dwCommandCode |= CMD_LOAD_ALLOW_ADDRESS;
	}
	else
	{
		dwCommandCode &= ~CMD_LOAD_ALLOW_ADDRESS;
	}

	//加载敏感词汇
	if (((CButton*)GetDlgItem(IDC_LOAD_FORBID_WORDS))->GetCheck() == TRUE)
	{
		dwCommandCode |= CMD_LOAD_FORBID_WORDS;
	}
	else
	{
		dwCommandCode &= ~CMD_LOAD_FORBID_WORDS;
	}

	//加载茶馆配置
	if (((CButton*)GetDlgItem(IDC_LOAD_GROUP_OPTION))->GetCheck() == TRUE)
	{
		dwCommandCode |= CMD_LOAD_GROUP_OPTION;
	}
	else
	{
		dwCommandCode &= ~CMD_LOAD_GROUP_OPTION;
	}

	//打印调试信息
	if (((CButton*)GetDlgItem(IDC_OUTPUT_DEBUG_INFO))->GetCheck() == TRUE)
	{
		dwCommandCode |= CMD_OUTPUT_DEBUG_INFO;
	}
	else
	{
		dwCommandCode &= ~CMD_OUTPUT_DEBUG_INFO;
	}

	//约战服务
	if (((CButton*)GetDlgItem(IDC_BATTLE_SERVICE))->GetCheck() == TRUE)
	{
		//停止服务
		if (((CButton*)GetDlgItem(IDC_STOP_BATTLE_SERVICE))->GetCheck() == TRUE)
		{
			dwCommandCode |= CMD_STOP_BATTLE_SERVICE;
			dwCommandCode &= ~CMD_START_BATTLE_SERVICE;
		}

		//启动服务
		if (((CButton*)GetDlgItem(IDC_START_BATTLE_SERVICE))->GetCheck() == TRUE)
		{
			dwCommandCode |= CMD_START_BATTLE_SERVICE;
			dwCommandCode &= ~CMD_STOP_BATTLE_SERVICE;
		}
	}

	//茶馆服务
	if (((CButton*)GetDlgItem(IDC_GROUP_SERVICE))->GetCheck() == TRUE)
	{
		//停止服务
		if (((CButton*)GetDlgItem(IDC_STOP_GROUP_SERVICE))->GetCheck() == TRUE)
		{
			dwCommandCode |= CMD_STOP_GROUP_SERVICE;
			dwCommandCode &= ~CMD_START_GROUP_SERVICE;
		}

		//启动服务
		if (((CButton*)GetDlgItem(IDC_START_GROUP_SERVICE))->GetCheck() == TRUE)
		{
			dwCommandCode |= CMD_START_GROUP_SERVICE;
			dwCommandCode &= ~CMD_STOP_GROUP_SERVICE;
		}
	}

	//比赛服务
	if (((CButton*)GetDlgItem(IDC_MATCH_SERVICE))->GetCheck() == TRUE)
	{
		//停止服务
		if (((CButton*)GetDlgItem(IDC_STOP_MATCH_SERVICE))->GetCheck() == TRUE)
		{
			dwCommandCode |= CMD_STOP_MATCH_SERVICE;
			dwCommandCode &= ~CMD_START_MATCH_SERVICE;
		}

		//启动服务
		if (((CButton*)GetDlgItem(IDC_START_MATCH_SERVICE))->GetCheck() == TRUE)
		{
			dwCommandCode |= CMD_START_MATCH_SERVICE;
			dwCommandCode &= ~CMD_STOP_MATCH_SERVICE;
		}
	}

	//配置判断
	if (dwCommandCode == 0)
	{
		AfxMessageBox(TEXT("请选择您要执行的命令！"));
		return;
	}

	//设置结构
	m_CommandCode.dwCommandCode = dwCommandCode;

	//发送事件
	CDispatchManager* pDispatchManager = CDispatchManager::GetInstance();
	if (pDispatchManager != NULL) pDispatchManager->SendEventInform(EVENT_COMMIT_MODIFY, 0L);

	//__super::OnOK();
}

//////////////////////////////////////////////////////////////////////////////////
