#include "StdAfx.h"
#include "Information.h"
#include "..\平台数据\GlobalUserInfo.h"
#include "..\..\消息定义\CMD_Commom.h"
#include "..\..\消息定义\CMD_LogonServer.h"
//////////////////////////////////////////////////////////////////////////

//控件标识
#define IDC_COLLOCATE_BAR			100									//子项控制

//尺寸定义
#define BAR_BT_SPACE				2									//按钮间距
#define BAR_BT_EXCURSION			10									//按钮偏移

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

//构造函数
CBankBar::CBankBar(IPlatformResource *pIPlatformResource)
{
	//活动信息
	m_bHovering=false;
	m_wCollocateDown=INVALID_WORD;
	m_wCollocateHover=INVALID_WORD;
	m_wCollocateActive=INVALID_WORD;

	//子项信息
	m_SizeButton.SetSize(0,0);
	m_rcItemCreate.SetRect(0,0,0,0);

	m_pIPlatformResource=pIPlatformResource;

	return;
}

//析构函数
CBankBar::~CBankBar()
{
}

//消息解释
BOOL CBankBar::PreTranslateMessage(MSG * pMsg)
{
	//消息过虑
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
        GetParent()->PostMessage(WM_CLOSE,0,0);
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//更新资源
bool CBankBar::UpdateSkinResource()
{
	//资源变量
	HINSTANCE hResInstance=m_pIPlatformResource->GetResInstance();
	IPlazaResource * pIPlazaResource=(IPlazaResource *)m_pIPlatformResource->GetPlazaResource(IID_IPlazaResource,VER_IPlazaResource);
	pIPlazaResource->GetPlazaCollocateImage(m_PlazaCollocateImage);

	//设置资源
	m_ImageBackGroud.SetLoadInfo(m_PlazaCollocateImage.uCollocateBar,hResInstance);

	//加载资源
	CPngImage ImageBtCollocate;
	ImageBtCollocate.LoadImage(hResInstance,m_PlazaCollocateImage.pszBtCollocateBar);
	m_SizeButton.SetSize(ImageBtCollocate.GetWidth()/5,ImageBtCollocate.GetHeight()/2);

	return true;
}

//保存参数
bool CBankBar::SaveParameter()
{
	//保存参数
	if (m_wCollocateActive!=INVALID_WORD)
	{
		CBankItem * pCollocateItem=m_CollocateItemArray[m_wCollocateActive];
		if (pCollocateItem->SaveParameter()==false) return false;
	}

	return true;
}

//默认参数
bool CBankBar::DefaultParameter()
{
	//设置子项
	for (INT_PTR i=0;i<m_CollocateItemArray.GetCount();i++)
	{
		//获取子项
		CBankItem * pCollocateItem=m_CollocateItemArray[i];

		//默认参数
		if (pCollocateItem->m_hWnd!=NULL) 
		{
			pCollocateItem->DefaultParameter();
			pCollocateItem->UpdateControlStatus();
		}
	}

	return true;
}

//设置区域
bool CBankBar::SetItemCreateRect(CRect rcItemCreate)
{
	//设置变量
	m_rcItemCreate=rcItemCreate;

	//调整控件
	for (INT_PTR i=0;i<m_CollocateItemArray.GetCount();i++)
	{
		CBankItem * pCollocateItem=m_CollocateItemArray[i];
		if (pCollocateItem->m_hWnd!=NULL) pCollocateItem->MoveWindow(&rcItemCreate);
	}

	return true;
}

//插入子项
bool CBankBar::InsertCollocateItem(CBankItem * pCollocateItem)
{
	//设置变量
	m_CollocateItemArray.Add(pCollocateItem);

	//激活子项
	if (m_wCollocateActive==INVALID_WORD) SetActiveCollocateItem(pCollocateItem);

	//更新界面
	Invalidate(FALSE);

	return true;
}

//激活设置
bool CBankBar::SetActiveCollocateItem(CBankItem * pCollocateItem)
{
	//效验参数
	ASSERT(pCollocateItem!=NULL);
	if (pCollocateItem==NULL) return false;

	//变量定义
	CBankItem * pCollocateActive=NULL;

	//切换判断
	if (m_wCollocateActive!=INVALID_WORD)
	{
		//获取子项
		ASSERT(m_wCollocateActive<m_CollocateItemArray.GetCount());
		pCollocateActive=m_CollocateItemArray[m_wCollocateActive];

		//切换判断
		if (pCollocateActive==pCollocateItem) return true;
		if (pCollocateActive->SaveParameter()==false) return false;
	}

	//激活子项
	for (INT_PTR i=0;i<m_CollocateItemArray.GetCount();i++)
	{
		if (m_CollocateItemArray[i]==pCollocateItem)
		{
			//创建子项
			if (pCollocateItem->m_hWnd==NULL)
			{
				pCollocateItem->CreateCollocateItem(GetParent(),m_rcItemCreate);
				pCollocateItem->UpdateControlStatus();
			}

			//设置变量
			m_wCollocateActive=(WORD)i;

			//显示窗口
			pCollocateItem->ShowWindow(SW_SHOW);
			pCollocateItem->SetFocus();
			pCollocateItem->UpdateData(FALSE);

			//隐藏旧项
			if (pCollocateActive->GetSafeHwnd()!=NULL) pCollocateActive->ShowWindow(SW_HIDE);

			//刷新界面
			Invalidate(FALSE);

			return true;
		}
	}

	return false;
}

//取当前项
CBankItem * CBankBar::GetActiveCollocateItem()
{
	if (m_wCollocateActive!=INVALID_WORD)
	{
		//获取子项
		ASSERT(m_wCollocateActive<m_CollocateItemArray.GetCount());
		return m_CollocateItemArray[m_wCollocateActive];
	}
	return NULL;
}

//索引切换
WORD CBankBar::SwitchToButtonIndex(CPoint MousePoint)
{
	//窗口位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//边界判断
	if (MousePoint.x<((rcClient.Width()-m_SizeButton.cx)/2)) return INVALID_WORD;
	if (MousePoint.x>((rcClient.Width()+m_SizeButton.cx)/2)) return INVALID_WORD;

	//子项判断
	for (WORD i=0;i<m_CollocateItemArray.GetCount();i++)
	{
		//位置计算
		CRect rcButton;
		rcButton.left=(rcClient.Width()-m_SizeButton.cx)/2;
		rcButton.right=(rcClient.Width()+m_SizeButton.cx)/2;
		rcButton.top=(m_SizeButton.cy+BAR_BT_SPACE)*i+BAR_BT_EXCURSION;
		rcButton.bottom=(m_SizeButton.cy+BAR_BT_SPACE)*i+m_SizeButton.cy+BAR_BT_EXCURSION;

		//区域判断
		if (rcButton.PtInRect(MousePoint)) return i;
	}

	return INVALID_WORD;
}

//重画消息
VOID CBankBar::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//创建缓冲
	CDC DCBuffer;
	CBitmap ImageBuffer;
	DCBuffer.CreateCompatibleDC(&dc);
	ImageBuffer.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());

	//设置 DC
	DCBuffer.SetBkMode(TRANSPARENT);
	DCBuffer.SelectObject(&ImageBuffer);
	DCBuffer.SelectObject(CSkinResourceManager::GetDefaultFont());

	//绘画背景
	CImageHandle HandleImageBackGroud(&m_ImageBackGroud);
	m_ImageBackGroud.BitBlt(DCBuffer,0,0);

	//加载资源
	CPngImage ImageBtCollocate;
	HINSTANCE hResInstance=m_pIPlatformResource->GetResInstance();
	ImageBtCollocate.LoadImage(hResInstance,m_PlazaCollocateImage.pszBtCollocateBar);

	//绘画按钮
	for (INT i=0;i<m_CollocateItemArray.GetCount();i++)
	{
		//横行计算
		INT nXImagePos=0;
		if (i==m_wCollocateDown) nXImagePos=m_SizeButton.cx;
		else if (i==m_wCollocateHover) nXImagePos=m_SizeButton.cx*3;

		//竖向计算
		INT nYImagePos=0;
		if (i==m_wCollocateActive) nYImagePos=m_SizeButton.cy;

		//颜色设置
		if (i==m_wCollocateActive) DCBuffer.SetTextColor(m_PlazaCollocateImage.crItemActiveTX);
		else if (i==m_wCollocateHover) DCBuffer.SetTextColor(m_PlazaCollocateImage.crItemHoverTX);
		else DCBuffer.SetTextColor(m_PlazaCollocateImage.crItemNormalTX);

		//绘画按钮
		INT nXDrawPos=(rcClient.Width()-m_SizeButton.cx)/2;
		INT nYDrawPos=(m_SizeButton.cy+BAR_BT_SPACE)*i+BAR_BT_EXCURSION;
		ImageBtCollocate.DrawImage(&DCBuffer,nXDrawPos,nYDrawPos,m_SizeButton.cx,m_SizeButton.cy,nXImagePos,nYImagePos);

		//获取标题
		CBankItem * pCollocateItem=m_CollocateItemArray[i];
		LPCTSTR pszCollocateName=pCollocateItem->GetCollocateName();

		//绘画字符
		CRect rcItem(nXDrawPos,nYDrawPos,m_SizeButton.cx+nXDrawPos,m_SizeButton.cy+nYDrawPos);
		DCBuffer.DrawText(pszCollocateName,lstrlen(pszCollocateName),rcItem,DT_END_ELLIPSIS|DT_VCENTER|DT_CENTER|DT_SINGLELINE);
	}

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&DCBuffer,0,0,SRCCOPY);

	//清理资源
	DCBuffer.DeleteDC();
	ImageBuffer.DeleteObject();

	return;
}

//建立消息
INT CBankBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置变量
	m_bHovering=false;
	m_wCollocateDown=INVALID_WORD;
	m_wCollocateHover=INVALID_WORD;
	m_wCollocateActive=INVALID_WORD;

	//子项信息
	m_rcItemCreate.SetRect(0,0,0,0);
	m_CollocateItemArray.RemoveAll();

	//更新资源
	UpdateSkinResource();

	//移动窗口
	CImageHandle HandleImageBackGroud(&m_ImageBackGroud);
	SetWindowPos(NULL,0,0,m_ImageBackGroud.GetWidth(),m_ImageBackGroud.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);

	return 0;
}

//鼠标消息
VOID CBankBar::OnMouseMove(UINT nFlags, CPoint Point)
{
	__super::OnMouseMove(nFlags, Point);

	//窗口位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//位置计算
	WORD wCollocateHover=SwitchToButtonIndex(Point);

	//盘旋变量
	if (wCollocateHover!=m_wCollocateHover)
	{
		//设置变量
		m_wCollocateHover=wCollocateHover;

		//更新界面
		Invalidate(FALSE);
	}

	//进入判断
	if (m_bHovering==false)
	{
		//设置变量
		m_bHovering=true;

		//变量定义
		TRACKMOUSEEVENT TrackMouseEvent;
		ZeroMemory(&TrackMouseEvent,sizeof(TrackMouseEvent));

		//注册消息
		TrackMouseEvent.hwndTrack=m_hWnd;
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);

		//注册事件
		_TrackMouseEvent(&TrackMouseEvent);
	}

	return;
}

//鼠标消息
VOID CBankBar::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags,Point);

	//激活子项
	if ((m_wCollocateHover!=INVALID_WORD)&&(m_wCollocateHover==m_wCollocateDown))
	{
		SetActiveCollocateItem(m_CollocateItemArray[m_wCollocateHover]);
	}

	//按起处理
	if (m_wCollocateDown!=INVALID_WORD)
	{
		//释放鼠标
		ReleaseCapture();

		//设置变量
		m_wCollocateDown=INVALID_WORD;

		//更新界面
		Invalidate(FALSE);
	}

	return;
}

//鼠标消息
VOID CBankBar::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//设置焦点
	SetFocus();

	//按下处理
	if ((m_wCollocateHover!=INVALID_WORD)&&(m_wCollocateHover!=m_wCollocateActive))
	{
		//鼠标扑获
		SetCapture();

		//设置变量
		m_wCollocateDown=m_wCollocateHover;

		//更新界面
		Invalidate(FALSE);
	}

	return;
}

//鼠标消息
LRESULT CBankBar::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	//更新界面
	if (m_wCollocateHover!=INVALID_WORD) Invalidate(FALSE);

	//设置变量
	m_bHovering=false;
	m_wCollocateHover=INVALID_WORD;

	return 0;
}

//光标消息
BOOL CBankBar::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//设置光标
	if (m_wCollocateHover!=INVALID_WORD)
	{
		SetCursor(LoadCursor(GetModuleHandle(CLIENT_CONTROLS_DLL_NAME),MAKEINTRESOURCE(IDC_HAND_CUR1)));
		return TRUE;
	}

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CBankItem::CBankItem(UINT uIDTemplate,IPlatformResource *pIPlatformResource,tagGlobalUserData *pGlobalUserData) : 
	CDialog(uIDTemplate),m_pIPlatformResource(pIPlatformResource),m_pGlobalUserData(pGlobalUserData)
{
	//获取资源
	HINSTANCE hResInstance=m_pIPlatformResource->GetResInstance();
	IPlazaResource * pIPlazaResource=(IPlazaResource *)m_pIPlatformResource->GetPlazaResource(IID_IPlazaResource,VER_IPlazaResource);
	pIPlazaResource->GetPlazaCollocateImage(m_PlazaCollocateImage);
	tagPlazaCollocateImage & PlazaCollocateImage=m_PlazaCollocateImage;

	//加载资源
	m_EncircleImageFrame.ImageML.SetLoadInfo(PlazaCollocateImage.uTitleML,hResInstance);
	m_EncircleImageFrame.ImageMR.SetLoadInfo(PlazaCollocateImage.uTitleMR,hResInstance);
	m_EncircleImageFrame.ImageTM.SetLoadInfo(PlazaCollocateImage.uTitleTM,hResInstance);
	m_EncircleImageFrame.ImageTR.SetLoadInfo(PlazaCollocateImage.uTitleTR,hResInstance);
	m_EncircleImageFrame.ImageBL.SetLoadInfo(PlazaCollocateImage.uTitleBL,hResInstance);
	m_EncircleImageFrame.ImageBM.SetLoadInfo(PlazaCollocateImage.uTitleBM,hResInstance);
	m_EncircleImageFrame.ImageBR.SetLoadInfo(PlazaCollocateImage.uTitleBR,hResInstance);
	m_EncircleImageFrame.ImageTL.SetLoadInfo(PlazaCollocateImage.uTitleTL,hResInstance);

	m_pIBankSink=NULL;
	//获取大小
	CSkinAide::GetEncircleInfo(m_EncircleInfoFrame,m_EncircleImageFrame);
	return;
}

//析构函数
CBankItem::~CBankItem()
{
}

//绘画标题
VOID CBankItem::DrawBackGround(CDC * pDC, INT nWidth, INT nHeight)
{
	//绘画框架
	CRect rcFrame(0,0,nWidth,nHeight);
	CSkinAide::DrawEncircleFrame(pDC,rcFrame,m_EncircleImageFrame);

	//绘画背景
	CRect rcInside;
	CSkinAide::CaleWindowRect(rcFrame,m_EncircleInfoFrame,rcInside);
	pDC->FillSolidRect(&rcInside,CSkinDialog::m_SkinAttribute.m_crBackGround);

	//设置 DC
	pDC->SetBkMode(TRANSPARENT);
	pDC->SelectObject(CSkinResourceManager::GetDefaultFont());
	pDC->SetTextColor(m_PlazaCollocateImage.crItemTitleTX);

	//绘画标题
	LPCTSTR pszCollocateName=GetCollocateName();
	CRect rcTitil(10,2,nWidth-20,m_EncircleInfoFrame.nTBorder);
	pDC->DrawText(pszCollocateName,lstrlen(pszCollocateName),rcTitil,DT_END_ELLIPSIS|DT_VCENTER|DT_SINGLELINE);

	return;
}

//创建窗口
bool CBankItem::CreateCollocateItem(CWnd * pParentWnd, CRect rcCreate)
{
	//创建窗口
	Create(m_lpszTemplateName,pParentWnd);

	//移动窗口
	SetWindowPos(NULL,rcCreate.left,rcCreate.top,rcCreate.Width(),rcCreate.Height(),SWP_NOZORDER);

	return true;
}

//背景消息
BOOL CBankItem::OnEraseBkgnd(CDC * pDC)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);
	
	//创建缓冲
	CBitmap ImageBuffer;
	ImageBuffer.CreateCompatibleBitmap(pDC,rcClient.Width(),rcClient.Height());

	//创建 DC
	CDC DCBuffer;
	DCBuffer.CreateCompatibleDC(pDC);
	DCBuffer.SelectObject(&ImageBuffer);

	//绘画界面
	DrawBackGround(&DCBuffer,rcClient.Width(),rcClient.Height());

	//绘画界面
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),&DCBuffer,0,0,SRCCOPY);

	//清理资源
	DCBuffer.DeleteDC();
	ImageBuffer.DeleteObject();

	return TRUE;
}

//颜色消息
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

//构造函数
CBankAccess::CBankAccess(IPlatformResource *pIPlatformResource,tagGlobalUserData *pGlobalUserData) : CBankItem(IDD_BANK_ACCESS,pIPlatformResource,pGlobalUserData)
{
	//默认参数
	DefaultParameter();

	return;
}

//析构函数
CBankAccess::~CBankAccess()
{
}

//保存参数
bool CBankAccess::SaveParameter()
{
	
	return true;
}

//默认参数
bool CBankAccess::DefaultParameter()
{

	return true;
}

//更新控制
bool CBankAccess::UpdateControlStatus()
{	
	GetDlgItem(IDC_EDIT_AMOUNT)->SetFocus();
	return true;
}

//获取标题
LPCTSTR CBankAccess::GetCollocateName()
{
	return TEXT("游戏币存取");
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
	//获取数据
	UpdateData();
	CString strText, strPassword;
	GetDlgItem(IDC_EDIT_AMOUNT)->GetWindowText(strText);
	GetDlgItem(IDC_EDIT_BANKPASS)->GetWindowText(strPassword);
    LONGLONG lAmount=_tstoi64(strText);

	//验证数据
	if(lAmount > m_pGlobalUserData->lGold)
	{
		ShowInformationEx(TEXT("系统提示"),TEXT("存入的游戏币不能大于您当前游戏币数量！"), MB_OK|MB_ICONINFORMATION); 
		GetDlgItem(IDC_EDIT_AMOUNT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetSel(0,-1);
		return;
	}
	else if(lAmount < MIN_BANK_COUNT)
	{
		CString strTip;
		strTip.Format(TEXT("保险柜存取游戏币的最小单位为%d"),MIN_BANK_COUNT);
		ShowInformationEx(TEXT("系统提示"),strTip,MB_OK|MB_ICONINFORMATION);
		GetDlgItem(IDC_EDIT_AMOUNT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetSel(0,-1);
		return;
	}
	else if(strPassword.IsEmpty())
	{
		ShowInformationEx(TEXT("系统提示"),TEXT("密码不能为空，请填入正确的保险柜密码！"), MB_OK|MB_ICONINFORMATION); 
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
	//获取数据
	UpdateData();
	CString strText, strPassword;
	GetDlgItem(IDC_EDIT_AMOUNT)->GetWindowText(strText);
	GetDlgItem(IDC_EDIT_BANKPASS)->GetWindowText(strPassword);
    LONGLONG lAmount=_tstoi64(strText);

	//验证数据
	if(lAmount > m_pGlobalUserData->lInsureGold)
	{
		ShowInformationEx(TEXT("系统提示"),TEXT("取出的游戏币不能大于您当前保险柜的游戏币数量！"), MB_OK|MB_ICONINFORMATION); 
		GetDlgItem(IDC_EDIT_AMOUNT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetSel(0,-1);
		return;
	}
	else if(lAmount < MIN_BANK_COUNT)
	{
		CString strTip;
		strTip.Format(TEXT("保险柜存取游戏币的最小单位为%d"),MIN_BANK_COUNT);
		ShowInformationEx(TEXT("系统提示"),strTip,MB_OK|MB_ICONINFORMATION);
		GetDlgItem(IDC_EDIT_AMOUNT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetSel(0,-1);
		return;
	}
	else if(strPassword.IsEmpty())
	{
		ShowInformationEx(TEXT("系统提示"),TEXT("密码不能为空，请填入正确的保险柜密码！"), MB_OK|MB_ICONINFORMATION); 
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

//构造函数
CBankTransfer::CBankTransfer(IPlatformResource *pIPlatformResource,tagGlobalUserData *pGlobalUserData) : CBankItem(IDD_BANK_TRANSFER,pIPlatformResource,pGlobalUserData)
{
	//默认参数
	DefaultParameter();
	m_nAccounts=1;
	//((CButton*)GetDlgItem(IDC_RADIO_ACCOUNT))->SetCheck(true);
	return;
}

//析构函数
CBankTransfer::~CBankTransfer()
{
}
 
//控件绑定
VOID CBankTransfer::DoDataExchange(CDataExchange * pDX)
{
	DDX_Text(pDX, IDC_CUR_GOLD, m_pGlobalUserData->lGold);
	DDX_Text(pDX, IDC_BANK_GOLD, m_pGlobalUserData->lInsureGold);
	DDX_Control(pDX, IDC_BTN_TRANSFER, m_btTransfer);
	DDX_Check(pDX,IDC_RADIO_ACCOUNT,m_nAccounts);
}

//保存参数
bool CBankTransfer::SaveParameter()
{
	return true;
}

//默认参数
bool CBankTransfer::DefaultParameter()
{

	return true;
}

//更新控制
bool CBankTransfer::UpdateControlStatus()
{
	return true;
}

//获取标题
LPCTSTR CBankTransfer::GetCollocateName()
{
	return TEXT("银行转账");
}

void CBankTransfer::OnBnClickedRadioAccount()
{
	GetDlgItem(IDC_STATIC_TYPE)->SetWindowText(TEXT("游戏帐号："));
}

void CBankTransfer::OnBnClickedRadioGameid()
{
	GetDlgItem(IDC_STATIC_TYPE)->SetWindowText(TEXT("游戏ＩＤ："));
}

void CBankTransfer::OnBnClickedBtnTransfer()
{
	UpdateData();
	//获取数据
	LONGLONG lTransfer;
	CString strText;
	GetDlgItem(IDC_EDIT_AMOUNT)->GetWindowText(strText);
	lTransfer=_tstoi64(strText);

	//参数验证
	if (lTransfer < MIN_BANK_COUNT || lTransfer > m_pGlobalUserData->lInsureGold)
	{
		CString strMessage;
		if(lTransfer<MIN_BANK_COUNT)
			strMessage.Format(TEXT("对不起，转帐的最小额度为%d。"),MIN_BANK_COUNT);
		else
			strMessage.Format(TEXT("转帐的数量不能大于您当前保险柜的游戏币,请确保保险柜中有足够的游戏币。"));
		ShowInformationEx(TEXT("系统提示"),strMessage,MB_OK|MB_ICONINFORMATION) ;

		GetDlgItem(IDC_EDIT_AMOUNT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetSel(0,-1);
		return;
	}

	CString strRecvUser;
	GetDlgItem(IDC_EDIT_RECV_ACCOUNT)->GetWindowText(strRecvUser);
	strRecvUser.Trim();
	if(strRecvUser=="")
	{
		ShowInformationEx(TEXT("系统消息"),TEXT("请输入接收赠送的昵称（或）ID"),MB_OK|MB_ICONINFORMATION) ;
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
			ShowInformationEx(TEXT("系统消息"),TEXT("不能转帐给自己"),MB_OK|MB_ICONINFORMATION) ;
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
			ShowInformationEx(TEXT("系统消息"),TEXT("不能赠送给自己"),MB_OK|MB_ICONINFORMATION) ;
			GetDlgItem(IDC_EDIT_RECV_ACCOUNT)->SetFocus();
			return;
		}
		Transfer.bIsAccount=true;
		lstrcpyn(Transfer.szRecvUser,strRecvUser,LEN_ACCOUNTS);
	}
	//密码验证
	TCHAR szPassword[LEN_PASSWORD]=TEXT("");
	GetDlgItemText(IDC_EDIT_BANKPASS,szPassword,CountArray(szPassword));
	if (szPassword[0]==0)
	{
		ShowInformationEx(TEXT("系统消息"),TEXT("密码不能为空，请重新输入密码！") ,MB_OK|MB_ICONINFORMATION) ;
		GetDlgItem(IDC_EDIT_BANKPASS)->SetWindowText("");
		GetDlgItem(IDC_EDIT_BANKPASS)->SetFocus();
		return;
	}
	CMD5Encrypt::EncryptData(szPassword, Transfer.szPassword);

	m_pIBankSink->OnEventBankOperate(BANK_TYPE_QUERY,&Transfer,sizeof(Transfer));

	GetDlgItem(IDC_EDIT_BANKPASS)->SetWindowText(TEXT(""));
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CBankModifyPass::CBankModifyPass(IPlatformResource *pIPlatformResource,tagGlobalUserData *pGlobalUserData) : CBankItem(IDD_BANK_MODIFYPASS,pIPlatformResource,pGlobalUserData)
{
	//默认参数
	DefaultParameter();

	return;
}

//析构函数
CBankModifyPass::~CBankModifyPass()
{
}

//保存参数
bool CBankModifyPass::SaveParameter()
{
	return true;
}

//默认参数
bool CBankModifyPass::DefaultParameter()
{
	return true;
}

//更新控制
bool CBankModifyPass::UpdateControlStatus()
{
	return true;
}

//获取标题
LPCTSTR CBankModifyPass::GetCollocateName()
{
	return TEXT("修改密码");
}

void CBankModifyPass::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, ID_BTN_MODIFY, m_btModifyPass);
}

//////////////////////////////////////////////////////////////////////////


//构造函数
CDlgBank::CDlgBank(IPlatformResource *pIPlatformResource,tagGlobalUserData *pGlobalUserData) : CSkinDialog(IDD),m_CollocateBar(pIPlatformResource),\
	m_BankAccess(pIPlatformResource,pGlobalUserData),m_BankTransfer(pIPlatformResource,pGlobalUserData),m_BankModifyPass(pIPlatformResource,pGlobalUserData),\
	m_pGlobalUserData(pGlobalUserData)
{
	//配置变量
	m_pIPlatformResource=pIPlatformResource;
	m_pIBankSink=NULL;
	return;
}

//析构函数
CDlgBank::~CDlgBank()
{
}

//配置参数
bool CDlgBank::InitBankSink(IBankSink *pIBankSink)
{
	//设置变量
	m_pIBankSink=pIBankSink;
	m_BankAccess.SetBankSink(pIBankSink);
	m_BankTransfer.SetBankSink(pIBankSink);
	m_BankModifyPass.SetBankSink(pIBankSink);

	return true;
}

//控件绑定
VOID CDlgBank::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}

//创建函数
BOOL CDlgBank::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("保险箱"));

	//设置标题
//	m_ImageTitle.LoadImage(AfxGetInstanceHandle(),TEXT("TITLE_DLG_COLLOCATE"));
	CMD_CM_BankUpdate BankUpdate={0};
	lstrcpyn(BankUpdate.szPassword,m_pGlobalUserData->szPassword,LEN_PASSWORD);
	BankUpdate.dwUserID=m_pGlobalUserData->dwUserID;
	if(m_pIBankSink!=NULL)
		m_pIBankSink->OnEventBankOperate(BANK_TYPE_UPDATE,&BankUpdate,sizeof(BankUpdate));

	//创建控制
	AfxSetResourceHandle(GetModuleHandle(CLIENT_CONTROLS_DLL_NAME));
	m_CollocateBar.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,IDC_COLLOCATE_BAR);
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//控制位置
	CRect rcCollocateBar;
	m_CollocateBar.GetWindowRect(&rcCollocateBar);
	m_CollocateBar.SetWindowPos(NULL,GetXBorder()+5,GetCaptionHeight()+5,rcCollocateBar.Width(),rcCollocateBar.Height(),SWP_NOZORDER);

	//子项位置
	CRect rcItemCreate;
	rcItemCreate.top=GetCaptionHeight()+5;
	rcItemCreate.right=rcClient.Width()-GetXBorder()-5;
	rcItemCreate.left=GetXBorder()+rcCollocateBar.Width()+10;
	rcItemCreate.bottom=GetCaptionHeight()+rcCollocateBar.Height()+5;
	m_CollocateBar.SetItemCreateRect(rcItemCreate);

	//移动窗口
	CRect rcButton;
	m_btOk.GetWindowRect(&rcButton);
	SetWindowPos(NULL,0,0,rcClient.Width(),GetCaptionHeight()+GetYBorder()+rcCollocateBar.Height()+rcButton.Height()+15,SWP_NOZORDER|SWP_NOMOVE);

	//移动按钮
	GetClientRect(&rcClient);
	m_btOk.SetWindowPos(NULL,rcClient.Width()-rcButton.Width()*3-GetXBorder()-15,rcClient.Height()-rcButton.Height()-GetYBorder()-3,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_btCancel.SetWindowPos(NULL,rcClient.Width()-rcButton.Width()-GetXBorder()-5,rcClient.Height()-rcButton.Height()-GetYBorder()-3,0,0,SWP_NOZORDER|SWP_NOSIZE);
	
	//全局配置
	m_CollocateBar.InsertCollocateItem(&m_BankAccess);
	m_CollocateBar.InsertCollocateItem(&m_BankTransfer);
	m_CollocateBar.InsertCollocateItem(&m_BankModifyPass);
	
	m_CollocateBar.GetActiveCollocateItem()->UpdateControlStatus();
	return TRUE;
}

//确定函数
VOID CDlgBank::OnOK()
{
	//保存参数
	if (m_CollocateBar.SaveParameter()==false) return;

	__super::OnOK();
}

//默认按钮
VOID CDlgBank::OnBnClickedDefault()
{
	//默认参数
	m_CollocateBar.DefaultParameter();

	return;
}

//刷新界面
bool CDlgBank::UpdateView()
{
	CBankItem *pItem=m_CollocateBar.GetActiveCollocateItem();
	if(pItem!=NULL)
		pItem->UpdateData(FALSE);
	return true;
}

BOOL CDlgBank::PreTranslateMessage(MSG* pMsg)
{
	//按钮过滤
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_RETURN))
	{
		return true;
	}
	return CSkinDialog::PreTranslateMessage(pMsg);
}

//////////////////////////////////////////////////////////////////////////



BOOL CBankAccess::PreTranslateMessage(MSG* pMsg)
{
	//按钮过滤
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
	//按钮过滤
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
	//按钮过滤
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
//变量定义
	TCHAR szSrcPassword[LEN_PASSWORD];
	TCHAR szDesPassword[2][LEN_PASSWORD];

	//获取信息
	GetDlgItemText(IDC_EDIT_OLDPASS,szSrcPassword,CountArray(szSrcPassword));
	GetDlgItemText(IDC_EDIT_NEWPASS,szDesPassword[0],CountArray(szDesPassword[0]));
	GetDlgItemText(IDC_EDIT_VERIFYPASS,szDesPassword[1],CountArray(szDesPassword[1]));

	//密码判断
	if (szSrcPassword[0]==0)
	{
		ShowInformation(TEXT("请输入原保险柜密码！"),MB_ICONERROR,0);
		GetDlgItem(IDC_EDIT_OLDPASS)->SetFocus();
		return;
	}

	//密码判断
	if (lstrcmp(szDesPassword[0],szDesPassword[1])!=0)
	{
		ShowInformation(TEXT("两次输入的保险柜密码不一致，请重新输入！"),MB_ICONERROR,0);
		GetDlgItem(IDC_EDIT_VERIFYPASS)->SetFocus();
		return;
	}

	//密码判断
	if (szDesPassword[0]==0)
	{
		ShowInformation(TEXT("新保险柜密码不能为空，请重新输入！"),MB_ICONERROR,0);
		GetDlgItem(IDC_EDIT_NEWPASS)->SetFocus();
		return;
	}

	//过滤少于6位的密码
	TCHAR szPassword[LEN_PASSWORD] = {0};
	lstrcpyn( szPassword,szDesPassword[0],CountArray(szPassword) );
	INT nPassLen = lstrlen(szPassword);
	if( nPassLen < 6 )
	{
		ShowInformation(TEXT("新密码不能少于6位，请重新输入！"),MB_ICONERROR);
		GetDlgItem(IDC_EDIT_NEWPASS)->SetFocus();
		return;
	}

	//过滤简单密码
	for( INT i = 0; i < nPassLen; i++ )
	{
		if( szPassword[i] < '0' || szPassword[i] > 'z' ||
			( szPassword[i] > '9' && szPassword[i] < 'A' ) || 
			( szPassword[i] > 'Z' && szPassword[i] < 'a' ) )
			break;
	}
	if( i == nPassLen )
	{
		//排序
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
					//设置标志
					bSorted=false;

					//扑克数据
					chSwap=szPassword[i];
					szPassword[i]=szPassword[i+1];
					szPassword[i+1]=chSwap;
				}	
			}
			nLast--;
		} while(bSorted==false);

		//判断是否相连或相同
		for( INT i = 1; i < nPassLen; i++ )
		{
			if( szPassword[0] == szPassword[i]+i ||
				szPassword[0] == szPassword[i] )
				continue;

			break;
		}
		if( i == nPassLen )
		{
			ShowInformation(TEXT("新密码不能是相同或连着的字母数字，请重新输入！"),MB_ICONERROR);
			GetDlgItem(IDC_EDIT_NEWPASS)->SetFocus();
			return;
		}
	}

	//变量定义
	CMD_GP_ModifyInsurePass ModifyInsurePass;
	ZeroMemory(&ModifyInsurePass,sizeof(ModifyInsurePass));

	//加密密码
	TCHAR szOldPassword[LEN_PASSWORD]=TEXT("");
	TCHAR szNewPassword[LEN_PASSWORD]=TEXT("");
	CMD5Encrypt::EncryptData(szSrcPassword,szOldPassword);
	CMD5Encrypt::EncryptData(szDesPassword[0],szNewPassword);

	//构造数据
	ModifyInsurePass.dwUserID=m_pGlobalUserData->dwUserID;
	lstrcpynW(ModifyInsurePass.szScrPassword,CT2CW(szOldPassword),CountArray(ModifyInsurePass.szScrPassword));
	lstrcpynW(ModifyInsurePass.szDesPassword,CT2CW(szNewPassword),CountArray(ModifyInsurePass.szDesPassword));

	//发送数据
	m_pIBankSink->OnEventBankOperate(BANK_TYPE_MODIFYPASS, &ModifyInsurePass,sizeof(ModifyInsurePass));
	GetDlgItem(IDC_EDIT_OLDPASS)->SetWindowText(TEXT(""));
}

