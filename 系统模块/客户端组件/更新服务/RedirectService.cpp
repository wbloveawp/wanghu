#include "StdAfx.h"
#include "RedirectService.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CRedirectService::CRedirectService()
{
	//地址变量
	m_szRedirectServer[0] = 0;

	//任务信息
	m_DownLoadStatus=DownLoadStatus_Null;
	ZeroMemory(&m_RequestInfo,sizeof(m_RequestInfo));

	//文件状态
	m_dwDownLoadSize=0L;
	m_dwTotalFileSize=0L;
	m_szDownLoadBuffer[0] = 0;

	//设置组件
	m_DownLoadSocket.SetDownLoadSocketSink(QUERY_ME_INTERFACE(IUnknownEx));
	m_pIRedirectServiceSink = NULL;

	return;
}

//析构函数
CRedirectService::~CRedirectService()
{
}

//接口查询
VOID * __cdecl CRedirectService::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IRedirectService,Guid,dwQueryVer);
	QUERYINTERFACE(IDownLoadSocketSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IRedirectService,Guid,dwQueryVer);
	return NULL;
}

//设置接口
bool __cdecl CRedirectService::SetRedirectServiceSink(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if (pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IRedirectServiceSink)!=NULL);
		m_pIRedirectServiceSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IRedirectServiceSink);

		//成功判断
		if (m_pIRedirectServiceSink==NULL) return false;
	}
	else m_pIRedirectServiceSink=NULL;

	return true;
}

//请求定向
bool __cdecl CRedirectService::RequestRedirect(LPCTSTR lpURL)
{
	//状态效验
	ASSERT(m_DownLoadStatus==DownLoadStatus_Null);
	if (m_DownLoadStatus!=DownLoadStatus_Null) return false;

	//地址变量
	m_szRedirectServer[0] = 0;

	//任务信息
	m_DownLoadStatus=DownLoadStatus_Connect;
	ZeroMemory(&m_RequestInfo,sizeof(m_RequestInfo));

	//文件信息
	m_dwDownLoadSize=0L;
	m_dwTotalFileSize=0L;
	m_szDownLoadBuffer[0] = 0;

	//请求连接
	bool bSuccess=ConnectServer(lpURL);

	return bSuccess;
}

//停止定向
VOID __cdecl CRedirectService::StopRedirect()
{
	//任务信息
	m_DownLoadStatus=DownLoadStatus_Null;
	ZeroMemory(&m_RequestInfo,sizeof(m_RequestInfo));

	//文件状态
	m_dwDownLoadSize=0L;
	m_dwTotalFileSize=0L;
	m_szDownLoadBuffer[0] = 0;

	//设置组件
	m_HttpHeadTranslate.CleanData();
	m_DownLoadSocket.CloseSocket(false);
}

//连接消息
bool __cdecl CRedirectService::OnSocketConnect(INT nErrorCode)
{
	//发送请求
	SendHttpRequest(m_RequestInfo);

	return true;
}

//关闭消息
bool __cdecl CRedirectService::OnSocketClose(bool bCloseByServer)
{
	//停止请求
	StopRedirect();

	return true;
}

//数据包流
VOID __cdecl CRedirectService::OnHttpDataStream(const VOID * pStreamData, WORD wStreamSize)
{
	//效验状态
	ASSERT(m_DownLoadStatus==DownLoadStatus_Service);
	if (m_DownLoadStatus!=DownLoadStatus_Service) return;

	ASSERT(m_dwDownLoadSize+wStreamSize<=CountArray(m_szDownLoadBuffer));
	if( m_dwDownLoadSize+wStreamSize>CountArray(m_szDownLoadBuffer) )
	{
		//事件通知
		ASSERT(m_pIRedirectServiceSink);
		m_pIRedirectServiceSink->OnRedirectException(DownLoadError_HttpServer);

		//停止下载
		StopRedirect();

		return;
	}
	//写入缓冲
	CopyMemory( &m_szDownLoadBuffer[m_dwDownLoadSize],pStreamData,wStreamSize );
	m_dwDownLoadSize+=wStreamSize;

	//完成判断
	if (m_dwDownLoadSize==m_dwTotalFileSize)
	{
		//设置变量
		m_DownLoadStatus=DownLoadStatus_Conclude;

		//设置变量
		m_szDownLoadBuffer[m_dwDownLoadSize] = 0;
		lstrcpyn( m_szRedirectServer,(LPCTSTR)CA2CT(m_szDownLoadBuffer),CountArray(m_szRedirectServer) );

		//事件通知
		ASSERT(m_pIRedirectServiceSink);
		m_pIRedirectServiceSink->OnRedirectComplete(m_szRedirectServer,0);

		//停止下载
		StopRedirect();
	}

	return;
}

//数据包头
VOID __cdecl CRedirectService::OnHttpDataHead(const VOID * pcbHeadData, WORD wHeadSize, INT nStatusCode)
{
	switch (nStatusCode)
	{
	case HTTP_STATUS_OK:				//获取成功
		{
			//设置变量
			m_DownLoadStatus=DownLoadStatus_Service;
			m_HttpHeadTranslate.SetHeadData((BYTE *)pcbHeadData,wHeadSize);

			//文件信息
			m_dwDownLoadSize=0L;
			m_dwTotalFileSize=m_HttpHeadTranslate.GetTotalFileSize();

			return;
		}
	case HTTP_STATUS_REDIRECT:			//目标跳转
	case HTTP_STATUS_REDIRECT_METHOD:	//目标跳转
		{
			//提取位置
			LPCSTR pszLocation=strstr((char *)pcbHeadData,"Location: ");
			if (pszLocation==NULL) m_DownLoadSocket.CloseSocket(true);

			//变量定义
			char szLocation[MAX_PATH]="";
			WORD wStartPos=(WORD)strlen("Location: ");
			
			//构造地址
			WORD wOff=0;
			while (pszLocation[wStartPos+wOff]!='\r')
			{
				//效验数据
				ASSERT(wOff<CountArray(szLocation));
				if (wOff>=CountArray(szLocation)) m_DownLoadSocket.CloseSocket(true);

				//设置字符
				WORD wIndex=wOff++;
				szLocation[wIndex]=pszLocation[wStartPos+wIndex];
			}
			szLocation[wOff]=0;

			//转向分析
			if (szLocation[0]=='/')
			{
				//设置请求
				CA2CT strLocation(szLocation);
				lstrcpyn(m_RequestInfo.szRequestPath,(LPCTSTR)strLocation,CountArray(m_RequestInfo.szRequestPath));

				//发送请求
				SendHttpRequest(m_RequestInfo);
			}
			else
			{
				//关闭连接
				m_DownLoadSocket.CloseSocket(false);

				//请求连接
				CA2CT strLocation(szLocation);
				ConnectServer((LPCTSTR)strLocation);
			}

			return;
		}
	default:							//默认处理
		{
			//关闭连接
			m_DownLoadSocket.CloseSocket(true);

			return;
		}
	}

	return;
}

//请求连接
bool CRedirectService::ConnectServer(LPCTSTR pszDownLoadUrl)
{
	//获取信息
	INTERNET_SCHEME nScheme=GetDownLoadInfo(pszDownLoadUrl,m_RequestInfo);

	//类型分析
	try
	{
		switch (nScheme)
		{
		case INTERNET_SERVICE_HTTP:		//HTTP 下载
			{
				//发起连接
				try
				{
					m_DownLoadSocket.Connect(m_RequestInfo.szHostName,m_RequestInfo.wHostPort);
				}
				catch (...)
				{
					throw DownLoadError_SocketConnect;
				}

				break;
			}
		default:						//默认处理
			{
				//参数错误
				throw DownLoadError_Parameter; 
			}
		}

		return true;
	}
	catch (enDownLoadError DownLoadError)
	{
		//设置变量
		m_DownLoadStatus=DownLoadStatus_Error;

		//事件通知
		ASSERT(m_pIRedirectServiceSink!=NULL);
		m_pIRedirectServiceSink->OnRedirectException(DownLoadError);

		//停止下载
		StopRedirect();

		return false;
	}

	return false;
}

//发送请求
bool CRedirectService::SendHttpRequest(const tagRequestInfo & RequestInfo)
{
	//变量定义
	char szHttpRequest[1024]="";
	CT2CA strHostName(RequestInfo.szHostName);
	CT2CA strRequestPath(RequestInfo.szRequestPath);

	//构造请求
	_snprintf_s(szHttpRequest,CountArray(szHttpRequest),"GET /%s HTTP/1.1\r\nHost:%s\r\nAccept:*/*\r\n"
		"User-Agent:Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)\r\nConnection:Keep-Alive\r\n\r\n",
		(LPCSTR)strRequestPath,(LPCSTR)strHostName);

	//发送请求
	m_DownLoadSocket.SendRequestData(szHttpRequest,(WORD)strlen(szHttpRequest));

	return true;
}

//获取信息
INTERNET_SCHEME CRedirectService::GetDownLoadInfo(LPCTSTR pszDownUrl, tagRequestInfo & RequestInfo)
{
	//变量定义
	TCHAR szDownLoadUrl[MAX_PATH]=TEXT("");
	ZeroMemory(&RequestInfo,sizeof(RequestInfo));

	//规范地址
	DWORD dwMaxLength=CountArray(szDownLoadUrl);
	DWORD dwFlags=ICU_BROWSER_MODE|ICU_NO_ENCODE|ICU_NO_META|ICU_ENCODE_SPACES_ONLY;
	if (InternetCanonicalizeUrl(pszDownUrl,szDownLoadUrl,&dwMaxLength,dwFlags)==FALSE) return INTERNET_SCHEME_UNKNOWN;

	//拆分地址
	URL_COMPONENTS Components;
	ZeroMemory(&Components,sizeof(Components));
	Components.dwStructSize=sizeof(Components);
	Components.lpszHostName=RequestInfo.szHostName;
	Components.lpszUrlPath=RequestInfo.szRequestPath;
	Components.dwHostNameLength=CountArray(RequestInfo.szHostName);
	Components.dwUrlPathLength=CountArray(RequestInfo.szRequestPath);
	if (InternetCrackUrl(szDownLoadUrl,0,0,&Components)==FALSE) return INTERNET_SCHEME_UNKNOWN;

	//设置变量
	RequestInfo.wHostPort=Components.nPort;

	return Components.nScheme;
}

//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(RedirectService);

//////////////////////////////////////////////////////////////////////////
