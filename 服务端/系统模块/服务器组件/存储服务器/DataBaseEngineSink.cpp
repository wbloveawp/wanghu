#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBaseEngineSink.h"
#include <afxinet.h>


//////////////////////////////////////////////////////////////////////////
//静态变量
CCriticalSection		CDataBaseEngineSink::m_DirectoryLocked;	

//上级目录
bool ToLastDirectory(LPTSTR pszDirectory,LPCTSTR pszFullDirectory)
{
	//校验参数
	int nLen=lstrlen(pszDirectory);		
	if (nLen<=0 || nLen>lstrlen(pszFullDirectory)) return false;

	while (*(pszFullDirectory+--nLen) != TEXT('\0')) 
	{
		if (*(pszFullDirectory+nLen) == '/' || *(pszFullDirectory+nLen) == '\\')
		{
			StringCchCopy(pszDirectory,nLen+1, pszFullDirectory);
			return true;
		}

		//退出判断
		if (nLen==0) break;
	}

	return false;
}

//下级目录
bool ToNextDirectory(LPTSTR pszDirectory,LPCTSTR pszFullDirectory)
{
	//校验参数
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

//构造函数
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return;
}

//析构函数
CDataBaseEngineSink::~CDataBaseEngineSink()
{
}

//接口查询
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	return true;
}

//停止事件
bool CDataBaseEngineSink::OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)
{
	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return true;
}

//时间事件
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//控制事件
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//请求事件
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	switch (wRequestID)
	{
	case DBR_GR_STORE_VIDEO:  //存储录像
		{
			return OnRequestStoreVideo(dwContextID, pData, wDataSize, pdwControlCode);
		}
	}

	return false;
}

//存储录像
bool CDataBaseEngineSink::OnRequestStoreVideo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//校验参数
	ASSERT (wDataSize>sizeof(DBR_GR_StoreVideo));
	if (wDataSize<=sizeof(DBR_GR_StoreVideo)) return false;

	//提取数据
	DBR_GR_StoreVideo * pStoreVideo=(DBR_GR_StoreVideo *)pData;

	//变量定义
	LPBYTE pcbVideoData=(LPBYTE)(pStoreVideo+1);
	SYSTEMTIME * pVideoCreateTime=&pStoreVideo->VideoCreateTime;

	//变量定义
	TCHAR szVideoDirectory[MAX_PATH];
	ZeroMemory(szVideoDirectory,sizeof(szVideoDirectory));

	//构造路径
	_sntprintf_s(szVideoDirectory,CountArray(szVideoDirectory),TEXT("%s\\%d-%02d-%02d\\%02d\\%d"),m_pInitParameter->m_szVideoPath,pVideoCreateTime->wYear,pVideoCreateTime->wMonth,pVideoCreateTime->wDay,pVideoCreateTime->wHour,pStoreVideo->wKindID);

	//校验路径
	if (IsDirectoryExist(szVideoDirectory)==FALSE)
	{
		//创建路径
		if (CreateDirectory(szVideoDirectory)==FALSE)
		{
			return false;
		}
	}

	//构造路径	
	TCHAR szVideoFilePath[MAX_PATH];
	ZeroMemory(szVideoFilePath,sizeof(szVideoFilePath));

	//构造路径
	_sntprintf_s(szVideoFilePath,CountArray(szVideoFilePath),TEXT("%s\\%s.video"),szVideoDirectory,pStoreVideo->szVideoNumber);

	//创建文件
	if (CreateFile(szVideoFilePath,pcbVideoData,pStoreVideo->wVideoSize)==FALSE)
	{
		return false;
	}

	return true;
}

//文件存在
BOOL CDataBaseEngineSink::IsFileExist(LPCTSTR pszFileName)
{
	//变量定义
	CFileFind FileFind;
  return FileFind.FindFile(pszFileName);
}

//目录存在
BOOL CDataBaseEngineSink::IsDirectoryExist(LPCTSTR pszDirectory)
{
	//变量定义
	WIN32_FIND_DATA wfd;
  BOOL bResult = FALSE;

	//查找文件
  HANDLE hFind = FindFirstFile(pszDirectory, &wfd);
  if ((hFind!=INVALID_HANDLE_VALUE) &&(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
  {
    bResult = TRUE;
  }

	//关闭句柄
  FindClose(hFind);

  return bResult;
}

//创建目录
BOOL CDataBaseEngineSink::CreateDirectory(LPCTSTR pszFileDirectory)
{
	//目录锁定
	CThreadLock ThreadLock(m_DirectoryLocked);

	//查找目录
	if (IsDirectoryExist(pszFileDirectory)==TRUE)
	{
		return TRUE;
	}

	//变量定义	
	TCHAR szDirectory[MAX_PATH];
	StringCchCopy(szDirectory,CountArray(szDirectory), pszFileDirectory);

	//构造结构
	SECURITY_ATTRIBUTES attrib;
	ZeroMemory(&attrib,sizeof(attrib));

	do
	{
		//设置变量
		attrib.bInheritHandle=FALSE;
		attrib.lpSecurityDescriptor=NULL;
		attrib.nLength=sizeof(SECURITY_ATTRIBUTES);

		//创建目录
		BOOL bResult=::CreateDirectory(szDirectory,&attrib);
		if (bResult!=TRUE)
		{
			//校验错误码			
			if (GetLastError()!=0x03) 
			{
				return FALSE;
			}

			//上级目录
			if (ToLastDirectory(szDirectory,pszFileDirectory)==false) return FALSE;
		}
		else
		{
			//校验目录
			if (lstrcmp(szDirectory,pszFileDirectory)==0) 
			{
				return TRUE;			
			}

			//下级目录
			if (ToNextDirectory(szDirectory,pszFileDirectory)==false) return FALSE;
		}

	} while (TRUE);	

	return TRUE;
}

//创建文件
BOOL CDataBaseEngineSink::CreateFile(LPCTSTR pszFileName,BYTE cbFileData[],DWORD dwFileSize)
{
	try
	{		
		//变量定义
		CFile File;

		//打开文件
		if (File.Open(pszFileName, CFile::modeCreate | CFile::modeWrite, NULL) == FALSE)
		{
			throw TEXT("无法创建文件");
		}
		
		//写入数据
		File.Write(cbFileData,dwFileSize);	
		
		//关闭文件
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

