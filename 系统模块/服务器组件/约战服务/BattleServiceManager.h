#ifndef BATTLE_SERVICE_MANAGER_HEAD_FILE
#define BATTLE_SERVICE_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "BattleServiceHead.h"
#include "ExtendDataBaseSink.h"

////////////////////////////////////////////////////////////////////////
//���Ͷ���
typedef CWHArray<ITableFrameItem *> CTableFrameArray;	

////////////////////////////////////////////////////////////////////////

//�������������
class BATTLE_SERVICE_CLASS CBattleServiceManager : public IExtendServiceManager
{
	//״̬����
protected:
	bool								m_bIsService;					//�����ʶ
	tagExtendServiceParameter			m_ExtendServiceParameter;		//�������	
	
	//��������
protected:	
	CTableFrameArray					m_TableFrameArray;				//��������

	//��̬��Ա
protected:
	static CBattleServiceManager*		m_pBattleServiceManager;		//�������
	
	//��������
public:
	//���캯��
	CBattleServiceManager(void);
	//��������
	virtual ~CBattleServiceManager(void);

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID  Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID *  QueryInterface(const IID & Guid, DWORD dwQueryVer);
	
	//���ƽӿ�
public:
	//��������
	virtual bool StartService();
	//ֹͣ����
	virtual bool ConcludeService();	
	
	//����ӿ�
public:	
	//��������
	virtual VOID* CreateExtendTableFrame();
	//��������
	virtual VOID* CreateExtendDataBaseSink(REFGUID Guid, DWORD dwQueryVer);
	//��ʼ���ӿ�
	virtual bool  InitServiceManager(tagExtendServiceParameter& ExtendServiceParameter);

	//ϵͳ�¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, VOID * pData, WORD wDataSize,DWORD dwContextID, DWORD dwTokenID);

	//�����¼�
public:
	//�����¼�
	virtual bool OnEventSocketRead(WORD wMainCmdID,WORD wSubCmdID, VOID* pData, WORD wDataSize) ;
	//�����¼�
	virtual bool OnEventNetworkRead(WORD wSubCmdID, VOID* pData, WORD wDataSize, IServerUserItem* pIServerUserItem, DWORD dwSocketID, DWORD dwTokenID) ;

	//�û��ӿ�
public:
	//�û���¼
	virtual bool OnEventUserLogon(IServerUserItem* pIServerUserItem);
	//�û��ǳ�
	virtual bool OnEventUserLogout(IServerUserItem* pIServerUserItem);
	//�û�����
	virtual bool OnEventUserItemScore(IServerUserItem* pIServerUserItem);

	//��̬����
public:
	//���ö���
	static CBattleServiceManager * GetInstance() { return m_pBattleServiceManager; }	
};

//////////////////////////////////////////////////////////////////////////
#endif