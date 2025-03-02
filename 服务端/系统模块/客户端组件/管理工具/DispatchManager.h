#ifndef DISPATCH_MANAGER_HEAD_FILE
#define DISPATCH_MANAGER_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////
//�¼�����

//��¼�¼�
#define EVENT_LOGON_SUCCESS			1									//��¼�ɹ�
#define EVENT_LOGOUT_NOTIFY			2									//�ǳ�����
#define EVENT_COMMIT_MODIFY			3									//�ύ�޸�
#define EVENT_USER_INFO_UPDATE		4									//���ϸ���


//ϵͳ��Ϣ
#define EVENT_DOWN_LOAD_FINISH		20									//���سɹ�

//////////////////////////////////////////////////////////////////////////

//֪ͨ��Ϣ
#define WM_EVENT_INFORM				(WM_USER+1000)						//�¼�֪ͨ

//////////////////////////////////////////////////////////////////////////

//���鶨��
typedef CWHArray<HWND> CEventWndArray;

//�¼�����
class CDispatchManager
{
	//��������
protected:
	CEventWndArray					m_EventWndArray;					//�¼�����

	//��̬����
protected:
	static CDispatchManager *		m_pDispatchManager;					//����ָ��

	//��������
public:
	//���캯��
	CDispatchManager();
	//��������
	virtual ~CDispatchManager();

	//ע�ᴰ��
public:
	//ע�ᴰ��
	bool RegisterEventWnd(HWND hEventWnd);
	//ע������
	bool UnRegisterEventWnd(HWND hEventWnd);

	//�����¼�
public:
	//�����¼�
	VOID SendEventInform(WORD wEventID, LPARAM lParam);
	//�����¼�
	VOID PostEventInform(WORD wEventID, LPARAM lParam);

	//��̬����
public:
	//��ȡ����
	static CDispatchManager * GetInstance() { return m_pDispatchManager; }
};

//////////////////////////////////////////////////////////////////////////

#endif