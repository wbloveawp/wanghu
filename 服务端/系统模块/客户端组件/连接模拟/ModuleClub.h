#ifndef MODULE_CLUB_HEAD_FILE
#define MODULE_CLUB_HEAD_FILE
#pragma once

//�����ļ�
#include "SimulateLinkHead.h"
#include "VirtualLinkManager.h"

/////////////////////////////////////////////////////////////////////////////////////

//��Ϣ����
#define WM_CLUB_LINK_SHUT			(WM_USER+1400)					//���ӹر�
#define WM_CLUB_LINK_FAILURE		(WM_USER+1401)					//����ʧ��
#define WM_CLUB_LOGON_FINISH		(WM_USER+1402)					//��¼���
#define WM_CLUB_LOGON_FAILURE		(WM_USER+1403)					//��¼ʧ��

/////////////////////////////////////////////////////////////////////////////////////
//���Ͷ���
typedef CWHArray<tagClubApplyMessage>	CArrayApplyMessage;

/////////////////////////////////////////////////////////////////////////////////////

//����ģ��
class CModuleClub : public IModuleItem, public CVirtualLinkItem
{
	//��������
protected:
	tagClubItem					m_ClubItem;							//��������
	CArrayApplyMessage			m_ArrayApplyMessage;				//������Ϣ

	//�������
protected:
	tagDataPacket				m_BufferPacket;						//��������

	//�ӿڱ���
protected:
	IModuleItemSink	*			m_pIModuleItemSink;					//�ص��ӿ�		

	//��������
public:
	//��������
	virtual ~CModuleClub();
	//���캯��
	CModuleClub(IModuleItemSink* pIModuleItemSink);

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
	virtual DWORD GetTimerStart() { return IDI_MODULE_CLUB_START; }
	//ʱ�ӷ�Χ
	virtual DWORD GetTimerRange() { return IDI_MODULE_CLUB_RANGE; }

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
	//��¼����
	bool OnSocketMainClubService(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//�����¼�
protected:
	//��������
	bool OnSocketSubClubItem(VOID* pData, WORD wDataSize);
	//�����Ա
	bool OnSocketSubClubMember(VOID* pData, WORD wDataSize);
	//������Ϣ
	bool OnSocketSubClubMessage(VOID* pData, WORD wDataSize);
	//����ɹ�
	bool OnSocketSubEnterSuccess(VOID* pData, WORD wDataSize);
	//����ʧ��
	bool OnSocketSubEnterFailure(VOID* pData, WORD wDataSize);			
	
	//���ܺ���
public:
	//����ģ��
	VOID ResetModule();
	//�������
	bool EnterService(DWORD dwClubID);
};

/////////////////////////////////////////////////////////////////////////////////////

#endif
