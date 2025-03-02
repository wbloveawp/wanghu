#include "Stdafx.h"
#include "Resource.h"
#include "SkinButton.h"
#include ".\skinbutton.h"

//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_TRANSITION				10									//���ɱ�ʶ
#define TIME_TRANSITION				30									//����ʱ��

//��������
#define MIN_TRANSITION_INDEX		0									//��������
#define MAX_TRANSITION_INDEX		8									//��������

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkinButton, CButton)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_NCDESTROY()
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//��̬����
CSkinButtonAttribute				CSkinButton::m_SkinAttribute;		//��ť����

//////////////////////////////////////////////////////////////////////////

//���캯��
CSkinButtonAttribute::CSkinButtonAttribute()
{
	//���ñ���
	m_crText=RGB(0,0,0);

	return;
}

//��������
CSkinButtonAttribute::~CSkinButtonAttribute() 
{
}

//������Դ
bool CSkinButtonAttribute::UpdateSkinResource(ISkinResource * pISkinResource)
{
	//��ȡ��Ϣ
	tagSkinButtonResource SkinButtonResource;
	HINSTANCE hResResource=pISkinResource->GetResInstance();

	//��ȡ��Դ
	if (pISkinResource!=NULL) pISkinResource->GetButtonResource(SkinButtonResource);

	//���ñ���
	m_crText=SkinButtonResource.crButtonText;

	//������Դ
	m_ImageBack.SetLoadInfo(SkinButtonResource.uButton,hResResource);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CSkinButton::CSkinButton()
{
	//״̬����
	m_bExpand=true;
	m_bHovering=false;
	m_bTransparent=false;

	//���ñ���
	m_pISkinMouseEvent=NULL;
	m_crText=m_SkinAttribute.m_crText;

	m_wImageIndex=MIN_TRANSITION_INDEX;

	return;
}

//��������
CSkinButton::~CSkinButton()
{
}

//�󶨺���
VOID CSkinButton::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//���ð�ť
	SetButtonStyle(GetButtonStyle()|BS_OWNERDRAW);
	SendMessage(WM_SETFONT,(WPARAM)(HFONT)(m_SkinAttribute.GetDefaultFont()),TRUE);

	return;
}

//��������
BOOL CSkinButton::OnEraseBkgnd(CDC * pDC)
{
	//���½���
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//������Ϣ
INT CSkinButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//���ð�ť
	SetButtonStyle(GetButtonStyle()|BS_OWNERDRAW);
	SendMessage(WM_SETFONT,(WPARAM)(HFONT)(m_SkinAttribute.GetDefaultFont()),TRUE);

	return 0;
}

//������ɫ
bool CSkinButton::SetTextColor(COLORREF crText)
{
	//���ñ���
	m_crText=crText;

	//���½���
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return true;
}

//����λͼ
bool CSkinButton::SetButtonImage(LPCTSTR pszFileName, bool bExpandImage, bool bTransparent)
{
	//Ч�����
	ASSERT(pszFileName);
	if (pszFileName==NULL) return false;

	//���ñ���
	m_bExpand=bExpandImage;
	m_bTransparent=bTransparent;
	m_ImageBack.SetLoadInfo(pszFileName);

	//���½���
	if (m_bExpand==false) RectifyControl();

	//���´���
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return true;
}

//����λͼ
bool CSkinButton::SetButtonImage(UINT uBitmapID, HINSTANCE hInstance, bool bExpandImage, bool bTransparent)
{
	//���ñ���
	m_bExpand=bExpandImage;
	m_bTransparent=bTransparent;
	m_ImageBack.SetLoadInfo(uBitmapID,hInstance);

	//���½���
	if (m_bExpand==false) RectifyControl();

	//���´���
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return true;
}

//��ȡλͼ
bool CSkinButton::GetButtonImage(tagImageLoadInfo & ImageLoadInfo)
{
	//��������
	bool bSuccess=false;

	//��ȡ����
	if (m_ImageBack.IsSetLoadInfo()==true)
	{
		bSuccess=m_ImageBack.GetLoadInfo(ImageLoadInfo);
	}
	else 
	{
		bSuccess=CSkinButton::m_SkinAttribute.m_ImageBack.GetLoadInfo(ImageLoadInfo);
	}

	return bSuccess;
}

//����λ��
VOID CSkinButton::RectifyControl()
{
	//����Ч��
	ASSERT((m_ImageBack.IsSetLoadInfo()==true)&&(m_hWnd!=NULL));
	if ((m_ImageBack.IsSetLoadInfo()==false)||(m_hWnd==NULL)) return;

	//������Դ
	CImageHandle ImageHandle(&m_ImageBack);

	//������С
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth()/5,m_ImageBack.GetHeight(),SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);

	//͸������
	if (m_bTransparent==true)
	{
		//��������
		CRgn RgnControl;
		m_ImageBack.CreateImageRegion(RgnControl,RGB(255,0,255));

		//��������
		SetWindowRgn(RgnControl,TRUE);
	}

	return;
}

//�����Ϣ
VOID CSkinButton::OnMouseMove(UINT nFlags, CPoint Point)
{
	__super::OnMouseMove(nFlags, Point);

	if (m_bHovering==false)
	{
		//���ñ���
		m_bHovering=true;

		//���½���
		Invalidate(FALSE);

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

		//���ɴ���
		if (m_wImageIndex<MAX_TRANSITION_INDEX)
		{
			SetTimer(IDI_TRANSITION,TIME_TRANSITION,NULL);
		}
	}

	return;
}

//�����Ϣ
LRESULT CSkinButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	//���ñ���
	m_bHovering=false;

	//���ɴ���
	if (m_wImageIndex>MIN_TRANSITION_INDEX)
	{
		SetTimer(IDI_TRANSITION,TIME_TRANSITION,NULL);
	}

	//���½���
	Invalidate(FALSE);

	return 0;
}

//�����Ϣ
BOOL CSkinButton::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//���ù��
	SetCursor(LoadCursor(GetModuleHandle(SKIN_CONTROL_DLL_NAME),MAKEINTRESOURCE(IDC_HAND_CUR)));

	return TRUE;
}

//������Ϣ
VOID CSkinButton::OnNcDestroy()
{
	//״̬����
	m_bExpand=true;
	m_bHovering=false;

	//��������
	m_wImageIndex=MIN_TRANSITION_INDEX;

	__super::OnNcDestroy();
}

//�滭����
VOID CSkinButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	HDC hDC=lpDrawItemStruct->hDC;
	CDC * pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	bool bDisable=((lpDrawItemStruct->itemState&ODS_DISABLED)!=0);
	bool bButtonDown=((lpDrawItemStruct->itemState&ODS_SELECTED)!=0);

	//��ȡ����
	CString strText;
	GetWindowText(strText);

	//������Դ
	CImageHandle ImageHandle;
	if (m_ImageBack.IsSetLoadInfo()==true) ImageHandle.AttachResource(&m_ImageBack);
	else ImageHandle.AttachResource(&m_SkinAttribute.m_ImageBack);

	//�滭����
	if (ImageHandle.IsValid()==true)
	{
		//��������
		INT nImagePos=0;
		INT nPartWidth=ImageHandle->GetWidth()/5;

		//����λ��
		if (bDisable==true) nImagePos=nPartWidth*4;
		else if (bButtonDown==true) nImagePos=nPartWidth;
		else if (m_bHovering==true) nImagePos=nPartWidth*3;

		//״̬�ж�
		bool bTransitionDraw=false;
		if ((m_bHovering==true)&&(m_wImageIndex<MAX_TRANSITION_INDEX)) bTransitionDraw=true;
		if ((m_bHovering==false)&&(m_wImageIndex>MIN_TRANSITION_INDEX)) bTransitionDraw=true;

		//�滭����
		if ((bDisable==false)&&(bButtonDown==false)&&(bTransitionDraw==true))
		{
			//��������
			CBitImage BufferImage;
			BufferImage.Create(rcClient.Width(),rcClient.Height(),32);

			//��ȡ�豸
			HDC hBufferDC=BufferImage.GetDC();
			CDC * pBufferDC=CDC::FromHandle(hBufferDC);
			pBufferDC->SelectObject(pDC->GetCurrentFont());

			//�滭��ͼ
			if (m_wImageIndex<MAX_TRANSITION_INDEX)
			{
				if (m_bExpand==false)
				{
					ImageHandle->BitBlt(hBufferDC,0,0,rcClient.Width(),rcClient.Height(),0,0,SRCCOPY);
				}
				else
				{
					ImageHandle->StretchBlt(hBufferDC,0,0,rcClient.Width(),rcClient.Height(),0,0,nPartWidth,ImageHandle->GetHeight(),SRCCOPY);
				}
			}

			//�滭����
			if (m_wImageIndex>MIN_TRANSITION_INDEX)
			{
				if (m_bTransparent==false)
				{
					ImageHandle->AlphaBlend(pBufferDC->GetSafeHdc(),0,0,rcClient.Width(),rcClient.Height(),
						nPartWidth*3,0,nPartWidth,ImageHandle->GetHeight(),m_wImageIndex*255/MAX_TRANSITION_INDEX);
				}
				else
				{
					ImageHandle->BlendDrawImage(pBufferDC,0,0,rcClient.Width(),rcClient.Height(),
						nPartWidth*3,0,RGB(255,0,255),m_wImageIndex*255/MAX_TRANSITION_INDEX);
				}
			}

			//����λ��
			rcClient.top++;
			if (bButtonDown==true) rcClient.top+=2;

			//�滭����
			pBufferDC->SetBkMode(TRANSPARENT);
			pBufferDC->SetTextColor((bDisable==true)?GetSysColor(COLOR_GRAYTEXT):m_crText);
			pBufferDC->DrawText(strText,strText.GetLength(),rcClient,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			//�滭����
			if (m_bTransparent==false)
			{
				BufferImage.BitBlt(hDC,0,0,rcClient.Width(),rcClient.Height(),0,0);
			}
			else
			{
				BufferImage.AlphaDrawImage(pDC,0,0,rcClient.Width(),rcClient.Height(),0,0,RGB(255,0,255));
			}

			//������Դ
			BufferImage.ReleaseDC();

			return;
		}
		else
		{
			if (m_bTransparent==false)
			{
				if (m_bExpand==false) ImageHandle->BitBlt(hDC,0,0,rcClient.Width(),rcClient.Height(),nImagePos,0,SRCCOPY);
				else ImageHandle->StretchBlt(hDC,0,0,rcClient.Width(),rcClient.Height(),nImagePos,0,nPartWidth,ImageHandle->GetHeight(),SRCCOPY);
			}
			else
			{
				CDC * pDestDC=CDC::FromHandle(hDC);
				ImageHandle->AlphaDrawImage(pDestDC,0,0,rcClient.Width(),rcClient.Height(),nImagePos,0,RGB(255,0,255));
			}
		}
	}
	else
	{
		//Ĭ�Ͻ���
		CDC * pDC=CDC::FromHandle(hDC);
		pDC->FillSolidRect(&rcClient,GetSysColor(COLOR_BTNFACE));
		if (bButtonDown) pDC->Draw3dRect(&rcClient,GetSysColor(COLOR_WINDOWFRAME),GetSysColor(COLOR_3DHILIGHT));
		else pDC->Draw3dRect(&rcClient,GetSysColor(COLOR_3DHILIGHT),GetSysColor(COLOR_WINDOWFRAME));
	}

	//����λ��
	rcClient.top++;
	if (bButtonDown==true) rcClient.top+=2;

	//�滭����
	::SetBkMode(hDC,TRANSPARENT);
	::SetTextColor(hDC,(bDisable==true)?GetSysColor(COLOR_GRAYTEXT):m_crText);
	DrawText(hDC,strText,strText.GetLength(),rcClient,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

	return;
}

//�ص�����
LRESULT CSkinButton::DefWindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
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
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
			{
				UINT uControlID=GetWindowLong(m_hWnd,GWL_ID);
				m_pISkinMouseEvent->OnEventMouse(uMessage,uControlID,(UINT)wParam,LOWORD(lParam),HIWORD(lParam));
				break;
			}
		}
	}

	return __super::DefWindowProc(uMessage, wParam, lParam);
}

//���ӿ�
bool CSkinButton::SetSkinMouseEvent(IUnknownEx * pIUnknownEx)
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
VOID * CSkinButton::GetSkinMouseEvent(const IID & Guid, DWORD dwQueryVer) 
{ 
	if (m_pISkinMouseEvent==NULL) return NULL;
	return m_pISkinMouseEvent->QueryInterface(Guid,dwQueryVer);
}

//ʱ����Ϣ
void CSkinButton::OnTimer(UINT_PTR nIDEvent)
{
	//���ɱ���
	if (nIDEvent==IDI_TRANSITION)
	{
		//��������
		if ((m_bHovering==true)&&(m_wImageIndex<MAX_TRANSITION_INDEX))
		{
			m_wImageIndex++;
		}

		//��̬����
		if ((m_bHovering==false)&&(m_wImageIndex>MIN_TRANSITION_INDEX))
		{
			m_wImageIndex--;
		}

		//ɾ��ʱ��
		if ((m_wImageIndex==MIN_TRANSITION_INDEX)||(m_wImageIndex==MAX_TRANSITION_INDEX))
		{
			KillTimer(IDI_TRANSITION);
		}

		//���½���
		Invalidate(FALSE);

		return;
	}

	__super::OnTimer(nIDEvent);
}
//////////////////////////////////////////////////////////////////////////

