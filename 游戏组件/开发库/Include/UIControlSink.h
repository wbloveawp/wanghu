#ifndef UI_CONTROL_SINK_HEAD_FILE
#define UI_CONTROL_SINK_HEAD_FILE

#pragma once

#include "ServiceFrameHead.h"

//////////////////////////////////////////////////////////////////////////

#define WM_UICONTROL_REQUEST		(WM_USER+100)						//��������

//////////////////////////////////////////////////////////////////////////

//�������
class SERVICE_FRAME_CLASS CUIControlSink
{
	//��������
private:
	HWND 							m_hHookWnd;							//�ҹ�����
	BYTE							m_cbBuffer[MAX_ASYNC_DATA];			//���ݻ���

	//�������
private:
	CWHDataQueue						m_DataQueue;						//���ݶ���
	CCriticalSection				m_CriticalSection;					//ͬ������

	//��������
public:
	//���캯��
	CUIControlSink();
	//��������
	virtual ~CUIControlSink();

	//���ú���
public:
	//���ô���
	bool SetHookWindow(HWND hHookWnd);

	//���ܺ���
public:
	//��ȡ����
	VOID * GetControlRequest(tagDataHead & DataHead);
	//Ͷ������
	bool PostControlRequest(WORD dwIdentifier, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////

#endif