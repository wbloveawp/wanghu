#ifndef CENTER_SERVER_DLG_HEAD_FILE
#define CENTER_SERVER_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ServiceUnits.h"

//////////////////////////////////////////////////////////////////////////

//���Ի���
class CCenterServerDlg : public CDialog, public IServiceUnitsSink
{
	//�������
protected:
	CRichEditTrace					m_RichEditTrace;					//׷�ٴ���

	//�������
protected:
	CServiceUnits					m_ServiceUnits;						//����Ԫ

	//��������
public:
	//���캯��
	CCenterServerDlg();
	//��������
	virtual ~CCenterServerDlg();

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
	//���Է���
	VOID OnBnClickedDebugService();
	//ֹͣ����
	VOID OnBnClickedConcludeService();

	//��Ϣӳ��
public:
	//�ر�ѯ��
	BOOL OnQueryEndSession();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif