#include "StdAfx.h"
#include "HttpService.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CHttpService::CHttpService()
{
	//��ַ����
	m_lpvBuffer = NULL;
	m_dwBufSize = 0;

	//������Ϣ
	m_DownLoadStatus=DownLoadStatus_Null;
	ZeroMemory(&m_RequestInfo,sizeof(m_RequestInfo));

	//�ļ�״̬
	m_dwDownLoadSize=0L;
	m_dwTotalFileSize=0L;

	//�������
	m_DownLoadSocket.SetDownLoadSocketSink(QUERY_ME_INTERFACE(IUnknownEx));
	m_pIHttpServiceSink = NULL;

	return;
}

//��������
CHttpService::~CHttpService()
{
}

//�ӿڲ�ѯ
VOID * __cdecl CHttpService::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IHttpService,Guid,dwQueryVer);
	QUERYINTERFACE(IDownLoadSocketSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IHttpService,Guid,dwQueryVer);
	return NULL;
}

//���ýӿ�
bool __cdecl CHttpService::SetHttpServiceSink(IHttpServiceSink * pSink)
{
	m_pIHttpServiceSink = pSink;
	return true;
}

//��������
bool __cdecl CHttpService::ConnectServer(LPCTSTR lpURL)
{
	//״̬Ч��
	ASSERT(m_DownLoadStatus==DownLoadStatus_Null);
	if (m_DownLoadStatus!=DownLoadStatus_Null) return false;

	//��ַ����
	m_lpvBuffer = NULL;
	m_dwBufSize = 0;

	//������Ϣ
	m_DownLoadStatus=DownLoadStatus_Connect;
	ZeroMemory(&m_RequestInfo,sizeof(m_RequestInfo));

	//�ļ���Ϣ
	m_dwDownLoadSize=0L;
	m_dwTotalFileSize=0L;

	//��ȡ��Ϣ
	INTERNET_SCHEME nScheme=GetDownLoadInfo(lpURL,m_RequestInfo);

	//���ͷ���
	try
	{
		switch (nScheme)
		{
		case INTERNET_SERVICE_HTTP:		//HTTP ����
			{
				//��������
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
		default:						//Ĭ�ϴ���
			{
				//��������
				throw DownLoadError_Parameter; 
			}
		}

		return true;
	}
	catch (enDownLoadError DownLoadError)
	{
		//���ñ���
		m_DownLoadStatus=DownLoadStatus_Error;

		//�¼�֪ͨ
		ASSERT(m_pIHttpServiceSink!=NULL);
		m_pIHttpServiceSink->OnHttpDataException(DownLoadError);

		//ֹͣ����
		StopService();

		return false;
	}

	return false;
}

//ֹͣ����
VOID __cdecl CHttpService::StopService()
{
	//������Ϣ
	m_DownLoadStatus=DownLoadStatus_Null;
	ZeroMemory(&m_RequestInfo,sizeof(m_RequestInfo));

	//�ļ�״̬
	m_dwDownLoadSize=0L;
	m_dwTotalFileSize=0L;

	//�������
	m_HttpHeadTranslate.CleanData();
	m_DownLoadSocket.CloseSocket(false);
}

//������Ϣ
bool __cdecl CHttpService::OnSocketConnect(INT nErrorCode)
{
	//��������
	SendHttpRequest(m_RequestInfo);

	return true;
}

//�ر���Ϣ
bool __cdecl CHttpService::OnSocketClose(bool bCloseByServer)
{
	//ֹͣ����
	StopService();

	//֪ͨ�ص�
	if( bCloseByServer && m_pIHttpServiceSink )
		m_pIHttpServiceSink->OnHttpSocketClose(bCloseByServer);

	return true;
}

//���ݰ���
VOID __cdecl CHttpService::OnHttpDataStream(const VOID * pStreamData, WORD wStreamSize)
{
	//Ч��״̬
	ASSERT(m_DownLoadStatus==DownLoadStatus_Service);
	if (m_DownLoadStatus!=DownLoadStatus_Service) return;
	ASSERT(m_lpvBuffer&&m_dwBufSize>0);
	if( !m_lpvBuffer || m_dwBufSize == 0 )
	{
		//�¼�֪ͨ
		ASSERT(m_pIHttpServiceSink);
		m_pIHttpServiceSink->OnHttpDataException(DownLoadError_FileSystem);

		//ֹͣ����
		StopService();
	}

	//д�뻺��
	DWORD dwCopySize = __min(wStreamSize,m_dwBufSize-m_dwDownLoadSize);
	CopyMemory( (CHAR *)m_lpvBuffer+m_dwDownLoadSize,pStreamData,dwCopySize );
	m_dwDownLoadSize+=dwCopySize;

	//����ж�
	if (m_dwDownLoadSize==m_dwTotalFileSize||m_dwDownLoadSize==m_dwBufSize)
	{
		//���ñ���
		m_DownLoadStatus=DownLoadStatus_Conclude;
		DWORD dwDownLoadSize = m_dwDownLoadSize;

		//ֹͣ����
		StopService();

		//�¼�֪ͨ
		ASSERT(m_pIHttpServiceSink);
		m_pIHttpServiceSink->OnHttpDataComplete(dwDownLoadSize);
	}

	return;
}

//���ݰ�ͷ
VOID __cdecl CHttpService::OnHttpDataHead(const VOID * pcbHeadData, WORD wHeadSize, INT nStatusCode)
{
	switch (nStatusCode)
	{
	case HTTP_STATUS_OK:				//��ȡ�ɹ�
		{
			//���ñ���
			m_DownLoadStatus=DownLoadStatus_Service;
			m_HttpHeadTranslate.SetHeadData((BYTE *)pcbHeadData,wHeadSize);

			//�ļ���Ϣ
			m_dwDownLoadSize=0L;
			m_dwTotalFileSize=m_HttpHeadTranslate.GetTotalFileSize();

			//��ȡ����
			ASSERT(m_pIHttpServiceSink);
			if( !m_pIHttpServiceSink )
			{
				StopService();
				return;
			}

			m_dwBufSize = m_pIHttpServiceSink->OnHttpRequestBuffer(m_dwTotalFileSize,&m_lpvBuffer);

			return;
		}
	case HTTP_STATUS_REDIRECT:			//Ŀ����ת
	case HTTP_STATUS_REDIRECT_METHOD:	//Ŀ����ת
		{
			//��ȡλ��
			LPCSTR pszLocation=strstr((char *)pcbHeadData,"Location: ");
			if (pszLocation==NULL) m_DownLoadSocket.CloseSocket(true);

			//��������
			char szLocation[MAX_PATH]="";
			WORD wStartPos=(WORD)strlen("Location: ");
			
			//�����ַ
			WORD wOff=0;
			while (pszLocation[wStartPos+wOff]!='\r')
			{
				//Ч������
				ASSERT(wOff<CountArray(szLocation));
				if (wOff>=CountArray(szLocation)) m_DownLoadSocket.CloseSocket(true);

				//�����ַ�
				WORD wIndex=wOff++;
				szLocation[wIndex]=pszLocation[wStartPos+wIndex];
			}
			szLocation[wOff]=0;

			//ת�����
			if (szLocation[0]=='/')
			{
				//��������
				CA2CT strLocation(szLocation);
				lstrcpyn(m_RequestInfo.szRequestPath,(LPCTSTR)strLocation,CountArray(m_RequestInfo.szRequestPath));

				//��������
				SendHttpRequest(m_RequestInfo);
			}
			else
			{
				//�ر�����
				m_DownLoadSocket.CloseSocket(false);

				//��������
				CA2CT strLocation(szLocation);
				ConnectServer((LPCTSTR)strLocation);
			}

			return;
		}
	default:							//Ĭ�ϴ���
		{
			//�ر�����
			OnSocketClose(true);

			return;
		}
	}

	return;
}

//��������
bool CHttpService::SendHttpRequest(const tagRequestInfo & RequestInfo)
{
	//��������
	char szHttpRequest[1024]="";
	CT2CA strHostName(RequestInfo.szHostName);
	CT2CA strRequestPath(RequestInfo.szRequestPath);

	//��������
	_snprintf_s(szHttpRequest,CountArray(szHttpRequest),"GET /%s HTTP/1.1\r\nHost:%s\r\nAccept:*/*\r\n"
		"User-Agent:Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)\r\nConnection:Keep-Alive\r\n\r\n",
		(LPCSTR)strRequestPath,(LPCSTR)strHostName);

	//��������
	m_DownLoadSocket.SendRequestData(szHttpRequest,(WORD)strlen(szHttpRequest));

	return true;
}

//��ȡ��Ϣ
INTERNET_SCHEME CHttpService::GetDownLoadInfo(LPCTSTR pszDownUrl, tagRequestInfo & RequestInfo)
{
	//��������
	TCHAR szDownLoadUrl[MAX_PATH]=TEXT("");
	ZeroMemory(&RequestInfo,sizeof(RequestInfo));

	//�淶��ַ
	DWORD dwMaxLength=CountArray(szDownLoadUrl);
	DWORD dwFlags=ICU_BROWSER_MODE;
	if (InternetCanonicalizeUrl(pszDownUrl,szDownLoadUrl,&dwMaxLength,dwFlags)==FALSE) return INTERNET_SCHEME_UNKNOWN;

	//��ֵ�ַ
	URL_COMPONENTS Components;
	ZeroMemory(&Components,sizeof(Components));
	Components.dwStructSize=sizeof(Components);
	Components.lpszHostName=RequestInfo.szHostName;
	Components.lpszUrlPath=RequestInfo.szRequestPath;
	Components.dwHostNameLength=CountArray(RequestInfo.szHostName);
	Components.dwUrlPathLength=CountArray(RequestInfo.szRequestPath);
	if (InternetCrackUrl(szDownLoadUrl,0,0,&Components)==FALSE) return INTERNET_SCHEME_UNKNOWN;

	//���ñ���
	RequestInfo.wHostPort=Components.nPort;

	return Components.nScheme;
}

//////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(HttpService);

//////////////////////////////////////////////////////////////////////////
