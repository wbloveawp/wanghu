#include "StdAfx.h"
#include "Resource.h"
#include "DlgSessionRobot.h"

//////////////////////////////////////////////////////////////////////////////////

//�궨��
#define COLOR_FRAME					RGB(239,249,255)					//�����ɫ
#define COLOR_BORAD					RGB(170,170,170)					//�߿���ɫ

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgSessionRobot, CDlgSessionItem)
	//ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BT_ADD, OnBnClickedBtAppend)
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
CDlgSessionRobot::CDlgSessionRobot() : CDlgSessionItem(IDD_DLG_ROBOT)
{
	//���ñ���
	m_pITCPSocket=NULL;
	m_pIMySelfUserItem=NULL;
	m_wServerType=GAME_GENRE_SCORE;
	m_nSelectItem = -1;

	//������Դ
	m_FrameBrush.CreateSolidBrush(COLOR_FRAME);
	m_BoradPen.CreatePen(PS_SOLID,1,COLOR_BORAD);

	return;
}

//��������
CDlgSessionRobot::~CDlgSessionRobot()
{
}

//�ؼ���
VOID CDlgSessionRobot::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//�󶨿ؼ�
	DDX_Control(pDX, IDC_BT_QUERY, m_btQuery);
	DDX_Control(pDX, IDC_BT_ADD, m_btAppendItem);
	DDX_Control(pDX, IDC_BT_MODIFY, m_btModifyItem);
	DDX_Control(pDX, IDC_BT_DELETE, m_btDeleteItem);
	DDX_Control(pDX, IDC_COMBO_SERVICE_MODE, m_ComboServiceMode);

	//�󶨿ؼ�
	DDX_Control(pDX, IDC_COMBO_GAME_LIST, m_cbGameList);
	DDX_Control(pDX, IDC_COMBO_SERVER_LIST, m_cbServerList);

	//�б�����
	DDX_Control(pDX, IDC_LIST_PARAMETER, m_ParemeterList);
}


//������Ϣ
BOOL CDlgSessionRobot::OnCommand(WPARAM wParam, LPARAM lParam)
{
	DWORD dwCommandID = HIWORD(wParam);
	switch (dwCommandID)
	{
	case CBN_SELCHANGE:
	{
		//��Ϸ�б�
		if (m_cbGameList.m_hWnd == (HWND)lParam)
		{
			//��������
			CWordArray KindArray;
			for (INT i = 0; i < m_cbGameList.GetCount(); i++)
			{
				if (m_cbGameList.GetCheck(i) == static_cast<BOOL>(true))
				{
					tagItemData* pItemData = (tagItemData*)m_cbGameList.GetItemDataPtr(i);
					KindArray.Add(pItemData->dwDataPtr);
				}
			}

			//ɾ������				
			while (m_cbServerList.GetCount()) m_cbServerList.DeleteString(0);

			//���뷿��
			bool bInsert = false;
			POSITION Position = NULL;
			CGameServerItem* pGameServerItem = NULL;
			do
			{
				pGameServerItem = m_pServerListManager->EmunGameServerItem(Position);
				if (pGameServerItem != NULL)
				{
					//���ñ�ʶ
					bInsert = false;

					//ƥ������
					for (INT_PTR i = 0; i < KindArray.GetCount(); i++)
					{
						if (pGameServerItem->m_GameServer.wKindID == KindArray[i])
						{
							bInsert = true;
							break;
						}
					}

					if (bInsert == true)
					{
						m_cbServerList.SetItemData(m_cbServerList.AddString(pGameServerItem->m_GameServer.szServerName), pGameServerItem->m_GameServer.wServerID);
					}

				}
			} while (Position != NULL);
		}

		return TRUE;
	}
	}

	return __super::OnCommand(wParam, lParam);
}

VOID CDlgSessionRobot::OnDestory()
{
	//ɾ������
	m_ParemeterList.DeleteAllItems();
}

//���ú���
BOOL CDlgSessionRobot::OnInitDialog()
{
	__super::OnInitDialog();

	//���ñ���
	SetWindowText(TEXT("AI���� -- ������ֵ��"));

	//��ʼ���ؼ�
	InitControls();

	return FALSE;
}

//ȷ������
VOID CDlgSessionRobot::OnOK()
{
	__super::OnOK();
}

//��Ϣ����
BOOL CDlgSessionRobot::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//�滭��Ϣ
VOID CDlgSessionRobot::OnDrawClientAreaBegin(CDC * pDC, INT nWidth, INT nHeight)
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
VOID CDlgSessionRobot::Initialization(ITCPSocket * pITCPSocket, IClientUserItem * pIMySelfUserItem, tagPCGameServer GameServer)
{
	//���ñ���
	m_pITCPSocket=pITCPSocket;
	m_pIMySelfUserItem=pIMySelfUserItem;
	m_GameServer = GameServer;

	return;
}

//�б����
VOID CDlgSessionRobot::OnEventListFinish()
{
	//��������
	POSITION Position = NULL;
	CGameKindItem* pGameKindItem = NULL;

	do
	{
		pGameKindItem = m_pServerListManager->EmunGameKindItem(Position);
		if (pGameKindItem == NULL) break;

		//��������
		tagItemData* pItemData = (tagItemData*)m_cbGameList.GetItemDataPtr(m_cbGameList.AddString(pGameKindItem->m_GameKind.szKindName));
		if (pItemData != NULL) pItemData->dwDataPtr = pGameKindItem->m_GameKind.wKindID;

	} while (Position != NULL);
}

//�ؼ�ʹ��
VOID CDlgSessionRobot::EnableControls(BOOL bEnabled)
{
	//���ÿؼ�
	m_btQuery.EnableWindow(bEnabled);
	m_btAppendItem.EnableWindow(bEnabled);
	m_btModifyItem.EnableWindow(bEnabled);
	m_btDeleteItem.EnableWindow(bEnabled);
}

//����޸�
VOID CDlgSessionRobot::AddModifyParameter(bool bModify)
{
	//��������
	TCHAR szMessage[128] = TEXT("");
	tagRobotParameter RobotParameter;
	ZeroMemory(&RobotParameter, sizeof(RobotParameter));

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

	//У�鷿��
	if (m_cbServerList.GetCurSel() == -1)
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("��ѡ����Ϸ���䣡"));

		return;
	}

	//������Ŀ
	RobotParameter.dwRobotCount = GetDlgItemInt(IDC_EDIT_COUNT_MEMBER0);

	//����ģʽ
	for (int i = 0; i < m_ComboServiceMode.GetCount(); i++)
	{
		auto pItemData = (tagItemData*)m_ComboServiceMode.GetItemDataPtr(i);
		if (pItemData != NULL && pItemData->bChecked)
		{
			RobotParameter.dwServiceMode |= pItemData->dwDataPtr;
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
		RobotParameter.dwEnterTime = pCombEnterHour->GetCurSel() * 3600;
	}
	if (pCombEnterMinute->GetCurSel() != -1)
	{
		RobotParameter.dwEnterTime += pCombEnterMinute->GetCurSel() * 60;
	}
	if (pCombEnterSecond->GetCurSel() != -1)
	{
		RobotParameter.dwEnterTime += pCombEnterSecond->GetCurSel();
	}

	//�뿪ʱ��
	if (pCombLeaveHour->GetCurSel() != -1)
	{
		RobotParameter.dwLeaveTime = pCombLeaveHour->GetCurSel() * 3600;
	}
	if (pCombLeaveMinute->GetCurSel() != -1)
	{
		RobotParameter.dwLeaveTime += pCombLeaveMinute->GetCurSel() * 60;
	}
	if (pCombLeaveSecond->GetCurSel() != -1)
	{
		RobotParameter.dwLeaveTime += pCombLeaveSecond->GetCurSel();
	}

	//���
	RobotParameter.dwEnterMinInterval = GetDlgItemInt(IDC_EDIT_ENTER_MIN_INTERVAL);
	RobotParameter.dwEnterMaxInterval = GetDlgItemInt(IDC_EDIT_ENTER_MAX_INTERVAL);
	RobotParameter.dwLeaveMinInterval = GetDlgItemInt(IDC_EDIT_LEAVE_MIN_INTERVAL);
	RobotParameter.dwLeaveMaxInterval = GetDlgItemInt(IDC_EDIT_LEAVE_MAX_INTERVAL);

	//Я������
	RobotParameter.lTakeMinScore = GetDlgItemInt(IDC_EDIT_TAKE_MIN_SCORE)*SCORE_MULTIPLE;
	RobotParameter.lTakeMaxScore = GetDlgItemInt(IDC_EDIT_TAKE_MAX_SCORE)*SCORE_MULTIPLE;

	//�뿪���
	RobotParameter.dwSwitchMinInnings = GetDlgItemInt(IDC_EDIT_SWITCH_MIN_INNINGS);
	RobotParameter.dwSwitchMaxInnings = GetDlgItemInt(IDC_EDIT_SWITCH_MAX_INNINGS);

	if (bModify == false)
	{
		//��ȡ����
		WORD wServerID = m_cbServerList.GetItemData(m_cbServerList.GetCurSel());		

		AppendRobotParameter(wServerID, &RobotParameter);
	}
	else
	{
		if (m_nSelectItem == -1) return;
		tagRobotParameter* pItemData = (tagRobotParameter*)m_ParemeterList.GetItemData(m_nSelectItem);
		if (pItemData == NULL) return;

		//��ȡ����
		WORD wServerID = m_cbServerList.GetItemData(m_cbServerList.GetCurSel());;
		if (m_cbServerList.GetCurSel()!=-1) wServerID = m_cbServerList.GetItemData(m_cbServerList.GetCurSel());

		//��������
		RobotParameter.dwBatchID = pItemData->dwBatchID;

		//�޸Ĳ���
		ModifyRobotParameter(wServerID, &RobotParameter);
	}

	return;
}

//ѡ������
VOID CDlgSessionRobot::SelectItem(int nIndex)
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
VOID CDlgSessionRobot::SetItemToParameterList(int nItemIndex, tagRobotParameter* pRobotParameter)
{
	//����У��
	ASSERT(nItemIndex != -1 && pRobotParameter != NULL);
	if (nItemIndex == -1 || pRobotParameter == NULL) return;

	//��������
	TCHAR szItemValue[32] = TEXT("");

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pRobotParameter->dwBatchID);
	m_ParemeterList.SetItemText(nItemIndex, 0, szItemValue);

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pRobotParameter->dwRobotCount);
	m_ParemeterList.SetItemText(nItemIndex, 1, szItemValue);

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pRobotParameter->dwServiceMode);
	m_ParemeterList.SetItemText(nItemIndex, 2, szItemValue);

	//����ʱ��
	WORD wHour = pRobotParameter->dwEnterTime / 3600;
	WORD wMinute = (pRobotParameter->dwEnterTime - wHour*3600)/60;
	WORD wSecond = pRobotParameter->dwEnterTime - wHour*3600 - wMinute*60;

	//����ʱ��
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%02d:%02d:%02d"), wHour, wMinute, wSecond);
	m_ParemeterList.SetItemText(nItemIndex, 3, szItemValue);

	//�뿪ʱ��
	wHour = pRobotParameter->dwLeaveTime / 3600;
	wMinute = (pRobotParameter->dwLeaveTime - wHour * 3600) / 60;
	wSecond = pRobotParameter->dwLeaveTime - wHour * 3600 - wMinute * 60;

	//�뿪ʱ��
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%02d:%02d:%02d"), wHour, wMinute, wSecond);
	m_ParemeterList.SetItemText(nItemIndex, 4, szItemValue);

	//������Ŀ
	SetDlgItemInt(IDC_EDIT_COUNT_MEMBER0, pRobotParameter->dwRobotCount);

	//���
	SetDlgItemInt(IDC_EDIT_ENTER_MIN_INTERVAL, pRobotParameter->dwEnterMinInterval);
	SetDlgItemInt(IDC_EDIT_ENTER_MAX_INTERVAL, pRobotParameter->dwEnterMaxInterval);
	SetDlgItemInt(IDC_EDIT_LEAVE_MIN_INTERVAL, pRobotParameter->dwLeaveMinInterval);
	SetDlgItemInt(IDC_EDIT_LEAVE_MAX_INTERVAL, pRobotParameter->dwLeaveMaxInterval);

	//Я������
	SetDlgItemInt(IDC_EDIT_TAKE_MIN_SCORE, pRobotParameter->lTakeMinScore / SCORE_MULTIPLE);
	SetDlgItemInt(IDC_EDIT_TAKE_MAX_SCORE, pRobotParameter->lTakeMaxScore / SCORE_MULTIPLE);

	//�뿪���
	SetDlgItemInt(IDC_EDIT_SWITCH_MIN_INNINGS, pRobotParameter->dwSwitchMinInnings);
	SetDlgItemInt(IDC_EDIT_SWITCH_MAX_INNINGS, pRobotParameter->dwSwitchMaxInnings);
}

//��ȡ�¼�
bool CDlgSessionRobot::OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	//������
	if (Command.wMainCmdID == MDM_GP_ROBOT_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_ROBOT_OPERATE_RESULT:		//�������
		{
			//����У��
			ASSERT(wDataSize <= sizeof(CMD_GP_RobotOperateResult));
			if (wDataSize > sizeof(CMD_GP_RobotOperateResult)) return false;

			//��ȡ����
			auto pRobotOperateResult = (CMD_GP_RobotOperateResult*)pData;

			//��ȡ����
			switch (pRobotOperateResult->wSubCommdID)
			{
			case SUB_GP_QUERY_PARAMETER:		//��ѯ����
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
				for (WORD wIndex = 0; wIndex < pRobotOperateResult->wParameterCount; wIndex++)
				{
					//��ȡ����
					pTempParameter = &pRobotOperateResult->RobotParameter[wIndex];
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

				break;
			}
			case SUB_GP_APPEND_PARAMETER:		//��Ӳ���
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
				SetItemToParameterList(nItemIndex, &pRobotOperateResult->RobotParameter[0]);

				//������Դ
				tagRobotParameter* pItemData = new tagRobotParameter;
				if (pItemData != NULL)
				{
					//��������
					CopyMemory(pItemData, &pRobotOperateResult->RobotParameter[0], sizeof(tagRobotParameter));

					//��������
					m_ParemeterList.SetItemData(nItemIndex, (DWORD_PTR)pItemData);
				}

				break;
			}
			case SUB_GP_MODIFY_PARAMETER:		//�޸Ĳ���
			{
				auto pRobotParameter = &pRobotOperateResult->RobotParameter[0];

				//���Ҷ���
				for (int nIndex = 0; nIndex < m_ParemeterList.GetItemCount(); nIndex++)
				{
					//��ȡ����
					tagRobotParameter* pItemData = (tagRobotParameter*)m_ParemeterList.GetItemData(nIndex);
					if (pItemData == NULL) continue;

					if (pItemData->dwBatchID == pRobotParameter->dwBatchID)
					{
						//��������
						CopyMemory(pItemData, pRobotParameter, sizeof(tagRobotParameter));
						//��������
						SetItemToParameterList(nIndex, pRobotParameter);

						break;
					}
				}

				break;
			}
			case SUB_GP_DELETE_PARAMETER:	//ɾ������
			{
				auto pRobotParameter = &pRobotOperateResult->RobotParameter[0];

				//���Ҷ���
				for (int nIndex = 0; nIndex < m_ParemeterList.GetItemCount(); nIndex++)
				{
					tagRobotParameter* pItemData = (tagRobotParameter*)m_ParemeterList.GetItemData(nIndex);
					if (pItemData == NULL) continue;

					if (pItemData->dwBatchID == pRobotParameter->dwBatchID)
					{
						//��������
						m_ParemeterList.DeleteItem(nIndex);

						break;
					}
				}

				break;
			}
			}

			//���ÿؼ�
			EnableControls(TRUE);

			return true;
		}
		}
	}

	//����ʧ��
	if (Command.wMainCmdID == MDM_GP_USER_SERVICE && Command.wSubCmdID == SUB_GP_OPERATE_FAILURE)
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

//��ʼ���ؼ�
VOID CDlgSessionRobot::InitControls()
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
VOID CDlgSessionRobot::OnHdnListParameterItemclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	//ѡ������
	SelectItem(phdr->iItem);

	*pResult = 0;
}

//ɾ������
VOID CDlgSessionRobot::OnLvnListParameterDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
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
VOID CDlgSessionRobot::OnLvnKeydownListParameter(NMHDR *pNMHDR, LRESULT *pResult)
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
VOID CDlgSessionRobot::OnBnClickedBtAppend()
{
	//�������
	AddModifyParameter(false);
}

//�༭����
VOID CDlgSessionRobot::OnBnClickedBtModify()
{
	//�������
	AddModifyParameter(true);
}

//ɾ������
VOID CDlgSessionRobot::OnBnClickedBtDelete()
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
	WORD wServerID = m_cbServerList.GetItemData(m_cbServerList.GetCurSel());

	//ɾ������
	DeleteRobotParameter(wServerID, pItemData->dwBatchID);

	//���ñ���
	m_nSelectItem = -1;

	return;
}

//��ѯ����
VOID CDlgSessionRobot::OnBnClickedBtQuery()
{
	//У�鷿��
	if (m_cbServerList.GetCurSel()==-1)
	{
		//ѡ����ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("��ѡ��һ����Ϸ���䣡"), MB_OK);

		return;
	}

	//��ȡ����
	WORD wServerID = m_cbServerList.GetItemData(m_cbServerList.GetCurSel());
	
	//��ȡ����
	QueryRobotParameter(wServerID);

	return;
}

// 
//��ȡ����
bool CDlgSessionRobot::QueryRobotParameter(WORD wServerID)
{
	//��ȡ����
	CMD_GP_QueryParameter QueryParameter = {};

	//���ñ���
	QueryParameter.wServerID = wServerID;
	
	//��������
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_ROBOT_SERVICE, SUB_GP_QUERY_PARAMETER, &QueryParameter, sizeof(QueryParameter));
}

//ɾ������
bool CDlgSessionRobot::DeleteRobotParameter(WORD wServerID, DWORD dwBatchID)
{
	//��ȡ����
	CMD_GP_DeleteParameter DeleteParameter = {};

	//���ñ���
	DeleteParameter.wServerID = wServerID;
	DeleteParameter.dwBatchID = dwBatchID;	

	//��������
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_ROBOT_SERVICE, SUB_GP_DELETE_PARAMETER, &DeleteParameter, sizeof(DeleteParameter));
}

//�޸Ĳ���
bool CDlgSessionRobot::ModifyRobotParameter(WORD wServerID, tagRobotParameter* pRobotParameter)
{
	//��ȡ����
	CMD_GP_ModifyParameter ModifyParameter = {};

	//���ñ���
	ModifyParameter.wServerID = wServerID;
	CopyMemory(&ModifyParameter.RobotParameter, pRobotParameter, sizeof(tagRobotParameter));

	//��������
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_ROBOT_SERVICE, SUB_GP_MODIFY_PARAMETER, &ModifyParameter, sizeof(ModifyParameter));
}

//��Ӳ���
bool CDlgSessionRobot::AppendRobotParameter(WORD wServerID, tagRobotParameter* pRobotParameter)
{
	//��ȡ����
	CMD_GP_AppendParameter AppendParameter = {};

	//���ñ���
	AppendParameter.wServerID = wServerID;
	CopyMemory(&AppendParameter.RobotParameter, pRobotParameter, sizeof(tagRobotParameter));

	//��������
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_ROBOT_SERVICE, SUB_GP_APPEND_PARAMETER, &AppendParameter, sizeof(AppendParameter));
}

//////////////////////////////////////////////////////////////////////////////////
