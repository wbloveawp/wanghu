#include "StdAfx.h"
#include "Resource.h"
#include "ModuleDBParameter.h"
#include "BattleListControl.h"


//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgBattleOption, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_BATTLE_LIST, OnNMlclkModuleList)
	ON_NOTIFY(NM_DBLCLK, IDC_BATTLE_LIST, OnNMlDBclkModuleList)
	
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CBattleOptionBuffer::CBattleOptionBuffer()
{
}

//��������
CBattleOptionBuffer::~CBattleOptionBuffer()
{
	//��������
	WORD wBattleID=0;
	tagBattleOption * pBattleOption=NULL;
	POSITION Position=m_BattleOptionMap.GetStartPosition();

	//ɾ������
	while (Position!=NULL)
	{
		m_BattleOptionMap.GetNextAssoc(Position,wBattleID,pBattleOption);
		SafeDelete(pBattleOption);
	}

	//ɾ������
	for (INT_PTR i=0;i<m_BattleOptionArray.GetCount();i++)
	{
		pBattleOption=m_BattleOptionArray[i];
		SafeDelete(pBattleOption);
	}

	//ɾ������
	m_BattleOptionMap.RemoveAll();
	m_BattleOptionArray.RemoveAll();

	return;
}

//��������
bool CBattleOptionBuffer::ResetBattleOption()
{
	//��������
	WORD wBattleID=0;
	tagBattleOption * pBattleOption=NULL;
	POSITION Position=m_BattleOptionMap.GetStartPosition();

	//ɾ������
	while (Position!=NULL)
	{
		m_BattleOptionMap.GetNextAssoc(Position,wBattleID,pBattleOption);
		m_BattleOptionArray.Add(pBattleOption);
	}

	//ɾ������
	m_BattleOptionMap.RemoveAll();

	return true;
}

//ɾ������
bool CBattleOptionBuffer::DeleteBattleOption(WORD wBattleID)
{
	//��������
	tagBattleOption * pBattleOption=NULL;
	if (m_BattleOptionMap.Lookup(wBattleID,pBattleOption)==FALSE) return false;

	//ɾ������
	m_BattleOptionMap.RemoveKey(wBattleID);
	m_BattleOptionArray.Add(pBattleOption);

	//���ñ���
	ZeroMemory(pBattleOption,sizeof(tagBattleOption));

	return true;
}

//��������
bool CBattleOptionBuffer::InsertBattleOption(tagBattleOption * pBattleOption)
{
	//Ч�����
	ASSERT(pBattleOption!=NULL);
	if (pBattleOption==NULL) return false;

	//�����ִ�
	WORD wBattleID=pBattleOption->wBattleID;
	tagBattleOption * pBattleOptionInsert=SearchBattleOption(wBattleID);

	//�����ж�
	if (pBattleOptionInsert==NULL)
	{
		//��������
		pBattleOptionInsert=CreateBattleOption();

		//����ж�
		if (pBattleOptionInsert==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//��������
	m_BattleOptionMap[wBattleID]=pBattleOptionInsert;
	CopyMemory(pBattleOptionInsert,pBattleOption,sizeof(tagBattleOption));

	return true;
}

//��ȡ��Ŀ
DWORD CBattleOptionBuffer::GetBattleOptionCount()
{
	return (DWORD)(m_BattleOptionMap.GetCount());
}

//��������
tagBattleOption * CBattleOptionBuffer::SearchBattleOption(WORD wBattleID)
{
	tagBattleOption * pBattleOption=NULL;
	m_BattleOptionMap.Lookup(wBattleID,pBattleOption);
	return pBattleOption;
}

//��������
tagBattleOption * CBattleOptionBuffer::CreateBattleOption()
{
	//��������
	tagBattleOption * pBattleOption=NULL;

	//��������
	try
	{
		INT_PTR nArrayCount=m_BattleOptionArray.GetCount();
		if (nArrayCount>0)
		{
			pBattleOption=m_BattleOptionArray[nArrayCount-1];
			m_BattleOptionArray.RemoveAt(nArrayCount-1);
		}
		else
		{
			pBattleOption=new tagBattleOption;
			if (pBattleOption==NULL) return NULL;
		}
	}
	catch (...) { return false; }

	//���ñ���
	ZeroMemory(pBattleOption,sizeof(tagBattleOption));

	return pBattleOption;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CBattleListControl::CBattleListControl()
{
	m_pBattleOptionBuffer=NULL;
}

//��������
CBattleListControl::~CBattleListControl()
{
}

//�����б�
VOID CBattleListControl::InitListControl()
{
	//��������
	INT nColIndex=0;

	//�����б�
	InsertColumn(nColIndex++,TEXT("��Ϸ����"),LVCFMT_CENTER,80);
	InsertColumn(nColIndex++,TEXT("���ͱ�ʶ"),LVCFMT_CENTER,80);	
	InsertColumn(nColIndex++,TEXT("��������"),LVCFMT_LEFT,140);
	InsertColumn(nColIndex++,TEXT("����״̬"),LVCFMT_LEFT,100);

	return;
}

//��������
INT CBattleListControl::SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex)
{
	//��������
	tagGameModuleInfo * pGameModuleInfo1=(tagGameModuleInfo *)lParam1;
	tagGameModuleInfo * pGameModuleInfo2=(tagGameModuleInfo *)lParam2;

	//��������
	switch (wColumnIndex)
	{
	case 0:		//��Ϸ����
		{
			return lstrcmp(pGameModuleInfo1->szModuleName,pGameModuleInfo2->szModuleName);
		}
	case 1:		//���ͱ�ʶ
		{
			return pGameModuleInfo1->wModuleID<pGameModuleInfo2->wModuleID?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 2:		//��������
		{
			return lstrcmp(pGameModuleInfo1->szServerDLLName,pGameModuleInfo2->szServerDLLName);
		}
	case 3:		//����״̬
		{
			//Լս��ʶ
			WORD wBattleID1=pGameModuleInfo1->wModuleID;
			WORD wBattleID2=pGameModuleInfo2->wModuleID;

			//������ʶ
			bool bCreateStatus1=m_pBattleOptionBuffer->SearchBattleOption(wBattleID1)!=NULL;
			bool bCreateStatus2=m_pBattleOptionBuffer->SearchBattleOption(wBattleID2)!=NULL;

			//��ͬ״̬
			if (bCreateStatus1==bCreateStatus2)
			{
				return wBattleID1<wBattleID2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			}

			return bCreateStatus1?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	}

	return 0;
}

//��ȡ��ɫ
VOID CBattleListControl::GetListItemColor(INT nItemID,LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor)
{
	//��������
	ASSERT(lItemParam!=NULL);
	tagGameModuleInfo * pGameModuleInfo=(tagGameModuleInfo *)lItemParam;

	//û�д���
	if (m_pBattleOptionBuffer->SearchBattleOption(pGameModuleInfo->wModuleID)==false)
	{
		//������ɫ
		ListItemColor.rcTextColor=RGB(127,127,127);
		ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(0,0,128):CR_NORMAL_BK;		
	}
	else
	{
		//������ɫ
		ListItemColor.rcTextColor=RGB(0,0,0);
		ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(127,127,127):CR_NORMAL_BK;
	}

	return;
}

//�����б�
bool CBattleListControl::InsertModuleInfo(tagGameModuleInfo * pGameModuleInfo)
{
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pGameModuleInfo;

	//�����ж�
	INT nInsertItem=FindItem(&FindInfo);
	if (nInsertItem!=LB_ERR) return true;

	//�����б�
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		if (i==0)
		{
			//��������
			INT nIndex=GetInsertIndex(pGameModuleInfo);
			LPCTSTR pszDescribe=GetDescribeString(pGameModuleInfo,i);
			nInsertItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,pszDescribe,0,0,0,(LPARAM)pGameModuleInfo);
		}
		else
		{
			//�ַ�����
			SetItem(nInsertItem,i,LVIF_TEXT,GetDescribeString(pGameModuleInfo,i),0,0,0,0);
		}
	}

	return true;
}

//���û���
void CBattleListControl::SetBattleOptionBuffer(CBattleOptionBuffer * pBattleOptionBuffer)
{
	//���û���
	m_pBattleOptionBuffer=pBattleOptionBuffer;
}

//��������
WORD CBattleListControl::GetInsertIndex(tagGameModuleInfo * pGameModuleInfo)
{
	//��������
	INT nItemCount=GetItemCount();
	tagGameModuleInfo * pGameModuleInfoTemp=NULL;

	//����״̬
	bool bCreateStatus=m_pBattleOptionBuffer->SearchBattleOption(pGameModuleInfo->wModuleID)!=NULL;

	//��ȡλ��
	for (INT i=0;i<nItemCount;i++)
	{
		//��ȡ����
		pGameModuleInfoTemp=(tagGameModuleInfo *)GetItemData(i);

		//������ʶ
		bool bCreateStatusTemp=m_pBattleOptionBuffer->SearchBattleOption(pGameModuleInfoTemp->wModuleID)!=NULL;

		//�����ж�
		if (bCreateStatus==false && bCreateStatusTemp==true)
		{
			continue;
		}

		//�����ж�
		if (bCreateStatus==true && bCreateStatusTemp==false)
		{
			return i;
		}
		
		//�����ж�
		if (pGameModuleInfo->wModuleID<pGameModuleInfoTemp->wModuleID)
		{
			return i;
		}
	}

	return nItemCount;
}

//�����ַ�
LPCTSTR CBattleListControl::GetDescribeString(tagGameModuleInfo * pGameModuleInfo, WORD wColumnIndex)
{
	//�����ַ�
	switch (wColumnIndex)
	{
	case 0:		//��Ϸ����
		{
			return pGameModuleInfo->szModuleName;
		}
	case 1:		//���ͱ�ʶ
		{
			static TCHAR szModuleID[12] = {0};
			_sntprintf_s(szModuleID,CountArray(szModuleID),TEXT("%d"),pGameModuleInfo->wModuleID);

			return szModuleID;
		}
	case 2:		//��������
		{
			return pGameModuleInfo->szServerDLLName;
		}
	case 3:		//����״̬
		{
			//����״̬
			bool bCreateStatus=m_pBattleOptionBuffer->SearchBattleOption(pGameModuleInfo->wModuleID)!=NULL;
			return bCreateStatus?TEXT("�Ѵ���"):TEXT("δ����");
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
//���캯��
CBattleOptionManager::CBattleOptionManager()
{
}

//��������
CBattleOptionManager::~CBattleOptionManager()
{
}

//�޸�ģ��
bool CBattleOptionManager::UpdateBattleOption(tagBattleOption & BattleOption)
{
	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//��������
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("UpdateGameModuleInfo ���� PlatformDBModule ����ʧ��"),MB_ICONERROR);
		return false;
	}

	//�������ݿ�
	try
	{
		//��������
		CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
		tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

		//��������
		PlatformDBModule->SetConnectionInfo(pDataBaseParameter->dwDataBaseAddr,pDataBaseParameter->wDataBasePort,
		CW2CT(pDataBaseParameter->szDataBaseName),CW2CT(pDataBaseParameter->szDataBaseUser),CW2CT(pDataBaseParameter->szDataBasePass));

		//��������
		PlatformDBModule->OpenConnection();
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//��ȡ�б�
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wBattleID"),BattleOption.wBattleID);
		PlatformDBAide.AddParameter(TEXT("@wServicePort"),BattleOption.wServicePort);
		PlatformDBAide.AddParameter(TEXT("@szModuleName"),BattleOption.szModuleName);
		PlatformDBAide.AddParameter(TEXT("@szServiceAddr"),BattleOption.szServiceAddr);

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_UpdateBattleOption"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//��ȡ��Ϣ
		BattleOption.wBattleID=PlatformDBAide.GetValue_WORD(TEXT("BattleID"));
		BattleOption.wServicePort=PlatformDBAide.GetValue_WORD(TEXT("ServicePort"));
		PlatformDBAide.GetValue_String(TEXT("ModuleName"),BattleOption.szModuleName,CountArray(BattleOption.szModuleName));
		PlatformDBAide.GetValue_String(TEXT("ServiceAddr"),BattleOption.szServiceAddr,CountArray(BattleOption.szServiceAddr));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}

//��������
bool CBattleOptionManager::InsertBattleOption(tagBattleOption & BattleOption)
{
	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//��������
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("UpdateGameModuleInfo ���� PlatformDBModule ����ʧ��"),MB_ICONERROR);
		return false;
	}

	//�������ݿ�
	try
	{
		//��������
		CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
		tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

		//��������
		PlatformDBModule->SetConnectionInfo(pDataBaseParameter->dwDataBaseAddr,pDataBaseParameter->wDataBasePort,
		CW2CT(pDataBaseParameter->szDataBaseName),CW2CT(pDataBaseParameter->szDataBaseUser),CW2CT(pDataBaseParameter->szDataBasePass));

		//��������
		PlatformDBModule->OpenConnection();
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//��ȡ�б�
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wBattleID"),BattleOption.wBattleID);
		PlatformDBAide.AddParameter(TEXT("@wServicePort"),BattleOption.wServicePort);
		PlatformDBAide.AddParameter(TEXT("@szModuleName"),BattleOption.szModuleName);
		PlatformDBAide.AddParameter(TEXT("@szServiceAddr"),BattleOption.szServiceAddr);

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_InsertBattleOption"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//��ȡ��Ϣ
		BattleOption.wBattleID=PlatformDBAide.GetValue_WORD(TEXT("BattleID"));
		BattleOption.wServicePort=PlatformDBAide.GetValue_WORD(TEXT("ServicePort"));
		PlatformDBAide.GetValue_String(TEXT("ModuleName"),BattleOption.szModuleName,CountArray(BattleOption.szModuleName));
		PlatformDBAide.GetValue_String(TEXT("ServiceAddr"),BattleOption.szServiceAddr,CountArray(BattleOption.szServiceAddr));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}

//����ģ��
bool CBattleOptionManager::LoadBattleOption(CBattleOptionBuffer & BattleOptionBuffer)
{
	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//��������
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("LoadGameModuleInfo ���� PlatformDBModule ����ʧ��"),MB_ICONERROR);
		return false;
	}

	//�������ݿ�
	try
	{
		//��������
		CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
		tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

		//��������
		PlatformDBModule->SetConnectionInfo(pDataBaseParameter->dwDataBaseAddr,pDataBaseParameter->wDataBasePort,
		CW2CT(pDataBaseParameter->szDataBaseName),CW2CT(pDataBaseParameter->szDataBaseUser),CW2CT(pDataBaseParameter->szDataBasePass));

		//��������
		PlatformDBModule->OpenConnection();
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//��ȡ�б�
		PlatformDBAide.ResetParameter();
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadBattleOption"),true)==DB_SUCCESS)
		{
			//����б�
			BattleOptionBuffer.ResetBattleOption();

			//��ȡ�б�
			while (PlatformDBModule->IsRecordsetEnd()==false)
			{
				//��������
				tagBattleOption BattleOption;
				ZeroMemory(&BattleOption,sizeof(BattleOption));

				//��ȡ��Ϣ
				BattleOption.wBattleID=PlatformDBAide.GetValue_WORD(TEXT("BattleID"));
				BattleOption.wServicePort=PlatformDBAide.GetValue_WORD(TEXT("ServicePort"));
				PlatformDBAide.GetValue_String(TEXT("ModuleName"),BattleOption.szModuleName,CountArray(BattleOption.szModuleName));
				PlatformDBAide.GetValue_String(TEXT("ServiceAddr"),BattleOption.szServiceAddr,CountArray(BattleOption.szServiceAddr));

				//�б���
				BattleOptionBuffer.InsertBattleOption(&BattleOption);

				//�ƶ���¼
				PlatformDBModule->MoveToNext();
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
//���캯��
CDlgBattleOption::CDlgBattleOption() : CDialog(IDD_BATTLE_OPTION)
{
	//���ñ���
	m_pGameModuleInfo=NULL;

	//���û���
	m_BattleListControl.SetBattleOptionBuffer(&m_BattleOptionBuffer);

	return;
}

//��������
CDlgBattleOption::~CDlgBattleOption()
{
}

//�ؼ���
VOID CDlgBattleOption::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);	
	DDX_Control(pDX, IDC_BATTLE_LIST, m_BattleListControl);
}

//��ʼ������
BOOL CDlgBattleOption::OnInitDialog()
{
	__super::OnInitDialog();

	//�����б�
	LoadDBModuleItem();

	return TRUE;
}

//ȷ������
VOID CDlgBattleOption::OnOK()
{
	//ѡ���ж�
	if (m_pGameModuleInfo==NULL)
	{
		AfxMessageBox(TEXT("��ΪԼս����ѡ��һ����Ϸ���ͣ�"),MB_ICONERROR);

		return;
	}

	//�����ַ
	TCHAR szServiceAddr[LEN_DOMAIN];
	GetDlgItemText(IDC_SERVICE_ADDR,szServiceAddr,CountArray(szServiceAddr));
	if (szServiceAddr[0]==0)
	{
		AfxMessageBox(TEXT("��������Ч�ķ����ַ��������"),MB_ICONERROR);

		return;
	}

	//����˿�
	WORD wServicePort=0;
	wServicePort=GetDlgItemInt(IDC_SERVICE_PORT);

	//������Ϣ
	tagBattleOption BattleOption;
	ZeroMemory(&BattleOption,sizeof(BattleOption));

	//ģ������
	BattleOption.wServicePort=wServicePort;	
	BattleOption.wBattleID=m_pGameModuleInfo->wModuleID;
	lstrcpyn(BattleOption.szServiceAddr,szServiceAddr,CountArray(BattleOption.szServiceAddr));
	lstrcpyn(BattleOption.szModuleName,m_pGameModuleInfo->szModuleName,CountArray(BattleOption.szModuleName));
	
	//��������
	if (m_BattleOptionBuffer.SearchBattleOption(BattleOption.wBattleID)==NULL)
	{
		//��������
		if (m_BattleOptionManager.InsertBattleOption(BattleOption)==false) return;

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("Լս���񴴽��ɹ�"),TraceLevel_Normal);
	}
	else	
	{
		//�޸�����
		if (m_BattleOptionManager.UpdateBattleOption(BattleOption)==false) return;

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("Լս�������óɹ�"),TraceLevel_Normal);
	}

	//��������
	CopyMemory(&m_BattleOption,&BattleOption,sizeof(m_BattleOption));

	//�رմ���
	EndDialog(IDOK);
}

//ȡ����Ϣ
VOID CDlgBattleOption::OnCancel()
{
	//�������
	ZeroMemory(&m_BattleOption,sizeof(m_BattleOption));

	//�رմ���
	EndDialog(IDCANCEL);
}

//�����б�
bool CDlgBattleOption::LoadDBModuleItem()
{
	//����Լս
	m_BattleOptionManager.LoadBattleOption(m_BattleOptionBuffer);

	//����ģ��
	if (m_ModuleInfoManager.LoadGameModuleInfo(m_ModuleInfoBuffer)==true)
	{
		//�����б�
		m_BattleListControl.DeleteAllItems();

		//��������
		POSITION Position=m_ModuleInfoBuffer.m_GameModuleInfoMap.GetStartPosition();

		//ö��ģ��
		while (Position!=NULL)
		{
			//��ȡ����
			WORD wModuleID=0L;
			tagGameModuleInfo * pGameModuleInfo=NULL;
			m_ModuleInfoBuffer.m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameModuleInfo);

			//�����б�
			ASSERT(pGameModuleInfo!=NULL);
			if (pGameModuleInfo!=NULL) m_BattleListControl.InsertModuleInfo(pGameModuleInfo);
		}
	}	

	return false;
}

//˫���б�
VOID CDlgBattleOption::OnNMlclkModuleList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//��������
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		//��������
		INT nListItem=((NMITEMACTIVATE *)pNMHDR)->iItem;
		m_pGameModuleInfo=(tagGameModuleInfo *)m_BattleListControl.GetItemData(nListItem);

		//����Լս
		tagBattleOption * pBattleOption=m_BattleOptionBuffer.SearchBattleOption(m_pGameModuleInfo->wModuleID);
		if (pBattleOption!=NULL)
		{
			SetDlgItemInt(IDC_SERVICE_PORT,pBattleOption->wServicePort);			 
			SetDlgItemText(IDC_SERVICE_ADDR,pBattleOption->szServiceAddr);			 
		}		
		else
		{
			SetDlgItemText(IDC_SERVICE_PORT,TEXT(""));			 
			SetDlgItemText(IDC_SERVICE_ADDR,TEXT(""));			 
		}

		return;
	}

	return;
}

//˫���б�
VOID CDlgBattleOption::OnNMlDBclkModuleList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//��������
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		//��������
		INT nListItem=((NMITEMACTIVATE *)pNMHDR)->iItem;
		m_pGameModuleInfo=(tagGameModuleInfo *)m_BattleListControl.GetItemData(nListItem);

		return OnOK();
	}

	return;
}

//////////////////////////////////////////////////////////////////////////