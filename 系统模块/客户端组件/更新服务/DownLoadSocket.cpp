#include "Stdafx.h"
#include "DownLoadSocket.h"

#pragma warning (disable : 4996)

//////////////////////////////////////////////////////////////////////////

//�궨��
#define ID_SOCKET_WND					100									//���ڱ�ʶ
#define WM_SOCKET_NOTIFY_MSG			WM_USER+100							//��Ϣ��ʶ

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDownLoadSocket, CWnd)
	ON_MESSAGE(WM_SOCKET_NOTIFY_MSG,OnSocketNotify)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CDownLoadSocket::CDownLoadSocket()
{
	//��������
	m_wRequestID=0;
	m_bHeadValid=false;
	m_bCloseByServer=false;

	//���ݰ�ͷ
	m_wHeadSize=0;
	ZeroMemory(m_szHeadData,sizeof(m_szHeadData));

	//�ں˱���
	m_hSocket=INVALID_SOCKET;
	m_pIDownLoadSocketSink=NULL;
	m_cbSocketStatus=SOCKET_STATUS_IDLE;

	//������Ϣ
	m_ProxyServerType=ProxyType_None;
	ZeroMemory(&m_ProxyServerInfo,sizeof(m_ProxyServerInfo));

	return;
}

//��������
CDownLoadSocket::~CDownLoadSocket()
{
	CloseSocket(false);
}

//���ýӿ�
bool CDownLoadSocket::SetDownLoadSocketSink(IUnknownEx * pIUnknownEx)
{
	//���ýӿ�
	if (pIUnknownEx!=NULL)
	{
		//��ѯ�ӿ�
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDownLoadSocketSink)!=NULL);
		m_pIDownLoadSocketSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDownLoadSocketSink);

		//�ɹ��ж�
		if (m_pIDownLoadSocketSink==NULL) return false;
	}
	else m_pIDownLoadSocketSink=NULL;

	return true;
}

//���ô���
bool CDownLoadSocket::SetProxyServerInfo(enProxyServerType ProxyServerType, const tagProxyServerInfo & ProxyServerInfo)
{
	switch (ProxyServerType)
	{
	case ProxyType_None:	//û�д���
		{
			//���ñ���
			m_ProxyServerType=ProxyServerType;
			ZeroMemory(&m_ProxyServerInfo,sizeof(m_ProxyServerInfo));

			return true;
		}
	case ProxyType_Http:	//HTTP ����
	case ProxyType_Socks4:	//SOKKS4 ����
	case ProxyType_Socks5:	//SOKKS5 ����
		{
			//���ñ���
			m_ProxyServerType=ProxyServerType;
			m_ProxyServerInfo=ProxyServerInfo;

			return true;
		}
	default: 
		{ 
			ASSERT(FALSE); 
			return false;
		}
	}

	return false;
}

//���Ӳ���
bool CDownLoadSocket::Connect(LPCTSTR pszServerAddr, WORD wPort)
{
	//Ч�����
	ASSERT(m_hSocket==INVALID_SOCKET);
	ASSERT(m_cbSocketStatus==SOCKET_STATUS_IDLE);
	if (m_hSocket!=INVALID_SOCKET) throw TEXT("���� SOCKET ����Ѿ�����");
	if (m_cbSocketStatus!=SOCKET_STATUS_IDLE) throw TEXT("����״̬���ǵȴ�����״̬");

	//��ַ����
	DWORD dwServerIP=TranslateAddr(pszServerAddr);
	if (dwServerIP==INADDR_NONE) throw TEXT("Ŀ���������ַ��ʽ����ȷ��������ٴγ���");

	try
	{
		//���� SOCKET
		m_hSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if (m_hSocket==INVALID_SOCKET) throw TEXT("SOCKET ����ʧ��");

		//��������
		if (m_hWnd==NULL)
		{
			CRect rcSocketWnd;
			Create(NULL,NULL,WS_CHILD,rcSocketWnd,GetDesktopWindow(),ID_SOCKET_WND);
		}

		//��������ַ
		SOCKADDR_IN SocketAddr;
		ZeroMemory(&SocketAddr,sizeof(SocketAddr));
		SocketAddr.sin_family=AF_INET;
		SocketAddr.sin_port=htons(wPort);
		SocketAddr.sin_addr.S_un.S_addr=dwServerIP;

		//��������
		if (m_ProxyServerType!=ProxyType_None)
		{
			//���Ӵ���
			ConnectProxyServer();

			//��������
			switch (m_ProxyServerType)
			{
			case ProxyType_Socks5:	//Socks5 ����
				{
					//��������
					INT nOff=4;
					CHAR cbDataBuffer[256]={5,1,0,1};

					//���ӵ�ַ
					CopyMemory(cbDataBuffer+nOff,&SocketAddr.sin_addr,sizeof(SocketAddr.sin_addr));
					nOff+=sizeof(SocketAddr.sin_addr);
					
					//���Ӷ˿�
					*(WORD*)(cbDataBuffer+nOff)=SocketAddr.sin_port;
					nOff+=sizeof(WORD);

					//��������
					INT nResult=send(m_hSocket,cbDataBuffer,nOff,0);
					if (nResult!=nOff) throw TEXT("��������������Զ������ʧ��");

					//������Ӧ
					nResult=recv(m_hSocket,cbDataBuffer,4,0);
					if (nResult!=4) throw TEXT("���մ���������������ӵ�Ӧ��ʧ��");
					if (cbDataBuffer[0]!=5) throw TEXT("���մ���������������ӵ�Ӧ��ʧ��");

					//ʧ���ж�
					if (cbDataBuffer[1]!=0) 
					{
						WSASetLastError(WSAECONNREFUSED);
						PostMessage(WM_SOCKET_NOTIFY_MSG,0,MAKELONG(FD_CONNECT,WSAECONNREFUSED));
						return true;
					}

					//��������
					INT nLeftDataLen=(cbDataBuffer[3]==1)?6:cbDataBuffer[4]+2;
					nResult=recv(m_hSocket,cbDataBuffer,nLeftDataLen,0);
					if (nResult!=nLeftDataLen) throw TEXT("���մ�����������ݹ����г����쳣");

					break;
				}
			case ProxyType_Socks4:	//Socks4 ����
				{
					//��������
					INT nOff=2;
					CHAR cbDataBuffer[256]={4,1};

					//���Ӷ˿�
					*(WORD*)(cbDataBuffer+nOff)=SocketAddr.sin_port;
					nOff+=sizeof(WORD);

					//���ӵ�ַ
					CopyMemory(cbDataBuffer+nOff,&SocketAddr.sin_addr,sizeof(SocketAddr.sin_addr));
					nOff+=sizeof(SocketAddr.sin_addr);

					//�����ֶ�
					cbDataBuffer[nOff++]=0;

					//��������
					INT nResult=send(m_hSocket,cbDataBuffer,nOff,0);
					if (nResult!=nOff) throw TEXT("��������������Զ������ʧ��");

					//������Ӧ
					nResult=recv(m_hSocket,cbDataBuffer,8,0);
					if (nResult!=8) throw TEXT("����������ܾ�����Զ�̷���������");
					if (cbDataBuffer[0]!=0) throw TEXT("���մ���������������ӵ�Ӧ��ʧ��");

					//ʧ���ж�
					if (cbDataBuffer[1]!=90) 
					{
						WSASetLastError(WSAECONNREFUSED);
						PostMessage(WM_SOCKET_NOTIFY_MSG,0,MAKELONG(FD_CONNECT,WSAECONNREFUSED));
						return true;
					}

					break;
				}
			default:		//Ĭ�ϴ���
				{
					ASSERT(FALSE);
					throw TEXT("��֧�ֵĴ������ͣ�");
				}
			}

			//���ӳɹ�
			WSASetLastError(0);
			PostMessage(WM_SOCKET_NOTIFY_MSG,0,MAKELONG(FD_CONNECT,0));
			WSAAsyncSelect(m_hSocket,m_hWnd,WM_SOCKET_NOTIFY_MSG,FD_READ|FD_CONNECT|FD_CLOSE);
		}
		else
		{
			//�󶨴���
			WSAAsyncSelect(m_hSocket,m_hWnd,WM_SOCKET_NOTIFY_MSG,FD_READ|FD_CONNECT|FD_CLOSE);

			//���ӷ�����
			INT nErrorCode=connect(m_hSocket,(SOCKADDR *)&SocketAddr,sizeof(SocketAddr));
			if (nErrorCode==SOCKET_ERROR)
			{
				nErrorCode=WSAGetLastError();
				if (nErrorCode!=WSAEWOULDBLOCK)
				{
					static TCHAR szBuffer[64];
					// add by humorly 2018/8/3
					// replace func
					_snwprintf_s(szBuffer, CountArray(szBuffer), TEXT("���ӷ������󣬴������ [ %d ]"), nErrorCode);
					throw szBuffer;
				}
			}

			//���ñ���
			m_cbSocketStatus=SOCKET_STATUS_WAIT;
		}

		return true;
	}
	catch (LPCTSTR pszError)
	{
		CloseSocket(false);
		throw pszError;
	}
	catch (...)
	{
		CloseSocket(false);
		throw TEXT("���Ӳ���δ֪�쳣����");
	}

	return false;
}

//��������
bool CDownLoadSocket::SendRequestData(VOID * const pData, WORD wDataSize)
{
	//Ч�����
	ASSERT((wDataSize!=0)&&(pData!=NULL));
	if ((wDataSize==0)||(pData==NULL)) return false;

	//Ч��״̬
	if (m_hSocket==INVALID_SOCKET) return false;
	if (m_cbSocketStatus!=SOCKET_STATUS_CONNECT) return false;

	//���ñ���
	m_wHeadSize=0;
	m_wRequestID++;
	m_bHeadValid=false;

	//��������
	WORD wSended=0;
	while (wSended<wDataSize)
	{
		INT nErrorCode=send(m_hSocket,(LPCSTR)pData+wSended,wDataSize-wSended,0);
		if (nErrorCode==SOCKET_ERROR) return (WSAGetLastError()==WSAEWOULDBLOCK);
		wSended+=nErrorCode;
	}

	return true;
}

//�ر�����
bool CDownLoadSocket::CloseSocket(bool bNotify)
{
	//���ñ���
	m_wRequestID++;
	m_bCloseByServer=false;
	m_cbSocketStatus=SOCKET_STATUS_IDLE;

	//�ر��ж�
	if (m_hSocket!=INVALID_SOCKET)
	{
		//�ر�����
		WSAAsyncSelect(m_hSocket,GetSafeHwnd(),WM_SOCKET_NOTIFY_MSG,0);
		closesocket(m_hSocket);
		m_hSocket=INVALID_SOCKET;

		//�¼�֪ͨ
		if (bNotify==true)
		{
			ASSERT(m_pIDownLoadSocketSink!=NULL);
			m_pIDownLoadSocketSink->OnSocketClose(m_bCloseByServer);
		}
	}

	return true;
}

//���Ӵ���
bool CDownLoadSocket::ConnectProxyServer()
{
	//Ч��״̬
	ASSERT(m_ProxyServerType!=ProxyType_None);
	if (m_ProxyServerType==ProxyType_None) throw TEXT("�����ڴ��������������Ϣ");

	//���Ӵ���
	if (m_ProxyServerType!=ProxyType_None)
	{
		//�����ַ
		SOCKADDR_IN SocketAddrProxy;
		ZeroMemory(&SocketAddrProxy,sizeof(SocketAddrProxy));
		SocketAddrProxy.sin_family=AF_INET;
		SocketAddrProxy.sin_port=htons(m_ProxyServerInfo.wProxyPort);
		SocketAddrProxy.sin_addr.S_un.S_addr=TranslateAddr(m_ProxyServerInfo.szProxyServer);

		//���Ӵ���
		INT nErrorCode=connect(m_hSocket,(sockaddr *)&SocketAddrProxy,sizeof(SocketAddrProxy));
		if (nErrorCode==SOCKET_ERROR) throw TEXT("�������������ʧ��");

		//��������
		if (m_ProxyServerType==ProxyType_Socks5)
		{
			//��������
			INT nOff=2;
			CHAR cbDataBuffer[256]={5,1};

			//��������
			cbDataBuffer[nOff++]=(m_ProxyServerInfo.szUserName[0]==0)?0:2;
			if (send(m_hSocket,cbDataBuffer,nOff,0)!=nOff) throw TEXT("������������ʧ��");

			//��������
			INT nResult=recv(m_hSocket,cbDataBuffer,2,0);
			if (nResult!=2) throw TEXT("���մ��������Ӧ��ʧ��");

			//��������
			if (cbDataBuffer[0]!=5) throw TEXT("���մ��������Ӧ��ʧ��");
			if ((cbDataBuffer[1]!=0)&&(cbDataBuffer[1]!=2)) throw TEXT("����������ܾ�����");

			//�����֤
			nOff=0;
			if (cbDataBuffer[1]==2)
			{
				//��������
				INT nStringLen=0;
				CT2CA strUserName(m_ProxyServerInfo.szUserName);
				CT2CA strPassword(m_ProxyServerInfo.szPassword);

				//��¼�ʺ�
				cbDataBuffer[nOff++]=1;
				nStringLen=lstrlenA(strUserName);
				cbDataBuffer[nOff++]=(CHAR)nStringLen;
				CopyMemory(cbDataBuffer+nOff,(LPCSTR)strUserName,nStringLen);
				nOff+=nStringLen;

				//��¼����
				nStringLen=lstrlenA(strPassword);
				cbDataBuffer[nOff++]=(CHAR)nStringLen;
				CopyMemory(cbDataBuffer+nOff,(LPCSTR)strPassword,nStringLen);
				nOff+=nStringLen;

				//��������
				INT nSend=send(m_hSocket,cbDataBuffer,nOff,0);
				if (nSend!=nOff) throw TEXT("�������������������֤ʧ��");

				//��������
				INT nRecv=recv(m_hSocket,cbDataBuffer,2,0);
				if (nRecv!=2) throw TEXT("���մ�������������֤Ӧ��ʧ��");

				//���ݷ���
				if (cbDataBuffer[0]!=1) throw TEXT("ͨ������������ʻ���Ϣ�����д�");
				if (cbDataBuffer[1]!=0) throw TEXT("������������ͨ�������������û���������");
			}
		}
	}

	return true;
}

//��ַ����
DWORD CDownLoadSocket::TranslateAddr(LPCTSTR pszServerAddr)
{
	//ת����ַ
	CT2CA ServerAddr(pszServerAddr);
	DWORD dwServerIP=inet_addr(ServerAddr);

	//��������
	if (dwServerIP==INADDR_NONE)
	{
		LPHOSTENT lpHost=gethostbyname(ServerAddr);
		if (lpHost==NULL) return INADDR_NONE;
		dwServerIP=((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}

	return dwServerIP;
}

//�����ȡ
LRESULT CDownLoadSocket::OnSocketNotifyRead(WPARAM wParam, LPARAM lParam)
{
	try
	{
		//��ȡ��ͷ
		if (m_bHeadValid==false)
		{
			//��ȡ����
			INT nResult=recv(m_hSocket,m_szHeadData+m_wHeadSize,sizeof(m_szHeadData)-m_wHeadSize,0);
			if (nResult==SOCKET_ERROR) throw TEXT("�������ӹرգ���ȡ����ʧ��");

			//����ͷβ
			WORD wRequestID=m_wRequestID;
			WORD wWholeSize=m_wHeadSize+nResult;
			LPCSTR pszFinality=strstr((LPCSTR)m_szHeadData,"\r\n\r\n");

			//����ж�
			if (pszFinality!=NULL)
			{
				//���ñ���
				m_bHeadValid=true;
				m_wHeadSize=(WORD)(pszFinality-m_szHeadData)+(WORD)strlen("\r\n\r\n");

				//��ȡ״̬
				INT nStatusCode=0;
				for (WORD i=0;i<m_wHeadSize;i++)
				{
					if (m_szHeadData[i]==' ')
					{
						//��������
						WORD nStatusOff=0;
						CHAR szStatusString[16]="";

						//��ȡ�ַ�
						do
						{
							szStatusString[nStatusOff++]=m_szHeadData[i++];
						} while (m_szHeadData[i]!=' ');

						//���ñ���
						szStatusString[nStatusOff]=0;
						nStatusCode=atol(szStatusString);

						break;
					}
				}

				//��ͷ�¼�
				ASSERT(m_pIDownLoadSocketSink!=NULL);
				m_pIDownLoadSocketSink->OnHttpDataHead(m_szHeadData,m_wHeadSize,nStatusCode);

				//�����¼�
				if ((wWholeSize>m_wHeadSize)&&(wRequestID==m_wRequestID))
				{
					ASSERT(m_pIDownLoadSocketSink!=NULL);
					m_pIDownLoadSocketSink->OnHttpDataStream(m_szHeadData+m_wHeadSize,wWholeSize-m_wHeadSize);
				}
			}

			return 1;
		}
		else
		{
			//��ȡ����
			CHAR cbReadBuffer[4096];
			INT nReadSize=recv(m_hSocket,cbReadBuffer,sizeof(cbReadBuffer),0);
			if (nReadSize==SOCKET_ERROR) throw TEXT("�������ӹرգ���ȡ����ʧ��");

			//�¼�֪ͨ
			ASSERT(m_pIDownLoadSocketSink!=NULL);
			m_pIDownLoadSocketSink->OnHttpDataStream(cbReadBuffer,nReadSize);
		}
	}
	catch (...) 
	{ 
		CloseSocket(true); 
	}

	return 1;
}

//����ر�
LRESULT CDownLoadSocket::OnSocketNotifyClose(WPARAM wParam, LPARAM lParam)
{
	//���ñ���
	m_bCloseByServer=true;

	//�ر�����
	CloseSocket(true);

	return 1;
}

//��������
LRESULT CDownLoadSocket::OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam)
{
	//��ȡ����
	INT nErrorCode=WSAGETSELECTERROR(lParam);

	//�ж�״̬
	if (nErrorCode==0) m_cbSocketStatus=SOCKET_STATUS_CONNECT;
	else CloseSocket(false);

	//����֪ͨ
	ASSERT(m_pIDownLoadSocketSink!=NULL);
	m_pIDownLoadSocketSink->OnSocketConnect(nErrorCode);

	return 1;
}

//������Ϣ
LRESULT	CDownLoadSocket::OnSocketNotify(WPARAM wParam, LPARAM lParam)
{
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:		//���ݶ�ȡ
		{ 
			return OnSocketNotifyRead(wParam,lParam); 
		}
	/*case FD_WRITE:		//���緢��
		{
			return OnSocketNotifyWrite(wParam,lParam);
		}*/
	case FD_CLOSE:		//����ر�
		{ 
			return OnSocketNotifyClose(wParam,lParam);	
		}
	case FD_CONNECT:	//��������
		{ 
			return OnSocketNotifyConnect(wParam,lParam); 
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
