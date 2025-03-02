#include "StdAfx.h"
#include "HttpService.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CHttpService::CHttpService()
{
	//地址变量
	m_lpvBuffer = NULL;
	m_dwBufSize = 0;

	//任务信息
	m_DownLoadStatus=DownLoadStatus_Null;
	ZeroMemory(&m_RequestInfo,sizeof(m_RequestInfo));

	//文件状态
	m_dwDownLoadSize=0L;
	m_dwTotalFileSize=0L;

	//设置组件
	m_DownLoadSocket.SetDownLoadSocketSink(QUERY_ME_INTERFACE(IUnknownEx));
	m_pIHttpServiceSink = NULL;

	return;
}

//析构函数
CHttpService::~CHttpService()
{
}

//接口查询
VOID * __cdecl CHttpService::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IHttpService,Guid,dwQueryVer);
	QUERYINTERFACE(IDownLoadSocketSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IHttpService,Guid,dwQueryVer);
	return NULL;
}

//设置接口
bool __cdecl CHttpService::SetHttpServiceSink(IHttpServiceSink * pSink)
{
	m_pIHttpServiceSink = pSink;
	return true;
}

//连接请求
bool __cdecl CHttpService::ConnectServer(LPCTSTR lpURL)
{
	//状态效验
	ASSERT(m_DownLoadStatus==DownLoadStatus_Null);
	if (m_DownLoadStatus!=DownLoadStatus_Null) return false;

	//地址变量
	m_lpvBuffer = NULL;
	m_dwBufSize = 0;

	//任务信息
	m_DownLoadStatus=DownLoadStatus_Connect;
	ZeroMemory(&m_RequestInfo,sizeof(m_RequestInfo));

	//文件信息
	m_dwDownLoadSize=0L;
	m_dwTotalFileSize=0L;

	//获取信息
	INTERNET_SCHEME nScheme=GetDownLoadInfo(lpURL,m_RequestInfo);

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
		ASSERT(m_pIHttpServiceSink!=NULL);
		m_pIHttpServiceSink->OnHttpDataException(DownLoadError);

		//停止下载
		StopService();

		return false;
	}

	return false;
}

//停止服务
VOID __cdecl CHttpService::StopService()
{
	//任务信息
	m_DownLoadStatus=DownLoadStatus_Null;
	ZeroMemory(&m_RequestInfo,sizeof(m_RequestInfo));

	//文件状态
	m_dwDownLoadSize=0L;
	m_dwTotalFileSize=0L;

	//设置组件
	m_HttpHeadTranslate.CleanData();
	m_DownLoadSocket.CloseSocket(false);
}

//连接消息
bool __cdecl CHttpService::OnSocketConnect(INT nErrorCode)
{
	//发送请求
	SendHttpRequest(m_RequestInfo);

	return true;
}

//关闭消息
bool __cdecl CHttpService::OnSocketClose(bool bCloseByServer)
{
	//停止请求
	StopService();

	//通知回调
	if( bCloseByServer && m_pIHttpServiceSink )
		m_pIHttpServiceSink->OnHttpSocketClose(bCloseByServer);

	return true;
}

//数据包流
VOID __cdecl CHttpService::OnHttpDataStream(const VOID * pStreamData, WORD wStreamSize)
{
	//效验状态
	ASSERT(m_DownLoadStatus==DownLoadStatus_Service);
	if (m_DownLoadStatus!=DownLoadStatus_Service) return;
	ASSERT(m_lpvBuffer&&m_dwBufSize>0);
	if( !m_lpvBuffer || m_dwBufSize == 0 )
	{
		//事件通知
		ASSERT(m_pIHttpServiceSink);
		m_pIHttpServiceSink->OnHttpDataException(DownLoadError_FileSystem);

		//停止下载
		StopService();
	}

	//写入缓冲
	DWORD dwCopySize = __min(wStreamSize,m_dwBufSize-m_dwDownLoadSize);
	CopyMemory( (CHAR *)m_lpvBuffer+m_dwDownLoadSize,pStreamData,dwCopySize );
	m_dwDownLoadSize+=dwCopySize;

	//完成判断
	if (m_dwDownLoadSize==m_dwTotalFileSize||m_dwDownLoadSize==m_dwBufSize)
	{
		//设置变量
		m_DownLoadStatus=DownLoadStatus_Conclude;
		DWORD dwDownLoadSize = m_dwDownLoadSize;

		//停止下载
		StopService();

		//事件通知
		ASSERT(m_pIHttpServiceSink);
		m_pIHttpServiceSink->OnHttpDataComplete(dwDownLoadSize);
	}

	return;
}

//数据包头
VOID __cdecl CHttpService::OnHttpDataHead(const VOID * pcbHeadData, WORD wHeadSize, INT nStatusCode)
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

			//获取缓冲
			ASSERT(m_pIHttpServiceSink);
			if( !m_pIHttpServiceSink )
			{
				StopService();
				return;
			}

			m_dwBufSize = m_pIHttpServiceSink->OnHttpRequestBuffer(m_dwTotalFileSize,&m_lpvBuffer);

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
			OnSocketClose(true);

			return;
		}
	}

	return;
}

//发送请求
bool CHttpService::SendHttpRequest(const tagRequestInfo & RequestInfo)
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
INTERNET_SCHEME CHttpService::GetDownLoadInfo(LPCTSTR pszDownUrl, tagRequestInfo & RequestInfo)
{
	//变量定义
	TCHAR szDownLoadUrl[MAX_PATH]=TEXT("");
	ZeroMemory(&RequestInfo,sizeof(RequestInfo));

	//规范地址
	DWORD dwMaxLength=CountArray(szDownLoadUrl);
	DWORD dwFlags=ICU_BROWSER_MODE;
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
DECLARE_CREATE_MODULE(HttpService);

//////////////////////////////////////////////////////////////////////////
