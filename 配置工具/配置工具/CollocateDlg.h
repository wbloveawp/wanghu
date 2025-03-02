#ifndef COLLOCATE_DLG_HEAD_FILE
#define COLLOCATE_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgTabViewItem1.h"

//////////////////////////////////////////////////////////////////////////////////

//���ô���
class CCollocateDlg : public CDialog
{
	//�������
protected:
	CInsideThread					m_InsideThread;						//�ڲ��߳�

	//�ؼ�����
protected:
	CTabCtrl						m_TabControl;						//ҳ��ؼ�
	CDlgTabViewItem1				m_DlgTabViewItem1;					//ҳ�洰��

	//��������
public:
	//���캯��
	CCollocateDlg();
	//��������
	virtual ~CCollocateDlg();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//���뺯��
	virtual BOOL OnInitDialog();
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//��Ϣӳ��
public:
	//ʱ����Ϣ
	VOID OnTimer(UINT_PTR nIDEvent);
	//���͸ı�
	VOID OnTcnSelchangeTabControl(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif