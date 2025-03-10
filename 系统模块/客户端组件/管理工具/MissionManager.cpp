#include "StdAfx.h"
#include "MissionManager.h"

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

BEGIN_MESSAGE_MAP(CMissionManager, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CMissionSocket::CMissionSocket()
{
	//当前连接
	m_wCurrentPort=PORT_LOGON;
	m_dwCurrentServer=INADDR_NONE;

	//接口变量
	m_pIMissionSocketSink=NULL;

	return;
}

//析构函数
CMissionSocket::~CMissionSocket()
{
}

//接口查询
VOID * __cdecl CMissionSocket::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITCPSocketSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITCPSocketSink,Guid,dwQueryVer);
	return NULL;
}

//连接事件
bool __cdecl CMissionSocket::OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode)
{
	//连接通知
	ASSERT(m_pIMissionSocketSink!=NULL);
	if (m_pIMissionSocketSink!=NULL) m_pIMissionSocketSink->OnEventMissionLink(nErrorCode);

	return true;
}

//关闭事件
bool __cdecl CMissionSocket::OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason)
{
	//关闭通知
	ASSERT(m_pIMissionSocketSink!=NULL);
	if (m_pIMissionSocketSink!=NULL) m_pIMissionSocketSink->OnEventMissionShut(cbShutReason);

	return true;
}

//读取事件
bool __cdecl CMissionSocket::OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID * pData, WORD wDataSize)
{
	//读取通知
	ASSERT(m_pIMissionSocketSink!=NULL);
	if (m_pIMissionSocketSink!=NULL) m_pIMissionSocketSink->OnEventMissionRead(Command,pData,wDataSize);

	return true;
}

//网络状态
BYTE CMissionSocket::GetSocketStatus()
{
	//变量定义
	BYTE cbSocketStatus=SOCKET_STATUS_IDLE;

	//获取状态
	if (m_TCPSocketModule.GetInterface()!=NULL)
	{
		cbSocketStatus=m_TCPSocketModule->GetSocketStatus();
	}

	return cbSocketStatus;
}

//设置连接信息
VOID CMissionSocket::SetConnectInfo( DWORD dwServerIP, WORD wPort )
{
	m_dwCurrentServer = dwServerIP;
	m_wCurrentPort = wPort;
}

//中断连接
bool CMissionSocket::PerformClose()
{
	//关闭连接
	ASSERT(m_TCPSocketModule.GetInterface()!=NULL);
	if (m_TCPSocketModule.GetInterface()!=NULL) m_TCPSocketModule->CloseSocket();

	return true;
}

//发起连接
bool CMissionSocket::PerformConnect()
{
	//状态效验
	ASSERT(m_dwCurrentServer!=INADDR_NONE);
	if( m_dwCurrentServer == INADDR_NONE ) return false;

	//创建组件
	if (m_TCPSocketModule.GetInterface()==NULL)
	{
		if (m_TCPSocketModule.CreateInstance()==false) return false;
		if (m_TCPSocketModule->SetTCPSocketSink(QUERY_ME_INTERFACE(IUnknownEx))==false) return false;
	}

	//设置代理
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();
	enProxyServerType ProxyServerType=pParameterGlobal->m_ProxyServerType;
	tagProxyServerInfo & ProxyServerInfo=pParameterGlobal->m_ProxyServerInfo;
	if (m_TCPSocketModule->SetProxyServerInfo(ProxyServerType,ProxyServerInfo)==false) return false;

	//发起连接
	if (m_TCPSocketModule->Connect(m_dwCurrentServer,m_wCurrentPort)!=CONNECT_SUCCESS) return false;

	return true;
}

//发送函数
bool CMissionSocket::PerformSendData(WORD wMainCmdID, WORD wSubCmdID)
{
	//状态效验
	ASSERT(GetSocketStatus()==SOCKET_STATUS_CONNECT);
	if (GetSocketStatus()!=SOCKET_STATUS_CONNECT) return false;

	//发送数据
	ASSERT(m_TCPSocketModule.GetInterface()!=NULL);
	if (m_TCPSocketModule.GetInterface()!=NULL) m_TCPSocketModule->SendData(wMainCmdID,wSubCmdID);

	return true;
}

//发送函数
bool CMissionSocket::PerformSendData(WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize)
{
	//状态效验
	ASSERT(GetSocketStatus()==SOCKET_STATUS_CONNECT);
	if (GetSocketStatus()!=SOCKET_STATUS_CONNECT) return false;

	//发送数据
	ASSERT(m_TCPSocketModule.GetInterface()!=NULL);
	if (m_TCPSocketModule.GetInterface()!=NULL) m_TCPSocketModule->SendData(wMainCmdID,wSubCmdID,pData,wDataSize);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CMissionItem::CMissionItem()
{
	//设置变量
	m_bActiveStatus=false;

	return;
}

//析构函数
CMissionItem::~CMissionItem()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CMissionManager::CMissionManager()
{
	//状态变量
	m_bSwitchDns=false;
	m_bTryDefault=false;
	m_bTryRedirect = false;
	m_wSwitchIndex=INVALID_WORD;

	//当前连接
	m_wCurrentPort=PORT_LOGON;
	m_dwCurrentServer=INADDR_NONE;

	//地址信息
	m_szCurrentServer[0]=0;

	//设置组件
	m_MissionSocket.SetMissionSocketSink(this);
	
	return;
}

//析构函数
CMissionManager::~CMissionManager()
{
}

//接口查询
VOID * __cdecl CMissionManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IRedirectServiceSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IRedirectServiceSink,Guid,dwQueryVer);
	return NULL;
}

//连接事件
bool CMissionManager::OnEventMissionLink(INT nErrorCode)
{
	//切换连接
	if ((nErrorCode==WSAETIMEDOUT)||(nErrorCode==WSAECONNREFUSED)||(nErrorCode==WSAEADDRNOTAVAIL))
	{
		//切换连接
		if (!SwitchServerItem())
		{
			//重定向
			if( RedirectionServer() ) return true;
		}
		else
		{
			m_MissionSocket.SetConnectInfo(m_dwCurrentServer,m_wCurrentPort);
			return m_MissionSocket.PerformConnect();
		}
	}

	//连接通知
	for (INT_PTR i=0;i<m_MissionItemArray.GetCount();i++)
	{
		//获取子项
		CMissionItem * pMissionItem=m_MissionItemArray[i];

		//事件处理
		if (pMissionItem->m_bActiveStatus==true)
		{
			//激活状态
			if (nErrorCode!=0L)
			{
				pMissionItem->m_bActiveStatus=false;
			}
			else
			{
				//创建虚拟链接
				CreateVirtualLink();
			}

			//事件通知
			pMissionItem->OnEventMissionLink(nErrorCode);
		}
	}

	return true;
}

//关闭事件
bool CMissionManager::OnEventMissionShut(BYTE cbShutReason)
{
	//关闭通知
	for (INT_PTR i=0;i<m_MissionItemArray.GetCount();i++)
	{
		//获取子项
		CMissionItem * pMissionItem=m_MissionItemArray[i];

		//事件处理
		if (pMissionItem->m_bActiveStatus==true)
		{
			pMissionItem->m_bActiveStatus=false;
			pMissionItem->OnEventMissionShut(cbShutReason);
		}
	}

	return true;
}

//读取事件
bool CMissionManager::OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize)
{
	//数据通知
	for (INT_PTR i=0;i<m_MissionItemArray.GetCount();i++)
	{
		CMissionItem * pMissionItem=m_MissionItemArray[i];
		if (pMissionItem->m_bActiveStatus == true)
			pMissionItem->OnEventMissionRead(Command, pData, wDataSize);
	}

	return true;
}

////下载完成
//bool __cdecl CMissionManager::OnRedirectComplete(LPCTSTR pszServerIP, int nParam)
//{
//	//设置变量
//	lstrcpyn( m_szCurrentServer,pszServerIP,CountArray(m_szCurrentServer) );
//
//	//提取地址
//	bool bSucc = GetServerInfo();
//
//	//设置地址
//	if( bSucc )
//		m_MissionSocket.SetConnectInfo(m_dwCurrentServer,m_wCurrentPort);
//
//	//发起连接
//	if ( !bSucc || m_MissionSocket.PerformConnect()==false )
//	{
//		//关闭通知
//		for (INT_PTR i=0;i<m_MissionItemArray.GetCount();i++)
//		{
//			//获取子项
//			CMissionItem * pMissionItem=m_MissionItemArray[i];
//
//			//事件处理
//			if (pMissionItem->m_bActiveStatus==true)
//			{
//				pMissionItem->m_bActiveStatus=false;
//				pMissionItem->OnEventMissionShut(SHUT_REASON_EXCEPTION);
//			}
//		}
//
//		return true;
//	}
//
//	//保存最后登录
//	if( m_szCurrentServer[0] != 0 )
//	{
//		CGlobalServer GlobalServer;
//		GlobalServer.SetLastServerName(m_szCurrentServer);
//	}
//
//	return true;
//}
//
////下载异常
//bool __cdecl CMissionManager::OnRedirectException(enDownLoadError DownLoadError)
//{
//	//关闭通知
//	for (INT_PTR i=0;i<m_MissionItemArray.GetCount();i++)
//	{
//		//获取子项
//		CMissionItem * pMissionItem=m_MissionItemArray[i];
//
//		//事件处理
//		if (pMissionItem->m_bActiveStatus==true)
//		{
//			pMissionItem->m_bActiveStatus=false;
//			pMissionItem->OnEventMissionShut(SHUT_REASON_EXCEPTION);
//		}
//	}
//
//	return true;
//}

//设置地址
VOID CMissionManager::SetCurrentServer(LPCTSTR pszCurrentServer)
{
	//设置地址
	lstrcpyn( m_szCurrentServer,pszCurrentServer,CountArray(m_szCurrentServer) );

	return;
}

//加入任务
bool CMissionManager::InsertMissionItem(CMissionItem * pMissionItem)
{
	//查找现有
	for (INT_PTR i=0;i<m_MissionItemArray.GetCount();i++)
	{
		if (m_MissionItemArray[i]==pMissionItem)
		{
			ASSERT(FALSE);
			return true;
		}
	}

	//设置子项
	pMissionItem->m_bActiveStatus=false;
	pMissionItem->m_pMissionManager=this;

	//加入任务
	m_MissionItemArray.Add(pMissionItem);

	return true;
}

//删除任务
bool CMissionManager::DeleteMissionItem(CMissionItem * pMissionItem)
{
	//终止任务
	if (pMissionItem->m_bActiveStatus==true)
	{
		ConcludeMissionItem(pMissionItem);
	}

	//删除任务
	for (INT_PTR i=0;i<m_MissionItemArray.GetCount();i++)
	{
		if (m_MissionItemArray[i]==pMissionItem)
		{
			//删除数组
			m_MissionItemArray.RemoveAt(i);

			//设置子项
			pMissionItem->m_bActiveStatus=false;
			pMissionItem->m_pMissionManager=NULL;

			return true;
		}
	}

	return false;
}

//激活任务
bool CMissionManager::AvtiveMissionItem(CMissionItem * pMissionItem, bool bContinue)
{
	//效验状态
	ASSERT(pMissionItem->m_pMissionManager==this);
	if (pMissionItem->m_pMissionManager!=this) return false;

	//关闭窗口
	if (m_hWnd!=NULL) DestroyWindow();

	//设置任务
	pMissionItem->m_bActiveStatus=true;

	//连接处理
	switch (m_MissionSocket.GetSocketStatus())
	{
	case SOCKET_STATUS_IDLE:	//空闲状态
		{
			if( bContinue==false )
			{
				//设置变量
				m_bSwitchDns=false;
				m_bTryDefault=true;						//不连接默认域名
				m_bTryRedirect = false;
				m_wSwitchIndex=INVALID_WORD;

				//读取地址
				if (m_szCurrentServer[0]==0)
				{
					CGlobalServer GlobalServer;
					GlobalServer.GetLastServerName(m_szCurrentServer);
				}

				//设置变量
				m_wCurrentPort=PORT_LOGON;

				//默认地址
				if ((GetServerInfo()==false)&&(SwitchServerItem()==false))
				{
					ASSERT(FALSE);
					pMissionItem->m_bActiveStatus = false;
					return false;
				}
			}
			else
			{
				//切换连接
				if (SwitchServerItem()==false)
				{
					//尝试重定向
					if( RedirectionServer() ) return true;

					pMissionItem->m_bActiveStatus = false;
					return false;
				}
			}

			m_MissionSocket.SetConnectInfo(m_dwCurrentServer,m_wCurrentPort);
			//发起连接
			if (m_MissionSocket.PerformConnect()==false)
			{
				//设置任务
				pMissionItem->m_bActiveStatus=false;

				return false;
			}

			if( m_szCurrentServer[0] != 0 )
			{
				CGlobalServer GlobalServer;
				GlobalServer.SetLastServerName(m_szCurrentServer);
			}

			break;
		}
	case SOCKET_STATUS_CONNECT:	//连接状态
		{
			//事件通知
			pMissionItem->OnEventMissionLink(0L);

			break;
		}
	}

	return true;
}

//终止任务
bool CMissionManager::ConcludeMissionItem(CMissionItem * pMissionItem)
{
	//效验状态
	ASSERT(pMissionItem->m_bActiveStatus==true);
	if (pMissionItem->m_bActiveStatus==false) return false;

	//设置任务
	pMissionItem->m_bActiveStatus=false;

	//变量定义
	UINT uActiveCount=0;
	BYTE cbSocketStatus=m_MissionSocket.GetSocketStatus();

	//搜索子项
	for (INT_PTR i=0;i<m_MissionItemArray.GetCount();i++)
	{
		CMissionItem * pMissionItem=m_MissionItemArray[i];
		if (pMissionItem->m_bActiveStatus==true) uActiveCount++;
	}

	//终止连接
	if ((uActiveCount==0)&&(cbSocketStatus!=SOCKET_STATUS_IDLE))
	{
		//变量定义
		CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();

		//关闭判断
		if (pParameterGlobal->m_wIntermitTime>0)
		{
			//创建窗口
			if (m_hWnd==NULL)
			{
				CRect rcCreate(0,0,0,0);
				Create(NULL,NULL,WS_CHILD,rcCreate,GetDesktopWindow(),100);
			}

			//设置时间
			SetTimer(IDI_CLOSE_LINK,pParameterGlobal->m_wIntermitTime*1000L,NULL);
		}
		else
		{
			//关闭连接
			m_MissionSocket.PerformClose();
		}
	}

	return false;
}

//发送函数
bool CMissionManager::SendData(WORD wMainCmdID, WORD wSubCmdID)
{
	//发送数据
	m_MissionSocket.PerformSendData(wMainCmdID,wSubCmdID);

	return true;
}

//发送函数
bool CMissionManager::SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize)
{
	//发送数据
	m_MissionSocket.PerformSendData(wMainCmdID,wSubCmdID,pData,wDataSize);

	return true;
}

//时间消息
VOID CMissionManager::OnTimer(UINT_PTR nIDEvent)
{
	//关闭连接
	if (nIDEvent==IDI_CLOSE_LINK)
	{
		//关闭窗口
		DestroyWindow();

		//终止连接
		m_MissionSocket.PerformClose();

		return;
	}

	__super::OnTimer(nIDEvent);
}

//地址信息
bool CMissionManager::GetServerInfo()
{
	//效验参数
	ASSERT(m_szCurrentServer[0]!=0);
	if (m_szCurrentServer[0]==0) return false;

	//变量定义
	tagServerItem ServerItem;
	ZeroMemory(&ServerItem,sizeof(ServerItem));

	//设置变量
	m_wCurrentPort=PORT_LOGON;
	m_dwCurrentServer=INADDR_NONE;

	//获取信息
	CGlobalServer GlobalServer;
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();
	bool bSuccess=GlobalServer.GetServerItemInfo(m_szCurrentServer,pParameterGlobal->m_NetworkKind,ServerItem);

	//转换地址
	if (bSuccess==true)
	{
		//使用域名
		if (m_bSwitchDns==true)
		{
			LPHOSTENT lpHost=gethostbyname(CT2CA(ServerItem.szServerHost));
			if (lpHost!=NULL) 
			{
				ServerItem.dwServerAddr=((LPIN_ADDR)lpHost->h_addr)->s_addr;

				//更新地址
				if( ServerItem.szServerHost[0] != 0 )
					GlobalServer.WriteServerItemInfo(m_szCurrentServer,pParameterGlobal->m_NetworkKind,ServerItem);
			}
		}

		//使用地址
		if ((m_bSwitchDns==false)&&(ServerItem.dwServerAddr==INADDR_NONE))
		{
			CT2CA strServerDomain(ServerItem.szServerHost);
			ServerItem.dwServerAddr=inet_addr(strServerDomain);
		}
	}

	//名字翻译
	if (ServerItem.dwServerAddr==INADDR_NONE)
	{
		//地址转换
		CT2CA CurrentServer(m_szCurrentServer);
		ServerItem.dwServerAddr=inet_addr(CurrentServer);

		//域名解释
		if (ServerItem.dwServerAddr==INADDR_NONE)
		{
			LPHOSTENT lpHost=gethostbyname(CurrentServer);
			if (lpHost!=NULL) ServerItem.dwServerAddr=((LPIN_ADDR)lpHost->h_addr)->s_addr;
		}
	}

	//结果处理
	if (ServerItem.dwServerAddr!=INADDR_NONE)
	{
		//设置变量
		m_wCurrentPort= ServerItem.wServerPort;
		m_dwCurrentServer=ServerItem.dwServerAddr;

		return true;
	}

	return false;
}

//切换连接
bool CMissionManager::SwitchServerItem()
{
	TCHAR szCurrentServer[LEN_SERVER] = TEXT("");
	lstrcpyn( szCurrentServer,m_szCurrentServer,CountArray(szCurrentServer) );

	do
	{
		//读取信息
		CGlobalServer GlobalServer;
		GlobalServer.GetServerItemName(++m_wSwitchIndex,m_szCurrentServer);

		//状态切换
		if ((m_szCurrentServer[0]==0)&&(m_bSwitchDns==false))
		{
			//m_bSwitchDns=true;
			//m_wSwitchIndex=INVALID_WORD;
			//continue;
			//不走域名
			break;
		}

		//结果判断
		if ((m_szCurrentServer[0]==0)&&(m_bSwitchDns==true)) break;

		//相同判断
		if ((m_bSwitchDns==false)&&(lstrcmp(m_szCurrentServer,szCurrentServer)==0)) continue;

		//获取信息
		if (GetServerInfo()==true) return true;

	} while (true);

	//尝试默认
	if (m_bTryDefault==false)
	{
		//设置变量 
		m_bTryDefault=true;
		lstrcpyn(m_szCurrentServer,szLogonServer,CountArray(m_szCurrentServer));

		//获取信息
		if (GetServerInfo()==true) return true;
	}

	return false;
}

//重新定向
bool CMissionManager::RedirectionServer()
{
	if( m_bTryRedirect ) return false;

	//设置变量
	m_bTryRedirect = true;

	//初始化定向服务
	if (m_RedirectServiceHelper.GetInterface()==NULL)
	{
		if( !m_RedirectServiceHelper.CreateInstance() ) return false;

		m_RedirectServiceHelper->SetRedirectServiceSink(QUERY_ME_INTERFACE(IUnknownEx));
	}

	//重定向地址
	TCHAR szUrl[MAX_PATH] = {};
	CGlobalStation * pGlobalStation=CGlobalStation::GetInstance();
	pGlobalStation->GetRedirectionServer(1,szUrl,CountArray(szUrl));

	return m_RedirectServiceHelper->RequestRedirect(szUrl);
}

//创建虚拟链接
void CMissionManager::CreateVirtualLink()
{
	CMD_GG_C_LinkService linkService;
	ZeroMemory(&linkService, sizeof(linkService));

	//linkService.dwStationID = 1000;
	linkService.wServiceID = INVALID_WORD;
	linkService.wServiceModule = SERVICE_MODULE_LOGON;

	//发送数据
	SendData(MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_C_LINK_SERVICE, &linkService, sizeof(linkService));
}

//////////////////////////////////////////////////////////////////////////