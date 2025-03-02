#include "stdafx.h"
#include "WininetHttp.h"


///////////////////////////////////////////////////////////////////////////////////////////
//��������
#define READ_BUFFER_SIZE    4096									//�����С

///////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CWininetHttp::CWininetHttp()
{
	//���ñ���
	m_hSession=NULL;
	m_hConnect=NULL;
	m_hRequest=NULL;
	m_hiError=Hi_Success;
}

//��������
CWininetHttp::~CWininetHttp()
{
	Release();
}

//������ҳ
 CStringA CWininetHttp::RequestWebPage(LPCSTR pszUrl, emHttpRequest hrType, LPCSTR pszHeader, LPCSTR pszPostData)
{
	//��������
	CStringA strResult="";

	try
	{
		//У�����
		if (pszUrl==NULL || strlen(pszUrl)==0)
		{
			throw Hi_ParamErr;
		}

		//�ͷž��
		Release();

		//��ʼ��
		m_hSession=InternetOpenA("Http-connect",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,NULL);
		if (m_hSession==NULL)
		{
			throw Hi_InitErr;
		}
		
		//��������
		CStringA strHostName="";
		CStringA strPagePath="";
		INTERNET_PORT iPort=INTERNET_DEFAULT_HTTP_PORT;	

		//����URL
		if (ParseURLWeb(pszUrl, strHostName, strPagePath, iPort) == false)
		{
			throw Hi_UrlErr;
		}

		//��������
		m_hConnect=InternetConnectA(m_hSession,strHostName,iPort,NULL,NULL,INTERNET_SERVICE_HTTP,NULL,NULL);
		if (m_hConnect==NULL)
		{
			throw Hi_ConnectErr;
		}

		//��������
		LPCSTR pszRequestType="GET";
		if (hrType=Hr_Post)
		{
			pszRequestType="POST";
		}

		//����ҳ��
		m_hRequest=HttpOpenRequestA(m_hConnect, pszRequestType,strPagePath,"HTTP/1.1",NULL,NULL,INTERNET_FLAG_RELOAD,NULL);
		if (m_hRequest==NULL)
		{
			throw Hi_InitErr;
		}

		//��������
		BOOL bRet=FALSE;
		DWORD dwHeaderSize=(pszHeader==NULL)?0:lstrlenA(pszHeader);		
		if (hrType==Hr_Get)
		{
			bRet=HttpSendRequestA(m_hRequest,pszHeader,dwHeaderSize,NULL,0);
		}
		else
		{
			DWORD dwPostSize=(pszPostData==NULL)?0:lstrlenA(pszPostData);
			bRet=HttpSendRequestA(m_hRequest,pszHeader,dwHeaderSize,(LPVOID)pszPostData, dwPostSize);
		}

		//У����
		if (!bRet)
		{
			throw Hi_SendErr;
		}

		//������Ϣ
		DWORD dwReadSize=READ_BUFFER_SIZE;
		CHAR szBuffer[READ_BUFFER_SIZE+1]={0};		
		if (!HttpQueryInfo(m_hRequest,HTTP_QUERY_RAW_HEADERS,szBuffer,&dwReadSize,NULL))
		{
			throw Hi_QueryErr;
		}
		if (strstr(szBuffer,"404")!=NULL)
		{
			throw Hi_404;
		}

		while (true)
		{
			//��ȡ�ļ�
			bRet=InternetReadFile(m_hRequest,szBuffer,READ_BUFFER_SIZE,&dwReadSize);
			if (!bRet||(dwReadSize==0))
			{
				break;
			}

			//���ý�����
			szBuffer[dwReadSize]='\0';
			strResult.Append(szBuffer);
		}
	}
	catch (emHttpInterfaceError hiError)
	{
		m_hiError=hiError;
	}

	return strResult;
}

//�رվ��
void CWininetHttp::Release()
{
	//���ñ���
	m_hiError = Hi_Success;

	//�ͷž��
	ReleaseHandle(m_hSession);
	ReleaseHandle(m_hConnect);
	ReleaseHandle(m_hRequest);	
}

//�ͷž��
void CWininetHttp::ReleaseHandle(HINTERNET & hInternet)
{
	if (hInternet!=NULL)
	{
		InternetCloseHandle(hInternet);
		hInternet=NULL;
	}
}
	
// ����URL��ַ
bool CWininetHttp::ParseURLWeb(LPCSTR pszUrl, CStringA& strHostName, CStringA& strPagePath, WORD& wPort)
{
	//��������
	wPort=80;
	CStringA strUrl(pszUrl);

	//ͷ������
	LPCSTR pszHttp="http://";
	LPCSTR pszHttps="https://";
	
	//�����ַ�
	LPCSTR pszHeader=pszHttps;
	int nPos=strUrl.Find(pszHeader);
	if (nPos==-1)
	{
		pszHeader=pszHttp;
		nPos=strUrl.Find(pszHeader);
		if (nPos==-1) return false;		
	}

	strUrl=strUrl.Mid(nPos+lstrlenA(pszHeader),strUrl.GetLength()-nPos-lstrlenA(pszHeader));	

	//�����ַ�
	nPos=strUrl.Find('/');
	if (nPos==-1)
	{
		strHostName=strUrl;
	}
	else
	{
		strHostName=strUrl.Left(nPos);
	}

	//�����ַ�
	int nPos1=strHostName.Find(':');
	if (nPos1!=-1)
	{
		CStringA strPort= strHostName.Mid(nPos1+1,strHostName.GetLength()-nPos1-1);
		strHostName=strHostName.Left(nPos1);
		wPort=(WORD)atoi(strPort);
	}

	if (nPos!=-1)
	{
		strPagePath=strUrl.Mid(nPos,strUrl.GetLength()-nPos);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////