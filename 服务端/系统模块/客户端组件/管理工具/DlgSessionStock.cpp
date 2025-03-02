#include "StdAfx.h"
#include "Resource.h"
#include "DlgSessionStock.h"

//////////////////////////////////////////////////////////////////////////////////

//操作模式
#define OPERATE_MODE_APPEND			1									//新增模式
#define OPERATE_MODE_MODIFY			2									//编辑模式
#define OPERATE_MODE_READONLY		0									//只读模式

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgSessionStock, CDlgSessionItem)
	//ON_WM_CTLCOLOR()	
	ON_BN_CLICKED(IDC_BT_CONFIRM, OnBnClickedBtConfirm)
	ON_BN_CLICKED(IDC_BT_QUERY_STOCK, OnBnClickedBtQueryStock)
	ON_BN_CLICKED(IDC_BT_APPEND_STOCK, OnBnClickedBtAppendStock)
	ON_BN_CLICKED(IDC_BT_MODIFY_STOCK, OnBnClickedBtModifyStock)
	ON_BN_CLICKED(IDC_BT_DELETE_STOCK, OnBnClickedBtDeleteStock)

	ON_BN_CLICKED(IDC_BT_APPEND_JACKPOT, OnBnClickedBtAppendJackpot)
	ON_BN_CLICKED(IDC_BT_MODIFY_JACKPOT, OnBnClickedBtModifyJackpot)
	ON_BN_CLICKED(IDC_BT_DELETE_JACKPOT, OnBnClickedBtDeleteJackpot)

	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST_STOCK_INFO, OnLvnListDeleteStockItem)
	ON_NOTIFY(NM_CLICK, IDC_LIST_STOCK_INFO, OnHdnListClickStockItem)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_STOCK_INFO, OnLvnListKeydownStockItem)

	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST_JACKPOT_INFO, OnLvnListDeleteJackpotItem)
	ON_NOTIFY(NM_CLICK, IDC_LIST_JACKPOT_INFO, OnHdnListClickJackpotItem)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_JACKPOT_INFO, OnLvnListKeydownJackpotItem)

	ON_WM_DESTROY()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//时间转时间戳
time_t SystemTime2Timet(const SYSTEMTIME& st)
{
	struct tm gm = { st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth - 1, st.wYear - 1900, st.wDayOfWeek, 0, 0 };

	return mktime(&gm);

}

//时间戳转时间
SYSTEMTIME Timet2SystemTime(time_t time)
{
	tm t;
	localtime_s(&t,&time);
	SYSTEMTIME systime{ t.tm_year+1900,t.tm_mon+1,0,t.tm_mday,t.tm_hour,t.tm_min,t.tm_sec,0};

	return systime;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgSessionStock::CDlgSessionStock() : CDlgSessionItem(IDD_DLG_STOCK)
{
	//设置变量
	m_nSelStockItem = -1;
	m_nSelJackpotItem = -1;

	//操作变量
	m_wJackpotCount = 0;
	m_cbOperateMode = OPERATE_MODE_READONLY;
	ZeroMemory(m_JackpotItem,sizeof(m_wJackpotCount));

	return;
}

//析构函数
CDlgSessionStock::~CDlgSessionStock()
{
}

//控件绑定
VOID CDlgSessionStock::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//绑定控件
	DDX_Control(pDX, IDC_COMBO_GAME_KIND, m_cbGameList);
	DDX_Control(pDX, IDC_COMBO_STOCK_LEVEL, m_cbStockLevel);

	//绑定控件
	DDX_Control(pDX, IDC_BT_CONFIRM, m_btConfirm);
	DDX_Control(pDX, IDC_BT_QUERY_STOCK, m_btQueryStock);
	DDX_Control(pDX, IDC_BT_APPEND_STOCK, m_btAppendStock);
	DDX_Control(pDX, IDC_BT_MODIFY_STOCK, m_btModifyStock);
	DDX_Control(pDX, IDC_BT_DELETE_STOCK, m_btDeleteStock);

	//按钮控件
	DDX_Control(pDX, IDC_BT_APPEND_JACKPOT, m_btAppendJackpot);
	DDX_Control(pDX, IDC_BT_MODIFY_JACKPOT, m_btModifyJackpot);
	DDX_Control(pDX, IDC_BT_DELETE_JACKPOT, m_btDeleteJackpot);

	//列表配置
	DDX_Control(pDX, IDC_LIST_STOCK_INFO, m_StockList);
	DDX_Control(pDX, IDC_LIST_JACKPOT_INFO, m_JackpotList);
}

VOID CDlgSessionStock::OnDestory()
{
	//删除子项
	m_StockList.DeleteAllItems();
}

//配置函数
BOOL CDlgSessionStock::OnInitDialog()
{
	__super::OnInitDialog();

	//初始化控件
	InitControls();

	return FALSE;
}

//确定函数
VOID CDlgSessionStock::OnOK()
{
	__super::OnOK();
}

//消息解释
BOOL CDlgSessionStock::PreTranslateMessage(MSG * pMsg)
{
	//按键过虑
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//添加修改
VOID CDlgSessionStock::AddModifyJackpotInfo(bool bModify)
{
	//库存信息
	tagJackpotItem JackpotItem;
	ZeroMemory(&JackpotItem, sizeof(JackpotItem));

	//选择判断
	if (m_cbOperateMode == OPERATE_MODE_READONLY) return;
	if (bModify == true && m_nSelJackpotItem == -1) return;

	//校验彩池数量
	if (!bModify && m_JackpotList.GetItemCount() >= MAX_JACKPOT_COUNT)
	{
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("彩池数量已达到最大值！"));

		return;
	}

	//变量定义
	CHAR szScoreValue[18] = { 0 };

	//彩池信息
	JackpotItem.wShrinkRatio = GetDlgItemInt(IDC_EDIT_SHRINK_RATIO);
	JackpotItem.wPayoutProb = GetDlgItemInt(IDC_EDIT_PAYOUT_PROB);

	//当前彩金
	GetDlgItemTextA(m_hWnd, IDC_EDIT_JACKPOT_SCORE, szScoreValue, CountArray(szScoreValue));
	JackpotItem.lStockScore = ScoreZoomin(szScoreValue);

	//虚拟彩金
	GetDlgItemTextA(m_hWnd, IDC_EDIT_VIRTUAL_SCORE, szScoreValue, CountArray(szScoreValue));
	JackpotItem.lVirtualScore = ScoreZoomin(szScoreValue);

	//额外配额
	GetDlgItemTextA(m_hWnd, IDC_EDIT_JACKPOT_EXTRA_SCORE, szScoreValue, CountArray(szScoreValue));
	JackpotItem.lExtraScore = ScoreZoomin(szScoreValue);

	//派彩限制
	GetDlgItemTextA(m_hWnd, IDC_EDIT_PAYOUT_LIMIT, szScoreValue, CountArray(szScoreValue));
	JackpotItem.lPayoutLimit = ScoreZoomin(szScoreValue);

	//派彩起点
	GetDlgItemTextA(m_hWnd, IDC_EDIT_PAYOUT_ORIGIN, szScoreValue, CountArray(szScoreValue));
	JackpotItem.lPayoutOrigin = ScoreZoomin(szScoreValue);	

	//新增彩池
	if (bModify == false)
	{
		//设置级别
		JackpotItem.wLevelID = m_wJackpotCount + 1;	
		CopyMemory(&m_JackpotItem[m_wJackpotCount++], &JackpotItem, sizeof(JackpotItem));

		//变量定义
		LVITEM lVItem;
		lVItem.mask = LVIF_TEXT;
		lVItem.iSubItem = 0;
		lVItem.pszText = TEXT("");
		lVItem.iItem = m_JackpotList.GetItemCount();

		//插入子项.
		auto nItemIndex = m_JackpotList.InsertItem(&lVItem);
		
		//设置子项
		SetJackpotItemToList(nItemIndex, &JackpotItem);
		
	}
	else
	{
		//彩池等级
		JackpotItem.wLevelID = GetDlgItemInt(IDC_EDIT_JACKPOT_LEVELID);	
		CopyMemory(&m_JackpotItem[m_nSelJackpotItem], &JackpotItem, sizeof(JackpotItem));

		//设置子项
		SetJackpotItemToList(m_nSelJackpotItem, &JackpotItem);
	}	

	return;
}

//选中子项
VOID CDlgSessionStock::SelectStockItem(int nIndex)
{
	//校验参数
	if (nIndex==-1 || nIndex >= m_StockList.GetItemCount())
	{
		return;
	}

	//设置变量
	m_nSelStockItem = nIndex;	

	//获取数据
	auto pStockInfo = (tagStockInfo*)m_StockList.GetItemData(m_nSelStockItem);
	if (pStockInfo == NULL) return;

	//设置模式
	m_cbOperateMode = OPERATE_MODE_READONLY;	

	//变量定义
	TCHAR szScoreString[21] = { 0 };

	//库存标识
	SetDlgItemInt(IDC_EDIT_STOCK_ID,pStockInfo->wStockID);

	//库存类型
	SetDlgItemInt(IDC_EDIT_STOCK_KIND,pStockInfo->wStockKind);

	//库存级别
	for (int i = 0; i < m_cbStockLevel.GetCount();i++)
	{
		if (m_cbStockLevel.GetItemData(i) == pStockInfo->wStockLevel)
		{
			m_cbStockLevel.SetCurSel(i);
			break;
		}
	}

	//库存名称
	SetDlgItemText(IDC_EDIT_STOCK_NAME, pStockInfo->szStockName);

	//库存状态
	LPCTSTR pszStockState[] = { TEXT("未启用"),TEXT("启用中"),TEXT("已锁定") };
	if (pStockInfo->cbStockState <= CountArray(pszStockState))
	{
		SetDlgItemText(IDC_EDIT_STOCK_STATE, pszStockState[pStockInfo->cbStockState]);
	}

	//游戏名称
	auto pComboGameName = (CComboBox*)GetDlgItem(IDC_COMBO_GAME_NAME);
	pComboGameName->SetCurSel(-1);
	for (int i = 0; i < pComboGameName->GetCount(); i++)
	{
		if (pComboGameName->GetItemData(i) == pStockInfo->wKindID)
		{
			pComboGameName->SetCurSel(i);
			break;
		}
	}

	//下注金额
	_sntprintf_s(szScoreString,CountArray(szScoreString),TEXT("%0.2f"), ScoreZoomout(pStockInfo->lBetAmount));
	SetDlgItemText(IDC_EDIT_BET_AMOUNT, szScoreString);

	//当前库存
	_sntprintf_s(szScoreString, CountArray(szScoreString), TEXT("%0.2f"), ScoreZoomout(pStockInfo->lStockScore));
	SetDlgItemText(IDC_EDIT_STOCK_SCORE, szScoreString);

	//额外配额
	_sntprintf_s(szScoreString, CountArray(szScoreString), TEXT("%0.2f"), ScoreZoomout(pStockInfo->lExtraScore));
	SetDlgItemText(IDC_EDIT_EXTRA_SCORE, szScoreString);

	//启用模式
	INT nCheckButton = pStockInfo->cbEnableMode == STOCK_ENABLE_MODE_AUTO ? IDC_RADIO_AUTO_MODE : IDC_RADIO_MANUAL_MODE;
	CheckRadioButton(IDC_RADIO_AUTO_MODE, IDC_RADIO_MANUAL_MODE, nCheckButton);

	//启用时间
	if (pStockInfo->cbEnableMode == STOCK_ENABLE_MODE_MANUAL)
	{
		GetDlgItem(IDC_DATE_END_DATE)->EnableWindow(true);
		GetDlgItem(IDC_DATE_END_TIME)->EnableWindow(true);
		GetDlgItem(IDC_DATE_START_DATE)->EnableWindow(true);
		GetDlgItem(IDC_DATE_START_TIME)->EnableWindow(true);

		//转换时间
		auto EnableEndTime = Timet2SystemTime(pStockInfo->dwEnableEndTime);
		auto EnableStartTime = Timet2SystemTime(pStockInfo->dwEnableStartTime);

		//设置时间
		((CDateTimeCtrl*)GetDlgItem(IDC_DATE_END_DATE))->SetTime(&EnableEndTime);
		((CDateTimeCtrl*)GetDlgItem(IDC_DATE_END_TIME))->SetTime(&EnableEndTime);
		((CDateTimeCtrl*)GetDlgItem(IDC_DATE_START_DATE))->SetTime(&EnableStartTime);
		((CDateTimeCtrl*)GetDlgItem(IDC_DATE_START_TIME))->SetTime(&EnableStartTime);
	}
	else
	{
		GetDlgItem(IDC_DATE_END_DATE)->EnableWindow(false);
		GetDlgItem(IDC_DATE_END_TIME)->EnableWindow(false);
		GetDlgItem(IDC_DATE_START_DATE)->EnableWindow(false);
		GetDlgItem(IDC_DATE_START_TIME)->EnableWindow(false);
	}

	//抽水配置
	SetDlgItemTextA(m_hWnd,IDC_EDIT_SHRINK_OPTION, pStockInfo->szShrinkOption);

	//关联库存
	SetDlgItemTextA(m_hWnd,IDC_EDIT_RELATION_ID_LIST, pStockInfo->szRelationStockID);

	//彩池配置
	m_JackpotList.DeleteAllItems();

	//彩池信息
	m_wJackpotCount = pStockInfo->wJackpotCount;
	CopyMemory(m_JackpotItem,pStockInfo->GameJackpotItem,sizeof(m_JackpotItem));

	//变量定义
	LVITEM lVItem;
	lVItem.mask = LVIF_TEXT;
	lVItem.iSubItem = 0;
	lVItem.pszText = TEXT("");

	//循环插入
	for (WORD i = 0; i < m_wJackpotCount; i++)
	{
		//获取参数
		auto pJackpotItem = &m_JackpotItem[i];
		if (pJackpotItem == NULL) continue;

		//插入子项.
		lVItem.iItem = i;
		auto nItemIndex = m_JackpotList.InsertItem(&lVItem);
		if (nItemIndex == -1) continue;

		//设置子项
		SetJackpotItemToList(nItemIndex, pJackpotItem);
	}

	//更新状态
	UpdateControlState();
}

//选择彩池
VOID CDlgSessionStock::SelectJackpotItem(int nIndex)
{
	//设置变量
	m_nSelJackpotItem = nIndex;
	if (m_nSelJackpotItem == -1 || m_nSelJackpotItem>=m_wJackpotCount) return;

	//获取彩池
	auto pJackpotItem = &m_JackpotItem[m_nSelJackpotItem];

	//变量定义
	TCHAR szScoreString[18] = { 0 };

	//彩池级别
	SetDlgItemInt(IDC_EDIT_JACKPOT_LEVELID, pJackpotItem->wLevelID);

	//彩池彩金
	_sntprintf_s(szScoreString,CountArray(szScoreString),TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lStockScore));
	SetDlgItemText(IDC_EDIT_JACKPOT_SCORE, szScoreString);

	//彩池初始彩金
	_sntprintf_s(szScoreString, CountArray(szScoreString), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lInitialScore));
	SetDlgItemText(IDC_EDIT_INITIAL_SCORE, szScoreString);

	//虚拟彩金
	_sntprintf_s(szScoreString, CountArray(szScoreString), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lVirtualScore));
	SetDlgItemText(IDC_EDIT_VIRTUAL_SCORE, szScoreString);

	//额外配置
	_sntprintf_s(szScoreString, CountArray(szScoreString), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lExtraScore));
	SetDlgItemText(IDC_EDIT_JACKPOT_EXTRA_SCORE, szScoreString);

	//抽水比例
	SetDlgItemInt(IDC_EDIT_SHRINK_RATIO, pJackpotItem->wShrinkRatio);

	//派彩概率
	SetDlgItemInt(IDC_EDIT_PAYOUT_PROB, pJackpotItem->wPayoutProb);

	//派彩限制
	_sntprintf_s(szScoreString, CountArray(szScoreString), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lPayoutLimit));
	SetDlgItemText(IDC_EDIT_PAYOUT_LIMIT, szScoreString);

	//派彩起点
	_sntprintf_s(szScoreString, CountArray(szScoreString), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lPayoutOrigin));
	SetDlgItemText(IDC_EDIT_PAYOUT_ORIGIN, szScoreString);
}

//设置子项
VOID CDlgSessionStock::SetStockItemToList(int nItemIndex, tagStockInfo* pStockInfo)
{
	//参数校验
	ASSERT (nItemIndex != -1 && pStockInfo != NULL);
	if (nItemIndex == -1 || pStockInfo == NULL) return;

	//变量定义
	TCHAR szItemValue[32] = TEXT("");

	//库存ID
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pStockInfo->wStockID);
	m_StockList.SetItemText(nItemIndex, 0, szItemValue);

	//库存类型
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pStockInfo->wStockKind);
	m_StockList.SetItemText(nItemIndex, 1, szItemValue);

	//库存状态
	LPCTSTR pszStockState[] = { TEXT("未启用"),TEXT("启用中"),TEXT("已锁定") };
	if (pStockInfo->cbStockState <= CountArray(pszStockState))
	{
		m_StockList.SetItemText(nItemIndex, 2, pszStockState[pStockInfo->cbStockState]);
	}

	//库存等级
	LPCTSTR szStockLevel[] = { TEXT("BET库存"),TEXT("游戏库存"), TEXT("机组库存"), TEXT("活动库存") };
	if (pStockInfo->wStockLevel >= STOCK_LEVEL_BET && pStockInfo->wStockLevel <= STOCK_LEVEL_ACTIVITY)
	{
		m_StockList.SetItemText(nItemIndex, 3, szStockLevel[pStockInfo->wStockLevel-1]);
	}

	//库存名称
	m_StockList.SetItemText(nItemIndex, 4, pStockInfo->szStockName);

	//库存名称
	auto pGameKind = m_pServerListManager->SearchGameKind(pStockInfo->wKindID);
	if (pGameKind != NULL)
	{
		m_StockList.SetItemText(nItemIndex, 5, pGameKind->m_GameKind.szKindName);
	}

	//下注金额
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.2f"), ScoreZoomout(pStockInfo->lBetAmount));
	m_StockList.SetItemText(nItemIndex, 6, szItemValue);

	//当前库存
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.2f"), ScoreZoomout(pStockInfo->lStockScore));
	m_StockList.SetItemText(nItemIndex, 7, szItemValue);

	//抽水利润
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.2f"), ScoreZoomout(pStockInfo->lShrinkProfit));
	m_StockList.SetItemText(nItemIndex, 8, szItemValue);

	//额外配额
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.2f"), ScoreZoomout(pStockInfo->lExtraScore));
	m_StockList.SetItemText(nItemIndex, 9, szItemValue);	

	//启用模式
	m_StockList.SetItemText(nItemIndex, 10, pStockInfo->cbEnableMode==STOCK_ENABLE_MODE_AUTO?TEXT("自动"):TEXT("手动"));

	//抽水配置
	m_StockList.SetItemText(nItemIndex, 11, CA2CT(pStockInfo->szShrinkOption).m_psz);

	//关联库存
	m_StockList.SetItemText(nItemIndex, 12, CA2CT(pStockInfo->szRelationStockID).m_psz);	
}

//设置子项
VOID CDlgSessionStock::SetJackpotItemToList(int nItemIndex, tagJackpotItem* pJackpotItem)
{
	//变量定义
	TCHAR szItemValue[32] = TEXT("");

	//彩池索引
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pJackpotItem->wLevelID);
	m_JackpotList.SetItemText(nItemIndex, 0, szItemValue);

	//当前彩金
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lStockScore));
	m_JackpotList.SetItemText(nItemIndex, 1, szItemValue);

	//初始彩金
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lInitialScore));
	m_JackpotList.SetItemText(nItemIndex, 2, szItemValue);

	//虚拟彩金
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lVirtualScore));
	m_JackpotList.SetItemText(nItemIndex, 3, szItemValue);

	//额外配额
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lExtraScore));
	m_JackpotList.SetItemText(nItemIndex, 4, szItemValue);

	//抽水比例
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pJackpotItem->wShrinkRatio);
	m_JackpotList.SetItemText(nItemIndex, 5, szItemValue);

	//派彩概率
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pJackpotItem->wPayoutProb);
	m_JackpotList.SetItemText(nItemIndex, 6, szItemValue);

	//派彩限制
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lPayoutLimit));
	m_JackpotList.SetItemText(nItemIndex, 7, szItemValue);

	//派彩起点
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lPayoutOrigin));
	m_JackpotList.SetItemText(nItemIndex, 8, szItemValue);
}

//删除库存
bool CDlgSessionStock::DeleteStockInfo(WORD wStockID)
{
	//获取对象
	CMD_GP_DeleteStockInfo DeleteStockInfo = {};

	//设置变量
	DeleteStockInfo.wStockID = wStockID;

	//发送数据
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_STOCK_SERVICE, SUB_GP_DELETE_STOCK_INFO, &DeleteStockInfo, sizeof(DeleteStockInfo));
}

//查询库存
bool CDlgSessionStock::QueryStockInfo(LPCTSTR pszKindIDList)
{
	//获取对象
	CMD_GP_QueryStockInfo QueryParameter = {};

	//设置变量
	StringCchCopy(QueryParameter.szKindID, CountArray(QueryParameter.szKindID), pszKindIDList);

	//发送数据
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_STOCK_SERVICE, SUB_GP_QUERY_STOCK_INFO, &QueryParameter, sizeof(QueryParameter));
}

//修改库存
bool CDlgSessionStock::ModifyStockInfo(tagStockInfo* pStockInfo)
{
	//获取对象
	CMD_GP_ModifyStockInfo ModifyStockInfo = {};

	//设置变量
	CopyMemory(&ModifyStockInfo.StockInfo, pStockInfo, sizeof(ModifyStockInfo.StockInfo));

	//发送数据
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_STOCK_SERVICE, SUB_GP_MODIFY_STOCK_INFO, &ModifyStockInfo, sizeof(ModifyStockInfo));
}

//添加库存
bool CDlgSessionStock::AppendStockInfo(tagStockInfo* pStockInfo)
{
	//获取对象
	CMD_GP_AppendStockInfo AppendStockInfo = {};

	//设置变量
	CopyMemory(&AppendStockInfo.StockInfo, pStockInfo, sizeof(AppendStockInfo.StockInfo));

	//发送数据
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_STOCK_SERVICE, SUB_GP_APPEND_STOCK_INFO, &AppendStockInfo, sizeof(AppendStockInfo));
}

//调整库存
bool CDlgSessionStock::AdjustStockScore(WORD wStockID, SCORE lChangeScore)
{
	//获取对象
	CMD_GP_AdjustStockScore AdjustStockScore = {};

	//设置变量
	AdjustStockScore.wStockID = wStockID;
	AdjustStockScore.lChangeScore = lChangeScore;

	//发送数据
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_STOCK_SERVICE, SUB_GP_ADJUST_STOCK_SCORE, &AdjustStockScore, sizeof(AdjustStockScore));
}

//调整彩金
bool CDlgSessionStock::AdjustJackpotScore(WORD wStockID, WORD wLevelID, SCORE lChangeScore)
{
	//获取对象
	CMD_GP_AdjustJackpotScore AdjustJackpotScore = {};

	//设置变量
	AdjustJackpotScore.wStockID = wStockID;
	AdjustJackpotScore.wLevelID = wLevelID;
	AdjustJackpotScore.lChangeScore = lChangeScore;

	//发送数据
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_STOCK_SERVICE, SUB_GP_ADJUST_JACKPOT_SCORE, &AdjustJackpotScore, sizeof(AdjustJackpotScore));
}

//读取事件
bool CDlgSessionStock::OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	//库存服务
	if (Command.wMainCmdID == MDM_GP_STOCK_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_STOCK_OPERATE_RESULT:		//操作结果
		{
			//参数校验
			ASSERT(wDataSize <= sizeof(CMD_GP_StockOperateResult));
			if (wDataSize > sizeof(CMD_GP_StockOperateResult)) return false;

			//提取变量
			auto pOperateResult = (CMD_GP_StockOperateResult*)pData;

			switch (pOperateResult->wSubCommdID)
			{
			case SUB_GP_QUERY_STOCK_INFO:	//查询库存
			{
				//变量定义
				int nItemIndex = -1;
				tagStockInfo* pItemData = NULL;
				tagStockInfo* pTempStockInfo = NULL;

				//清空数据
				if (pOperateResult->wPacketIdx == 0)
				{
					m_StockList.DeleteAllItems();
				}

				//变量定义
				LVITEM lVItem;
				lVItem.mask = LVIF_TEXT;
				lVItem.iSubItem = 0;
				lVItem.pszText = TEXT("");

				//循环插入
				for (WORD wIndex = 0; wIndex < pOperateResult->wStockCount; wIndex++)
				{
					//获取参数
					pTempStockInfo = &pOperateResult->StockList[wIndex];
					if (pTempStockInfo == NULL) continue;

					//插入子项.
					lVItem.iItem = m_StockList.GetItemCount();
					nItemIndex = m_StockList.InsertItem(&lVItem);
					if (nItemIndex == -1) continue;

					//设置子项
					SetStockItemToList(nItemIndex, pTempStockInfo);

					//申请资源
					pItemData = new tagStockInfo;
					if (pItemData != NULL)
					{
						//拷贝数据
						CopyMemory(pItemData, pTempStockInfo, sizeof(tagStockInfo));

						//设置数据
						m_StockList.SetItemData(nItemIndex, (DWORD_PTR)pItemData);
					}
				}

				//默认选择
				SelectStockItem(0);

				break;
			}
			case SUB_GP_APPEND_STOCK_INFO:		//添加库存
			{
				//变量定义
				int nItemIndex = -1;

				//变量定义
				LVITEM lVItem;
				lVItem.mask = LVIF_TEXT;
				lVItem.iSubItem = 0;
				lVItem.pszText = TEXT("");

				//插入子项
				lVItem.iItem = m_StockList.GetItemCount();
				nItemIndex = m_StockList.InsertItem(&lVItem);

				//获取库存
				auto pStockInfo = &pOperateResult->StockList[0];

				//设置子项
				SetStockItemToList(nItemIndex, pStockInfo);

				//申请资源
				auto pItemData = new tagStockInfo;
				if (pItemData != NULL)
				{
					//拷贝数据
					CopyMemory(pItemData, pStockInfo, sizeof(tagStockInfo));

					//设置数据
					m_StockList.SetItemData(nItemIndex, (DWORD_PTR)pItemData);
				}

				//选择库存
				SelectStockItem(nItemIndex);

				break;
			}
			case SUB_GP_MODIFY_STOCK_INFO:		//修改库存
			{
				//变量定义
				int nItemIndex = -1;

				//获取库存
				auto pStockInfo = &pOperateResult->StockList[0];

				//查找对象
				for (int nIndex = 0; nIndex < m_StockList.GetItemCount(); nIndex++)
				{
					//获取数据
					auto pItemData = (tagStockInfo*)m_StockList.GetItemData(nIndex);
					if (pItemData == NULL) continue;

					if (pItemData->wStockID == pStockInfo->wStockID)
					{
						//拷贝数据
						CopyMemory(pItemData, pStockInfo, sizeof(tagStockInfo));

						//设置子项
						SetStockItemToList(nIndex, pStockInfo);

						//设置索引
						nItemIndex = nIndex;

						break;
					}

				}

				//选择库存
				SelectStockItem(m_nSelStockItem);

				break;
			}
			case SUB_GP_DELETE_STOCK_INFO:		//删除库存
			{
				//获取库存
				auto pStockInfo = &pOperateResult->StockList[0];

				//查找对象
				for (int nIndex = 0; nIndex < m_StockList.GetItemCount(); nIndex++)
				{
					auto pItemData = (tagStockInfo*)m_StockList.GetItemData(nIndex);
					if (pItemData == NULL) continue;

					if (pItemData->wStockID == pStockInfo->wStockID)
					{
						//设置子项
						m_StockList.DeleteItem(nIndex);

						break;
					}

				}

				//选择库存
				SelectStockItem(m_nSelStockItem);

				break;
			}
			}

			return true;
		}
		}
	}
	
	//操作失败
	if (Command.wMainCmdID == MDM_GP_USER_SERVICE && Command.wSubCmdID==SUB_GP_OPERATE_FAILURE)
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

	return false;
}

//缩小分数
float CDlgSessionStock::ScoreZoomout(SCORE lScore)
{
	return static_cast<float>(lScore/1000.f);
}

//放大分数
SCORE CDlgSessionStock::ScoreZoomin(LPCSTR pszScore)
{
	auto fScore = atof(pszScore);
	return static_cast<SCORE>(fScore*1000);
}

//初始化控件
VOID CDlgSessionStock::InitControls()
{
	//设置控件
	DWORD dwExstyle = m_StockList.GetExtendedStyle();
	//m_StockList.SetExtendedStyle(dwExstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | WS_EX_STATICEDGE);

	//列表控件
	m_StockList.InsertColumn(0, TEXT("ID"), LVCFMT_LEFT, 30);
	m_StockList.InsertColumn(1, TEXT("库存类型"), LVCFMT_LEFT, 60);
	m_StockList.InsertColumn(2, TEXT("库存状态"), LVCFMT_LEFT, 60);
	m_StockList.InsertColumn(3, TEXT("库存级别"), LVCFMT_LEFT, 60);
	m_StockList.InsertColumn(4, TEXT("库存名称"), LVCFMT_LEFT, 80);
	m_StockList.InsertColumn(5, TEXT("游戏名称"), LVCFMT_LEFT, 80);
	m_StockList.InsertColumn(6, TEXT("下注金额"), LVCFMT_LEFT, 60);
	m_StockList.InsertColumn(7, TEXT("当前库存"), LVCFMT_LEFT, 60);
	m_StockList.InsertColumn(8, TEXT("抽水利润"), LVCFMT_LEFT, 60);
	m_StockList.InsertColumn(9, TEXT("额外配额"), LVCFMT_LEFT, 60);
	m_StockList.InsertColumn(10, TEXT("启用模式"), LVCFMT_LEFT, 60);
	m_StockList.InsertColumn(11, TEXT("抽水配置"), LVCFMT_LEFT, 120);
	m_StockList.InsertColumn(12, TEXT("关联库存"), LVCFMT_LEFT, 120);

	//设置控件
	dwExstyle = m_JackpotList.GetExtendedStyle();
	m_JackpotList.SetExtendedStyle(dwExstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | WS_EX_STATICEDGE);

	//列表控件
	m_JackpotList.InsertColumn(0, TEXT("彩池索引"), LVCFMT_CENTER, 60);
	m_JackpotList.InsertColumn(1, TEXT("当前彩金"), LVCFMT_CENTER, 60);
	m_JackpotList.InsertColumn(2, TEXT("初始彩金"), LVCFMT_CENTER, 60);
	m_JackpotList.InsertColumn(3, TEXT("虚拟彩金"), LVCFMT_CENTER, 60);
	m_JackpotList.InsertColumn(4, TEXT("额外配额"), LVCFMT_CENTER, 80);
	m_JackpotList.InsertColumn(5, TEXT("抽水比例"), LVCFMT_CENTER, 80);
	m_JackpotList.InsertColumn(6, TEXT("派彩概率"), LVCFMT_CENTER, 60);
	m_JackpotList.InsertColumn(7, TEXT("派彩限制"), LVCFMT_CENTER, 60);
	m_JackpotList.InsertColumn(8, TEXT("派彩起点"), LVCFMT_CENTER, 60);
	
	//启用模式
	CheckRadioButton(IDC_RADIO_AUTO_MODE, IDC_RADIO_MANUAL_MODE, IDC_RADIO_AUTO_MODE);

	//库存解绑
	WORD wStockLevel[] = { STOCK_LEVEL_BET,STOCK_LEVEL_GAME,STOCK_LEVEL_GROUP,STOCK_LEVEL_ACTIVITY };
	LPCTSTR szStockLevel[] = { TEXT("BET库存"),TEXT("游戏库存"), TEXT("机组库存"), TEXT("活动库存") };		
	for (int i = 0; i < CountArray(szStockLevel); i++)
	{
		m_cbStockLevel.SetItemData(m_cbStockLevel.InsertString(i, szStockLevel[i]),wStockLevel[i]);
	}	
}

//更新状态
VOID CDlgSessionStock::UpdateControlState()
{
	//只读模式
	if (m_cbOperateMode == OPERATE_MODE_READONLY)
	{
		//按钮控件
		m_btConfirm.EnableWindow(FALSE);
		m_btQueryStock.EnableWindow(TRUE);
		m_btAppendStock.EnableWindow(TRUE);
		m_btModifyStock.EnableWindow(TRUE);
		m_btDeleteStock.EnableWindow(TRUE);
		m_btAppendJackpot.EnableWindow(FALSE);
		m_btModifyJackpot.EnableWindow(FALSE);
		m_btDeleteJackpot.EnableWindow(FALSE);

		//其他控件
		GetDlgItem(IDC_DATE_END_DATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_DATE_END_TIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_DATE_START_DATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_DATE_START_TIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_GAME_NAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_STOCK_LEVEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_AUTO_MODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_MANUAL_MODE)->EnableWindow(FALSE);

		//库存控件
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_ID))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_KIND))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_STATE))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_NAME))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_BET_AMOUNT))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_SCORE))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_EXTRA_SCORE))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_SHRINK_OPTION))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_RELATION_ID_LIST))->SetReadOnly(TRUE);		

		//彩池控件
		((CEdit*)GetDlgItem(IDC_EDIT_JACKPOT_LEVELID))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_JACKPOT_SCORE))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_INITIAL_SCORE))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_VIRTUAL_SCORE))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_JACKPOT_EXTRA_SCORE))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_SHRINK_RATIO))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_PAYOUT_PROB))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_PAYOUT_LIMIT))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_PAYOUT_ORIGIN))->SetReadOnly(TRUE);		
	}	

	//新增模式
	if (m_cbOperateMode == OPERATE_MODE_APPEND)
	{
		//按钮控件
		m_btConfirm.EnableWindow(TRUE);
		m_btQueryStock.EnableWindow(TRUE);
		m_btAppendStock.EnableWindow(FALSE);
		m_btModifyStock.EnableWindow(FALSE);
		m_btDeleteStock.EnableWindow(FALSE);
		m_btAppendJackpot.EnableWindow(TRUE);
		m_btModifyJackpot.EnableWindow(TRUE);
		m_btDeleteJackpot.EnableWindow(TRUE);

		//其他控件
		GetDlgItem(IDC_DATE_END_DATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_DATE_END_TIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_DATE_START_DATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_DATE_START_TIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_GAME_NAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_STOCK_LEVEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_AUTO_MODE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_MANUAL_MODE)->EnableWindow(TRUE);

		//库存控件
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_ID))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_KIND))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_STATE))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_NAME))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_BET_AMOUNT))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_SCORE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_EXTRA_SCORE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_SHRINK_OPTION))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_RELATION_ID_LIST))->SetReadOnly(FALSE);

		//彩池控件
		((CEdit*)GetDlgItem(IDC_EDIT_JACKPOT_LEVELID))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_JACKPOT_SCORE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_INITIAL_SCORE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_VIRTUAL_SCORE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_JACKPOT_EXTRA_SCORE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_SHRINK_RATIO))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_PAYOUT_PROB))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_PAYOUT_LIMIT))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_PAYOUT_ORIGIN))->SetReadOnly(FALSE);

		//设置控件
		SetDlgItemText(IDC_EDIT_STOCK_ID,TEXT(""));
		SetDlgItemText(IDC_EDIT_STOCK_STATE, TEXT("未启用"));
	}

	//编辑模式
	if (m_cbOperateMode == OPERATE_MODE_MODIFY)
	{
		//按钮控件
		m_btConfirm.EnableWindow(TRUE);
		m_btQueryStock.EnableWindow(TRUE);
		m_btAppendStock.EnableWindow(FALSE);
		m_btModifyStock.EnableWindow(FALSE);
		m_btDeleteStock.EnableWindow(FALSE);
		m_btAppendJackpot.EnableWindow(TRUE);
		m_btModifyJackpot.EnableWindow(TRUE);
		m_btDeleteJackpot.EnableWindow(TRUE);

		//其他控件
		GetDlgItem(IDC_DATE_END_DATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_DATE_END_TIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_DATE_START_DATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_DATE_START_TIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_GAME_NAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_STOCK_LEVEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_AUTO_MODE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_MANUAL_MODE)->EnableWindow(TRUE);

		//库存控件
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_ID))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_KIND))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_BET_AMOUNT))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_STATE))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_NAME))->SetReadOnly(FALSE);		
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_SCORE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_EXTRA_SCORE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_SHRINK_OPTION))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_RELATION_ID_LIST))->SetReadOnly(FALSE);

		//彩池控件
		((CEdit*)GetDlgItem(IDC_EDIT_JACKPOT_LEVELID))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_JACKPOT_SCORE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_INITIAL_SCORE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_VIRTUAL_SCORE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_JACKPOT_EXTRA_SCORE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_SHRINK_RATIO))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_PAYOUT_PROB))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_PAYOUT_LIMIT))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_PAYOUT_ORIGIN))->SetReadOnly(FALSE);
	}
}

//控件使能
VOID CDlgSessionStock::EnableControls(BOOL bEnabled)
{
	//查询状态
	m_btQueryStock.EnableWindow(bEnabled);
}

//单击子项
VOID CDlgSessionStock::OnHdnListClickStockItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	//选中子项
	SelectStockItem(phdr->iItem);

	*pResult = 0;
}

//删除子项
VOID CDlgSessionStock::OnLvnListDeleteStockItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	//获取数据
	auto pItemData = (tagStockInfo*)m_StockList.GetItemData(pNMLV->iItem);
	if (pItemData != NULL)
	{
		SafeDelete(pItemData);
	}

	*pResult = 0;
}

//键盘按下
VOID CDlgSessionStock::OnLvnListKeydownStockItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	//变量定义
	int nItem = -1;
	POSITION pos;

	switch (pLVKeyDow->wVKey)
	{
	case VK_DOWN:
	{
		pos = m_StockList.GetFirstSelectedItemPosition();
		while (pos)
		{
			//获取子项
			nItem = m_StockList.GetNextSelectedItem(pos);
			if (nItem == -1) break;

			//设置子项
			nItem = __min(nItem + 1, m_StockList.GetItemCount() - 1);
			SelectStockItem(nItem);
			break;
		}

		break;
	}
	case VK_UP:
	{
		pos = m_StockList.GetFirstSelectedItemPosition();
		while (pos)
		{
			//获取子项
			nItem = m_StockList.GetNextSelectedItem(pos);
			if (nItem == -1) break;

			//设置子项
			nItem = __max(nItem - 1, 0);
			SelectStockItem(nItem);
			break;
		}
		break;
	}
	}

	*pResult = 0;
}

//单击子项
VOID CDlgSessionStock::OnHdnListClickJackpotItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	//选中子项
	SelectJackpotItem(phdr->iItem);

	*pResult = 0;
}

//删除子项
VOID CDlgSessionStock::OnLvnListDeleteJackpotItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
}

//键盘按下
VOID CDlgSessionStock::OnLvnListKeydownJackpotItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	//变量定义
	int nItem = -1;
	POSITION pos;

	switch (pLVKeyDow->wVKey)
	{
	case VK_DOWN:
	{
		pos = m_JackpotList.GetFirstSelectedItemPosition();
		while (pos)
		{
			//获取子项
			nItem = m_JackpotList.GetNextSelectedItem(pos);
			if (nItem == -1) break;

			//设置子项
			nItem = __min(nItem + 1, m_JackpotList.GetItemCount() - 1);
			SelectJackpotItem(nItem);
			break;
		}

		break;
	}
	case VK_UP:
	{
		pos = m_JackpotList.GetFirstSelectedItemPosition();
		while (pos)
		{
			//获取子项
			nItem = m_JackpotList.GetNextSelectedItem(pos);
			if (nItem == -1) break;

			//设置子项
			nItem = __max(nItem - 1, 0);
			SelectJackpotItem(nItem);
			break;
		}
		break;
	}
	}

	*pResult = 0;
}

//添加子项
VOID CDlgSessionStock::OnBnClickedBtAppendStock()
{
	//设置变量
	m_wJackpotCount = 0;
	m_cbOperateMode = OPERATE_MODE_APPEND;	
	ZeroMemory(&m_JackpotItem,sizeof(m_JackpotItem));

	//清空彩池
	m_JackpotList.DeleteAllItems();

	//更新状态
	UpdateControlState();	
}

//编辑子项
VOID CDlgSessionStock::OnBnClickedBtModifyStock()
{
	//获取索引
	if (m_nSelStockItem == -1) return;

	//获取库存
	auto pStockInfo = (tagStockInfo*)m_StockList.GetItemData(m_nSelStockItem);
	if (pStockInfo == NULL) return;

	//校验状态
	if (pStockInfo->cbStockState == STOCK_STATE_LOCKED)
	{
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("当前库存锁定中不允许修改！"));

		return;
	}

	//设置模式
	m_cbOperateMode = OPERATE_MODE_MODIFY;

	//更新状态
	UpdateControlState();	
}

//删除子项
VOID CDlgSessionStock::OnBnClickedBtDeleteStock()
{
	//获取索引
	if (m_nSelStockItem == -1) return;

	//调整选项
	if (m_StockList.GetItemCount() <= m_nSelStockItem)
	{
		m_nSelStockItem = -1;

		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("请选择您要删除的库存！"));

		return;
	}

	//获取数据
	auto pStockInfo = (tagStockInfo*)m_StockList.GetItemData(m_nSelStockItem);
	if (pStockInfo == NULL) return;

	//校验状态
	if (pStockInfo->cbStockState == STOCK_STATE_LOCKED)
	{
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("当前库存锁定中不允许删除！"));

		return;
	}

	//消息提示
	CInformation Information;
	if (Information.ShowMessageBox(TEXT("提示"), TEXT("您确定要删除当前库存吗？"), MB_OKCANCEL)!=IDOK)
	{
		return;
	}	

	//删除库存
	DeleteStockInfo(pStockInfo->wStockID);
	
	//设置变量
	m_nSelStockItem = -1;
	m_cbOperateMode = OPERATE_MODE_READONLY;

	//更新控件
	UpdateControlState();

	return;
}

//添加彩池
VOID CDlgSessionStock::OnBnClickedBtAppendJackpot()
{
	AddModifyJackpotInfo(false);
}

//编辑彩池
VOID CDlgSessionStock::OnBnClickedBtModifyJackpot()
{
	AddModifyJackpotInfo(true);
}

//删除彩池
VOID CDlgSessionStock::OnBnClickedBtDeleteJackpot()
{
	//获取索引
	if (m_nSelJackpotItem == -1) return;

	//调整选项
	if (m_JackpotList.GetItemCount() <= m_nSelJackpotItem)
	{
		m_nSelJackpotItem = -1;

		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("请选择您要删除的彩池信息！"));

		return;
	}

	//调整选项
	if (m_nSelJackpotItem != m_JackpotList.GetItemCount()-1)
	{
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("彩金池只能依次删除最后一个！"));

		return;
	}

	//获取数据
	auto pItemData = (tagStockInfo*)m_StockList.GetItemData(m_nSelStockItem);
	if (pItemData == NULL) return;

	//删除彩池
	if (m_nSelJackpotItem < m_wJackpotCount)
	{
		m_wJackpotCount--;
		ZeroMemory(&m_JackpotItem[m_nSelJackpotItem],sizeof(m_nSelJackpotItem));
	}

	//删除彩池
	m_JackpotList.DeleteItem(m_nSelJackpotItem);

	return;
}

//列表完成
VOID CDlgSessionStock::OnEventListFinish()
{
	//变量定义
	POSITION Position = NULL;
	CGameKindItem* pGameKindItem = NULL;

	//插入游戏
	TCHAR szItemString[64] = { 0 };
	auto pComboGameName = (CComboBox*)GetDlgItem(IDC_COMBO_GAME_NAME);

	//清空数据
	while (m_cbGameList.GetCount()) m_cbGameList.DeleteString(0);
	while (pComboGameName->GetCount()) pComboGameName->DeleteString(0);	

	do
	{
		pGameKindItem = m_pServerListManager->EmunGameKindItem(Position);
		if (pGameKindItem == NULL) break;

		//插入新项
		tagItemData* pItemData = (tagItemData*)m_cbGameList.GetItemDataPtr(m_cbGameList.AddString(pGameKindItem->m_GameKind.szKindName));
		if (pItemData != NULL) pItemData->dwDataPtr = pGameKindItem->m_GameKind.wKindID;
		
		if (pComboGameName != NULL)
		{
			pComboGameName->SetItemData(pComboGameName->AddString(pGameKindItem->m_GameKind.szKindName), pGameKindItem->m_GameKind.wKindID);
		}

	} while (Position != NULL);

	//默认游戏
	tagItemData* pItemData = (tagItemData*)m_cbGameList.GetItemDataPtr(m_cbGameList.InsertString(0, TEXT("")));
	if (pItemData != NULL) pItemData->dwDataPtr = 0;

	//默认游戏
	if (pComboGameName != NULL)
	{
		pComboGameName->SetItemData(pComboGameName->InsertString(0, TEXT("")), 0);
	}
}

//确认操作
VOID CDlgSessionStock::OnBnClickedBtConfirm()
{
	//只读模式
	if (m_cbOperateMode == OPERATE_MODE_READONLY)
	{
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("没有任何修改，无需确认！"));

		return;
	}

	//编辑模式
	if (m_cbOperateMode == OPERATE_MODE_MODIFY && m_nSelStockItem == -1)
	{
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("请选择需要编辑的库存！"));

		return;
	}

	//变量定义
	TCHAR szMessage[128] = TEXT("");

	//库存信息
	tagStockInfo StockInfo;
	ZeroMemory(&StockInfo, sizeof(StockInfo));

	//游戏名称
	auto pCombGameName = (CComboBox*)GetDlgItem(IDC_COMBO_GAME_NAME);

	//校验类型
	if (pCombGameName->GetCurSel() == -1)
	{
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("游戏名称不能为空！"));

		return;
	}

	//校验类型
	if (m_cbStockLevel.GetCurSel() == -1)
	{
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("库存等级不能为空！"));

		return;
	}

	//变量定义
	CHAR szScoreValue[18] = { 0 };

	//库存信息
	StockInfo.wStockID = GetDlgItemInt(IDC_EDIT_STOCK_ID);
	StockInfo.wStockKind = GetDlgItemInt(IDC_EDIT_STOCK_KIND);

	//库存级别
	if (m_cbStockLevel.GetCurSel() != -1)
	{
		StockInfo.wStockLevel = m_cbStockLevel.GetItemData(m_cbStockLevel.GetCurSel());
	}		

	//下注金额
	GetDlgItemTextA(m_hWnd, IDC_EDIT_BET_AMOUNT, szScoreValue, CountArray(szScoreValue));
	StockInfo.lBetAmount = ScoreZoomin(szScoreValue);

	//额外配额
	GetDlgItemTextA(m_hWnd, IDC_EDIT_EXTRA_SCORE, szScoreValue, CountArray(szScoreValue));
	StockInfo.lExtraScore = ScoreZoomin(szScoreValue);

	//当前库存
	GetDlgItemTextA(m_hWnd, IDC_EDIT_STOCK_SCORE, szScoreValue, CountArray(szScoreValue));
	StockInfo.lStockScore = ScoreZoomin(szScoreValue);

	//类型标识
	StockInfo.wKindID = (WORD)pCombGameName->GetItemData(pCombGameName->GetCurSel());

	//库存名称
	GetDlgItemText(IDC_EDIT_STOCK_NAME, StockInfo.szStockName, CountArray(StockInfo.szStockName));

	//Json配置
	GetDlgItemTextA(m_hWnd, IDC_EDIT_SHRINK_OPTION, StockInfo.szShrinkOption, CountArray(StockInfo.szShrinkOption));
	GetDlgItemTextA(m_hWnd, IDC_EDIT_RELATION_ID_LIST, StockInfo.szRelationStockID, CountArray(StockInfo.szRelationStockID));

	//启动模式
	if (GetCheckedRadioButton(IDC_RADIO_AUTO_MODE, IDC_RADIO_MANUAL_MODE) == IDC_RADIO_AUTO_MODE)
	{
		StockInfo.cbEnableMode = STOCK_ENABLE_MODE_AUTO;
	}
	else
	{
		StockInfo.cbEnableMode = STOCK_ENABLE_MODE_MANUAL;

		//停用时间
		auto pCtrlEndDate = (CDateTimeCtrl*)GetDlgItem(IDC_DATE_END_DATE);
		auto pCtrlEndTime = (CDateTimeCtrl*)GetDlgItem(IDC_DATE_END_TIME);

		//启用时间
		auto pCtrlStartDate = (CDateTimeCtrl*)GetDlgItem(IDC_DATE_START_DATE);
		auto pCtrlStartTime = (CDateTimeCtrl*)GetDlgItem(IDC_DATE_START_TIME);

		//时间定义
		SYSTEMTIME	EndDate, EndTime; 
		SYSTEMTIME	StartDate, StartTime;		
		SYSTEMTIME	StartDateTime, EndDateTime;

		//获取时间
		pCtrlEndDate->GetTime(&EndDate);
		pCtrlEndTime->GetTime(&EndTime);
		pCtrlStartDate->GetTime(&StartDate);
		pCtrlStartTime->GetTime(&StartTime);

		//合并结束时间
		EndDateTime.wYear = EndDate.wYear;
		EndDateTime.wMonth = EndDate.wMonth;
		EndDateTime.wDay = EndDate.wDay;
		EndDateTime.wDayOfWeek = EndDate.wDayOfWeek;
		EndDateTime.wHour = EndTime.wHour;
		EndDateTime.wMinute = EndTime.wMinute;
		EndDateTime.wSecond = EndTime.wSecond;
		EndDateTime.wMilliseconds = EndTime.wMilliseconds;

		//合并开始时间
		StartDateTime.wYear = StartDate.wYear;
		StartDateTime.wMonth = StartDate.wMonth;
		StartDateTime.wDay = StartDate.wDay;
		StartDateTime.wDayOfWeek = StartDate.wDayOfWeek;
		StartDateTime.wHour = StartTime.wHour;
		StartDateTime.wMinute = StartTime.wMinute;
		StartDateTime.wSecond = StartTime.wSecond;
		StartDateTime.wMilliseconds = StartTime.wMilliseconds;

		//获取时区
		StockInfo.dwEnableEndTime = SystemTime2Timet(EndDateTime);
		StockInfo.dwEnableStartTime = SystemTime2Timet(StartDateTime);		
	}

	//校验模式
	if (StockInfo.wStockLevel == 1 && StockInfo.cbEnableMode != STOCK_ENABLE_MODE_AUTO)
	{
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("BET库存只支持自动启用模式！"));

		return;
	}

	//校验模式
	if (StockInfo.wStockLevel > 1 && StockInfo.cbEnableMode == STOCK_ENABLE_MODE_AUTO)
	{
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("非BET库存只支持手动启用模式！"));

		return;
	}

	//彩池信息
	StockInfo.wJackpotCount = m_wJackpotCount;
	CopyMemory(StockInfo.GameJackpotItem, m_JackpotItem, sizeof(StockInfo.GameJackpotItem));

	//新增库存
	if (m_cbOperateMode == OPERATE_MODE_APPEND)
	{
		//添加库存
		AppendStockInfo(&StockInfo);
	}
	else
	{
		//修改库存
		ModifyStockInfo(&StockInfo);

		//库存调整		
		auto pStockInfo = (tagStockInfo*)m_StockList.GetItemData(m_nSelStockItem);
		if (pStockInfo != NULL)
		{
			//库存变化
			if (StockInfo.lStockScore != pStockInfo->lStockScore)
			{
				AdjustStockScore(StockInfo.wStockID, StockInfo.lStockScore- pStockInfo->lStockScore);
			}

			//彩金变化
			for (int i = 0; i < StockInfo.wJackpotCount; i++)
			{
				for (int j = 0; j < pStockInfo->wJackpotCount; j++)
				{
					if (StockInfo.GameJackpotItem[i].wLevelID == pStockInfo->GameJackpotItem[j].wLevelID)
					{
						if (StockInfo.GameJackpotItem[i].lStockScore != pStockInfo->GameJackpotItem[j].lStockScore)
						{
							AdjustJackpotScore(StockInfo.wStockID, StockInfo.GameJackpotItem[i].wLevelID, StockInfo.GameJackpotItem[i].lStockScore - pStockInfo->GameJackpotItem[j].lStockScore);
						}
					}
				}				
			}
		}
	}	

	return;
}

//查询参数
VOID CDlgSessionStock::OnBnClickedBtQueryStock()
{
	//数组定义
	WORD wKindCount = 0;
	WORD wKindIDList[100];	

	//收集类型标识
	for (INT i = 0; i < m_cbGameList.GetCount(); i++)
	{
		if (m_cbGameList.GetCheck(i) == static_cast<BOOL>(true))
		{
			//提取数据
			tagItemData* pItemData = (tagItemData*)m_cbGameList.GetItemData(i);
			wKindIDList[wKindCount++] = (WORD)pItemData->dwDataPtr;
		}
	}

	//校验数量
	if (wKindCount > 0)
	{
		//格式化
		CString strKindIDList = TEXT("[");
		for (int i = 0; i < wKindCount; i++)
		{
			if (i > 0) strKindIDList.AppendChar(',');

			strKindIDList.AppendFormat(TEXT("%d"), wKindIDList[i]);
		}
		strKindIDList.AppendChar(']');

		//查询库存
		QueryStockInfo(strKindIDList.GetString());
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////
