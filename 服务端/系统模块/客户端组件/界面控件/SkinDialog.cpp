#include "Stdafx.h"
#include "SkinDialog.h"
#include ".\skindialog.h"

#pragma warning (disable : 4302)

//////////////////////////////////////////////////////////////////////////
//��̬����

CSkinDialogAttribute				CSkinDialog::m_SkinAttribute;		//��Դ����
CSkinLayeredMap						CSkinLayered::m_SkinLayeredMap;		//�ֲ��б�

//////////////////////////////////////////////////////////////////////////////////

//��������
const TCHAR * strWndClassName = TEXT("PerrySkinWnd");

//////////////////////////////////////////////////////////////////////////////////

//ö�ٽṹ
struct tagEnumChildInfo
{
	//CWnd *							pWndControl;						//���ƴ���
	HWND							hWndControl;						//���ھ��
	CWnd *							pWndLayered;						//�ֲ㴰��
	CRgn *							pRegionResult;						//�������
};

//////////////////////////////////////////////////////////////////////////
//�궨��

//��ť��ʶ
#define BST_CLOSE					0									//�رհ�ť
#define BST_MAX_SIZE				1									//��ԭ��ť
#define BST_MIN_SIZE				2									//��С��ť

//��ť״̬
#define BUT_EXIST					0x01								//��ť����
#define BUT_NORMAL					0x02								//��ť����
#define BUT_DISABLE					0x04								//��ť��ֹ

#define ICON_WIDTH					20									//ICON���
#define ICON_HEIGHT					20									//ICON�߶�

#define X_BORDER					2									//�߿���
#define Y_BORDER					2									//�߿�߶�

//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSkinLayered, CWnd)
	ON_WM_CLOSE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CSkinDialog, CDialog)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_WM_ACTIVATE()
	ON_WM_ERASEBKGND()
	ON_WM_ACTIVATEAPP()
	ON_WM_NCHITTEST()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETTINGCHANGE()
	ON_WM_WINDOWPOSCHANGED()
	ON_MESSAGE(WM_SETTEXT,OnSetTextMesage)
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CSkinDialogAttribute::CSkinDialogAttribute()
{
	//������Ϣ
	m_bShowIcon=false;
	m_bShowTitle=true;
	m_hDefaultIcon=NULL;

	//ƫ����Ϣ
	m_nXIconExcursion=0;
	m_nYIconExcursion=0;
	m_nXButtonExcursion=0;
	m_nYButtonExcursion=0;
	m_nXTitleTextExcursion=0;
	m_nYTitleTextExcursion=0;

	//�߿���Ϣ
	m_crTitleText=RGB(0,0,0);
	m_crBackGround=RGB(0,0,0);
	m_crControlText=RGB(0,0,0);

	//��������
	m_nButtonWidth=0;
	m_nButtonHeigth=0;

	//�߿����
	m_nXBorder=0;
	m_nYBorder=0;
	m_nCaptionHeigth=0;

	return;
}
	
//��������
CSkinDialogAttribute::~CSkinDialogAttribute()
{
}

//������Դ
bool CSkinDialogAttribute::UpdateSkinResource(ISkinResource * pISkinResource)
{
	//��ȡ��Ϣ
	tagSkinDialogResource SkinDialogResource;
	HINSTANCE hResResource=pISkinResource->GetResInstance();

	//��ȡ��Դ
	if (pISkinResource!=NULL) pISkinResource->GetDialogResource(SkinDialogResource);

	//������Ϣ
	m_bShowIcon=SkinDialogResource.bShowIcon;
	m_bShowTitle=SkinDialogResource.bShowTitle;
	m_hDefaultIcon=SkinDialogResource.hDefaultIcon;

	//ƫ����Ϣ
	m_nXIconExcursion=SkinDialogResource.nXIconExcursion;
	m_nYIconExcursion=SkinDialogResource.nYIconExcursion;
	m_nXButtonExcursion=SkinDialogResource.nXButtonExcursion;
	m_nYButtonExcursion=SkinDialogResource.nYButtonExcursion;
	m_nXTitleTextExcursion=SkinDialogResource.nXTitleTextExcursion;
	m_nYTitleTextExcursion=SkinDialogResource.nYTitleTextExcursion;

	//��ɫ��Ϣ
	m_crTitleText=SkinDialogResource.crTitleText;
	m_crBackGround=SkinDialogResource.crBackGround;
	m_crControlText=SkinDialogResource.crControlText;

	//������Ϣ
	m_SkinImageTL.SetLoadInfo(SkinDialogResource.uImageTL,hResResource);
	m_SkinImageTM.SetLoadInfo(SkinDialogResource.uImageTM,hResResource);
	m_SkinImageTR.SetLoadInfo(SkinDialogResource.uImageTR,hResResource);
	m_SkinImageMR.SetLoadInfo(SkinDialogResource.uImageMR,hResResource);
	m_SkinImageML.SetLoadInfo(SkinDialogResource.uImageML,hResResource);
	m_SkinImageBL.SetLoadInfo(SkinDialogResource.uImageBL,hResResource);
	m_SkinImageBM.SetLoadInfo(SkinDialogResource.uImageBM,hResResource);
	m_SkinImageBR.SetLoadInfo(SkinDialogResource.uImageBR,hResResource);

	//��ť��Ϣ
	m_ButtonMin.SetLoadInfo(SkinDialogResource.uButtonMin,hResResource);
	m_ButtonMax.SetLoadInfo(SkinDialogResource.uButtonMax,hResResource);
	m_ButtonRes.SetLoadInfo(SkinDialogResource.uButtonRes,hResResource);
	m_ButtonClose.SetLoadInfo(SkinDialogResource.uButtonClose,hResResource);

	//������ˢ
	m_brBackGround.DeleteObject();
	m_brBackGround.CreateSolidBrush(m_crBackGround);

	//������Դ
	CImageHandle ButtonHandle(&m_ButtonClose);
	CImageHandle SkinImageTMHandle(&m_SkinImageTM);
	CImageHandle SkinImageMLHandle(&m_SkinImageML);
	CImageHandle SkinImageBMHandle(&m_SkinImageBM);

	//�����С
	m_nXBorder=m_SkinImageML.GetWidth();
	m_nYBorder=m_SkinImageBM.GetHeight();
	m_nButtonWidth=m_ButtonClose.GetWidth()/4;
	m_nButtonHeigth=m_ButtonClose.GetHeight();
	m_nCaptionHeigth=m_SkinImageTM.GetHeight();

	return true;
}


//////////////////////////////////////////////////////////////////////////////////

//���캯��
CSkinLayered::CSkinLayered()
{	
	//���ñ���	
	m_hwndControl=NULL;
	m_OriParentProc=NULL;

	return;
}

//��������
CSkinLayered::~CSkinLayered()
{
}

//��������
bool CSkinLayered::CreateLayered(HWND hWnd)
{
	//Ч�����
	ASSERT(hWnd!=NULL);
	if(hWnd==NULL) return false;

	//���ñ���
	m_hwndControl=hWnd;

	//���ñ���
	HINSTANCE hInstance=AfxGetInstanceHandle();

	//��������
	WNDCLASSEX ShadowClass;
	memset(&ShadowClass, 0, sizeof(ShadowClass));

	//��������
	ShadowClass.cbSize = sizeof(WNDCLASSEX); 
	ShadowClass.style  = CS_HREDRAW | CS_VREDRAW;
	ShadowClass.lpfnWndProc	= ::DefWindowProc;
	ShadowClass.cbClsExtra = 0;
	ShadowClass.cbWndExtra = 0;
	ShadowClass.hInstance = hInstance;
	ShadowClass.hIcon = NULL;
	ShadowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	ShadowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	ShadowClass.lpszMenuName = NULL;
	ShadowClass.lpszClassName = strWndClassName;
	ShadowClass.hIconSm = NULL;

	//ע�ᴰ����
	RegisterClassEx(&ShadowClass);

	//��ȡλ��
	CRect rcWindow;	
	::GetWindowRect(m_hwndControl,&rcWindow);

	//��������
	CreateEx(WS_EX_LAYERED|WS_EX_TRANSPARENT,strWndClassName,NULL,WS_POPUPWINDOW,rcWindow.left,rcWindow.top,rcWindow.Width(),rcWindow.Height(),m_hwndControl, NULL);

	//��ȡ���ڹ���
	m_OriParentProc = GetWindowLongPtr(m_hwndControl, GWLP_WNDPROC);

	//�����´��ڹ���
	SetWindowLongPtr(m_hwndControl, GWLP_WNDPROC, (LONG_PTR)ParentProc);

	//���ñ���
	m_SkinLayeredMap[m_hwndControl]=this;

	return true;
}

//��������
VOID CSkinLayered::InitLayeredArea(CDC * pDCImage, BYTE cbAlpha, CRect & rcUnLayered, CPoint & PointRound, bool bUnLayeredChild)
{
	//Ч�����
	ASSERT((pDCImage!=NULL)&&(pDCImage->m_hDC!=NULL));
	if ((pDCImage==NULL)||(pDCImage->m_hDC==NULL)) return;

	//��������
	BITMAP Bitmap;
	ZeroMemory(&Bitmap,sizeof(Bitmap));

	//��ȡͼ��
	CBitmap * pBitmap=pDCImage->GetCurrentBitmap();
	if (pBitmap!=NULL) pBitmap->GetBitmap(&Bitmap);

	//��ȡ��С
	CSize SizeImage;
	SizeImage.SetSize(Bitmap.bmWidth,Bitmap.bmHeight);

	//Ч���С
	ASSERT((SizeImage.cx>0)&&(SizeImage.cy>0));
	if ((SizeImage.cx==0)||(SizeImage.cy==0)) return;

	//��������
	BLENDFUNCTION BlendFunction;
	ZeroMemory(&BlendFunction,sizeof(BlendFunction));

	//���ò���
	BlendFunction.BlendOp=0;
	BlendFunction.BlendFlags=0;
	BlendFunction.AlphaFormat=AC_SRC_ALPHA;
	BlendFunction.SourceConstantAlpha=cbAlpha;

	//���÷ֲ�
	CPoint ImagePoint(0,0);
	CClientDC ClientDC(this);
	UpdateLayeredWindow(&ClientDC,NULL,&SizeImage,pDCImage,&ImagePoint,0L,&BlendFunction,ULW_ALPHA);

	//��������
	CRgn RegionResult;
	RegionResult.CreateRectRgn(0,0,SizeImage.cx,SizeImage.cy);

	//�����ų�
	if (bUnLayeredChild==true)
	{
		//��������
		tagEnumChildInfo EnumChildInfo;
		ZeroMemory(&EnumChildInfo,sizeof(EnumChildInfo));

		//���ñ���
		EnumChildInfo.pWndLayered=this;
		EnumChildInfo.hWndControl=m_hwndControl;
		EnumChildInfo.pRegionResult=&RegionResult;

		//ö�ٴ���
		ASSERT(m_hwndControl!=NULL);
		EnumChildWindows(m_hwndControl,EnumChildProc,(LPARAM)&EnumChildInfo);
	}

	//�����ų�
	if (rcUnLayered.IsRectEmpty()==FALSE)
	{
		//��������
		CRgn RegionUnLayered;
		RegionUnLayered.CreateRoundRectRgn(rcUnLayered.left,rcUnLayered.top,rcUnLayered.right+1,rcUnLayered.bottom+1,PointRound.x,PointRound.y);

		//�ϲ�����
		RegionResult.CombineRgn(&RegionResult,&RegionUnLayered,RGN_DIFF);
	}

	//��������
	SetWindowRgn(RegionResult,TRUE);

	return;
}

//��������
VOID CSkinLayered::InitLayeredArea(CPngImageWarpper & Image, BYTE cbAlpha, CRect & rcUnLayered, CPoint & PointRound, bool bUnLayeredChild)
{
	//��������
	CImage ImageBuffer;
	ImageBuffer.Create(Image.GetWidth(),Image.GetHeight(),32);

	//�滭����
	CImageDC ImageDC(ImageBuffer);
	CDC * pBufferDC=CDC::FromHandle(ImageDC);

	//�滭����
	ASSERT(pBufferDC!=NULL);
	if (pBufferDC!=NULL) Image.DrawImage(pBufferDC,0,0);

	//�����ֲ�
	InitLayeredArea(pBufferDC,cbAlpha,rcUnLayered,PointRound,bUnLayeredChild);

	return;
}

//�ر���Ϣ
VOID CSkinLayered::OnClose()
{
	if(m_hwndControl!=NULL)
	{
		::PostMessage(m_hwndControl,WM_CLOSE,0,0);
	}
}

//���ý���
VOID CSkinLayered::OnSetFocus(CWnd* pOldWnd)
{
	if(m_hwndControl!=NULL)
	{
		::SetFocus(m_hwndControl);
	}
}

//ö�ٺ���
BOOL CALLBACK CSkinLayered::EnumChildProc(HWND hWndChild, LPARAM lParam)
{
	//��ȡλ��
	CRect rcWindow;
	::GetWindowRect(hWndChild,&rcWindow);

	//��������
	if ((rcWindow.Width()>0)&&(rcWindow.Height()>0))
	{
		//��������
		ASSERT(lParam!=0L);
		tagEnumChildInfo * pEnumChildInfo=(tagEnumChildInfo *)lParam;

		//�����ж�
		HWND hWndParent=::GetParent(hWndChild);
		if (hWndParent!=pEnumChildInfo->hWndControl)
		{
			return TRUE;
		}

		//ת��λ��
		ASSERT(pEnumChildInfo->hWndControl!=NULL);
		CWnd::FromHandle(pEnumChildInfo->hWndControl)->ScreenToClient(&rcWindow);

		//��������
		CRgn RgnChild;
		RgnChild.CreateRectRgn(rcWindow.left,rcWindow.top,rcWindow.right,rcWindow.bottom);

		//�ϲ�����
		ASSERT(pEnumChildInfo->pRegionResult!=NULL);
		pEnumChildInfo->pRegionResult->CombineRgn(pEnumChildInfo->pRegionResult,&RgnChild,RGN_DIFF);
	}

	return TRUE;
}

//���ڹ���
LRESULT CALLBACK CSkinLayered::ParentProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//���Ҷ���
	CSkinLayered * pSkinLayered=NULL;
	m_SkinLayeredMap.Lookup(hwnd,pSkinLayered);
	if(pSkinLayered==NULL || pSkinLayered->m_hwndControl!=hwnd) return 1L;

	//��ȡ���ڹ���
	WNDPROC pDefaultProc = (WNDPROC)pSkinLayered->m_OriParentProc;

	switch(uMsg)
	{
	case WM_MOVE:
		{
			if(pSkinLayered->IsWindowVisible()==TRUE)
			{
				CRect ParentRect;
				::GetWindowRect(hwnd,&ParentRect);
				pSkinLayered->SetWindowPos(NULL,ParentRect.left,ParentRect.top,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
			}
		}
		break;
	case WM_WINDOWPOSCHANGED: 
		{
			LRESULT lResult = pDefaultProc(hwnd, uMsg, wParam, lParam);
			if(pSkinLayered->m_hWnd != NULL)
			{
				WINDOWPOS * lpWndPos=(WINDOWPOS *)lParam;
				if((lpWndPos->cx>0)&&(lpWndPos->cy>0))
				{
					pSkinLayered->SetWindowPos(NULL,lpWndPos->x,lpWndPos->y,lpWndPos->cx,lpWndPos->cy,SWP_NOZORDER|SWP_NOACTIVATE);
				}
			}
			return lResult;
		}
		break;	
	case WM_SIZE:
		{
			if(pSkinLayered->IsWindowEnabled()==TRUE)
			{
				if(wParam==SIZE_MINIMIZED)
				{
					pSkinLayered->ShowWindow(SW_HIDE);
				}
				else
				{
					LONG lParentStyle = GetWindowLong(hwnd, GWL_STYLE);
					if(WS_VISIBLE & lParentStyle) pSkinLayered->ShowWindow(SW_SHOW);
				}
			}
		}
		break;
	case WM_SHOWWINDOW:
		{
			if(pSkinLayered->IsWindowEnabled()==TRUE)
			{
				LRESULT lResult = pDefaultProc(hwnd, uMsg, wParam, lParam);
				pSkinLayered->ShowWindow(wParam?SW_SHOW:SW_HIDE);

				return lResult;
			}
		}
		break;
	case WM_NCDESTROY:
		{
			m_SkinLayeredMap.RemoveKey(hwnd);	
		}
		break;
	}

	return pDefaultProc(hwnd, uMsg, wParam, lParam);
}


//////////////////////////////////////////////////////////////////////////

//���캯��
CSkinDialog::CSkinDialog(UINT nIDTemplate, CWnd * pParentWnd) : CDialog(nIDTemplate,pParentWnd)
{
	//����״̬
	m_bActive=false;
	m_bMaxShow=false;

	//��ť״̬
	m_cbNowHotButton=255;
	m_cbNowDownButton=255;
	ZeroMemory(m_cbButtonState,sizeof(m_cbButtonState));

	return;
}

//��������
CSkinDialog::~CSkinDialog()
{
}

//��ʼ����Ϣ
BOOL CSkinDialog::OnInitDialog()
{
	__super::OnInitDialog();

	//���ñ���
	m_bActive=false;
	m_bMaxShow=false;

	//��ť״̬
	m_cbNowHotButton=255;
	m_cbNowDownButton=255;
	ZeroMemory(m_cbButtonState,sizeof(m_cbButtonState));

	//��������
	LONG lWindowStyte=GetWindowLongPtr(m_hWnd,GWL_STYLE);
	if (lWindowStyte&WS_SYSMENU)
	{
		m_cbButtonState[BST_CLOSE]=BUT_EXIST|BUT_NORMAL;
		if ((lWindowStyte&WS_MAXIMIZEBOX)||(lWindowStyte&WS_MINIMIZEBOX))
		{
			m_cbButtonState[BST_MAX_SIZE]=BUT_EXIST|((lWindowStyte&WS_MAXIMIZEBOX)?BUT_NORMAL:BUT_DISABLE);
			m_cbButtonState[BST_MIN_SIZE]=BUT_EXIST|((lWindowStyte&WS_MINIMIZEBOX)?BUT_NORMAL:BUT_DISABLE);
		}
	}

	//��������
	ModifyStyle(WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU|WS_TILED,0,WS_CLIPCHILDREN|WS_CLIPSIBLINGS);

	//����λ��
	CRect rcClient;
	GetClientRect(&rcClient);
	GetWindowRect(&m_rcNormalSize);
	AdjustTitleButton(rcClient.Width(),rcClient.Height());

	//���ñ���
	SetClassLongPtr(m_hWnd,GCLP_HBRBACKGROUND,NULL);

	return TRUE;
}

//��󴰿�
VOID CSkinDialog::MaxSizeWindow()
{
	//״̬�ж�
	if (m_bMaxShow==true) return;

	//����λ��
	CRect rcClient;
	GetClientRect(&rcClient);
	ClientToScreen(&rcClient);
	GetWindowRect(&m_rcNormalSize);

	//����λ��
	CRect rcRect;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcRect,SPIF_SENDCHANGE);
	rcRect.top-=(rcClient.top-m_rcNormalSize.top);
	rcRect.left-=(rcClient.left-m_rcNormalSize.left);
	rcRect.right+=(m_rcNormalSize.right-rcClient.right);
	rcRect.bottom+=(m_rcNormalSize.bottom-rcClient.bottom);

	//�ƶ�����
	m_bMaxShow=true;
	SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOCOPYBITS);

	return;
}

//��ԭ����
VOID CSkinDialog::RestoreWindow()
{
	//״̬�ж�
	if (m_bMaxShow==false) return;

	//�ƶ�����
	m_bMaxShow=false;
	SetWindowPos(NULL,m_rcNormalSize.left,m_rcNormalSize.top,m_rcNormalSize.Width(),m_rcNormalSize.Height(),SWP_NOZORDER|SWP_NOCOPYBITS);

	return;
}

//��ť����
VOID CSkinDialog::EnableButton(UINT uButtonStyle)
{
	//���ð�ť
	if ((uButtonStyle&WS_MAXIMIZEBOX)||(uButtonStyle&WS_MINIMIZEBOX))
	{
		m_cbButtonState[BST_MAX_SIZE]=BUT_EXIST|((uButtonStyle&WS_MAXIMIZEBOX)?BUT_NORMAL:BUT_DISABLE);
		m_cbButtonState[BST_MIN_SIZE]=BUT_EXIST|((uButtonStyle&WS_MINIMIZEBOX)?BUT_NORMAL:BUT_DISABLE);
	}
	else
	{
		m_cbButtonState[BST_MAX_SIZE]=0;
		m_cbButtonState[BST_MIN_SIZE]=0;
	}

	//���½���
	if (m_hWnd!=NULL) Invalidate();

	return;
}

//���±���
VOID CSkinDialog::UpdateCaption()
{
	//��ȡ����
	CRect rcClient;
	GetClientRect(&rcClient);

	//״̬�ж�
	if (rcClient.IsRectEmpty()) return;

	//�滭����
	CClientDC ClientDC(this);
	DrawCaption(&ClientDC);

	return;
}

//������ť
VOID CSkinDialog::AdjustTitleButton(INT nWidth, INT nHeight)
{
	//������ť
	for (INT i=0;i<CountArray(m_rcButton);i++)
	{
		//����ƫ��
		INT nYPos=m_SkinAttribute.m_nYButtonExcursion;
		INT nXPos=nWidth-m_SkinAttribute.m_nButtonWidth*(i+1)-m_SkinAttribute.m_nXButtonExcursion;

		//����λ��
		m_rcButton[i].top=nYPos;
		m_rcButton[i].left=nXPos;
		m_rcButton[i].right=nXPos+m_SkinAttribute.m_nButtonWidth;
		m_rcButton[i].bottom=nYPos+m_SkinAttribute.m_nButtonHeigth;
	}

	return;
}

//�滭�߿�
VOID CSkinDialog::DrawBorder(CDC * pDC)
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);
	if (rcClient.IsRectEmpty()) return;

	//������Դ
	CImageHandle ImageHandleMR(&m_SkinAttribute.m_SkinImageMR);
	CImageHandle ImageHandleML(&m_SkinAttribute.m_SkinImageML);
	CImageHandle ImageHandleBL(&m_SkinAttribute.m_SkinImageBL);
	CImageHandle ImageHandleBM(&m_SkinAttribute.m_SkinImageBM);
	CImageHandle ImageHandleBR(&m_SkinAttribute.m_SkinImageBR);

	//�߿���
	INT nXDrawPos,nYDrawPos,nDrawStep;

	//�滭����
	nDrawStep=ImageHandleMR->GetHeight();
	nXDrawPos=rcClient.right-m_SkinAttribute.m_nXBorder;
	for (nYDrawPos=m_SkinAttribute.m_nCaptionHeigth;nYDrawPos<(rcClient.bottom-m_SkinAttribute.m_nYBorder);nYDrawPos+=nDrawStep)
	{
		ImageHandleML->BitBlt(pDC->m_hDC,0,nYDrawPos);
		ImageHandleMR->BitBlt(pDC->m_hDC,nXDrawPos,nYDrawPos);
	}

	//�滭�±�
	nDrawStep=ImageHandleBM->GetWidth();
	nYDrawPos=rcClient.bottom-m_SkinAttribute.m_nYBorder;
	ImageHandleBL->BitBlt(pDC->m_hDC,0,rcClient.bottom-ImageHandleBL->GetHeight());
	for (nXDrawPos=ImageHandleBL->GetWidth();nXDrawPos<rcClient.right-m_SkinAttribute.m_nXBorder;nXDrawPos+=nDrawStep)
	{
		ImageHandleBM->BitBlt(pDC->m_hDC,nXDrawPos,nYDrawPos);
	}
	ImageHandleBR->BitBlt(pDC->m_hDC,rcClient.right-ImageHandleBR->GetWidth(),rcClient.bottom-ImageHandleBR->GetHeight());
	
	return;
}

//�滭����
VOID CSkinDialog::DrawCaption(CDC * pDC)
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);
	if (rcClient.IsRectEmpty()) return;

	//������Դ
	CImageHandle ImageHandleTL(&m_SkinAttribute.m_SkinImageTL);
	CImageHandle ImageHandleTM(&m_SkinAttribute.m_SkinImageTM);
	CImageHandle ImageHandleTR(&m_SkinAttribute.m_SkinImageTR);

	//��������
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),m_SkinAttribute.m_nCaptionHeigth,16);

	//�滭����
	CDC BufferDC;
	BufferDC.CreateCompatibleDC(pDC);
	BufferDC.SelectObject(ImageBuffer);

	//��ȡ���
	INT nMidWidth=m_SkinAttribute.m_SkinImageTM.GetWidth();
	INT nLeftWidth=m_SkinAttribute.m_SkinImageTL.GetWidth();
	INT nRightWidth=m_SkinAttribute.m_SkinImageTR.GetWidth();

	//�滭����
	ImageHandleTL->BitBlt(BufferDC,0,0);
	for (INT i=nLeftWidth;i<(rcClient.right-nRightWidth);i+=nMidWidth) ImageHandleTM->BitBlt(BufferDC,i,0);
	ImageHandleTR->BitBlt(BufferDC,rcClient.right-nRightWidth,0);

	//�滭ͼ��
	if (m_SkinAttribute.m_bShowIcon==true)
	{
		//����ͼ��
		HICON hIcon=GetIcon(FALSE);
		if (hIcon==NULL) hIcon=m_SkinAttribute.m_hDefaultIcon;

		//�滭ͼ��
		if (hIcon!=NULL)
		{
			INT nXPos=m_SkinAttribute.m_nXIconExcursion;
			INT nYPos=m_SkinAttribute.m_nYIconExcursion;
			DrawIconEx(BufferDC,nXPos,nYPos,hIcon,ICON_WIDTH,ICON_HEIGHT,0,NULL,DI_NORMAL);
		}
	}
	
	//�滭����
	if ((m_SkinAttribute.m_bShowTitle==true)&&(m_ImageTitle.IsNull()==true))
	{
		//��ȡ����
		CString strTitle;
		GetWindowText(strTitle);

		//��������
		CRect rcTitle;
		rcTitle.right=rcClient.right-nRightWidth;
		rcTitle.bottom=m_SkinAttribute.m_nCaptionHeigth;
		rcTitle.top=m_SkinAttribute.m_nYTitleTextExcursion;
		rcTitle.left=m_SkinAttribute.m_nXTitleTextExcursion;

		//�滭����
		BufferDC.SetBkMode(TRANSPARENT);
		BufferDC.SetTextColor(m_SkinAttribute.m_crTitleText);
		BufferDC.SelectObject(CSkinResourceManager::GetDefaultFont());
		BufferDC.DrawText(strTitle,&rcTitle,DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS);
	}

	//�滭����
	if (m_ImageTitle.IsNull()==false)
	{
		INT nYPos=(CY_DEF_FONT-m_ImageTitle.GetHeight())/2;
		m_ImageTitle.DrawImage(&BufferDC,m_SkinAttribute.m_nXTitleTextExcursion,m_SkinAttribute.m_nYTitleTextExcursion+nYPos);
	}

	//�رհ�ť
	if (m_cbButtonState[BST_CLOSE]&BUT_EXIST)
	{
		INT nDrawPos=0;
		CImageHandle ImageHandle(&m_SkinAttribute.m_ButtonClose);
		if (m_cbButtonState[BST_CLOSE]&BUT_DISABLE) nDrawPos=ImageHandle->GetWidth()*3/4;
		else if (m_cbNowDownButton==BST_CLOSE) nDrawPos=ImageHandle->GetWidth()/4;
		else if (m_cbNowHotButton==BST_CLOSE) nDrawPos=ImageHandle->GetWidth()*2/4;
		ImageHandle->BitBlt(BufferDC,m_rcButton[BST_CLOSE].left,m_rcButton[BST_CLOSE].top,
			m_SkinAttribute.m_nButtonWidth,m_SkinAttribute.m_nButtonHeigth,nDrawPos,0);
	}

	//��󻯺ͻ�ԭ��ť
	if (m_cbButtonState[BST_MAX_SIZE]&BUT_EXIST)
	{
		INT nDrawPos=0;
		CImageHandle ImageHandle;
		if (m_bMaxShow==true) ImageHandle.AttachResource(&m_SkinAttribute.m_ButtonRes);
		else ImageHandle.AttachResource(&m_SkinAttribute.m_ButtonMax);
		if (m_cbButtonState[BST_MAX_SIZE]&BUT_DISABLE) nDrawPos=ImageHandle->GetWidth()*3/4;
		if (m_cbNowDownButton==BST_MAX_SIZE) nDrawPos=ImageHandle->GetWidth()/4;
		else if (m_cbNowHotButton==BST_MAX_SIZE) nDrawPos=ImageHandle->GetWidth()*2/4;
		ImageHandle->BitBlt(BufferDC,m_rcButton[BST_MAX_SIZE].left,m_rcButton[BST_MAX_SIZE].top,
			m_SkinAttribute.m_nButtonWidth,m_SkinAttribute.m_nButtonHeigth,nDrawPos,0);
	}

	//��С����ť
	if (m_cbButtonState[BST_MIN_SIZE]&BUT_EXIST)	
	{
		INT nDrawPos=0;
		CImageHandle ImageHandle(&m_SkinAttribute.m_ButtonMin);
		if (m_cbButtonState[BST_MIN_SIZE]&BUT_DISABLE) nDrawPos=ImageHandle->GetWidth()*3/4;
		if (m_cbNowDownButton==BST_MIN_SIZE) nDrawPos=ImageHandle->GetWidth()/4;
		else if (m_cbNowHotButton==BST_MIN_SIZE) nDrawPos=ImageHandle->GetWidth()*2/4;
		ImageHandle->BitBlt(BufferDC,m_rcButton[BST_MIN_SIZE].left,m_rcButton[BST_MIN_SIZE].top,
			m_SkinAttribute.m_nButtonWidth,m_SkinAttribute.m_nButtonHeigth,nDrawPos,0);
	}

	//�滭����
	pDC->BitBlt(0,0,rcClient.Width(),m_SkinAttribute.m_nCaptionHeigth,&BufferDC,0,0,SRCCOPY);

	//������Դ
	BufferDC.DeleteDC();
	ImageBuffer.Destroy();

	return;	
}

//�滭����
VOID CSkinDialog::DrawBackGround(CDC * pDC)
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);
	if (rcClient.IsRectEmpty()) return;

	//��䱳��
	pDC->FillSolidRect(rcClient.left+m_SkinAttribute.m_nXBorder,m_SkinAttribute.m_nCaptionHeigth,
		rcClient.right-m_SkinAttribute.m_nXBorder*2,rcClient.bottom-m_SkinAttribute.m_nCaptionHeigth-m_SkinAttribute.m_nYBorder,
		m_SkinAttribute.m_crBackGround);

	return;
}

//������Ϣ
VOID CSkinDialog::OnActivate(UINT nState, CWnd * pWndOther, BOOL bMinimized)
{
	__super::OnActivate(nState,pWndOther,bMinimized);

	//���ñ���
	m_bActive=(nState!=WA_INACTIVE);

	return;
}

//������Ϣ
VOID CSkinDialog::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	__super::OnActivateApp(bActive,dwThreadID);

	//���ñ���
	m_bActive=bActive?true:false;

	return;
}

//�����Ϣ
VOID CSkinDialog::OnLButtonDblClk(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDblClk(nFlags,Point);

	//�жϰ���
	if (m_cbNowDownButton!=255) return;	

	//��ť����
	for (INT i=0;i<CountArray(m_cbButtonState);i++)
	{
		if ((m_cbButtonState[i]&BUT_EXIST)&&(m_rcButton[i].PtInRect(Point))) return;
	}

	//״̬�ж�
	if (Point.y>m_SkinAttribute.m_nCaptionHeigth) return;
	if (((m_cbButtonState[BST_MAX_SIZE]&BUT_EXIST)==0)||(m_cbButtonState[BST_MAX_SIZE]&BUT_DISABLE)!=0) return;

	//���ƴ���
	if (m_bMaxShow==true) RestoreWindow();
	else MaxSizeWindow();
	
	return;
}

//�����Ϣ
VOID CSkinDialog::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//�жϰ���
	if (m_cbNowDownButton!=255) return;

	//��ť����
	for (INT i=0;i<CountArray(m_rcButton);i++)
	{
		if ((m_cbButtonState[i]&BUT_EXIST)&&((m_cbButtonState[i]&BUT_DISABLE)==0)&&(m_rcButton[i].PtInRect(Point)))
		{
			//���ñ���
			m_cbNowDownButton=i;

			//���½���
			SetCapture();
			UpdateCaption();

			return;
		}
	}

	//ģ�ⰴ����
	if ((m_bMaxShow==false)&&(Point.y<m_SkinAttribute.m_nCaptionHeigth))
	{
		PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));
	}

	return;
}

//�����Ϣ
VOID CSkinDialog::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags,Point);

	//״̬�ж�
	if (m_cbNowDownButton==255) return;

	//�����ͷ�
	ReleaseCapture();

	//��ť�ж�
	if ((m_rcButton[m_cbNowDownButton].PtInRect(Point))&&((m_cbButtonState[m_cbNowDownButton]&BUT_DISABLE)==0))
	{
		switch (m_cbNowDownButton)
		{
		case BST_CLOSE:		//�رհ�ť
			{
				m_cbNowDownButton=255;
				PostMessage(WM_CLOSE,0,0);
				return;
			}
		case BST_MAX_SIZE:		//��ԭ��ť
			{
				m_cbNowDownButton=255;
				if (m_bMaxShow==true) RestoreWindow();
				else MaxSizeWindow();
				return;
			}
		case BST_MIN_SIZE:		//��С����ť
			{
				m_cbNowDownButton=255;
				ShowWindow(SW_MINIMIZE);
				return;
			}
		}
	}

	//��������
	m_cbNowDownButton=255;

	//���±���
	UpdateCaption();

	return;
}

//�����Ϣ
VOID CSkinDialog::OnMouseMove(UINT nFlags, CPoint Point)
{
	__super::OnMouseMove(nFlags,Point);

	//���ù��
	SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW)));

	//�жϰ���
	if (m_cbNowDownButton!=255) return;
	
	//��ť����
	for (INT i=0;i<CountArray(m_rcButton);i++)
	{
		if ((m_cbButtonState[i]&BUT_EXIST)&&(m_rcButton[i].PtInRect(Point)))
		{
			if (m_cbNowHotButton==i) return;
			if (m_cbNowHotButton!=255)
			{
				m_cbButtonState[m_cbNowHotButton]|=BUT_NORMAL;
				m_cbNowHotButton=255;
			}
			if ((m_cbButtonState[i]&BUT_EXIST)&&(!(m_cbButtonState[i]&BUT_DISABLE)))
			{
				m_cbNowHotButton=i;
				m_cbButtonState[m_cbNowHotButton]&=~BUT_NORMAL;
			}

			//���±���
			UpdateCaption();

			return;
		}
	}

	//������
	if (m_cbNowHotButton!=255)
	{
		//���ñ���
		m_cbButtonState[m_cbNowHotButton]|=BUT_NORMAL;
		m_cbNowHotButton=255;

		//���±���
		UpdateCaption();
	}

	return;
}

//λ�øı�
VOID CSkinDialog::OnWindowPosChanged(WINDOWPOS * lpwndpos)
{
	__super::OnWindowPosChanged(lpwndpos);

	//����λ��
	CRect rcClient;
	m_cbNowHotButton=255;
	m_cbNowDownButton=255;
	GetClientRect(&rcClient);
	AdjustTitleButton(rcClient.Width(),rcClient.Height());

	//�ػ�����
	Invalidate();

	return;
}

//���øı�
VOID CSkinDialog::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	__super::OnSettingChange(uFlags,lpszSection);

	//�������
	if (m_bMaxShow==true)
	{
		//����λ��
		CRect rcClient;
		GetClientRect(&rcClient);
		ClientToScreen(&rcClient);
		GetWindowRect(&m_rcNormalSize);

		//����λ��
		CRect rcRect;
		SystemParametersInfo(SPI_GETWORKAREA,0,&rcRect,SPIF_SENDCHANGE);
		rcRect.top-=(rcClient.top-m_rcNormalSize.top);
		rcRect.left-=(rcClient.left-m_rcNormalSize.left);
		rcRect.right+=(m_rcNormalSize.right-rcClient.right);
		rcRect.bottom+=(m_rcNormalSize.bottom-rcClient.bottom);

		//�ƶ�����
		SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOCOPYBITS);
	}

	return;
}

//�滭����
BOOL CSkinDialog::OnEraseBkgnd(CDC * pDC)
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	CDC BufferDC;
	CImage ImageBuffer;
	BufferDC.CreateCompatibleDC(NULL);
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//���û���
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SelectObject(ImageBuffer);
	BufferDC.SelectObject(CSkinResourceManager::GetDefaultFont());

	//�滭����
	DrawBorder(&BufferDC);
	DrawCaption(&BufferDC);
	DrawBackGround(&BufferDC);
	OnDrawClientArea(&BufferDC,rcClient.Width(),rcClient.Height());

	//�滭����
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);

	//������Դ
	BufferDC.DeleteDC();
	ImageBuffer.Destroy();
	return TRUE;
}

//��������
LRESULT CSkinDialog::OnNcHitTest(CPoint Point)
{
	//��ť����
	if ((m_bMaxShow==false)&&(m_cbButtonState[BST_MAX_SIZE]&BUT_EXIST)&&(m_cbButtonState[BST_MAX_SIZE]&BUT_NORMAL))
	{
		//��ȡλ��
		CRect rcClient;
		GetClientRect(&rcClient);

		//ת��λ��
		CPoint ClientPoint=Point;
		ScreenToClient(&ClientPoint);

		//����λ��
		if (ClientPoint.x<=X_BORDER)
		{
			if (ClientPoint.y<=Y_BORDER) return HTTOPLEFT;
			if (ClientPoint.y>=(rcClient.Height()-Y_BORDER)) return HTBOTTOMLEFT;
			return HTLEFT;
		}

		//����λ��
		if (ClientPoint.x>=(rcClient.Width()-X_BORDER))
		{
			if (ClientPoint.y<=Y_BORDER) return HTTOPRIGHT;
			if (ClientPoint.y>=(rcClient.Height()-Y_BORDER)) return HTBOTTOMRIGHT;
			return HTRIGHT;
		}

		//����λ��
		if (ClientPoint.y<=Y_BORDER) 
		{
			if( ( !(m_cbButtonState[BST_CLOSE]&BUT_EXIST) || !m_rcButton[2].PtInRect(ClientPoint) ) &&
				( !(m_cbButtonState[BST_MAX_SIZE]&BUT_EXIST) || !m_rcButton[1].PtInRect(ClientPoint) ) &&
				( !(m_cbButtonState[BST_MIN_SIZE]&BUT_EXIST) || !m_rcButton[0].PtInRect(ClientPoint) ) )
				return HTTOP;
		}
		if (ClientPoint.y>=(rcClient.Height()-Y_BORDER)) return HTBOTTOM;
	}

	return __super::OnNcHitTest(Point);
}

//�ؼ���ɫ
HBRUSH CSkinDialog::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_STATIC:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetBkColor(m_SkinAttribute.m_crBackGround);
			pDC->SetTextColor(m_SkinAttribute.m_crControlText);
			return m_SkinAttribute.m_brBackGround;
		}
	}
	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

//������Ϣ
LRESULT	CSkinDialog::OnSetTextMesage(WPARAM wParam, LPARAM lParam)
{
	//Ĭ�ϵ���
	LRESULT lResult=DefWindowProc(WM_SETTEXT,wParam,lParam);

	//���±���
	UpdateCaption();

	return lResult;
}

//��ȡ��С
VOID CSkinDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	__super::OnGetMinMaxInfo(lpMMI);

	lpMMI->ptMinTrackSize.x = GetXBorder()*2+(m_SkinAttribute.m_bShowIcon?ICON_WIDTH:0);
	lpMMI->ptMinTrackSize.y = GetYBorder()+GetCaptionHeight();

	if( m_cbButtonState[BST_CLOSE] & BUT_EXIST )
	{
		lpMMI->ptMinTrackSize.x += m_SkinAttribute.m_nButtonWidth+m_SkinAttribute.m_nXButtonExcursion;
	}

	if( m_cbButtonState[BST_MAX_SIZE] & BUT_EXIST )
	{
		lpMMI->ptMinTrackSize.x += m_SkinAttribute.m_nButtonWidth+m_SkinAttribute.m_nXButtonExcursion;
	}

	if( m_cbButtonState[BST_MIN_SIZE] & BUT_EXIST )
	{
		lpMMI->ptMinTrackSize.x += m_SkinAttribute.m_nButtonWidth+m_SkinAttribute.m_nXButtonExcursion;
	}
}

//////////////////////////////////////////////////////////////////////////
