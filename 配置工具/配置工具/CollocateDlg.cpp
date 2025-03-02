#include "StdAfx.h"
#include "Resource.h"
#include "CollocateDlg.h"

//////////////////////////////////////////////////////////////////////////////////

//ҳ��ؼ�
#define ID_TAB_VIEW_ITEM_1			0									//��Ϣҳ��

//ʱ�䶨��
#define IDI_DEBUG_CHECK_TIMER		100									//����ʱ��

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCollocateDlg, CDialog)
	ON_WM_TIMER()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CONTROL, OnTcnSelchangeTabControl)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CCollocateDlg::CCollocateDlg() : CDialog(IDD_DLG_COLLOCATE)
{
}

//��������
CCollocateDlg::~CCollocateDlg()
{
}

//�ؼ���
VOID CCollocateDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_CONTROL, m_TabControl);
}

//���ú���
BOOL CCollocateDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//����ͼ��
	HINSTANCE hInstance = AfxGetInstanceHandle();
	SetIcon(LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINFRAME)), TRUE);
	SetIcon(LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINFRAME)), FALSE);

	//�����ǩ
	m_TabControl.InsertItem(ID_TAB_VIEW_ITEM_1, TEXT(" ��Ȩ���� "));

	//��ȡλ��
	CRect rcItemView;
	m_TabControl.GetWindowRect(&rcItemView);

	//����λ��
	ScreenToClient(&rcItemView);
	m_TabControl.AdjustRect(FALSE, &rcItemView);

	//��������
	m_DlgTabViewItem1.Create(IDD_TAB_VIEW_ITEM_1, this);
	m_DlgTabViewItem1.SetWindowPos(NULL, rcItemView.left, rcItemView.top, rcItemView.Width(), rcItemView.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);

	//���Լ��
#ifdef DISABLE_DEBUG

	//�����߳�
	m_InsideThread.StartThread();

	//����ʱ��
	SetTimer(IDI_DEBUG_CHECK_TIMER, 100L, NULL);

#endif

	return TRUE;
}

//��Ϣ����
BOOL CCollocateDlg::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message == WM_KEYDOWN) && ((pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN)))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//ʱ����Ϣ
VOID CCollocateDlg::OnTimer(UINT_PTR nIDEvent)
{
	//��ֹ����
#ifdef DISABLE_DEBUG

	//���Լ��
	if (nIDEvent == IDI_DEBUG_CHECK_TIMER)
	{
		if (IsDebuggerPresent() == TRUE)
		{
			//�رճ���
			PostMessage(WM_CLOSE, 0L, 0L);

			//ɾ��ʱ��
			KillTimer(IDI_DEBUG_CHECK_TIMER);
		}

		return;
	}

#endif

	__super::OnTimer(nIDEvent);
}

//���͸ı�
VOID CCollocateDlg::OnTcnSelchangeTabControl(NMHDR * pNMHDR, LRESULT * pResult)
{
	//����ҳ��
	INT nCurrentSel = m_TabControl.GetCurSel();
	m_DlgTabViewItem1.ShowWindow((nCurrentSel == ID_TAB_VIEW_ITEM_1) ? SW_SHOW : SW_HIDE);

	return;
}

//////////////////////////////////////////////////////////////////////////////////
