#ifndef DLG_STATUS_HEAD_FILE
#define DLG_STATUS_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//״̬����
class CDlgStatus : public CDialog
{
	//��������
protected:
	UINT							m_uCommomID;						//�����ʶ
	HWND 							m_hNotifyWnd;						//������

	//�ؼ�����
protected:
	CSkinButton						m_btGiveUp;							//ȡ����ť
	CProgressCtrl					m_ProgressCtrl;						//���ȿؼ�

	//��������
public:
	//���캯��
	CDlgStatus();
	//��������
	virtual ~CDlgStatus();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ������
	virtual VOID OnOK() { return; };
	//ȡ������
	virtual VOID OnCancel() { return; };

	//���ܺ���
public:
	//֪ͨ����
	HWND GetNotifyWnd() { return m_hNotifyWnd; }
	//������Ϣ
	VOID ShowStatusWnd(LPCTSTR pszDescribe, HWND hNotifyWnd, UINT uCommomID);

	//��ť��Ϣ
protected:
	//ȡ����ť
	VOID OnBnClickedGiveUp();

	//��Ϣӳ��
protected:
	//ʱ����Ϣ
	VOID OnTimer(UINT_PTR nIDEvent);
	//�����Ϣ
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//�ؼ���ɫ
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif