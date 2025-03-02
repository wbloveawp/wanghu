#include "StdAfx.h"
#include "WHCustomfile.h"

/////////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CWHCustomFile::CWHCustomFile(void)
{
	//���ñ���
	m_szFileName[0]=0;
	m_hFile=INVALID_HANDLE_VALUE;	
}

//��������
CWHCustomFile::~CWHCustomFile(void)
{
	//�ر��ļ�
	Close();
}

//�����ļ�
BOOL CWHCustomFile::Create(LPCTSTR pszFileName,DWORD dwGate,DWORD dwCreation)
{
	return Create(pszFileName,dwGate,0,NULL,dwCreation,FILE_ATTRIBUTE_NORMAL,NULL);
}

//�����ļ�
BOOL CWHCustomFile::Create(LPCTSTR pszFileName,DWORD dwGate,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreation,DWORD dwAttributes,HANDLE hTemplateFile)
{
	//�����ļ�
	m_hFile = CreateFile(pszFileName,dwGate,dwShareMode,lpSecurityAttributes,dwCreation,dwAttributes,hTemplateFile);

	//У����
	if(m_hFile==INVALID_HANDLE_VALUE) return FALSE;

	//�����ļ���
	StringCchCopy(m_szFileName,CountArray(m_szFileName), pszFileName);

	return TRUE;
}

//��ȡ�ļ�
DWORD CWHCustomFile::Read(LPVOID lpBuffer,DWORD dwBufferCount)
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
VOID CWHCustomFile::Write(LPCVOID lpBuffer,DWORD dwBufferCount)
{
	//���У��
	ASSERT(m_hFile!=INVALID_HANDLE_VALUE);
	if(m_hFile==INVALID_HANDLE_VALUE) return;

	//����У��
	ASSERT(lpBuffer!=NULL);
	if(lpBuffer==NULL) return;

	//��������
	DWORD dwWriteCount=0;

	//д���ļ�
	WriteFile(m_hFile,lpBuffer,dwBufferCount,&dwWriteCount,NULL);

	return;
}

//������ʼ
DWORD CWHCustomFile::SeekToBegin()
{
	return Seek(0,FILE_BEGIN);
}

//������β
DWORD CWHCustomFile::SeekToEnd()
{
	return Seek(0,FILE_END);
}

//�ƶ�ָ��
DWORD CWHCustomFile::Seek(SCORE lOffset,UINT nFrom)
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
DWORD CWHCustomFile::GetFileLength()
{
	//��ȡ����
	return GetFileSize(m_hFile,NULL);
}

//���ý�β
 BOOL CWHCustomFile::SetEndOfFile()
 {
	 //�����ļ���β
	return ::SetEndOfFile(m_hFile);
 }

//���ó���
//BOOL CWHCustomFile::SetFileValidLength(SCORE lValidlength)
//{
//	//�����ļ���β
//	SeekToEnd();
//	SetEndOfFile();
//
//	//������Ч����
//	return SetFileValidData(m_hFile,lValidlength);
//}

//�ر��ļ�
VOID CWHCustomFile::Close()
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
