#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
}

//析构函数
CDlgCustomRule::~CDlgCustomRule()
{
}

//控件绑定
VOID CDlgCustomRule::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}

//初始化函数
BOOL CDlgCustomRule::OnInitDialog()
{
	__super::OnInitDialog();

	//变量定义
	INT nButtonID;

	//最大簸簸数
	SetDlgItemInt(IDC_EDIT_MAX_BOBO_MULTIPLE,m_CustomConfig.lMaxBoboMultiple);

	//最低下注
	SetDlgItemInt(IDC_EDIT_MIN_BET_AMOUNT,m_CustomConfig.lMinBetAmount);

	//先手设置
	nButtonID=(m_CustomConfig.cbRuleOption&GR_ALLOW_FIRSTER)?IDC_RADIO_HAVE_FIRSTER:IDC_RADIO_NOT_FIRSTER;		
	CheckRadioButton(IDC_RADIO_HAVE_FIRSTER,IDC_RADIO_NOT_FIRSTER,nButtonID);

	//滚滚翻设置
	nButtonID=(m_CustomConfig.cbRuleOption&GR_ALLOW_BET_GGF)?IDC_RADIO_HAVE_BET_GGF:IDC_RADIO_NOT_BET_GGF;		
	CheckRadioButton(IDC_RADIO_HAVE_BET_GGF,IDC_RADIO_NOT_BET_GGF,nButtonID);

	//特牌设置
	nButtonID=(m_CustomConfig.cbRuleOption&GR_SHS_AND_SHL)?IDC_RADIO_HAVE_SPECIAL_CARD:IDC_RADIO_NOT_SPECIAL_CARD;		
	CheckRadioButton(IDC_RADIO_NOT_SPECIAL_CARD,IDC_RADIO_HAVE_SPECIAL_CARD,nButtonID);

	//地九王
	nButtonID=(m_CustomConfig.cbRuleOption&GR_DJW_BIG_CARD)?IDC_RADIO_DJW_BIG_CARD:IDC_RADIO_DJW_SMALL_CARD;		
	CheckRadioButton(IDC_RADIO_DJW_BIG_CARD,IDC_RADIO_DJW_SMALL_CARD,nButtonID);

	//底分设置
	nButtonID=(m_CustomConfig.cbRuleOption&GR_ALLOW_CUSTOM_SCORE)?IDC_RADIO_ALLOW_CUSTOM_SCORE:IDC_RADIO_FORBID_CUSTOM_SCORE;		
	CheckRadioButton(IDC_RADIO_ALLOW_CUSTOM_SCORE,IDC_RADIO_FORBID_CUSTOM_SCORE,nButtonID);

	return FALSE;
}

//确定函数
VOID CDlgCustomRule::OnOK() 
{ 
	//投递消息
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//取消消息
VOID CDlgCustomRule::OnCancel() 
{ 
	//投递消息
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//设置配置
bool CDlgCustomRule::SetCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize)
{
	//设置变量
	m_wCustomSize=wCustomSize;


	tagCustomConfig *pCustomConfig = (tagCustomConfig *)pcbCustomRule;

	//设置变量
	m_CustomConfig.cbRuleOption=pCustomConfig->cbRuleOption;
	m_CustomConfig.lMinBetAmount=pCustomConfig->lMinBetAmount;
	m_CustomConfig.lMaxBoboMultiple=pCustomConfig->lMaxBoboMultiple;
	
	//更新界面
	if( m_hWnd ) UpdateData(FALSE);

	return true;
}


//默认配置
bool CDlgCustomRule::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize)
{
	tagCustomConfig * pCustomConfig = (tagCustomConfig *)pcbCustomRule;

	m_CustomConfig.PrimaryDefaultCustomRule();

	CopyMemory(pCustomConfig, &m_CustomConfig, sizeof(tagCustomConfig));

	//更新界面
	if (m_hWnd)
	{
		SetCustomRule(pcbCustomRule, sizeof(tagCustomConfig));
	}

	return true;
}

//保存数据
bool CDlgCustomRule::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize)
{
	//保存数据
	if (!UpdateData(TRUE)) return false;

	//簸簸上限
	m_CustomConfig.lMaxBoboMultiple=GetDlgItemInt(IDC_EDIT_MAX_BOBO_MULTIPLE);
	if (m_CustomConfig.lMaxBoboMultiple<10) m_CustomConfig.lMaxBoboMultiple=10;

	//最低下注
	m_CustomConfig.lMinBetAmount=GetDlgItemInt(IDC_EDIT_MIN_BET_AMOUNT);
	if (m_CustomConfig.lMinBetAmount<1) m_CustomConfig.lMinBetAmount=1;	

	//先手配置
	INT nButtonID=GetCheckedRadioButton(IDC_RADIO_HAVE_FIRSTER,IDC_RADIO_NOT_FIRSTER);
	if (nButtonID==IDC_RADIO_HAVE_FIRSTER)
	{
		m_CustomConfig.cbRuleOption|=GR_ALLOW_FIRSTER;
	}
	else
	{
		m_CustomConfig.cbRuleOption&=~GR_ALLOW_FIRSTER;
	}

	//滚滚翻配置
	nButtonID=GetCheckedRadioButton(IDC_RADIO_HAVE_BET_GGF,IDC_RADIO_NOT_BET_GGF);
	if (nButtonID==IDC_RADIO_HAVE_BET_GGF)
	{
		m_CustomConfig.cbRuleOption|=GR_ALLOW_BET_GGF;
	}
	else
	{
		m_CustomConfig.cbRuleOption&=~GR_ALLOW_BET_GGF;
	}

	//特牌配置
	nButtonID=GetCheckedRadioButton(IDC_RADIO_NOT_SPECIAL_CARD,IDC_RADIO_HAVE_SPECIAL_CARD);
	if (nButtonID==IDC_RADIO_NOT_SPECIAL_CARD)
	{
		m_CustomConfig.cbRuleOption&=~GR_SHS_AND_SHL;
	}
	else
	{
		m_CustomConfig.cbRuleOption|=GR_SHS_AND_SHL;
	}

	//地九王配置
	nButtonID=GetCheckedRadioButton(IDC_RADIO_DJW_BIG_CARD,IDC_RADIO_DJW_SMALL_CARD);
	if (nButtonID==IDC_RADIO_DJW_BIG_CARD)
	{
		m_CustomConfig.cbRuleOption|=GR_DJW_BIG_CARD;
	}
	else
	{
		m_CustomConfig.cbRuleOption&=~GR_DJW_BIG_CARD;
	}

	//底分设置
	nButtonID=GetCheckedRadioButton(IDC_RADIO_ALLOW_CUSTOM_SCORE,IDC_RADIO_FORBID_CUSTOM_SCORE);
	if (nButtonID==IDC_RADIO_ALLOW_CUSTOM_SCORE)
	{
		m_CustomConfig.cbRuleOption|=GR_ALLOW_CUSTOM_SCORE;
	}
	else
	{
		m_CustomConfig.cbRuleOption&=~GR_ALLOW_CUSTOM_SCORE;
	}
	
	tagCustomConfig *pCustomConfig=(tagCustomConfig *)pcbCustomRule;

	//番数设置
	pCustomConfig->cbRuleOption=m_CustomConfig.cbRuleOption;
	pCustomConfig->lMinBetAmount=m_CustomConfig.lMinBetAmount;
	pCustomConfig->lMaxBoboMultiple=m_CustomConfig.lMaxBoboMultiple;

	return true;
}

//////////////////////////////////////////////////////////////////////////
