#include "StdAfx.h"
#include "WHDataQueue.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CWHDataQueue::CWHDataQueue()
{
	//��ѯ����
	m_dwInsertPos = 0L;
	m_dwSearchPos = 0L;
	m_dwTerminalPos = 0L;

	//���ݱ���
	m_dwDataSize = 0L;
	m_dwDataPacketCount = 0L;

	//�������
	m_dwBufferSize = 0L;
	m_pDataQueueBuffer = NULL;

	return;
}

//��������
CWHDataQueue::~CWHDataQueue()
{
	SafeDeleteArray(m_pDataQueueBuffer);
	return;
}

//������Ϣ
VOID CWHDataQueue::GetBurthenInfo(tagBurthenInfo & BurthenInfo)
{
	//���ñ���
	BurthenInfo.dwDataSize = m_dwDataSize;
	BurthenInfo.dwBufferSize = m_dwBufferSize;
	BurthenInfo.dwDataPacketCount = m_dwDataPacketCount;

	return;
}

//��������
bool CWHDataQueue::InsertData(WORD dwIdentifier, VOID * pBuffer, WORD wDataSize)
{
	//��������
	tagDataHead DataHead;
	ZeroMemory(&DataHead, sizeof(DataHead));

	//���ñ���
	DataHead.wDataSize = wDataSize;
	DataHead.dwIdentifier = dwIdentifier;
	DataHead.lInsertTime = GetTickCount64();

	//�����洢
	if (RectifyBuffer(sizeof(DataHead) + DataHead.wDataSize) == false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	try
	{
		//��������
		CopyMemory(m_pDataQueueBuffer + m_dwInsertPos, &DataHead, sizeof(DataHead));

		//��������
		if (wDataSize > 0)
		{
			ASSERT(pBuffer != NULL);
			CopyMemory(m_pDataQueueBuffer + m_dwInsertPos + sizeof(DataHead), pBuffer, wDataSize);
		}

		//��������
		m_dwDataPacketCount++;
		m_dwDataSize += sizeof(DataHead) + wDataSize;
		m_dwInsertPos += sizeof(DataHead) + wDataSize;
		m_dwTerminalPos = __max(m_dwTerminalPos, m_dwInsertPos);

		return true;
	}
	catch (...)
	{
		ASSERT(FALSE);
		return false;
	}

	return false;
}

//��������
bool CWHDataQueue::InsertData(WORD dwIdentifier, tagDataBuffer DataBuffer[], WORD wDataCount)
{
	//��������
	tagDataHead DataHead;
	ZeroMemory(&DataHead, sizeof(DataHead));

	//���ñ���
	DataHead.wDataSize = 0;
	DataHead.dwIdentifier = dwIdentifier;
	DataHead.lInsertTime = GetTickCount64();

	//�ۼƴ�С
	for (WORD i = 0; i < wDataCount; i++)
	{
		if (DataBuffer[i].wDataSize > 0)
		{
			DataHead.wDataSize += DataBuffer[i].wDataSize;
		}
	}

	//�����洢
	if (RectifyBuffer(sizeof(DataHead) + DataHead.wDataSize) == false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	try
	{
		//��������
		CopyMemory(m_pDataQueueBuffer + m_dwInsertPos, &DataHead, sizeof(DataHead));

		//��������
		if (DataHead.wDataSize > 0)
		{
			//��������
			WORD wExcursion = sizeof(DataHead);

			//��������
			for (WORD i = 0; i < wDataCount; i++)
			{
				if (DataBuffer[i].wDataSize > 0L)
				{
					//Ч��״̬
					ASSERT(DataBuffer[i].pDataBuffer != NULL);

					//��������
					CopyMemory(m_pDataQueueBuffer + m_dwInsertPos + wExcursion, DataBuffer[i].pDataBuffer, DataBuffer[i].wDataSize);

					//���ñ���
					wExcursion += DataBuffer[i].wDataSize;
				}
			}
		}

		//��������
		m_dwDataPacketCount++;
		m_dwDataSize += sizeof(DataHead) + DataHead.wDataSize;
		m_dwInsertPos += sizeof(DataHead) + DataHead.wDataSize;
		m_dwTerminalPos = __max(m_dwTerminalPos, m_dwInsertPos);

		return true;
	}
	catch (...)
	{
		ASSERT(FALSE);
		return false;
	}

	return false;
}

//ɾ������
VOID CWHDataQueue::RemoveData(DWORD dwPacketCount)
{
	//��������
	ASSERT(dwPacketCount <= m_dwDataPacketCount);
	if ((dwPacketCount > m_dwDataPacketCount)) dwPacketCount = m_dwDataPacketCount;

	//ɾ������
	for (DWORD i = 0; i < dwPacketCount; i++)
	{
		//Ч�����
		ASSERT((m_dwDataSize > 0L) && (m_dwDataPacketCount > 0L));
		if ((m_dwDataSize == 0L) || (m_dwDataPacketCount == 0L)) return;

		//��������
		if (m_dwSearchPos == m_dwTerminalPos)
		{
			m_dwSearchPos = 0L;
			m_dwTerminalPos = m_dwInsertPos;
		}

		//��ȡָ��
		ASSERT(m_dwBufferSize >= (m_dwSearchPos + sizeof(tagDataHead)));
		tagDataHead * pDataHead = (tagDataHead *)(m_pDataQueueBuffer + m_dwSearchPos);

		//Ч��״̬
		ASSERT(m_dwDataSize >= (sizeof(tagDataHead) + pDataHead->wDataSize));
		ASSERT(m_dwBufferSize >= (m_dwSearchPos + sizeof(tagDataHead) + pDataHead->wDataSize));

		//ɾ������
		m_dwDataPacketCount--;
		m_dwDataSize -= (sizeof(tagDataHead) + pDataHead->wDataSize);
		m_dwSearchPos += (sizeof(tagDataHead) + pDataHead->wDataSize);
	}

	return;
}

//��ȡ����
bool CWHDataQueue::DistillData(tagDataHead & DataHead, VOID * pDataBuffer, WORD wBufferSize, bool bRemoveData)
{
	//Ч�����
	ASSERT(m_dwDataSize > 0L);
	ASSERT(m_dwDataPacketCount > 0);
	ASSERT(m_pDataQueueBuffer != NULL);

	//Ч�����
	if (m_dwDataSize == 0L) return false;
	if (m_dwDataPacketCount == 0L) return false;

	//��������
	if (m_dwSearchPos == m_dwTerminalPos)
	{
		m_dwSearchPos = 0L;
		m_dwTerminalPos = m_dwInsertPos;
	}

	//��ȡָ��
	ASSERT(m_dwBufferSize >= (m_dwSearchPos + sizeof(tagDataHead)));
	tagDataHead * pDataHead = (tagDataHead *)(m_pDataQueueBuffer + m_dwSearchPos);

	//��ȡ��С
	WORD wDataSize = pDataHead->wDataSize;
	WORD wPacketSize = sizeof(DataHead) + pDataHead->wDataSize;

	//��������
	CopyMemory(&DataHead, pDataHead, sizeof(DataHead));

	//��������
	if (wDataSize > 0)
	{
		if (wBufferSize < wDataSize)
		{
			DataHead.wDataSize = 0L;
		}
		else
		{
			CopyMemory(pDataBuffer, pDataHead + 1, DataHead.wDataSize);
		}
	}

	//ɾ������
	if (bRemoveData == true)
	{
		//Ч��״̬
		ASSERT(m_dwDataSize >= wPacketSize);
		ASSERT(m_dwBufferSize >= (m_dwSearchPos + wPacketSize));

		//���ñ���
		m_dwDataPacketCount--;
		m_dwDataSize -= wPacketSize;
		m_dwSearchPos += wPacketSize;
	}

	return true;
}

//ɾ������
VOID CWHDataQueue::RemoveData(bool bFreeMemroy)
{
	//���ñ���
	m_dwSearchPos = 0L;
	m_dwInsertPos = 0L;
	m_dwTerminalPos = 0L;

	//���ñ���
	m_dwDataSize = 0L;
	m_dwDataPacketCount = 0L;

	//ɾ���ڴ�
	if (bFreeMemroy == true)
	{
		m_dwBufferSize = 0;
		SafeDeleteArray(m_pDataQueueBuffer);
	}

	return;
}

//���ݶ���
VOID CWHDataQueue::BackupQueue(tagQueueSnapshoot& QueueSnapshoot)
{
	//��ѯ����
	QueueSnapshoot.dwSearchPos=m_dwSearchPos;
	QueueSnapshoot.dwInsertPos=m_dwInsertPos;
	QueueSnapshoot.dwTerminalPos=m_dwTerminalPos;

	//���ݱ���
	QueueSnapshoot.dwDataSize=m_dwDataSize;
	QueueSnapshoot.dwDataPacketCount=m_dwDataPacketCount;

	//�������
	QueueSnapshoot.dwBufferSize=m_dwBufferSize;
}

//�ָ�����
bool CWHDataQueue::ResumeQueue(const tagQueueSnapshoot& QueueSnapshoot)
{
	//У���С
	if (QueueSnapshoot.dwBufferSize != m_dwBufferSize)
	{
		return false;
	}

	//��ѯ����
	m_dwSearchPos=QueueSnapshoot.dwSearchPos;
	m_dwInsertPos=QueueSnapshoot.dwInsertPos;
	m_dwTerminalPos=QueueSnapshoot.dwTerminalPos;

	//���ݱ���
	m_dwDataSize=QueueSnapshoot.dwDataSize;
	m_dwDataPacketCount=QueueSnapshoot.dwDataPacketCount;

	return true;
}

//�����ڴ�
bool CWHDataQueue::MallocBuffer(DWORD dwNeedSize)
{
	try
	{
		//�����ڴ�
		DWORD dwMinBufferRise = __min(m_dwBufferSize / 2L, 1024L);
		DWORD dwRiseBufferSize = __max(dwMinBufferRise, dwNeedSize * 10L);
		LPBYTE pDataQueueBuffer = new BYTE[m_dwBufferSize + dwRiseBufferSize];

		//�����ж�
		ASSERT(pDataQueueBuffer != NULL);
		if (pDataQueueBuffer == NULL) return false;

		//��������
		if (m_pDataQueueBuffer != NULL)
		{
			//Ч��״̬
			ASSERT(m_dwTerminalPos >= m_dwDataSize);
			ASSERT(m_dwTerminalPos >= m_dwSearchPos);

			//��������
			DWORD dwHeadDataSize = m_dwTerminalPos - m_dwSearchPos;
			if (dwHeadDataSize > 0L) CopyMemory(pDataQueueBuffer, m_pDataQueueBuffer + m_dwSearchPos, dwHeadDataSize);

			if (m_dwDataSize > dwHeadDataSize)
			{
				ASSERT((m_dwInsertPos + dwHeadDataSize) == m_dwDataSize);
				CopyMemory(pDataQueueBuffer + dwHeadDataSize, m_pDataQueueBuffer, m_dwInsertPos);
			}
		}

		//���û���
		SafeDeleteArray(m_pDataQueueBuffer);
		m_pDataQueueBuffer = pDataQueueBuffer;

		//���ñ���
		m_dwSearchPos = 0L;
		m_dwInsertPos = m_dwDataSize;
		m_dwTerminalPos = m_dwDataSize;
		m_dwBufferSize = m_dwBufferSize + dwRiseBufferSize;
	}
	catch (...)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//�����洢
bool CWHDataQueue::RectifyBuffer(DWORD dwNeedSize)
{
	//Ч��״̬
	ASSERT(m_dwSearchPos <= m_dwTerminalPos);
	ASSERT(m_dwInsertPos <= m_dwTerminalPos);

	//����ģʽ
	if ((m_dwInsertPos == m_dwTerminalPos) && ((m_dwInsertPos + dwNeedSize) > m_dwBufferSize))
	{
		//�����ڴ�
		if (m_dwSearchPos < dwNeedSize)
		{
			return MallocBuffer(dwNeedSize);
		}

		//��ͷ��ʼ
		if (m_dwSearchPos >= dwNeedSize)
		{
			//���ñ���
			m_dwInsertPos = 0L;

			//���ñ���
			if (m_dwSearchPos == m_dwTerminalPos)
			{
				m_dwSearchPos = 0L;
				m_dwTerminalPos = 0L;
			}
		}

		return true;
	}

	//ͷβģʽ
	if ((m_dwInsertPos < m_dwTerminalPos) && ((m_dwInsertPos + dwNeedSize) >= m_dwSearchPos))
	{
		return MallocBuffer(dwNeedSize);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
