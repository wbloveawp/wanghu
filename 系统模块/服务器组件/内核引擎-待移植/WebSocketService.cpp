#include "stdafx.h"
#include "WebSocketService.h"

//消息定义
#define WM_SOCKET_NOTIFY			WM_USER+100							//消息标识
#define WM_SERVICE_REQUEST			WM_USER+101							//服务请求

//////////////////////////////////////////////////////////////////////////
//动作定义

#define REQUEST_CONNECT				1									//请求连接
#define REQUEST_SEND_DATA			2									//请求发送
#define REQUEST_CLOSE_SOCKET		3									//请求关闭

//连接请求
struct tagConnectRequest
{
	WORD							wPort;								//连接端口
	TCHAR							szUrl[256];							//连接地址
};

//发送请求
struct tagSendDataRequest
{
	WORD							wDataSize;							//数据大小
	BYTE							cbSendBuffer[SOCKET_TCP_PACKET];	//数据缓存
};


//构造函数
CWebSocketServiceThread::CWebSocketServiceThread()
{
	//内核变量
	m_hWnd = NULL;
	m_hSocket = INVALID_SOCKET;
	m_TCPSocketStatus = SOCKET_STATUS_IDLE;

	//接收变量
	m_wRecvSize = 0;
	ZeroMemory(m_cbRecvBuf, sizeof(m_cbRecvBuf));

	//缓冲变量
	m_dwBufferData = 0L;
	m_dwBufferSize = 0L;
	m_bNeedBuffer = false;
	m_pcbDataBuffer = NULL;

}

bool CWebSocketServiceThread::ConcludeThread(DWORD dwWaitSeconds)
{
	//退出消息
	if (IsRuning() == true)
	{
		PostThreadMessage(WM_QUIT, 0, 0);
	}

	return __super::ConcludeThread(dwWaitSeconds);
}

//开始事件
bool CWebSocketServiceThread::OnEventThreadStrat()
{
	//变量定义
	WNDCLASS WndClass;
	ZeroMemory(&WndClass, sizeof(WndClass));

	//设置变量
	WndClass.lpfnWndProc = DefWindowProc;
	WndClass.hInstance = AfxGetInstanceHandle();
	WndClass.lpszClassName = TEXT("WebSocketStatusServiceThread");

	//注册窗口
	RegisterClass(&WndClass);

	//创建窗口
	HWND hParentWnd = GetDesktopWindow();
	m_hWnd = CreateWindow(WndClass.lpszClassName, NULL, WS_CHILD, 0, 0, 0, 0, hParentWnd, NULL, WndClass.hInstance, NULL);

	return true;
}

//停止事件
bool CWebSocketServiceThread::OnEventThreadConclude()
{
	//关闭窗口
	if (m_hWnd != NULL)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}

	//关闭连接
	PerformCloseSocket(SHUT_REASON_INSIDE);

	//清理队列
	CWHDataLocker ThreadLock(m_CriticalSection);
	m_DataQueue.RemoveData(false);

	return true;
}

//运行函数
bool CWebSocketServiceThread::OnEventThreadRun()
{
	//获取消息
	MSG Message;
	if (GetMessage(&Message, NULL, 0, 0) == FALSE) return false;

	//消息处理
	switch (Message.message)
	{
	case WM_SOCKET_NOTIFY:		//网络消息
	{
		OnSocketNotify(Message.wParam, Message.lParam);
		return true;
	}
	case WM_SERVICE_REQUEST:	//服务请求
	{
		OnServiceRequest(Message.wParam, Message.lParam);
		return true;
	}
	default:					//默认处理
	{
		DefWindowProc(Message.hwnd, Message.message, Message.wParam, Message.lParam);
		return true;
	}
	}

	return false;
}


bool CWebSocketServiceThread::PostThreadRequest(WORD wIdentifier, VOID * const pBuffer, WORD wDataSize)
{
	//投递请求
	CWHDataLocker ThreadLock(m_CriticalSection);
	m_DataQueue.InsertData(wIdentifier, pBuffer, wDataSize);

	//发送消息
	ASSERT(m_hWnd != NULL);
	if (m_hWnd != NULL) PostMessage(m_hWnd, WM_SERVICE_REQUEST, wDataSize, GetCurrentThreadId());

	return true;
}


//执行连接
DWORD CWebSocketServiceThread::PerformConnect(DWORD dwServerIP, WORD wPort)
{
	//效验参数
	ASSERT(m_hSocket == INVALID_SOCKET);
	ASSERT(m_TCPSocketStatus == SOCKET_STATUS_IDLE);
	ASSERT((dwServerIP != INADDR_NONE) && (dwServerIP != 0));

	try
	{
		//效验状态
		if (m_hSocket != INVALID_SOCKET) throw CONNECT_EXCEPTION;
		if (m_TCPSocketStatus != SOCKET_STATUS_IDLE) throw CONNECT_EXCEPTION;
		if ((dwServerIP == INADDR_NONE) || (dwServerIP == 0)) throw CONNECT_EXCEPTION;

		//设置参数
		m_wRecvSize = 0;

		//建立 SOCKET
		m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_hSocket == INVALID_SOCKET) throw CONNECT_EXCEPTION;

		//变量定义
		SOCKADDR_IN SocketAddr;
		ZeroMemory(&SocketAddr, sizeof(SocketAddr));

		//设置变量
		SocketAddr.sin_family = AF_INET;
		SocketAddr.sin_port = htons(wPort);
		SocketAddr.sin_addr.S_un.S_addr = dwServerIP;

		//绑定窗口
		WSAAsyncSelect(m_hSocket, m_hWnd, WM_SOCKET_NOTIFY, FD_READ | FD_CONNECT | FD_CLOSE | FD_WRITE);

		//连接服务器
		INT nErrorCode = connect(m_hSocket, (SOCKADDR *)&SocketAddr, sizeof(SocketAddr));
		if ((nErrorCode == SOCKET_ERROR) && (WSAGetLastError() != WSAEWOULDBLOCK)) throw CONNECT_EXCEPTION;

		//设置变量
		m_TCPSocketStatus = SOCKET_STATUS_WAIT;

		return CONNECT_SUCCESS;
	}
	catch (...)
	{
		//关闭连接
		PerformCloseSocket(SHUT_REASON_INSIDE);
	}

	return CONNECT_EXCEPTION;
}

//关闭连接
VOID CWebSocketServiceThread::PerformCloseSocket(BYTE cbShutReason)
{

	//关闭判断
	if (m_hSocket != INVALID_SOCKET)
	{
		//关闭连接
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;

		//关闭通知
		if (cbShutReason != SHUT_REASON_INSIDE)
		{
			CWebSocketService * pTCPSocketStatusService = CONTAINING_RECORD(this, CWebSocketService, m_WebSocketServiceThread);
			pTCPSocketStatusService->OnEventWEBSocketShut(cbShutReason);
		}
	}

	//内核变量
	m_wRecvSize = 0;  //14580
	m_dwBufferData = 0L;   //147556
	m_bNeedBuffer = false;  //147554
	m_TCPSocketStatus = SOCKET_STATUS_IDLE; //147558

}

//发送数据
DWORD CWebSocketServiceThread::SendBuffer(VOID * pBuffer, WORD wSendSize)
{
	//变量定义
	WORD wTotalCount = 0;

	if (pBuffer && wSendSize && m_hSocket != INVALID_SOCKET && m_TCPSocketStatus == SOCKET_STATUS_CONNECT)
	{ 

		//发送数据
		while ((m_bNeedBuffer == false) && (wTotalCount < wSendSize))
		{
			//发生数据
			INT nSendCount = send(m_hSocket, (char *)pBuffer + wTotalCount, wSendSize - wTotalCount, 0);

			//错误判断
			if (nSendCount == SOCKET_ERROR)
			{
				//缓冲判断
				if (WSAGetLastError() == WSAEWOULDBLOCK)
				{
					AmortizeBuffer((LPBYTE)pBuffer + wTotalCount, wSendSize - wTotalCount);
					return wSendSize;
				}

				//关闭连接
				PerformCloseSocket(SHUT_REASON_EXCEPTION);

				return 0L;
			}
			else
			{
				//设置变量
				wTotalCount += nSendCount;
			}
		}
	}

	//缓冲数据
	if (wTotalCount < wSendSize)
	{
		AmortizeBuffer((LPBYTE)pBuffer + wTotalCount, wSendSize - wTotalCount);
	}

	return wSendSize;
}

//网络消息
LRESULT CWebSocketServiceThread::OnSocketNotify(WPARAM wParam, LPARAM lParam)
{
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:		//数据读取
	{
		return OnSocketNotifyRead(wParam, lParam);
	}
	case FD_WRITE:		//网络发送
	{
		return OnSocketNotifyWrite(wParam, lParam);
	}
	case FD_CLOSE:		//网络关闭
	{
		return OnSocketNotifyClose(wParam, lParam);
	}
	case FD_CONNECT:	//网络连接
	{
		return OnSocketNotifyConnect(wParam, lParam);
	}
	}

	return 0;
}

//网络读取
LRESULT CWebSocketServiceThread::OnSocketNotifyRead(WPARAM wParam, LPARAM lParam)
{
	try
	{
		if (m_bFlag)
		{
			BYTE pBuffer[4096];
			//读取数据
			INT iRetCode = recv(m_hSocket, (char *)pBuffer, sizeof(pBuffer), 0);
			if (iRetCode == SOCKET_ERROR) throw TEXT("网络连接关闭，读取数据失败");

			//处理数据
			if (!true)
			{
				throw TEXT("未取得合法授权，服务器将以受限模式运行！");
			}

		}
		else
		{
			//读取数据
			INT iRetCode = recv(m_hSocket, (char *)m_cbRecvBuf + m_wRecvSize, sizeof(m_cbRecvBuf) - m_wRecvSize, 0);

			if (iRetCode == SOCKET_ERROR) throw TEXT("网络连接关闭，读取数据失败");

			m_wRecvSize += iRetCode;

			//判断头部是否接收完整
			const char * cbBuffer = strstr((const char*)m_cbRecvBuf, "\r\n\r\n");
			if (cbBuffer)
			{
				m_bFlag = 1;		//头部接受完成
				m_wRecvSize = 0;

				char oneByte;
				for (int i = 0; i < m_wRecvSize; i++)
				{
					oneByte = m_cbRecvBuf[i];

					if (oneByte == 32)	//空格
					{
						do
						{
							++i;

						} while (oneByte != 32);
					}
				}
			}
		}
	}
	catch (...)
	{
		//关闭连接
		PerformCloseSocket(true);
	}

	return 1;
}

//网络发送  
LRESULT CWebSocketServiceThread::OnSocketNotifyWrite(WPARAM wParam, LPARAM lParam)
{
	if ((m_bNeedBuffer == true) && (m_dwBufferData>0L))
	{
		//变量定义
		DWORD dwTotalCount = 0;
		DWORD dwPacketSize = 4096;


		//发送数据
		while (dwTotalCount<m_dwBufferData)
		{
			//发生数据
			WORD wSendSize = (WORD)__min(dwPacketSize, m_dwBufferData - dwTotalCount);
			INT nSendCount = send(m_hSocket, (char *)m_pcbDataBuffer + dwTotalCount, wSendSize, 0);

			//错误判断
			if (nSendCount == SOCKET_ERROR)
			{
				//缓冲判断
				if (WSAGetLastError() == WSAEWOULDBLOCK)
				{
					//设置变量
					m_bNeedBuffer = false;
					m_dwBufferData -= dwTotalCount;

					//移动内存
					if (m_dwBufferData>0L)
					{
						m_bNeedBuffer = true;
						MoveMemory(m_pcbDataBuffer, m_pcbDataBuffer + dwTotalCount, m_dwBufferData);
					}

					return 1L;
				}

				//关闭连接
				PerformCloseSocket(SHUT_REASON_EXCEPTION);

				return 1L;
			}

			//设置变量
			dwTotalCount += nSendCount;
		}

		//设置变量
		m_dwBufferData = 0L;
		m_bNeedBuffer = false;
	}

	return 1L;
}

//网络关闭
LRESULT CWebSocketServiceThread::OnSocketNotifyClose(WPARAM wParam, LPARAM lParam)
{
	//关闭连接
	PerformCloseSocket(true);

	return 1;
}

//网络连接
LRESULT CWebSocketServiceThread::OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam)
{
	//获取错误
	INT nErrorCode = WSAGETSELECTERROR(lParam);

	//事件通知
	CWebSocketService * pWebSocketStatusService = CONTAINING_RECORD(this, CWebSocketService, m_WebSocketServiceThread);
	pWebSocketStatusService->OnEventWEBSocketLink(nErrorCode);

	//关闭判断
	if (nErrorCode != 0)
	{
		PerformCloseSocket(SHUT_REASON_INSIDE);
		return 0;
	}

	//设置状态
	m_TCPSocketStatus = SOCKET_STATUS_CONNECT;

	return 1;
}

//请求消息
LRESULT CWebSocketServiceThread::OnServiceRequest(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	tagDataHead DataHead;
	CWHDataLocker ThreadLock(m_CriticalSection);

	//提取数据
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
	if (m_DataQueue.DistillData(DataHead, cbBuffer, sizeof(cbBuffer)) == false) return 0;

	//数据处理
	switch (DataHead.wIdentifier)
	{
	case REQUEST_CONNECT:		//连接请求
	{
		//效验数据
		ASSERT(DataHead.wDataSize == sizeof(tagConnectRequest));
		tagConnectRequest * pConnectRequest = (tagConnectRequest *)cbBuffer;

		//数据处理
		if (PerformConnect(pConnectRequest->szUrl, pConnectRequest->wPort) != CONNECT_SUCCESS)
		{
			//事件通知
			CWebSocketService * pWebSocketStatusService = CONTAINING_RECORD(this, CWebSocketService, m_WebSocketServiceThread);
			pWebSocketStatusService->OnEventWEBSocketLink(WSAEADDRNOTAVAIL);
		}

		return 1;
	}
	case REQUEST_SEND_DATA:		//发送请求
	{
		//效验数据
		ASSERT(DataHead.wDataSize == sizeof(tagSendDataRequest));
		tagSendDataRequest * pSendDataRequest = (tagSendDataRequest *)cbBuffer;

		//数据处理
		SendBuffer(pSendDataRequest->cbSendBuffer, pSendDataRequest->wDataSize);

		return 1;
	}

	case REQUEST_CLOSE_SOCKET:	//关闭请求
	{
		//关闭连接
		PerformCloseSocket(true);

		return 1;
	}
	}

	return 0;
}




//构造函数
CWebSocketService::CWebSocketService()
{
	//内核变量
	m_wServiceID = 0;
	m_bService = false;
	m_pIWebSocketEvent = NULL;
}


CWebSocketService::~CWebSocketService()
{
	ConcludeService();
}

VOID * CWebSocketService::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServiceModule, Guid, dwQueryVer);
	QUERYINTERFACE(IWEBSocketService, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IWEBSocketService, Guid, dwQueryVer);
	return NULL;
}

bool CWebSocketService::StartService()
{
	//运行判断
	ASSERT((m_bService == false) && (m_pIWebSocketEvent != NULL));
	if ((m_bService == true) || (m_pIWebSocketEvent == NULL)) return false;

	//服务线程
	if (m_WebSocketServiceThread.StartThread() == false) return false;

	//设置变量
	m_bService = true;

	return true;
}
//停止服务
bool CWebSocketService::ConcludeService()
{
	//设置变量
	m_bService = false;

	//停止线程
	m_WebSocketServiceThread.ConcludeThread(INFINITE);

	return true;
}
bool CWebSocketService::SetServiceID(WORD wServiceID)
{
	//状态效验
	ASSERT(m_bService == false);
	if (m_bService == true) return false;

	//设置变量
	m_wServiceID = wServiceID;

	return true;
}
//设置接口
bool CWebSocketService::SetWEBSocketEvent(IUnknownEx * pIUnknownEx)
{
	//状态效验
	ASSERT(m_bService == false);
	if (m_bService == true) return false;

	//查询接口
	m_pIWebSocketEvent = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IWEBSocketEvent);

	//错误判断
	if (m_pIWebSocketEvent == NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//关闭连接
bool CWebSocketService::CloseSocket(BYTE cbShutReason)
{
	//状态效验
	ASSERT(m_bService == true);
	if (m_bService == false) return false;

	//投递请求
	return m_WebSocketServiceThread.PostThreadRequest(REQUEST_CLOSE_SOCKET, &cbShutReason, 1);
}

//连接操作
bool CWebSocketService::ConnectServer(LPCTSTR pszURL, WORD wPort)
{
	if (m_bService) return false;

	tagConnectRequest	connentRequest;
	connentRequest.wPort = wPort;
	lstrcpy(connentRequest.szUrl, pszURL);

	return m_WebSocketServiceThread.PostThreadRequest(REQUEST_CONNECT, &connentRequest, sizeof(connentRequest));
	return true;
}

//发送请求
bool CWebSocketService::SendRequestData(VOID * pData, WORD wDataSize)
{
	if (m_bService) return false;

	tagSendDataRequest SendRequest;

	ZeroMemory(&SendRequest, sizeof(SendRequest));
	SendRequest.wDataSize = wDataSize;

	if (wDataSize > 0)
	{
		CopyMemory(SendRequest.cbSendBuffer, pData, wDataSize);
	}

	m_WebSocketServiceThread.PostThreadRequest(REQUEST_SEND_DATA, SendRequest.cbSendBuffer, SendRequest.wDataSize);

	return true;
}

bool CWebSocketService::OnEventWEBSocketLink(WORD wServiceID, WORD wRequestID, INT nErrorCode)
{
	return true;
}
//关闭消息
bool  CWebSocketService::OnEventWEBSocketShut(WORD wServiceID, WORD wRequestID, BYTE cbShutReason)
{
	return true;
}
//数据包流
bool  CWebSocketService::OnEventWEBSocketMain(WORD wServiceID, WORD wRequestID, VOID * pcbMailData, WORD wStreamSize)
{
	return true;
}

//数据包头
bool  CWebSocketService::OnEventWEBSocketHead(WORD wServiceID, WORD wRequestID, VOID * pcbHeadData, WORD wHeadSize, INT nStatusCode)
{
	if (nStatusCode == 200)
	{
		return true;
	}

	//错误码
	if (nStatusCode <= 300 || nStatusCode > 303)
	{
		return false;
	}
	else   //重定向
	{
		const char* szLocation = strstr((const char*)pcbHeadData, "Location: ");
		if (!szLocation)
		{
			//CloseSocket(SHUT_REASON_INSIDE);
		}
	}
	return true;
}

DECLARE_CREATE_MODULE(WebSocketService);