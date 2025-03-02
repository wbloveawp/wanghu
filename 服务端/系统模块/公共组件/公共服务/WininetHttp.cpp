#include "stdafx.h"
#include "WininetHttp.h"


///////////////////////////////////////////////////////////////////////////////////////////
//常量定义
#define READ_BUFFER_SIZE    4096									//缓冲大小

///////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CWininetHttp::CWininetHttp()
{
	//设置变量
	m_hSession=NULL;
	m_hConnect=NULL;
	m_hRequest=NULL;
	m_hiError=Hi_Success;
}

//析构函数
CWininetHttp::~CWininetHttp()
{
	Release();
}

//请求网页
 CStringA CWininetHttp::RequestWebPage(LPCSTR pszUrl, emHttpRequest hrType, LPCSTR pszHeader, LPCSTR pszPostData)
{
	//变量定义
	CStringA strResult="";

	try
	{
		//校验参数
		if (pszUrl==NULL || strlen(pszUrl)==0)
		{
			throw Hi_ParamErr;
		}

		//释放句柄
		Release();

		//初始化
		m_hSession=InternetOpenA("Http-connect",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,NULL);
		if (m_hSession==NULL)
		{
			throw Hi_InitErr;
		}
		
		//变量定义
		CStringA strHostName="";
		CStringA strPagePath="";
		INTERNET_PORT iPort=INTERNET_DEFAULT_HTTP_PORT;	

		//解析URL
		if (ParseURLWeb(pszUrl, strHostName, strPagePath, iPort) == false)
		{
			throw Hi_UrlErr;
		}

		//建立连接
		m_hConnect=InternetConnectA(m_hSession,strHostName,iPort,NULL,NULL,INTERNET_SERVICE_HTTP,NULL,NULL);
		if (m_hConnect==NULL)
		{
			throw Hi_ConnectErr;
		}

		//请求类型
		LPCSTR pszRequestType="GET";
		if (hrType=Hr_Post)
		{
			pszRequestType="POST";
		}

		//请求页面
		m_hRequest=HttpOpenRequestA(m_hConnect, pszRequestType,strPagePath,"HTTP/1.1",NULL,NULL,INTERNET_FLAG_RELOAD,NULL);
		if (m_hRequest==NULL)
		{
			throw Hi_InitErr;
		}

		//发送请求
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

		//校验结果
		if (!bRet)
		{
			throw Hi_SendErr;
		}

		//请求信息
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
			//读取文件
			bRet=InternetReadFile(m_hRequest,szBuffer,READ_BUFFER_SIZE,&dwReadSize);
			if (!bRet||(dwReadSize==0))
			{
				break;
			}

			//设置结束符
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

//关闭句柄
void CWininetHttp::Release()
{
	//设置变量
	m_hiError = Hi_Success;

	//释放句柄
	ReleaseHandle(m_hSession);
	ReleaseHandle(m_hConnect);
	ReleaseHandle(m_hRequest);	
}

//释放句柄
void CWininetHttp::ReleaseHandle(HINTERNET & hInternet)
{
	if (hInternet!=NULL)
	{
		InternetCloseHandle(hInternet);
		hInternet=NULL;
	}
}
	
// 解析URL地址
bool CWininetHttp::ParseURLWeb(LPCSTR pszUrl, CStringA& strHostName, CStringA& strPagePath, WORD& wPort)
{
	//变量定义
	wPort=80;
	CStringA strUrl(pszUrl);

	//头部定义
	LPCSTR pszHttp="http://";
	LPCSTR pszHttps="https://";
	
	//查找字符
	LPCSTR pszHeader=pszHttps;
	int nPos=strUrl.Find(pszHeader);
	if (nPos==-1)
	{
		pszHeader=pszHttp;
		nPos=strUrl.Find(pszHeader);
		if (nPos==-1) return false;		
	}

	strUrl=strUrl.Mid(nPos+lstrlenA(pszHeader),strUrl.GetLength()-nPos-lstrlenA(pszHeader));	

	//查找字符
	nPos=strUrl.Find('/');
	if (nPos==-1)
	{
		strHostName=strUrl;
	}
	else
	{
		strHostName=strUrl.Left(nPos);
	}

	//查找字符
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