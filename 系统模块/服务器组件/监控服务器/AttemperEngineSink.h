#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////
//���ݶ���

//��������
enum enServiceKind
{
	ServiceKind_None,				//��Ч����
	ServiceKind_Game,				//��Ϸ����	
	ServiceKind_Gate,				//���ط���
};

//�󶨲���
struct tagBindParameter
{
	//��������
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwActiveTime;						//����ʱ��

	//��������
	WORD							wServiceID;							//�����ʶ
	WORD							wServiceModule;						//����ģ��
};

//////////////////////////////////////////////////////////////////////////
//���Ͷ���
typedef CWHStockData<WORD, tagGameKindEx>  CStockGameKind;				//��Ϸ����

//////////////////////////////////////////////////////////////////////////

//���ȹ���
class CAttemperEngineSink : public IAttemperEngineSink, IStockManagerSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//״̬����
protected:
	bool							m_bMonitorService;					//�����ʶ	
	bool							m_bWillConcludeService;				//Ԥֹͣ״̬
	DWORD							m_StockUpdateID;					//���±�ʶ
	CWordArray						m_StockUpdateArray;					//��������

	//��������
protected:
	tagServiceItem					m_MonitorItem;						//״̬����
	tagServiceOption *				m_pServiceOption;					//Լս����
	CInitParameter *				m_pInitParameter;					//���ò���
	tagBindParameter *				m_pBindParameter;					//��������		

	//�������
protected:
	CStockGameKind					m_StockGameKind;					//��Ϸ����
	CShareStockManager				m_ShareStockManager;				//������	
	
	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	IDataBaseEngine *				m_pIDataBaseEngine;					//��������
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//��������
	ITCPSocketService *				m_pITCPSocketCenter;				//�������	

	//��������
public:
	//���캯��
	CAttemperEngineSink();
	//��������
	virtual ~CAttemperEngineSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//�첽�ӿ�
public:
	//�����¼�
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnAttemperEngineStop(IUnknownEx * pIUnknownEx);

	//�¼��ӿ�
public:
	//�����¼�
	virtual bool OnEventControl(WORD dwIdentifier, VOID * pData, WORD wDataSize);
	//�Զ��¼�
	virtual bool OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize);

	//�ں��¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);

	//�����¼�
public:
	//�����¼�
	virtual bool OnEventTCPSocketLink(DWORD dwServiceID, INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventTCPSocketShut(DWORD dwServiceID, BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize,DWORD * pdwTokenID,WORD wTokenCount);

	//�����¼�
public:
	//Ӧ���¼�
	virtual bool OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID);
	//�ر��¼�
	virtual bool OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID);
	//��ȡ�¼�
	virtual bool OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);

	//Ԥ���ӿ�
public:
	//���Ԥ��
	virtual VOID OnEventStockScoreAlarm(tagStockAlarmItem StockAlarmList[], WORD wItemCount);
	//�ʳ�Ԥ��
	virtual VOID OnEventJackpotScoreAlarm(tagJackpotAlarmItem JackpotAlarmList[], WORD wItemCount);

	//���½ӿ�
public:
	//������
	virtual VOID OnEventStockScoreUpdate(tagStockVariationItem StockVariationList[], WORD wItemCount);
	//״̬����
	virtual VOID OnEventStockStateUpdate(tagStateVariationItem StateVariationList[], WORD wItemCount);
	//�ʳظ���
	virtual VOID OnEventJackpotScoreUpdate(tagJackpotVariationItem JackpotVariationList[], WORD wItemCount);

	//���Ӵ���
protected:
	//ע���¼�
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnTCPSocketMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//ת������
	bool OnTCPSocketMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//�������
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);	

	//�����¼�
protected:
	//��Ϸ����
	bool OnDBResultGameKindItem(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�������
	bool OnDBResultStockListItem(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//��R����
	bool OnDBResultSuperPlayerControl(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�������
	bool OnDBResultLoadConfigFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//��ѯ���
	bool OnDBResultQueryStockItem(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//���¿��
	bool OnDBResultUpdateStockScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//����״̬
	bool OnDBResultUpdateStockState(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//���²ʽ�
	bool OnDBResultUpdateJackpotScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);

	//��������
protected:
	//�ϱ��ʽ�
	VOID ReportGameJackpot();
	//�Ƴ���ʶ
	VOID RemoveStockUpdateID(DWORD dwUpdateID);
	//��������
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);	
};

//////////////////////////////////////////////////////////////////////////

#endif