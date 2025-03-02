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
    //变量定义
private:
    mutex                       m_Mutex;                                //互斥对象     
    uint16_t                    m_wServiceID;                           //服务标识
    uint32_t                    m_bSequence;                            //序列标识
    uint64_t                    m_LastTimestamp;                        //时间戳
        
    //函数定义
public:
    //构造函数
    CSnowflakeIdWorker();
    //拷贝构造
    CSnowflakeIdWorker(const CSnowflakeIdWorker&) = delete;
    //拷贝构造
    CSnowflakeIdWorker& operator=(const CSnowflakeIdWorker&) = delete;

    //函数定义
public:
    //析构函数
    virtual ~CSnowflakeIdWorker();

    //服务函数
public:
    //获取标识
    uint64_t GetID();    
    //服务标识
    void SetServiceID(uint16_t ServiceID);

    //内部函数
protected:    
    //获得下一个ID 
    uint64_t GetNextID();
    //当前时间   
    uint64_t GetTimeGen();    
    //阻塞到下一个毫秒
    uint64_t TilNextMillis(uint64_t lastTimestamp);

     //常量定义
private:
    //开始时间截 (2018-01-01 00:00:00.000)
    const uint64_t initEpoch = 1514736000000;

    //服务ID所占的位数     
    const uint16_t ServiceIDBits = 8;

    //序列所占的位数
    const uint16_t SequenceBits = 12;

    //服务ID向左移
    const uint16_t ServiceIDShift = SequenceBits;

    //时间戳向左移
    const uint16_t TimestampLeftShift = ServiceIDShift + ServiceIDBits;

    //支持的服务id
    const uint16_t MaxServiceID = -1 ^ (-1 << ServiceIDBits);   

    //生成序列的掩码
    const uint16_t SequenceMask = -1 ^ (-1 << SequenceBits);  

};

////////////////////////////////////////////////////////////////////////////////////

#define g_IdWorker   (CSnowflakeIdWorker::getInstance())

////////////////////////////////////////////////////////////////////////////////////

#endif

