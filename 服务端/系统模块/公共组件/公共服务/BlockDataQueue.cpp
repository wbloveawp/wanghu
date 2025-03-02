#include "StdAfx.h"
#include "BlockDataQueue.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CBlockDataQueue::CBlockDataQueue()
{
	//��ѯ����
	m_dwInsertPos=0L;
	m_dwTerminalPos=0L;
	m_dwDataQueryPos=0L;

	//���ݱ���
	m_dwDataSize=0L;
	m_dwDataPacketCount=0L;

	//�������
	m_dwBufferSize=0L;
	m_pDataQueueBuffer=NULL;

	return;
}

//��������
CBlockDataQueue::~CBlockDataQueue()
{
	SafeDeleteArray(m_pDataQueueBuffer);
	return;
}

//��������
bool CBlockDataQueue::EnQueue(WORD wIdentifier, VOID * const pBuffer, WORD wDataSize)
{
	//��ʼ������
	tagDataHead DataHead;
	DataHead.wDataSize=wDataSize;
	DataHead.wIdentifier=wIdentifier;
	DWORD dwCopySize=sizeof(DataHead)+wDataSize;

	try
	{
		//Ч�黺��
		if ((m_dwDataSize+dwCopySize)>m_dwBufferSize) throw 0;
		else if ((m_dwInsertPos==m_dwTerminalPos)&&((m_dwInsertPos+dwCopySize)>m_dwBufferSize)) 
		{
			if (m_dwDataQueryPos>=dwCopySize) m_dwInsertPos=0;
			else throw 0;
		}
		else if ((m_dwInsertPos<m_dwTerminalPos)&&((m_dwInsertPos+dwCopySize)>=m_dwDataQueryPos)) throw 0;
	}
	catch (...)
	{
		try
		{
			//�����ڴ�
			DWORD dwNewBufferSize=m_dwBufferSize+__max(m_dwBufferSize/2L,(wDataSize+sizeof(tagDataHead))*10L);
			BYTE * pNewQueueServiceBuffer=new BYTE [dwNewBufferSize];
			if (pNewQueueServiceBuffer==NULL) return false;

			//��������
			if (m_pDataQueueBuffer!=NULL) 
			{
				ASSERT(m_dwTerminalPos>=m_dwDataSize);
				ASSERT(m_dwTerminalPos>=m_dwDataQueryPos);
				DWORD dwPartOneSize=m_dwTerminalPos-m_dwDataQueryPos;
				if (dwPartOneSize>0L) CopyMemory(pNewQueueServiceBuffer,m_pDataQueueBuffer+m_dwDataQueryPos,dwPartOneSize);
				if (m_dwDataSize>dwPartOneSize) 
				{
					ASSERT((m_dwInsertPos+dwPartOneSize)==m_dwDataSize);
					CopyMemory(pNewQueueServiceBuffer+dwPartOneSize,m_pDataQueueBuffer,m_dwInsertPos);
				}
			}

			//��������
			m_dwDataQueryPos=0L;
			m_dwInsertPos=m_dwDataSize;
			m_dwTerminalPos=m_dwDataSize;
			m_dwBufferSize=dwNewBufferSize;
			SafeDeleteArray(m_pDataQueueBuffer);
			m_pDataQueueBuffer=pNewQueueServiceBuffer;
		}
		catch (...) { return false; }
	}

	try
	{
		//��������
		CopyMemory(m_pDataQueueBuffer+m_dwInsertPos,&DataHead,sizeof(DataHead));
		if (wDataSize>0) 
		{
			ASSERT(pBuffer!=NULL);
			CopyMemory(m_pDataQueueBuffer+m_dwInsertPos+sizeof(DataHead),pBuffer,wDataSize);
		}

		//��������
		m_dwDataPacketCount++;
		m_dwDataSize+=dwCopySize;
		m_dwInsertPos+=dwCopySize;
		m_dwTerminalPos=__max(m_dwTerminalPos,m_dwInsertPos);

		return true;
	}
	catch (...) { }

	return false;
}

//��ȡ��ͷ����ɾ��
bool CBlockDataQueue::DeQueue(tagDataHead & DataHead, VOID * pBuffer, WORD wBufferSize)
{
	//Ч�����
	ASSERT(m_dwDataSize>0L);
	ASSERT(m_dwDataPacketCount>0);
	ASSERT(m_pDataQueueBuffer!=NULL);
	if (m_dwDataSize==0L) return false;
	if (m_dwDataPacketCount==0L) return false;

	//��������
	if (m_dwDataQueryPos==m_dwTerminalPos)
	{
		m_dwDataQueryPos=0L;
		m_dwTerminalPos=m_dwInsertPos;
	}

	//��ȡָ��
	ASSERT(m_dwBufferSize>=(m_dwDataQueryPos+sizeof(tagDataHead)));
	tagDataHead * pDataHead=(tagDataHead *)(m_pDataQueueBuffer+m_dwDataQueryPos);

	//��������ͷ
	DataHead=*pDataHead;

	//��ȡ��С
	WORD wPacketSize=sizeof(DataHead)+pDataHead->wDataSize;

	//Ч�����
	ASSERT(wPacketSize<=m_dwDataSize);
	ASSERT(m_dwBufferSize>=(m_dwDataQueryPos+wPacketSize));

	//��������
	if( pBuffer != NULL )
	{
		ASSERT(wBufferSize>=pDataHead->wDataSize);
		if( wBufferSize < pDataHead->wDataSize )
		{
			DataHead.wDataSize = 0;
		}
		else
		{
			//��������
			CopyMemory(pBuffer,pDataHead+1,DataHead.wDataSize);
		}
	}

	//ɾ������
	m_dwDataPacketCount--;
	m_dwDataSize-=wPacketSize;
	m_dwDataQueryPos+=wPacketSize;

	return true;
}

//ɾ����ͷ
bool CBlockDataQueue::DeQueue()
{
	//Ч�����
	ASSERT(m_dwDataSize>0L);
	ASSERT(m_dwDataPacketCount>0);
	ASSERT(m_pDataQueueBuffer!=NULL);
	if (m_dwDataSize==0L) return false;
	if (m_dwDataPacketCount==0L) return false;

	//��������
	if (m_dwDataQueryPos==m_dwTerminalPos)
	{
		m_dwDataQueryPos=0L;
		m_dwTerminalPos=m_dwInsertPos;
	}

	//��ȡָ��
	ASSERT(m_dwBufferSize>=(m_dwDataQueryPos+sizeof(tagDataHead)));
	tagDataHead * pDataHead=(tagDataHead *)(m_pDataQueueBuffer+m_dwDataQueryPos);

	//��ȡ��С
	WORD wPacketSize=sizeof(tagDataHead)+pDataHead->wDataSize;

	//Ч�����
	ASSERT(wPacketSize<=m_dwDataSize);
	ASSERT(m_dwBufferSize>=(m_dwDataQueryPos+wPacketSize));

	//ɾ������
	m_dwDataPacketCount--;
	m_dwDataSize-=wPacketSize;
	m_dwDataQueryPos+=wPacketSize;

	return true;
}

//��ȡ��ͷ
bool CBlockDataQueue::GetHead(tagDataHead & DataHead, VOID * pBuffer, WORD wBufferSize)
{
	//Ч�����
	if (m_dwDataSize==0L) return false;
	if (m_dwDataPacketCount==0L) return false;
	ASSERT(m_pDataQueueBuffer!=NULL);

	//��������
	if (m_dwDataQueryPos==m_dwTerminalPos)
	{
		m_dwDataQueryPos=0L;
		m_dwTerminalPos=m_dwInsertPos;
	}

	//��ȡָ��
	ASSERT(m_dwBufferSize>=(m_dwDataQueryPos+sizeof(tagDataHead)));
	tagDataHead * pDataHead=(tagDataHead *)(m_pDataQueueBuffer+m_dwDataQueryPos);

	//��������ͷ
	DataHead=*pDataHead;

	//��ȡ��С
	WORD wPacketSize=sizeof(DataHead)+pDataHead->wDataSize;

	//Ч�����
	ASSERT(wPacketSize<=m_dwDataSize);
	ASSERT(m_dwBufferSize>=(m_dwDataQueryPos+wPacketSize));

	//��������
	if( pBuffer != NULL )
	{
		ASSERT(wBufferSize>=pDataHead->wDataSize);
		if( wBufferSize < pDataHead->wDataSize )
		{
			DataHead.wDataSize = 0;
		}
		else
		{
			//��������
			CopyMemory(pBuffer,pDataHead+1,DataHead.wDataSize);
		}
	}

	return true;
}

//�������
VOID CBlockDataQueue::ClearQueue(bool bFreeMemory)
{
	//���ñ���
	m_dwDataSize=0L;
	m_dwInsertPos=0L;
	m_dwTerminalPos=0L;
	m_dwDataQueryPos=0L;
	m_dwDataPacketCount=0L;

	//ɾ���ڴ�
	if (bFreeMemory==true) 
	{
		m_dwBufferSize=0;
		SafeDeleteArray(m_pDataQueueBuffer);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
