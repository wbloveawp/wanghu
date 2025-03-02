#include "Stdafx.h"
#include "DownLoadSocket.h"

#pragma warning (disable : 4996)

//////////////////////////////////////////////////////////////////////////

//宏定义
#define ID_SOCKET_WND					100									//窗口标识
#define WM_SOCKET_NOTIFY_MSG			WM_USER+100							//消息标识

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDownLoadSocket, CWnd)
	ON_MESSAGE(WM_SOCKET_NOTIFY_MSG,OnSocketNotify)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDownLoadSocket::CDownLoadSocket()
{
	//辅助变量
	m_wRequestID=0;
	m_bHeadValid=false;
	m_bCloseByServer=false;

	//数据包头
	m_wHeadSize=0;
	ZeroMemory(m_szHeadData,sizeof(m_szHeadData));

	//内核变量
	m_hSocket=INVALID_SOCKET;
	m_pIDownLoadSocketSink=NULL;
	m_cbSocketStatus=SOCKET_STATUS_IDLE;

	//代理信息
	m_ProxyServerType=ProxyType_None;
	ZeroMemory(&m_ProxyServerInfo,sizeof(m_ProxyServerInfo));

	return;
}

//析构函数
CDownLoadSocket::~CDownLoadSocket()
{
	CloseSocket(false);
}

//设置接口
bool CDownLoadSocket::SetDownLoadSocketSink(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if (pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDownLoadSocketSink)!=NULL);
		m_pIDownLoadSocketSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDownLoadSocketSink);

		//成功判断
		if (m_pIDownLoadSocketSink==NULL) return false;
	}
	else m_pIDownLoadSocketSink=NULL;

	return true;
}

//设置代理
bool CDownLoadSocket::SetProxyServerInfo(enProxyServerType ProxyServerType, const tagProxyServerInfo & ProxyServerInfo)
{
	switch (ProxyServerType)
	{
	case ProxyType_None:	//没有代理
		{
			//设置变量
			m_ProxyServerType=ProxyServerType;
			ZeroMemory(&m_ProxyServerInfo,sizeof(m_ProxyServerInfo));

			return true;
		}
	case ProxyType_Http:	//HTTP 代理
	case ProxyType_Socks4:	//SOKKS4 代理
	case ProxyType_Socks5:	//SOKKS5 代理
		{
			//设置变量
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

//连接操作
bool CDownLoadSocket::Connect(LPCTSTR pszServerAddr, WORD wPort)
{
	//效验参数
	ASSERT(m_hSocket==INVALID_SOCKET);
	ASSERT(m_cbSocketStatus==SOCKET_STATUS_IDLE);
	if (m_hSocket!=INVALID_SOCKET) throw TEXT("连接 SOCKET 句柄已经存在");
	if (m_cbSocketStatus!=SOCKET_STATUS_IDLE) throw TEXT("连接状态不是等待连接状态");

	//地址解释
	DWORD dwServerIP=TranslateAddr(pszServerAddr);
	if (dwServerIP==INADDR_NONE) throw TEXT("目标服务器地址格式不正确，请检查后再次尝试");

	try
	{
		//建立 SOCKET
		m_hSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if (m_hSocket==INVALID_SOCKET) throw TEXT("SOCKET 创建失败");

		//创建窗口
		if (m_hWnd==NULL)
		{
			CRect rcSocketWnd;
			Create(NULL,NULL,WS_CHILD,rcSocketWnd,GetDesktopWindow(),ID_SOCKET_WND);
		}

		//服务器地址
		SOCKADDR_IN SocketAddr;
		ZeroMemory(&SocketAddr,sizeof(SocketAddr));
		SocketAddr.sin_family=AF_INET;
		SocketAddr.sin_port=htons(wPort);
		SocketAddr.sin_addr.S_un.S_addr=dwServerIP;

		//代理连接
		if (m_ProxyServerType!=ProxyType_None)
		{
			//连接代理
			ConnectProxyServer();

			//请求连接
			switch (m_ProxyServerType)
			{
			case ProxyType_Socks5:	//Socks5 代理
				{
					//构造请求
					INT nOff=4;
					CHAR cbDataBuffer[256]={5,1,0,1};

					//连接地址
					CopyMemory(cbDataBuffer+nOff,&SocketAddr.sin_addr,sizeof(SocketAddr.sin_addr));
					nOff+=sizeof(SocketAddr.sin_addr);
					
					//连接端口
					*(WORD*)(cbDataBuffer+nOff)=SocketAddr.sin_port;
					nOff+=sizeof(WORD);

					//连接请求
					INT nResult=send(m_hSocket,cbDataBuffer,nOff,0);
					if (nResult!=nOff) throw TEXT("请求代理服务器与远程连接失败");

					//接收响应
					nResult=recv(m_hSocket,cbDataBuffer,4,0);
					if (nResult!=4) throw TEXT("接收代理服务器对于连接的应答失败");
					if (cbDataBuffer[0]!=5) throw TEXT("接收代理服务器对于连接的应答失败");

					//失败判断
					if (cbDataBuffer[1]!=0) 
					{
						WSASetLastError(WSAECONNREFUSED);
						PostMessage(WM_SOCKET_NOTIFY_MSG,0,MAKELONG(FD_CONNECT,WSAECONNREFUSED));
						return true;
					}

					//接收数据
					INT nLeftDataLen=(cbDataBuffer[3]==1)?6:cbDataBuffer[4]+2;
					nResult=recv(m_hSocket,cbDataBuffer,nLeftDataLen,0);
					if (nResult!=nLeftDataLen) throw TEXT("接收代理服务器数据过程中出现异常");

					break;
				}
			case ProxyType_Socks4:	//Socks4 代理
				{
					//构造请求
					INT nOff=2;
					CHAR cbDataBuffer[256]={4,1};

					//连接端口
					*(WORD*)(cbDataBuffer+nOff)=SocketAddr.sin_port;
					nOff+=sizeof(WORD);

					//连接地址
					CopyMemory(cbDataBuffer+nOff,&SocketAddr.sin_addr,sizeof(SocketAddr.sin_addr));
					nOff+=sizeof(SocketAddr.sin_addr);

					//保留字段
					cbDataBuffer[nOff++]=0;

					//连接请求
					INT nResult=send(m_hSocket,cbDataBuffer,nOff,0);
					if (nResult!=nOff) throw TEXT("请求代理服务器与远程连接失败");

					//接收响应
					nResult=recv(m_hSocket,cbDataBuffer,8,0);
					if (nResult!=8) throw TEXT("代理服务器拒绝连接远程服务器请求");
					if (cbDataBuffer[0]!=0) throw TEXT("接收代理服务器对于连接的应答失败");

					//失败判断
					if (cbDataBuffer[1]!=90) 
					{
						WSASetLastError(WSAECONNREFUSED);
						PostMessage(WM_SOCKET_NOTIFY_MSG,0,MAKELONG(FD_CONNECT,WSAECONNREFUSED));
						return true;
					}

					break;
				}
			default:		//默认处理
				{
					ASSERT(FALSE);
					throw TEXT("不支持的代理类型！");
				}
			}

			//连接成功
			WSASetLastError(0);
			PostMessage(WM_SOCKET_NOTIFY_MSG,0,MAKELONG(FD_CONNECT,0));
			WSAAsyncSelect(m_hSocket,m_hWnd,WM_SOCKET_NOTIFY_MSG,FD_READ|FD_CONNECT|FD_CLOSE);
		}
		else
		{
			//绑定窗口
			WSAAsyncSelect(m_hSocket,m_hWnd,WM_SOCKET_NOTIFY_MSG,FD_READ|FD_CONNECT|FD_CLOSE);

			//连接服务器
			INT nErrorCode=connect(m_hSocket,(SOCKADDR *)&SocketAddr,sizeof(SocketAddr));
			if (nErrorCode==SOCKET_ERROR)
			{
				nErrorCode=WSAGetLastError();
				if (nErrorCode!=WSAEWOULDBLOCK)
				{
					static TCHAR szBuffer[64];
					// add by humorly 2018/8/3
					// replace func
					_snwprintf_s(szBuffer, CountArray(szBuffer), TEXT("连接发生错误，错误代码 [ %d ]"), nErrorCode);
					throw szBuffer;
				}
			}

			//设置变量
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
		throw TEXT("连接产生未知异常错误");
	}

	return false;
}

//发送请求
bool CDownLoadSocket::SendRequestData(VOID * const pData, WORD wDataSize)
{
	//效验参数
	ASSERT((wDataSize!=0)&&(pData!=NULL));
	if ((wDataSize==0)||(pData==NULL)) return false;

	//效验状态
	if (m_hSocket==INVALID_SOCKET) return false;
	if (m_cbSocketStatus!=SOCKET_STATUS_CONNECT) return false;

	//设置变量
	m_wHeadSize=0;
	m_wRequestID++;
	m_bHeadValid=false;

	//发送数据
	WORD wSended=0;
	while (wSended<wDataSize)
	{
		INT nErrorCode=send(m_hSocket,(LPCSTR)pData+wSended,wDataSize-wSended,0);
		if (nErrorCode==SOCKET_ERROR) return (WSAGetLastError()==WSAEWOULDBLOCK);
		wSended+=nErrorCode;
	}

	return true;
}

//关闭连接
bool CDownLoadSocket::CloseSocket(bool bNotify)
{
	//设置变量
	m_wRequestID++;
	m_bCloseByServer=false;
	m_cbSocketStatus=SOCKET_STATUS_IDLE;

	//关闭判断
	if (m_hSocket!=INVALID_SOCKET)
	{
		//关闭连接
		WSAAsyncSelect(m_hSocket,GetSafeHwnd(),WM_SOCKET_NOTIFY_MSG,0);
		closesocket(m_hSocket);
		m_hSocket=INVALID_SOCKET;

		//事件通知
		if (bNotify==true)
		{
			ASSERT(m_pIDownLoadSocketSink!=NULL);
			m_pIDownLoadSocketSink->OnSocketClose(m_bCloseByServer);
		}
	}

	return true;
}

//连接代理
bool CDownLoadSocket::ConnectProxyServer()
{
	//效验状态
	ASSERT(m_ProxyServerType!=ProxyType_None);
	if (m_ProxyServerType==ProxyType_None) throw TEXT("不存在代理服务器连接信息");

	//连接代理
	if (m_ProxyServerType!=ProxyType_None)
	{
		//代理地址
		SOCKADDR_IN SocketAddrProxy;
		ZeroMemory(&SocketAddrProxy,sizeof(SocketAddrProxy));
		SocketAddrProxy.sin_family=AF_INET;
		SocketAddrProxy.sin_port=htons(m_ProxyServerInfo.wProxyPort);
		SocketAddrProxy.sin_addr.S_un.S_addr=TranslateAddr(m_ProxyServerInfo.szProxyServer);

		//连接代理
		INT nErrorCode=connect(m_hSocket,(sockaddr *)&SocketAddrProxy,sizeof(SocketAddrProxy));
		if (nErrorCode==SOCKET_ERROR) throw TEXT("代理服务器连接失败");

		//发送数据
		if (m_ProxyServerType==ProxyType_Socks5)
		{
			//变量定义
			INT nOff=2;
			CHAR cbDataBuffer[256]={5,1};

			//发送请求
			cbDataBuffer[nOff++]=(m_ProxyServerInfo.szUserName[0]==0)?0:2;
			if (send(m_hSocket,cbDataBuffer,nOff,0)!=nOff) throw TEXT("请求代理服务器失败");

			//接受请求
			INT nResult=recv(m_hSocket,cbDataBuffer,2,0);
			if (nResult!=2) throw TEXT("接收代理服务器应答失败");

			//分析数据
			if (cbDataBuffer[0]!=5) throw TEXT("接收代理服务器应答失败");
			if ((cbDataBuffer[1]!=0)&&(cbDataBuffer[1]!=2)) throw TEXT("代理服务器拒绝请求");

			//身份验证
			nOff=0;
			if (cbDataBuffer[1]==2)
			{
				//变量定义
				INT nStringLen=0;
				CT2CA strUserName(m_ProxyServerInfo.szUserName);
				CT2CA strPassword(m_ProxyServerInfo.szPassword);

				//登录帐号
				cbDataBuffer[nOff++]=1;
				nStringLen=lstrlenA(strUserName);
				cbDataBuffer[nOff++]=(CHAR)nStringLen;
				CopyMemory(cbDataBuffer+nOff,(LPCSTR)strUserName,nStringLen);
				nOff+=nStringLen;

				//登录密码
				nStringLen=lstrlenA(strPassword);
				cbDataBuffer[nOff++]=(CHAR)nStringLen;
				CopyMemory(cbDataBuffer+nOff,(LPCSTR)strPassword,nStringLen);
				nOff+=nStringLen;

				//发送数据
				INT nSend=send(m_hSocket,cbDataBuffer,nOff,0);
				if (nSend!=nOff) throw TEXT("向代理服务器发送身份验证失败");

				//接受数据
				INT nRecv=recv(m_hSocket,cbDataBuffer,2,0);
				if (nRecv!=2) throw TEXT("接收代理服务器身份验证应答失败");

				//数据分析
				if (cbDataBuffer[0]!=1) throw TEXT("通往代理服务器帐户信息可能有错");
				if (cbDataBuffer[1]!=0) throw TEXT("请重新输入您通向代理服务器的用户名和密码");
			}
		}
	}

	return true;
}

//地址解释
DWORD CDownLoadSocket::TranslateAddr(LPCTSTR pszServerAddr)
{
	//转化地址
	CT2CA ServerAddr(pszServerAddr);
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

//网络读取
LRESULT CDownLoadSocket::OnSocketNotifyRead(WPARAM wParam, LPARAM lParam)
{
	try
	{
		//读取包头
		if (m_bHeadValid==false)
		{
			//读取数据
			INT nResult=recv(m_hSocket,m_szHeadData+m_wHeadSize,sizeof(m_szHeadData)-m_wHeadSize,0);
			if (nResult==SOCKET_ERROR) throw TEXT("网络连接关闭，读取数据失败");

			//搜索头尾
			WORD wRequestID=m_wRequestID;
			WORD wWholeSize=m_wHeadSize+nResult;
			LPCSTR pszFinality=strstr((LPCSTR)m_szHeadData,"\r\n\r\n");

			//完成判断
			if (pszFinality!=NULL)
			{
				//设置变量
				m_bHeadValid=true;
				m_wHeadSize=(WORD)(pszFinality-m_szHeadData)+(WORD)strlen("\r\n\r\n");

				//提取状态
				INT nStatusCode=0;
				for (WORD i=0;i<m_wHeadSize;i++)
				{
					if (m_szHeadData[i]==' ')
					{
						//变量定义
						WORD nStatusOff=0;
						CHAR szStatusString[16]="";

						//提取字符
						do
						{
							szStatusString[nStatusOff++]=m_szHeadData[i++];
						} while (m_szHeadData[i]!=' ');

						//设置变量
						szStatusString[nStatusOff]=0;
						nStatusCode=atol(szStatusString);

						break;
					}
				}

				//包头事件
				ASSERT(m_pIDownLoadSocketSink!=NULL);
				m_pIDownLoadSocketSink->OnHttpDataHead(m_szHeadData,m_wHeadSize,nStatusCode);

				//流包事件
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
			//读取数据
			CHAR cbReadBuffer[4096];
			INT nReadSize=recv(m_hSocket,cbReadBuffer,sizeof(cbReadBuffer),0);
			if (nReadSize==SOCKET_ERROR) throw TEXT("网络连接关闭，读取数据失败");

			//事件通知
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

//网络关闭
LRESULT CDownLoadSocket::OnSocketNotifyClose(WPARAM wParam, LPARAM lParam)
{
	//设置变量
	m_bCloseByServer=true;

	//关闭连接
	CloseSocket(true);

	return 1;
}

//网络连接
LRESULT CDownLoadSocket::OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam)
{
	//获取错误
	INT nErrorCode=WSAGETSELECTERROR(lParam);

	//判断状态
	if (nErrorCode==0) m_cbSocketStatus=SOCKET_STATUS_CONNECT;
	else CloseSocket(false);

	//发送通知
	ASSERT(m_pIDownLoadSocketSink!=NULL);
	m_pIDownLoadSocketSink->OnSocketConnect(nErrorCode);

	return 1;
}

//网络消息
LRESULT	CDownLoadSocket::OnSocketNotify(WPARAM wParam, LPARAM lParam)
{
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:		//数据读取
		{ 
			return OnSocketNotifyRead(wParam,lParam); 
		}
	/*case FD_WRITE:		//网络发送
		{
			return OnSocketNotifyWrite(wParam,lParam);
		}*/
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

//////////////////////////////////////////////////////////////////////////
