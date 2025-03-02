#ifndef BATTLE_SERVER_DLG_HEAD_FILE
#define BATTLE_SERVER_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ServiceUnits.h"

//////////////////////////////////////////////////////////////////////////

//���Ի���
class CStoreServerDlg : public CDialog, public IServiceUnitsSink
{
	//�������
protected:
	CRichEditTrace					m_RichEditTrace;			//׷�ٴ���
	CModuleDBParameter				m_ModuleDBParameter;		//ģ�����

	//�������
protected:
	CServiceUnits*					m_pServiceUnits;			//����Ԫ

	//��������
public:
	//���캯��
	CStoreServerDlg();
	//��������
	virtual ~CStoreServerDlg();

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

	//��������
protected:
	//���ñ���
	VOID SetWindowStatusText(LPCTSTR pszServiceStatus);

	//��ť��Ϣ
protected:
	//��������
	VOID OnBnClickedStartService();
	//ֹͣ����
	VOID OnBnClickedConcludeService();
	//���÷���
	VOID OnBnClickedOptionService();

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