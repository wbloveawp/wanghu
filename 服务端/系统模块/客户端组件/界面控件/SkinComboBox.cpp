#include "StdAfx.h"
#include "SkinDialog.h"
#include "SkinComboBox.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
CSkinComboBoxAttribute			CSkinComboBox::m_SkinAttribute;			//��ť����

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkinComboEdit, CEdit)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CSkinComboBox, CComboBox)
    ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_NCDESTROY()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CSkinComboBoxAttribute::CSkinComboBoxAttribute()
{
	//���ñ���
	m_crBoradHover=RGB(0,0,0);
	m_crBoradNormal=RGB(0,0,0);

	return;
}

//��������
CSkinComboBoxAttribute::~CSkinComboBoxAttribute() 
{
}

//������Դ
bool CSkinComboBoxAttribute::UpdateSkinResource(ISkinResource * pISkinResource)
{
	//��ȡ��Ϣ
	tagSkinComboBoxResource SkinComboBoxResource;
	HINSTANCE hResResource=pISkinResource->GetResInstance();

	//��ȡ��Դ
	if (pISkinResource!=NULL) pISkinResource->GetComboBoxResource(SkinComboBoxResource);

	//���ñ���
	m_crBoradHover=SkinComboBoxResource.crBoradHover;
	m_crBoradNormal=SkinComboBoxResource.crBoradNormal;

	//������Դ
	m_ImageButtonThumb.SetLoadInfo(SkinComboBoxResource.uThumb,hResResource);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CSkinComboEdit::CSkinComboEdit()
{
	//����״̬
	m_bHovering=false;

	return;
}

//��������
CSkinComboEdit::~CSkinComboEdit()
{
}

//������Ϣ
VOID CSkinComboEdit::OnNcDestroy()
{
	//����״̬
	m_bHovering=false;

	__super::OnNcDestroy();
}

//�����Ϣ
VOID CSkinComboEdit::OnMouseMove(UINT nFlags, CPoint Point)
{
	__super::OnMouseMove(nFlags, Point);

	if (m_bHovering==false)
	{
		//���ñ���
		m_bHovering=true;

		//���½���
		Invalidate();

		//��������
		TRACKMOUSEEVENT TrackMouseEvent;
		ZeroMemory(&TrackMouseEvent,sizeof(TrackMouseEvent));

		//ע����Ϣ
		TrackMouseEvent.hwndTrack=m_hWnd;
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);

		//ע���¼�
		::TrackMouseEvent(&TrackMouseEvent);
	}

	return;
}

//�����Ϣ
LRESULT CSkinComboEdit::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	//���ñ���
	m_bHovering=false;

	//���½���
	Invalidate();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CSkinComboBox::CSkinComboBox()
{
	//����״̬
	m_bHovering=false;

	return;
}

//��������
CSkinComboBox::~CSkinComboBox()
{
}

//�ؼ���
VOID CSkinComboBox::PreSubclassWindow ()
{
	//�󶨿ؼ�
    HWND hEdit=::GetWindow(GetSafeHwnd(),GW_CHILD);
    if (hEdit!=NULL) m_SkinComboEdit.SubclassWindow(hEdit);

	return;
}

//�ػ���Ϣ
VOID CSkinComboBox::OnPaint()
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��ť����
    CRect rcThumb;
	rcThumb.SetRect(rcClient.right-GetSystemMetrics(SM_CXHTHUMB)-1,rcClient.top+1,rcClient.right-1, rcClient.bottom-1);

	//Ĭ�ϴ���
    ValidateRect(rcThumb);
    Default();

	//��Դ����
    CClientDC ClientDC(this);

	//״̬����
	bool bDisable=(IsWindowEnabled()==FALSE);
	bool bDropped=(DefWindowProc(CB_GETDROPPEDSTATE,0L,0L)!=0L);
	bool bHovering=(m_bHovering==true)||(m_SkinComboEdit.m_bHovering==true);

	//�滭����
	if (bDisable==false)
	{
		COLORREF crColor=GetSysColor(COLOR_WINDOW);
		ClientDC.Draw3dRect(1,1,rcClient.Width()-rcThumb.Width()-2,rcClient.Height()-2,crColor,crColor);
	}
	else
	{
		COLORREF crColor=CSkinDialog::m_SkinAttribute.m_crBackGround;
		ClientDC.Draw3dRect(1,1,rcClient.Width()-rcThumb.Width()-2,rcClient.Height()-2,crColor,crColor);
	}

	//�滭�߿�
	if ((bDisable==false)&&(bHovering==true))
	{
		ClientDC.Draw3dRect(&rcClient,m_SkinAttribute.m_crBoradHover,m_SkinAttribute.m_crBoradHover);
	}
	else
	{
		ClientDC.Draw3dRect(&rcClient,m_SkinAttribute.m_crBoradNormal,m_SkinAttribute.m_crBoradNormal);
	}

	//����λ��
	INT nImageIndex=0;
	if (bDisable==true) nImageIndex=4;
	else if (bDropped==true) nImageIndex=1;
	else if (bHovering==true) nImageIndex=3;

	//�滭��ť
	CImageHandle HandleThumb(&m_SkinAttribute.m_ImageButtonThumb);
	INT nButtonWidth=m_SkinAttribute.m_ImageButtonThumb.GetWidth()/5L;
	m_SkinAttribute.m_ImageButtonThumb.BitBlt(ClientDC,rcThumb.left,rcThumb.top,nButtonWidth,rcThumb.Height(),nImageIndex*nButtonWidth,0);

	return;
}

//������Ϣ
VOID CSkinComboBox::OnNcDestroy()
{
	//����״̬
	m_bHovering=false;

	__super::OnNcDestroy();
}

//������Ϣ
INT CSkinComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//�󶨿ؼ�
    HWND hEdit=::GetWindow(GetSafeHwnd(),GW_CHILD);
    if (hEdit!=NULL) m_SkinComboEdit.SubclassWindow(hEdit);

	return 0;
}

//�����Ϣ
VOID CSkinComboBox::OnMouseMove(UINT nFlags, CPoint Point)
{
	__super::OnMouseMove(nFlags, Point);

	if (m_bHovering==false)
	{
		//���ñ���
		m_bHovering=true;

		//���½���
		Invalidate();

		//��������
		TRACKMOUSEEVENT TrackMouseEvent;
		ZeroMemory(&TrackMouseEvent,sizeof(TrackMouseEvent));

		//ע����Ϣ
		TrackMouseEvent.hwndTrack=m_hWnd;
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);

		//ע���¼�
		::TrackMouseEvent(&TrackMouseEvent);
	}

	return;
}

//�����Ϣ
LRESULT CSkinComboBox::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	//���ñ���
	m_bHovering=false;

	//���½���
	Invalidate();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
