#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "InitParameter.h"
#include "DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////

//���ݿ⹳��
class CDataBaseEngineSink : public IDataBaseEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//�������
protected:
	CDataBaseAide					m_PlatformDBAide;					//ƽ̨���ݿ�	
	CDataBaseHelper					m_PlatformDBModule;					//ƽ̨���ݿ�

	//�������
protected:
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

	//ϵͳ����
protected:
	//��������
	bool OnRequestLoadLoggerOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

//	//Լս�淨
//protected:
//	//����淨
//	bool OnRequestAppendConfig(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
//	//�޸��淨
//	bool OnRequestModifyConfig(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
//	//ɾ���淨
//	bool OnRequestDeleteConfig(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	
	//��������
protected:		
	//����쳣
	void TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException);
};

//////////////////////////////////////////////////////////////////////////

#endif