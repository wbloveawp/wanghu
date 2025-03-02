#include "Stdafx.h"
#include "Resource.h"
#include "DlgServerMatch.h"
#include "DlgMatchWizard.h"
#include "ModuleDBParameter.h"


//列表属性
#define LIST_STYTE LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_FLATSB

//////////////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgServerMatch, CDialog)

	//按钮消息
	ON_BN_CLICKED(IDC_CREATE_MATCH, OnBnClickedCreateMatch)
	ON_BN_CLICKED(IDC_DELETE_MATCH, OnBnClickedDeleteMatch)
	ON_BN_CLICKED(IDC_OPTION_MATCH, OnBnClickedOptionMatch)

	//控件消息
	ON_NOTIFY(NM_DBLCLK, IDC_MATCH_LIST, OnNMDblclkServerList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MATCH_LIST, OnLvnItemChangedServerList)

END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CDlgServerMatch::CDlgServerMatch() : CDialog(IDD_SERVER_MATCH)
{
	//模块参数
	ZeroMemory(&m_MatchOption,sizeof(m_MatchOption));	

	return;
}

//析构函数
CDlgServerMatch::~CDlgServerMatch()
{
}

//初始窗口
BOOL CDlgServerMatch::OnInitDialog()
{
	__super::OnInitDialog();

	//加载比赛
	LoadDBMatchItem();

	return TRUE; 
}

//数据交换
void CDlgServerMatch::DoDataExchange(CDataExchange* pDX)
{	
	__super::DoDataExchange(pDX);

	//比赛列表
	DDX_Control(pDX,IDC_MATCH_LIST,m_MatchListControl);
}

//打开比赛
bool CDlgServerMatch::OpenGameMatch()
{
	//设置资源
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//配置房间
	if (DoModal()==IDOK)
	{
		return true;
	}

	return false;
}

//加载房间
bool CDlgServerMatch::LoadDBMatchItem()
{
	//重置列表
	m_MatchListControl.DeleteAllItems();

	//获取机器码
	TCHAR szServiceMachine[LEN_MACHINE_ID];
	CWHService::GetMachineID(szServiceMachine);

	//加载信息
	CMatchInfoManager MatchInfoManager;
	if (MatchInfoManager.LoadMatchOption(0,szServiceMachine,m_MatchInfoBuffer)==true)
	{
		//变量定义
		tagMatchOption * pMatchOption=NULL;

		for(INT_PTR nIndex=0;nIndex<m_MatchInfoBuffer.m_MatchOptionArray.GetCount();nIndex++)
		{
			//获取对象
			pMatchOption=m_MatchInfoBuffer.m_MatchOptionArray[nIndex];

			//插入列表
			ASSERT(pMatchOption!=NULL);
			if (pMatchOption!=NULL) m_MatchListControl.InsertMatchOption(pMatchOption);
		}
		
		return true;
	}

	return false;
}

//配置比赛
VOID CDlgServerMatch::OnBnClickedOptionMatch()
{
	//获取选择
	ASSERT(m_MatchListControl.GetFirstSelectedItemPosition()!=NULL);
	POSITION Position=m_MatchListControl.GetFirstSelectedItemPosition();
	
	//获取房间
	INT nListItem=m_MatchListControl.GetNextSelectedItem(Position);
	tagMatchOption * pMatchOption=(tagMatchOption *)m_MatchListControl.GetItemData(nListItem);

	//变量定义
	tagMatchOption MatchOption;
	CopyMemory(&MatchOption,pMatchOption,sizeof(MatchOption));

	//设置控件
	CDlgMatchWizard DlgMatchWizard;
	DlgMatchWizard.SetMatchOption(MatchOption);	

	//配置比赛
	if (DlgMatchWizard.DoModal()==IDOK)
	{
		//获取配置
		tagMatchOption MatchOption;
		DlgMatchWizard.GetMatchOption(MatchOption);

		//修改配置
		m_MatchInfoBuffer.InsertMatchOption(&MatchOption);
		m_MatchListControl.UpdateMatchOption(pMatchOption);
	}
}

//删除比赛
VOID CDlgServerMatch::OnBnClickedDeleteMatch()
{
	//获取选择
	ASSERT(m_MatchListControl.GetFirstSelectedItemPosition()!=NULL);
	POSITION Position=m_MatchListControl.GetFirstSelectedItemPosition();
	
	//获取房间
	INT nListItem=m_MatchListControl.GetNextSelectedItem(Position);
	tagMatchOption * pMatchOption=(tagMatchOption *)m_MatchListControl.GetItemData(nListItem);

	//删除比赛
	CMatchInfoManager MatchInfoManager;
	if (MatchInfoManager.DeleteMatchOption(pMatchOption->dwMatchID))
	{		
		m_MatchListControl.DeleteItem(nListItem);
		m_MatchInfoBuffer.DeleteMatchOption(pMatchOption->dwMatchID);
	}
}

//创建比赛
VOID CDlgServerMatch::OnBnClickedCreateMatch()
{
	//设置控件
	CDlgMatchWizard DlgMatchWizard;

	//配置房间
  if (DlgMatchWizard.DoModal()==IDOK)
	{
		//获取配置
		tagMatchOption MatchOption;
		DlgMatchWizard.GetMatchOption(MatchOption);

		//插入配置
		tagMatchOption * pMatchOption=m_MatchInfoBuffer.InsertMatchOption(&MatchOption);
		if (pMatchOption!=NULL)
		{
			m_MatchListControl.InsertMatchOption(pMatchOption);
		}		
	}
}

//双击列表
VOID CDlgServerMatch::OnNMDblclkServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//加载配置
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		OnBnClickedOptionMatch();
	}

	return;
}

//选择改变
VOID CDlgServerMatch::OnLvnItemChangedServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//获取选择
	POSITION Position=m_MatchListControl.GetFirstSelectedItemPosition();

	//设置列表
	if (Position!=NULL)
	{
		//设置按钮		
		GetDlgItem(IDC_OPTION_MATCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_DELETE_MATCH)->EnableWindow(TRUE);
	}
	else
	{
		//设置按钮
		GetDlgItem(IDC_OPTION_MATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE_MATCH)->EnableWindow(FALSE);
	}

	return;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
