#include "StdAfx.h"
#include "Resource.h"
#include "DataFile.h"
#include "DlgServiceOption.h"

//////////////////////////////////////////////////////////////////////////////////
//��Ϣӳ��
BEGIN_MESSAGE_MAP(CDlgServiceOption, CDialog)
	ON_BN_CLICKED(IDC_BT_QUIT, OnBnClickedQuit)
	ON_BN_CLICKED(IDC_BT_ADD_SERVICE, OnBnClickedAddServiceInfo)
	ON_BN_CLICKED(IDC_BT_MODIFY_SERVICE, OnBnClickedModifyServiceInfo)
	ON_BN_CLICKED(IDC_BT_DELETE_SERVICE, OnBnClickedDeleteServiceInfo)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SERVICE_INFO, OnHdnItemclickListServiceInfo)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SERVICE_INFO, OnHdnItemDblclkListServiceInfo)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_SERVICE_INFO, OnLvnItemKeydownListServiceInfo)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServiceOption::CDlgServiceOption() : CDialog(IDD_SERVICE_OPTION)
{
	//���ñ���
	m_bOptionModule=false;
	m_szDataFileName[0]=0;
	m_nSelectServiceItem=-1;
	ZeroMemory(&m_ServiceOption,sizeof(m_ServiceOption));

	return;
}

//��������
CDlgServiceOption::~CDlgServiceOption()
{
	//�ͷŶ���
	m_ServiceOptionActive.Append(m_ServiceOptionBuffer);

	//�ͷŶ���
	for (INT_PTR i=0; i<m_ServiceOptionActive.GetCount(); i++)
	{
		SafeDelete(m_ServiceOptionActive[i]);
	}

	//�Ƴ�����
	m_ServiceOptionActive.RemoveAll();
	m_ServiceOptionBuffer.RemoveAll();
}

//��ʼ������
BOOL CDlgServiceOption::OnInitDialog()
{
	__super::OnInitDialog();

	//��ʼ�б�
	InitControl();

	//��ȡ����
	LoadServiceOption();
	
	//���ÿؼ�
	GetDlgItem(IDC_SERVICE_ID)->ShowWindow(m_bOptionModule?SW_HIDE:SW_SHOW);
	GetDlgItem(IDC_COMBO_MODULE)->ShowWindow(m_bOptionModule?SW_SHOW:SW_HIDE);

	//����ģ��
	if (m_bOptionModule == true) LoadDBModuleItem();

	return TRUE;
}

//��Ϣ����
BOOL CDlgServiceOption::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message==WM_KEYDOWN)&&((pMsg->wParam==VK_ESCAPE)||(pMsg->wParam==VK_RETURN)))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//�ؼ���
VOID CDlgServiceOption::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);	
}

//��������
VOID CDlgServiceOption::SetServiceKind(tagServiceKind ServiceKind[], WORD wKindCount)
{
	//�������
	for (int i = 0; i < wKindCount; i++)
	{
		m_ServiceKindArray.Add(ServiceKind[i]);
	}
}

//���ñ�ʶ
VOID CDlgServiceOption::SetOptionModule(bool bOptionModule)
{
	m_bOptionModule = bOptionModule;
}

//�ļ�����
VOID CDlgServiceOption::SetDataFileName(LPCTSTR pszFileName)
{
	StringCchCopy(m_szDataFileName, CountArray(m_szDataFileName), pszFileName);
}

//��ȡ����
tagServiceOption * CDlgServiceOption::GetServiceOption()
{ 
	return &m_ServiceOption; 
}

//��ȡ����
tagServiceOption * CDlgServiceOption::GetServiceOption(WORD wServiceID)
{
	//����·��
	TCHAR szDataFile[MAX_PATH] = TEXT("");
	GetDataFilePath(szDataFile, CountArray(szDataFile));

	//��ȡ����
	CDataFile DataFile;
	if (DataFile.OpenDataFile(szDataFile) == FALSE)
	{
		return NULL;
	}

	//���ҷ���
	WORD wItemCount = DataFile.GetItemCount();
	for (WORD wIndex = 0; wIndex < wItemCount; ++wIndex)
	{
		//��ȡ����
		if (DataFile.ReadDataItem(&m_ServiceOption, sizeof(m_ServiceOption), wIndex)==0)
		{
			break;
		}

		//У�����
		if (m_ServiceOption.wServiceID == wServiceID)
		{
			return &m_ServiceOption;
		}
	}	

	return NULL;
}

//��������
bool CDlgServiceOption::LoadServiceOption()
{
	//����·��
	TCHAR szDataFile[MAX_PATH] = TEXT("");
	GetDataFilePath(szDataFile, CountArray(szDataFile));

	//��ȡ����
	CDataFile DataFile;
	if (DataFile.OpenDataFile(szDataFile)==FALSE) return false;

	//��������
	WORD wItemCount=DataFile.GetItemCount();
	if (wItemCount>0)
	{
		//��ȡָ��
		CListCtrl * pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);

		for (WORD wIndex=0;wIndex<wItemCount;++wIndex)
		{
			//��ȡ����
			tagServiceOption * pServiceOption=ActiveServiceOption();
			if (pServiceOption==NULL) return false;

			//��ȡ����
			DataFile.ReadDataItem(pServiceOption,sizeof(tagServiceOption),wIndex);			

			//����ṹ
			LVITEM lVItem;
			lVItem.mask=LVIF_TEXT;  
			lVItem.iItem=pListCtrl->GetItemCount();
			lVItem.iSubItem=0; 
			lVItem.pszText=TEXT("");

			//�����б�
			int nInsertItem=pListCtrl->InsertItem(&lVItem);

			//��������
			SetServiceToControl(pServiceOption,pListCtrl,nInsertItem);

			//��������
			pListCtrl->SetItemData(nInsertItem,(DWORD_PTR)pServiceOption);
		}

		//��������
		pListCtrl->UpdateData();
	}	
	
	return true;
}

//��������
bool CDlgServiceOption::SaveServiceOption()
{
	//����·��
	TCHAR szDataFile[MAX_PATH] = TEXT("");	
	GetDataFilePath(szDataFile, CountArray(szDataFile));

	//�ļ�
	CDataFile DataFile;
	if (DataFile.CreateDataFile(szDataFile)==FALSE)
	{
		AfxMessageBox(TEXT("�����ļ�����ʧ��"),MB_ICONQUESTION);

		return false;
	}

	//��ȡָ��
	CListCtrl * pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);

	if (pListCtrl->GetItemCount()>0)
	{
		//��������
		tagServiceOption *pServiceOption=NULL;

		for (int nIndex=0;nIndex<pListCtrl->GetItemCount();++nIndex)
		{
			//��ȡ�ı�
			pServiceOption=(tagServiceOption *)pListCtrl->GetItemData(nIndex);			

			//�������
			DataFile.WriteDataItem(pServiceOption,sizeof(tagServiceOption));
		}
	}

	return true;
}

//�������
tagServiceOption * CDlgServiceOption::ActiveServiceOption()
{
	//��������
	tagServiceOption *pServiceOption=NULL;
	INT nItemCount=m_ServiceOptionBuffer.GetCount();

	//���һ���
	if (nItemCount>0)
	{		
		pServiceOption=m_ServiceOptionBuffer[nItemCount-1];
		m_ServiceOptionBuffer.RemoveAt(nItemCount-1);
		m_ServiceOptionActive.Add(pServiceOption);
		return pServiceOption;
	}

	try
	{
		//��������
		pServiceOption=new tagServiceOption;
		if (pServiceOption==NULL) throw TEXT("���󴴽�ʧ�ܣ�");

		//��ʼ����
		m_ServiceOptionActive.Add(pServiceOption);
		ZeroMemory(pServiceOption,sizeof(tagServiceOption));		
	}
	catch (...) 
	{ 
		ASSERT(FALSE); 
	}

	return pServiceOption;
}

//�ͷŶ���
void CDlgServiceOption::FreeServiceOption(tagServiceOption * pServiceOption)
{
	for (INT_PTR i=0; i<m_ServiceOptionActive.GetCount(); i++)
	{
		if (m_ServiceOptionActive[i]==pServiceOption)
		{
			m_ServiceOptionActive.RemoveAt(i);
			break;
		}
	}

	//���뻺��
	m_ServiceOptionBuffer.Add(pServiceOption);
}

//��ȡ��ʶ
bool CDlgServiceOption::GetServiceID(WORD & wServiceID)
{
	//�����ʶ
	if (m_bOptionModule == false)
	{
		wServiceID=GetDlgItemInt(IDC_SERVICE_ID);

		return true;
	}

	//��ȡ�ؼ�		
	CComboBox * pComboBox = (CComboBox *)GetDlgItem(IDC_COMBO_MODULE);
	if (pComboBox->GetCurSel() == -1)
	{
		AfxMessageBox(TEXT("����ģ�鲻��Ϊ�գ�"));
		GetDlgItem(IDC_COMBO_MODULE)->SetFocus();

		return false;
	}

	//��ȡģ��
	tagGameModuleInfo * pGameModuleInfo = (tagGameModuleInfo *)pComboBox->GetItemData(pComboBox->GetCurSel());

	//���ñ�ʶ
	wServiceID = pGameModuleInfo->wModuleID;	

	return true;
}

//���ñ�ʶ
bool CDlgServiceOption::SetServiceID(WORD wServiceID)
{
	//���ñ�ʶ
	if (m_bOptionModule == false)
	{
		SetDlgItemInt(IDC_SERVICE_ID, wServiceID);

		return true;
	}

	//��������
	tagGameModuleInfo * pGameModuleInfo = NULL;
	CComboBox * pComboBox = (CComboBox *)GetDlgItem(IDC_COMBO_MODULE);

	//����ģ��
	for (INT_PTR i = 0; i < pComboBox->GetCount(); i++)
	{
		pGameModuleInfo = (tagGameModuleInfo *)pComboBox->GetItemData(i);
		if (pGameModuleInfo->wModuleID == wServiceID)
		{
			pComboBox->SetCurSel(i);

			return true;
		}
	}

	return false;
}

//��ʼ�б�
VOID CDlgServiceOption::InitControl()
{
	//��ȡָ��
	auto pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);
	pListCtrl->SetExtendedStyle(pListCtrl->GetExtendedStyle()|LVS_EX_FULLROWSELECT);

	//����ͷ��
	pListCtrl->InsertColumn(0,TEXT("�����ʶ"),LVCFMT_LEFT,60);	
	pListCtrl->InsertColumn(1,TEXT("��������"),LVCFMT_LEFT,100);
	pListCtrl->InsertColumn(2, TEXT("��������"), LVCFMT_LEFT, 100);
	pListCtrl->InsertColumn(3,TEXT("�������"),LVCFMT_LEFT,70);
	pListCtrl->InsertColumn(4,TEXT("����˿�"),LVCFMT_LEFT,70);
	pListCtrl->InsertColumn(5,TEXT("�����ַ"),LVCFMT_LEFT,80);	

	//��ȡ�ؼ�
	auto pComboBox = (CComboBox*)GetDlgItem(IDC_SERVICE_KIND);

	//�������
	for (INT_PTR i = 0; i < m_ServiceKindArray.GetCount(); i++)
	{
		pComboBox->SetItemData(pComboBox->InsertString(i,m_ServiceKindArray[i].szServiceKindName), m_ServiceKindArray[i].wServiceKindID);
	}

	//Ĭ��ѡ��
	if (pComboBox->GetCurSel() == -1) pComboBox->SetCurSel(0);
}

//�����б�
VOID CDlgServiceOption::LoadDBModuleItem()
{
	//��ȡģ��
	CComboBox * pComboBox = (CComboBox *)GetDlgItem(IDC_COMBO_MODULE);

	//����ģ��
	if (m_ModuleInfoManager.LoadGameModuleInfo(m_ModuleInfoBuffer) == true)
	{
		//����ģ��
		tagGameModuleInfo GameModuleInfo;
		ZeroMemory(&GameModuleInfo,sizeof(GameModuleInfo));

		//���ñ���
		GameModuleInfo.wModuleID = MAX_SERVICE_ID;
		StringCchCopy(GameModuleInfo.szModuleName, CountArray(GameModuleInfo.szModuleName), TEXT("ȫ����Ϸ"));

		//����ģ��
		m_ModuleInfoBuffer.InsertModuleInfo(&GameModuleInfo);

		//��������
		WORD wIndex = 0;
		POSITION Position = m_ModuleInfoBuffer.m_GameModuleInfoMap.GetStartPosition();

		//ö��ģ��
		while (Position != NULL)
		{
			//��ȡ����
			WORD wModuleID = 0L;
			tagGameModuleInfo * pGameModuleInfo = NULL;
			m_ModuleInfoBuffer.m_GameModuleInfoMap.GetNextAssoc(Position, wModuleID, pGameModuleInfo);

			//�����б�
			ASSERT (pGameModuleInfo != NULL);
			if (pGameModuleInfo != NULL)
			{
				pComboBox->SetItemData(pComboBox->InsertString(wIndex++,pGameModuleInfo->szModuleName), (DWORD_PTR)pGameModuleInfo);
			}
		}
	}
}

//ѡ������
VOID CDlgServiceOption::SelectServiceItem(int nIndex)
{
	//��ȡָ��
	auto pComboBox = (CComboBox*)GetDlgItem(IDC_SERVICE_KIND);
	auto pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);	

	//���ñ���
	m_nSelectServiceItem=nIndex;
	tagServiceOption * pServiceOption=(tagServiceOption *)pListCtrl->GetItemData(m_nSelectServiceItem);
 	
	//��ȡ��ʶ
	TCHAR szItemText[16] = TEXT("");
	pListCtrl->GetItemText(m_nSelectServiceItem, 0, szItemText,CountArray(szItemText));

	//���ñ�ʶ
	SetServiceID(_ttoi(szItemText));		

	//���ÿؼ�	
	SetDlgItemText(IDC_SERVICE_NAME,pListCtrl->GetItemText(m_nSelectServiceItem,1));	
	SetDlgItemText(IDC_MAX_CONNECT,pListCtrl->GetItemText(m_nSelectServiceItem,3));
	SetDlgItemText(IDC_SERVICE_PORT,pListCtrl->GetItemText(m_nSelectServiceItem,4));	
	SetDlgItemText(IDC_SERVICE_ADDR,pListCtrl->GetItemText(m_nSelectServiceItem,5));	

	//��������
	for (INT_PTR i = 0; i < pComboBox->GetCount(); i++)
	{
		if (pComboBox->GetItemData(i) == pServiceOption->wServiceKindID)
		{
			pComboBox->SetCurSel(i);
			break;
		}
	}
}

//��ȡ·��
VOID CDlgServiceOption::GetDataFilePath(TCHAR szFilePath[], WORD wBufferCount)
{
	//��ȡ·��
	TCHAR szWorkDir[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szWorkDir, CountArray(szWorkDir));

	//�ļ�Ŀ¼
	TCHAR szServiceDir[MAX_PATH] = TEXT("");
	_sntprintf_s(szServiceDir, CountArray(szServiceDir), TEXT("%s\\ServiceOption"), szWorkDir);

	//����Ŀ¼
	if (!PathIsDirectory(szServiceDir)) CreateDirectory(szServiceDir, NULL);

	//����·��
	_sntprintf_s(szFilePath,wBufferCount,wBufferCount, TEXT("%s\\%s"), szServiceDir, m_szDataFileName);
}

//���ÿؼ�
VOID CDlgServiceOption::SetServiceToControl(tagServiceOption *pServiceOption,CListCtrl * pListCtrl,int nIndex)
{
	//��������
	CString strItemText;

	//����ID
	strItemText.Format(TEXT("%d"),pServiceOption->wServiceID);
	pListCtrl->SetItemText(nIndex,0,strItemText);

	//��������
	pListCtrl->SetItemText(nIndex,1,pServiceOption->szServiceName);

	//��������
	strItemText.Format(TEXT("%s"), pServiceOption->szServiceKindName[0]? pServiceOption->szServiceKindName:TEXT("Ĭ������"));
	pListCtrl->SetItemText(nIndex, 2, strItemText);

	//�������
	strItemText.Format(TEXT("%d"),pServiceOption->wMaxConnect);
	pListCtrl->SetItemText(nIndex,3,strItemText);

	//����˿�
	strItemText.Format(TEXT("%d"),pServiceOption->wServicePort);
	pListCtrl->SetItemText(nIndex,4,strItemText);

	//�����ַ
	strItemText.Format(TEXT("%s"),pServiceOption->szServiceDomain);
	pListCtrl->SetItemText(nIndex,5,strItemText);	

	
}

//�˳�����
VOID CDlgServiceOption::OnBnClickedQuit()
{
	EndDialog(IDCANCEL);
}

//�������
VOID CDlgServiceOption::OnBnClickedAddServiceInfo()
{
	//��������
	tagServiceOption ServiceOption;
	ZeroMemory(&ServiceOption,sizeof(ServiceOption));

	//��ȡ��ʶ
	if (GetServiceID(ServiceOption.wServiceID) == false)
	{
		return;
	}

	//��ȡ����		
	ServiceOption.wMaxConnect=GetDlgItemInt(IDC_MAX_CONNECT);
	ServiceOption.wServicePort=GetDlgItemInt(IDC_SERVICE_PORT);
	GetDlgItemText(IDC_SERVICE_NAME,ServiceOption.szServiceName,CountArray(ServiceOption.szServiceName));
	GetDlgItemText(IDC_SERVICE_ADDR,ServiceOption.szServiceDomain,CountArray(ServiceOption.szServiceDomain));	

	//��������
	auto pComboBox = (CComboBox*)GetDlgItem(IDC_SERVICE_KIND);
	if (pComboBox->GetCurSel() != -1)
	{
		ServiceOption.wServiceKindID = pComboBox->GetItemData(pComboBox->GetCurSel());
		pComboBox->GetWindowText(ServiceOption.szServiceKindName, CountArray(ServiceOption.szServiceKindName));		
	}	
	
	//�����ʶ
	if (m_bOptionModule==false && ServiceOption.wServiceID==0)
	{
		AfxMessageBox(TEXT("��������ȷ�ķ����ʶ��"));
		GetDlgItem(IDC_SERVICE_ID)->SetFocus();

		return;
	}

	//����˿�
	if (ServiceOption.wServicePort==0)
	{
		AfxMessageBox(TEXT("����˿ڲ���Ϊ0,���������룡"));
		GetDlgItem(IDC_SERVICE_PORT)->SetFocus();

		return;
	}

	//�����ַ
	if (ServiceOption.szServiceDomain[0]==0)
	{
		AfxMessageBox(TEXT("�����ַ����Ϊ��,���������룡"));
		GetDlgItem(IDC_SERVICE_ADDR)->SetFocus();

		return;
	}

	//��������
	TCHAR szItemText[16]=TEXT("");
	CListCtrl * pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);

	//�ظ��ж�
	for (INT nIndex=0;nIndex<pListCtrl->GetItemCount();++nIndex)
	{		
		//��ȡ��ʶ
		pListCtrl->GetItemText(nIndex,0,szItemText,CountArray(szItemText));

		//��ʶת��
		WORD wServiceID=_ttoi(szItemText);
		if (wServiceID ==ServiceOption.wServiceID)
		{
			AfxMessageBox(TEXT("��Ǹ,����ӵķ����ʶ�Ѵ��ڣ�"));

			return;
		}

		//��ȡ�˿�
		pListCtrl->GetItemText(nIndex,3,szItemText,CountArray(szItemText));

		//��ʶת��
		WORD wServicePort=_ttoi(szItemText);
		if (wServicePort==ServiceOption.wServicePort)
		{
			AfxMessageBox(TEXT("��Ǹ,����ӵķ���˿��г�ͻ��"));

			return;
		}
	}
	
	//����ṹ
	LVITEM lVItem;
	lVItem.mask = LVIF_TEXT;  
	lVItem.iItem=pListCtrl->GetItemCount();
	lVItem.iSubItem = 0; 
	lVItem.pszText = TEXT("");

	//�����б�
	int nInsertItem = pListCtrl->InsertItem(&lVItem);

	//��ȡ����
	tagServiceOption * pServiceOption=ActiveServiceOption();
	if (pServiceOption==NULL) return;

	//��������
	CopyMemory(pServiceOption,&ServiceOption,sizeof(tagServiceOption));

	//���ÿؼ�
	SetServiceToControl(pServiceOption,pListCtrl,nInsertItem);	
	pListCtrl->SetItemData(nInsertItem,(DWORD_PTR)pServiceOption);

	//���¿ؼ�
	pListCtrl->UpdateData();

	//��������
	SaveServiceOption();
	
	return;
}

//�޸�����
VOID CDlgServiceOption::OnBnClickedModifyServiceInfo()
{
	//��������
	tagServiceOption ServiceOption;
	ZeroMemory(&ServiceOption,sizeof(ServiceOption));

	//��ȡ��ʶ
	if (GetServiceID(ServiceOption.wServiceID) == false)
	{
		return;
	}

	//��ȡ����	
	ServiceOption.wMaxConnect=GetDlgItemInt(IDC_MAX_CONNECT);	
	ServiceOption.wServicePort=GetDlgItemInt(IDC_SERVICE_PORT);
	GetDlgItemText(IDC_SERVICE_NAME,ServiceOption.szServiceName,CountArray(ServiceOption.szServiceName));
	GetDlgItemText(IDC_SERVICE_ADDR,ServiceOption.szServiceDomain,CountArray(ServiceOption.szServiceDomain));

	//��������
	auto pComboBox = (CComboBox*)GetDlgItem(IDC_SERVICE_KIND);
	if (pComboBox->GetCurSel() != -1)
	{
		ServiceOption.wServiceKindID = pComboBox->GetItemData(pComboBox->GetCurSel());
		pComboBox->GetWindowText(ServiceOption.szServiceKindName, CountArray(ServiceOption.szServiceKindName));
	}
	
	//�����ʶ
	if (m_bOptionModule == false && ServiceOption.wServiceID==0)
	{
		AfxMessageBox(TEXT("��������ȷ�ķ����ʶ��"));
		GetDlgItem(IDC_SERVICE_ID)->SetFocus();

		return;
	}

	//����˿�
	if (ServiceOption.wServicePort==0)
	{
		AfxMessageBox(TEXT("����˿ڲ���Ϊ0,���������룡"));
		GetDlgItem(IDC_SERVICE_PORT)->SetFocus();

		return;
	}

	//�����ַ
	if (ServiceOption.szServiceDomain[0]==0)
	{
		AfxMessageBox(TEXT("�����ַ����Ϊ��,���������룡"));
		GetDlgItem(IDC_SERVICE_ADDR)->SetFocus();

		return;
	}	

	//��������
	TCHAR szItemText[16]=TEXT("");
	CListCtrl * pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);

	//�ظ��ж�
	for (INT nIndex=0;nIndex<pListCtrl->GetItemCount();++nIndex)
	{		
		if (m_nSelectServiceItem==nIndex) continue;

		//��ȡ��ʶ
		pListCtrl->GetItemText(nIndex,0,szItemText,CountArray(szItemText));

		//��ʶת��
		WORD wServiceID =_ttoi(szItemText);
		if (wServiceID ==ServiceOption.wServiceID)
		{
			AfxMessageBox(TEXT("��Ǹ,����ӵķ����ʶ�Ѵ��ڣ�"));

			return;
		}

		//��ȡ�˿�
		pListCtrl->GetItemText(nIndex,3,szItemText,CountArray(szItemText));

		//��ʶת��
		WORD wServicePort=_ttoi(szItemText);
		if (wServicePort==ServiceOption.wServicePort)
		{
			AfxMessageBox(TEXT("��Ǹ,����ӵķ���˿��г�ͻ��"));

			return;
		}
	}

	//��ȡָ��
	if (m_nSelectServiceItem==-1) return;

	//��������
	TCHAR szUpdateItem[LEN_MD5]=TEXT("");

	//���ÿؼ�
	SetServiceToControl(&ServiceOption,pListCtrl,m_nSelectServiceItem);

	//��������
	tagServiceOption * pServiceOption=(tagServiceOption *)pListCtrl->GetItemData(m_nSelectServiceItem);
	CopyMemory(pServiceOption,&ServiceOption,sizeof(ServiceOption));
	
	//���¿ؼ�
	pListCtrl->UpdateData();

	//��������
	SaveServiceOption();

	return;
}

//ɾ������
VOID CDlgServiceOption::OnBnClickedDeleteServiceInfo()
{
	//����У��
	if (m_nSelectServiceItem==-1) return;

	//��ȡָ��
	CListCtrl * pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);

	//��ȡ����
	tagServiceOption * pServiceOption=(tagServiceOption *)pListCtrl->GetItemData(m_nSelectServiceItem);

	//ɾ������
	FreeServiceOption(pServiceOption);
	pListCtrl->DeleteItem(m_nSelectServiceItem);

	//���ñ���
	m_nSelectServiceItem=-1;

	//��������
	SaveServiceOption();

	return;
}

//��������
VOID CDlgServiceOption::OnHdnItemclickListServiceInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->iItem!=-1) SelectServiceItem(pNMLV->iItem);
	*pResult = 0;
}

//˫������
VOID CDlgServiceOption::OnHdnItemDblclkListServiceInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);	

	//���ñ���
	*pResult = 0;
	m_nSelectServiceItem=pNMItemActivate->iItem;

	//����У��
	if (m_nSelectServiceItem==-1) return;

	//��ȡ����
	CListCtrl * pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);
	tagServiceOption * pServiceOption=(tagServiceOption *)pListCtrl->GetItemData(m_nSelectServiceItem);

	//ɾ������
	CopyMemory(&m_ServiceOption,pServiceOption,sizeof(m_ServiceOption));

	//�رմ���
	EndDialog(IDOK);
}

//���̰���
VOID CDlgServiceOption::OnLvnItemKeydownListServiceInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	//��������
	int nItem=-1;
	POSITION pos;
	CListCtrl * pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);

	switch(pLVKeyDow->wVKey) 
	{
	case VK_DOWN:
		{
			pos=pListCtrl->GetFirstSelectedItemPosition();
			while (pos) 
			{
				//��ȡ����
				nItem=pListCtrl->GetNextSelectedItem(pos);
				if(nItem==-1) break;

				//��������
				nItem = __min(nItem+1,pListCtrl->GetItemCount()-1);
				SelectServiceItem(nItem);
				break;
			}

			break;
		}
	case VK_UP:
		{
			pos=pListCtrl->GetFirstSelectedItemPosition();
			while (pos) 
			{
				//��ȡ����
				nItem=pListCtrl->GetNextSelectedItem(pos);
				if (nItem==-1) break;

				//��������
				nItem = __max(nItem-1,0);
				SelectServiceItem(nItem);
				break;
			}
			break;
		}
	}

	*pResult = 0;
}

//////////////////////////////////////////////////////////////////////////////////
