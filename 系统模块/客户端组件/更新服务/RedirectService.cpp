#include "StdAfx.h"
#include "RedirectService.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CRedirectService::CRedirectService()
{
	//��ַ����
	m_szRedirectServer[0] = 0;

	//������Ϣ
	m_DownLoadStatus=DownLoadStatus_Null;
	ZeroMemory(&m_RequestInfo,sizeof(m_RequestInfo));

	//�ļ�״̬
	m_dwDownLoadSize=0L;
	m_dwTotalFileSize=0L;
	m_szDownLoadBuffer[0] = 0;

	//�������
	m_DownLoadSocket.SetDownLoadSocketSink(QUERY_ME_INTERFACE(IUnknownEx));
	m_pIRedirectServiceSink = NULL;

	return;
}

//��������
CRedirectService::~CRedirectService()
{
}

//�ӿڲ�ѯ
VOID * __cdecl CRedirectService::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IRedirectService,Guid,dwQueryVer);
	QUERYINTERFACE(IDownLoadSocketSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IRedirectService,Guid,dwQueryVer);
	return NULL;
}

//���ýӿ�
bool __cdecl CRedirectService::SetRedirectServiceSink(IUnknownEx * pIUnknownEx)
{
	//���ýӿ�
	if (pIUnknownEx!=NULL)
	{
		//��ѯ�ӿ�
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IRedirectServiceSink)!=NULL);
		m_pIRedirectServiceSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IRedirectServiceSink);

		//�ɹ��ж�
		if (m_pIRedirectServiceSink==NULL) return false;
	}
	else m_pIRedirectServiceSink=NULL;

	return true;
}

//������
bool __cdecl CRedirectService::RequestRedirect(LPCTSTR lpURL)
{
	//״̬Ч��
	ASSERT(m_DownLoadStatus==DownLoadStatus_Null);
	if (m_DownLoadStatus!=DownLoadStatus_Null) return false;

	//��ַ����
	m_szRedirectServer[0] = 0;

	//������Ϣ
	m_DownLoadStatus=DownLoadStatus_Connect;
	ZeroMemory(&m_RequestInfo,sizeof(m_RequestInfo));

	//�ļ���Ϣ
	m_dwDownLoadSize=0L;
	m_dwTotalFileSize=0L;
	m_szDownLoadBuffer[0] = 0;

	//��������
	bool bSuccess=ConnectServer(lpURL);

	return bSuccess;
}

//ֹͣ����
VOID __cdecl CRedirectService::StopRedirect()
{
	//������Ϣ
	m_DownLoadStatus=DownLoadStatus_Null;
	ZeroMemory(&m_RequestInfo,sizeof(m_RequestInfo));

	//�ļ�״̬
	m_dwDownLoadSize=0L;
	m_dwTotalFileSize=0L;
	m_szDownLoadBuffer[0] = 0;

	//�������
	m_HttpHeadTranslate.CleanData();
	m_DownLoadSocket.CloseSocket(false);
}

//������Ϣ
bool __cdecl CRedirectService::OnSocketConnect(INT nErrorCode)
{
	//��������
	SendHttpRequest(m_RequestInfo);

	return true;
}

//�ر���Ϣ
bool __cdecl CRedirectService::OnSocketClose(bool bCloseByServer)
{
	//ֹͣ����
	StopRedirect();

	return true;
}

//���ݰ���
VOID __cdecl CRedirectService::OnHttpDataStream(const VOID * pStreamData, WORD wStreamSize)
{
	//Ч��״̬
	ASSERT(m_DownLoadStatus==DownLoadStatus_Service);
	if (m_DownLoadStatus!=DownLoadStatus_Service) return;

	ASSERT(m_dwDownLoadSize+wStreamSize<=CountArray(m_szDownLoadBuffer));
	if( m_dwDownLoadSize+wStreamSize>CountArray(m_szDownLoadBuffer) )
	{
		//�¼�֪ͨ
		ASSERT(m_pIRedirectServiceSink);
		m_pIRedirectServiceSink->OnRedirectException(DownLoadError_HttpServer);

		//ֹͣ����
		StopRedirect();

		return;
	}
	//д�뻺��
	CopyMemory( &m_szDownLoadBuffer[m_dwDownLoadSize],pStreamData,wStreamSize );
	m_dwDownLoadSize+=wStreamSize;

	//����ж�
	if (m_dwDownLoadSize==m_dwTotalFileSize)
	{
		//���ñ���
		m_DownLoadStatus=DownLoadStatus_Conclude;

		//���ñ���
		m_szDownLoadBuffer[m_dwDownLoadSize] = 0;
		lstrcpyn( m_szRedirectServer,(LPCTSTR)CA2CT(m_szDownLoadBuffer),CountArray(m_szRedirectServer) );

		//�¼�֪ͨ
		ASSERT(m_pIRedirectServiceSink);
		m_pIRedirectServiceSink->OnRedirectComplete(m_szRedirectServer,0);

		//ֹͣ����
		StopRedirect();
	}

	return;
}

//���ݰ�ͷ
VOID __cdecl CRedirectService::OnHttpDataHead(const VOID * pcbHeadData, WORD wHeadSize, INT nStatusCode)
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
			m_DownLoadSocket.CloseSocket(true);

			return;
		}
	}

	return;
}

//��������
bool CRedirectService::ConnectServer(LPCTSTR pszDownLoadUrl)
{
	//��ȡ��Ϣ
	INTERNET_SCHEME nScheme=GetDownLoadInfo(pszDownLoadUrl,m_RequestInfo);

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
		ASSERT(m_pIRedirectServiceSink!=NULL);
		m_pIRedirectServiceSink->OnRedirectException(DownLoadError);

		//ֹͣ����
		StopRedirect();

		return false;
	}

	return false;
}

//��������
bool CRedirectService::SendHttpRequest(const tagRequestInfo & RequestInfo)
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
INTERNET_SCHEME CRedirectService::GetDownLoadInfo(LPCTSTR pszDownUrl, tagRequestInfo & RequestInfo)
{
	//��������
	TCHAR szDownLoadUrl[MAX_PATH]=TEXT("");
	ZeroMemory(&RequestInfo,sizeof(RequestInfo));

	//�淶��ַ
	DWORD dwMaxLength=CountArray(szDownLoadUrl);
	DWORD dwFlags=ICU_BROWSER_MODE|ICU_NO_ENCODE|ICU_NO_META|ICU_ENCODE_SPACES_ONLY;
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
DECLARE_CREATE_MODULE(RedirectService);

//////////////////////////////////////////////////////////////////////////
