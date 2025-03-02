#include "stdafx.h"
#include "WebSocketService.h"

//��Ϣ����
#define WM_SOCKET_NOTIFY			WM_USER+100							//��Ϣ��ʶ
#define WM_SERVICE_REQUEST			WM_USER+101							//��������

//////////////////////////////////////////////////////////////////////////
//��������

#define REQUEST_CONNECT				1									//��������
#define REQUEST_SEND_DATA			2									//������
#define REQUEST_CLOSE_SOCKET		3									//����ر�

//��������
struct tagConnectRequest
{
	WORD							wPort;								//���Ӷ˿�
	TCHAR							szUrl[256];							//���ӵ�ַ
};

//��������
struct tagSendDataRequest
{
	WORD							wDataSize;							//���ݴ�С
	BYTE							cbSendBuffer[SOCKET_TCP_PACKET];	//���ݻ���
};


//���캯��
CWebSocketServiceThread::CWebSocketServiceThread()
{
	//�ں˱���
	m_hWnd = NULL;
	m_hSocket = INVALID_SOCKET;
	m_TCPSocketStatus = SOCKET_STATUS_IDLE;

	//���ձ���
	m_wRecvSize = 0;
	ZeroMemory(m_cbRecvBuf, sizeof(m_cbRecvBuf));

	//�������
	m_dwBufferData = 0L;
	m_dwBufferSize = 0L;
	m_bNeedBuffer = false;
	m_pcbDataBuffer = NULL;

}

bool CWebSocketServiceThread::ConcludeThread(DWORD dwWaitSeconds)
{
	//�˳���Ϣ
	if (IsRuning() == true)
	{
		PostThreadMessage(WM_QUIT, 0, 0);
	}

	return __super::ConcludeThread(dwWaitSeconds);
}

//��ʼ�¼�
bool CWebSocketServiceThread::OnEventThreadStrat()
{
	//��������
	WNDCLASS WndClass;
	ZeroMemory(&WndClass, sizeof(WndClass));

	//���ñ���
	WndClass.lpfnWndProc = DefWindowProc;
	WndClass.hInstance = AfxGetInstanceHandle();
	WndClass.lpszClassName = TEXT("WebSocketStatusServiceThread");

	//ע�ᴰ��
	RegisterClass(&WndClass);

	//��������
	HWND hParentWnd = GetDesktopWindow();
	m_hWnd = CreateWindow(WndClass.lpszClassName, NULL, WS_CHILD, 0, 0, 0, 0, hParentWnd, NULL, WndClass.hInstance, NULL);

	return true;
}

//ֹͣ�¼�
bool CWebSocketServiceThread::OnEventThreadConclude()
{
	//�رմ���
	if (m_hWnd != NULL)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}

	//�ر�����
	PerformCloseSocket(SHUT_REASON_INSIDE);

	//�������
	CWHDataLocker ThreadLock(m_CriticalSection);
	m_DataQueue.RemoveData(false);

	return true;
}

//���к���
bool CWebSocketServiceThread::OnEventThreadRun()
{
	//��ȡ��Ϣ
	MSG Message;
	if (GetMessage(&Message, NULL, 0, 0) == FALSE) return false;

	//��Ϣ����
	switch (Message.message)
	{
	case WM_SOCKET_NOTIFY:		//������Ϣ
	{
		OnSocketNotify(Message.wParam, Message.lParam);
		return true;
	}
	case WM_SERVICE_REQUEST:	//��������
	{
		OnServiceRequest(Message.wParam, Message.lParam);
		return true;
	}
	default:					//Ĭ�ϴ���
	{
		DefWindowProc(Message.hwnd, Message.message, Message.wParam, Message.lParam);
		return true;
	}
	}

	return false;
}


bool CWebSocketServiceThread::PostThreadRequest(WORD wIdentifier, VOID * const pBuffer, WORD wDataSize)
{
	//Ͷ������
	CWHDataLocker ThreadLock(m_CriticalSection);
	m_DataQueue.InsertData(wIdentifier, pBuffer, wDataSize);

	//������Ϣ
	ASSERT(m_hWnd != NULL);
	if (m_hWnd != NULL) PostMessage(m_hWnd, WM_SERVICE_REQUEST, wDataSize, GetCurrentThreadId());

	return true;
}


//ִ������
DWORD CWebSocketServiceThread::PerformConnect(DWORD dwServerIP, WORD wPort)
{
	//Ч�����
	ASSERT(m_hSocket == INVALID_SOCKET);
	ASSERT(m_TCPSocketStatus == SOCKET_STATUS_IDLE);
	ASSERT((dwServerIP != INADDR_NONE) && (dwServerIP != 0));

	try
	{
		//Ч��״̬
		if (m_hSocket != INVALID_SOCKET) throw CONNECT_EXCEPTION;
		if (m_TCPSocketStatus != SOCKET_STATUS_IDLE) throw CONNECT_EXCEPTION;
		if ((dwServerIP == INADDR_NONE) || (dwServerIP == 0)) throw CONNECT_EXCEPTION;

		//���ò���
		m_wRecvSize = 0;

		//���� SOCKET
		m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_hSocket == INVALID_SOCKET) throw CONNECT_EXCEPTION;

		//��������
		SOCKADDR_IN SocketAddr;
		ZeroMemory(&SocketAddr, sizeof(SocketAddr));

		//���ñ���
		SocketAddr.sin_family = AF_INET;
		SocketAddr.sin_port = htons(wPort);
		SocketAddr.sin_addr.S_un.S_addr = dwServerIP;

		//�󶨴���
		WSAAsyncSelect(m_hSocket, m_hWnd, WM_SOCKET_NOTIFY, FD_READ | FD_CONNECT | FD_CLOSE | FD_WRITE);

		//���ӷ�����
		INT nErrorCode = connect(m_hSocket, (SOCKADDR *)&SocketAddr, sizeof(SocketAddr));
		if ((nErrorCode == SOCKET_ERROR) && (WSAGetLastError() != WSAEWOULDBLOCK)) throw CONNECT_EXCEPTION;

		//���ñ���
		m_TCPSocketStatus = SOCKET_STATUS_WAIT;

		return CONNECT_SUCCESS;
	}
	catch (...)
	{
		//�ر�����
		PerformCloseSocket(SHUT_REASON_INSIDE);
	}

	return CONNECT_EXCEPTION;
}

//�ر�����
VOID CWebSocketServiceThread::PerformCloseSocket(BYTE cbShutReason)
{

	//�ر��ж�
	if (m_hSocket != INVALID_SOCKET)
	{
		//�ر�����
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;

		//�ر�֪ͨ
		if (cbShutReason != SHUT_REASON_INSIDE)
		{
			CWebSocketService * pTCPSocketStatusService = CONTAINING_RECORD(this, CWebSocketService, m_WebSocketServiceThread);
			pTCPSocketStatusService->OnEventWEBSocketShut(cbShutReason);
		}
	}

	//�ں˱���
	m_wRecvSize = 0;  //14580
	m_dwBufferData = 0L;   //147556
	m_bNeedBuffer = false;  //147554
	m_TCPSocketStatus = SOCKET_STATUS_IDLE; //147558

}

//��������
DWORD CWebSocketServiceThread::SendBuffer(VOID * pBuffer, WORD wSendSize)
{
	//��������
	WORD wTotalCount = 0;

	if (pBuffer && wSendSize && m_hSocket != INVALID_SOCKET && m_TCPSocketStatus == SOCKET_STATUS_CONNECT)
	{ 

		//��������
		while ((m_bNeedBuffer == false) && (wTotalCount < wSendSize))
		{
			//��������
			INT nSendCount = send(m_hSocket, (char *)pBuffer + wTotalCount, wSendSize - wTotalCount, 0);

			//�����ж�
			if (nSendCount == SOCKET_ERROR)
			{
				//�����ж�
				if (WSAGetLastError() == WSAEWOULDBLOCK)
				{
					AmortizeBuffer((LPBYTE)pBuffer + wTotalCount, wSendSize - wTotalCount);
					return wSendSize;
				}

				//�ر�����
				PerformCloseSocket(SHUT_REASON_EXCEPTION);

				return 0L;
			}
			else
			{
				//���ñ���
				wTotalCount += nSendCount;
			}
		}
	}

	//��������
	if (wTotalCount < wSendSize)
	{
		AmortizeBuffer((LPBYTE)pBuffer + wTotalCount, wSendSize - wTotalCount);
	}

	return wSendSize;
}

//������Ϣ
LRESULT CWebSocketServiceThread::OnSocketNotify(WPARAM wParam, LPARAM lParam)
{
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:		//���ݶ�ȡ
	{
		return OnSocketNotifyRead(wParam, lParam);
	}
	case FD_WRITE:		//���緢��
	{
		return OnSocketNotifyWrite(wParam, lParam);
	}
	case FD_CLOSE:		//����ر�
	{
		return OnSocketNotifyClose(wParam, lParam);
	}
	case FD_CONNECT:	//��������
	{
		return OnSocketNotifyConnect(wParam, lParam);
	}
	}

	return 0;
}

//�����ȡ
LRESULT CWebSocketServiceThread::OnSocketNotifyRead(WPARAM wParam, LPARAM lParam)
{
	try
	{
		if (m_bFlag)
		{
			BYTE pBuffer[4096];
			//��ȡ����
			INT iRetCode = recv(m_hSocket, (char *)pBuffer, sizeof(pBuffer), 0);
			if (iRetCode == SOCKET_ERROR) throw TEXT("�������ӹرգ���ȡ����ʧ��");

			//��������
			if (!true)
			{
				throw TEXT("δȡ�úϷ���Ȩ����������������ģʽ���У�");
			}

		}
		else
		{
			//��ȡ����
			INT iRetCode = recv(m_hSocket, (char *)m_cbRecvBuf + m_wRecvSize, sizeof(m_cbRecvBuf) - m_wRecvSize, 0);

			if (iRetCode == SOCKET_ERROR) throw TEXT("�������ӹرգ���ȡ����ʧ��");

			m_wRecvSize += iRetCode;

			//�ж�ͷ���Ƿ��������
			const char * cbBuffer = strstr((const char*)m_cbRecvBuf, "\r\n\r\n");
			if (cbBuffer)
			{
				m_bFlag = 1;		//ͷ���������
				m_wRecvSize = 0;

				char oneByte;
				for (int i = 0; i < m_wRecvSize; i++)
				{
					oneByte = m_cbRecvBuf[i];

					if (oneByte == 32)	//�ո�
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
		//�ر�����
		PerformCloseSocket(true);
	}

	return 1;
}

//���緢��  
LRESULT CWebSocketServiceThread::OnSocketNotifyWrite(WPARAM wParam, LPARAM lParam)
{
	if ((m_bNeedBuffer == true) && (m_dwBufferData>0L))
	{
		//��������
		DWORD dwTotalCount = 0;
		DWORD dwPacketSize = 4096;


		//��������
		while (dwTotalCount<m_dwBufferData)
		{
			//��������
			WORD wSendSize = (WORD)__min(dwPacketSize, m_dwBufferData - dwTotalCount);
			INT nSendCount = send(m_hSocket, (char *)m_pcbDataBuffer + dwTotalCount, wSendSize, 0);

			//�����ж�
			if (nSendCount == SOCKET_ERROR)
			{
				//�����ж�
				if (WSAGetLastError() == WSAEWOULDBLOCK)
				{
					//���ñ���
					m_bNeedBuffer = false;
					m_dwBufferData -= dwTotalCount;

					//�ƶ��ڴ�
					if (m_dwBufferData>0L)
					{
						m_bNeedBuffer = true;
						MoveMemory(m_pcbDataBuffer, m_pcbDataBuffer + dwTotalCount, m_dwBufferData);
					}

					return 1L;
				}

				//�ر�����
				PerformCloseSocket(SHUT_REASON_EXCEPTION);

				return 1L;
			}

			//���ñ���
			dwTotalCount += nSendCount;
		}

		//���ñ���
		m_dwBufferData = 0L;
		m_bNeedBuffer = false;
	}

	return 1L;
}

//����ر�
LRESULT CWebSocketServiceThread::OnSocketNotifyClose(WPARAM wParam, LPARAM lParam)
{
	//�ر�����
	PerformCloseSocket(true);

	return 1;
}

//��������
LRESULT CWebSocketServiceThread::OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam)
{
	//��ȡ����
	INT nErrorCode = WSAGETSELECTERROR(lParam);

	//�¼�֪ͨ
	CWebSocketService * pWebSocketStatusService = CONTAINING_RECORD(this, CWebSocketService, m_WebSocketServiceThread);
	pWebSocketStatusService->OnEventWEBSocketLink(nErrorCode);

	//�ر��ж�
	if (nErrorCode != 0)
	{
		PerformCloseSocket(SHUT_REASON_INSIDE);
		return 0;
	}

	//����״̬
	m_TCPSocketStatus = SOCKET_STATUS_CONNECT;

	return 1;
}

//������Ϣ
LRESULT CWebSocketServiceThread::OnServiceRequest(WPARAM wParam, LPARAM lParam)
{
	//��������
	tagDataHead DataHead;
	CWHDataLocker ThreadLock(m_CriticalSection);

	//��ȡ����
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
	if (m_DataQueue.DistillData(DataHead, cbBuffer, sizeof(cbBuffer)) == false) return 0;

	//���ݴ���
	switch (DataHead.wIdentifier)
	{
	case REQUEST_CONNECT:		//��������
	{
		//Ч������
		ASSERT(DataHead.wDataSize == sizeof(tagConnectRequest));
		tagConnectRequest * pConnectRequest = (tagConnectRequest *)cbBuffer;

		//���ݴ���
		if (PerformConnect(pConnectRequest->szUrl, pConnectRequest->wPort) != CONNECT_SUCCESS)
		{
			//�¼�֪ͨ
			CWebSocketService * pWebSocketStatusService = CONTAINING_RECORD(this, CWebSocketService, m_WebSocketServiceThread);
			pWebSocketStatusService->OnEventWEBSocketLink(WSAEADDRNOTAVAIL);
		}

		return 1;
	}
	case REQUEST_SEND_DATA:		//��������
	{
		//Ч������
		ASSERT(DataHead.wDataSize == sizeof(tagSendDataRequest));
		tagSendDataRequest * pSendDataRequest = (tagSendDataRequest *)cbBuffer;

		//���ݴ���
		SendBuffer(pSendDataRequest->cbSendBuffer, pSendDataRequest->wDataSize);

		return 1;
	}

	case REQUEST_CLOSE_SOCKET:	//�ر�����
	{
		//�ر�����
		PerformCloseSocket(true);

		return 1;
	}
	}

	return 0;
}




//���캯��
CWebSocketService::CWebSocketService()
{
	//�ں˱���
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
	//�����ж�
	ASSERT((m_bService == false) && (m_pIWebSocketEvent != NULL));
	if ((m_bService == true) || (m_pIWebSocketEvent == NULL)) return false;

	//�����߳�
	if (m_WebSocketServiceThread.StartThread() == false) return false;

	//���ñ���
	m_bService = true;

	return true;
}
//ֹͣ����
bool CWebSocketService::ConcludeService()
{
	//���ñ���
	m_bService = false;

	//ֹͣ�߳�
	m_WebSocketServiceThread.ConcludeThread(INFINITE);

	return true;
}
bool CWebSocketService::SetServiceID(WORD wServiceID)
{
	//״̬Ч��
	ASSERT(m_bService == false);
	if (m_bService == true) return false;

	//���ñ���
	m_wServiceID = wServiceID;

	return true;
}
//���ýӿ�
bool CWebSocketService::SetWEBSocketEvent(IUnknownEx * pIUnknownEx)
{
	//״̬Ч��
	ASSERT(m_bService == false);
	if (m_bService == true) return false;

	//��ѯ�ӿ�
	m_pIWebSocketEvent = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IWEBSocketEvent);

	//�����ж�
	if (m_pIWebSocketEvent == NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//�ر�����
bool CWebSocketService::CloseSocket(BYTE cbShutReason)
{
	//״̬Ч��
	ASSERT(m_bService == true);
	if (m_bService == false) return false;

	//Ͷ������
	return m_WebSocketServiceThread.PostThreadRequest(REQUEST_CLOSE_SOCKET, &cbShutReason, 1);
}

//���Ӳ���
bool CWebSocketService::ConnectServer(LPCTSTR pszURL, WORD wPort)
{
	if (m_bService) return false;

	tagConnectRequest	connentRequest;
	connentRequest.wPort = wPort;
	lstrcpy(connentRequest.szUrl, pszURL);

	return m_WebSocketServiceThread.PostThreadRequest(REQUEST_CONNECT, &connentRequest, sizeof(connentRequest));
	return true;
}

//��������
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
//�ر���Ϣ
bool  CWebSocketService::OnEventWEBSocketShut(WORD wServiceID, WORD wRequestID, BYTE cbShutReason)
{
	return true;
}
//���ݰ���
bool  CWebSocketService::OnEventWEBSocketMain(WORD wServiceID, WORD wRequestID, VOID * pcbMailData, WORD wStreamSize)
{
	return true;
}

//���ݰ�ͷ
bool  CWebSocketService::OnEventWEBSocketHead(WORD wServiceID, WORD wRequestID, VOID * pcbHeadData, WORD wHeadSize, INT nStatusCode)
{
	if (nStatusCode == 200)
	{
		return true;
	}

	//������
	if (nStatusCode <= 300 || nStatusCode > 303)
	{
		return false;
	}
	else   //�ض���
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