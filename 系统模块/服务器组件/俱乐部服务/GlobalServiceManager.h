#ifndef GLOBAL_SERVICE_MANAGER_HEAD_FILE
#define GLOBAL_SERVICE_MANAGER_HEAD_FILE

#pragma once

#include "ClubServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//��˵��
class CGlobalBattleService;

//���Ͷ���
typedef CWHArray<CTCPSocketServiceHelper*> CSocketServiceArray;

//��������
typedef CWHStockData<WORD, CGlobalBattleService> CStockBattleService;

//////////////////////////////////////////////////////////////////////////

//Լս����
class CGlobalBattleService : public CGlobalServiceItem
{
	//��������
public:
	ITCPSocketService *				m_pITCPSocketService;				//����ӿ�	

	//��������
protected:
	//���캯��
	CGlobalBattleService();
	//��������
	virtual ~CGlobalBattleService();

	//��������
public:
	//��������
	virtual VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////

//ȫ����Ϣ
class CGlobalServiceManager
{
	//���ú���
protected:
	IUnknownEx*						m_pIAttemperEngine;					//���Ƚӿ�

	//��������
protected:	
	CStockBattleService				m_StockBattleService;				//Լս����

	//�������
protected:
	CSocketServiceArray				m_SocketServiceActive;				//����洢
	CSocketServiceArray				m_SocketServiceBuffer;				//���񻺳�

	//��������
public:
	//���캯��
	CGlobalServiceManager();
	//��������
	virtual ~CGlobalServiceManager();

	//������
public:
	//��������
	VOID ResetData();
	//������Ŀ
	WORD GetBattleServiceCount() { return (WORD)m_StockBattleService.GetItemCount();}

	//���ú���
public:
	//�ص��ӿ�
	VOID SetIAttemperEngine(IUnknownEx* pIAttemperEngine) { m_pIAttemperEngine = pIAttemperEngine; }

	//Լս����
public:
	//ɾ������
	bool DeleteBattleService(WORD wServiceID);
	//Ѱ�ҷ���
	CGlobalBattleService * SearchBattleService(WORD wServiceID);
	//ö�ٷ���
	CGlobalBattleService * EnumBattleService(CGlobalBattleService* pGlobalBattleService);
	//�������
	CGlobalBattleService * ActiveBattleService(DWORD dwServiceID, tagServiceItem & ServiceItem);	

	//��������
private:
	//�������
	ITCPSocketService* ActiveSocketService(DWORD dwServiceID);
	//�Ƴ�����
	bool RemoveSocketService(ITCPSocketService* pITCPSocketService);
};

//////////////////////////////////////////////////////////////////////////

#endif