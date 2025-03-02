#include "StdAfx.h"
#include "Resource.h"
#include "GlobalUnits.h"
#include "SessionLogon.h"
#include "PlatformFrame.h"
#include "DispatchManager.h"
//#include "ServiceManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//静态变量

CSessionLogon * CSessionLogon::m_pMissionLogon=NULL;					//对象指针

//////////////////////////////////////////////////////////////////////////

//构造函数
CSessionLogon::CSessionLogon()
{
	//设置变量
	m_cbMissionType = MT_LOGON;
	ZeroMemory(m_chBuffer,sizeof(m_chBuffer));

	//控件指针
	m_pDlgLogon=NULL;
	m_pDlgStatus=NULL;

	//设置对象
	ASSERT(m_pMissionLogon==NULL);
	if (m_pMissionLogon==NULL) m_pMissionLogon=this;

	return;
}

//析构函数
CSessionLogon::~CSessionLogon()
{
	//删除对象
	SafeDelete(m_pDlgLogon);
	SafeDelete(m_pDlgStatus);

	//释放对象
	ASSERT(m_pMissionLogon==this);
	if (m_pMissionLogon==this) m_pMissionLogon=NULL;

	return;
}

////请求缓冲
//DWORD __cdecl CSessionLogon::OnHttpRequestBuffer(DWORD dwRequestSize, LPVOID *ppvBuffer)
//{
//	if( ppvBuffer != NULL )
//	{
//		*ppvBuffer = (LPVOID)m_chBuffer;
//		return sizeof(m_chBuffer);
//	}
//
//	return 0;
//}
//
////下载完成
//bool __cdecl CSessionLogon::OnHttpDataComplete(DWORD dwDataSize)
//{
//	//变量定义
//	BOOL bValidate = FALSE;
//	CString strTips = TEXT("");
//	CString strAdText = TEXT("");
//	CString strAdUrl = TEXT("");
//
//	ASSERT(dwDataSize>0);
//	ShowInformation(TEXT("创建组件失败，缺少系统组件msxml2.dll或更高版本！"), MB_ICONERROR, 30);
//	//if( dwDataSize > 0 )
//	//{
//	//	m_chBuffer[CountArray(m_chBuffer)-1] = 0;
//
//	//	//解析XML
//	//	try
//	//	{
//	//		if( m_pXmlDoc == NULL )
//	//		{
//	//			HRESULT hr = m_pXmlDoc.CreateInstance(MSXML2::CLSID_DOMDocument);
//	//			if( FAILED(hr) ) _com_issue_error(hr);
//
//	//			//解析时效验
//	//			m_pXmlDoc->PutvalidateOnParse(VARIANT_TRUE);
//	//		}
//
//	//		//加载数据
//	//		if( VARIANT_TRUE == m_pXmlDoc->loadXML((_bstr_t)m_chBuffer) )
//	//		{
//	//			//变量定义
//	//			MSXML2::IXMLDOMNodePtr  pNode;
//
//	//			//判断验证通过
//	//			pNode = m_pXmlDoc->selectSingleNode(TEXT("//Validate/Flag"));
//	//			if( pNode != NULL )
//	//			{
//	//				MSXML2::IXMLDOMNodePtr pChild = pNode->firstChild;
//	//				ASSERT(pChild != NULL && MSXML2::NODE_TEXT == pChild->nodeType);
//	//				if( pChild != NULL && MSXML2::NODE_TEXT == pChild->nodeType )
//	//				{
//	//					bValidate = (_tstol(pChild->text)==0?TRUE:FALSE);
//	//				}
//
//	//				pNode.Release();
//	//			}
//
//	//			//如果验证通过
//	//			if( bValidate )
//	//			{
//	//				//获取提示
//	//				pNode = m_pXmlDoc->selectSingleNode(TEXT("//Validate/Tips"));
//	//				if( pNode != NULL )
//	//				{
//	//					MSXML2::IXMLDOMNodePtr pChild = pNode->firstChild;
//	//					ASSERT(pChild != NULL && MSXML2::NODE_TEXT == pChild->nodeType);
//	//					if( pChild != NULL && MSXML2::NODE_TEXT == pChild->nodeType )
//	//					{
//	//						strTips = (LPCTSTR)pChild->text;
//	//					}
//
//	//					pNode.Release();
//	//				}
//	//				//获取链接
//	//				pNode = m_pXmlDoc->selectSingleNode(TEXT("//Validate/AdUrl"));
//	//				if( pNode != NULL )
//	//				{
//	//					MSXML2::IXMLDOMNodePtr pChild = pNode->firstChild;
//	//					if( pChild != NULL && MSXML2::NODE_TEXT == pChild->nodeType )
//	//					{
//	//						strAdUrl = (LPCTSTR)pChild->text;
//	//					}
//
//	//					pNode.Release();
//	//				}
//	//				//获取链接文字
//	//				pNode = m_pXmlDoc->selectSingleNode(TEXT("//Validate/AdText"));
//	//				if( pNode != NULL )
//	//				{
//	//					MSXML2::IXMLDOMNodePtr pChild = pNode->firstChild;
//	//					if( pChild != NULL && MSXML2::NODE_TEXT == pChild->nodeType )
//	//					{
//	//						strAdText = (LPCTSTR)pChild->text;
//	//					}
//
//	//					pNode.Release();
//	//				}
//	//			}
//	//		}
//	//	}catch( _com_error &err )
//	//	{
//	//		//设置变量
//	//		bValidate = FALSE;
//
//	//		if( CO_E_CLASSSTRING == err.Error() || CO_E_IIDSTRING == err.Error() )
//	//		{
//	//			ShowInformation(TEXT("创建组件失败，缺少系统组件msxml2.dll或更高版本！"),MB_ICONERROR,30);
//	//		}
//	//		else
//	//		{
//	//			TCHAR szMess[128] = TEXT("");
//	//			// add by humroly 2018/8/3
//	//			// replace func
//	//			_sntprintf_s(szMess, CountArray(szMess), TEXT("验证过程中出现异常：(0x%x,%s)"), err.Error(), err.ErrorMessage());
//	//			ShowInformation( szMess,MB_ICONERROR,30 );
//	//		}
//	//	}
//	//}
//
//	//关闭状态
//	CloseStatusInfo();
//
//	//显示消息
//	ShowInformation(TEXT("您的帐号不存在或者密码输入有误，请查证后再次尝试登录！"),MB_ICONERROR,30);
//
//	//重新登录
//	RestoreShow();
//
//	return true;
//}
//
////下载异常
//bool __cdecl CSessionLogon::OnHttpDataException(enDownLoadError DownLoadError)
//{
//	//进度界面
//	CloseStatusInfo();
//
//	//显示消息
//	ShowInformation(TEXT("您的帐号不存在或者密码输入有误，请查证后再次尝试登录！"),MB_ICONERROR,30);
//
//	//重新登录
//	RestoreShow();
//
//	return true;
//}
//
////连接关闭
//bool __cdecl CSessionLogon::OnHttpSocketClose(bool bCloseByServer)
//{
//	if( !bCloseByServer ) return true;
//
//	//进度界面
//	CloseStatusInfo();
//
//	//显示消息
//	ShowInformation(TEXT("您的帐号不存在或者密码输入有误，请查证后再次尝试登录！"),MB_ICONERROR,30);
//
//	//重新登录
//	RestoreShow();
//
//	return true;
//}

//连接事件
bool CSessionLogon::OnEventMissionLink(LONG nErrorCode)
{
	//错误处理
	if (nErrorCode!=0L)
	{
		//重试任务
		/*CMissionManager * pMissionManager=GetMissionManager();
		if (pMissionManager->AvtiveSessionItem(this,true)==true) return true;*/

		//终止任务
		m_bActiveStatus = false;

		//关闭提示
		CloseStatusInfo();

		//显示提示
		CInformation Information(CPlatformFrame::GetInstance());
		Information.ShowMessageBox(TEXT("尝试了所有的服务器都无法成功连接服务器，请留意网站维护公告！"),MB_ICONERROR);

		//重新登录
		RestoreShow();
	}
	else
	{
		//变量定义
		WORD wPacketSize, wSubCmdID;
		BYTE cbBuffer[SOCKET_PACKET];

		//构造数据
		if (m_cbMissionType == MT_LOGON)
		{
			ASSERT(m_pDlgLogon != NULL);
			wPacketSize = m_pDlgLogon->ConstructLogonPacket(cbBuffer, sizeof(cbBuffer));
			wSubCmdID = SUB_GP_LOGON_MANAGETOOL;
		}

		//叠加数据
		CSendPacketHelper Packet(cbBuffer + wPacketSize, sizeof(cbBuffer) - wPacketSize);

		//变量定义
		WCHAR szMachine[LEN_MACHINE_ID] = { 0 };
		WCHAR szNewMachine[LEN_MACHINE_ID] = { 0 };

		//获取机器
		//CWHService::GetMachineID(szMachine);
		CWHService::GetMachineID(szNewMachine);

		//追加数据
		Packet.AddPacket(szMachine, sizeof(WCHAR) * LEN_MACHINE_ID, DTP_GP_MACHINE_ID);
		Packet.AddPacket(szNewMachine, sizeof(WCHAR) * LEN_MACHINE_ID, DTP_GP_NEW_COMPUTER_ID);

		//发送数据
		auto pVirtualLinkItem = GetVirtualLinkItem();
		pVirtualLinkItem->SendVirtualData(MDM_GP_LOGON, wSubCmdID, cbBuffer, wPacketSize + Packet.GetDataSize());

		//提示信息
		ShowStatusInfo(TEXT("正在验证用户登录信息..."));
	}

	return true;
}

//关闭事件
bool CSessionLogon::OnEventMissionShut(BYTE cbShutReason)
{
	//终止任务
	m_bActiveStatus = false;

	//关闭处理
	if (cbShutReason!=SHUT_REASON_NORMAL)
	{
		//关闭窗口
		CloseStatusInfo();
	}
	
	//发送事件
	CDispatchManager* pDispatchManager = CDispatchManager::GetInstance();
	if (pDispatchManager != NULL) pDispatchManager->PostEventInform(EVENT_LOGOUT_NOTIFY, 1L);	

	return true;
}

//读取事件
bool CSessionLogon::OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize)
{
	WORD wCmd = LOCMMD(Command.wMainCmdID);
	switch(wCmd)
	{
	case MDM_GP_LOGON:			//登录消息
		{
			return OnSocketMainLogon(Command.wSubCmdID,pData,wDataSize);
		}
	case MDM_CM_SYSTEM:			//系统消息
		{
			return OnSocketMainSystem(Command.wSubCmdID,pData,wDataSize);
		}
	case MDM_GP_USER_SERVICE:	//玩家服务
		{
			return OnSocketMainService(Command.wSubCmdID,pData,wDataSize);
		}
	case MDM_GP_SERVER_LIST:	//游戏房间
	{
		return OnSocketMainServerList(Command.wSubCmdID, pData, wDataSize);
	}	
	}

	return false;
}

//显示登录
VOID CSessionLogon::ShowLogon()
{
	//创建窗口
	if (m_pDlgLogon==NULL)
	{
		m_pDlgLogon=new CDlgLogon;
	}

	//创建窗口
	if (m_pDlgLogon->m_hWnd==NULL)
	{
		m_pDlgLogon->Create(IDD_DLG_LOGON,CPlatformFrame::GetInstance());
	}

	//设置变量
	m_cbMissionType = MT_LOGON;

	//显示窗口
	m_pDlgLogon->ShowWindow(SW_SHOW);
	m_pDlgLogon->SetForegroundWindow();

	return;
}

//执行登录
bool CSessionLogon::PerformLogonMission(BYTE cbMissonType)
{
	//设置状态
	m_bActiveStatus = false;
	m_cbMissionType = cbMissonType;

	//获取地址
	TCHAR szCustomServer[LEN_SERVER]=TEXT("");
	if (m_pDlgLogon!=NULL) lstrcpyn(szCustomServer,m_pDlgLogon->m_szLogonServer,CountArray(szCustomServer));

	//设置地址
	auto pVirtualLinkItem = GetVirtualLinkItem();
	if (pVirtualLinkItem != NULL)
	{
		pVirtualLinkItem->GetVirtualLinkManager()->SetCurrentServer(szCustomServer);		
	}

	//发起连接
	if (pVirtualLinkItem->CreateVirtualLink(INVALID_WORD,0,TEXT(""),NULL,0) == false)
	{
		//错误提示
		CInformation Information(CPlatformFrame::GetInstance());
		Information.ShowMessageBox(TEXT("服务器连接失败，请检查网络配置以及网络连接状况！"),MB_ICONERROR);

		//重新登录
		RestoreShow();

		return false;
	}

	//设置状态
	m_bActiveStatus = true;

	//写入地址
	CGlobalServer GlobalServer;
	GlobalServer.SetLastServerName(szCustomServer);

	//显示进度
	ShowStatusInfo(TEXT("正在连接服务器，请耐心稍候片刻..."));

	return true;
}

//记录用户
VOID CSessionLogon::UpdateUserInfo()
{
	//变量定义
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//构造信息
	TCHAR szUserID[16];
	// add by humroly 2018/8/3
	// replace func
	_sntprintf_s(szUserID, CountArray(szUserID), TEXT("%ld"), pGlobalUserData->dwUserID);

	//上次用户
	CRegKeyAide RegUserInfo;
	RegUserInfo.OpenRegKey(REG_USER_INFO,true);
	RegUserInfo.WriteValue(TEXT("LastUserID"),pGlobalUserData->dwUserID);
	if(m_pDlgLogon!=NULL)
		RegUserInfo.WriteValue(TEXT("LogonMode"),m_pDlgLogon->GetLogonMode());

	//登录信息
	if (pParameterGlobal->m_AcountsRule!=AcountsRule_None)
	{
		//变量定义
		TCHAR szGameID[16]=TEXT("");
		TCHAR szStationID[16]=TEXT("");
		TCHAR szPassBuffer[MAX_TARGET_LEN]=TEXT("");

		//用户表项
		CRegKeyAide RegUserItem;
		RegUserItem.Attach(RegUserInfo.CreateItemKey(szUserID));

		//游戏标识
		if (pGlobalUserData->dwGameID!=0L)
		{
			// add by humroly 2018/8/3
			// replace func
			_sntprintf_s(szGameID, CountArray(szGameID), TEXT("%ld"), pGlobalUserData->dwGameID);
		}

		// add by humroly 2018/8/3
		// replace func
		_sntprintf_s(szStationID, CountArray(szStationID), TEXT("%ld"), pGlobalUserData->dwStationID);

		//用户密码
		if (pParameterGlobal->m_AcountsRule==AcountsRule_AccountsAndPass) 
		{
			LPCTSTR pszPassword = NULL;
			pszPassword = m_pDlgLogon->GetPassword();

			CXOREncrypt::EncryptData(pszPassword,szPassBuffer,CountArray(szPassBuffer));
		}

		//写入信息
		RegUserItem.WriteString(TEXT("GameID"),szGameID);
		RegUserItem.WriteString(TEXT("UserAccount"),pGlobalUserData->szAccounts);
		RegUserItem.WriteString(TEXT("UserPassToken"),pGlobalUserData->szPassword);
		RegUserItem.WriteString(TEXT("StationID"),szStationID);
		if(m_pDlgLogon!=NULL && m_pDlgLogon->GetLogonMode()==LogonMode_BBS_Accounts)
		{
			CXOREncrypt::EncryptData(m_pDlgLogon->m_szPassword,szPassBuffer,CountArray(szPassBuffer));
			RegUserItem.WriteString(TEXT("BBSPassword"),szPassBuffer);
			RegUserItem.WriteString(TEXT("BBSAccount"),m_pDlgLogon->m_szAccounts);
		}
		else
			RegUserItem.WriteString(TEXT("UserPassword"),szPassBuffer);
	}

	//删除记录
	if (pParameterGlobal->m_AcountsRule==AcountsRule_None)
	{
		RegUserInfo.RecurseDeleteKey(szUserID);
	}

	return;
}

//显示状态
VOID CSessionLogon::ShowStatusInfo(LPCTSTR pszStatusInfo)
{
	//创建窗口
	if (m_pDlgStatus==NULL)
	{
		m_pDlgStatus=new CDlgStatus;
	}

	//创建窗口
	if (m_pDlgStatus->m_hWnd==NULL)
	{
		m_pDlgStatus->Create(IDD_DLG_STATUS,CPlatformFrame::GetInstance());
	}

	//显示进度
	m_pDlgStatus->ShowStatusWnd(pszStatusInfo,m_pDlgLogon->m_hWnd,IDM_GIVE_UP_CONNECT);

	return;
}

//登录处理
bool CSessionLogon::OnSocketMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GP_LOGON_SUCCESS:	//登录成功
	{
		return OnSocketSubLogonSuccess(pData,wDataSize);
	}
	case SUB_GP_LOGON_FAILURE:	//登录失败
	{
		return OnSocketSubLogonFailure(pData,wDataSize);
	}
	case SUB_GP_LOGON_FINISH:	//登录完成
	{
		return OnSocketSubLogonFinish(pData,wDataSize);
	}
	case SUB_GP_LOGOUT_NOTIFY:	//登出提醒
	{
		return OnSocketSubLogoutNotify(pData, wDataSize);
	}
	case SUB_GP_UPDATE_NOTIFY:	//升级提示
	{
		return OnSocketSubUpdateNotify(pData,wDataSize);
	}
	case SUB_GP_LOGON_ACTIVE:	//激活帐号
	{
		return OnSocketSubActivateAccounts(pData,wDataSize);
	}
	case SUB_GP_SHOW_ONLINE:	//人数显示
	{
		return OnSocketSubShowOnLine(pData,wDataSize);
	}
	case SUB_GP_UPDATE_SILENT:	//后台升级
	{
		return OnSocketSubUpdateSilent(pData,wDataSize);
	}
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//系统处理
bool CSessionLogon::OnSocketMainSystem(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch( wSubCmdID )
	{
	case SUB_CM_ACTION_MESSAGE:	//动作消息
		{
			return OnSocketSubActionMessage(pData,wDataSize);
		}
	//case SUB_CM_SAFE_PROMPT:	//安全提醒
	//	{
	//		return OnSocketSubSafePrompt(pData,wDataSize);
	//	}
	case SUB_CM_SYSTEM_MESSAGE:  //系统消息
	{
		
		//效验参数
		//ASSERT(wDataSize == sizeof(CMD_CM_SystemMessage));
		//if (wDataSize != sizeof(CMD_CM_SystemMessage)) return false;

		//变量定义
		CMD_CM_SystemMessage* pSystemMessage = (CMD_CM_SystemMessage*)pData;

		//弹出提示
		CInformation Information;
		Information.ShowMessageBox(pSystemMessage->szString, MB_ICONERROR);
	}
	}

	ASSERT(FALSE);
	return false;
}

//玩家服务
bool CSessionLogon::OnSocketMainService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//房间列表
bool CSessionLogon::OnSocketMainServerList(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GP_LIST_TYPE:		//种类信息
	{
		//效验参数
		ASSERT(wDataSize % sizeof(tagGameType) == 0);
		if (wDataSize % sizeof(tagGameType) != 0) return false;

		//变量定义
		WORD wItemCount = wDataSize / sizeof(tagGameType);
		tagGameType* pGameType = (tagGameType*)(pData);

		//获取对象
		ASSERT(CServerListManager::GetInstance() != NULL);
		CServerListManager* pServerListManager = CServerListManager::GetInstance();

		//更新数据
		for (WORD i = 0; i < wItemCount; i++)
		{
			pServerListManager->InsertGameType(pGameType++);
		}

		return true;
	}
	case SUB_GP_LIST_KIND:		//类型信息
	{
		//效验参数
		ASSERT(wDataSize % sizeof(tagGameKind) == 0);
		if (wDataSize % sizeof(tagGameKind) != 0) return false;

		//变量定义
		WORD wItemCount = wDataSize / sizeof(tagGameKind);
		tagGameKind* pGameKind = (tagGameKind*)(pData);

		//获取对象
		ASSERT(CServerListManager::GetInstance() != NULL);
		CServerListManager* pServerListManager = CServerListManager::GetInstance();

		//更新数据
		for (WORD i = 0; i < wItemCount; i++)
		{
			pServerListManager->InsertGameKind(pGameKind++);
		}

		return true;
	}
	case SUB_GP_LIST_NODE:		//节点信息
	{
		//效验参数
		ASSERT(wDataSize % sizeof(tagGameNode) == 0);
		if (wDataSize % sizeof(tagGameNode) != 0) return false;

		//变量定义
		WORD wItemCount = wDataSize / sizeof(tagGameNode);
		tagGameNode* pGameNode = (tagGameNode*)(pData);

		//获取对象
		ASSERT(CServerListManager::GetInstance() != NULL);
		CServerListManager* pServerListManager = CServerListManager::GetInstance();

		//更新数据
		for (WORD i = 0; i < wItemCount; i++)
		{
			pServerListManager->InsertGameNode(pGameNode++);
		}

		return true;
	}
	case SUB_GP_LIST_SERVER:	//房间信息
	{
		//效验参数
		ASSERT(wDataSize % sizeof(tagPCGameServer) == 0);
		if (wDataSize % sizeof(tagPCGameServer) != 0) return false;

		//变量定义
		WORD wItemCount = wDataSize / sizeof(tagPCGameServer);
		tagPCGameServer* pGameServer = (tagPCGameServer*)(pData);

		//获取对象
		CServerListManager* pServerListManager = CServerListManager::GetInstance();

		//更新数据
		for (WORD i = 0; i < wItemCount; i++)
		{
			pServerListManager->InsertGameServer(pGameServer++);
		}

		return true;
	}
	case SUB_GP_LIST_CUSTOM:	//定制信息
	{
		//效验参数
		ASSERT(wDataSize % sizeof(tagGameCustom) == 0);
		if (wDataSize % sizeof(tagGameCustom) != 0) return false;

		//变量定义
		WORD wItemCount = wDataSize / sizeof(tagGameCustom);
		tagGameCustom* pGameCustom = (tagGameCustom*)(pData);

		//获取对象
		CServerListManager* pServerListManager = CServerListManager::GetInstance();

		//更新数据
		for (WORD i = 0; i < wItemCount; i++)
		{
			pServerListManager->InsertGameCustom(pGameCustom++);
		}

		return true;
	}
	case SUB_GP_LIST_FINISH:	//列表信息
	{
		//列表完成
		ASSERT(CServerListManager::GetInstance() != NULL);
		if (CServerListManager::GetInstance() != NULL) CServerListManager::GetInstance()->OnEventListFinish();

		//更新人数
		CServerListManager::GetInstance()->UpdateOnlineCount();

		return true;
	}
	case SUB_GP_SERVER_FINISH:	//房间完成
	{
		//效验数据
		ASSERT(wDataSize % sizeof(WORD) == 0);
		if (wDataSize % sizeof(WORD) != 0) return false;

		//获取对象
		ASSERT(CServerListManager::GetInstance() != NULL);
		CServerListManager* pServerListManager = CServerListManager::GetInstance();

		//任务处理
		for (UINT i = 0; i < (wDataSize / sizeof(WORD)); i++)
		{
			//m_GameKindIDPending.RemoveKey(((WORD*)pData)[i]);
			pServerListManager->OnEventKindFinish(((WORD*)pData)[i]);
		}

		//终止任务
		//if ((m_GameKindIDPending.GetCount() == 0L) && (m_bOnLineMission == false))
		{
			m_bActiveStatus = false;
		}

		return true;
	}
	case SUB_GR_KINE_ONLINE:	//类型在线
	{
		//变量定义
		CMD_GP_KindOnline* pKindOnline = (CMD_GP_KindOnline*)pData;
		WORD wHeadSize = (sizeof(CMD_GP_KindOnline) - sizeof(pKindOnline->OnLineInfoKind));

		//效验数据
		ASSERT((wDataSize >= wHeadSize) && (wDataSize == (wHeadSize + pKindOnline->wKindCount * sizeof(tagOnLineInfoKind))));
		if ((wDataSize < wHeadSize) || (wDataSize != (wHeadSize + pKindOnline->wKindCount * sizeof(tagOnLineInfoKind)))) return false;

		//获取对象
		CServerListManager* pServerListManager = CServerListManager::GetInstance();

		//设置人数
		for (WORD i = 0; i < pKindOnline->wKindCount; i++)
		{
			tagOnLineInfoKind* pOnLineInfoKind = &pKindOnline->OnLineInfoKind[i];
			pServerListManager->SetKindOnLineCount(pOnLineInfoKind->wKindID, pOnLineInfoKind->dwOnLineCount);
		}

		//更新人数
		pServerListManager->UpdateOnlineCount();

		return true;
	}
	case SUB_GR_SERVER_ONLINE:	//房间在线
	{
		//变量定义
		CMD_GP_ServerOnline* pServerOnline = (CMD_GP_ServerOnline*)pData;
		WORD wHeadSize = (sizeof(CMD_GP_ServerOnline) - sizeof(pServerOnline->OnLineInfoServer));

		//效验数据
		ASSERT((wDataSize >= wHeadSize) && (wDataSize == (wHeadSize + pServerOnline->wServerCount * sizeof(tagOnLineInfoServer))));
		if ((wDataSize < wHeadSize) || (wDataSize != (wHeadSize + pServerOnline->wServerCount * sizeof(tagOnLineInfoServer)))) return false;

		//获取对象
		CServerListManager* pServerListManager = CServerListManager::GetInstance();

		//设置人数
		for (WORD i = 0; i < pServerOnline->wServerCount; i++)
		{
			tagOnLineInfoServer* pOnLineInfoServer = &pServerOnline->OnLineInfoServer[i];
			pServerListManager->SetServerOnLineCount(pOnLineInfoServer->wServerID, pOnLineInfoServer->dwOnLineCount);
		}

		//终止任务
		//if ((m_GameKindIDPending.GetCount() == 0L) && (m_bOnLineMission == false))
		{
			//终止任务
			m_bActiveStatus = false;
		}

		return true;
	}
	}

	return false;
}

//登录成功
bool CSessionLogon::OnSocketSubLogonSuccess(VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize>=sizeof(CMD_GP_LogonSuccess));
	if (wDataSize<sizeof(CMD_GP_LogonSuccess)) return false;

	//消息处理
	CMD_GP_LogonSuccess * pLogonSuccess=(CMD_GP_LogonSuccess *)pData;

	//变量定义
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//保存信息
	pGlobalUserData->cbAge=pLogonSuccess->cbAge;
	pGlobalUserData->wFaceID=pLogonSuccess->wFaceID;
	pGlobalUserData->cbGender=pLogonSuccess->cbGender;
	pGlobalUserData->dwGameID=pLogonSuccess->dwGameID;
	pGlobalUserData->dwUserID=pLogonSuccess->dwUserID;
	pGlobalUserData->dwCustomID=pLogonSuccess->dwCustomID;
	pGlobalUserData->dwExperience=pLogonSuccess->dwExperience;
	pGlobalUserData->cbMasterOrder = pLogonSuccess->cbMasterOrder;
	pGlobalUserData->dwProtectID = pLogonSuccess->dwProtectID;
	pGlobalUserData->cbMoorMachine = pLogonSuccess->cbMoorMachine;
	pGlobalUserData->lGold = pLogonSuccess->lUserGold;
	pGlobalUserData->lInsureGold = pLogonSuccess->lInsureGold;

	//用户帐号
	CW2CT strAccounts(pLogonSuccess->szAccounts);
	lstrcpyn(pGlobalUserData->szAccounts,strAccounts,CountArray(pGlobalUserData->szAccounts));

	//用户密码
	CW2CT strPassword(pLogonSuccess->szPassword);
	lstrcpyn(pGlobalUserData->szPassword,strPassword,CountArray(pGlobalUserData->szPassword));

	//扩展信息
	VOID * pDataBuffer=NULL;
	tagDataDescribe DataDescribe;
	CRecvPacketHelper RecvPacket(pLogonSuccess+1,wDataSize-sizeof(CMD_GP_LogonSuccess));
	while (true)
	{
		//提取数据
		pDataBuffer=RecvPacket.GetData(DataDescribe);
		if (DataDescribe.wDataDescribe==DTP_NULL) break;

		//提取数据
		switch (DataDescribe.wDataDescribe)
		{
		case DTP_GP_AREA_INFO:		//区域信息
			{
				ASSERT(DataDescribe.wDataSize==sizeof(DTP_GP_AreaInfo));
				if (DataDescribe.wDataSize==sizeof(DTP_GP_AreaInfo))
				{
					CW2CT strArea(((DTP_GP_AreaInfo *)pDataBuffer)->szArea);
					CW2CT strCity(((DTP_GP_AreaInfo *)pDataBuffer)->szCity);
					CW2CT strProvince(((DTP_GP_AreaInfo *)pDataBuffer)->szProvince);
					lstrcpyn(pGlobalUserData->szArea,strArea,CountArray(pGlobalUserData->szArea));
					lstrcpyn(pGlobalUserData->szCity,strCity,CountArray(pGlobalUserData->szCity));
					lstrcpyn(pGlobalUserData->szProvince,strProvince,CountArray(pGlobalUserData->szProvince));
				}

				break;
			}
		case DTP_GP_MEMBER_INFO:	//会员信息
			{
				ASSERT(DataDescribe.wDataSize==sizeof(DTP_GP_MemberInfo));
				if (DataDescribe.wDataSize==sizeof(DTP_GP_MemberInfo))
				{
					DTP_GP_MemberInfo * pMemberInfo=(DTP_GP_MemberInfo *)pDataBuffer;
					pGlobalUserData->cbMemberOrder=pMemberInfo->cbMemberOrder;
					//pGlobalUserData->MemberOverDate=pMemberInfo->MemberOverDate;
				}

				break;
			}
		case DTP_GP_UNDER_WRITE:	//个性签名
			{
				CW2CT strUnderWrite((WCHAR *)pDataBuffer);
				lstrcpyn(pGlobalUserData->szUnderWrite,strUnderWrite,CountArray(pGlobalUserData->szUnderWrite));

				break;
			}
		case DTP_GP_NICKNAME:		//用户昵称
			{
				CW2CT strNickName((WCHAR *)pDataBuffer);
				lstrcpyn(pGlobalUserData->szNickName,strNickName,CountArray(pGlobalUserData->szNickName));

				break;
			}
		}
	}

	//本地加载
	/*if (pLogonSuccess->dwCustomID!=0)
	{
		CCustomFaceManager * pCustomFaceManager=CCustomFaceManager::GetInstance();
		pCustomFaceManager->LoadUserCustomFace(pLogonSuccess->dwUserID,pLogonSuccess->dwCustomID,pGlobalUserData->CustomFaceInfo);
	}*/

	//设置提示
	ShowStatusInfo(TEXT("正在获取游戏列表..."));

	return true;
}

//登录失败
bool CSessionLogon::OnSocketSubLogonFailure(VOID * pData, WORD wDataSize)
{
	//变量定义
	CMD_GP_LogonFailure * pLogonFailure=(CMD_GP_LogonFailure *)pData;

	//效验数据
	ASSERT(wDataSize>=(sizeof(CMD_GP_LogonFailure)-sizeof(pLogonFailure->szDescribeString)));
	if (wDataSize<(sizeof(CMD_GP_LogonFailure)-sizeof(pLogonFailure->szDescribeString))) return false;

	//关闭处理
	m_bActiveStatus = false;

	//进度界面
	if ((m_pDlgStatus!=NULL)&&(m_pDlgStatus->m_hWnd!=NULL))
	{
		m_pDlgStatus->DestroyWindow();
	}

	//激活请求
	if( pLogonFailure->lErrorCode == LGF_NEED_ACTIVATE )
	{
		CGlobalStation *pGlobalStation = CGlobalStation::GetInstance();
		ASSERT(pGlobalStation&&m_pDlgLogon);
		if( pGlobalStation && m_pDlgLogon )
		{
			TCHAR szPassword[LEN_MD5] = TEXT("");
			CMD5Encrypt::EncryptData(m_pDlgLogon->GetPassword(),szPassword);

			//构造地址
			TCHAR szURL[MAX_PATH] = TEXT("");
			// add by humroly 2018/8/3
			// replace func
			_sntprintf_s(szURL, CountArray(szURL), TEXT("http://service.abcdefgh.com/AccountsCheck.aspx?StationID=%ld&UserName=%s&Password=%s"),
				pGlobalStation->GetStationID(),m_pDlgLogon->GetAccounts(),szPassword );

			if( m_HttpServiceHelper.GetInterface() == NULL )
			{
				m_HttpServiceHelper.CreateInstance();

				m_HttpServiceHelper->SetHttpServiceSink(this);
			}

			m_HttpServiceHelper->ConnectServer(szURL);
			return true;
		}
	}
	else if( pLogonFailure->lErrorCode == LGF_CHANGE_ACCOUNT )
	{
		//提示更改帐号
		CW2CT strDescribeString(pLogonFailure->szDescribeString);
		if( lstrlen(strDescribeString)>0 )
		{
			TCHAR szTips[128] = TEXT("");
			// add by humroly 2018/8/3
			// replace func
			_sntprintf_s(szTips, CountArray(szTips), TEXT("您的帐号已经激活并绑定了新帐号 【 %s 】，请记住新帐号并使用新帐号登录！"), (LPCTSTR)strDescribeString);

			ShowInformation(szTips,MB_ICONINFORMATION|MB_OK,30,AfxGetMainWnd());

			//设置帐号
			if( m_pDlgLogon && m_pDlgLogon->m_hWnd )
			{
				m_pDlgLogon->DeleteAccounts(NULL);
				m_pDlgLogon->SetAccounts((LPCTSTR)strDescribeString);
			}

			m_cbMissionType = MT_LOGON;

			RestoreShow();

			return true;
		}
	}

	//显示消息
	CW2CT strDescribeString(pLogonFailure->szDescribeString);
	if (lstrlen(strDescribeString)>0) ShowInformation(strDescribeString,MB_ICONERROR,60);

	//重新登录
	RestoreShow();

	return true;
}

//登录完成
bool CSessionLogon::OnSocketSubLogonFinish(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GP_LogonFinish));
	if (wDataSize!=sizeof(CMD_GP_LogonFinish)) return false;

	//变量定义
	CMD_GP_LogonFinish * pLogonFinish=(CMD_GP_LogonFinish *)pData;

	//变量定义
	CGlobalStation * pGlobalStation=CGlobalStation::GetInstance();
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();

	//时间配置
	pParameterGlobal->m_wIntermitTime=pLogonFinish->wIntermitTime;
	pParameterGlobal->m_wOnLineCountTime=pLogonFinish->wOnLineCountTime;

	//更新信息
	UpdateUserInfo();

	//删除控件
	SafeDelete(m_pDlgLogon);
	SafeDelete(m_pDlgStatus);

	//终止任务
	//m_bActiveStatus = false;

	//发送事件
	CDispatchManager * pDispatchManager=CDispatchManager::GetInstance();
	if (pDispatchManager!=NULL) pDispatchManager->SendEventInform(EVENT_LOGON_SUCCESS,0L);

	//变量定义
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//网络加载
	//if ((pGlobalUserData->dwCustomID!=0)&&(pGlobalUserData->CustomFaceInfo.dwDataSize==0L))
	//{
	//	CCustomFaceManager * pCustomFaceManager=CCustomFaceManager::GetInstance();
	//	pCustomFaceManager->LoadUserCustomFace(pGlobalUserData->dwUserID,pGlobalUserData->dwCustomID);
	//}

	return true;
}

//登录登出
bool CSessionLogon::OnSocketSubLogoutNotify(VOID* pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize <= sizeof(CMD_GP_LogoutNotify));
	if (wDataSize > sizeof(CMD_GP_LogoutNotify)) return false;

	//变量定义
	auto pLogoutNotify = (CMD_GP_LogoutNotify*)pData;

	//终止任务
	m_bActiveStatus = false;
	
	//发送事件
	CDispatchManager* pDispatchManager = CDispatchManager::GetInstance();
	if (pDispatchManager != NULL) pDispatchManager->SendEventInform(EVENT_LOGOUT_NOTIFY, 0L);

	return true;
}

//升级提示
bool CSessionLogon::OnSocketSubUpdateNotify(VOID * pData, WORD wDataSize)
{                                       
	//进度界面
	CloseStatusInfo();

	//错误提示
	CInformation Information(CPlatformFrame::GetInstance());
	Information.ShowMessageBox(TEXT("版本错误！"), MB_ICONERROR);

	//重新登录
	RestoreShow();

	return true;
}

//动作消息
bool CSessionLogon::OnSocketSubActionMessage(VOID * pData, WORD wDataSize)
{
	//变量定义
	CMD_CM_ActionMessage * pActionMessage=(CMD_CM_ActionMessage *)pData;
	WORD wHeadSize=sizeof(CMD_CM_ActionMessage)-sizeof(pActionMessage->szString);

	//效验参数
	ASSERT((wDataSize>wHeadSize)&&(wDataSize>=(wHeadSize+pActionMessage->wLength*sizeof(WCHAR))));
	if ((wDataSize<=wHeadSize)||(wDataSize<(wHeadSize+pActionMessage->wLength*sizeof(WCHAR)))) return false;

	//弹出消息
	CW2CT strMessage(pActionMessage->szString);
	INT nResultCode = ShowInformation(strMessage,pActionMessage->nButtonType,0,AfxGetMainWnd());

	//变量定义
	WORD wExcursion=wHeadSize+pActionMessage->wLength*sizeof(WCHAR);

	////提取动作
	//while (wExcursion<wDataSize)
	//{
	//	//变量定义
	//	tagActionHead * pActionHead=(tagActionHead *)((BYTE *)pData+wExcursion);

	//	//效验参数
	//	ASSERT((wExcursion+sizeof(tagActionHead))<=wDataSize);
	//	ASSERT((wExcursion+sizeof(tagActionHead)+pActionHead->wAppendSize)<=wDataSize);

	//	//效验参数
	//	if ((wExcursion+sizeof(tagActionHead))>wDataSize) return false;
	//	if ((wExcursion+sizeof(tagActionHead)+pActionHead->wAppendSize)>wDataSize) return false;

	//	//动作出来
	//	if (nResultCode==pActionHead->uResponseID)
	//	{
	//		switch (pActionHead->cbActionType)
	//		{
	//		case ACT_BROWSE:	//浏览动作
	//			{
	//				//变量定义
	//				WORD wDataPos=wExcursion+sizeof(tagActionHead);
	//				tagActionBrowse * pActionBrowse=(tagActionBrowse *)((BYTE *)pData+wDataPos);

	//				//I E 浏览
	//				if (pActionBrowse->cbBrowseType&BRT_IE)
	//				{
	//					CW2CT strBrowseUrl(pActionBrowse->szBrowseUrl);
	//					ShellExecute(NULL,TEXT("open"),strBrowseUrl,NULL,NULL,SW_NORMAL);
	//				}

	//				break;
	//			}
	//		case ACT_DOWN_LOAD:	//下载动作
	//			{
	//				//变量定义
	//				WORD wDataPos=wExcursion+sizeof(tagActionHead);
	//				tagActionDownLoad * pActionDownLoad=(tagActionDownLoad *)((BYTE *)pData+wDataPos);

	//				//I E 下载
	//				if (pActionDownLoad->cbDownLoadMode&DLT_IE)
	//				{
	//					CW2CT strDownLoadUrl(pActionDownLoad->szDownLoadUrl);
	//					ShellExecute(NULL,TEXT("open"),strDownLoadUrl,NULL,NULL,SW_NORMAL);
	//				}

	//				//下载模块
	//				if (pActionDownLoad->cbDownLoadMode&DLT_MODULE)
	//				{
	//					CW2CT strDownLoadUrl(pActionDownLoad->szDownLoadUrl);
	//					ShellExecute(NULL,TEXT("open"),strDownLoadUrl,NULL,NULL,SW_NORMAL);
	//				}

	//				break;
	//			}
	//		}
	//	}

	//	//增加偏移
	//	wExcursion+=(sizeof(tagActionHead)+pActionHead->wAppendSize);
	//}

	return true;
}

//关闭状态
VOID CSessionLogon::CloseStatusInfo()
{
	//关闭提示
	if ((m_pDlgStatus!=NULL)&&(m_pDlgStatus->m_hWnd!=NULL))
	{
		m_pDlgStatus->DestroyWindow();
	}

	return;
}

//恢复界面
VOID CSessionLogon::RestoreShow()
{
	//显示界面
	ShowLogon();

	return;
}

//安全提醒
bool CSessionLogon::OnSocketSubSafePrompt(VOID * pData, WORD wDataSize)
{
	//RestoreShow();

	return true;
}

//激活帐号
bool CSessionLogon::OnSocketSubActivateAccounts(VOID * pData, WORD wDataSize)
{
	//RestoreShow();

	return true;
}

//人数显示
bool CSessionLogon::OnSocketSubShowOnLine(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DWORD));
	if(wDataSize!=sizeof(DWORD)) return false;

	return true;
}

//后台升级
bool CSessionLogon::OnSocketSubUpdateSilent(VOID * pData, WORD wDataSize)
{
	return true;
}
//////////////////////////////////////////////////////////////////////////
