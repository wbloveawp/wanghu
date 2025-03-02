#include "StdAfx.h"
#include "Resource.h"
#include "DlgSessionItem.h"

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

BEGIN_MESSAGE_MAP(CDlgSessionItem, CDialog)
	ON_WM_SIZE()	
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgSessionOption,CDlgSessionItem)

	//系统消息
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BT_MODIFY, OnBnClickedBtModify)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgSessionCommand,CDlgSessionItem)	
	ON_BN_CLICKED(IDC_BT_EXECUTE_CMD, OnBnClickedBtExecuteCmd)
	ON_BN_CLICKED(IDC_BT_RELOAD_CONFIG, OnBnClickedBtReloadConfig)
	
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgSessionItem::CDlgSessionItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
	m_pServerListManager = NULL;
}

//析构函数
CDlgSessionItem::~CDlgSessionItem()
{
}

//切换子项
VOID CDlgSessionItem::SwitchViewItem(bool bActive)
{
	m_bActiveStatus = bActive;
}

//创建子项
VOID CDlgSessionItem::CreateViewItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//创建窗口
	BOOL bSuccess=Create(m_lpszTemplateName,pParentWnd);
	SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);
}

//位置消息
VOID CDlgSessionItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgSessionOption::CDlgSessionOption() : CDlgSessionItem(IDD_SERVER_OPTION)
{
}

//析构函数
CDlgSessionOption::~CDlgSessionOption()
{
}

//初始窗口
BOOL CDlgSessionOption::OnInitDialog()
{
	__super::OnInitDialog();

	return TRUE;  
}

//交换数据
VOID CDlgSessionOption::DoDataExchange(CDataExchange* pDX)
{
	//绑定控件
	DDX_Control(pDX, IDC_COMBO_GAME_LIST, m_cbGameList);
	DDX_Control(pDX, IDC_COMBO_SERVER_LIST, m_cbServerList);

	__super::DoDataExchange(pDX);
}

//命令消息
BOOL CDlgSessionOption::OnCommand(WPARAM wParam, LPARAM lParam)
{
	DWORD dwCommandID = HIWORD(wParam);
	UINT nCommandID = LOWORD(wParam);
	switch (dwCommandID)
	{
	case CBN_SELCHANGE:
	{
		//游戏列表
		if (m_cbGameList.m_hWnd == (HWND)lParam)
		{
			//变量定义
			CWordArray KindArray;
			for (INT i = 0; i < m_cbGameList.GetCount(); i++)
			{
				if (m_cbGameList.GetCheck(i) == static_cast<BOOL>(true))
				{
					tagItemData* pItemData = (tagItemData*)m_cbGameList.GetItemDataPtr(i);
					KindArray.Add(pItemData->dwDataPtr);
				}
			}

			//删除房间				
			while (m_cbServerList.GetCount()) m_cbServerList.DeleteString(0);

			//插入房间
			bool bInsert = false;
			POSITION Position = NULL;
			CGameServerItem* pGameServerItem = NULL;
			do
			{
				pGameServerItem = m_pServerListManager->EmunGameServerItem(Position);
				if (pGameServerItem != NULL)
				{
					//设置标识
					bInsert = false;

					//匹配类型
					for (INT_PTR i = 0; i < KindArray.GetCount(); i++)
					{
						if (pGameServerItem->m_GameServer.wKindID == KindArray[i])
						{
							bInsert = true;
							break;
						}
					}

					if (bInsert == true)
					{
						tagItemData* pItemData = (tagItemData*)m_cbServerList.GetItemDataPtr(m_cbServerList.AddString(pGameServerItem->m_GameServer.szServerName));
						if (pItemData != NULL) pItemData->dwDataPtr = pGameServerItem->m_GameServer.wServerID;						
					}

				}
			} while (Position != NULL);
		}

		return TRUE;
	}
	}

	return __super::OnCommand(wParam, lParam);
}

//读取事件
bool CDlgSessionOption::OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	if (Command.wMainCmdID == MDM_GP_MANAGER_SERVICE && Command.wSubCmdID == SUB_GP_MANAGE_SUCCESS)
	{
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("操作成功！"));

		//设置控件
		EnableControls(TRUE);

		return true;
	}

	return false;
}

//列表完成
VOID CDlgSessionOption::OnEventListFinish()
{
	//变量定义
	POSITION Position = NULL;
	CGameKindItem* pGameKindItem = NULL;

	//清空数据
	while (m_cbGameList.GetCount()) m_cbGameList.DeleteString(0);

	do
	{
		pGameKindItem=m_pServerListManager->EmunGameKindItem(Position);
		if (pGameKindItem == NULL) break;

		//插入新项
		tagItemData* pItemData = (tagItemData*)m_cbGameList.GetItemDataPtr(m_cbGameList.AddString(pGameKindItem->m_GameKind.szKindName));
		if (pItemData != NULL) pItemData->dwDataPtr = pGameKindItem->m_GameKind.wKindID;

	} while (Position!=NULL);
}

//控件使能
VOID CDlgSessionOption::EnableControls(BOOL bEnabled)
{
	GetDlgItem(IDC_BT_MODIFY)->EnableWindow(bEnabled);
}

//编辑子项
VOID CDlgSessionOption::OnBnClickedBtModify()
{
	//变量定义
	DWORD dwTableRuleAdd=0,dwTableRuleDel=0;
	DWORD dwServerRuleAdd=0,dwServerRuleDel=0;

	//清理数据
	ZeroMemory(&m_ModifyOption,sizeof(m_ModifyOption));

	//机器占座
	if (((CButton*)GetDlgItem(IDC_ROBOT_SIMULATE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_ROBOT_SIMULATE))->GetCheck()==TRUE)
			dwServerRuleDel|= SR_ALLOW_ROBOT_SIMULATE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_ROBOT_SIMULATE))->GetCheck()==TRUE)
			dwServerRuleAdd|= SR_ALLOW_ROBOT_SIMULATE;
	}

	//机器陪打
	if (((CButton*)GetDlgItem(IDC_ROBOT_ATTEND))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_ROBOT_ATTEND))->GetCheck()==TRUE)
			dwServerRuleDel|= SR_ALLOW_ROBOT_ATTEND;
		if (((CButton*)GetDlgItem(IDC_ALLOW_ROBOT_ATTEND))->GetCheck()==TRUE)
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
	m_ModifyOption.dwTableRuleAdd=dwTableRuleAdd;
	m_ModifyOption.dwTableRuleDel=dwTableRuleDel;
	m_ModifyOption.dwServerRuleAdd=dwServerRuleAdd;
	m_ModifyOption.dwServerRuleDel=dwServerRuleDel;	

	//缓冲定义
	CMD_GP_ModifyOption ModifyOption = {};

	//变量定义
	ModifyOption.ModifyOption = m_ModifyOption;

	//获取类型	
	for (INT i = 0; i < m_cbServerList.GetCount(); i++)
	{
		if (m_cbServerList.GetCheck(i) == static_cast<BOOL>(true))
		{
			//提取数据
			tagItemData* pItemData = (tagItemData*)m_cbServerList.GetItemData(i);
			ModifyOption.wServerIDList[ModifyOption.wServerCount++] = (WORD)pItemData->dwDataPtr;			
		}
	}

	//数据大小
	WORD wDataSize = sizeof(ModifyOption)-sizeof(ModifyOption.wServerIDList)+ ModifyOption.wServerCount*sizeof(ModifyOption.wServerIDList[0]);

	//发送数据
	m_pVirtualLinkItem->SendVirtualData(MDM_GP_MANAGER_SERVICE, SUB_GP_MODIFY_OPTION, &ModifyOption, wDataSize);

	//设置控件
	EnableControls(FALSE);
}

//销毁消息
VOID CDlgSessionOption::OnDestroy()
{
	__super::OnDestroy();
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgSessionCommand::CDlgSessionCommand() : CDlgSessionItem(IDD_CUSTOM_OPTION)
{
	//设置变量
	ZeroMemory(&m_CommandCode,sizeof(m_CommandCode));
}

//析构函数
CDlgSessionCommand::~CDlgSessionCommand()
{
}

//初始窗口
BOOL CDlgSessionCommand::OnInitDialog()
{
	__super::OnInitDialog();

	return TRUE;
}

//交换数据
VOID CDlgSessionCommand::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}

//读取事件
bool CDlgSessionCommand::OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	if (Command.wMainCmdID==MDM_GP_MANAGER_SERVICE && Command.wSubCmdID== SUB_GP_MANAGE_SUCCESS)
	{
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("操作成功！"));

		//更新控件
		EnableControls(TRUE);

		return true;	
	}

	return false;
}

//控件使能
VOID CDlgSessionCommand::EnableControls(BOOL bEnabled)
{
	//使能设置
	GetDlgItem(IDC_BT_EXECUTE_CMD)->EnableWindow(bEnabled);
}

//编辑子项
VOID CDlgSessionCommand::OnBnClickedBtExecuteCmd()
{	
	//变量定义
	DWORD dwCommandCode=0;
	DWORD dwCommandParam = 0;
	
	//约战开关
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

	//茶社开关
	if (((CButton*)GetDlgItem(IDC_CLUB_SERVICE))->GetCheck()==TRUE)
	{
		//停止服务
		if (((CButton*)GetDlgItem(IDC_STOP_CLUB_SERVICE))->GetCheck()==TRUE)
		{
			dwCommandCode|=CMD_STOP_CLUB_SERVICE;
			dwCommandCode&=~CMD_START_CLUB_SERVICE;
		}

		//启动服务
		if (((CButton*)GetDlgItem(IDC_START_CLUB_SERVICE))->GetCheck()==TRUE)
		{
			dwCommandCode|=CMD_START_CLUB_SERVICE;
			dwCommandCode&=~CMD_STOP_CLUB_SERVICE;
		}
	}

	//重载茶社数据
	if (((CButton*)GetDlgItem(IDC_RELOAD_CLUB_DATA))->GetCheck() == TRUE)
	{
		dwCommandCode |= CMD_RELOAD_CLUB_DATA;
	}

	//打印茶社信息
	if (((CButton*)GetDlgItem(IDC_PRINT_CLUB_INFO))->GetCheck() == TRUE)
	{
		dwCommandCode |= CMD_PRINT_CLUB_INFO;
	}

	//比赛开关
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
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("请至少选择一个服务开关！"));

		return;
	}
	
	//设置结构
	m_CommandCode.dwCommandCode=dwCommandCode;
	m_CommandCode.dwCommandParam=dwCommandParam;

	//发送数据
	m_pVirtualLinkItem->SendVirtualData(MDM_GP_MANAGER_SERVICE, SUB_GP_EXECUTE_COMMAND, &m_CommandCode, sizeof(m_CommandCode));

	//更新控件
	EnableControls(FALSE);
}

//重载配置
VOID CDlgSessionCommand::OnBnClickedBtReloadConfig()
{
	//变量定义
	DWORD dwCommandCode = 0;
	DWORD dwCommandParam = 0;

	//加载游戏列表
	if (((CButton*)GetDlgItem(IDC_LOAD_GAME_LIST))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_GAME_LIST;
	}

	//加载游戏等级
	if (((CButton*)GetDlgItem(IDC_LOAD_GAME_LEVEL))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_GAME_LEVEL;
	}

	//加载物品信息
	if (((CButton*)GetDlgItem(IDC_LOAD_GAME_GOODS))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_GAME_GOODS;
	}

	//加载机器参数
	if (((CButton*)GetDlgItem(IDC_LOAD_ROBOT_PARAM))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_ROBOT_PARAM;
	}

	//加载低保参数
	if (((CButton*)GetDlgItem(IDC_LOAD_SUBSIDY_PARAM))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_SUBSIDY_PARAM;
	}

	//加载点控参数
	if (((CButton*)GetDlgItem(IDC_LOAD_CONTROL_PARAM))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_CONTROL_PARAM;
	}

	//加载约战选项
	if (((CButton*)GetDlgItem(IDC_LOAD_BATTLE_OPTION))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_BATTLE_OPTION;
	}

	//加载游戏版本
	if (((CButton*)GetDlgItem(IDC_LOAD_GAME_VERSION))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_GAME_VERSION;
	}

	//加载大厅版本
	if (((CButton*)GetDlgItem(IDC_LOAD_PLAZA_VERSION))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_PLAZA_VERSION;
	}

	//加载敏感词汇
	if (((CButton*)GetDlgItem(IDC_LOAD_FORBID_WORDS))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_FILTER_WORDS;
	}

	//加载转账参数
	if (((CButton*)GetDlgItem(IDC_LOAD_TRANSFER_PARAM))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_TRANSFER_PARAM;
	}

	//加载转盘选项
	if (((CButton*)GetDlgItem(IDC_LOAD_TURNTABLE_CONFIG))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= OPTION_MASK_TURNTABLE;
	}

	//加载跑马灯配置
	if (((CButton*)GetDlgItem(IDC_LOAD_MARQUEE_CONFIG))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= OPTION_MASK_MARQUEE_NOTICE;
	}

	//加载库存配置
	if (((CButton*)GetDlgItem(IDC_LOAD_STOCK_CONFIG))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_GAME_STOCK;
	}

	//加载大R配置
	if (((CButton*)GetDlgItem(IDC_LOAD_SPLAYER_CONFIG))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_SPLAYER_CONTROL;
	}

	//加载游戏基础配置
	if (((CButton*)GetDlgItem(IDC_LOAD_GAME_BASIC_CONFIG))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_BASIC_CONFIG;
	}

	//加载游戏权重配置
	if (((CButton*)GetDlgItem(IDC_LOAD_GAME_WEIGHT_CONFIG))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_WEIGHT_CONFIG;
	}

	//配置判断
	if (dwCommandCode == 0)
	{
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("请选择您要重载的配置项！"));
		return;
	}

	//设置结构
	m_CommandCode.dwCommandCode = dwCommandCode;
	m_CommandCode.dwCommandParam = dwCommandParam;

	//发送数据
	m_pVirtualLinkItem->SendVirtualData(MDM_GP_MANAGER_SERVICE, SUB_GP_EXECUTE_COMMAND, &m_CommandCode, sizeof(m_CommandCode));
}

//////////////////////////////////////////////////////////////////////////