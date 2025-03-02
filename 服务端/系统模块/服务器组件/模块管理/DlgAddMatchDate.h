#ifndef DLG_ADD_MATCH_DATE_HEAD_FILE
#define DLG_ADD_MATCH_DATE_HEAD_FILE

#pragma once

#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////

//��ӱ���������
class MODULE_MANAGER_CLASS CDlgAddMatchDate : public CDialog
{
	//�������
public:
	CTime					m_TimeBegin;				//��ʼʱ��
	CTime					m_TimeEnd;					//����ʱ��

	//�ؼ�����
protected:
	CDateTimeCtrl			m_CtrlDateBegin;			//��ʼ����
	CDateTimeCtrl			m_CtrlTimeBegin;			//��ʼʱ��
	CDateTimeCtrl			m_CtrlDateEnd;				//��������
	CDateTimeCtrl			m_CtrlTimeEnd;				//����ʱ��

	//��������
public:
	//���캯��
	CDlgAddMatchDate();   
	//��������
	virtual ~CDlgAddMatchDate();

	//���غ���
protected:
	//���ݽ���
	virtual void DoDataExchange(CDataExchange* pDX); 
	//��ʼ��
	virtual BOOL OnInitDialog();
	//ȷ������
	VOID OnOK();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif