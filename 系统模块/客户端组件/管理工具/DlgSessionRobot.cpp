#include "StdAfx.h"
#include "Resource.h"
#include "DlgSessionRobot.h"

//////////////////////////////////////////////////////////////////////////////////

//宏定义
#define COLOR_FRAME					RGB(239,249,255)					//填充颜色
#define COLOR_BORAD					RGB(170,170,170)					//边框颜色

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

//构造函数
CDlgSessionRobot::CDlgSessionRobot() : CDlgSessionItem(IDD_DLG_ROBOT)
{
	//设置变量
	m_pITCPSocket=NULL;
	m_pIMySelfUserItem=NULL;
	m_wServerType=GAME_GENRE_SCORE;
	m_nSelectItem = -1;

	//创建资源
	m_FrameBrush.CreateSolidBrush(COLOR_FRAME);
	m_BoradPen.CreatePen(PS_SOLID,1,COLOR_BORAD);

	return;
}

//析构函数
CDlgSessionRobot::~CDlgSessionRobot()
{
}

//控件绑定
VOID CDlgSessionRobot::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//绑定控件
	DDX_Control(pDX, IDC_BT_QUERY, m_btQuery);
	DDX_Control(pDX, IDC_BT_ADD, m_btAppendItem);
	DDX_Control(pDX, IDC_BT_MODIFY, m_btModifyItem);
	DDX_Control(pDX, IDC_BT_DELETE, m_btDeleteItem);
	DDX_Control(pDX, IDC_COMBO_SERVICE_MODE, m_ComboServiceMode);

	//绑定控件
	DDX_Control(pDX, IDC_COMBO_GAME_LIST, m_cbGameList);
	DDX_Control(pDX, IDC_COMBO_SERVER_LIST, m_cbServerList);

	//列表配置
	DDX_Control(pDX, IDC_LIST_PARAMETER, m_ParemeterList);
}


//命令消息
BOOL CDlgSessionRobot::OnCommand(WPARAM wParam, LPARAM lParam)
{
	DWORD dwCommandID = HIWORD(wParam);
	switch (dwCommandID)
	{
	case CBN_SELCHANGE:
	{
		//游戏列表
		if (m_cbGameList.m_hWnd == (HWND)lParam)
		{
			//变量定义
			CWordArray KindArray;
			for (INT i = 0; i < m_cbGameList.GetCount(); i++)
			{
				if (m_cbGameList.GetCheck(i) == static_cast<BOOL>(true))
				{
					tagItemData* pItemData = (tagItemData*)m_cbGameList.GetItemDataPtr(i);
					KindArray.Add(pItemData->dwDataPtr);
				}
			}

			//删除房间				
			while (m_cbServerList.GetCount()) m_cbServerList.DeleteString(0);

			//插入房间
			bool bInsert = false;
			POSITION Position = NULL;
			CGameServerItem* pGameServerItem = NULL;
			do
			{
				pGameServerItem = m_pServerListManager->EmunGameServerItem(Position);
				if (pGameServerItem != NULL)
				{
					//设置标识
					bInsert = false;

					//匹配类型
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
	//删除子项
	m_ParemeterList.DeleteAllItems();
}

//配置函数
BOOL CDlgSessionRobot::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("AI管理 -- （运行值）"));

	//初始化控件
	InitControls();

	return FALSE;
}

//确定函数
VOID CDlgSessionRobot::OnOK()
{
	__super::OnOK();
}

//消息解释
BOOL CDlgSessionRobot::PreTranslateMessage(MSG * pMsg)
{
	//按键过虑
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//绘画消息
VOID CDlgSessionRobot::OnDrawClientAreaBegin(CDC * pDC, INT nWidth, INT nHeight)
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
VOID CDlgSessionRobot::Initialization(ITCPSocket * pITCPSocket, IClientUserItem * pIMySelfUserItem, tagPCGameServer GameServer)
{
	//设置变量
	m_pITCPSocket=pITCPSocket;
	m_pIMySelfUserItem=pIMySelfUserItem;
	m_GameServer = GameServer;

	return;
}

//列表完成
VOID CDlgSessionRobot::OnEventListFinish()
{
	//变量定义
	POSITION Position = NULL;
	CGameKindItem* pGameKindItem = NULL;

	do
	{
		pGameKindItem = m_pServerListManager->EmunGameKindItem(Position);
		if (pGameKindItem == NULL) break;

		//插入新项
		tagItemData* pItemData = (tagItemData*)m_cbGameList.GetItemDataPtr(m_cbGameList.AddString(pGameKindItem->m_GameKind.szKindName));
		if (pItemData != NULL) pItemData->dwDataPtr = pGameKindItem->m_GameKind.wKindID;

	} while (Position != NULL);
}

//控件使能
VOID CDlgSessionRobot::EnableControls(BOOL bEnabled)
{
	//设置控件
	m_btQuery.EnableWindow(bEnabled);
	m_btAppendItem.EnableWindow(bEnabled);
	m_btModifyItem.EnableWindow(bEnabled);
	m_btDeleteItem.EnableWindow(bEnabled);
}

//添加修改
VOID CDlgSessionRobot::AddModifyParameter(bool bModify)
{
	//变量定义
	TCHAR szMessage[128] = TEXT("");
	tagRobotParameter RobotParameter;
	ZeroMemory(&RobotParameter, sizeof(RobotParameter));

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

	//校验房间
	if (m_cbServerList.GetCurSel() == -1)
	{
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("请选择游戏房间！"));

		return;
	}

	//机器数目
	RobotParameter.dwRobotCount = GetDlgItemInt(IDC_EDIT_COUNT_MEMBER0);

	//服务模式
	for (int i = 0; i < m_ComboServiceMode.GetCount(); i++)
	{
		auto pItemData = (tagItemData*)m_ComboServiceMode.GetItemDataPtr(i);
		if (pItemData != NULL && pItemData->bChecked)
		{
			RobotParameter.dwServiceMode |= pItemData->dwDataPtr;
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

	//离开时间
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

	//间隔
	RobotParameter.dwEnterMinInterval = GetDlgItemInt(IDC_EDIT_ENTER_MIN_INTERVAL);
	RobotParameter.dwEnterMaxInterval = GetDlgItemInt(IDC_EDIT_ENTER_MAX_INTERVAL);
	RobotParameter.dwLeaveMinInterval = GetDlgItemInt(IDC_EDIT_LEAVE_MIN_INTERVAL);
	RobotParameter.dwLeaveMaxInterval = GetDlgItemInt(IDC_EDIT_LEAVE_MAX_INTERVAL);

	//携带分数
	RobotParameter.lTakeMinScore = GetDlgItemInt(IDC_EDIT_TAKE_MIN_SCORE)*SCORE_MULTIPLE;
	RobotParameter.lTakeMaxScore = GetDlgItemInt(IDC_EDIT_TAKE_MAX_SCORE)*SCORE_MULTIPLE;

	//离开间隔
	RobotParameter.dwSwitchMinInnings = GetDlgItemInt(IDC_EDIT_SWITCH_MIN_INNINGS);
	RobotParameter.dwSwitchMaxInnings = GetDlgItemInt(IDC_EDIT_SWITCH_MAX_INNINGS);

	if (bModify == false)
	{
		//获取数据
		WORD wServerID = m_cbServerList.GetItemData(m_cbServerList.GetCurSel());		

		AppendRobotParameter(wServerID, &RobotParameter);
	}
	else
	{
		if (m_nSelectItem == -1) return;
		tagRobotParameter* pItemData = (tagRobotParameter*)m_ParemeterList.GetItemData(m_nSelectItem);
		if (pItemData == NULL) return;

		//获取数据
		WORD wServerID = m_cbServerList.GetItemData(m_cbServerList.GetCurSel());;
		if (m_cbServerList.GetCurSel()!=-1) wServerID = m_cbServerList.GetItemData(m_cbServerList.GetCurSel());

		//设置批次
		RobotParameter.dwBatchID = pItemData->dwBatchID;

		//修改参数
		ModifyRobotParameter(wServerID, &RobotParameter);
	}

	return;
}

//选中子项
VOID CDlgSessionRobot::SelectItem(int nIndex)
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
VOID CDlgSessionRobot::SetItemToParameterList(int nItemIndex, tagRobotParameter* pRobotParameter)
{
	//参数校验
	ASSERT(nItemIndex != -1 && pRobotParameter != NULL);
	if (nItemIndex == -1 || pRobotParameter == NULL) return;

	//变量定义
	TCHAR szItemValue[32] = TEXT("");

	//设置子项
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pRobotParameter->dwBatchID);
	m_ParemeterList.SetItemText(nItemIndex, 0, szItemValue);

	//设置子项
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pRobotParameter->dwRobotCount);
	m_ParemeterList.SetItemText(nItemIndex, 1, szItemValue);

	//设置子项
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pRobotParameter->dwServiceMode);
	m_ParemeterList.SetItemText(nItemIndex, 2, szItemValue);

	//进入时间
	WORD wHour = pRobotParameter->dwEnterTime / 3600;
	WORD wMinute = (pRobotParameter->dwEnterTime - wHour*3600)/60;
	WORD wSecond = pRobotParameter->dwEnterTime - wHour*3600 - wMinute*60;

	//进入时间
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%02d:%02d:%02d"), wHour, wMinute, wSecond);
	m_ParemeterList.SetItemText(nItemIndex, 3, szItemValue);

	//离开时间
	wHour = pRobotParameter->dwLeaveTime / 3600;
	wMinute = (pRobotParameter->dwLeaveTime - wHour * 3600) / 60;
	wSecond = pRobotParameter->dwLeaveTime - wHour * 3600 - wMinute * 60;

	//离开时间
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%02d:%02d:%02d"), wHour, wMinute, wSecond);
	m_ParemeterList.SetItemText(nItemIndex, 4, szItemValue);

	//机器数目
	SetDlgItemInt(IDC_EDIT_COUNT_MEMBER0, pRobotParameter->dwRobotCount);

	//间隔
	SetDlgItemInt(IDC_EDIT_ENTER_MIN_INTERVAL, pRobotParameter->dwEnterMinInterval);
	SetDlgItemInt(IDC_EDIT_ENTER_MAX_INTERVAL, pRobotParameter->dwEnterMaxInterval);
	SetDlgItemInt(IDC_EDIT_LEAVE_MIN_INTERVAL, pRobotParameter->dwLeaveMinInterval);
	SetDlgItemInt(IDC_EDIT_LEAVE_MAX_INTERVAL, pRobotParameter->dwLeaveMaxInterval);

	//携带分数
	SetDlgItemInt(IDC_EDIT_TAKE_MIN_SCORE, pRobotParameter->lTakeMinScore / SCORE_MULTIPLE);
	SetDlgItemInt(IDC_EDIT_TAKE_MAX_SCORE, pRobotParameter->lTakeMaxScore / SCORE_MULTIPLE);

	//离开间隔
	SetDlgItemInt(IDC_EDIT_SWITCH_MIN_INNINGS, pRobotParameter->dwSwitchMinInnings);
	SetDlgItemInt(IDC_EDIT_SWITCH_MAX_INNINGS, pRobotParameter->dwSwitchMaxInnings);
}

//读取事件
bool CDlgSessionRobot::OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	//库存服务
	if (Command.wMainCmdID == MDM_GP_ROBOT_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_ROBOT_OPERATE_RESULT:		//操作结果
		{
			//参数校验
			ASSERT(wDataSize <= sizeof(CMD_GP_RobotOperateResult));
			if (wDataSize > sizeof(CMD_GP_RobotOperateResult)) return false;

			//提取变量
			auto pRobotOperateResult = (CMD_GP_RobotOperateResult*)pData;

			//获取参数
			switch (pRobotOperateResult->wSubCommdID)
			{
			case SUB_GP_QUERY_PARAMETER:		//查询参数
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
				for (WORD wIndex = 0; wIndex < pRobotOperateResult->wParameterCount; wIndex++)
				{
					//获取参数
					pTempParameter = &pRobotOperateResult->RobotParameter[wIndex];
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

				break;
			}
			case SUB_GP_APPEND_PARAMETER:		//添加参数
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
				SetItemToParameterList(nItemIndex, &pRobotOperateResult->RobotParameter[0]);

				//申请资源
				tagRobotParameter* pItemData = new tagRobotParameter;
				if (pItemData != NULL)
				{
					//拷贝数据
					CopyMemory(pItemData, &pRobotOperateResult->RobotParameter[0], sizeof(tagRobotParameter));

					//设置数据
					m_ParemeterList.SetItemData(nItemIndex, (DWORD_PTR)pItemData);
				}

				break;
			}
			case SUB_GP_MODIFY_PARAMETER:		//修改参数
			{
				auto pRobotParameter = &pRobotOperateResult->RobotParameter[0];

				//查找对象
				for (int nIndex = 0; nIndex < m_ParemeterList.GetItemCount(); nIndex++)
				{
					//获取数据
					tagRobotParameter* pItemData = (tagRobotParameter*)m_ParemeterList.GetItemData(nIndex);
					if (pItemData == NULL) continue;

					if (pItemData->dwBatchID == pRobotParameter->dwBatchID)
					{
						//拷贝数据
						CopyMemory(pItemData, pRobotParameter, sizeof(tagRobotParameter));
						//设置子项
						SetItemToParameterList(nIndex, pRobotParameter);

						break;
					}
				}

				break;
			}
			case SUB_GP_DELETE_PARAMETER:	//删除参数
			{
				auto pRobotParameter = &pRobotOperateResult->RobotParameter[0];

				//查找对象
				for (int nIndex = 0; nIndex < m_ParemeterList.GetItemCount(); nIndex++)
				{
					tagRobotParameter* pItemData = (tagRobotParameter*)m_ParemeterList.GetItemData(nIndex);
					if (pItemData == NULL) continue;

					if (pItemData->dwBatchID == pRobotParameter->dwBatchID)
					{
						//设置子项
						m_ParemeterList.DeleteItem(nIndex);

						break;
					}
				}

				break;
			}
			}

			//设置控件
			EnableControls(TRUE);

			return true;
		}
		}
	}

	//操作失败
	if (Command.wMainCmdID == MDM_GP_USER_SERVICE && Command.wSubCmdID == SUB_GP_OPERATE_FAILURE)
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

//初始化控件
VOID CDlgSessionRobot::InitControls()
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
VOID CDlgSessionRobot::OnHdnListParameterItemclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	//选中子项
	SelectItem(phdr->iItem);

	*pResult = 0;
}

//删除子项
VOID CDlgSessionRobot::OnLvnListParameterDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
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
VOID CDlgSessionRobot::OnLvnKeydownListParameter(NMHDR *pNMHDR, LRESULT *pResult)
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
VOID CDlgSessionRobot::OnBnClickedBtAppend()
{
	//添加子项
	AddModifyParameter(false);
}

//编辑子项
VOID CDlgSessionRobot::OnBnClickedBtModify()
{
	//添加子项
	AddModifyParameter(true);
}

//删除子项
VOID CDlgSessionRobot::OnBnClickedBtDelete()
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
	WORD wServerID = m_cbServerList.GetItemData(m_cbServerList.GetCurSel());

	//删除参数
	DeleteRobotParameter(wServerID, pItemData->dwBatchID);

	//设置变量
	m_nSelectItem = -1;

	return;
}

//查询参数
VOID CDlgSessionRobot::OnBnClickedBtQuery()
{
	//校验房间
	if (m_cbServerList.GetCurSel()==-1)
	{
		//选择提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("请选择一个游戏房间！"), MB_OK);

		return;
	}

	//获取数据
	WORD wServerID = m_cbServerList.GetItemData(m_cbServerList.GetCurSel());
	
	//获取参数
	QueryRobotParameter(wServerID);

	return;
}

// 
//获取参数
bool CDlgSessionRobot::QueryRobotParameter(WORD wServerID)
{
	//获取对象
	CMD_GP_QueryParameter QueryParameter = {};

	//设置变量
	QueryParameter.wServerID = wServerID;
	
	//发送数据
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_ROBOT_SERVICE, SUB_GP_QUERY_PARAMETER, &QueryParameter, sizeof(QueryParameter));
}

//删除参数
bool CDlgSessionRobot::DeleteRobotParameter(WORD wServerID, DWORD dwBatchID)
{
	//获取对象
	CMD_GP_DeleteParameter DeleteParameter = {};

	//设置变量
	DeleteParameter.wServerID = wServerID;
	DeleteParameter.dwBatchID = dwBatchID;	

	//发送数据
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_ROBOT_SERVICE, SUB_GP_DELETE_PARAMETER, &DeleteParameter, sizeof(DeleteParameter));
}

//修改参数
bool CDlgSessionRobot::ModifyRobotParameter(WORD wServerID, tagRobotParameter* pRobotParameter)
{
	//获取对象
	CMD_GP_ModifyParameter ModifyParameter = {};

	//设置变量
	ModifyParameter.wServerID = wServerID;
	CopyMemory(&ModifyParameter.RobotParameter, pRobotParameter, sizeof(tagRobotParameter));

	//发送数据
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_ROBOT_SERVICE, SUB_GP_MODIFY_PARAMETER, &ModifyParameter, sizeof(ModifyParameter));
}

//添加参数
bool CDlgSessionRobot::AppendRobotParameter(WORD wServerID, tagRobotParameter* pRobotParameter)
{
	//获取对象
	CMD_GP_AppendParameter AppendParameter = {};

	//设置变量
	AppendParameter.wServerID = wServerID;
	CopyMemory(&AppendParameter.RobotParameter, pRobotParameter, sizeof(tagRobotParameter));

	//发送数据
	return m_pVirtualLinkItem->SendVirtualData(MDM_GP_ROBOT_SERVICE, SUB_GP_APPEND_PARAMETER, &AppendParameter, sizeof(AppendParameter));
}

//////////////////////////////////////////////////////////////////////////////////
