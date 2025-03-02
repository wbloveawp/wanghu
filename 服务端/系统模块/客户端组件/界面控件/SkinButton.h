#ifndef SKIN_BUTTON_HEAD_FILE
#define SKIN_BUTTON_HEAD_FILE

#pragma once

#include "SkinControlsHead.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////

//��ť����
class CSkinButtonAttribute : public CSkinResourceManager
{
	friend class CSkinButton;

	//���ò���
public:
	COLORREF						m_crText;							//������ɫ
	CBitImage						m_ImageBack;						//��ťλͼ

	//��������
protected:
	//���캯��
	CSkinButtonAttribute();
	//��������
	virtual ~CSkinButtonAttribute();

	//���ܺ���
public:
	//������Դ
	bool UpdateSkinResource(ISkinResource * pISkinResource);
};

//////////////////////////////////////////////////////////////////////////

//��ť��
class SKIN_CONTROL_CLASS CSkinButton : public CButton
{
	//״̬����
protected:
	bool							m_bExpand;							//�Ƿ�����
	bool							m_bHovering;						//������־
	bool							m_bTransparent;						//�Ƿ�͸��
		
	//���ñ���
protected:
	COLORREF						m_crText;							//������ɫ
	CBitImage						m_ImageBack;						//��ťλͼ
	
	//�ӿڱ���
protected:
	ISkinMouseEvent *				m_pISkinMouseEvent;					//�¼��ӿ�
	WORD							m_wImageIndex;						//��������

	//��ť����
public:
	static CSkinButtonAttribute		m_SkinAttribute;					//��ť����
	
	//��������
public:
	//���캯��
	CSkinButton();
	//��������
	virtual ~CSkinButton();

	//���غ���
protected:
	//�󶨺���
	virtual VOID PreSubclassWindow();
	//�滭����
	virtual VOID DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//�ص�����
	virtual LRESULT DefWindowProc(UINT uMmessage, WPARAM wParam, LPARAM lParam);

	//���ú���
public:
	//���ýӿ�
	bool SetSkinMouseEvent(IUnknownEx * pIUnknownEx);
	//��ȡ�ӿ�
	VOID * GetSkinMouseEvent(const IID & Guid, DWORD dwQueryVer);

	//���ú���
public:
	//������ɫ
	bool SetTextColor(COLORREF crText);
	//����λͼ
	bool SetButtonImage(LPCTSTR pszFileName, bool bExpandImage, bool bTransparent);
	//����λͼ
	bool SetButtonImage(UINT uBitmapID, HINSTANCE hInstance, bool bExpandImage, bool bTransparent);

	//���Բ�ѯ
public:
	//��ȡ��ɫ
	COLORREF GetTextColor() { return m_crText; }
	//��ȡλͼ
	bool GetButtonImage(tagImageLoadInfo & ImageLoadInfo);

	//��������
private:
	//�����ؼ�
	VOID RectifyControl();

	//��Ϣ����
protected:
	//������Ϣ
	VOID OnNcDestroy();
	//������Ϣ
	BOOL OnEraseBkgnd(CDC * pDC);
	//������Ϣ
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//����ƶ�
	VOID OnMouseMove(UINT nFlags, CPoint Point);
	//����뿪
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	//�����Ϣ
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

//////////////////////////////////////////////////////////////////////////

#endif