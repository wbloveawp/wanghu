#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBaseEngineSink.h"
#include <afxinet.h>


//////////////////////////////////////////////////////////////////////////
//��̬����
CCriticalSection		CDataBaseEngineSink::m_DirectoryLocked;	

//�ϼ�Ŀ¼
bool ToLastDirectory(LPTSTR pszDirectory,LPCTSTR pszFullDirectory)
{
	//У�����
	int nLen=lstrlen(pszDirectory);		
	if (nLen<=0 || nLen>lstrlen(pszFullDirectory)) return false;

	while (*(pszFullDirectory+--nLen) != TEXT('\0')) 
	{
		if (*(pszFullDirectory+nLen) == '/' || *(pszFullDirectory+nLen) == '\\')
		{
			StringCchCopy(pszDirectory,nLen+1, pszFullDirectory);
			return true;
		}

		//�˳��ж�
		if (nLen==0) break;
	}

	return false;
}

//�¼�Ŀ¼
bool ToNextDirectory(LPTSTR pszDirectory,LPCTSTR pszFullDirectory)
{
	//У�����
	int nLen=lstrlen(pszDirectory);		
	if (nLen<=0 || nLen>lstrlen(pszFullDirectory)) return false;

	while (*(pszFullDirectory+nLen++) != TEXT('\0')) 
	{
		if (*(pszFullDirectory+nLen) == '/' || *(pszFullDirectory+nLen) == '\\' || nLen==lstrlen(pszFullDirectory))
		{
			StringCchCopy(pszDirectory,nLen+1, pszFullDirectory);
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//�������
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return;
}

//��������
CDataBaseEngineSink::~CDataBaseEngineSink()
{
}

//�ӿڲ�ѯ
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	return true;
}

//ֹͣ�¼�
bool CDataBaseEngineSink::OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)
{
	//�������
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return true;
}

//ʱ���¼�
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	switch (wRequestID)
	{
	case DBR_GR_STORE_VIDEO:  //�洢¼��
		{
			return OnRequestStoreVideo(dwContextID, pData, wDataSize, pdwControlCode);
		}
	}

	return false;
}

//�洢¼��
bool CDataBaseEngineSink::OnRequestStoreVideo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//У�����
	ASSERT (wDataSize>sizeof(DBR_GR_StoreVideo));
	if (wDataSize<=sizeof(DBR_GR_StoreVideo)) return false;

	//��ȡ����
	DBR_GR_StoreVideo * pStoreVideo=(DBR_GR_StoreVideo *)pData;

	//��������
	LPBYTE pcbVideoData=(LPBYTE)(pStoreVideo+1);
	SYSTEMTIME * pVideoCreateTime=&pStoreVideo->VideoCreateTime;

	//��������
	TCHAR szVideoDirectory[MAX_PATH];
	ZeroMemory(szVideoDirectory,sizeof(szVideoDirectory));

	//����·��
	_sntprintf_s(szVideoDirectory,CountArray(szVideoDirectory),TEXT("%s\\%d-%02d-%02d\\%02d\\%d"),m_pInitParameter->m_szVideoPath,pVideoCreateTime->wYear,pVideoCreateTime->wMonth,pVideoCreateTime->wDay,pVideoCreateTime->wHour,pStoreVideo->wKindID);

	//У��·��
	if (IsDirectoryExist(szVideoDirectory)==FALSE)
	{
		//����·��
		if (CreateDirectory(szVideoDirectory)==FALSE)
		{
			return false;
		}
	}

	//����·��	
	TCHAR szVideoFilePath[MAX_PATH];
	ZeroMemory(szVideoFilePath,sizeof(szVideoFilePath));

	//����·��
	_sntprintf_s(szVideoFilePath,CountArray(szVideoFilePath),TEXT("%s\\%s.video"),szVideoDirectory,pStoreVideo->szVideoNumber);

	//�����ļ�
	if (CreateFile(szVideoFilePath,pcbVideoData,pStoreVideo->wVideoSize)==FALSE)
	{
		return false;
	}

	return true;
}

//�ļ�����
BOOL CDataBaseEngineSink::IsFileExist(LPCTSTR pszFileName)
{
	//��������
	CFileFind FileFind;
  return FileFind.FindFile(pszFileName);
}

//Ŀ¼����
BOOL CDataBaseEngineSink::IsDirectoryExist(LPCTSTR pszDirectory)
{
	//��������
	WIN32_FIND_DATA wfd;
  BOOL bResult = FALSE;

	//�����ļ�
  HANDLE hFind = FindFirstFile(pszDirectory, &wfd);
  if ((hFind!=INVALID_HANDLE_VALUE) &&(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
  {
    bResult = TRUE;
  }

	//�رվ��
  FindClose(hFind);

  return bResult;
}

//����Ŀ¼
BOOL CDataBaseEngineSink::CreateDirectory(LPCTSTR pszFileDirectory)
{
	//Ŀ¼����
	CThreadLock ThreadLock(m_DirectoryLocked);

	//����Ŀ¼
	if (IsDirectoryExist(pszFileDirectory)==TRUE)
	{
		return TRUE;
	}

	//��������	
	TCHAR szDirectory[MAX_PATH];
	StringCchCopy(szDirectory,CountArray(szDirectory), pszFileDirectory);

	//����ṹ
	SECURITY_ATTRIBUTES attrib;
	ZeroMemory(&attrib,sizeof(attrib));

	do
	{
		//���ñ���
		attrib.bInheritHandle=FALSE;
		attrib.lpSecurityDescriptor=NULL;
		attrib.nLength=sizeof(SECURITY_ATTRIBUTES);

		//����Ŀ¼
		BOOL bResult=::CreateDirectory(szDirectory,&attrib);
		if (bResult!=TRUE)
		{
			//У�������			
			if (GetLastError()!=0x03) 
			{
				return FALSE;
			}

			//�ϼ�Ŀ¼
			if (ToLastDirectory(szDirectory,pszFileDirectory)==false) return FALSE;
		}
		else
		{
			//У��Ŀ¼
			if (lstrcmp(szDirectory,pszFileDirectory)==0) 
			{
				return TRUE;			
			}

			//�¼�Ŀ¼
			if (ToNextDirectory(szDirectory,pszFileDirectory)==false) return FALSE;
		}

	} while (TRUE);	

	return TRUE;
}

//�����ļ�
BOOL CDataBaseEngineSink::CreateFile(LPCTSTR pszFileName,BYTE cbFileData[],DWORD dwFileSize)
{
	try
	{		
		//��������
		CFile File;

		//���ļ�
		if (File.Open(pszFileName, CFile::modeCreate | CFile::modeWrite, NULL) == FALSE)
		{
			throw TEXT("�޷������ļ�");
		}
		
		//д������
		File.Write(cbFileData,dwFileSize);	
		
		//�ر��ļ�
		File.Close();

		return TRUE;
	}
	catch (LPCTSTR pszException)
	{
		CTraceService::TraceString(pszException,TraceLevel_Exception);
	}	

	return FALSE;
}
//////////////////////////////////////////////////////////////////////////

