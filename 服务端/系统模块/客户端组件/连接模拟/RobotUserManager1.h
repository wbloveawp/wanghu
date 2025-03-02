#ifndef ROBOT_USER_MANAGER_HEAD_FILE
#define ROBOT_USER_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "RobotUserItem.h"
#include "SimulateLinkHead.h"

///////////////////////////////////////////////////////////////////////////////////////

//��������
typedef CWHArray<CRobotUserItem *> CRobotUserItemArray;
typedef CWHStockData<DWORD,CRobotUserItem> CStockRobotUserItem;

//////////////////////////////////////////////////////////////////////////////////

//��������
class CRobotUserManager
{
	//��������
protected:	
	bool							m_bServiceContinue;					//��������
	DWORD							m_dwMinSitInterval;					//��Ϣʱ��
	DWORD							m_dwMaxSitInterval;					//��Ϣʱ��	

	//ϵͳ����
public:
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������

	//�û�����
protected:
	CRobotUserItemMap				m_ROBOTUserItemMap;				//�û�����
	CRobotParameterArray			m_ROBOTParameterArray;			//��������
	CRobotUserItemArray			m_ROBOTUserItemArray;				//�洢����
	CRobotUserItemArray			m_ROBOTUserItemBuffer;			//���ж���

	//��������
public:
	//���캯��
	CRobotUserManager();
	//��������
	virtual ~CRobotUserManager();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ƽӿ�
public:
	//��������
	virtual bool StartService();
	//ֹͣ����
	virtual bool ConcludeService();

	//���ýӿ�
public:	
	//�Ƴ�����
	virtual bool RemoveRobotParameter(DWORD dwBatchID);
	//���ò���
	virtual bool AddRobotParameter(tagRobotParameter RobotParameter[], WORD wParameterCount);
	//�������
	virtual bool InsertRobotInfo(tagRobotAccountsInfo RobotAccountsInfo[],WORD wRobotCount,DWORD dwBatchID);

	//����ӿ�
public:
	//�ӻ�����
	virtual bool PostponeRobotAction(DWORD dwUserID,WORD wActionCode);
	//ɾ������
	virtual bool DeleteRobotUserItem(DWORD dwRobotID,bool bStockRetrieve);
	//���һ���
	virtual IRobotUserItem * SearchRobotUserItem(DWORD dwUserID, DWORD dwContextID);
	//��������
	virtual IRobotUserItem * CreateRobotUserItem(tagRobotItemConfig & RobotItemConfig);

	//ϵͳ�¼�
public:
	//�����¼�
	virtual bool OnEventTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter);

	//״̬�ӿ�
public:
	//������Ŀ
	virtual WORD GetRobotCount() { return (WORD)m_ROBOTUserItemMap.GetCount(); }
	//�����Ŀ
	virtual WORD GetRobotStock() { return m_wStockCount; }
	//���ػ���
	virtual bool GetRobotLoadInfo(DWORD & dwBatchID,DWORD & dwLoadCount);
	//�û�״��
	virtual WORD GetRobotUserInfo(tagRobotUserInfo & RobotUserInfo, DWORD dwServiceMode);

	//����ӿ�
public:
	//��������
	virtual bool SendDataToClient(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendDataToClient(DWORD dwRobotID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendDataToServer(DWORD dwRobotID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//�������
protected:
	//�ͷŶ���
	VOID FreeRobotUserItem(DWORD dwUserID);	
	//��ȡ����
	CRobotUserItem * GetRobotUserItem(WORD wIndex);
	//�������
	CRobotUserItem * ActiveRobotUserItem(tagRobotItemConfig & RobotItemConfig);
	//ǿ������
	VOID ForceRobotUserItemStandup(CRobotUserItem * pRobotUserItem);

	//��������
protected:	 	
	//ת��ʱ��
	DWORD GetTodayTickCount();
	//��ȡ����
	tagRobotParameterEx * GetRobotParameter(DWORD dwBatchID);
	//��ȡ����
	WORD GetRobotParameterIndex(tagRobotParameterEx * pRobotParameter);	
	//�ȴ�ʱ��
	DWORD BatchServiceWaitTime(tagRobotParameter * pRobotParameter,DWORD dwTodayTickCount);
	//����ʱ��
	DWORD BatchServiceRemaindTime(tagRobotParameter * pRobotParameter,DWORD dwTodayTickCount);	
};

//////////////////////////////////////////////////////////////////////////////////

#endif