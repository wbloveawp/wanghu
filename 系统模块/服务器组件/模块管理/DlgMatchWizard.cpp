#include "StdAfx.h"
#include "Resource.h"
#include "DlgMatchWizard.h"

//////////////////////////////////////////////////////////////////////////
//常量定义

#define TAB_OPTION_SIGNUP1					0
#define TAB_OPTION_SIGNUP2					1
#define TAB_OPTION_RANKINGMODE				2
#define TAB_OPTION_DISTRIBUTE				3
#define TAB_OPTION_REWARD					4
//#define TAB_OPTION_ROUND					5

//////////////////////////////////////////////////////////////////////////
//消息映射

BEGIN_MESSAGE_MAP(CDlgMatchItem, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchWizardItem, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchSignup1, CDlgMatchItem)
	ON_CBN_SELCHANGE(IDC_COMBO_FROM_MATCH, OnCbnSelchangeComboFromMatch)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchReward, CDlgMatchItem)
	ON_BN_CLICKED(IDC_ADD_REWARD, OnBnClickedAddReward)
	ON_BN_CLICKED(IDC_MODIFY_REWARD, OnBnClickedModifyReward)
	ON_BN_CLICKED(IDC_DELETE_REWARD, OnBnClickedDeleteReward)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_REWARD, OnLvnItemchangedListReward)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchWizardStep1, CDlgMatchWizardItem)	
	ON_CBN_SELCHANGE(IDC_COMBO_MATCH_TYPE, OnCbnSelchangeComboMatchType)
	ON_NOTIFY(TCN_SELCHANGE, IDC_MATCH_OPTION, OnTcnSelchangeMatchOption)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchWizardStep2, CDlgMatchWizardItem)	
	ON_BN_CLICKED(IDC_ADD_ROUND, OnBnClickedAddRound)	
	ON_BN_CLICKED(IDC_MODIFY_ROUND, OnBnClickedModifyRound)
	ON_BN_CLICKED(IDC_DELETE_ROUND, OnBnClickedDeleteRound)	
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ROUND, OnLvnItemchangedListRound)
	ON_CBN_SELCHANGE(IDC_COMBO_PROMOTE_MODE, OnCbnSelchangeComboPromoteMode)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchWizard, CDialog)
	ON_BN_CLICKED(IDC_LAST, OnBnClickedLast)
	ON_BN_CLICKED(IDC_NEXT, OnBnClickedNext)
	ON_BN_CLICKED(IDC_FINISH, OnBnClickedFinish)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgPromoteModeItem, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchImmediate, CDlgMatchItem)	
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchTournament, CDlgMatchItem)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
//构造函数
CDlgMatchItem::CDlgMatchItem(UINT nIDTemplate): CDialog(nIDTemplate)
{
	//设置变量		
	m_pMatchOption=NULL;
	m_pMatchInfoBuffer=NULL;	
}

//析构函数
CDlgMatchItem::~CDlgMatchItem()
{
}

//设置比赛
VOID CDlgMatchItem::SetMatchInfoBuffer(CMatchInfoBuffer * pMatchInfoBuffer)
{
	m_pMatchInfoBuffer=pMatchInfoBuffer;
}

//设置配置
VOID CDlgMatchItem::SetMatchOption(tagMatchOption * pMatchOption)
{
	m_pMatchOption=pMatchOption;
}

//创建向导
bool CDlgMatchItem::ShowMatchItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//创建窗口
	if (m_hWnd==NULL) CreateMatchItem(rcRect,pParentWnd);

	//显示窗口
	ShowWindow(SW_SHOW);

	return true;
}

//创建子项
VOID CDlgMatchItem::CreateMatchItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//设置资源
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//创建窗口
	BOOL bSuccess=Create(m_lpszTemplateName,pParentWnd);
	SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

	//设置资源
	AfxSetResourceHandle(GetModuleHandle(NULL));
}

//类型变更
VOID CDlgMatchItem::OnEventMatchTypeChanged(BYTE cbMatchType)
{
}

//位置消息
VOID CDlgMatchItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//调整控件
	RectifyControl(cx,cy);

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgMatchSignup1::CDlgMatchSignup1() : CDlgMatchItem(IDD_MATCH_SIGNUP1)
{
}

//析构函数
CDlgMatchSignup1::~CDlgMatchSignup1()
{
}

//初始化函数
BOOL CDlgMatchSignup1::OnInitDialog()
{
	__super::OnInitDialog();

	//设置控件
	((CEdit *)GetDlgItem(IDC_EDIT_MATCH_FEE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_SIGNUP_COUNT))->LimitText(3);

	//报名次数
	SetDlgItemInt(IDC_EDIT_SIGNUP_COUNT, (UINT)m_pMatchOption->wSignupCount);

	////朋友圈分享
	//if (m_pMatchOption->cbSignupMode&SIGNUP_MODE_WECHAT_SHARE)
	//{
	//	//设置选择
	//	((CButton *)GetDlgItem(IDC_CHECK_WECHAT_SHARE))->SetCheck(TRUE);
	//}

	//使用比赛券
	if (m_pMatchOption->cbSignupMode & SIGNUP_MODE_MATCH_TICKET)
	{
		//设置选择
		((CButton*)GetDlgItem(IDC_CHECK_MATCH_TICKET))->SetCheck(TRUE);

		//设置选择
		((CButton*)GetDlgItem(IDC_CHECK_RESET_SCORE))->SetCheck(m_pMatchOption->wBuyTicketOption&MATCH_BTOP_RESET_SCORE?TRUE:FALSE);

		//设置控件
		SetDlgItemInt(IDC_EDIT_USE_TICKET_COUNT, (int)m_pMatchOption->wUseTicketCount);
		SetDlgItemInt(IDC_EDIT_BUY_TICKET_COUNT, (int)m_pMatchOption->wBuyTicketCount);
		SetDlgItemInt(IDC_EDIT_BUY_TICKET_AMOUNT, (int)m_pMatchOption->lBuyTicketAmount);				

		//费用类型
		switch (m_pMatchOption->wBuyTicketMoneyID)
		{
		case GOODS_ID_GOLD:		//金币
		{
			CheckRadioButton(IDC_RADIO_GOLD_1, IDC_RADIO_INGOT_1, IDC_RADIO_GOLD_1);
			break;
		}
		//case GOODS_ID_INGOT:	//钻石
		//{
		//	CheckRadioButton(IDC_RADIO_GOLD_1, IDC_RADIO_INGOT_1, IDC_RADIO_INGOT_1);
		//	break;
		//}
		}
	}

	//缴纳费用
	if (m_pMatchOption->cbSignupMode&SIGNUP_MODE_SIGNUP_FEE)
	{
		//设置选择
		((CButton *)GetDlgItem(IDC_CHECK_SIGNUP_FEE))->SetCheck(TRUE);

		//报名费用
		SetDlgItemInt(IDC_EDIT_SIGNUP_FEE,(UINT)m_pMatchOption->lFeeAmount);

		//费用类型
		switch (m_pMatchOption->wFeeMoneyID)
		{
		case GOODS_ID_GOLD:		//金币
		{
			CheckRadioButton(IDC_RADIO_GOLD, IDC_RADIO_INGOT, IDC_RADIO_GOLD);
			break;
		}
		//case GOODS_ID_INGOT:	//钻石
		//{
		//	CheckRadioButton(IDC_RADIO_GOLD, IDC_RADIO_INGOT, IDC_RADIO_INGOT);
		//	break;
		//}
		}
	}

	//比赛晋级
	//if (m_pMatchOption->cbSignupMode&SIGNUP_MODE_MATCH_USER)
	//{
	//	//设置选择
	//	((CButton *)GetDlgItem(IDC_CHECK_MATCH_USER))->SetCheck(TRUE);		

	//	//结束日期
	//	CDateTimeCtrl * pEndtDate=(CDateTimeCtrl *)GetDlgItem(IDC_DATE_MATCH_END);
	//	pEndtDate->SetTime(&m_pMatchOption->MatchEndDate);

	//	//开始日期
	//	CDateTimeCtrl * pStartDate=(CDateTimeCtrl *)GetDlgItem(IDC_DATE_MATCH_START);
	//	pStartDate->SetTime(&m_pMatchOption->MatchStartDate);		

	//	//单轮排名
	//	if (m_pMatchOption->cbFilterType==FILTER_TYPE_SINGLE_TURN)
	//	{			
	//		SetDlgItemInt(IDC_EDIT_SINGLE_RANK,m_pMatchOption->wMaxRankID);
	//		CheckRadioButton(IDC_RADIO_SINGLE_TURN,IDC_RADIO_TOTAL_RANKING,IDC_RADIO_SINGLE_TURN);			
	//	}

	//	//总排名次
	//	if (m_pMatchOption->cbFilterType==FILTER_TYPE_TOTAL_RANKING)
	//	{
	//		SetDlgItemInt(IDC_EDIT_TOTAL_RANK,m_pMatchOption->wMaxRankID);
	//		CheckRadioButton(IDC_RADIO_SINGLE_TURN,IDC_RADIO_TOTAL_RANKING,IDC_RADIO_TOTAL_RANKING);			
	//	}		
	//}
	//else
	//{
	//	//默认设置
	//	CheckRadioButton(IDC_RADIO_SINGLE_TURN,IDC_RADIO_TOTAL_RANKING,IDC_RADIO_SINGLE_TURN);
	//}

	////获取控件
	//CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_FROM_MATCH);
	//if (pComboBox!=NULL)
	//{
	//	//加载比赛
	//	if (m_pMatchInfoBuffer!=NULL)
	//	{
	//		//遍历赛事
	//		for (INT_PTR i=0;i<m_pMatchInfoBuffer->m_MatchOptionArray.GetCount();i++)
	//		{
	//			//获取对象
	//			tagMatchOption * pMatchOption=m_pMatchInfoBuffer->m_MatchOptionArray[i];
	//			if (pMatchOption->dwMatchID==m_pMatchOption->dwMatchID) continue;
	//		
	//			//添加赛事
	//			int nIndex=pComboBox->AddString(CW2CT(pMatchOption->szMatchName));
	//			pComboBox->SetItemDataPtr(nIndex,pMatchOption);
	//			if (pMatchOption->dwMatchID==m_pMatchOption->dwFromMatchID)
	//			{
	//				if (m_pMatchOption->cbSignupMode&SIGNUP_MODE_MATCH_USER)
	//				{
	//					pComboBox->SetCurSel(nIndex);
	//				}
	//			}
	//		}
	//	}

	//	//模拟选择
	//	OnCbnSelchangeComboFromMatch();
	//}
	
	return FALSE;
}

//保存输入
bool CDlgMatchSignup1::SaveInputInfo()
{
	//朋友圈分享
	//if (IsDlgButtonChecked(IDC_CHECK_WECHAT_SHARE))
	//{
	//	m_pMatchOption->cbSignupMode|=SIGNUP_MODE_WECHAT_SHARE;
	//}
	//else
	//{
	//	m_pMatchOption->cbSignupMode&=~SIGNUP_MODE_WECHAT_SHARE;
	//}

	//报名次数
	m_pMatchOption->wSignupCount = GetDlgItemInt(IDC_EDIT_SIGNUP_COUNT);

	//使用比赛券
	if (IsDlgButtonChecked(IDC_CHECK_MATCH_TICKET))
	{
		//费用额度
		m_pMatchOption->cbSignupMode |= SIGNUP_MODE_MATCH_TICKET;
		m_pMatchOption->wUseTicketCount = GetDlgItemInt(IDC_EDIT_USE_TICKET_COUNT);

		//购票选项
		if (IsDlgButtonChecked(IDC_CHECK_RESET_SCORE))
		{
			m_pMatchOption->wBuyTicketOption |= MATCH_BTOP_RESET_SCORE;
		}
		else
		{
			m_pMatchOption->wBuyTicketOption &= ~MATCH_BTOP_RESET_SCORE;
		}

		//购买配置
		m_pMatchOption->wBuyTicketCount = GetDlgItemInt(IDC_EDIT_BUY_TICKET_COUNT);
		m_pMatchOption->lBuyTicketAmount = GetDlgItemInt(IDC_EDIT_BUY_TICKET_AMOUNT);
		if (IsDlgButtonChecked(IDC_RADIO_GOLD_1)) m_pMatchOption->wBuyTicketMoneyID = GOODS_ID_GOLD;
		//if (IsDlgButtonChecked(IDC_RADIO_INGOT_1)) m_pMatchOption->wBuyTicketMoneyID = GOODS_ID_INGOT;
	}
	else
	{
		m_pMatchOption->cbSignupMode &= ~SIGNUP_MODE_MATCH_TICKET;
	}

	//缴纳费用
	if (IsDlgButtonChecked(IDC_CHECK_SIGNUP_FEE))
	{
		//费用额度
		m_pMatchOption->cbSignupMode|=SIGNUP_MODE_SIGNUP_FEE;
		m_pMatchOption->lFeeAmount=GetDlgItemInt(IDC_EDIT_SIGNUP_FEE);

		//费用类型
		if (IsDlgButtonChecked(IDC_RADIO_GOLD)) m_pMatchOption->wFeeMoneyID=GOODS_ID_GOLD;
		//if (IsDlgButtonChecked(IDC_RADIO_INGOT)) m_pMatchOption->wFeeMoneyID =GOODS_ID_INGOT;
	}
	else
	{
		m_pMatchOption->cbSignupMode&=~SIGNUP_MODE_SIGNUP_FEE;
	}

	////比赛用户
	//if (IsDlgButtonChecked(IDC_CHECK_MATCH_USER))
	//{
	//	m_pMatchOption->cbSignupMode|=SIGNUP_MODE_MATCH_USER;

	//	//获取控件
	//	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_FROM_MATCH);
	//	if (pComboBox!=NULL && pComboBox->GetCurSel()!=LB_ERR)
	//	{
	//		tagMatchOption * pMatchOption=(tagMatchOption *)pComboBox->GetItemData(pComboBox->GetCurSel());
	//		if (pMatchOption!=NULL) m_pMatchOption->dwFromMatchID=pMatchOption->dwMatchID;
	//	}

	//	//结束日期
	//	CDateTimeCtrl * pEndtDate=(CDateTimeCtrl *)GetDlgItem(IDC_DATE_MATCH_END);
	//	pEndtDate->GetTime(&m_pMatchOption->MatchEndDate);

	//	//开始日期 
	//	CDateTimeCtrl * pStartDate=(CDateTimeCtrl *)GetDlgItem(IDC_DATE_MATCH_START);
	//	pStartDate->GetTime(&m_pMatchOption->MatchStartDate);

	//	//单轮排名
	//	if (IsDlgButtonChecked(IDC_RADIO_SINGLE_TURN))
	//	{			
	//		m_pMatchOption->cbFilterType=FILTER_TYPE_SINGLE_TURN;
	//		m_pMatchOption->wMaxRankID=GetDlgItemInt(IDC_EDIT_SINGLE_RANK);
	//	}

	//	//总排名次
	//	if (IsDlgButtonChecked(IDC_RADIO_TOTAL_RANKING))
	//	{
	//		m_pMatchOption->cbFilterType=FILTER_TYPE_TOTAL_RANKING;
	//		m_pMatchOption->wMaxRankID=GetDlgItemInt(IDC_EDIT_TOTAL_RANK);
	//	}
	//}
	//else
	//{
	//	m_pMatchOption->cbSignupMode&=~SIGNUP_MODE_MATCH_USER;
	//}

	//报名方式
	if (m_pMatchOption->cbSignupMode==0)
	{
		AfxMessageBox(TEXT("报名方式不能为空，请重新选择！"));

		return false;
	}

	//比赛用户
	//if (m_pMatchOption->cbSignupMode&SIGNUP_MODE_MATCH_USER)
	//{
	//	if (m_pMatchOption->dwFromMatchID==0)
	//	{
	//		AfxMessageBox(TEXT("您勾选了参加特定比赛作为报名方式，必须选择一场赛事来确定比赛用户的来源！"));

	//		return false;
	//	}

	//	//查找比赛
	//	tagMatchOption * pMatchOption=m_pMatchInfoBuffer->SearchMatchOption(m_pMatchOption->dwFromMatchID);
	//	if (pMatchOption!=NULL)
	//	{
	//		if (pMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
	//		{
	//			if (m_pMatchOption->cbFilterType!=FILTER_TYPE_SINGLE_TURN)
	//			{
	//				AfxMessageBox(TEXT("您选择了即时赛作为比赛用户的来源，筛选方式必须为单轮比赛排名！"));

	//				return false;
	//			}
	//		}

	//		if (pMatchOption->cbMatchType==MATCH_TYPE_LOCKTIME)
	//		{
	//			if (m_pMatchOption->cbFilterType!=FILTER_TYPE_TOTAL_RANKING)
	//			{
	//				AfxMessageBox(TEXT("您选择了定时赛作为比赛用户的来源，筛选方式必须为比赛总排名！"));

	//				return false;
	//			}
	//		}
	//	}
	//}

	//特殊判断
	/*if (m_pMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE && m_pMatchOption->cbSignupMode&SIGNUP_MODE_MATCH_USER)
	{
		AfxMessageBox(TEXT("即时赛不能选择比赛用户作为报名方式，请重新选择！"));

		return false;
	}*/

	return true;
}

//调整控件
VOID CDlgMatchSignup1::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//类型变更
VOID CDlgMatchSignup1::OnEventMatchTypeChanged(BYTE cbMatchType)
{
	//设置控件
	CButton * pCheckButton=(CButton *)GetDlgItem(IDC_CHECK_MATCH_USER);
	if (pCheckButton!=NULL)
	{
		pCheckButton->SetCheck(FALSE);
		pCheckButton->EnableWindow(cbMatchType==MATCH_TYPE_LOCKTIME);
	}	
}

//选择变更
void CDlgMatchSignup1::OnCbnSelchangeComboFromMatch()
{
	//获取对象
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_FROM_MATCH);
	if (pComboBox!=NULL && pComboBox->GetCurSel()!=LB_ERR)
	{
		tagMatchOption * pMatchOption=(tagMatchOption *)pComboBox->GetItemDataPtr(pComboBox->GetCurSel());

		//设置控件
		GetDlgItem(IDC_DATE_MATCH_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_LOOP_PERIOD)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_SINGLE_RANK)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_SINGLE_TURN)->EnableWindow(TRUE);		
		GetDlgItem(IDC_EDIT_TOTAL_RANK)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_TOTAL_RANKING)->EnableWindow(TRUE);				

		//即时赛类型
		if (pMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
		{
			//禁用控件
			GetDlgItem(IDC_EDIT_TOTAL_RANK)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_TOTAL_RANKING)->EnableWindow(FALSE);			

			//设置选择
			CheckRadioButton(IDC_RADIO_SINGLE_TURN,IDC_RADIO_TOTAL_RANKING,IDC_RADIO_SINGLE_TURN);
		}

		//定时赛类型
		if (pMatchOption->cbMatchType==MATCH_TYPE_LOCKTIME)
		{
			//禁用控件
			GetDlgItem(IDC_DATE_MATCH_START)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_LOOP_PERIOD)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_SINGLE_TURN)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_SINGLE_RANK)->EnableWindow(FALSE);

			//设置选择
			CheckRadioButton(IDC_RADIO_SINGLE_TURN,IDC_RADIO_TOTAL_RANKING,IDC_RADIO_TOTAL_RANKING);
		}		
	}	
}


//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgMatchSignup2::CDlgMatchSignup2() : CDlgMatchItem(IDD_MATCH_SIGNUP2)
{
}

//析构函数
CDlgMatchSignup2::~CDlgMatchSignup2()
{
}

//初始化函数
BOOL CDlgMatchSignup2::OnInitDialog()
{
	__super::OnInitDialog();

	//用户经验
	if (m_pMatchOption->cbJoinCondition&MATCH_JOINCD_EXPERIENCE)
	{
		//设置选择
		((CButton *)GetDlgItem(IDC_CHECK_EXPERIENCE))->SetCheck(TRUE);

		//设置经验
		SetDlgItemInt(IDC_EDIT_EXPERIENCE,m_pMatchOption->lExperience);
	}

  //会员等级
	if (m_pMatchOption->cbJoinCondition&MATCH_JOINCD_MEMBER_ORDER)
	{
		//设置选择
		((CButton *)GetDlgItem(IDC_CHECK_MEMBER_ORDER))->SetCheck(TRUE);

		//会员等级
		BYTE cbMemberIndex=0;
		BYTE cbMemberOrder[]={0,6,9,12,15};
		for (BYTE i=0; i<CountArray(cbMemberOrder); i++)
		{
			if (m_pMatchOption->cbMemberOrder==cbMemberOrder[i])
			{
				cbMemberIndex=i;
				break;
			}
		}

		//设置选择
		CheckRadioButton(IDC_RADIO_NOAMAL,IDC_RADIO_MEMBER_D,IDC_RADIO_NOAMAL+cbMemberIndex);		
	}
	else
	{
		//默认设置
		CheckRadioButton(IDC_RADIO_NOAMAL,IDC_RADIO_MEMBER_D,IDC_RADIO_NOAMAL);		
	}	

	return FALSE;
}

//保存输入
bool CDlgMatchSignup2::SaveInputInfo()
{
	//用户经验
	if (IsDlgButtonChecked(IDC_CHECK_EXPERIENCE))
	{
		m_pMatchOption->cbJoinCondition|=MATCH_JOINCD_EXPERIENCE;

		//设置经验
		m_pMatchOption->lExperience=GetDlgItemInt(IDC_EDIT_EXPERIENCE);
	}
	else
	{
		m_pMatchOption->cbJoinCondition&=~MATCH_JOINCD_EXPERIENCE;
	}

	//会员等级
	if (IsDlgButtonChecked(IDC_CHECK_MEMBER_ORDER))
	{
		m_pMatchOption->cbJoinCondition|=MATCH_JOINCD_MEMBER_ORDER;		

		//设置等级
		BYTE cbMemberOrder[]={0,6,9,12,15};
		UINT nCheckedID=GetCheckedRadioButton(IDC_RADIO_NOAMAL,IDC_RADIO_MEMBER_D);
		m_pMatchOption->cbMemberOrder=cbMemberOrder[nCheckedID-IDC_RADIO_NOAMAL];
	}
	else
	{
		m_pMatchOption->cbJoinCondition&=~MATCH_JOINCD_MEMBER_ORDER;
	}	

	//报名方式
	if (m_pMatchOption->cbSignupMode==0)
	{
		AfxMessageBox(TEXT("报名方式不能为空，请重新选择！"));

		return false;
	}

	return true;
}

//调整控件
VOID CDlgMatchSignup2::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgMatchDistribute::CDlgMatchDistribute() : CDlgMatchItem(IDD_MATCH_DISTRIBUTE)
{
}

//析构函数
CDlgMatchDistribute::~CDlgMatchDistribute()
{
}

//初始化函数
BOOL CDlgMatchDistribute::OnInitDialog()
{
	__super::OnInitDialog();

	//设置控件
	((CEdit *)GetDlgItem(IDC_DISTRIBUTE_MINCOUNT))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_DISTRIBUTE_INTERVAL))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_PARTAKE_MIN_GAMEUSER))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_PARTAKE_MAX_GAMEUSER))->LimitText(2);	

	//提取配置
	auto pMatchDistribute = (tagMatchDistribute*)m_pMatchOption->cbMatchDistribute;

	//同桌选项
	if ((pMatchDistribute->wDistributeRule&DISTRIBUTE_SAME_TABLE)!=0)
	{
		((CButton *)GetDlgItem(IDC_ALLOW_LAST_SAMETABLE))->SetCheck(TRUE);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_ALLOW_LAST_SAMETABLE))->SetCheck(FALSE);
	}

	//地址选项
	if ((pMatchDistribute->wDistributeRule&DISTRIBUTE_SAME_ADDRESS)!=0)
	{
		((CButton *)GetDlgItem(IDC_ALLOW_SAME_ADDRESS))->SetCheck(TRUE);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_ALLOW_SAME_ADDRESS))->SetCheck(FALSE);
	}

	//配桌人数
	if (pMatchDistribute->wDistributeMinUser!=0)
	{
		SetDlgItemInt(IDC_DISTRIBUTE_MINCOUNT, pMatchDistribute->wDistributeMinUser);
	}

	//分组间隔
	if (pMatchDistribute->wDistributeInterval!=0)
	{
		SetDlgItemInt(IDC_DISTRIBUTE_INTERVAL, pMatchDistribute->wDistributeInterval);
	}

	//最小人数
	if (pMatchDistribute->wMinPartakeGameUser!=0)
	{
		SetDlgItemInt(IDC_PARTAKE_MIN_GAMEUSER, pMatchDistribute->wMinPartakeGameUser);
	}

	//最大人数
	if (pMatchDistribute->wMaxPartakeGameUser!=0)
	{
		SetDlgItemInt(IDC_PARTAKE_MAX_GAMEUSER, pMatchDistribute->wMaxPartakeGameUser);
	}

	return FALSE;
}

//保存输入
bool CDlgMatchDistribute::SaveInputInfo()
{
	//提取配置
	auto pMatchDistribute = (tagMatchDistribute*)m_pMatchOption->cbMatchDistribute;

	//配桌参数
	pMatchDistribute->wDistributeMinUser=GetDlgItemInt(IDC_DISTRIBUTE_MINCOUNT);
	pMatchDistribute->wDistributeInterval=GetDlgItemInt(IDC_DISTRIBUTE_INTERVAL);
	pMatchDistribute->wMinPartakeGameUser=GetDlgItemInt(IDC_PARTAKE_MIN_GAMEUSER);
	pMatchDistribute->wMaxPartakeGameUser=GetDlgItemInt(IDC_PARTAKE_MAX_GAMEUSER);

	//同桌选项
	if (((CButton *)GetDlgItem(IDC_ALLOW_LAST_SAMETABLE))->GetCheck()==BST_CHECKED)
	{
		pMatchDistribute->wDistributeRule|=DISTRIBUTE_SAME_TABLE;
	}
	else
	{
		pMatchDistribute->wDistributeRule&=~DISTRIBUTE_SAME_TABLE;
	}

	//地址选项
	if (((CButton *)GetDlgItem(IDC_ALLOW_SAME_ADDRESS))->GetCheck()==BST_CHECKED)
	{
		pMatchDistribute->wDistributeRule|=DISTRIBUTE_SAME_ADDRESS;
	}
	else
	{
		pMatchDistribute->wDistributeRule&=~DISTRIBUTE_SAME_ADDRESS;
	}

	return true;
}

//调整控件
VOID CDlgMatchDistribute::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}


////////////////////////////////////////////////////////////////////////////
//
////构造函数
//CDlgMatchRankingMode::CDlgMatchRankingMode() : CDlgMatchItem(IDD_MATCH_RANKING_MODE)
//{
//}
//
////析构函数
//CDlgMatchRankingMode::~CDlgMatchRankingMode()
//{
//}
//
////初始化函数
//BOOL CDlgMatchRankingMode::OnInitDialog()
//{
//	__super::OnInitDialog();
//
//	//设置控件
//	((CEdit *)GetDlgItem(IDC_EDIT_COUNT_INNINGS))->LimitText(3);
//
//	//设置控件
//	CheckRadioButton(IDC_RADIO_TOTAL_GRADES,IDC_RADIO_SPECIALLY_GRADES,IDC_RADIO_TOTAL_GRADES+(m_pMatchOption->cbRankingMode-RANKING_MODE_TOTAL_GRADES));
//
//	//特定局数排名
//	if (IsDlgButtonChecked(IDC_RADIO_SPECIALLY_GRADES))
//	{
//		//统计局数
//		SetDlgItemInt(IDC_EDIT_COUNT_INNINGS,m_pMatchOption->wCountInnings);
//
//		//设置控件
//		CheckRadioButton(IDC_RADIO_BEST_GRADES,IDC_RADIO_AVERAGEEX_GRADES,IDC_RADIO_BEST_GRADES+(m_pMatchOption->cbFilterGradesMode-FILTER_GRADES_MODE_BEST));
//	}
//	else
//	{
//		CheckRadioButton(IDC_RADIO_BEST_GRADES,IDC_RADIO_AVERAGEEX_GRADES,IDC_RADIO_BEST_GRADES);
//	}
//
//	return FALSE;
//}
//
////保存输入
//bool CDlgMatchRankingMode::SaveInputInfo()
//{
//	//排名方式
//	if (IsDlgButtonChecked(IDC_RADIO_TOTAL_GRADES)) m_pMatchOption->cbRankingMode=RANKING_MODE_TOTAL_GRADES;
//	if (IsDlgButtonChecked(IDC_RADIO_SPECIALLY_GRADES)) m_pMatchOption->cbRankingMode=RANKING_MODE_SPECIAL_GRADES;
//
//	//特定局数
//	if (m_pMatchOption->cbRankingMode==RANKING_MODE_SPECIAL_GRADES)
//	{
//		//统计局数
//		m_pMatchOption->wCountInnings=GetDlgItemInt(IDC_EDIT_COUNT_INNINGS);
//
//		//合法校验
//		if (m_pMatchOption->wCountInnings==0)
//		{
//			AfxMessageBox(TEXT("统计局数设置不合法，请重新设置！"));
//
//			return false;
//		}
//
//		//筛选模式
//		if (IsDlgButtonChecked(IDC_RADIO_BEST_GRADES)) m_pMatchOption->cbFilterGradesMode=FILTER_GRADES_MODE_BEST;
//		if (IsDlgButtonChecked(IDC_RADIO_AVERAGE_GRADES)) m_pMatchOption->cbFilterGradesMode=FILTER_GRADES_MODE_AVERAGE;
//		if (IsDlgButtonChecked(IDC_RADIO_AVERAGEEX_GRADES)) m_pMatchOption->cbFilterGradesMode=FILTER_GRADES_MODE_AVERAGEEX;
//	}	
//
//	return true;
//}
//
////调整控件
//VOID CDlgMatchRankingMode::RectifyControl(INT nWidth, INT nHeight)
//{
//	return;
//}
//
////类型变更
//VOID CDlgMatchRankingMode::OnEventMatchTypeChanged(BYTE cbMatchType)
//{
//	//设置控件
//	GetDlgItem(IDC_RADIO_SPECIALLY_GRADES)->EnableWindow(cbMatchType==MATCH_TYPE_LOCKTIME);
//}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgMatchReward::CDlgMatchReward() : CDlgMatchItem(IDD_MATCH_REWARD)
{
}

//析构函数
CDlgMatchReward::~CDlgMatchReward()
{
}

//初始化函数
BOOL CDlgMatchReward::OnInitDialog()
{
	__super::OnInitDialog();	

	//设置控件
	((CEdit*)GetDlgItem(IDC_REWARD_GOODS))->LimitText(CountArray(tagMatchRewardItem::szRewardGoods));
	((CEdit*)GetDlgItem(IDC_REWARD_ENTITY))->LimitText(CountArray(tagMatchRewardItem::szRewardEntity));

	for (WORD i=0; i<m_pMatchOption->wRewardCount; i++)
	{
		m_RewardInfoBuffer.InsertMatchRewardItem(m_pMatchOption->MatchRewardItem[i]);
	}
	
	for(INT_PTR i=0;i<m_RewardInfoBuffer.m_MatchRewardItemArray.GetCount();i++) 
	{
		m_RewardListControl.InsertMatchReward(m_RewardInfoBuffer.m_MatchRewardItemArray[i]);
	}	

	return FALSE;
}

//控件绑定
VOID CDlgMatchReward::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//奖励列表
	DDX_Control(pDX,IDC_LIST_REWARD,m_RewardListControl);
}

//保存输入
bool CDlgMatchReward::SaveInputInfo()
{
	//变量定义
	m_pMatchOption->wRewardCount=__min(CountArray(m_pMatchOption->MatchRewardItem),m_RewardInfoBuffer.GetMatchRewardCount());

	for (WORD i=0; i<m_pMatchOption->wRewardCount; i++)
	{
		CopyMemory(&m_pMatchOption->MatchRewardItem[i],m_RewardInfoBuffer.m_MatchRewardItemArray[i],sizeof(m_pMatchOption->MatchRewardItem[i]));
	}

	//奖励判断
	if (m_pMatchOption->wRewardCount==0)
	{
		AfxMessageBox(TEXT("请至少配置一项奖励！"));

		return false;
	}

	return true;
}

//调整控件
VOID CDlgMatchReward::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//奖励配置
bool CDlgMatchReward::GetMatchRewardItem(tagMatchRewardItem & MatchRewardItem)
{
	//获取名次
	MatchRewardItem.wEndRankID=GetDlgItemInt(IDC_RANKID_END);
	MatchRewardItem.wStartRankID=GetDlgItemInt(IDC_RANKID_START);	

	//调整名次
	if (MatchRewardItem.wEndRankID==0) 
	{
		MatchRewardItem.wEndRankID=MatchRewardItem.wStartRankID;
	}

	//名次校验
	if (MatchRewardItem.wStartRankID == 0 || MatchRewardItem.wEndRankID < MatchRewardItem.wStartRankID)
	{
		AfxMessageBox(TEXT("您输入的比赛名次不合法，请重新输入！"));

		//设置焦点
		GetDlgItem(IDC_RANKID_END)->SetFocus();

		return false;
	}

	//实物奖励
	TCHAR szRewardEntity[CountArray(MatchRewardItem.szRewardEntity)];
	GetDlgItemText(IDC_REWARD_ENTITY,szRewardEntity,CountArray(szRewardEntity));

	//物品奖励
	TCHAR szRewardGoods[CountArray(MatchRewardItem.szRewardGoods)];
	GetDlgItemText(IDC_REWARD_GOODS, szRewardGoods, CountArray(szRewardGoods));

	//校验奖励
	if (szRewardEntity[0] == 0 && szRewardGoods[0] == 0)
	{
		AfxMessageBox(TEXT("请至少设置一种奖励！"));

		//设置焦点
		GetDlgItem(IDC_REWARD_ENTITY)->SetFocus();

		return false;
	}

	//数组定义
	CGoodsItemArray GoodsItemArray;

	//校验实物
	GoodsItemArray.RemoveAll();
	if (szRewardEntity[0] != 0 && !g_GameGoodsManager->ParseFromJson(szRewardEntity, GoodsItemArray, GoodsItemArray))
	{
		AfxMessageBox(TEXT("您输入的\"实物奖励\"格式不合法或长度过长，请认真检查后重新输入！"));

		//设置焦点
		GetDlgItem(IDC_REWARD_ENTITY)->SetFocus();

		return false;		
	}

	//校验物品
	GoodsItemArray.RemoveAll();
	if (szRewardGoods[0] != 0 && !g_GameGoodsManager->ParseFromJson(szRewardGoods, GoodsItemArray, GoodsItemArray))
	{
		AfxMessageBox(TEXT("您输入的\"物品奖励\"格式不合法或长度过长，请认真检查后重新输入！"));

		//设置焦点
		GetDlgItem(IDC_REWARD_GOODS)->SetFocus();

		return false;
	}	

	//拷贝数据
	StringCchCopy(MatchRewardItem.szRewardGoods, CountArray(MatchRewardItem.szRewardGoods), szRewardGoods);

	//拷贝数据
	StringCchCopy(MatchRewardItem.szRewardEntity, CountArray(MatchRewardItem.szRewardEntity), szRewardEntity);	

	return true;
}

//清空控件
VOID CDlgMatchReward::CleanControl()
{
	//设置控件
	SetDlgItemText(IDC_RANKID_END,TEXT(""));
	SetDlgItemText(IDC_RANKID_START,TEXT(""));	
	SetDlgItemText(IDC_REWARD_GOLD,TEXT(""));
	SetDlgItemText(IDC_REWARD_MEDAL,TEXT(""));
	SetDlgItemText(IDC_REWARD_INGOT,TEXT(""));
	SetDlgItemText(IDC_REWARD_ENTITY,TEXT(""));
}

//设置配置
VOID CDlgMatchReward::SetMatchRewardItem(tagMatchRewardItem * pMatchRewardItem)
{
	//参数校验
	ASSERT(pMatchRewardItem!=NULL);
	if (pMatchRewardItem==NULL) return;	

	//设置名次
	SetDlgItemInt(IDC_RANKID_END,(UINT)pMatchRewardItem->wEndRankID);
	SetDlgItemInt(IDC_RANKID_START,(UINT)pMatchRewardItem->wStartRankID);	

	//实物奖励
	SetDlgItemText(IDC_REWARD_ENTITY,pMatchRewardItem->szRewardEntity);

	//物品奖励
	SetDlgItemText(IDC_REWARD_GOODS, pMatchRewardItem->szRewardGoods);
}


//添加奖励
VOID CDlgMatchReward::OnBnClickedAddReward()
{
	//变量定义	
	tagMatchRewardItem MatchRewardItem;

	//获取配置
	if (GetMatchRewardItem(MatchRewardItem)==false) return;	

	//存在判断
	for (WORD wRankID=MatchRewardItem.wStartRankID; wRankID<=MatchRewardItem.wEndRankID; wRankID++)
	{
		//存在判断
		if (m_RewardInfoBuffer.SearchMatchReward(wRankID)!=NULL)
		{
			AfxMessageBox(TEXT("抱歉，该名次的奖励已存在,如要修改请点击修改奖励按钮！"));

			return;
		}		
	}

	//插入奖励
	tagMatchRewardItem * pMatchRewardItem=m_RewardInfoBuffer.InsertMatchRewardItem(MatchRewardItem);
	if (pMatchRewardItem!=NULL)
	{
		m_RewardListControl.InsertMatchReward(pMatchRewardItem);
	}

	//清理控件
	CleanControl();
}

//修改奖励
VOID CDlgMatchReward::OnBnClickedModifyReward()
{
	//变量定义	
	tagMatchRewardItem MatchRewardItem;

	//获取配置
	if (GetMatchRewardItem(MatchRewardItem) == false) return;

	//查询奖励
	auto pMatchRewardItem = m_RewardInfoBuffer.SearchMatchReward(MatchRewardItem.wStartRankID);
	if (pMatchRewardItem == NULL)
	{
		AfxMessageBox(TEXT("抱歉，未查询到该名次区间的奖励配置！"));
		return;
	}

	//校验名次
	if (pMatchRewardItem->wStartRankID != MatchRewardItem.wStartRankID || pMatchRewardItem->wEndRankID != MatchRewardItem.wEndRankID)
	{
		AfxMessageBox(TEXT("抱歉，名次区间不可修改！"));
		return;
	}

	//拷贝数据
	CopyMemory(pMatchRewardItem,&MatchRewardItem,sizeof(tagMatchRewardItem));

	//更新数据
	m_RewardListControl.UpdateMatchReward(pMatchRewardItem);
}

//修改奖励
VOID CDlgMatchReward::OnBnClickedDeleteReward()
{
	//变量定义	
	tagMatchRewardItem MatchRewardItem;

	//获取配置
	MatchRewardItem.wEndRankID = GetDlgItemInt(IDC_RANKID_END);
	MatchRewardItem.wStartRankID = GetDlgItemInt(IDC_RANKID_START);

	//查找奖励
	tagMatchRewardItem * pMatchRewardItem=m_RewardInfoBuffer.SearchMatchReward(MatchRewardItem.wStartRankID);
	if (pMatchRewardItem==NULL)
	{
		AfxMessageBox(TEXT("抱歉，该名次的奖励不存在,如要添加请点击添加奖励按钮！"));
		return;
	}

	//更新控件
	if (pMatchRewardItem->wStartRankID==MatchRewardItem.wStartRankID && pMatchRewardItem->wEndRankID==MatchRewardItem.wEndRankID)
	{
		m_RewardListControl.DeleteMatchReward(pMatchRewardItem);
		m_RewardInfoBuffer.DeleteMatchRewardItem(pMatchRewardItem->wStartRankID);	
	}

	//清理控件 
	CleanControl();
}

//子项变更
VOID CDlgMatchReward::OnLvnItemchangedListReward(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	//变量定义
	if (pNMLV->iItem!=LB_ERR)
	{
		//设置配置
		tagMatchRewardItem * pMatchRewardItem=(tagMatchRewardItem *)m_RewardListControl.GetItemData(pNMLV->iItem);
		if (pMatchRewardItem!=NULL) SetMatchRewardItem(pMatchRewardItem);
	}

	*pResult = 0;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgMatchLockTime::CDlgMatchLockTime() : CDlgMatchItem(IDD_MATCH_LOCKTIME)
{
}

//析构函数
CDlgMatchLockTime::~CDlgMatchLockTime()
{
}

//初始化函数
BOOL CDlgMatchLockTime::OnInitDialog()
{
	__super::OnInitDialog();

	//输入限制	
	((CEdit *)GetDlgItem(IDC_REVIVE_FEE))->LimitText(8);	
	((CEdit *)GetDlgItem(IDC_REVIVE_TIMES))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_REVIVE_MULTI))->LimitText(8);
	((CEdit *)GetDlgItem(IDC_MIN_PLAYER_COUNT))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_MAX_PLAYER_COUNT))->LimitText(2);

	//变量定义
	TCHAR szValue[8];

	//周
	for (int i = 1; i <= 7; i++)
	{
		_sntprintf_s(szValue, CountArray(szValue), TEXT(" %d"), i);
		auto pItemData = (tagItemData*)m_ComboWeekDay.GetItemDataPtr(m_ComboWeekDay.InsertString(i - 1, szValue));
		if (pItemData != NULL)
		{
			pItemData->dwDataPtr = i;
		}
	}

	//月
	for (int i = 1; i <= 31; i++)
	{
		_sntprintf_s(szValue, CountArray(szValue), TEXT(" %d"), i);
		auto pItemData = (tagItemData*)m_ComboMonthDay.GetItemDataPtr(m_ComboMonthDay.InsertString(i - 1, szValue));
		if (pItemData != NULL)
		{
			pItemData->dwDataPtr = i;
		}
	}

	//时
	CComboBox * pComboBox1=(CComboBox *)GetDlgItem(IDC_COMBO_HOUR_1);
	CComboBox * pComboBox2=(CComboBox *)GetDlgItem(IDC_COMBO_HOUR_2);
	CComboBox * pComboBox3=(CComboBox *)GetDlgItem(IDC_COMBO_HOUR_3);
	for (int i=0; i<24; i++)
	{
		_itot_s(i,szValue,10);
		pComboBox1->SetItemData(pComboBox1->InsertString(i,szValue),i);
		pComboBox2->SetItemData(pComboBox2->InsertString(i,szValue),i);
	}

	for (int i=0; i<240; i++)
	{
		_itot_s(i,szValue,10);
		pComboBox3->SetItemData(pComboBox3->InsertString(i,szValue),i);
	}

	//分
	pComboBox1=(CComboBox *)GetDlgItem(IDC_COMBO_MINUTE_1);
	pComboBox2=(CComboBox *)GetDlgItem(IDC_COMBO_MINUTE_2);
	pComboBox3=(CComboBox *)GetDlgItem(IDC_COMBO_MINUTE_3);
	for (int i=0; i<60; i++)
	{
		_itot_s(i,szValue,10);
		pComboBox1->SetItemData(pComboBox1->InsertString(i,szValue),i);
		pComboBox2->SetItemData(pComboBox2->InsertString(i,szValue),i);
		pComboBox3->SetItemData(pComboBox3->InsertString(i,szValue),i);
	}

	//秒
	pComboBox1=(CComboBox *)GetDlgItem(IDC_COMBO_SECOND_1);
	pComboBox2=(CComboBox *)GetDlgItem(IDC_COMBO_SECOND_2);
	pComboBox3=(CComboBox *)GetDlgItem(IDC_COMBO_SECOND_3);
	for (int i=0; i<60; i++)
	{
		_itot_s(i,szValue,10);
		pComboBox1->SetItemData(pComboBox1->InsertString(i,szValue),i);
		pComboBox2->SetItemData(pComboBox2->InsertString(i,szValue),i);
		pComboBox3->SetItemData(pComboBox3->InsertString(i,szValue),i);
	}

	//变量定义
	tagLockTimeMatch * pLockTimeMatch=(tagLockTimeMatch *)m_pMatchOption->cbMatchRule;

	//循环开赛
	if (pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND)
	{
		CheckRadioButton(IDC_RADIO_MATCH_LOCKTIME,IDC_RADIO_MATCH_ROUND,IDC_RADIO_MATCH_ROUND);
	}

	//定时开赛
	if (pLockTimeMatch->cbMatchMode==MATCH_MODE_LOCKTIME)
	{
		CheckRadioButton(IDC_RADIO_MATCH_LOCKTIME,IDC_RADIO_MATCH_ROUND,IDC_RADIO_MATCH_LOCKTIME);
	}	

	//游戏人数
	if (pLockTimeMatch->wMinPlayerCount!=0)
	{
		SetDlgItemInt(IDC_MIN_PLAYER_COUNT,(UINT)pLockTimeMatch->wMinPlayerCount);
	}

	//游戏人数
	if (pLockTimeMatch->wMaxPlayerCount!=0)
	{
		SetDlgItemInt(IDC_MAX_PLAYER_COUNT,(UINT)pLockTimeMatch->wMaxPlayerCount);
	}

	//复活分数
	if (pLockTimeMatch->lReviveMulti!=0)
	{
		SetDlgItemInt(IDC_REVIVE_MULTI,(UINT)pLockTimeMatch->lReviveMulti);
	}

	//复活次数
	if (pLockTimeMatch->cbReviveTimes!=0)
	{
		SetDlgItemInt(IDC_REVIVE_TIMES,(UINT)pLockTimeMatch->cbReviveTimes);
	}

	//复活费用
	if (pLockTimeMatch->lFeeAmount!=0)
	{
		SetDlgItemInt(IDC_REVIVE_FEE,(UINT)pLockTimeMatch->lFeeAmount);
	}	

	//费用类型
	switch (pLockTimeMatch->wFeeMoneyID)
	{
	case GOODS_ID_GOLD:		//金币
	{
		CheckRadioButton(IDC_RADIO_FEETYPE_GOLD, IDC_RADIO_FEETYPE_INGOT, IDC_RADIO_FEETYPE_GOLD);
		break;
	}
	//case GOODS_ID_INGOT:	//钻石
	//{
	//	CheckRadioButton(IDC_RADIO_FEETYPE_GOLD, IDC_RADIO_FEETYPE_INGOT, IDC_RADIO_FEETYPE_INGOT);
	//	break;
	//}
	}	

	//时间定义	
	SYSTEMTIME MatchInterVal=pLockTimeMatch->MatchInterval;
	SYSTEMTIME MatchLockTime=pLockTimeMatch->MatchLockTime;		
	SYSTEMTIME MatchSignupTime=pLockTimeMatch->MatchSignupTime;
	SYSTEMTIME MatchStartTime=pLockTimeMatch->MatchStartTime;	
	SYSTEMTIME MatchCutOffTime=pLockTimeMatch->MatchCutOffTime;	

	//开始日期
	CDateTimeCtrl * pStartDate=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_START_DATE);
	pStartDate->SetTime(&MatchStartTime);

	//结束日期
	CDateTimeCtrl * pCutOffDate=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_CUTOFF_DATE);
	pCutOffDate->SetTime(&MatchCutOffTime);

	//开始时间
	CDateTimeCtrl * pStartTime=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_START_TIME);
	pStartTime->SetTime(&MatchStartTime);

	//结束时间
	CDateTimeCtrl * pCutOffTime=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_CUTOFF_TIME);
	pCutOffTime->SetTime(&MatchCutOffTime);

  //开赛间隔
	SetMatchTime(MatchInterVal,IDC_COMBO_HOUR_1,IDC_COMBO_MINUTE_1,IDC_COMBO_SECOND_1);
	
	//开赛时间
	SetMatchTime(MatchLockTime,IDC_COMBO_HOUR_2,IDC_COMBO_MINUTE_2,IDC_COMBO_SECOND_2);

	//报名时间
	SetMatchTime(MatchSignupTime,IDC_COMBO_HOUR_3,IDC_COMBO_MINUTE_3,IDC_COMBO_SECOND_3);

	//每周开
	if (pLockTimeMatch->cbMatchCycle==MATCH_CYCLE_PER_WEEK)
	{
		CheckRadioButton(IDC_RADIO_EVERY_DAY,IDC_RADIO_EVERY_MONTH,IDC_RADIO_EVERY_WEEK);		
		SetComboDataList(&m_ComboWeekDay, pLockTimeMatch->cbDayFlagList, CountArray(pLockTimeMatch->cbDayFlagList));
	}
	//每月开
	else if (MatchLockTime.wDay!=0)
	{
		CheckRadioButton(IDC_RADIO_EVERY_DAY,IDC_RADIO_EVERY_MONTH,IDC_RADIO_EVERY_MONTH);
		SetComboDataList(&m_ComboMonthDay, pLockTimeMatch->cbDayFlagList,CountArray(pLockTimeMatch->cbDayFlagList));
	}
	//每天开
	else
	{
		CheckRadioButton(IDC_RADIO_EVERY_DAY,IDC_RADIO_EVERY_MONTH,IDC_RADIO_EVERY_DAY);
	}

	return FALSE;
}

//交换数据
VOID CDlgMatchLockTime::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_COMBO_WEEK_DAY, m_ComboWeekDay);
	DDX_Control(pDX, IDC_COMBO_MONTH_DAY, m_ComboMonthDay);

	__super::DoDataExchange(pDX);
}

//保存输入
bool CDlgMatchLockTime::SaveInputInfo()
{
	//变量定义
	tagLockTimeMatch * pLockTimeMatch=(tagLockTimeMatch *)m_pMatchOption->cbMatchRule;
	ZeroMemory(pLockTimeMatch,sizeof(tagLockTimeMatch));

	//获取控件
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_MATCH_MODE);

	//基础配置
	pLockTimeMatch->wMinPlayerCount=GetDlgItemInt(IDC_MIN_PLAYER_COUNT);
	pLockTimeMatch->wMaxPlayerCount=GetDlgItemInt(IDC_MAX_PLAYER_COUNT);

	//复活配置
	pLockTimeMatch->lFeeAmount=GetDlgItemInt(IDC_REVIVE_FEE);
	pLockTimeMatch->lReviveMulti=GetDlgItemInt(IDC_REVIVE_MULTI);
	pLockTimeMatch->cbReviveTimes=GetDlgItemInt(IDC_REVIVE_TIMES);

	//费用类型
	if (IsDlgButtonChecked(IDC_RADIO_FEETYPE_GOLD)) pLockTimeMatch->wFeeMoneyID=GOODS_ID_GOLD;
	//if (IsDlgButtonChecked(IDC_RADIO_FEETYPE_INGOT)) pLockTimeMatch->wFeeMoneyID=GOODS_ID_INGOT;

	//开赛模式
	if (IsDlgButtonChecked(IDC_RADIO_MATCH_ROUND)) pLockTimeMatch->cbMatchMode=MATCH_MODE_ROUND;
	if (IsDlgButtonChecked(IDC_RADIO_MATCH_LOCKTIME)) pLockTimeMatch->cbMatchMode=MATCH_MODE_LOCKTIME;

	//游戏人数
	if (pLockTimeMatch->wMinPlayerCount==0)
	{
		AfxMessageBox(TEXT("最少游戏人数设置不合法，请重新输入！"));

		GetDlgItem(IDC_MIN_PLAYER_COUNT)->SetFocus();

		return false;
	}
	
	//游戏人数
	if (pLockTimeMatch->wMaxPlayerCount!=0 && pLockTimeMatch->wMaxPlayerCount<pLockTimeMatch->wMinPlayerCount)
	{
		AfxMessageBox(TEXT("最大游戏人数设置不合法，请重新输入！"));

		GetDlgItem(IDC_MIN_PLAYER_COUNT)->SetFocus();

		return false;
	}
  
	//时间定义
	CTime MatchTime,MatchDate;
	CDateTimeCtrl * pDatTimeCtrl;				
  
	//开始日期
	pDatTimeCtrl=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_START_DATE);
	pDatTimeCtrl->GetTime(MatchDate);

	//开始时间
	pDatTimeCtrl=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_START_TIME);
	pDatTimeCtrl->GetTime(MatchTime);

	//合并时间
	CTime MatchStartTime(MatchDate.GetYear(),MatchDate.GetMonth(),MatchDate.GetDay(),MatchTime.GetHour(),MatchTime.GetMinute(),MatchTime.GetSecond());	

	//比赛时间	
	MatchStartTime.GetAsSystemTime(pLockTimeMatch->MatchStartTime);

	//截止日期
	pDatTimeCtrl=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_CUTOFF_DATE);
	pDatTimeCtrl->GetTime(MatchDate);	

	//截止时间
	pDatTimeCtrl=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_CUTOFF_TIME);
	pDatTimeCtrl->GetTime(MatchTime);

	//合并时间
	CTime MatchCutOffTime(MatchDate.GetYear(),MatchDate.GetMonth(),MatchDate.GetDay(),MatchTime.GetHour(),MatchTime.GetMinute(),MatchTime.GetSecond());

	//获取时间
	MatchCutOffTime.GetAsSystemTime(pLockTimeMatch->MatchCutOffTime);	

	//报名时间
	GetMatchTime(pLockTimeMatch->MatchSignupTime,IDC_COMBO_HOUR_3,IDC_COMBO_MINUTE_3,IDC_COMBO_SECOND_3);

	//循环开赛
	if (pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND)
	{
		//设置变量
		pLockTimeMatch->MatchInterval.wDay=0;
		pLockTimeMatch->MatchInterval.wYear=0;		
		pLockTimeMatch->MatchInterval.wMonth=0;
		pLockTimeMatch->MatchInterval.wDayOfWeek=0;

		//获取时间
		GetMatchTime(pLockTimeMatch->MatchInterval,IDC_COMBO_HOUR_1,IDC_COMBO_MINUTE_1,IDC_COMBO_SECOND_1);				
	}

	//定时开赛
	if (pLockTimeMatch->cbMatchMode==MATCH_MODE_LOCKTIME)
	{
		//设置变量
		pLockTimeMatch->MatchLockTime.wDay=0;
		pLockTimeMatch->MatchLockTime.wYear=0;		
		pLockTimeMatch->MatchLockTime.wMonth=0;
		pLockTimeMatch->MatchLockTime.wDayOfWeek=0;

		//开始时间
		GetMatchTime(pLockTimeMatch->MatchLockTime,IDC_COMBO_HOUR_2,IDC_COMBO_MINUTE_2,IDC_COMBO_SECOND_2);					

		//每周开
		if (IsDlgButtonChecked(IDC_RADIO_EVERY_WEEK))
		{
			//获取配置
			GetComboDataList(&m_ComboWeekDay, pLockTimeMatch->cbDayFlagList, CountArray(pLockTimeMatch->cbDayFlagList));

			//设置周期
			pLockTimeMatch->cbMatchCycle = MATCH_CYCLE_PER_WEEK;
		}

		//每月开
		if (IsDlgButtonChecked(IDC_RADIO_EVERY_MONTH)) 
		{
			//获取配置
			GetComboDataList(&m_ComboMonthDay, pLockTimeMatch->cbDayFlagList, CountArray(pLockTimeMatch->cbDayFlagList));

			//设置周期
			pLockTimeMatch->cbMatchCycle = MATCH_CYCLE_PER_MONTH;
		}
	}	

	return true;
}

//调整控件
VOID CDlgMatchLockTime::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//报名时间
VOID CDlgMatchLockTime::GetMatchTime(SYSTEMTIME &MatchTime,UINT uHourID,UINT uMinuteID,UINT uSecondID)
{
	//时
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(uHourID);
	if (pComboBox!=NULL)
	{
		MatchTime.wHour=0;
		if (pComboBox->GetCurSel()!=-1) MatchTime.wHour=pComboBox->GetItemData(pComboBox->GetCurSel()); 
	}

	//分
	pComboBox=(CComboBox *)GetDlgItem(uMinuteID);
	if (pComboBox!=NULL)
	{
		MatchTime.wMinute=0;
		if (pComboBox->GetCurSel()!=-1) MatchTime.wMinute=pComboBox->GetItemData(pComboBox->GetCurSel()); 
	}

	//秒
	pComboBox=(CComboBox *)GetDlgItem(uSecondID);
	if (pComboBox!=NULL)
	{
		MatchTime.wSecond=0;
		if (pComboBox->GetCurSel()!=-1) MatchTime.wSecond=pComboBox->GetItemData(pComboBox->GetCurSel()); 
	}
}

//报名时间
VOID CDlgMatchLockTime::SetMatchTime(const SYSTEMTIME &MatchTime,UINT uHourID,UINT uMinuteID,UINT uSecondID)
{
	//时
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(uHourID);
	if (pComboBox!=NULL)
	{
		for (int i=0;i<240;i++)
		{
			if (pComboBox->GetItemData(i)==MatchTime.wHour)
			{
				pComboBox->SetCurSel(i);
				break;
			}
		}
	}

	//分
	pComboBox=(CComboBox *)GetDlgItem(uMinuteID);
	if (pComboBox!=NULL)
	{
		for (int i=0;i<60;i++)
		{
			if (pComboBox->GetItemData(i)==MatchTime.wMinute)
			{
				pComboBox->SetCurSel(i);
				break;
			}
		}
	}

	//秒
	pComboBox=(CComboBox *)GetDlgItem(uSecondID);
	if (pComboBox!=NULL)
	{
		for (int i=0;i<60;i++)
		{
			if (pComboBox->GetItemData(i)==MatchTime.wSecond)
			{
				pComboBox->SetCurSel(i);
				break;
			}
		}
	}
}

//获取天列表
VOID CDlgMatchLockTime::GetComboDataList(CCheckComboBox* pCheckComboBox, BYTE cboDataFlagList[], BYTE cbDataCount)
{
	//获取类型	
	for (INT i = 0; i < pCheckComboBox->GetCount(); i++)
	{
		auto pItemData = (tagItemData*)pCheckComboBox->GetItemDataPtr(i);
		cboDataFlagList[i] = pItemData->bChecked;
		if (i >= cbDataCount) break;
	}
}

//设置天列表
VOID CDlgMatchLockTime::SetComboDataList(CCheckComboBox* pCheckComboBox, BYTE cboDataFlagList[], BYTE cbDataCount)
{
	//获取类型	
	for (INT j = 0; j < pCheckComboBox->GetCount(); j++)
	{
		auto pItemData = (tagItemData*)pCheckComboBox->GetItemDataPtr(j);
		if (pItemData->dwDataPtr > cbDataCount) continue;
		if (cboDataFlagList[pItemData->dwDataPtr - 1] == TRUE)
		{
			pItemData->bChecked = TRUE;
		}
	}

	//更新数据
	pCheckComboBox->UpdateData();
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgMatchImmediate::CDlgMatchImmediate(): CDlgMatchItem(IDD_MATCH_IMMEDIATE)
{	
}

//析构函数
CDlgMatchImmediate::~CDlgMatchImmediate()
{
}

//初始化函数
BOOL CDlgMatchImmediate::OnInitDialog()
{
	__super::OnInitDialog();

	//输入限制
	((CEdit *)GetDlgItem(IDC_PLAYER_COUNT_MIN))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_PLAYER_COUNT_MAX))->LimitText(2);

	//变量定义
	tagImmediateMatch * pImmediateMatch=(tagImmediateMatch *)m_pMatchOption->cbMatchRule;

	//设置人数
	SetDlgItemInt(IDC_PLAYER_COUNT_MIN,pImmediateMatch->wMinPlayerCount);
	SetDlgItemInt(IDC_PLAYER_COUNT_MAX,pImmediateMatch->wMaxPlayerCount);

	//比赛时间	
	SYSTEMTIME MatchStartTime=pImmediateMatch->MatchStartTime;	
	SYSTEMTIME MatchCutOffTime=pImmediateMatch->MatchCutOffTime;	

	//开始日期
	CDateTimeCtrl * pStartDate=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_START_DATE);
	pStartDate->SetTime(&MatchStartTime);

	//开始时间
	CDateTimeCtrl * pStartTime=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_START_TIME);
	pStartTime->SetTime(&MatchStartTime);

	//截止日期
	CDateTimeCtrl * pCutOffDate=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_CUTOFF_DATE);
	pCutOffDate->SetTime(&MatchCutOffTime);

	//截止时间
	CDateTimeCtrl * pCutOffTime=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_CUTOFF_TIME);
	pCutOffTime->SetTime(&MatchCutOffTime);

	return TRUE;
}

//控件绑定
VOID CDlgMatchImmediate::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);	
}

//保存输入
bool CDlgMatchImmediate::SaveInputInfo()
{
	//变量定义
	tagImmediateMatch * pImmediateMatch=(tagImmediateMatch *)m_pMatchOption->cbMatchRule;

	//获取人数
	pImmediateMatch->wMinPlayerCount=GetDlgItemInt(IDC_PLAYER_COUNT_MIN);
	pImmediateMatch->wMaxPlayerCount=GetDlgItemInt(IDC_PLAYER_COUNT_MAX);

	//校验人数
	if (pImmediateMatch->wMinPlayerCount==0)
	{
		AfxMessageBox(TEXT("游戏人数设置有误，请重新设置！"));

		return false;
  }

	//校验人数
	if (pImmediateMatch->wMinPlayerCount>pImmediateMatch->wMaxPlayerCount)
	{
		AfxMessageBox(TEXT("游戏人数设置有误，请重新设置！"));

		return false;
  }

	//时间定义	
	CTime StartTime,StartDate;
	CTime CutOffTime,CutOffDate;		
	CDateTimeCtrl * pStartDate,* pStartTime;
	CDateTimeCtrl * pCutOffDate,* pCutOffTime;

	//开始日期
	pStartDate=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_START_DATE);
	pStartDate->GetTime(StartDate);

	//开始时间
	pStartTime=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_START_TIME);
	pStartTime->GetTime(StartTime);
  
	//截止日期
	pCutOffDate=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_CUTOFF_DATE);
	pCutOffDate->GetTime(CutOffDate);

	//截止时间
	pCutOffTime=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_CUTOFF_TIME);
	pCutOffTime->GetTime(CutOffTime);

	//合并时间	
	CTime StartDateTime(StartDate.GetYear(),StartDate.GetMonth(),StartDate.GetDay(),StartTime.GetHour(),StartTime.GetMinute(),StartTime.GetSecond());
	CTime CutOffDateTime(CutOffDate.GetYear(),CutOffDate.GetMonth(),CutOffDate.GetDay(),CutOffTime.GetHour(),CutOffTime.GetMinute(),CutOffTime.GetSecond());	

	//比赛时间	
	StartDateTime.GetAsSystemTime(pImmediateMatch->MatchStartTime);
	CutOffDateTime.GetAsSystemTime(pImmediateMatch->MatchCutOffTime);

	return true;
}

//调整控件
VOID CDlgMatchImmediate::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgMatchTournament::CDlgMatchTournament() : CDlgMatchItem(IDD_MATCH_TOURNAMENT)
{
}

//析构函数
CDlgMatchTournament::~CDlgMatchTournament()
{
}

//初始化函数
BOOL CDlgMatchTournament::OnInitDialog()
{
	__super::OnInitDialog();

	//输入限制		
	((CEdit*)GetDlgItem(IDC_EDIT_PLAY_COUNT))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_EDIT_BASE_SCORE))->LimitText(8);
	((CEdit*)GetDlgItem(IDC_EDIT_MATCH_SCORE))->LimitText(10);
	((CEdit*)GetDlgItem(IDC_MAX_TABLE_COUNT))->LimitText(5);
	((CEdit*)GetDlgItem(IDC_MIN_PLAYER_COUNT))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_MAX_PLAYER_COUNT))->LimitText(2);	

	//变量定义
	TCHAR szValue[8];
	
	//周天
	for (int i = 1; i <= 7; i++)
	{
		_sntprintf_s(szValue,CountArray(szValue),TEXT(" %d"),i);
		auto pItemData = (tagItemData *)m_ComboWeekDay.GetItemDataPtr(m_ComboWeekDay.InsertString(i-1, szValue));
		if (pItemData != NULL)
		{
			pItemData->dwDataPtr = i;
		}		
	}

	//月天
	for (int i = 1; i <= MAX_DAY; i++)
	{
		_sntprintf_s(szValue, CountArray(szValue), TEXT(" %d"), i);
		auto pItemData = (tagItemData*)m_ComboMonthDay.GetItemDataPtr(m_ComboMonthDay.InsertString(i-1, szValue));
		if (pItemData != NULL)
		{
			pItemData->dwDataPtr = i;
		}	
	}

	//年月
	for (int i = 1; i <= MAX_MONTH; i++)
	{
		_sntprintf_s(szValue, CountArray(szValue), TEXT(" %d"), i);
		auto pItemData = (tagItemData*)m_ComboYearMonth.GetItemDataPtr(m_ComboYearMonth.InsertString(i - 1, szValue));
		if (pItemData != NULL)
		{
			pItemData->dwDataPtr = i;
		}
	}

	
	//时
	CComboBox* pComboBox1 = (CComboBox*)GetDlgItem(IDC_COMBO_HOUR_1);
	CComboBox* pComboBox2 = (CComboBox*)GetDlgItem(IDC_COMBO_HOUR_2);
	CComboBox* pComboBox3 = (CComboBox*)GetDlgItem(IDC_COMBO_HOUR_3);
	CComboBox* pComboBox4 = (CComboBox*)GetDlgItem(IDC_COMBO_HOUR_4);
	CComboBox* pComboBox5 = (CComboBox*)GetDlgItem(IDC_COMBO_HOUR_5);
	CComboBox* pComboBox6 = (CComboBox*)GetDlgItem(IDC_COMBO_HOUR_6);
	for (int i = 0; i < 24; i++)
	{
		_itot_s(i, szValue, 10);
		pComboBox1->SetItemData(pComboBox1->InsertString(i, szValue), i);
		pComboBox2->SetItemData(pComboBox2->InsertString(i, szValue), i);
	}

	for (int i = 0; i < 1440; i++)
	{
		_itot_s(i, szValue, 10);
		pComboBox3->SetItemData(pComboBox3->InsertString(i, szValue), i);
		pComboBox4->SetItemData(pComboBox4->InsertString(i, szValue), i);
		pComboBox5->SetItemData(pComboBox5->InsertString(i, szValue), i);
		pComboBox6->SetItemData(pComboBox6->InsertString(i, szValue), i);
	}

	//分
	pComboBox1 = (CComboBox*)GetDlgItem(IDC_COMBO_MINUTE_1);
	pComboBox2 = (CComboBox*)GetDlgItem(IDC_COMBO_MINUTE_2);
	pComboBox3 = (CComboBox*)GetDlgItem(IDC_COMBO_MINUTE_3);
	pComboBox4 = (CComboBox*)GetDlgItem(IDC_COMBO_MINUTE_4);
	pComboBox5 = (CComboBox*)GetDlgItem(IDC_COMBO_MINUTE_5);
	pComboBox6 = (CComboBox*)GetDlgItem(IDC_COMBO_MINUTE_6);
	for (int i = 0; i < 60; i++)
	{
		_itot_s(i, szValue, 10);
		pComboBox1->SetItemData(pComboBox1->InsertString(i, szValue), i);
		pComboBox2->SetItemData(pComboBox2->InsertString(i, szValue), i);
		pComboBox3->SetItemData(pComboBox3->InsertString(i, szValue), i);
		pComboBox4->SetItemData(pComboBox4->InsertString(i, szValue), i);
		pComboBox5->SetItemData(pComboBox5->InsertString(i, szValue), i);
		pComboBox6->SetItemData(pComboBox6->InsertString(i, szValue), i);
	}

	//秒
	pComboBox1 = (CComboBox*)GetDlgItem(IDC_COMBO_SECOND_1);
	pComboBox2 = (CComboBox*)GetDlgItem(IDC_COMBO_SECOND_2);
	pComboBox3 = (CComboBox*)GetDlgItem(IDC_COMBO_SECOND_3);
	pComboBox4 = (CComboBox*)GetDlgItem(IDC_COMBO_SECOND_4);
	pComboBox5 = (CComboBox*)GetDlgItem(IDC_COMBO_SECOND_5);
	pComboBox6 = (CComboBox*)GetDlgItem(IDC_COMBO_SECOND_6);
	for (int i = 0; i < 60; i++)
	{
		_itot_s(i, szValue, 10);
		pComboBox1->SetItemData(pComboBox1->InsertString(i, szValue), i);
		pComboBox2->SetItemData(pComboBox2->InsertString(i, szValue), i);
		pComboBox3->SetItemData(pComboBox3->InsertString(i, szValue), i);
		pComboBox4->SetItemData(pComboBox4->InsertString(i, szValue), i);
		pComboBox5->SetItemData(pComboBox5->InsertString(i, szValue), i);
		pComboBox6->SetItemData(pComboBox6->InsertString(i, szValue), i);
	}

	//变量定义
	auto pTournamentMatch = (tagTournamentMatch*)m_pMatchOption->cbMatchRule;

	//循环开赛
	if (pTournamentMatch->cbMatchMode == MATCH_MODE_ROUND)
	{
		CheckRadioButton(IDC_RADIO_MATCH_LOCKTIME, IDC_RADIO_MATCH_ROUND, IDC_RADIO_MATCH_ROUND);
	}

	//定时开赛
	if (pTournamentMatch->cbMatchMode == MATCH_MODE_LOCKTIME)
	{
		CheckRadioButton(IDC_RADIO_MATCH_LOCKTIME, IDC_RADIO_MATCH_ROUND, IDC_RADIO_MATCH_LOCKTIME);
	}

	//游戏局数
	if (pTournamentMatch->wPlayCount!= 0)
	{
		SetDlgItemInt(IDC_EDIT_PLAY_COUNT, (UINT)pTournamentMatch->wPlayCount);
	}

	//游戏底分
	if (pTournamentMatch->lInitBaseScore != 0)
	{
		SetDlgItemInt(IDC_EDIT_BASE_SCORE, (UINT)pTournamentMatch->lInitBaseScore);
	}

	//初始积分
	SetDlgItemInt(IDC_EDIT_MATCH_SCORE, (UINT)pTournamentMatch->lInitMatchScore);

	//最大桌数
	if (pTournamentMatch->wMaxTableCount!= 0)
	{
		SetDlgItemInt(IDC_MAX_TABLE_COUNT, (UINT)pTournamentMatch->wMaxTableCount);
	}

	//游戏人数
	if (pTournamentMatch->wMinPlayerCount != 0)
	{
		SetDlgItemInt(IDC_MIN_PLAYER_COUNT, (UINT)pTournamentMatch->wMinPlayerCount);
	}

	//游戏人数
	if (pTournamentMatch->wMaxPlayerCount != 0)
	{
		SetDlgItemInt(IDC_MAX_PLAYER_COUNT, (UINT)pTournamentMatch->wMaxPlayerCount);
	}

	//时间定义	
	SYSTEMTIME MatchInterVal = pTournamentMatch->MatchInterval;
	SYSTEMTIME MatchLockTime = pTournamentMatch->MatchLockTime;
	SYSTEMTIME MatchStartTime = pTournamentMatch->MatchStartTime;
	SYSTEMTIME MatchCutOffTime = pTournamentMatch->MatchCutOffTime;
	SYSTEMTIME MatchSignupStart = pTournamentMatch->MatchSignupStart;
	SYSTEMTIME MatchSignupCutOff = pTournamentMatch->MatchSignupCutOff;	
	SYSTEMTIME MatchDurationTime = pTournamentMatch->MatchDurationTime;	
	SYSTEMTIME MatchSaleTicketTime = pTournamentMatch->MatchSaleTicketTime;

	//开始日期
	CDateTimeCtrl* pStartDate = (CDateTimeCtrl*)GetDlgItem(IDC_MATCH_START_DATE);
	pStartDate->SetTime(&MatchStartTime);

	//结束日期
	CDateTimeCtrl* pCutOffDate = (CDateTimeCtrl*)GetDlgItem(IDC_MATCH_CUTOFF_DATE);
	pCutOffDate->SetTime(&MatchCutOffTime);

	//开始时间
	CDateTimeCtrl* pStartTime = (CDateTimeCtrl*)GetDlgItem(IDC_MATCH_START_TIME);
	pStartTime->SetTime(&MatchStartTime);

	//结束时间
	CDateTimeCtrl* pCutOffTime = (CDateTimeCtrl*)GetDlgItem(IDC_MATCH_CUTOFF_TIME);
	pCutOffTime->SetTime(&MatchCutOffTime);

	//开赛间隔
	SetMatchTime(MatchInterVal, IDC_COMBO_HOUR_1, IDC_COMBO_MINUTE_1, IDC_COMBO_SECOND_1);

	//开赛时间
	SetMatchTime(MatchLockTime, IDC_COMBO_HOUR_2, IDC_COMBO_MINUTE_2, IDC_COMBO_SECOND_2);

	//报名时间
	SetMatchTime(MatchSignupStart, IDC_COMBO_HOUR_3, IDC_COMBO_MINUTE_3, IDC_COMBO_SECOND_3);
	SetMatchTime(MatchSignupCutOff,IDC_COMBO_HOUR_5, IDC_COMBO_MINUTE_5, IDC_COMBO_SECOND_5);

	//持续时间
	SetMatchTime(MatchDurationTime, IDC_COMBO_HOUR_4, IDC_COMBO_MINUTE_4, IDC_COMBO_SECOND_4);

	//售票时间
	SetMatchTime(MatchSaleTicketTime, IDC_COMBO_HOUR_6, IDC_COMBO_MINUTE_6, IDC_COMBO_SECOND_6);

	//每周开
	if (pTournamentMatch->cbMatchCycle == MATCH_CYCLE_PER_WEEK)
	{
		CheckRadioButton(IDC_RADIO_EVERY_DAY, IDC_RADIO_EVERY_MONTH, IDC_RADIO_EVERY_WEEK);
		SetComboDataList(&m_ComboWeekDay, pTournamentMatch->cbDayFlagList,CountArray(pTournamentMatch->cbDayFlagList));
	}
	//每月开
	else if (pTournamentMatch->cbMatchCycle == MATCH_CYCLE_PER_MONTH)
	{
		CheckRadioButton(IDC_RADIO_EVERY_DAY, IDC_RADIO_EVERY_MONTH, IDC_RADIO_EVERY_MONTH);
		SetComboDataList(&m_ComboMonthDay, pTournamentMatch->cbDayFlagList, CountArray(pTournamentMatch->cbDayFlagList));
		SetComboDataList(&m_ComboYearMonth, pTournamentMatch->cbMonthFlagList,CountArray(pTournamentMatch->cbMonthFlagList));
	}
	//每天开
	else
	{
		CheckRadioButton(IDC_RADIO_EVERY_DAY, IDC_RADIO_EVERY_MONTH, IDC_RADIO_EVERY_DAY);
	}

	return FALSE;
}

//交换数据
VOID CDlgMatchTournament::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_COMBO_WEEK_DAY, m_ComboWeekDay);
	DDX_Control(pDX, IDC_COMBO_MONTH_DAY, m_ComboMonthDay);
	DDX_Control(pDX, IDC_COMBO_YEAR_MONTH, m_ComboYearMonth);

	__super::DoDataExchange(pDX);
}

//保存输入
bool CDlgMatchTournament::SaveInputInfo()
{
	//变量定义
	tagTournamentMatch* pTournamentMatch = (tagTournamentMatch*)m_pMatchOption->cbMatchRule;
	ZeroMemory(pTournamentMatch, sizeof(tagTournamentMatch));

	//获取控件
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_MATCH_MODE);

	//基础配置
	pTournamentMatch->wPlayCount = GetDlgItemInt(IDC_EDIT_PLAY_COUNT);
	pTournamentMatch->lInitBaseScore = GetDlgItemInt(IDC_EDIT_BASE_SCORE);
	pTournamentMatch->lInitMatchScore = GetDlgItemInt(IDC_EDIT_MATCH_SCORE);
	pTournamentMatch->wMaxTableCount = GetDlgItemInt(IDC_MAX_TABLE_COUNT);
	pTournamentMatch->wMinPlayerCount = GetDlgItemInt(IDC_MIN_PLAYER_COUNT);
	pTournamentMatch->wMaxPlayerCount = GetDlgItemInt(IDC_MAX_PLAYER_COUNT);

	//开赛模式
	if (IsDlgButtonChecked(IDC_RADIO_MATCH_ROUND)) pTournamentMatch->cbMatchMode = MATCH_MODE_ROUND;
	if (IsDlgButtonChecked(IDC_RADIO_MATCH_LOCKTIME)) pTournamentMatch->cbMatchMode = MATCH_MODE_LOCKTIME;

	//游戏局数
	if (pTournamentMatch->wPlayCount == 0)
	{
		AfxMessageBox(TEXT("游戏局数设置不合法，请重新输入！"));

		GetDlgItem(IDC_MIN_PLAYER_COUNT)->SetFocus();

		return false;
	}

	//游戏底分
	if (pTournamentMatch->lInitBaseScore == 0)
	{
		AfxMessageBox(TEXT("游戏底分设置不合法，请重新输入！"));

		GetDlgItem(IDC_MIN_PLAYER_COUNT)->SetFocus();

		return false;
	}

	//最大桌数
	if (pTournamentMatch->wMaxTableCount == 0)
	{
		AfxMessageBox(TEXT("最大桌数设置不合法，请重新输入！"));

		GetDlgItem(IDC_MAX_TABLE_COUNT)->SetFocus();

		return false;
	}

	//游戏人数
	if (pTournamentMatch->wMinPlayerCount == 0)
	{
		AfxMessageBox(TEXT("最少游戏人数设置不合法，请重新输入！"));

		GetDlgItem(IDC_MIN_PLAYER_COUNT)->SetFocus();

		return false;
	}

	//游戏人数
	if (pTournamentMatch->wMaxPlayerCount != 0 && pTournamentMatch->wMaxPlayerCount < pTournamentMatch->wMinPlayerCount)
	{
		AfxMessageBox(TEXT("最大游戏人数设置不合法，请重新输入！"));

		GetDlgItem(IDC_MIN_PLAYER_COUNT)->SetFocus();

		return false;
	}

	//时间定义
	CTime MatchTime, MatchDate;
	CDateTimeCtrl* pDatTimeCtrl;

	//开始日期
	pDatTimeCtrl = (CDateTimeCtrl*)GetDlgItem(IDC_MATCH_START_DATE);
	pDatTimeCtrl->GetTime(MatchDate);

	//开始时间
	pDatTimeCtrl = (CDateTimeCtrl*)GetDlgItem(IDC_MATCH_START_TIME);
	pDatTimeCtrl->GetTime(MatchTime);

	//合并时间
	CTime MatchStartTime(MatchDate.GetYear(), MatchDate.GetMonth(), MatchDate.GetDay(), MatchTime.GetHour(), MatchTime.GetMinute(), MatchTime.GetSecond());

	//比赛时间	
	MatchStartTime.GetAsSystemTime(pTournamentMatch->MatchStartTime);

	//截止日期
	pDatTimeCtrl = (CDateTimeCtrl*)GetDlgItem(IDC_MATCH_CUTOFF_DATE);
	pDatTimeCtrl->GetTime(MatchDate);

	//截止时间
	pDatTimeCtrl = (CDateTimeCtrl*)GetDlgItem(IDC_MATCH_CUTOFF_TIME);
	pDatTimeCtrl->GetTime(MatchTime);

	//合并时间
	CTime MatchCutOffTime(MatchDate.GetYear(), MatchDate.GetMonth(), MatchDate.GetDay(), MatchTime.GetHour(), MatchTime.GetMinute(), MatchTime.GetSecond());

	//获取时间
	MatchCutOffTime.GetAsSystemTime(pTournamentMatch->MatchCutOffTime);

	//报名时间
	GetMatchTime(pTournamentMatch->MatchSignupStart, IDC_COMBO_HOUR_3, IDC_COMBO_MINUTE_3, IDC_COMBO_SECOND_3);
	GetMatchTime(pTournamentMatch->MatchSignupCutOff,IDC_COMBO_HOUR_5, IDC_COMBO_MINUTE_5, IDC_COMBO_SECOND_5);

	//持续时间
	GetMatchTime(pTournamentMatch->MatchDurationTime, IDC_COMBO_HOUR_4, IDC_COMBO_MINUTE_4, IDC_COMBO_SECOND_4);

	//售票时间
	GetMatchTime(pTournamentMatch->MatchSaleTicketTime, IDC_COMBO_HOUR_6, IDC_COMBO_MINUTE_6, IDC_COMBO_SECOND_6);

	//循环开赛
	if (pTournamentMatch->cbMatchMode == MATCH_MODE_ROUND)
	{
		//设置变量
		pTournamentMatch->MatchInterval.wDay = 0;
		pTournamentMatch->MatchInterval.wYear = 0;
		pTournamentMatch->MatchInterval.wMonth = 0;
		pTournamentMatch->MatchInterval.wDayOfWeek = 0;

		//获取时间
		GetMatchTime(pTournamentMatch->MatchInterval, IDC_COMBO_HOUR_1, IDC_COMBO_MINUTE_1, IDC_COMBO_SECOND_1);
	}

	//定时开赛
	if (pTournamentMatch->cbMatchMode == MATCH_MODE_LOCKTIME)
	{
		//设置变量
		pTournamentMatch->MatchLockTime.wDay = 0;
		pTournamentMatch->MatchLockTime.wYear = 0;
		pTournamentMatch->MatchLockTime.wMonth = 0;
		pTournamentMatch->MatchLockTime.wDayOfWeek = 0;

		//设置周期
		pTournamentMatch->cbMatchCycle = MATCH_CYCLE_PER_DAY;

		//开始时间
		GetMatchTime(pTournamentMatch->MatchLockTime, IDC_COMBO_HOUR_2, IDC_COMBO_MINUTE_2, IDC_COMBO_SECOND_2);

		//按周开
		if (IsDlgButtonChecked(IDC_RADIO_EVERY_WEEK))
		{
			//获取配置
			GetComboDataList(&m_ComboWeekDay, pTournamentMatch->cbDayFlagList, CountArray(pTournamentMatch->cbDayFlagList));

			//设置周期
			pTournamentMatch->cbMatchCycle = MATCH_CYCLE_PER_WEEK;
		}

		//按月开
		if (IsDlgButtonChecked(IDC_RADIO_EVERY_MONTH))
		{
			//获取配置
			GetComboDataList(&m_ComboMonthDay, pTournamentMatch->cbDayFlagList, CountArray(pTournamentMatch->cbDayFlagList));
			GetComboDataList(&m_ComboYearMonth, pTournamentMatch->cbMonthFlagList, CountArray(pTournamentMatch->cbMonthFlagList));

			//设置周期
			pTournamentMatch->cbMatchCycle = MATCH_CYCLE_PER_MONTH;
		}
	}

	return true;
}

//调整控件
VOID CDlgMatchTournament::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//报名时间
VOID CDlgMatchTournament::GetMatchTime(SYSTEMTIME& MatchTime, UINT uHourID, UINT uMinuteID, UINT uSecondID)
{
	//时
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(uHourID);
	if (pComboBox != NULL)
	{
		MatchTime.wHour = 0;
		if (pComboBox->GetCurSel() != -1) MatchTime.wHour = pComboBox->GetItemData(pComboBox->GetCurSel());
	}

	//分
	pComboBox = (CComboBox*)GetDlgItem(uMinuteID);
	if (pComboBox != NULL)
	{
		MatchTime.wMinute = 0;
		if (pComboBox->GetCurSel() != -1) MatchTime.wMinute = pComboBox->GetItemData(pComboBox->GetCurSel());
	}

	//秒
	pComboBox = (CComboBox*)GetDlgItem(uSecondID);
	if (pComboBox != NULL)
	{
		MatchTime.wSecond = 0;
		if (pComboBox->GetCurSel() != -1) MatchTime.wSecond = pComboBox->GetItemData(pComboBox->GetCurSel());
	}
}

//报名时间
VOID CDlgMatchTournament::SetMatchTime(const SYSTEMTIME& MatchTime, UINT uHourID, UINT uMinuteID, UINT uSecondID)
{
	//时
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(uHourID);
	if (pComboBox != NULL)
	{
		for (int i = 0; i < 1440; i++)
		{
			if (pComboBox->GetItemData(i) == MatchTime.wHour)
			{
				pComboBox->SetCurSel(i);
				break;
			}
		}
	}

	//分
	pComboBox = (CComboBox*)GetDlgItem(uMinuteID);
	if (pComboBox != NULL)
	{
		for (int i = 0; i < 60; i++)
		{
			if (pComboBox->GetItemData(i) == MatchTime.wMinute)
			{
				pComboBox->SetCurSel(i);
				break;
			}
		}
	}

	//秒
	pComboBox = (CComboBox*)GetDlgItem(uSecondID);
	if (pComboBox != NULL)
	{
		for (int i = 0; i < 60; i++)
		{
			if (pComboBox->GetItemData(i) == MatchTime.wSecond)
			{
				pComboBox->SetCurSel(i);
				break;
			}
		}
	}
}

//获取天列表
VOID CDlgMatchTournament::GetComboDataList(CCheckComboBox* pCheckComboBox, BYTE cboDataFlagList[], BYTE cbDataCount)
{
	//获取类型	
	for (INT i = 0; i < pCheckComboBox->GetCount(); i++)
	{
		auto pItemData = (tagItemData*)pCheckComboBox->GetItemDataPtr(i);
		cboDataFlagList[i] = pItemData->bChecked;
		if (i >= cbDataCount) break;
	}
}

//设置天列表
VOID CDlgMatchTournament::SetComboDataList(CCheckComboBox* pCheckComboBox, BYTE cboDataFlagList[], BYTE cbDataCount)
{
	//获取类型	
	for (INT j = 0; j < pCheckComboBox->GetCount(); j++)
	{
		auto pItemData = (tagItemData*)pCheckComboBox->GetItemDataPtr(j);
		if (pItemData->dwDataPtr > cbDataCount) continue;		
		if (cboDataFlagList[pItemData->dwDataPtr-1]==TRUE)
		{
			pItemData->bChecked = TRUE;
		}			
	}	

	//更新数据
	pCheckComboBox->UpdateData();
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgMatchWizardItem::CDlgMatchWizardItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
	//设置变量
	m_bSaveData=true;

	//设置变量	
	m_pMatchOption=NULL;
	m_pDlgMatchWizard=NULL;
	m_pMatchInfoBuffer=NULL;
	m_pModuleInfoBuffer=NULL;

	return;
}

//析构函数
CDlgMatchWizardItem::~CDlgMatchWizardItem()
{
}

//保存数据
bool CDlgMatchWizardItem::SaveItemData()
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
bool CDlgMatchWizardItem::ShowWizardItem(const CRect & rcRect, CWnd * pParentWnd)
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

	//激活子项
	OnMatchItemActive();

	//显示窗口
	ShowWindow(SW_SHOW);

	return true;
}

//确定函数
VOID CDlgMatchWizardItem::OnOK() 
{ 
	//投递消息
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//取消消息
VOID CDlgMatchWizardItem::OnCancel() 
{ 
	//投递消息
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//位置消息
VOID CDlgMatchWizardItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//调整控件
	RectifyControl(cx,cy);

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgMatchWizardStep1::CDlgMatchWizardStep1() : CDlgMatchWizardItem(IDD_MATCH_WIZARD_1)
{
	//变量定义
	m_cbItemIndex=0xff;
	ZeroMemory(m_pDlgMatchItem,sizeof(m_pDlgMatchItem));

	return;
}

//析构函数
CDlgMatchWizardStep1::~CDlgMatchWizardStep1()
{
}

//控件绑定
VOID CDlgMatchWizardStep1::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Control(pDX,IDC_COMBO_MODULE_NAME,m_ModuleComboBox);
}

//初始化函数
BOOL CDlgMatchWizardStep1::OnInitDialog()
{
	__super::OnInitDialog();

	//初始化配置
	InitBaseOption();

	//获取控件
	CTabCtrl * pTabCtrl=(CTabCtrl *)GetDlgItem(IDC_MATCH_OPTION);

	//插入子项
	pTabCtrl->InsertItem(TAB_OPTION_SIGNUP1,TEXT("报名方式"));
	pTabCtrl->InsertItem(TAB_OPTION_SIGNUP2,TEXT("报名条件"));
	//pTabCtrl->InsertItem(TAB_OPTION_RANKINGMODE,TEXT("排名方式"));
	pTabCtrl->InsertItem(TAB_OPTION_DISTRIBUTE,TEXT("撮桌配置"));	
	pTabCtrl->InsertItem(TAB_OPTION_REWARD,TEXT("奖励配置"));	
	//pTabCtrl->InsertItem(TAB_OPTION_ROUND,TEXT("轮次配置"));	

	//设置大小
	pTabCtrl->SetItemSize(CSize(120,25));

	//添加子项
	m_pDlgMatchItem[0]=&m_DlgMatchSignup1;
	m_pDlgMatchItem[1]=&m_DlgMatchSignup2;
	//m_pDlgMatchItem[2]=&m_DlgMatchRankingMode;
	m_pDlgMatchItem[2]=&m_DlgMatchDistribute;	
	m_pDlgMatchItem[3]=&m_DlgMatchReward;	
	//m_pDlgMatchItem[5]=&m_DlgMatchRound;	

	//变量定义
	CRect rcCreate(0,0,0,0);

	//设置子项
	for (int i=0; i<CountArray(m_pDlgMatchItem); i++)
	{
		//指针判断
		if (m_pDlgMatchItem[i]==NULL) continue;

		//设置控件		
		m_pDlgMatchItem[i]->SetMatchOption(m_pMatchOption);
		m_pDlgMatchItem[i]->SetMatchInfoBuffer(m_pMatchInfoBuffer);		

		//创建子项		
		m_pDlgMatchItem[i]->CreateMatchItem(rcCreate,pTabCtrl);
	}

	//激活子项
	ActiveOptionItem(TAB_OPTION_SIGNUP1);

	return TRUE;
}

//保存输入
bool CDlgMatchWizardStep1::SaveInputInfo()
{
	//保存输入
	if (SaveBaseOption()==false) return false;

	//遍历子项
	for (int i=0; i<CountArray(m_pDlgMatchItem); i++)
	{
		if (m_pDlgMatchItem[i]->SaveInputInfo() == false) return false;
	}

	return true;
}

//激活子项
VOID CDlgMatchWizardStep1::OnMatchItemActive()
{
	return;
}

//调整控件
VOID CDlgMatchWizardStep1::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//基础配置
VOID CDlgMatchWizardStep1::InitBaseOption()
{
	//输入限制
	((CEdit *)GetDlgItem(IDC_EDIT_MATCH_NAME))->LimitText(32);		

	//比赛名称
	SetDlgItemText(IDC_EDIT_MATCH_NAME,CW2CT(m_pMatchOption->szMatchName));

	//比赛类型
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_MATCH_TYPE);
	if (pComboBox!=NULL)
	{
		//变量定义
		BYTE cbMatchType[]={MATCH_TYPE_LOCKTIME,MATCH_TYPE_IMMEDIATE,MATCH_TYPE_TOURNAMENT };
		LPCTSTR pszMatchType[]={TEXT("定时赛类型"),TEXT("即时赛类型"), TEXT("锦标赛类型")};

		//构造类型
		for (INT i=0;i<CountArray(cbMatchType);i++)
		{
			int nIndex=pComboBox->AddString(pszMatchType[i]);
			pComboBox->SetItemData(nIndex,cbMatchType[i]);
			if (m_pMatchOption->cbMatchType==cbMatchType[i])
			{
				pComboBox->SetCurSel(nIndex);
			}
		}

		//默认选择
		if (pComboBox->GetCurSel()==CB_ERR) pComboBox->SetCurSel(0);
		if (m_pMatchOption->dwMatchID!=0) pComboBox->EnableWindow(FALSE);
	}

	//展示类型
	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_SHOW_TYPE);
	if (pComboBox != NULL)
	{
		//变量定义
		BYTE cbShowType[] = { 0, MATCH_SHOW_TTYPE_DAYLY,MATCH_SHOW_TTYPE_WEEKLY,MATCH_SHOW_TTYPE_PLAYOFF };
		LPCTSTR pszShowType[] = { TEXT("普通类型") ,TEXT("日赛类型"),TEXT("周赛类型"),TEXT("线下赛类型") };

		//构造类型
		for (INT i = 0; i < CountArray(cbShowType); i++)
		{
			int nIndex = pComboBox->AddString(pszShowType[i]);
			pComboBox->SetItemData(nIndex, cbShowType[i]);
			if (m_pMatchOption->cbShowType == cbShowType[i])
			{
				pComboBox->SetCurSel(nIndex);
			}
		}

		//默认选择
		if (pComboBox->GetCurSel() == CB_ERR) pComboBox->SetCurSel(0);		
	}

	//参赛条件
	pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_JOIN_CONDITION);
	if (pComboBox!=NULL)
	{
		//准备数据
		OnCbnSelchangeComboMatchType();	

		//设置选择
		for (INT i=0;i<pComboBox->GetCount();i++)
		{
			if (pComboBox->GetItemData(i)==m_pMatchOption->cbJoinCondition)
			{
				pComboBox->SetCurSel(i);
				break;
			}
		}
	}

	//模块列表
	if (m_pModuleInfoBuffer!=NULL)
	{
		//变量定义
		tagGameModuleInfo * pGameModuleInfo=NULL;

		//变量定义
		POSITION Position=m_pModuleInfoBuffer->m_GameModuleInfoMap.GetStartPosition();

		//枚举模块
		while (Position!=NULL)
		{
			//获取对象
			WORD wModuleID=0L;
			tagGameModuleInfo * pGameModuleInfo=NULL;
			m_pModuleInfoBuffer->m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameModuleInfo);

			//插入列表
			ASSERT(pGameModuleInfo!=NULL);
			if (pGameModuleInfo!=NULL)
			{
				int nItemIndex=m_ModuleComboBox.AddString(pGameModuleInfo->szModuleName);
				m_ModuleComboBox.SetItemData(nItemIndex,pGameModuleInfo->wModuleID);
				if (pGameModuleInfo->wModuleID==m_pMatchOption->wKindID)
				{
					m_ModuleComboBox.SetCurSel(nItemIndex);
				}
			}
		}

		//默认选择
		if (m_ModuleComboBox.GetCurSel()==-1) m_ModuleComboBox.SetCurSel(0);
	}	
}

//保存配置
bool CDlgMatchWizardStep1::SaveBaseOption()
{
	//比赛信息
	//m_pMatchOption->lSignupFee=GetDlgItemInt(IDC_EDIT_MATCH_FEE);

	//比赛名称
	TCHAR szMatchName[32];
	GetDlgItemText(IDC_EDIT_MATCH_NAME,szMatchName,CountArray(szMatchName));

	//拷贝数据
	StringCchCopy(m_pMatchOption->szMatchName, CountArray(m_pMatchOption->szMatchName),szMatchName);

	//验证名称
	if (m_pMatchOption->szMatchName[0]==0)
	{
		AfxMessageBox(TEXT("比赛名称不能为空，请重新输入！"));

		//设置焦点
		GetDlgItem(IDC_EDIT_MATCH_NAME)->SetFocus();

		return false;
	}

	//比赛类型
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_MATCH_TYPE);
	m_pMatchOption->cbMatchType=(BYTE)pComboBox->GetItemData(pComboBox->GetCurSel());	

	//展示类型
	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_SHOW_TYPE);
	m_pMatchOption->cbShowType = (BYTE)pComboBox->GetItemData(pComboBox->GetCurSel());

	//模块名称
	pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_MODULE_NAME);	
	m_pMatchOption->wKindID=pComboBox->GetItemData(pComboBox->GetCurSel());

	//模块名称
	TCHAR szModuleName[LEN_KIND];
	pComboBox->GetLBText(pComboBox->GetCurSel(),szModuleName);

	//拷贝数据
	StringCchCopy(m_pMatchOption->szModuleName, CountArray(m_pMatchOption->szModuleName),szModuleName);	

	return true;
}

//激活子项
VOID CDlgMatchWizardStep1::ActiveOptionItem(BYTE cbItemIndex)
{
	//判断索引
	if (m_cbItemIndex==cbItemIndex) return;

	//设置索引
	m_cbItemIndex=cbItemIndex;

	//控件区域
	CRect rcTabCtrl;
	GetDlgItem(IDC_MATCH_OPTION)->GetClientRect(&rcTabCtrl);

	//设置控件
	if (m_pDlgMatchItem[m_cbItemIndex]->m_hWnd!=NULL)
	{
		m_pDlgMatchItem[m_cbItemIndex]->SetWindowPos(NULL,5,30,rcTabCtrl.Width()-10,rcTabCtrl.Height()-30-5,SWP_NOZORDER|SWP_SHOWWINDOW);
	}

	//设置控件
	for (int i=0; i<CountArray(m_pDlgMatchItem); i++)
	{
		if (i==m_cbItemIndex) continue;
		if (m_pDlgMatchItem[i]->m_hWnd!=NULL) m_pDlgMatchItem[i]->ShowWindow(SW_HIDE);
	}
}

//选择变更
void CDlgMatchWizardStep1::OnCbnSelchangeComboMatchType()
{
	//获取对象
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_MATCH_TYPE);
	if (pComboBox!=NULL && pComboBox->GetCurSel()!=LB_ERR)
	{
		//比赛类型
		BYTE cbMatchType=(BYTE)pComboBox->GetItemData(pComboBox->GetCurSel());

		//事件通知
		for (int i=0; i<CountArray(m_pDlgMatchItem); i++)
		{
			m_pDlgMatchItem[i]->OnEventMatchTypeChanged(cbMatchType);
		}
	}
}

void CDlgMatchWizardStep1::OnTcnSelchangeMatchOption(NMHDR *pNMHDR, LRESULT *pResult)
{
	//获取控件
	CTabCtrl * pTabCtrl=(CTabCtrl *)GetDlgItem(IDC_MATCH_OPTION);
	ActiveOptionItem(pTabCtrl->GetCurSel());	
}

//////////////////////////////////////////////////////////////////////////
//构造函数
CDlgMatchWizardStep2::CDlgMatchWizardStep2() : CDlgMatchWizardItem(IDD_MATCH_WIZARD_2)
{
	//设置变量
	m_cbItemIndex=0;
	ZeroMemory(&m_MatchRoundItem,sizeof(m_MatchRoundItem));
	ZeroMemory(m_pPromoteModeItem,sizeof(m_pPromoteModeItem));
}

//析构函数
CDlgMatchWizardStep2::~CDlgMatchWizardStep2()
{
}

//控件绑定
VOID CDlgMatchWizardStep2::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//奖励列表
	DDX_Control(pDX,IDC_LIST_ROUND,m_RoundListControl);
}

//初始化函数
BOOL CDlgMatchWizardStep2::OnInitDialog()
{
	__super::OnInitDialog();

	for (WORD i=0; i<m_pMatchOption->wRoundCount; i++)
	{
		m_RoundInfoBuffer.InsertMatchRoundItem(m_pMatchOption->MatchRoundItem[i]);
	}
	
	for(INT_PTR i=0;i<m_RoundInfoBuffer.m_MatchRoundItemArray.GetCount();i++) 
	{
		m_RoundListControl.InsertMatchRound(m_RoundInfoBuffer.m_MatchRoundItemArray[i]);
	}

	//变量定义	
	LPCTSTR pszPromoteMode[]={TEXT("打立出局"),TEXT("定局积分"),TEXT("按桌晋级")};
	BYTE cbPromoteMode[]={PROMOTE_MODE_OUT_GAME,PROMOTE_MODE_FIXED_GAME,PROMOTE_MODE_FIXED_TABLE};

	//获取控件
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_PROMOTE_MODE);
	for (INT i=0; i<CountArray(pszPromoteMode); i++)
	{
		pComboBox->SetItemData(pComboBox->AddString(pszPromoteMode[i]),cbPromoteMode[i]);
	}

	//获取控件
	CWnd * pPromoteCtrl=GetDlgItem(IDC_PROMOTE_RULE);

	//添加子项
	m_pPromoteModeItem[PROMOTE_MODE_OUT_GAME]=&m_PromoteOutGame;
	m_pPromoteModeItem[PROMOTE_MODE_FIXED_GAME]=&m_PromoteFixedGame;
	m_pPromoteModeItem[PROMOTE_MODE_FIXED_TABLE]=&m_PromoteFixedTable;	

	//变量定义
	CRect rcCreate(0,0,0,0);

	//设置子项
	for (int i=0; i<CountArray(m_pPromoteModeItem); i++)
	{
		//指针判断
		if (m_pPromoteModeItem[i]==NULL) continue;

		//设置控件		
		m_pPromoteModeItem[i]->SetMatchRoundItem(&m_MatchRoundItem);

		//创建子项		
		m_pPromoteModeItem[i]->CreatePromoteItem(rcCreate,pPromoteCtrl);
	}

	return TRUE;
}

//清理控件
VOID CDlgMatchWizardStep2::CleanControl()
{
	//设置控件	
	SetDlgItemText(IDC_ROUNDID,TEXT(""));
	SetDlgItemText(IDC_START_MIN_COUNT,TEXT(""));
	SetDlgItemText(IDC_START_MAX_COUNT,TEXT(""));
	SetDlgItemText(IDC_PROMOTE_COUNT,TEXT(""));

	//设置控件	
	SetDlgItemText(IDC_PLAY_COUNT,TEXT(""));
	SetDlgItemText(IDC_EDIT_MAX_BASE_SCORE,TEXT(""));
	SetDlgItemText(IDC_EDIT_INIT_BASE_SCORE,TEXT(""));

	//增长模式
	SetDlgItemText(IDC_EDIT_START_INNING,TEXT(""));
	SetDlgItemText(IDC_EDIT_INCREASE_RATIO1,TEXT(""));
	SetDlgItemText(IDC_EDIT_INTERVAL_INNING,TEXT(""));
	
	//增长模式
	SetDlgItemText(IDC_EDIT_INCREASE_RATIO2,TEXT(""));
	SetDlgItemText(IDC_EDIT_INTERVAL_SECOND,TEXT(""));
	
	//比赛分数
	SetDlgItemText(IDC_EDIT_INIT_SCORE,TEXT(""));
	SetDlgItemText(IDC_EDIT_RESERVE_RATIO,TEXT(""));

	//设置控件
	((CButton *)GetDlgItem(IDC_RADIO_INCREASE_MODE1))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO_INCREASE_MODE2))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO_RESET_SCORE))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO_RESERVE_SCORE))->SetCheck(FALSE);
}

//显示子项
VOID CDlgMatchWizardStep2::ShowPromoteItem(BYTE cbItemIndex)
{
	//判断索引
	if (cbItemIndex>=CountArray(m_pPromoteModeItem)) return;

	//设置索引
	m_cbItemIndex=cbItemIndex;

	//控件区域
	CRect rcTabCtrl;
	GetDlgItem(IDC_PROMOTE_RULE)->GetClientRect(&rcTabCtrl);

	//设置控件
	if (m_pPromoteModeItem[m_cbItemIndex]->m_hWnd!=NULL)
	{
		m_pPromoteModeItem[m_cbItemIndex]->RefreshControl();
		m_pPromoteModeItem[m_cbItemIndex]->SetWindowPos(NULL,6,16,rcTabCtrl.Width()-12,rcTabCtrl.Height()-16-4,SWP_NOZORDER|SWP_SHOWWINDOW);
	}

	//设置控件
	for (int i=0; i<CountArray(m_pPromoteModeItem); i++)
	{
		if (i==m_cbItemIndex) continue;
		if (m_pPromoteModeItem[i]->m_hWnd!=NULL) m_pPromoteModeItem[i]->ShowWindow(SW_HIDE);
	}
}

//设置配置
VOID CDlgMatchWizardStep2::SetMatchRoundItem(tagMatchRoundItem * pMatchRoundItem)
{
	//参数校验
	ASSERT(pMatchRoundItem!=NULL);
	if (pMatchRoundItem==NULL) return;	

	//设置标识
	SetDlgItemInt(IDC_ROUNDID,(UINT)pMatchRoundItem->wRoundID);	

	//获取控件
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_PROMOTE_MODE);
	for (INT_PTR i=0; i<pComboBox->GetCount(); i++)
	{
		if (pComboBox->GetItemData(i)==pMatchRoundItem->cbPromoteMode)
		{
			pComboBox->SetCurSel(i);
			break;
		}
	}

	//开始人数
	SetDlgItemInt(IDC_START_MIN_COUNT,(UINT)pMatchRoundItem->wStartMinCount);	
	SetDlgItemInt(IDC_START_MAX_COUNT,(UINT)pMatchRoundItem->wStartMaxCount);

	//晋级人数
	SetDlgItemInt(IDC_PROMOTE_COUNT,(UINT)pMatchRoundItem->wPromoteCount);

	//游戏局数
	SetDlgItemInt(IDC_PLAY_COUNT,(UINT)pMatchRoundItem->wPlayCount);

	//游戏底分		
	SetDlgItemInt(IDC_EDIT_MAX_BASE_SCORE,(UINT)pMatchRoundItem->lMaxBaseScore);
	SetDlgItemInt(IDC_EDIT_INIT_BASE_SCORE,(UINT)pMatchRoundItem->lInitBaseScore);

	//增长模式1
	if (pMatchRoundItem->cbIncreaseMode==INCREASE_MODE_INNING)
	{
		SetDlgItemInt(IDC_EDIT_START_INNING,(UINT)pMatchRoundItem->wStartInning);
		SetDlgItemInt(IDC_EDIT_INCREASE_RATIO1,(UINT)pMatchRoundItem->wIncreaseRatio);	
		SetDlgItemInt(IDC_EDIT_INTERVAL_INNING,(UINT)pMatchRoundItem->wIntervalInning);	
		CheckRadioButton(IDC_RADIO_INCREASE_MODE1,IDC_RADIO_INCREASE_MODE2,IDC_RADIO_INCREASE_MODE1);
	}

	//增长模式1
	if (pMatchRoundItem->cbIncreaseMode==INCREASE_MODE_TIME)
	{
		SetDlgItemInt(IDC_EDIT_INCREASE_RATIO2,(UINT)pMatchRoundItem->wIncreaseRatio);	
		SetDlgItemInt(IDC_EDIT_INTERVAL_SECOND,(UINT)pMatchRoundItem->wIntervalSecond);
		CheckRadioButton(IDC_RADIO_INCREASE_MODE1,IDC_RADIO_INCREASE_MODE2,IDC_RADIO_INCREASE_MODE2);
	}

	//比赛分数
	if (pMatchRoundItem->cbMScoreSetType==MSCORE_SET_TYPE_RESET)
	{
		SetDlgItemInt(IDC_EDIT_INIT_SCORE,(UINT)pMatchRoundItem->lInitMatchScore);
		CheckRadioButton(IDC_RADIO_RESET_SCORE,IDC_RADIO_RESERVE_SCORE,IDC_RADIO_RESET_SCORE);
	}

	if (pMatchRoundItem->cbMScoreSetType==MSCORE_SET_TYPE_RESERVE)
	{
		SetDlgItemInt(IDC_EDIT_RESERVE_RATIO,(UINT)pMatchRoundItem->wReserveRatio);
		CheckRadioButton(IDC_RADIO_RESET_SCORE,IDC_RADIO_RESERVE_SCORE,IDC_RADIO_RESERVE_SCORE);
	}
}

//获取配置
bool CDlgMatchWizardStep2::GetMatchRoundItem(tagMatchRoundItem & MatchRoundItem)
{
	//设置变量
	ZeroMemory(&MatchRoundItem,sizeof(MatchRoundItem));

	//获取标识
	MatchRoundItem.wRoundID=GetDlgItemInt(IDC_ROUNDID);

	//获取控件
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_PROMOTE_MODE);
	if (pComboBox->GetCurSel()==-1)
	{
		AfxMessageBox(TEXT("请选择晋级方式！"));
		return false;
	}

	//晋级方式
	MatchRoundItem.cbPromoteMode=pComboBox->GetItemData(pComboBox->GetCurSel());

	//开赛人数
	MatchRoundItem.wStartMinCount=GetDlgItemInt(IDC_START_MIN_COUNT);
	MatchRoundItem.wStartMaxCount=GetDlgItemInt(IDC_START_MAX_COUNT);

	//校验人数
	if (MatchRoundItem.wStartMinCount==0)
	{
		AfxMessageBox(TEXT("最小开始人数设置有误，请重新设置！"));
		return false;
	}

	//校验人数
	if (MatchRoundItem.wStartMaxCount==0)
	{
		AfxMessageBox(TEXT("最大开始人数设置有误，请重新设置！"));
		return false;
	}

	//校验人数
	if (MatchRoundItem.wStartMaxCount>MAX_MATCH_USER)
	{
		AfxMessageBox(TEXT("开始人数超出系统最大支持人数，请重新设置！"));
		return false;
	}

	//校验人数
	if (MatchRoundItem.wStartMinCount>MatchRoundItem.wStartMaxCount)
	{
		AfxMessageBox(TEXT("最小人数不能超过最大人数，请重新设置！"));
		return false;
	}	

	//开始人数
	MatchRoundItem.wStartCount=MatchRoundItem.wStartMaxCount;

	//晋级人数
	MatchRoundItem.wPromoteCount=GetDlgItemInt(IDC_PROMOTE_COUNT);
	if (MatchRoundItem.wPromoteCount==0)
	{
		AfxMessageBox(TEXT("晋级人数设置有误，请重新设置！"));
		return false;
	}

	//游戏局数
	MatchRoundItem.wPlayCount=GetDlgItemInt(IDC_PLAY_COUNT);
	if (MatchRoundItem.wPlayCount==0)
	{
		AfxMessageBox(TEXT("游戏局数设置有误，请重新设置！"));
		return false;
	}

	//保存配置
	if (m_pPromoteModeItem[m_cbItemIndex]->SaveInputInfo()==false)
	{
		return false;
	}

	//游戏底分		
	MatchRoundItem.lMaxBaseScore=GetDlgItemInt(IDC_EDIT_MAX_BASE_SCORE);
	MatchRoundItem.lInitBaseScore=GetDlgItemInt(IDC_EDIT_INIT_BASE_SCORE);

	//增长模式1
	if (IsDlgButtonChecked(IDC_RADIO_INCREASE_MODE1))
	{
		MatchRoundItem.cbIncreaseMode=INCREASE_MODE_INNING;
		MatchRoundItem.wStartInning=GetDlgItemInt(IDC_EDIT_START_INNING);
		MatchRoundItem.wIncreaseRatio=GetDlgItemInt(IDC_EDIT_INCREASE_RATIO1);
		MatchRoundItem.wIntervalInning=GetDlgItemInt(IDC_EDIT_INTERVAL_INNING);	

		//间隔局数
		if (MatchRoundItem.wIntervalInning==0)
		{
			AfxMessageBox(TEXT("间隔局数不能为0，请重新设置！"));
			return false;
		}
	}

	//增长模式2
	if (IsDlgButtonChecked(IDC_RADIO_INCREASE_MODE2))
	{
		MatchRoundItem.cbIncreaseMode=INCREASE_MODE_TIME;
		MatchRoundItem.wIncreaseRatio=GetDlgItemInt(IDC_EDIT_INCREASE_RATIO2);
		MatchRoundItem.wIntervalSecond=GetDlgItemInt(IDC_EDIT_INTERVAL_SECOND);	

		//间隔时间
		if (MatchRoundItem.wIntervalSecond==0)
		{
			AfxMessageBox(TEXT("间隔时间不能为0，请重新设置！"));
			return false;
		}
	}

	//校验分数
	if (MatchRoundItem.lInitBaseScore==0)
	{
		AfxMessageBox(TEXT("游戏底分不能为0，请重新设置！"));
		return false;
	}

	//校验分数
	if (MatchRoundItem.lMaxBaseScore<MatchRoundItem.lInitBaseScore)
	{
		AfxMessageBox(TEXT("游戏封顶底分不能小于初始底分，请重新设置！"));
		return false;
	}

	//比赛分数
	if (IsDlgButtonChecked(IDC_RADIO_RESET_SCORE))
	{
		MatchRoundItem.cbMScoreSetType=MSCORE_SET_TYPE_RESET;
		MatchRoundItem.lInitMatchScore=GetDlgItemInt(IDC_EDIT_INIT_SCORE);
	}
	if (IsDlgButtonChecked(IDC_RADIO_RESERVE_SCORE))
	{
		MatchRoundItem.cbMScoreSetType=MSCORE_SET_TYPE_RESERVE;
		MatchRoundItem.wReserveRatio=GetDlgItemInt(IDC_EDIT_RESERVE_RATIO);		
	}

	//拷贝规则
	CopyMemory(MatchRoundItem.cbPromoteRule,m_MatchRoundItem.cbPromoteRule,sizeof(MatchRoundItem.cbPromoteRule));

	return true;
}

//保存输入
bool CDlgMatchWizardStep2::SaveInputInfo()
{
	//变量定义
	m_pMatchOption->wRoundCount=__min(CountArray(m_pMatchOption->MatchRoundItem),m_RoundInfoBuffer.GetMatchRoundCount());

	for (WORD i=0; i<m_pMatchOption->wRoundCount; i++)
	{
		CopyMemory(&m_pMatchOption->MatchRoundItem[i],m_RoundInfoBuffer.m_MatchRoundItemArray[i],sizeof(m_pMatchOption->MatchRoundItem[i]));
		if (m_pMatchOption->MatchRoundItem[i].wRoundID!=i+1)
		{
			AfxMessageBox(TEXT("请按轮次顺序配置轮次标识！"));

			return false;
		}
	}	

	return true;
}

//激活事件
VOID CDlgMatchWizardStep2::OnMatchItemActive()
{
	//校验类型
	if (m_pMatchOption->cbMatchType == MATCH_TYPE_TOURNAMENT)
	{
		EnableWindow(FALSE);		
	}
	else
	{
		EnableWindow(TRUE);		
	}
}

//调整控件
VOID CDlgMatchWizardStep2::RectifyControl(INT nWidth, INT nHeight)
{
}

//添加奖励
VOID CDlgMatchWizardStep2::OnBnClickedAddRound()
{
	//变量定义	
	tagMatchRoundItem MatchRoundItem;

	//获取配置
	if (GetMatchRoundItem(MatchRoundItem)==false) return;

	//存在判断
	if (m_RoundInfoBuffer.SearchMatchRound(MatchRoundItem.wRoundID)!=NULL)
	{
		AfxMessageBox(TEXT("抱歉，该轮次标识已存在！"));

		return;
	}		
	
	//插入轮次
	tagMatchRoundItem * pMatchRoundItem=m_RoundInfoBuffer.InsertMatchRoundItem(MatchRoundItem);
	if (pMatchRoundItem!=NULL)
	{
		m_RoundListControl.InsertMatchRound(pMatchRoundItem);
	}

	//清理控件
	CleanControl();
}

//修改轮次
VOID CDlgMatchWizardStep2::OnBnClickedModifyRound()
{
	//变量定义	
	tagMatchRoundItem MatchRoundItem;

	//获取配置
	if (GetMatchRoundItem(MatchRoundItem)==false) return;

	//存在判断
	if (m_RoundInfoBuffer.SearchMatchRound(MatchRoundItem.wRoundID)==NULL)
	{
		AfxMessageBox(TEXT("抱歉，该轮次标识不存在！"));

		return;
	}		
	
	//修改轮次
	tagMatchRoundItem * pMatchRoundItem=m_RoundInfoBuffer.SearchMatchRound(MatchRoundItem.wRoundID);
	if (pMatchRoundItem!=NULL)
	{
		CopyMemory(pMatchRoundItem,&MatchRoundItem,sizeof(MatchRoundItem));
		m_RoundListControl.UpdateMatchRound(pMatchRoundItem);
	}
}

//修改奖励
VOID CDlgMatchWizardStep2::OnBnClickedDeleteRound()
{
	//变量定义	
	tagMatchRoundItem MatchRoundItem;

	//获取配置
	if (GetMatchRoundItem(MatchRoundItem)==false) return;

	//查找轮次
	tagMatchRoundItem * pMatchRoundItem=m_RoundInfoBuffer.SearchMatchRound(MatchRoundItem.wRoundID);
	if (pMatchRoundItem==NULL)
	{
		AfxMessageBox(TEXT("抱歉，该轮次标识不存在！"));
		return;
	}

	//更新控件
	m_RoundListControl.DeleteMatchRound(pMatchRoundItem);
	m_RoundInfoBuffer.DeleteMatchRoundItem(pMatchRoundItem->wRoundID);

	//清理控件
	CleanControl();
}

//选择变更
void CDlgMatchWizardStep2::OnCbnSelchangeComboPromoteMode()
{
	//获取对象
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_PROMOTE_MODE);
	if (pComboBox!=NULL && pComboBox->GetCurSel()!=LB_ERR)
	{
		//晋级方式
		BYTE cbPromoteMode=(BYTE)pComboBox->GetItemData(pComboBox->GetCurSel());

		//显示子项
		ShowPromoteItem(cbPromoteMode);
	}
}

//子项变更
VOID CDlgMatchWizardStep2::OnLvnItemchangedListRound(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	//变量定义
	if (pNMLV->iItem!=LB_ERR)
	{
		//设置配置
		tagMatchRoundItem * pMatchRoundItem=(tagMatchRoundItem *)m_RoundListControl.GetItemData(pNMLV->iItem);
		if (pMatchRoundItem!=NULL) 
		{
			//设置子项
			SetMatchRoundItem(pMatchRoundItem);

			//拷贝数据
			CopyMemory(&m_MatchRoundItem,pMatchRoundItem,sizeof(m_MatchRoundItem));
			ShowPromoteItem(pMatchRoundItem->cbPromoteMode);
		}
	}

	*pResult = 0;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgMatchWizardStep3::CDlgMatchWizardStep3() : CDlgMatchWizardItem(IDD_MATCH_WIZARD_3)
{
	//设置变量
	m_pDlgMatchItem=NULL;
}

//析构函数
CDlgMatchWizardStep3::~CDlgMatchWizardStep3()
{
}

//控件绑定
VOID CDlgMatchWizardStep3::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

//初始化函数
BOOL CDlgMatchWizardStep3::OnInitDialog()
{
	__super::OnInitDialog();	
	
	return FALSE;
}

//保存输入
bool CDlgMatchWizardStep3::SaveInputInfo()
{
	//保存输入
	if (m_pDlgMatchItem->SaveInputInfo() == false) return false;

	return true;
}

//激活子项
VOID CDlgMatchWizardStep3::OnMatchItemActive()
{
	//隐藏窗口
	if (m_pDlgMatchItem!=NULL) m_pDlgMatchItem->ShowWindow(SW_HIDE);

	//获取位置
	CRect rcItemRect;
	GetWindowRect(rcItemRect);
	ScreenToClient(&rcItemRect);

	//选择子项
	switch (m_pMatchOption->cbMatchType)
	{
	case MATCH_TYPE_LOCKTIME:	//定时赛
	{
		m_pDlgMatchItem = &m_DlgMatchLockTime;
		break;
	}
	case MATCH_TYPE_IMMEDIATE:	//即时赛
	{
		m_pDlgMatchItem = &m_DlgMatchImmediate;
		break;
	}
	case MATCH_TYPE_TOURNAMENT:	//锦标赛
	{
		m_pDlgMatchItem = &m_DlgMatchTournament;
		break;
	}
	}

	//设置变量
	m_pDlgMatchItem->SetMatchOption(m_pMatchOption);

	//显示子项
	rcItemRect.DeflateRect(5,5,5,10);
	m_pDlgMatchItem->ShowMatchItem(rcItemRect,this);
}

//调整控件
VOID CDlgMatchWizardStep3::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgMatchWizard::CDlgMatchWizard() : CDialog(IDD_MATCH_WIZARD)
{
	//设置变量
	m_wActiveIndex=INVALID_WORD;
	m_pWizardItem[0]=&m_MatchWizardStep1;
	m_pWizardItem[1]=&m_MatchWizardStep2;
	m_pWizardItem[2]=&m_MatchWizardStep3;

	//配置信息	
	ZeroMemory(&m_MatchOption,sizeof(m_MatchOption));

	return;
}

//析构函数
CDlgMatchWizard::~CDlgMatchWizard()
{
}

//初始化函数
BOOL CDlgMatchWizard::OnInitDialog()
{
	__super::OnInitDialog();

	//变量定义
	CMatchInfoManager MatchInfoManager;
	CModuleInfoManager ModuleInfoManager;

	//获取机器码
	TCHAR szServiceMachine[LEN_MACHINE_ID];
	CWHService::GetMachineID(szServiceMachine);

	//比赛信息	
	MatchInfoManager.LoadMatchOption(m_MatchOption.wKindID,szServiceMachine,m_MatchInfoBuffer);

	//模块信息
	ModuleInfoManager.LoadGameModuleInfo(m_ModuleInfoBuffer);

	//激活向导
	ActiveWizardItem(0);

	return FALSE;
}

//确定函数
VOID CDlgMatchWizard::OnOK()
{
	if ((m_wActiveIndex+1)<CountArray(m_pWizardItem))
	{
		OnBnClickedNext();
	}
	else
	{
		OnBnClickedFinish();
	}

	return;
}


//获取配置
VOID CDlgMatchWizard::GetMatchOption(tagMatchOption & MatchOption)
{
	//设置变量
	MatchOption=m_MatchOption;
}

//设置配置
VOID CDlgMatchWizard::SetMatchOption(tagMatchOption & MatchOption)
{
	//设置变量
	m_MatchOption=MatchOption;
}

//激活向导
bool CDlgMatchWizard::ActiveWizardItem(WORD wIndex)
{
	//判断状态
	if (m_wActiveIndex==wIndex)
	{
		m_pWizardItem[m_wActiveIndex]->SetFocus();
		return true;
	}

	//保存旧项
	CDlgMatchWizardItem * pItemWizard=NULL;
	if (m_wActiveIndex<CountArray(m_pWizardItem))
	{
		//设置变量
		pItemWizard=m_pWizardItem[m_wActiveIndex];

		//保存数据
		if ((wIndex>m_wActiveIndex)&&(m_pWizardItem[m_wActiveIndex]->SaveItemData()==false)) return false;
	}

	//获取位置
	CRect rcItemRect;
	GetDlgItem(IDC_MATCH_FRAME)->GetWindowRect(rcItemRect);
	ScreenToClient(&rcItemRect);

	//设置新项
	m_pWizardItem[wIndex]->m_pDlgMatchWizard=this;
	m_pWizardItem[wIndex]->m_pMatchOption=&m_MatchOption;
	m_pWizardItem[wIndex]->m_pMatchInfoBuffer=&m_MatchInfoBuffer;
	m_pWizardItem[wIndex]->m_pModuleInfoBuffer=&m_ModuleInfoBuffer;	

	//创建新项
	m_wActiveIndex=wIndex;
	m_pWizardItem[m_wActiveIndex]->ShowWizardItem(rcItemRect,this);

	//激活新项
	m_pWizardItem[m_wActiveIndex]->SetFocus();
	if (pItemWizard!=NULL) pItemWizard->ShowWindow(SW_HIDE);

	//界面变量
	CButton * pButtonLast=(CButton * )GetDlgItem(IDC_LAST);
	CButton * pButtonNext=(CButton * )GetDlgItem(IDC_NEXT);
	CButton * pButtonFinish=(CButton * )GetDlgItem(IDC_FINISH);

	//进度界面
	pButtonLast->EnableWindow((m_wActiveIndex>0)?TRUE:FALSE);
	pButtonNext->EnableWindow(((m_wActiveIndex+1)<CountArray(m_pWizardItem))?TRUE:FALSE);
	pButtonFinish->EnableWindow((m_wActiveIndex==(CountArray(m_pWizardItem)-1))?TRUE:FALSE);	

	//构造标题
	TCHAR szTitle[128]=TEXT("");
	_sntprintf_s(szTitle,CountArray(szTitle),TEXT("比赛配置向导 --- [ 步骤 %d ]"),m_wActiveIndex+1);
	
	//设置标题
	SetWindowText(szTitle);

	return true;
}

//上一步
VOID CDlgMatchWizard::OnBnClickedLast()
{
	//效验参数
	ASSERT(m_wActiveIndex>0);
	ASSERT(m_wActiveIndex<CountArray(m_pWizardItem));

	//切换页面
	ActiveWizardItem(m_wActiveIndex-1);

	return;
}

//下一步
VOID CDlgMatchWizard::OnBnClickedNext()
{
	//效验参数
	ASSERT((m_wActiveIndex+1)<CountArray(m_pWizardItem));

	//激活页面
	ActiveWizardItem(m_wActiveIndex+1);

	return;
}

//完成按钮
VOID CDlgMatchWizard::OnBnClickedFinish()
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
	tagCreateMatchInfo CreateMatchInfo;
	ZeroMemory(&CreateMatchInfo,sizeof(CreateMatchInfo));

	//基础信息
	CreateMatchInfo.wKindID=m_MatchOption.wKindID;	
	CreateMatchInfo.dwMatchID=m_MatchOption.dwMatchID;
	CreateMatchInfo.dwClubID=m_MatchOption.dwClubID;
	CreateMatchInfo.cbShowType=m_MatchOption.cbShowType;
	CreateMatchInfo.cbMatchType=m_MatchOption.cbMatchType;	

	//报名信息
	CreateMatchInfo.lFeeAmount = m_MatchOption.lFeeAmount;
	CreateMatchInfo.wFeeMoneyID=m_MatchOption.wFeeMoneyID;
	CreateMatchInfo.wUseTicketCount=m_MatchOption.wUseTicketCount;
	CreateMatchInfo.wBuyTicketCount=m_MatchOption.wBuyTicketCount;
	CreateMatchInfo.wBuyTicketOption=m_MatchOption.wBuyTicketOption;
	CreateMatchInfo.lBuyTicketAmount=m_MatchOption.lBuyTicketAmount;
	CreateMatchInfo.wBuyTicketMoneyID=m_MatchOption.wBuyTicketMoneyID;	
	CreateMatchInfo.cbSignupMode=m_MatchOption.cbSignupMode;	
	CreateMatchInfo.cbJoinCondition=m_MatchOption.cbJoinCondition;	
	CreateMatchInfo.cbMemberOrder=m_MatchOption.cbMemberOrder;	
	CreateMatchInfo.lExperience=m_MatchOption.lExperience;

	//比赛配桌
	CopyMemory(CreateMatchInfo.cbMatchDistribute, m_MatchOption.cbMatchDistribute, sizeof(CreateMatchInfo.cbMatchDistribute));

	//拷贝信息
	StringCchCopy(CreateMatchInfo.szModuleName, CountArray(CreateMatchInfo.szModuleName),m_MatchOption.szModuleName);
	StringCchCopy(CreateMatchInfo.szMatchName, CountArray(CreateMatchInfo.szMatchName), m_MatchOption.szMatchName);

	//比赛奖励
	CreateMatchInfo.wRewardCount=m_MatchOption.wRewardCount;
	CopyMemory(CreateMatchInfo.MatchRewardItem,m_MatchOption.MatchRewardItem,sizeof(CreateMatchInfo.MatchRewardItem));

	//比赛轮次
	CreateMatchInfo.wRoundCount=m_MatchOption.wRoundCount;
	CopyMemory(CreateMatchInfo.MatchRoundItem,m_MatchOption.MatchRoundItem,sizeof(CreateMatchInfo.MatchRoundItem));

	//扩展配置
	CWHService::GetMachineID(CreateMatchInfo.szServiceMachine);
	CopyMemory(CreateMatchInfo.cbMatchRule,m_MatchOption.cbMatchRule,sizeof(CreateMatchInfo.cbMatchRule));

	//变量定义
	tagMatchOption GameMatchResult;
	CMatchInfoManager MatchInfoManager;
	
	//插入比赛
	if (CreateMatchInfo.dwMatchID==0)
	{
		if (MatchInfoManager.InsertMatchOption(&CreateMatchInfo,GameMatchResult)==false) return;

		//提示消息
		CTraceService::TraceString(TEXT("游戏比赛创建成功"),TraceLevel_Normal);
	}

	//修改比赛
	if (CreateMatchInfo.dwMatchID!=0)
	{
		//修改房间
		if (MatchInfoManager.ModifyMatchOption(&CreateMatchInfo,GameMatchResult)==false) return;

		//提示消息
		CTraceService::TraceString(TEXT("游戏比赛配置成功"),TraceLevel_Normal);
	}

	//设置标识
	m_MatchOption.dwMatchID=GameMatchResult.dwMatchID;

	//比赛奖励
	m_MatchOption.wRewardCount=GameMatchResult.wRewardCount;
	CopyMemory(m_MatchOption.MatchRewardItem,GameMatchResult.MatchRewardItem,sizeof(m_MatchOption.MatchRewardItem));

	//关闭窗口
	EndDialog(IDOK);

	return;
}

//////////////////////////////////////////////////////////////////////////
//构造函数
CDlgPromoteModeItem::CDlgPromoteModeItem(UINT nIDTemplate): CDialog(nIDTemplate)
{
	//设置变量		
	m_pMatchRoundItem=NULL;	
}

//析构函数
CDlgPromoteModeItem::~CDlgPromoteModeItem()
{
}

//设置配置
VOID CDlgPromoteModeItem::SetMatchRoundItem(tagMatchRoundItem * pMatchRoundItem)
{
	m_pMatchRoundItem=pMatchRoundItem;
}

//创建向导
bool CDlgPromoteModeItem::ShowPromoteItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//创建窗口
	if (m_hWnd==NULL) CreatePromoteItem(rcRect,pParentWnd);

	//显示窗口
	ShowWindow(SW_SHOW);

	return true;
}

//创建子项
VOID CDlgPromoteModeItem::CreatePromoteItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//设置资源
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//创建窗口
	BOOL bSuccess=Create(m_lpszTemplateName,pParentWnd);
	SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE);

	//设置资源
	AfxSetResourceHandle(GetModuleHandle(NULL));
}

//位置消息
VOID CDlgPromoteModeItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//调整控件
	RectifyControl(cx,cy);

	return;
}

//////////////////////////////////////////////////////////////////////////
//构造函数
CDlgPromoteOutGame::CDlgPromoteOutGame() : CDlgPromoteModeItem(IDD_PROMOTE_OUT_GAME)
{
}

//析构函数
CDlgPromoteOutGame::~CDlgPromoteOutGame()
{
}

//初始化函数
BOOL CDlgPromoteOutGame::OnInitDialog()
{
	__super::OnInitDialog();

	return FALSE;
}

//保存输入
bool CDlgPromoteOutGame::SaveInputInfo()
{
	//获取对象
	tagPromoteOutGame * pPromoteOutGame=(tagPromoteOutGame * )m_pMatchRoundItem->cbPromoteRule;
	ZeroMemory(pPromoteOutGame,sizeof(tagPromoteOutGame));

	//设置控件	
	pPromoteOutGame->wEliminateMulti=GetDlgItemInt(IDC_EDIT_ELIMINATE_MULTI);	
	pPromoteOutGame->wDistributeCount=GetDlgItemInt(IDC_EDIT_DISTRIBUTE_COUNT);	

	return true;
}

//刷新控件
bool CDlgPromoteOutGame::RefreshControl()
{
	//获取对象
	tagPromoteOutGame * pPromoteOutGame=(tagPromoteOutGame * )m_pMatchRoundItem->cbPromoteRule;

	//设置控件	
	SetDlgItemInt(IDC_EDIT_ELIMINATE_MULTI,pPromoteOutGame->wEliminateMulti);
	SetDlgItemInt(IDC_EDIT_DISTRIBUTE_COUNT,pPromoteOutGame->wDistributeCount);

	return true;
}

//调整控件
VOID CDlgPromoteOutGame::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////
//构造函数
CDlgPromoteFixedGame::CDlgPromoteFixedGame() : CDlgPromoteModeItem(IDD_PROMOTE_FIXED_GAME)
{
}

//析构函数
CDlgPromoteFixedGame::~CDlgPromoteFixedGame()
{
}

//初始化函数
BOOL CDlgPromoteFixedGame::OnInitDialog()
{
	__super::OnInitDialog();

	return FALSE;
}

//保存输入
bool CDlgPromoteFixedGame::SaveInputInfo()
{
	//获取对象
	tagPromoteFixedGame * pPromoteFixedGame=(tagPromoteFixedGame * )m_pMatchRoundItem->cbPromoteRule;

	//设置控件
	pPromoteFixedGame->wSwitchCount=GetDlgItemInt(IDC_EDIT_SWITCH_COUNT);

	return true;
}

//刷新控件
bool CDlgPromoteFixedGame::RefreshControl()
{
	//获取对象
	tagPromoteFixedGame * pPromoteFixedGame=(tagPromoteFixedGame * )m_pMatchRoundItem->cbPromoteRule;

	//设置控件
	SetDlgItemInt(IDC_EDIT_SWITCH_COUNT,pPromoteFixedGame->wSwitchCount);

	return true;
}

//调整控件
VOID CDlgPromoteFixedGame::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////
//构造函数
CDlgPromoteFixedTable::CDlgPromoteFixedTable() : CDlgPromoteModeItem(IDD_PROMOTE_FIXED_TABLE)
{

}

//析构函数
CDlgPromoteFixedTable::~CDlgPromoteFixedTable()
{
}

//初始化函数
BOOL CDlgPromoteFixedTable::OnInitDialog()
{
	__super::OnInitDialog();

	return FALSE;
}

//保存输入
bool CDlgPromoteFixedTable::SaveInputInfo()
{
	return true;
}

//刷新控件
bool CDlgPromoteFixedTable::RefreshControl()
{
	return true;
}

//调整控件
VOID CDlgPromoteFixedTable::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////
