#include "StdAfx.h"
#include "DispatchManager.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
CDispatchManager * CDispatchManager::m_pDispatchManager=NULL;			//����ָ��

//////////////////////////////////////////////////////////////////////////

//���캯��
CDispatchManager::CDispatchManager()
{
	//���ö���
	ASSERT(m_pDispatchManager==NULL);
	if (m_pDispatchManager==NULL) m_pDispatchManager=this;

	return;
}

//��������
CDispatchManager::~CDispatchManager()
{
	//�ͷŶ���
	ASSERT(m_pDispatchManager==this);
	if (m_pDispatchManager==this) m_pDispatchManager=NULL;

	return;
}

//ע�ᴰ��
bool CDispatchManager::RegisterEventWnd(HWND hEventWnd)
{
	//Ч�����
	ASSERT(IsWindow(hEventWnd));
	if (IsWindow(hEventWnd)==FALSE) return false;

	//�����ж�
	for (INT_PTR i=0;i<m_EventWndArray.GetCount();i++)
	{
		if (m_EventWndArray[i]==hEventWnd)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//��������
	m_EventWndArray.Add(hEventWnd);

	return true;
}

//ע������
bool CDispatchManager::UnRegisterEventWnd(HWND hEventWnd)
{
	//Ч�����
	ASSERT(IsWindow(hEventWnd));
	if (IsWindow(hEventWnd)==FALSE) return false;

	//ɾ���ж�
	for (INT_PTR i=0;i<m_EventWndArray.GetCount();i++)
	{
		if (m_EventWndArray[i]==hEventWnd)
		{
			m_EventWndArray.RemoveAt(i);
			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//�����¼�
VOID CDispatchManager::SendEventInform(WORD wEventID, LPARAM lParam)
{
	//�����¼�
	for (INT_PTR i=0;i<m_EventWndArray.GetCount();i++)
	{
		HWND hWnd=m_EventWndArray[i];
		if (IsWindow(hWnd)) SendMessage(hWnd,WM_EVENT_INFORM,wEventID,lParam);
	}

	return;
}

//�����¼�
VOID CDispatchManager::PostEventInform(WORD wEventID, LPARAM lParam)
{
	//�����¼�
	for (INT_PTR i=0;i<m_EventWndArray.GetCount();i++)
	{
		HWND hWnd=m_EventWndArray[i];
		if (IsWindow(hWnd)) PostMessage(hWnd,WM_EVENT_INFORM,wEventID,lParam);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
