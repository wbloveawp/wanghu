#include "StdAfx.h"
#include "Resource.h"
#include "GlobalUnits.h"
#include "DlgSessionServer.h"
#include	"DataDefine.h"
//////////////////////////////////////////////////////////////////////////////////

//宏定义
#define COLOR_FRAME					RGB(239,249,255)					//填充颜色
#define COLOR_BORAD					RGB(170,170,170)					//边框颜色

//游戏ID
#define KIND_ID_XMOX				112									//虾米牛牛
#define KIND_ID_TEEMPATTI			246									//TeenPatti
#define KIND_ID_DEALRUMMY			248									//DealRummy
#define KIND_ID_WINGO				250									//WinGo
#define KIND_ID_SAWABALOOT			247									//SawaBaloot
#define KIND_ID_SUPERTOUZI			800									//超级骰宝
#define KIND_ID_DRAGONTIGER			145									//龙虎斗
#define KIND_ID_ANIMALBATTLE		123									//飞禽走兽
#define KIND_ID_BALOOTCARD			251									//Baloot牌型小游戏

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgSessionServer, CDlgSessionItem)
	//ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BT_ENTER_ROOM, OnBnClickedBtEnterRoom)
	ON_BN_CLICKED(IDC_BT_EXIT_ROOM, OnBnClickedBtExitRoom)
	ON_BN_CLICKED(IDC_BT_GAME_DEBUG, OnBnClickedBtGameDebug)
	ON_BN_CLICKED(IDC_BT_ROBOT_DEBUG, OnBnClickedBtRobotDebug)
	ON_BN_CLICKED(IDC_BT_CARD_DISPATCHER, OnBnClickedCardDispatcher)
	ON_NOTIFY(NM_RCLICK, IDC_USER_LIST_CONTROL, OnNMRclickUserList)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgSessionServer::CDlgSessionServer() : CDlgSessionItem(IDD_SERVER_VIEW)
{
	m_pIRobotDebug = NULL;
	m_pIClientDebug = NULL;
	m_pICardDispatcher = NULL;
	m_hRobotDebugInst = NULL;
	m_hClientDebugInst = NULL;
	m_hCardDispatcherInst = NULL;
	m_wCurrentPort = PORT_LOGON;
	m_dwCurrentServer = INADDR_NONE;
}

//析构函数
CDlgSessionServer::~CDlgSessionServer()
{
	ReleaseInterface(m_hRobotDebugInst, m_pIRobotDebug);
	ReleaseInterface(m_hClientDebugInst, m_pIClientDebug);
	ReleaseInterface(m_hCardDispatcherInst, m_pICardDispatcher);
}

//接口查询
VOID* CDlgSessionServer::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IRoomConfig, Guid, dwQueryVer);
	QUERYINTERFACE(IUserManagerSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IRoomConfig, Guid, dwQueryVer);
	return NULL;
}


//控件绑定
VOID CDlgSessionServer::DoDataExchange(CDataExchange * pDX)
{
	//绑定控件
	DDX_Control(pDX, IDC_COMBO_GAME_LIST, m_cbGameList);
	DDX_Control(pDX, IDC_COMBO_SERVER_LIST, m_cbServerList);
	DDX_Control(pDX, IDC_USER_LIST_CONTROL, m_UserListControl);

	__super::DoDataExchange(pDX);
}

//配置函数
BOOL CDlgSessionServer::OnInitDialog()
{
	__super::OnInitDialog();

	//设置接口
	//m_MissionMaster.SetManagerOperateCenter(this);

	//设置回调
	//CServerListManager::GetInstance()->SetServerListViewSink(this);

	//创建组件
	if (m_PlazaUserManagerModule.CreateInstance() == false) throw TEXT("用户管理组件创建失败！");
	//配置组件
	//CUserInformation* pUserInformation = CUserInformation::GetInstance();
	IUnknownEx* pIUnknownEx = QUERY_ME_INTERFACE(IRoomConfig);
	if (m_PlazaUserManagerModule->SetUserManagerSink(pIUnknownEx) == false) throw TEXT("用户管理组件配置失败！");
	//if (m_PlazaUserManagerModule->SetUserInformation(pUserInformation) == false) throw TEXT("用户管理组件配置失败！");

	//登录系统
	//m_MissionLogon->ShowLogon();

	//设置按钮
	EnableButton(true);

	return TRUE;
}

//列表完成
VOID CDlgSessionServer::OnEventListFinish()
{
	//变量定义
	POSITION Position = NULL;
	CGameKindItem* pGameKindItem = NULL;

	//清空数据
	while (m_cbGameList.GetCount()) m_cbGameList.DeleteString(0);

	do
	{
		pGameKindItem = m_pServerListManager->EmunGameKindItem(Position);
		if (pGameKindItem == NULL) break;

		//插入新项
		tagItemData* pItemData = (tagItemData*)m_cbGameList.GetItemDataPtr(m_cbGameList.AddString(pGameKindItem->m_GameKind.szKindName));
		if (pItemData != NULL) pItemData->dwDataPtr = pGameKindItem->m_GameKind.wKindID;

	} while (Position != NULL);
}

//命令消息
BOOL CDlgSessionServer::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD wLCommandID = LOWORD(wParam);
	WORD wHCommandID = HIWORD(wParam);

	switch (wHCommandID)
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

	switch (wLCommandID)
	{	
	case IDM_COPY_USER_NAME:	//拷贝昵称
	{
		//效验状态
		//ASSERT(m_MenuUserItemArray.GetCount() > 0);
		//if (m_MenuUserItemArray.GetCount() == 0) return TRUE;

		//拷贝字符
		IClientUserItem* pIClientUserItem = NULL;
		//选择子项
		POSITION nSelectPos = m_UserListControl.GetFirstSelectedItemPosition();
		INT nSelectItem = m_UserListControl.GetNextSelectedItem(nSelectPos);
		DWORD_PTR lItemData = m_UserListControl.GetItemData(nSelectItem);
		if (lItemData != NULL) pIClientUserItem = ((IClientUserItem*)(lItemData));

		TCHAR strGameID[20];
		if (pIClientUserItem != NULL)
			_sntprintf_s(strGameID, CountArray(strGameID), TEXT("%d"), pIClientUserItem->GetGameID());
		CWHService::CopyToClipBoard(strGameID);

		//设置字符
		/*CControlUI * pEditChat = GetControlByName(szEditChatControlName);
		if (pEditChat)
		{
		pEditChat->SetText(pIClientUserItem->GetNickName());
		pEditChat->SetFocus();
		}*/

		return TRUE;
	}
	case IDM_KILL_USER:             //踢用户下线
	{
		//效验状态
		//ASSERT(m_MenuUserItemArray.GetCount() > 0);
		//if (m_MenuUserItemArray.GetCount() == 0) return TRUE;

		//获取用户
		IClientUserItem* pIClientUserItem = NULL;

		//选择子项
		POSITION nSelectPos = m_UserListControl.GetFirstSelectedItemPosition();
		INT nSelectItem = m_UserListControl.GetNextSelectedItem(nSelectPos);
		DWORD_PTR lItemData = m_UserListControl.GetItemData(nSelectItem);
		if (lItemData != NULL) pIClientUserItem = ((IClientUserItem*)(lItemData));
		if (pIClientUserItem == NULL) break;

		////隐藏信息
		//bool bHideUserInfo = CServerRule::IsAllowAvertDebugMode(m_dwServerRule);
		//if ((bHideUserInfo == true) && (m_pIMySelfUserItem->GetMasterOrder() == 0) &&
		//	(m_pIMySelfUserItem->GetMemberOrder() <= pIClientUserItem->GetMemberOrder())) return FALSE;

		//获取属性
		WORD wTableID = pIClientUserItem->GetTableID();
		WORD wChairID = pIClientUserItem->GetChairID();
		BYTE cbUserStatus = pIClientUserItem->GetUserStatus();
		if (cbUserStatus == US_PLAYING)
		{
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("该玩家已开始游戏，不能踢！"), MB_ICONINFORMATION);
			return TRUE;
		}

		//变量定义
		CMD_GR_KickUser KickUser;
		KickUser.dwTargetUserID = pIClientUserItem->GetUserID();

		//发送数据
		if (m_pIMySelfUserItem->GetMasterOrder() != 0)
		{
			m_pVirtualLinkItem->SendVirtualData(MDM_GR_MANAGE, SUB_GR_KILL_USER, &KickUser, sizeof(KickUser));
		}
		else
		{
			//变量定义
			//CPlatformFrame* pPlatrformFrame = CPlatformFrame::GetInstance();
			/*CPlazaViewServer * pPlazaViewServer = pPlatrformFrame->GetPlazaViewServer();
			ITableViewFrame * pITableViewFrame = pPlazaViewServer->GetTableViewFrame();

			//百人游戏
			if (pITableViewFrame->GetChairCount() >= INDEX_ENTER_CHAIR)
			{
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("很抱歉，百人游戏不许踢人！"), MB_ICONQUESTION);
			return TRUE;
			}*/

			//m_TCPSocketModule->SendData(MDM_GR_USER, SUB_GR_USER_KICK_USER, &KickUser, sizeof(KickUser));
		}
		return TRUE;
	}
	case IDM_DISMISS_TABLE:					//解散桌子
	{
        //获取用户
        IClientUserItem* pIClientUserItem = NULL;

        //选择子项
        POSITION nSelectPos = m_UserListControl.GetFirstSelectedItemPosition();
        INT nSelectItem = m_UserListControl.GetNextSelectedItem(nSelectPos);
        DWORD_PTR lItemData = m_UserListControl.GetItemData(nSelectItem);
        if (lItemData != NULL) pIClientUserItem = ((IClientUserItem*)(lItemData));
		if (pIClientUserItem == NULL)
		{
            CInformation Information(this);
            Information.ShowMessageBox(TEXT("玩家不存在！"), MB_ICONINFORMATION);
            return TRUE;
		}

        //获取属性
        WORD wTableID = pIClientUserItem->GetTableID();

        //变量定义
		CMD_GR_DismissGame DismissGame;
		DismissGame.wDismissTableNum = wTableID;

        //发送数据
        if (m_pIMySelfUserItem->GetMasterOrder() != 0)
        {
			m_pVirtualLinkItem->SendVirtualData(MDM_GR_MANAGE, SUB_GR_DISMISSGAME, &DismissGame, sizeof(DismissGame));
        }

        return TRUE;
	}
	case IDM_SEND_CARD:
	{
		//获取用户
		IClientUserItem * pIClientUserItem = NULL;

		//选择子项
		POSITION nSelectPos = m_UserListControl.GetFirstSelectedItemPosition();
		INT nSelectItem = m_UserListControl.GetNextSelectedItem(nSelectPos);
		DWORD_PTR lItemData = m_UserListControl.GetItemData(nSelectItem);
		if (lItemData != NULL) pIClientUserItem = ((IClientUserItem *)(lItemData));
		if (pIClientUserItem == NULL)
		{
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("玩家不存在！"), MB_ICONINFORMATION);
			return TRUE;
		}
		CGameServerItem * pGameServerItem = NULL;
		pGameServerItem = m_pServerListManager->SearchGameServer(m_GameServer.wServerID);
		if (pGameServerItem == NULL)
		{
			MessageBox(TEXT("当前选择的Server为空！"));
			return TRUE;
		}
		//获取属性
		WORD wTableID = pIClientUserItem->GetTableID();

		WORD wTypeID = pGameServerItem->m_pGameKindItem->m_GameKind.wTypeID;

		//变量定义
		BYTE cbBuffer[SOCKET_BUFFER];
		ZeroMemory(cbBuffer, sizeof(cbBuffer));

		if (wTypeID == 3)
		{
			//麻将类型
//#ifdef  DEBUG

			// 类型定义
			typedef BYTE(_cdecl * SHOWMAHJONG)(CWnd *, WORD, CString *, int, BYTE *, BYTE *);

			// 静态变量
			SHOWMAHJONG ShowMahjong = NULL;
			static HMODULE hTestDLL = NULL;

			// 状态判断
			if (hTestDLL != NULL) return TRUE;

			// 当前目录
			TCHAR szDirectory[MAX_PATH];
			GetCurrentDirectory(sizeof(szDirectory), szDirectory);

			// 设置目录
			CString strDirectory;

			//设置文件名
			TCHAR szPath[MAX_PATH] = TEXT("");
			TCHAR szOutBuf[MAX_PATH] = TEXT("");
			CString strApp = TEXT("Config");
			CString strConfigFileName;
			GetCurrentDirectory(sizeof(szPath), szPath);
			strConfigFileName.Format(TEXT("%s\\SparrowConfig.ini"), szPath);

			//最大库存
			BYTE cbMaxCardCount = GetPrivateProfileInt(strApp, TEXT("MaxRepertory"), 144, strConfigFileName);
			const WORD wGamePlayerCount = GetPrivateProfileInt(strApp, TEXT("GamePlayer"), 4, strConfigFileName);

			strDirectory.Format(TEXT("%s\\"), szDirectory);
			//strDirectory.Append(TEXT(MODULE_NAME));

			CGameServerItem * pGameServerItem = NULL;
			pGameServerItem = m_pServerListManager->SearchGameServer(m_GameServer.wServerID);
			if (pGameServerItem == NULL)
			{
				MessageBox(TEXT("当前选择的Server为空！"));
				return TRUE;
			}

			strDirectory.Append(pGameServerItem->m_pGameKindItem->m_GameKind.szKindName);
			SetCurrentDirectory(strDirectory);

			// 加载组件
			hTestDLL = LoadLibrary(TEXT("MahjongClient.dll"));

			DWORD dwError = GetLastError();

			if (hTestDLL == NULL)
			{
				return TRUE;
			}

			// 获取地址
			ShowMahjong = (SHOWMAHJONG)GetProcAddress(hTestDLL, "ShowMahjongDlg");
			if (ShowMahjong == NULL)
			{
				FreeLibrary(hTestDLL);
				return TRUE;
			}

			WORD wBankerUser = pIClientUserItem->GetChairID();//(WORD)wParam;
			//for (int i = 0; i < GAME_PLAYER; i++)
			//{
			//	if (wBankerUser == SwitchViewChairID(i))
			//	{
			//		wBankerUser = i;
			//		break;
			//	}
			//}
			CString strName[4];
			WORD wIndex = 0;
			IClientUserItem * pCurrentUserItem = m_PlazaUserManagerModule->EnumUserItem(wIndex);
			while (pCurrentUserItem != NULL)
			{

				if (pCurrentUserItem->GetTableID() == wTableID)
				{
					// 变量定义
					for (int i = 0; i < 4; i++)
					{

						if (pCurrentUserItem->GetChairID()==i)
						{
							strName[i] = pCurrentUserItem->GetAccounts();
							break;
						}
					}
				}
				pCurrentUserItem = m_PlazaUserManagerModule->EnumUserItem(++wIndex);
			}

			// 显示配牌
			BYTE cbMagicData[6] = { 0 };
			BYTE cbCardData[MAX_REPERTORY_CNT] = { 0 };
			BOOL cbControlGameCount = ShowMahjong(this, wBankerUser, strName, wGamePlayerCount, cbCardData, cbMagicData);

			//// 释放模块
			FreeLibrary(hTestDLL);
			hTestDLL = NULL;

			// 还原目录
			SetCurrentDirectory(szDirectory);

			// 发送数据
			if (cbControlGameCount > 0)
			{
				// 构造结构
				CMD_GR_SendCard SendCard;
				SendCard.wBankerUser = wBankerUser;
				SendCard.cbCardCount = cbMaxCardCount;
				SendCard.cbControlGameCount = cbControlGameCount;
				CopyMemory(SendCard.cbCardData, cbCardData, sizeof(BYTE) * SendCard.cbCardCount);

				// 财神数据
				CopyMemory(SendCard.cbMagicIndex, cbMagicData, sizeof(SendCard.cbMagicIndex));
				SendCard.cbMagicOpenPos = cbMagicData[4];

				//变量定义
				BYTE cbBuffer[SOCKET_BUFFER];
				ZeroMemory(cbBuffer, sizeof(cbBuffer));

				//变量定义
				CMD_GR_CardDispatcherDebug * pRoomDebug = (CMD_GR_CardDispatcherDebug *)cbBuffer;
				pRoomDebug->wTableID = wTableID;
				//调试信息
				if (sizeof(SendCard) != 0)
				{
					CopyMemory(cbBuffer + sizeof(CMD_GR_CardDispatcherDebug), &SendCard, sizeof(SendCard));
				}

				//发送数据
				WORD wSendSize = sizeof(CMD_GR_CardDispatcherDebug) + sizeof(SendCard);
				m_pVirtualLinkItem->SendVirtualData(MDM_GR_MANAGE, SUB_GR_CARDDISPATCHERDEBUG, cbBuffer, wSendSize);

			}
		}
		else if (wTypeID == 2)
		{
			//扑克类型
				//定义变量
				CString filePath;
				TCHAR szFilter[] = { TEXT("配牌文件(*.acd)|*.acd||") };
				CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL);

				//选择文件
				if (dlg.DoModal() == IDOK)
				{
					//获取文件名
					filePath = dlg.GetPathName();

					//打开文件
					CFile file;
					if (file.Open(filePath, CFile::modeRead) == TRUE)
					{
						//文件头
						tagFileHead FileHead;
						file.Read(&FileHead, sizeof(FileHead));

						//文件格式判断
						if (FileHead.cbFlags1 != 'W'
							|| FileHead.cbFlags2 != 'H'
							|| FileHead.cbFlags3 != 'A'
							|| FileHead.cbFlags4 != 'C')
						{
							//关闭句柄
							file.Close();

							//错误提示
							AfxMessageBox(TEXT("配牌数据文件内容格式不正确，请选择正确的配牌文件！"));

							return 0;
						}

						//版本判断
						if (FileHead.dwVesion != VESION)
						{
							//关闭句柄
							file.Close();

							//错误提示
							TCHAR szMsg[128];
							_sntprintf(szMsg, 128, TEXT("配牌数据文件版本不正确，请选择正确版本的配牌文件，工具当前版本：%ld ！"), VESION);
							AfxMessageBox(szMsg);
							return 0;
						}

						//配牌信息
						tagAllotCardInfo AllotCardInfo;
						file.Read(&AllotCardInfo, sizeof(AllotCardInfo));

						//变量定义
						tagConfigCard AllotCard;
						ZeroMemory(&AllotCard, sizeof(AllotCard));

						//配牌数据
						BYTE cbCount = 0;
						for (WORD i = 0; i < AllotCardInfo.dwChairCount; i++)
						{
							//读取数据
							tagChairCardData CardData;
							file.Read(&CardData, sizeof(CardData));

							//判断数据
							if (CardData.wCardCount >= MAX_GAME_HAND_CARD)
							{
								//关闭句柄
								file.Close();

								//错误提示
								AfxMessageBox(TEXT("2配牌数据不正确，请选择正确的配牌文件！"));

								return 0;
							}

							//设置数据
							CopyMemory(&AllotCard.cbCardData[cbCount], CardData.cbCardData, sizeof(BYTE)* CardData.wCardCount);
							cbCount += CardData.wCardCount;
						}

						if (AllotCardInfo.cbDipaiCard == 1)
						{
							//读取数据
							tagChairCardData CardData;
							file.Read(&CardData, sizeof(CardData));
							
							//设置数据
							CopyMemory(&AllotCard.cbCardData[cbCount], CardData.cbCardData, sizeof(BYTE) * CardData.wCardCount);
							cbCount += CardData.wCardCount;
						}

						AllotCard.bAllotFlag = TRUE;

						//变量定义
						CMD_GR_CardDispatcherDebug * pRoomDebug = (CMD_GR_CardDispatcherDebug *)cbBuffer;
						pRoomDebug->wTableID = wTableID;
						//调试信息
						if (sizeof(AllotCard) != 0)
						{
							CopyMemory(cbBuffer + sizeof(CMD_GR_CardDispatcherDebug), &AllotCard, sizeof(AllotCard));
						}

						//发送数据
						WORD wSendSize = sizeof(CMD_GR_CardDispatcherDebug) + sizeof(AllotCard);
						m_pVirtualLinkItem->SendVirtualData(MDM_GR_MANAGE, SUB_GR_CARDDISPATCHERDEBUG, cbBuffer, wSendSize);

						//关闭句柄
						file.Close();
					}

				}
				else
				{
					//变量定义
					tagConfigCard AllotCard;
					ZeroMemory(&AllotCard, sizeof(AllotCard));

					//发送消息
					AllotCard.bAllotFlag = FALSE;

					//变量定义
					CMD_GR_CardDispatcherDebug * pRoomDebug = (CMD_GR_CardDispatcherDebug *)cbBuffer;
					pRoomDebug->wTableID = wTableID;
					//调试信息
					if (sizeof(AllotCard) != 0)
					{
						CopyMemory(cbBuffer + sizeof(CMD_GR_CardDispatcherDebug), &AllotCard, sizeof(AllotCard));
					}

					//发送数据
					WORD wSendSize = sizeof(CMD_GR_CardDispatcherDebug) + sizeof(AllotCard);
					m_pVirtualLinkItem->SendVirtualData(MDM_GR_MANAGE, SUB_GR_CARDDISPATCHERDEBUG, cbBuffer, wSendSize);
				}
		}
		return TRUE;
//#endif
	}
	default:
		break;
	}

	return __super::OnCommand(wParam, lParam);
}

//////////////////////////////////////////////////////////////////////
//按钮事件
//进入房间
VOID CDlgSessionServer::OnBnClickedBtEnterRoom()
{
    //获取连接信息
    GetConnectInfo();

	//校验房间
	if (m_cbServerList.GetCurSel() == -1)
	{
		MessageBox(TEXT("没有选择游戏服务器！"));
		return;
	}

	//获取标识
	WORD wServerID = m_cbServerList.GetItemData(m_cbServerList.GetCurSel());
	
	//变量定义
	POSITION Position = NULL;
	CGameServerItem* pGameServerItem = NULL;

	pGameServerItem = m_pServerListManager->SearchGameServer(wServerID);
	if (pGameServerItem == NULL)
	{
		MessageBox(TEXT("当前选择的Server为空！"));
		return;
	}

	m_GameServer = pGameServerItem->m_GameServer;

	//变量定义
	CGlobalUserInfo* pGlobalUserInfo = CGlobalUserInfo::GetInstance();
	tagGlobalUserData* pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();

	//激活任务
	m_bActiveStatus = true;

	//创建连接
	m_pVirtualLinkItem->CreateVirtualLink(wServerID, pGlobalUserData->dwUserID, pGlobalUserData->szPassword, NULL, 0);

	//查找类型
	CGameKindItem* pGameKindItem = m_pServerListManager->SearchGameKind(pGameServerItem->m_GameServer.wKindID);

	//构造标题
	TCHAR szServerTitle[128] = TEXT("");
	if (pGameKindItem)
	{
		m_GameKind = pGameKindItem->m_GameKind;

		_sntprintf_s(szServerTitle, CountArray(szServerTitle), TEXT("游戏大厅 > %s > %s"), pGameKindItem->m_GameKind.szKindName, pGameServerItem->m_GameServer.szServerName);
	}
	else
	{
		_sntprintf_s(szServerTitle, CountArray(szServerTitle), TEXT("游戏大厅 > %s"), pGameServerItem->m_GameServer.szServerName);
	}

	//设置标题
	SetWindowText(szServerTitle);
}

//退出房间
VOID CDlgSessionServer::OnBnClickedBtExitRoom()
{
	//关闭链接
	m_pVirtualLinkItem->CloseVirtualLink();

	EnableButton(TRUE);
	ReleaseInterface(m_hCardDispatcherInst, m_pICardDispatcher);

	CleanListData();
}

//游戏调试
VOID CDlgSessionServer::OnBnClickedBtGameDebug()
{
	//权限判断
	if (CUserRight::CanCheat(m_dwUserRight) == false)
	{
		MessageBox(TEXT("您当前没有调试权限！"));
		return;
	}

	if (m_pIClientDebug == NULL)
	{
		//变量定义
		TCHAR szDebugName[LEN_KIND * 2] = TEXT("");

		//获取控制组件
		int nProcessNameLen = lstrlen(m_GameKind.szProcessName)+1;
		lstrcpyn(szDebugName, m_GameKind.szProcessName, nProcessNameLen - 4);
		_sntprintf(szDebugName, CountArray(szDebugName),TEXT("%sClientDebug.dll"), szDebugName);

		//读取配置
		TCHAR szWorkPath[MAX_PATH] = TEXT("");
		CWHService::GetWorkDirectory(szWorkPath, sizeof(szWorkPath));

		//构造路径
		TCHAR szFileName[MAX_PATH] = TEXT("");
		_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\%s"), szWorkPath, szDebugName);

		//查找文件
		DWORD dwAttrib = GetFileAttributes(szFileName);
		if (!(INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)))
		{
			CString strMessage;
			strMessage.Format(TEXT("调试组件 %s 不存在！"), szFileName);
			MessageBox(strMessage);
			return;
		}

		//加载组件
		m_hClientDebugInst = LoadLibraryEx(szFileName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

		//获取函数
		if (m_hClientDebugInst)
		{
			typedef void* (*CREATE)();
			CREATE pFunction = (CREATE)GetProcAddress(m_hClientDebugInst, "CreateClientDebug");
			if (pFunction)
			{
				m_pIClientDebug = static_cast<IClientDebug*>(pFunction());
				m_pIClientDebug->Create(this, this);
			}
		}
		else
		{
			CString strMessage;
			strMessage.Format(TEXT("调试组件打开失败！错误代码 %d ！"), GetLastError());
			MessageBox(strMessage);
			return;
		}
	}

	// 回调
	if (m_pIClientDebug)
	{
		m_pIClientDebug->ShowWindow(true);
	}
}

//机器调试
VOID CDlgSessionServer::OnBnClickedBtRobotDebug()
{
	//权限判断
	if (CUserRight::CanCheat(m_dwUserRight) == false)
	{
		MessageBox(TEXT("您当前没有调试权限！"));
		return;
	}

	if (m_pIRobotDebug == NULL)
	{
		//变量定义
		TCHAR szDebugName[LEN_KIND * 2] = TEXT("");

		//获取控制组件
		int nProcessNameLen = lstrlen(m_GameKind.szProcessName)+1;
		lstrcpyn(szDebugName, m_GameKind.szProcessName, nProcessNameLen - 4);
		_sntprintf(szDebugName, CountArray(szDebugName), TEXT("%sRobotDebug.dll"), szDebugName);

		//读取配置
		TCHAR szWorkPath[MAX_PATH] = TEXT("");
		CWHService::GetWorkDirectory(szWorkPath, sizeof(szWorkPath));

		//构造路径
		TCHAR szFileName[MAX_PATH] = TEXT("");
		_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\%s"), szWorkPath, szDebugName);

		//查找文件
		DWORD dwAttrib = GetFileAttributes(szFileName);
		if (!(INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)))
		{
			CString strMessage;
			strMessage.Format(TEXT("调试组件 %s 不存在！"), szFileName);
			MessageBox(strMessage);
			return;
		}

		//加载组件
		m_hRobotDebugInst = LoadLibraryEx(szFileName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

		//获取函数
		if (m_hRobotDebugInst)
		{
			typedef void* (*CREATE)();
			CREATE pFunction = (CREATE)GetProcAddress(m_hRobotDebugInst, "CreateRobotDebug");
			if (pFunction)
			{
				m_pIRobotDebug = static_cast<IClientDebug*>(pFunction());
				m_pIRobotDebug->Create(this, this);
			}
		}
		else
		{
			CString strMessage;
			strMessage.Format(TEXT("调试组件打开失败！错误代码 %d ！"), GetLastError());
			MessageBox(strMessage);
			return;
		}
	}

	// 回调
	if (m_pIRobotDebug)
	{
		m_pIRobotDebug->ShowWindow(true);
	}
}

//配牌器
VOID CDlgSessionServer::OnBnClickedCardDispatcher()
{
	////权限判断
	//if (CUserRight::CanCheat(m_dwUserRight) == false)
	//{
	//	MessageBox(TEXT("您当前没有调试权限！"));
	//	return;
	//}

//#ifdef  DEBUG

//	// 类型定义
//	typedef BYTE(_cdecl * SHOWMAHJONG)(CWnd *, WORD, CString *, int, BYTE *, BYTE *);
//
//	// 静态变量
//	SHOWMAHJONG ShowMahjong = NULL;
//	static HMODULE hTestDLL = NULL;
//
//	// 状态判断
//	if (hTestDLL != NULL) return ;
//
//	// 当前目录
//	TCHAR szDirectory[MAX_PATH];
//	GetCurrentDirectory(sizeof(szDirectory), szDirectory);
//
//	// 设置目录
//	CString strDirectory;
//	// strDirectory.Format(TEXT("%s\\MODULE_NAME"),szDirectory);
//
//	// Add by humorly 2018/4/22
//	// 添加路径
////#define MODULE_NAME					"SparrowSQ"
//#define GAME_PLAYER					4
////#define MAX_REPERTORY				108
//	strDirectory.Format(TEXT("%s\\"), szDirectory);
//	//strDirectory.Append(TEXT(MODULE_NAME));
//
//	CGameServerItem * pGameServerItem = NULL;
//	pGameServerItem = m_pServerListManager->SearchGameServer(m_GameServer.wServerID);
//	if (pGameServerItem == NULL)
//	{
//		MessageBox(TEXT("当前选择的Server为空！"));
//		return;
//	}
//
//	strDirectory.Append(pGameServerItem->m_pGameKindItem->m_GameKind.szKindName);
//	SetCurrentDirectory(strDirectory);
//
//	// 加载组件
//	hTestDLL = LoadLibrary(TEXT("MahjongClient.dll"));
//
//	DWORD dwError = GetLastError();
//
//	if (hTestDLL == NULL)
//	{
//		return ;
//	}
//
//	// 获取地址
//	ShowMahjong = (SHOWMAHJONG)GetProcAddress(hTestDLL, "ShowMahjongDlg");
//	if (ShowMahjong == NULL)
//	{
//		FreeLibrary(hTestDLL);
//		return ;
//	}
//
//	WORD wBankerUser = 0;//(WORD)wParam;
//	//for (int i = 0; i < GAME_PLAYER; i++)
//	//{
//	//	if (wBankerUser == SwitchViewChairID(i))
//	//	{
//	//		wBankerUser = i;
//	//		break;
//	//	}
//	//}
//	CString strName[GAME_PLAYER];
//
//	//// 变量定义
//	//for (int i = 0; i < GAME_PLAYER; i++)
//	//{
//	//	IClientUserItem * pIClientUserItem = GetTableUserItem(i);
//	//	if (pIClientUserItem != NULL)
//	//	{
//	//		strName[i] = pIClientUserItem->GetAccounts();
//	//	}
//	//}
//
//	// 显示配牌
//	BYTE cbMagicData[3] = { 0 };
//	BYTE cbCardData[MAX_REPERTORY_CNT] = { 0 };
//	BOOL cbControlGameCount = ShowMahjong(this, wBankerUser, strName, GAME_PLAYER, cbCardData, cbMagicData);
//
//	//// 释放模块
//	FreeLibrary(hTestDLL);
//	hTestDLL = NULL;
//
//	// 还原目录
//	SetCurrentDirectory(szDirectory);
//
//	// 发送数据
//	if (cbControlGameCount > 0)
//	{
//		// 构造结构
//		CMD_GR_SendCard SendCard;
//		SendCard.wBankerUser = wBankerUser;
//
//		//设置文件名
//		TCHAR szPath[MAX_PATH] = TEXT("");
//		TCHAR szOutBuf[MAX_PATH] = TEXT("");
//		CString strApp = TEXT("Config");
//		CString strConfigFileName;
//		GetCurrentDirectory(sizeof(szPath), szPath);
//		strConfigFileName.Format(TEXT("%s\\SparrowConfig.ini"), szPath);
//
//		//最大库存
//		SendCard.cbCardCount = GetPrivateProfileInt(strApp, TEXT("MaxRepertory"), 144, strConfigFileName);
//		SendCard.cbControlGameCount = cbControlGameCount;
//		CopyMemory(SendCard.cbCardData, cbCardData, sizeof(BYTE) * SendCard.cbCardCount);
//
//		// 财神数据
//		SendCard.cbMagicOpenPos = cbMagicData[1];
//		SendCard.cbMagicIndex = cbMagicData[2];
//	}
//#endif

	return ;


	//if (m_pICardDispatcher == NULL)
	//{
	//	//变量定义
	//	TCHAR DebugName[LEN_KIND * 2] = _T("");
	//	TCHAR DebugFolder[LEN_KIND * 2] = _T("");
	//	//获取控制组件
	//	size_t nProcessNameLen = _tcslen(m_GameKind.szProcessName);
	//	wcsncpy_s(DebugName, m_GameKind.szProcessName, nProcessNameLen - 4);
	//	wsprintf(DebugFolder, TEXT("%s"), DebugName);
	//	wsprintf(DebugName, TEXT("%s\\CardDispatcher.dll"), DebugName);

	//	//读取配置
	//	TCHAR szPath[MAX_PATH] = TEXT("");
	//	CWHService::GetWorkDirectory(szPath, sizeof(szPath));
	//	TCHAR szFileName[MAX_PATH] = TEXT("");
	//	_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\CardDispatcher\\%s"), szPath, DebugName);

	//	DWORD dwAttrib = GetFileAttributes(szFileName);
	//	if (!(INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)))
	//	{
	//		CString str;
	//		str.Format(TEXT("配牌器组件 %s 不存在！"), szFileName);
	//		MessageBox(str);
	//		return;
	//	}

	//	m_hCardDispatcherInst = LoadLibraryEx(szFileName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

	//	if (m_hCardDispatcherInst)
	//	{
	//		typedef void* (*CREATE)();
	//		CREATE pFunction = (CREATE)GetProcAddress(m_hCardDispatcherInst, "CreateCardDispatcher");
	//		if (pFunction)
	//		{
	//			m_pICardDispatcher = static_cast<ICardDispatcher*>(pFunction());
	//			m_pICardDispatcher->Create(this, this, DebugFolder);
	//		}
	//	}
	//	else
	//	{
	//		DWORD dwLastError = GetLastError();
	//		CString str;
	//		str.Format(TEXT("调试组件打开失败！错误代码 %d ！"), dwLastError);
	//		MessageBox(str);
	//		return;
	//	}
	//}
	//// 回调
	//if (m_pICardDispatcher)
	//{
	//	m_pICardDispatcher->ShowWindow(true);
	//}
}

// 配牌器信息
bool CDlgSessionServer::OnCardDispatcherDebugInfo(WORD wTableID, void* pData, WORD nSize)
{
	//校验状态
	if (m_pVirtualLinkItem->GetLinkStatus() != LINK_STATUS_LINKED)
	{
		return false;
	}

	//变量定义
	BYTE cbBuffer[SOCKET_BUFFER];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//变量定义
	CMD_GR_CardDispatcherDebug* pRoomDebug = (CMD_GR_CardDispatcherDebug*)cbBuffer;
	pRoomDebug->wTableID = wTableID;
	//调试信息
	if (nSize != 0)
	{
		CopyMemory(cbBuffer + sizeof(CMD_GR_CardDispatcherDebug), pData, nSize);
	}

	//发送数据
	WORD wSendSize = sizeof(CMD_GR_CardDispatcherDebug) + nSize;
	m_pVirtualLinkItem->SendVirtualData(MDM_GR_MANAGE, SUB_GR_CARDDISPATCHERDEBUG, cbBuffer, wSendSize);

	return true;
}

//显示按钮
VOID CDlgSessionServer::EnableButton(bool bEnabled)
{
	GetDlgItem(IDC_BT_ENTER_ROOM)->ShowWindow(bEnabled);
	//GetDlgItem(IDC_BT_ENTER_ROOM)->EnableWindow(bEnabled);	

	GetDlgItem(IDC_BT_EXIT_ROOM)->ShowWindow(!bEnabled);
	//GetDlgItem(IDC_BT_EXIT_ROOM)->EnableWindow(!bEnabled);

	GetDlgItem(IDC_BT_GAME_DEBUG)->EnableWindow(!bEnabled);
	GetDlgItem(IDC_BT_ROBOT_DEBUG)->EnableWindow(!bEnabled);
	GetDlgItem(IDC_BT_CARD_DISPATCHER)->EnableWindow(!bEnabled);}

//发送登录
bool CDlgSessionServer::SendLogonPacket()
{
	//变量定义
	CMD_GR_LogonComputer LogonUserID;
	ZeroMemory(&LogonUserID, sizeof(LogonUserID));

	//变量定义
	CGlobalUserInfo* pGlobalUserInfo = CGlobalUserInfo::GetInstance();
	tagGlobalUserData* pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();

	//游戏版本
	//LPCTSTR pszProcessName = m_GameKind.szProcessName;
	//CWHService::GetModuleVersion(pszProcessName, LogonUserID.dwProcessVersion);

	//附加信息
	LogonUserID.dwPlazaVersion = 0; // g_GlobalUnits.GetPlazaVersion();
	LogonUserID.dwFrameVersion = 0; // g_GlobalUnits.GetFrameVersion();
	GetProcessVersion(m_GameKind.wKindID, LogonUserID.dwProcessVersion);

	//登录信息
	LogonUserID.dwUserID = pGlobalUserData->dwUserID;
	//变量定义
	TCHAR szPassword[LEN_MD5];
	CMD5Encrypt::EncryptData(TEXT("123456"), szPassword);
	//	lstrcpyn(LogonUserID.szServerPasswd, szPassword, CountArray(LogonUserID.szServerPasswd));
	lstrcpyn(LogonUserID.szPassword, pGlobalUserData->szPassword, CountArray(LogonUserID.szPassword));

	//发送数据
	m_pVirtualLinkItem->SendVirtualData(MDM_GR_LOGON, SUB_GR_LOGON_COMPUTER, &LogonUserID, sizeof(LogonUserID));

	return true;
}

//获取游戏版本
VOID CDlgSessionServer::GetProcessVersion(WORD wKindID, DWORD& dwProcessVersion)
{
	if (wKindID == KIND_ID_XMOX)
		dwProcessVersion = 134217729;
	else if (wKindID == KIND_ID_TEEMPATTI)
		dwProcessVersion = 134217729;
	else if (wKindID == KIND_ID_DEALRUMMY)
		dwProcessVersion = 134217729;
	else if (wKindID == KIND_ID_WINGO)
		dwProcessVersion = 134217729;
	else if (wKindID == KIND_ID_SAWABALOOT)
		dwProcessVersion = 101187585;
	else if (wKindID == KIND_ID_SUPERTOUZI)
		dwProcessVersion = 134217729;
    else if (wKindID == KIND_ID_BALOOTCARD)
        dwProcessVersion = 134283265;
	else if (wKindID == KIND_ID_DRAGONTIGER)
		dwProcessVersion = 101187585;
	else if (wKindID == KIND_ID_ANIMALBATTLE)
		dwProcessVersion = 101187585;	
	else
		dwProcessVersion = 101187585;
}

//设置状态
VOID CDlgSessionServer::SetServiceStatus(enServiceStatus ServiceStatus)
{
	//设置变量
	m_ServiceStatus = ServiceStatus;

	return;
}

//获取连接信息
void CDlgSessionServer::GetConnectInfo()
{
	//读取信息
	CGlobalServer GlobalServer;
	TCHAR szCurrentServer[LEN_SERVER] = TEXT("");
    GlobalServer.GetLastServerName(szCurrentServer);
	if (szCurrentServer[0] == 0)
		GlobalServer.GetServerItemName(0, szCurrentServer);

	tagServerItem ServerItem;
	ZeroMemory(&ServerItem, sizeof(ServerItem));

	//设置变量
	m_wCurrentPort = PORT_LOGON;
	m_dwCurrentServer = INADDR_NONE;

	//获取信息
	CParameterGlobal* pParameterGlobal = CParameterGlobal::GetInstance();
	bool bSuccess = GlobalServer.GetServerItemInfo(szCurrentServer, pParameterGlobal->m_NetworkKind, ServerItem);

	//转换地址
	if (bSuccess == true)
	{
		//使用域名
		if (false)
		{
			LPHOSTENT lpHost = gethostbyname(CT2CA(ServerItem.szServerHost));
			if (lpHost != NULL)
			{
				ServerItem.dwServerAddr = ((LPIN_ADDR)lpHost->h_addr)->s_addr;

				//更新地址
				if (ServerItem.szServerHost[0] != 0)
					GlobalServer.WriteServerItemInfo(szCurrentServer, pParameterGlobal->m_NetworkKind, ServerItem);
			}
		}

		//使用地址
		if (ServerItem.dwServerAddr == INADDR_NONE)
		{
			CT2CA strServerDomain(ServerItem.szServerHost);
			ServerItem.dwServerAddr = inet_addr(strServerDomain);
		}
	}

	//名字翻译
	if (ServerItem.dwServerAddr == INADDR_NONE)
	{
		//地址转换
		CT2CA CurrentServer(szCurrentServer);
		ServerItem.dwServerAddr = inet_addr(CurrentServer);

		//域名解释
		if (ServerItem.dwServerAddr == INADDR_NONE)
		{
			LPHOSTENT lpHost = gethostbyname(CurrentServer);
			if (lpHost != NULL) ServerItem.dwServerAddr = ((LPIN_ADDR)lpHost->h_addr)->s_addr;
		}
	}

	//结果处理
	if (ServerItem.dwServerAddr != INADDR_NONE)
	{
		//设置变量
		m_wCurrentPort = ServerItem.wServerPort;
		m_dwCurrentServer = ServerItem.dwServerAddr;
	}
}

//用户激活
VOID CDlgSessionServer::OnUserItemAcitve(IClientUserItem* pIClientUserItem)
{
	//判断自己
	if (m_pIMySelfUserItem == NULL)
	{
		m_pIMySelfUserItem = pIClientUserItem;
	}

	//设置桌子
	BYTE cbUserStatus = pIClientUserItem->GetUserStatus();
	if ((cbUserStatus >= US_SIT) && (cbUserStatus != US_LOOKON))
	{
		WORD wTableID = pIClientUserItem->GetTableID();
		WORD wChairID = pIClientUserItem->GetChairID();
		//m_pITableViewFrame->SetClientUserItem(wTableID, wChairID, pIClientUserItem);
	}

	//用户进入
	OnEventUserEnter(pIClientUserItem);

	return;
}

//用户进入
VOID CDlgSessionServer::OnEventUserEnter(IClientUserItem* pIClientUserItem)
{
	//设置自己
	//	if (m_pIMySelfUserItem == NULL)
	{
		//变量定义
		CGlobalUserInfo* pGlobalUserInfo = CGlobalUserInfo::GetInstance();

		//自己判断
		if (pGlobalUserInfo->GetGlobalUserData()->dwUserID == pIClientUserItem->GetUserID())
		{
			//设置变量
			m_pIMySelfUserItem = pIClientUserItem;

			//设置界面
			m_UserListControl.SetMySelfUserID(m_pIMySelfUserItem->GetUserID());
			//m_UserListControl.SetServerRule(m_dwServerRule);
		}
	}

	//插入用户
	m_UserListControl.InsertUserItem(pIClientUserItem);
	//	m_UserListControl.SortItemData();

	//更新状态
	m_UserListControl.UpdateUserItem(pIClientUserItem);

	return;
}

//用户删除
VOID CDlgSessionServer::OnUserItemDelete(IClientUserItem* pIClientUserItem)
{
	//变量定义
	WORD wLastTableID = pIClientUserItem->GetTableID();
	WORD wLastChairID = pIClientUserItem->GetChairID();
	DWORD dwLeaveUserID = pIClientUserItem->GetUserID();
	tagClientUserInfo* pMeUserInfo = m_pIMySelfUserItem->GetUserInfo();

	//更新状态
	m_UserListControl.UpdateUserItem(pIClientUserItem);

	//用户进入
	OnEventUserLeave(pIClientUserItem);

	return;
}

//用户离开
VOID CDlgSessionServer::OnEventUserLeave(IClientUserItem* pIClientUserItem)
{
	//变量定义
	DWORD dwLeaveUserID = pIClientUserItem->GetUserID();

	//删除用户
	m_UserListControl.DeleteUserItem(pIClientUserItem);

	//更新状态
	m_UserListControl.UpdateUserItem(pIClientUserItem);


	return;
}
//用户更新
VOID CDlgSessionServer::OnUserFaceUpdate(IClientUserItem* pIClientUserItem)
{
	//变量定义
	tagClientUserInfo* pUserInfo = pIClientUserItem->GetUserInfo();
	tagCustomFaceInfo* pCustomFaceInfo = pIClientUserItem->GetCustomFaceInfo();

	return;
}

//用户更新
VOID CDlgSessionServer::OnUserItemUpdate(IClientUserItem* pIClientUserItem, const tagClientUserScore& LastScore)
{
	//变量定义
	tagClientUserInfo* pUserInfo = pIClientUserItem->GetUserInfo();
	tagClientUserInfo* pMeUserInfo = m_pIMySelfUserItem->GetUserInfo();

	//更新状态
	m_UserListControl.UpdateUserItem(pIClientUserItem);

	return;
}

//用户更新
VOID CDlgSessionServer::OnUserItemUpdate(IClientUserItem* pIClientUserItem, const tagUserStatus& LastStatus)
{
	//变量定义
	tagClientUserInfo* pUserInfo = pIClientUserItem->GetUserInfo();
	tagClientUserInfo* pMeUserInfo = m_pIMySelfUserItem->GetUserInfo();
	WORD wNowTableID = pIClientUserItem->GetTableID(), wLastTableID = LastStatus.wTableID;
	WORD wNowChairID = pIClientUserItem->GetChairID(), wLastChairID = LastStatus.wChairID;
	BYTE cbNowStatus = pIClientUserItem->GetUserStatus(), cbLastStatus = LastStatus.cbUserStatus;

	//用户更新
	//m_pPlazaViewServerCtrl->OnEventUserUpdate(pIClientUserItem);

	//更新状态
	m_UserListControl.UpdateUserItem(pIClientUserItem);

	return;
}

////用户更新
//VOID CDlgSessionServer::OnUserItemUpdate(IClientUserItem* pIClientUserItem, const tagUserSegment& LastSegment)
//{
//	//变量定义
//	WORD wMeTableID = m_pIMySelfUserItem->GetTableID();
//	WORD wUserTableID = pIClientUserItem->GetTableID();
//
//	return;
//}

// 控制信息
bool CDlgSessionServer::SendDebugData(WORD nMessageID, WORD wTableID, void* pData, WORD wDataSize)
{
	//校验连接
	if (m_pVirtualLinkItem->GetLinkStatus() != LINK_STATUS_LINKED)
	{
		return false;
	}

	//缓冲定义
	BYTE cbBuffer[SOCKET_BUFFER];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//获取用户
	if (nMessageID == SUB_C_GET_SERVER_USER)
	{
		//变量定义
		WORD wIndex = 0;
		WORD wEnumIndex = 0;
		IClientUserItem* pIClientUserItem = NULL;

		//数组定义
		tagDebugUserInfo UserInfoList[MAX_CHAIR];
		ZeroMemory(UserInfoList, sizeof(UserInfoList));

		do
		{
			pIClientUserItem = m_PlazaUserManagerModule->EnumUserItem(wEnumIndex++);
			if (pIClientUserItem != NULL)
			{
				UserInfoList[wIndex].bIsRobot = pIClientUserItem->IsRobot();
				UserInfoList[wIndex].dwUserID = pIClientUserItem->GetUserID();
				UserInfoList[wIndex].dwGameID = pIClientUserItem->GetGameID();
				UserInfoList[wIndex].lUserScore = pIClientUserItem->GetUserScore();
				UserInfoList[wIndex].lInsureScore = pIClientUserItem->GetInsureGold();
				//UserInfo.lHistoryKindIDWin[wIndex] = pIClientUserItem->GetUserInfo()->lHistoryKindIDWin;
				UserInfoList[wIndex].wTableID = pIClientUserItem->GetTableID();
				UserInfoList[wIndex].wChairID = pIClientUserItem->GetChairID();
				UserInfoList[wIndex].cbUserStatus = pIClientUserItem->GetUserStatus();
				lstrcpyn(UserInfoList[wIndex].szNickName, pIClientUserItem->GetAccounts(), LEN_NICKNAME);

				wIndex++;
			}
		} while (pIClientUserItem != NULL);

		if (wIndex > 0)
		{
			m_pIClientDebug->OnEventDebugMessage(SUB_S_SERVER_USER_LIST, wTableID,&UserInfoList, wIndex * sizeof(UserInfoList[0]));
		}

		return true;
	}

	//变量定义
	auto pDebugPacket = (CMD_GR_DebugPacket *)cbBuffer;

	//设置变量
	pDebugPacket->wTableID = wTableID;
	pDebugPacket->wGameCmdID = nMessageID + SUB_GF_GAME_FRAME_MIN;	

	//调试数据
	if (wDataSize != 0)
	{
		CopyMemory(pDebugPacket+1, pData, wDataSize);
	}

	//发送数据
	m_pVirtualLinkItem->SendVirtualData(MAKECMMD(MDM_GR_DEBUG, SERVICE_MODULE_GAME), SUB_GR_DEBUG_PACKET, cbBuffer, sizeof(CMD_GR_DebugPacket)+ wDataSize);

	return true;
}

//清理列表数据
VOID CDlgSessionServer::CleanListData()
{
	m_UserListControl.DeleteAllItems();

	//枚举用户
	WORD wEnumIndex = 0;
	IClientUserItem* pIClientUserItem = NULL;
	do
	{
		pIClientUserItem = m_PlazaUserManagerModule->EnumUserItem(wEnumIndex);
		if (pIClientUserItem != NULL)
		{
			m_PlazaUserManagerModule->DeleteUserItem(pIClientUserItem);
		}
	} while (pIClientUserItem != NULL);

}


//连接事件
bool CDlgSessionServer::OnEventMissionLink(LONG nErrorCode)
{
	//错误提示
	if (nErrorCode != 0L)
	{
		//关闭房间
		PostMessage(WM_COMMAND, IDM_DELETE_SERVER_ITEM, 0);

		//提示消息
		CInformation Information(AfxGetMainWnd());
		Information.ShowMessageBox(TEXT("很抱歉，游戏房间连接失败，请稍后再试或留意网站公告！"), MB_ICONINFORMATION, 0);
		return true;
	}

	//发送登录
	SendLogonPacket();

	//设置状态
	SetServiceStatus(ServiceStatus_Validate);

	return true;
}

//关闭事件
bool CDlgSessionServer::OnEventMissionShut(BYTE cbShutReason)
{
	//关闭处理
	if (cbShutReason != SHUT_REASON_NORMAL)
	{
		if (m_ServiceStatus == ServiceStatus_ServiceIng)
		{
			MessageBox(TEXT("由于意外的原因，房间连接失败，请稍后重新尝试！"));
		}
		else
		{
			MessageBox(TEXT("由于意外的原因，房间连接失败，请稍后重新尝试！"));
		}
	}

	//设置状态
	m_bActiveStatus = false;

	//设置状态
	m_ServiceStatus = ServiceStatus_NetworkDown;
	m_wTableID = INVALID_CHAIR;
	EnableButton(true);
	CleanListData();

	//释放句柄
	ReleaseInterface(m_hRobotDebugInst, m_pIRobotDebug);
	ReleaseInterface(m_hClientDebugInst, m_pIClientDebug);
	ReleaseInterface(m_hCardDispatcherInst, m_pICardDispatcher);

	//构造标题
	TCHAR szServerTitle[128] = TEXT("");
	_sntprintf_s(szServerTitle, CountArray(szServerTitle), TEXT("游戏大厅"));

	//设置标题
	SetWindowText(szServerTitle);

	return true;
}

//读取事件
bool CDlgSessionServer::OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	bool br = false;
	WORD wCmd = LOCMMD(Command.wMainCmdID);
	switch (wCmd)
	{
	case MDM_GR_LOGON:		//登录消息
	{
		br = OnSocketMainLogon(Command.wSubCmdID, pData, wDataSize);

		return true;
	}
	case MDM_GR_CONFIG:		//配置信息
	{
		//br = OnSocketMainConfig(Command.wSubCmdID, pData, wDataSize);

		return true;
	}
	case MDM_GR_DEBUG:		//调试命令
	{
		br = OnSocketMainDebug(Command.wSubCmdID, pData, wDataSize);

		return true;
	}
	case MDM_GR_USER:		//用户信息
	{
		br = OnSocketMainUser(Command.wSubCmdID, pData, wDataSize);

		return true;
	}
	case MDM_GR_MANAGE:		//管理消息
	{
		br = OnSocketMainManager(Command.wSubCmdID, pData, wDataSize);

		return true;
	}
	case MDM_CM_SYSTEM:		//系统消息
	{
		if (Command.wSubCmdID == SUB_CM_SYSTEM_MESSAGE)
		{
			CMD_CM_SystemMessage* pSystemMessage = (CMD_CM_SystemMessage*)pData;

			::ReleaseCapture();
			CInformation Information(AfxGetMainWnd());
			Information.ShowMessageBox(pSystemMessage->szString, MB_ICONINFORMATION, 0);
		}

		return true;
	}	
	case MDM_GF_GAME:		//游戏消息
	case MDM_GF_FRAME:		//框架消息
	{
		br = OnSocketMainGameFrame(Command.wMainCmdID, Command.wSubCmdID, pData, wDataSize);

		return true;
	}
	}

	return true;
}

//登录处理
bool CDlgSessionServer::OnSocketMainLogon(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_LOGON_SUCCESS:	//登录成功
	{
		return OnSocketSubLogonSuccess(pData, wDataSize);
	}
	case SUB_GR_LOGON_FAILURE:	//登录失败
	{
		return OnSocketSubLogonFailure(pData, wDataSize);
	}
	case SUB_GR_LOGON_FINISH:	//登录完成
	{
		return OnSocketSubLogonFinish(pData, wDataSize);
	}
	case SUB_GR_UPDATE_NOTIFY:	//更新提示
	{
		return OnSocketSubUpdateNotify(pData, wDataSize);
	}
	}

	return true;
}

//调试处理
bool CDlgSessionServer::OnSocketMainDebug(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_DEBUG_PACKET:		//调试数据包
	{
		return OnSocketSubDebugPacket(pData, wDataSize);
	}
	}

	return  true;
}

//登录完成
bool CDlgSessionServer::OnSocketSubLogonFinish(VOID* pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(CMD_GR_LogonFinish));
	//if (wDataSize != sizeof(CMD_GR_LogonFinish)) return false;

	////提取数据
	//CMD_GR_LogonFinish* pLogonFinish = (CMD_GR_LogonFinish*)pData;

	//设置状态
	SetServiceStatus(ServiceStatus_ServiceIng);

	return true;
}

//登录成功
bool CDlgSessionServer::OnSocketSubLogonSuccess(VOID* pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(CMD_GR_LogonSuccess));
	if (wDataSize != sizeof(CMD_GR_LogonSuccess)) return false;

	//消息处理
	CMD_GR_LogonSuccess* pLogonSuccess = (CMD_GR_LogonSuccess*)pData;

	//玩家属性
	m_dwUserRight = pLogonSuccess->dwUserRight;
	m_dwMasterRight = pLogonSuccess->dwMasterRight;

	//设置状态
	SetServiceStatus(ServiceStatus_RecvInfo);

	EnableButton(false);
	ReleaseInterface(m_hCardDispatcherInst, m_pICardDispatcher);

	return true;
}

//登录失败
bool CDlgSessionServer::OnSocketSubLogonFailure(VOID* pData, WORD wDataSize)
{
	//效验参数
	CMD_GR_LogonFailure* pLogonFailure = (CMD_GR_LogonFailure*)pData;
	ASSERT(wDataSize >= (sizeof(CMD_GR_LogonFailure) - sizeof(pLogonFailure->szDescribeString)));
	if (wDataSize < (sizeof(CMD_GR_LogonFailure) - sizeof(pLogonFailure->szDescribeString))) return false;

	//关闭处理
	m_pVirtualLinkItem->CloseVirtualLink();

	//设置状态
	SetServiceStatus(ServiceStatus_NetworkDown);

	EnableButton(true);

	//显示消息
	LPCTSTR pszDescribeString = pLogonFailure->szDescribeString;
	if (lstrlen(pszDescribeString) > 0)
	{
		MessageBox(pszDescribeString);

		CleanListData();
	}

	return true;
}

//升级提示
bool CDlgSessionServer::OnSocketSubUpdateNotify(VOID* pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(CMD_GR_UpdateNotify));
	if (wDataSize != sizeof(CMD_GR_UpdateNotify)) return false;

	//变量定义
	CMD_GR_UpdateNotify* pUpdateNotify = (CMD_GR_UpdateNotify*)pData;
	if (pUpdateNotify->cbMustUpdatePlaza)
	{
		//提示消息
		CInformation Information(AfxGetMainWnd());
		Information.ShowMessageBox(TEXT("大厅版本需要更新！"), MB_ICONERROR);
	}
	else if (pUpdateNotify->cbMustUpdateClient)
	{
		//提示消息
		CInformation Information(AfxGetMainWnd());
		Information.ShowMessageBox(TEXT("游戏版本需要更新！"), MB_ICONERROR);
	}

	return true;
}

//管理处理
bool CDlgSessionServer::OnSocketMainManager(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	//switch (wSubCmdID)
	//{
	//case SUB_GR_OPTION_CURRENT:	//当前配置
	//{
	//	//效验参数
	//	ASSERT(wDataSize == sizeof(CMD_GR_OptionCurrent));
	//	if (wDataSize != sizeof(CMD_GR_OptionCurrent)) return false;

	//	//消息处理
	//	if (m_pDlgServerManager != NULL)
	//	{
	//		CMD_GR_OptionCurrent* pOptionCurrent = (CMD_GR_OptionCurrent*)pData;
	//		m_pDlgServerManager->SetServerType(m_wServerType);
	//		m_pDlgServerManager->SetServerOptionInfo(pOptionCurrent->ServerOptionInfo, pOptionCurrent->dwRuleMask);
	//	}

	//	return true;
	//}
	//}


	return true;
}

//游戏处理
bool CDlgSessionServer::OnSocketMainGameFrame(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize <= SOCKET_PACKET);
	if (wDataSize > SOCKET_PACKET) return false;

	switch (wSubCmdID)
	{
	case SUB_GF_SYSTEM_MESSAGE:		//系统消息
	{
		return OnSocketSubSystemMessage(pData, wDataSize);
	}

	return true;
	}

	return true;
}

//系统消息
bool CDlgSessionServer::OnSocketSubSystemMessage(VOID* pData, WORD wDataSize)
{
	//变量定义
	CMD_CM_SystemMessage* pSystemMessage = (CMD_CM_SystemMessage*)pData;
	WORD wHeadSize = sizeof(CMD_CM_SystemMessage) - sizeof(pSystemMessage->szString);

	//效验参数
	ASSERT((wDataSize > wHeadSize) && (wDataSize == (wHeadSize + pSystemMessage->wLength * sizeof(TCHAR))));
	if ((wDataSize <= wHeadSize) || (wDataSize != (wHeadSize + pSystemMessage->wLength * sizeof(TCHAR)))) return false;

	//显示消息
	if ((pSystemMessage->wType & SMT_CHAT) /*&& (m_pIStringMessage != NULL)*/)
	{
		//m_ChatMessage.InsertSystemString(pSystemMessage->szString);
	}

	//弹出消息
	if (pSystemMessage->wType & SMT_EJECT)
	{
		::ReleaseCapture();
		CInformation Information(AfxGetMainWnd());
		Information.ShowMessageBox(pSystemMessage->szString, MB_ICONINFORMATION, 0);
	}

	return true;
}

//用户处理
bool CDlgSessionServer::OnSocketMainUser(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_USER_ENTER:			//用户进入
	{
		return OnSocketSubUserEnter(pData, wDataSize);
	}
	case SUB_GR_USER_SCORE:			//用户积分
	{
		return OnSocketSubUserScore(pData, wDataSize);
	}
	case SUB_GR_USER_STATUS:		//用户状态
	{
		return OnSocketSubUserStatus(pData, wDataSize);
	}
	}

	return true;
}

//用户进入
bool CDlgSessionServer::OnSocketSubUserEnter(VOID* pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize >= sizeof(tagPCUserInfoHead));
	if (wDataSize < sizeof(tagPCUserInfoHead)) return false;

	//变量定义
	tagClientUserInfo UserInfo;
	tagCustomFaceInfo CustomFaceInfo;
	ZeroMemory(&UserInfo, sizeof(UserInfo));
	ZeroMemory(&CustomFaceInfo, sizeof(CustomFaceInfo));

	//消息处理
	tagPCUserInfoHead* pUserInfoHead = (tagPCUserInfoHead*)pData;

	//变量定义
	CGlobalUserInfo* pGlobalUserInfo = CGlobalUserInfo::GetInstance();
	tagGlobalUserData* pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();

	//变量定义	
	bool bMySelfUserItem = pGlobalUserData->dwUserID == pUserInfoHead->dwUserID;
	bool bMasterUserOrder = ((pUserInfoHead->cbMasterOrder > 0));

	//读取信息
	if ((bMySelfUserItem == true) || (bMasterUserOrder == true))
	{
		//用户属性
		UserInfo.wFaceID = pUserInfoHead->wFaceID;
		UserInfo.dwGameID = pUserInfoHead->dwGameID;
		UserInfo.dwUserID = pUserInfoHead->dwUserID;
		UserInfo.dwCustomID = pUserInfoHead->dwCustomID;
		lstrcpyn(UserInfo.szAccounts, pGlobalUserData->szAccounts, CountArray(UserInfo.szAccounts));

		//用户状态
		UserInfo.wTableID = pUserInfoHead->wTableID;
		UserInfo.wChairID = pUserInfoHead->wChairID;
		UserInfo.cbUserStatus = pUserInfoHead->cbUserStatus;

		//用户属性
		UserInfo.cbGender = pUserInfoHead->cbGender;
		UserInfo.cbMasterOrder = pUserInfoHead->cbMasterOrder;

		//用户积分
		UserInfo.lGold = pUserInfoHead->lGold;
		UserInfo.lScore = pUserInfoHead->lScore;
		UserInfo.lInsureGold = pUserInfoHead->lInsureGold;
		UserInfo.lWinCount = pUserInfoHead->lWinCount;
		UserInfo.lLostCount = pUserInfoHead->lLostCount;
		UserInfo.lDrawCount = pUserInfoHead->lDrawCount;
		UserInfo.lFleeCount = pUserInfoHead->lFleeCount;
		UserInfo.lExperience = pUserInfoHead->lExperience;
		//UserInfo.lLoveHeart = pUserInfoHead->lLoveHeart;
		//UserInfo.lIntegralCount = pUserInfoHead->lIntegralCount;
		//UserInfo.lHistoryKindIDWin = pUserInfoHead->lHistoryKindIDWin;		
	}
	else
	{
		//用户信息
		UserInfo.dwUserID = pUserInfoHead->dwUserID;
		UserInfo.dwGameID = pUserInfoHead->dwGameID;
		UserInfo.bIsRobot = pUserInfoHead->bIsRobot;
		
		//用户属性
		UserInfo.cbGender = pUserInfoHead->cbGender;		
		UserInfo.cbMasterOrder = pUserInfoHead->cbMasterOrder;

		//用户状态
		UserInfo.wTableID = pUserInfoHead->wTableID;
		UserInfo.wChairID = pUserInfoHead->wChairID;
		UserInfo.cbUserStatus = pUserInfoHead->cbUserStatus;

		//用户属性
		UserInfo.cbGender = pUserInfoHead->cbGender;
		UserInfo.cbMasterOrder = pUserInfoHead->cbMasterOrder;

		//用户积分
		UserInfo.lGold = pUserInfoHead->lGold;
		UserInfo.lScore = pUserInfoHead->lScore;
		UserInfo.lInsureGold = pUserInfoHead->lInsureGold;
		UserInfo.lWinCount = pUserInfoHead->lWinCount;
		UserInfo.lLostCount = pUserInfoHead->lLostCount;
		UserInfo.lDrawCount = pUserInfoHead->lDrawCount;
		UserInfo.lFleeCount = pUserInfoHead->lFleeCount;
		UserInfo.lExperience = pUserInfoHead->lExperience;

        //变量定义
        VOID* pDataBuffer = NULL;
        tagDataDescribe DataDescribe;
        CRecvPacketHelper RecvPacket(pUserInfoHead + 1, wDataSize - sizeof(tagPCUserInfoHead));

        //扩展信息
        while (true)
        {
        	pDataBuffer = RecvPacket.GetData(DataDescribe);
        	if (DataDescribe.wDataDescribe == DTP_NULL) break;
        	switch (DataDescribe.wDataDescribe)
        	{

            case DTP_GR_USER_NICKNAME:		//用户昵称
            {
            	ASSERT(pDataBuffer != NULL);
            	ASSERT(DataDescribe.wDataSize <= sizeof(UserInfo.szAccounts));
            	if (DataDescribe.wDataSize <= sizeof(UserInfo.szAccounts))
            	{
            		CopyMemory(&UserInfo.szAccounts, pDataBuffer, DataDescribe.wDataSize);
            		UserInfo.szAccounts[CountArray(UserInfo.szAccounts) - 1] = 0;
            	}
            	break;
            }
            default:
            	break;
            }
        }
	}

	//激活用户
	IClientUserItem* pIClientUserItem = m_PlazaUserManagerModule->SearchUserByUserID(UserInfo.dwUserID);
	if (pIClientUserItem == NULL) pIClientUserItem = m_PlazaUserManagerModule->ActiveUserItem(UserInfo, CustomFaceInfo);

	return true;
}

//用户积分
bool CDlgSessionServer::OnSocketSubUserScore(VOID* pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(CMD_GR_UserScore));
	if (wDataSize < sizeof(CMD_GR_UserScore)) return false;

	//寻找用户
	CMD_GR_UserScore* pUserScore = (CMD_GR_UserScore*)pData;
	IClientUserItem* pIClientUserItem = m_PlazaUserManagerModule->SearchUserByUserID(pUserScore->dwUserID);

	//用户判断
	ASSERT(pIClientUserItem != NULL);
	if (pIClientUserItem == NULL) return true;

	//变量定义
	CGlobalUserInfo* pGlobalUserInfo = CGlobalUserInfo::GetInstance();
	tagGlobalUserData* pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();

	//变量定义
	bool bMySelfUserItem = pGlobalUserData->dwUserID == pUserScore->dwUserID;
	//bool bAvertDebugMode = CServerRule::IsAllowAvertDebugMode(m_dwServerRule);
	bool bMasterUserOrder = ((pIClientUserItem->GetMasterOrder() > 0) || (m_pIMySelfUserItem->GetMasterOrder() > 0));

	//更新用户
	if ((bMySelfUserItem == true) || (bMasterUserOrder == true))
	{
		m_PlazaUserManagerModule->UpdateUserItemScore(pIClientUserItem, &pUserScore->UserScore);
	}

	return true;
}

//用户状态
bool CDlgSessionServer::OnSocketSubUserStatus(VOID* pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(CMD_GR_UserStatus));
	if (wDataSize < sizeof(CMD_GR_UserStatus)) return false;

	//寻找用户
	CMD_GR_UserStatus* pUserStatus = (CMD_GR_UserStatus*)pData;

	//变量设置
	m_wTableID = pUserStatus->UserStatus.wTableID;

	IClientUserItem* pIClientUserItem = m_PlazaUserManagerModule->SearchUserByUserID(pUserStatus->dwUserID);

	//用户判断
	ASSERT(pIClientUserItem != NULL);
	if (pIClientUserItem == NULL) return true;

	//之前状态
	tagUserStatus LastUserStatus;
	ZeroMemory(&LastUserStatus, sizeof(LastUserStatus));
	LastUserStatus.wTableID = pIClientUserItem->GetTableID();
	LastUserStatus.wChairID = pIClientUserItem->GetChairID();
	LastUserStatus.cbUserStatus = pIClientUserItem->GetUserStatus();

	//设置状态
	if (pUserStatus->UserStatus.cbUserStatus == US_NULL)
	{
		//删除用户
		m_PlazaUserManagerModule->DeleteUserItem(pIClientUserItem);

		////获取对象
		//CServerListData * pServerListData = CServerListData::GetInstance();

		////人数更新
		//pServerListData->SetServerOnLineCount(m_GameServer.wServerID, m_PlazaUserManagerModule->GetActiveUserCount());
	}
	else
	{
		//更新用户
		m_PlazaUserManagerModule->UpdateUserItemStatus(pIClientUserItem, &pUserStatus->UserStatus);

		//CString strUserGameID;
		//strUserGameID.Format(TEXT("[GAMEID_%d]"), pIClientUserItem->GetGameID());
		//m_ChatMessage.InsertUserStatus(pIClientUserItem->GetNickName(), strUserGameID, pUserStatus->UserStatus, LastUserStatus);
	}

	//if (m_pIClientDebug)m_pIClientDebug->OnEventUserStatus(pIClientUserItem);

	return true;
}

//调试数据包
bool CDlgSessionServer::OnSocketSubDebugPacket(VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pDebugPacket = (CMD_GR_DebugPacket*)pData;

	//游戏控制
	if (pDebugPacket->wDebugKind == DEBUG_KIND_GAME_CONTROL)
	{
		if (m_pIClientDebug != NULL)
		{
			m_pIClientDebug->OnEventDebugMessage(pDebugPacket->wGameCmdID, pDebugPacket->wTableID, pDebugPacket+1,wDataSize-sizeof(CMD_GR_DebugPacket));
		}
	}

	//机器配置
	if (pDebugPacket->wDebugKind == DEBUG_KIND_ROBOT_CONFIG)
	{
		if (m_pIRobotDebug != NULL)
		{
			m_pIRobotDebug->OnEventDebugMessage(pDebugPacket->wGameCmdID, pDebugPacket->wTableID, pDebugPacket + 1, wDataSize - sizeof(CMD_GR_DebugPacket));
		}
	}

	return true;
}

//右键列表
VOID CDlgSessionServer::OnNMRclickUserList(NMHDR* pNMHDR, LRESULT* pResult)
{
	//变量定义
	NMITEMACTIVATE* pListNotify = (NMITEMACTIVATE*)pNMHDR;

	//弹出菜单
	if (pListNotify->iItem != -1)
	{
		//选择数目
		UINT nSelectCount = m_UserListControl.GetSelectedCount();

		//选择处理
		if (nSelectCount > 1L)
		{
			//用户数组
			IClientUserItem* pIClientUserItem[32];
			ZeroMemory(pIClientUserItem, sizeof(pIClientUserItem));

			//变量定义
			WORD wUserCount = 0;
			POSITION nSelectPos = m_UserListControl.GetFirstSelectedItemPosition();

			//选择收集
			while ((nSelectPos != NULL) && (wUserCount < CountArray(pIClientUserItem)))
			{
				//获取选择
				INT nSelectItem = m_UserListControl.GetNextSelectedItem(nSelectPos);

				//插入用户
				DWORD_PTR lItemData = m_UserListControl.GetItemData(nSelectItem);
				if (lItemData != NULL) pIClientUserItem[wUserCount++] = ((IClientUserItem*)(lItemData));
			};

			//弹出菜单
			//if (wUserCount > 0) TrackUserItemMenu(pIClientUserItem, wUserCount);
		}
		else
		{
			//弹出菜单
			DWORD_PTR lItemData = m_UserListControl.GetItemData(pListNotify->iItem);
			if (lItemData != NULL) TrackUserItemMenu((IClientUserItem*)(lItemData));
		}
	}

	return;
}

//显示菜单
VOID CDlgSessionServer::TrackUserItemMenu(IClientUserItem* pIClientUserItem)
{
	//效验参数
	ASSERT(pIClientUserItem != NULL);
	if (pIClientUserItem == NULL) return;

	//设置变量
	//m_MenuUserItemArray.RemoveAll();
	//m_MenuUserItemArray.Add(pIClientUserItem);

	//构造菜单
	CSkinMenu UserInfoMenu;
	UserInfoMenu.CreateMenu();

	//变量定义
	TCHAR szMenuString[256] = TEXT("");
	bool bMeUserItem = (pIClientUserItem == m_pIMySelfUserItem);

	//变量定义
	tagClientUserInfo * pUserInfo = pIClientUserItem->GetUserInfo();
	tagClientUserInfo* pMeUserInfo = m_pIMySelfUserItem->GetUserInfo();
	/*IClientUserItem * pIChatTargetItem = m_ChatControl.GetChatTargetUserItem();*/

	//插入分割
	UserInfoMenu.AppendSeparator();

	//常规菜单
	UserInfoMenu.AppendMenu(IDM_COPY_USER_NAME, TEXT("复制ID"));

	//会员菜单
	if (!bMeUserItem/*&& m_pIClientKernel->GetServerAttribute()->wServerType != GAME_GENRE_MATCH*/)
	{
		//插入分割
		UserInfoMenu.AppendSeparator();

		//变量定义
		BYTE  cbMeUserStatus = m_pIMySelfUserItem->GetUserStatus();
		BYTE  cbUserStatus = pIClientUserItem->GetUserStatus();
		bool bEnableKick = true;
		//	if (pMeUserInfo->cbMemberOrder <= pUserInfo->cbMemberOrder ||/* (cbMeUserStatus != US_SIT&&cbMeUserStatus != US_READY) ||*/
		//		(cbUserStatus != US_SIT&&cbUserStatus != US_READY))
		//		bEnableKick = false;

		//旁观菜单
		_sntprintf_s(szMenuString, CountArray(szMenuString), TEXT("踢 [ %s ] 离开"), pIClientUserItem->GetAccounts());
		UserInfoMenu.AppendMenu(IDM_KILL_USER, szMenuString, bEnableKick ? MF_ENABLED : MF_GRAYED);

        //插入分割
        UserInfoMenu.AppendSeparator();

		//解散桌子
		UserInfoMenu.AppendMenu(IDM_DISMISS_TABLE, TEXT("解散桌子"), bEnableKick ? MF_ENABLED : MF_GRAYED);

		//插入分割
		UserInfoMenu.AppendSeparator();

		//配牌
		UserInfoMenu.AppendMenu(IDM_SEND_CARD, TEXT("配牌"), bEnableKick ? MF_ENABLED : MF_GRAYED);
	}

	//弹出菜单
	UserInfoMenu.TrackPopupMenu(this);

	return;
}

//////////////////////////////////////////////////////////////////////////////////
