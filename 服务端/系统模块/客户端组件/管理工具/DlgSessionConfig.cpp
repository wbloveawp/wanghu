#include "StdAfx.h"
#include "Resource.h"
#include "DlgSessionConfig.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgSessionConfig, CDlgSessionItem)
	//ON_WM_CTLCOLOR()	
	ON_BN_CLICKED(IDC_BT_QUERY, OnBnClickedBtQuery)
	ON_BN_CLICKED(IDC_BT_APPEND, OnBnClickedBtAppend)
	ON_BN_CLICKED(IDC_BT_MODIFY, OnBnClickedBtModify)
	ON_BN_CLICKED(IDC_BT_DELETE, OnBnClickedBtDelete)
	ON_BN_CLICKED(IDC_BT_SAVE_BASIC_CONFIG, OnBnClickedBtUpdateBasicConfig)
	ON_BN_CLICKED(IDC_BT_SAVE_NORMAL_TABLE, OnBnClickedBtUpdateNormalWeight)
	ON_BN_CLICKED(IDC_BT_SAVE_WHITELIST_TABLE, OnBnClickedBtUpdateWhiteListWeight)

	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST_NORMAL_WEIGHT, OnLvnListDeleteWeightItem)
	ON_NOTIFY(NM_CLICK, IDC_LIST_NORMAL_WEIGHT, OnHdnListClickWeightItem)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_NORMAL_WEIGHT, OnLvnListKeydownWeightItem)

	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST_WHITELIST_WEIGHT, OnLvnListDeleteWeightItem)
	ON_NOTIFY(NM_CLICK, IDC_LIST_WHITELIST_WEIGHT, OnHdnListClickWeightItem)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_WHITELIST_WEIGHT, OnLvnListKeydownWeightItem)

	ON_WM_DESTROY()
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgSessionConfig::CDlgSessionConfig() : CDlgSessionItem(IDD_DLG_CONFIG)
{
	//设置变量
	m_wKindID = 0;
	m_pSelListCtrl = NULL;
	m_nSelWeightItem = -1;	
}

//析构函数
CDlgSessionConfig::~CDlgSessionConfig()
{
}

//控件绑定
VOID CDlgSessionConfig::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//绑定控件
	DDX_Control(pDX, IDC_BT_QUERY,  m_btQuery);
	DDX_Control(pDX, IDC_BT_SAVE_BASIC_CONFIG, m_btUpdateBasicConfig);
	DDX_Control(pDX, IDC_BT_SAVE_NORMAL_TABLE, m_btSaveNormalWeight);
	DDX_Control(pDX, IDC_BT_SAVE_WHITELIST_TABLE, m_btSaveWhiteListWeight);

	//绑定控件
	DDX_Control(pDX, IDC_COMBO_GAME_LIST, m_cbGameList);
	DDX_Control(pDX, IDC_COMBO_WEIGHT_TYPE, m_cbWeightType);

	//列表配置
	DDX_Control(pDX, IDC_LIST_NORMAL_WEIGHT, m_ListNormalWeight);
	DDX_Control(pDX, IDC_LIST_WHITELIST_WEIGHT, m_ListWhiteListWeight);
}

VOID CDlgSessionConfig::OnDestory()
{
	//删除子项
	m_ListNormalWeight.DeleteAllItems();
	m_ListWhiteListWeight.DeleteAllItems();
}

//配置函数
BOOL CDlgSessionConfig::OnInitDialog()
{
	__super::OnInitDialog();

	//初始化控件
	InitControls();

	return FALSE;
}

//确定函数
VOID CDlgSessionConfig::OnOK()
{
	__super::OnOK();
}

//消息解释
BOOL CDlgSessionConfig::PreTranslateMessage(MSG * pMsg)
{
	//按键过虑
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//添加修改
VOID CDlgSessionConfig::AddModifyWightItem(bool bModify)
{
	//选择判断
	if (bModify == true && m_nSelWeightItem == -1) return;

	//构造结构
	tagTimesWeightItem WeightItem = {};

	//倍数区间
	WeightItem.dwTimesRange[0] = GetDlgItemInt(IDC_EDIT_MIN_TIMES);
	WeightItem.dwTimesRange[1] = GetDlgItemInt(IDC_EDIT_MAX_TIMES);

	//校验倍数
	if (WeightItem.dwTimesRange[0]>INVALID_WORD || WeightItem.dwTimesRange[1]>INVALID_WORD)
	{
		CString strMessage;
		strMessage.Format(TEXT("最大倍数不能超过%d！"), INVALID_WORD);

		//弹出提示
		CInformation Information;
		Information.ShowMessageBox(strMessage, MB_ICONERROR);

		return;
	}

	//倍数权重
	WeightItem.wTimesWeight = GetDlgItemInt(IDC_EDIT_TIMES_WEIGHT);

	//倍数类型
	WeightItem.cbTimesKind = (GetCheckedRadioButton(IDC_RADIO_LOST, IDC_RADIO_WIN) == IDC_RADIO_LOST) ? 0 : 1;

	//权重类型
	if (m_cbWeightType.GetCurSel() != -1)
	{
		WeightItem.cbWeightType = m_cbWeightType.GetItemData(m_cbWeightType.GetCurSel());
	}

	//列表控件
	auto pListCtrl = m_pSelListCtrl;
	if (bModify == false)
	{
		pListCtrl = WeightItem.cbWeightType == 0 ? &m_ListNormalWeight : &m_ListWhiteListWeight;
	}

	//校验倍数区间
	for (int i = 0; i < pListCtrl->GetItemCount(); i++)
	{
		//
		if (bModify && m_nSelWeightItem == i) continue;

		//校验区间
		auto pTimeWeightItem = (tagTimesWeightItem*)pListCtrl->GetItemData(i);
		if ((pTimeWeightItem->dwTimesRange[0] >= WeightItem.dwTimesRange[0] && pTimeWeightItem->dwTimesRange[0] <= WeightItem.dwTimesRange[1]) ||
			(pTimeWeightItem->dwTimesRange[1] >= WeightItem.dwTimesRange[0] && pTimeWeightItem->dwTimesRange[1] <= WeightItem.dwTimesRange[1]))
		{
			//弹出提示
			CInformation Information;
			Information.ShowMessageBox(TEXT("倍数区间与现有配置项重叠！"), MB_ICONERROR);

			return;
		}
	}

	//新增权重
	if (bModify == false)
	{
		//变量定义
		LVITEM lVItem;
		lVItem.mask = LVIF_TEXT;
		lVItem.iSubItem = 0;
		lVItem.pszText = TEXT("");
		lVItem.iItem = pListCtrl->GetItemCount();

		//插入子项.
		auto nItemIndex = pListCtrl->InsertItem(&lVItem);
		
		//设置子项
		//SetWeightItemToList(pListCtrl, nItemIndex, &WeightItem);	

		//申请资源
		auto pItemData = new tagTimesWeightItem;
		if (pItemData != NULL)
		{
			//拷贝数据
			CopyMemory(pItemData, &WeightItem, sizeof(tagTimesWeightItem));

			//设置数据
			pListCtrl->SetItemData(nItemIndex, (DWORD_PTR)pItemData);
		}
	}
	else
	{
		//更新列表
		//SetWeightItemToList(pListCtrl, m_nSelWeightItem, &WeightItem);

		//设置数据
		auto pItemData = (tagTimesWeightItem*)pListCtrl->GetItemData(m_nSelWeightItem);
		if (pItemData != NULL)
		{
			//拷贝数据
			CopyMemory(pItemData, &WeightItem, sizeof(tagTimesWeightItem));
		}	
	}	

	//统计权重值
	DWORD dwTotalWeight[2] = {0};
	for (int i = 0; i < pListCtrl->GetItemCount(); i++)
	{
		auto pItemData = (tagTimesWeightItem*)pListCtrl->GetItemData(i);
		if (pItemData != NULL)
		{
			dwTotalWeight[pItemData->cbTimesKind] += pItemData->wTimesWeight;
		}
	}

	for (int i = 0; i < pListCtrl->GetItemCount(); i++)
	{
		auto pItemData = (tagTimesWeightItem*)pListCtrl->GetItemData(i);
		if (pItemData != NULL)
		{
			pItemData->fWightRatio = static_cast<float>(pItemData->wTimesWeight) / dwTotalWeight[pItemData->cbTimesKind];

			//更新列表
			SetWeightItemToList(pListCtrl, i, pItemData);
		}
	}
	
	return;
}

//格式化配置
bool CDlgSessionConfig::FormatJsonConfig(LPCTSTR pszBasicConfig, TCHAR szFormatConfig[], WORD wBuffSize)
{
	//变量定义
	WORD wIndex = 0;
	int nStringLen = lstrlen(pszBasicConfig);
	for (int i = 0; i < nStringLen; i++)
	{
		if (wIndex + 1 >= wBuffSize) return false;

		//复制字符
		szFormatConfig[wIndex++] = pszBasicConfig[i];

		//判断特殊字符
		if (pszBasicConfig[i] == L'[' || pszBasicConfig[i] == L']' || pszBasicConfig[i] == L'{' || pszBasicConfig[i] == L'}' || pszBasicConfig[i] == L',')
		{
			if (wIndex + 4 >= wBuffSize) return  false;

			//添加换行符
			szFormatConfig[wIndex++] = L'\r';
			szFormatConfig[wIndex++] = L'\n';

			if (i < nStringLen - 1)
			{
				szFormatConfig[wIndex++] = L' ';
				szFormatConfig[wIndex++] = L' ';
			}
		}

		//判断特殊字符
		if (i + 1 < nStringLen && pszBasicConfig[i + 1] == L']' || pszBasicConfig[i+1] == L'}')
		{
			if (wIndex + 2 >= wBuffSize) return  false;

			//添加换行符
			szFormatConfig[wIndex++] = L'\r';
			szFormatConfig[wIndex++] = L'\n';			
		}
	}

	if (wIndex + 1 >= wBuffSize) return false;

	//添加结束符
	szFormatConfig[wIndex++] = '\0';

	return true;
}

//压缩配置
bool CDlgSessionConfig::CompressJsonConfig(LPCTSTR pszFormatConfig, TCHAR szBasicConfig[], WORD wBuffSize)
{
	//变量定义
	WORD wIndex = 0;
	int nStringLen = lstrlen(pszFormatConfig);
	for (int i = 0; i < nStringLen; i++)
	{
		//过滤字符
		auto chr = pszFormatConfig[i];	
		if (chr == L'\r' || chr == L'\n' || chr == L' ') continue;
		
		//检验缓冲
		if (wIndex + 1 >= wBuffSize) return false;

		//设置字符
		szBasicConfig[wIndex++] = chr;
	}

	//校验缓冲
	if (wIndex + 1 >= wBuffSize) return false;

	//添加结束符
	szBasicConfig[wIndex++] = '\0';

	return true;
}

//选中子项
VOID CDlgSessionConfig::SelectWeightItem(CListCtrl* pListCtrl, int nIndex)
{
	//校验参数
	if (pListCtrl == NULL || nIndex == -1) return;

	//设置变量
	m_pSelListCtrl=pListCtrl;
	m_nSelWeightItem=nIndex;	

	//获取数据
	auto pTimesWeightItem = (tagTimesWeightItem*)m_pSelListCtrl->GetItemData(m_nSelWeightItem);
	if (pTimesWeightItem == NULL) return;

	//倍数区间
	SetDlgItemInt(IDC_EDIT_MIN_TIMES, pTimesWeightItem->dwTimesRange[0]);
	SetDlgItemInt(IDC_EDIT_MAX_TIMES, pTimesWeightItem->dwTimesRange[1]);

	//倍数权重
	SetDlgItemInt(IDC_EDIT_TIMES_WEIGHT, pTimesWeightItem->wTimesWeight);

	//倍数标记
	CheckRadioButton(IDC_RADIO_LOST, IDC_RADIO_WIN, pTimesWeightItem->cbTimesKind==0?IDC_RADIO_LOST: IDC_RADIO_WIN);
	
	//权重类型
	auto cbWeightType = (m_pSelListCtrl==GetDlgItem(IDC_LIST_NORMAL_WEIGHT))?0:1;
	for (int i = 0; i < m_cbWeightType.GetCount(); i++)
	{
		if (m_cbWeightType.GetItemData(i) == cbWeightType)
		{
			m_cbWeightType.SetCurSel(i);
			break;
		}
	}
}

//收集权重
int CDlgSessionConfig::CollectWeightItem(CListCtrl* pListCtrl, int nBuffCount, tagTimesWeightItem TimesWeightItem[])
{
	int nItemCount = 0;

	for (int i = 0; i < pListCtrl->GetItemCount(); i++)
	{
		auto pItemData = (tagTimesWeightItem*)pListCtrl->GetItemData(i);
		if (pItemData != NULL)
		{
			CopyMemory(&TimesWeightItem[nItemCount++], pItemData,sizeof(tagTimesWeightItem));
			if (nItemCount >= nBuffCount) break;
		}
	}

	return nItemCount;
}

//设置子项
VOID CDlgSessionConfig::SetWeightItemToList(CListCtrl* pListCtrl, int nItemIndex, tagTimesWeightItem* pTimesWeightItem)
{
	//校验参数
	if (pListCtrl == NULL || pTimesWeightItem == NULL) return;

	//变量定义
	TCHAR szItemValue[32] = TEXT("");

	//倍数区间
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d-%d"), pTimesWeightItem->dwTimesRange[0], pTimesWeightItem->dwTimesRange[1]);
	pListCtrl->SetItemText(nItemIndex, 0, szItemValue);

	//倍数类型
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%s"), pTimesWeightItem->cbTimesKind == 0 ? TEXT("输") : TEXT("赢"));
	pListCtrl->SetItemText(nItemIndex, 1, szItemValue);		

	//倍数权重
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pTimesWeightItem->wTimesWeight);
	pListCtrl->SetItemText(nItemIndex, 2, szItemValue);

	//权重占比
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.3f"), pTimesWeightItem->fWightRatio);
	pListCtrl->SetItemText(nItemIndex, 3, szItemValue);
}

//读取事件
bool CDlgSessionConfig::OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	//库存服务
	if (Command.wMainCmdID == MDM_GP_CONFIG_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_GAME_BASIC_CONFIG:		//基础配置
		{
			//提取变量
			auto pBasicConfig = (CMD_GP_GameBasicConfig*)pData;

			//获取控件
			auto pEditBasicConfig = (CEdit*)GetDlgItem(IDC_EDIT_BASIC_CONFIG);

			//变量定义
			TCHAR szBasicConfig[4000] = {0};
			if (FormatJsonConfig(pBasicConfig->szBasicConfig, szBasicConfig, CountArray(szBasicConfig)))
			{
				//设置文本
				pEditBasicConfig->SetWindowText(szBasicConfig);
			}
			else
			{
				//设置文本
				pEditBasicConfig->SetWindowText(pBasicConfig->szBasicConfig);
			}

			break;
		}
		case SUB_GP_GAME_WEIGHT_CONFIG:		//权重配置
		{
			//提取变量
			auto pWeightConfig = (CMD_GP_GameWeightConfig*)pData;

			//变量定义
			int nItemIndex = -1;
			CListCtrl* pListCtrl = NULL;

			//变量定义
			LVITEM lVItem;
			lVItem.mask = LVIF_TEXT;
			lVItem.iSubItem = 0;
			lVItem.pszText = TEXT("");

			for (int i = 0; i < pWeightConfig->wItemCount; i++)
			{
				auto pTimesWeightItem = &pWeightConfig->TimesWeightItem[i];

				//普通权重
				if (pTimesWeightItem->cbWeightType == 0)
				{
					pListCtrl = &m_ListNormalWeight;
				}

				//白名单权重
				if (pTimesWeightItem->cbWeightType == 1)
				{
					pListCtrl = &m_ListWhiteListWeight;
				}

				//插入子项
				lVItem.iItem = pListCtrl->GetItemCount();
				nItemIndex = pListCtrl->InsertItem(&lVItem);

				//设置子项
				SetWeightItemToList(pListCtrl, nItemIndex, pTimesWeightItem);

				//申请资源
				auto pItemData = new tagTimesWeightItem;
				if (pItemData != NULL)
				{
					//拷贝数据
					CopyMemory(pItemData, pTimesWeightItem, sizeof(tagTimesWeightItem));

					//设置数据
					pListCtrl->SetItemData(nItemIndex, (DWORD_PTR)pItemData);
				}
			}

			//选择权重
			SelectWeightItem(pListCtrl, 0);

			break;
		}
		case SUB_GP_GAME_CONFIG_FINISH:		//配置完成
		{
			//控件设置
			EnableControls(TRUE);

			break;
		}
		}
	}
	
	//操作失败
	if (Command.wMainCmdID == MDM_GP_USER_SERVICE)
	{	
		//操作成功
		if (Command.wSubCmdID == SUB_GP_OPERATE_SUCCESS)
		{
			//参数校验
			ASSERT(sizeof(CMD_GP_OperateSuccess) >= wDataSize);
			if (sizeof(CMD_GP_OperateSuccess) < wDataSize) return false;

			//提取数据
			auto pOperateSuccess = (CMD_GP_OperateSuccess*)pData;

			//控件设置
			EnableControls(TRUE);

			//弹出提示
			CInformation Information;
			Information.ShowMessageBox(pOperateSuccess->szDescribeString);

			return true;
		}

		//操作失败
		if (Command.wSubCmdID == SUB_GP_OPERATE_FAILURE)
		{
			//参数校验
			ASSERT(sizeof(CMD_GP_OperateFailure) >= wDataSize);
			if (sizeof(CMD_GP_OperateFailure) < wDataSize) return false;

			//提取数据
			CMD_GP_OperateFailure* pOperateFailure = (CMD_GP_OperateFailure*)pData;

			//控件设置
			EnableControls(TRUE);

			//弹出提示
			CInformation Information;
			Information.ShowMessageBox(pOperateFailure->szDescribeString, MB_ICONERROR);

			return true;
		}
	}

	return false;
}

//初始化控件
VOID CDlgSessionConfig::InitControls()
{
	//设置控件
	DWORD dwExstyle = m_ListNormalWeight.GetExtendedStyle();
	//m_ListNormalWeight.SetExtendedStyle(dwExstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | WS_EX_STATICEDGE);

	//列表控件
	m_ListNormalWeight.InsertColumn(0, TEXT("倍数区间"), LVCFMT_LEFT, 80);
	m_ListNormalWeight.InsertColumn(1, TEXT("类型"), LVCFMT_LEFT, 40);	
	m_ListNormalWeight.InsertColumn(2, TEXT("权重"), LVCFMT_LEFT, 40);
	m_ListNormalWeight.InsertColumn(3, TEXT("占比"), LVCFMT_LEFT, 40);
	
	//设置控件
	dwExstyle = m_ListWhiteListWeight.GetExtendedStyle();
	//m_ListWhiteListWeight.SetExtendedStyle(dwExstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | WS_EX_STATICEDGE);

	//列表控件
	m_ListWhiteListWeight.InsertColumn(0, TEXT("倍数区间"), LVCFMT_LEFT, 80);
	m_ListWhiteListWeight.InsertColumn(1, TEXT("类型"), LVCFMT_LEFT, 40);	
	m_ListWhiteListWeight.InsertColumn(2, TEXT("权重"), LVCFMT_LEFT, 40);
	m_ListWhiteListWeight.InsertColumn(3, TEXT("占比"), LVCFMT_LEFT, 40);

	//倍数类型
	CheckRadioButton(IDC_RADIO_LOST, IDC_RADIO_WIN, IDC_RADIO_LOST);	

	//权重类型
	BYTE cbWeightType[] = { 0,1 };
	LPCTSTR pszWeightType[] = { TEXT("普通类型"), TEXT("白名单类型") };
	for (int i = 0; i < CountArray(pszWeightType); i++)
	{
		m_cbWeightType.SetItemData(m_cbWeightType.InsertString(i,pszWeightType[i]), cbWeightType[i]);
	}
}

//控件使能
VOID CDlgSessionConfig::EnableControls(BOOL bEnabled)
{
	//查询状态
	m_btQuery.EnableWindow(bEnabled);
	m_btUpdateBasicConfig.EnableWindow(bEnabled);
	m_btSaveNormalWeight.EnableWindow(bEnabled);
	m_btSaveWhiteListWeight.EnableWindow(bEnabled);
}

//单击子项
VOID CDlgSessionConfig::OnHdnListClickWeightItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	CListCtrl* pListCtrl = NULL;

	//普通权重
	if (pNMHDR->hwndFrom == m_ListNormalWeight.GetSafeHwnd())
	{
		//选中子项
		pListCtrl = &m_ListNormalWeight;
	}

	//白名单权重
	if (pNMHDR->hwndFrom == m_ListWhiteListWeight.GetSafeHwnd())
	{
		//选中子项
		pListCtrl = &m_ListWhiteListWeight;
	}

	//选中子项
	if (pListCtrl)
	{		
		SelectWeightItem(pListCtrl, phdr->iItem);
	}

	*pResult = 0;
}

//删除子项
VOID CDlgSessionConfig::OnLvnListDeleteWeightItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	//变量定义
	CListCtrl* pListCtrl = NULL;

	//普通权重
	if (pNMHDR->hwndFrom == m_ListNormalWeight.GetSafeHwnd())
	{
		//选中子项
		pListCtrl=&m_ListNormalWeight;
	}

	//白名单权重
	if (pNMHDR->hwndFrom == m_ListWhiteListWeight.GetSafeHwnd())
	{
		//选中子项
		pListCtrl=&m_ListWhiteListWeight;
	}

	//获取数据
	if (pListCtrl != NULL)
	{
		auto pItemData = (tagStockInfo*)pListCtrl->GetItemData(pNMLV->iItem);
		if (pItemData != NULL)
		{
			SafeDelete(pItemData);
		}
	}

	*pResult = 0;
}

//键盘按下
VOID CDlgSessionConfig::OnLvnListKeydownWeightItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	//变量定义
	CListCtrl* pListCtrl = NULL;

	//普通权重
	if (pNMHDR->hwndFrom == m_ListNormalWeight.GetSafeHwnd())
	{
		//选中子项
		pListCtrl = &m_ListNormalWeight;
	}

	//白名单权重
	if (pNMHDR->hwndFrom == m_ListWhiteListWeight.GetSafeHwnd())
	{
		//选中子项
		pListCtrl = &m_ListWhiteListWeight;
	}

	if (pListCtrl != NULL)
	{
		//变量定义
		int nItem = -1;
		POSITION pos;

		switch (pLVKeyDow->wVKey)
		{
		case VK_DOWN:
		{
			pos = pListCtrl->GetFirstSelectedItemPosition();
			while (pos)
			{
				//获取子项
				nItem = pListCtrl->GetNextSelectedItem(pos);
				if (nItem == -1) break;

				//设置子项
				nItem = __min(nItem + 1, pListCtrl->GetItemCount() - 1);
				SelectWeightItem(pListCtrl,nItem);
				break;
			}

			break;
		}
		case VK_UP:
		{
			pos = pListCtrl->GetFirstSelectedItemPosition();
			while (pos)
			{
				//获取子项
				nItem = pListCtrl->GetNextSelectedItem(pos);
				if (nItem == -1) break;

				//设置子项
				nItem = __max(nItem - 1, 0);
				SelectWeightItem(pListCtrl,nItem);
				break;
			}
			break;
		}
		}
	}

	*pResult = 0;
}


//查询参数
VOID CDlgSessionConfig::OnBnClickedBtQuery()
{
	//游戏选择
	if (m_cbGameList.GetCurSel() == -1)
	{
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("请选择游戏类型！"));
	}

	//游戏类型
	m_wKindID = m_cbGameList.GetItemData(m_cbGameList.GetCurSel());

	//查询配置
	if (QueryGameConfig(m_wKindID))
	{
		EnableControls(FALSE);

		//清除列表
		m_ListNormalWeight.DeleteAllItems();
		m_ListWhiteListWeight.DeleteAllItems();

		//清除文本
		GetDlgItem(IDC_EDIT_BASIC_CONFIG)->SetWindowText(TEXT(""));
	}

	return;
}

//添加子项
VOID CDlgSessionConfig::OnBnClickedBtAppend()
{
	//校验类型
	if (m_wKindID==0)
	{
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("请先选择一款游戏并查询！"));

		return;
	}

	//设置变量
	AddModifyWightItem(false);
}

//编辑子项
VOID CDlgSessionConfig::OnBnClickedBtModify()
{
	//设置变量
	AddModifyWightItem(true);
}

//删除子项
VOID CDlgSessionConfig::OnBnClickedBtDelete()
{
	//获取索引
	if (m_pSelListCtrl == NULL) return;
	if (m_nSelWeightItem == -1) return;

	//调整选项
	if (m_pSelListCtrl->GetItemCount() <= m_nSelWeightItem)
	{
		m_nSelWeightItem = -1;

		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("请选择您要删除的倍数！"));

		return;
	}

	m_pSelListCtrl->DeleteItem(m_nSelWeightItem);

	return;
}

//保存配置
VOID CDlgSessionConfig::OnBnClickedBtUpdateBasicConfig()
{
	//游戏配置
	TCHAR  szBasicConfig[4000] = {0};
	GetDlgItemText(IDC_EDIT_BASIC_CONFIG, szBasicConfig, CountArray(szBasicConfig));

	//压缩配置
	TCHAR szCompressConfig[4000] = {0};
	if (!CompressJsonConfig(szBasicConfig, szCompressConfig, CountArray(szCompressConfig)))
	{
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("基础配置内容过长！"));
	
		return;
	}

	//保存配置
	if (UpdateBasicConfig(m_wKindID, szCompressConfig))
	{
		EnableControls(FALSE);
	}
}

//保存权重
VOID CDlgSessionConfig::OnBnClickedBtUpdateNormalWeight()
{
	//变量定义
	WORD wItemCount = 0;
	tagTimesWeightItem TimesWeightItem[128] = {};

	//变量定义
	wItemCount = CollectWeightItem(&m_ListNormalWeight, CountArray(TimesWeightItem), TimesWeightItem);

	//保存权重表
	if (UpdateWeightTable(m_wKindID, TimesWeightItem, wItemCount))
	{
		EnableControls(FALSE);
	}
}

//保存权重
VOID CDlgSessionConfig::OnBnClickedBtUpdateWhiteListWeight()
{
	//变量定义
	WORD wItemCount = 0;
	tagTimesWeightItem TimesWeightItem[128] = {};

	//变量定义
	wItemCount = CollectWeightItem(&m_ListWhiteListWeight, CountArray(TimesWeightItem), TimesWeightItem);

	//保存权重表
	if (UpdateWeightTable(m_wKindID, TimesWeightItem, wItemCount))
	{
		EnableControls(FALSE);
	}
}

//列表完成
VOID CDlgSessionConfig::OnEventListFinish()
{
	//变量定义
	POSITION Position = NULL;
	CGameKindItem* pGameKindItem = NULL;

	//清空数据
	while (m_cbGameList.GetCount()>0) m_cbGameList.DeleteString(0);

	do
	{
		pGameKindItem = m_pServerListManager->EmunGameKindItem(Position);
		if (pGameKindItem == NULL) break;

		//插入新项
		m_cbGameList.SetItemData(m_cbGameList.AddString(pGameKindItem->m_GameKind.szKindName), pGameKindItem->m_GameKind.wKindID);		

	} while (Position != NULL);
}

//查询库存
bool CDlgSessionConfig::QueryGameConfig(WORD wKindID)
{
	//构造对象
	CMD_GP_QueryGameConfig QueryGameConfig = {};

	//设置类型
	QueryGameConfig.wKindID = wKindID;

	//发送消息
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_CONFIG_SERVICE, SUB_GP_QUERY_GAME_CONFIG, &QueryGameConfig,sizeof(QueryGameConfig));
}

//保存基础配置
bool CDlgSessionConfig::UpdateBasicConfig(WORD wKindID, LPCTSTR pszBasiceConfig)
{
	//构造对象
	CMD_GP_UpdateBasicConfig UpdateBasicConfig = {};

	//设置类型
	UpdateBasicConfig.wKindID = wKindID;
	StringCchCopy(UpdateBasicConfig.szBasicConfig,CountArray(UpdateBasicConfig.szBasicConfig),pszBasiceConfig);

	//数据大小
	WORD wDataSize = CountStringBuffer(UpdateBasicConfig.szBasicConfig);
	wDataSize += sizeof(UpdateBasicConfig) - sizeof(UpdateBasicConfig.szBasicConfig);	

	//发送消息
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_CONFIG_SERVICE, SUB_GP_UPDATE_BASIC_CONFIG, &UpdateBasicConfig, wDataSize);
}

//更新权重表
bool CDlgSessionConfig::UpdateWeightTable(WORD wKindID, tagTimesWeightItem TimesWeightItem[], WORD wItemCount)
{
	//构造对象
	CMD_GP_UpdateWeightConfig UpdateWeightConfig = {};

	//设置类型
	UpdateWeightConfig.wKindID = wKindID;
	UpdateWeightConfig.wItemCount = wItemCount;
	UpdateWeightConfig.cbTypeID = TimesWeightItem[0].cbWeightType;
	CopyMemory(UpdateWeightConfig.TimesWeightItem, TimesWeightItem,sizeof(UpdateWeightConfig.TimesWeightItem));

	//数据大小
	WORD wDataSize = sizeof(UpdateWeightConfig) - sizeof(UpdateWeightConfig.TimesWeightItem);
	wDataSize += UpdateWeightConfig.wItemCount*sizeof(UpdateWeightConfig.TimesWeightItem[0]);

	//发送消息
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_CONFIG_SERVICE, SUB_GP_UPDATE_WEIGHT_CONFIG, &UpdateWeightConfig, wDataSize);
}

//////////////////////////////////////////////////////////////////////////////////
