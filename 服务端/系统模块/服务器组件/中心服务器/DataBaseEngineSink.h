#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "redis++.h"
#include "DataBasePacket.h"

using namespace redis;

//////////////////////////////////////////////////////////////////////////

//���ݿ⹳��
class CDataBaseEngineSink : public IDataBaseEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//�������
protected:
	CWininetHttp					m_WininetHttp;						//��ҳ����

	//�������
protected:
	CDataBaseAide					m_AccountsDBAide;					//�û����ݿ�
	CDataBaseHelper					m_AccountsDBModule;					//�û����ݿ�

	//�������
protected:
	CDataBaseAide					m_PlatformDBAide;					//ƽ̨���ݿ�
	CDataBaseHelper					m_PlatformDBModule;					//ƽ̨���ݿ�

	//�������
protected:
	Redis *							m_pRedisClient;						//�������
	CInitParameter *				m_pInitParameter;					//���ò���
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//�����¼�

	//��������
public:
	//���캯��
	CDataBaseEngineSink();
	//��������
	virtual ~CDataBaseEngineSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//ϵͳ�¼�
public:
	//�����¼�
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnDataBaseEngineStop(IUnknownEx * pIUnknownEx);

	//�ں��¼�
public:
	//ʱ���¼�
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//�����¼�
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//�����¼�
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

	//ϵͳ�¼�
public:	
	//��������
	bool OnRequestLoadConfig(DWORD dwContextID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�������
	bool OnRequestUnlockPlayer(DWORD dwContextID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//������ҳ
	bool OnRequestRequestWebPage(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//������Ϣ
	bool OnRequestOnLineCountInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//��������
	bool OnRequestUpdateGateList(DWORD dwContextID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�ϱ�����
	bool OnRequestUpdateServerList(DWORD dwContextID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//���°�
	bool OnRequestUpdateRankingList(DWORD dwContextID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);	

};

//////////////////////////////////////////////////////////////////////////

#endif