#ifndef SYNCHRONISE_ENGINE_HEAD_FILE
#define SYNCHRONISE_ENGINE_HEAD_FILE

#pragma once

#include "GameServiceHead.h"
#include <afxtempl.h>

//////////////////////////////////////////////////////////////////////////
//���ݶ���

//����ṹ
struct tagSyncRequest
{
	DWORD							dwSyncID;							//ͬ��ID
	DWORD							dwRequestCount;						//�������

	//���캯��
	tagSyncRequest()
{
		dwSyncID = 0xffffffff;
		dwRequestCount = 0;
	}
};

//////////////////////////////////////////////////////////////////////////
//�궨��

typedef CWHArray<IDataBaseEngine *> CDataBaseEngineArray;			//�������� 
typedef CWHArray<IDataBaseEngineSink *> CDataBaseEngineSinkArray;	//���ȹ���
typedef CWHArray<tagSyncRequest> CSyncReqeust;					//��������
typedef CMap<DWORD,DWORD,INT_PTR,INT_PTR> CMapSyncID;					//IDӳ��

//////////////////////////////////////////////////////////////////////////

//���ݿ�������
class CDataBaseSyncEngine: public IDataBaseSyncEngine
{
	//��Ԫ����
	friend class CServiceUnits;

	//��������
protected:
	bool							m_bService;							//����״̬
	CMapSyncID 						m_SyncIDMap;						//ͬ��ID

	//�������
protected:
	CDataBaseEngineArray			m_DataBaseEngineArry;				//��������
	CDataBaseEngineSinkArray		m_DataBaseEngineSinkArry;			//���ȹ���
	CCriticalSection				m_AsyncCritical;					//��������
	CSyncReqeust					m_SyncRequestArry;					//����ṹ

	//�������
protected:
	ITimerEngine*					m_pITimerEngine;					//ʱ������

	//�������
protected:
	CBlockDataQueue					m_DataQueue;						//���ݶ���
	
	//��������
public:
	//���캯��
	CDataBaseSyncEngine();
	//��������
	virtual ~CDataBaseSyncEngine();

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

	//���ýӿ�
public:
	//����ʱ������
	virtual VOID SetTimerEngine(ITimerEngine *pITimerEngine) { m_pITimerEngine = pITimerEngine; }
	//����ģ��
	virtual bool InitDataBaseSyncEngine(IDataBaseEngine * pIDataBaseEngine[], IDataBaseEngineSink *pIDataBaseEngineSink[], WORD wSinkCount);	

	//��ѯ�ӿ�
public:
	//��ѯ����
	virtual INT_PTR GetDataBaseEngineSinkIndex(IDataBaseEngineSink *pIDataBaseEngineSink);

	//���ܺ���
public:
	//����ʱ��
	virtual bool SetTimer(INT_PTR nSyncIndex, DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter);
	//ɾ��ʱ��
	virtual bool KillTimer(INT_PTR nSyncIndex, DWORD dwTimerID);

	//�����¼�
public:
	//ʱ���¼�
	virtual bool PostDataBaseTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//�����¼�
	virtual bool PostDataBaseControl(DWORD dwSyncID, WORD wControlID, VOID * pData, WORD wDataSize);
	//�����¼�
	virtual bool PostDataBaseRequest(DWORD dwSyncID, WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);

	//ͬ���¼�
public:
	//�������
	virtual bool OnPostRequestComplete(INT_PTR nSyncIndex);

	//��������
protected:
	//�����첽����
	INT_PTR SearchValidEngine(DWORD dwSyncID);
	//�����ж�
	bool AmortizeEstimate();
	//��������
	bool AmortizeSyncData(WORD wIdentifier, DWORD dwSyncID, VOID * pData, WORD wDataSize);
	//ִ�л���
	VOID PerformAmortisation();
};

//////////////////////////////////////////////////////////////////////////
//������

DECLARE_MODULE_HELPER(DataBaseSyncEngine,GAME_SERVICE_DLL_NAME,"CreateDataBaseSyncEngine")

//////////////////////////////////////////////////////////////////////////

#endif
