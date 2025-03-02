#include "stdafx.h"
#include "Snowflake.h"

////////////////////////////////////////////////////////////////////////////////////
//构造函数
CSnowflakeIdWorker::CSnowflakeIdWorker() : 
    m_wServiceID(0), 
    m_bSequence(0), 
    m_LastTimestamp(0)
{ 
}

//析构函数
CSnowflakeIdWorker::~CSnowflakeIdWorker()
{
}

 //机器编号
void CSnowflakeIdWorker::SetServiceID(uint16_t ServiceID)
{
    m_wServiceID = ServiceID;
}

//获取标识
uint64_t CSnowflakeIdWorker::GetID()
{
    return GetNextID();    
}

//获得下一个ID 
uint64_t CSnowflakeIdWorker::GetNextID()
{
    unique_lock<mutex> lock(m_Mutex);
    uint64_t timestamp=GetTimeGen();

    if (timestamp < m_LastTimestamp)
    {
        return 0;
    }

    if (m_LastTimestamp == timestamp) 
    {       
        m_bSequence = (m_bSequence + 1) & SequenceMask;
        if (0 == m_bSequence)
        {
            timestamp = TilNextMillis(m_LastTimestamp);
        }
    }
    else 
    {
        m_bSequence = 0;
    }


    m_LastTimestamp = timestamp;

    return ((timestamp - initEpoch) << TimestampLeftShift) | (m_wServiceID << ServiceIDShift) | m_bSequence;
}


//返回以毫秒为单位的当前时间
uint64_t CSnowflakeIdWorker::GetTimeGen()
{
    auto now = system_clock::now().time_since_epoch();
    return duration_cast<milliseconds>(now).count();
}

//阻塞到下一个毫秒，直到获得新的时间戳    
uint64_t CSnowflakeIdWorker::TilNextMillis(uint64_t lastTimestamp)
{
    uint64_t timestamp = GetTimeGen();
    while (timestamp <= lastTimestamp)
    {
        timestamp = GetTimeGen();
    }
    return timestamp;
}

///////////////////////////////////////////////////////////////////////////////////
