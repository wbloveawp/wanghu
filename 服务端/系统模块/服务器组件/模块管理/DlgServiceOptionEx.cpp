#include "StdAfx.h"
#include "Resource.h"
#include "DataFile.h"
#include "DlgServiceExOption.h"

//////////////////////////////////////////////////////////////////////////////////
//消息映射
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

//构造函数
CDlgServiceOptionEx::CDlgServiceOptionEx() : CDialog(IDD_SERVICE_OPTION)
{
	//设置变量
	m_szDataFileName[0] = 0;
	m_nSelectServiceItem=-1;
	ZeroMemory(&m_ServiceOption,sizeof(m_ServiceOption));

	return;
}

//析构函数
CDlgServiceOptionEx::~CDlgServiceOptionEx()
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
BOOL CDlgServiceOptionEx::OnInitDialog()
{
	__super::OnInitDialog();

	//初始列表
	InitControl();

	//读取配置
	LoadServiceOption();

	return TRUE;
}

//消息解释
BOOL CDlgServiceOptionEx::PreTranslateMessage(MSG * pMsg)
{
	//按键过滤
	if ((pMsg->message==WM_KEYDOWN)&&((pMsg->wParam==VK_ESCAPE)||(pMsg->wParam==VK_RETURN)))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//控件绑定
VOID CDlgServiceOptionEx::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);	
}

//文件名称
VOID CDlgServiceOptionEx::SetDataFileName(LPCTSTR pszFileName)
{
	lstrcpyn(m_szDataFileName, pszFileName,CountArray(m_szDataFileName));
}

//加载配置
bool CDlgServiceOptionEx::LoadServiceOption()
{
	//获取路径
	TCHAR szWorkDir[MAX_PATH]=TEXT("");
	CAssistant::GetWorkDirectory(szWorkDir,CountArray(szWorkDir));

	//构造路径
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf_s(szIniFile,CountArray(szIniFile),TEXT("%s\\%s"),szWorkDir,m_szDataFileName);

	//读取数据
	CDataFile DataFile;
	if (DataFile.OpenDataFile(szIniFile)==FALSE)  return false;

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
bool CDlgServiceOptionEx::SaveServiceOption()
{
	//获取路径
	TCHAR szWorkDir[MAX_PATH]=TEXT("");
	CAssistant::GetWorkDirectory(szWorkDir,CountArray(szWorkDir));

	//构造路径
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf_s(szIniFile, CountArray(szIniFile), TEXT("%s\\%s"), szWorkDir, m_szDataFileName);

	//文件
	CDataFile DataFile;
	if (DataFile.CreateDataFile(szIniFile)==FALSE)
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
tagServiceOption * CDlgServiceOptionEx::ActiveServiceOption()
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

	//加入缓存
	m_ServiceOptionBuffer.Add(pServiceOption);
}

//初始列表
VOID CDlgServiceOptionEx::InitControl()
{
	//获取指针
	CListCtrl * pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);
	pListCtrl->SetExtendedStyle(pListCtrl->GetExtendedStyle()|LVS_EX_FULLROWSELECT);

	//插入头部
	pListCtrl->InsertColumn(0,TEXT("服务标识"),LVCFMT_LEFT,60);	
	pListCtrl->InsertColumn(1,TEXT("服务名称"),LVCFMT_LEFT,120);
	pListCtrl->InsertColumn(2,TEXT("服务站点"), LVCFMT_LEFT,60);
	pListCtrl->InsertColumn(3,TEXT("最大连接"),LVCFMT_LEFT,70);
	pListCtrl->InsertColumn(4,TEXT("服务端口"),LVCFMT_LEFT,70);
	pListCtrl->InsertColumn(5,TEXT("服务地址"),LVCFMT_LEFT,80);
}

//选择子项
VOID CDlgServiceOptionEx::SelectServiceItem(int nIndex)
{
	//获取指针
	CListCtrl * pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_SERVICE_INFO);

	//设置变量
	m_nSelectServiceItem=nIndex;
	tagServiceOption * pServiceOption=(tagServiceOption *)pListCtrl->GetItemData(m_nSelectServiceItem);

	//设置控件
	SetDlgItemText(IDC_SERVICE_ID,pListCtrl->GetItemText(m_nSelectServiceItem,0));	
	SetDlgItemText(IDC_SERVICE_NAME,pListCtrl->GetItemText(m_nSelectServiceItem,1));
	SetDlgItemText(IDC_SERVICE_STATION,pListCtrl->GetItemText(m_nSelectServiceItem, 2));
	SetDlgItemText(IDC_MAX_CONNECT,pListCtrl->GetItemText(m_nSelectServiceItem,3));
	SetDlgItemText(IDC_SERVICE_PORT,pListCtrl->GetItemText(m_nSelectServiceItem,4));	
	SetDlgItemText(IDC_SERVICE_ADDR,pListCtrl->GetItemText(m_nSelectServiceItem,5));	
}

//设置控件
VOID CDlgServiceOptionEx::SetServiceToControl(tagServiceOption *pServiceOption,CListCtrl * pListCtrl,int nIndex)
{
	//变量定义
    CString strItemText;

	//服务ID
	strItemText.Format(TEXT("%d"),pServiceOption->wServiceID);
	pListCtrl->SetItemText(nIndex,0,strItemText);

	//服务名称
	pListCtrl->SetItemText(nIndex,1,pServiceOption->szServiceName);

	//服务站点
	strItemText.Format(TEXT("%d"), pServiceOption->dwStationID);
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
VOID CDlgServiceOptionEx::OnBnClickedQuit()
{
	EndDialog(IDCANCEL);
}

//添加数据
VOID CDlgServiceOptionEx::OnBnClickedAddServiceInfo()
{
	//变量定义
	tagServiceOption ServiceOption;
	ZeroMemory(&ServiceOption,sizeof(ServiceOption));

	//获取数据	
	ServiceOption.wServiceID=GetDlgItemInt(IDC_SERVICE_ID);	
	ServiceOption.wMaxConnect=GetDlgItemInt(IDC_MAX_CONNECT);
	ServiceOption.wServicePort=GetDlgItemInt(IDC_SERVICE_PORT);
	ServiceOption.dwStationID = GetDlgItemInt(IDC_SERVICE_STATION);
	GetDlgItemText(IDC_SERVICE_NAME,ServiceOption.szServiceName,CountArray(ServiceOption.szServiceName));
	GetDlgItemText(IDC_SERVICE_ADDR,ServiceOption.szServiceDomain,CountArray(ServiceOption.szServiceDomain));
	
	//服务标识
	if (ServiceOption.wServiceID==0)
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
VOID CDlgServiceOptionEx::OnBnClickedModifyServiceInfo()
{
	//变量定义
	tagServiceOption ServiceOption;
	ZeroMemory(&ServiceOption,sizeof(ServiceOption));

	//获取数据	
	ServiceOption.wServiceID=GetDlgItemInt(IDC_SERVICE_ID);	
	ServiceOption.wMaxConnect=GetDlgItemInt(IDC_MAX_CONNECT);	
	ServiceOption.wServicePort=GetDlgItemInt(IDC_SERVICE_PORT);
	ServiceOption.dwStationID=GetDlgItemInt(IDC_SERVICE_STATION);
	GetDlgItemText(IDC_SERVICE_NAME,ServiceOption.szServiceName,CountArray(ServiceOption.szServiceName));
	GetDlgItemText(IDC_SERVICE_ADDR,ServiceOption.szServiceDomain,CountArray(ServiceOption.szServiceDomain));
	
	//服务标识
	if (ServiceOption.wServiceID==0)
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
VOID CDlgServiceOptionEx::OnBnClickedDeleteServiceInfo()
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
VOID CDlgServiceOptionEx::OnHdnItemclickListServiceInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->iItem!=-1) SelectServiceItem(pNMLV->iItem);
	*pResult = 0;
}

//双击子项
VOID CDlgServiceOptionEx::OnHdnItemDblclkListServiceInfo(NMHDR *pNMHDR, LRESULT *pResult)
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
VOID CDlgServiceOptionEx::OnLvnItemKeydownListServiceInfo(NMHDR *pNMHDR, LRESULT *pResult)
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
