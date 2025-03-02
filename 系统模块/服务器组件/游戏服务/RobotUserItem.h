#ifndef ROBOT_USER_TIEM_HEAD_FILE
#define ROBOT_USER_TIEM_HEAD_FILE

#pragma once

#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//ʱ������
struct tagTimerItem
{
	UINT							nTimerID;							//ʱ���ʶ
	UINT							nTimeLeave;							//ʣ��ʱ��
};

//��˵��
typedef CWHArray<tagTimerItem *> CTimerItemArray;					//ʱ������

//////////////////////////////////////////////////////////////////////////

//��������Ϣ
class CRobotUserItem : public IRobotUserItem
{
	//��Ԫ����
	friend class CRobotUserManager;

	//״̬����
protected:
	bool							m_bWaitLeave;						//�ȴ��뿪
	bool							m_bStartClient;						//��Ϸ��־
	BYTE							m_cbGameStatus;						//��Ϸ״̬
	tagUserStatus					m_CurrentUserStatus;				//�û�״̬
	WORD							m_wRobotAction;						//��������
	DWORD							m_dwPlayInnings;					//��Ϸ����

	//���Ա���
protected:
	UINT							m_nCurrTimerID;						//ʱ�ӱ���

	//�󶨱���
protected:
	DWORD							m_dwClientVersion;					//��Ϸ�汾
	ITableFrameItem*				m_pITableFrameItem;					//���ӽӿ�
	IServerUserItem *				m_pIServerUserItem;					//�û��ӿ�

	//״̬��Ϣ
protected:
	tagRobotService					m_RobotService;						//������Ϣ
	tagRobotItemConfig				m_RobotItemConfig;					//������Ϣ

	//��������
protected:
	WORD							m_wRoundID;							//ѭ������
	WORD							m_wRobotIndex;						//��������

	//ʱ�����
protected:
	DWORD							m_dwMinSitInterval;					//���¼��
	DWORD							m_dwMaxSitInterval;					//���¼��
	DWORD							m_dwStandupTickCount;				//����ʱ��
	DWORD							m_dwReposeTickCount;				//��Ϣʱ��

	//ʱ�����
protected:
	CTimerItemArray					m_TimerItemActive;					//�����
	static CTimerItemArray			m_TimerItemStorage;					//�������

	//�ӿڱ���
protected:
	IServerUserManager *			m_pIServerUserManager;				//�û�����
	IRobotUserManager *				m_pIRobotUserManager;				//����ӿ�
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
	virtual VOID Release() { delete this; }
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

	//��Ϣ�ӿ�
public:
	//��ȡ�Լ�
	virtual IServerUserItem* GetMeUserItem() { return m_pIServerUserItem; }

	//����ӿ�
public:
	//���ͺ���
	virtual bool SendSocketData(WORD wSubCmdID);
	//���ͺ���
	virtual bool SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize);

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
	//���ö���
	virtual void SetRobotAction(WORD wRobotAction, bool bEnable);
	//���ð汾
	virtual VOID SetAdroidVersion( DWORD dwVersion ) { m_dwClientVersion = dwVersion; }	

	//���ܺ���
public:
	//���ܺ���
	bool JudgeRobotAction(WORD wAction);
	//���ӽӿ�
	VOID SetTableFrameItem(ITableFrameItem* pITableFrameItem);

	//ͬ����Ϣ
public:
	//������Ŀ
	virtual WORD GetTableRealUserCount();
	//ͬ���û�
	virtual IServerUserItem* GetTableUserItem(WORD wChairID);	

	//������Ϣ
public:
	//��ȡ״̬
	tagRobotService * GetRobotService() { return &m_RobotService; }
	//��ȡ����
	tagRobotParameter * GetRobotParameter() { return m_RobotItemConfig.pRobotParameter; }

	//�¼�֪ͨ
public:
	//ʱ���¼�
	bool OnTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter);
	//������Ϣ
	bool OnSocketRead(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//�����¼�
protected:
	//��¼ʧ��
	bool OnSocketSubLogonFailure(VOID * pData, WORD wDataSize);	
	//��¼���
	bool OnSocketSubLogonFinish(VOID * pData, WORD wDataSize);
	//�û�״̬
	bool OnSocketSubUserStatus(VOID * pData, WORD wDataSize);
	//����ʧ��
	bool OnSocketSubRequestFailure(VOID * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	bool OnSocketGameMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//ϵͳ��Ϣ
	bool OnSocketSubSystemMessage(VOID * pData, WORD wDataSize);

	//���ƺ���
protected:
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