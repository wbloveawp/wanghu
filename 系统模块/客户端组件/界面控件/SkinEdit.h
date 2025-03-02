#ifndef SKIN_EDIT_HEAD_FILE
#define SKIN_EDIT_HEAD_FILE

#pragma once

#include "SkinControlsHead.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////

//�༭������
class CSkinEditAttribute : public CSkinResourceManager
{
	friend class CSkinEdit;
	friend class CSkinEditEx;

	//���ò���
public:
	COLORREF						m_crFocusTX;						//������ɫ
	COLORREF						m_crFocusBK;						//������ɫ
	COLORREF						m_crNoFocusTX;						//������ɫ
	COLORREF						m_crNoFocusBK;						//������ɫ
	COLORREF						m_crDisFocusTX;						//������ɫ
	COLORREF						m_crDisFocusBK;						//������ɫ
	COLORREF						m_crBoradNormal;					//�߿���ɫ

	//GDI ����
public:
	CBrush							m_brFocus;							//��ˢ��Դ
	CBrush							m_brNoFocus;						//��ˢ��Դ
	CBrush							m_brDisbale;						//��ˢ��Դ

	//��������
protected:
	//���캯��
	CSkinEditAttribute();
	//��������
	virtual ~CSkinEditAttribute();

	//���ܺ���
public:
	//������Դ
	bool UpdateSkinResource(ISkinResource * pISkinResource);
};

//////////////////////////////////////////////////////////////////////////

//�༭��
class SKIN_CONTROL_CLASS CSkinEdit : public CEdit
{
	//״̬����
protected:
	bool							m_bFocus;							//�����־

	//��̬����
public:
	static CSkinEditAttribute		m_SkinAttribute;					//��������

	//��������
public:
	//���캯��
	CSkinEdit();
	//��������
	virtual ~CSkinEdit();

	//��ѯ����
public:
	//��ȡ���Խӿ�
	static CSkinEditAttribute * GetSkinAttribute() { return &m_SkinAttribute; }

	//��Ϣ���� 
protected:
	//�ػ���Ϣ
    VOID OnNcPaint();
	//�õ�����
	VOID OnSetFocus(CWnd * pOldWnd);
	//ʧȥ����
	VOID OnKillFocus(CWnd * pNewWnd);
	//�滭�ؼ�
	HBRUSH CtlColor(CDC * pDC, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//�༭��
class SKIN_CONTROL_CLASS CSkinEditEx : public CSkinEdit
{
	//��ɫ����
protected:
	COLORREF						m_crFocusTX;						//������ɫ
	COLORREF						m_crFocusBK;						//������ɫ
	COLORREF						m_crNoFocusTX;						//������ɫ
	COLORREF						m_crNoFocusBK;						//������ɫ
	COLORREF						m_crDisFocusTX;						//������ɫ
	COLORREF						m_crDisFocusBK;						//������ɫ

	//GDI ����
public:
	CBrush							m_brFocus;							//��ˢ��Դ
	CBrush							m_brNoFocus;						//��ˢ��Դ
	CBrush							m_brDisbale;						//��ˢ��Դ

	//��������
public:
	//���캯��
	CSkinEditEx();
	//��������
	virtual ~CSkinEditEx();

	//���ܺ���
public:
	//������ɫ
	VOID SetFocusColor(COLORREF crFocusTX, COLORREF crFocusBK);
	//������ɫ
	VOID SetNoFocusColor(COLORREF crNoFocusTX, COLORREF crNoFocusBK);
	//������ɫ
	VOID SetDisFocusColor(COLORREF crDisFocusTX, COLORREF crDisFocusBK);

	//��Ϣ���� 
protected:
	//�ػ���Ϣ
    VOID OnNcPaint();
	//�滭�ؼ�
	HBRUSH CtlColor(CDC * pDC, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif