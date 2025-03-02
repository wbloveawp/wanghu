#ifndef SKIN_DIALOG_HEAD_FILE
#define SKIN_DIALOG_HEAD_FILE

#pragma once

#include "SkinControlsHead.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////////////
//��������
class CSkinDialogAttribute;
class CSkinLayered;
class CSkinDialog;

//��������
typedef CMap<HWND,HWND,CSkinLayered*,CSkinLayered*>		CSkinLayeredMap;

//////////////////////////////////////////////////////////////////////////

//������Դ
class CSkinDialogAttribute : public CSkinResourceManager
{
	friend class CSkinDialog;

	//������Ϣ
public:
	bool							m_bShowIcon;						//��ʾͼ��
	bool							m_bShowTitle;						//��ʾ����
	HICON							m_hDefaultIcon;						//Ĭ��ͼ��

	//ƫ����Ϣ
public:
	INT								m_nXIconExcursion;					//ͼ��ƫ��
	INT								m_nYIconExcursion;					//ͼ��ƫ��
	INT								m_nXButtonExcursion;				//��ťƫ��
	INT								m_nYButtonExcursion;				//��ťƫ��
	INT								m_nXTitleTextExcursion;				//����ƫ��
	INT								m_nYTitleTextExcursion;				//����ƫ��

	//��ɫ����
public:
	COLORREF						m_crTitleText;						//������ɫ
	COLORREF						m_crBackGround;						//������ɫ
	COLORREF						m_crControlText;					//������ɫ

	//��������
public:
	INT								m_nButtonWidth;						//��ť���
	INT								m_nButtonHeigth;					//��ť�߶�

	//�߿����
public:
	INT								m_nXBorder;							//�߿��С
	INT								m_nYBorder;							//�߿��С
	INT								m_nCaptionHeigth;					//����߶�

	//������ˢ
public:
	CBrush							m_brBackGround;						//������ˢ

	//������Դ
public:
	CBitImage						m_SkinImageTL;						//����λͼ
	CBitImage						m_SkinImageTM;						//����λͼ
	CBitImage						m_SkinImageTR;						//����λͼ
	CBitImage						m_SkinImageMR;						//����λͼ
	CBitImage						m_SkinImageML;						//����λͼ
	CBitImage						m_SkinImageBL;						//����λͼ
	CBitImage						m_SkinImageBM;						//����λͼ
	CBitImage						m_SkinImageBR;						//����λͼ

	//��ť��Դ
public:
	CBitImage						m_ButtonMin;						//��ťλͼ
	CBitImage						m_ButtonMax;						//��ťλͼ
	CBitImage						m_ButtonRes;						//��ťλͼ
	CBitImage						m_ButtonClose;						//��ťλͼ

	//��������
protected:
	//���캯��
	CSkinDialogAttribute();
	//��������
	virtual ~CSkinDialogAttribute();

	//���ܺ���
public:
	//������Դ
	bool UpdateSkinResource(ISkinResource * pISkinResource);
};

//////////////////////////////////////////////////////////////////////////////////
//�ֲ㴰��
class SKIN_CONTROL_CLASS CSkinLayered : public CWnd
{
	//��������
protected:	
	HWND							m_hwndControl;						//���ھ��
	LONG_PTR						m_OriParentProc;					//���ڹ���

	//��̬����
protected:
	static CSkinLayeredMap			m_SkinLayeredMap;					//�ֲ��б�

	//��������
public:
	//���캯��
	CSkinLayered();
	//��������
	virtual ~CSkinLayered();

	//���ܺ���
public:
	//��������
	bool CreateLayered(HWND hWnd);
	//��������
	VOID InitLayeredArea(CDC * pDCImage, BYTE cbAlpha, CRect & rcUnLayered, CPoint & PointRound, bool bUnLayeredChild);
	//��������
	VOID InitLayeredArea(CPngImageWarpper & Image, BYTE cbAlpha, CRect & rcUnLayered, CPoint & PointRound, bool bUnLayeredChild);

	//��Ϣ����
public:
	//�ر���Ϣ
	VOID OnClose();
	//���ý���
	VOID OnSetFocus(CWnd* pOldWnd);

	//��̬����
protected:
	//ö�ٺ���
	static BOOL CALLBACK EnumChildProc(HWND hWndChild, LPARAM lParam);
	//���ڹ���
	static LRESULT CALLBACK ParentProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////// 

//�Ի�����
class SKIN_CONTROL_CLASS CSkinDialog : public CDialog
{
	//����״̬
private:
	bool							m_bActive;							//�����־
	bool							m_bMaxShow;							//����־

	//����ͼƬ
public:

	// add by humorly 2018/8/1
	// replace CPngImage --> CPngImageWarpper
	CPngImageWarpper				m_ImageTitle;						//����ͼƬ

	//��ť״̬
private:
	BYTE							m_cbNowHotButton;					//���ڽ���
	BYTE							m_cbNowDownButton;					//���°�ť
	BYTE							m_cbButtonState[3];					//��ť״̬

	//λ�ñ���
private:
	CRect							m_rcButton[3];						//��ťλ��
	CRect							m_rcNormalSize;						//����λ��

	//��Դ����
public:
	static CSkinDialogAttribute		m_SkinAttribute;					//��Դ����

	//��������
public:
	//���캯��
	CSkinDialog(UINT nIDTemplate, CWnd * pParentWnd=NULL);
	//��������
	virtual ~CSkinDialog();

	//���غ���
public:
	//��ʼ����Ϣ
	virtual BOOL OnInitDialog();
	//�滭��Ϣ
	virtual VOID OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeigth){}

	//���ƺ���
public:
	//��󴰿�
	VOID MaxSizeWindow();
	//��ԭ����
	VOID RestoreWindow();
	//��ť����
	VOID EnableButton(UINT uButtonStyle);

	//��Ϣ����
protected:
	//��ȡ�߿�
	INT GetXBorder() { return m_SkinAttribute.m_nXBorder; }
	//��ȡ�߿�
	INT GetYBorder() { return m_SkinAttribute.m_nYBorder; }
	//��ȡ����
	INT GetCaptionHeight() { return m_SkinAttribute.m_nCaptionHeigth; }

	//�滭����
protected:
	//�滭�߿�
	VOID DrawBorder(CDC * pDC);
	//�滭����
	VOID DrawCaption(CDC * pDC);
	//�滭����
	VOID DrawBackGround(CDC * pDC);

	//��������
private:
	//���±���
	VOID UpdateCaption();
	//������ť
	VOID AdjustTitleButton(INT nWidth, INT nHeight);

	//��Ϣӳ��
protected:
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);
	//��������
	LRESULT OnNcHitTest(CPoint Point);
	//�����Ϣ
	VOID OnMouseMove(UINT nFlags, CPoint Point);
	//�����Ϣ
	VOID OnLButtonUp(UINT nFlags, CPoint Point);
	//�����Ϣ
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//�����Ϣ
	VOID OnLButtonDblClk(UINT nFlags, CPoint Point);
	//λ�øı�
	VOID OnWindowPosChanged(WINDOWPOS * lpwndpos);
	//������Ϣ
	VOID OnActivateApp(BOOL bActive, DWORD dwThreadID);
	//������Ϣ
	VOID OnActivate(UINT nState, CWnd * pWndOther, BOOL bMinimized);
	//�ı���Ϣ
	VOID OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	//�ؼ���ɫ
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);
	//������Ϣ
	LRESULT	OnSetTextMesage(WPARAM wParam, LPARAM lParam);
	//��ȡ��С
	VOID OnGetMinMaxInfo(MINMAXINFO* lpMMI);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif