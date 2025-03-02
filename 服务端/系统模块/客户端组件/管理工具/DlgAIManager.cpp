#include "StdAfx.h"
#include "Resource.h"
#include "DlgAIManager.h"
//#include "DlgInformation.h"

//////////////////////////////////////////////////////////////////////////////////

//宏定义
#define COLOR_FRAME					RGB(239,249,255)					//填充颜色
#define COLOR_BORAD					RGB(170,170,170)					//边框颜色

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

//构造函数
CDlgAIManager::CDlgAIManager() : CDlgOptionItem(IDD_DLG_AI_MANAGER)
{
	//设置变量
	m_pITCPSocket=NULL;
	m_pIMySelfUserItem=NULL;
	m_wServerType=GAME_GENRE_SCORE;
	m_nSelectItem = -1;

	//创建资源
	m_FrameBrush.CreateSolidBrush(COLOR_FRAME);
	m_BoradPen.CreatePen(PS_SOLID,1,COLOR_BORAD);

	//任务组件
	m_MissionManager.InsertMissionItem(&m_MissionAndroid);

	return;
}

//析构函数
CDlgAIManager::~CDlgAIManager()
{
}

//控件绑定
VOID CDlgAIManager::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//绑定控件
	DDX_Control(pDX, IDC_BT_QUERY, m_btQuery);
	DDX_Control(pDX, IDC_BT_ADD, m_btAddItem);
	DDX_Control(pDX, IDC_BT_MODIFY, m_btModifyItem);
	DDX_Control(pDX, IDC_BT_DELETE, m_btDeleteItem);
	DDX_Control(pDX, IDC_COMBO_SERVICE_MODE, m_ComboServiceMode);

	//列表配置
	DDX_Control(pDX, IDC_LIST_PARAMETER, m_ParemeterList);
}

VOID CDlgAIManager::OnDestory()
{
	//删除子项
	m_ParemeterList.DeleteAllItems();
}

//配置函数
BOOL CDlgAIManager::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("AI管理 -- （运行值）"));

	//初始化控件
	InitControls();

	//设置接口
	m_MissionAndroid.SetAndroidOperateCenter(this);

	return FALSE;
}

//确定函数
VOID CDlgAIManager::OnOK()
{
	__super::OnOK();
}

//消息解释
BOOL CDlgAIManager::PreTranslateMessage(MSG * pMsg)
{
	//按键过虑
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//绘画消息
VOID CDlgAIManager::OnDrawClientAreaBegin(CDC * pDC, INT nWidth, INT nHeight)
{
	////选择对象
	//pDC->SelectObject(m_BoradPen);
	//pDC->SelectObject(m_FrameBrush);

	////绘画底框
	//pDC->RoundRect(20,47,510,162,5,5);
	//pDC->RoundRect(20,172,510,320,5,5);
	//pDC->RoundRect(20,330,510,475,5,5);

	return;
}

//配置管理
VOID CDlgAIManager::Initialization(ITCPSocket * pITCPSocket, IClientUserItem * pIMySelfUserItem, CCheckComboBox * pCheckComboBox, tagPCGameServer GameServer)
{
	//设置变量
	m_pITCPSocket=pITCPSocket;
	m_pIMySelfUserItem=pIMySelfUserItem;
	m_pServerList = pCheckComboBox;
	m_GameServer = GameServer;

	//查询参数
	//OnBnClickedBtQuery();

	return;
}

//控件使能
VOID CDlgAIManager::EnableControls(BOOL bEnabled)
{
	//使能设置
	//GetDlgItem(IDC_BT_EXECUTE_CMD)->EnableWindow(bEnabled);
}

//构造数据
WORD CDlgAIManager::ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize)
{
	return 0;
}

//颜色消息
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

//添加修改
VOID CDlgAIManager::AddModifyParameter(bool bModify)
{
	//变量定义
	TCHAR szMessage[128] = TEXT("");
	tagRobotParameter AndroidParameter;
	ZeroMemory(&AndroidParameter, sizeof(AndroidParameter));

	//选择判断
	if (bModify == true && m_nSelectItem == -1) return;

	//机器数目校验
	if (GetDlgItemInt(IDC_EDIT_COUNT_MEMBER0) <= 0 || GetDlgItemInt(IDC_EDIT_COUNT_MEMBER0) > 100)
	{
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("抱歉,每个房间的AI数目最多为%d个,请重新输入！"), 100);

		if (GetDlgItemInt(IDC_EDIT_COUNT_MEMBER0) <= 0)
		{
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("抱歉,每个房间的AI数不可设置空！"));
		}
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), szMessage);

		return;
	}

	//机器数目
	AndroidParameter.dwRobotCount = GetDlgItemInt(IDC_EDIT_COUNT_MEMBER0);

	//服务模式
	for (int i = 0; i < m_ComboServiceMode.GetCount(); i++)
	{
		auto pItemData = (tagItemData*)m_ComboServiceMode.GetItemDataPtr(i);
		if (pItemData != NULL && pItemData->bChecked)
		{
			AndroidParameter.dwServiceMode |= pItemData->dwDataPtr;
		}
	}

	//获取控件
	auto pCombEnterHour = (CComboBox*)GetDlgItem(IDC_COMBO_ENTER_HOUR);
	auto pCombEnterMinute = (CComboBox*)GetDlgItem(IDC_COMBO_ENTER_MINUTE);
	auto pCombEnterSecond = (CComboBox*)GetDlgItem(IDC_COMBO_ENTER_SECOND);
	auto pCombLeaveHour = (CComboBox*)GetDlgItem(IDC_COMBO_LEAVE_HOUR);
	auto pCombLeaveMinute = (CComboBox*)GetDlgItem(IDC_COMBO_LEAVE_MINUTE);
	auto pCombLeaveSecond = (CComboBox*)GetDlgItem(IDC_COMBO_LEAVE_SECOND);

	// 进入时间
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

	//离开时间
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

	//间隔
	AndroidParameter.dwEnterMinInterval = GetDlgItemInt(IDC_EDIT_ENTER_MIN_INTERVAL);
	AndroidParameter.dwEnterMaxInterval = GetDlgItemInt(IDC_EDIT_ENTER_MAX_INTERVAL);
	AndroidParameter.dwLeaveMinInterval = GetDlgItemInt(IDC_EDIT_LEAVE_MIN_INTERVAL);
	AndroidParameter.dwLeaveMaxInterval = GetDlgItemInt(IDC_EDIT_LEAVE_MAX_INTERVAL);

	//携带分数
	AndroidParameter.lTakeMinScore = GetDlgItemInt(IDC_EDIT_TAKE_MIN_SCORE)*SCORE_MULTIPLE;
	AndroidParameter.lTakeMaxScore = GetDlgItemInt(IDC_EDIT_TAKE_MAX_SCORE)*SCORE_MULTIPLE;

	//离开间隔
	AndroidParameter.dwSwitchMinInnings = GetDlgItemInt(IDC_EDIT_SWITCH_MIN_INNINGS);
	AndroidParameter.dwSwitchMaxInnings = GetDlgItemInt(IDC_EDIT_SWITCH_MAX_INNINGS);

	if (bModify == false)
	{
		//数目判断
		//if (m_ParemeterList.GetItemCount() >= 1)
		//{
		//	_sntprintf_s(szMessage, CountArray(szMessage), TEXT("抱歉,每个房间最多只能添加1批AI参数！"));

		//	CInformation Information;
		//	Information.ShowMessageBox(TEXT("提示"), szMessage);

		//	return;
		//}

		//获取数据
		WORD wServerID = INVALID_WORD;
		for (INT i = 0; i < m_pServerList->GetCount(); i++)
		{
			if (m_pServerList->GetCheck(i) == static_cast<BOOL>(true))
			{
				//提取数据
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

		//获取数据
		WORD wServerID = INVALID_WORD;
		for (INT i = 0; i < m_pServerList->GetCount(); i++)
		{
			if (m_pServerList->GetCheck(i) == static_cast<BOOL>(true))
			{
				//提取数据
				tagItemData* pItemData = (tagItemData*)m_pServerList->GetItemData(i);
				wServerID = (WORD)pItemData->dwDataPtr;
				break;
			}
		}

		AndroidParameter.dwBatchID = pItemData->dwBatchID;

		//修改参数
		m_MissionAndroid.ModifyAndroidParameter(wServerID, &AndroidParameter);
	}

	return;
}

//选中子项
VOID CDlgAIManager::SelectItem(int nIndex)
{
	//设置变量
	m_nSelectItem = nIndex;
	if (m_nSelectItem == -1) return;

	//获取数据
	tagRobotParameter* pItemData = (tagRobotParameter*)m_ParemeterList.GetItemData(m_nSelectItem);
	if (pItemData == NULL) return;

	//机器数目
	SetDlgItemInt(IDC_EDIT_COUNT_MEMBER0, pItemData->dwRobotCount);

	//服务模式
	SetDlgItemInt(IDC_EDIT_SERVICE_MODE, pItemData->dwServiceMode);

	//选择模式
	for (int i = 0; i < m_ComboServiceMode.GetCount(); i++)
	{
		auto pItem =(tagItemData *)m_ComboServiceMode.GetItemDataPtr(i);
		if (pItem != NULL)
		{
			pItem->bChecked = (pItem->dwDataPtr & pItemData->dwServiceMode)?true:false;
		}
	}

	//进入时间
	WORD wEnterHour = pItemData->dwEnterTime / 3600;
	WORD wEnterMimute = (pItemData->dwEnterTime - wEnterHour*3600)/60;
	WORD wEnterSecond = pItemData->dwEnterTime - wEnterHour * 3600 - wEnterMimute*60;

	//离开时间
	WORD wLeaveHour = pItemData->dwLeaveTime / 3600;
	WORD wLeaveMimute = (pItemData->dwLeaveTime - wLeaveHour * 3600) / 60;
	WORD wLeaveSecond = pItemData->dwLeaveTime - wLeaveHour * 3600 - wLeaveMimute * 60;

	//获取控件
	auto pCombEnterHour = (CComboBox*)GetDlgItem(IDC_COMBO_ENTER_HOUR);
	auto pCombEnterMinute = (CComboBox*)GetDlgItem(IDC_COMBO_ENTER_MINUTE);
	auto pCombEnterSecond = (CComboBox*)GetDlgItem(IDC_COMBO_ENTER_SECOND);
	auto pCombLeaveHour = (CComboBox*)GetDlgItem(IDC_COMBO_LEAVE_HOUR);
	auto pCombLeaveMinute = (CComboBox*)GetDlgItem(IDC_COMBO_LEAVE_MINUTE);
	auto pCombLeaveSecond = (CComboBox*)GetDlgItem(IDC_COMBO_LEAVE_SECOND);	

	//设置控件
	pCombEnterHour->SetCurSel(wEnterHour); pCombEnterMinute->SetCurSel(wEnterMimute); pCombEnterSecond->SetCurSel(wEnterSecond);
	pCombLeaveHour->SetCurSel(wLeaveHour); pCombLeaveMinute->SetCurSel(wLeaveMimute); pCombLeaveSecond->SetCurSel(wLeaveSecond);

	//间隔
	SetDlgItemInt(IDC_EDIT_ENTER_MIN_INTERVAL, pItemData->dwEnterMinInterval);
	SetDlgItemInt(IDC_EDIT_ENTER_MAX_INTERVAL, pItemData->dwEnterMaxInterval);
	SetDlgItemInt(IDC_EDIT_LEAVE_MIN_INTERVAL, pItemData->dwLeaveMinInterval);
	SetDlgItemInt(IDC_EDIT_LEAVE_MAX_INTERVAL, pItemData->dwLeaveMaxInterval);

	//携带分数
	SetDlgItemInt(IDC_EDIT_TAKE_MIN_SCORE, pItemData->lTakeMinScore / SCORE_MULTIPLE);
	SetDlgItemInt(IDC_EDIT_TAKE_MAX_SCORE, pItemData->lTakeMaxScore / SCORE_MULTIPLE);

	//离开间隔
	SetDlgItemInt(IDC_EDIT_SWITCH_MIN_INNINGS, pItemData->dwSwitchMinInnings);
	SetDlgItemInt(IDC_EDIT_SWITCH_MAX_INNINGS, pItemData->dwSwitchMaxInnings);
}

//设置子项
VOID CDlgAIManager::SetItemToParameterList(int nItemIndex, tagRobotParameter* pAndroidParameter)
{
	//参数校验
	ASSERT(nItemIndex != -1 && pAndroidParameter != NULL);
	if (nItemIndex == -1 || pAndroidParameter == NULL) return;

	//变量定义
	TCHAR szItemValue[32] = TEXT("");

	//设置子项
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwBatchID);
	m_ParemeterList.SetItemText(nItemIndex, 0, szItemValue);

	//设置子项
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwRobotCount);
	m_ParemeterList.SetItemText(nItemIndex, 1, szItemValue);

	//设置子项
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwServiceMode);
	m_ParemeterList.SetItemText(nItemIndex, 2, szItemValue);

	//进入时间
	WORD wHour = pAndroidParameter->dwEnterTime / 3600;
	WORD wMinute = (pAndroidParameter->dwEnterTime - wHour*3600)/60;
	WORD wSecond = pAndroidParameter->dwEnterTime - wHour*3600 - wMinute*60;

	//进入时间
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%02d:%02d:%02d"), wHour, wMinute, wSecond);
	m_ParemeterList.SetItemText(nItemIndex, 3, szItemValue);

	//离开时间
	wHour = pAndroidParameter->dwLeaveTime / 3600;
	wMinute = (pAndroidParameter->dwLeaveTime - wHour * 3600) / 60;
	wSecond = pAndroidParameter->dwLeaveTime - wHour * 3600 - wMinute * 60;

	//离开时间
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%02d:%02d:%02d"), wHour, wMinute, wSecond);
	m_ParemeterList.SetItemText(nItemIndex, 4, szItemValue);

	//机器数目
	SetDlgItemInt(IDC_EDIT_COUNT_MEMBER0, pAndroidParameter->dwRobotCount);

	//间隔
	SetDlgItemInt(IDC_EDIT_ENTER_MIN_INTERVAL, pAndroidParameter->dwEnterMinInterval);
	SetDlgItemInt(IDC_EDIT_ENTER_MAX_INTERVAL, pAndroidParameter->dwEnterMaxInterval);
	SetDlgItemInt(IDC_EDIT_LEAVE_MIN_INTERVAL, pAndroidParameter->dwLeaveMinInterval);
	SetDlgItemInt(IDC_EDIT_LEAVE_MAX_INTERVAL, pAndroidParameter->dwLeaveMaxInterval);

	//携带分数
	SetDlgItemInt(IDC_EDIT_TAKE_MIN_SCORE, pAndroidParameter->lTakeMinScore / SCORE_MULTIPLE);
	SetDlgItemInt(IDC_EDIT_TAKE_MAX_SCORE, pAndroidParameter->lTakeMaxScore / SCORE_MULTIPLE);

	//离开间隔
	SetDlgItemInt(IDC_EDIT_SWITCH_MIN_INNINGS, pAndroidParameter->dwSwitchMinInnings);
	SetDlgItemInt(IDC_EDIT_SWITCH_MAX_INNINGS, pAndroidParameter->dwSwitchMaxInnings);
}

//控件使能
VOID CDlgAIManager::OnEventEnableControls(BOOL bEnabled)
{
	//设置控件
	m_btQuery.EnableWindow(bEnabled);
	m_btAddItem.EnableWindow(bEnabled);
	m_btModifyItem.EnableWindow(bEnabled);
	m_btDeleteItem.EnableWindow(bEnabled);
}

//机器参数
VOID CDlgAIManager::OnEventAndroidParenter(WORD wSubCommdID, WORD wParameterCount, tagRobotParameter* pAndroidParameter)
{
	//获取参数
	if (wSubCommdID == SUB_GP_GET_PARAMETER)
	{
		//变量定义
		int nItemIndex = -1;
		tagRobotParameter* pTempParameter = NULL;
		tagRobotParameter* pItemData = NULL;

		//清除子项
		m_ParemeterList.DeleteAllItems();

		//变量定义
		LVITEM lVItem;
		lVItem.mask = LVIF_TEXT;
		lVItem.iSubItem = 0;
		lVItem.pszText = TEXT("");

		//循环插入
		for (WORD wIndex = 0; wIndex < wParameterCount; wIndex++)
		{
			//获取参数
			pTempParameter = &pAndroidParameter[wIndex];
			if (pTempParameter == NULL) continue;

			//插入子项.
			lVItem.iItem = wIndex;
			nItemIndex = m_ParemeterList.InsertItem(&lVItem);
			if (nItemIndex == -1) continue;

			//设置子项
			SetItemToParameterList(nItemIndex, pTempParameter);

			//申请资源
			pItemData = new tagRobotParameter;
			if (pItemData != NULL)
			{
				//拷贝数据
				CopyMemory(pItemData, pTempParameter, sizeof(tagRobotParameter));

				//设置数据
				m_ParemeterList.SetItemData(nItemIndex, (DWORD_PTR)pItemData);
			}
		}

		return;
	}

	//添加参数
	if (wSubCommdID == SUB_GP_ADD_PARAMETER)
	{
		//变量定义
		int nItemIndex = -1;

		//变量定义
		LVITEM lVItem;
		lVItem.mask = LVIF_TEXT;
		lVItem.iSubItem = 0;
		lVItem.pszText = TEXT("");

		//插入子项
		lVItem.iItem = m_ParemeterList.GetItemCount();
		nItemIndex = m_ParemeterList.InsertItem(&lVItem);

		//设置子项
		SetItemToParameterList(nItemIndex, pAndroidParameter);

		//申请资源
		tagRobotParameter* pItemData = new tagRobotParameter;
		if (pItemData != NULL)
		{
			//拷贝数据
			CopyMemory(pItemData, pAndroidParameter, sizeof(tagRobotParameter));

			//设置数据
			m_ParemeterList.SetItemData(nItemIndex, (DWORD_PTR)pItemData);
		}

		return;
	}

	//修改参数
	if (wSubCommdID == SUB_GP_MODIFY_PARAMETER)
	{
		//变量定义
		//ASSERT(m_ParemeterList.GetItemCount() == MAX_BATCH);

		//查找对象
		for (int nIndex = 0; nIndex < m_ParemeterList.GetItemCount(); nIndex++)
		{
			//获取数据
			tagRobotParameter* pItemData = (tagRobotParameter*)m_ParemeterList.GetItemData(nIndex);
			if (pItemData == NULL) continue;

			if (pItemData->dwBatchID == pAndroidParameter->dwBatchID)
			{
				//拷贝数据
				CopyMemory(pItemData, pAndroidParameter, sizeof(tagRobotParameter));
				//设置子项
				SetItemToParameterList(nIndex, pAndroidParameter);

				break;
			}
			
		}

		return;
	}

	//删除参数
	if (wSubCommdID == SUB_GP_DELETE_PARAMETER)
	{
		//变量定义
		//ASSERT(m_ParemeterList.GetItemCount() == MAX_BATCH);

		//查找对象
		for (int nIndex = 0; nIndex < m_ParemeterList.GetItemCount(); nIndex++)
		{
			tagRobotParameter* pItemData = (tagRobotParameter*)m_ParemeterList.GetItemData(nIndex);
			if (pItemData == NULL) continue;

			if (pItemData->dwBatchID == pAndroidParameter->dwBatchID)
			{
				//设置子项
				m_ParemeterList.DeleteItem(nIndex);

				break;
			}

		}

		return;
	}

	return;
}

//初始化控件
VOID CDlgAIManager::InitControls()
{
	//设置控件
	DWORD dwExstyle = m_ParemeterList.GetExtendedStyle();
	m_ParemeterList.SetExtendedStyle(dwExstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | WS_EX_STATICEDGE);

	//列表控件
	m_ParemeterList.InsertColumn(0, TEXT("批次ID"), LVCFMT_CENTER, 60);
	m_ParemeterList.InsertColumn(1, TEXT("机器数目"), LVCFMT_CENTER, 120);
	m_ParemeterList.InsertColumn(2, TEXT("服务模式"), LVCFMT_CENTER, 120);
	m_ParemeterList.InsertColumn(3, TEXT("进入时间"), LVCFMT_CENTER, 120);
	m_ParemeterList.InsertColumn(4, TEXT("离开时间"), LVCFMT_CENTER, 120);

	//服务模式
	WORD wServiceMode[] = {ROBOT_SIMULATE,ROBOT_INITIATIVE,ROBOT_PASSIVITY };
	LPCTSTR pszServiceMode[] = {TEXT("相互模拟"),TEXT("主动陪打"),TEXT("被动陪打")};

	for (int i = 0; i < CountArray(wServiceMode); i++)
	{
		tagItemData* pItemData = (tagItemData*)m_ComboServiceMode.GetItemDataPtr(m_ComboServiceMode.InsertString(i,pszServiceMode[i]));
		if (pItemData != NULL)
		{
			pItemData->bChecked = true;
			pItemData->dwDataPtr = wServiceMode[i];
		}
	}

	//组合控件
	auto pCombEnterHour =	(CComboBox*)GetDlgItem(IDC_COMBO_ENTER_HOUR);
	auto pCombEnterMinute = (CComboBox*)GetDlgItem(IDC_COMBO_ENTER_MINUTE);
	auto pCombEnterSecond = (CComboBox*)GetDlgItem(IDC_COMBO_ENTER_SECOND);
	auto pCombLeaveHour =	(CComboBox*)GetDlgItem(IDC_COMBO_LEAVE_HOUR);
	auto pCombLeaveMinute = (CComboBox*)GetDlgItem(IDC_COMBO_LEAVE_MINUTE);
	auto pCombLeaveSecond = (CComboBox*)GetDlgItem(IDC_COMBO_LEAVE_SECOND);

	//变量定义
	TCHAR szValue[8];

	//时
	for (int i = 0; i < 24; i++)
	{
		_itot_s(i, szValue, 10);
		pCombEnterHour->SetItemData(pCombEnterHour->InsertString(i, szValue), i);
		pCombLeaveHour->SetItemData(pCombLeaveHour->InsertString(i, szValue), i);
	}

	//分
	for (int i = 0; i < 60; i++)
	{
		_itot_s(i, szValue, 10);
		pCombEnterMinute->SetItemData(pCombEnterMinute->InsertString(i, szValue), i);
		pCombLeaveMinute->SetItemData(pCombLeaveMinute->InsertString(i, szValue), i);
	}

	//秒
	for (int i = 0; i < 60; i++)
	{
		_itot_s(i, szValue, 10);
		pCombEnterSecond->SetItemData(pCombEnterSecond->InsertString(i, szValue), i);
		pCombLeaveSecond->SetItemData(pCombLeaveSecond->InsertString(i, szValue), i);
	}

	//默认时间
	pCombEnterHour->SetCurSel(9);pCombEnterMinute->SetCurSel(0);pCombEnterSecond->SetCurSel(0);
	pCombLeaveHour->SetCurSel(21);pCombLeaveMinute->SetCurSel(0);pCombLeaveSecond->SetCurSel(0);	

	//机器数目
	SetDlgItemInt(IDC_EDIT_COUNT_MEMBER0, 20);

	//间隔
	SetDlgItemInt(IDC_EDIT_ENTER_MIN_INTERVAL, 3);
	SetDlgItemInt(IDC_EDIT_ENTER_MAX_INTERVAL, 5);
	SetDlgItemInt(IDC_EDIT_LEAVE_MIN_INTERVAL, 3);
	SetDlgItemInt(IDC_EDIT_LEAVE_MAX_INTERVAL, 5);

	//携带分数
	SetDlgItemInt(IDC_EDIT_TAKE_MIN_SCORE, 25000);
	SetDlgItemInt(IDC_EDIT_TAKE_MAX_SCORE, 50000);

	//离开间隔
	SetDlgItemInt(IDC_EDIT_SWITCH_MIN_INNINGS, 2);
	SetDlgItemInt(IDC_EDIT_SWITCH_MAX_INNINGS, 5);
	
}

//单击子项
VOID CDlgAIManager::OnHdnListParameterItemclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	//选中子项
	SelectItem(phdr->iItem);

	*pResult = 0;
}

//删除子项
VOID CDlgAIManager::OnLvnListParameterDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	//获取数据
	tagRobotParameter* pItemData = (tagRobotParameter*)m_ParemeterList.GetItemData(pNMLV->iItem);
	if (pItemData != NULL)
	{
		SafeDelete(pItemData);
	}

	*pResult = 0;
}

//键盘按下
VOID CDlgAIManager::OnLvnKeydownListParameter(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	//变量定义
	int nItem = -1;
	POSITION pos;

	switch (pLVKeyDow->wVKey)
	{
	case VK_DOWN:
	{
		pos = m_ParemeterList.GetFirstSelectedItemPosition();
		while (pos)
		{
			//获取子项
			nItem = m_ParemeterList.GetNextSelectedItem(pos);
			if (nItem == -1) break;

			//设置子项
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
			//获取子项
			nItem = m_ParemeterList.GetNextSelectedItem(pos);
			if (nItem == -1) break;

			//设置子项
			nItem = __max(nItem - 1, 0);
			SelectItem(nItem);
			break;
		}
		break;
	}
	}

	*pResult = 0;
}

//添加子项
VOID CDlgAIManager::OnBnClickedBtAdd()
{
	//添加子项
	AddModifyParameter(false);
}

//编辑子项
VOID CDlgAIManager::OnBnClickedBtModify()
{
	//添加子项
	AddModifyParameter(true);
}

//删除子项
VOID CDlgAIManager::OnBnClickedBtDelete()
{
	//获取索引
	if (m_nSelectItem == -1) return;
	if (m_ParemeterList.GetItemCount() <= m_nSelectItem)
	{
		m_nSelectItem = -1;
		return;
	}

	//获取数据
	tagRobotParameter* pItemData = (tagRobotParameter*)m_ParemeterList.GetItemData(m_nSelectItem);
	if (pItemData == NULL) return;

	//获取时间
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	DWORD dwTodayTickCount = SystemTime.wHour * 3600 + SystemTime.wMinute * 60 + SystemTime.wSecond;

	//删除提示
	{
		//选择提示
		CInformation Information;
		INT nRetCode = Information.ShowMessageBox(TEXT("警告"), TEXT("该批AI正处在服务状态,您确定要删除吗？"), MB_YESNO);

		//暂时不退出
		if (nRetCode == IDNO) return;
	}

	//获取数据
	WORD wServerID = INVALID_WORD;
	for (INT i = 0; i < m_pServerList->GetCount(); i++)
	{
		if (m_pServerList->GetCheck(i) == static_cast<BOOL>(true))
		{
			//提取数据
			tagItemData* pItemData = (tagItemData*)m_pServerList->GetItemData(i);
			wServerID = (WORD)pItemData->dwDataPtr;
			break;
		}
	}
	//删除参数
	m_MissionAndroid.DeleteAndroidParameter(wServerID, pItemData->dwBatchID);

	//设置变量
	m_nSelectItem = -1;

	return;
}

//查询参数
VOID CDlgAIManager::OnBnClickedBtQuery()
{
	//获取数据
	WORD wServerID = INVALID_WORD;
	for (INT i = 0; i < m_pServerList->GetCount(); i++)
	{
		if (m_pServerList->GetCheck(i) == static_cast<BOOL>(true))
		{
			//提取数据
			tagItemData* pItemData = (tagItemData*)m_pServerList->GetItemData(i);
			wServerID = (WORD)pItemData->dwDataPtr;
			break;
		}
	}

	//获取参数
	m_MissionAndroid.GetAndroidParameter(wServerID);

	return;
}

//////////////////////////////////////////////////////////////////////////////////
