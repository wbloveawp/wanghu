#include "StdAfx.h"
#include "VirtualLinkManager.h"

#pragma warning (disable : 4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//关闭连接
#define IDI_CLOSE_LINK					100								//激活任务

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CVirtualLinkManager, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
// 
//构造函数
CSessionItem::CSessionItem()
{
	//设置变量
	m_bActiveStatus = false;
	m_pVirtualLinkItem = NULL;
}

//析构函数
CSessionItem::~CSessionItem()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CVirtualLinkItem::CVirtualLinkItem()
{
	//状态变量
	m_cbLinkStatus= LINK_STATUS_IDLE;

	//设置变量	
	m_wServiceID=0;
	m_wServiceModule=0;
	m_pVirtualLinkManager=NULL;
	ZeroMemory(&m_LinkService, sizeof(m_LinkService));

	return;
}

//析构函数
CVirtualLinkItem::~CVirtualLinkItem()
{
}

//准备就绪
bool CVirtualLinkItem::OnEventSocketReady()
{
	if (m_pVirtualLinkManager != NULL)
	{
		m_pVirtualLinkManager->SendSocketData(MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_C_LINK_SERVICE, &m_LinkService, sizeof(m_LinkService));
	}

	return true;
}

//关闭事件
bool CVirtualLinkItem::OnEventVirtualShut(BYTE cbShutReason)
{
	for (INT_PTR i = 0; i < m_SessionItemArray.GetCount(); i++)
	{
		if (m_SessionItemArray[i]->m_bActiveStatus == true)
		{
			m_SessionItemArray[i]->m_bActiveStatus = false;
			m_SessionItemArray[i]->OnEventMissionShut(cbShutReason);			
		}
	}

	return true;
}

//连接事件
bool CVirtualLinkItem::OnEventVirtualLink(LONG lErrorCode)
{
	for (INT_PTR i = 0; i < m_SessionItemArray.GetCount(); i++)
	{
		if (m_SessionItemArray[i]->m_bActiveStatus==true)
		{
			m_SessionItemArray[i]->OnEventMissionLink(lErrorCode);			
		}
	}

	return true;
}

//读取事件
bool CVirtualLinkItem::OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	for (INT_PTR i = 0; i < m_SessionItemArray.GetCount(); i++)
	{
		if (m_SessionItemArray[i]->m_bActiveStatus == true)
		{
			m_SessionItemArray[i]->OnEventMissionRead(Command, pData, wDataSize);
		}
	}

	return true;
}

//加入任务
bool CVirtualLinkItem::InsertSessionItem(CSessionItem* pSessionItem)
{
	//查找现有
	for (INT_PTR i = 0; i < m_SessionItemArray.GetCount(); i++)
	{
		if (m_SessionItemArray[i] == pSessionItem)
		{
			//ASSERT(FALSE);
			return true;
		}
	}

	//设置子项
	pSessionItem->m_bActiveStatus = false;
	pSessionItem->m_pVirtualLinkItem = this;

	//加入任务
	m_SessionItemArray.Add(pSessionItem);

	return true;
}

//删除任务
VOID CVirtualLinkItem::DeleteSessionItem()
{
	//删除任务
	for (INT_PTR i = 0; i < m_SessionItemArray.GetCount(); i++)
	{
		//设置子项
		m_SessionItemArray[i]->m_bActiveStatus = false;
		m_SessionItemArray[i]->m_pVirtualLinkItem = NULL;
	}

	m_SessionItemArray.RemoveAll();
}

//删除任务
bool CVirtualLinkItem::DeleteSessionItem(CSessionItem* pSessionItem)
{
	//删除任务
	for (INT_PTR i = 0; i < m_SessionItemArray.GetCount(); i++)
	{
		if (m_SessionItemArray[i] == pSessionItem)
		{
			//删除数组
			m_SessionItemArray.RemoveAt(i);

			//设置子项
			pSessionItem->m_bActiveStatus = false;
			pSessionItem->m_pVirtualLinkItem = NULL;

			return true;
		}
	}

	return false;
}

//发送数据
bool CVirtualLinkItem::SendVirtualData(WORD wMainCmdID, WORD wSubCmdID)
{
	//校验状态
	ASSERT(m_wServiceModule != 0 && m_cbLinkStatus == LINK_STATUS_LINKED);
	if (m_wServiceModule == 0 || m_cbLinkStatus != LINK_STATUS_LINKED) return false;

	return m_pVirtualLinkManager->SendSocketData(MAKECMMD(wMainCmdID, m_wServiceModule), wSubCmdID, NULL, 0);
}

//发送数据
bool CVirtualLinkItem::SendVirtualData(WORD wMainCmdID, WORD wSubCmdID, VOID* const pData, WORD wDataSize)
{
	//校验状态
	ASSERT(m_wServiceModule != 0 && m_cbLinkStatus == LINK_STATUS_LINKED);
	if (m_wServiceModule == 0 || m_cbLinkStatus != LINK_STATUS_LINKED) return false;

	return m_pVirtualLinkManager->SendSocketData(MAKECMMD(wMainCmdID, m_wServiceModule), wSubCmdID, pData, wDataSize);
}

//关闭链接
VOID CVirtualLinkItem::CloseVirtualLink()
{
	//构造结构
	CMD_GG_C_DislinkService DislinkService;
	ZeroMemory(&DislinkService,sizeof(DislinkService));

	//设置变量
	DislinkService.wServiceModule = m_wServiceModule;

	//发送数据
	m_pVirtualLinkManager->SendSocketData(MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_C_DISLINK_SERVICE, &DislinkService, sizeof(DislinkService));

	//设置状态
	m_cbLinkStatus = LINK_STATUS_IDLE;
}

//创建连接
bool CVirtualLinkItem::CreateVirtualLink(WORD wServiceID, DWORD dwUserID, LPCTSTR pszLogonToken, BYTE cbExtendData[], WORD wDataSize)
{
	//模块信息
	m_LinkService.wServiceID = wServiceID;
	m_LinkService.wServiceModule = m_wServiceModule;

	//用户信息
	m_LinkService.dwUserID = dwUserID;

	//登录令牌
	if (pszLogonToken != NULL)
	{
		StringCchCopy(m_LinkService.szLogonToken, CountArray(m_LinkService.szLogonToken), pszLogonToken);
	}	

	//扩展数据
	if (wDataSize>0 && wDataSize<=CountArray(m_LinkService.cbLinkExtend))
	{
		CopyMemory(m_LinkService.cbLinkExtend, cbExtendData, wDataSize);
	}	

	//判断状态
	return m_pVirtualLinkManager->ActiveVirtualLink(this);	
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CVirtualLinkManager::CVirtualLinkManager()
{
	//状态变量
	m_bSwitchDns = false;
	m_bTryDefault = false;
	m_bTryRedirect = false;
	m_wSwitchIndex = INVALID_WORD;

	//当前连接
	m_wCurrentPort = PORT_LOGON;
	m_dwCurrentServer = INADDR_NONE;

	//地址信息
	m_szCurrentServer[0] = 0;

	return;
}

//析构函数
CVirtualLinkManager::~CVirtualLinkManager()
{
	//关闭连接
	ConcludeVirtualLink();
}

//接口查询
VOID* __cdecl CVirtualLinkManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITCPSocketSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITCPSocketSink, Guid, dwQueryVer);
	return NULL;
}

//连接事件
bool CVirtualLinkManager::OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode)
{
	//关闭通知
	for (INT_PTR i = 0; i < m_VirtualLinkArray.GetCount(); i++)
	{
		//获取子项
		CVirtualLinkItem* pVirtualLinkItem = m_VirtualLinkArray[i];
		if (pVirtualLinkItem == NULL) continue;

		//事件处理
		if (pVirtualLinkItem->m_cbLinkStatus == LINK_STATUS_WAIT)
		{
			if (nErrorCode == 0)
			{
				pVirtualLinkItem->OnEventSocketReady();
			}
			else
			{
				pVirtualLinkItem->OnEventVirtualLink(nErrorCode);
			}
		}
	}

	return true;
}

//关闭事件
bool CVirtualLinkManager::OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason)
{
	//关闭通知
	for (INT_PTR i=0; i<m_VirtualLinkArray.GetCount(); i++)
	{
		//获取子项
		CVirtualLinkItem * pVirtualLinkItem=m_VirtualLinkArray[i];
		if (pVirtualLinkItem == NULL) continue;

		//事件处理
		if (pVirtualLinkItem->m_cbLinkStatus == LINK_STATUS_LINKED)
		{
			//设置状态
			pVirtualLinkItem->m_wServiceID = 0;
			pVirtualLinkItem->m_cbLinkStatus = LINK_STATUS_IDLE;

			//关闭通知
			pVirtualLinkItem->OnEventVirtualShut(cbShutReason);
		}
	}	

	return true;
}

//读取事件
bool CVirtualLinkManager::OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID* pData, WORD wDataSize)
{
	//获取模块
	WORD wServiceModule = HICMMD(Command.wMainCmdID);

	//移除模块
	Command.wMainCmdID = LOCMMD(Command.wMainCmdID);

	//网关模块
	if (wServiceModule == SERVICE_MODULE_GATE)
	{
		//链接成功
		if (Command.wSubCmdID == SUB_GG_S_LINK_SUCCESS)
		{
			//提取数据
			auto pLinkSuccess = (CMD_GG_S_LinkSuccess*)pData;		
			if (m_VirtualLinkArray.GetCount() >= pLinkSuccess->wServiceModule)
			{
				//获取链接
				auto pVirtualLinkItem = m_VirtualLinkArray.GetAt(pLinkSuccess->wServiceModule - 1);
				if (pVirtualLinkItem != NULL)
				{
					//设置状态
					pVirtualLinkItem->m_cbLinkStatus = LINK_STATUS_LINKED;
					pVirtualLinkItem->m_wServiceID = pLinkSuccess->wServiceID;

					//链接通知
					pVirtualLinkItem->OnEventVirtualLink(0);
				}
			}

			return true;
		}
		
		//链接失败
		if (Command.wSubCmdID == SUB_GG_S_LINK_FAILURE)
		{
			//提取数据
			auto pLinkFailure = (CMD_GG_S_LinkFailure*)pData;
			if (m_VirtualLinkArray.GetCount() >= pLinkFailure->wServiceModule)
			{
				//获取链接
				auto pVirtualLinkItem = m_VirtualLinkArray.GetAt(pLinkFailure->wServiceModule - 1);
				if (pVirtualLinkItem != NULL)
				{
					//链接通知
					pVirtualLinkItem->OnEventVirtualLink(pLinkFailure->lErrorCode);
				}
			}

			return true;
		}

		//链接断开
		if (Command.wSubCmdID == SUB_GG_S_LINK_SHUTDOWN)
		{
			//提取数据
			auto pLinkShutdown = (CMD_GG_S_LinkShutdown*)pData;
			if (m_VirtualLinkArray.GetCount() >= pLinkShutdown->wServiceModule)
			{
				//获取链接
				auto pVirtualLinkItem = m_VirtualLinkArray.GetAt(pLinkShutdown->wServiceModule - 1);
				if (pVirtualLinkItem != NULL)
				{
					//设置状态
					pVirtualLinkItem->m_wServiceID = 0;
					pVirtualLinkItem->m_cbLinkStatus = LINK_STATUS_IDLE;

					//关闭通知
					pVirtualLinkItem->OnEventVirtualShut(SHUT_REASON_NORMAL);
				}
			}

			//统计链接
			WORD wLinkCount = 0;
			for (int i = 0; i < m_VirtualLinkArray.GetCount(); i++)
			{
				if (m_VirtualLinkArray[i] == NULL) continue;
				if (m_VirtualLinkArray[i]->m_cbLinkStatus != LINK_STATUS_IDLE) wLinkCount++;
			}

			//关闭网络
			if (wLinkCount==0) ConcludeVirtualLink();

			return true;
		}

		return true;
	}

	//获取链接
	if (m_VirtualLinkArray.GetCount() >= wServiceModule)
	{
		auto pVirtualLinkItem = m_VirtualLinkArray.GetAt(wServiceModule - 1);
		if (pVirtualLinkItem != NULL && pVirtualLinkItem->m_cbLinkStatus == LINK_STATUS_LINKED)
		{
			pVirtualLinkItem->OnEventVirtualRead(Command, pData, wDataSize);
		}
	}

	return true;
}

//网络状态
BYTE CVirtualLinkManager::GetSocketStatus()
{
	//变量定义
	BYTE cbSocketStatus = SOCKET_STATUS_IDLE;

	//获取状态
	if (m_TCPSocketModule.GetInterface() != NULL)
	{
		cbSocketStatus = m_TCPSocketModule->GetSocketStatus();
	}

	return cbSocketStatus;
}

//设置地址
VOID CVirtualLinkManager::SetCurrentServer(LPCTSTR pszCurrentServer)
{
	//设置地址
	StringCchCopy(m_szCurrentServer, CountArray(m_szCurrentServer), pszCurrentServer);

	return;
}

//加入链接
bool CVirtualLinkManager::InsertVirtualLink(CVirtualLinkItem * pVirtualLinkItem)
{
	//校验参数
	ASSERT (pVirtualLinkItem!=NULL && pVirtualLinkItem->GetServiceModule()>0);
	if (pVirtualLinkItem == NULL || pVirtualLinkItem->GetServiceModule() == 0) return false;

	//获取模块
	WORD wServiceModule = pVirtualLinkItem->GetServiceModule();

	//调整大小
	if (wServiceModule > m_VirtualLinkArray.GetCount())
	{
		m_VirtualLinkArray.SetSize(wServiceModule);
	}
	
	//设置子项
	pVirtualLinkItem->m_cbLinkStatus= LINK_STATUS_IDLE;
	pVirtualLinkItem->m_pVirtualLinkManager=this;

	//设置子项
	m_VirtualLinkArray[wServiceModule-1]=pVirtualLinkItem;

	return true;
}

//删除任务
bool CVirtualLinkManager::DeleteVirtualLink(CVirtualLinkItem * pVirtualLinkItem)
{
	//关闭链接
	if (pVirtualLinkItem->m_cbLinkStatus==LINK_STATUS_LINKED)
	{
		pVirtualLinkItem->CloseVirtualLink();
	}

	//获取模块
	WORD wServiceModule = pVirtualLinkItem->GetServiceModule();
	if (wServiceModule > m_VirtualLinkArray.GetCount()) return false;

	//移除链接
	m_VirtualLinkArray.RemoveAt(wServiceModule-1);

	return true;
}

//关闭网络
bool CVirtualLinkManager::ConcludeVirtualLink()
{
	//关闭连接
	//ASSERT(m_TCPSocketModule.GetInterface() != NULL);
	if (m_TCPSocketModule.GetInterface() != NULL) m_TCPSocketModule->CloseSocket();

	return true;
}

//发起连接
bool CVirtualLinkManager::ActiveVirtualLink(CVirtualLinkItem* pVirtualLinkItem)
{
	//校验参数
	ASSERT (pVirtualLinkItem!=NULL);
	if (pVirtualLinkItem == NULL) return false;

	//获取状态
	auto cbSocketStatus = GetSocketStatus();

	//空闲状态
	if (cbSocketStatus ==SOCKET_STATUS_IDLE)
	{
		//关闭窗口
		if (m_hWnd != NULL) DestroyWindow();

		//设置变量
		m_bSwitchDns = false;
		m_bTryDefault = true;
		m_bTryRedirect = false;
		m_wSwitchIndex = INVALID_WORD;

		//设置状态
		pVirtualLinkItem->m_cbLinkStatus = LINK_STATUS_IDLE;

		//读取地址
		if (m_szCurrentServer[0] == 0)
		{
			CGlobalServer GlobalServer;
			GlobalServer.GetLastServerName(m_szCurrentServer);
		}

		//设置变量
		m_wCurrentPort = PORT_LOGON;

		//默认地址
		if ((GetServerInfo() == false) && (SwitchServerItem() == false))
		{
			ASSERT(FALSE);
			return false;
		}

		//创建窗口
		if (m_hWnd == NULL)
		{
			CRect rcCreate(0, 0, 0, 0);
			Create(NULL, NULL, WS_CHILD, rcCreate, GetDesktopWindow(), 100);
		}

		//创建组件
		if (m_TCPSocketModule.GetInterface() == NULL)
		{
			if (m_TCPSocketModule.CreateInstance() == false) return false;
			if (m_TCPSocketModule->SetTCPSocketSink(QUERY_ME_INTERFACE(IUnknownEx)) == false) return false;
		}

		//发起连接
		if (m_TCPSocketModule->Connect(m_dwCurrentServer, m_wCurrentPort) != CONNECT_SUCCESS) return false;

		if (m_szCurrentServer[0] != 0)
		{
			CGlobalServer GlobalServer;
			GlobalServer.SetLastServerName(m_szCurrentServer);
		}		

		//设置状态
		pVirtualLinkItem->m_cbLinkStatus = LINK_STATUS_WAIT;
	}

	//连接状态
	if (cbSocketStatus == SOCKET_STATUS_CONNECT)
	{
		//设置状态
		pVirtualLinkItem->m_cbLinkStatus = LINK_STATUS_LINKED;

		//连接就绪
		pVirtualLinkItem->OnEventSocketReady();
	}	

	return true;
}

//发送函数
bool CVirtualLinkManager::SendSocketData(WORD wMainCmdID, WORD wSubCmdID)
{
	//状态效验
	//ASSERT(GetSocketStatus() == SOCKET_STATUS_CONNECT);
	if (GetSocketStatus() != SOCKET_STATUS_CONNECT) return false;

	//发送数据
	ASSERT(m_TCPSocketModule.GetInterface() != NULL);
	if (m_TCPSocketModule.GetInterface() != NULL) m_TCPSocketModule->SendData(wMainCmdID, wSubCmdID);

	return true;
}

//发送函数
bool CVirtualLinkManager::SendSocketData(WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize)
{
	//状态效验
	//ASSERT(GetSocketStatus() == SOCKET_STATUS_CONNECT);
	if (GetSocketStatus() != SOCKET_STATUS_CONNECT) return false;

	//发送数据
	ASSERT(m_TCPSocketModule.GetInterface() != NULL);
	if (m_TCPSocketModule.GetInterface() != NULL) m_TCPSocketModule->SendData(wMainCmdID, wSubCmdID, pData, wDataSize);

	return true;
}

//时间消息
VOID CVirtualLinkManager::OnTimer(UINT_PTR nIDEvent)
{
	//关闭连接
	if (nIDEvent==IDI_CLOSE_LINK)
	{
		//关闭窗口
		DestroyWindow();

		//终止连接
		//PerformClose();

		return;
	}

	__super::OnTimer(nIDEvent);
}


//地址信息
bool CVirtualLinkManager::GetServerInfo()
{
	//效验参数
	ASSERT(m_szCurrentServer[0] != 0);
	if (m_szCurrentServer[0] == 0) return false;

	//变量定义
	tagServerItem ServerItem;
	ZeroMemory(&ServerItem, sizeof(ServerItem));

	//设置变量
	m_wCurrentPort = PORT_LOGON;
	m_dwCurrentServer = INADDR_NONE;

	//获取信息
	CGlobalServer GlobalServer;
	CParameterGlobal* pParameterGlobal = CParameterGlobal::GetInstance();
	bool bSuccess = GlobalServer.GetServerItemInfo(m_szCurrentServer, pParameterGlobal->m_NetworkKind, ServerItem);

	//转换地址
	if (bSuccess == true)
	{
		//使用域名
		if (m_bSwitchDns == true)
		{
			LPHOSTENT lpHost = gethostbyname(CT2CA(ServerItem.szServerHost));
			if (lpHost != NULL)
			{
				ServerItem.dwServerAddr = ((LPIN_ADDR)lpHost->h_addr)->s_addr;

				//更新地址
				if (ServerItem.szServerHost[0] != 0)
					GlobalServer.WriteServerItemInfo(m_szCurrentServer, pParameterGlobal->m_NetworkKind, ServerItem);
			}
		}

		//使用地址
		if ((m_bSwitchDns == false) && (ServerItem.dwServerAddr == INADDR_NONE))
		{
			CT2CA strServerDomain(ServerItem.szServerHost);
			ServerItem.dwServerAddr = inet_addr(strServerDomain);
		}
	}

	//名字翻译
	if (ServerItem.dwServerAddr == INADDR_NONE)
	{
		//地址转换
		CT2CA CurrentServer(m_szCurrentServer);
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

		return true;
	}

	return false;
}

//切换连接
bool CVirtualLinkManager::SwitchServerItem()
{
	TCHAR szCurrentServer[LEN_SERVER] = TEXT("");
	StringCchCopy(szCurrentServer, CountArray(szCurrentServer), m_szCurrentServer);

	do
	{
		//读取信息
		CGlobalServer GlobalServer;
		GlobalServer.GetServerItemName(++m_wSwitchIndex, m_szCurrentServer);

		//状态切换
		if ((m_szCurrentServer[0] == 0) && (m_bSwitchDns == false))
		{
			//m_bSwitchDns=true;
			//m_wSwitchIndex=INVALID_WORD;
			//continue;
			//不走域名
			break;
		}

		//结果判断
		if ((m_szCurrentServer[0] == 0) && (m_bSwitchDns == true)) break;

		//相同判断
		if ((m_bSwitchDns == false) && (lstrcmp(m_szCurrentServer, szCurrentServer) == 0)) continue;

		//获取信息
		if (GetServerInfo() == true) return true;

	} while (true);

	//尝试默认
	if (m_bTryDefault == false)
	{
		//设置变量 
		m_bTryDefault = true;
		StringCchCopy(m_szCurrentServer, CountArray(m_szCurrentServer), szLogonServer);

		//获取信息
		if (GetServerInfo() == true) return true;
	}

	return false;
}

//地址解释
DWORD CVirtualLinkManager::TranslateAddress(LPCTSTR szServerIP)
{
	//转化地址
	CT2CA ServerAddr(szServerIP);
	DWORD dwServerIP = inet_addr(ServerAddr);

	//域名解释
	if (dwServerIP == INADDR_NONE)
	{
		LPHOSTENT lpHost = gethostbyname(ServerAddr);
		if (lpHost == NULL) return INADDR_NONE;
		dwServerIP = ((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}

	return dwServerIP;
}

//////////////////////////////////////////////////////////////////////////