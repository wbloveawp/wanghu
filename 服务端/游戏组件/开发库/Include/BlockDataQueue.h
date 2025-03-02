#ifndef BLOCK_DATA_QUEUE_HEAD_FILE
#define BLOCK_DATA_QUEUE_HEAD_FILE

#pragma once

#include "CommonServiceHead.h"
#include "DataQueue.h"

//////////////////////////////////////////////////////////////////////////

//数据队列
class COMMON_SERVICE_CLASS CBlockDataQueue
{
	//查询变量
protected:
	DWORD							m_dwInsertPos;						//插入位置
	DWORD							m_dwTerminalPos;					//结束位置
	DWORD							m_dwDataQueryPos;					//查询位置

	//数据变量
protected:
	DWORD							m_dwDataSize;						//数据大小
	DWORD							m_dwDataPacketCount;				//数据包数

	//缓冲变量
protected:
	DWORD							m_dwBufferSize;						//缓冲长度
	LPBYTE							m_pDataQueueBuffer;					//缓冲指针

	//函数定义
public:
	//构造函数
	CBlockDataQueue();
	//析构函数
	virtual ~CBlockDataQueue();

	//功能函数
public:
	//查询数目
	DWORD GetLength() { return m_dwDataPacketCount; }

	//数据管理
public:
	//插入数据
	bool EnQueue(WORD wIdentifier, VOID * const pBuffer, WORD wDataSize);
	//提取队头，并删除
	bool DeQueue(tagDataHead & DataHead, VOID * pBuffer, WORD wBufferSize);
	//删除队头
	bool DeQueue();
	//获取队头
	bool GetHead(tagDataHead & DataHead, VOID * pBuffer, WORD wBufferSize);
	//清除队列
	VOID ClearQueue(bool bFreeMemory);
};

//////////////////////////////////////////////////////////////////////////

#endif