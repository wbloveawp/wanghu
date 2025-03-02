#include "StdAfx.h"
#include "Datafile.h"
//#include "MD5CheckSum.h"


////////////////////////////////////////////////////////////////////////////////////////
//�ļ�ͷ��
const BYTE g_cbFileHead[] = {'H','M','X','T','T','D','O','0'};

////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CDataFile::CDataFile()
{
}

//��������
CDataFile::~CDataFile()
{
	//�ر��ļ�
	CloseFile();
}

//���ļ�
BOOL CDataFile::OpenDataFile(LPCTSTR pszFileName)
{
	//���ļ�
	m_CustomFile.Create(pszFileName,GENERIC_READ|GENERIC_WRITE);
	if(m_CustomFile.IsValid()==FALSE) return FALSE;

	//��������
	tagFileHead FileHead;
	
	//��ȡͷ��
	m_CustomFile.SeekToBegin();
	m_CustomFile.Read(&FileHead,sizeof(FileHead));

	//��������
	BOOL bSuccessed=TRUE;

  //У��ͷ��
	for(BYTE cbIndex=0;cbIndex<CountArray(FileHead.cbFileHead);cbIndex++)
	{
		if(FileHead.cbFileHead[cbIndex]!=g_cbFileHead[cbIndex])
		{
			bSuccessed=FALSE;
			break;
		}
	}

	//ƥ��ʧ��
	if (bSuccessed==FALSE) m_CustomFile.Close();

	return bSuccessed;
}

//�����ļ�
BOOL CDataFile::CreateDataFile(LPCTSTR pszFileName)
{
	//�����ļ�
	m_CustomFile.Create(pszFileName,GENERIC_READ|GENERIC_WRITE,CREATE_ALWAYS);

	//д��汾
	if (m_CustomFile.IsValid()==TRUE)
	{
		//����ṹ
		tagFileHead FileHead;
		CopyMemory(FileHead.cbFileHead,g_cbFileHead,CountArray(FileHead.cbFileHead));

		//д������
		m_CustomFile.Write(&FileHead,sizeof(FileHead));

		//����ṹ
		tagDataCollect DataCollect;
		ZeroMemory(&DataCollect,sizeof(DataCollect));

		//�ƶ�ָ��
		m_CustomFile.SeekToEnd();
		m_CustomFile.Write(&DataCollect,sizeof(DataCollect));
	}

	return m_CustomFile.IsValid();
}

//��������
VOID CDataFile::UpdateDataItem(LPVOID lpBuffer,WORD wBufferSize)
{
	//����У��
	if(lpBuffer==NULL) return;

	//��Ч�ж�
	if(m_CustomFile.IsValid()==FALSE) return;

	//д������
	return m_CustomFile.Write(lpBuffer,wBufferSize);
}

//ɾ������
BOOL CDataFile::DeleteDataItem(WORD wItemIndex,WORD wItemSize)
{
	//��������
	WORD wItemCount = GetItemCount();
	if(wItemIndex>=wItemCount) return FALSE;

	//����ƫ��
	SCORE lOffset = sizeof(tagFileHead)+sizeof(tagDataCollect)+wItemIndex*wItemSize;	

	//λ���ж�
	if(lOffset<m_CustomFile.GetFileLength()-wItemSize)
	{
		//�����ڴ�
		LPBYTE pcbFileData = new BYTE[(wItemCount-1)*wItemSize];
		if(pcbFileData==NULL) return FALSE;

		//��ȡ����
		m_CustomFile.Seek(lOffset+wItemSize,FILE_BEGIN);
		m_CustomFile.Read(pcbFileData,(wItemCount-1)*wItemSize);

		//д������
		m_CustomFile.Seek(lOffset,FILE_BEGIN);
		m_CustomFile.Write(pcbFileData,(wItemCount-1)*wItemSize);	

		//��ȡ����
		tagDataCollect DataCollect;
		m_CustomFile.Seek(sizeof(tagFileHead),FILE_BEGIN);
		m_CustomFile.Read(&DataCollect,sizeof(DataCollect));

		//���±���
		DataCollect.wItemCount--;
		m_CustomFile.Seek(sizeof(tagFileHead),FILE_BEGIN);
		m_CustomFile.Write(&DataCollect,sizeof(DataCollect));

		//�ͷ���Դ
		SafeDelete(pcbFileData);

		//���ý�β
		return m_CustomFile.SetEndOfFile();
	}

	return TRUE;
}

//д������
VOID CDataFile::WriteDataItem(LPVOID lpBuffer,WORD wBufferSize)
{
	//����У��
	if (lpBuffer==NULL) return;

	//��Ч�ж�
	if (m_CustomFile.IsValid()==FALSE) return;

	//��������
	tagDataCollect DataCollect;

	//�ƶ�ָ��
	m_CustomFile.Seek(sizeof(tagFileHead),FILE_BEGIN);

	//��ȡ����
	m_CustomFile.Read(&DataCollect,sizeof(DataCollect));

	//���±���
	++DataCollect.wItemCount;
	m_CustomFile.Seek(sizeof(tagFileHead),FILE_BEGIN);
	m_CustomFile.Write(&DataCollect,sizeof(DataCollect));

	//д������
	m_CustomFile.SeekToEnd();
	m_CustomFile.Write(lpBuffer,wBufferSize);

	return;
}


//��ȡ����
DWORD CDataFile::ReadDataItem(LPVOID lpBuffer,WORD wItemSize,WORD wItemIndex)
{
	//����У��
	if (lpBuffer==NULL) return 0L;

	//��Ч�ж�
	if (m_CustomFile.IsValid()==FALSE) return 0L;

	//�ƶ�ָ��
	m_CustomFile.Seek(sizeof(tagFileHead)+sizeof(tagDataCollect)+wItemIndex*wItemSize,FILE_BEGIN);

  //��ȡ����
	return m_CustomFile.Read(lpBuffer,wItemSize);
}

//������Ŀ
WORD CDataFile::GetItemCount()
{
	//��Ч�ж�
	if (m_CustomFile.IsValid()==FALSE) return 0;

	//��������
	tagDataCollect DataCollect;

	//�ƶ�ָ��
	m_CustomFile.Seek(sizeof(tagFileHead),FILE_BEGIN);

	//��ȡ����
	m_CustomFile.Read(&DataCollect,sizeof(DataCollect));

	//���±���
	return DataCollect.wItemCount;
}

//�ر��ļ�
VOID CDataFile::CloseFile()
{
	if (m_CustomFile.IsValid()==TRUE)
	{
		//����У���
		//BuildCheckSum();

		//�ر��ļ�
		m_CustomFile.Close();
	}
}

//����У���
VOID CDataFile::BuildCheckSum()
{
	if (m_CustomFile.IsValid()==FALSE) return;

	//��������
	DWORD dwDataSize = m_CustomFile.GetFileLength()-sizeof(tagFileHead);

	try
	{
		//������Դ
		LPBYTE pData = new BYTE[dwDataSize];
		if(pData==NULL) return;

		//��ȡ�ļ�ͷ
		tagFileHead FileHead;
		m_CustomFile.SeekToBegin();
		m_CustomFile.Read(&FileHead,sizeof(FileHead));

		//��ȡ����
		m_CustomFile.Read(pData,dwDataSize);

		//��������
		m_CustomFile.SeekToBegin();
		m_CustomFile.Write(&FileHead,sizeof(FileHead));

		//�ͷ���Դ
		SafeDelete(pData);
	}
	catch(...){}

	return;
};

////////////////////////////////////////////////////////////////////////////////////////