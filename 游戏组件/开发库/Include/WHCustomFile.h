#ifndef CUSTOM_FILE_HEAD_FILE
#define	CUSTOM_FILE_HEAD_FILE	
#pragma once

//引入文件
#include "CommonServiceHead.h"

/////////////////////////////////////////////////////////////////////////////////////////////
//自定义文件
class COMMON_SERVICE_CLASS CWHCustomFile
{
	//变量定义
protected:
	HANDLE									m_hFile;								//文件句柄
	TCHAR									m_szFileName[MAX_PATH];					//文件名称

	//函数定义
public:
	//构造函数
	CWHCustomFile(void);
	//析构函数
	virtual ~CWHCustomFile(void);

	//功能函数
public:
	//创建文件
	BOOL Create(LPCTSTR pszFileName,DWORD dwGate,DWORD dwCreation=OPEN_EXISTING);
	//创建文件
	BOOL Create(LPCTSTR pszFileName,DWORD dwGate,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwCreation,DWORD dwAttributes,HANDLE hTemplateFile);
	//读取文件
	DWORD Read(LPVOID lpBuffer,DWORD dwBufferCount);
	//写入文件
	VOID Write(LPCVOID lpBuffer,DWORD dwBufferCount);

	//功能函数
public:
	//移至开始
	DWORD SeekToBegin();
	//移至结尾
	DWORD SeekToEnd();
	//移动指针
	DWORD Seek(SCORE lOffset,UINT nFrom=FILE_CURRENT);	
	//关闭文件
	VOID Close(); 

	//辅助函数
public:
	//获取长度
	DWORD GetFileLength();
	//设置结尾
  BOOL SetEndOfFile();
	//设置长度
	//BOOL SetFileValidLength(SCORE lValidlength);
	//有效判断
	BOOL IsValid() { return m_hFile!=INVALID_HANDLE_VALUE; }
	//获取文件名
	LPCTSTR GetFileName() { return m_szFileName; }

};
/////////////////////////////////////////////////////////////////////////////////////////////
#endif