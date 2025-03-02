#include "Stdafx.h"
#include "Resource.h"
#include "GlobalUnits.h"
#include "GameServerDlg.h"
#include "DlgServerList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//列表属性
#define LIST_STYTE LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_FLATSB

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgServerList, CDialog)

	//按钮消息
	ON_BN_CLICKED(IDC_LOAD_SERVER, OnBnClickedLoadServer)
	ON_BN_CLICKED(IDC_RELOAD_LIST, OnBnClickedReLoadList)
	ON_BN_CLICKED(IDC_DELETE_SERVER, OnBnClickedDeleteServer)
	ON_BN_CLICKED(IDC_SERVICE_ATTRIB, OnBnClickedServiceAttrib)

	//控件消息
	ON_NOTIFY(NM_DBLCLK, IDC_SERVER_LIST, OnNMDblclkServerList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SERVER_LIST, OnLvnItemChangedServerList)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgServerList::CDlgServerList() : CDialog(IDD_DLG_SERVER_LIST)
{
	//模块参数
	m_bOptionSuccess=false;
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	return;
}

//析构函数
CDlgServerList::~CDlgServerList()
{
}

//控件绑定
VOID CDlgServerList::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVER_LIST, m_ServerListControl);
}

//消息解释
BOOL CDlgServerList::PreTranslateMessage(MSG * pMsg)
{
	//按键过滤
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//初始化函数
BOOL CDlgServerList::OnInitDialog()
{
	__super::OnInitDialog();

	//加载房间
	LoadDBServerItem();

	return TRUE;
}

//确定消息
VOID CDlgServerList::OnOK()
{
	return;
}

//加载列表
bool CDlgServerList::LoadDBServerItem()
{
	//重置列表
	m_ServerListControl.DeleteAllItems();

	//设置按钮
	GetDlgItem(IDC_LOAD_SERVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_SERVICE_ATTRIB)->EnableWindow(FALSE);

	//机器标识
	WCHAR szMachineID[LEN_MACHINE_ID];
	CWHService::GetMachineID(szMachineID);

	//加载信息
	CServerInfoManager ServerInfoManager;
	if (ServerInfoManager.LoadGameServerInfo(szMachineID,m_ServerInfoBuffer)==true)
	{
		//变量定义
		POSITION Position=m_ServerInfoBuffer.m_GameServerInfoMap.GetStartPosition();

		//枚举模块
		while (Position!=NULL)
		{
			//获取对象
			WORD wServerID=0L;
			tagGameServerInfo * pGameServerInfo=NULL;
			m_ServerInfoBuffer.m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);

			//插入列表
			ASSERT(pGameServerInfo!=NULL);
			if (pGameServerInfo!=NULL) m_ServerListControl.InsertServerInfo(pGameServerInfo);
		}

		return true;
	}

	return false;
}

//加载房间
VOID CDlgServerList::OnBnClickedLoadServer()
{
	//获取选择
	ASSERT(m_ServerListControl.GetFirstSelectedItemPosition()!=NULL);
	POSITION Position=m_ServerListControl.GetFirstSelectedItemPosition();
	
	//获取房间
	INT nListItem=m_ServerListControl.GetNextSelectedItem(Position);
	tagGameServerInfo * pGameServerInfo=(tagGameServerInfo *)m_ServerListControl.GetItemData(nListItem);

	//设置变量
	if (pGameServerInfo!=NULL)
	{
		if( InitGameServer(pGameServerInfo) )
		{
			//标志变量
			m_bOptionSuccess=true;

			//关闭窗口
			EndDialog(IDC_LOAD_SERVER);
		}
	}

	return;
}

//刷新列表
VOID CDlgServerList::OnBnClickedReLoadList()
{
	//加载列表
	LoadDBServerItem();

	return;
}

//删除房间
VOID CDlgServerList::OnBnClickedDeleteServer()
{
	//获取选择
	ASSERT(m_ServerListControl.GetFirstSelectedItemPosition()!=NULL);
	POSITION Position=m_ServerListControl.GetFirstSelectedItemPosition();
	
	//获取房间
	INT nListItem=m_ServerListControl.GetNextSelectedItem(Position);
	tagGameServerInfo * pGameServerInfo=(tagGameServerInfo *)m_ServerListControl.GetItemData(nListItem);

	//设置变量
	if (pGameServerInfo!=NULL)
	{
		//构造提示
		TCHAR szString[128]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),TEXT("确实要删除 [ %s ] 游戏房间吗？"),pGameServerInfo->szServerName);

		//提示消息
		if (AfxMessageBox(szString,MB_ICONQUESTION|MB_YESNO)!=IDYES) return;

		//删除房间
		CServerInfoManager ServerInfoManager;
		if (ServerInfoManager.DeleteGameServer(pGameServerInfo->wServerID)==false) return;

		//删除列表
		m_ServerListControl.DeleteItem(nListItem);
		m_ServerInfoBuffer.DeleteServerInfo(pGameServerInfo->wServerID);

		//设置列表
		if (m_ServerListControl.GetItemCount()>0)
		{
			//设置变量
			INT nItemCount=m_ServerListControl.GetItemCount();
			INT nNextItem=(nListItem>=nItemCount)?(nItemCount-1):nListItem;

			//设置选择
			m_ServerListControl.SetItemState(nNextItem,LVIS_SELECTED,LVIS_SELECTED);
		}
		else
		{
			//设置按钮
			GetDlgItem(IDC_LOAD_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_DELETE_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_SERVICE_ATTRIB)->EnableWindow(FALSE);
		}
	}

	return;
}

//组件属性
VOID CDlgServerList::OnBnClickedServiceAttrib()
{
	return;
}

//双击列表
VOID CDlgServerList::OnNMDblclkServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//加载配置
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		OnBnClickedLoadServer();
	}

	return;
}

//选择改变
VOID CDlgServerList::OnLvnItemChangedServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//变量定义
	NMLISTVIEW * pNMListView=(NMLISTVIEW *)pNMHDR;

	//界面控制
	if ((pNMListView->iItem!=LB_ERR)&&(((pNMListView->uNewState&ODS_SELECTED)!=0)))
	{
		//变量定义
		INT nListItem=((NMLISTVIEW *)pNMHDR)->iItem;
		tagGameServerInfo * pGameServerInfo=(tagGameServerInfo *)m_ServerListControl.GetItemData(nListItem);

		//设置按钮
		if (pGameServerInfo->dwNativeVersion!=0L)
		{
			GetDlgItem(IDC_LOAD_SERVER)->EnableWindow(TRUE);
			GetDlgItem(IDC_DELETE_SERVER)->EnableWindow(TRUE);
			GetDlgItem(IDC_SERVICE_ATTRIB)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_LOAD_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_DELETE_SERVER)->EnableWindow(TRUE);
			GetDlgItem(IDC_SERVICE_ATTRIB)->EnableWindow(FALSE);
		}
	}
	else
	{
		GetDlgItem(IDC_LOAD_SERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE_SERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_SERVICE_ATTRIB)->EnableWindow(FALSE);
	}

	return;
}

//加载指定房间
bool CDlgServerList::LoadGameServer(WORD wServerID)
{
	//机器标识
	WCHAR szMachineID[LEN_MACHINE_ID];
	CWHService::GetMachineID(szMachineID);

	//加载信息
	CServerInfoManager ServerInfoManager;
	CServerInfoBuffer ServerInfoBuffer;
	if (ServerInfoManager.LoadGameServerInfo(szMachineID,ServerInfoBuffer)==true)
	{
		//变量定义
		POSITION Position=ServerInfoBuffer.m_GameServerInfoMap.GetStartPosition();

		//枚举模块
		while (Position!=NULL)
		{
			//获取对象
			WORD wSvrID=0L;
			tagGameServerInfo * pGameServerInfo=NULL;
			ServerInfoBuffer.m_GameServerInfoMap.GetNextAssoc(Position,wSvrID,pGameServerInfo);

			//加载成功
			ASSERT(pGameServerInfo!=NULL);
			if ( pGameServerInfo!=NULL && pGameServerInfo->wServerID == wServerID )
			{
				if( !InitGameServer(pGameServerInfo) ) 
					return false;

				return true;
			}
		}
	}

	return false;
}

//初始化房间
bool CDlgServerList::InitGameServer(tagGameServerInfo *pGameServerInfo)
{
	ASSERT(pGameServerInfo);
	if( !pGameServerInfo ) return false;

	//游戏模块
	CGameServiceManagerHelper GameServiceManager;
	GameServiceManager.SetModuleCreateInfo(pGameServerInfo->szServerDLLName,GAME_SERVICE_CREATE_NAME);

	//创建判断
	if (pGameServerInfo->dwNativeVersion==0)
	{
		//构造提示
		TCHAR szString[128]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] 服务组件还没有安装，请先安装服务组件"),pGameServerInfo->szModuleName);

		//提示消息
		AfxMessageBox(szString,MB_ICONERROR);

		return false;
	}

	//更新判断
	if (pGameServerInfo->dwNativeVersion!=pGameServerInfo->dwModuleVersion)
	{
		//构造提示
		TCHAR szString[128]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] 服务组件更新了，本地版本过低，无法加载游戏房间！"),pGameServerInfo->szModuleName);

		//提示消息
		AfxMessageBox(szString,MB_ICONERROR);
		return false;
	}

	//加载模块
	if (GameServiceManager.CreateInstance()==false)
	{
		//构造提示
		TCHAR szString[128]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] 服务组件不存在或者加载失败，请重新安装服务组件"),pGameServerInfo->szModuleName);

		//提示消息
		AfxMessageBox(szString,MB_ICONERROR);

		return false;
	}

	//变量定义
	UINT uCustomRuleSize=sizeof(m_ModuleInitParameter.GameServiceOption.cbCustomRule);

	//模块属性
	GameServiceManager->GetGameServiceAttrib(m_ModuleInitParameter.GameServiceAttrib);

	//挂接属性
	m_ModuleInitParameter.GameServiceOption.wKindID=pGameServerInfo->wKindID;
	m_ModuleInitParameter.GameServiceOption.wNodeID=pGameServerInfo->wNodeID;
	m_ModuleInitParameter.GameServiceOption.wSortID=pGameServerInfo->wSortID;	
	m_ModuleInitParameter.GameServiceOption.wVisibleMask=pGameServerInfo->wVisibleMask;
	m_ModuleInitParameter.GameServiceOption.wDBListID=pGameServerInfo->wDBListID;
	
	//房间属性
	m_ModuleInitParameter.GameServiceOption.wServerID=pGameServerInfo->wServerID;
	m_ModuleInitParameter.GameServiceOption.wTableCount=pGameServerInfo->wTableCount;
	m_ModuleInitParameter.GameServiceOption.wServerType=pGameServerInfo->wServerType;
	m_ModuleInitParameter.GameServiceOption.wServerPort=pGameServerInfo->wServerPort;
	m_ModuleInitParameter.GameServiceOption.wServerLevel=pGameServerInfo->wServerLevel;
	m_ModuleInitParameter.GameServiceOption.cbMatchType= pGameServerInfo->cbMatchType;
	StringCchCopy(m_ModuleInitParameter.GameServiceOption.szServerName,LEN_SERVER, pGameServerInfo->szServerName);

	//积分配置
	m_ModuleInitParameter.GameServiceOption.lRevenue=pGameServerInfo->lRevenue;
	m_ModuleInitParameter.GameServiceOption.lCellScore=pGameServerInfo->lCellScore;
	m_ModuleInitParameter.GameServiceOption.lLessScore=pGameServerInfo->lLessScore;
	m_ModuleInitParameter.GameServiceOption.lExpGainRate=pGameServerInfo->lExpGainRate;
	m_ModuleInitParameter.GameServiceOption.lRestrictScore=pGameServerInfo->lRestrictScore;
	m_ModuleInitParameter.GameServiceOption.lMinEnterScore=pGameServerInfo->lMinEnterScore;
	m_ModuleInitParameter.GameServiceOption.lMaxEnterScore=pGameServerInfo->lMaxEnterScore;
	m_ModuleInitParameter.GameServiceOption.lMinEnterMember=pGameServerInfo->lMinEnterMember;
	m_ModuleInitParameter.GameServiceOption.lMaxEnterMember=pGameServerInfo->lMaxEnterMember;

	//房间配置
	m_ModuleInitParameter.GameServiceOption.wMaxPlayer=pGameServerInfo->wMaxPlayer;
	m_ModuleInitParameter.GameServiceOption.dwServerRule=pGameServerInfo->dwServerRule;
	m_ModuleInitParameter.GameServiceOption.dwAttachUserRight=pGameServerInfo->dwAttachUserRight;

	//扩展配置	
	CopyMemory(m_ModuleInitParameter.GameServiceOption.cbCustomRule,pGameServerInfo->cbCustomRule,uCustomRuleSize);		

	//约战配置
	m_ModuleInitParameter.GameServiceOption.dwBattleRule=pGameServerInfo->dwBattleRule;

	//分组参数
	m_ModuleInitParameter.GameServiceOption.cbDistributeMode=pGameServerInfo->cbDistributeMode;
	m_ModuleInitParameter.GameServiceOption.cbDistributeRule=pGameServerInfo->cbDistributeRule;
	m_ModuleInitParameter.GameServiceOption.wMinPlayerCount=pGameServerInfo->wMinPlayerCount;
	m_ModuleInitParameter.GameServiceOption.wMinDistributeUser=pGameServerInfo->wMinDistributeUser;	
	m_ModuleInitParameter.GameServiceOption.wMinPartakeGameUser=pGameServerInfo->wMinPartakeGameUser;
	m_ModuleInitParameter.GameServiceOption.wMaxPartakeGameUser=pGameServerInfo->wMaxPartakeGameUser;
	m_ModuleInitParameter.GameServiceOption.wDistributeTimeSpace=pGameServerInfo->wDistributeTimeSpace;

	//服务信息
	StringCchCopy(m_ModuleInitParameter.GameServiceOption.szServiceAddr,LEN_ADDRESS, pGameServerInfo->szServiceAddr);
	StringCchCopy(m_ModuleInitParameter.GameServiceOption.szServiceDomain,LEN_DOMAIN, pGameServerInfo->szServiceDomain);

	return true;
}

//////////////////////////////////////////////////////////////////////////
