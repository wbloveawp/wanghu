#ifndef SKIN_HYPER_LINK_HEAD_FILE
#define SKIN_HYPER_LINK_HEAD_FILE

#pragma once

#include "SkinControlsHead.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////

//�б�������
class CSkinHyperLinkAttribute : public CSkinResourceManager
{
	friend class CSkinHyperLink;

	//���ò���
public:
	COLORREF						m_crBackColor;						//������ɫ
	COLORREF						m_crHoverText;						//������ɫ
	COLORREF						m_crNormalText;						//������ɫ
	COLORREF						m_crVisitedText;					//������ɫ

	//��������
protected:
	//���캯��
	CSkinHyperLinkAttribute();
	//��������
	virtual ~CSkinHyperLinkAttribute();

	//���ܺ���
public:
	//������Դ
	bool UpdateSkinResource(ISkinResource * pISkinResource);
};

//////////////////////////////////////////////////////////////////////////

//��������
class SKIN_CONTROL_CLASS CSkinHyperLink : public CStatic
{
	//���ñ���
protected:
    bool							m_bUnderLine;						//�Ƿ���
    bool							m_AutoRectify;						//�Զ�����

	//��ɫ����
protected:
	COLORREF						m_crBackColor;						//������ɫ
	COLORREF						m_crHoverText;						//������ɫ
	COLORREF						m_crNormalText;						//������ɫ
	COLORREF						m_crVisitedText;					//������ɫ

	//״̬����
protected:
	bool							m_bVisited;                     	//�Ƿ����
   	bool							m_bHovering;						//������־
    CString							m_strHyperLink;						//���ӵ�ַ

	//�������
protected:
    CToolTipCtrl					m_ToolTipCtrl;						//��ʾ�ؼ�
	ISkinMouseEvent *				m_pISkinMouseEvent;					//�¼��ӿ�

	//���Ա���
public:
	static CSkinHyperLinkAttribute	m_SkinAttribute;					//����������

	//��������
public:
	//���캯��
	CSkinHyperLink();
	//��������
	virtual ~CSkinHyperLink();

	//���ú���
public:
	//���ýӿ�
	bool SetSkinMouseEvent(IUnknownEx * pIUnknownEx);
	//��ȡ�ӿ�
	VOID * GetSkinMouseEvent(const IID & Guid, DWORD dwQueryVer);

	//��ѯ����
public:
	//��ȡ����
	bool IsVisiteded() { return m_bVisited; }
	//��ȡ�»���
	bool IsUnderline() { return m_bUnderLine; }
	//��ȡ����
	bool IsAutoRectify() { return m_AutoRectify; }
	//��ȡ��ַ
	LPCTSTR GetHyperLinkUrl() { return m_strHyperLink; }

	//���ú���
public:
	//���÷���
    VOID SetVisited(bool bVisited);
	//�����»���
    VOID SetUnderLine(bool bUnderLine);
	//���õ���
    VOID SetAutoRectify(bool bAutoRectify);
	//���� URL
	VOID SetHyperLinkUrl(LPCTSTR pszHyperLink);

	//��ɫ����
public:
	//������ɫ
	VOID SetBackGroundColor(COLORREF crColor);
	//������ɫ
	VOID SetHyperLinkTextColor(COLORREF crHoverText, COLORREF crNormalText, COLORREF crVisitedText);

	//���غ���
protected:
	//�ؼ����໯
	virtual VOID PreSubclassWindow();
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//Ĭ�ϻص�����
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	//��������
protected:
	//��ʼ��
	VOID InitHyperLink();
	//������ʾ
	VOID UpdateToolTip();
	//����λ��
	VOID RectifyControl();

	//��Ϣ����
protected:
	//�滭��Ϣ
	VOID OnPaint();
	//�����ؼ�
	VOID OnStnClicked();
	//������Ϣ
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//����ƶ�
	VOID OnMouseMove(UINT nFlags, CPoint Point);
	//�����Ϣ
    BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);
	//����뿪
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	//������Ϣ
	LRESULT	OnSetTextMesage(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif