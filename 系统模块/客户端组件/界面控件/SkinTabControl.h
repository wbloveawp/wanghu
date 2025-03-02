#ifndef SKIN_TAB_CONTROL_HEAD_FILE
#define SKIN_TAB_CONTROL_HEAD_FILE

#pragma once

#include "SkinControlsHead.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////

//��˵��
class CSkinTabControlItem;

//���鶨��
typedef CWHArray<CSkinTabControlItem *>  CSkinTabControlItemArray;//��������

//////////////////////////////////////////////////////////////////////////

//��������
class CSkinTabControlAttribute : public CSkinResourceManager
{
	friend class CSkinTabControl;

	//���ò���
public:
	COLORREF						m_crNormalTX;						//������ɫ
	COLORREF						m_crSelectTX;						//������ɫ
	COLORREF						m_crControlBack;					//������ɫ
	COLORREF						m_crControlBorad;					//�߿���ɫ

	//��������
protected:
	//���캯��
	CSkinTabControlAttribute();
	//��������
	virtual ~CSkinTabControlAttribute();

	//���ܺ���
public:
	//������Դ
	bool UpdateSkinResource(ISkinResource * pISkinResource);
};

//////////////////////////////////////////////////////////////////////////

//����ؼ�
class SKIN_CONTROL_CLASS CSkinTabControl : public CTabCtrl
{
	//��ť����
public:
	static CSkinTabControlAttribute	m_SkinAttribute;					//��������

	//��������
public:
	//���캯��
	CSkinTabControl();
	//��������
	virtual ~CSkinTabControl();

	//���غ���
protected:
	//�󶨺���
	virtual VOID PreSubclassWindow();

	//��Ϣ����
protected:
	//�ػ�����	
	VOID OnPaint();
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//ҳ������
class SKIN_CONTROL_CLASS CSkinTabControlItem : public CDialog
{
	//��������
public:
	UINT							m_nIDTemplate;						//ģ���ʶ
	CSize							m_ControlSize;						//�ؼ���С

	//��������
public:
	//���캯��
	CSkinTabControlItem(UINT nIDTemplate);
	//��������
	virtual ~CSkinTabControlItem();

	//���غ���
protected:
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//ȷ����Ϣ
	virtual VOID OnOK() { return; }
	//ȡ����Ϣ
	virtual VOID OnCancel();

	//���غ���
public:
	//��ȡ����
	virtual LPCTSTR GetItemTitle()=NULL;
	//�����ؼ�
	virtual bool CreateControlItem(CWnd * pParentWnd);

	//��Ϣӳ��
public:
	//�滭��Ϣ
	VOID OnPaint();
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);
	//�ؼ���ɫ
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif