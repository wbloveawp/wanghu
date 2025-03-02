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

//构造函数
CVirtualLinkItem::CVirtualLinkItem()
{
	//状态变量
	m_bLinkStatus=false;

	//设置变量	
	m_wServiceID=0;
	m_wServiceModule=0;
	m_pVirtualLinkManager=NULL;

	return;
}

//析构函数
CVirtualLinkItem::~CVirtualLinkItem()
{
}

//发送数据
bool CVirtualLinkItem::SendVirtualData(WORD wMainCmdID, WORD wSubCmdID)
{
	//校验状态
	ASSERT (m_wServiceID!=0 && m_wServiceModule!=0);
	if (m_wServiceID == 0 || m_wServiceModule==0) return false;

	return m_pVirtualLinkManager->SendSocketData(MAKECMMD(wMainCmdID, m_wServiceModule), wSubCmdID, NULL, 0);
}

//发送数据
bool CVirtualLinkItem::SendVirtualData(WORD wMainCmdID, WORD wSubCmdID, VOID* const pData, WORD wDataSize)
{
	//校验状态
	ASSERT(m_wServiceID != 0 && m_wServiceModule != 0);
	if (m_wServiceID == 0 || m_wServiceModule == 0) return false;

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
	m_bLinkStatus = false;
}

//创建连接
bool CVirtualLinkItem::CreateVirtualLink(WORD wServiceID, DWORD dwUserID, LPCTSTR pszLogonToken, BYTE cbExtendData[], WORD wDataSize)
{
	//构造结构
	CMD_GG_C_LinkService LinkService;
	ZeroMemory(&LinkService,sizeof(LinkService));

	//模块信息
	LinkService.wServiceID = wServiceID;
	LinkService.wServiceModule = m_wServiceModule;

	//用户信息
	LinkService.dwUserID = dwUserID;

	//登录令牌
	if (pszLogonToken != NULL)
	{
		StringCchCopy(LinkService.szLogonToken, CountArray(LinkService.szLogonToken), pszLogonToken);
	}	

	//扩展数据
	if (wDataSize>0 && wDataSize<=CountArray(LinkService.cbLinkExtend))
	{
		CopyMemory(LinkService.cbLinkExtend, cbExtendData, wDataSize);
	}	

	return m_pVirtualLinkManager->SendSocketData(MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_C_LINK_SERVICE, &LinkService, sizeof(LinkService));
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CVirtualLinkManager::CVirtualLinkManager()
{
	//连接信息
	m_wServerPort=0;
	m_dwServerAddr=INADDR_NONE;

	return;
}

//析构函数
CVirtualLinkManager::~CVirtualLinkManager()
{
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
	//事件通知
	ASSERT(m_pIVirtualManagerSink != NULL);
	m_pIVirtualManagerSink->OnEventTCPSocketLink(nErrorCode);

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
		if (pVirtualLinkItem->m_bLinkStatus ==true)
		{
			pVirtualLinkItem->m_bLinkStatus =false;
			pVirtualLinkItem->OnEventVirtualShut();
		}
	}

	//事件通知
	ASSERT(m_pIVirtualManagerSink != NULL);
	m_pIVirtualManagerSink->OnEventTCPSocketShut(cbShutReason);

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

			//获取链接
			auto pVirtualLinkItem = m_VirtualLinkArray.GetAt(pLinkSuccess->wServiceModule-1);
			if (pVirtualLinkItem != NULL)
			{
				//设置状态
				pVirtualLinkItem->m_bLinkStatus = true;
				pVirtualLinkItem->m_wServiceID = pLinkSuccess->wServiceID;

				//链接通知
				pVirtualLinkItem->OnEventVirtualLink(0);
			}

			return true;
		}
		
		//链接失败
		if (Command.wSubCmdID == SUB_GG_S_LINK_FAILURE)
		{
			//提取数据
			auto pLinkFailure = (CMD_GG_S_LinkFailure*)pData;

			//获取链接
			auto pVirtualLinkItem = m_VirtualLinkArray.GetAt(pLinkFailure->wServiceModule-1);
			if (pVirtualLinkItem != NULL)
			{
				//链接通知
				pVirtualLinkItem->OnEventVirtualLink(pLinkFailure->lErrorCode);
			}

			return true;
		}

		//链接断开
		if (Command.wSubCmdID == SUB_GG_S_LINK_SHUTDOWN)
		{
			//提取数据
			auto pLinkShutdown = (CMD_GG_S_LinkShutdown*)pData;

			//获取链接
			auto pVirtualLinkItem = m_VirtualLinkArray.GetAt(pLinkShutdown->wServiceModule-1);
			if (pVirtualLinkItem != NULL)
			{
				//设置状态
				pVirtualLinkItem->m_bLinkStatus = false;
				
				//关闭通知
				pVirtualLinkItem->OnEventVirtualShut();

				//重置标识
				pVirtualLinkItem->m_wServiceID = 0;
			}

			return true;
		}

		return true;
	}

	//获取链接
	auto pVirtualLinkItem = m_VirtualLinkArray.GetAt(wServiceModule-1);
	if (pVirtualLinkItem != NULL && pVirtualLinkItem->m_bLinkStatus==true)
	{
		pVirtualLinkItem->OnEventVirtualRead(Command,pData,wDataSize);
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
	pVirtualLinkItem->m_bLinkStatus=false;
	pVirtualLinkItem->m_pVirtualLinkManager=this;

	//设置子项
	m_VirtualLinkArray[wServiceModule-1]=pVirtualLinkItem;

	return true;
}

//删除任务
bool CVirtualLinkManager::DeleteVirtualLink(CVirtualLinkItem * pVirtualLinkItem)
{
	//关闭链接
	if (pVirtualLinkItem->m_bLinkStatus==true)
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

//设置接口
VOID CVirtualLinkManager::SetVirtualManagerSink(IVirtualManagerSink* pIVirtualManagerSink)
{
	m_pIVirtualManagerSink = pIVirtualManagerSink;
}

//中断连接
bool CVirtualLinkManager::PerformClose()
{
	//关闭连接
	ASSERT(m_TCPSocketModule.GetInterface() != NULL);
	if (m_TCPSocketModule.GetInterface() != NULL) m_TCPSocketModule->CloseSocket();

	return true;
}

//发起连接
bool CVirtualLinkManager::PerformConnect(LPCTSTR pszServerAddr, WORD wServerPort)
{
	//状态效验
	ASSERT(pszServerAddr != NULL);
	if (pszServerAddr == NULL) return false;

	//设置变量
	m_wServerPort = wServerPort;
	m_dwServerAddr = TranslateAddress(pszServerAddr);

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
	if (m_TCPSocketModule->Connect(m_dwServerAddr, m_wServerPort) != CONNECT_SUCCESS) return false;

	return true;
}

//发送函数
bool CVirtualLinkManager::SendSocketData(WORD wMainCmdID, WORD wSubCmdID)
{
	//状态效验
	ASSERT(GetSocketStatus() == SOCKET_STATUS_CONNECT);
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
	ASSERT(GetSocketStatus() == SOCKET_STATUS_CONNECT);
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
		PerformClose();

		return;
	}

	__super::OnTimer(nIDEvent);
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