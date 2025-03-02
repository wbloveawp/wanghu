#include "Stdafx.h"
#include "Resource.h"
#include "CenterServerDlg.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCenterServerDlg, CDialog)
	ON_WM_QUERYENDSESSION()
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedConcludeService)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)
	ON_BN_CLICKED(IDC_DEBUG_SERVICE, OnBnClickedDebugService)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CCenterServerDlg::CCenterServerDlg() : CDialog(IDD_DLG_CENTER_SERVER)
{
}

//��������
CCenterServerDlg::~CCenterServerDlg()
{
}

//�ؼ���
VOID CCenterServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_RichEditTrace);
}

//��ʼ������
BOOL CCenterServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//���ñ���
	SetWindowText(TEXT("���ķ����� -- [ ֹͣ ]"));

	//����ͼ��
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	return TRUE;
}

//ȷ����Ϣ
VOID CCenterServerDlg::OnOK()
{
	return;
}

//ȡ������
VOID CCenterServerDlg::OnCancel()
{
	if (m_ServiceUnits.GetServiceStatus()!=SERVICE_STATUS_STOP)
	{
		LPCTSTR pszQuestion=TEXT("���ķ��������������У���ȷʵҪ�رշ�������");
		if (AfxMessageBox(pszQuestion,MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION)!=IDYES) return;
	}

	//ֹͣ����
	m_ServiceUnits.ConcludeService();

	__super::OnCancel();
}

//��Ϣ����
BOOL CCenterServerDlg::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//����״̬
VOID CCenterServerDlg::OnServiceUnitsStatus(WORD ServiceStatus)
{
	//״̬����
	switch (ServiceStatus)
	{
	case SERVICE_STATUS_STOP:	//ֹͣ״̬
		{
			//���ñ���
			SetWindowText(TEXT("���ķ����� -- [ ֹͣ ]"));

			//���ð�ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("����ֹͣ�ɹ�"),TraceLevel_Normal);

			break;
		}
	case SERVICE_STATUS_CONFIG:	//����״̬
		{
			//���ñ���
			SetWindowText(TEXT("���ķ����� -- [ ��ʼ�� ]"));

			//���ð�ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ڳ�ʼ�����..."),TraceLevel_Normal);

			break;
		}
	case SERVICE_STATUS_SERVICE:	//����״̬
		{
			//���ñ���
			SetWindowText(TEXT("���ķ����� -- [ ���� ]"));

			//���ð�ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���������ɹ�"),TraceLevel_Normal);

			break;
		}
	}

	return;
}

//��������
VOID CCenterServerDlg::OnBnClickedStartService()
{
	//���ýӿ�
	if (m_ServiceUnits.SetServiceUnitsSink(this)==false)
	{
		CTraceService::TraceString(TEXT("�󶨷������״̬�ӿ�ʧ��"),TraceLevel_Exception);
		return;
	}

	//��������
	try
	{
		m_ServiceUnits.StartService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//���Է���
VOID CCenterServerDlg::OnBnClickedDebugService()
{
	//��������
	try
	{
		m_ServiceUnits.DebugService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//ֹͣ����
VOID CCenterServerDlg::OnBnClickedConcludeService()
{
	//ֹͣ����
	try
	{
		m_ServiceUnits.ConcludeService(true);
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//�ر�ѯ��
BOOL CCenterServerDlg::OnQueryEndSession()
{
	//��ʾ��Ϣ
	if (m_ServiceUnits.GetServiceStatus()!=SERVICE_STATUS_STOP)
	{
		CTraceService::TraceString(TEXT("�������������У�ϵͳҪ��ע���ػ�����ʧ��"),TraceLevel_Warning);
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
