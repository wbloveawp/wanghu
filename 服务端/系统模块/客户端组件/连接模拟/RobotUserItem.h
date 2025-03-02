#ifndef ROBOT_USER_TIEM_HEAD_FILE
#define ROBOT_USER_TIEM_HEAD_FILE

#pragma once

#include "VirtualLinkManager.h"
#include "SimulateLinkHead.h"

//////////////////////////////////////////////////////////////////////////

//ʱ������
struct tagTimerItem
{
	UINT							nTimerID;							//ʱ���ʶ
	UINT							nTimeLeave;							//ʣ��ʱ��
};

//��˵��
typedef CWHArray<tagTimerItem *> CTimerItemArray;						//ʱ������

//////////////////////////////////////////////////////////////////////////

//��������Ϣ
class CRobotUserItem : public IRobotUserItem
{
	//��Ԫ����
	friend class CRobotUserManager;

	//״̬����
protected:
	bool							m_bStartClient;						//��Ϸ��־
	BYTE							m_cbGameStatus;						//��Ϸ״̬	
	
	//�󶨱���
protected:
	DWORD							m_dwClientVersion;					//��Ϸ�汾
	tagGlobalUserData *				m_pGlobalUserData;					//�û��ṹ

	//ʱ�����
protected:
	CTimerItemArray					m_TimerItemActive;					//�����
	static CTimerItemArray			m_TimerItemStorage;					//�������

	//�ӿڱ���
protected:
	CVirtualLinkItem*				m_pVirtualLinkItem;					//��������
	IRobotUserItemSink *			m_pIRobotUserItemSink;				//�ص��ӿ�

	//��������
public:
	//���캯��
	CRobotUserItem();
	//��������
	virtual ~CRobotUserItem();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() {}
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//״̬�ӿ�
public:
	//��ȡ״̬
	virtual BYTE GetGameStatus() { return m_cbGameStatus; }
	//����״̬
	virtual VOID SetGameStatus(BYTE cbGameStatus) { m_cbGameStatus=cbGameStatus; }

	//�û���Ϣ
public:
	//��ȡ I D
	virtual DWORD GetUserID();
	//���Ӻ���
	virtual WORD GetTableID();
	//���Ӻ���
	virtual WORD GetChairID();
	//�û�״̬
	virtual BYTE GetUserStatus();
	//�û�����
	virtual LPCTSTR GetAccounts();

	//������Ϣ
public:
	//�û�����
	virtual SCORE GetUserScore();

	//����ӿ�
public:
	//���ͺ���
	virtual bool SendSocketData(WORD wSubCmdID);
	//���ͺ���
	virtual bool SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//��Ϣ�ӿ�
public:
	//��ȡ�Լ�
	virtual IServerUserItem* GetMeUserItem() { return NULL; }

	//ͬ����Ϣ
public:
	//������Ŀ
	virtual WORD GetTableRealUserCount() { return 0; }
	//ͬ���û�
	virtual IServerUserItem* GetTableUserItem(WORD wChairID) { return NULL; }

	//���ܽӿ�
public:
	//ɾ��ʱ��
	virtual bool KillGameTimer(UINT nTimerID);
	//����ʱ��
	virtual bool SetGameTimer(UINT nTimerID, UINT nElapse);
	//����׼��
	virtual bool SendUserReady(VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendChatMessage(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor);

	//���ú���
public:
	//���ð汾
	virtual VOID SetAdroidVersion( DWORD dwVersion ) { m_dwClientVersion = dwVersion; }
	//���ö���
	virtual void SetRobotAction(WORD wTestAction, bool bEnable) {}

	//���ú���
public:
	//�û�����
	VOID SetGlobalUserData(tagGlobalUserData* pGlobalUserData);
	//��������
	VOID SetVirtualLinkItem(CVirtualLinkItem* pVirtualLinkItem);

	//��Ϣ����
public:
	//������־
	bool IsStartClient() { return m_bStartClient; }

	//�¼�֪ͨ
public:
	//ʱ���¼�
	bool OnTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter);

	//�����¼�
public:
	//��Ϸ��Ϣ
	bool OnSocketGameMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//���ƺ���
public:
	//������Ϸ
	VOID StartGameClient();
	//�ر���Ϸ
	VOID CloseGameClient();

	//��������
public:
	//��λ����
	VOID RepositUserItem();
};
//////////////////////////////////////////////////////////////////////////

#endif