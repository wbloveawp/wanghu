#include "StdAfx.h"
#include "Resource.h"
#include "DataFile.h"
#include "DlgServiceExOption.h"

//////////////////////////////////////////////////////////////////////////////////
//��Ϣӳ��
BEGIN_MESSAGE_MAP(CDlgServiceOptionEx, CDialog)
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
CDlgServiceOptionEx::CDlgServiceOptionEx() : CDialog(IDD_SERVICE_OPTION)
{
	//���ñ���
	m_szDataFileName[0] = 0;
	m_nSelectServiceItem=-1;
	ZeroMemory(&m_ServiceOption,sizeof(m_ServiceOption));

	return;
}

//��������
CDlgServiceOptionEx::~CDlgServiceOptionEx()
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
BOOL CDlgServiceOptionEx::OnInitDialog()
{
	__super::OnInitDialog();

	//��ʼ�б�
	InitControl();

	//��ȡ����
	LoadServiceOption();

	return TRUE;
}

//��Ϣ����
BOOL CDlgServiceOptionEx::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message==WM_KEYDOWN)&&((pMsg->wParam==VK_ESCAPE)||(pMsg->wParam==VK_RETURN)))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//�ؼ���
VOID CDlgServiceOptionEx::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);	
}

//�ļ�����
VOID CDlgServiceOptionEx::SetDataFileName(LPCTSTR pszFileName)
{
	lstrcpyn(m_szDataFileName, pszFileName,CountArray(m_szDataFileName));
}

//��������
bool CDlgServiceOptionEx::LoadServiceOption()
{
	//��ȡ·��
	TCHAR szWorkDir[MAX_PATH]=TEXT("");
	CAssistant::GetWorkDirectory(szWorkDir,CountArray(szWorkDir));

	//����·��
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf_s(szIniFile,CountArray(szIniFile),TEXT("%s\\%s"),szWorkDir,m_szDataFileName);

	//��ȡ����
	CDataFile DataFile;
	if (DataFile.OpenDataFile(szIniFile)==FALSE)  return false;

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
bool CDlgServiceOptionEx::SaveServiceOption()
{
	//��ȡ·��
	TCHAR szWorkDir[MAX_PATH]=TEXT("");
	CAssistant::GetWorkDirectory(szWorkDir,CountArray(szWorkDir));

	//����·��
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf_s(szIniFile, CountArray(szIniFile), TEXT("%s\\%s"), szWorkDir, m_szDataFileName);

	//�ļ�
	CDataFile DataFile;
	if (DataFile.CreateDataFile(szIniFile)==FALSE)
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
tagServiceOption * CDlgServiceOptionEx::ActiveServiceOption()
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
void CDlgServiceOptionEx::FreeServiceOption(tagServiceOption * pServiceOption)
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

//��ʼ�б�
VOID CDlgServiceOptionEx::InitControl()
{
	//��ȡָ��
	CListCtrl * pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);
	pListCtrl->SetExtendedStyle(pListCtrl->GetExtendedStyle()|LVS_EX_FULLROWSELECT);

	//����ͷ��
	pListCtrl->InsertColumn(0,TEXT("�����ʶ"),LVCFMT_LEFT,60);	
	pListCtrl->InsertColumn(1,TEXT("��������"),LVCFMT_LEFT,120);
	pListCtrl->InsertColumn(2,TEXT("����վ��"), LVCFMT_LEFT,60);
	pListCtrl->InsertColumn(3,TEXT("�������"),LVCFMT_LEFT,70);
	pListCtrl->InsertColumn(4,TEXT("����˿�"),LVCFMT_LEFT,70);
	pListCtrl->InsertColumn(5,TEXT("�����ַ"),LVCFMT_LEFT,80);
}

//ѡ������
VOID CDlgServiceOptionEx::SelectServiceItem(int nIndex)
{
	//��ȡָ��
	CListCtrl * pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);

	//���ñ���
	m_nSelectServiceItem=nIndex;
	tagServiceOption * pServiceOption=(tagServiceOption *)pListCtrl->GetItemData(m_nSelectServiceItem);

	//���ÿؼ�
	SetDlgItemText(IDC_SERVICE_ID,pListCtrl->GetItemText(m_nSelectServiceItem,0));	
	SetDlgItemText(IDC_SERVICE_NAME,pListCtrl->GetItemText(m_nSelectServiceItem,1));
	SetDlgItemText(IDC_SERVICE_STATION,pListCtrl->GetItemText(m_nSelectServiceItem, 2));
	SetDlgItemText(IDC_MAX_CONNECT,pListCtrl->GetItemText(m_nSelectServiceItem,3));
	SetDlgItemText(IDC_SERVICE_PORT,pListCtrl->GetItemText(m_nSelectServiceItem,4));	
	SetDlgItemText(IDC_SERVICE_ADDR,pListCtrl->GetItemText(m_nSelectServiceItem,5));	
}

//���ÿؼ�
VOID CDlgServiceOptionEx::SetServiceToControl(tagServiceOption *pServiceOption,CListCtrl * pListCtrl,int nIndex)
{
	//��������
    CString strItemText;

	//����ID
	strItemText.Format(TEXT("%d"),pServiceOption->wServiceID);
	pListCtrl->SetItemText(nIndex,0,strItemText);

	//��������
	pListCtrl->SetItemText(nIndex,1,pServiceOption->szServiceName);

	//����վ��
	strItemText.Format(TEXT("%d"), pServiceOption->dwStationID);
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
VOID CDlgServiceOptionEx::OnBnClickedQuit()
{
	EndDialog(IDCANCEL);
}

//�������
VOID CDlgServiceOptionEx::OnBnClickedAddServiceInfo()
{
	//��������
	tagServiceOption ServiceOption;
	ZeroMemory(&ServiceOption,sizeof(ServiceOption));

	//��ȡ����	
	ServiceOption.wServiceID=GetDlgItemInt(IDC_SERVICE_ID);	
	ServiceOption.wMaxConnect=GetDlgItemInt(IDC_MAX_CONNECT);
	ServiceOption.wServicePort=GetDlgItemInt(IDC_SERVICE_PORT);
	ServiceOption.dwStationID = GetDlgItemInt(IDC_SERVICE_STATION);
	GetDlgItemText(IDC_SERVICE_NAME,ServiceOption.szServiceName,CountArray(ServiceOption.szServiceName));
	GetDlgItemText(IDC_SERVICE_ADDR,ServiceOption.szServiceDomain,CountArray(ServiceOption.szServiceDomain));
	
	//�����ʶ
	if (ServiceOption.wServiceID==0)
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
VOID CDlgServiceOptionEx::OnBnClickedModifyServiceInfo()
{
	//��������
	tagServiceOption ServiceOption;
	ZeroMemory(&ServiceOption,sizeof(ServiceOption));

	//��ȡ����	
	ServiceOption.wServiceID=GetDlgItemInt(IDC_SERVICE_ID);	
	ServiceOption.wMaxConnect=GetDlgItemInt(IDC_MAX_CONNECT);	
	ServiceOption.wServicePort=GetDlgItemInt(IDC_SERVICE_PORT);
	ServiceOption.dwStationID=GetDlgItemInt(IDC_SERVICE_STATION);
	GetDlgItemText(IDC_SERVICE_NAME,ServiceOption.szServiceName,CountArray(ServiceOption.szServiceName));
	GetDlgItemText(IDC_SERVICE_ADDR,ServiceOption.szServiceDomain,CountArray(ServiceOption.szServiceDomain));
	
	//�����ʶ
	if (ServiceOption.wServiceID==0)
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
VOID CDlgServiceOptionEx::OnBnClickedDeleteServiceInfo()
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
VOID CDlgServiceOptionEx::OnHdnItemclickListServiceInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->iItem!=-1) SelectServiceItem(pNMLV->iItem);
	*pResult = 0;
}

//˫������
VOID CDlgServiceOptionEx::OnHdnItemDblclkListServiceInfo(NMHDR *pNMHDR, LRESULT *pResult)
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
VOID CDlgServiceOptionEx::OnLvnItemKeydownListServiceInfo(NMHDR *pNMHDR, LRESULT *pResult)
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
