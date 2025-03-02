#ifndef LOGGERm_szFileName_HEAD_FILE
#define	LOGGERm_szFileName_HEAD_FILE	
#pragma once

//引入头文件
#include "LoggerServiceHead.h"

/////////////////////////////////////////////////////////////////////////////////////////////
//日志文件
class LOGGER_SERVICE_CLASS CLoggerFile
{
	//变量定义
protected:
	HANDLE									m_hFile;								//文件句柄
	TCHAR									m_szFileName[MAX_PATH];					//文件名称

	//函数定义
public:
	//构造函数
	CLoggerFile(void);
	//析构函数
	virtual ~CLoggerFile(void);

	//功能函数
public:
	//创建文件
	BOOL Create(LPCTSTR pszFileName,DWORD dwAccess,DWORD dwCreation=OPEN_EXISTING);
	//创建文件
	BOOL Create(LPCTSTR pszFileName,DWORD dwAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwCreation,DWORD dwAttributes,HANDLE hTemplateFile);
	//读取文件
	DWORD Read(LPVOID lpBuffer,DWORD dwBufferCount);
	//写入文件
	DWORD Write(LPCVOID lpBuffer,DWORD dwBufferCount);

	//功能函数
public:
	//移至开始
	DWORD SeekToBegin();
	//移至结尾
	DWORD SeekToEnd();
	//移动指针
	DWORD Seek(LONGLONG lOffset,UINT nFrom=FILE_CURRENT);	
	//关闭文件
	VOID Close(); 

	//辅助函数
public:
	//获取长度
	DWORD GetFileLength();
	//设置结尾
  BOOL SetEndOfFile();
	//获取文件名
	LPCTSTR GetFileName() { return m_szFileName; }
	//有效判断
	BOOL IsValid() { return m_hFile!=INVALID_HANDLE_VALUE; }	

};
/////////////////////////////////////////////////////////////////////////////////////////////
#endif