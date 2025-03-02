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

//���캯��
CDlgSessionConfig::CDlgSessionConfig() : CDlgSessionItem(IDD_DLG_CONFIG)
{
	//���ñ���
	m_wKindID = 0;
	m_pSelListCtrl = NULL;
	m_nSelWeightItem = -1;	
}

//��������
CDlgSessionConfig::~CDlgSessionConfig()
{
}

//�ؼ���
VOID CDlgSessionConfig::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//�󶨿ؼ�
	DDX_Control(pDX, IDC_BT_QUERY,  m_btQuery);
	DDX_Control(pDX, IDC_BT_SAVE_BASIC_CONFIG, m_btUpdateBasicConfig);
	DDX_Control(pDX, IDC_BT_SAVE_NORMAL_TABLE, m_btSaveNormalWeight);
	DDX_Control(pDX, IDC_BT_SAVE_WHITELIST_TABLE, m_btSaveWhiteListWeight);

	//�󶨿ؼ�
	DDX_Control(pDX, IDC_COMBO_GAME_LIST, m_cbGameList);
	DDX_Control(pDX, IDC_COMBO_WEIGHT_TYPE, m_cbWeightType);

	//�б�����
	DDX_Control(pDX, IDC_LIST_NORMAL_WEIGHT, m_ListNormalWeight);
	DDX_Control(pDX, IDC_LIST_WHITELIST_WEIGHT, m_ListWhiteListWeight);
}

VOID CDlgSessionConfig::OnDestory()
{
	//ɾ������
	m_ListNormalWeight.DeleteAllItems();
	m_ListWhiteListWeight.DeleteAllItems();
}

//���ú���
BOOL CDlgSessionConfig::OnInitDialog()
{
	__super::OnInitDialog();

	//��ʼ���ؼ�
	InitControls();

	return FALSE;
}

//ȷ������
VOID CDlgSessionConfig::OnOK()
{
	__super::OnOK();
}

//��Ϣ����
BOOL CDlgSessionConfig::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//����޸�
VOID CDlgSessionConfig::AddModifyWightItem(bool bModify)
{
	//ѡ���ж�
	if (bModify == true && m_nSelWeightItem == -1) return;

	//����ṹ
	tagTimesWeightItem WeightItem = {};

	//��������
	WeightItem.dwTimesRange[0] = GetDlgItemInt(IDC_EDIT_MIN_TIMES);
	WeightItem.dwTimesRange[1] = GetDlgItemInt(IDC_EDIT_MAX_TIMES);

	//У�鱶��
	if (WeightItem.dwTimesRange[0]>INVALID_WORD || WeightItem.dwTimesRange[1]>INVALID_WORD)
	{
		CString strMessage;
		strMessage.Format(TEXT("��������ܳ���%d��"), INVALID_WORD);

		//������ʾ
		CInformation Information;
		Information.ShowMessageBox(strMessage, MB_ICONERROR);

		return;
	}

	//����Ȩ��
	WeightItem.wTimesWeight = GetDlgItemInt(IDC_EDIT_TIMES_WEIGHT);

	//��������
	WeightItem.cbTimesKind = (GetCheckedRadioButton(IDC_RADIO_LOST, IDC_RADIO_WIN) == IDC_RADIO_LOST) ? 0 : 1;

	//Ȩ������
	if (m_cbWeightType.GetCurSel() != -1)
	{
		WeightItem.cbWeightType = m_cbWeightType.GetItemData(m_cbWeightType.GetCurSel());
	}

	//�б�ؼ�
	auto pListCtrl = m_pSelListCtrl;
	if (bModify == false)
	{
		pListCtrl = WeightItem.cbWeightType == 0 ? &m_ListNormalWeight : &m_ListWhiteListWeight;
	}

	//У�鱶������
	for (int i = 0; i < pListCtrl->GetItemCount(); i++)
	{
		//
		if (bModify && m_nSelWeightItem == i) continue;

		//У������
		auto pTimeWeightItem = (tagTimesWeightItem*)pListCtrl->GetItemData(i);
		if ((pTimeWeightItem->dwTimesRange[0] >= WeightItem.dwTimesRange[0] && pTimeWeightItem->dwTimesRange[0] <= WeightItem.dwTimesRange[1]) ||
			(pTimeWeightItem->dwTimesRange[1] >= WeightItem.dwTimesRange[0] && pTimeWeightItem->dwTimesRange[1] <= WeightItem.dwTimesRange[1]))
		{
			//������ʾ
			CInformation Information;
			Information.ShowMessageBox(TEXT("���������������������ص���"), MB_ICONERROR);

			return;
		}
	}

	//����Ȩ��
	if (bModify == false)
	{
		//��������
		LVITEM lVItem;
		lVItem.mask = LVIF_TEXT;
		lVItem.iSubItem = 0;
		lVItem.pszText = TEXT("");
		lVItem.iItem = pListCtrl->GetItemCount();

		//��������.
		auto nItemIndex = pListCtrl->InsertItem(&lVItem);
		
		//��������
		//SetWeightItemToList(pListCtrl, nItemIndex, &WeightItem);	

		//������Դ
		auto pItemData = new tagTimesWeightItem;
		if (pItemData != NULL)
		{
			//��������
			CopyMemory(pItemData, &WeightItem, sizeof(tagTimesWeightItem));

			//��������
			pListCtrl->SetItemData(nItemIndex, (DWORD_PTR)pItemData);
		}
	}
	else
	{
		//�����б�
		//SetWeightItemToList(pListCtrl, m_nSelWeightItem, &WeightItem);

		//��������
		auto pItemData = (tagTimesWeightItem*)pListCtrl->GetItemData(m_nSelWeightItem);
		if (pItemData != NULL)
		{
			//��������
			CopyMemory(pItemData, &WeightItem, sizeof(tagTimesWeightItem));
		}	
	}	

	//ͳ��Ȩ��ֵ
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

			//�����б�
			SetWeightItemToList(pListCtrl, i, pItemData);
		}
	}
	
	return;
}

//��ʽ������
bool CDlgSessionConfig::FormatJsonConfig(LPCTSTR pszBasicConfig, TCHAR szFormatConfig[], WORD wBuffSize)
{
	//��������
	WORD wIndex = 0;
	int nStringLen = lstrlen(pszBasicConfig);
	for (int i = 0; i < nStringLen; i++)
	{
		if (wIndex + 1 >= wBuffSize) return false;

		//�����ַ�
		szFormatConfig[wIndex++] = pszBasicConfig[i];

		//�ж������ַ�
		if (pszBasicConfig[i] == L'[' || pszBasicConfig[i] == L']' || pszBasicConfig[i] == L'{' || pszBasicConfig[i] == L'}' || pszBasicConfig[i] == L',')
		{
			if (wIndex + 4 >= wBuffSize) return  false;

			//��ӻ��з�
			szFormatConfig[wIndex++] = L'\r';
			szFormatConfig[wIndex++] = L'\n';

			if (i < nStringLen - 1)
			{
				szFormatConfig[wIndex++] = L' ';
				szFormatConfig[wIndex++] = L' ';
			}
		}

		//�ж������ַ�
		if (i + 1 < nStringLen && pszBasicConfig[i + 1] == L']' || pszBasicConfig[i+1] == L'}')
		{
			if (wIndex + 2 >= wBuffSize) return  false;

			//��ӻ��з�
			szFormatConfig[wIndex++] = L'\r';
			szFormatConfig[wIndex++] = L'\n';			
		}
	}

	if (wIndex + 1 >= wBuffSize) return false;

	//��ӽ�����
	szFormatConfig[wIndex++] = '\0';

	return true;
}

//ѹ������
bool CDlgSessionConfig::CompressJsonConfig(LPCTSTR pszFormatConfig, TCHAR szBasicConfig[], WORD wBuffSize)
{
	//��������
	WORD wIndex = 0;
	int nStringLen = lstrlen(pszFormatConfig);
	for (int i = 0; i < nStringLen; i++)
	{
		//�����ַ�
		auto chr = pszFormatConfig[i];	
		if (chr == L'\r' || chr == L'\n' || chr == L' ') continue;
		
		//���黺��
		if (wIndex + 1 >= wBuffSize) return false;

		//�����ַ�
		szBasicConfig[wIndex++] = chr;
	}

	//У�黺��
	if (wIndex + 1 >= wBuffSize) return false;

	//��ӽ�����
	szBasicConfig[wIndex++] = '\0';

	return true;
}

//ѡ������
VOID CDlgSessionConfig::SelectWeightItem(CListCtrl* pListCtrl, int nIndex)
{
	//У�����
	if (pListCtrl == NULL || nIndex == -1) return;

	//���ñ���
	m_pSelListCtrl=pListCtrl;
	m_nSelWeightItem=nIndex;	

	//��ȡ����
	auto pTimesWeightItem = (tagTimesWeightItem*)m_pSelListCtrl->GetItemData(m_nSelWeightItem);
	if (pTimesWeightItem == NULL) return;

	//��������
	SetDlgItemInt(IDC_EDIT_MIN_TIMES, pTimesWeightItem->dwTimesRange[0]);
	SetDlgItemInt(IDC_EDIT_MAX_TIMES, pTimesWeightItem->dwTimesRange[1]);

	//����Ȩ��
	SetDlgItemInt(IDC_EDIT_TIMES_WEIGHT, pTimesWeightItem->wTimesWeight);

	//�������
	CheckRadioButton(IDC_RADIO_LOST, IDC_RADIO_WIN, pTimesWeightItem->cbTimesKind==0?IDC_RADIO_LOST: IDC_RADIO_WIN);
	
	//Ȩ������
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

//�ռ�Ȩ��
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

//��������
VOID CDlgSessionConfig::SetWeightItemToList(CListCtrl* pListCtrl, int nItemIndex, tagTimesWeightItem* pTimesWeightItem)
{
	//У�����
	if (pListCtrl == NULL || pTimesWeightItem == NULL) return;

	//��������
	TCHAR szItemValue[32] = TEXT("");

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d-%d"), pTimesWeightItem->dwTimesRange[0], pTimesWeightItem->dwTimesRange[1]);
	pListCtrl->SetItemText(nItemIndex, 0, szItemValue);

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%s"), pTimesWeightItem->cbTimesKind == 0 ? TEXT("��") : TEXT("Ӯ"));
	pListCtrl->SetItemText(nItemIndex, 1, szItemValue);		

	//����Ȩ��
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pTimesWeightItem->wTimesWeight);
	pListCtrl->SetItemText(nItemIndex, 2, szItemValue);

	//Ȩ��ռ��
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%0.3f"), pTimesWeightItem->fWightRatio);
	pListCtrl->SetItemText(nItemIndex, 3, szItemValue);
}

//��ȡ�¼�
bool CDlgSessionConfig::OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	//������
	if (Command.wMainCmdID == MDM_GP_CONFIG_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_GAME_BASIC_CONFIG:		//��������
		{
			//��ȡ����
			auto pBasicConfig = (CMD_GP_GameBasicConfig*)pData;

			//��ȡ�ؼ�
			auto pEditBasicConfig = (CEdit*)GetDlgItem(IDC_EDIT_BASIC_CONFIG);

			//��������
			TCHAR szBasicConfig[4000] = {0};
			if (FormatJsonConfig(pBasicConfig->szBasicConfig, szBasicConfig, CountArray(szBasicConfig)))
			{
				//�����ı�
				pEditBasicConfig->SetWindowText(szBasicConfig);
			}
			else
			{
				//�����ı�
				pEditBasicConfig->SetWindowText(pBasicConfig->szBasicConfig);
			}

			break;
		}
		case SUB_GP_GAME_WEIGHT_CONFIG:		//Ȩ������
		{
			//��ȡ����
			auto pWeightConfig = (CMD_GP_GameWeightConfig*)pData;

			//��������
			int nItemIndex = -1;
			CListCtrl* pListCtrl = NULL;

			//��������
			LVITEM lVItem;
			lVItem.mask = LVIF_TEXT;
			lVItem.iSubItem = 0;
			lVItem.pszText = TEXT("");

			for (int i = 0; i < pWeightConfig->wItemCount; i++)
			{
				auto pTimesWeightItem = &pWeightConfig->TimesWeightItem[i];

				//��ͨȨ��
				if (pTimesWeightItem->cbWeightType == 0)
				{
					pListCtrl = &m_ListNormalWeight;
				}

				//������Ȩ��
				if (pTimesWeightItem->cbWeightType == 1)
				{
					pListCtrl = &m_ListWhiteListWeight;
				}

				//��������
				lVItem.iItem = pListCtrl->GetItemCount();
				nItemIndex = pListCtrl->InsertItem(&lVItem);

				//��������
				SetWeightItemToList(pListCtrl, nItemIndex, pTimesWeightItem);

				//������Դ
				auto pItemData = new tagTimesWeightItem;
				if (pItemData != NULL)
				{
					//��������
					CopyMemory(pItemData, pTimesWeightItem, sizeof(tagTimesWeightItem));

					//��������
					pListCtrl->SetItemData(nItemIndex, (DWORD_PTR)pItemData);
				}
			}

			//ѡ��Ȩ��
			SelectWeightItem(pListCtrl, 0);

			break;
		}
		case SUB_GP_GAME_CONFIG_FINISH:		//�������
		{
			//�ؼ�����
			EnableControls(TRUE);

			break;
		}
		}
	}
	
	//����ʧ��
	if (Command.wMainCmdID == MDM_GP_USER_SERVICE)
	{	
		//�����ɹ�
		if (Command.wSubCmdID == SUB_GP_OPERATE_SUCCESS)
		{
			//����У��
			ASSERT(sizeof(CMD_GP_OperateSuccess) >= wDataSize);
			if (sizeof(CMD_GP_OperateSuccess) < wDataSize) return false;

			//��ȡ����
			auto pOperateSuccess = (CMD_GP_OperateSuccess*)pData;

			//�ؼ�����
			EnableControls(TRUE);

			//������ʾ
			CInformation Information;
			Information.ShowMessageBox(pOperateSuccess->szDescribeString);

			return true;
		}

		//����ʧ��
		if (Command.wSubCmdID == SUB_GP_OPERATE_FAILURE)
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
	}

	return false;
}

//��ʼ���ؼ�
VOID CDlgSessionConfig::InitControls()
{
	//���ÿؼ�
	DWORD dwExstyle = m_ListNormalWeight.GetExtendedStyle();
	//m_ListNormalWeight.SetExtendedStyle(dwExstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | WS_EX_STATICEDGE);

	//�б�ؼ�
	m_ListNormalWeight.InsertColumn(0, TEXT("��������"), LVCFMT_LEFT, 80);
	m_ListNormalWeight.InsertColumn(1, TEXT("����"), LVCFMT_LEFT, 40);	
	m_ListNormalWeight.InsertColumn(2, TEXT("Ȩ��"), LVCFMT_LEFT, 40);
	m_ListNormalWeight.InsertColumn(3, TEXT("ռ��"), LVCFMT_LEFT, 40);
	
	//���ÿؼ�
	dwExstyle = m_ListWhiteListWeight.GetExtendedStyle();
	//m_ListWhiteListWeight.SetExtendedStyle(dwExstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | WS_EX_STATICEDGE);

	//�б�ؼ�
	m_ListWhiteListWeight.InsertColumn(0, TEXT("��������"), LVCFMT_LEFT, 80);
	m_ListWhiteListWeight.InsertColumn(1, TEXT("����"), LVCFMT_LEFT, 40);	
	m_ListWhiteListWeight.InsertColumn(2, TEXT("Ȩ��"), LVCFMT_LEFT, 40);
	m_ListWhiteListWeight.InsertColumn(3, TEXT("ռ��"), LVCFMT_LEFT, 40);

	//��������
	CheckRadioButton(IDC_RADIO_LOST, IDC_RADIO_WIN, IDC_RADIO_LOST);	

	//Ȩ������
	BYTE cbWeightType[] = { 0,1 };
	LPCTSTR pszWeightType[] = { TEXT("��ͨ����"), TEXT("����������") };
	for (int i = 0; i < CountArray(pszWeightType); i++)
	{
		m_cbWeightType.SetItemData(m_cbWeightType.InsertString(i,pszWeightType[i]), cbWeightType[i]);
	}
}

//�ؼ�ʹ��
VOID CDlgSessionConfig::EnableControls(BOOL bEnabled)
{
	//��ѯ״̬
	m_btQuery.EnableWindow(bEnabled);
	m_btUpdateBasicConfig.EnableWindow(bEnabled);
	m_btSaveNormalWeight.EnableWindow(bEnabled);
	m_btSaveWhiteListWeight.EnableWindow(bEnabled);
}

//��������
VOID CDlgSessionConfig::OnHdnListClickWeightItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	CListCtrl* pListCtrl = NULL;

	//��ͨȨ��
	if (pNMHDR->hwndFrom == m_ListNormalWeight.GetSafeHwnd())
	{
		//ѡ������
		pListCtrl = &m_ListNormalWeight;
	}

	//������Ȩ��
	if (pNMHDR->hwndFrom == m_ListWhiteListWeight.GetSafeHwnd())
	{
		//ѡ������
		pListCtrl = &m_ListWhiteListWeight;
	}

	//ѡ������
	if (pListCtrl)
	{		
		SelectWeightItem(pListCtrl, phdr->iItem);
	}

	*pResult = 0;
}

//ɾ������
VOID CDlgSessionConfig::OnLvnListDeleteWeightItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	//��������
	CListCtrl* pListCtrl = NULL;

	//��ͨȨ��
	if (pNMHDR->hwndFrom == m_ListNormalWeight.GetSafeHwnd())
	{
		//ѡ������
		pListCtrl=&m_ListNormalWeight;
	}

	//������Ȩ��
	if (pNMHDR->hwndFrom == m_ListWhiteListWeight.GetSafeHwnd())
	{
		//ѡ������
		pListCtrl=&m_ListWhiteListWeight;
	}

	//��ȡ����
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

//���̰���
VOID CDlgSessionConfig::OnLvnListKeydownWeightItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	//��������
	CListCtrl* pListCtrl = NULL;

	//��ͨȨ��
	if (pNMHDR->hwndFrom == m_ListNormalWeight.GetSafeHwnd())
	{
		//ѡ������
		pListCtrl = &m_ListNormalWeight;
	}

	//������Ȩ��
	if (pNMHDR->hwndFrom == m_ListWhiteListWeight.GetSafeHwnd())
	{
		//ѡ������
		pListCtrl = &m_ListWhiteListWeight;
	}

	if (pListCtrl != NULL)
	{
		//��������
		int nItem = -1;
		POSITION pos;

		switch (pLVKeyDow->wVKey)
		{
		case VK_DOWN:
		{
			pos = pListCtrl->GetFirstSelectedItemPosition();
			while (pos)
			{
				//��ȡ����
				nItem = pListCtrl->GetNextSelectedItem(pos);
				if (nItem == -1) break;

				//��������
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
				//��ȡ����
				nItem = pListCtrl->GetNextSelectedItem(pos);
				if (nItem == -1) break;

				//��������
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


//��ѯ����
VOID CDlgSessionConfig::OnBnClickedBtQuery()
{
	//��Ϸѡ��
	if (m_cbGameList.GetCurSel() == -1)
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("��ѡ����Ϸ���ͣ�"));
	}

	//��Ϸ����
	m_wKindID = m_cbGameList.GetItemData(m_cbGameList.GetCurSel());

	//��ѯ����
	if (QueryGameConfig(m_wKindID))
	{
		EnableControls(FALSE);

		//����б�
		m_ListNormalWeight.DeleteAllItems();
		m_ListWhiteListWeight.DeleteAllItems();

		//����ı�
		GetDlgItem(IDC_EDIT_BASIC_CONFIG)->SetWindowText(TEXT(""));
	}

	return;
}

//�������
VOID CDlgSessionConfig::OnBnClickedBtAppend()
{
	//У������
	if (m_wKindID==0)
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("����ѡ��һ����Ϸ����ѯ��"));

		return;
	}

	//���ñ���
	AddModifyWightItem(false);
}

//�༭����
VOID CDlgSessionConfig::OnBnClickedBtModify()
{
	//���ñ���
	AddModifyWightItem(true);
}

//ɾ������
VOID CDlgSessionConfig::OnBnClickedBtDelete()
{
	//��ȡ����
	if (m_pSelListCtrl == NULL) return;
	if (m_nSelWeightItem == -1) return;

	//����ѡ��
	if (m_pSelListCtrl->GetItemCount() <= m_nSelWeightItem)
	{
		m_nSelWeightItem = -1;

		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("��ѡ����Ҫɾ���ı�����"));

		return;
	}

	m_pSelListCtrl->DeleteItem(m_nSelWeightItem);

	return;
}

//��������
VOID CDlgSessionConfig::OnBnClickedBtUpdateBasicConfig()
{
	//��Ϸ����
	TCHAR  szBasicConfig[4000] = {0};
	GetDlgItemText(IDC_EDIT_BASIC_CONFIG, szBasicConfig, CountArray(szBasicConfig));

	//ѹ������
	TCHAR szCompressConfig[4000] = {0};
	if (!CompressJsonConfig(szBasicConfig, szCompressConfig, CountArray(szCompressConfig)))
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("�����������ݹ�����"));
	
		return;
	}

	//��������
	if (UpdateBasicConfig(m_wKindID, szCompressConfig))
	{
		EnableControls(FALSE);
	}
}

//����Ȩ��
VOID CDlgSessionConfig::OnBnClickedBtUpdateNormalWeight()
{
	//��������
	WORD wItemCount = 0;
	tagTimesWeightItem TimesWeightItem[128] = {};

	//��������
	wItemCount = CollectWeightItem(&m_ListNormalWeight, CountArray(TimesWeightItem), TimesWeightItem);

	//����Ȩ�ر�
	if (UpdateWeightTable(m_wKindID, TimesWeightItem, wItemCount))
	{
		EnableControls(FALSE);
	}
}

//����Ȩ��
VOID CDlgSessionConfig::OnBnClickedBtUpdateWhiteListWeight()
{
	//��������
	WORD wItemCount = 0;
	tagTimesWeightItem TimesWeightItem[128] = {};

	//��������
	wItemCount = CollectWeightItem(&m_ListWhiteListWeight, CountArray(TimesWeightItem), TimesWeightItem);

	//����Ȩ�ر�
	if (UpdateWeightTable(m_wKindID, TimesWeightItem, wItemCount))
	{
		EnableControls(FALSE);
	}
}

//�б����
VOID CDlgSessionConfig::OnEventListFinish()
{
	//��������
	POSITION Position = NULL;
	CGameKindItem* pGameKindItem = NULL;

	//�������
	while (m_cbGameList.GetCount()>0) m_cbGameList.DeleteString(0);

	do
	{
		pGameKindItem = m_pServerListManager->EmunGameKindItem(Position);
		if (pGameKindItem == NULL) break;

		//��������
		m_cbGameList.SetItemData(m_cbGameList.AddString(pGameKindItem->m_GameKind.szKindName), pGameKindItem->m_GameKind.wKindID);		

	} while (Position != NULL);
}

//��ѯ���
bool CDlgSessionConfig::QueryGameConfig(WORD wKindID)
{
	//�������
	CMD_GP_QueryGameConfig QueryGameConfig = {};

	//��������
	QueryGameConfig.wKindID = wKindID;

	//������Ϣ
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_CONFIG_SERVICE, SUB_GP_QUERY_GAME_CONFIG, &QueryGameConfig,sizeof(QueryGameConfig));
}

//�����������
bool CDlgSessionConfig::UpdateBasicConfig(WORD wKindID, LPCTSTR pszBasiceConfig)
{
	//�������
	CMD_GP_UpdateBasicConfig UpdateBasicConfig = {};

	//��������
	UpdateBasicConfig.wKindID = wKindID;
	StringCchCopy(UpdateBasicConfig.szBasicConfig,CountArray(UpdateBasicConfig.szBasicConfig),pszBasiceConfig);

	//���ݴ�С
	WORD wDataSize = CountStringBuffer(UpdateBasicConfig.szBasicConfig);
	wDataSize += sizeof(UpdateBasicConfig) - sizeof(UpdateBasicConfig.szBasicConfig);	

	//������Ϣ
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_CONFIG_SERVICE, SUB_GP_UPDATE_BASIC_CONFIG, &UpdateBasicConfig, wDataSize);
}

//����Ȩ�ر�
bool CDlgSessionConfig::UpdateWeightTable(WORD wKindID, tagTimesWeightItem TimesWeightItem[], WORD wItemCount)
{
	//�������
	CMD_GP_UpdateWeightConfig UpdateWeightConfig = {};

	//��������
	UpdateWeightConfig.wKindID = wKindID;
	UpdateWeightConfig.wItemCount = wItemCount;
	UpdateWeightConfig.cbTypeID = TimesWeightItem[0].cbWeightType;
	CopyMemory(UpdateWeightConfig.TimesWeightItem, TimesWeightItem,sizeof(UpdateWeightConfig.TimesWeightItem));

	//���ݴ�С
	WORD wDataSize = sizeof(UpdateWeightConfig) - sizeof(UpdateWeightConfig.TimesWeightItem);
	wDataSize += UpdateWeightConfig.wItemCount*sizeof(UpdateWeightConfig.TimesWeightItem[0]);

	//������Ϣ
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_CONFIG_SERVICE, SUB_GP_UPDATE_WEIGHT_CONFIG, &UpdateWeightConfig, wDataSize);
}

//////////////////////////////////////////////////////////////////////////////////
