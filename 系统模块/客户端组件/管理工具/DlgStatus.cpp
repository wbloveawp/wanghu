#include "Stdafx.h"
#include "Resource.h"
#include "DlgStatus.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDT_STATUS					100									//状态时间

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgStatus, CDialog)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_GIVE_UP, OnBnClickedGiveUp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgStatus::CDlgStatus() : CDialog(IDD_DLG_STATUS)
{
	//设置变量
	m_uCommomID=0;
	m_hNotifyWnd=NULL;

	return;
}

//析构函数
CDlgStatus::~CDlgStatus()
{
}

//控件绑定
VOID CDlgStatus::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GIVE_UP, m_btGiveUp);
	DDX_Control(pDX, IDC_PROGRESS, m_ProgressCtrl);
}

//初始化函数
BOOL CDlgStatus::OnInitDialog()
{
	__super::OnInitDialog();

	//设置控件
	SetTimer(IDT_STATUS,50,NULL);
	m_ProgressCtrl.SetRange(0,199);

	//设置广告
	CGlobalStation * pGlobalStation=CGlobalStation::GetInstance();
	((CStatic *)GetDlgItem(IDC_PUBLICIZE_LOGO))->SetBitmap(pGlobalStation->GetPublicizeImage());

	return TRUE;
}

//设置消息
VOID CDlgStatus::ShowStatusWnd(LPCTSTR pszDescribe, HWND hNotifyWnd, UINT uCommomID)
{
	//设置变量
	m_uCommomID=uCommomID;
	m_hNotifyWnd=hNotifyWnd;

	//创建窗口
	if (m_hWnd==NULL)
	{
		Create(IDD_DLG_STATUS,AfxGetMainWnd());
	}

	//设置描述
	SetDlgItemText(IDC_STATUS,pszDescribe);

	//更新窗口
	UpdateWindow();

	//显示窗口
	ShowWindow(SW_SHOW);
	SetForegroundWindow();

	return;
}

//时间消息
VOID CDlgStatus::OnTimer(UINT_PTR nIDEvent)
{
	//设置进度
	if (nIDEvent==IDT_STATUS)
	{
		m_ProgressCtrl.SetPos((m_ProgressCtrl.GetPos()+1)%200);
		return;
	}

	__super::OnTimer(nIDEvent);
}

//鼠标消息
VOID CDlgStatus::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//模拟标题
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));

	return;
}

//取消按钮
VOID CDlgStatus::OnBnClickedGiveUp()
{
	//发送消息
	::PostMessage(m_hNotifyWnd,WM_COMMAND,m_uCommomID,0);

	return;
}

//控件颜色
HBRUSH CDlgStatus::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_STATIC:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(CSkinDialog::m_SkinAttribute.m_crControlText);
			return CSkinDialog::m_SkinAttribute.m_brBackGround;
		}
	}

	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

//////////////////////////////////////////////////////////////////////////
