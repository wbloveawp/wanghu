#ifndef MODULE_MATCH_HEAD_FILE
#define MODULE_MATCH_HEAD_FILE
#pragma once

//�����ļ�
#include "SimulateLinkHead.h"
#include "VirtualLinkManager.h"

/////////////////////////////////////////////////////////////////////////////////////

//��Ϣ����
#define WM_MATCH_LINK_SHUT			(WM_USER+1200)					//���ӹر�
#define WM_MATCH_LINK_FAILURE		(WM_USER+1201)					//����ʧ��
#define WM_MATCH_LOGON_FINISH		(WM_USER+1202)					//��¼���
#define WM_MATCH_LOGON_FAILURE		(WM_USER+1203)					//��¼ʧ��


/////////////////////////////////////////////////////////////////////////////////////
// 
//��������
struct tagMatchGroupInfo
{
	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������	
	BYTE							cbMatchType;						//��������
	WORD							wStartCount;						//��ʼ����
	WORD							wSignupCount;						//��������

	//��ȯ��Ϣ
	DWORD							dwMatchTicket;						//����ȯ

	//��չ����
	BYTE							cbGroupDetail[512];					//��������
};

/////////////////////////////////////////////////////////////////////////////////////

//����ģ��
class CModuleMatch : public IModuleItem, public CVirtualLinkItem
{
	//���ñ���
protected:
	tagMatchGroupInfo			m_MatchGroupInfo;					//������Ϣ
	tagMatchViewOption			m_MatchViewOption;					//��������	

	//�������
protected:
	tagDataPacket				m_BufferPacket;						//��������

	//�ӿڱ���
protected:
	IModuleItemSink	*			m_pIModuleItemSink;					//�ص��ӿ�		

	//��������
public:
	//��������
	virtual ~CModuleMatch();
	//���캯��
	CModuleMatch(IModuleItemSink* pIModuleItemSink);

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
	virtual DWORD GetTimerStart() { return IDI_MODULE_MATCH_START; }
	//ʱ�ӷ�Χ
	virtual DWORD GetTimerRange() { return IDI_MODULE_MATCH_RANGE; }

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

	//�����¼�
protected:
	//ϵͳ����
	bool OnSocketMainSystem(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//��¼����
	bool OnSocketMainMatchService(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//�����¼�
protected:
	//�������
	bool OnSocketSubEnterFinish(VOID* pData, WORD wDataSize);
	//������
	bool OnSocketSubBuyTickResult(VOID* pData, WORD wDataSize);
	//��ѯ���
	bool OnSocketSubQueryTickResult(VOID* pData, WORD wDataSize);
	//��������
	bool OnSocketSubMatchSessionItem(VOID* pData, WORD wDataSize);
	//�����б�
	bool OnSocketSubMatchSessionList(VOID* pData, WORD wDataSize);
	//��������
	bool OnSocketSubMatchSessionDetail(VOID* pData, WORD wDataSize);
	//����״̬
	bool OnSocketSubMatchSessionStatus(VOID* pData, WORD wDataSize);
	//�����б�
	bool OnSocketSubMatchOptionList(VOID* pData, WORD wDataSize);
	//����״̬
	bool OnSocketSubMatchOptionStatus(VOID* pData, WORD wDataSize);			

	//�����¼�
protected:
	//ϵͳ��Ϣ
	bool OnSocketSubSystemMessage(VOID* pData, WORD wDataSize);
	//����ʧ��
	bool OnSocketSubOperateFailure(VOID* pData, WORD wDataSize);
	
	//���ܺ���
public:
	//����ģ��
	VOID ResetModule();
	//�������
	bool EnterService();
};

/////////////////////////////////////////////////////////////////////////////////////

#endif
