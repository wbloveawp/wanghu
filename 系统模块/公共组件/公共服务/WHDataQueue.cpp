#include "StdAfx.h"
#include "WHDataQueue.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CWHDataQueue::CWHDataQueue()
{
	//查询变量
	m_dwInsertPos = 0L;
	m_dwSearchPos = 0L;
	m_dwTerminalPos = 0L;

	//数据变量
	m_dwDataSize = 0L;
	m_dwDataPacketCount = 0L;

	//缓冲变量
	m_dwBufferSize = 0L;
	m_pDataQueueBuffer = NULL;

	return;
}

//析构函数
CWHDataQueue::~CWHDataQueue()
{
	SafeDeleteArray(m_pDataQueueBuffer);
	return;
}

//负荷信息
VOID CWHDataQueue::GetBurthenInfo(tagBurthenInfo & BurthenInfo)
{
	//设置变量
	BurthenInfo.dwDataSize = m_dwDataSize;
	BurthenInfo.dwBufferSize = m_dwBufferSize;
	BurthenInfo.dwDataPacketCount = m_dwDataPacketCount;

	return;
}

//插入数据
bool CWHDataQueue::InsertData(WORD dwIdentifier, VOID * pBuffer, WORD wDataSize)
{
	//变量定义
	tagDataHead DataHead;
	ZeroMemory(&DataHead, sizeof(DataHead));

	//设置变量
	DataHead.wDataSize = wDataSize;
	DataHead.dwIdentifier = dwIdentifier;
	DataHead.lInsertTime = GetTickCount64();

	//调整存储
	if (RectifyBuffer(sizeof(DataHead) + DataHead.wDataSize) == false)
	{
		ASSERT(FALSE);
		return false;
	}

	//插入数据
	try
	{
		//拷贝数据
		CopyMemory(m_pDataQueueBuffer + m_dwInsertPos, &DataHead, sizeof(DataHead));

		//附加数据
		if (wDataSize > 0)
		{
			ASSERT(pBuffer != NULL);
			CopyMemory(m_pDataQueueBuffer + m_dwInsertPos + sizeof(DataHead), pBuffer, wDataSize);
		}

		//调整数据
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

//插入数据
bool CWHDataQueue::InsertData(WORD dwIdentifier, tagDataBuffer DataBuffer[], WORD wDataCount)
{
	//变量定义
	tagDataHead DataHead;
	ZeroMemory(&DataHead, sizeof(DataHead));

	//设置变量
	DataHead.wDataSize = 0;
	DataHead.dwIdentifier = dwIdentifier;
	DataHead.lInsertTime = GetTickCount64();

	//累计大小
	for (WORD i = 0; i < wDataCount; i++)
	{
		if (DataBuffer[i].wDataSize > 0)
		{
			DataHead.wDataSize += DataBuffer[i].wDataSize;
		}
	}

	//调整存储
	if (RectifyBuffer(sizeof(DataHead) + DataHead.wDataSize) == false)
	{
		ASSERT(FALSE);
		return false;
	}

	//插入数据
	try
	{
		//拷贝数据
		CopyMemory(m_pDataQueueBuffer + m_dwInsertPos, &DataHead, sizeof(DataHead));

		//附加数据
		if (DataHead.wDataSize > 0)
		{
			//变量定义
			WORD wExcursion = sizeof(DataHead);

			//插入数据
			for (WORD i = 0; i < wDataCount; i++)
			{
				if (DataBuffer[i].wDataSize > 0L)
				{
					//效验状态
					ASSERT(DataBuffer[i].pDataBuffer != NULL);

					//拷贝数据
					CopyMemory(m_pDataQueueBuffer + m_dwInsertPos + wExcursion, DataBuffer[i].pDataBuffer, DataBuffer[i].wDataSize);

					//设置变量
					wExcursion += DataBuffer[i].wDataSize;
				}
			}
		}

		//调整数据
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

//删除数据
VOID CWHDataQueue::RemoveData(DWORD dwPacketCount)
{
	//调整变量
	ASSERT(dwPacketCount <= m_dwDataPacketCount);
	if ((dwPacketCount > m_dwDataPacketCount)) dwPacketCount = m_dwDataPacketCount;

	//删除数据
	for (DWORD i = 0; i < dwPacketCount; i++)
	{
		//效验变量
		ASSERT((m_dwDataSize > 0L) && (m_dwDataPacketCount > 0L));
		if ((m_dwDataSize == 0L) || (m_dwDataPacketCount == 0L)) return;

		//调整参数
		if (m_dwSearchPos == m_dwTerminalPos)
		{
			m_dwSearchPos = 0L;
			m_dwTerminalPos = m_dwInsertPos;
		}

		//获取指针
		ASSERT(m_dwBufferSize >= (m_dwSearchPos + sizeof(tagDataHead)));
		tagDataHead * pDataHead = (tagDataHead *)(m_pDataQueueBuffer + m_dwSearchPos);

		//效验状态
		ASSERT(m_dwDataSize >= (sizeof(tagDataHead) + pDataHead->wDataSize));
		ASSERT(m_dwBufferSize >= (m_dwSearchPos + sizeof(tagDataHead) + pDataHead->wDataSize));

		//删除数据
		m_dwDataPacketCount--;
		m_dwDataSize -= (sizeof(tagDataHead) + pDataHead->wDataSize);
		m_dwSearchPos += (sizeof(tagDataHead) + pDataHead->wDataSize);
	}

	return;
}

//获取数据
bool CWHDataQueue::DistillData(tagDataHead & DataHead, VOID * pDataBuffer, WORD wBufferSize, bool bRemoveData)
{
	//效验变量
	ASSERT(m_dwDataSize > 0L);
	ASSERT(m_dwDataPacketCount > 0);
	ASSERT(m_pDataQueueBuffer != NULL);

	//效验变量
	if (m_dwDataSize == 0L) return false;
	if (m_dwDataPacketCount == 0L) return false;

	//调整参数
	if (m_dwSearchPos == m_dwTerminalPos)
	{
		m_dwSearchPos = 0L;
		m_dwTerminalPos = m_dwInsertPos;
	}

	//获取指针
	ASSERT(m_dwBufferSize >= (m_dwSearchPos + sizeof(tagDataHead)));
	tagDataHead * pDataHead = (tagDataHead *)(m_pDataQueueBuffer + m_dwSearchPos);

	//获取大小
	WORD wDataSize = pDataHead->wDataSize;
	WORD wPacketSize = sizeof(DataHead) + pDataHead->wDataSize;

	//拷贝数据
	CopyMemory(&DataHead, pDataHead, sizeof(DataHead));

	//拷贝数据
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

	//删除数据
	if (bRemoveData == true)
	{
		//效验状态
		ASSERT(m_dwDataSize >= wPacketSize);
		ASSERT(m_dwBufferSize >= (m_dwSearchPos + wPacketSize));

		//设置变量
		m_dwDataPacketCount--;
		m_dwDataSize -= wPacketSize;
		m_dwSearchPos += wPacketSize;
	}

	return true;
}

//删除数据
VOID CWHDataQueue::RemoveData(bool bFreeMemroy)
{
	//设置变量
	m_dwSearchPos = 0L;
	m_dwInsertPos = 0L;
	m_dwTerminalPos = 0L;

	//设置变量
	m_dwDataSize = 0L;
	m_dwDataPacketCount = 0L;

	//删除内存
	if (bFreeMemroy == true)
	{
		m_dwBufferSize = 0;
		SafeDeleteArray(m_pDataQueueBuffer);
	}

	return;
}

//备份队列
VOID CWHDataQueue::BackupQueue(tagQueueSnapshoot& QueueSnapshoot)
{
	//查询变量
	QueueSnapshoot.dwSearchPos=m_dwSearchPos;
	QueueSnapshoot.dwInsertPos=m_dwInsertPos;
	QueueSnapshoot.dwTerminalPos=m_dwTerminalPos;

	//数据变量
	QueueSnapshoot.dwDataSize=m_dwDataSize;
	QueueSnapshoot.dwDataPacketCount=m_dwDataPacketCount;

	//缓冲变量
	QueueSnapshoot.dwBufferSize=m_dwBufferSize;
}

//恢复队列
bool CWHDataQueue::ResumeQueue(const tagQueueSnapshoot& QueueSnapshoot)
{
	//校验大小
	if (QueueSnapshoot.dwBufferSize != m_dwBufferSize)
	{
		return false;
	}

	//查询变量
	m_dwSearchPos=QueueSnapshoot.dwSearchPos;
	m_dwInsertPos=QueueSnapshoot.dwInsertPos;
	m_dwTerminalPos=QueueSnapshoot.dwTerminalPos;

	//数据变量
	m_dwDataSize=QueueSnapshoot.dwDataSize;
	m_dwDataPacketCount=QueueSnapshoot.dwDataPacketCount;

	return true;
}

//申请内存
bool CWHDataQueue::MallocBuffer(DWORD dwNeedSize)
{
	try
	{
		//申请内存
		DWORD dwMinBufferRise = __min(m_dwBufferSize / 2L, 1024L);
		DWORD dwRiseBufferSize = __max(dwMinBufferRise, dwNeedSize * 10L);
		LPBYTE pDataQueueBuffer = new BYTE[m_dwBufferSize + dwRiseBufferSize];

		//错误判断
		ASSERT(pDataQueueBuffer != NULL);
		if (pDataQueueBuffer == NULL) return false;

		//拷贝数据
		if (m_pDataQueueBuffer != NULL)
		{
			//效验状态
			ASSERT(m_dwTerminalPos >= m_dwDataSize);
			ASSERT(m_dwTerminalPos >= m_dwSearchPos);

			//拷贝数据
			DWORD dwHeadDataSize = m_dwTerminalPos - m_dwSearchPos;
			if (dwHeadDataSize > 0L) CopyMemory(pDataQueueBuffer, m_pDataQueueBuffer + m_dwSearchPos, dwHeadDataSize);

			if (m_dwDataSize > dwHeadDataSize)
			{
				ASSERT((m_dwInsertPos + dwHeadDataSize) == m_dwDataSize);
				CopyMemory(pDataQueueBuffer + dwHeadDataSize, m_pDataQueueBuffer, m_dwInsertPos);
			}
		}

		//设置缓冲
		SafeDeleteArray(m_pDataQueueBuffer);
		m_pDataQueueBuffer = pDataQueueBuffer;

		//设置变量
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

//调整存储
bool CWHDataQueue::RectifyBuffer(DWORD dwNeedSize)
{
	//效验状态
	ASSERT(m_dwSearchPos <= m_dwTerminalPos);
	ASSERT(m_dwInsertPos <= m_dwTerminalPos);

	//连续模式
	if ((m_dwInsertPos == m_dwTerminalPos) && ((m_dwInsertPos + dwNeedSize) > m_dwBufferSize))
	{
		//申请内存
		if (m_dwSearchPos < dwNeedSize)
		{
			return MallocBuffer(dwNeedSize);
		}

		//重头开始
		if (m_dwSearchPos >= dwNeedSize)
		{
			//设置变量
			m_dwInsertPos = 0L;

			//设置变量
			if (m_dwSearchPos == m_dwTerminalPos)
			{
				m_dwSearchPos = 0L;
				m_dwTerminalPos = 0L;
			}
		}

		return true;
	}

	//头尾模式
	if ((m_dwInsertPos < m_dwTerminalPos) && ((m_dwInsertPos + dwNeedSize) >= m_dwSearchPos))
	{
		return MallocBuffer(dwNeedSize);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
