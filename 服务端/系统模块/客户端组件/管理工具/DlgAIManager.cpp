#include "StdAfx.h"
#include "Resource.h"
#include "DlgAIManager.h"
//#include "DlgInformation.h"

//////////////////////////////////////////////////////////////////////////////////

//�궨��
#define COLOR_FRAME					RGB(239,249,255)					//�����ɫ
#define COLOR_BORAD					RGB(170,170,170)					//�߿���ɫ

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgAIManager, CDlgOptionItem)
	//ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BT_ADD, OnBnClickedBtAdd)
	ON_BN_CLICKED(IDC_BT_MODIFY, OnBnClickedBtModify)
	ON_BN_CLICKED(IDC_BT_DELETE, OnBnClickedBtDelete)
	ON_BN_CLICKED(IDC_BT_QUERY, OnBnClickedBtQuery)

	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST_PARAMETER, OnLvnListParameterDeleteitem)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PARAMETER, OnHdnListParameterItemclick)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_PARAMETER, OnLvnKeydownListParameter)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgAIManager::CDlgAIManager() : CDlgOptionItem(IDD_DLG_AI_MANAGER)
{
	//���ñ���
	m_pITCPSocket=NULL;
	m_pIMySelfUserItem=NULL;
	m_wServerType=GAME_GENRE_SCORE;
	m_nSelectItem = -1;

	//������Դ
	m_FrameBrush.CreateSolidBrush(COLOR_FRAME);
	m_BoradPen.CreatePen(PS_SOLID,1,COLOR_BORAD);

	//�������
	m_MissionManager.InsertMissionItem(&m_MissionAndroid);

	return;
}

//��������
CDlgAIManager::~CDlgAIManager()
{
}

//�ؼ���
VOID CDlgAIManager::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//�󶨿ؼ�
	DDX_Control(pDX, IDC_BT_QUERY, m_btQuery);
	DDX_Control(pDX, IDC_BT_ADD, m_btAddItem);
	DDX_Control(pDX, IDC_BT_MODIFY, m_btModifyItem);
	DDX_Control(pDX, IDC_BT_DELETE, m_btDeleteItem);
	DDX_Control(pDX, IDC_COMBO_SERVICE_MODE, m_ComboServiceMode);

	//�б�����
	DDX_Control(pDX, IDC_LIST_PARAMETER, m_ParemeterList);
}

VOID CDlgAIManager::OnDestory()
{
	//ɾ������
	m_ParemeterList.DeleteAllItems();
}

//���ú���
BOOL CDlgAIManager::OnInitDialog()
{
	__super::OnInitDialog();

	//���ñ���
	SetWindowText(TEXT("AI���� -- ������ֵ��"));

	//��ʼ���ؼ�
	InitControls();

	//���ýӿ�
	m_MissionAndroid.SetAndroidOperateCenter(this);

	return FALSE;
}

//ȷ������
VOID CDlgAIManager::OnOK()
{
	__super::OnOK();
}

//��Ϣ����
BOOL CDlgAIManager::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//�滭��Ϣ
VOID CDlgAIManager::OnDrawClientAreaBegin(CDC * pDC, INT nWidth, INT nHeight)
{
	////ѡ�����
	//pDC->SelectObject(m_BoradPen);
	//pDC->SelectObject(m_FrameBrush);

	////�滭�׿�
	//pDC->RoundRect(20,47,510,162,5,5);
	//pDC->RoundRect(20,172,510,320,5,5);
	//pDC->RoundRect(20,330,510,475,5,5);

	return;
}

//���ù���
VOID CDlgAIManager::Initialization(ITCPSocket * pITCPSocket, IClientUserItem * pIMySelfUserItem, CCheckComboBox * pCheckComboBox, tagPCGameServer GameServer)
{
	//���ñ���
	m_pITCPSocket=pITCPSocket;
	m_pIMySelfUserItem=pIMySelfUserItem;
	m_pServerList = pCheckComboBox;
	m_GameServer = GameServer;

	//��ѯ����
	//OnBnClickedBtQuery();

	return;
}

//�ؼ�ʹ��
VOID CDlgAIManager::EnableControls(BOOL bEnabled)
{
	//ʹ������
	//GetDlgItem(IDC_BT_EXECUTE_CMD)->EnableWindow(bEnabled);
}

//��������
WORD CDlgAIManager::ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize)
{
	return 0;
}

//��ɫ��Ϣ
//HBRUSH CDlgAIManager::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
//{
//	switch (nCtlColor)
//	{
//	case CTLCOLOR_DLG:
//	case CTLCOLOR_STATIC:
//		{
//			pDC->SetBkMode(TRANSPARENT);
//			pDC->SetBkColor(COLOR_FRAME);
//			pDC->SetTextColor(CSkinDialog::m_SkinAttribute.m_crControlText);
//			return CSkinDialog::m_SkinAttribute.m_brLabelRound;
//		}
//	}
//
//	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
//}

//����޸�
VOID CDlgAIManager::AddModifyParameter(bool bModify)
{
	//��������
	TCHAR szMessage[128] = TEXT("");
	tagRobotParameter AndroidParameter;
	ZeroMemory(&AndroidParameter, sizeof(AndroidParameter));

	//ѡ���ж�
	if (bModify == true && m_nSelectItem == -1) return;

	//������ĿУ��
	if (GetDlgItemInt(IDC_EDIT_COUNT_MEMBER0) <= 0 || GetDlgItemInt(IDC_EDIT_COUNT_MEMBER0) > 100)
	{
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��Ǹ,ÿ�������AI��Ŀ���Ϊ%d��,���������룡"), 100);

		if (GetDlgItemInt(IDC_EDIT_COUNT_MEMBER0) <= 0)
		{
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��Ǹ,ÿ�������AI���������ÿգ�"));
		}
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), szMessage);

		return;
	}

	//������Ŀ
	AndroidParameter.dwRobotCount = GetDlgItemInt(IDC_EDIT_COUNT_MEMBER0);

	//����ģʽ
	for (int i = 0; i < m_ComboServiceMode.GetCount(); i++)
	{
		auto pItemData = (tagItemData*)m_ComboServiceMode.GetItemDataPtr(i);
		if (pItemData != NULL && pItemData->bChecked)
		{
			AndroidParameter.dwServiceMode |= pItemData->dwDataPtr;
		}
	}

	//��ȡ�ؼ�
	auto pCombEnterHour = (CComboBox*)GetDlgItem(IDC_COMBO_ENTER_HOUR);
	auto pCombEnterMinute = (CComboBox*)GetDlgItem(IDC_COMBO_ENTER_MINUTE);
	auto pCombEnterSecond = (CComboBox*)GetDlgItem(IDC_COMBO_ENTER_SECOND);
	auto pCombLeaveHour = (CComboBox*)GetDlgItem(IDC_COMBO_LEAVE_HOUR);
	auto pCombLeaveMinute = (CComboBox*)GetDlgItem(IDC_COMBO_LEAVE_MINUTE);
	auto pCombLeaveSecond = (CComboBox*)GetDlgItem(IDC_COMBO_LEAVE_SECOND);

	// ����ʱ��
	if (pCombEnterHour->GetCurSel() != -1)
	{
		AndroidParameter.dwEnterTime = pCombEnterHour->GetCurSel() * 3600;
	}
	if (pCombEnterMinute->GetCurSel() != -1)
	{
		AndroidParameter.dwEnterTime += pCombEnterMinute->GetCurSel() * 60;
	}
	if (pCombEnterSecond->GetCurSel() != -1)
	{
		AndroidParameter.dwEnterTime += pCombEnterSecond->GetCurSel();
	}

	//�뿪ʱ��
	if (pCombLeaveHour->GetCurSel() != -1)
	{
		AndroidParameter.dwLeaveTime = pCombLeaveHour->GetCurSel() * 3600;
	}
	if (pCombLeaveMinute->GetCurSel() != -1)
	{
		AndroidParameter.dwLeaveTime += pCombLeaveMinute->GetCurSel() * 60;
	}
	if (pCombLeaveSecond->GetCurSel() != -1)
	{
		AndroidParameter.dwLeaveTime += pCombLeaveSecond->GetCurSel();
	}

	//���
	AndroidParameter.dwEnterMinInterval = GetDlgItemInt(IDC_EDIT_ENTER_MIN_INTERVAL);
	AndroidParameter.dwEnterMaxInterval = GetDlgItemInt(IDC_EDIT_ENTER_MAX_INTERVAL);
	AndroidParameter.dwLeaveMinInterval = GetDlgItemInt(IDC_EDIT_LEAVE_MIN_INTERVAL);
	AndroidParameter.dwLeaveMaxInterval = GetDlgItemInt(IDC_EDIT_LEAVE_MAX_INTERVAL);

	//Я������
	AndroidParameter.lTakeMinScore = GetDlgItemInt(IDC_EDIT_TAKE_MIN_SCORE)*SCORE_MULTIPLE;
	AndroidParameter.lTakeMaxScore = GetDlgItemInt(IDC_EDIT_TAKE_MAX_SCORE)*SCORE_MULTIPLE;

	//�뿪���
	AndroidParameter.dwSwitchMinInnings = GetDlgItemInt(IDC_EDIT_SWITCH_MIN_INNINGS);
	AndroidParameter.dwSwitchMaxInnings = GetDlgItemInt(IDC_EDIT_SWITCH_MAX_INNINGS);

	if (bModify == false)
	{
		//��Ŀ�ж�
		//if (m_ParemeterList.GetItemCount() >= 1)
		//{
		//	_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��Ǹ,ÿ���������ֻ�����1��AI������"));

		//	CInformation Information;
		//	Information.ShowMessageBox(TEXT("��ʾ"), szMessage);

		//	return;
		//}

		//��ȡ����
		WORD wServerID = INVALID_WORD;
		for (INT i = 0; i < m_pServerList->GetCount(); i++)
		{
			if (m_pServerList->GetCheck(i) == static_cast<BOOL>(true))
			{
				//��ȡ����
				tagItemData* pItemData = (tagItemData*)m_pServerList->GetItemData(i);
				wServerID = (WORD)pItemData->dwDataPtr;
				break;
			}
		}

		m_MissionAndroid.AddAndroidParameter(wServerID, &AndroidParameter);
	}
	else
	{
		if (m_nSelectItem == -1) return;
		tagRobotParameter* pItemData = (tagRobotParameter*)m_ParemeterList.GetItemData(m_nSelectItem);
		if (pItemData == NULL) return;

		//��ȡ����
		WORD wServerID = INVALID_WORD;
		for (INT i = 0; i < m_pServerList->GetCount(); i++)
		{
			if (m_pServerList->GetCheck(i) == static_cast<BOOL>(true))
			{
				//��ȡ����
				tagItemData* pItemData = (tagItemData*)m_pServerList->GetItemData(i);
				wServerID = (WORD)pItemData->dwDataPtr;
				break;
			}
		}

		AndroidParameter.dwBatchID = pItemData->dwBatchID;

		//�޸Ĳ���
		m_MissionAndroid.ModifyAndroidParameter(wServerID, &AndroidParameter);
	}

	return;
}

//ѡ������
VOID CDlgAIManager::SelectItem(int nIndex)
{
	//���ñ���
	m_nSelectItem = nIndex;
	if (m_nSelectItem == -1) return;

	//��ȡ����
	tagRobotParameter* pItemData = (tagRobotParameter*)m_ParemeterList.GetItemData(m_nSelectItem);
	if (pItemData == NULL) return;

	//������Ŀ
	SetDlgItemInt(IDC_EDIT_COUNT_MEMBER0, pItemData->dwRobotCount);

	//����ģʽ
	SetDlgItemInt(IDC_EDIT_SERVICE_MODE, pItemData->dwServiceMode);

	//ѡ��ģʽ
	for (int i = 0; i < m_ComboServiceMode.GetCount(); i++)
	{
		auto pItem =(tagItemData *)m_ComboServiceMode.GetItemDataPtr(i);
		if (pItem != NULL)
		{
			pItem->bChecked = (pItem->dwDataPtr & pItemData->dwServiceMode)?true:false;
		}
	}

	//����ʱ��
	WORD wEnterHour = pItemData->dwEnterTime / 3600;
	WORD wEnterMimute = (pItemData->dwEnterTime - wEnterHour*3600)/60;
	WORD wEnterSecond = pItemData->dwEnterTime - wEnterHour * 3600 - wEnterMimute*60;

	//�뿪ʱ��
	WORD wLeaveHour = pItemData->dwLeaveTime / 3600;
	WORD wLeaveMimute = (pItemData->dwLeaveTime - wLeaveHour * 3600) / 60;
	WORD wLeaveSecond = pItemData->dwLeaveTime - wLeaveHour * 3600 - wLeaveMimute * 60;

	//��ȡ�ؼ�
	auto pCombEnterHour = (CComboBox*)GetDlgItem(IDC_COMBO_ENTER_HOUR);
	auto pCombEnterMinute = (CComboBox*)GetDlgItem(IDC_COMBO_ENTER_MINUTE);
	auto pCombEnterSecond = (CComboBox*)GetDlgItem(IDC_COMBO_ENTER_SECOND);
	auto pCombLeaveHour = (CComboBox*)GetDlgItem(IDC_COMBO_LEAVE_HOUR);
	auto pCombLeaveMinute = (CComboBox*)GetDlgItem(IDC_COMBO_LEAVE_MINUTE);
	auto pCombLeaveSecond = (CComboBox*)GetDlgItem(IDC_COMBO_LEAVE_SECOND);	

	//���ÿؼ�
	pCombEnterHour->SetCurSel(wEnterHour); pCombEnterMinute->SetCurSel(wEnterMimute); pCombEnterSecond->SetCurSel(wEnterSecond);
	pCombLeaveHour->SetCurSel(wLeaveHour); pCombLeaveMinute->SetCurSel(wLeaveMimute); pCombLeaveSecond->SetCurSel(wLeaveSecond);

	//���
	SetDlgItemInt(IDC_EDIT_ENTER_MIN_INTERVAL, pItemData->dwEnterMinInterval);
	SetDlgItemInt(IDC_EDIT_ENTER_MAX_INTERVAL, pItemData->dwEnterMaxInterval);
	SetDlgItemInt(IDC_EDIT_LEAVE_MIN_INTERVAL, pItemData->dwLeaveMinInterval);
	SetDlgItemInt(IDC_EDIT_LEAVE_MAX_INTERVAL, pItemData->dwLeaveMaxInterval);

	//Я������
	SetDlgItemInt(IDC_EDIT_TAKE_MIN_SCORE, pItemData->lTakeMinScore / SCORE_MULTIPLE);
	SetDlgItemInt(IDC_EDIT_TAKE_MAX_SCORE, pItemData->lTakeMaxScore / SCORE_MULTIPLE);

	//�뿪���
	SetDlgItemInt(IDC_EDIT_SWITCH_MIN_INNINGS, pItemData->dwSwitchMinInnings);
	SetDlgItemInt(IDC_EDIT_SWITCH_MAX_INNINGS, pItemData->dwSwitchMaxInnings);
}

//��������
VOID CDlgAIManager::SetItemToParameterList(int nItemIndex, tagRobotParameter* pAndroidParameter)
{
	//����У��
	ASSERT(nItemIndex != -1 && pAndroidParameter != NULL);
	if (nItemIndex == -1 || pAndroidParameter == NULL) return;

	//��������
	TCHAR szItemValue[32] = TEXT("");

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwBatchID);
	m_ParemeterList.SetItemText(nItemIndex, 0, szItemValue);

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwRobotCount);
	m_ParemeterList.SetItemText(nItemIndex, 1, szItemValue);

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwServiceMode);
	m_ParemeterList.SetItemText(nItemIndex, 2, szItemValue);

	//����ʱ��
	WORD wHour = pAndroidParameter->dwEnterTime / 3600;
	WORD wMinute = (pAndroidParameter->dwEnterTime - wHour*3600)/60;
	WORD wSecond = pAndroidParameter->dwEnterTime - wHour*3600 - wMinute*60;

	//����ʱ��
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%02d:%02d:%02d"), wHour, wMinute, wSecond);
	m_ParemeterList.SetItemText(nItemIndex, 3, szItemValue);

	//�뿪ʱ��
	wHour = pAndroidParameter->dwLeaveTime / 3600;
	wMinute = (pAndroidParameter->dwLeaveTime - wHour * 3600) / 60;
	wSecond = pAndroidParameter->dwLeaveTime - wHour * 3600 - wMinute * 60;

	//�뿪ʱ��
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%02d:%02d:%02d"), wHour, wMinute, wSecond);
	m_ParemeterList.SetItemText(nItemIndex, 4, szItemValue);

	//������Ŀ
	SetDlgItemInt(IDC_EDIT_COUNT_MEMBER0, pAndroidParameter->dwRobotCount);

	//���
	SetDlgItemInt(IDC_EDIT_ENTER_MIN_INTERVAL, pAndroidParameter->dwEnterMinInterval);
	SetDlgItemInt(IDC_EDIT_ENTER_MAX_INTERVAL, pAndroidParameter->dwEnterMaxInterval);
	SetDlgItemInt(IDC_EDIT_LEAVE_MIN_INTERVAL, pAndroidParameter->dwLeaveMinInterval);
	SetDlgItemInt(IDC_EDIT_LEAVE_MAX_INTERVAL, pAndroidParameter->dwLeaveMaxInterval);

	//Я������
	SetDlgItemInt(IDC_EDIT_TAKE_MIN_SCORE, pAndroidParameter->lTakeMinScore / SCORE_MULTIPLE);
	SetDlgItemInt(IDC_EDIT_TAKE_MAX_SCORE, pAndroidParameter->lTakeMaxScore / SCORE_MULTIPLE);

	//�뿪���
	SetDlgItemInt(IDC_EDIT_SWITCH_MIN_INNINGS, pAndroidParameter->dwSwitchMinInnings);
	SetDlgItemInt(IDC_EDIT_SWITCH_MAX_INNINGS, pAndroidParameter->dwSwitchMaxInnings);
}

//�ؼ�ʹ��
VOID CDlgAIManager::OnEventEnableControls(BOOL bEnabled)
{
	//���ÿؼ�
	m_btQuery.EnableWindow(bEnabled);
	m_btAddItem.EnableWindow(bEnabled);
	m_btModifyItem.EnableWindow(bEnabled);
	m_btDeleteItem.EnableWindow(bEnabled);
}

//��������
VOID CDlgAIManager::OnEventAndroidParenter(WORD wSubCommdID, WORD wParameterCount, tagRobotParameter* pAndroidParameter)
{
	//��ȡ����
	if (wSubCommdID == SUB_GP_GET_PARAMETER)
	{
		//��������
		int nItemIndex = -1;
		tagRobotParameter* pTempParameter = NULL;
		tagRobotParameter* pItemData = NULL;

		//�������
		m_ParemeterList.DeleteAllItems();

		//��������
		LVITEM lVItem;
		lVItem.mask = LVIF_TEXT;
		lVItem.iSubItem = 0;
		lVItem.pszText = TEXT("");

		//ѭ������
		for (WORD wIndex = 0; wIndex < wParameterCount; wIndex++)
		{
			//��ȡ����
			pTempParameter = &pAndroidParameter[wIndex];
			if (pTempParameter == NULL) continue;

			//��������.
			lVItem.iItem = wIndex;
			nItemIndex = m_ParemeterList.InsertItem(&lVItem);
			if (nItemIndex == -1) continue;

			//��������
			SetItemToParameterList(nItemIndex, pTempParameter);

			//������Դ
			pItemData = new tagRobotParameter;
			if (pItemData != NULL)
			{
				//��������
				CopyMemory(pItemData, pTempParameter, sizeof(tagRobotParameter));

				//��������
				m_ParemeterList.SetItemData(nItemIndex, (DWORD_PTR)pItemData);
			}
		}

		return;
	}

	//��Ӳ���
	if (wSubCommdID == SUB_GP_ADD_PARAMETER)
	{
		//��������
		int nItemIndex = -1;

		//��������
		LVITEM lVItem;
		lVItem.mask = LVIF_TEXT;
		lVItem.iSubItem = 0;
		lVItem.pszText = TEXT("");

		//��������
		lVItem.iItem = m_ParemeterList.GetItemCount();
		nItemIndex = m_ParemeterList.InsertItem(&lVItem);

		//��������
		SetItemToParameterList(nItemIndex, pAndroidParameter);

		//������Դ
		tagRobotParameter* pItemData = new tagRobotParameter;
		if (pItemData != NULL)
		{
			//��������
			CopyMemory(pItemData, pAndroidParameter, sizeof(tagRobotParameter));

			//��������
			m_ParemeterList.SetItemData(nItemIndex, (DWORD_PTR)pItemData);
		}

		return;
	}

	//�޸Ĳ���
	if (wSubCommdID == SUB_GP_MODIFY_PARAMETER)
	{
		//��������
		//ASSERT(m_ParemeterList.GetItemCount() == MAX_BATCH);

		//���Ҷ���
		for (int nIndex = 0; nIndex < m_ParemeterList.GetItemCount(); nIndex++)
		{
			//��ȡ����
			tagRobotParameter* pItemData = (tagRobotParameter*)m_ParemeterList.GetItemData(nIndex);
			if (pItemData == NULL) continue;

			if (pItemData->dwBatchID == pAndroidParameter->dwBatchID)
			{
				//��������
				CopyMemory(pItemData, pAndroidParameter, sizeof(tagRobotParameter));
				//��������
				SetItemToParameterList(nIndex, pAndroidParameter);

				break;
			}
			
		}

		return;
	}

	//ɾ������
	if (wSubCommdID == SUB_GP_DELETE_PARAMETER)
	{
		//��������
		//ASSERT(m_ParemeterList.GetItemCount() == MAX_BATCH);

		//���Ҷ���
		for (int nIndex = 0; nIndex < m_ParemeterList.GetItemCount(); nIndex++)
		{
			tagRobotParameter* pItemData = (tagRobotParameter*)m_ParemeterList.GetItemData(nIndex);
			if (pItemData == NULL) continue;

			if (pItemData->dwBatchID == pAndroidParameter->dwBatchID)
			{
				//��������
				m_ParemeterList.DeleteItem(nIndex);

				break;
			}

		}

		return;
	}

	return;
}

//��ʼ���ؼ�
VOID CDlgAIManager::InitControls()
{
	//���ÿؼ�
	DWORD dwExstyle = m_ParemeterList.GetExtendedStyle();
	m_ParemeterList.SetExtendedStyle(dwExstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | WS_EX_STATICEDGE);

	//�б�ؼ�
	m_ParemeterList.InsertColumn(0, TEXT("����ID"), LVCFMT_CENTER, 60);
	m_ParemeterList.InsertColumn(1, TEXT("������Ŀ"), LVCFMT_CENTER, 120);
	m_ParemeterList.InsertColumn(2, TEXT("����ģʽ"), LVCFMT_CENTER, 120);
	m_ParemeterList.InsertColumn(3, TEXT("����ʱ��"), LVCFMT_CENTER, 120);
	m_ParemeterList.InsertColumn(4, TEXT("�뿪ʱ��"), LVCFMT_CENTER, 120);

	//����ģʽ
	WORD wServiceMode[] = {ROBOT_SIMULATE,ROBOT_INITIATIVE,ROBOT_PASSIVITY };
	LPCTSTR pszServiceMode[] = {TEXT("�໥ģ��"),TEXT("�������"),TEXT("�������")};

	for (int i = 0; i < CountArray(wServiceMode); i++)
	{
		tagItemData* pItemData = (tagItemData*)m_ComboServiceMode.GetItemDataPtr(m_ComboServiceMode.InsertString(i,pszServiceMode[i]));
		if (pItemData != NULL)
		{
			pItemData->bChecked = true;
			pItemData->dwDataPtr = wServiceMode[i];
		}
	}

	//��Ͽؼ�
	auto pCombEnterHour =	(CComboBox*)GetDlgItem(IDC_COMBO_ENTER_HOUR);
	auto pCombEnterMinute = (CComboBox*)GetDlgItem(IDC_COMBO_ENTER_MINUTE);
	auto pCombEnterSecond = (CComboBox*)GetDlgItem(IDC_COMBO_ENTER_SECOND);
	auto pCombLeaveHour =	(CComboBox*)GetDlgItem(IDC_COMBO_LEAVE_HOUR);
	auto pCombLeaveMinute = (CComboBox*)GetDlgItem(IDC_COMBO_LEAVE_MINUTE);
	auto pCombLeaveSecond = (CComboBox*)GetDlgItem(IDC_COMBO_LEAVE_SECOND);

	//��������
	TCHAR szValue[8];

	//ʱ
	for (int i = 0; i < 24; i++)
	{
		_itot_s(i, szValue, 10);
		pCombEnterHour->SetItemData(pCombEnterHour->InsertString(i, szValue), i);
		pCombLeaveHour->SetItemData(pCombLeaveHour->InsertString(i, szValue), i);
	}

	//��
	for (int i = 0; i < 60; i++)
	{
		_itot_s(i, szValue, 10);
		pCombEnterMinute->SetItemData(pCombEnterMinute->InsertString(i, szValue), i);
		pCombLeaveMinute->SetItemData(pCombLeaveMinute->InsertString(i, szValue), i);
	}

	//��
	for (int i = 0; i < 60; i++)
	{
		_itot_s(i, szValue, 10);
		pCombEnterSecond->SetItemData(pCombEnterSecond->InsertString(i, szValue), i);
		pCombLeaveSecond->SetItemData(pCombLeaveSecond->InsertString(i, szValue), i);
	}

	//Ĭ��ʱ��
	pCombEnterHour->SetCurSel(9);pCombEnterMinute->SetCurSel(0);pCombEnterSecond->SetCurSel(0);
	pCombLeaveHour->SetCurSel(21);pCombLeaveMinute->SetCurSel(0);pCombLeaveSecond->SetCurSel(0);	

	//������Ŀ
	SetDlgItemInt(IDC_EDIT_COUNT_MEMBER0, 20);

	//���
	SetDlgItemInt(IDC_EDIT_ENTER_MIN_INTERVAL, 3);
	SetDlgItemInt(IDC_EDIT_ENTER_MAX_INTERVAL, 5);
	SetDlgItemInt(IDC_EDIT_LEAVE_MIN_INTERVAL, 3);
	SetDlgItemInt(IDC_EDIT_LEAVE_MAX_INTERVAL, 5);

	//Я������
	SetDlgItemInt(IDC_EDIT_TAKE_MIN_SCORE, 25000);
	SetDlgItemInt(IDC_EDIT_TAKE_MAX_SCORE, 50000);

	//�뿪���
	SetDlgItemInt(IDC_EDIT_SWITCH_MIN_INNINGS, 2);
	SetDlgItemInt(IDC_EDIT_SWITCH_MAX_INNINGS, 5);
	
}

//��������
VOID CDlgAIManager::OnHdnListParameterItemclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	//ѡ������
	SelectItem(phdr->iItem);

	*pResult = 0;
}

//ɾ������
VOID CDlgAIManager::OnLvnListParameterDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	//��ȡ����
	tagRobotParameter* pItemData = (tagRobotParameter*)m_ParemeterList.GetItemData(pNMLV->iItem);
	if (pItemData != NULL)
	{
		SafeDelete(pItemData);
	}

	*pResult = 0;
}

//���̰���
VOID CDlgAIManager::OnLvnKeydownListParameter(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	//��������
	int nItem = -1;
	POSITION pos;

	switch (pLVKeyDow->wVKey)
	{
	case VK_DOWN:
	{
		pos = m_ParemeterList.GetFirstSelectedItemPosition();
		while (pos)
		{
			//��ȡ����
			nItem = m_ParemeterList.GetNextSelectedItem(pos);
			if (nItem == -1) break;

			//��������
			nItem = __min(nItem + 1, m_ParemeterList.GetItemCount() - 1);
			SelectItem(nItem);
			break;
		}

		break;
	}
	case VK_UP:
	{
		pos = m_ParemeterList.GetFirstSelectedItemPosition();
		while (pos)
		{
			//��ȡ����
			nItem = m_ParemeterList.GetNextSelectedItem(pos);
			if (nItem == -1) break;

			//��������
			nItem = __max(nItem - 1, 0);
			SelectItem(nItem);
			break;
		}
		break;
	}
	}

	*pResult = 0;
}

//�������
VOID CDlgAIManager::OnBnClickedBtAdd()
{
	//�������
	AddModifyParameter(false);
}

//�༭����
VOID CDlgAIManager::OnBnClickedBtModify()
{
	//�������
	AddModifyParameter(true);
}

//ɾ������
VOID CDlgAIManager::OnBnClickedBtDelete()
{
	//��ȡ����
	if (m_nSelectItem == -1) return;
	if (m_ParemeterList.GetItemCount() <= m_nSelectItem)
	{
		m_nSelectItem = -1;
		return;
	}

	//��ȡ����
	tagRobotParameter* pItemData = (tagRobotParameter*)m_ParemeterList.GetItemData(m_nSelectItem);
	if (pItemData == NULL) return;

	//��ȡʱ��
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	DWORD dwTodayTickCount = SystemTime.wHour * 3600 + SystemTime.wMinute * 60 + SystemTime.wSecond;

	//ɾ����ʾ
	{
		//ѡ����ʾ
		CInformation Information;
		INT nRetCode = Information.ShowMessageBox(TEXT("����"), TEXT("����AI�����ڷ���״̬,��ȷ��Ҫɾ����"), MB_YESNO);

		//��ʱ���˳�
		if (nRetCode == IDNO) return;
	}

	//��ȡ����
	WORD wServerID = INVALID_WORD;
	for (INT i = 0; i < m_pServerList->GetCount(); i++)
	{
		if (m_pServerList->GetCheck(i) == static_cast<BOOL>(true))
		{
			//��ȡ����
			tagItemData* pItemData = (tagItemData*)m_pServerList->GetItemData(i);
			wServerID = (WORD)pItemData->dwDataPtr;
			break;
		}
	}
	//ɾ������
	m_MissionAndroid.DeleteAndroidParameter(wServerID, pItemData->dwBatchID);

	//���ñ���
	m_nSelectItem = -1;

	return;
}

//��ѯ����
VOID CDlgAIManager::OnBnClickedBtQuery()
{
	//��ȡ����
	WORD wServerID = INVALID_WORD;
	for (INT i = 0; i < m_pServerList->GetCount(); i++)
	{
		if (m_pServerList->GetCheck(i) == static_cast<BOOL>(true))
		{
			//��ȡ����
			tagItemData* pItemData = (tagItemData*)m_pServerList->GetItemData(i);
			wServerID = (WORD)pItemData->dwDataPtr;
			break;
		}
	}

	//��ȡ����
	m_MissionAndroid.GetAndroidParameter(wServerID);

	return;
}

//////////////////////////////////////////////////////////////////////////////////
