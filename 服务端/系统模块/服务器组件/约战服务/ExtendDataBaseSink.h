#ifndef EXTEND_DATABASE_SINK_HEAD
#define EXTEND_DATABASE_SINK_HEAD
#pragma once

#include "DataBasePacket.h"
#include "BattleServiceManager.h"

///////////////////////////////////////////////////////////////////////////////////////////
//���ݿ⹳��
class CExtendDataBaseSink: public IExtendDataBaseSink
{
	friend class CBattleServiceManager;

	//��Ϸ���ݿ�
protected:
	CDataBaseAide*					 m_pGameDBAide;					//��Ϸ���ݿ�
	IDataBase*						 m_pIGameDBModule;				//��Ϸ���ݿ�

	//�˺����ݿ�
protected:
	CDataBaseAide*					 m_pAccountsDBAide;				//�˺����ݿ�
	IDataBase*						 m_pIAccountsDBModule;			//�˺����ݿ�

	//ƽ̨���ݿ�
protected:
	CDataBaseAide*					m_pPlatformDBAide;				//��Ϸ���ݿ�
	IDataBase*						m_pIPlatformDBModule;			//��Ϸ���ݿ�

	//���ñ���
protected:
	tagGameServiceAttrib*			m_pGameServiceAttrib;			//��������
	tagGameServiceOption*			m_pGameServiceOption;			//���ò���

	//�������
protected:
	ITimerEngine*					m_pITimerEngine;				//��ʱ����
	IDataBaseEngine*				m_pIDataBaseEngine;				//��������
	IDataBaseEngineEvent*			m_pIDataBaseEngineEvent;		//�����¼�

	//��������
public:
	//���캯��
	CExtendDataBaseSink();
	//��������
	virtual ~CExtendDataBaseSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID* QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ýӿ�
public:
	//��ʼ��
	virtual bool InitExtendDataBaseSink(tagExtendDBSinkParameter& ExtendDBSinkParameter);

	//�ں��¼�
public:
	//ʱ���¼�
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//�����¼�
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID* pData, WORD wDataSize);
	//�����¼�
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//Լս����
private:
	//Լս����
	bool OnRequestBattleLock(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//Լս����
	bool OnRequestBattleUnlock(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//ս����¼
	bool OnRequestBattleRecord(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//Լս����
	bool OnRequestBattleReward(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�۳�����
	bool OnRequestDeductBattleCost(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);	
	//�˻�����
	bool OnRequestReturnBattleCost(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�۳�����
	bool OnRequestDeductBattleCostAA(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);	
};


///////////////////////////////////////////////////////////////////////////////////////////

#endif