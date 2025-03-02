#ifndef SKIN_COMBOBOX_HEAD_FILE
#define SKIN_COMBOBOX_HEAD_FILE

#pragma once

#include "SkinControlsHead.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////

//��ť����
class CSkinComboBoxAttribute : public CSkinResourceManager
{
	friend class CSkinComboBox;

	//���ò���
public:
	COLORREF						m_crBoradHover;						//�߿���ɫ
	COLORREF						m_crBoradNormal;					//�߿���ɫ
	CBitImage						m_ImageButtonThumb;					//��ťλͼ

	//��������
protected:
	//���캯��
	CSkinComboBoxAttribute();
	//��������
	virtual ~CSkinComboBoxAttribute();

	//���ܺ���
public:
	//������Դ
	bool UpdateSkinResource(ISkinResource * pISkinResource);
};

//////////////////////////////////////////////////////////////////////////

//�༭�ؼ�
class CSkinComboEdit : public CEdit
{
	//��Ԫ����
	friend class CSkinComboBox;

	//״̬����
protected:
	bool							m_bHovering;						//������־

	//��������
public:
	//���캯��
	CSkinComboEdit();
	//��������
	virtual ~CSkinComboEdit();

	//��Ϣӳ��
protected:
	//������Ϣ
	VOID OnNcDestroy();
	//����ƶ�
	VOID OnMouseMove(UINT nFlags, CPoint Point);
	//����뿪
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//ѡ��ؼ�
class SKIN_CONTROL_CLASS CSkinComboBox : public CComboBox
{
	//״̬����
protected:
	bool							m_bHovering;						//������־

	//�ؼ�����
protected:
	CSkinComboEdit					m_SkinComboEdit;					//�༭�ؼ�

	//��ť����
public:
	static CSkinComboBoxAttribute	m_SkinAttribute;					//��ť����

	//��������
public:
	//���캯��
	CSkinComboBox();
	//��������
	virtual ~CSkinComboBox();

	//���غ���
protected:
	//�ؼ���
    virtual VOID PreSubclassWindow();

	//��Ϣӳ��
protected:
	//�ػ���Ϣ
    VOID OnPaint();
	//������Ϣ
	VOID OnNcDestroy();
	//������Ϣ
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//����ƶ�
	VOID OnMouseMove(UINT nFlags, CPoint Point);
	//����뿪
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif