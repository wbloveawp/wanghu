#include "StdAfx.h"
#include "Resource.h"
#include "DownLoadMission.h"
#include "DownLoadService.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CDownLoadMission::CDownLoadMission()
{
	//任务信息
	m_dwMissionID=0L;
	m_DownLoadStatus=DownLoadStatus_Null;

	//文件状态
	m_dwDownLoadSize=0L;
	m_dwTotalFileSize=0L;
	m_pszFileEntityTag=NULL;

	//内核变量
	m_bRelease=false;
	ZeroMemory(&m_RequestInfo,sizeof(m_RequestInfo));
	ZeroMemory(&m_MissionRequest,sizeof(m_MissionRequest));

	//设置组件
	m_DownLoadSocket.SetDownLoadSocketSink(QUERY_ME_INTERFACE(IUnknownEx));

	return;
}

//析构函数
CDownLoadMission::~CDownLoadMission()
{
}

//接口查询
VOID * __cdecl CDownLoadMission::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDownLoadMission,Guid,dwQueryVer);
	QUERYINTERFACE(IDownLoadSocketSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDownLoadMission,Guid,dwQueryVer);
	return NULL;
}

//连接消息
bool __cdecl CDownLoadMission::OnSocketConnect(INT nErrorCode)
{
	//发送请求
	SendHttpRequest(m_RequestInfo);

	return true;
}

//关闭消息
bool __cdecl CDownLoadMission::OnSocketClose(bool bCloseByServer)
{
	//设置变量
	m_DownLoadStatus=DownLoadStatus_Error;

	//事件通知
	ASSERT(m_pIDownLoadMissionSink!=NULL);
	m_pIDownLoadMissionSink->OnDownLoadException(DownLoadError_HttpServer,this);

	//停止下载
	StopDownLoad();

	return true;
}

//数据包流
VOID __cdecl CDownLoadMission::OnHttpDataStream(const VOID * pStreamData, WORD wStreamSize)
{
	//效验状态
	ASSERT(m_DownLoadStatus==DownLoadStatus_Service);
	if (m_DownLoadStatus!=DownLoadStatus_Service) return;

	//写入文件
	m_dwDownLoadSize+=wStreamSize;
	m_BufferFile.Write(pStreamData,wStreamSize);

	//完成判断
	if (m_dwDownLoadSize==m_dwTotalFileSize)
	{
		//设置变量
		m_DownLoadStatus=DownLoadStatus_Conclude;

		//文件名字
		DWORD dwIndex=lstrlen(m_RequestInfo.szRequestPath);
		while ((dwIndex>0)&&(m_RequestInfo.szRequestPath[dwIndex]!=TEXT('/'))) dwIndex--;

		//构造路径
		TCHAR szTargetName[MAX_PATH]=TEXT("");
		TCHAR szBufferName[MAX_PATH]=TEXT("");
		LPCTSTR pszFileName=&m_RequestInfo.szRequestPath[dwIndex+1];
		lstrcpyn(szBufferName,(LPCTSTR)m_BufferFile.GetFilePath(),CountArray(szBufferName));
		// add by humorly 2018/8/3
		// replace func
		_snwprintf_s(szTargetName, CountArray(szTargetName), TEXT("%s\\%s"), m_MissionRequest.szLocalPath, pszFileName);

		//关闭文件
		m_BufferFile.Close();

		//文件转换
		DeleteFile(szTargetName);
		BOOL bSuccess=MoveFile(szBufferName,szTargetName);

		//事件通知
		ASSERT(m_pIDownLoadMissionSink!=NULL);
		m_pIDownLoadMissionSink->OnDownLoadComplete((bSuccess==FALSE)?szBufferName:szTargetName,this);

		//停止下载
		StopDownLoad();
	}
	else
	{
		//事件通知
		ASSERT(m_pIDownLoadMissionSink!=NULL);
		m_pIDownLoadMissionSink->OnDownLoadStatus(m_dwDownLoadSize,this);
	}

	return;
}

//数据包头
VOID __cdecl CDownLoadMission::OnHttpDataHead(const VOID * pcbHeadData, WORD wHeadSize, INT nStatusCode)
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
			m_pszFileEntityTag=m_HttpHeadTranslate.GetFileEntityTag();

			//事件通知
			ASSERT(m_pIDownLoadMissionSink!=NULL);
			m_pIDownLoadMissionSink->OnDownLoadStart(m_dwTotalFileSize,this);

			//下载准备
			if (m_DownLoadStatus==DownLoadStatus_Service)
			{
				//创建文件
				if (CreateBufferFile(m_MissionRequest)==false)
				{
					//关闭连接
					m_DownLoadSocket.CloseSocket(false);

					//事件通知
					ASSERT(m_pIDownLoadMissionSink!=NULL);
					m_pIDownLoadMissionSink->OnDownLoadException(DownLoadError_FileSystem,this);

					//停止下载
					StopDownLoad();
				}
			}

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

//设置释放
bool CDownLoadMission::SetRelease(bool bRelease)
{
	//设置变量
	m_bRelease=bRelease;

	return true;
}

//对比请求
bool CDownLoadMission::CompareRequest(const tagMissionRequest & MissionRequest)
{
	//请求对比
	if (lstrcmp(m_MissionRequest.szLocalPath,MissionRequest.szLocalPath)!=0) return false;
	if (lstrcmp(m_MissionRequest.szDownLoadUrl,MissionRequest.szDownLoadUrl)!=0) return false;

	return true;
}

//设置接口
bool __cdecl CDownLoadMission::SetDownLoadMissionSink(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if (pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDownLoadMissionSink)!=NULL);
		m_pIDownLoadMissionSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDownLoadMissionSink);

		//成功判断
		if (m_pIDownLoadMissionSink==NULL) return false;
	}
	else m_pIDownLoadMissionSink=NULL;

	return true;
}

//结束下载
bool __cdecl CDownLoadMission::StopDownLoad()
{
	//任务信息
	m_dwMissionID=0L;
	m_DownLoadStatus=DownLoadStatus_Null;

	//文件状态
	m_dwDownLoadSize=0L;
	m_dwTotalFileSize=0L;
	m_pszFileEntityTag=NULL;

	//内核变量
	ZeroMemory(&m_RequestInfo,sizeof(m_RequestInfo));
	ZeroMemory(&m_MissionRequest,sizeof(m_MissionRequest));

	//设置组件
	m_HttpHeadTranslate.CleanData();
	m_DownLoadSocket.CloseSocket(false);
	if (m_BufferFile.m_hFile!=INVALID_HANDLE_VALUE) m_BufferFile.Close();

	//释放下载
	if (m_bRelease==true)
	{
		CDownLoadService * pDownLoadService=CDownLoadService::GetInstance();
		if (pDownLoadService!=NULL) pDownLoadService->ReleaseDownLoad(this);
	}

	return true;
}

//开始下载
bool __cdecl CDownLoadMission::StartDownLoad(DWORD dwMissionID, const tagMissionRequest & MissionRequest)
{
	//状态效验
	ASSERT(m_DownLoadStatus==DownLoadStatus_Null);
	if (m_DownLoadStatus!=DownLoadStatus_Null) return false;

	//任务信息
	m_dwMissionID=dwMissionID;
	m_DownLoadStatus=DownLoadStatus_Connect;

	//文件信息
	m_dwDownLoadSize=0L;
	m_dwTotalFileSize=0L;
	m_pszFileEntityTag=NULL;

	//内核信息
	m_MissionRequest=MissionRequest;
	ZeroMemory(&m_RequestInfo,sizeof(m_RequestInfo));

	//请求连接
	bool bSuccess=ConnectServer(m_MissionRequest.szDownLoadUrl);

	return bSuccess;
}

//请求连接
bool CDownLoadMission::ConnectServer(LPCTSTR pszDownLoadUrl)
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
		ASSERT(m_pIDownLoadMissionSink!=NULL);
		m_pIDownLoadMissionSink->OnDownLoadException(DownLoadError,this);

		//停止下载
		StopDownLoad();

		return false;
	}

	return false;
}

//发送请求
bool CDownLoadMission::SendHttpRequest(const tagRequestInfo & RequestInfo)
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

//创建文件
bool CDownLoadMission::CreateBufferFile(const tagMissionRequest & MissionRequest)
{
	//效验状态
	ASSERT(m_BufferFile.m_hFile==INVALID_HANDLE_VALUE);
	if (m_BufferFile.m_hFile!=INVALID_HANDLE_VALUE) return false;

	//目录变量
	DWORD dwExcursion=0;
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	lstrcpyn(szDirectory,MissionRequest.szLocalPath,CountArray(szDirectory));

	//建立目录
	do
	{
		//结束判断
		if (szDirectory[dwExcursion]==0)
		{
			CreateDirectory(szDirectory,NULL);
			break;
		}

		//创建目录
		if (szDirectory[dwExcursion]==TEXT('\\'))
		{
			szDirectory[dwExcursion]=0;
			CreateDirectory(szDirectory,NULL);
			szDirectory[dwExcursion]=TEXT('\\');
		}

		//设置索引
		dwExcursion++;

	} while (true);

	//文件名字
	DWORD dwIndex=lstrlen(m_RequestInfo.szRequestPath);
	while ((dwIndex>0)&&(m_RequestInfo.szRequestPath[dwIndex]!=TEXT('/'))) dwIndex--;

	//构造路径
	TCHAR szFilePath[MAX_PATH]=TEXT("");
	LPCTSTR pszFileName=&m_RequestInfo.szRequestPath[dwIndex+1];
	_sntprintf_s(szFilePath,CountArray(szFilePath),TEXT("%s\\%s.IDL"),MissionRequest.szLocalPath,pszFileName);

	//创建文件
	if (m_BufferFile.Open(szFilePath,CFile::modeCreate|CFile::modeWrite)==false) return false;

	return true;
}

//获取信息
INTERNET_SCHEME CDownLoadMission::GetDownLoadInfo(LPCTSTR pszDownUrl, tagRequestInfo & RequestInfo)
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
DECLARE_CREATE_MODULE(DownLoadMission);

//////////////////////////////////////////////////////////////////////////
