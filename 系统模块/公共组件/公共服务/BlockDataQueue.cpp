#include "StdAfx.h"
#include "BlockDataQueue.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CBlockDataQueue::CBlockDataQueue()
{
	//查询变量
	m_dwInsertPos=0L;
	m_dwTerminalPos=0L;
	m_dwDataQueryPos=0L;

	//数据变量
	m_dwDataSize=0L;
	m_dwDataPacketCount=0L;

	//缓冲变量
	m_dwBufferSize=0L;
	m_pDataQueueBuffer=NULL;

	return;
}

//析构函数
CBlockDataQueue::~CBlockDataQueue()
{
	SafeDeleteArray(m_pDataQueueBuffer);
	return;
}

//插入数据
bool CBlockDataQueue::EnQueue(WORD wIdentifier, VOID * const pBuffer, WORD wDataSize)
{
	//初始化数据
	tagDataHead DataHead;
	DataHead.wDataSize=wDataSize;
	DataHead.wIdentifier=wIdentifier;
	DWORD dwCopySize=sizeof(DataHead)+wDataSize;

	try
	{
		//效验缓冲
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
			//申请内存
			DWORD dwNewBufferSize=m_dwBufferSize+__max(m_dwBufferSize/2L,(wDataSize+sizeof(tagDataHead))*10L);
			BYTE * pNewQueueServiceBuffer=new BYTE [dwNewBufferSize];
			if (pNewQueueServiceBuffer==NULL) return false;

			//拷贝数据
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

			//调整变量
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
		//拷贝数据
		CopyMemory(m_pDataQueueBuffer+m_dwInsertPos,&DataHead,sizeof(DataHead));
		if (wDataSize>0) 
		{
			ASSERT(pBuffer!=NULL);
			CopyMemory(m_pDataQueueBuffer+m_dwInsertPos+sizeof(DataHead),pBuffer,wDataSize);
		}

		//调整数据
		m_dwDataPacketCount++;
		m_dwDataSize+=dwCopySize;
		m_dwInsertPos+=dwCopySize;
		m_dwTerminalPos=__max(m_dwTerminalPos,m_dwInsertPos);

		return true;
	}
	catch (...) { }

	return false;
}

//提取队头，并删除
bool CBlockDataQueue::DeQueue(tagDataHead & DataHead, VOID * pBuffer, WORD wBufferSize)
{
	//效验变量
	ASSERT(m_dwDataSize>0L);
	ASSERT(m_dwDataPacketCount>0);
	ASSERT(m_pDataQueueBuffer!=NULL);
	if (m_dwDataSize==0L) return false;
	if (m_dwDataPacketCount==0L) return false;

	//调整参数
	if (m_dwDataQueryPos==m_dwTerminalPos)
	{
		m_dwDataQueryPos=0L;
		m_dwTerminalPos=m_dwInsertPos;
	}

	//获取指针
	ASSERT(m_dwBufferSize>=(m_dwDataQueryPos+sizeof(tagDataHead)));
	tagDataHead * pDataHead=(tagDataHead *)(m_pDataQueueBuffer+m_dwDataQueryPos);

	//设置数据头
	DataHead=*pDataHead;

	//获取大小
	WORD wPacketSize=sizeof(DataHead)+pDataHead->wDataSize;

	//效验参数
	ASSERT(wPacketSize<=m_dwDataSize);
	ASSERT(m_dwBufferSize>=(m_dwDataQueryPos+wPacketSize));

	//复制数据
	if( pBuffer != NULL )
	{
		ASSERT(wBufferSize>=pDataHead->wDataSize);
		if( wBufferSize < pDataHead->wDataSize )
		{
			DataHead.wDataSize = 0;
		}
		else
		{
			//拷贝数据
			CopyMemory(pBuffer,pDataHead+1,DataHead.wDataSize);
		}
	}

	//删除队列
	m_dwDataPacketCount--;
	m_dwDataSize-=wPacketSize;
	m_dwDataQueryPos+=wPacketSize;

	return true;
}

//删除队头
bool CBlockDataQueue::DeQueue()
{
	//效验变量
	ASSERT(m_dwDataSize>0L);
	ASSERT(m_dwDataPacketCount>0);
	ASSERT(m_pDataQueueBuffer!=NULL);
	if (m_dwDataSize==0L) return false;
	if (m_dwDataPacketCount==0L) return false;

	//调整参数
	if (m_dwDataQueryPos==m_dwTerminalPos)
	{
		m_dwDataQueryPos=0L;
		m_dwTerminalPos=m_dwInsertPos;
	}

	//获取指针
	ASSERT(m_dwBufferSize>=(m_dwDataQueryPos+sizeof(tagDataHead)));
	tagDataHead * pDataHead=(tagDataHead *)(m_pDataQueueBuffer+m_dwDataQueryPos);

	//获取大小
	WORD wPacketSize=sizeof(tagDataHead)+pDataHead->wDataSize;

	//效验参数
	ASSERT(wPacketSize<=m_dwDataSize);
	ASSERT(m_dwBufferSize>=(m_dwDataQueryPos+wPacketSize));

	//删除队列
	m_dwDataPacketCount--;
	m_dwDataSize-=wPacketSize;
	m_dwDataQueryPos+=wPacketSize;

	return true;
}

//获取队头
bool CBlockDataQueue::GetHead(tagDataHead & DataHead, VOID * pBuffer, WORD wBufferSize)
{
	//效验变量
	if (m_dwDataSize==0L) return false;
	if (m_dwDataPacketCount==0L) return false;
	ASSERT(m_pDataQueueBuffer!=NULL);

	//调整参数
	if (m_dwDataQueryPos==m_dwTerminalPos)
	{
		m_dwDataQueryPos=0L;
		m_dwTerminalPos=m_dwInsertPos;
	}

	//获取指针
	ASSERT(m_dwBufferSize>=(m_dwDataQueryPos+sizeof(tagDataHead)));
	tagDataHead * pDataHead=(tagDataHead *)(m_pDataQueueBuffer+m_dwDataQueryPos);

	//设置数据头
	DataHead=*pDataHead;

	//获取大小
	WORD wPacketSize=sizeof(DataHead)+pDataHead->wDataSize;

	//效验参数
	ASSERT(wPacketSize<=m_dwDataSize);
	ASSERT(m_dwBufferSize>=(m_dwDataQueryPos+wPacketSize));

	//复制数据
	if( pBuffer != NULL )
	{
		ASSERT(wBufferSize>=pDataHead->wDataSize);
		if( wBufferSize < pDataHead->wDataSize )
		{
			DataHead.wDataSize = 0;
		}
		else
		{
			//拷贝数据
			CopyMemory(pBuffer,pDataHead+1,DataHead.wDataSize);
		}
	}

	return true;
}

//清除队列
VOID CBlockDataQueue::ClearQueue(bool bFreeMemory)
{
	//设置变量
	m_dwDataSize=0L;
	m_dwInsertPos=0L;
	m_dwTerminalPos=0L;
	m_dwDataQueryPos=0L;
	m_dwDataPacketCount=0L;

	//删除内存
	if (bFreeMemory==true) 
	{
		m_dwBufferSize=0;
		SafeDeleteArray(m_pDataQueueBuffer);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
