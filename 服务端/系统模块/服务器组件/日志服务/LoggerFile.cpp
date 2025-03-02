#include "StdAfx.h"
#include "Loggerfile.h"

/////////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CLoggerFile::CLoggerFile(void)
{
	//���ñ���
	m_szFileName[0]=0;
	m_hFile=INVALID_HANDLE_VALUE;	
}

//��������
CLoggerFile::~CLoggerFile(void)
{
	//�ر��ļ�
	Close();
}

//�����ļ�
BOOL CLoggerFile::Create(LPCTSTR pszFileName,DWORD dwAccess,DWORD dwCreation)
{
	return Create(pszFileName,dwAccess,0,NULL,dwCreation,FILE_ATTRIBUTE_NORMAL,NULL);
}

//�����ļ�
BOOL CLoggerFile::Create(LPCTSTR pszFileName,DWORD dwAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreation,DWORD dwAttributes,HANDLE hTemplateFile)
{
	//�����ļ�
	m_hFile = CreateFile(pszFileName,dwAccess,dwShareMode,lpSecurityAttributes,dwCreation,dwAttributes,hTemplateFile);

	//У����
	if(m_hFile==INVALID_HANDLE_VALUE) return FALSE;

	//�����ļ���
	StringCchCopy(m_szFileName,CountArray(m_szFileName), pszFileName);

	return TRUE;
}

//��ȡ�ļ�
DWORD CLoggerFile::Read(LPVOID lpBuffer,DWORD dwBufferCount)
{
	//���У��
	ASSERT(m_hFile!=INVALID_HANDLE_VALUE);
	if(m_hFile==INVALID_HANDLE_VALUE) return 0;

	//����У��
	ASSERT(lpBuffer!=NULL);
	if(lpBuffer==NULL) return 0;

	//��������
	DWORD dwReadCount=0;

	//��ȡ�ļ�
	ReadFile(m_hFile,lpBuffer,dwBufferCount,&dwReadCount,NULL);

	return dwReadCount;
}

//д���ļ�
DWORD CLoggerFile::Write(LPCVOID lpBuffer,DWORD dwBufferCount)
{
	//���У��
	ASSERT(m_hFile!=INVALID_HANDLE_VALUE);
	if(m_hFile==INVALID_HANDLE_VALUE) return 0;

	//����У��
	ASSERT(lpBuffer!=NULL);
	if(lpBuffer==NULL) return 0;

	//��������
	DWORD dwWriteCount=0;

	//д���ļ�
	WriteFile(m_hFile,lpBuffer,dwBufferCount,&dwWriteCount,NULL);

	return dwWriteCount;
}

//������ʼ
DWORD CLoggerFile::SeekToBegin()
{
	return Seek(0,FILE_BEGIN);
}

//������β
DWORD CLoggerFile::SeekToEnd()
{
	return Seek(0,FILE_END);
}

//�ƶ�ָ��
DWORD CLoggerFile::Seek(LONGLONG lOffset,UINT nFrom)
{
	//���У��
	ASSERT(m_hFile!=INVALID_HANDLE_VALUE);
	if(m_hFile==INVALID_HANDLE_VALUE) return 0;

	//��������
	LARGE_INTEGER liDistance;
	liDistance.QuadPart = lOffset;

	//�ƶ�ָ��
	liDistance.LowPart = SetFilePointer(m_hFile,liDistance.LowPart,&liDistance.HighPart,nFrom);
	
	//��������
	if (liDistance.LowPart == INVALID_SET_FILE_POINTER && GetLastError()!= NO_ERROR)
	{

		liDistance.QuadPart = -1;
	}

	return static_cast<DWORD>(liDistance.QuadPart);
}

//��ȡ����
DWORD CLoggerFile::GetFileLength()
{
	//��ȡ����
	return GetFileSize(m_hFile,NULL);
}

//���ý�β
 BOOL CLoggerFile::SetEndOfFile()
 {
	 //�����ļ���β
	return ::SetEndOfFile(m_hFile);
 }

//���ó���
//BOOL CLoggerFile::SetFileValidLength(LONGLONG lValidlength)
//{
//	//�����ļ���β
//	SeekToEnd();
//	SetEndOfFile();
//
//	//������Ч����
//	return SetFileValidData(m_hFile,lValidlength);
//}

//�ر��ļ�
VOID CLoggerFile::Close()
{
	//�رվ��
	if(m_hFile!=INVALID_HANDLE_VALUE) 
	{
		CloseHandle(m_hFile);
		m_hFile=INVALID_HANDLE_VALUE;
	}

	//���ñ���
	m_szFileName[0]=0;
}
/////////////////////////////////////////////////////////////////////////////////////////////
