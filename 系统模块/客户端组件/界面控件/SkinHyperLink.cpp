#include "Stdafx.h"
#include "Resource.h"
#include "SkinHyperLink.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
CSkinHyperLinkAttribute				CSkinHyperLink::m_SkinAttribute;	//�ؼ�����

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkinHyperLink, CStatic)
	ON_WM_PAINT()
	ON_WM_CREATE()
    ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_SETTEXT, OnSetTextMesage)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_CONTROL_REFLECT(STN_CLICKED, OnStnClicked)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CSkinHyperLinkAttribute::CSkinHyperLinkAttribute()
{
	//���ñ���
	m_crBackColor=RGB(0,0,0);
	m_crHoverText=RGB(0,0,0);
	m_crNormalText=RGB(0,0,0);
	m_crVisitedText=RGB(0,0,0);

	return;
}

//��������
CSkinHyperLinkAttribute::~CSkinHyperLinkAttribute()
{
}

//��������
bool CSkinHyperLinkAttribute::UpdateSkinResource(ISkinResource * pISkinResource)
{
	//��ȡ��Ϣ
	tagSkinHyperLinkResource SkinHyperLinkResource;
	if (pISkinResource!=NULL) pISkinResource->GetHyperLinkResource(SkinHyperLinkResource);

	//���ñ���
	m_crBackColor=SkinHyperLinkResource.crBackColor;
	m_crHoverText=SkinHyperLinkResource.crHoverText;
	m_crNormalText=SkinHyperLinkResource.crNormalText;
	m_crVisitedText=SkinHyperLinkResource.crVisitedText;

	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CSkinHyperLink::CSkinHyperLink()
{
	//���ñ���
	m_bUnderLine=true;
	m_AutoRectify=true;

	//��ɫ����
	m_crBackColor=m_SkinAttribute.m_crBackColor;
	m_crHoverText=m_SkinAttribute.m_crHoverText;
	m_crNormalText=m_SkinAttribute.m_crNormalText;
	m_crVisitedText=m_SkinAttribute.m_crVisitedText;

	//״̬����
	m_bVisited=false;
	m_bHovering=false;

	//�������
	m_pISkinMouseEvent=NULL;

	return;
}

//��������
CSkinHyperLink::~CSkinHyperLink()
{
}

//���ӿ�
bool CSkinHyperLink::SetSkinMouseEvent(IUnknownEx * pIUnknownEx)
{
	//���ýӿ�
	if (pIUnknownEx!=NULL)
	{
		//��ѯ�ӿ�
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ISkinMouseEvent)!=NULL);
		m_pISkinMouseEvent=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ISkinMouseEvent);

		//�ɹ��ж�
		if (m_pISkinMouseEvent==NULL) return false;
	}
	else m_pISkinMouseEvent=NULL;

	return true;
}

//��ȡ�ӿ�
VOID * CSkinHyperLink::GetSkinMouseEvent(const IID & Guid, DWORD dwQueryVer) 
{ 
	if (m_pISkinMouseEvent==NULL) return NULL;
	return m_pISkinMouseEvent->QueryInterface(Guid,dwQueryVer);
}

//��Ϣ����
BOOL CSkinHyperLink::PreTranslateMessage(MSG * pMsg)
{
	//��ʾ����
	if (m_ToolTipCtrl.m_hWnd!=NULL) m_ToolTipCtrl.RelayEvent(pMsg);

	return __super::PreTranslateMessage(pMsg);
}

//�ؼ����໯
VOID CSkinHyperLink::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//��ʼ��
	InitHyperLink();

	return;
}

//������Ϣ
INT CSkinHyperLink::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//��ʼ��
	InitHyperLink();

	return 0;
}

//�滭��Ϣ
VOID CSkinHyperLink::OnPaint()
{
	CPaintDC dc(this);

	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//�滭����
	dc.FillSolidRect(&rcClient,m_crBackColor);

	//���� DC
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(m_SkinAttribute.GetDefaultFont());

	//������ɫ
	if (m_bHovering==true) dc.SetTextColor(m_crHoverText);
	else if (m_bVisited==true) dc.SetTextColor(m_crVisitedText);
	else dc.SetTextColor(m_crNormalText);

	//��ȡ��ʽ
	LONG dwStyte=GetWindowLong(m_hWnd,GWL_STYLE);
	UINT uFormat=DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
	if (dwStyte&SS_RIGHT) uFormat|=DT_RIGHT;
	else if (dwStyte&SS_CENTER) uFormat|=DT_CENTER;
	else uFormat|=DT_LEFT;

	//�滭����
	CString strText;
	GetWindowText(strText);
	dc.DrawText(strText,&rcClient,uFormat);

	return;
}

//�����Ϣ
BOOL CSkinHyperLink::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//���ù��
	::SetCursor(LoadCursor(GetModuleHandle(SKIN_CONTROL_DLL_NAME),MAKEINTRESOURCE(IDC_HAND_CUR)));

	return TRUE;
}

//�����ؼ�
VOID CSkinHyperLink::OnStnClicked()
{
	//�ж�״̬
	if ((m_pISkinMouseEvent!=NULL)||(m_strHyperLink.IsEmpty()))	return;

	//�ػ��ؼ�
	if (m_bVisited==false)
	{
		m_bVisited=true;
		Invalidate(FALSE);
	}

	//����ҳ
	ShellExecute(NULL,TEXT("open"),m_strHyperLink,NULL,NULL,SW_SHOW);

	return;
}

//�����Ϣ
VOID CSkinHyperLink::OnMouseMove(UINT nFlags, CPoint Point)
{
	//״̬�ж�
	if (m_bHovering==true) return;

	//���ñ���
	m_bHovering=true;

	//ע����Ϣ
	TRACKMOUSEEVENT TrackMouseEvent;
	TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);
	TrackMouseEvent.dwFlags=TME_LEAVE;
	TrackMouseEvent.hwndTrack=GetSafeHwnd();
	TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
	_TrackMouseEvent(&TrackMouseEvent);

	//�ػ�����
	Invalidate(FALSE);

	__super::OnMouseMove(nFlags, Point);
}

//�����Ϣ
LRESULT CSkinHyperLink::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	//���ñ���
	m_bHovering=false;

	//�ػ��ؼ�
	Invalidate(FALSE);

	return 0;
}

//������Ϣ
LRESULT	CSkinHyperLink::OnSetTextMesage(WPARAM wParam, LPARAM lParam)
{
	//Ĭ�ϵ���
	LRESULT lResult=Default();

	//�����ؼ�
	if (m_AutoRectify==true) RectifyControl();

	return lResult;
}

//���÷���
VOID CSkinHyperLink::SetVisited(bool bVisited)
{
	//״̬�ж�
	if (m_bVisited==bVisited) return;

	//���ñ���
	m_bVisited=bVisited;

	//�ػ�����
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return;
}

//�����»���
VOID CSkinHyperLink::SetUnderLine(bool bUnderLine)
{
	//״̬�ж�
	if (m_bUnderLine==bUnderLine) return;

	//���ñ���
	m_bUnderLine=bUnderLine;

	//�ػ�����
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return;
}

//���õ���
VOID CSkinHyperLink::SetAutoRectify(bool bAutoRectify)
{
	//״̬�ж�
	if (m_AutoRectify==bAutoRectify) return;

	//���ñ���
	m_AutoRectify=bAutoRectify;

	//�����ؼ�
	if (m_AutoRectify==true) RectifyControl();

	return;
}

//������ɫ
VOID CSkinHyperLink::SetBackGroundColor(COLORREF crColor)
{
	//���ñ���
	m_crBackColor=crColor;

	//�ػ�����
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return;
}

//������ɫ
VOID CSkinHyperLink::SetHyperLinkTextColor(COLORREF crHoverText, COLORREF crNormalText, COLORREF crVisitedText)
{
	//���ñ���
	m_crHoverText=crHoverText;
	m_crNormalText=crNormalText;
	m_crVisitedText=crVisitedText;

	//�ػ�����
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return;
}

//���� URL
VOID CSkinHyperLink::SetHyperLinkUrl(LPCTSTR pszHyperLink)
{
	//���ñ���
	m_strHyperLink=pszHyperLink;

	//������ʾ
	UpdateToolTip();

	return;
}

//��ʼ��
VOID CSkinHyperLink::InitHyperLink()
{
	//��������
	m_crBackColor=m_SkinAttribute.m_crBackColor;
	SetWindowLong(m_hWnd,GWL_STYLE,GetStyle()|SS_NOTIFY);

	//��������
	if (m_AutoRectify=true) RectifyControl();

	//������ʾ
	UpdateToolTip();

	return;
}

//������ʾ
VOID CSkinHyperLink::UpdateToolTip()
{
	return;
	//״̬�ж�
	if (m_hWnd==NULL) return;

	//�����ؼ�
	if (m_ToolTipCtrl.m_hWnd==NULL) m_ToolTipCtrl.Create(this);

	//������ʾ
	if (m_strHyperLink.IsEmpty()==false)
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		m_ToolTipCtrl.Activate(TRUE);
		m_ToolTipCtrl.AddTool(this,m_strHyperLink,&rcClient,10);
	}
	else m_ToolTipCtrl.Activate(FALSE);

	return;
}

//����λ��
VOID CSkinHyperLink::RectifyControl()
{
	//��ȡ����
	CString strText;
	GetWindowText(strText);

	//��ȡλ��
	CRect ControlRect;
	GetWindowRect(&ControlRect);
	GetParent()->ScreenToClient(&ControlRect);

	//���㳤��
	CDC * pDC=GetDC();
	pDC->SelectObject(&m_SkinAttribute.m_DefaultFont);
	CSize Size=pDC->GetTextExtent(strText);
	ReleaseDC(pDC);

	//�ƶ�λ��
	MoveWindow(ControlRect.left,ControlRect.top,Size.cx,Size.cy);

	return;
}

//Ĭ�ϻص�����
LRESULT CSkinHyperLink::DefWindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	//�¼�����
	if (m_pISkinMouseEvent!=NULL)
	{
		switch (uMessage)
		{
		case WM_MOUSEMOVE:
		case WM_MOUSELEAVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			{
				UINT uControlID=GetWindowLong(m_hWnd,GWL_ID);
				m_pISkinMouseEvent->OnEventMouse(uMessage,uControlID,(UINT)wParam,LOWORD(lParam),HIWORD(lParam));
				break;
			}
		}
	}

	return __super::DefWindowProc(uMessage, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////

