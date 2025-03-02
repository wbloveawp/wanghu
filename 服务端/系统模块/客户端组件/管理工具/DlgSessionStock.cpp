#include "StdAfx.h"
#include "Resource.h"
#include "DlgSessionStock.h"

//////////////////////////////////////////////////////////////////////////////////

//����ģʽ
#define OPERATE_MODE_APPEND			1									//����ģʽ
#define OPERATE_MODE_MODIFY			2									//�༭ģʽ
#define OPERATE_MODE_READONLY		0									//ֻ��ģʽ

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

//ʱ��תʱ���
time_t SystemTime2Timet(const SYSTEMTIME& st)
{
	struct tm gm = { st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth - 1, st.wYear - 1900, st.wDayOfWeek, 0, 0 };

	return mktime(&gm);

}

//ʱ���תʱ��
SYSTEMTIME Timet2SystemTime(time_t time)
{
	tm t;
	localtime_s(&t,&time);
	SYSTEMTIME systime{ t.tm_year+1900,t.tm_mon+1,0,t.tm_mday,t.tm_hour,t.tm_min,t.tm_sec,0};

	return systime;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgSessionStock::CDlgSessionStock() : CDlgSessionItem(IDD_DLG_STOCK)
{
	//���ñ���
	m_nSelStockItem = -1;
	m_nSelJackpotItem = -1;

	//��������
	m_wJackpotCount = 0;
	m_cbOperateMode = OPERATE_MODE_READONLY;
	ZeroMemory(m_JackpotItem,sizeof(m_wJackpotCount));

	return;
}

//��������
CDlgSessionStock::~CDlgSessionStock()
{
}

//�ؼ���
VOID CDlgSessionStock::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//�󶨿ؼ�
	DDX_Control(pDX, IDC_COMBO_GAME_KIND, m_cbGameList);
	DDX_Control(pDX, IDC_COMBO_STOCK_LEVEL, m_cbStockLevel);

	//�󶨿ؼ�
	DDX_Control(pDX, IDC_BT_CONFIRM, m_btConfirm);
	DDX_Control(pDX, IDC_BT_QUERY_STOCK, m_btQueryStock);
	DDX_Control(pDX, IDC_BT_APPEND_STOCK, m_btAppendStock);
	DDX_Control(pDX, IDC_BT_MODIFY_STOCK, m_btModifyStock);
	DDX_Control(pDX, IDC_BT_DELETE_STOCK, m_btDeleteStock);

	//��ť�ؼ�
	DDX_Control(pDX, IDC_BT_APPEND_JACKPOT, m_btAppendJackpot);
	DDX_Control(pDX, IDC_BT_MODIFY_JACKPOT, m_btModifyJackpot);
	DDX_Control(pDX, IDC_BT_DELETE_JACKPOT, m_btDeleteJackpot);

	//�б�����
	DDX_Control(pDX, IDC_LIST_STOCK_INFO, m_StockList);
	DDX_Control(pDX, IDC_LIST_JACKPOT_INFO, m_JackpotList);
}

VOID CDlgSessionStock::OnDestory()
{
	//ɾ������
	m_StockList.DeleteAllItems();
}

//���ú���
BOOL CDlgSessionStock::OnInitDialog()
{
	__super::OnInitDialog();

	//��ʼ���ؼ�
	InitControls();

	return FALSE;
}

//ȷ������
VOID CDlgSessionStock::OnOK()
{
	__super::OnOK();
}

//��Ϣ����
BOOL CDlgSessionStock::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//����޸�
VOID CDlgSessionStock::AddModifyJackpotInfo(bool bModify)
{
	//�����Ϣ
	tagJackpotItem JackpotItem;
	ZeroMemory(&JackpotItem, sizeof(JackpotItem));

	//ѡ���ж�
	if (m_cbOperateMode == OPERATE_MODE_READONLY) return;
	if (bModify == true && m_nSelJackpotItem == -1) return;

	//У��ʳ�����
	if (!bModify && m_JackpotList.GetItemCount() >= MAX_JACKPOT_COUNT)
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("�ʳ������Ѵﵽ���ֵ��"));

		return;
	}

	//��������
	CHAR szScoreValue[18] = { 0 };

	//�ʳ���Ϣ
	JackpotItem.wShrinkRatio = GetDlgItemInt(IDC_EDIT_SHRINK_RATIO);
	JackpotItem.wPayoutProb = GetDlgItemInt(IDC_EDIT_PAYOUT_PROB);

	//��ǰ�ʽ�
	GetDlgItemTextA(m_hWnd, IDC_EDIT_JACKPOT_SCORE, szScoreValue, CountArray(szScoreValue));
	JackpotItem.lStockScore = ScoreZoomin(szScoreValue);

	//����ʽ�
	GetDlgItemTextA(m_hWnd, IDC_EDIT_VIRTUAL_SCORE, szScoreValue, CountArray(szScoreValue));
	JackpotItem.lVirtualScore = ScoreZoomin(szScoreValue);

	//�������
	GetDlgItemTextA(m_hWnd, IDC_EDIT_JACKPOT_EXTRA_SCORE, szScoreValue, CountArray(szScoreValue));
	JackpotItem.lExtraScore = ScoreZoomin(szScoreValue);

	//�ɲ�����
	GetDlgItemTextA(m_hWnd, IDC_EDIT_PAYOUT_LIMIT, szScoreValue, CountArray(szScoreValue));
	JackpotItem.lPayoutLimit = ScoreZoomin(szScoreValue);

	//�ɲ����
	GetDlgItemTextA(m_hWnd, IDC_EDIT_PAYOUT_ORIGIN, szScoreValue, CountArray(szScoreValue));
	JackpotItem.lPayoutOrigin = ScoreZoomin(szScoreValue);	

	//�����ʳ�
	if (bModify == false)
	{
		//���ü���
		JackpotItem.wLevelID = m_wJackpotCount + 1;	
		CopyMemory(&m_JackpotItem[m_wJackpotCount++], &JackpotItem, sizeof(JackpotItem));

		//��������
		LVITEM lVItem;
		lVItem.mask = LVIF_TEXT;
		lVItem.iSubItem = 0;
		lVItem.pszText = TEXT("");
		lVItem.iItem = m_JackpotList.GetItemCount();

		//��������.
		auto nItemIndex = m_JackpotList.InsertItem(&lVItem);
		
		//��������
		SetJackpotItemToList(nItemIndex, &JackpotItem);
		
	}
	else
	{
		//�ʳصȼ�
		JackpotItem.wLevelID = GetDlgItemInt(IDC_EDIT_JACKPOT_LEVELID);	
		CopyMemory(&m_JackpotItem[m_nSelJackpotItem], &JackpotItem, sizeof(JackpotItem));

		//��������
		SetJackpotItemToList(m_nSelJackpotItem, &JackpotItem);
	}	

	return;
}

//ѡ������
VOID CDlgSessionStock::SelectStockItem(int nIndex)
{
	//У�����
	if (nIndex==-1 || nIndex >= m_StockList.GetItemCount())
	{
		return;
	}

	//���ñ���
	m_nSelStockItem = nIndex;	

	//��ȡ����
	auto pStockInfo = (tagStockInfo*)m_StockList.GetItemData(m_nSelStockItem);
	if (pStockInfo == NULL) return;

	//����ģʽ
	m_cbOperateMode = OPERATE_MODE_READONLY;	

	//��������
	TCHAR szScoreString[21] = { 0 };

	//����ʶ
	SetDlgItemInt(IDC_EDIT_STOCK_ID,pStockInfo->wStockID);

	//�������
	SetDlgItemInt(IDC_EDIT_STOCK_KIND,pStockInfo->wStockKind);

	//��漶��
	for (int i = 0; i < m_cbStockLevel.GetCount();i++)
	{
		if (m_cbStockLevel.GetItemData(i) == pStockInfo->wStockLevel)
		{
			m_cbStockLevel.SetCurSel(i);
			break;
		}
	}

	//�������
	SetDlgItemText(IDC_EDIT_STOCK_NAME, pStockInfo->szStockName);

	//���״̬
	LPCTSTR pszStockState[] = { TEXT("δ����"),TEXT("������"),TEXT("������") };
	if (pStockInfo->cbStockState <= CountArray(pszStockState))
	{
		SetDlgItemText(IDC_EDIT_STOCK_STATE, pszStockState[pStockInfo->cbStockState]);
	}

	//��Ϸ����
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

	//��ע���
	_sntprintf_s(szScoreString,CountArray(szScoreString),TEXT("%0.2f"), ScoreZoomout(pStockInfo->lBetAmount));
	SetDlgItemText(IDC_EDIT_BET_AMOUNT, szScoreString);

	//��ǰ���
	_sntprintf_s(szScoreString, CountArray(szScoreString), TEXT("%0.2f"), ScoreZoomout(pStockInfo->lStockScore));
	SetDlgItemText(IDC_EDIT_STOCK_SCORE, szScoreString);

	//�������
	_sntprintf_s(szScoreString, CountArray(szScoreString), TEXT("%0.2f"), ScoreZoomout(pStockInfo->lExtraScore));
	SetDlgItemText(IDC_EDIT_EXTRA_SCORE, szScoreString);

	//����ģʽ
	INT nCheckButton = pStockInfo->cbEnableMode == STOCK_ENABLE_MODE_AUTO ? IDC_RADIO_AUTO_MODE : IDC_RADIO_MANUAL_MODE;
	CheckRadioButton(IDC_RADIO_AUTO_MODE, IDC_RADIO_MANUAL_MODE, nCheckButton);

	//����ʱ��
	if (pStockInfo->cbEnableMode == STOCK_ENABLE_MODE_MANUAL)
	{
		GetDlgItem(IDC_DATE_END_DATE)->EnableWindow(true);
		GetDlgItem(IDC_DATE_END_TIME)->EnableWindow(true);
		GetDlgItem(IDC_DATE_START_DATE)->EnableWindow(true);
		GetDlgItem(IDC_DATE_START_TIME)->EnableWindow(true);

		//ת��ʱ��
		auto EnableEndTime = Timet2SystemTime(pStockInfo->dwEnableEndTime);
		auto EnableStartTime = Timet2SystemTime(pStockInfo->dwEnableStartTime);

		//����ʱ��
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

	//��ˮ����
	SetDlgItemTextA(m_hWnd,IDC_EDIT_SHRINK_OPTION, pStockInfo->szShrinkOption);

	//�������
	SetDlgItemTextA(m_hWnd,IDC_EDIT_RELATION_ID_LIST, pStockInfo->szRelationStockID);

	//�ʳ�����
	m_JackpotList.DeleteAllItems();

	//�ʳ���Ϣ
	m_wJackpotCount = pStockInfo->wJackpotCount;
	CopyMemory(m_JackpotItem,pStockInfo->GameJackpotItem,sizeof(m_JackpotItem));

	//��������
	LVITEM lVItem;
	lVItem.mask = LVIF_TEXT;
	lVItem.iSubItem = 0;
	lVItem.pszText = TEXT("");

	//ѭ������
	for (WORD i = 0; i < m_wJackpotCount; i++)
	{
		//��ȡ����
		auto pJackpotItem = &m_JackpotItem[i];
		if (pJackpotItem == NULL) continue;

		//��������.
		lVItem.iItem = i;
		auto nItemIndex = m_JackpotList.InsertItem(&lVItem);
		if (nItemIndex == -1) continue;

		//��������
		SetJackpotItemToList(nItemIndex, pJackpotItem);
	}

	//����״̬
	UpdateControlState();
}

//ѡ��ʳ�
VOID CDlgSessionStock::SelectJackpotItem(int nIndex)
{
	//���ñ���
	m_nSelJackpotItem = nIndex;
	if (m_nSelJackpotItem == -1 || m_nSelJackpotItem>=m_wJackpotCount) return;

	//��ȡ�ʳ�
	auto pJackpotItem = &m_JackpotItem[m_nSelJackpotItem];

	//��������
	TCHAR szScoreString[18] = { 0 };

	//�ʳؼ���
	SetDlgItemInt(IDC_EDIT_JACKPOT_LEVELID, pJackpotItem->wLevelID);

	//�ʳزʽ�
	_sntprintf_s(szScoreString,CountArray(szScoreString),TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lStockScore));
	SetDlgItemText(IDC_EDIT_JACKPOT_SCORE, szScoreString);

	//�ʳس�ʼ�ʽ�
	_sntprintf_s(szScoreString, CountArray(szScoreString), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lInitialScore));
	SetDlgItemText(IDC_EDIT_INITIAL_SCORE, szScoreString);

	//����ʽ�
	_sntprintf_s(szScoreString, CountArray(szScoreString), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lVirtualScore));
	SetDlgItemText(IDC_EDIT_VIRTUAL_SCORE, szScoreString);

	//��������
	_sntprintf_s(szScoreString, CountArray(szScoreString), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lExtraScore));
	SetDlgItemText(IDC_EDIT_JACKPOT_EXTRA_SCORE, szScoreString);

	//��ˮ����
	SetDlgItemInt(IDC_EDIT_SHRINK_RATIO, pJackpotItem->wShrinkRatio);

	//�ɲʸ���
	SetDlgItemInt(IDC_EDIT_PAYOUT_PROB, pJackpotItem->wPayoutProb);

	//�ɲ�����
	_sntprintf_s(szScoreString, CountArray(szScoreString), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lPayoutLimit));
	SetDlgItemText(IDC_EDIT_PAYOUT_LIMIT, szScoreString);

	//�ɲ����
	_sntprintf_s(szScoreString, CountArray(szScoreString), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lPayoutOrigin));
	SetDlgItemText(IDC_EDIT_PAYOUT_ORIGIN, szScoreString);
}

//��������
VOID CDlgSessionStock::SetStockItemToList(int nItemIndex, tagStockInfo* pStockInfo)
{
	//����У��
	ASSERT (nItemIndex != -1 && pStockInfo != NULL);
	if (nItemIndex == -1 || pStockInfo == NULL) return;

	//��������
	TCHAR szItemValue[32] = TEXT("");

	//���ID
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pStockInfo->wStockID);
	m_StockList.SetItemText(nItemIndex, 0, szItemValue);

	//�������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pStockInfo->wStockKind);
	m_StockList.SetItemText(nItemIndex, 1, szItemValue);

	//���״̬
	LPCTSTR pszStockState[] = { TEXT("δ����"),TEXT("������"),TEXT("������") };
	if (pStockInfo->cbStockState <= CountArray(pszStockState))
	{
		m_StockList.SetItemText(nItemIndex, 2, pszStockState[pStockInfo->cbStockState]);
	}

	//���ȼ�
	LPCTSTR szStockLevel[] = { TEXT("BET���"),TEXT("��Ϸ���"), TEXT("������"), TEXT("����") };
	if (pStockInfo->wStockLevel >= STOCK_LEVEL_BET && pStockInfo->wStockLevel <= STOCK_LEVEL_ACTIVITY)
	{
		m_StockList.SetItemText(nItemIndex, 3, szStockLevel[pStockInfo->wStockLevel-1]);
	}

	//�������
	m_StockList.SetItemText(nItemIndex, 4, pStockInfo->szStockName);

	//�������
	auto pGameKind = m_pServerListManager->SearchGameKind(pStockInfo->wKindID);
	if (pGameKind != NULL)
	{
		m_StockList.SetItemText(nItemIndex, 5, pGameKind->m_GameKind.szKindName);
	}

	//��ע���
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.2f"), ScoreZoomout(pStockInfo->lBetAmount));
	m_StockList.SetItemText(nItemIndex, 6, szItemValue);

	//��ǰ���
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.2f"), ScoreZoomout(pStockInfo->lStockScore));
	m_StockList.SetItemText(nItemIndex, 7, szItemValue);

	//��ˮ����
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.2f"), ScoreZoomout(pStockInfo->lShrinkProfit));
	m_StockList.SetItemText(nItemIndex, 8, szItemValue);

	//�������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.2f"), ScoreZoomout(pStockInfo->lExtraScore));
	m_StockList.SetItemText(nItemIndex, 9, szItemValue);	

	//����ģʽ
	m_StockList.SetItemText(nItemIndex, 10, pStockInfo->cbEnableMode==STOCK_ENABLE_MODE_AUTO?TEXT("�Զ�"):TEXT("�ֶ�"));

	//��ˮ����
	m_StockList.SetItemText(nItemIndex, 11, CA2CT(pStockInfo->szShrinkOption).m_psz);

	//�������
	m_StockList.SetItemText(nItemIndex, 12, CA2CT(pStockInfo->szRelationStockID).m_psz);	
}

//��������
VOID CDlgSessionStock::SetJackpotItemToList(int nItemIndex, tagJackpotItem* pJackpotItem)
{
	//��������
	TCHAR szItemValue[32] = TEXT("");

	//�ʳ�����
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pJackpotItem->wLevelID);
	m_JackpotList.SetItemText(nItemIndex, 0, szItemValue);

	//��ǰ�ʽ�
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lStockScore));
	m_JackpotList.SetItemText(nItemIndex, 1, szItemValue);

	//��ʼ�ʽ�
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lInitialScore));
	m_JackpotList.SetItemText(nItemIndex, 2, szItemValue);

	//����ʽ�
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lVirtualScore));
	m_JackpotList.SetItemText(nItemIndex, 3, szItemValue);

	//�������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lExtraScore));
	m_JackpotList.SetItemText(nItemIndex, 4, szItemValue);

	//��ˮ����
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pJackpotItem->wShrinkRatio);
	m_JackpotList.SetItemText(nItemIndex, 5, szItemValue);

	//�ɲʸ���
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pJackpotItem->wPayoutProb);
	m_JackpotList.SetItemText(nItemIndex, 6, szItemValue);

	//�ɲ�����
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lPayoutLimit));
	m_JackpotList.SetItemText(nItemIndex, 7, szItemValue);

	//�ɲ����
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.2f"), ScoreZoomout(pJackpotItem->lPayoutOrigin));
	m_JackpotList.SetItemText(nItemIndex, 8, szItemValue);
}

//ɾ�����
bool CDlgSessionStock::DeleteStockInfo(WORD wStockID)
{
	//��ȡ����
	CMD_GP_DeleteStockInfo DeleteStockInfo = {};

	//���ñ���
	DeleteStockInfo.wStockID = wStockID;

	//��������
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_STOCK_SERVICE, SUB_GP_DELETE_STOCK_INFO, &DeleteStockInfo, sizeof(DeleteStockInfo));
}

//��ѯ���
bool CDlgSessionStock::QueryStockInfo(LPCTSTR pszKindIDList)
{
	//��ȡ����
	CMD_GP_QueryStockInfo QueryParameter = {};

	//���ñ���
	StringCchCopy(QueryParameter.szKindID, CountArray(QueryParameter.szKindID), pszKindIDList);

	//��������
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_STOCK_SERVICE, SUB_GP_QUERY_STOCK_INFO, &QueryParameter, sizeof(QueryParameter));
}

//�޸Ŀ��
bool CDlgSessionStock::ModifyStockInfo(tagStockInfo* pStockInfo)
{
	//��ȡ����
	CMD_GP_ModifyStockInfo ModifyStockInfo = {};

	//���ñ���
	CopyMemory(&ModifyStockInfo.StockInfo, pStockInfo, sizeof(ModifyStockInfo.StockInfo));

	//��������
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_STOCK_SERVICE, SUB_GP_MODIFY_STOCK_INFO, &ModifyStockInfo, sizeof(ModifyStockInfo));
}

//��ӿ��
bool CDlgSessionStock::AppendStockInfo(tagStockInfo* pStockInfo)
{
	//��ȡ����
	CMD_GP_AppendStockInfo AppendStockInfo = {};

	//���ñ���
	CopyMemory(&AppendStockInfo.StockInfo, pStockInfo, sizeof(AppendStockInfo.StockInfo));

	//��������
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_STOCK_SERVICE, SUB_GP_APPEND_STOCK_INFO, &AppendStockInfo, sizeof(AppendStockInfo));
}

//�������
bool CDlgSessionStock::AdjustStockScore(WORD wStockID, SCORE lChangeScore)
{
	//��ȡ����
	CMD_GP_AdjustStockScore AdjustStockScore = {};

	//���ñ���
	AdjustStockScore.wStockID = wStockID;
	AdjustStockScore.lChangeScore = lChangeScore;

	//��������
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_STOCK_SERVICE, SUB_GP_ADJUST_STOCK_SCORE, &AdjustStockScore, sizeof(AdjustStockScore));
}

//�����ʽ�
bool CDlgSessionStock::AdjustJackpotScore(WORD wStockID, WORD wLevelID, SCORE lChangeScore)
{
	//��ȡ����
	CMD_GP_AdjustJackpotScore AdjustJackpotScore = {};

	//���ñ���
	AdjustJackpotScore.wStockID = wStockID;
	AdjustJackpotScore.wLevelID = wLevelID;
	AdjustJackpotScore.lChangeScore = lChangeScore;

	//��������
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_STOCK_SERVICE, SUB_GP_ADJUST_JACKPOT_SCORE, &AdjustJackpotScore, sizeof(AdjustJackpotScore));
}

//��ȡ�¼�
bool CDlgSessionStock::OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	//������
	if (Command.wMainCmdID == MDM_GP_STOCK_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_STOCK_OPERATE_RESULT:		//�������
		{
			//����У��
			ASSERT(wDataSize <= sizeof(CMD_GP_StockOperateResult));
			if (wDataSize > sizeof(CMD_GP_StockOperateResult)) return false;

			//��ȡ����
			auto pOperateResult = (CMD_GP_StockOperateResult*)pData;

			switch (pOperateResult->wSubCommdID)
			{
			case SUB_GP_QUERY_STOCK_INFO:	//��ѯ���
			{
				//��������
				int nItemIndex = -1;
				tagStockInfo* pItemData = NULL;
				tagStockInfo* pTempStockInfo = NULL;

				//�������
				if (pOperateResult->wPacketIdx == 0)
				{
					m_StockList.DeleteAllItems();
				}

				//��������
				LVITEM lVItem;
				lVItem.mask = LVIF_TEXT;
				lVItem.iSubItem = 0;
				lVItem.pszText = TEXT("");

				//ѭ������
				for (WORD wIndex = 0; wIndex < pOperateResult->wStockCount; wIndex++)
				{
					//��ȡ����
					pTempStockInfo = &pOperateResult->StockList[wIndex];
					if (pTempStockInfo == NULL) continue;

					//��������.
					lVItem.iItem = m_StockList.GetItemCount();
					nItemIndex = m_StockList.InsertItem(&lVItem);
					if (nItemIndex == -1) continue;

					//��������
					SetStockItemToList(nItemIndex, pTempStockInfo);

					//������Դ
					pItemData = new tagStockInfo;
					if (pItemData != NULL)
					{
						//��������
						CopyMemory(pItemData, pTempStockInfo, sizeof(tagStockInfo));

						//��������
						m_StockList.SetItemData(nItemIndex, (DWORD_PTR)pItemData);
					}
				}

				//Ĭ��ѡ��
				SelectStockItem(0);

				break;
			}
			case SUB_GP_APPEND_STOCK_INFO:		//��ӿ��
			{
				//��������
				int nItemIndex = -1;

				//��������
				LVITEM lVItem;
				lVItem.mask = LVIF_TEXT;
				lVItem.iSubItem = 0;
				lVItem.pszText = TEXT("");

				//��������
				lVItem.iItem = m_StockList.GetItemCount();
				nItemIndex = m_StockList.InsertItem(&lVItem);

				//��ȡ���
				auto pStockInfo = &pOperateResult->StockList[0];

				//��������
				SetStockItemToList(nItemIndex, pStockInfo);

				//������Դ
				auto pItemData = new tagStockInfo;
				if (pItemData != NULL)
				{
					//��������
					CopyMemory(pItemData, pStockInfo, sizeof(tagStockInfo));

					//��������
					m_StockList.SetItemData(nItemIndex, (DWORD_PTR)pItemData);
				}

				//ѡ����
				SelectStockItem(nItemIndex);

				break;
			}
			case SUB_GP_MODIFY_STOCK_INFO:		//�޸Ŀ��
			{
				//��������
				int nItemIndex = -1;

				//��ȡ���
				auto pStockInfo = &pOperateResult->StockList[0];

				//���Ҷ���
				for (int nIndex = 0; nIndex < m_StockList.GetItemCount(); nIndex++)
				{
					//��ȡ����
					auto pItemData = (tagStockInfo*)m_StockList.GetItemData(nIndex);
					if (pItemData == NULL) continue;

					if (pItemData->wStockID == pStockInfo->wStockID)
					{
						//��������
						CopyMemory(pItemData, pStockInfo, sizeof(tagStockInfo));

						//��������
						SetStockItemToList(nIndex, pStockInfo);

						//��������
						nItemIndex = nIndex;

						break;
					}

				}

				//ѡ����
				SelectStockItem(m_nSelStockItem);

				break;
			}
			case SUB_GP_DELETE_STOCK_INFO:		//ɾ�����
			{
				//��ȡ���
				auto pStockInfo = &pOperateResult->StockList[0];

				//���Ҷ���
				for (int nIndex = 0; nIndex < m_StockList.GetItemCount(); nIndex++)
				{
					auto pItemData = (tagStockInfo*)m_StockList.GetItemData(nIndex);
					if (pItemData == NULL) continue;

					if (pItemData->wStockID == pStockInfo->wStockID)
					{
						//��������
						m_StockList.DeleteItem(nIndex);

						break;
					}

				}

				//ѡ����
				SelectStockItem(m_nSelStockItem);

				break;
			}
			}

			return true;
		}
		}
	}
	
	//����ʧ��
	if (Command.wMainCmdID == MDM_GP_USER_SERVICE && Command.wSubCmdID==SUB_GP_OPERATE_FAILURE)
	{	
		//����У��
		ASSERT(sizeof(CMD_GP_OperateFailure) >= wDataSize);
		if (sizeof(CMD_GP_OperateFailure) < wDataSize) return false;

		//��ȡ����
		CMD_GP_OperateFailure* pOperateFailure = (CMD_GP_OperateFailure*)pData;

		//�ؼ�����
		EnableControls(TRUE);	

		//������ʾ
		CInformation Information;
		Information.ShowMessageBox(pOperateFailure->szDescribeString, MB_ICONERROR);

		return true;
	}

	return false;
}

//��С����
float CDlgSessionStock::ScoreZoomout(SCORE lScore)
{
	return static_cast<float>(lScore/1000.f);
}

//�Ŵ����
SCORE CDlgSessionStock::ScoreZoomin(LPCSTR pszScore)
{
	auto fScore = atof(pszScore);
	return static_cast<SCORE>(fScore*1000);
}

//��ʼ���ؼ�
VOID CDlgSessionStock::InitControls()
{
	//���ÿؼ�
	DWORD dwExstyle = m_StockList.GetExtendedStyle();
	//m_StockList.SetExtendedStyle(dwExstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | WS_EX_STATICEDGE);

	//�б�ؼ�
	m_StockList.InsertColumn(0, TEXT("ID"), LVCFMT_LEFT, 30);
	m_StockList.InsertColumn(1, TEXT("�������"), LVCFMT_LEFT, 60);
	m_StockList.InsertColumn(2, TEXT("���״̬"), LVCFMT_LEFT, 60);
	m_StockList.InsertColumn(3, TEXT("��漶��"), LVCFMT_LEFT, 60);
	m_StockList.InsertColumn(4, TEXT("�������"), LVCFMT_LEFT, 80);
	m_StockList.InsertColumn(5, TEXT("��Ϸ����"), LVCFMT_LEFT, 80);
	m_StockList.InsertColumn(6, TEXT("��ע���"), LVCFMT_LEFT, 60);
	m_StockList.InsertColumn(7, TEXT("��ǰ���"), LVCFMT_LEFT, 60);
	m_StockList.InsertColumn(8, TEXT("��ˮ����"), LVCFMT_LEFT, 60);
	m_StockList.InsertColumn(9, TEXT("�������"), LVCFMT_LEFT, 60);
	m_StockList.InsertColumn(10, TEXT("����ģʽ"), LVCFMT_LEFT, 60);
	m_StockList.InsertColumn(11, TEXT("��ˮ����"), LVCFMT_LEFT, 120);
	m_StockList.InsertColumn(12, TEXT("�������"), LVCFMT_LEFT, 120);

	//���ÿؼ�
	dwExstyle = m_JackpotList.GetExtendedStyle();
	m_JackpotList.SetExtendedStyle(dwExstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | WS_EX_STATICEDGE);

	//�б�ؼ�
	m_JackpotList.InsertColumn(0, TEXT("�ʳ�����"), LVCFMT_CENTER, 60);
	m_JackpotList.InsertColumn(1, TEXT("��ǰ�ʽ�"), LVCFMT_CENTER, 60);
	m_JackpotList.InsertColumn(2, TEXT("��ʼ�ʽ�"), LVCFMT_CENTER, 60);
	m_JackpotList.InsertColumn(3, TEXT("����ʽ�"), LVCFMT_CENTER, 60);
	m_JackpotList.InsertColumn(4, TEXT("�������"), LVCFMT_CENTER, 80);
	m_JackpotList.InsertColumn(5, TEXT("��ˮ����"), LVCFMT_CENTER, 80);
	m_JackpotList.InsertColumn(6, TEXT("�ɲʸ���"), LVCFMT_CENTER, 60);
	m_JackpotList.InsertColumn(7, TEXT("�ɲ�����"), LVCFMT_CENTER, 60);
	m_JackpotList.InsertColumn(8, TEXT("�ɲ����"), LVCFMT_CENTER, 60);
	
	//����ģʽ
	CheckRadioButton(IDC_RADIO_AUTO_MODE, IDC_RADIO_MANUAL_MODE, IDC_RADIO_AUTO_MODE);

	//�����
	WORD wStockLevel[] = { STOCK_LEVEL_BET,STOCK_LEVEL_GAME,STOCK_LEVEL_GROUP,STOCK_LEVEL_ACTIVITY };
	LPCTSTR szStockLevel[] = { TEXT("BET���"),TEXT("��Ϸ���"), TEXT("������"), TEXT("����") };		
	for (int i = 0; i < CountArray(szStockLevel); i++)
	{
		m_cbStockLevel.SetItemData(m_cbStockLevel.InsertString(i, szStockLevel[i]),wStockLevel[i]);
	}	
}

//����״̬
VOID CDlgSessionStock::UpdateControlState()
{
	//ֻ��ģʽ
	if (m_cbOperateMode == OPERATE_MODE_READONLY)
	{
		//��ť�ؼ�
		m_btConfirm.EnableWindow(FALSE);
		m_btQueryStock.EnableWindow(TRUE);
		m_btAppendStock.EnableWindow(TRUE);
		m_btModifyStock.EnableWindow(TRUE);
		m_btDeleteStock.EnableWindow(TRUE);
		m_btAppendJackpot.EnableWindow(FALSE);
		m_btModifyJackpot.EnableWindow(FALSE);
		m_btDeleteJackpot.EnableWindow(FALSE);

		//�����ؼ�
		GetDlgItem(IDC_DATE_END_DATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_DATE_END_TIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_DATE_START_DATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_DATE_START_TIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_GAME_NAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_STOCK_LEVEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_AUTO_MODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_MANUAL_MODE)->EnableWindow(FALSE);

		//���ؼ�
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_ID))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_KIND))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_STATE))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_NAME))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_BET_AMOUNT))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_SCORE))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_EXTRA_SCORE))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_SHRINK_OPTION))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_RELATION_ID_LIST))->SetReadOnly(TRUE);		

		//�ʳؿؼ�
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

	//����ģʽ
	if (m_cbOperateMode == OPERATE_MODE_APPEND)
	{
		//��ť�ؼ�
		m_btConfirm.EnableWindow(TRUE);
		m_btQueryStock.EnableWindow(TRUE);
		m_btAppendStock.EnableWindow(FALSE);
		m_btModifyStock.EnableWindow(FALSE);
		m_btDeleteStock.EnableWindow(FALSE);
		m_btAppendJackpot.EnableWindow(TRUE);
		m_btModifyJackpot.EnableWindow(TRUE);
		m_btDeleteJackpot.EnableWindow(TRUE);

		//�����ؼ�
		GetDlgItem(IDC_DATE_END_DATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_DATE_END_TIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_DATE_START_DATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_DATE_START_TIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_GAME_NAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_STOCK_LEVEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_AUTO_MODE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_MANUAL_MODE)->EnableWindow(TRUE);

		//���ؼ�
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_ID))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_KIND))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_STATE))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_NAME))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_BET_AMOUNT))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_SCORE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_EXTRA_SCORE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_SHRINK_OPTION))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_RELATION_ID_LIST))->SetReadOnly(FALSE);

		//�ʳؿؼ�
		((CEdit*)GetDlgItem(IDC_EDIT_JACKPOT_LEVELID))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_JACKPOT_SCORE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_INITIAL_SCORE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_VIRTUAL_SCORE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_JACKPOT_EXTRA_SCORE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_SHRINK_RATIO))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_PAYOUT_PROB))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_PAYOUT_LIMIT))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_PAYOUT_ORIGIN))->SetReadOnly(FALSE);

		//���ÿؼ�
		SetDlgItemText(IDC_EDIT_STOCK_ID,TEXT(""));
		SetDlgItemText(IDC_EDIT_STOCK_STATE, TEXT("δ����"));
	}

	//�༭ģʽ
	if (m_cbOperateMode == OPERATE_MODE_MODIFY)
	{
		//��ť�ؼ�
		m_btConfirm.EnableWindow(TRUE);
		m_btQueryStock.EnableWindow(TRUE);
		m_btAppendStock.EnableWindow(FALSE);
		m_btModifyStock.EnableWindow(FALSE);
		m_btDeleteStock.EnableWindow(FALSE);
		m_btAppendJackpot.EnableWindow(TRUE);
		m_btModifyJackpot.EnableWindow(TRUE);
		m_btDeleteJackpot.EnableWindow(TRUE);

		//�����ؼ�
		GetDlgItem(IDC_DATE_END_DATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_DATE_END_TIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_DATE_START_DATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_DATE_START_TIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_GAME_NAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_STOCK_LEVEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_AUTO_MODE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_MANUAL_MODE)->EnableWindow(TRUE);

		//���ؼ�
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_ID))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_KIND))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_BET_AMOUNT))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_STATE))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_NAME))->SetReadOnly(FALSE);		
		((CEdit*)GetDlgItem(IDC_EDIT_STOCK_SCORE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_EXTRA_SCORE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_SHRINK_OPTION))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_RELATION_ID_LIST))->SetReadOnly(FALSE);

		//�ʳؿؼ�
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

//�ؼ�ʹ��
VOID CDlgSessionStock::EnableControls(BOOL bEnabled)
{
	//��ѯ״̬
	m_btQueryStock.EnableWindow(bEnabled);
}

//��������
VOID CDlgSessionStock::OnHdnListClickStockItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	//ѡ������
	SelectStockItem(phdr->iItem);

	*pResult = 0;
}

//ɾ������
VOID CDlgSessionStock::OnLvnListDeleteStockItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	//��ȡ����
	auto pItemData = (tagStockInfo*)m_StockList.GetItemData(pNMLV->iItem);
	if (pItemData != NULL)
	{
		SafeDelete(pItemData);
	}

	*pResult = 0;
}

//���̰���
VOID CDlgSessionStock::OnLvnListKeydownStockItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	//��������
	int nItem = -1;
	POSITION pos;

	switch (pLVKeyDow->wVKey)
	{
	case VK_DOWN:
	{
		pos = m_StockList.GetFirstSelectedItemPosition();
		while (pos)
		{
			//��ȡ����
			nItem = m_StockList.GetNextSelectedItem(pos);
			if (nItem == -1) break;

			//��������
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
			//��ȡ����
			nItem = m_StockList.GetNextSelectedItem(pos);
			if (nItem == -1) break;

			//��������
			nItem = __max(nItem - 1, 0);
			SelectStockItem(nItem);
			break;
		}
		break;
	}
	}

	*pResult = 0;
}

//��������
VOID CDlgSessionStock::OnHdnListClickJackpotItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	//ѡ������
	SelectJackpotItem(phdr->iItem);

	*pResult = 0;
}

//ɾ������
VOID CDlgSessionStock::OnLvnListDeleteJackpotItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
}

//���̰���
VOID CDlgSessionStock::OnLvnListKeydownJackpotItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	//��������
	int nItem = -1;
	POSITION pos;

	switch (pLVKeyDow->wVKey)
	{
	case VK_DOWN:
	{
		pos = m_JackpotList.GetFirstSelectedItemPosition();
		while (pos)
		{
			//��ȡ����
			nItem = m_JackpotList.GetNextSelectedItem(pos);
			if (nItem == -1) break;

			//��������
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
			//��ȡ����
			nItem = m_JackpotList.GetNextSelectedItem(pos);
			if (nItem == -1) break;

			//��������
			nItem = __max(nItem - 1, 0);
			SelectJackpotItem(nItem);
			break;
		}
		break;
	}
	}

	*pResult = 0;
}

//�������
VOID CDlgSessionStock::OnBnClickedBtAppendStock()
{
	//���ñ���
	m_wJackpotCount = 0;
	m_cbOperateMode = OPERATE_MODE_APPEND;	
	ZeroMemory(&m_JackpotItem,sizeof(m_JackpotItem));

	//��ղʳ�
	m_JackpotList.DeleteAllItems();

	//����״̬
	UpdateControlState();	
}

//�༭����
VOID CDlgSessionStock::OnBnClickedBtModifyStock()
{
	//��ȡ����
	if (m_nSelStockItem == -1) return;

	//��ȡ���
	auto pStockInfo = (tagStockInfo*)m_StockList.GetItemData(m_nSelStockItem);
	if (pStockInfo == NULL) return;

	//У��״̬
	if (pStockInfo->cbStockState == STOCK_STATE_LOCKED)
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("��ǰ��������в������޸ģ�"));

		return;
	}

	//����ģʽ
	m_cbOperateMode = OPERATE_MODE_MODIFY;

	//����״̬
	UpdateControlState();	
}

//ɾ������
VOID CDlgSessionStock::OnBnClickedBtDeleteStock()
{
	//��ȡ����
	if (m_nSelStockItem == -1) return;

	//����ѡ��
	if (m_StockList.GetItemCount() <= m_nSelStockItem)
	{
		m_nSelStockItem = -1;

		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("��ѡ����Ҫɾ���Ŀ�棡"));

		return;
	}

	//��ȡ����
	auto pStockInfo = (tagStockInfo*)m_StockList.GetItemData(m_nSelStockItem);
	if (pStockInfo == NULL) return;

	//У��״̬
	if (pStockInfo->cbStockState == STOCK_STATE_LOCKED)
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("��ǰ��������в�����ɾ����"));

		return;
	}

	//��Ϣ��ʾ
	CInformation Information;
	if (Information.ShowMessageBox(TEXT("��ʾ"), TEXT("��ȷ��Ҫɾ����ǰ�����"), MB_OKCANCEL)!=IDOK)
	{
		return;
	}	

	//ɾ�����
	DeleteStockInfo(pStockInfo->wStockID);
	
	//���ñ���
	m_nSelStockItem = -1;
	m_cbOperateMode = OPERATE_MODE_READONLY;

	//���¿ؼ�
	UpdateControlState();

	return;
}

//��Ӳʳ�
VOID CDlgSessionStock::OnBnClickedBtAppendJackpot()
{
	AddModifyJackpotInfo(false);
}

//�༭�ʳ�
VOID CDlgSessionStock::OnBnClickedBtModifyJackpot()
{
	AddModifyJackpotInfo(true);
}

//ɾ���ʳ�
VOID CDlgSessionStock::OnBnClickedBtDeleteJackpot()
{
	//��ȡ����
	if (m_nSelJackpotItem == -1) return;

	//����ѡ��
	if (m_JackpotList.GetItemCount() <= m_nSelJackpotItem)
	{
		m_nSelJackpotItem = -1;

		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("��ѡ����Ҫɾ���Ĳʳ���Ϣ��"));

		return;
	}

	//����ѡ��
	if (m_nSelJackpotItem != m_JackpotList.GetItemCount()-1)
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("�ʽ��ֻ������ɾ�����һ����"));

		return;
	}

	//��ȡ����
	auto pItemData = (tagStockInfo*)m_StockList.GetItemData(m_nSelStockItem);
	if (pItemData == NULL) return;

	//ɾ���ʳ�
	if (m_nSelJackpotItem < m_wJackpotCount)
	{
		m_wJackpotCount--;
		ZeroMemory(&m_JackpotItem[m_nSelJackpotItem],sizeof(m_nSelJackpotItem));
	}

	//ɾ���ʳ�
	m_JackpotList.DeleteItem(m_nSelJackpotItem);

	return;
}

//�б����
VOID CDlgSessionStock::OnEventListFinish()
{
	//��������
	POSITION Position = NULL;
	CGameKindItem* pGameKindItem = NULL;

	//������Ϸ
	TCHAR szItemString[64] = { 0 };
	auto pComboGameName = (CComboBox*)GetDlgItem(IDC_COMBO_GAME_NAME);

	//�������
	while (m_cbGameList.GetCount()) m_cbGameList.DeleteString(0);
	while (pComboGameName->GetCount()) pComboGameName->DeleteString(0);	

	do
	{
		pGameKindItem = m_pServerListManager->EmunGameKindItem(Position);
		if (pGameKindItem == NULL) break;

		//��������
		tagItemData* pItemData = (tagItemData*)m_cbGameList.GetItemDataPtr(m_cbGameList.AddString(pGameKindItem->m_GameKind.szKindName));
		if (pItemData != NULL) pItemData->dwDataPtr = pGameKindItem->m_GameKind.wKindID;
		
		if (pComboGameName != NULL)
		{
			pComboGameName->SetItemData(pComboGameName->AddString(pGameKindItem->m_GameKind.szKindName), pGameKindItem->m_GameKind.wKindID);
		}

	} while (Position != NULL);

	//Ĭ����Ϸ
	tagItemData* pItemData = (tagItemData*)m_cbGameList.GetItemDataPtr(m_cbGameList.InsertString(0, TEXT("")));
	if (pItemData != NULL) pItemData->dwDataPtr = 0;

	//Ĭ����Ϸ
	if (pComboGameName != NULL)
	{
		pComboGameName->SetItemData(pComboGameName->InsertString(0, TEXT("")), 0);
	}
}

//ȷ�ϲ���
VOID CDlgSessionStock::OnBnClickedBtConfirm()
{
	//ֻ��ģʽ
	if (m_cbOperateMode == OPERATE_MODE_READONLY)
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("û���κ��޸ģ�����ȷ�ϣ�"));

		return;
	}

	//�༭ģʽ
	if (m_cbOperateMode == OPERATE_MODE_MODIFY && m_nSelStockItem == -1)
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("��ѡ����Ҫ�༭�Ŀ�棡"));

		return;
	}

	//��������
	TCHAR szMessage[128] = TEXT("");

	//�����Ϣ
	tagStockInfo StockInfo;
	ZeroMemory(&StockInfo, sizeof(StockInfo));

	//��Ϸ����
	auto pCombGameName = (CComboBox*)GetDlgItem(IDC_COMBO_GAME_NAME);

	//У������
	if (pCombGameName->GetCurSel() == -1)
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("��Ϸ���Ʋ���Ϊ�գ�"));

		return;
	}

	//У������
	if (m_cbStockLevel.GetCurSel() == -1)
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("���ȼ�����Ϊ�գ�"));

		return;
	}

	//��������
	CHAR szScoreValue[18] = { 0 };

	//�����Ϣ
	StockInfo.wStockID = GetDlgItemInt(IDC_EDIT_STOCK_ID);
	StockInfo.wStockKind = GetDlgItemInt(IDC_EDIT_STOCK_KIND);

	//��漶��
	if (m_cbStockLevel.GetCurSel() != -1)
	{
		StockInfo.wStockLevel = m_cbStockLevel.GetItemData(m_cbStockLevel.GetCurSel());
	}		

	//��ע���
	GetDlgItemTextA(m_hWnd, IDC_EDIT_BET_AMOUNT, szScoreValue, CountArray(szScoreValue));
	StockInfo.lBetAmount = ScoreZoomin(szScoreValue);

	//�������
	GetDlgItemTextA(m_hWnd, IDC_EDIT_EXTRA_SCORE, szScoreValue, CountArray(szScoreValue));
	StockInfo.lExtraScore = ScoreZoomin(szScoreValue);

	//��ǰ���
	GetDlgItemTextA(m_hWnd, IDC_EDIT_STOCK_SCORE, szScoreValue, CountArray(szScoreValue));
	StockInfo.lStockScore = ScoreZoomin(szScoreValue);

	//���ͱ�ʶ
	StockInfo.wKindID = (WORD)pCombGameName->GetItemData(pCombGameName->GetCurSel());

	//�������
	GetDlgItemText(IDC_EDIT_STOCK_NAME, StockInfo.szStockName, CountArray(StockInfo.szStockName));

	//Json����
	GetDlgItemTextA(m_hWnd, IDC_EDIT_SHRINK_OPTION, StockInfo.szShrinkOption, CountArray(StockInfo.szShrinkOption));
	GetDlgItemTextA(m_hWnd, IDC_EDIT_RELATION_ID_LIST, StockInfo.szRelationStockID, CountArray(StockInfo.szRelationStockID));

	//����ģʽ
	if (GetCheckedRadioButton(IDC_RADIO_AUTO_MODE, IDC_RADIO_MANUAL_MODE) == IDC_RADIO_AUTO_MODE)
	{
		StockInfo.cbEnableMode = STOCK_ENABLE_MODE_AUTO;
	}
	else
	{
		StockInfo.cbEnableMode = STOCK_ENABLE_MODE_MANUAL;

		//ͣ��ʱ��
		auto pCtrlEndDate = (CDateTimeCtrl*)GetDlgItem(IDC_DATE_END_DATE);
		auto pCtrlEndTime = (CDateTimeCtrl*)GetDlgItem(IDC_DATE_END_TIME);

		//����ʱ��
		auto pCtrlStartDate = (CDateTimeCtrl*)GetDlgItem(IDC_DATE_START_DATE);
		auto pCtrlStartTime = (CDateTimeCtrl*)GetDlgItem(IDC_DATE_START_TIME);

		//ʱ�䶨��
		SYSTEMTIME	EndDate, EndTime; 
		SYSTEMTIME	StartDate, StartTime;		
		SYSTEMTIME	StartDateTime, EndDateTime;

		//��ȡʱ��
		pCtrlEndDate->GetTime(&EndDate);
		pCtrlEndTime->GetTime(&EndTime);
		pCtrlStartDate->GetTime(&StartDate);
		pCtrlStartTime->GetTime(&StartTime);

		//�ϲ�����ʱ��
		EndDateTime.wYear = EndDate.wYear;
		EndDateTime.wMonth = EndDate.wMonth;
		EndDateTime.wDay = EndDate.wDay;
		EndDateTime.wDayOfWeek = EndDate.wDayOfWeek;
		EndDateTime.wHour = EndTime.wHour;
		EndDateTime.wMinute = EndTime.wMinute;
		EndDateTime.wSecond = EndTime.wSecond;
		EndDateTime.wMilliseconds = EndTime.wMilliseconds;

		//�ϲ���ʼʱ��
		StartDateTime.wYear = StartDate.wYear;
		StartDateTime.wMonth = StartDate.wMonth;
		StartDateTime.wDay = StartDate.wDay;
		StartDateTime.wDayOfWeek = StartDate.wDayOfWeek;
		StartDateTime.wHour = StartTime.wHour;
		StartDateTime.wMinute = StartTime.wMinute;
		StartDateTime.wSecond = StartTime.wSecond;
		StartDateTime.wMilliseconds = StartTime.wMilliseconds;

		//��ȡʱ��
		StockInfo.dwEnableEndTime = SystemTime2Timet(EndDateTime);
		StockInfo.dwEnableStartTime = SystemTime2Timet(StartDateTime);		
	}

	//У��ģʽ
	if (StockInfo.wStockLevel == 1 && StockInfo.cbEnableMode != STOCK_ENABLE_MODE_AUTO)
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("BET���ֻ֧���Զ�����ģʽ��"));

		return;
	}

	//У��ģʽ
	if (StockInfo.wStockLevel > 1 && StockInfo.cbEnableMode == STOCK_ENABLE_MODE_AUTO)
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("��BET���ֻ֧���ֶ�����ģʽ��"));

		return;
	}

	//�ʳ���Ϣ
	StockInfo.wJackpotCount = m_wJackpotCount;
	CopyMemory(StockInfo.GameJackpotItem, m_JackpotItem, sizeof(StockInfo.GameJackpotItem));

	//�������
	if (m_cbOperateMode == OPERATE_MODE_APPEND)
	{
		//��ӿ��
		AppendStockInfo(&StockInfo);
	}
	else
	{
		//�޸Ŀ��
		ModifyStockInfo(&StockInfo);

		//������		
		auto pStockInfo = (tagStockInfo*)m_StockList.GetItemData(m_nSelStockItem);
		if (pStockInfo != NULL)
		{
			//���仯
			if (StockInfo.lStockScore != pStockInfo->lStockScore)
			{
				AdjustStockScore(StockInfo.wStockID, StockInfo.lStockScore- pStockInfo->lStockScore);
			}

			//�ʽ�仯
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

//��ѯ����
VOID CDlgSessionStock::OnBnClickedBtQueryStock()
{
	//���鶨��
	WORD wKindCount = 0;
	WORD wKindIDList[100];	

	//�ռ����ͱ�ʶ
	for (INT i = 0; i < m_cbGameList.GetCount(); i++)
	{
		if (m_cbGameList.GetCheck(i) == static_cast<BOOL>(true))
		{
			//��ȡ����
			tagItemData* pItemData = (tagItemData*)m_cbGameList.GetItemData(i);
			wKindIDList[wKindCount++] = (WORD)pItemData->dwDataPtr;
		}
	}

	//У������
	if (wKindCount > 0)
	{
		//��ʽ��
		CString strKindIDList = TEXT("[");
		for (int i = 0; i < wKindCount; i++)
		{
			if (i > 0) strKindIDList.AppendChar(',');

			strKindIDList.AppendFormat(TEXT("%d"), wKindIDList[i]);
		}
		strKindIDList.AppendChar(']');

		//��ѯ���
		QueryStockInfo(strKindIDList.GetString());
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////
