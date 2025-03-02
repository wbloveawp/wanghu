#include "StdAfx.h"
#include "Resource.h"
#include "DlgServerWizard.h"

//////////////////////////////////////////////////////////////////////////
//标签索引
#define TAB_SERVER_OPTION1					0
#define TAB_SERVER_OPTION2					1
#define TAB_SERVER_OPTION3					2
#define TAB_SERVER_OPTION4					3
#define TAB_SERVER_OPTION5					4

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgServerWizardItem, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerOptionItem, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerWizardStep1, CDlgServerWizardItem)
	ON_NOTIFY(NM_DBLCLK, IDC_MODULE_LIST, OnNMDblclkModuleList)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerOptionCustom, CDlgServerOptionItem)
	ON_WM_SETFOCUS()
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerWizard, CDialog)
	ON_BN_CLICKED(IDC_NEXT, OnBnClickedNext)
	ON_BN_CLICKED(IDC_FINISH, OnBnClickedFinish)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerWizardStep2, CDlgServerWizardItem)
	ON_NOTIFY(TCN_SELCHANGE, IDC_SERVER_OPTION, OnTcnSelchangeServerOption)
	ON_CBN_SELCHANGE(IDC_SERVER_TYPE, OnCbnSelchangeServerType)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgServerWizardItem::CDlgServerWizardItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
	//设置变量
	m_bSaveData=true;

	//设置变量
	m_pDlgServerWizard=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	return;
}

//析构函数
CDlgServerWizardItem::~CDlgServerWizardItem()
{
}

//保存数据
bool CDlgServerWizardItem::SaveItemData()
{
	//保存数据
	if ((m_hWnd!=NULL)&&(m_bSaveData==false))
	{
		//保存数据
		m_bSaveData=SaveInputInfo();

		//结果判断
		if (m_bSaveData==false) return false;
	}

	return true;
}

//创建向导
bool CDlgServerWizardItem::ShowWizardItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//创建窗口
	if (m_hWnd==NULL) 
	{
		//设置资源
		AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

		//创建窗口
		Create(m_lpszTemplateName,pParentWnd);
		SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

		//设置资源
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//设置变量
	m_bSaveData=false;

	//显示窗口
	ShowWindow(SW_SHOW);

	return true;
}

//确定函数
VOID CDlgServerWizardItem::OnOK() 
{ 
	//投递消息
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//取消消息
VOID CDlgServerWizardItem::OnCancel() 
{ 
	//投递消息
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//位置消息
VOID CDlgServerWizardItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//调整控件
	RectifyControl(cx,cy);

	return;
}

//////////////////////////////////////////////////////////////////////////
//房间选项类
	
//构造函数
CDlgServerOptionItem::CDlgServerOptionItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
	//设置变量
	m_pDlgServerWizard=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;
}

//析构函数
CDlgServerOptionItem::~CDlgServerOptionItem()
{
}


//确定函数
VOID CDlgServerOptionItem::OnOK()
{
	//投递消息
	//GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//取消消息
VOID CDlgServerOptionItem::OnCancel()
{
	//投递消息
	//GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//创建子项
void CDlgServerOptionItem::CreateOptionItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//设置资源
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//创建窗口
	Create(m_lpszTemplateName,pParentWnd);
	SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

	//设置资源
	AfxSetResourceHandle(GetModuleHandle(NULL));
}

//位置消息
VOID CDlgServerOptionItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//调整控件
	RectifyControl(cx,cy);
}

//////////////////////////////////////////////////////////////////////////
//房间选项1

//构造函数
CDlgServerOption1::CDlgServerOption1() : CDlgServerOptionItem(IDD_SERVER_OPTION_1)
{
}

//析构函数
CDlgServerOption1::~CDlgServerOption1()
{
}

//初始化函数
BOOL CDlgServerOption1::OnInitDialog()
{
	__super::OnInitDialog();

	//输入限制
	((CEdit *)GetDlgItem(IDC_KIND_ID))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_NODE_ID))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_SORT_ID))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_MAX_PLAYER))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EXPGAIN_RATE))->LimitText(3);

	//变量定义
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_SERVER_LEVEL);
	LPCTSTR pszServerLevel[] = { TEXT("爱好者"),TEXT("初级"),TEXT("高级"),TEXT("专业"),TEXT("高手") };

	//构造级别
	for (INT i = 0; i < CountArray(pszServerLevel); i++)
	{
		WORD wServerLevel = i + 1;
		pComboBox->SetItemData(pComboBox->InsertString(i,pszServerLevel[i]), wServerLevel);		
	}

	//选择级别
	for (INT i = 0; i < CountArray(pszServerLevel); i++)
	{
		WORD wServerLevel = (WORD)pComboBox->GetItemData(i);
		if (wServerLevel == m_pGameServiceOption->wServerLevel)
		{
			pComboBox->SetCurSel(i);
			break;
		}
	}

	//变量定义
	pComboBox = (CComboBox*)GetDlgItem(IDC_MATCH_TYPE);
	LPCTSTR pszMatchType[] = { TEXT("即时赛"),TEXT("定时赛"),TEXT("锦标赛")};
	BYTE cbMatchType[] = { MATCH_TYPE_IMMEDIATE, MATCH_TYPE_LOCKTIME, MATCH_TYPE_TOURNAMENT };

	//构造级别
	for (INT i = 0; i < CountArray(pszMatchType); i++)
	{
		pComboBox->SetItemData(pComboBox->InsertString(i, pszMatchType[i]), cbMatchType[i]);
	}

	//比赛房间
	if (m_pGameServiceOption->wServerType & GAME_GENRE_MATCH)
	{
		for (INT i = 0; i < CountArray(pszServerLevel); i++)
		{
			BYTE cbMatchType = (BYTE)pComboBox->GetItemData(i);
			if (cbMatchType == m_pGameServiceOption->cbMatchType)
			{
				pComboBox->SetCurSel(i);
				break;
			}
		}
	}	

	//房间配置
	if (m_pGameServiceOption->lCellScore!=0) SetDlgItemInt(IDC_CELL_SCORE,(LONG)SCORE_ZOOMOUT(m_pGameServiceOption->lCellScore));
	if (m_pGameServiceOption->lLessScore!=0) SetDlgItemInt(IDC_LESS_SCORE,(LONG)SCORE_ZOOMOUT(m_pGameServiceOption->lLessScore));
	if (m_pGameServiceOption->wMaxPlayer!=0) SetDlgItemInt(IDC_MAX_PLAYER,(LONG)m_pGameServiceOption->wMaxPlayer);
	if (m_pGameServiceOption->lRestrictScore!=0) SetDlgItemInt(IDC_RESTRICT_SCORE,(LONG)SCORE_ZOOMOUT(m_pGameServiceOption->lRestrictScore));
	if (m_pGameServiceOption->szServiceAddr[0]!=0) SetDlgItemText(IDC_SERVICE_ADDR,m_pGameServiceOption->szServiceAddr);
	if (m_pGameServiceOption->szServiceDomain[0]!=0) SetDlgItemText(IDC_SERVICE_DOMAIN,m_pGameServiceOption->szServiceDomain);
	if (m_pGameServiceOption->lRevenue!=0) SetDlgItemInt(IDC_GAME_REVENUE,m_pGameServiceOption->lRevenue);
	if (m_pGameServiceOption->lExpGainRate!= 0) SetDlgItemInt(IDC_EXPGAIN_RATE, m_pGameServiceOption->lExpGainRate);

	//收费模式
	BOOL bCheck = CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	if( bCheck ) ((CButton *)GetDlgItem(IDC_FEE))->SetCheck(TRUE);
	else ((CButton *)GetDlgItem(IDC_PERCENT))->SetCheck(TRUE);

	//挂接配置
	WORD wGameKindID=m_pGameServiceOption->wKindID;
	WORD wAttribKindID=m_pGameServiceAttrib->wKindID;
	if (m_pGameServiceOption->wNodeID!=0) SetDlgItemInt(IDC_NODE_ID,m_pGameServiceOption->wNodeID);
	if (m_pGameServiceOption->wSortID!=0) SetDlgItemInt(IDC_SORT_ID,m_pGameServiceOption->wSortID);
	if ((wGameKindID!=0)&&(wGameKindID!=wAttribKindID))	SetDlgItemInt(IDC_KIND_ID,m_pGameServiceOption->wKindID);

	//电脑可视
	bCheck=(m_pGameServiceOption->wVisibleMask&MASK_VISIBLE_COMPUTER)!=0; 
	((CButton *)GetDlgItem(IDC_VISIBLE_COMPUTER))->SetCheck(bCheck);

	//手机可视
	bCheck=(m_pGameServiceOption->wVisibleMask&MASK_VISIBLE_MOBILE)!=0; 
	((CButton *)GetDlgItem(IDC_VISIBLE_MOBILE))->SetCheck(bCheck);

	//H5可视
	bCheck=(m_pGameServiceOption->wVisibleMask&MASK_VISIBLE_FLASH)!=0; 
	((CButton *)GetDlgItem(IDC_VISIBLE_FLASH))->SetCheck(bCheck);

	return TRUE;
}

//保存输入
bool CDlgServerOption1::SaveInputInfo()
{
	//挂接属性
	m_pGameServiceOption->wKindID=GetDlgItemInt(IDC_KIND_ID);
	m_pGameServiceOption->wNodeID=GetDlgItemInt(IDC_NODE_ID);
	m_pGameServiceOption->wSortID=GetDlgItemInt(IDC_SORT_ID);

	//房间配置
	m_pGameServiceOption->wMaxPlayer=GetDlgItemInt(IDC_MAX_PLAYER);
	m_pGameServiceOption->lCellScore= SCORE_ZOOMIN(GetDlgItemInt(IDC_CELL_SCORE));
	m_pGameServiceOption->lLessScore= SCORE_ZOOMIN(GetDlgItemInt(IDC_LESS_SCORE));
	m_pGameServiceOption->lRestrictScore= SCORE_ZOOMIN(GetDlgItemInt(IDC_RESTRICT_SCORE));
	m_pGameServiceOption->lRevenue=GetDlgItemInt(IDC_GAME_REVENUE);
	m_pGameServiceOption->lExpGainRate =GetDlgItemInt(IDC_EXPGAIN_RATE);
	GetDlgItemText(IDC_SERVICE_ADDR,m_pGameServiceOption->szServiceAddr,CountArray(m_pGameServiceOption->szServiceAddr));
	GetDlgItemText(IDC_SERVICE_DOMAIN,m_pGameServiceOption->szServiceDomain,CountArray(m_pGameServiceOption->szServiceDomain));

	//房间类型
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_SERVER_LEVEL);
	if (pComboBox->GetCurSel() == -1)
	{
		m_pGameServiceOption->wServerLevel = 0;		
	}
	else
	{
		m_pGameServiceOption->wServerLevel = (WORD)pComboBox->GetItemData(pComboBox->GetCurSel());
	}

	//比赛类型
	if (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)
	{
		pComboBox = (CComboBox*)GetDlgItem(IDC_MATCH_TYPE);
		if (pComboBox->GetCurSel() == -1)
		{
			m_pGameServiceOption->cbMatchType = 0;
		}
		else
		{
			m_pGameServiceOption->cbMatchType = (BYTE)pComboBox->GetItemData(pComboBox->GetCurSel());
		}
	}

	//电脑可视	
	if (((CButton *)GetDlgItem(IDC_VISIBLE_COMPUTER))->GetCheck()) 
	{
		m_pGameServiceOption->wVisibleMask|=MASK_VISIBLE_COMPUTER;
	}
	else
	{
		m_pGameServiceOption->wVisibleMask&=~MASK_VISIBLE_COMPUTER;
	}

	//手机可视
	if (((CButton *)GetDlgItem(IDC_VISIBLE_MOBILE))->GetCheck()) 
	{
		m_pGameServiceOption->wVisibleMask|=MASK_VISIBLE_MOBILE;
	}
	else
	{
		m_pGameServiceOption->wVisibleMask&=~MASK_VISIBLE_MOBILE;
	}
	
	//H5可视
	if (((CButton *)GetDlgItem(IDC_VISIBLE_FLASH))->GetCheck()) 
	{
		m_pGameServiceOption->wVisibleMask|=MASK_VISIBLE_FLASH;
	}
	else
	{
		m_pGameServiceOption->wVisibleMask&=~MASK_VISIBLE_FLASH;
	}

	//收费模式
	bool bEnable = ((CButton *)GetDlgItem(IDC_FEE))->GetCheck()?true:false;
	CServerRule::SetChargeModeFee( m_pGameServiceOption->dwServerRule,bEnable );

	//比赛房间
	if (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH && m_pGameServiceOption->cbMatchType==MATCH_TYPE_NONE)
	{
		AfxMessageBox(TEXT("请选择比赛类型！"), MB_ICONERROR);
		GetDlgItem(IDC_MATCH_TYPE)->SetFocus();
		return false;
	}

	//税收比例
	if (CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule)==false)
	{
		if (m_pGameServiceOption->lRevenue>=1000||m_pGameServiceOption->lRevenue<0)
		{
			AfxMessageBox(TEXT("游戏税收值无效，请填写游戏税收值，有效范围从 0 至 999"),MB_ICONERROR);
			GetDlgItem(IDC_GAME_REVENUE)->SetFocus();
			return false;
		}
	}
	else if (m_pGameServiceOption->lRevenue<0)
	{
		AfxMessageBox(TEXT("游戏税收值无效，请填写游戏税收值，有效值必须大于等于0"),MB_ICONERROR);
		GetDlgItem(IDC_GAME_REVENUE)->SetFocus();
		return false;
	}

	return true;
}

//调整控件
VOID CDlgServerOption1::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//类型变更
VOID CDlgServerOption1::OnEventServerTypeChange(WORD wServerType)
{
	//设置控件
	GetDlgItem(IDC_MATCH_TYPE)->ShowWindow((wServerType&GAME_GENRE_MATCH)?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_STATIC_MATCH_TYPE)->ShowWindow((wServerType&GAME_GENRE_MATCH)?SW_SHOW:SW_HIDE);
}

//////////////////////////////////////////////////////////////////////////
//房间选项2

//构造函数
CDlgServerOption2::CDlgServerOption2() : CDlgServerOptionItem(IDD_SERVER_OPTION_2)
{
}

//析构函数
CDlgServerOption2::~CDlgServerOption2()
{
}

//初始化函数
BOOL CDlgServerOption2::OnInitDialog()
{
	__super::OnInitDialog();

	//最低进入会员
	if (m_pGameServiceOption->lMinEnterMember != 0)
		SetDlgItemInt(IDC_EDT_MIN_MEMBER, (UINT)m_pGameServiceOption->lMinEnterMember);
	//最高进入会员
	if (m_pGameServiceOption->lMaxEnterMember != 0)
		SetDlgItemInt(IDC_EDT_MAX_MEMEBR, (UINT)m_pGameServiceOption->lMaxEnterMember);
	//最低进入积分
	if (m_pGameServiceOption->lMinEnterScore!=0) 
		SetDlgItemInt(IDC_EDT_MIN_ENTER,(UINT)SCORE_ZOOMOUT(m_pGameServiceOption->lMinEnterScore));
	//最高进入积分
	if (m_pGameServiceOption->lMaxEnterScore!=0) 
		SetDlgItemInt(IDC_EDT_MAX_ENTER,(UINT)SCORE_ZOOMOUT(m_pGameServiceOption->lMaxEnterScore));
	

	//即时写分
	BOOL bCheck = CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_IMMEDIATE_WRITE_SCORE))->SetCheck(bCheck);

	//每局记录积分
	bCheck = CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_RECORD_GAME_SCORE))->SetCheck(bCheck);

	//记录游戏录像
	bCheck = CServerRule::IsRecordGameVideo(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_RECORD_GAME_VIDEO))->SetCheck(bCheck);

	//房间禁止公聊
	bCheck = CServerRule::IsForbidRoomChat(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_ROOM_CHAT))->SetCheck(bCheck);

	//房间禁止私聊
	bCheck = CServerRule::IsForbidRoomWisper(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_ROOM_WISPER))->SetCheck(bCheck);

	//游戏禁止公聊
	bCheck = CServerRule::IsForbidGameChat(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBIT_GAME_CHAT))->SetCheck(bCheck);

	//允许游戏机器陪打
	bCheck = CServerRule::IsAllowRobotAttend(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_ALLOW_ROBOT_ATTEND))->SetCheck(bCheck);

	//允许游戏机器占座
	bCheck = CServerRule::IsAllowRobotSimulate(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_ALLOW_ROBOT_SIMULATE))->SetCheck(bCheck);

	//桌子随游戏状态改变
	bCheck = CServerRule::IsTableEndImmediate(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_TABLE_END_IMMEDIATE))->SetCheck(bCheck);

	//动态加入游戏
	bCheck = CServerRule::IsDynamicJoinGame(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_DYNAMIC_JOIN_GAME))->SetCheck(bCheck);

	//同桌禁止私聊
	bCheck = CServerRule::IsForbidPlayChat(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBIT_PLAY_CHAT))->SetCheck(bCheck);

	//会员可聊天
	bCheck = CServerRule::IsMemberCanChat(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_MEMBER_CAN_CHAT))->SetCheck(bCheck);

	//禁止旁观
	bCheck = CServerRule::IsForbidLookon(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_LOOKON))->SetCheck(bCheck);

	//禁止发喇叭
	bCheck = CServerRule::IsForbidBugle(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_BUGLE))->SetCheck(bCheck);

	//强退禁止起立
	bCheck = CServerRule::IsForbidForceStandup(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_FORCE_STANDUP))->SetCheck(bCheck);

	//掉线超时禁止起立
	bCheck = CServerRule::IsForbidOfflineStandup(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_OFFLINE_STANDUP))->SetCheck(bCheck);	

	//隐藏用户信息
	bCheck = CServerRule::IsHideUserInfo(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_HIDE_USER_INFO))->SetCheck(bCheck);

	//允许语音聊天
	bCheck = CServerRule::IsAllowVoiceChat(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_ALLOW_VOICE_CHAT))->SetCheck(bCheck);

	//允许赠送低保
	bCheck = CServerRule::IsAllowLargessSubsidy(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_ALLOW_LARGESS_Subsidy))->SetCheck(bCheck);

	//允许协议解散
	bCheck = CServerRule::IsAllowAgreementDismiss(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_ALLOW_AGREEMENT_DISMISS))->SetCheck(bCheck);	

	//根据分数匹配用户
	bCheck = CServerRule::IsMatchUserByScore(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_MATCH_USER_BY_SCORE))->SetCheck(bCheck);	

	return TRUE;
}

//保存输入
bool CDlgServerOption2::SaveInputInfo()
{
	//变量定义
	bool bEnable=false;

	//房间条件
	m_pGameServiceOption->lMinEnterMember = GetDlgItemInt(IDC_EDT_MIN_MEMBER);
	m_pGameServiceOption->lMaxEnterMember = GetDlgItemInt(IDC_EDT_MAX_MEMEBR);
	m_pGameServiceOption->lMinEnterScore= SCORE_ZOOMIN(GetDlgItemInt(IDC_EDT_MIN_ENTER));
	m_pGameServiceOption->lMaxEnterScore= SCORE_ZOOMIN(GetDlgItemInt(IDC_EDT_MAX_ENTER));	

	//即时写分
	bEnable = ((CButton *)GetDlgItem(IDC_IMMEDIATE_WRITE_SCORE))->GetCheck()?true:false;
	CServerRule::SetImmediateWriteScore( m_pGameServiceOption->dwServerRule,bEnable );

	//每局记录积分
	bEnable = ((CButton *)GetDlgItem(IDC_RECORD_GAME_SCORE))->GetCheck()?true:false;
	CServerRule::SetRecordGameScore( m_pGameServiceOption->dwServerRule,bEnable );

	//记录游戏录像
	bEnable = ((CButton *)GetDlgItem(IDC_RECORD_GAME_VIDEO))->GetCheck()?true:false;
	CServerRule::SetRecordGameVideo( m_pGameServiceOption->dwServerRule,bEnable );

	//房间禁止公聊
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_ROOM_CHAT))->GetCheck()?true:false;
	CServerRule::SetForbidRoomChat( m_pGameServiceOption->dwServerRule,bEnable );

	//房间禁止私聊
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_ROOM_WISPER))->GetCheck()?true:false;
	CServerRule::SetForbidRoomWisper( m_pGameServiceOption->dwServerRule,bEnable );

	//游戏禁止公聊
	bEnable = ((CButton *)GetDlgItem(IDC_FORBIT_GAME_CHAT))->GetCheck()?true:false;
	CServerRule::SetForbidGameChat( m_pGameServiceOption->dwServerRule,bEnable );

	//允许游戏机器人陪打
	bEnable = ((CButton *)GetDlgItem(IDC_ALLOW_ROBOT_ATTEND))->GetCheck()?true:false;
	CServerRule::SetAllowRobotAttend( m_pGameServiceOption->dwServerRule,bEnable );

	//允许游戏机器人占座
	bEnable = ((CButton *)GetDlgItem(IDC_ALLOW_ROBOT_SIMULATE))->GetCheck()?true:false;
	CServerRule::SetAllowRobotSimulate( m_pGameServiceOption->dwServerRule,bEnable );

	//桌子随游戏状态改变
	bEnable = ((CButton *)GetDlgItem(IDC_TABLE_END_IMMEDIATE))->GetCheck()?true:false;
	CServerRule::SetTableEndImmediate( m_pGameServiceOption->dwServerRule,bEnable );

	//动态加入游戏
	bEnable = ((CButton *)GetDlgItem(IDC_DYNAMIC_JOIN_GAME))->GetCheck()?true:false;
	CServerRule::SetDynamicJoinGame( m_pGameServiceOption->dwServerRule,bEnable );

	//同桌禁止私聊
	bEnable = ((CButton *)GetDlgItem(IDC_FORBIT_PLAY_CHAT))->GetCheck()?true:false;
	CServerRule::SetForbidPlayChat( m_pGameServiceOption->dwServerRule,bEnable );

	//会员可聊天
	bEnable = ((CButton *)GetDlgItem(IDC_MEMBER_CAN_CHAT))->GetCheck()?true:false;
	CServerRule::SetMemberCanChat( m_pGameServiceOption->dwServerRule,bEnable );

	//禁止旁观
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_LOOKON))->GetCheck()?true:false;
	CServerRule::SetForbidLookon( m_pGameServiceOption->dwServerRule,bEnable );

	//禁止发喇叭
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_BUGLE))->GetCheck()?true:false;
	CServerRule::SetForbidBugle(m_pGameServiceOption->dwServerRule,bEnable);

	//强退禁止起立
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_FORCE_STANDUP))->GetCheck()?true:false;
	CServerRule::SetForbidForceStandup(m_pGameServiceOption->dwServerRule,bEnable);

	//掉线超时禁止起立
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_OFFLINE_STANDUP))->GetCheck()?true:false;
	CServerRule::SetForbidOfflineStandup(m_pGameServiceOption->dwServerRule,bEnable);

	//隐藏用户信息
	bEnable = ((CButton *)GetDlgItem(IDC_HIDE_USER_INFO))->GetCheck()?true:false;
	CServerRule::SetHideUserInfo(m_pGameServiceOption->dwServerRule,bEnable);	

	//允许语音聊天
	bEnable = ((CButton *)GetDlgItem(IDC_ALLOW_VOICE_CHAT))->GetCheck()?true:false;
	CServerRule::SetAllowVoiceChat(m_pGameServiceOption->dwServerRule,bEnable);	

	//允许赠送低保
	bEnable = ((CButton *)GetDlgItem(IDC_ALLOW_LARGESS_Subsidy))->GetCheck()?true:false;
	CServerRule::SetAllowLargessSubsidy(m_pGameServiceOption->dwServerRule,bEnable);	

	//允许协议解散
	bEnable = ((CButton *)GetDlgItem(IDC_ALLOW_AGREEMENT_DISMISS))->GetCheck()?true:false;
	CServerRule::SetAllowAgreementDismiss(m_pGameServiceOption->dwServerRule,bEnable);	

	//根据分数匹配用户
	bEnable = ((CButton *)GetDlgItem(IDC_MATCH_USER_BY_SCORE))->GetCheck()?true:false;
	CServerRule::SetMatchUserByScore(m_pGameServiceOption->dwServerRule,bEnable);	

	//允许赠送低保
	if (CServerRule::IsAllowLargessSubsidy(m_pGameServiceOption->dwServerRule)==true &&
	  (m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_BATTLE))==0)
	{
		AfxMessageBox(TEXT("抱歉，只有财富类型房间才支持低保赠送！"),MB_ICONERROR);
		return false;
	}

	return true;
}

//调整控件
VOID CDlgServerOption2::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////
//房间选项3

//构造函数
CDlgServerOption3::CDlgServerOption3() : CDlgServerOptionItem(IDD_SERVER_OPTION_3)
{
}

//析构函数
CDlgServerOption3::~CDlgServerOption3()
{
}

//初始化函数
BOOL CDlgServerOption3::OnInitDialog()
{
	__super::OnInitDialog();

	//变量定义
	bool bCheck=false;

	//禁止胜率限制
	bCheck = CServerRule::IsForbidWinRateSet(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_WINRATE))->SetCheck(bCheck);

	//禁止逃率限制
	bCheck = CServerRule::IsForbidFleeRateSet(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_FLEERATE))->SetCheck(bCheck);

	//禁止成绩限制
	bCheck = CServerRule::IsForbidScoreLimit(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_SCORELIMIT))->SetCheck(bCheck);

	//禁止不跟不受欢迎玩家游戏的设置
	bCheck = CServerRule::IsForbidBlackListSet(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_BLACK))->SetCheck(bCheck);

	//禁止IP限制
	bCheck = CServerRule::IsForbidIPLimit(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_IPSET))->SetCheck(bCheck);

	//禁止密码设置
	bCheck = CServerRule::IsForbidPasswordSet(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_PASSWORD))->SetCheck(bCheck);

	//禁止显示底分
	bCheck = CServerRule::IsForbidShowBaseScore(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_TBLE_SHOW_BASE_SCORE))->SetCheck(bCheck);

	//禁止绑定用户
	bCheck=CBattleRule::IsForbidBindUser(m_pGameServiceOption->dwBattleRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_BIND_USER))->SetCheck(bCheck);

	//禁止发送成绩
	bCheck=CBattleRule::IsForbidSendGrade(m_pGameServiceOption->dwBattleRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_SEND_GRADE))->SetCheck(bCheck);

	//禁止写入奖励
	bCheck=CBattleRule::IsForbidBattleReward(m_pGameServiceOption->dwBattleRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_BATTLE_REWARD))->SetCheck(bCheck);

	//禁止约战记录
	bCheck=CBattleRule::IsForbidBattleRecord(m_pGameServiceOption->dwBattleRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_BATTLE_RECORD))->SetCheck(bCheck);

	//禁止同IP提醒
	bCheck=CBattleRule::IsForbidSameIPRemind(m_pGameServiceOption->dwBattleRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_SAMEIP_REMIND))->SetCheck(bCheck);
	

	return TRUE;
}

//保存输入
bool CDlgServerOption3::SaveInputInfo()
{
	//变量定义
	bool bEnable=false;

	//禁止胜率限制
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_WINRATE))->GetCheck()?true:false;
	CServerRule::SetForbidWinRateSet(m_pGameServiceOption->dwServerRule,bEnable);

	//禁止逃率限制
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_FLEERATE))->GetCheck()?true:false;
	CServerRule::SetForbidFleeRateSet(m_pGameServiceOption->dwServerRule,bEnable);

	//禁止成绩限制
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_SCORELIMIT))->GetCheck()?true:false;
	CServerRule::SetForbidScoreLimit(m_pGameServiceOption->dwServerRule,bEnable);

	//禁止不跟不受欢迎玩家游戏的设置
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_BLACK))->GetCheck()?true:false;
	CServerRule::SetForbidBlackListSet(m_pGameServiceOption->dwServerRule,bEnable);

	//禁止IP限制
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_IPSET))->GetCheck()?true:false;
	CServerRule::SetForbidIPLimit(m_pGameServiceOption->dwServerRule,bEnable);

	//禁止密码设置
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_PASSWORD))->GetCheck()?true:false;
	CServerRule::SetForbidPasswordSet(m_pGameServiceOption->dwServerRule,bEnable);

	//禁止显示底分
	bEnable = ((CButton *)GetDlgItem(IDC_TBLE_SHOW_BASE_SCORE))->GetCheck()?true:false;
	CServerRule::SetForbidShowBaseScore( m_pGameServiceOption->dwServerRule,bEnable );

	//禁止绑定用户
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_BIND_USER))->GetCheck()?true:false;
	CBattleRule::SetForbidBindUser(m_pGameServiceOption->dwBattleRule,bEnable );

	//禁止发送成绩
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_SEND_GRADE))->GetCheck()?true:false;
	CBattleRule::SetForbidSendGrade(m_pGameServiceOption->dwBattleRule,bEnable );

	//禁止写入奖励
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_BATTLE_REWARD))->GetCheck()?true:false;
	CBattleRule::SetForbidWriteReward(m_pGameServiceOption->dwBattleRule,bEnable );

	//禁止约战记录
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_BATTLE_RECORD))->GetCheck()?true:false;
	CBattleRule::SetForbidBattleRecord(m_pGameServiceOption->dwBattleRule,bEnable );

	//禁止同IP提醒
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_SAMEIP_REMIND))->GetCheck()?true:false;
	CBattleRule::SetForbidSameIPRemind(m_pGameServiceOption->dwBattleRule,bEnable );

	return true;
}

//调整控件
VOID CDlgServerOption3::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////
//房间选项4

//构造函数
CDlgServerOption4::CDlgServerOption4() : CDlgServerOptionItem(IDD_SERVER_OPTION_4)
{
}

//析构函数
CDlgServerOption4::~CDlgServerOption4()
{
}

//初始化函数
BOOL CDlgServerOption4::OnInitDialog()
{
	__super::OnInitDialog();

	//立即模式
	if (m_pGameServiceOption->cbDistributeMode==DISTRIBUTE_MODE_IMMEDIATELY)
	{
		//设置控件
		CheckRadioButton(IDC_RADIO_IMMEDIATELY_MODE,IDC_RADIO_DELAYWAIT_MODE,IDC_RADIO_IMMEDIATELY_MODE);
	}

	//延时模式
	if (m_pGameServiceOption->cbDistributeMode==DISTRIBUTE_MODE_DELAYWAIT)
	{
		//设置控件
		CheckRadioButton(IDC_RADIO_IMMEDIATELY_MODE,IDC_RADIO_DELAYWAIT_MODE,IDC_RADIO_DELAYWAIT_MODE);
	}

	//控件设置
	SetDlgItemInt(IDC_MIN_PLAYER_COUNT,m_pGameServiceOption->wMinPlayerCount);
	SetDlgItemInt(IDC_DISTRIBUTE_MINCOUNT,m_pGameServiceOption->wMinDistributeUser);
	SetDlgItemInt(IDC_DISTRIBUTE_INTERVAL,m_pGameServiceOption->wDistributeTimeSpace);
	SetDlgItemInt(IDC_PARTAKE_MIN_GAMEUSER,m_pGameServiceOption->wMinPartakeGameUser);
	SetDlgItemInt(IDC_PARTAKE_MAX_GAMEUSER,m_pGameServiceOption->wMaxPartakeGameUser);

	//同桌设置
	if ((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_SAME_TABLE)!=0)
	{
		((CButton *)GetDlgItem(IDC_ALLOW_SAME_TABLE))->SetCheck(BST_CHECKED);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_ALLOW_SAME_TABLE))->SetCheck(BST_UNCHECKED);
	}

	//地址设置
	if ((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_SAME_ADDRESS)!=0)
	{
		((CButton *)GetDlgItem(IDC_ALLOW_SAME_ADDRESS))->SetCheck(BST_CHECKED);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_ALLOW_SAME_ADDRESS))->SetCheck(BST_UNCHECKED);
	}

	//机器设置
	if ((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_SAME_MACHINE)!=0)
	{
		((CButton *)GetDlgItem(IDC_ALLOW_SAME_MACHINE))->SetCheck(BST_CHECKED);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_ALLOW_SAME_MACHINE))->SetCheck(BST_UNCHECKED);
	}

	//优先设置
	if ((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_PLAYER_FIRST)!=0)
	{
		((CButton *)GetDlgItem(IDC_ALLOW_PLAYER_FIRST))->SetCheck(BST_CHECKED);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_ALLOW_PLAYER_FIRST))->SetCheck(BST_UNCHECKED);
	}


	return TRUE;	
}

//保存输入
bool CDlgServerOption4::SaveInputInfo()
{
	//获取设置
	m_pGameServiceOption->wMinPlayerCount=GetDlgItemInt(IDC_MIN_PLAYER_COUNT);
	m_pGameServiceOption->wMinDistributeUser=GetDlgItemInt(IDC_DISTRIBUTE_MINCOUNT);
	m_pGameServiceOption->wDistributeTimeSpace=GetDlgItemInt(IDC_DISTRIBUTE_INTERVAL);
	m_pGameServiceOption->wMinPartakeGameUser=GetDlgItemInt(IDC_PARTAKE_MIN_GAMEUSER);
	m_pGameServiceOption->wMaxPartakeGameUser=GetDlgItemInt(IDC_PARTAKE_MAX_GAMEUSER);

	//立即模式
	if (((CButton *)GetDlgItem(IDC_RADIO_IMMEDIATELY_MODE))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeMode=DISTRIBUTE_MODE_IMMEDIATELY;
	}

	//延时模式
	if (((CButton *)GetDlgItem(IDC_RADIO_DELAYWAIT_MODE))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeMode=DISTRIBUTE_MODE_DELAYWAIT;
	}

	//同桌设置
	if (((CButton *)GetDlgItem(IDC_ALLOW_SAME_TABLE))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeRule|=DISTRIBUTE_SAME_TABLE;
	}
	else
	{
		m_pGameServiceOption->cbDistributeRule&=~DISTRIBUTE_SAME_TABLE;
	}

	//地址设置
	if (((CButton *)GetDlgItem(IDC_ALLOW_SAME_ADDRESS))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeRule|=DISTRIBUTE_SAME_ADDRESS;
	}
	else
	{
		m_pGameServiceOption->cbDistributeRule&=~DISTRIBUTE_SAME_ADDRESS;
	}

	//机器设置
	if (((CButton *)GetDlgItem(IDC_ALLOW_SAME_MACHINE))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeRule|=DISTRIBUTE_SAME_MACHINE;
	}
	else
	{
		m_pGameServiceOption->cbDistributeRule&=~DISTRIBUTE_SAME_MACHINE;
	}

	//优先设置
	if (((CButton *)GetDlgItem(IDC_ALLOW_PLAYER_FIRST))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeRule|=DISTRIBUTE_PLAYER_FIRST;
	}
	else
	{
		m_pGameServiceOption->cbDistributeRule&=~DISTRIBUTE_PLAYER_FIRST;
	}

	return true;
}

//调整控件
VOID CDlgServerOption4::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgServerWizardStep1::CDlgServerWizardStep1() : CDlgServerWizardItem(IDD_SERVER_WIZARD_1)
{
	//设置变量
	m_pGameModuleInfo=NULL;

	return;
}

//析构函数
CDlgServerWizardStep1::~CDlgServerWizardStep1()
{
}

//控件绑定
VOID CDlgServerWizardStep1::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROMPT, m_StaticPrompt);
	DDX_Control(pDX, IDC_MODULE_LIST, m_ModuleListControl);
}

//初始化函数
BOOL CDlgServerWizardStep1::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetDlgItemText(IDC_PROMPT,TEXT("请您从下面的游戏列表中选择游戏组件："));

	//加载列表
	LoadDBModuleItem();

	return TRUE;
}

//保存输入
bool CDlgServerWizardStep1::SaveInputInfo()
{
	//变量定义
	tagGameModuleInfo * pGameModuleInfo=NULL;
	POSITION Position=m_ModuleListControl.GetFirstSelectedItemPosition();
	
	//获取选择
	if (Position!=NULL)
	{
		INT nListItem=m_ModuleListControl.GetNextSelectedItem(Position);
		pGameModuleInfo=(tagGameModuleInfo *)m_ModuleListControl.GetItemData(nListItem);
	}

	//选择判断
	if (pGameModuleInfo==NULL)
	{
		AfxMessageBox(TEXT("请您先从游戏列表中选择游戏组件"),MB_ICONERROR);
		return false;
	}

	//设置变量
	if (m_pGameModuleInfo!=pGameModuleInfo)
	{
		//游戏模块
		m_GameServiceManager.CloseInstance();
		m_GameServiceManager.SetModuleCreateInfo(pGameModuleInfo->szServerDLLName,GAME_SERVICE_CREATE_NAME);

		//创建判断
		if (pGameModuleInfo->dwNativeVersion==0)
		{
			//构造提示
			TCHAR szString[512]=TEXT("");
			_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] 服务组件还没有安装，请先安装服务组件"),pGameModuleInfo->szModuleName);

			//提示消息
			AfxMessageBox(szString,MB_ICONERROR);

			return false;
		}

		//更新判断
		if (pGameModuleInfo->dwNativeVersion!=pGameModuleInfo->dwModuleVersion)
		{
			//构造提示
			TCHAR szString[512]=TEXT("");
			_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] 服务组件更新了，本地版本过低，无法创建游戏房间！"),pGameModuleInfo->szModuleName);

			//提示消息
			AfxMessageBox(szString,MB_ICONERROR);
			return false;
		}

		//加载模块
		if (m_GameServiceManager.CreateInstance()==false)
		{
			//构造提示
			TCHAR szString[512]=TEXT("");
			_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] 服务组件加载失败，创建游戏房间失败(%s)"),pGameModuleInfo->szModuleName,m_GameServiceManager.GetErrorDescribe());

			//提示消息
			AfxMessageBox(szString,MB_ICONERROR);

			return false;
		}

		//配置模块
		m_pGameModuleInfo=pGameModuleInfo;
		ZeroMemory(m_pGameServiceOption,sizeof(tagGameServiceOption));
		ZeroMemory(m_pGameServiceAttrib,sizeof(m_pGameServiceAttrib));

		//默认配置
		CServerRule::SetForbidShowBaseScore(m_pGameServiceOption->dwServerRule,true);
		m_pDlgServerWizard->SetGameServiceManager(m_GameServiceManager.GetInterface());

		return true;
	}

	return true;
}

//调整控件
VOID CDlgServerWizardStep1::RectifyControl(INT nWidth, INT nHeight)
{
	//调整提示
	if (m_StaticPrompt.m_hWnd!=NULL)
	{
		m_StaticPrompt.SetWindowPos(NULL,5,8,nWidth-10,12,SWP_NOZORDER|SWP_NOCOPYBITS);
	}

	//调整列表
	if (m_ModuleListControl.m_hWnd!=NULL)
	{
		m_ModuleListControl.SetWindowPos(NULL,5,28,nWidth-10,nHeight-28,SWP_NOZORDER|SWP_NOCOPYBITS);
	}

	return;
}

//加载列表
bool CDlgServerWizardStep1::LoadDBModuleItem()
{
	//加载信息
	if (m_ModuleInfoManager.LoadGameModuleInfo(m_ModuleInfoBuffer)==true)
	{
		//重置列表
		m_ModuleListControl.DeleteAllItems();

		//变量定义
		POSITION Position=m_ModuleInfoBuffer.m_GameModuleInfoMap.GetStartPosition();

		//枚举模块
		while (Position!=NULL)
		{
			//获取对象
			WORD wModuleID=0L;
			tagGameModuleInfo * pGameModuleInfo=NULL;
			m_ModuleInfoBuffer.m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameModuleInfo);

			//插入列表
			ASSERT(pGameModuleInfo!=NULL);
			if (pGameModuleInfo!=NULL) m_ModuleListControl.InsertModuleInfo(pGameModuleInfo);
		}
	}

	return false;
}

//双击列表
VOID CDlgServerWizardStep1::OnNMDblclkModuleList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//加载配置
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		//变量定义
		INT nListItem=((NMITEMACTIVATE *)pNMHDR)->iItem;
		tagGameModuleInfo * pGameModuleInfo=(tagGameModuleInfo *)m_ModuleListControl.GetItemData(nListItem);

		//更新版本
		if( pGameModuleInfo->dwModuleVersion < pGameModuleInfo->dwNativeVersion )
		{
			CModuleInfoManager ModuleInfoManager;
			if( !ModuleInfoManager.UpdateGameModuleInfo(*pGameModuleInfo) ) return;
		}

		//投递消息
		if (pGameModuleInfo->dwNativeVersion!=0L)
		{
			GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDC_NEXT,0),0);
		}

		return;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgServerWizardStep2::CDlgServerWizardStep2() : CDlgServerWizardItem(IDD_SERVER_WIZARD_2)
{
}

//析构函数
CDlgServerWizardStep2::~CDlgServerWizardStep2()
{
}

//初始化函数
BOOL CDlgServerWizardStep2::OnInitDialog()
{
	__super::OnInitDialog();
	
	//初始化配置
	InitBaseOption();

	//获取控件
	CTabCtrl * pTabCtrl=(CTabCtrl *)GetDlgItem(IDC_SERVER_OPTION);
	pTabCtrl->SetItemSize(CSize(140,25));

	//插入子项
	pTabCtrl->InsertItem(TAB_SERVER_OPTION1,TEXT("基础配置"));
	pTabCtrl->InsertItem(TAB_SERVER_OPTION2,TEXT("房间选项"));
	pTabCtrl->InsertItem(TAB_SERVER_OPTION3,TEXT("桌子选项"));
	pTabCtrl->InsertItem(TAB_SERVER_OPTION4,TEXT("分组配置"));	
	pTabCtrl->InsertItem(TAB_SERVER_OPTION5,TEXT("服务定制"));		

	//设置子项
	m_pDlgServerOptionItem[0]=&m_DlgServerOption1;
	m_pDlgServerOptionItem[1]=&m_DlgServerOption2;
	m_pDlgServerOptionItem[2]=&m_DlgServerOption3;
	m_pDlgServerOptionItem[3]=&m_DlgServerOption4;	
	m_pDlgServerOptionItem[4]=&m_DlgServerOption5;	

	//变量定义
	CRect rcCreate(0,0,0,0);

	//设置子项
	for (int i=0; i<CountArray(m_pDlgServerOptionItem); i++)
	{
		//指针判断
		if (m_pDlgServerOptionItem[i]==NULL) continue;

		//设置控件		
		m_pDlgServerOptionItem[i]->m_pDlgServerWizard=m_pDlgServerWizard;
		m_pDlgServerOptionItem[i]->m_pGameServiceAttrib=m_pGameServiceAttrib;
		m_pDlgServerOptionItem[i]->m_pGameServiceOption=m_pGameServiceOption;

		//创建子项		
		m_pDlgServerOptionItem[i]->CreateOptionItem(rcCreate,pTabCtrl);
	}

	//激活子项
	ActiveOptionItem(TAB_SERVER_OPTION1);

	//手动触发
	OnCbnSelchangeServerType();

	return TRUE;
}

//保存输入
bool CDlgServerWizardStep2::SaveInputInfo()
{
	//保存输入
	if (SaveBaseOption()==false) return false;

	//遍历子项
	for (int i=0; i<CountArray(m_pDlgServerOptionItem); i++)
	{
		if (m_pDlgServerOptionItem[i]->SaveInputInfo() == false) return false;
	}

	return true;
}

//调整控件
VOID CDlgServerWizardStep2::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//基础配置
VOID CDlgServerWizardStep2::InitBaseOption()
{
	//输入限制
	((CEdit *)GetDlgItem(IDC_TABLE_COUNT))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_SERVER_PORT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_SERVER_NAME))->LimitText(LEN_SERVER-1);

	//调整类型
	WORD wSupportType = CServerRule::GetSupportType();
	m_pGameServiceAttrib->wSupporType &= wSupportType;

	//变量定义
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_SERVER_TYPE);
	WORD wServerType[]={GAME_GENRE_GOLD,GAME_GENRE_SCORE,GAME_GENRE_MATCH,GAME_GENRE_BATTLE,GAME_GENRE_RANKING};
	LPCTSTR pszServerType[]={TEXT("财富类型"),TEXT("积分类型"),TEXT("比赛类型"),TEXT("约战类型"),TEXT("排位类型") };

	//构造类型
	for (INT i=0;i<CountArray(wServerType);i++)
	{
		if ((m_pGameServiceAttrib->wSupporType&wServerType[i])==0) continue;
		pComboBox->SetItemData(pComboBox->AddString(pszServerType[i]),wServerType[i]);
	}

	//游戏类型
	for (INT i=0;i<CountArray(wServerType);i++)
	{
		WORD wItemServerType=(WORD)pComboBox->GetItemData(i);
		if ((wItemServerType&m_pGameServiceOption->wServerType)!=0) pComboBox->SetCurSel(i);
	}

	//变量定义
	CComboBox * pTableComboBox=(CComboBox *)GetDlgItem(IDC_TABLE_GENRE);
	WORD wTableGenre[]={TABLE_GENRE_NORMAL,TABLE_GENRE_DISTRIBUTE};
	LPCTSTR pszTableGenre[]={TEXT("普通入桌"),TEXT("系统分配")};

	//开桌模式
	for (INT i=0;i<CountArray(wTableGenre);i++)
	{		
		if ((m_pGameServiceAttrib->wSupporType&wTableGenre[i])==0) continue;
		pTableComboBox->SetItemData(pTableComboBox->AddString(pszTableGenre[i]),wTableGenre[i]);		
	}

	//默认配置
	if (pTableComboBox->GetCount()==0)
	{		
		pTableComboBox->SetItemData(pTableComboBox->AddString(pszTableGenre[0]),wTableGenre[0]);						
	}

	//设置模式
	for (INT i=0;i<CountArray(wTableGenre);i++)
	{
		WORD wItemServerType=(WORD)pTableComboBox->GetItemData(i);
		if ((wItemServerType&m_pGameServiceOption->wServerType)!=0) 
		{
			pTableComboBox->SetCurSel(i);
			break;
		}
	}

	//默认选择
	if (pTableComboBox->GetCurSel()==-1)
	{	
		for (INT i=0;i<CountArray(wTableGenre);i++)
		{
			if ((pTableComboBox->GetItemData(i)&wTableGenre[0])!=0) 
			{
				pTableComboBox->SetCurSel(i);
				break;
			}
		}
	}

	//房间名字
	if (m_pGameServiceOption->szServerName[0]==0)
	{
		SetDlgItemText(IDC_SERVER_NAME,m_pGameServiceAttrib->szGameName);
	}
	else SetDlgItemText(IDC_SERVER_NAME,m_pGameServiceOption->szServerName);

	//获取数据库
	CDatabaseListManager *pDBListMgr = CDatabaseListManager::GetInstance();
	if( pDBListMgr )
	{
		CComboBox *pcomboDB = ((CComboBox *)GetDlgItem(IDC_COMBO_DB));
		pcomboDB->ResetContent();

		POSITION pos = pDBListMgr->GetStartPosition();
		while( pos )
		{
			tagDataBaseList *pDBList = pDBListMgr->GetNext(pos);
			ASSERT( pDBList );
			if( !pDBList ) break;

			//构造名称
			TCHAR szDBName[128] = {0};
			_sntprintf_s( szDBName,CountArray(szDBName),TEXT("%s(%s)"),pDBList->szDBAliasName,pDBList->szDBName );
			int nIndex = pcomboDB->AddString(szDBName);
			if( CB_ERR == nIndex ) break;

			//设置数据
			pcomboDB->SetItemData( nIndex,pDBList->wDBListID );

			//选择项目
			if( pDBList->wDBListID == m_pGameServiceOption->wDBListID )
			{
				pcomboDB->SetCurSel(nIndex);
			}
		}
	}

	//房间配置
	if (m_pGameServiceOption->wServerPort!=0) 
	{
		SetDlgItemInt(IDC_SERVER_PORT,(LONG)m_pGameServiceOption->wServerPort);
	}
	else
	{
		SetDlgItemText(IDC_SERVER_PORT,TEXT("自动适配"));
	}

	if (m_pGameServiceOption->wTableCount!=0) SetDlgItemInt(IDC_TABLE_COUNT,(LONG)m_pGameServiceOption->wTableCount);
}

//保存配置
bool CDlgServerWizardStep2::SaveBaseOption()
{
	//房间配置
	m_pGameServiceOption->wServerPort=GetDlgItemInt(IDC_SERVER_PORT);
	m_pGameServiceOption->wTableCount=GetDlgItemInt(IDC_TABLE_COUNT);

	//游戏类型
	CComboBox * pServerType=(CComboBox *)GetDlgItem(IDC_SERVER_TYPE);
	m_pGameServiceOption->wServerType=(WORD)pServerType->GetItemData(pServerType->GetCurSel());

	//开桌模式
	CComboBox * pTableComboBox=(CComboBox *)GetDlgItem(IDC_TABLE_GENRE);
	m_pGameServiceOption->wServerType|=(WORD)pTableComboBox->GetItemData(pTableComboBox->GetCurSel());

	//字符数据
	GetDlgItemText(IDC_SERVER_NAME,m_pGameServiceOption->szServerName,CountArray(m_pGameServiceOption->szServerName));

	//房间名字
	if (m_pGameServiceOption->szServerName[0]==0)
	{
		AfxMessageBox(TEXT("房间名字不能为空，请输入游戏房间名字"),MB_ICONERROR);
		GetDlgItem(IDC_SERVER_NAME)->SetFocus();
		return false;
	}

	//游戏类型
	if ((m_pGameServiceOption->wServerType==0)||(m_pGameServiceOption->wServerType==INVALID_WORD))
	{
		AfxMessageBox(TEXT("房间类型模式无效，请选择房间的类型模式"),MB_ICONERROR);
		GetDlgItem(IDC_SERVER_TYPE)->SetFocus();
		
		return false;
	}

	//桌子数目
	if ((m_pGameServiceOption->wTableCount==0)||(m_pGameServiceOption->wTableCount>MAX_TABLE))
	{
		AfxMessageBox(TEXT("桌子数目无效，请填写游戏桌子数目，有效范围从 1 至 512"),MB_ICONERROR);
		GetDlgItem(IDC_TABLE_COUNT)->SetFocus();
		return false;
	}

	//桌子数目
	if ((m_pGameServiceOption->wTableCount>1)&&(m_pGameServiceAttrib->wChairCount>=MAX_CHAIR))
	{
		AfxMessageBox(TEXT("桌子数目无效，百人游戏只能配置一张桌子！"),MB_ICONERROR);
		GetDlgItem(IDC_TABLE_COUNT)->SetFocus();
		return false;
	}

	//数据库索引
	int nCurSel=((CComboBox *)GetDlgItem(IDC_COMBO_DB))->GetCurSel();
	if (nCurSel!=CB_ERR)
	{
		m_pGameServiceOption->wDBListID=(WORD)((CComboBox *)GetDlgItem(IDC_COMBO_DB))->GetItemData(nCurSel);
	}
	else
	{
		AfxMessageBox(TEXT("请选择一个有效的数据库"),MB_ICONERROR);
		GetDlgItem(IDC_COMBO_DB)->SetFocus();
		return false;
	}
	
	return true;
}

//激活子项
VOID CDlgServerWizardStep2::ActiveOptionItem(BYTE cbItemIndex)
{
	//判断索引
	if (m_cbItemIndex==cbItemIndex) return;

	//设置索引
	m_cbItemIndex=cbItemIndex;

	//控件区域
	CRect rcTabCtrl;
	GetDlgItem(IDC_SERVER_OPTION)->GetClientRect(&rcTabCtrl);	

	//设置控件
	if (m_pDlgServerOptionItem[m_cbItemIndex]->m_hWnd!=NULL)
	{
		m_pDlgServerOptionItem[m_cbItemIndex]->SetWindowPos(NULL,4,30,rcTabCtrl.Width()-10,rcTabCtrl.Height()-30-4,SWP_NOZORDER|SWP_SHOWWINDOW);		
	}

	//设置控件
	for (int i=0; i<CountArray(m_pDlgServerOptionItem); i++)
	{
		if (i==m_cbItemIndex) continue;
		if (m_pDlgServerOptionItem[i]->m_hWnd!=NULL) m_pDlgServerOptionItem[i]->ShowWindow(SW_HIDE);
	}
}

//选择变更:
void CDlgServerWizardStep2::OnCbnSelchangeServerType()
{
	//游戏类型
	CComboBox* pServerType = (CComboBox*)GetDlgItem(IDC_SERVER_TYPE);
	auto wServerType = (WORD)pServerType->GetItemData(pServerType->GetCurSel());

	//通知变更
	for (int i = 0; i < CountArray(m_pDlgServerOptionItem); i++)
	{
		m_pDlgServerOptionItem[i]->OnEventServerTypeChange(wServerType);
	}
}

//选择变更
void CDlgServerWizardStep2::OnTcnSelchangeServerOption(NMHDR *pNMHDR, LRESULT *pResult)
{
	//获取控件
	CTabCtrl * pTabCtrl=(CTabCtrl *)GetDlgItem(IDC_SERVER_OPTION);
	ActiveOptionItem(pTabCtrl->GetCurSel());	
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgServerOptionCustom::CDlgServerOptionCustom() : CDlgServerOptionItem(IDD_SERVER_OPTION_CUSTOM)
{
	//设置变量
	m_hCustomRule=NULL;

	return;
}

//析构函数
CDlgServerOptionCustom::~CDlgServerOptionCustom()
{
}

//初始化函数
BOOL CDlgServerOptionCustom::OnInitDialog()
{
	__super::OnInitDialog();

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//创建窗口
	IGameServiceCustomRule * pIGameServiceCustomRule=m_pDlgServerWizard->m_pIGameServiceCustomRule;
	if( pIGameServiceCustomRule == NULL ) return TRUE;

	//创建窗口
	m_hCustomRule=pIGameServiceCustomRule->CreateCustomRule(this,rcClient,m_pGameServiceOption->cbCustomRule,sizeof(m_pGameServiceOption->cbCustomRule));

	//设置默认值
	if (m_pGameServiceOption->wServerID == 0)
	{
		pIGameServiceCustomRule->DefaultCustomRule(m_pGameServiceOption->cbCustomRule, sizeof(m_pGameServiceOption->cbCustomRule));
	}

	return TRUE;
}

//保存输入
bool CDlgServerOptionCustom::SaveInputInfo()
{
	IGameServiceCustomRule * pIGameServiceCustomRule=m_pDlgServerWizard->m_pIGameServiceCustomRule;
	if (pIGameServiceCustomRule==NULL) return true;

	//保存输入
	if ((m_hCustomRule!=NULL)&&(m_pDlgServerWizard->m_pIGameServiceCustomRule->SaveCustomRule(m_pGameServiceOption->cbCustomRule, sizeof(m_pGameServiceOption->cbCustomRule))==false))
	{
		return false;
	}

	return true;
}

//调整控件
VOID CDlgServerOptionCustom::RectifyControl(INT nWidth, INT nHeight)
{
	//调整位置
	if (m_hCustomRule!=NULL) ::SetWindowPos(m_hCustomRule,NULL,0,0,nWidth,nHeight,SWP_NOMOVE|SWP_NOZORDER);

	return;
}

//消耗消息
VOID CDlgServerOptionCustom::OnNcDestroy()
{
	//设置变量
	m_hCustomRule=NULL;

	__super::OnNcDestroy();
}

//焦点消息
VOID CDlgServerOptionCustom::OnSetFocus(CWnd * pNewWnd)
{
	__super::OnSetFocus(pNewWnd);

	//设置焦点
	if (m_hCustomRule!=NULL) ::SetFocus(m_hCustomRule);

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgServerWizard::CDlgServerWizard() : CDialog(IDD_SERVER_WIZARD)
{
	//设置变量
	m_wActiveIndex=INVALID_WORD;
	m_pWizardItem[0]=&m_ServerWizardStep1;
	m_pWizardItem[1]=&m_ServerWizardStep2;

	//接口变量
	m_pIGameServiceManager=NULL;
	m_pIGameServiceCustomRule=NULL;

	//配置信息
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	return;
}

//析构函数
CDlgServerWizard::~CDlgServerWizard()
{
}

//初始化函数
BOOL CDlgServerWizard::OnInitDialog()
{
	__super::OnInitDialog();

	m_DBListMgr.Load();

	//激活向导
	ActiveWizardItem((m_pIGameServiceManager==NULL)?0:1);

	return FALSE;
}

//确定函数
VOID CDlgServerWizard::OnOK()
{
	OnBnClickedFinish();

	return;
}

//获取属性
VOID CDlgServerWizard::GetGameServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	//设置变量
	GameServiceAttrib=m_ModuleInitParameter.GameServiceAttrib;

	return;
}

//获取配置
VOID CDlgServerWizard::GetGameServiceOption(tagGameServiceOption & GameServiceOption)
{
	//设置变量
	GameServiceOption=m_ModuleInitParameter.GameServiceOption;

	return;
}

//设置配置
VOID CDlgServerWizard::SetGameServiceOption(tagGameServiceOption & GameServiceOption)
{
	//设置变量
	m_ModuleInitParameter.GameServiceOption=GameServiceOption;

	return;
}

//设置接口
VOID CDlgServerWizard::SetGameServiceManager(IGameServiceManager * pIGameServiceManager)
{
	//销毁子项
	if (m_wActiveIndex==0)
	{
		for (WORD i=1;i<CountArray(m_pWizardItem);i++)
		{
			if ((m_pWizardItem[i]!=NULL)&&(m_pWizardItem[i]->m_hWnd!=NULL))
			{
				m_pWizardItem[i]->DestroyWindow();
			}
		}
	}

	//管理接口
	m_pIGameServiceManager=pIGameServiceManager;

	//设置属性
	pIGameServiceManager->GetGameServiceAttrib(m_ModuleInitParameter.GameServiceAttrib);

	//自定接口
	m_pIGameServiceCustomRule=QUERY_OBJECT_PTR_INTERFACE(m_pIGameServiceManager,IGameServiceCustomRule);

	return;
}

//激活向导
bool CDlgServerWizard::ActiveWizardItem(WORD wIndex)
{
	//判断状态
	if (m_wActiveIndex==wIndex)
	{
		m_pWizardItem[m_wActiveIndex]->SetFocus();
		return true;
	}

	//保存旧项
	CDlgServerWizardItem * pItemWizard=NULL;
	if (m_wActiveIndex<CountArray(m_pWizardItem))
	{
		//设置变量
		pItemWizard=m_pWizardItem[m_wActiveIndex];

		//保存数据
		if ((wIndex>m_wActiveIndex)&&(m_pWizardItem[m_wActiveIndex]->SaveItemData()==false)) return false;
	} 

	//获取位置
	CRect rcItemRect;
	GetDlgItem(IDC_ITEM_FRAME)->GetWindowRect(rcItemRect);
	ScreenToClient(&rcItemRect);

	//设置新项
	m_pWizardItem[wIndex]->m_pDlgServerWizard=this;
	m_pWizardItem[wIndex]->m_pGameServiceAttrib=&m_ModuleInitParameter.GameServiceAttrib;
	m_pWizardItem[wIndex]->m_pGameServiceOption=&m_ModuleInitParameter.GameServiceOption;

	//创建新项
	m_wActiveIndex=wIndex;
	m_pWizardItem[m_wActiveIndex]->ShowWizardItem(rcItemRect,this);

	//激活新项
	m_pWizardItem[m_wActiveIndex]->SetFocus();
	if (pItemWizard!=NULL) pItemWizard->ShowWindow(SW_HIDE);

	//界面变量	
	CButton * pButtonFinish=(CButton * )GetDlgItem(IDC_FINISH);
	pButtonFinish->EnableWindow((m_wActiveIndex>0)?TRUE:FALSE);
	
	//构造标题
	TCHAR szTitle[128]=TEXT("");
	_sntprintf_s(szTitle,CountArray(szTitle),TEXT("房间配置向导 --- [ 步骤 %d ]"),m_wActiveIndex+1);
	
	//设置标题
	SetWindowText(szTitle);

	return true;
}

//下一步
VOID CDlgServerWizard::OnBnClickedNext()
{
	//效验参数
	ASSERT((m_wActiveIndex+1)<CountArray(m_pWizardItem));

	//激活页面
	ActiveWizardItem(m_wActiveIndex+1);

	return;
}

//完成按钮
VOID CDlgServerWizard::OnBnClickedFinish()
{
	//保存设置
	ASSERT(m_wActiveIndex>=1);
	if (m_pWizardItem[m_wActiveIndex]->SaveItemData()==false) return;

	//保存子项
	for (WORD i=0;i<CountArray(m_pWizardItem);i++)
	{
		if ((m_pWizardItem[i]!=NULL)&&(m_pWizardItem[i]->SaveItemData()==false))
		{
			ActiveWizardItem(i);
			return;
		}
	}

	//房间信息
	tagCreateServerInfo CreateServerInfo;
	ZeroMemory(&CreateServerInfo,sizeof(CreateServerInfo));

	//参数调整
	ASSERT(m_pIGameServiceManager!=NULL);
	m_pIGameServiceManager->RectifyOptionParameter(m_ModuleInitParameter.GameServiceOption);

	//模块索引
	CreateServerInfo.wModuleID=m_ModuleInitParameter.GameServiceAttrib.wKindID;
	CreateServerInfo.wServerID=m_ModuleInitParameter.GameServiceOption.wServerID;
	CreateServerInfo.wDBListID = m_ModuleInitParameter.GameServiceOption.wDBListID;
	CreateServerInfo.cbMatchType = m_ModuleInitParameter.GameServiceOption.cbMatchType;

	//挂接配置
	CreateServerInfo.wKindID=m_ModuleInitParameter.GameServiceOption.wKindID;
	CreateServerInfo.wNodeID=m_ModuleInitParameter.GameServiceOption.wNodeID;
	CreateServerInfo.wSortID=m_ModuleInitParameter.GameServiceOption.wSortID;
	CreateServerInfo.wVisibleMask=m_ModuleInitParameter.GameServiceOption.wVisibleMask;

	//积分配置
	CreateServerInfo.lRevenue=m_ModuleInitParameter.GameServiceOption.lRevenue;
	CreateServerInfo.lCellScore = m_ModuleInitParameter.GameServiceOption.lCellScore;
	CreateServerInfo.lLessScore = m_ModuleInitParameter.GameServiceOption.lLessScore;
	CreateServerInfo.lExpGainRate = m_ModuleInitParameter.GameServiceOption.lExpGainRate;
	CreateServerInfo.lRestrictScore=m_ModuleInitParameter.GameServiceOption.lRestrictScore;
	CreateServerInfo.lMinEnterScore = m_ModuleInitParameter.GameServiceOption.lMinEnterScore;
	CreateServerInfo.lMaxEnterScore = m_ModuleInitParameter.GameServiceOption.lMaxEnterScore;
	CreateServerInfo.lMinEnterMember = m_ModuleInitParameter.GameServiceOption.lMinEnterMember;
	CreateServerInfo.lMaxEnterMember = m_ModuleInitParameter.GameServiceOption.lMaxEnterMember;
	
	//服务配置
	StringCchCopy(CreateServerInfo.szServiceAddr, CountArray(CreateServerInfo.szServiceAddr),m_ModuleInitParameter.GameServiceOption.szServiceAddr);
	StringCchCopy(CreateServerInfo.szServiceDomain, CountArray(CreateServerInfo.szServiceDomain),m_ModuleInitParameter.GameServiceOption.szServiceDomain);

	//约战规则
	CreateServerInfo.dwBattleRule = m_ModuleInitParameter.GameServiceOption.dwBattleRule;

	//房间信息
	CreateServerInfo.wMaxPlayer=m_ModuleInitParameter.GameServiceOption.wMaxPlayer;
	CreateServerInfo.wTableCount=m_ModuleInitParameter.GameServiceOption.wTableCount;
	CreateServerInfo.wServerType=m_ModuleInitParameter.GameServiceOption.wServerType;
	CreateServerInfo.wServerPort=m_ModuleInitParameter.GameServiceOption.wServerPort;
	CreateServerInfo.wServerLevel=m_ModuleInitParameter.GameServiceOption.wServerLevel;
	CreateServerInfo.dwServerRule=m_ModuleInitParameter.GameServiceOption.dwServerRule;
	CreateServerInfo.dwAttachUserRight = m_ModuleInitParameter.GameServiceOption.dwAttachUserRight;	
	StringCchCopy(CreateServerInfo.szServerName, CountArray(CreateServerInfo.szServerName),m_ModuleInitParameter.GameServiceOption.szServerName);

	//分组参数
	CreateServerInfo.cbDistributeMode=m_ModuleInitParameter.GameServiceOption.cbDistributeMode;
	CreateServerInfo.cbDistributeRule=m_ModuleInitParameter.GameServiceOption.cbDistributeRule;
	CreateServerInfo.wMinPlayerCount=m_ModuleInitParameter.GameServiceOption.wMinPlayerCount;
	CreateServerInfo.wMinDistributeUser=m_ModuleInitParameter.GameServiceOption.wMinDistributeUser;	
	CreateServerInfo.wMinPartakeGameUser=m_ModuleInitParameter.GameServiceOption.wMinPartakeGameUser;
	CreateServerInfo.wMaxPartakeGameUser=m_ModuleInitParameter.GameServiceOption.wMaxPartakeGameUser;
	CreateServerInfo.wDistributeTimeSpace=m_ModuleInitParameter.GameServiceOption.wDistributeTimeSpace;

	//扩展配置
	CWHService::GetMachineID(CreateServerInfo.szServiceMachine);
	CopyMemory(CreateServerInfo.cbCustomRule,m_ModuleInitParameter.GameServiceOption.cbCustomRule,sizeof(CreateServerInfo.cbCustomRule));

	//变量定义
	tagGameServerInfo GameServerResult;
	CServerInfoManager ServerInfoManager;
	
	//插入房间
	if (CreateServerInfo.wServerID==0)
	{
		if (ServerInfoManager.InsertGameServer(&CreateServerInfo,GameServerResult)==false) return;

		//提示消息
		CTraceService::TraceString(TEXT("游戏房间创建成功"),TraceLevel_Normal);
	}

	//修改房间
	if (CreateServerInfo.wServerID!=0)
	{
		//修改房间
		if (ServerInfoManager.ModifyGameServer(&CreateServerInfo,GameServerResult)==false) return;

		//提示消息
		CTraceService::TraceString(TEXT("游戏房间配置成功"),TraceLevel_Normal);
	}

	//变量定义
	UINT uCustomRuleSize=sizeof(m_ModuleInitParameter.GameServiceOption.cbCustomRule);

	//基础属性
	m_ModuleInitParameter.GameServiceOption.wDBListID = GameServerResult.wDBListID;
	
	//挂接属性
	m_ModuleInitParameter.GameServiceOption.wKindID=GameServerResult.wKindID;
	m_ModuleInitParameter.GameServiceOption.wNodeID=GameServerResult.wNodeID;
	m_ModuleInitParameter.GameServiceOption.wSortID=GameServerResult.wSortID;
	m_ModuleInitParameter.GameServiceOption.wVisibleMask=GameServerResult.wVisibleMask;

	//房间属性
	m_ModuleInitParameter.GameServiceOption.wServerID=GameServerResult.wServerID;
	m_ModuleInitParameter.GameServiceOption.wTableCount=GameServerResult.wTableCount;
	m_ModuleInitParameter.GameServiceOption.wServerType=GameServerResult.wServerType;
	m_ModuleInitParameter.GameServiceOption.wServerPort=GameServerResult.wServerPort;
	m_ModuleInitParameter.GameServiceOption.wServerLevel=GameServerResult.wServerLevel;
	m_ModuleInitParameter.GameServiceOption.cbMatchType = GameServerResult.cbMatchType;
	StringCchCopy(m_ModuleInitParameter.GameServiceOption.szServerName, LEN_SERVER,GameServerResult.szServerName);

	//服务配置
	StringCchCopy(m_ModuleInitParameter.GameServiceOption.szServiceAddr,CountArray(m_ModuleInitParameter.GameServiceOption.szServiceAddr), GameServerResult.szServiceAddr);
	StringCchCopy(m_ModuleInitParameter.GameServiceOption.szServiceDomain,CountArray(m_ModuleInitParameter.GameServiceOption.szServiceDomain), GameServerResult.szServiceDomain);

	//积分配置
	m_ModuleInitParameter.GameServiceOption.lRevenue=GameServerResult.lRevenue;
	m_ModuleInitParameter.GameServiceOption.lCellScore=GameServerResult.lCellScore;
	m_ModuleInitParameter.GameServiceOption.lLessScore=GameServerResult.lLessScore;
	m_ModuleInitParameter.GameServiceOption.lExpGainRate = GameServerResult.lExpGainRate;
	m_ModuleInitParameter.GameServiceOption.lRestrictScore=GameServerResult.lRestrictScore;
	m_ModuleInitParameter.GameServiceOption.lMinEnterScore=GameServerResult.lMinEnterScore;
	m_ModuleInitParameter.GameServiceOption.lMaxEnterScore=GameServerResult.lMaxEnterScore;
	m_ModuleInitParameter.GameServiceOption.lMinEnterMember=GameServerResult.lMinEnterMember;
	m_ModuleInitParameter.GameServiceOption.lMaxEnterMember=GameServerResult.lMaxEnterMember;

	//约战配置
	m_ModuleInitParameter.GameServiceOption.dwBattleRule=GameServerResult.dwBattleRule;

	//分组参数
	m_ModuleInitParameter.GameServiceOption.cbDistributeMode=GameServerResult.cbDistributeMode;
	m_ModuleInitParameter.GameServiceOption.cbDistributeRule=GameServerResult.cbDistributeRule;
	m_ModuleInitParameter.GameServiceOption.wMinPlayerCount=GameServerResult.wMinPlayerCount;
	m_ModuleInitParameter.GameServiceOption.wMinDistributeUser=GameServerResult.wMinDistributeUser;	
	m_ModuleInitParameter.GameServiceOption.wMinPartakeGameUser=GameServerResult.wMinPartakeGameUser;
	m_ModuleInitParameter.GameServiceOption.wMaxPartakeGameUser=GameServerResult.wMaxPartakeGameUser;
	m_ModuleInitParameter.GameServiceOption.wDistributeTimeSpace=GameServerResult.wDistributeTimeSpace;

	//服务配置
	StringCchCopy(m_ModuleInitParameter.GameServiceOption.szServiceAddr, LEN_ADDRESS, GameServerResult.szServiceAddr);
	StringCchCopy(m_ModuleInitParameter.GameServiceOption.szServiceDomain, LEN_DOMAIN, GameServerResult.szServiceDomain);

	//房间配置
	m_ModuleInitParameter.GameServiceOption.wMaxPlayer=GameServerResult.wMaxPlayer;
	m_ModuleInitParameter.GameServiceOption.dwServerRule=GameServerResult.dwServerRule;
	m_ModuleInitParameter.GameServiceOption.dwAttachUserRight = GameServerResult.dwAttachUserRight;

	//扩展配置
	CopyMemory(m_ModuleInitParameter.GameServiceOption.cbCustomRule,GameServerResult.cbCustomRule,uCustomRuleSize);

	//关闭窗口
	EndDialog(IDOK);

	return;
}

//////////////////////////////////////////////////////////////////////////