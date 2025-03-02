#ifndef ASYNCHRONISM_ENGINE_HEAD_FILE
#define ASYNCHRONISM_ENGINE_HEAD_FILE

#pragma once

#include "KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////

//�����߳�
class CAsynchronismThread : public CServiceThread
{
	//���б���
protected:
	HANDLE							m_hCompletionPort;					//��ɶ˿�
	IAsyncEngineSink *	        	m_pIAsyncEngineSink;		     	//�ص��ӿ�

	//��������
private:
	BYTE							m_cbBuffer[MAX_ASYNC_DATA];	//���ջ���

	//��������
public:
	//���캯��
	CAsynchronismThread();
	//��������
	virtual ~CAsynchronismThread();

	//���ܺ���
public:
	//���ýӿ�
	VOID SetAsyncEngineSink(IAsyncEngineSink * pIAsynchronismEngineSink);
	//���ú���
	HANDLE GetCompletionPort() { return m_hCompletionPort;  }

	//���غ���
public:
	//��ʼ�¼�
	virtual bool OnEventThreadStrat();
	//���к���
	virtual bool OnEventThreadRun();
	//��ֹ�¼�
	virtual bool OnEventThreadConclude();
};

//////////////////////////////////////////////////////////////////////////

//�첽����
class IAsyncEngine : public IServiceModule
{
	//��Ԫ����
	friend class CAsynchronismThread;

	//�������
protected:
	CWHDataQueue						m_DataQueue;						//���ݶ���
	CCriticalSection					m_CriticalSection;					//��������
	CAsynchronismThread					m_AsynchronismThread;				//�̶߳���

	//��������
public:
	//���캯��
	IAsyncEngine();
	//��������
	virtual ~IAsyncEngine();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��������
	virtual bool StartService();
	//ֹͣ����
	virtual bool ConcludeService();

	//�첽�ӿ�
public:
	//���и���
	virtual bool GetBurthenInfo(tagBurthenInfo & BurthenInfo);
	//����ģ��
	virtual bool SetAsynchronismSink(IUnknownEx * pIUnknownEx);
	//�첽����
	virtual bool PostAsynchronismData(WORD wIdentifier, VOID * pData, WORD wDataSize);
	//Ͷ������
	virtual bool PostAsynchronismData(WORD wIdentifier, tagDataBuffer DataBuffer[], WORD wDataCount);
};

//////////////////////////////////////////////////////////////////////////

#endif