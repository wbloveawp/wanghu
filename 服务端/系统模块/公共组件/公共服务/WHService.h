#ifndef ASSISTANT_HEAD_FILE
#define ASSISTANT_HEAD_FILE

#pragma once

#include "CommonServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//公共辅助
class COMMON_SERVICE_CLASS CWHService
{
	//函数定义
private:
	//构造函数
	CWHService();

	//机器标识
public:
	//网卡地址
	static bool GetNetWorkID(TCHAR szNetWorkID[LEN_NETWORK_ID]);
	//获取机器码
	static bool GetMachineID(TCHAR szMachineID[LEN_MACHINE_ID]);	

	//系统功能
public:
	//拷贝字符
	static bool CopyToClipBoard(LPCTSTR pszString);
	//注册热键
	static bool RegisterHotKey(HWND hWnd, UINT uKeyID, WORD wHotKey);

	//系统文件
public:
	//进程名称
	static bool GetProcessName(TCHAR szProcessName[], WORD wBufferCount);
	//工作目录
	static bool GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount);
	//文件版本
	static bool GetModuleVersion(LPCTSTR pszModuleName, DWORD & dwVersionInfo);

	//编码转换
public:
	//GB2312转UTF8
	static int GB23122Utf8(LPCSTR pszGB2312, std::string& sUtf8);
	//GB2312转UTF8
	static int GB23122Utf8(LPCSTR pszGB2312, LPSTR pszUtf8, int nBuffSize);
	//UTF8转GB2312
	static int Utf82GB2312(LPCSTR pszUtf8, std::string& sAscii);
	//UTF8转GB2312
	static int Utf82GB2312(LPCSTR pszUtf8, LPSTR pszGB2312, int nBuffSize);	

	//编码转换
public:
	//Unicode转UTF8
	static int Unicode2Utf8(LPCWSTR pszUnicode, std::string & sUtf8);
	//Unicode转UTF8
	static int Unicode2Utf8(LPCWSTR pszUnicode, LPSTR pszUtf8, int nBuffSize);
	//UTF8转UNICODE
	static int Utf82Unicode(LPCSTR pszUtf8, LPWSTR pszUnicode, int nBuffSize);
	//UTF8转UNICODE
	static int Utf82Unicode(std::string& sUtf8, LPWSTR pszUnicode, int nBuffSize);

	//消息格式化
public:
	//格式化消息	
	static LPCTSTR FormatMessage(WORD wMsgIndex, LPCTSTR pszMsg);
	//格式化消息	
	static LPCTSTR FormatMessageV(WORD wMsgIndex, LPCTSTR pszMsgFormat, ...);

	//编码函数
public:
	//数据解码
	static WORD DecodeString(LPCTSTR pszString, BYTE cbDataBuffer[], WORD wDataCount);
	//数据编码
	static WORD EncodeString(BYTE cbDataBuffer[], WORD wDataCount, TCHAR szString[], WORD wStringCount);
};

//////////////////////////////////////////////////////////////////////////

#endif