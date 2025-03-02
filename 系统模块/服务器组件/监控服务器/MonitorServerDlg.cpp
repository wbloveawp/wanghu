#include "Stdafx.h"
#include "Resource.h"
#include "MonitorServerDlg.h"

//////////////////////////////////////////////////////////////////////////
//��Ϣ����

#define WM_PROCESS_CMD_LINE			(WM_USER+100)						//��������

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMonitorServerDlg, CDialog)
	ON_WM_QUERYENDSESSION()
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedConcludeService)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)
	ON_BN_CLICKED(IDC_OPTION_SERVICE, OnBnClickedOptionService)

	//�Զ���Ϣ
	ON_MESSAGE(WM_PROCESS_CMD_LINE, OnMessageProcessCmdLine)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CMonitorServerDlg::CMonitorServerDlg() : CDialog(IDD_DLG_MONITOR_SERVER)
{
}

//��������
CMonitorServerDlg::~CMonitorServerDlg()
{
}

//�ؼ���
VOID CMonitorServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_RichEditTrace);
}

//��ʼ������
BOOL CMonitorServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//���ñ���
	SetWindowText(TEXT("��ط����� -- [ ֹͣ ]"));

	//����ͼ��
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//��ʼ������
	CInitParameter * pInitParameter=m_ServiceUnits.GetInitParameter();

	//��������
	m_ModuleDBParameter.SetPlatformDBParameter(pInitParameter->m_PlatformDBParameter);	

	//�����
	LPCTSTR pszCmdLine = AfxGetApp()->m_lpCmdLine;
	if (pszCmdLine[0] != 0) PostMessage(WM_PROCESS_CMD_LINE, 0, (LPARAM)pszCmdLine);

	return TRUE;
}

//ȷ����Ϣ
VOID CMonitorServerDlg::OnOK()
{
	return;
}

//ȡ������
VOID CMonitorServerDlg::OnCancel()
{
	if (m_ServiceUnits.GetServiceStatus()!=SERVICE_STATUS_STOP)
	{
		LPCTSTR pszQuestion=TEXT("��ط��������������У�����ֹͣ����");
		AfxMessageBox(pszQuestion, MB_OK);
		return;		
	}

	//ֹͣ����
	m_ServiceUnits.ConcludeService();

	__super::OnCancel();
}

//��Ϣ����
BOOL CMonitorServerDlg::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//����״̬
VOID CMonitorServerDlg::OnServiceUnitsStatus(WORD ServiceStatus)
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
			CTraceService::TraceString(TEXT("����ֹͣ�ɹ�"),TraceLevel_Normal);

			break;
		}
	case SERVICE_STATUS_CONFIG:	//����״̬
		{
			//���ñ���
			SetWindowStatusText(TEXT("��ʼ��"));

			//���ð�ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_OPTION_SERVICE)->EnableWindow(FALSE);

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ڳ�ʼ�����..."),TraceLevel_Normal);

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
			CTraceService::TraceString(TEXT("���������ɹ�"),TraceLevel_Normal);

			break;
		}
	}

	return;
}

//���ñ���
VOID CMonitorServerDlg::SetWindowStatusText(LPCTSTR pszServiceStatus)
{
	//��ȡ����
	const CInitParameter * pInitParameter = m_ServiceUnits.GetInitParameter();
	ASSERT(pInitParameter!=NULL);

	//Լս����
	const tagServiceOption * pServiceOption = m_ServiceUnits.GetServiceOption();
	ASSERT(pServiceOption !=NULL);
	
	//��������
	TCHAR szWindowText[128]=TEXT("");
	_sntprintf_s(szWindowText,CountArray(szWindowText),TEXT("��ط�����[%d] -- [ %s ]"), pServiceOption->wServicePort,pszServiceStatus);

	//���ñ���
	SetWindowText(szWindowText);

	return;
}

//��������
VOID CMonitorServerDlg::OnBnClickedStartService()
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

//ֹͣ����
VOID CMonitorServerDlg::OnBnClickedConcludeService()
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

//���÷���
VOID CMonitorServerDlg::OnBnClickedOptionService()
{	
	//������Դ
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//��������
	CDlgServiceOption DlgServiceOption;
	DlgServiceOption.SetDataFileName(TEXT("Monitor.Data"));

	if (DlgServiceOption.DoModal() == IDOK)
	{
		//�����־
		CTraceService::TraceString(TEXT("�������ü��سɹ���"), TraceLevel_Normal);

		//��������
		m_ServiceUnits.SetServiceOption(*DlgServiceOption.GetServiceOption());

		//���ð�ť
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);

		return;
	}
}

//�ر�ѯ��
BOOL CMonitorServerDlg::OnQueryEndSession()
{
	//��ʾ��Ϣ
	if (m_ServiceUnits.GetServiceStatus()!=SERVICE_STATUS_STOP)
	{
		CTraceService::TraceString(TEXT("�������������У�ϵͳҪ��ע���ػ�����ʧ��"),TraceLevel_Warning);
		return FALSE;
	}

	return TRUE;
}


//�����
LRESULT CMonitorServerDlg::OnMessageProcessCmdLine(WPARAM wParam, LPARAM lParam)
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
			DlgServiceOption.SetDataFileName(TEXT("Monitor.Data"));

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
