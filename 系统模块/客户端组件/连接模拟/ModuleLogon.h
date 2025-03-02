#ifndef MODULE_LOGON_HEAD_FILE
#define MODULE_LOGON_HEAD_FILE
#pragma once

//�����ļ�
#include "SimulateLinkHead.h"
#include "VirtualLinkManager.h"

/////////////////////////////////////////////////////////////////////////////////////

//��Ϣ����
#define WM_LOGON_LINK_SHUT			(WM_USER+1000)					//���ӹر�
#define WM_LOGON_LINK_FAILURE		(WM_USER+1001)					//����ʧ��
#define WM_LOGON_LOGON_FINISH		(WM_USER+1002)					//��¼���
#define WM_LOGON_LOGON_FAILURE		(WM_USER+1003)					//��¼ʧ��

/////////////////////////////////////////////////////////////////////////////////////

//��¼ģ��
class CModuleLogon : public IModuleItem, public CVirtualLinkItem
{
	//��������
protected:
	IModuleItemSink	*			m_pIModuleItemSink;					//�ص��ӿ�

	//�������
protected:
	tagDataPacket				m_BufferPacket;						//��������

	//��������
public:
	//��������
	virtual ~CModuleLogon();
	//���캯��
	CModuleLogon(IModuleItemSink* pIModuleItemSink);

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
	virtual DWORD GetTimerStart() { return IDI_MODULE_LOGON_START; }
	//ʱ�ӷ�Χ
	virtual DWORD GetTimerRange() { return IDI_MODULE_LOGON_RANGE; }

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
	bool OnSocketMainLogon(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//�б���
	bool OnSocketMainServerList(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//�û�����
	bool OnSocketMainUserService(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//�����¼�
protected:
	//��¼ʧ��
	bool OnSocketSubLogonFailure(VOID* pData, WORD wDataSize);
	//��¼�ɹ�
	bool OnSocketSubLogonSuccess(VOID* pData, WORD wDataSize);
	//��¼���
	bool OnSocketSubLogonFinish(VOID* pData, WORD wDataSize);
	//������ʾ
	bool OnSocketSubUpdateNotify(VOID* pData, WORD wDataSize);

	//���ܺ���
public:
	//�ο͵�¼
	bool LogonByVisitor(LPCTSTR pszMachineID);
	//�˺ŵ�¼
	bool LogonByAccounts(LPCTSTR pszAccounts, LPCTSTR pszLogonPass);	

	//���ܺ���
public:
	//����ģ��
	VOID ResetModule();
};

/////////////////////////////////////////////////////////////////////////////////////

#endif
