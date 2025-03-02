#ifndef GATE_SERVER_DLG_HEAD_FILE
#define GATE_SERVER_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ServiceUnits.h"

//////////////////////////////////////////////////////////////////////////

//���Ի���
class CGateServerDlg : public CDialog, public IServiceUnitsSink
{
	//�������
protected:
	CRichEditTrace					m_RichEditTrace;					//׷�ٴ���

	//�������
protected:	
	CServiceUnits *					m_pServiceUnits;					//����Ԫ 

	//��������
public:
	//���캯��
	CGateServerDlg();
	//��������
	virtual ~CGateServerDlg();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ����Ϣ
	virtual VOID OnOK();
	//ȡ������
	virtual VOID OnCancel();
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//����ӿ�
public:
	//����״̬
	virtual VOID OnServiceUnitsStatus(WORD ServiceStatus);

	//��ť��Ϣ
protected:
	//��������
	VOID OnBnClickedStartService();
	//ֹͣ����
	VOID OnBnClickedConcludeService();
	//��������
	VOID OnBnClickedLoadOption();
	//���ñ���
	VOID SetWindowStatusText(LPCTSTR pszServiceStatus);

	//��Ϣӳ��
public:
	//�ر�ѯ��
	BOOL OnQueryEndSession();
	//�����
	LRESULT OnMessageProcessCmdLine(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif