#include "StdAfx.h"
#include "UIControlSink.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CUIControlSink::CUIControlSink()
{
	//���ñ���
	m_hHookWnd=NULL;
	ZeroMemory(m_cbBuffer,sizeof(m_cbBuffer));

	return;
}

//��������
CUIControlSink::~CUIControlSink()
{
}

//���ô���
bool CUIControlSink::SetHookWindow(HWND hHookWnd)
{
	//Ч�����
	ASSERT(IsWindow(hHookWnd));
	if (IsWindow(hHookWnd)==FALSE) return false;

	//���ñ���
	m_hHookWnd=hHookWnd;

	return true;
}

//��ȡ����
VOID * CUIControlSink::GetControlRequest(tagDataHead & DataHead)
{
	//��ȡ����
	CThreadLock ThreadLock(m_CriticalSection);
	if (m_DataQueue.DistillData(DataHead,m_cbBuffer,sizeof(m_cbBuffer),true)==false) return NULL;

	return m_cbBuffer;
}

//Ͷ������
bool CUIControlSink::PostControlRequest(WORD dwIdentifier, VOID * pData, WORD wDataSize)
{
	//״̬�ж�
	ASSERT(IsWindow(m_hHookWnd));
	if (IsWindow(m_hHookWnd)==FALSE) return false;

	//�������
	CThreadLock ThreadLock(m_CriticalSection);
	if (m_DataQueue.InsertData(dwIdentifier,pData,wDataSize)==false) return false;

	//������Ϣ
	::PostMessage(m_hHookWnd,WM_UICONTROL_REQUEST,dwIdentifier,wDataSize);

	return true;
}

//////////////////////////////////////////////////////////////////////////
