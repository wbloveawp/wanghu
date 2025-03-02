#include "StdAfx.h"
#include "Resource.h"
#include "DownLoadMission.h"
#include "DownLoadService.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CDownLoadMission::CDownLoadMission()
{
	//������Ϣ
	m_dwMissionID=0L;
	m_DownLoadStatus=DownLoadStatus_Null;

	//�ļ�״̬
	m_dwDownLoadSize=0L;
	m_dwTotalFileSize=0L;
	m_pszFileEntityTag=NULL;

	//�ں˱���
	m_bRelease=false;
	ZeroMemory(&m_RequestInfo,sizeof(m_RequestInfo));
	ZeroMemory(&m_MissionRequest,sizeof(m_MissionRequest));

	//�������
	m_DownLoadSocket.SetDownLoadSocketSink(QUERY_ME_INTERFACE(IUnknownEx));

	return;
}

//��������
CDownLoadMission::~CDownLoadMission()
{
}

//�ӿڲ�ѯ
VOID * __cdecl CDownLoadMission::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDownLoadMission,Guid,dwQueryVer);
	QUERYINTERFACE(IDownLoadSocketSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDownLoadMission,Guid,dwQueryVer);
	return NULL;
}

//������Ϣ
bool __cdecl CDownLoadMission::OnSocketConnect(INT nErrorCode)
{
	//��������
	SendHttpRequest(m_RequestInfo);

	return true;
}

//�ر���Ϣ
bool __cdecl CDownLoadMission::OnSocketClose(bool bCloseByServer)
{
	//���ñ���
	m_DownLoadStatus=DownLoadStatus_Error;

	//�¼�֪ͨ
	ASSERT(m_pIDownLoadMissionSink!=NULL);
	m_pIDownLoadMissionSink->OnDownLoadException(DownLoadError_HttpServer,this);

	//ֹͣ����
	StopDownLoad();

	return true;
}

//���ݰ���
VOID __cdecl CDownLoadMission::OnHttpDataStream(const VOID * pStreamData, WORD wStreamSize)
{
	//Ч��״̬
	ASSERT(m_DownLoadStatus==DownLoadStatus_Service);
	if (m_DownLoadStatus!=DownLoadStatus_Service) return;

	//д���ļ�
	m_dwDownLoadSize+=wStreamSize;
	m_BufferFile.Write(pStreamData,wStreamSize);

	//����ж�
	if (m_dwDownLoadSize==m_dwTotalFileSize)
	{
		//���ñ���
		m_DownLoadStatus=DownLoadStatus_Conclude;

		//�ļ�����
		DWORD dwIndex=lstrlen(m_RequestInfo.szRequestPath);
		while ((dwIndex>0)&&(m_RequestInfo.szRequestPath[dwIndex]!=TEXT('/'))) dwIndex--;

		//����·��
		TCHAR szTargetName[MAX_PATH]=TEXT("");
		TCHAR szBufferName[MAX_PATH]=TEXT("");
		LPCTSTR pszFileName=&m_RequestInfo.szRequestPath[dwIndex+1];
		lstrcpyn(szBufferName,(LPCTSTR)m_BufferFile.GetFilePath(),CountArray(szBufferName));
		// add by humorly 2018/8/3
		// replace func
		_snwprintf_s(szTargetName, CountArray(szTargetName), TEXT("%s\\%s"), m_MissionRequest.szLocalPath, pszFileName);

		//�ر��ļ�
		m_BufferFile.Close();

		//�ļ�ת��
		DeleteFile(szTargetName);
		BOOL bSuccess=MoveFile(szBufferName,szTargetName);

		//�¼�֪ͨ
		ASSERT(m_pIDownLoadMissionSink!=NULL);
		m_pIDownLoadMissionSink->OnDownLoadComplete((bSuccess==FALSE)?szBufferName:szTargetName,this);

		//ֹͣ����
		StopDownLoad();
	}
	else
	{
		//�¼�֪ͨ
		ASSERT(m_pIDownLoadMissionSink!=NULL);
		m_pIDownLoadMissionSink->OnDownLoadStatus(m_dwDownLoadSize,this);
	}

	return;
}

//���ݰ�ͷ
VOID __cdecl CDownLoadMission::OnHttpDataHead(const VOID * pcbHeadData, WORD wHeadSize, INT nStatusCode)
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
			m_pszFileEntityTag=m_HttpHeadTranslate.GetFileEntityTag();

			//�¼�֪ͨ
			ASSERT(m_pIDownLoadMissionSink!=NULL);
			m_pIDownLoadMissionSink->OnDownLoadStart(m_dwTotalFileSize,this);

			//����׼��
			if (m_DownLoadStatus==DownLoadStatus_Service)
			{
				//�����ļ�
				if (CreateBufferFile(m_MissionRequest)==false)
				{
					//�ر�����
					m_DownLoadSocket.CloseSocket(false);

					//�¼�֪ͨ
					ASSERT(m_pIDownLoadMissionSink!=NULL);
					m_pIDownLoadMissionSink->OnDownLoadException(DownLoadError_FileSystem,this);

					//ֹͣ����
					StopDownLoad();
				}
			}

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

//�����ͷ�
bool CDownLoadMission::SetRelease(bool bRelease)
{
	//���ñ���
	m_bRelease=bRelease;

	return true;
}

//�Ա�����
bool CDownLoadMission::CompareRequest(const tagMissionRequest & MissionRequest)
{
	//����Ա�
	if (lstrcmp(m_MissionRequest.szLocalPath,MissionRequest.szLocalPath)!=0) return false;
	if (lstrcmp(m_MissionRequest.szDownLoadUrl,MissionRequest.szDownLoadUrl)!=0) return false;

	return true;
}

//���ýӿ�
bool __cdecl CDownLoadMission::SetDownLoadMissionSink(IUnknownEx * pIUnknownEx)
{
	//���ýӿ�
	if (pIUnknownEx!=NULL)
	{
		//��ѯ�ӿ�
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDownLoadMissionSink)!=NULL);
		m_pIDownLoadMissionSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDownLoadMissionSink);

		//�ɹ��ж�
		if (m_pIDownLoadMissionSink==NULL) return false;
	}
	else m_pIDownLoadMissionSink=NULL;

	return true;
}

//��������
bool __cdecl CDownLoadMission::StopDownLoad()
{
	//������Ϣ
	m_dwMissionID=0L;
	m_DownLoadStatus=DownLoadStatus_Null;

	//�ļ�״̬
	m_dwDownLoadSize=0L;
	m_dwTotalFileSize=0L;
	m_pszFileEntityTag=NULL;

	//�ں˱���
	ZeroMemory(&m_RequestInfo,sizeof(m_RequestInfo));
	ZeroMemory(&m_MissionRequest,sizeof(m_MissionRequest));

	//�������
	m_HttpHeadTranslate.CleanData();
	m_DownLoadSocket.CloseSocket(false);
	if (m_BufferFile.m_hFile!=INVALID_HANDLE_VALUE) m_BufferFile.Close();

	//�ͷ�����
	if (m_bRelease==true)
	{
		CDownLoadService * pDownLoadService=CDownLoadService::GetInstance();
		if (pDownLoadService!=NULL) pDownLoadService->ReleaseDownLoad(this);
	}

	return true;
}

//��ʼ����
bool __cdecl CDownLoadMission::StartDownLoad(DWORD dwMissionID, const tagMissionRequest & MissionRequest)
{
	//״̬Ч��
	ASSERT(m_DownLoadStatus==DownLoadStatus_Null);
	if (m_DownLoadStatus!=DownLoadStatus_Null) return false;

	//������Ϣ
	m_dwMissionID=dwMissionID;
	m_DownLoadStatus=DownLoadStatus_Connect;

	//�ļ���Ϣ
	m_dwDownLoadSize=0L;
	m_dwTotalFileSize=0L;
	m_pszFileEntityTag=NULL;

	//�ں���Ϣ
	m_MissionRequest=MissionRequest;
	ZeroMemory(&m_RequestInfo,sizeof(m_RequestInfo));

	//��������
	bool bSuccess=ConnectServer(m_MissionRequest.szDownLoadUrl);

	return bSuccess;
}

//��������
bool CDownLoadMission::ConnectServer(LPCTSTR pszDownLoadUrl)
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
		ASSERT(m_pIDownLoadMissionSink!=NULL);
		m_pIDownLoadMissionSink->OnDownLoadException(DownLoadError,this);

		//ֹͣ����
		StopDownLoad();

		return false;
	}

	return false;
}

//��������
bool CDownLoadMission::SendHttpRequest(const tagRequestInfo & RequestInfo)
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

//�����ļ�
bool CDownLoadMission::CreateBufferFile(const tagMissionRequest & MissionRequest)
{
	//Ч��״̬
	ASSERT(m_BufferFile.m_hFile==INVALID_HANDLE_VALUE);
	if (m_BufferFile.m_hFile!=INVALID_HANDLE_VALUE) return false;

	//Ŀ¼����
	DWORD dwExcursion=0;
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	lstrcpyn(szDirectory,MissionRequest.szLocalPath,CountArray(szDirectory));

	//����Ŀ¼
	do
	{
		//�����ж�
		if (szDirectory[dwExcursion]==0)
		{
			CreateDirectory(szDirectory,NULL);
			break;
		}

		//����Ŀ¼
		if (szDirectory[dwExcursion]==TEXT('\\'))
		{
			szDirectory[dwExcursion]=0;
			CreateDirectory(szDirectory,NULL);
			szDirectory[dwExcursion]=TEXT('\\');
		}

		//��������
		dwExcursion++;

	} while (true);

	//�ļ�����
	DWORD dwIndex=lstrlen(m_RequestInfo.szRequestPath);
	while ((dwIndex>0)&&(m_RequestInfo.szRequestPath[dwIndex]!=TEXT('/'))) dwIndex--;

	//����·��
	TCHAR szFilePath[MAX_PATH]=TEXT("");
	LPCTSTR pszFileName=&m_RequestInfo.szRequestPath[dwIndex+1];
	_sntprintf_s(szFilePath,CountArray(szFilePath),TEXT("%s\\%s.IDL"),MissionRequest.szLocalPath,pszFileName);

	//�����ļ�
	if (m_BufferFile.Open(szFilePath,CFile::modeCreate|CFile::modeWrite)==false) return false;

	return true;
}

//��ȡ��Ϣ
INTERNET_SCHEME CDownLoadMission::GetDownLoadInfo(LPCTSTR pszDownUrl, tagRequestInfo & RequestInfo)
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
DECLARE_CREATE_MODULE(DownLoadMission);

//////////////////////////////////////////////////////////////////////////
