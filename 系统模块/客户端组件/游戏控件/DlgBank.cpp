#include "StdAfx.h"
#include "Information.h"
#include "..\ƽ̨����\GlobalUserInfo.h"
#include "..\..\��Ϣ����\CMD_Commom.h"
#include "..\..\��Ϣ����\CMD_LogonServer.h"
//////////////////////////////////////////////////////////////////////////

//�ؼ���ʶ
#define IDC_COLLOCATE_BAR			100									//�������

//�ߴ綨��
#define BAR_BT_SPACE				2									//��ť���
#define BAR_BT_EXCURSION			10									//��ťƫ��

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CBankBar, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CBankItem, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CBankTransfer, CBankItem)
	ON_BN_CLICKED(IDC_RADIO_ACCOUNT, OnBnClickedRadioAccount)
	ON_BN_CLICKED(IDC_RADIO_GAMEID, OnBnClickedRadioGameid)
	ON_BN_CLICKED(IDC_BTN_TRANSFER, OnBnClickedBtnTransfer)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CBankAccess, CBankItem)
	ON_BN_CLICKED(IDC_BTN_STORAGE, OnBnClickedBtnStorage)
	ON_EN_CHANGE(IDC_EDIT_AMOUNT, OnEnChangeEditAmount)
	ON_BN_CLICKED(IDC_BTN_DRAWOUT, OnBnClickedBtnDrawout)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CBankModifyPass, CBankItem)
	
	ON_BN_CLICKED(ID_BTN_MODIFY, OnBnClickedBtnModify)
END_MESSAGE_MAP()

 BEGIN_MESSAGE_MAP(CDlgBank, CSkinDialog)
	 ON_BN_CLICKED(IDOK, OnOK)
 END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CBankBar::CBankBar(IPlatformResource *pIPlatformResource)
{
	//���Ϣ
	m_bHovering=false;
	m_wCollocateDown=INVALID_WORD;
	m_wCollocateHover=INVALID_WORD;
	m_wCollocateActive=INVALID_WORD;

	//������Ϣ
	m_SizeButton.SetSize(0,0);
	m_rcItemCreate.SetRect(0,0,0,0);

	m_pIPlatformResource=pIPlatformResource;

	return;
}

//��������
CBankBar::~CBankBar()
{
}

//��Ϣ����
BOOL CBankBar::PreTranslateMessage(MSG * pMsg)
{
	//��Ϣ����
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
        GetParent()->PostMessage(WM_CLOSE,0,0);
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//������Դ
bool CBankBar::UpdateSkinResource()
{
	//��Դ����
	HINSTANCE hResInstance=m_pIPlatformResource->GetResInstance();
	IPlazaResource * pIPlazaResource=(IPlazaResource *)m_pIPlatformResource->GetPlazaResource(IID_IPlazaResource,VER_IPlazaResource);
	pIPlazaResource->GetPlazaCollocateImage(m_PlazaCollocateImage);

	//������Դ
	m_ImageBackGroud.SetLoadInfo(m_PlazaCollocateImage.uCollocateBar,hResInstance);

	//������Դ
	CPngImage ImageBtCollocate;
	ImageBtCollocate.LoadImage(hResInstance,m_PlazaCollocateImage.pszBtCollocateBar);
	m_SizeButton.SetSize(ImageBtCollocate.GetWidth()/5,ImageBtCollocate.GetHeight()/2);

	return true;
}

//�������
bool CBankBar::SaveParameter()
{
	//�������
	if (m_wCollocateActive!=INVALID_WORD)
	{
		CBankItem * pCollocateItem=m_CollocateItemArray[m_wCollocateActive];
		if (pCollocateItem->SaveParameter()==false) return false;
	}

	return true;
}

//Ĭ�ϲ���
bool CBankBar::DefaultParameter()
{
	//��������
	for (INT_PTR i=0;i<m_CollocateItemArray.GetCount();i++)
	{
		//��ȡ����
		CBankItem * pCollocateItem=m_CollocateItemArray[i];

		//Ĭ�ϲ���
		if (pCollocateItem->m_hWnd!=NULL) 
		{
			pCollocateItem->DefaultParameter();
			pCollocateItem->UpdateControlStatus();
		}
	}

	return true;
}

//��������
bool CBankBar::SetItemCreateRect(CRect rcItemCreate)
{
	//���ñ���
	m_rcItemCreate=rcItemCreate;

	//�����ؼ�
	for (INT_PTR i=0;i<m_CollocateItemArray.GetCount();i++)
	{
		CBankItem * pCollocateItem=m_CollocateItemArray[i];
		if (pCollocateItem->m_hWnd!=NULL) pCollocateItem->MoveWindow(&rcItemCreate);
	}

	return true;
}

//��������
bool CBankBar::InsertCollocateItem(CBankItem * pCollocateItem)
{
	//���ñ���
	m_CollocateItemArray.Add(pCollocateItem);

	//��������
	if (m_wCollocateActive==INVALID_WORD) SetActiveCollocateItem(pCollocateItem);

	//���½���
	Invalidate(FALSE);

	return true;
}

//��������
bool CBankBar::SetActiveCollocateItem(CBankItem * pCollocateItem)
{
	//Ч�����
	ASSERT(pCollocateItem!=NULL);
	if (pCollocateItem==NULL) return false;

	//��������
	CBankItem * pCollocateActive=NULL;

	//�л��ж�
	if (m_wCollocateActive!=INVALID_WORD)
	{
		//��ȡ����
		ASSERT(m_wCollocateActive<m_CollocateItemArray.GetCount());
		pCollocateActive=m_CollocateItemArray[m_wCollocateActive];

		//�л��ж�
		if (pCollocateActive==pCollocateItem) return true;
		if (pCollocateActive->SaveParameter()==false) return false;
	}

	//��������
	for (INT_PTR i=0;i<m_CollocateItemArray.GetCount();i++)
	{
		if (m_CollocateItemArray[i]==pCollocateItem)
		{
			//��������
			if (pCollocateItem->m_hWnd==NULL)
			{
				pCollocateItem->CreateCollocateItem(GetParent(),m_rcItemCreate);
				pCollocateItem->UpdateControlStatus();
			}

			//���ñ���
			m_wCollocateActive=(WORD)i;

			//��ʾ����
			pCollocateItem->ShowWindow(SW_SHOW);
			pCollocateItem->SetFocus();
			pCollocateItem->UpdateData(FALSE);

			//���ؾ���
			if (pCollocateActive->GetSafeHwnd()!=NULL) pCollocateActive->ShowWindow(SW_HIDE);

			//ˢ�½���
			Invalidate(FALSE);

			return true;
		}
	}

	return false;
}

//ȡ��ǰ��
CBankItem * CBankBar::GetActiveCollocateItem()
{
	if (m_wCollocateActive!=INVALID_WORD)
	{
		//��ȡ����
		ASSERT(m_wCollocateActive<m_CollocateItemArray.GetCount());
		return m_CollocateItemArray[m_wCollocateActive];
	}
	return NULL;
}

//�����л�
WORD CBankBar::SwitchToButtonIndex(CPoint MousePoint)
{
	//����λ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//�߽��ж�
	if (MousePoint.x<((rcClient.Width()-m_SizeButton.cx)/2)) return INVALID_WORD;
	if (MousePoint.x>((rcClient.Width()+m_SizeButton.cx)/2)) return INVALID_WORD;

	//�����ж�
	for (WORD i=0;i<m_CollocateItemArray.GetCount();i++)
	{
		//λ�ü���
		CRect rcButton;
		rcButton.left=(rcClient.Width()-m_SizeButton.cx)/2;
		rcButton.right=(rcClient.Width()+m_SizeButton.cx)/2;
		rcButton.top=(m_SizeButton.cy+BAR_BT_SPACE)*i+BAR_BT_EXCURSION;
		rcButton.bottom=(m_SizeButton.cy+BAR_BT_SPACE)*i+m_SizeButton.cy+BAR_BT_EXCURSION;

		//�����ж�
		if (rcButton.PtInRect(MousePoint)) return i;
	}

	return INVALID_WORD;
}

//�ػ���Ϣ
VOID CBankBar::OnPaint()
{
	CPaintDC dc(this);

	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	CDC DCBuffer;
	CBitmap ImageBuffer;
	DCBuffer.CreateCompatibleDC(&dc);
	ImageBuffer.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());

	//���� DC
	DCBuffer.SetBkMode(TRANSPARENT);
	DCBuffer.SelectObject(&ImageBuffer);
	DCBuffer.SelectObject(CSkinResourceManager::GetDefaultFont());

	//�滭����
	CImageHandle HandleImageBackGroud(&m_ImageBackGroud);
	m_ImageBackGroud.BitBlt(DCBuffer,0,0);

	//������Դ
	CPngImage ImageBtCollocate;
	HINSTANCE hResInstance=m_pIPlatformResource->GetResInstance();
	ImageBtCollocate.LoadImage(hResInstance,m_PlazaCollocateImage.pszBtCollocateBar);

	//�滭��ť
	for (INT i=0;i<m_CollocateItemArray.GetCount();i++)
	{
		//���м���
		INT nXImagePos=0;
		if (i==m_wCollocateDown) nXImagePos=m_SizeButton.cx;
		else if (i==m_wCollocateHover) nXImagePos=m_SizeButton.cx*3;

		//�������
		INT nYImagePos=0;
		if (i==m_wCollocateActive) nYImagePos=m_SizeButton.cy;

		//��ɫ����
		if (i==m_wCollocateActive) DCBuffer.SetTextColor(m_PlazaCollocateImage.crItemActiveTX);
		else if (i==m_wCollocateHover) DCBuffer.SetTextColor(m_PlazaCollocateImage.crItemHoverTX);
		else DCBuffer.SetTextColor(m_PlazaCollocateImage.crItemNormalTX);

		//�滭��ť
		INT nXDrawPos=(rcClient.Width()-m_SizeButton.cx)/2;
		INT nYDrawPos=(m_SizeButton.cy+BAR_BT_SPACE)*i+BAR_BT_EXCURSION;
		ImageBtCollocate.DrawImage(&DCBuffer,nXDrawPos,nYDrawPos,m_SizeButton.cx,m_SizeButton.cy,nXImagePos,nYImagePos);

		//��ȡ����
		CBankItem * pCollocateItem=m_CollocateItemArray[i];
		LPCTSTR pszCollocateName=pCollocateItem->GetCollocateName();

		//�滭�ַ�
		CRect rcItem(nXDrawPos,nYDrawPos,m_SizeButton.cx+nXDrawPos,m_SizeButton.cy+nYDrawPos);
		DCBuffer.DrawText(pszCollocateName,lstrlen(pszCollocateName),rcItem,DT_END_ELLIPSIS|DT_VCENTER|DT_CENTER|DT_SINGLELINE);
	}

	//�滭����
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&DCBuffer,0,0,SRCCOPY);

	//������Դ
	DCBuffer.DeleteDC();
	ImageBuffer.DeleteObject();

	return;
}

//������Ϣ
INT CBankBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//���ñ���
	m_bHovering=false;
	m_wCollocateDown=INVALID_WORD;
	m_wCollocateHover=INVALID_WORD;
	m_wCollocateActive=INVALID_WORD;

	//������Ϣ
	m_rcItemCreate.SetRect(0,0,0,0);
	m_CollocateItemArray.RemoveAll();

	//������Դ
	UpdateSkinResource();

	//�ƶ�����
	CImageHandle HandleImageBackGroud(&m_ImageBackGroud);
	SetWindowPos(NULL,0,0,m_ImageBackGroud.GetWidth(),m_ImageBackGroud.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);

	return 0;
}

//�����Ϣ
VOID CBankBar::OnMouseMove(UINT nFlags, CPoint Point)
{
	__super::OnMouseMove(nFlags, Point);

	//����λ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//λ�ü���
	WORD wCollocateHover=SwitchToButtonIndex(Point);

	//��������
	if (wCollocateHover!=m_wCollocateHover)
	{
		//���ñ���
		m_wCollocateHover=wCollocateHover;

		//���½���
		Invalidate(FALSE);
	}

	//�����ж�
	if (m_bHovering==false)
	{
		//���ñ���
		m_bHovering=true;

		//��������
		TRACKMOUSEEVENT TrackMouseEvent;
		ZeroMemory(&TrackMouseEvent,sizeof(TrackMouseEvent));

		//ע����Ϣ
		TrackMouseEvent.hwndTrack=m_hWnd;
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);

		//ע���¼�
		_TrackMouseEvent(&TrackMouseEvent);
	}

	return;
}

//�����Ϣ
VOID CBankBar::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags,Point);

	//��������
	if ((m_wCollocateHover!=INVALID_WORD)&&(m_wCollocateHover==m_wCollocateDown))
	{
		SetActiveCollocateItem(m_CollocateItemArray[m_wCollocateHover]);
	}

	//������
	if (m_wCollocateDown!=INVALID_WORD)
	{
		//�ͷ����
		ReleaseCapture();

		//���ñ���
		m_wCollocateDown=INVALID_WORD;

		//���½���
		Invalidate(FALSE);
	}

	return;
}

//�����Ϣ
VOID CBankBar::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//���ý���
	SetFocus();

	//���´���
	if ((m_wCollocateHover!=INVALID_WORD)&&(m_wCollocateHover!=m_wCollocateActive))
	{
		//����˻�
		SetCapture();

		//���ñ���
		m_wCollocateDown=m_wCollocateHover;

		//���½���
		Invalidate(FALSE);
	}

	return;
}

//�����Ϣ
LRESULT CBankBar::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	//���½���
	if (m_wCollocateHover!=INVALID_WORD) Invalidate(FALSE);

	//���ñ���
	m_bHovering=false;
	m_wCollocateHover=INVALID_WORD;

	return 0;
}

//�����Ϣ
BOOL CBankBar::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//���ù��
	if (m_wCollocateHover!=INVALID_WORD)
	{
		SetCursor(LoadCursor(GetModuleHandle(CLIENT_CONTROLS_DLL_NAME),MAKEINTRESOURCE(IDC_HAND_CUR1)));
		return TRUE;
	}

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CBankItem::CBankItem(UINT uIDTemplate,IPlatformResource *pIPlatformResource,tagGlobalUserData *pGlobalUserData) : 
	CDialog(uIDTemplate),m_pIPlatformResource(pIPlatformResource),m_pGlobalUserData(pGlobalUserData)
{
	//��ȡ��Դ
	HINSTANCE hResInstance=m_pIPlatformResource->GetResInstance();
	IPlazaResource * pIPlazaResource=(IPlazaResource *)m_pIPlatformResource->GetPlazaResource(IID_IPlazaResource,VER_IPlazaResource);
	pIPlazaResource->GetPlazaCollocateImage(m_PlazaCollocateImage);
	tagPlazaCollocateImage & PlazaCollocateImage=m_PlazaCollocateImage;

	//������Դ
	m_EncircleImageFrame.ImageML.SetLoadInfo(PlazaCollocateImage.uTitleML,hResInstance);
	m_EncircleImageFrame.ImageMR.SetLoadInfo(PlazaCollocateImage.uTitleMR,hResInstance);
	m_EncircleImageFrame.ImageTM.SetLoadInfo(PlazaCollocateImage.uTitleTM,hResInstance);
	m_EncircleImageFrame.ImageTR.SetLoadInfo(PlazaCollocateImage.uTitleTR,hResInstance);
	m_EncircleImageFrame.ImageBL.SetLoadInfo(PlazaCollocateImage.uTitleBL,hResInstance);
	m_EncircleImageFrame.ImageBM.SetLoadInfo(PlazaCollocateImage.uTitleBM,hResInstance);
	m_EncircleImageFrame.ImageBR.SetLoadInfo(PlazaCollocateImage.uTitleBR,hResInstance);
	m_EncircleImageFrame.ImageTL.SetLoadInfo(PlazaCollocateImage.uTitleTL,hResInstance);

	m_pIBankSink=NULL;
	//��ȡ��С
	CSkinAide::GetEncircleInfo(m_EncircleInfoFrame,m_EncircleImageFrame);
	return;
}

//��������
CBankItem::~CBankItem()
{
}

//�滭����
VOID CBankItem::DrawBackGround(CDC * pDC, INT nWidth, INT nHeight)
{
	//�滭���
	CRect rcFrame(0,0,nWidth,nHeight);
	CSkinAide::DrawEncircleFrame(pDC,rcFrame,m_EncircleImageFrame);

	//�滭����
	CRect rcInside;
	CSkinAide::CaleWindowRect(rcFrame,m_EncircleInfoFrame,rcInside);
	pDC->FillSolidRect(&rcInside,CSkinDialog::m_SkinAttribute.m_crBackGround);

	//���� DC
	pDC->SetBkMode(TRANSPARENT);
	pDC->SelectObject(CSkinResourceManager::GetDefaultFont());
	pDC->SetTextColor(m_PlazaCollocateImage.crItemTitleTX);

	//�滭����
	LPCTSTR pszCollocateName=GetCollocateName();
	CRect rcTitil(10,2,nWidth-20,m_EncircleInfoFrame.nTBorder);
	pDC->DrawText(pszCollocateName,lstrlen(pszCollocateName),rcTitil,DT_END_ELLIPSIS|DT_VCENTER|DT_SINGLELINE);

	return;
}

//��������
bool CBankItem::CreateCollocateItem(CWnd * pParentWnd, CRect rcCreate)
{
	//��������
	Create(m_lpszTemplateName,pParentWnd);

	//�ƶ�����
	SetWindowPos(NULL,rcCreate.left,rcCreate.top,rcCreate.Width(),rcCreate.Height(),SWP_NOZORDER);

	return true;
}

//������Ϣ
BOOL CBankItem::OnEraseBkgnd(CDC * pDC)
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);
	
	//��������
	CBitmap ImageBuffer;
	ImageBuffer.CreateCompatibleBitmap(pDC,rcClient.Width(),rcClient.Height());

	//���� DC
	CDC DCBuffer;
	DCBuffer.CreateCompatibleDC(pDC);
	DCBuffer.SelectObject(&ImageBuffer);

	//�滭����
	DrawBackGround(&DCBuffer,rcClient.Width(),rcClient.Height());

	//�滭����
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),&DCBuffer,0,0,SRCCOPY);

	//������Դ
	DCBuffer.DeleteDC();
	ImageBuffer.DeleteObject();

	return TRUE;
}

//��ɫ��Ϣ
HBRUSH CBankItem::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_STATIC:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetBkColor(CSkinDialog::m_SkinAttribute.m_crBackGround);
			pDC->SetTextColor(CSkinDialog::m_SkinAttribute.m_crControlText);
			return CSkinDialog::m_SkinAttribute.m_brBackGround;
		}
	}

	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CBankAccess::CBankAccess(IPlatformResource *pIPlatformResource,tagGlobalUserData *pGlobalUserData) : CBankItem(IDD_BANK_ACCESS,pIPlatformResource,pGlobalUserData)
{
	//Ĭ�ϲ���
	DefaultParameter();

	return;
}

//��������
CBankAccess::~CBankAccess()
{
}

//�������
bool CBankAccess::SaveParameter()
{
	
	return true;
}

//Ĭ�ϲ���
bool CBankAccess::DefaultParameter()
{

	return true;
}

//���¿���
bool CBankAccess::UpdateControlStatus()
{	
	GetDlgItem(IDC_EDIT_AMOUNT)->SetFocus();
	return true;
}

//��ȡ����
LPCTSTR CBankAccess::GetCollocateName()
{
	return TEXT("��Ϸ�Ҵ�ȡ");
}

void CBankAccess::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_BTN_STORAGE, m_btStorage);
	DDX_Control(pDX, IDC_BTN_DRAWOUT, m_btDrawout);
	DDX_Text(pDX, IDC_CUR_GOLD, m_pGlobalUserData->lGold);
	DDX_Text(pDX, IDC_BANK_GOLD, m_pGlobalUserData->lInsureGold);
}

void CBankAccess::OnBnClickedBtnStorage()
{
	//��ȡ����
	UpdateData();
	CString strText, strPassword;
	GetDlgItem(IDC_EDIT_AMOUNT)->GetWindowText(strText);
	GetDlgItem(IDC_EDIT_BANKPASS)->GetWindowText(strPassword);
    LONGLONG lAmount=_tstoi64(strText);

	//��֤����
	if(lAmount > m_pGlobalUserData->lGold)
	{
		ShowInformationEx(TEXT("ϵͳ��ʾ"),TEXT("�������Ϸ�Ҳ��ܴ�������ǰ��Ϸ��������"), MB_OK|MB_ICONINFORMATION); 
		GetDlgItem(IDC_EDIT_AMOUNT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetSel(0,-1);
		return;
	}
	else if(lAmount < MIN_BANK_COUNT)
	{
		CString strTip;
		strTip.Format(TEXT("���չ��ȡ��Ϸ�ҵ���С��λΪ%d"),MIN_BANK_COUNT);
		ShowInformationEx(TEXT("ϵͳ��ʾ"),strTip,MB_OK|MB_ICONINFORMATION);
		GetDlgItem(IDC_EDIT_AMOUNT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetSel(0,-1);
		return;
	}
	else if(strPassword.IsEmpty())
	{
		ShowInformationEx(TEXT("ϵͳ��ʾ"),TEXT("���벻��Ϊ�գ���������ȷ�ı��չ����룡"), MB_OK|MB_ICONINFORMATION); 
		GetDlgItem(IDC_EDIT_BANKPASS)->SetFocus();
		return;
	}
	CMD_CM_BankAccess BankAccess;
	BankAccess.dwUserID=m_pGlobalUserData->dwUserID;
	BankAccess.cbGameAction=0;
	BankAccess.lAmount=lAmount;
	CMD5Encrypt::EncryptData(strPassword, BankAccess.szPassword);
	m_pIBankSink->OnEventBankOperate(BANK_TYPE_STORAGE,&BankAccess,sizeof(BankAccess));
	GetDlgItem(IDC_EDIT_BANKPASS)->SetWindowText(TEXT(""));
}

void CBankAccess::OnEnChangeEditAmount()
{

}

void CBankAccess::OnBnClickedBtnDrawout()
{
	//��ȡ����
	UpdateData();
	CString strText, strPassword;
	GetDlgItem(IDC_EDIT_AMOUNT)->GetWindowText(strText);
	GetDlgItem(IDC_EDIT_BANKPASS)->GetWindowText(strPassword);
    LONGLONG lAmount=_tstoi64(strText);

	//��֤����
	if(lAmount > m_pGlobalUserData->lInsureGold)
	{
		ShowInformationEx(TEXT("ϵͳ��ʾ"),TEXT("ȡ������Ϸ�Ҳ��ܴ�������ǰ���չ����Ϸ��������"), MB_OK|MB_ICONINFORMATION); 
		GetDlgItem(IDC_EDIT_AMOUNT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetSel(0,-1);
		return;
	}
	else if(lAmount < MIN_BANK_COUNT)
	{
		CString strTip;
		strTip.Format(TEXT("���չ��ȡ��Ϸ�ҵ���С��λΪ%d"),MIN_BANK_COUNT);
		ShowInformationEx(TEXT("ϵͳ��ʾ"),strTip,MB_OK|MB_ICONINFORMATION);
		GetDlgItem(IDC_EDIT_AMOUNT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetSel(0,-1);
		return;
	}
	else if(strPassword.IsEmpty())
	{
		ShowInformationEx(TEXT("ϵͳ��ʾ"),TEXT("���벻��Ϊ�գ���������ȷ�ı��չ����룡"), MB_OK|MB_ICONINFORMATION); 
		GetDlgItem(IDC_EDIT_BANKPASS)->SetFocus();
		return;
	}
	CMD_CM_BankAccess BankAccess;
	BankAccess.dwUserID=m_pGlobalUserData->dwUserID;
	BankAccess.cbGameAction=0;
	BankAccess.lAmount=lAmount;
	CMD5Encrypt::EncryptData(strPassword, BankAccess.szPassword);
	m_pIBankSink->OnEventBankOperate(BANK_TYPE_DRAWOUT,&BankAccess,sizeof(BankAccess));
	GetDlgItem(IDC_EDIT_BANKPASS)->SetWindowText(TEXT(""));
}	

//////////////////////////////////////////////////////////////////////////

//���캯��
CBankTransfer::CBankTransfer(IPlatformResource *pIPlatformResource,tagGlobalUserData *pGlobalUserData) : CBankItem(IDD_BANK_TRANSFER,pIPlatformResource,pGlobalUserData)
{
	//Ĭ�ϲ���
	DefaultParameter();
	m_nAccounts=1;
	//((CButton*)GetDlgItem(IDC_RADIO_ACCOUNT))->SetCheck(true);
	return;
}

//��������
CBankTransfer::~CBankTransfer()
{
}
 
//�ؼ���
VOID CBankTransfer::DoDataExchange(CDataExchange * pDX)
{
	DDX_Text(pDX, IDC_CUR_GOLD, m_pGlobalUserData->lGold);
	DDX_Text(pDX, IDC_BANK_GOLD, m_pGlobalUserData->lInsureGold);
	DDX_Control(pDX, IDC_BTN_TRANSFER, m_btTransfer);
	DDX_Check(pDX,IDC_RADIO_ACCOUNT,m_nAccounts);
}

//�������
bool CBankTransfer::SaveParameter()
{
	return true;
}

//Ĭ�ϲ���
bool CBankTransfer::DefaultParameter()
{

	return true;
}

//���¿���
bool CBankTransfer::UpdateControlStatus()
{
	return true;
}

//��ȡ����
LPCTSTR CBankTransfer::GetCollocateName()
{
	return TEXT("����ת��");
}

void CBankTransfer::OnBnClickedRadioAccount()
{
	GetDlgItem(IDC_STATIC_TYPE)->SetWindowText(TEXT("��Ϸ�ʺţ�"));
}

void CBankTransfer::OnBnClickedRadioGameid()
{
	GetDlgItem(IDC_STATIC_TYPE)->SetWindowText(TEXT("��Ϸ�ɣģ�"));
}

void CBankTransfer::OnBnClickedBtnTransfer()
{
	UpdateData();
	//��ȡ����
	LONGLONG lTransfer;
	CString strText;
	GetDlgItem(IDC_EDIT_AMOUNT)->GetWindowText(strText);
	lTransfer=_tstoi64(strText);

	//������֤
	if (lTransfer < MIN_BANK_COUNT || lTransfer > m_pGlobalUserData->lInsureGold)
	{
		CString strMessage;
		if(lTransfer<MIN_BANK_COUNT)
			strMessage.Format(TEXT("�Բ���ת�ʵ���С���Ϊ%d��"),MIN_BANK_COUNT);
		else
			strMessage.Format(TEXT("ת�ʵ��������ܴ�������ǰ���չ����Ϸ��,��ȷ�����չ������㹻����Ϸ�ҡ�"));
		ShowInformationEx(TEXT("ϵͳ��ʾ"),strMessage,MB_OK|MB_ICONINFORMATION) ;

		GetDlgItem(IDC_EDIT_AMOUNT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetSel(0,-1);
		return;
	}

	CString strRecvUser;
	GetDlgItem(IDC_EDIT_RECV_ACCOUNT)->GetWindowText(strRecvUser);
	strRecvUser.Trim();
	if(strRecvUser=="")
	{
		ShowInformationEx(TEXT("ϵͳ��Ϣ"),TEXT("������������͵��ǳƣ���ID"),MB_OK|MB_ICONINFORMATION) ;
		GetDlgItem(IDC_EDIT_RECV_ACCOUNT)->SetFocus();
		return;
	}
	CMD_CM_BankTransfer Transfer;
	ZeroMemory(&Transfer, sizeof(Transfer));
	Transfer.dwUserID=m_pGlobalUserData->dwUserID;
	Transfer.lAmount=lTransfer;

	if(m_nAccounts==0)
	{
		DWORD dwUserID=(DWORD)atoi(strRecvUser);
		if(dwUserID==m_pGlobalUserData->dwGameID)
		{
			ShowInformationEx(TEXT("ϵͳ��Ϣ"),TEXT("����ת�ʸ��Լ�"),MB_OK|MB_ICONINFORMATION) ;
			GetDlgItem(IDC_EDIT_RECV_ACCOUNT)->SetFocus();
			return;
		}
		Transfer.bIsAccount=false;
		lstrcpyn(Transfer.szRecvUser,strRecvUser,LEN_ACCOUNTS);
	}	
	else
	{
		strRecvUser.MakeLower();
		CString strAccounts(m_pGlobalUserData->szAccounts);
		strAccounts.MakeLower();
		if(strcmp(strRecvUser,strAccounts)==0)
		{
			ShowInformationEx(TEXT("ϵͳ��Ϣ"),TEXT("�������͸��Լ�"),MB_OK|MB_ICONINFORMATION) ;
			GetDlgItem(IDC_EDIT_RECV_ACCOUNT)->SetFocus();
			return;
		}
		Transfer.bIsAccount=true;
		lstrcpyn(Transfer.szRecvUser,strRecvUser,LEN_ACCOUNTS);
	}
	//������֤
	TCHAR szPassword[LEN_PASSWORD]=TEXT("");
	GetDlgItemText(IDC_EDIT_BANKPASS,szPassword,CountArray(szPassword));
	if (szPassword[0]==0)
	{
		ShowInformationEx(TEXT("ϵͳ��Ϣ"),TEXT("���벻��Ϊ�գ��������������룡") ,MB_OK|MB_ICONINFORMATION) ;
		GetDlgItem(IDC_EDIT_BANKPASS)->SetWindowText("");
		GetDlgItem(IDC_EDIT_BANKPASS)->SetFocus();
		return;
	}
	CMD5Encrypt::EncryptData(szPassword, Transfer.szPassword);

	m_pIBankSink->OnEventBankOperate(BANK_TYPE_QUERY,&Transfer,sizeof(Transfer));

	GetDlgItem(IDC_EDIT_BANKPASS)->SetWindowText(TEXT(""));
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CBankModifyPass::CBankModifyPass(IPlatformResource *pIPlatformResource,tagGlobalUserData *pGlobalUserData) : CBankItem(IDD_BANK_MODIFYPASS,pIPlatformResource,pGlobalUserData)
{
	//Ĭ�ϲ���
	DefaultParameter();

	return;
}

//��������
CBankModifyPass::~CBankModifyPass()
{
}

//�������
bool CBankModifyPass::SaveParameter()
{
	return true;
}

//Ĭ�ϲ���
bool CBankModifyPass::DefaultParameter()
{
	return true;
}

//���¿���
bool CBankModifyPass::UpdateControlStatus()
{
	return true;
}

//��ȡ����
LPCTSTR CBankModifyPass::GetCollocateName()
{
	return TEXT("�޸�����");
}

void CBankModifyPass::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, ID_BTN_MODIFY, m_btModifyPass);
}

//////////////////////////////////////////////////////////////////////////


//���캯��
CDlgBank::CDlgBank(IPlatformResource *pIPlatformResource,tagGlobalUserData *pGlobalUserData) : CSkinDialog(IDD),m_CollocateBar(pIPlatformResource),\
	m_BankAccess(pIPlatformResource,pGlobalUserData),m_BankTransfer(pIPlatformResource,pGlobalUserData),m_BankModifyPass(pIPlatformResource,pGlobalUserData),\
	m_pGlobalUserData(pGlobalUserData)
{
	//���ñ���
	m_pIPlatformResource=pIPlatformResource;
	m_pIBankSink=NULL;
	return;
}

//��������
CDlgBank::~CDlgBank()
{
}

//���ò���
bool CDlgBank::InitBankSink(IBankSink *pIBankSink)
{
	//���ñ���
	m_pIBankSink=pIBankSink;
	m_BankAccess.SetBankSink(pIBankSink);
	m_BankTransfer.SetBankSink(pIBankSink);
	m_BankModifyPass.SetBankSink(pIBankSink);

	return true;
}

//�ؼ���
VOID CDlgBank::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}

//��������
BOOL CDlgBank::OnInitDialog()
{
	__super::OnInitDialog();

	//���ñ���
	SetWindowText(TEXT("������"));

	//���ñ���
//	m_ImageTitle.LoadImage(AfxGetInstanceHandle(),TEXT("TITLE_DLG_COLLOCATE"));
	CMD_CM_BankUpdate BankUpdate={0};
	lstrcpyn(BankUpdate.szPassword,m_pGlobalUserData->szPassword,LEN_PASSWORD);
	BankUpdate.dwUserID=m_pGlobalUserData->dwUserID;
	if(m_pIBankSink!=NULL)
		m_pIBankSink->OnEventBankOperate(BANK_TYPE_UPDATE,&BankUpdate,sizeof(BankUpdate));

	//��������
	AfxSetResourceHandle(GetModuleHandle(CLIENT_CONTROLS_DLL_NAME));
	m_CollocateBar.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,IDC_COLLOCATE_BAR);
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//����λ��
	CRect rcCollocateBar;
	m_CollocateBar.GetWindowRect(&rcCollocateBar);
	m_CollocateBar.SetWindowPos(NULL,GetXBorder()+5,GetCaptionHeight()+5,rcCollocateBar.Width(),rcCollocateBar.Height(),SWP_NOZORDER);

	//����λ��
	CRect rcItemCreate;
	rcItemCreate.top=GetCaptionHeight()+5;
	rcItemCreate.right=rcClient.Width()-GetXBorder()-5;
	rcItemCreate.left=GetXBorder()+rcCollocateBar.Width()+10;
	rcItemCreate.bottom=GetCaptionHeight()+rcCollocateBar.Height()+5;
	m_CollocateBar.SetItemCreateRect(rcItemCreate);

	//�ƶ�����
	CRect rcButton;
	m_btOk.GetWindowRect(&rcButton);
	SetWindowPos(NULL,0,0,rcClient.Width(),GetCaptionHeight()+GetYBorder()+rcCollocateBar.Height()+rcButton.Height()+15,SWP_NOZORDER|SWP_NOMOVE);

	//�ƶ���ť
	GetClientRect(&rcClient);
	m_btOk.SetWindowPos(NULL,rcClient.Width()-rcButton.Width()*3-GetXBorder()-15,rcClient.Height()-rcButton.Height()-GetYBorder()-3,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_btCancel.SetWindowPos(NULL,rcClient.Width()-rcButton.Width()-GetXBorder()-5,rcClient.Height()-rcButton.Height()-GetYBorder()-3,0,0,SWP_NOZORDER|SWP_NOSIZE);
	
	//ȫ������
	m_CollocateBar.InsertCollocateItem(&m_BankAccess);
	m_CollocateBar.InsertCollocateItem(&m_BankTransfer);
	m_CollocateBar.InsertCollocateItem(&m_BankModifyPass);
	
	m_CollocateBar.GetActiveCollocateItem()->UpdateControlStatus();
	return TRUE;
}

//ȷ������
VOID CDlgBank::OnOK()
{
	//�������
	if (m_CollocateBar.SaveParameter()==false) return;

	__super::OnOK();
}

//Ĭ�ϰ�ť
VOID CDlgBank::OnBnClickedDefault()
{
	//Ĭ�ϲ���
	m_CollocateBar.DefaultParameter();

	return;
}

//ˢ�½���
bool CDlgBank::UpdateView()
{
	CBankItem *pItem=m_CollocateBar.GetActiveCollocateItem();
	if(pItem!=NULL)
		pItem->UpdateData(FALSE);
	return true;
}

BOOL CDlgBank::PreTranslateMessage(MSG* pMsg)
{
	//��ť����
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_RETURN))
	{
		return true;
	}
	return CSkinDialog::PreTranslateMessage(pMsg);
}

//////////////////////////////////////////////////////////////////////////



BOOL CBankAccess::PreTranslateMessage(MSG* pMsg)
{
	//��ť����
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_RETURN))
	{
		CWnd *pCurWnd = GetFocus();
        if(pCurWnd!=NULL)
		{
			CWnd   *pNext = GetNextDlgTabItem(pCurWnd);
			pNext-> SetFocus(); 			
		}
		return true;
	}
	return CBankItem::PreTranslateMessage(pMsg);
}

BOOL CBankModifyPass::PreTranslateMessage(MSG* pMsg)
{
	//��ť����
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_RETURN))
	{
		CWnd *pCurWnd = GetFocus();
        if(pCurWnd!=NULL)
		{
			CWnd   *pNext = GetNextDlgTabItem(pCurWnd);
			pNext-> SetFocus(); 			
		}
		return true;
	}
	return CBankItem::PreTranslateMessage(pMsg);
}

BOOL CBankTransfer::PreTranslateMessage(MSG* pMsg)
{
	//��ť����
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_RETURN))
	{
		CWnd *pCurWnd = GetFocus();
        if(pCurWnd!=NULL)
		{
			CWnd   *pNext = GetNextDlgTabItem(pCurWnd);
			pNext-> SetFocus(); 			
		}
		return true;
	}
	return CBankItem::PreTranslateMessage(pMsg);
}

void CBankModifyPass::OnBnClickedBtnModify()
{
//��������
	TCHAR szSrcPassword[LEN_PASSWORD];
	TCHAR szDesPassword[2][LEN_PASSWORD];

	//��ȡ��Ϣ
	GetDlgItemText(IDC_EDIT_OLDPASS,szSrcPassword,CountArray(szSrcPassword));
	GetDlgItemText(IDC_EDIT_NEWPASS,szDesPassword[0],CountArray(szDesPassword[0]));
	GetDlgItemText(IDC_EDIT_VERIFYPASS,szDesPassword[1],CountArray(szDesPassword[1]));

	//�����ж�
	if (szSrcPassword[0]==0)
	{
		ShowInformation(TEXT("������ԭ���չ����룡"),MB_ICONERROR,0);
		GetDlgItem(IDC_EDIT_OLDPASS)->SetFocus();
		return;
	}

	//�����ж�
	if (lstrcmp(szDesPassword[0],szDesPassword[1])!=0)
	{
		ShowInformation(TEXT("��������ı��չ����벻һ�£����������룡"),MB_ICONERROR,0);
		GetDlgItem(IDC_EDIT_VERIFYPASS)->SetFocus();
		return;
	}

	//�����ж�
	if (szDesPassword[0]==0)
	{
		ShowInformation(TEXT("�±��չ����벻��Ϊ�գ����������룡"),MB_ICONERROR,0);
		GetDlgItem(IDC_EDIT_NEWPASS)->SetFocus();
		return;
	}

	//��������6λ������
	TCHAR szPassword[LEN_PASSWORD] = {0};
	lstrcpyn( szPassword,szDesPassword[0],CountArray(szPassword) );
	INT nPassLen = lstrlen(szPassword);
	if( nPassLen < 6 )
	{
		ShowInformation(TEXT("�����벻������6λ�����������룡"),MB_ICONERROR);
		GetDlgItem(IDC_EDIT_NEWPASS)->SetFocus();
		return;
	}

	//���˼�����
	for( INT i = 0; i < nPassLen; i++ )
	{
		if( szPassword[i] < '0' || szPassword[i] > 'z' ||
			( szPassword[i] > '9' && szPassword[i] < 'A' ) || 
			( szPassword[i] > 'Z' && szPassword[i] < 'a' ) )
			break;
	}
	if( i == nPassLen )
	{
		//����
		bool bSorted=true;
		TCHAR chSwap = 0;
		INT nLast=nPassLen-1;
		do
		{
			bSorted=true;
			for (INT i=0;i<nLast;i++)
			{
				if (szPassword[i]<szPassword[i+1])
				{
					//���ñ�־
					bSorted=false;

					//�˿�����
					chSwap=szPassword[i];
					szPassword[i]=szPassword[i+1];
					szPassword[i+1]=chSwap;
				}	
			}
			nLast--;
		} while(bSorted==false);

		//�ж��Ƿ���������ͬ
		for( INT i = 1; i < nPassLen; i++ )
		{
			if( szPassword[0] == szPassword[i]+i ||
				szPassword[0] == szPassword[i] )
				continue;

			break;
		}
		if( i == nPassLen )
		{
			ShowInformation(TEXT("�����벻������ͬ�����ŵ���ĸ���֣����������룡"),MB_ICONERROR);
			GetDlgItem(IDC_EDIT_NEWPASS)->SetFocus();
			return;
		}
	}

	//��������
	CMD_GP_ModifyInsurePass ModifyInsurePass;
	ZeroMemory(&ModifyInsurePass,sizeof(ModifyInsurePass));

	//��������
	TCHAR szOldPassword[LEN_PASSWORD]=TEXT("");
	TCHAR szNewPassword[LEN_PASSWORD]=TEXT("");
	CMD5Encrypt::EncryptData(szSrcPassword,szOldPassword);
	CMD5Encrypt::EncryptData(szDesPassword[0],szNewPassword);

	//��������
	ModifyInsurePass.dwUserID=m_pGlobalUserData->dwUserID;
	lstrcpynW(ModifyInsurePass.szScrPassword,CT2CW(szOldPassword),CountArray(ModifyInsurePass.szScrPassword));
	lstrcpynW(ModifyInsurePass.szDesPassword,CT2CW(szNewPassword),CountArray(ModifyInsurePass.szDesPassword));

	//��������
	m_pIBankSink->OnEventBankOperate(BANK_TYPE_MODIFYPASS, &ModifyInsurePass,sizeof(ModifyInsurePass));
	GetDlgItem(IDC_EDIT_OLDPASS)->SetWindowText(TEXT(""));
}

