#include "pch.h"
#include "Stdafx.h"
#include "AnalyseToolDlg.h"

//////////////////////////////////////////////////////////////////////////

LPCTSTR szGameName[] = { TEXT("Bingo"),TEXT("Buffalo777"),TEXT("Firelink"),TEXT("LifeOfLuxury"),TEXT("Vegas777"), 
TEXT("埃及拉霸"),TEXT("多福多财"),TEXT("疯狂的猴子"),TEXT("豪华水果连线"),TEXT("九线拉王"),
TEXT("森林拉霸"),TEXT("水果玛丽"),TEXT("糖果派对") };

LPCTSTR szGameServer[] = { TEXT("BingoServer"),TEXT("Buffalo777Server"),TEXT("FirelinkServer"),TEXT("LifeOfLuxuryServer"),TEXT("Vegas777Server"),
TEXT("EgyptLabaServer"),TEXT("WealthServer"),TEXT("MadMonkeyServer"),TEXT("LuxuryFruitServer"),TEXT("Line9Server"),
TEXT("ForestLabaServer"),TEXT("FruitsCasinoServer"),TEXT("CandyPartyServer") };

BEGIN_MESSAGE_MAP(CAnalyseToolDlg, CDialog)

	//按钮消息
	ON_BN_CLICKED(IDC_CHECK_ALL_RESULTS, OnBnQueryAllResults)
	ON_BN_CLICKED(IDC_QUERY_ALL_INVENTORY, OnBnQueryAllInventory)
	ON_BN_CLICKED(IDC_GAME_STAR, OnBnGameStar)
	ON_BN_CLICKED(IDC_QUERY, OnBnQueryGameWinRate)

	ON_NOTIFY(NM_CLICK, IDC_RESULTS_SHOW, OnListShowsItemclick)
	ON_NOTIFY(NM_CLICK, IDC_INVENTORY_SHOWS, OnListInventoryItemclick)

	ON_CBN_SELCHANGE(IDC_COMBO_GAME_KIND, OnCbnSelchangeKindList)
	ON_CBN_SELCHANGE(IDC_EDIT_GAME_ID, OnCbnSelchangeUserList)
	ON_CBN_SELCHANGE(IDC_COMBO_GAME_TYPE, OnCbnSelchangeTypeList)

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_RESULTS_SHOW, OnLvnItemchangedResultsShow)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_INVENTORY_SHOWS, OnLvnItemchangedInventoryShows)
	ON_BN_CLICKED(IDC_PREVIOUS_PAGE, OnBnQueryPreviousPage)
	ON_BN_CLICKED(IDC_NEXT_PAGE, OnBnQueryNextPage)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CAnalyseToolDlg::CAnalyseToolDlg() : CDialog(IDD_ANALYSETOOL_DIALOG)
{
	//设置变量
	m_lUserID = 0;
	m_lUserNumber = 0;
	m_lGameNumber = 0;
	m_lCurrentNumber = 1;
	m_lSumNumber = 0;
	//配置信息
	m_wUserCount = 0;
	ZeroMemory(m_lUserId, sizeof(m_lUserId));
	ZeroMemory(m_szModuleName, sizeof(m_szModuleName));
	ZeroMemory(m_szGameName, sizeof(m_szGameName));

	return;
}

//接口查询
VOID* CAnalyseToolDlg::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	return NULL;
}

//析构函数
CAnalyseToolDlg::~CAnalyseToolDlg()
{
	//设置变量
	m_lUserID = 0;
	m_lUserNumber = 0;
	m_lGameNumber = 0;
	m_lCurrentNumber = 1;
	m_lSumNumber = 0;

	//配置信息
	m_wUserCount = 0;
	ZeroMemory(m_szModuleName, sizeof(m_szModuleName));
	ZeroMemory(m_szGameName, sizeof(m_szGameName));
}
//控件绑定
void CAnalyseToolDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RESULTS_SHOW, m_ResultsShow);
	DDX_Control(pDX, IDC_INVENTORY_SHOWS, m_InventoryShows);

}

//创建函数
BOOL CAnalyseToolDlg::OnInitDialog()
{
	__super::OnInitDialog();

	SetDlgItemText(IDC_MODULE_NAME, TEXT("BingoServer"));
	SetDlgItemInt(IDC_CURRENT_NUMBER, m_lCurrentNumber);

	m_ResultsShow.EnableWindow(FALSE);
	m_InventoryShows.EnableWindow(FALSE);

	//初始化控件
	OnBnSearchUser();
	InitControls();

	return TRUE;
}

//初始化控件
VOID CAnalyseToolDlg::InitControls()
{
	//设置控件
	DWORD dwTmpExstyle = m_InventoryShows.GetExtendedStyle();
	m_InventoryShows.SetExtendedStyle(dwTmpExstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	//列表控件
	m_InventoryShows.InsertColumn(0, TEXT("局数"), LVCFMT_CENTER, 60);
	m_InventoryShows.InsertColumn(1, TEXT("系统库存"), LVCFMT_CENTER, 110);
	m_InventoryShows.InsertColumn(2, TEXT("玩家库存"), LVCFMT_CENTER, 110);
	m_InventoryShows.InsertColumn(3, TEXT("调节系数"), LVCFMT_CENTER, 110);
	m_InventoryShows.InsertColumn(4, TEXT("强制区间"), LVCFMT_CENTER, 110);
	m_InventoryShows.InsertColumn(5, TEXT("库存重置次数"), LVCFMT_CENTER, 115);
	m_InventoryShows.InsertColumn(6, TEXT("累积库存统计"), LVCFMT_CENTER, 115);

	//设置控件
	DWORD dwExstyle = m_ResultsShow.GetExtendedStyle();
	m_ResultsShow.SetExtendedStyle(dwExstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	//列表控件
	m_ResultsShow.InsertColumn(0, TEXT("用户ID"), LVCFMT_CENTER, 60);
	m_ResultsShow.InsertColumn(1, TEXT("游戏名称"), LVCFMT_CENTER, 75);
	m_ResultsShow.InsertColumn(2, TEXT("局数"), LVCFMT_CENTER, 40);
	m_ResultsShow.InsertColumn(3, TEXT("初始金币"), LVCFMT_CENTER, 75);
	m_ResultsShow.InsertColumn(4, TEXT("当前金币"), LVCFMT_CENTER, 75);
	m_ResultsShow.InsertColumn(5, TEXT("游戏下注"), LVCFMT_CENTER, 75);
	m_ResultsShow.InsertColumn(6, TEXT("游戏得分"), LVCFMT_CENTER, 75);
	m_ResultsShow.InsertColumn(7, TEXT("本局输赢"), LVCFMT_CENTER, 75);
	m_ResultsShow.InsertColumn(8, TEXT("胜利局数"), LVCFMT_CENTER, 60);
	m_ResultsShow.InsertColumn(9, TEXT("胜率"), LVCFMT_CENTER, 60);
	m_ResultsShow.InsertColumn(10, TEXT("返奖率"), LVCFMT_CENTER, 60);

	//库存
	CComboBox* pComboName = (CComboBox*)GetDlgItem(IDC_COMBO_GAME_KIND);
	//构造级别
	for (INT i = 0; i < CountArray(szGameName); i++)
	{
		WORD wServerLevel = i + 1;
		pComboName->SetItemData(pComboName->InsertString(i, szGameName[i]), wServerLevel);
	}

	//默认选择
	pComboName->SetCurSel(0);

	//胜率
	CComboBox* pComboGameName = (CComboBox*)GetDlgItem(IDC_COMBO_GAME_TYPE);
	//构造级别
	for (INT i = 0; i < CountArray(szGameName); i++)
	{
		WORD wServerLevel = i + 1;
		pComboGameName->SetItemData(pComboGameName->InsertString(i, szGameName[i]), wServerLevel);
	}

	//默认选择
	pComboGameName->SetCurSel(0);

	CString strName = szGameServer[0];
	TCHAR* szModuleName = new TCHAR[128];
	szModuleName = strName.GetBuffer(strName.GetLength());
	StringCchCopy(m_szModuleName, CountArray(m_szModuleName), szModuleName);

	strName = szGameName[0];
	szModuleName = strName.GetBuffer(strName.GetLength());
	StringCchCopy(m_szGameName, CountArray(m_szModuleName), szModuleName);

	OnBnSumNumber();

	return;
}


void CAnalyseToolDlg::OnCbnSelchangeKindList()
{
	//获取控件
	CComboBox* pComboName = (CComboBox*)GetDlgItem(IDC_COMBO_GAME_KIND);
	if (pComboName->GetCurSel() == -1)
	{
		AfxMessageBox(TEXT("请选择需要查看的游戏！"), MB_ICONSTOP);
		return;
	}

	//游戏名称	
	int nGameKind = (WORD)pComboName->GetItemData(pComboName->GetCurSel());
	if (nGameKind > 0)
	{
		CString strGameName = szGameServer[nGameKind-1];
		TCHAR* szModuleName = new TCHAR[128];
		szModuleName = strGameName.GetBuffer(strGameName.GetLength());
		StringCchCopy(m_szModuleName, CountArray(m_szModuleName), szModuleName);
	}
	OnBnSumNumber();
	return;
}

void CAnalyseToolDlg::OnCbnSelchangeTypeList()
{
	//获取控件
	CComboBox* pComboName = (CComboBox*)GetDlgItem(IDC_COMBO_GAME_TYPE);
	if (pComboName->GetCurSel() == -1)
	{
		AfxMessageBox(TEXT("请选择需要查看的游戏！"), MB_ICONSTOP);
		return;
	}

	//游戏名称	
	int nGameKind = (WORD)pComboName->GetItemData(pComboName->GetCurSel());
	if (nGameKind > 0)
	{
		CString strGameName = szGameName[nGameKind - 1];
		TCHAR* szModuleName = new TCHAR[128];
		szModuleName = strGameName.GetBuffer(strGameName.GetLength());
		StringCchCopy(m_szGameName, CountArray(m_szModuleName), szModuleName);
	}
	OnBnSumNumber();
	return;
}

void CAnalyseToolDlg::OnCbnSelchangeUserList()
{
	//获取控件
	CComboBox* pComboName = (CComboBox*)GetDlgItem(IDC_EDIT_GAME_ID);
	if (pComboName->GetCurSel() == -1)
	{
		AfxMessageBox(TEXT("请选择需要查看的游戏ID！"), MB_ICONSTOP);
		return;
	}

	//游戏ID
	SCORE wUserID = (WORD)pComboName->GetItemData(pComboName->GetCurSel());
	if (wUserID > 0)
	{
		m_lUserID = m_lUserId[wUserID - 1];
	}
	
	return;
}

//单击子项
VOID CAnalyseToolDlg::OnListShowsItemclick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	//选中子项
	SelectItemShows(phdr->iItem);

	*pResult = 0;
}

//选中子项
VOID CAnalyseToolDlg::SelectItemShows(int nIndex)
{
	//设置变量
	if (nIndex == -1) return;
	if (nIndex >= m_ResultsShow.GetItemCount()) return;

	//获取数据	
	auto pItemData = (GameInfo*)m_ResultsShow.GetItemData(nIndex);
	if (pItemData == NULL) return;

	OnBnQueryResults(pItemData);
}

//单击子项
VOID CAnalyseToolDlg::OnListInventoryItemclick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	//选中子项
	SelectItemInventory(phdr->iItem);

	*pResult = 0;
}

//选中子项
VOID CAnalyseToolDlg::SelectItemInventory(int nIndex)
{
	//设置变量
	if (nIndex == -1) return;
	if (nIndex >= m_InventoryShows.GetItemCount()) return;

	//获取数据
	auto pItemData = (GameDebugInfo*)m_InventoryShows.GetItemData(nIndex);
	if (pItemData == NULL) return;

	OnBnShowInventory(pItemData);
}

//选择变更
VOID CAnalyseToolDlg::OnLvnItemchangedResultsShow(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	//选中子项
	SelectItemShows(pNMLV->iItem);

	*pResult = 0;
}

//子项变更
VOID CAnalyseToolDlg::OnLvnItemchangedInventoryShows(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	//选中子项
	SelectItemInventory(pNMLV->iItem);

	*pResult = 0;
}

//消息解释
BOOL CAnalyseToolDlg::PreTranslateMessage(MSG* pMsg)
{
	//按键过虑
	if (pMsg->message == WM_KEYDOWN)
	{
		if ((pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN)) return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//查询战绩
VOID CAnalyseToolDlg::OnBnQueryResults(GameInfo* pItemData)
{
	if (pItemData == NULL) return;
	
	SetDlgItemInt(IDC_EDIT_CONFIG_PLAYER_ID, pItemData->wUserId);
	DDX_DOUBLE(IDC_FIRST_SCORE, (pItemData->lFirstScore));
	DDX_DOUBLE(IDC_CURRENT_SCORE, (pItemData->lUserScore));
	DDX_DOUBLE(IDC_USER_BET, (pItemData->lUserBetTotalScore));
	DDX_DOUBLE(IDC_GAME_SCORE, (pItemData->lUserTotalGameScore));
	DDX_DOUBLE(IDC_BUREAU_WINNING, (pItemData->lGameScore));
	SetDlgItemInt(IDC_VICTORY, (pItemData->lVictory));
	SetDlgItemInt(IDC_WIN_RATE, (pItemData->lWinRate));
	SetDlgItemInt(IDC_SLIPPER_RATE, (pItemData->lSlipperRate));
	SetDlgItemInt(IDC_ROUNDS_PLAYED, pItemData->lSelectGame);
	DDX_DOUBLE(IDC_SUM_WINNING, (pItemData->lSumWinning));
	return;
}

static int  CB_sqlite_count(void* data, int argc, char** argv, char** pszColName)
{
	for (int i = 0; i < argc; i++)
	{
		if (argv == NULL)
		{
			return 0;
		}
		*((int*)data) = atoi(argv[i]);
	}
	return 0;
}


//查询全部战绩
VOID CAnalyseToolDlg::OnBnQueryAllResults()
{
	//m_InventoryShows.Clear();
	m_ResultsShow.EnableWindow(TRUE);
	//清除子项
	m_ResultsShow.DeleteAllItems();
	sqlite3* db;
	int nRes = 0;
	//打开数据库
	string strName = OnGetPath();
	nRes = sqlite3_open(strName.c_str(), &db);
	if (nRes != SQLITE_OK) {
		//数据库打开失败
		return;
	}

	char strSql[256] = {};
	sprintf_s(strSql, "SELECT * from ServerData where UserId = '%d';", m_lUserID);
	char** dbResult;
	int nRow, nColumn = 0;
	int index = 0;
	nRes = sqlite3_get_table(db, strSql, &dbResult, &nRow, &nColumn, NULL);
	//查询成功
	GameInfo *UserRecord = NULL;		//玩家游戏记录
	if (SQLITE_OK == 0)
	{
		index = nColumn;
		for (int i = 0; i < nRow; i++)
		{		
			//申请资源
			UserRecord = new GameInfo;
			UserRecord->wUserId = atoi(dbResult[index]);
			UserRecord->sGameName = (dbResult[index+1]);
			UserRecord->lSelectGame = i + 1;
			UserRecord->lFirstScore = (atoi(dbResult[index + 3]));
			UserRecord->lUserScore = (atoi(dbResult[index + 4]));
			UserRecord->lUserBetTotalScore = (atoi(dbResult[index + 5]));
			UserRecord->lUserTotalGameScore = (atoi(dbResult[index + 6]));
			UserRecord->lGameScore = (atoi(dbResult[index + 7]));
			UserRecord->lVictory = atoi(dbResult[index + 8]);
			UserRecord->lWinRate = atoi(dbResult[index + 9]);
			UserRecord->lSlipperRate = atoi(dbResult[index + 10]);
			UserRecord->lSumWinning = UserRecord->lUserScore - UserRecord->lFirstScore;
			
			//变量定义
			int nItemIndex = -1;

			//变量定义
			LVITEM lVItem;
			lVItem.mask = LVIF_TEXT;
			lVItem.iSubItem = 0;
			lVItem.pszText = TEXT("");

			//插入子项.
			lVItem.iItem = i;
			nItemIndex = m_ResultsShow.InsertItem(&lVItem);
			if (nItemIndex == -1) continue;

			for (int Tmpindex = 0; Tmpindex < 11; Tmpindex++)
			{
				if (nItemIndex == -1) return;

				//变量定义
				TCHAR szItemValue[32] = TEXT("");

				//设置子项
				if (Tmpindex == 1)
				{
					//中文格式转换
					CString csName;
					string sName = dbResult[index + Tmpindex];
					int len = MultiByteToWideChar(CP_UTF8, 0, sName.data(), -1, NULL, 0);
					WCHAR* strNew = new WCHAR[len + 1];
					MultiByteToWideChar(CP_UTF8, 0, sName.data(), -1, strNew, len);
					sName.clear();
					sName = CW2A(strNew);
					delete[] strNew;
					csName = sName.c_str();
					m_ResultsShow.SetItemText(nItemIndex, Tmpindex, csName);
				}
				else if (Tmpindex == 2)
				{
					_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), i+1);
					m_ResultsShow.SetItemText(nItemIndex, Tmpindex, szItemValue);
				}
				else if (Tmpindex > 2 && Tmpindex < 8)
				{
					DOUBLE	lScore = atoi(dbResult[index + Tmpindex]);
					_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%.2lf"), SCORE_ZOOMOUT(lScore));
					m_ResultsShow.SetItemText(nItemIndex, Tmpindex, szItemValue);
				}
				else if (Tmpindex > 8 && Tmpindex < 11)
				{
					_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d%%"), atoi(dbResult[index + Tmpindex]));
					m_ResultsShow.SetItemText(nItemIndex, Tmpindex, szItemValue);
				}
				else
				{
					_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), atoi(dbResult[index + Tmpindex]));
					m_ResultsShow.SetItemText(nItemIndex, Tmpindex, szItemValue);
				}				
			}
			//设置数据
			m_ResultsShow.SetItemData(nItemIndex, (DWORD_PTR)UserRecord);
			index += 12;
		}
	}
	//关闭数据库
	sqlite3_close(db);

	return;
}

//显示库存
VOID CAnalyseToolDlg::OnBnShowInventory(GameDebugInfo* pItemData)
{	
	DDX_DOUBLE(IDC_EDIT_CONFIG_STORAGE_SYSTEM, (pItemData->lCurSysStorage));
	DDX_DOUBLE(IDC_EDIT_CONFIG_STORAGE_PLAYER, (pItemData->lCurPlayerStorage));
	SetDlgItemInt(IDC_EDIT_CONFIG_STORAGE_PARATERMK, (pItemData->lCurParameterK));
	SetDlgItemInt(IDC_EDIT_CONFIG_STORAGE_FORCE_REST_SECTION, (pItemData->lCurResetSection));
	DDX_DOUBLE(IDC_STATIC_CUR_STORAGE_SYATEM_WINLOSE, pItemData->lCurStorageSyatemWinlose);
	SetDlgItemInt(IDC_STATIC_CUR_STORAGE_RESET_TIMES, (pItemData->lResetTimes));

	return;
}

//查询全部库存
VOID CAnalyseToolDlg::OnBnQueryAllInventory()
{
	m_InventoryShows.EnableWindow(TRUE);
	//清除子项
	m_InventoryShows.DeleteAllItems();

	m_lCurrentNumber = GetDlgItemInt(IDC_CURRENT_NUMBER);
	SetDlgItemInt(IDC_CURRENT_NUMBER, m_lCurrentNumber);
	if (m_lCurrentNumber < 1)
	{
		m_lCurrentNumber = 1;
	}
	OnBnQueryInventory();
	
	return;
}

//查询库存上一页
VOID CAnalyseToolDlg::OnBnQueryPreviousPage()
{
	m_lCurrentNumber = GetDlgItemInt(IDC_CURRENT_NUMBER);
	if (m_lCurrentNumber > 1)
	{
		m_lCurrentNumber -= 1;
	}
	else
	{
		return;
	}
	m_InventoryShows.EnableWindow(TRUE);
	//清除子项
	m_InventoryShows.DeleteAllItems();
	
	SetDlgItemInt(IDC_CURRENT_NUMBER, m_lCurrentNumber);
	OnBnQueryInventory();

	return;
}

//查询库存下一页
VOID CAnalyseToolDlg::OnBnQueryNextPage()
{
	m_lCurrentNumber = GetDlgItemInt(IDC_CURRENT_NUMBER);
	if (m_lCurrentNumber < m_lSumNumber)
	{
		m_lCurrentNumber += 1;
	}
	else
	{
		return;
	}
	m_InventoryShows.EnableWindow(TRUE);
	//清除子项
	m_InventoryShows.DeleteAllItems();
	
	SetDlgItemInt(IDC_CURRENT_NUMBER, m_lCurrentNumber);
	OnBnQueryInventory();

	return;
}

//查询库存
VOID CAnalyseToolDlg::OnBnQueryInventory()
{
	sqlite3* db;
	int nRes = 0;
	//打开数据库
	string strName = OnGetPath();
	nRes = sqlite3_open(strName.c_str(), &db);
	if (nRes != SQLITE_OK) {
		//数据库打开失败
		return;
	}

	USES_CONVERSION;
	char* szBuffer = W2A(m_szModuleName);
	string sTmpName = szBuffer;
	sTmpName = sTmpName + "Stock ";
	char strSql[256] = {};
	sprintf_s(strSql, "select * from %s limit %d ,%d ;", sTmpName.c_str(), (m_lCurrentNumber - 1) * 200,200);

	char** dbResult;
	int nRow, nColumn = 0;
	int index = 0;
	nRes = sqlite3_get_table(db, strSql, &dbResult, &nRow, &nColumn, NULL);
	//查询成功
	GameDebugInfo* DebugRecord = NULL;		//库存
	if (SQLITE_OK == 0)
	{
		index = nColumn;
		for (int i = 0; i < nRow; i++)
		{
			DebugRecord = new GameDebugInfo;
			DebugRecord->lNumbergames = (i + 1)+ (m_lCurrentNumber-1)*200;
			DebugRecord->lCurSysStorage = (atoi(dbResult[index + 1]));
			DebugRecord->lCurPlayerStorage = (atoi(dbResult[index + 2]));
			DebugRecord->lCurParameterK = atoi(dbResult[index + 3]);
			DebugRecord->lCurResetSection = atoi(dbResult[index + 4]);
			DebugRecord->lSysDbgSysWin = atoi(dbResult[index + 5]);
			DebugRecord->lSysDbgPlayerWin = atoi(dbResult[index + 6]);
			DebugRecord->lResetTimes = atoi(dbResult[index + 7]);
			DebugRecord->lCurStorageSyatemWinlose = DebugRecord->lCurSysStorage - DebugRecord->lCurPlayerStorage;
			//变量定义
			int nItemIndex = -1;

			//变量定义
			LVITEM lVItem;
			lVItem.mask = LVIF_TEXT;
			lVItem.iSubItem = 0;
			lVItem.pszText = TEXT("");

			//插入子项.
			lVItem.iItem = i;
			nItemIndex = m_InventoryShows.InsertItem(&lVItem);
			if (nItemIndex == -1) continue;

			for (int Tmpindex = 0; Tmpindex < 10; Tmpindex++)
			{
				if (nItemIndex == -1) return;

				//变量定义
				TCHAR szItemValue[32] = TEXT("");

				//设置子项
				if (Tmpindex == 0)
				{
					_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), DebugRecord->lNumbergames);
					m_InventoryShows.SetItemText(nItemIndex, Tmpindex, szItemValue);
				}
				else if (Tmpindex > 0 && Tmpindex < 3)
				{
					DOUBLE	lScore = atoi(dbResult[index + Tmpindex]);
					_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%.2lf"), SCORE_ZOOMOUT(lScore));
					m_InventoryShows.SetItemText(nItemIndex, Tmpindex, szItemValue);
				}
				else if (Tmpindex > 2 && Tmpindex < 5)
				{
					_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d‰"), atoi(dbResult[index + Tmpindex]));
					m_InventoryShows.SetItemText(nItemIndex, Tmpindex, szItemValue);
				}
				else if (Tmpindex == 7)
				{
					_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), atoi(dbResult[index + Tmpindex]));
					m_InventoryShows.SetItemText(nItemIndex, 5, szItemValue);

					DOUBLE	lScore = (atoi(dbResult[index + 1])) - (atoi(dbResult[index + 2]));
					_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%.2lf"), SCORE_ZOOMOUT(lScore));
					m_InventoryShows.SetItemText(nItemIndex, 6, szItemValue);
				}
			}
			//设置数据
			m_InventoryShows.SetItemData(nItemIndex, (DWORD_PTR)DebugRecord);
			index += 9;
		}
	}
	//关闭数据库
	sqlite3_close(db);

	return;
}

void CAnalyseToolDlg::DDX_DOUBLE(int nIDC, DOUBLE value)
{
	CString cc;
	value = SCORE_ZOOMOUT(value);
	cc.Format(_T("%.2lf"), value);
	SetDlgItemText(nIDC, cc);
}

//查找用户
VOID CAnalyseToolDlg::OnBnSearchUser()
{
	sqlite3* db;
	int nRes = 0;
	//打开数据库
	string strName = OnGetPath();
	nRes = sqlite3_open(strName.c_str(), &db);
	if (nRes != SQLITE_OK) {
		//数据库打开失败
		return ;
	}

	string strSql = "select * from ServerUser order by GameUser desc ;";
	char** dbResult;
	int nRow, nColumn = 0;
	int index = 0;
	nRes = sqlite3_get_table(db, strSql.c_str(), &dbResult, &nRow, &nColumn, NULL);
	if (SQLITE_OK == 0)
	{
		index = nColumn;
		for (int i = 0; i < nRow; i++)
		{
			if (i > 199) break;
			m_lUserId[i] = atoi(dbResult[index + 1]);
			index += 3;
		}
	}

	//关闭数据库
	sqlite3_close(db);

	CComboBox* pComboName = (CComboBox*)GetDlgItem(IDC_EDIT_GAME_ID);
	//构造级别
	for (INT i = 0; i < CountArray(m_lUserId); i++)
	{
		if (m_lUserId[i] == 0) break;
		m_lUserID = m_lUserId[0];
		WORD wServerLevel = i + 1;
		string str = to_string(m_lUserId[i]);
		TCHAR nUserId[128] = {};
		_stprintf_s(nUserId, 128, _T("%S"), str.c_str());
		pComboName->SetItemData(pComboName->InsertString(i, nUserId), wServerLevel);
	}

	//默认选择
	pComboName->SetCurSel(0);

	return;
}

//总页数
VOID CAnalyseToolDlg::OnBnSumNumber()
{
	sqlite3* db;
	int nRes = 0;
	//打开数据库
	string strName = OnGetPath();
	nRes = sqlite3_open(strName.c_str(), &db);
	if (nRes != SQLITE_OK) {
		//数据库打开失败
		return;
	}

	USES_CONVERSION;
	char* szBuffer = W2A(m_szModuleName);
	string sTmpName = szBuffer;
	sTmpName = sTmpName + "Stock ;";
	string strSql = "select count(*)  from " + sTmpName + "; ";

	char** dbResult;
	int nRow, nColumn = 0;
	int index = 0;
	nRes = sqlite3_get_table(db, strSql.c_str(), &dbResult, &nRow, &nColumn, NULL);
	if (SQLITE_OK == 0)
	{
		index = nColumn;
		for (int i = 0; i < nRow; i++)
		{
			m_lSumNumber = atoi(dbResult[index])/200;
			if (atoi(dbResult[index])%200 != 0)
			{
				m_lSumNumber += 1;
			}
			SetDlgItemInt(IDC_SUM_NUMBER, m_lSumNumber);
		}
		if (nRow == 0 )
		{
			SetDlgItemInt(IDC_SUM_NUMBER, 0);
		}
	}

	//关闭数据库
	sqlite3_close(db);
}

//查询游戏胜率
VOID CAnalyseToolDlg::OnBnQueryGameWinRate()
{
	sqlite3* db;
	int nRes = 0;
	//打开数据库
	string strName = OnGetPath();
	nRes = sqlite3_open(strName.c_str(), &db);
	if (nRes != SQLITE_OK) {
		//数据库打开失败
		return;
	}

	GameWinRate RateInfo;
	ZeroMemory(&RateInfo, sizeof(RateInfo));

	USES_CONVERSION;
	char* szBuffer = W2A(m_szGameName);
	string sTmpName = szBuffer;
	//中文格式转换
	int len = WideCharToMultiByte(CP_UTF8, 0, CA2W((char*)sTmpName.data()), -1, NULL, 0, NULL, NULL);
	char* strNew = new char[len + 1];
	WideCharToMultiByte(CP_UTF8, 0, CA2W((char*)sTmpName.data()), -1, strNew, len, NULL, NULL);
	strNew[len] = '\0';
	sTmpName.clear();
	sTmpName = strNew;
	delete[] strNew;
	string strSql = "SELECT * from ServerData where GameName = '" + sTmpName + "' order by SystemTime desc";

	int iCount = 0;
	nRes = sqlite3_exec(db, strSql.c_str(), CB_sqlite_count, &iCount, NULL);
	if (nRes != SQLITE_OK)
	{
		//关闭数据库
		sqlite3_close(db);
		return;
	}

	if (iCount != 0)
	{
		string strTmpSql[6];
		strTmpSql[0] = "select sum(UserBetTotalScore)  from ServerData where GameName = '" + sTmpName + "' order by SystemTime desc ";
		strTmpSql[1] = "select sum(GameScore)  from ServerData where GameName = '" + sTmpName + "' and GameScore > 0 order by SystemTime desc ";
		strTmpSql[2] = "select sum(GameScore)  from ServerData where GameName = '" + sTmpName + "' and GameScore < 0 order by SystemTime desc ";
		strTmpSql[3] = "select sum(UserTotalGameScore)  from ServerData where GameName = '" + sTmpName + "' order by SystemTime desc ";
		strTmpSql[4] = "select count(*)  from ServerData where GameName = '" + sTmpName + "' order by SystemTime desc ";
		strTmpSql[5] = "select count(*)  from ServerData where GameName = '" + sTmpName + "'and GameScore > 0 order by SystemTime desc ";

		for (int i = 0; i < 6; i++)
		{
			int iCount = 0;
			char** dbResult;
			int nRow, nColumn = 0;
			int index = 0;
			nRes = sqlite3_get_table(db, strTmpSql[i].c_str(), &dbResult, &nRow, &nColumn, NULL);
			if (nRes != SQLITE_OK || nRow == 0)
			{
				//关闭数据库
				sqlite3_close(db);
				return;
			}

			if (SQLITE_OK == 0)
			{
				if (i == 0)
				{
					index = nColumn;
					if (dbResult[index] != NULL)
					RateInfo.lSumBetTotal = atoi(dbResult[index]);
				}
				else if (i == 1)
				{
					index = nColumn;
					if (dbResult[index] != NULL)
					RateInfo.lSumWinPoints = atoi(dbResult[index]);
				}
				else if (i == 2)
				{
					index = nColumn;
					if (dbResult[index]!=NULL)
					RateInfo.lSumLosePoints = atoi(dbResult[index]);
					
				}
				else if (i == 3)
				{
					index = nColumn;
					if (dbResult[index] != NULL)
					RateInfo.lSumGameScore = fabs(atoi(dbResult[index]));
				}
				else if (i == 4)
				{
					index = nColumn;
					if (dbResult[index] != NULL)
					RateInfo.lSumPlayed = atoi(dbResult[index]);
				}
				else if (i == 5)
				{
					index = nColumn;
					if (dbResult[index] != NULL)
					RateInfo.lSumSuccess = atoi(dbResult[index]);
				}
			}
		}
	}
	//关闭数据库
	sqlite3_close(db);

	DDX_DOUBLE(IDC_BET_TOTAL, (RateInfo.lSumBetTotal));
	DDX_DOUBLE(IDC_SUM_WIN_POINTS, (RateInfo.lSumWinPoints));
	DDX_DOUBLE(IDC_SUM_LOSE_POINTS, (RateInfo.lSumLosePoints));
	if (RateInfo.lSumBetTotal == 0) RateInfo.lSumBetTotal = 1;
	SetDlgItemInt(IDC_SUM_SLIPPER_RATE, fabs(RateInfo.lSumGameScore * 100 / RateInfo.lSumBetTotal));
	SetDlgItemInt(IDC_SUM_PLAYED, (RateInfo.lSumPlayed));
	SetDlgItemInt(IDC_SUM_SUCCESS, (RateInfo.lSumSuccess));
	SetDlgItemInt(IDC_SUM_LOSE, (RateInfo.lSumPlayed - RateInfo.lSumSuccess));
	if (RateInfo.lSumPlayed == 0) RateInfo.lSumPlayed = 1;
	SetDlgItemInt(IDC_SUM_WIN_RATE, (RateInfo.lSumSuccess*100/ RateInfo.lSumPlayed));
}

//路径
string CAnalyseToolDlg::OnGetPath()
{
	TCHAR nNickName[LEN_NICKNAME] = _T("SqliteData.db");
	int iLen = WideCharToMultiByte(CP_ACP, 0, nNickName, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen * sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, nNickName, -1, chRtn, iLen, NULL, NULL);
	string sName = chRtn;
	string strName = "./SqliteData/" + sName;

	return strName;
}

//关闭
VOID CAnalyseToolDlg::OnBnGameStar()
{
	PostQuitMessage(0);
}