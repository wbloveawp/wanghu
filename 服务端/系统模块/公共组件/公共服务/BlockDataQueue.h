#ifndef BLOCK_DATA_QUEUE_HEAD_FILE
#define BLOCK_DATA_QUEUE_HEAD_FILE

#pragma once

#include "CommonServiceHead.h"
#include "DataQueue.h"

//////////////////////////////////////////////////////////////////////////

//���ݶ���
class COMMON_SERVICE_CLASS CBlockDataQueue
{
	//��ѯ����
protected:
	DWORD							m_dwInsertPos;						//����λ��
	DWORD							m_dwTerminalPos;					//����λ��
	DWORD							m_dwDataQueryPos;					//��ѯλ��

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
	CBlockDataQueue();
	//��������
	virtual ~CBlockDataQueue();

	//���ܺ���
public:
	//��ѯ��Ŀ
	DWORD GetLength() { return m_dwDataPacketCount; }

	//���ݹ���
public:
	//��������
	bool EnQueue(WORD wIdentifier, VOID * const pBuffer, WORD wDataSize);
	//��ȡ��ͷ����ɾ��
	bool DeQueue(tagDataHead & DataHead, VOID * pBuffer, WORD wBufferSize);
	//ɾ����ͷ
	bool DeQueue();
	//��ȡ��ͷ
	bool GetHead(tagDataHead & DataHead, VOID * pBuffer, WORD wBufferSize);
	//�������
	VOID ClearQueue(bool bFreeMemory);
};

//////////////////////////////////////////////////////////////////////////

#endif