#include "stdafx.h"
#include "Snowflake.h"

////////////////////////////////////////////////////////////////////////////////////
//���캯��
CSnowflakeIdWorker::CSnowflakeIdWorker() : 
    m_wServiceID(0), 
    m_bSequence(0), 
    m_LastTimestamp(0)
{ 
}

//��������
CSnowflakeIdWorker::~CSnowflakeIdWorker()
{
}

 //�������
void CSnowflakeIdWorker::SetServiceID(uint16_t ServiceID)
{
    m_wServiceID = ServiceID;
}

//��ȡ��ʶ
uint64_t CSnowflakeIdWorker::GetID()
{
    return GetNextID();    
}

//�����һ��ID 
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


//�����Ժ���Ϊ��λ�ĵ�ǰʱ��
uint64_t CSnowflakeIdWorker::GetTimeGen()
{
    auto now = system_clock::now().time_since_epoch();
    return duration_cast<milliseconds>(now).count();
}

//��������һ�����룬ֱ������µ�ʱ���    
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
