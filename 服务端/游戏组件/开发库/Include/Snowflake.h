#ifndef SNOWFLAKE_HEAD_FILE
#define SNOWFLAKE_HEAD_FILE
#pragma once

#include <mutex>
#include <atomic>
#include <chrono>
#include "CommonServiceHead.h"

using namespace std;
using namespace std::chrono;

////////////////////////////////////////////////////////////////////////////////////

class COMMON_SERVICE_CLASS CSnowflakeIdWorker : public CSingleton<CSnowflakeIdWorker>
{
    //��������
private:
    mutex                       m_Mutex;                                //�������     
    uint16_t                    m_wServiceID;                           //�����ʶ
    uint32_t                    m_bSequence;                            //���б�ʶ
    uint64_t                    m_LastTimestamp;                        //ʱ���
        
    //��������
public:
    //���캯��
    CSnowflakeIdWorker();
    //��������
    CSnowflakeIdWorker(const CSnowflakeIdWorker&) = delete;
    //��������
    CSnowflakeIdWorker& operator=(const CSnowflakeIdWorker&) = delete;

    //��������
public:
    //��������
    virtual ~CSnowflakeIdWorker();

    //������
public:
    //��ȡ��ʶ
    uint64_t GetID();    
    //�����ʶ
    void SetServiceID(uint16_t ServiceID);

    //�ڲ�����
protected:    
    //�����һ��ID 
    uint64_t GetNextID();
    //��ǰʱ��   
    uint64_t GetTimeGen();    
    //��������һ������
    uint64_t TilNextMillis(uint64_t lastTimestamp);

     //��������
private:
    //��ʼʱ��� (2018-01-01 00:00:00.000)
    const uint64_t initEpoch = 1514736000000;

    //����ID��ռ��λ��     
    const uint16_t ServiceIDBits = 8;

    //������ռ��λ��
    const uint16_t SequenceBits = 12;

    //����ID������
    const uint16_t ServiceIDShift = SequenceBits;

    //ʱ���������
    const uint16_t TimestampLeftShift = ServiceIDShift + ServiceIDBits;

    //֧�ֵķ���id
    const uint16_t MaxServiceID = -1 ^ (-1 << ServiceIDBits);   

    //�������е�����
    const uint16_t SequenceMask = -1 ^ (-1 << SequenceBits);  

};

////////////////////////////////////////////////////////////////////////////////////

#define g_IdWorker   (CSnowflakeIdWorker::getInstance())

////////////////////////////////////////////////////////////////////////////////////

#endif

