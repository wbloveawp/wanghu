#ifndef LOGGERm_szFileName_HEAD_FILE
#define	LOGGERm_szFileName_HEAD_FILE	
#pragma once

//����ͷ�ļ�
#include "LoggerServiceHead.h"

/////////////////////////////////////////////////////////////////////////////////////////////
//��־�ļ�
class LOGGER_SERVICE_CLASS CLoggerFile
{
	//��������
protected:
	HANDLE									m_hFile;								//�ļ����
	TCHAR									m_szFileName[MAX_PATH];					//�ļ�����

	//��������
public:
	//���캯��
	CLoggerFile(void);
	//��������
	virtual ~CLoggerFile(void);

	//���ܺ���
public:
	//�����ļ�
	BOOL Create(LPCTSTR pszFileName,DWORD dwAccess,DWORD dwCreation=OPEN_EXISTING);
	//�����ļ�
	BOOL Create(LPCTSTR pszFileName,DWORD dwAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwCreation,DWORD dwAttributes,HANDLE hTemplateFile);
	//��ȡ�ļ�
	DWORD Read(LPVOID lpBuffer,DWORD dwBufferCount);
	//д���ļ�
	DWORD Write(LPCVOID lpBuffer,DWORD dwBufferCount);

	//���ܺ���
public:
	//������ʼ
	DWORD SeekToBegin();
	//������β
	DWORD SeekToEnd();
	//�ƶ�ָ��
	DWORD Seek(LONGLONG lOffset,UINT nFrom=FILE_CURRENT);	
	//�ر��ļ�
	VOID Close(); 

	//��������
public:
	//��ȡ����
	DWORD GetFileLength();
	//���ý�β
  BOOL SetEndOfFile();
	//��ȡ�ļ���
	LPCTSTR GetFileName() { return m_szFileName; }
	//��Ч�ж�
	BOOL IsValid() { return m_hFile!=INVALID_HANDLE_VALUE; }	

};
/////////////////////////////////////////////////////////////////////////////////////////////
#endif