#ifndef GAME_SERVER_DLG_HEAD_FILE
#define GAME_SERVER_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//���Ի���
class CGameServerDlg : public CDialog, public IServiceUnitsSink
{
	//ģ�����
public:
	bool							m_bOptionSuccess;					//���ñ�־	
	tagModuleInitParameter			m_ModuleInitParameter;				//���ò���

	//�������
protected:
	CRichEditTrace					m_RichEditTrace;					//׷�ٴ���
	CGameServiceUnitsHelper			m_ServiceUnitsModule;				//����Ԫ

	//��������
public:
	//���캯��
	CGameServerDlg();
	//��������
	virtual ~CGameServerDlg();

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
	//�򿪷���
	VOID OnBnClickedOpenServer();
	//��������
	VOID OnBnClickedCreateServer();
	//���÷���
	VOID OnBnClickedOptionServer();
	//�������
	VOID OnBnClickedDebugService();

	//��������
protected:
	//���±���
	VOID UpdateServerTitle();
	//���óɹ�
	VOID OnLoadModuleParameter();

	//��Ϣӳ��
protected:
	//�ر�ѯ��
	BOOL OnQueryEndSession();
	//�����
	LRESULT OnMessageProcessCmdLine(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif