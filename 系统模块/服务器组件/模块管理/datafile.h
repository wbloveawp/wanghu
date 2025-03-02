#ifndef DATA_FILE_HEAD_FILE
#define DATA_FILE_HEAD_FILE
#pragma once

//�����ļ�
#include "ModuleManagerHead.h"

/////////////////////////////////////////////////////////////////////////////////////////
//�ṹ����

//�ļ�ͷ��
struct tagFileHead
{
	BYTE									cbFileHead[8];						//�ļ�ͷ��
};

//�����ռ�
struct tagDataCollect
{
	WORD									wItemCount;							//������Ŀ
};

/////////////////////////////////////////////////////////////////////////////////////////
//�����ļ�
class CDataFile
{
	//�ں˱���
protected:
	CWHCustomFile							m_CustomFile;						//�Զ��ļ�

	//��������
public:
	//���캯��
	CDataFile();
	//��������
	virtual ~CDataFile();

	//��������
public:
	//���ļ�
	BOOL OpenDataFile(LPCTSTR pszFileName);
	//�����ļ�
	BOOL CreateDataFile(LPCTSTR pszFileName);	
	//��������
	VOID UpdateDataItem(LPVOID lpBuffer,WORD wBufferSize);
	//ɾ������
	BOOL DeleteDataItem(WORD wItemIndex,WORD wItemSize);
	//д������
	VOID WriteDataItem(LPVOID lpBuffer,WORD wBufferSize);
  //ö������
	DWORD ReadDataItem(LPVOID lpBuffer,WORD wItemSize,WORD wItemIndex);

	//��������
public:
	//������Ŀ
	WORD GetItemCount();	
	//�ر��ļ�
	VOID CloseFile();

	//�ڲ�����
protected:	
	//����У���
	VOID BuildCheckSum();

};

/////////////////////////////////////////////////////////////////////////////////////////
#endif

