#include "Stdafx.h"
#include "Resource.h"
#include "ClubServerDlg.h"

//////////////////////////////////////////////////////////////////////////
//��Ϣ����

#define WM_PROCESS_CMD_LINE			(WM_USER+100)						//��������

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CClubServerDlg, CDialog)
	ON_WM_QUERYENDSESSION()
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedStopService)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)
	ON_BN_CLICKED(IDC_OPTION_SERVICE, OnBnClickedOptionService)

	//�Զ���Ϣ
	ON_MESSAGE(WM_PROCESS_CMD_LINE, OnMessageProcessCmdLine)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CClubServerDlg::CClubServerDlg() : CDialog(IDD_DLG_CLUB_SERVER)
{
}

//��������
CClubServerDlg::~CClubServerDlg()
{
}

//�ؼ���
VOID CClubServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_RichEditTrace);
}

//��ʼ������
BOOL CClubServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//���ñ���
	SetWindowText(TEXT("���ֲ����� -- [ ֹͣ ]"));

	//����ͼ��
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	//�����
	LPCTSTR pszCmdLine = AfxGetApp()->m_lpCmdLine;
	if (pszCmdLine[0] != 0) PostMessage(WM_PROCESS_CMD_LINE, 0, (LPARAM)pszCmdLine);

	return TRUE;
}

//ȷ����Ϣ
VOID CClubServerDlg::OnOK()
{
	return;
}

//ȡ������
VOID CClubServerDlg::OnCancel()
{
	if (m_ServiceUnits.GetServiceStatus() != SERVICE_STATUS_STOP)
	{
		LPCTSTR pszQuestion = TEXT("���ֲ��������������У���ȷʵҪ�رշ�������");
		if (AfxMessageBox(pszQuestion, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDYES) return;
	}

	//ֹͣ����
	m_ServiceUnits.ConcludeService();

	__super::OnCancel();
}

//��Ϣ����
BOOL CClubServerDlg::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//����״̬
VOID CClubServerDlg::OnServiceUnitsStatus(WORD ServiceStatus)
{
	//״̬����
	switch (ServiceStatus)
	{
	case SERVICE_STATUS_STOP:	//ֹͣ״̬
	{
		//���ñ���
		SetWindowStatusText(TEXT("ֹͣ"));

		//���ð�ť
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_OPTION_SERVICE)->EnableWindow(TRUE);

		//��ʾ��Ϣ
		LPCTSTR pszDescribe = TEXT("����ֹͣ�ɹ�");
		CTraceService::TraceString(pszDescribe, TraceLevel_Normal);

		break;
	}
	case SERVICE_STATUS_CONFIG:	//����״̬
	{
		//���ñ���
		SetWindowStatusText(TEXT("��ʼ�� "));

		//���ð�ť
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_OPTION_SERVICE)->EnableWindow(FALSE);

		//��ʾ��Ϣ
		LPCTSTR pszDescribe = TEXT("���ڳ�ʼ�����...");
		CTraceService::TraceString(pszDescribe, TraceLevel_Normal);

		break;
	}
	case SERVICE_STATUS_SERVICE:	//����״̬
	{
		//���ñ���
		SetWindowStatusText(TEXT("����"));

		//���ð�ť
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_OPTION_SERVICE)->EnableWindow(FALSE);

		//��ʾ��Ϣ
		LPCTSTR pszDescribe = TEXT("���������ɹ�");
		CTraceService::TraceString(pszDescribe, TraceLevel_Normal);

		break;
	}
	}

	return;
}

//���ñ���
VOID CClubServerDlg::SetWindowStatusText(LPCTSTR pszServiceStatus)
{
	//��ȡ����
	tagServiceOption * pServiceOption = m_ServiceUnits.GetServiceOption();
	ASSERT(pServiceOption != NULL);

	//��������
	TCHAR szWindowText[128] = TEXT("");
	_sntprintf_s(szWindowText, CountArray(szWindowText), TEXT("���ֲ�����[%d] -- [ %s ]"), pServiceOption->wServicePort, pszServiceStatus);

	//���ñ���
	SetWindowText(szWindowText);

	return;
}

//��������
VOID CClubServerDlg::OnBnClickedStartService()
{
	//���ýӿ�
	if (m_ServiceUnits.SetServiceUnitsSink(this) == false)
	{
		CTraceService::TraceString(TEXT("�󶨷������״̬�ӿ�ʧ�� "), TraceLevel_Exception);
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

//ֹͣ����
VOID CClubServerDlg::OnBnClickedStopService()
{
	//ֹͣ����
	try
	{
		m_ServiceUnits.ConcludeService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//���÷���
VOID CClubServerDlg::OnBnClickedOptionService()
{
	//��������
	CDlgServiceOption DlgServiceOption;
	DlgServiceOption.SetDataFileName(TEXT("Club.Data"));

	if (DlgServiceOption.DoModal() == IDOK)
	{
		//�����־
		CTraceService::TraceString(TEXT("�������ü��سɹ�!"), TraceLevel_Normal);

		//��������
		m_ServiceUnits.SetServiceOption(*DlgServiceOption.GetServiceOption());

		//���ð�ť
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);

		return;
	}
}

//�ر�ѯ��
BOOL CClubServerDlg::OnQueryEndSession()
{
	//��ʾ��Ϣ
	if (m_ServiceUnits.GetServiceStatus() != SERVICE_STATUS_STOP)
	{
		CTraceService::TraceString(TEXT("�������������У�ϵͳҪ��ע���ػ�����ʧ��"), TraceLevel_Warning);
		return FALSE;
	}

	return TRUE;
}


//�����
LRESULT CClubServerDlg::OnMessageProcessCmdLine(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR lpszComLine = (LPCTSTR)(lParam);

	//��������
	if (lpszComLine[0] != 0)
	{
		//��������
		WORD wServiceID = 0;

		//��ȡ����
		UINT nStringLength = 0;
		CString strItemString;
		LPCTSTR lpszBeginString = lpszComLine;

		//���ط����ʶ
		LPCTSTR pszServiceID = TEXT("/ServiceID:");
		UINT nServiceIDLength = lstrlen(pszServiceID);

		//��ȡ����
		while (true)
		{
			//��������
			LPCTSTR lpszEndString = _tcschr(lpszBeginString, TEXT(' '));
			nStringLength = (lpszEndString == NULL) ? lstrlen(lpszBeginString) : (UINT)(lpszEndString - lpszBeginString);

			//���䴰��
			if ((nStringLength >= nServiceIDLength) && (memcmp(lpszBeginString, pszServiceID, nServiceIDLength * sizeof(TCHAR)) == 0))
			{
				//��ȡ����
				CopyMemory(strItemString.GetBufferSetLength(nStringLength - nServiceIDLength), lpszBeginString + nServiceIDLength,
					(nStringLength - nServiceIDLength) * sizeof(TCHAR));
				strItemString.ReleaseBuffer();

				//��ȡ����
				wServiceID = (WORD)(_tstoi(strItemString));

				break;
			}

			//���ñ���
			if (lpszEndString == NULL) break;
			lpszBeginString = (lpszEndString + 1);
		}

		if (wServiceID != 0)
		{
			//�������
			CDlgServiceOption DlgServiceOption;
			DlgServiceOption.SetDataFileName(TEXT("Club.Data"));

			//��ȡ����
			tagServiceOption * pServiceOption = DlgServiceOption.GetServiceOption(wServiceID);
			if (pServiceOption != NULL)
			{
				//�����־
				CTraceService::TraceString(TEXT("�������ü��سɹ���"), TraceLevel_Normal);

				//��������
				m_ServiceUnits.SetServiceOption(*pServiceOption);

				//���ð�ť
				GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
				GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);
			}
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
