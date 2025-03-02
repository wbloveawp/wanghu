#ifndef CUSTOM_FILE_HEAD_FILE
#define	CUSTOM_FILE_HEAD_FILE	
#pragma once

//�����ļ�
#include "CommonServiceHead.h"

/////////////////////////////////////////////////////////////////////////////////////////////
//�Զ����ļ�
class COMMON_SERVICE_CLASS CWHCustomFile
{
	//��������
protected:
	HANDLE									m_hFile;								//�ļ����
	TCHAR									m_szFileName[MAX_PATH];					//�ļ�����

	//��������
public:
	//���캯��
	CWHCustomFile(void);
	//��������
	virtual ~CWHCustomFile(void);

	//���ܺ���
public:
	//�����ļ�
	BOOL Create(LPCTSTR pszFileName,DWORD dwGate,DWORD dwCreation=OPEN_EXISTING);
	//�����ļ�
	BOOL Create(LPCTSTR pszFileName,DWORD dwGate,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwCreation,DWORD dwAttributes,HANDLE hTemplateFile);
	//��ȡ�ļ�
	DWORD Read(LPVOID lpBuffer,DWORD dwBufferCount);
	//д���ļ�
	VOID Write(LPCVOID lpBuffer,DWORD dwBufferCount);

	//���ܺ���
public:
	//������ʼ
	DWORD SeekToBegin();
	//������β
	DWORD SeekToEnd();
	//�ƶ�ָ��
	DWORD Seek(SCORE lOffset,UINT nFrom=FILE_CURRENT);	
	//�ر��ļ�
	VOID Close(); 

	//��������
public:
	//��ȡ����
	DWORD GetFileLength();
	//���ý�β
  BOOL SetEndOfFile();
	//���ó���
	//BOOL SetFileValidLength(SCORE lValidlength);
	//��Ч�ж�
	BOOL IsValid() { return m_hFile!=INVALID_HANDLE_VALUE; }
	//��ȡ�ļ���
	LPCTSTR GetFileName() { return m_szFileName; }

};
/////////////////////////////////////////////////////////////////////////////////////////////
#endif