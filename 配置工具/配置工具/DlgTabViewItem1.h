#ifndef DLG_TABVIEW_ITEM2_HEAD_FILE
#define DLG_TABVIEW_ITEM2_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//ҳ�洰��
class CDlgTabViewItem1 : public CDialog
{
	//�������
protected:
	CRichEditTrace					m_TraceServiceControl;				//׷�ٴ���

	//��֤����
protected:
	CValidateDataHelper				m_ValidateDataModule;				//��֤����
	CValidateModuleHelper			m_ValidateServiceModule;			//��֤����

	//��������
public:
	//���캯��
	CDlgTabViewItem1();
	//��������
	virtual ~CDlgTabViewItem1();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//���ú���
	virtual BOOL OnInitDialog();

	//���ܺ���
protected:
	//����֤��
	VOID SetControlStatus(tagValidateInfo & ValidateInfo);
	//��ȡ��Ϣ
	bool GetClientAccounts(tagClientAccounts & ClientAccounts);

	//��ť��Ϣ
protected:
	//������Ȩ
	VOID OnBnClickedCreateAccredit();
	//ע����Ȩ
	VOID OnBnClickedDeleteAccredit();
	//��Ȩ��֤
	VOID OnBnClickedValidateAccredit();
	//����֤��
	VOID OnBnClickedReLoadValidateInfo();
	//����ϵͳ
	VOID OnBnClickedEnterProductManager();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif