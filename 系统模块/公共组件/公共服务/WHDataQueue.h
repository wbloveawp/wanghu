#ifndef WH_DATA_QUEUE_HEAD_FILE
#define WH_DATA_QUEUE_HEAD_FILE

#pragma once

#include "CommonServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//�ṹ����

//������ͷ
struct tagDataHead
{
	WORD							wDataSize;							//���ݴ�С
	DWORD							dwIdentifier;						//���ͱ�ʶ
	ULONGLONG						lInsertTime;						//����ʱ��	
};

//������Ϣ
struct tagBurthenInfo
{
	DWORD							dwDataSize;							//���ݴ�С
	DWORD							dwBufferSize;						//���峤��
	DWORD							dwDataPacketCount;					//���ݰ���
};

//������Ϣ
struct tagDataBuffer
{
	WORD							wDataSize;							//���ݴ�С
	LPVOID							pDataBuffer;						//����ָ��
};

//���п���
struct tagQueueSnapshoot
{
	//��ѯ����
	DWORD							dwSearchPos;						//��ѯλ��
	DWORD							dwInsertPos;						//����λ��
	DWORD							dwTerminalPos;						//����λ��

	//���ݱ���
	DWORD							dwDataSize;							//���ݴ�С
	DWORD							dwDataPacketCount;					//���ݰ���

	//�������
	DWORD							dwBufferSize;						//���峤��
};

//////////////////////////////////////////////////////////////////////////////////

//���ݶ���
class COMMON_SERVICE_CLASS CWHDataQueue
{
	//��ѯ����
protected:
	DWORD							m_dwSearchPos;						//��ѯλ��
	DWORD							m_dwInsertPos;						//����λ��
	DWORD							m_dwTerminalPos;					//����λ��

	//���ݱ���
protected:
	DWORD							m_dwDataSize;						//���ݴ�С
	DWORD							m_dwDataPacketCount;				//���ݰ���

	//�������
protected:
	DWORD							m_dwBufferSize;						//���峤��
	LPBYTE							m_pDataQueueBuffer;					//����ָ��

	//��������
public:
	//���캯��
	CWHDataQueue();
	//��������
	virtual ~CWHDataQueue();

	//���ܺ���
public:
	//������Ϣ
	VOID GetBurthenInfo(tagBurthenInfo & BurthenInfo);
	//������Ϣ
	DWORD GetDataPacketCount() { return m_dwDataPacketCount; }

	//��������
public:
	//��������
	bool InsertData(WORD dwIdentifier, VOID * pBuffer, WORD wDataSize);
	//��������
	bool InsertData(WORD dwIdentifier, tagDataBuffer DataBuffer[], WORD wDataCount);

	//���ݹ���
public:
	//ɾ������
	VOID RemoveData(bool bFreeMemroy);
	//ɾ������
	VOID RemoveData(DWORD dwPacketCount);
	//��ȡ����
	bool DistillData(tagDataHead & DataHead, VOID * pDataBuffer, WORD wBufferSize, bool bRemoveData);

	//���й���
public:
	//���ݶ���
	VOID BackupQueue(tagQueueSnapshoot& QueueSnapshoot);
	//�ָ�����
	bool ResumeQueue(const tagQueueSnapshoot& QueueSnapshoot);

	//�ڲ�����
private:
	//�����ڴ�
	bool MallocBuffer(DWORD dwNeedSize);
	//�����洢
	bool RectifyBuffer(DWORD dwNeedSize);
};

//////////////////////////////////////////////////////////////////////////////////

#endif