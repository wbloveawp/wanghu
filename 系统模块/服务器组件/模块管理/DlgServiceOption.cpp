#include "StdAfx.h"
#include "Resource.h"
#include "DataFile.h"
#include "DlgServiceOption.h"

//////////////////////////////////////////////////////////////////////////////////
//消息映射
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

//构造函数
CDlgServiceOption::CDlgServiceOption() : CDialog(IDD_SERVICE_OPTION)
{
	//设置变量
	m_bOptionModule=false;
	m_szDataFileName[0]=0;
	m_nSelectServiceItem=-1;
	ZeroMemory(&m_ServiceOption,sizeof(m_ServiceOption));

	return;
}

//析构函数
CDlgServiceOption::~CDlgServiceOption()
{
	//释放对象
	m_ServiceOptionActive.Append(m_ServiceOptionBuffer);

	//释放对象
	for (INT_PTR i=0; i<m_ServiceOptionActive.GetCount(); i++)
	{
		SafeDelete(m_ServiceOptionActive[i]);
	}

	//移除对象
	m_ServiceOptionActive.RemoveAll();
	m_ServiceOptionBuffer.RemoveAll();
}

//初始化函数
BOOL CDlgServiceOption::OnInitDialog()
{
	__super::OnInitDialog();

	//初始列表
	InitControl();

	//读取配置
	LoadServiceOption();
	
	//设置控件
	GetDlgItem(IDC_SERVICE_ID)->ShowWindow(m_bOptionModule?SW_HIDE:SW_SHOW);
	GetDlgItem(IDC_COMBO_MODULE)->ShowWindow(m_bOptionModule?SW_SHOW:SW_HIDE);

	//加载模块
	if (m_bOptionModule == true) LoadDBModuleItem();

	return TRUE;
}

//消息解释
BOOL CDlgServiceOption::PreTranslateMessage(MSG * pMsg)
{
	//按键过滤
	if ((pMsg->message==WM_KEYDOWN)&&((pMsg->wParam==VK_ESCAPE)||(pMsg->wParam==VK_RETURN)))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//控件绑定
VOID CDlgServiceOption::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);	
}

//设置类型
VOID CDlgServiceOption::SetServiceKind(tagServiceKind ServiceKind[], WORD wKindCount)
{
	//添加类型
	for (int i = 0; i < wKindCount; i++)
	{
		m_ServiceKindArray.Add(ServiceKind[i]);
	}
}

//设置标识
VOID CDlgServiceOption::SetOptionModule(bool bOptionModule)
{
	m_bOptionModule = bOptionModule;
}

//文件名称
VOID CDlgServiceOption::SetDataFileName(LPCTSTR pszFileName)
{
	StringCchCopy(m_szDataFileName, CountArray(m_szDataFileName), pszFileName);
}

//获取配置
tagServiceOption * CDlgServiceOption::GetServiceOption()
{ 
	return &m_ServiceOption; 
}

//获取配置
tagServiceOption * CDlgServiceOption::GetServiceOption(WORD wServiceID)
{
	//构造路径
	TCHAR szDataFile[MAX_PATH] = TEXT("");
	GetDataFilePath(szDataFile, CountArray(szDataFile));

	//读取数据
	CDataFile DataFile;
	if (DataFile.OpenDataFile(szDataFile) == FALSE)
	{
		return NULL;
	}

	//查找服务
	WORD wItemCount = DataFile.GetItemCount();
	for (WORD wIndex = 0; wIndex < wItemCount; ++wIndex)
	{
		//读取数据
		if (DataFile.ReadDataItem(&m_ServiceOption, sizeof(m_ServiceOption), wIndex)==0)
		{
			break;
		}

		//校验服务
		if (m_ServiceOption.wServiceID == wServiceID)
		{
			return &m_ServiceOption;
		}
	}	

	return NULL;
}

//加载配置
bool CDlgServiceOption::LoadServiceOption()
{
	//构造路径
	TCHAR szDataFile[MAX_PATH] = TEXT("");
	GetDataFilePath(szDataFile, CountArray(szDataFile));

	//读取数据
	CDataFile DataFile;
	if (DataFile.OpenDataFile(szDataFile)==FALSE) return false;

	//变量定义
	WORD wItemCount=DataFile.GetItemCount();
	if (wItemCount>0)
	{
		//获取指针
		CListCtrl * pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);

		for (WORD wIndex=0;wIndex<wItemCount;++wIndex)
		{
			//读取子项
			tagServiceOption * pServiceOption=ActiveServiceOption();
			if (pServiceOption==NULL) return false;

			//读取数据
			DataFile.ReadDataItem(pServiceOption,sizeof(tagServiceOption),wIndex);			

			//构造结构
			LVITEM lVItem;
			lVItem.mask=LVIF_TEXT;  
			lVItem.iItem=pListCtrl->GetItemCount();
			lVItem.iSubItem=0; 
			lVItem.pszText=TEXT("");

			//插入列表
			int nInsertItem=pListCtrl->InsertItem(&lVItem);

			//设置子项
			SetServiceToControl(pServiceOption,pListCtrl,nInsertItem);

			//设置数据
			pListCtrl->SetItemData(nInsertItem,(DWORD_PTR)pServiceOption);
		}

		//更新数据
		pListCtrl->UpdateData();
	}	
	
	return true;
}

//保存配置
bool CDlgServiceOption::SaveServiceOption()
{
	//构造路径
	TCHAR szDataFile[MAX_PATH] = TEXT("");	
	GetDataFilePath(szDataFile, CountArray(szDataFile));

	//文件
	CDataFile DataFile;
	if (DataFile.CreateDataFile(szDataFile)==FALSE)
	{
		AfxMessageBox(TEXT("数据文件创建失败"),MB_ICONQUESTION);

		return false;
	}

	//获取指针
	CListCtrl * pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);

	if (pListCtrl->GetItemCount()>0)
	{
		//变量定义
		tagServiceOption *pServiceOption=NULL;

		for (int nIndex=0;nIndex<pListCtrl->GetItemCount();++nIndex)
		{
			//获取文本
			pServiceOption=(tagServiceOption *)pListCtrl->GetItemData(nIndex);			

			//添加数据
			DataFile.WriteDataItem(pServiceOption,sizeof(tagServiceOption));
		}
	}

	return true;
}

//激活对象
tagServiceOption * CDlgServiceOption::ActiveServiceOption()
{
	//变量定义
	tagServiceOption *pServiceOption=NULL;
	INT nItemCount=m_ServiceOptionBuffer.GetCount();

	//查找缓存
	if (nItemCount>0)
	{		
		pServiceOption=m_ServiceOptionBuffer[nItemCount-1];
		m_ServiceOptionBuffer.RemoveAt(nItemCount-1);
		m_ServiceOptionActive.Add(pServiceOption);
		return pServiceOption;
	}

	try
	{
		//创建对象
		pServiceOption=new tagServiceOption;
		if (pServiceOption==NULL) throw TEXT("对象创建失败！");

		//初始数据
		m_ServiceOptionActive.Add(pServiceOption);
		ZeroMemory(pServiceOption,sizeof(tagServiceOption));		
	}
	catch (...) 
	{ 
		ASSERT(FALSE); 
	}

	return pServiceOption;
}

//释放对象
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

	//加入缓存
	m_ServiceOptionBuffer.Add(pServiceOption);
}

//获取标识
bool CDlgServiceOption::GetServiceID(WORD & wServiceID)
{
	//服务标识
	if (m_bOptionModule == false)
	{
		wServiceID=GetDlgItemInt(IDC_SERVICE_ID);

		return true;
	}

	//获取控件		
	CComboBox * pComboBox = (CComboBox *)GetDlgItem(IDC_COMBO_MODULE);
	if (pComboBox->GetCurSel() == -1)
	{
		AfxMessageBox(TEXT("服务模块不能为空！"));
		GetDlgItem(IDC_COMBO_MODULE)->SetFocus();

		return false;
	}

	//获取模型
	tagGameModuleInfo * pGameModuleInfo = (tagGameModuleInfo *)pComboBox->GetItemData(pComboBox->GetCurSel());

	//设置标识
	wServiceID = pGameModuleInfo->wModuleID;	

	return true;
}

//设置标识
bool CDlgServiceOption::SetServiceID(WORD wServiceID)
{
	//设置标识
	if (m_bOptionModule == false)
	{
		SetDlgItemInt(IDC_SERVICE_ID, wServiceID);

		return true;
	}

	//变量定义
	tagGameModuleInfo * pGameModuleInfo = NULL;
	CComboBox * pComboBox = (CComboBox *)GetDlgItem(IDC_COMBO_MODULE);

	//查找模块
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

//初始列表
VOID CDlgServiceOption::InitControl()
{
	//获取指针
	auto pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);
	pListCtrl->SetExtendedStyle(pListCtrl->GetExtendedStyle()|LVS_EX_FULLROWSELECT);

	//插入头部
	pListCtrl->InsertColumn(0,TEXT("服务标识"),LVCFMT_LEFT,60);	
	pListCtrl->InsertColumn(1,TEXT("服务名称"),LVCFMT_LEFT,100);
	pListCtrl->InsertColumn(2, TEXT("服务类型"), LVCFMT_LEFT, 100);
	pListCtrl->InsertColumn(3,TEXT("最大连接"),LVCFMT_LEFT,70);
	pListCtrl->InsertColumn(4,TEXT("服务端口"),LVCFMT_LEFT,70);
	pListCtrl->InsertColumn(5,TEXT("服务地址"),LVCFMT_LEFT,80);	

	//获取控件
	auto pComboBox = (CComboBox*)GetDlgItem(IDC_SERVICE_KIND);

	//添加类型
	for (INT_PTR i = 0; i < m_ServiceKindArray.GetCount(); i++)
	{
		pComboBox->SetItemData(pComboBox->InsertString(i,m_ServiceKindArray[i].szServiceKindName), m_ServiceKindArray[i].wServiceKindID);
	}

	//默认选择
	if (pComboBox->GetCurSel() == -1) pComboBox->SetCurSel(0);
}

//加载列表
VOID CDlgServiceOption::LoadDBModuleItem()
{
	//获取模块
	CComboBox * pComboBox = (CComboBox *)GetDlgItem(IDC_COMBO_MODULE);

	//加载模块
	if (m_ModuleInfoManager.LoadGameModuleInfo(m_ModuleInfoBuffer) == true)
	{
		//构造模块
		tagGameModuleInfo GameModuleInfo;
		ZeroMemory(&GameModuleInfo,sizeof(GameModuleInfo));

		//设置变量
		GameModuleInfo.wModuleID = MAX_SERVICE_ID;
		StringCchCopy(GameModuleInfo.szModuleName, CountArray(GameModuleInfo.szModuleName), TEXT("全部游戏"));

		//插入模块
		m_ModuleInfoBuffer.InsertModuleInfo(&GameModuleInfo);

		//变量定义
		WORD wIndex = 0;
		POSITION Position = m_ModuleInfoBuffer.m_GameModuleInfoMap.GetStartPosition();

		//枚举模块
		while (Position != NULL)
		{
			//获取对象
			WORD wModuleID = 0L;
			tagGameModuleInfo * pGameModuleInfo = NULL;
			m_ModuleInfoBuffer.m_GameModuleInfoMap.GetNextAssoc(Position, wModuleID, pGameModuleInfo);

			//插入列表
			ASSERT (pGameModuleInfo != NULL);
			if (pGameModuleInfo != NULL)
			{
				pComboBox->SetItemData(pComboBox->InsertString(wIndex++,pGameModuleInfo->szModuleName), (DWORD_PTR)pGameModuleInfo);
			}
		}
	}
}

//选择子项
VOID CDlgServiceOption::SelectServiceItem(int nIndex)
{
	//获取指针
	auto pComboBox = (CComboBox*)GetDlgItem(IDC_SERVICE_KIND);
	auto pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);	

	//设置变量
	m_nSelectServiceItem=nIndex;
	tagServiceOption * pServiceOption=(tagServiceOption *)pListCtrl->GetItemData(m_nSelectServiceItem);
 	
	//获取标识
	TCHAR szItemText[16] = TEXT("");
	pListCtrl->GetItemText(m_nSelectServiceItem, 0, szItemText,CountArray(szItemText));

	//设置标识
	SetServiceID(_ttoi(szItemText));		

	//设置控件	
	SetDlgItemText(IDC_SERVICE_NAME,pListCtrl->GetItemText(m_nSelectServiceItem,1));	
	SetDlgItemText(IDC_MAX_CONNECT,pListCtrl->GetItemText(m_nSelectServiceItem,3));
	SetDlgItemText(IDC_SERVICE_PORT,pListCtrl->GetItemText(m_nSelectServiceItem,4));	
	SetDlgItemText(IDC_SERVICE_ADDR,pListCtrl->GetItemText(m_nSelectServiceItem,5));	

	//服务类型
	for (INT_PTR i = 0; i < pComboBox->GetCount(); i++)
	{
		if (pComboBox->GetItemData(i) == pServiceOption->wServiceKindID)
		{
			pComboBox->SetCurSel(i);
			break;
		}
	}
}

//获取路径
VOID CDlgServiceOption::GetDataFilePath(TCHAR szFilePath[], WORD wBufferCount)
{
	//获取路径
	TCHAR szWorkDir[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szWorkDir, CountArray(szWorkDir));

	//文件目录
	TCHAR szServiceDir[MAX_PATH] = TEXT("");
	_sntprintf_s(szServiceDir, CountArray(szServiceDir), TEXT("%s\\ServiceOption"), szWorkDir);

	//创建目录
	if (!PathIsDirectory(szServiceDir)) CreateDirectory(szServiceDir, NULL);

	//构造路径
	_sntprintf_s(szFilePath,wBufferCount,wBufferCount, TEXT("%s\\%s"), szServiceDir, m_szDataFileName);
}

//设置控件
VOID CDlgServiceOption::SetServiceToControl(tagServiceOption *pServiceOption,CListCtrl * pListCtrl,int nIndex)
{
	//变量定义
	CString strItemText;

	//服务ID
	strItemText.Format(TEXT("%d"),pServiceOption->wServiceID);
	pListCtrl->SetItemText(nIndex,0,strItemText);

	//服务名称
	pListCtrl->SetItemText(nIndex,1,pServiceOption->szServiceName);

	//服务类型
	strItemText.Format(TEXT("%s"), pServiceOption->szServiceKindName[0]? pServiceOption->szServiceKindName:TEXT("默认类型"));
	pListCtrl->SetItemText(nIndex, 2, strItemText);

	//最大连接
	strItemText.Format(TEXT("%d"),pServiceOption->wMaxConnect);
	pListCtrl->SetItemText(nIndex,3,strItemText);

	//服务端口
	strItemText.Format(TEXT("%d"),pServiceOption->wServicePort);
	pListCtrl->SetItemText(nIndex,4,strItemText);

	//服务地址
	strItemText.Format(TEXT("%s"),pServiceOption->szServiceDomain);
	pListCtrl->SetItemText(nIndex,5,strItemText);	

	
}

//退出窗口
VOID CDlgServiceOption::OnBnClickedQuit()
{
	EndDialog(IDCANCEL);
}

//添加数据
VOID CDlgServiceOption::OnBnClickedAddServiceInfo()
{
	//变量定义
	tagServiceOption ServiceOption;
	ZeroMemory(&ServiceOption,sizeof(ServiceOption));

	//获取标识
	if (GetServiceID(ServiceOption.wServiceID) == false)
	{
		return;
	}

	//获取数据		
	ServiceOption.wMaxConnect=GetDlgItemInt(IDC_MAX_CONNECT);
	ServiceOption.wServicePort=GetDlgItemInt(IDC_SERVICE_PORT);
	GetDlgItemText(IDC_SERVICE_NAME,ServiceOption.szServiceName,CountArray(ServiceOption.szServiceName));
	GetDlgItemText(IDC_SERVICE_ADDR,ServiceOption.szServiceDomain,CountArray(ServiceOption.szServiceDomain));	

	//服务类型
	auto pComboBox = (CComboBox*)GetDlgItem(IDC_SERVICE_KIND);
	if (pComboBox->GetCurSel() != -1)
	{
		ServiceOption.wServiceKindID = pComboBox->GetItemData(pComboBox->GetCurSel());
		pComboBox->GetWindowText(ServiceOption.szServiceKindName, CountArray(ServiceOption.szServiceKindName));		
	}	
	
	//服务标识
	if (m_bOptionModule==false && ServiceOption.wServiceID==0)
	{
		AfxMessageBox(TEXT("请输入正确的服务标识！"));
		GetDlgItem(IDC_SERVICE_ID)->SetFocus();

		return;
	}

	//服务端口
	if (ServiceOption.wServicePort==0)
	{
		AfxMessageBox(TEXT("服务端口不能为0,请重新输入！"));
		GetDlgItem(IDC_SERVICE_PORT)->SetFocus();

		return;
	}

	//服务地址
	if (ServiceOption.szServiceDomain[0]==0)
	{
		AfxMessageBox(TEXT("服务地址不能为空,请重新输入！"));
		GetDlgItem(IDC_SERVICE_ADDR)->SetFocus();

		return;
	}

	//变量定义
	TCHAR szItemText[16]=TEXT("");
	CListCtrl * pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);

	//重复判断
	for (INT nIndex=0;nIndex<pListCtrl->GetItemCount();++nIndex)
	{		
		//获取标识
		pListCtrl->GetItemText(nIndex,0,szItemText,CountArray(szItemText));

		//标识转换
		WORD wServiceID=_ttoi(szItemText);
		if (wServiceID ==ServiceOption.wServiceID)
		{
			AfxMessageBox(TEXT("抱歉,您添加的服务标识已存在！"));

			return;
		}

		//获取端口
		pListCtrl->GetItemText(nIndex,3,szItemText,CountArray(szItemText));

		//标识转换
		WORD wServicePort=_ttoi(szItemText);
		if (wServicePort==ServiceOption.wServicePort)
		{
			AfxMessageBox(TEXT("抱歉,您添加的服务端口有冲突！"));

			return;
		}
	}
	
	//构造结构
	LVITEM lVItem;
	lVItem.mask = LVIF_TEXT;  
	lVItem.iItem=pListCtrl->GetItemCount();
	lVItem.iSubItem = 0; 
	lVItem.pszText = TEXT("");

	//插入列表
	int nInsertItem = pListCtrl->InsertItem(&lVItem);

	//读取子项
	tagServiceOption * pServiceOption=ActiveServiceOption();
	if (pServiceOption==NULL) return;

	//拷贝数据
	CopyMemory(pServiceOption,&ServiceOption,sizeof(tagServiceOption));

	//设置控件
	SetServiceToControl(pServiceOption,pListCtrl,nInsertItem);	
	pListCtrl->SetItemData(nInsertItem,(DWORD_PTR)pServiceOption);

	//更新控件
	pListCtrl->UpdateData();

	//保存配置
	SaveServiceOption();
	
	return;
}

//修改数据
VOID CDlgServiceOption::OnBnClickedModifyServiceInfo()
{
	//变量定义
	tagServiceOption ServiceOption;
	ZeroMemory(&ServiceOption,sizeof(ServiceOption));

	//获取标识
	if (GetServiceID(ServiceOption.wServiceID) == false)
	{
		return;
	}

	//获取数据	
	ServiceOption.wMaxConnect=GetDlgItemInt(IDC_MAX_CONNECT);	
	ServiceOption.wServicePort=GetDlgItemInt(IDC_SERVICE_PORT);
	GetDlgItemText(IDC_SERVICE_NAME,ServiceOption.szServiceName,CountArray(ServiceOption.szServiceName));
	GetDlgItemText(IDC_SERVICE_ADDR,ServiceOption.szServiceDomain,CountArray(ServiceOption.szServiceDomain));

	//服务类型
	auto pComboBox = (CComboBox*)GetDlgItem(IDC_SERVICE_KIND);
	if (pComboBox->GetCurSel() != -1)
	{
		ServiceOption.wServiceKindID = pComboBox->GetItemData(pComboBox->GetCurSel());
		pComboBox->GetWindowText(ServiceOption.szServiceKindName, CountArray(ServiceOption.szServiceKindName));
	}
	
	//服务标识
	if (m_bOptionModule == false && ServiceOption.wServiceID==0)
	{
		AfxMessageBox(TEXT("请输入正确的服务标识！"));
		GetDlgItem(IDC_SERVICE_ID)->SetFocus();

		return;
	}

	//服务端口
	if (ServiceOption.wServicePort==0)
	{
		AfxMessageBox(TEXT("服务端口不能为0,请重新输入！"));
		GetDlgItem(IDC_SERVICE_PORT)->SetFocus();

		return;
	}

	//服务地址
	if (ServiceOption.szServiceDomain[0]==0)
	{
		AfxMessageBox(TEXT("服务地址不能为空,请重新输入！"));
		GetDlgItem(IDC_SERVICE_ADDR)->SetFocus();

		return;
	}	

	//变量定义
	TCHAR szItemText[16]=TEXT("");
	CListCtrl * pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);

	//重复判断
	for (INT nIndex=0;nIndex<pListCtrl->GetItemCount();++nIndex)
	{		
		if (m_nSelectServiceItem==nIndex) continue;

		//获取标识
		pListCtrl->GetItemText(nIndex,0,szItemText,CountArray(szItemText));

		//标识转换
		WORD wServiceID =_ttoi(szItemText);
		if (wServiceID ==ServiceOption.wServiceID)
		{
			AfxMessageBox(TEXT("抱歉,您添加的服务标识已存在！"));

			return;
		}

		//获取端口
		pListCtrl->GetItemText(nIndex,3,szItemText,CountArray(szItemText));

		//标识转换
		WORD wServicePort=_ttoi(szItemText);
		if (wServicePort==ServiceOption.wServicePort)
		{
			AfxMessageBox(TEXT("抱歉,您添加的服务端口有冲突！"));

			return;
		}
	}

	//获取指针
	if (m_nSelectServiceItem==-1) return;

	//变量定义
	TCHAR szUpdateItem[LEN_MD5]=TEXT("");

	//设置控件
	SetServiceToControl(&ServiceOption,pListCtrl,m_nSelectServiceItem);

	//拷贝数据
	tagServiceOption * pServiceOption=(tagServiceOption *)pListCtrl->GetItemData(m_nSelectServiceItem);
	CopyMemory(pServiceOption,&ServiceOption,sizeof(ServiceOption));
	
	//更新控件
	pListCtrl->UpdateData();

	//保存配置
	SaveServiceOption();

	return;
}

//删除数据
VOID CDlgServiceOption::OnBnClickedDeleteServiceInfo()
{
	//子项校验
	if (m_nSelectServiceItem==-1) return;

	//获取指针
	CListCtrl * pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);

	//提取数据
	tagServiceOption * pServiceOption=(tagServiceOption *)pListCtrl->GetItemData(m_nSelectServiceItem);

	//删除子项
	FreeServiceOption(pServiceOption);
	pListCtrl->DeleteItem(m_nSelectServiceItem);

	//设置变量
	m_nSelectServiceItem=-1;

	//保存配置
	SaveServiceOption();

	return;
}

//单击子项
VOID CDlgServiceOption::OnHdnItemclickListServiceInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->iItem!=-1) SelectServiceItem(pNMLV->iItem);
	*pResult = 0;
}

//双击子项
VOID CDlgServiceOption::OnHdnItemDblclkListServiceInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);	

	//设置变量
	*pResult = 0;
	m_nSelectServiceItem=pNMItemActivate->iItem;

	//子项校验
	if (m_nSelectServiceItem==-1) return;

	//提取数据
	CListCtrl * pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);
	tagServiceOption * pServiceOption=(tagServiceOption *)pListCtrl->GetItemData(m_nSelectServiceItem);

	//删除子项
	CopyMemory(&m_ServiceOption,pServiceOption,sizeof(m_ServiceOption));

	//关闭窗口
	EndDialog(IDOK);
}

//键盘按下
VOID CDlgServiceOption::OnLvnItemKeydownListServiceInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	//变量定义
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
				//获取子项
				nItem=pListCtrl->GetNextSelectedItem(pos);
				if(nItem==-1) break;

				//设置子项
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
				//获取子项
				nItem=pListCtrl->GetNextSelectedItem(pos);
				if (nItem==-1) break;

				//设置子项
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
