#include "Stdafx.h"
#include "SkinEdit.h"
#include "SkinDialog.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
CSkinEditAttribute					CSkinEdit::m_SkinAttribute;				//��������

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkinEdit, CEdit)
    ON_WM_NCPAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkinEditEx, CSkinEdit)
    ON_WM_NCPAINT()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CSkinEditAttribute::CSkinEditAttribute()
{
	//���ñ���
	m_crFocusTX=RGB(0,0,0);
	m_crFocusBK=RGB(0,0,0);
	m_crNoFocusTX=RGB(0,0,0);
	m_crNoFocusBK=RGB(0,0,0);
	m_crDisFocusTX=RGB(0,0,0);
	m_crDisFocusBK=RGB(0,0,0);

	return;
}

//��������
CSkinEditAttribute::~CSkinEditAttribute() 
{
}

//������Դ
bool CSkinEditAttribute::UpdateSkinResource(ISkinResource * pISkinResource)
{
	//��ȡ��Ϣ
	tagSkinEditResource SkinEditResource;
	if (pISkinResource!=NULL) pISkinResource->GetEditResource(SkinEditResource);

	//���ñ���
	m_crFocusTX=SkinEditResource.crFocusTX;
	m_crFocusBK=SkinEditResource.crFocusBK;
	m_crNoFocusTX=SkinEditResource.crNoFocusTX;
	m_crNoFocusBK=SkinEditResource.crNoFocusBK;
	m_crDisFocusTX=SkinEditResource.crDisFocusTX;
	m_crDisFocusBK=SkinEditResource.crDisFocusBK;
	m_crBoradNormal=SkinEditResource.crBoradNormal;

	//������ˢ
	m_brFocus.DeleteObject();
	m_brNoFocus.DeleteObject();
	m_brDisbale.DeleteObject();
	m_brFocus.CreateSolidBrush(m_crFocusBK);
	m_brNoFocus.CreateSolidBrush(m_crNoFocusBK);
	m_brDisbale.CreateSolidBrush(m_crDisFocusBK);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CSkinEdit::CSkinEdit()
{
	//���ñ���
	m_bFocus=false;

	return;
}

//��������
CSkinEdit::~CSkinEdit()
{
}

//�滭�ؼ�
HBRUSH CSkinEdit::CtlColor(CDC * pDC, UINT nCtlColor)
{
	//����״̬
	if (IsWindowEnabled())
	{
		if (m_bFocus==true) 
		{
			pDC->SetBkColor(m_SkinAttribute.m_crFocusBK);
			pDC->SetTextColor(m_SkinAttribute.m_crFocusTX);
			return m_SkinAttribute.m_brFocus;
		}
		else
		{
			pDC->SetBkColor(m_SkinAttribute.m_crNoFocusBK);
			pDC->SetTextColor(m_SkinAttribute.m_crNoFocusTX);
			return m_SkinAttribute.m_brNoFocus;
		}
	}

	//����״̬
	pDC->SetBkColor(m_SkinAttribute.m_crDisFocusBK);
	pDC->SetTextColor(m_SkinAttribute.m_crDisFocusBK);

	return m_SkinAttribute.m_brDisbale;
}

//�ػ���Ϣ
VOID CSkinEdit::OnNcPaint()
{
	__super::OnNcPaint();

	//��ȡλ��
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	ScreenToClient(&rcWindow);

	//��ȡ����
	DWORD dwStyle=GetStyle();

	//�滭�߿�
	if ((dwStyle&WS_BORDER|WS_DLGFRAME)!=0)
	{
		//״̬����
		bool bDisable=(IsWindowEnabled()==FALSE);

		//�滭�߿�
		if (bDisable==false)
		{
			//�滭�߿�
			CClientDC ClientDC(this);
			ClientDC.Draw3dRect(&rcWindow,m_SkinAttribute.m_crBoradNormal,m_SkinAttribute.m_crBoradNormal);

			//�滭�ڿ�
			if (m_bFocus==true)
			{
				//������ɫ
				COLORREF crBackGroud=m_SkinAttribute.m_crFocusBK;
				ClientDC.Draw3dRect(rcWindow.left+1,rcWindow.top+1,rcWindow.Width()-2,rcWindow.Height()-2,crBackGroud,crBackGroud);
			}
			else
			{
				//��ͨ��ɫ
				COLORREF crBackGroud=m_SkinAttribute.m_crNoFocusBK;
				ClientDC.Draw3dRect(rcWindow.left+1,rcWindow.top+1,rcWindow.Width()-2,rcWindow.Height()-2,crBackGroud,crBackGroud);
			}
		}
		else
		{
			//�滭�߿�
			CClientDC ClientDC(this);
			ClientDC.Draw3dRect(&rcWindow,m_SkinAttribute.m_crDisFocusBK,m_SkinAttribute.m_crDisFocusBK);

			//�滭�ڿ�
			COLORREF crBackGroud=CSkinDialog::m_SkinAttribute.m_crBackGround;
			ClientDC.Draw3dRect(rcWindow.left+1,rcWindow.top+1,rcWindow.Width()-2,rcWindow.Height()-2,crBackGroud,crBackGroud);
		}
	}

	return;
}

//�õ�����
VOID CSkinEdit::OnSetFocus(CWnd * pOldWnd)
{
	__super::OnSetFocus(pOldWnd);

	//���ñ���
	m_bFocus=true;

	//�ػ�����
	Invalidate(TRUE);

	return;
}

//ʧȥ����
VOID CSkinEdit::OnKillFocus(CWnd * pNewWnd)
{
	__super::OnKillFocus(pNewWnd);

	//���ñ���
	m_bFocus=false;

	//�ػ�����
	Invalidate(TRUE);

	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CSkinEditEx::CSkinEditEx()
{
	//���ñ���
	m_crFocusTX=m_SkinAttribute.m_crFocusTX;
	m_crFocusBK=m_SkinAttribute.m_crFocusBK;
	m_crNoFocusTX=m_SkinAttribute.m_crNoFocusTX;
	m_crNoFocusBK=m_SkinAttribute.m_crNoFocusBK;
	m_crDisFocusTX=m_SkinAttribute.m_crDisFocusTX;
	m_crDisFocusBK=m_SkinAttribute.m_crDisFocusBK;

	//������ˢ
	m_brFocus.DeleteObject();
	m_brNoFocus.DeleteObject();
	m_brDisbale.DeleteObject();
	m_brFocus.CreateSolidBrush(m_crFocusBK);
	m_brNoFocus.CreateSolidBrush(m_crNoFocusBK);
	m_brDisbale.CreateSolidBrush(m_crDisFocusBK);

	return;
}

//��������
CSkinEditEx::~CSkinEditEx()
{
}

//������ɫ
VOID CSkinEditEx::SetFocusColor(COLORREF crFocusTX, COLORREF crFocusBK)
{
	//������ɫ
	m_crFocusTX=crFocusTX;
	m_crFocusBK=crFocusBK;

	//������ˢ
	m_brFocus.DeleteObject();
	m_brFocus.CreateSolidBrush(m_crFocusBK);

	//�滭����
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return;
}

//������ɫ
VOID CSkinEditEx::SetNoFocusColor(COLORREF crNoFocusTX, COLORREF crNoFocusBK)
{
	//������ɫ
	m_crNoFocusTX=crNoFocusTX;
	m_crNoFocusBK=crNoFocusBK;

	//������ˢ
	m_brNoFocus.DeleteObject();
	m_brNoFocus.CreateSolidBrush(m_crNoFocusBK);

	//�滭����
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return;
}

//������ɫ
VOID CSkinEditEx::SetDisFocusColor(COLORREF crDisFocusTX, COLORREF crDisFocusBK)
{
	//������ɫ
	m_crDisFocusTX=crDisFocusTX;
	m_crDisFocusBK=crDisFocusBK;

	//������ˢ
	m_brDisbale.DeleteObject();
	m_brDisbale.CreateSolidBrush(m_crDisFocusBK);

	//�滭����
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return;
}

//�ػ���Ϣ
VOID CSkinEditEx::OnNcPaint()
{
	//�滭�ؼ�
	CEdit::OnNcPaint();

	return;
}

//�滭�ؼ�
HBRUSH CSkinEditEx::CtlColor(CDC * pDC, UINT nCtlColor)
{
	//����״̬
	if (IsWindowEnabled())
	{
		if (m_bFocus==true) 
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetBkColor(m_crFocusBK);
			pDC->SetTextColor(m_crFocusTX);
			return m_brFocus;
		}
		else
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetBkColor(m_crNoFocusBK);
			pDC->SetTextColor(m_crNoFocusTX);
			return m_brNoFocus;
		}
	}

	//����״̬
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(m_crDisFocusBK);
	pDC->SetTextColor(m_crDisFocusBK);
	return m_brDisbale;
}

//////////////////////////////////////////////////////////////////////////
