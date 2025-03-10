#include "Stdafx.h"
#include "TCPSocketService.h"
#include "TraceServiceManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////

//消息定义
#define WM_SOCKET_NOTIFY			WM_USER+100							//消息标识
#define WM_SERVICE_REQUEST			WM_USER+101							//服务请求

//////////////////////////////////////////////////////////////////////////
//动作定义

#define REQUEST_CONNECT				1									//请求连接
#define REQUEST_SEND_DATA			2									//请求发送
#define REQUEST_SEND_DATA_EX		3									//请求发送
#define REQUEST_CLOSE_SOCKET		4									//请求关闭

struct tagCrevasseBufferResult
{
	WORD							wDataSize;							//连接端口
	DWORD							wSubCmdID;							//连接地址
};

//连接请求
struct tagConnectRequest
{
	WORD							wPort;								//连接端口
	DWORD							dwServerIP;							//连接地址
};

//发送请求
struct tagSendDataRequest
{
	WORD							wMainCmdID;							//主命令码
	WORD							wSubCmdID;							//子命令码
};

//发送请求
struct tagSendDataExRequest
{
	WORD							wMainCmdID;							//主命令码
	WORD							wSubCmdID;							//子命令码
	WORD							wDataSize;							//数据大小
	BYTE							cbSendBuffer[SOCKET_PACKET];		//发送缓冲
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CTCPSocketServiceThread::CTCPSocketServiceThread()
{
	//内核变量
	m_hWnd = NULL;
	m_hSocket = INVALID_SOCKET;
	m_TCPSocketStatus = EVENT_TCP_NETWORK_ACCEPT;

	//接收变量
	m_wRecvSize = 0;
	ZeroMemory(m_cbRecvBuf, sizeof(m_cbRecvBuf));

	//缓冲变量
	m_dwBufferData = 0L;
	m_dwBufferSize = 0L;
	m_bNeedBuffer = false;
	m_pcbDataBuffer = NULL;

	//设置客户端数据加密模式
	m_Cipher.SetDataKind(DATA_MASK_CRYPTED);

	return;
}

//析构函数
CTCPSocketServiceThread::~CTCPSocketServiceThread()
{
	//关闭连接
	if (m_hSocket!=INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket=INVALID_SOCKET;
	}

	//删除缓冲
	SafeDeleteArray(m_pcbDataBuffer);

	return;
}

//停止线程
bool CTCPSocketServiceThread::ConcludeThread(DWORD dwWaitSeconds)
{
	//退出消息
	if (IsRuning()==true)
	{
		PostThreadMessage(WM_QUIT,0,0);
	}

	return __super::ConcludeThread(dwWaitSeconds);
}

//投递请求
bool CTCPSocketServiceThread::PostThreadRequest(WORD wIdentifier, VOID * const pBuffer, WORD wDataSize)
{
	//投递请求
	CThreadLock ThreadLock(m_CriticalSection);
	m_DataQueue.InsertData(wIdentifier,pBuffer,wDataSize);
	//发送消息
	ASSERT(m_hWnd!=NULL);
	if (m_hWnd!=NULL) PostMessage(m_hWnd,WM_SERVICE_REQUEST,wDataSize,GetCurrentThreadId());

	return true;
}

//开始事件
bool CTCPSocketServiceThread::OnEventThreadStrat()
{
	//变量定义
	WNDCLASS WndClass;
	ZeroMemory(&WndClass,sizeof(WndClass));

	//设置变量
	WndClass.lpfnWndProc=DefWindowProc;
	WndClass.hInstance=AfxGetInstanceHandle();
	WndClass.lpszClassName=TEXT("TCPSocketStatusServiceThread");

	//注册窗口
	RegisterClass(&WndClass);

	//创建窗口
	HWND hParentWnd=GetDesktopWindow();
	m_hWnd=CreateWindow(WndClass.lpszClassName,NULL,WS_CHILD,0,0,0,0,hParentWnd,NULL,WndClass.hInstance,NULL);

	return true;
}

//停止事件
bool CTCPSocketServiceThread::OnEventThreadConclude()
{
	//关闭窗口
	if (m_hWnd!=NULL)
	{
		DestroyWindow(m_hWnd);
		m_hWnd=NULL;
	}

	//关闭连接
	PerformCloseSocket(SHUT_REASON_INSIDE);

	//清理队列
	CThreadLock ThreadLock(m_CriticalSection);
	m_DataQueue.RemoveData(false);

	return true;
}

//运行函数
bool CTCPSocketServiceThread::OnEventThreadRun()
{
	//获取消息
	MSG Message;
	if (GetMessage(&Message,NULL,0,0)==FALSE) return false;

	//消息处理
	switch (Message.message)
	{
	case WM_SOCKET_NOTIFY:		//网络消息
		{
			OnSocketNotify(Message.wParam,Message.lParam);
			return true;
		}
	case WM_SERVICE_REQUEST:	//服务请求
		{
			OnServiceRequest(Message.wParam,Message.lParam);
			return true;
		}
	default:					//默认处理
		{ 
			DefWindowProc(Message.hwnd,Message.message,Message.wParam,Message.lParam); 
			return true;
		}
	}

	return false;
}

//执行连接
DWORD CTCPSocketServiceThread::PerformConnect(DWORD dwServerIP, WORD wPort)
{
	//效验参数
	ASSERT(m_hSocket==INVALID_SOCKET);
	ASSERT(m_TCPSocketStatus==SOCKET_STATUS_IDLE);
	ASSERT((dwServerIP!=INADDR_NONE)&&(dwServerIP!=0));
	
	try
	{
		//效验状态
		if (m_hSocket!=INVALID_SOCKET) throw CONNECT_EXCEPTION;
		if (m_TCPSocketStatus!= EVENT_TCP_NETWORK_ACCEPT) throw CONNECT_EXCEPTION;
		if ((dwServerIP==INADDR_NONE)||(dwServerIP==0)) throw CONNECT_EXCEPTION;

		//设置参数
		m_wRecvSize=0;

		m_dwSendTickCount=GetTickCount()/1000L;
		m_dwRecvTickCount=GetTickCount()/1000L;

		//建立 SOCKET
		m_hSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if (m_hSocket==INVALID_SOCKET) throw CONNECT_EXCEPTION;

		//变量定义
		SOCKADDR_IN SocketAddr;
		ZeroMemory(&SocketAddr,sizeof(SocketAddr));

		//设置变量
		SocketAddr.sin_family=AF_INET;
		SocketAddr.sin_port=htons(wPort);
		SocketAddr.sin_addr.S_un.S_addr=dwServerIP;

		//绑定窗口
		WSAAsyncSelect(m_hSocket,m_hWnd,WM_SOCKET_NOTIFY,FD_READ|FD_CONNECT|FD_CLOSE|FD_WRITE);

		//连接服务器
		INT nErrorCode=connect(m_hSocket,(SOCKADDR *)&SocketAddr,sizeof(SocketAddr));
		if ((nErrorCode==SOCKET_ERROR)&&(WSAGetLastError()!=WSAEWOULDBLOCK)) throw CONNECT_EXCEPTION;

		//设置变量
		m_TCPSocketStatus= EVENT_TCP_NETWORK_READ;

		return CONNECT_SUCCESS;
	}
	catch (...)
	{
		//关闭连接
		PerformCloseSocket(SHUT_REASON_INSIDE);
	}

	return CONNECT_EXCEPTION;
}

//发送函数
bool CTCPSocketServiceThread::PerformSendData(WORD wMainCmdID, WORD wSubCmdID)
{
	//加密数据 发送数据
	if (m_hSocket != INVALID_SOCKET
		&& m_TCPSocketStatus == EVENT_TCP_SOCKET_CONNECT
		&& m_Cipher.EncryptBuffer(wMainCmdID, wSubCmdID, NULL, 0))
	{
		return SendBuffer(m_Cipher.GetData(), m_Cipher.GetDataSize()) != 0;
	}

	return false;
}

//发送函数
bool CTCPSocketServiceThread::PerformSendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//加密数据 发送数据
	if (wDataSize <= EVENT_MASK_KERNEL
		&& m_hSocket != INVALID_SOCKET
		&& m_TCPSocketStatus == EVENT_TCP_SOCKET_CONNECT
		&& m_Cipher.EncryptBuffer(wMainCmdID, wSubCmdID, pData, wDataSize))
	{
		return SendBuffer(m_Cipher.GetData(), m_Cipher.GetDataSize()) != 0;
	}
	return false;
}

//关闭连接
VOID CTCPSocketServiceThread::PerformCloseSocket(BYTE cbShutReason)
{
	//内核变量
	m_wRecvSize=0;
	m_dwBufferData=0L; 
	m_bNeedBuffer=false;
	m_TCPSocketStatus=SOCKET_STATUS_IDLE; 

	//计数变量
	m_dwSendTickCount=0;
	m_dwRecvTickCount=0;

	//关闭判断
	if (m_hSocket!=INVALID_SOCKET)
	{
		//关闭连接
		closesocket(m_hSocket);
		m_hSocket=INVALID_SOCKET;

		//关闭通知
		if (cbShutReason!=SHUT_REASON_INSIDE)
		{
			CTCPSocketService * pTCPSocketStatusService=CONTAINING_RECORD(this,CTCPSocketService,m_TCPSocketServiceThread);
			pTCPSocketStatusService->OnSocketShut(cbShutReason);
		}
	}

}

//发送数据
DWORD CTCPSocketServiceThread::SendBuffer(VOID * pBuffer, WORD wSendSize)
{
	//变量定义
	WORD wTotalCount=0;

	//设置变量
	m_dwSendTickCount=GetTickCount()/1000L;


	//发送数据
	while ((m_bNeedBuffer == false) && (wTotalCount<wSendSize))
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

	//缓冲数据
	if (wTotalCount < wSendSize)
	{
		AmortizeBuffer((LPBYTE)pBuffer+wTotalCount,wSendSize-wTotalCount);
	}

	return wSendSize;
}

//缓冲数据
VOID CTCPSocketServiceThread::AmortizeBuffer(VOID * pData, WORD wDataSize)
{
	//申请缓冲
	if ((m_dwBufferData+wDataSize)>m_dwBufferSize)
	{
		//变量定义
		LPBYTE pcbDataBuffer=NULL;
		LPBYTE pcbDeleteBuffer=m_pcbDataBuffer;

		//计算大小
		DWORD dwNeedSize=m_dwBufferData+wDataSize;
		DWORD dwApplySize=__max(dwNeedSize,m_dwBufferSize*2L);

		//申请缓冲
		try
		{
			pcbDataBuffer=new BYTE [dwApplySize];
		}
		catch (...)	{ }

		//失败判断
		if (pcbDataBuffer==NULL)
		{
			PerformCloseSocket(SHUT_REASON_EXCEPTION);
			return;
		}

		//设置变量
		m_dwBufferSize=dwApplySize;
		m_pcbDataBuffer=pcbDataBuffer;
		CopyMemory(m_pcbDataBuffer,pcbDeleteBuffer,m_dwBufferData);

		//删除缓冲
		SafeDeleteArray(pcbDeleteBuffer);
	}

	//设置变量
	m_bNeedBuffer=true;
	CopyMemory(m_pcbDataBuffer + m_dwBufferData, pData, wDataSize);
	m_dwBufferData+=wDataSize;

	return;
}


//请求消息
LRESULT CTCPSocketServiceThread::OnServiceRequest(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	tagDataHead DataHead;
	CThreadLock ThreadLock(m_CriticalSection);

	//提取数据
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
	if (m_DataQueue.DistillData(DataHead,cbBuffer,sizeof(cbBuffer))==false) return 0;

	//数据处理
	switch (DataHead.wIdentifier)
	{
	case REQUEST_CONNECT:		//连接请求
		{
			//效验数据
			ASSERT(DataHead.wDataSize==sizeof(tagConnectRequest));
			tagConnectRequest * pConnectRequest=(tagConnectRequest *)cbBuffer;

			//数据处理
			if (PerformConnect(pConnectRequest->dwServerIP,pConnectRequest->wPort)!=CONNECT_SUCCESS)
			{
				//事件通知
				CTCPSocketService * pTCPSocketStatusService=CONTAINING_RECORD(this,CTCPSocketService,m_TCPSocketServiceThread);
				pTCPSocketStatusService->OnSocketLink(WSAEADDRNOTAVAIL);
			}

			return 1;
		}
	case REQUEST_SEND_DATA:		//发送请求
		{
			//效验数据
			ASSERT(DataHead.wDataSize==sizeof(tagSendDataRequest));
			tagSendDataRequest * pSendDataRequest=(tagSendDataRequest *)cbBuffer;

			//数据处理
			PerformSendData(pSendDataRequest->wMainCmdID,pSendDataRequest->wSubCmdID);

			return 1;
		}
	case REQUEST_SEND_DATA_EX:	//发送请求
		{
			//效验数据
			tagSendDataExRequest * pSendDataExRequest=(tagSendDataExRequest *)cbBuffer;
			ASSERT(DataHead.wDataSize>=(sizeof(tagSendDataExRequest)-sizeof(pSendDataExRequest->cbSendBuffer)));
			ASSERT(DataHead.wDataSize==(pSendDataExRequest->wDataSize+sizeof(tagSendDataExRequest)-sizeof(pSendDataExRequest->cbSendBuffer)));

			//数据处理
			PerformSendData(pSendDataExRequest->wMainCmdID,pSendDataExRequest->wSubCmdID,pSendDataExRequest->cbSendBuffer,pSendDataExRequest->wDataSize);

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

//网络消息
LRESULT CTCPSocketServiceThread::OnSocketNotify(WPARAM wParam, LPARAM lParam)
{
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:		//数据读取
		{ 
			return OnSocketNotifyRead(wParam,lParam);
		}
	case FD_WRITE:		//网络发送
		{
			return OnSocketNotifyWrite(wParam,lParam);
		}
	case FD_CLOSE:		//网络关闭
		{ 
			return OnSocketNotifyClose(wParam,lParam);	
		}
	case FD_CONNECT:	//网络连接
		{ 
			return OnSocketNotifyConnect(wParam,lParam); 
		}
	}

	return 0;
}

//网络读取
LRESULT CTCPSocketServiceThread::OnSocketNotifyRead(WPARAM wParam, LPARAM lParam)
{
	try
	{
		//读取数据
		INT iRetCode=recv(m_hSocket,(char *)m_cbRecvBuf+m_wRecvSize,sizeof(m_cbRecvBuf)-m_wRecvSize,0);

		if (iRetCode==SOCKET_ERROR) throw TEXT("网络连接关闭，读取数据失败");

		m_wRecvSize+=iRetCode;
		m_dwRecvTickCount=GetTickCount()/1000L;

		//变量定义
		WORD wPacketSize=0;
		CMD_Head *pHead = (CMD_Head *)m_cbRecvBuf;

		while (m_wRecvSize>=sizeof(CMD_Head))
		{
			//效验参数
			wPacketSize=pHead->CmdInfo.wPacketSize;

			if (wPacketSize > SOCKET_BUFFER) throw TEXT("数据包过长");
			if (wPacketSize < sizeof(CMD_Info)) throw TEXT("数据包非法");
			if (m_wRecvSize<wPacketSize) return 1;

			tagDecryptResult DecryptResult;
			//数据解密失败
			if (!m_Cipher.CrevasseBuffer(m_cbRecvBuf, wPacketSize, &DecryptResult))
			{
				PerformCloseSocket(SHUT_REASON_EXCEPTION);
				return 1;
			}

			//删除数据
			m_wRecvSize-=wPacketSize;
			MoveMemory(m_cbRecvBuf,m_cbRecvBuf+wPacketSize,m_wRecvSize);

			//解释数据
			CMD_Command Command = DecryptResult.CommandInfo;

			//内核数据
			if (Command.wMainCmdID == MDM_KN_COMMAND)
			{
				switch (Command.wSubCmdID)
				{
					case SUB_KN_DETECT_SOCKET:	//网络检测
					{
						//发送数据
						PerformSendData(MDM_KN_COMMAND, SUB_KN_DETECT_SOCKET);
						break;
					}
					//case SUB_KN_SHUT_DOWN_SOCKET:	//中断连接
					//	{
					//		//中断连接
					//		PerformCloseSocket(true); 

					//		break;
					//	}
				}
			}
			else
			{
				//处理数据
				CTCPSocketService * pTCPSocketStatusService = CONTAINING_RECORD(this, CTCPSocketService, m_TCPSocketServiceThread);
				if (!pTCPSocketStatusService->OnSocketRead(Command, DecryptResult.pData, DecryptResult.wDataSize))
					throw TEXT("网络数据包处理失败");
			}
		};
	}
	catch (...) 
	{
		//关闭连接
		PerformCloseSocket(SHUT_REASON_EXCEPTION);
	}

	return 1;
}

//网络发送
LRESULT CTCPSocketServiceThread::OnSocketNotifyWrite(WPARAM wParam, LPARAM lParam)
{
	//缓冲判断
	if ((m_bNeedBuffer==true)&&(m_dwBufferData>0L))
	{
		//变量定义
		DWORD dwTotalCount=0;
		DWORD dwPacketSize=4096;

		//设置变量
		m_dwSendTickCount=GetTickCount()/1000L;

		//发送数据
		while (dwTotalCount<m_dwBufferData)
		{
			//发生数据
			WORD wSendSize=(WORD)__min(dwPacketSize,m_dwBufferData-dwTotalCount);
			INT nSendCount=send(m_hSocket,(char *)m_pcbDataBuffer+dwTotalCount,wSendSize,0);

			//错误判断
			if (nSendCount==SOCKET_ERROR)
			{
				//缓冲判断
				if (WSAGetLastError()==WSAEWOULDBLOCK)
				{
					//设置变量
					m_bNeedBuffer=false;
					m_dwBufferData-=dwTotalCount;

					//移动内存
					if (m_dwBufferData>0L)
					{
						m_bNeedBuffer=true;
						MoveMemory(m_pcbDataBuffer,m_pcbDataBuffer+dwTotalCount,m_dwBufferData);
					}

					return 1L;
				}

				//关闭连接
				PerformCloseSocket(SHUT_REASON_EXCEPTION);

				return 1L;
			}

			//设置变量
			dwTotalCount+=nSendCount;
		}

		//设置变量
		m_dwBufferData=0L;
		m_bNeedBuffer=false;
	}

	return 1L;
}

//网络关闭
LRESULT CTCPSocketServiceThread::OnSocketNotifyClose(WPARAM wParam, LPARAM lParam)
{
	//关闭连接
	PerformCloseSocket(true);

	return 1;
}

//网络连接
LRESULT CTCPSocketServiceThread::OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam)
{
	//获取错误
	INT nErrorCode=WSAGETSELECTERROR(lParam);

	//事件通知
	CTCPSocketService * pTCPSocketStatusService = CONTAINING_RECORD(this, CTCPSocketService, m_TCPSocketServiceThread);
	pTCPSocketStatusService->OnSocketLink(nErrorCode);
	//关闭判断
	if (nErrorCode!=0)
	{
		PerformCloseSocket(SHUT_REASON_INSIDE);
		return 0;
	}

	//设置状态
	m_TCPSocketStatus = SOCKET_STATUS_CONNECT;

	//发送连接授权信息
	TCHAR szValidate[128] = TEXT("");
	*(WORD*)(szValidate) = 4;
	lstrcpy(szValidate + 1, szCompilation);

	PerformSendData(MDM_KN_COMMAND, SUB_KN_VALIDATE_SOCKET, szValidate, CountArray(szValidate));

	return 1;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTCPSocketService::CTCPSocketService()
{
	//内核变量
	m_wServiceID=0;
	m_bService=false;
	m_pITCPSocketEvent=NULL;

	return;
}

//析构函数
CTCPSocketService::~CTCPSocketService()
{
	//停止服务
	ConcludeService();

	return;
}

//接口查询
VOID * CTCPSocketService::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServiceModule,Guid,dwQueryVer);
	QUERYINTERFACE(ITCPSocketService,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITCPSocketService,Guid,dwQueryVer);
	return NULL;
}

//启动服务
bool CTCPSocketService::StartService()
{
	//运行判断
	ASSERT((m_bService==false)&&(m_pITCPSocketEvent!=NULL));
	if ((m_bService==true)||(m_pITCPSocketEvent==NULL)) return false;

	//服务线程
	if (m_TCPSocketServiceThread.StartThread()==false) return false;

	//设置变量
	m_bService=true;

	return true;
}

//停止服务
bool CTCPSocketService::ConcludeService()
{
	//设置变量
	m_bService=false;

	//停止线程
	m_TCPSocketServiceThread.ConcludeThread(INFINITE);

	return true;
}

//配置函数
bool CTCPSocketService::SetServiceID(WORD wServiceID)
{
	//状态效验
	ASSERT(m_bService==false);
	if (m_bService==true) return false;

	//设置变量
	m_wServiceID=wServiceID;

	return true;
}

//设置接口
bool CTCPSocketService::SetTCPSocketEvent(IUnknownEx * pIUnknownEx)
{
	//状态效验
	ASSERT(m_bService==false);
	if (m_bService==true) return false;

	//查询接口
	m_pITCPSocketEvent=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITCPSocketEvent);

	//错误判断
	if (m_pITCPSocketEvent==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

    return true;
}

//关闭连接
bool CTCPSocketService::CloseSocket()
{
	//状态效验
	ASSERT(m_bService==true);
	if (m_bService==false) return false;

	//投递请求
	return m_TCPSocketServiceThread.PostThreadRequest(REQUEST_CLOSE_SOCKET,NULL,0);
}

//连接服务器
bool CTCPSocketService::Connect(DWORD dwServerIP, WORD wPort)
{
	//状态效验
	ASSERT(m_bService==true);
	if (m_bService==false) return false;

	//构造数据
	tagConnectRequest ConnectRequest;
	ZeroMemory(&ConnectRequest,sizeof(ConnectRequest));

	//设置变量
	ConnectRequest.wPort=wPort;
	ConnectRequest.dwServerIP=htonl(dwServerIP);

	//投递请求
	return m_TCPSocketServiceThread.PostThreadRequest(REQUEST_CONNECT,&ConnectRequest,sizeof(ConnectRequest));
}

//连接地址
bool CTCPSocketService::Connect(LPCTSTR szServerIP, WORD wPort)
{
	//状态效验
	ASSERT(m_bService==true);
	if (m_bService==false) return false;

	//构造数据
	tagConnectRequest ConnectRequest;
	ZeroMemory(&ConnectRequest,sizeof(ConnectRequest));

	//设置变量
	ConnectRequest.wPort=wPort;
	ConnectRequest.dwServerIP=TranslateAddress(szServerIP);

	//投递请求
	return m_TCPSocketServiceThread.PostThreadRequest(REQUEST_CONNECT,&ConnectRequest,sizeof(ConnectRequest));
}

//发送函数
bool CTCPSocketService::SendData(WORD wMainCmdID, WORD wSubCmdID)
{
	//状态效验
	ASSERT(m_bService==true);
	if (m_bService==false) return false;

	//构造数据
	tagSendDataRequest SendDataRequest;
	ZeroMemory(&SendDataRequest,sizeof(SendDataRequest));

	//设置变量
	SendDataRequest.wSubCmdID=wSubCmdID;
	SendDataRequest.wMainCmdID=wMainCmdID;

	//投递请求
	return m_TCPSocketServiceThread.PostThreadRequest(REQUEST_SEND_DATA,&SendDataRequest,sizeof(SendDataRequest));
}

//发送函数
bool CTCPSocketService::SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//状态效验
	ASSERT(m_bService==true);
	if (m_bService==false) return false;

	//构造数据
	tagSendDataExRequest SendRequestEx;
	ZeroMemory(&SendRequestEx,sizeof(SendRequestEx));

	//设置变量
	SendRequestEx.wDataSize=wDataSize;
	SendRequestEx.wSubCmdID=wSubCmdID;
	SendRequestEx.wMainCmdID=wMainCmdID;

	//附加数据
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(SendRequestEx.cbSendBuffer,pData,wDataSize);
	}

	//投递请求
	WORD wSendSize=sizeof(SendRequestEx)-sizeof(SendRequestEx.cbSendBuffer)+wDataSize;
	return m_TCPSocketServiceThread.PostThreadRequest(REQUEST_SEND_DATA_EX,&SendRequestEx,wSendSize);
}

//连接消息
bool CTCPSocketService::OnSocketLink(INT nErrorCode)
{
	//投递事件
	ASSERT(m_pITCPSocketEvent!=NULL);
	return m_pITCPSocketEvent->OnEventTCPSocketLink(m_wServiceID,nErrorCode);
}

//关闭消息
bool CTCPSocketService::OnSocketShut(BYTE cbShutReason)
{
	//投递事件
	ASSERT(m_pITCPSocketEvent!=NULL);
	return m_pITCPSocketEvent->OnEventTCPSocketShut(m_wServiceID,cbShutReason);
}

//读取消息
bool CTCPSocketService::OnSocketRead(CMD_Command Command, VOID * pData, WORD wDataSize)
{
	//投递事件
	ASSERT(m_pITCPSocketEvent!=NULL);
	return m_pITCPSocketEvent->OnEventTCPSocketRead(m_wServiceID,Command,pData,wDataSize);
}

//地址解释
DWORD CTCPSocketService::TranslateAddress(LPCTSTR szServerIP)
{
	//转化地址
	CT2CA ServerAddr(szServerIP);
	DWORD dwServerIP=inet_addr(ServerAddr);

	//域名解释
	if (dwServerIP==INADDR_NONE)
	{
		LPHOSTENT lpHost=gethostbyname(ServerAddr);
		if (lpHost==NULL) return INADDR_NONE;
		dwServerIP=((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}

	return dwServerIP;
}

//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(TCPSocketService);

//////////////////////////////////////////////////////////////////////////
