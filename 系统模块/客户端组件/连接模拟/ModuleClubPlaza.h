#ifndef MODULE_CLUBPLAZA_HEAD_FILE
#define MODULE_CLUBPLAZA_HEAD_FILE
#pragma once

//�����ļ�
#include "SimulateLinkHead.h"
#include "VirtualLinkManager.h"

/////////////////////////////////////////////////////////////////////////////////////

//��Ϣ����
#define WM_CLUBPLAZA_LINK_SHUT			(WM_USER+1500)					//���ӹر�
#define WM_CLUBPLAZA_LINK_FAILURE		(WM_USER+1501)					//����ʧ��
#define WM_CLUBPLAZA_LOGON_FINISH		(WM_USER+1502)					//��¼���
#define WM_CLUBPLAZA_LOGON_FAILURE		(WM_USER+1503)					//��¼ʧ��

/////////////////////////////////////////////////////////////////////////////////////
//���Ͷ���
typedef CWHArray<tagClubApplyMessage>	CArrayApplyMessage;

/////////////////////////////////////////////////////////////////////////////////////

//����㳡
class CModuleClubPlaza : public IModuleItem, public CVirtualLinkItem
{
	//��־����
protected:
	bool						m_bTestFlag;						//���Ա�־

	//���ñ���
protected:
	tagClubOption				m_ClubOption;						//��������
	tagClubNameCard				m_ClubNameCard;						//������Ƭ	
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
	virtual ~CModuleClubPlaza();
	//���캯��
	CModuleClubPlaza(IModuleItemSink* pIModuleItemSink);

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
	virtual DWORD GetTimerStart() { return IDI_MODULE_CLUBPLAZA_START; }
	//ʱ�ӷ�Χ
	virtual DWORD GetTimerRange() { return IDI_MODULE_CLUBPLAZA_RANGE; }

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
	bool OnSocketMainClubPlazaService(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//�����¼�
protected:
	//��������
	bool OnSocketSubClubOption(VOID* pData, WORD wDataSize);
	//������Ƭ
	bool OnSocketSubClubNameCard(VOID* pData, WORD wDataSize);
	//������Ϣ
	bool OnSocketSubClubMessage(VOID* pData, WORD wDataSize);
	//�������
	bool OnSocketSubLogonFinish(VOID* pData, WORD wDataSize);
	//����ɹ�
	bool OnSocketSubLogonSuccess(VOID* pData, WORD wDataSize);	
	//����ʧ��
	bool OnSocketSubLogonFailure(VOID* pData, WORD wDataSize);	
	//��ѯ���
	bool OnSocketSubSearchResult(VOID* pData, WORD wDataSize);
	//�����ɹ�
	bool OnSocketSubOperateSuccess(VOID* pData, WORD wDataSize);
	//����ʧ��
	bool OnSocketSubOperateFailure(VOID* pData, WORD wDataSize);
	
	//���ܺ���
public:
	//����ģ��
	VOID ResetModule();
	//�������
	bool EnterService();

	//���Ժ���
public:
	//��������
	bool CreateClubItem();
	//��ѯ����
	VOID SearchClubItem(DWORD dwClubID);
	//ɾ������
	VOID DeleteClubItem(DWORD dwClubID);
	//���²���
	VOID UpdateClubItem(DWORD dwClubID);
	//�������
	VOID RequestJoinClub(DWORD dwClubID);
	//ɾ����Ϣ
	VOID DeleteClubMessage(DWORD dwMessageID,BYTE cbMessageType);
	//ɾ����Ա
	VOID DeleteClubMember(DWORD dwClubID, DWORD dwMemberID, BYTE cbDeleteKind);
	//���³�Ա
	VOID UpdateClubMemberType(DWORD dwClubID, DWORD dwMemberID, BYTE cbMemberType);
	//���³�Ա
	VOID UpdateClubMemberStatus(DWORD dwClubID, DWORD dwMemberID, BYTE cbMemberStatus);
	//��Ӧ����
	VOID RespondJoinClub(tagClubApplyMessage* pApplyMessage, BYTE cbApplyStatus);
};

/////////////////////////////////////////////////////////////////////////////////////

#endif
