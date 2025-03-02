#pragma once

#include "KernelEngineHead.h"
#include "TimerEngine.h"

//////////////////////////////////////////////////////////////////////////

class CDataBaseEngine;


//�����߳�
class CDataBaseConnection : public CWHThread
{
	friend class CDataBaseEngine;
	//���б���
protected:
	HANDLE										m_hCompletionPort;					//��ɶ˿�
	CWHDataQueue								m_DataQueue;						//�̶߳���
	CCriticalSection							m_CriticalSection;					//��������

	IDataBaseEngineSink							*m_pIDataBaseEngineSink;
	CDataBaseEngine							    *m_pDataBaseEngine;
	CDataBaseConnection							*m_pConnectNext;

	BYTE										m_cbBuffer[MAX_ASYNCHRONISM_DATA];	//���ջ���

	//��������
public:
	//���캯��
	CDataBaseConnection();
	//��������
	virtual ~CDataBaseConnection();

	//���ܺ���
public:
	//���ýӿ�
	bool OnAsynchronismEngineData(WORD wIdentifier, VOID * pData, WORD wDataSize);

	//���غ���
public:
	//���к���
	virtual bool OnEventThreadStrat();
	virtual bool OnEventThreadRun();
	virtual bool OnEventThreadConclude();
};


//typedef CList<CDataBaseEngineThread *> CDataBaseThreadPtrArray;

//���ݿ�������
class CDataBaseEngine : public IDataBaseEngine, ITimerEngineEvent
{
	//�������
protected:
	bool										m_bService;							//�����Ƿ�����
	CWHDataQueue								m_DataQueue;	
	CCriticalSection							m_CriticalSection;					//��������
	CTimerEngine								m_TimerEngine;

	//�ӿڱ���
protected:
	CDataBaseConnection							*m_pConnectionFree;
	CDataBaseConnection							*m_pConnectionActive;
	//CDataBaseThreadPtrArray						m_ThreadListFree;					//�����߳�
	//CDataBaseThreadPtrArray						m_ThreadListActive;					//�����
	//��������
protected:
	BYTE										m_cbBuffer[MAX_ASYNCHRONISM_DATA];		//��ʱ���� �û�Ͷ������

	//��������
public:
	//���캯��
	CDataBaseEngine();
	//��������
	virtual ~CDataBaseEngine();

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

	//���渺��
	virtual bool GetBurthenInfo(tagBurthenInfo & BurthenInfo);

	//���ýӿ�
public:
	//����ģ��
	virtual bool SetDataBaseEngineSink(IUnknownEx * pIUnknownEx);
	//����ģ��
	virtual bool SetDataBaseEngineSink(IUnknownEx * pIUnknownEx[], WORD wSinkCount);
protected:
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//�����¼�
private:
	//Ͷ������
	bool PostDataBaseRequest(WORD wIdentifier, VOID * pData, WORD wDataSize);
	bool PostDataBaseRequest(WORD wIdentifier, tagDataBuffer DataBuffer[], WORD wDataCount);
public:
	//�߳�ִ�����֮���һЩ�Ż�
	bool RectifyThread(CDataBaseConnection * pCurConnection , bool bSuccess);
public:
	//�����¼�
	virtual bool PostDataBaseControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//�����¼�
	virtual bool PostDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool DeferDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

};

//////////////////////////////////////////////////////////////////////////
