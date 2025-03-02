#ifndef MODULE_SERVER_HEAD_FILE
#define MODULE_SERVER_HEAD_FILE
#pragma once

//�����ļ�
#include "RobotUserItem.h"
#include "SimulateLinkHead.h"
#include "VirtualLinkManager.h"

/////////////////////////////////////////////////////////////////////////////////////
//��Ϣ����

#define WM_SERVER_LINK_SHUT			(WM_USER+1100)					//���ӹر�
#define WM_SERVER_LINK_FAILURE		(WM_USER+1101)					//���ӹر�
#define WM_SERVER_LOGON_FINISH		(WM_USER+1102)					//��¼���	
#define WM_SERVER_LOGON_FAILURE		(WM_USER+1103)					//��¼ʧ��
#define WM_SERVER_MATCH_COMPLETED	(WM_USER+1104)					//�������

/////////////////////////////////////////////////////////////////////////////////////
 
//����ģ��
class CModuleServer : public IModuleItem, public CVirtualLinkItem
{
	//���ñ���
protected:
	WORD						m_wKindID;							//��������
	WORD						m_wTableCount;						//������Ŀ
	WORD						m_wChairCount;						//������Ŀ
	WORD						m_wServerType;						//��������
	DWORD						m_dwServerRule;						//�������	

	//�������
protected:
	CRobotUserItem*				m_pRobotUserItem;					//��������
	tagGlobalUserData*			m_pGlobalUserData;					//�û�ָ��

	//�������
protected:
	tagDataPacket				m_BufferPacket;						//��������

	//��������
protected:
	DWORD						m_dwMatchID;						//������ʶ
	SCORE						m_lMatchNo;							//��������
	BYTE						m_cbMatchUStatus;					//����״̬

	//�ӿڱ���
protected:
	IModuleItemSink*			m_pIModuleItemSink;					//�ص��ӿ�

	//��������
public:
	//��������
	virtual ~CModuleServer();
	//���캯��
	CModuleServer(IModuleItemSink* pIModuleItemSink);

	//�ص�����
public:
	//�ر��¼�
	virtual bool OnEventVirtualShut();
	//�����¼�
	virtual bool OnEventVirtualLink(LONG lErrorCode);
	//��ȡ�¼�
	virtual bool OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize);

	//��Ϣ����
public:
	//ʱ�ӱ�ʶ
	virtual DWORD GetTimerStart() { return IDI_MODULE_SERVER_START; }
	//ʱ�ӷ�Χ
	virtual DWORD GetTimerRange() { return IDI_MODULE_SERVER_RANGE; }

	//���ܺ���
public:
	//�ر�ʱ��
	virtual bool KillModuleTimer(UINT uTimerID);
	//����ʱ��
	virtual bool SetModuleTimer(UINT uTimerID, UINT nElapse);

	//ʱ���¼�
public:
	//����ʱ��
	virtual bool OnEventTimer(UINT uTimerID, WPARAM wBindParam);

	//���纯��
public:
	//��¼����
	bool OnSocketMainLogon(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//���ô���
	bool OnSocketMainConfig(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//�û�����
	bool OnSocketMainUser(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//״̬����
	bool OnSocketMainStatus(WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//ϵͳ����
	bool OnSocketMainSystem(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//��������
	bool OnSocketMainMatch(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//���紦��
public:
	//��¼ʧ��
	bool OnSocketSubLogonFailure(VOID* pData, WORD wDataSize);
	//��¼�ɹ�
	bool OnSocketSubLogonSuccess(VOID* pData, WORD wDataSize);
	//��¼���
	bool OnSocketSubLogonFinish(VOID* pData, WORD wDataSize);
	//������ʾ
	bool OnSocketSubUpdateNotify(VOID* pData, WORD wDataSize);
	//���ù���
	bool OnSocketSubConfigRule(VOID* pData, WORD wDataSize);
	//���÷���
	bool OnSocketSubConfigServer(VOID* pData, WORD wDataSize);	
	//�û�����
	bool OnSocketSubUserEnter(VOID* pData, WORD wDataSize);
	//�û�����
	bool OnSocketSubUserScore(VOID* pData, WORD wDataSize);
	//�û�״̬
	bool OnSocketSubUserStatus(VOID* pData, WORD wDataSize);
	//ϵͳ��Ϣ
	bool OnSocketSubSystemMessage(VOID* pData, WORD wDataSize);
	//����ʧ��
	bool OnSocketSubRequestFailure(VOID* pData, WORD wDataSize);
	//����״̬
	bool OnSocketSubMatchStatus(VOID* pData, WORD wDataSize);
	//����״̬
	bool OnSocketSubMatchUStatus(VOID* pData, WORD wDataSize);	
	//�����ɹ�
	bool OnSocketSubSignupSuccess(VOID* pData, WORD wDataSize);

	//���ܺ���
public:
	//����ģ��
	VOID ResetModule();
	//���뷿��
	bool EnterServer(WORD wKindID, WORD wServerID, WORD wServerLevel);
	//���뷿��
	bool EnterMatchServer(WORD wKindID, WORD wServerID, DWORD dwMatchID, SCORE lMatchNo);		

	//���ƺ���
protected:
	//��¼����
	bool SendLogonPacket(WORD wKindID, WORD wServerLevel, WORD wServerID, VOID* pExtendData, WORD wDataSize);
};

/////////////////////////////////////////////////////////////////////////////////////

#endif

