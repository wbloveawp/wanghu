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

//构造函数
CBattleOptionBuffer::CBattleOptionBuffer()
{
}

//析构函数
CBattleOptionBuffer::~CBattleOptionBuffer()
{
	//变量定义
	WORD wBattleID=0;
	tagBattleOption * pBattleOption=NULL;
	POSITION Position=m_BattleOptionMap.GetStartPosition();

	//删除索引
	while (Position!=NULL)
	{
		m_BattleOptionMap.GetNextAssoc(Position,wBattleID,pBattleOption);
		SafeDelete(pBattleOption);
	}

	//删除数组
	for (INT_PTR i=0;i<m_BattleOptionArray.GetCount();i++)
	{
		pBattleOption=m_BattleOptionArray[i];
		SafeDelete(pBattleOption);
	}

	//删除引用
	m_BattleOptionMap.RemoveAll();
	m_BattleOptionArray.RemoveAll();

	return;
}

//重置数据
bool CBattleOptionBuffer::ResetBattleOption()
{
	//变量定义
	WORD wBattleID=0;
	tagBattleOption * pBattleOption=NULL;
	POSITION Position=m_BattleOptionMap.GetStartPosition();

	//删除对象
	while (Position!=NULL)
	{
		m_BattleOptionMap.GetNextAssoc(Position,wBattleID,pBattleOption);
		m_BattleOptionArray.Add(pBattleOption);
	}

	//删除索引
	m_BattleOptionMap.RemoveAll();

	return true;
}

//删除数据
bool CBattleOptionBuffer::DeleteBattleOption(WORD wBattleID)
{
	//查找类型
	tagBattleOption * pBattleOption=NULL;
	if (m_BattleOptionMap.Lookup(wBattleID,pBattleOption)==FALSE) return false;

	//删除数据
	m_BattleOptionMap.RemoveKey(wBattleID);
	m_BattleOptionArray.Add(pBattleOption);

	//设置变量
	ZeroMemory(pBattleOption,sizeof(tagBattleOption));

	return true;
}

//插入数据
bool CBattleOptionBuffer::InsertBattleOption(tagBattleOption * pBattleOption)
{
	//效验参数
	ASSERT(pBattleOption!=NULL);
	if (pBattleOption==NULL) return false;

	//查找现存
	WORD wBattleID=pBattleOption->wBattleID;
	tagBattleOption * pBattleOptionInsert=SearchBattleOption(wBattleID);

	//创建判断
	if (pBattleOptionInsert==NULL)
	{
		//创建对象
		pBattleOptionInsert=CreateBattleOption();

		//结果判断
		if (pBattleOptionInsert==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//设置数据
	m_BattleOptionMap[wBattleID]=pBattleOptionInsert;
	CopyMemory(pBattleOptionInsert,pBattleOption,sizeof(tagBattleOption));

	return true;
}

//获取数目
DWORD CBattleOptionBuffer::GetBattleOptionCount()
{
	return (DWORD)(m_BattleOptionMap.GetCount());
}

//查找数据
tagBattleOption * CBattleOptionBuffer::SearchBattleOption(WORD wBattleID)
{
	tagBattleOption * pBattleOption=NULL;
	m_BattleOptionMap.Lookup(wBattleID,pBattleOption);
	return pBattleOption;
}

//创建对象
tagBattleOption * CBattleOptionBuffer::CreateBattleOption()
{
	//变量定义
	tagBattleOption * pBattleOption=NULL;

	//创建对象
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

	//设置变量
	ZeroMemory(pBattleOption,sizeof(tagBattleOption));

	return pBattleOption;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CBattleListControl::CBattleListControl()
{
	m_pBattleOptionBuffer=NULL;
}

//析构函数
CBattleListControl::~CBattleListControl()
{
}

//配置列表
VOID CBattleListControl::InitListControl()
{
	//变量定义
	INT nColIndex=0;

	//配置列表
	InsertColumn(nColIndex++,TEXT("游戏名字"),LVCFMT_CENTER,80);
	InsertColumn(nColIndex++,TEXT("类型标识"),LVCFMT_CENTER,80);	
	InsertColumn(nColIndex++,TEXT("服务器名"),LVCFMT_LEFT,140);
	InsertColumn(nColIndex++,TEXT("创建状态"),LVCFMT_LEFT,100);

	return;
}

//子项排序
INT CBattleListControl::SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex)
{
	//变量定义
	tagGameModuleInfo * pGameModuleInfo1=(tagGameModuleInfo *)lParam1;
	tagGameModuleInfo * pGameModuleInfo2=(tagGameModuleInfo *)lParam2;

	//子项排序
	switch (wColumnIndex)
	{
	case 0:		//游戏名字
		{
			return lstrcmp(pGameModuleInfo1->szModuleName,pGameModuleInfo2->szModuleName);
		}
	case 1:		//类型标识
		{
			return pGameModuleInfo1->wModuleID<pGameModuleInfo2->wModuleID?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 2:		//服务器名
		{
			return lstrcmp(pGameModuleInfo1->szServerDLLName,pGameModuleInfo2->szServerDLLName);
		}
	case 3:		//创建状态
		{
			//约战标识
			WORD wBattleID1=pGameModuleInfo1->wModuleID;
			WORD wBattleID2=pGameModuleInfo2->wModuleID;

			//创建标识
			bool bCreateStatus1=m_pBattleOptionBuffer->SearchBattleOption(wBattleID1)!=NULL;
			bool bCreateStatus2=m_pBattleOptionBuffer->SearchBattleOption(wBattleID2)!=NULL;

			//相同状态
			if (bCreateStatus1==bCreateStatus2)
			{
				return wBattleID1<wBattleID2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			}

			return bCreateStatus1?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	}

	return 0;
}

//获取颜色
VOID CBattleListControl::GetListItemColor(INT nItemID,LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor)
{
	//变量定义
	ASSERT(lItemParam!=NULL);
	tagGameModuleInfo * pGameModuleInfo=(tagGameModuleInfo *)lItemParam;

	//没有创建
	if (m_pBattleOptionBuffer->SearchBattleOption(pGameModuleInfo->wModuleID)==false)
	{
		//设置颜色
		ListItemColor.rcTextColor=RGB(127,127,127);
		ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(0,0,128):CR_NORMAL_BK;		
	}
	else
	{
		//设置颜色
		ListItemColor.rcTextColor=RGB(0,0,0);
		ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(127,127,127):CR_NORMAL_BK;
	}

	return;
}

//插入列表
bool CBattleListControl::InsertModuleInfo(tagGameModuleInfo * pGameModuleInfo)
{
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pGameModuleInfo;

	//存在判断
	INT nInsertItem=FindItem(&FindInfo);
	if (nInsertItem!=LB_ERR) return true;

	//插入列表
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		if (i==0)
		{
			//插入首项
			INT nIndex=GetInsertIndex(pGameModuleInfo);
			LPCTSTR pszDescribe=GetDescribeString(pGameModuleInfo,i);
			nInsertItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,pszDescribe,0,0,0,(LPARAM)pGameModuleInfo);
		}
		else
		{
			//字符子项
			SetItem(nInsertItem,i,LVIF_TEXT,GetDescribeString(pGameModuleInfo,i),0,0,0,0);
		}
	}

	return true;
}

//设置缓冲
void CBattleListControl::SetBattleOptionBuffer(CBattleOptionBuffer * pBattleOptionBuffer)
{
	//设置缓冲
	m_pBattleOptionBuffer=pBattleOptionBuffer;
}

//插入索引
WORD CBattleListControl::GetInsertIndex(tagGameModuleInfo * pGameModuleInfo)
{
	//变量定义
	INT nItemCount=GetItemCount();
	tagGameModuleInfo * pGameModuleInfoTemp=NULL;

	//创建状态
	bool bCreateStatus=m_pBattleOptionBuffer->SearchBattleOption(pGameModuleInfo->wModuleID)!=NULL;

	//获取位置
	for (INT i=0;i<nItemCount;i++)
	{
		//获取数据
		pGameModuleInfoTemp=(tagGameModuleInfo *)GetItemData(i);

		//创建标识
		bool bCreateStatusTemp=m_pBattleOptionBuffer->SearchBattleOption(pGameModuleInfoTemp->wModuleID)!=NULL;

		//创建判断
		if (bCreateStatus==false && bCreateStatusTemp==true)
		{
			continue;
		}

		//创建判断
		if (bCreateStatus==true && bCreateStatusTemp==false)
		{
			return i;
		}
		
		//名字判断
		if (pGameModuleInfo->wModuleID<pGameModuleInfoTemp->wModuleID)
		{
			return i;
		}
	}

	return nItemCount;
}

//描述字符
LPCTSTR CBattleListControl::GetDescribeString(tagGameModuleInfo * pGameModuleInfo, WORD wColumnIndex)
{
	//构造字符
	switch (wColumnIndex)
	{
	case 0:		//游戏名字
		{
			return pGameModuleInfo->szModuleName;
		}
	case 1:		//类型标识
		{
			static TCHAR szModuleID[12] = {0};
			_sntprintf_s(szModuleID,CountArray(szModuleID),TEXT("%d"),pGameModuleInfo->wModuleID);

			return szModuleID;
		}
	case 2:		//服务器名
		{
			return pGameModuleInfo->szServerDLLName;
		}
	case 3:		//创建状态
		{
			//创建状态
			bool bCreateStatus=m_pBattleOptionBuffer->SearchBattleOption(pGameModuleInfo->wModuleID)!=NULL;
			return bCreateStatus?TEXT("已创建"):TEXT("未创建");
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
//构造函数
CBattleOptionManager::CBattleOptionManager()
{
}

//析构函数
CBattleOptionManager::~CBattleOptionManager()
{
}

//修改模块
bool CBattleOptionManager::UpdateBattleOption(tagBattleOption & BattleOption)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//创建对象
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("UpdateGameModuleInfo 创建 PlatformDBModule 对象失败"),MB_ICONERROR);
		return false;
	}

	//连接数据库
	try
	{
		//变量定义
		CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
		tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

		//设置连接
		PlatformDBModule->SetConnectionInfo(pDataBaseParameter->dwDataBaseAddr,pDataBaseParameter->wDataBasePort,
		CW2CT(pDataBaseParameter->szDataBaseName),CW2CT(pDataBaseParameter->szDataBaseUser),CW2CT(pDataBaseParameter->szDataBasePass));

		//发起连接
		PlatformDBModule->OpenConnection();
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//读取列表
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wBattleID"),BattleOption.wBattleID);
		PlatformDBAide.AddParameter(TEXT("@wServicePort"),BattleOption.wServicePort);
		PlatformDBAide.AddParameter(TEXT("@szModuleName"),BattleOption.szModuleName);
		PlatformDBAide.AddParameter(TEXT("@szServiceAddr"),BattleOption.szServiceAddr);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_UpdateBattleOption"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//读取信息
		BattleOption.wBattleID=PlatformDBAide.GetValue_WORD(TEXT("BattleID"));
		BattleOption.wServicePort=PlatformDBAide.GetValue_WORD(TEXT("ServicePort"));
		PlatformDBAide.GetValue_String(TEXT("ModuleName"),BattleOption.szModuleName,CountArray(BattleOption.szModuleName));
		PlatformDBAide.GetValue_String(TEXT("ServiceAddr"),BattleOption.szServiceAddr,CountArray(BattleOption.szServiceAddr));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}

//插入配置
bool CBattleOptionManager::InsertBattleOption(tagBattleOption & BattleOption)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//创建对象
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("UpdateGameModuleInfo 创建 PlatformDBModule 对象失败"),MB_ICONERROR);
		return false;
	}

	//连接数据库
	try
	{
		//变量定义
		CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
		tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

		//设置连接
		PlatformDBModule->SetConnectionInfo(pDataBaseParameter->dwDataBaseAddr,pDataBaseParameter->wDataBasePort,
		CW2CT(pDataBaseParameter->szDataBaseName),CW2CT(pDataBaseParameter->szDataBaseUser),CW2CT(pDataBaseParameter->szDataBasePass));

		//发起连接
		PlatformDBModule->OpenConnection();
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//读取列表
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wBattleID"),BattleOption.wBattleID);
		PlatformDBAide.AddParameter(TEXT("@wServicePort"),BattleOption.wServicePort);
		PlatformDBAide.AddParameter(TEXT("@szModuleName"),BattleOption.szModuleName);
		PlatformDBAide.AddParameter(TEXT("@szServiceAddr"),BattleOption.szServiceAddr);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_InsertBattleOption"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//读取信息
		BattleOption.wBattleID=PlatformDBAide.GetValue_WORD(TEXT("BattleID"));
		BattleOption.wServicePort=PlatformDBAide.GetValue_WORD(TEXT("ServicePort"));
		PlatformDBAide.GetValue_String(TEXT("ModuleName"),BattleOption.szModuleName,CountArray(BattleOption.szModuleName));
		PlatformDBAide.GetValue_String(TEXT("ServiceAddr"),BattleOption.szServiceAddr,CountArray(BattleOption.szServiceAddr));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}

//加载模块
bool CBattleOptionManager::LoadBattleOption(CBattleOptionBuffer & BattleOptionBuffer)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//创建对象
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("LoadGameModuleInfo 创建 PlatformDBModule 对象失败"),MB_ICONERROR);
		return false;
	}

	//连接数据库
	try
	{
		//变量定义
		CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
		tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

		//设置连接
		PlatformDBModule->SetConnectionInfo(pDataBaseParameter->dwDataBaseAddr,pDataBaseParameter->wDataBasePort,
		CW2CT(pDataBaseParameter->szDataBaseName),CW2CT(pDataBaseParameter->szDataBaseUser),CW2CT(pDataBaseParameter->szDataBasePass));

		//发起连接
		PlatformDBModule->OpenConnection();
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//读取列表
		PlatformDBAide.ResetParameter();
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadBattleOption"),true)==DB_SUCCESS)
		{
			//清空列表
			BattleOptionBuffer.ResetBattleOption();

			//读取列表
			while (PlatformDBModule->IsRecordsetEnd()==false)
			{
				//变量定义
				tagBattleOption BattleOption;
				ZeroMemory(&BattleOption,sizeof(BattleOption));

				//读取信息
				BattleOption.wBattleID=PlatformDBAide.GetValue_WORD(TEXT("BattleID"));
				BattleOption.wServicePort=PlatformDBAide.GetValue_WORD(TEXT("ServicePort"));
				PlatformDBAide.GetValue_String(TEXT("ModuleName"),BattleOption.szModuleName,CountArray(BattleOption.szModuleName));
				PlatformDBAide.GetValue_String(TEXT("ServiceAddr"),BattleOption.szServiceAddr,CountArray(BattleOption.szServiceAddr));

				//列表处理
				BattleOptionBuffer.InsertBattleOption(&BattleOption);

				//移动记录
				PlatformDBModule->MoveToNext();
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
//构造函数
CDlgBattleOption::CDlgBattleOption() : CDialog(IDD_BATTLE_OPTION)
{
	//设置变量
	m_pGameModuleInfo=NULL;

	//设置缓冲
	m_BattleListControl.SetBattleOptionBuffer(&m_BattleOptionBuffer);

	return;
}

//析构函数
CDlgBattleOption::~CDlgBattleOption()
{
}

//控件绑定
VOID CDlgBattleOption::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);	
	DDX_Control(pDX, IDC_BATTLE_LIST, m_BattleListControl);
}

//初始化函数
BOOL CDlgBattleOption::OnInitDialog()
{
	__super::OnInitDialog();

	//加载列表
	LoadDBModuleItem();

	return TRUE;
}

//确定函数
VOID CDlgBattleOption::OnOK()
{
	//选择判断
	if (m_pGameModuleInfo==NULL)
	{
		AfxMessageBox(TEXT("请为约战服务选择一个游戏类型！"),MB_ICONERROR);

		return;
	}

	//服务地址
	TCHAR szServiceAddr[LEN_DOMAIN];
	GetDlgItemText(IDC_SERVICE_ADDR,szServiceAddr,CountArray(szServiceAddr));
	if (szServiceAddr[0]==0)
	{
		AfxMessageBox(TEXT("请输入有效的服务地址或域名！"),MB_ICONERROR);

		return;
	}

	//服务端口
	WORD wServicePort=0;
	wServicePort=GetDlgItemInt(IDC_SERVICE_PORT);

	//房间信息
	tagBattleOption BattleOption;
	ZeroMemory(&BattleOption,sizeof(BattleOption));

	//模块索引
	BattleOption.wServicePort=wServicePort;	
	BattleOption.wBattleID=m_pGameModuleInfo->wModuleID;
	lstrcpyn(BattleOption.szServiceAddr,szServiceAddr,CountArray(BattleOption.szServiceAddr));
	lstrcpyn(BattleOption.szModuleName,m_pGameModuleInfo->szModuleName,CountArray(BattleOption.szModuleName));
	
	//插入配置
	if (m_BattleOptionBuffer.SearchBattleOption(BattleOption.wBattleID)==NULL)
	{
		//插入配置
		if (m_BattleOptionManager.InsertBattleOption(BattleOption)==false) return;

		//提示消息
		CTraceService::TraceString(TEXT("约战服务创建成功"),TraceLevel_Normal);
	}
	else	
	{
		//修改配置
		if (m_BattleOptionManager.UpdateBattleOption(BattleOption)==false) return;

		//提示消息
		CTraceService::TraceString(TEXT("约战服务配置成功"),TraceLevel_Normal);
	}

	//拷贝配置
	CopyMemory(&m_BattleOption,&BattleOption,sizeof(m_BattleOption));

	//关闭窗口
	EndDialog(IDOK);
}

//取消消息
VOID CDlgBattleOption::OnCancel()
{
	//清除配置
	ZeroMemory(&m_BattleOption,sizeof(m_BattleOption));

	//关闭窗口
	EndDialog(IDCANCEL);
}

//加载列表
bool CDlgBattleOption::LoadDBModuleItem()
{
	//加载约战
	m_BattleOptionManager.LoadBattleOption(m_BattleOptionBuffer);

	//加载模块
	if (m_ModuleInfoManager.LoadGameModuleInfo(m_ModuleInfoBuffer)==true)
	{
		//重置列表
		m_BattleListControl.DeleteAllItems();

		//变量定义
		POSITION Position=m_ModuleInfoBuffer.m_GameModuleInfoMap.GetStartPosition();

		//枚举模块
		while (Position!=NULL)
		{
			//获取对象
			WORD wModuleID=0L;
			tagGameModuleInfo * pGameModuleInfo=NULL;
			m_ModuleInfoBuffer.m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameModuleInfo);

			//插入列表
			ASSERT(pGameModuleInfo!=NULL);
			if (pGameModuleInfo!=NULL) m_BattleListControl.InsertModuleInfo(pGameModuleInfo);
		}
	}	

	return false;
}

//双击列表
VOID CDlgBattleOption::OnNMlclkModuleList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//加载配置
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		//变量定义
		INT nListItem=((NMITEMACTIVATE *)pNMHDR)->iItem;
		m_pGameModuleInfo=(tagGameModuleInfo *)m_BattleListControl.GetItemData(nListItem);

		//查找约战
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

//双击列表
VOID CDlgBattleOption::OnNMlDBclkModuleList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//加载配置
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		//变量定义
		INT nListItem=((NMITEMACTIVATE *)pNMHDR)->iItem;
		m_pGameModuleInfo=(tagGameModuleInfo *)m_BattleListControl.GetItemData(nListItem);

		return OnOK();
	}

	return;
}

//////////////////////////////////////////////////////////////////////////