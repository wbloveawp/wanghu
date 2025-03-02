#include "StdAfx.h"
#include "WHCustomfile.h"

/////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CWHCustomFile::CWHCustomFile(void)
{
	//设置变量
	m_szFileName[0]=0;
	m_hFile=INVALID_HANDLE_VALUE;	
}

//析构函数
CWHCustomFile::~CWHCustomFile(void)
{
	//关闭文件
	Close();
}

//创建文件
BOOL CWHCustomFile::Create(LPCTSTR pszFileName,DWORD dwGate,DWORD dwCreation)
{
	return Create(pszFileName,dwGate,0,NULL,dwCreation,FILE_ATTRIBUTE_NORMAL,NULL);
}

//创建文件
BOOL CWHCustomFile::Create(LPCTSTR pszFileName,DWORD dwGate,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreation,DWORD dwAttributes,HANDLE hTemplateFile)
{
	//创建文件
	m_hFile = CreateFile(pszFileName,dwGate,dwShareMode,lpSecurityAttributes,dwCreation,dwAttributes,hTemplateFile);

	//校验句柄
	if(m_hFile==INVALID_HANDLE_VALUE) return FALSE;

	//设置文件名
	StringCchCopy(m_szFileName,CountArray(m_szFileName), pszFileName);

	return TRUE;
}

//读取文件
DWORD CWHCustomFile::Read(LPVOID lpBuffer,DWORD dwBufferCount)
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
VOID CWHCustomFile::Write(LPCVOID lpBuffer,DWORD dwBufferCount)
{
	//句柄校验
	ASSERT(m_hFile!=INVALID_HANDLE_VALUE);
	if(m_hFile==INVALID_HANDLE_VALUE) return;

	//参数校验
	ASSERT(lpBuffer!=NULL);
	if(lpBuffer==NULL) return;

	//变量定义
	DWORD dwWriteCount=0;

	//写入文件
	WriteFile(m_hFile,lpBuffer,dwBufferCount,&dwWriteCount,NULL);

	return;
}

//移至开始
DWORD CWHCustomFile::SeekToBegin()
{
	return Seek(0,FILE_BEGIN);
}

//移至结尾
DWORD CWHCustomFile::SeekToEnd()
{
	return Seek(0,FILE_END);
}

//移动指针
DWORD CWHCustomFile::Seek(SCORE lOffset,UINT nFrom)
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
DWORD CWHCustomFile::GetFileLength()
{
	//获取长度
	return GetFileSize(m_hFile,NULL);
}

//设置结尾
 BOOL CWHCustomFile::SetEndOfFile()
 {
	 //设置文件结尾
	return ::SetEndOfFile(m_hFile);
 }

//设置长度
//BOOL CWHCustomFile::SetFileValidLength(SCORE lValidlength)
//{
//	//设置文件结尾
//	SeekToEnd();
//	SetEndOfFile();
//
//	//设置有效长度
//	return SetFileValidData(m_hFile,lValidlength);
//}

//关闭文件
VOID CWHCustomFile::Close()
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
