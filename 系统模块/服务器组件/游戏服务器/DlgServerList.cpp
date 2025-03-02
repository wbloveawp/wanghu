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

//�б�����
#define LIST_STYTE LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_FLATSB

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgServerList, CDialog)

	//��ť��Ϣ
	ON_BN_CLICKED(IDC_LOAD_SERVER, OnBnClickedLoadServer)
	ON_BN_CLICKED(IDC_RELOAD_LIST, OnBnClickedReLoadList)
	ON_BN_CLICKED(IDC_DELETE_SERVER, OnBnClickedDeleteServer)
	ON_BN_CLICKED(IDC_SERVICE_ATTRIB, OnBnClickedServiceAttrib)

	//�ؼ���Ϣ
	ON_NOTIFY(NM_DBLCLK, IDC_SERVER_LIST, OnNMDblclkServerList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SERVER_LIST, OnLvnItemChangedServerList)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerList::CDlgServerList() : CDialog(IDD_DLG_SERVER_LIST)
{
	//ģ�����
	m_bOptionSuccess=false;
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	return;
}

//��������
CDlgServerList::~CDlgServerList()
{
}

//�ؼ���
VOID CDlgServerList::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVER_LIST, m_ServerListControl);
}

//��Ϣ����
BOOL CDlgServerList::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//��ʼ������
BOOL CDlgServerList::OnInitDialog()
{
	__super::OnInitDialog();

	//���ط���
	LoadDBServerItem();

	return TRUE;
}

//ȷ����Ϣ
VOID CDlgServerList::OnOK()
{
	return;
}

//�����б�
bool CDlgServerList::LoadDBServerItem()
{
	//�����б�
	m_ServerListControl.DeleteAllItems();

	//���ð�ť
	GetDlgItem(IDC_LOAD_SERVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_SERVICE_ATTRIB)->EnableWindow(FALSE);

	//������ʶ
	WCHAR szMachineID[LEN_MACHINE_ID];
	CWHService::GetMachineID(szMachineID);

	//������Ϣ
	CServerInfoManager ServerInfoManager;
	if (ServerInfoManager.LoadGameServerInfo(szMachineID,m_ServerInfoBuffer)==true)
	{
		//��������
		POSITION Position=m_ServerInfoBuffer.m_GameServerInfoMap.GetStartPosition();

		//ö��ģ��
		while (Position!=NULL)
		{
			//��ȡ����
			WORD wServerID=0L;
			tagGameServerInfo * pGameServerInfo=NULL;
			m_ServerInfoBuffer.m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);

			//�����б�
			ASSERT(pGameServerInfo!=NULL);
			if (pGameServerInfo!=NULL) m_ServerListControl.InsertServerInfo(pGameServerInfo);
		}

		return true;
	}

	return false;
}

//���ط���
VOID CDlgServerList::OnBnClickedLoadServer()
{
	//��ȡѡ��
	ASSERT(m_ServerListControl.GetFirstSelectedItemPosition()!=NULL);
	POSITION Position=m_ServerListControl.GetFirstSelectedItemPosition();
	
	//��ȡ����
	INT nListItem=m_ServerListControl.GetNextSelectedItem(Position);
	tagGameServerInfo * pGameServerInfo=(tagGameServerInfo *)m_ServerListControl.GetItemData(nListItem);

	//���ñ���
	if (pGameServerInfo!=NULL)
	{
		if( InitGameServer(pGameServerInfo) )
		{
			//��־����
			m_bOptionSuccess=true;

			//�رմ���
			EndDialog(IDC_LOAD_SERVER);
		}
	}

	return;
}

//ˢ���б�
VOID CDlgServerList::OnBnClickedReLoadList()
{
	//�����б�
	LoadDBServerItem();

	return;
}

//ɾ������
VOID CDlgServerList::OnBnClickedDeleteServer()
{
	//��ȡѡ��
	ASSERT(m_ServerListControl.GetFirstSelectedItemPosition()!=NULL);
	POSITION Position=m_ServerListControl.GetFirstSelectedItemPosition();
	
	//��ȡ����
	INT nListItem=m_ServerListControl.GetNextSelectedItem(Position);
	tagGameServerInfo * pGameServerInfo=(tagGameServerInfo *)m_ServerListControl.GetItemData(nListItem);

	//���ñ���
	if (pGameServerInfo!=NULL)
	{
		//������ʾ
		TCHAR szString[128]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),TEXT("ȷʵҪɾ�� [ %s ] ��Ϸ������"),pGameServerInfo->szServerName);

		//��ʾ��Ϣ
		if (AfxMessageBox(szString,MB_ICONQUESTION|MB_YESNO)!=IDYES) return;

		//ɾ������
		CServerInfoManager ServerInfoManager;
		if (ServerInfoManager.DeleteGameServer(pGameServerInfo->wServerID)==false) return;

		//ɾ���б�
		m_ServerListControl.DeleteItem(nListItem);
		m_ServerInfoBuffer.DeleteServerInfo(pGameServerInfo->wServerID);

		//�����б�
		if (m_ServerListControl.GetItemCount()>0)
		{
			//���ñ���
			INT nItemCount=m_ServerListControl.GetItemCount();
			INT nNextItem=(nListItem>=nItemCount)?(nItemCount-1):nListItem;

			//����ѡ��
			m_ServerListControl.SetItemState(nNextItem,LVIS_SELECTED,LVIS_SELECTED);
		}
		else
		{
			//���ð�ť
			GetDlgItem(IDC_LOAD_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_DELETE_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_SERVICE_ATTRIB)->EnableWindow(FALSE);
		}
	}

	return;
}

//�������
VOID CDlgServerList::OnBnClickedServiceAttrib()
{
	return;
}

//˫���б�
VOID CDlgServerList::OnNMDblclkServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//��������
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		OnBnClickedLoadServer();
	}

	return;
}

//ѡ��ı�
VOID CDlgServerList::OnLvnItemChangedServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//��������
	NMLISTVIEW * pNMListView=(NMLISTVIEW *)pNMHDR;

	//�������
	if ((pNMListView->iItem!=LB_ERR)&&(((pNMListView->uNewState&ODS_SELECTED)!=0)))
	{
		//��������
		INT nListItem=((NMLISTVIEW *)pNMHDR)->iItem;
		tagGameServerInfo * pGameServerInfo=(tagGameServerInfo *)m_ServerListControl.GetItemData(nListItem);

		//���ð�ť
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

//����ָ������
bool CDlgServerList::LoadGameServer(WORD wServerID)
{
	//������ʶ
	WCHAR szMachineID[LEN_MACHINE_ID];
	CWHService::GetMachineID(szMachineID);

	//������Ϣ
	CServerInfoManager ServerInfoManager;
	CServerInfoBuffer ServerInfoBuffer;
	if (ServerInfoManager.LoadGameServerInfo(szMachineID,ServerInfoBuffer)==true)
	{
		//��������
		POSITION Position=ServerInfoBuffer.m_GameServerInfoMap.GetStartPosition();

		//ö��ģ��
		while (Position!=NULL)
		{
			//��ȡ����
			WORD wSvrID=0L;
			tagGameServerInfo * pGameServerInfo=NULL;
			ServerInfoBuffer.m_GameServerInfoMap.GetNextAssoc(Position,wSvrID,pGameServerInfo);

			//���سɹ�
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

//��ʼ������
bool CDlgServerList::InitGameServer(tagGameServerInfo *pGameServerInfo)
{
	ASSERT(pGameServerInfo);
	if( !pGameServerInfo ) return false;

	//��Ϸģ��
	CGameServiceManagerHelper GameServiceManager;
	GameServiceManager.SetModuleCreateInfo(pGameServerInfo->szServerDLLName,GAME_SERVICE_CREATE_NAME);

	//�����ж�
	if (pGameServerInfo->dwNativeVersion==0)
	{
		//������ʾ
		TCHAR szString[128]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] ���������û�а�װ�����Ȱ�װ�������"),pGameServerInfo->szModuleName);

		//��ʾ��Ϣ
		AfxMessageBox(szString,MB_ICONERROR);

		return false;
	}

	//�����ж�
	if (pGameServerInfo->dwNativeVersion!=pGameServerInfo->dwModuleVersion)
	{
		//������ʾ
		TCHAR szString[128]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] ������������ˣ����ذ汾���ͣ��޷�������Ϸ���䣡"),pGameServerInfo->szModuleName);

		//��ʾ��Ϣ
		AfxMessageBox(szString,MB_ICONERROR);
		return false;
	}

	//����ģ��
	if (GameServiceManager.CreateInstance()==false)
	{
		//������ʾ
		TCHAR szString[128]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] ������������ڻ��߼���ʧ�ܣ������°�װ�������"),pGameServerInfo->szModuleName);

		//��ʾ��Ϣ
		AfxMessageBox(szString,MB_ICONERROR);

		return false;
	}

	//��������
	UINT uCustomRuleSize=sizeof(m_ModuleInitParameter.GameServiceOption.cbCustomRule);

	//ģ������
	GameServiceManager->GetGameServiceAttrib(m_ModuleInitParameter.GameServiceAttrib);

	//�ҽ�����
	m_ModuleInitParameter.GameServiceOption.wKindID=pGameServerInfo->wKindID;
	m_ModuleInitParameter.GameServiceOption.wNodeID=pGameServerInfo->wNodeID;
	m_ModuleInitParameter.GameServiceOption.wSortID=pGameServerInfo->wSortID;	
	m_ModuleInitParameter.GameServiceOption.wVisibleMask=pGameServerInfo->wVisibleMask;
	m_ModuleInitParameter.GameServiceOption.wDBListID=pGameServerInfo->wDBListID;
	
	//��������
	m_ModuleInitParameter.GameServiceOption.wServerID=pGameServerInfo->wServerID;
	m_ModuleInitParameter.GameServiceOption.wTableCount=pGameServerInfo->wTableCount;
	m_ModuleInitParameter.GameServiceOption.wServerType=pGameServerInfo->wServerType;
	m_ModuleInitParameter.GameServiceOption.wServerPort=pGameServerInfo->wServerPort;
	m_ModuleInitParameter.GameServiceOption.wServerLevel=pGameServerInfo->wServerLevel;
	m_ModuleInitParameter.GameServiceOption.cbMatchType= pGameServerInfo->cbMatchType;
	StringCchCopy(m_ModuleInitParameter.GameServiceOption.szServerName,LEN_SERVER, pGameServerInfo->szServerName);

	//��������
	m_ModuleInitParameter.GameServiceOption.lRevenue=pGameServerInfo->lRevenue;
	m_ModuleInitParameter.GameServiceOption.lCellScore=pGameServerInfo->lCellScore;
	m_ModuleInitParameter.GameServiceOption.lLessScore=pGameServerInfo->lLessScore;
	m_ModuleInitParameter.GameServiceOption.lExpGainRate=pGameServerInfo->lExpGainRate;
	m_ModuleInitParameter.GameServiceOption.lRestrictScore=pGameServerInfo->lRestrictScore;
	m_ModuleInitParameter.GameServiceOption.lMinEnterScore=pGameServerInfo->lMinEnterScore;
	m_ModuleInitParameter.GameServiceOption.lMaxEnterScore=pGameServerInfo->lMaxEnterScore;
	m_ModuleInitParameter.GameServiceOption.lMinEnterMember=pGameServerInfo->lMinEnterMember;
	m_ModuleInitParameter.GameServiceOption.lMaxEnterMember=pGameServerInfo->lMaxEnterMember;

	//��������
	m_ModuleInitParameter.GameServiceOption.wMaxPlayer=pGameServerInfo->wMaxPlayer;
	m_ModuleInitParameter.GameServiceOption.dwServerRule=pGameServerInfo->dwServerRule;
	m_ModuleInitParameter.GameServiceOption.dwAttachUserRight=pGameServerInfo->dwAttachUserRight;

	//��չ����	
	CopyMemory(m_ModuleInitParameter.GameServiceOption.cbCustomRule,pGameServerInfo->cbCustomRule,uCustomRuleSize);		

	//Լս����
	m_ModuleInitParameter.GameServiceOption.dwBattleRule=pGameServerInfo->dwBattleRule;

	//�������
	m_ModuleInitParameter.GameServiceOption.cbDistributeMode=pGameServerInfo->cbDistributeMode;
	m_ModuleInitParameter.GameServiceOption.cbDistributeRule=pGameServerInfo->cbDistributeRule;
	m_ModuleInitParameter.GameServiceOption.wMinPlayerCount=pGameServerInfo->wMinPlayerCount;
	m_ModuleInitParameter.GameServiceOption.wMinDistributeUser=pGameServerInfo->wMinDistributeUser;	
	m_ModuleInitParameter.GameServiceOption.wMinPartakeGameUser=pGameServerInfo->wMinPartakeGameUser;
	m_ModuleInitParameter.GameServiceOption.wMaxPartakeGameUser=pGameServerInfo->wMaxPartakeGameUser;
	m_ModuleInitParameter.GameServiceOption.wDistributeTimeSpace=pGameServerInfo->wDistributeTimeSpace;

	//������Ϣ
	StringCchCopy(m_ModuleInitParameter.GameServiceOption.szServiceAddr,LEN_ADDRESS, pGameServerInfo->szServiceAddr);
	StringCchCopy(m_ModuleInitParameter.GameServiceOption.szServiceDomain,LEN_DOMAIN, pGameServerInfo->szServiceDomain);

	return true;
}

//////////////////////////////////////////////////////////////////////////
