#ifndef SKIN_RICHEDIT_CTRL_HEAD_FILE
#define SKIN_RICHEDIT_CTRL_HEAD_FILE

#pragma once
#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//׷�ٷ���
class CRecordEditCtrl : public CRichEditCtrl
{
	//��������
protected:
	LONG							m_lMaxLineCount;					//�������
	LONG							m_lReserveLineCount;				//��������

	//�߳���Ϣ
protected:
	DWORD							m_dwMainThreadID;					//�̱߳�ʶ
	HANDLE							m_hMainThreadHandle;				//�߳̾��

	//�������
protected:
	CWHDataQueue					m_DataQueue;						//���ݶ���
	CCriticalSection				m_CriticalSection;					//ͬ������

	//��������
protected:

	//��������
public:
	//���캯��
	CRecordEditCtrl();
	//��������
	virtual ~CRecordEditCtrl();

	//���غ���
protected:
	//�󶨺���
	virtual VOID PreSubclassWindow();

	//���ܺ���
public:
	//������Ϣ
	bool LoadMessage(LPCTSTR pszFileName);
	//������Ϣ
	bool SaveMessage(LPCTSTR pszFileName);
	//���ò���
	bool SetParameter(LONG lMaxLineCount, LONG lReserveLineCount);

	//��������
public:
	//���÷���
	VOID InitializeService();	
	//�����ִ�
	VOID InsertString(LPCTSTR pszString, CHARFORMAT2 & CharFormat);

	//�ص�����
private:
	//���ػص�
	static DWORD CALLBACK LoadCallBack(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb);
	//����ص�
	static DWORD CALLBACK SaveCallBack(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb);

	//�˵�����
protected:
	//ɾ����Ϣ
	VOID OnClearAll();
	//ȫ��ѡ��
	VOID OnSelectAll();
	//�����ַ�
	VOID OnCopyString();
	//������Ϣ
	VOID OnSaveString();
	//ɾ���ַ�
	VOID OnDeleteString();

	//��Ϣӳ��
protected:
	//������Ϣ
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//�Ҽ���Ϣ
	VOID OnRButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif