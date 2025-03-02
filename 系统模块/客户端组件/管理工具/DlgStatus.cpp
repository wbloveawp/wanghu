#include "Stdafx.h"
#include "Resource.h"
#include "DlgStatus.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDT_STATUS					100									//״̬ʱ��

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgStatus, CDialog)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_GIVE_UP, OnBnClickedGiveUp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgStatus::CDlgStatus() : CDialog(IDD_DLG_STATUS)
{
	//���ñ���
	m_uCommomID=0;
	m_hNotifyWnd=NULL;

	return;
}

//��������
CDlgStatus::~CDlgStatus()
{
}

//�ؼ���
VOID CDlgStatus::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GIVE_UP, m_btGiveUp);
	DDX_Control(pDX, IDC_PROGRESS, m_ProgressCtrl);
}

//��ʼ������
BOOL CDlgStatus::OnInitDialog()
{
	__super::OnInitDialog();

	//���ÿؼ�
	SetTimer(IDT_STATUS,50,NULL);
	m_ProgressCtrl.SetRange(0,199);

	//���ù��
	CGlobalStation * pGlobalStation=CGlobalStation::GetInstance();
	((CStatic *)GetDlgItem(IDC_PUBLICIZE_LOGO))->SetBitmap(pGlobalStation->GetPublicizeImage());

	return TRUE;
}

//������Ϣ
VOID CDlgStatus::ShowStatusWnd(LPCTSTR pszDescribe, HWND hNotifyWnd, UINT uCommomID)
{
	//���ñ���
	m_uCommomID=uCommomID;
	m_hNotifyWnd=hNotifyWnd;

	//��������
	if (m_hWnd==NULL)
	{
		Create(IDD_DLG_STATUS,AfxGetMainWnd());
	}

	//��������
	SetDlgItemText(IDC_STATUS,pszDescribe);

	//���´���
	UpdateWindow();

	//��ʾ����
	ShowWindow(SW_SHOW);
	SetForegroundWindow();

	return;
}

//ʱ����Ϣ
VOID CDlgStatus::OnTimer(UINT_PTR nIDEvent)
{
	//���ý���
	if (nIDEvent==IDT_STATUS)
	{
		m_ProgressCtrl.SetPos((m_ProgressCtrl.GetPos()+1)%200);
		return;
	}

	__super::OnTimer(nIDEvent);
}

//�����Ϣ
VOID CDlgStatus::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//ģ�����
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));

	return;
}

//ȡ����ť
VOID CDlgStatus::OnBnClickedGiveUp()
{
	//������Ϣ
	::PostMessage(m_hNotifyWnd,WM_COMMAND,m_uCommomID,0);

	return;
}

//�ؼ���ɫ
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
