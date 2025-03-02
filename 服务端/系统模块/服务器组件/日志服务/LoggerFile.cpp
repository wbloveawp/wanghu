#include "StdAfx.h"
#include "Loggerfile.h"

/////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CLoggerFile::CLoggerFile(void)
{
	//设置变量
	m_szFileName[0]=0;
	m_hFile=INVALID_HANDLE_VALUE;	
}

//析构函数
CLoggerFile::~CLoggerFile(void)
{
	//关闭文件
	Close();
}

//创建文件
BOOL CLoggerFile::Create(LPCTSTR pszFileName,DWORD dwAccess,DWORD dwCreation)
{
	return Create(pszFileName,dwAccess,0,NULL,dwCreation,FILE_ATTRIBUTE_NORMAL,NULL);
}

//创建文件
BOOL CLoggerFile::Create(LPCTSTR pszFileName,DWORD dwAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreation,DWORD dwAttributes,HANDLE hTemplateFile)
{
	//创建文件
	m_hFile = CreateFile(pszFileName,dwAccess,dwShareMode,lpSecurityAttributes,dwCreation,dwAttributes,hTemplateFile);

	//校验句柄
	if(m_hFile==INVALID_HANDLE_VALUE) return FALSE;

	//设置文件名
	StringCchCopy(m_szFileName,CountArray(m_szFileName), pszFileName);

	return TRUE;
}

//读取文件
DWORD CLoggerFile::Read(LPVOID lpBuffer,DWORD dwBufferCount)
{
	//句柄校验
	ASSERT(m_hFile!=INVALID_HANDLE_VALUE);
	if(m_hFile==INVALID_HANDLE_VALUE) return 0;

	//参数校验
	ASSERT(lpBuffer!=NULL);
	if(lpBuffer==NULL) return 0;

	//变量定义
	DWORD dwReadCount=0;

	//读取文件
	ReadFile(m_hFile,lpBuffer,dwBufferCount,&dwReadCount,NULL);

	return dwReadCount;
}

//写入文件
DWORD CLoggerFile::Write(LPCVOID lpBuffer,DWORD dwBufferCount)
{
	//句柄校验
	ASSERT(m_hFile!=INVALID_HANDLE_VALUE);
	if(m_hFile==INVALID_HANDLE_VALUE) return 0;

	//参数校验
	ASSERT(lpBuffer!=NULL);
	if(lpBuffer==NULL) return 0;

	//变量定义
	DWORD dwWriteCount=0;

	//写入文件
	WriteFile(m_hFile,lpBuffer,dwBufferCount,&dwWriteCount,NULL);

	return dwWriteCount;
}

//移至开始
DWORD CLoggerFile::SeekToBegin()
{
	return Seek(0,FILE_BEGIN);
}

//移至结尾
DWORD CLoggerFile::SeekToEnd()
{
	return Seek(0,FILE_END);
}

//移动指针
DWORD CLoggerFile::Seek(LONGLONG lOffset,UINT nFrom)
{
	//句柄校验
	ASSERT(m_hFile!=INVALID_HANDLE_VALUE);
	if(m_hFile==INVALID_HANDLE_VALUE) return 0;

	//变量定义
	LARGE_INTEGER liDistance;
	liDistance.QuadPart = lOffset;

	//移动指针
	liDistance.LowPart = SetFilePointer(m_hFile,liDistance.LowPart,&liDistance.HighPart,nFrom);
	
	//调整变量
	if (liDistance.LowPart == INVALID_SET_FILE_POINTER && GetLastError()!= NO_ERROR)
	{

		liDistance.QuadPart = -1;
	}

	return static_cast<DWORD>(liDistance.QuadPart);
}

//获取长度
DWORD CLoggerFile::GetFileLength()
{
	//获取长度
	return GetFileSize(m_hFile,NULL);
}

//设置结尾
 BOOL CLoggerFile::SetEndOfFile()
 {
	 //设置文件结尾
	return ::SetEndOfFile(m_hFile);
 }

//设置长度
//BOOL CLoggerFile::SetFileValidLength(LONGLONG lValidlength)
//{
//	//设置文件结尾
//	SeekToEnd();
//	SetEndOfFile();
//
//	//设置有效长度
//	return SetFileValidData(m_hFile,lValidlength);
//}

//关闭文件
VOID CLoggerFile::Close()
{
	//关闭句柄
	if(m_hFile!=INVALID_HANDLE_VALUE) 
	{
		CloseHandle(m_hFile);
		m_hFile=INVALID_HANDLE_VALUE;
	}

	//设置变量
	m_szFileName[0]=0;
}
/////////////////////////////////////////////////////////////////////////////////////////////
