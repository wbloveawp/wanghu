#include "Stdafx.h"
#include "Resource.h"
#include "DlgServerMatch.h"
#include "DlgMatchWizard.h"
#include "ModuleDBParameter.h"


//�б�����
#define LIST_STYTE LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_FLATSB

//////////////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgServerMatch, CDialog)

	//��ť��Ϣ
	ON_BN_CLICKED(IDC_CREATE_MATCH, OnBnClickedCreateMatch)
	ON_BN_CLICKED(IDC_DELETE_MATCH, OnBnClickedDeleteMatch)
	ON_BN_CLICKED(IDC_OPTION_MATCH, OnBnClickedOptionMatch)

	//�ؼ���Ϣ
	ON_NOTIFY(NM_DBLCLK, IDC_MATCH_LIST, OnNMDblclkServerList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MATCH_LIST, OnLvnItemChangedServerList)

END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CDlgServerMatch::CDlgServerMatch() : CDialog(IDD_SERVER_MATCH)
{
	//ģ�����
	ZeroMemory(&m_MatchOption,sizeof(m_MatchOption));	

	return;
}

//��������
CDlgServerMatch::~CDlgServerMatch()
{
}

//��ʼ����
BOOL CDlgServerMatch::OnInitDialog()
{
	__super::OnInitDialog();

	//���ر���
	LoadDBMatchItem();

	return TRUE; 
}

//���ݽ���
void CDlgServerMatch::DoDataExchange(CDataExchange* pDX)
{	
	__super::DoDataExchange(pDX);

	//�����б�
	DDX_Control(pDX,IDC_MATCH_LIST,m_MatchListControl);
}

//�򿪱���
bool CDlgServerMatch::OpenGameMatch()
{
	//������Դ
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//���÷���
	if (DoModal()==IDOK)
	{
		return true;
	}

	return false;
}

//���ط���
bool CDlgServerMatch::LoadDBMatchItem()
{
	//�����б�
	m_MatchListControl.DeleteAllItems();

	//��ȡ������
	TCHAR szServiceMachine[LEN_MACHINE_ID];
	CWHService::GetMachineID(szServiceMachine);

	//������Ϣ
	CMatchInfoManager MatchInfoManager;
	if (MatchInfoManager.LoadMatchOption(0,szServiceMachine,m_MatchInfoBuffer)==true)
	{
		//��������
		tagMatchOption * pMatchOption=NULL;

		for(INT_PTR nIndex=0;nIndex<m_MatchInfoBuffer.m_MatchOptionArray.GetCount();nIndex++)
		{
			//��ȡ����
			pMatchOption=m_MatchInfoBuffer.m_MatchOptionArray[nIndex];

			//�����б�
			ASSERT(pMatchOption!=NULL);
			if (pMatchOption!=NULL) m_MatchListControl.InsertMatchOption(pMatchOption);
		}
		
		return true;
	}

	return false;
}

//���ñ���
VOID CDlgServerMatch::OnBnClickedOptionMatch()
{
	//��ȡѡ��
	ASSERT(m_MatchListControl.GetFirstSelectedItemPosition()!=NULL);
	POSITION Position=m_MatchListControl.GetFirstSelectedItemPosition();
	
	//��ȡ����
	INT nListItem=m_MatchListControl.GetNextSelectedItem(Position);
	tagMatchOption * pMatchOption=(tagMatchOption *)m_MatchListControl.GetItemData(nListItem);

	//��������
	tagMatchOption MatchOption;
	CopyMemory(&MatchOption,pMatchOption,sizeof(MatchOption));

	//���ÿؼ�
	CDlgMatchWizard DlgMatchWizard;
	DlgMatchWizard.SetMatchOption(MatchOption);	

	//���ñ���
	if (DlgMatchWizard.DoModal()==IDOK)
	{
		//��ȡ����
		tagMatchOption MatchOption;
		DlgMatchWizard.GetMatchOption(MatchOption);

		//�޸�����
		m_MatchInfoBuffer.InsertMatchOption(&MatchOption);
		m_MatchListControl.UpdateMatchOption(pMatchOption);
	}
}

//ɾ������
VOID CDlgServerMatch::OnBnClickedDeleteMatch()
{
	//��ȡѡ��
	ASSERT(m_MatchListControl.GetFirstSelectedItemPosition()!=NULL);
	POSITION Position=m_MatchListControl.GetFirstSelectedItemPosition();
	
	//��ȡ����
	INT nListItem=m_MatchListControl.GetNextSelectedItem(Position);
	tagMatchOption * pMatchOption=(tagMatchOption *)m_MatchListControl.GetItemData(nListItem);

	//ɾ������
	CMatchInfoManager MatchInfoManager;
	if (MatchInfoManager.DeleteMatchOption(pMatchOption->dwMatchID))
	{		
		m_MatchListControl.DeleteItem(nListItem);
		m_MatchInfoBuffer.DeleteMatchOption(pMatchOption->dwMatchID);
	}
}

//��������
VOID CDlgServerMatch::OnBnClickedCreateMatch()
{
	//���ÿؼ�
	CDlgMatchWizard DlgMatchWizard;

	//���÷���
  if (DlgMatchWizard.DoModal()==IDOK)
	{
		//��ȡ����
		tagMatchOption MatchOption;
		DlgMatchWizard.GetMatchOption(MatchOption);

		//��������
		tagMatchOption * pMatchOption=m_MatchInfoBuffer.InsertMatchOption(&MatchOption);
		if (pMatchOption!=NULL)
		{
			m_MatchListControl.InsertMatchOption(pMatchOption);
		}		
	}
}

//˫���б�
VOID CDlgServerMatch::OnNMDblclkServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//��������
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		OnBnClickedOptionMatch();
	}

	return;
}

//ѡ��ı�
VOID CDlgServerMatch::OnLvnItemChangedServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//��ȡѡ��
	POSITION Position=m_MatchListControl.GetFirstSelectedItemPosition();

	//�����б�
	if (Position!=NULL)
	{
		//���ð�ť		
		GetDlgItem(IDC_OPTION_MATCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_DELETE_MATCH)->EnableWindow(TRUE);
	}
	else
	{
		//���ð�ť
		GetDlgItem(IDC_OPTION_MATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE_MATCH)->EnableWindow(FALSE);
	}

	return;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
