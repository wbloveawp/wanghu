#include "StdAfx.h"
#include "Resource.h"
#include "CollocateDlg.h"

//////////////////////////////////////////////////////////////////////////////////

//页面控件
#define ID_TAB_VIEW_ITEM_1			0									//信息页面

//时间定义
#define IDI_DEBUG_CHECK_TIMER		100									//调试时间

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCollocateDlg, CDialog)
	ON_WM_TIMER()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CONTROL, OnTcnSelchangeTabControl)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CCollocateDlg::CCollocateDlg() : CDialog(IDD_DLG_COLLOCATE)
{
}

//析构函数
CCollocateDlg::~CCollocateDlg()
{
}

//控件绑定
VOID CCollocateDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_CONTROL, m_TabControl);
}

//配置函数
BOOL CCollocateDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//设置图标
	HINSTANCE hInstance = AfxGetInstanceHandle();
	SetIcon(LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINFRAME)), TRUE);
	SetIcon(LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINFRAME)), FALSE);

	//加入标签
	m_TabControl.InsertItem(ID_TAB_VIEW_ITEM_1, TEXT(" 授权配置 "));

	//获取位置
	CRect rcItemView;
	m_TabControl.GetWindowRect(&rcItemView);

	//计算位置
	ScreenToClient(&rcItemView);
	m_TabControl.AdjustRect(FALSE, &rcItemView);

	//创建窗口
	m_DlgTabViewItem1.Create(IDD_TAB_VIEW_ITEM_1, this);
	m_DlgTabViewItem1.SetWindowPos(NULL, rcItemView.left, rcItemView.top, rcItemView.Width(), rcItemView.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);

	//调试检测
#ifdef DISABLE_DEBUG

	//启动线程
	m_InsideThread.StartThread();

	//设置时间
	SetTimer(IDI_DEBUG_CHECK_TIMER, 100L, NULL);

#endif

	return TRUE;
}

//消息解释
BOOL CCollocateDlg::PreTranslateMessage(MSG * pMsg)
{
	//按键过滤
	if ((pMsg->message == WM_KEYDOWN) && ((pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN)))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//时间消息
VOID CCollocateDlg::OnTimer(UINT_PTR nIDEvent)
{
	//防止调试
#ifdef DISABLE_DEBUG

	//调试检测
	if (nIDEvent == IDI_DEBUG_CHECK_TIMER)
	{
		if (IsDebuggerPresent() == TRUE)
		{
			//关闭程序
			PostMessage(WM_CLOSE, 0L, 0L);

			//删除时间
			KillTimer(IDI_DEBUG_CHECK_TIMER);
		}

		return;
	}

#endif

	__super::OnTimer(nIDEvent);
}

//类型改变
VOID CCollocateDlg::OnTcnSelchangeTabControl(NMHDR * pNMHDR, LRESULT * pResult)
{
	//设置页面
	INT nCurrentSel = m_TabControl.GetCurSel();
	m_DlgTabViewItem1.ShowWindow((nCurrentSel == ID_TAB_VIEW_ITEM_1) ? SW_SHOW : SW_HIDE);

	return;
}

//////////////////////////////////////////////////////////////////////////////////
