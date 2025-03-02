#ifndef ASSISTANT_HEAD_FILE
#define ASSISTANT_HEAD_FILE

#pragma once

#include "CommonServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//��������
class COMMON_SERVICE_CLASS CWHService
{
	//��������
private:
	//���캯��
	CWHService();

	//������ʶ
public:
	//������ַ
	static bool GetNetWorkID(TCHAR szNetWorkID[LEN_NETWORK_ID]);
	//��ȡ������
	static bool GetMachineID(TCHAR szMachineID[LEN_MACHINE_ID]);	

	//ϵͳ����
public:
	//�����ַ�
	static bool CopyToClipBoard(LPCTSTR pszString);
	//ע���ȼ�
	static bool RegisterHotKey(HWND hWnd, UINT uKeyID, WORD wHotKey);

	//ϵͳ�ļ�
public:
	//��������
	static bool GetProcessName(TCHAR szProcessName[], WORD wBufferCount);
	//����Ŀ¼
	static bool GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount);
	//�ļ��汾
	static bool GetModuleVersion(LPCTSTR pszModuleName, DWORD & dwVersionInfo);

	//����ת��
public:
	//GB2312תUTF8
	static int GB23122Utf8(LPCSTR pszGB2312, std::string& sUtf8);
	//GB2312תUTF8
	static int GB23122Utf8(LPCSTR pszGB2312, LPSTR pszUtf8, int nBuffSize);
	//UTF8תGB2312
	static int Utf82GB2312(LPCSTR pszUtf8, std::string& sAscii);
	//UTF8תGB2312
	static int Utf82GB2312(LPCSTR pszUtf8, LPSTR pszGB2312, int nBuffSize);	

	//����ת��
public:
	//UnicodeתUTF8
	static int Unicode2Utf8(LPCWSTR pszUnicode, std::string & sUtf8);
	//UnicodeתUTF8
	static int Unicode2Utf8(LPCWSTR pszUnicode, LPSTR pszUtf8, int nBuffSize);
	//UTF8תUNICODE
	static int Utf82Unicode(LPCSTR pszUtf8, LPWSTR pszUnicode, int nBuffSize);
	//UTF8תUNICODE
	static int Utf82Unicode(std::string& sUtf8, LPWSTR pszUnicode, int nBuffSize);

	//��Ϣ��ʽ��
public:
	//��ʽ����Ϣ	
	static LPCTSTR FormatMessage(WORD wMsgIndex, LPCTSTR pszMsg);
	//��ʽ����Ϣ	
	static LPCTSTR FormatMessageV(WORD wMsgIndex, LPCTSTR pszMsgFormat, ...);

	//���뺯��
public:
	//���ݽ���
	static WORD DecodeString(LPCTSTR pszString, BYTE cbDataBuffer[], WORD wDataCount);
	//���ݱ���
	static WORD EncodeString(BYTE cbDataBuffer[], WORD wDataCount, TCHAR szString[], WORD wStringCount);
};

//////////////////////////////////////////////////////////////////////////

#endif