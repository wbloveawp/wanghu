#include "Stdafx.h"
#include "Resource.h"
#include "GlobalUnits.h"
#include "GameServerDlg.h"
#include "DlgServerList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//��Ϣ����

#define WM_PROCESS_CMD_LINE			(WM_USER+100)						//��������

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameServerDlg, CDialog)
	ON_WM_QUERYENDSESSION()
	ON_BN_CLICKED(IDC_OPEN_SERVER, OnBnClickedOpenServer)
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedConcludeService)
	ON_BN_CLICKED(IDC_DEBUG_SERVICE, OnBnClickedDebugService)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)
	ON_BN_CLICKED(IDC_CREATE_SERVER, OnBnClickedCreateServer)
	ON_BN_CLICKED(IDC_OPTION_SERVER, OnBnClickedOptionServer)	
	


	//�Զ���Ϣ
	ON_MESSAGE(WM_PROCESS_CMD_LINE,OnMessageProcessCmdLine)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameServerDlg::CGameServerDlg() : CDialog(IDD_DLG_GAME_SERVER)
{
	//ģ�����
	m_bOptionSuccess=false;	
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	return;
}

//��������
CGameServerDlg::~CGameServerDlg()
{
}

//�ؼ���
VOID CGameServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_RichEditTrace);
}

//��ʼ������
BOOL CGameServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//���ñ���
	SetWindowText(TEXT("��Ϸ������ -- [ ֹͣ ]"));

	//����ͼ��
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//��Ϣ���
	if (CTraceService::SetTraceService(QUERY_OBJECT_INTERFACE(m_RichEditTrace,IUnknownEx))==false)
	{
		CTraceService::TraceString(TEXT("��Ϣ����ӿ�����ʧ�ܣ�ϵͳ������Ϣ���޷���ʾ"),TraceLevel_Exception);
	}

	//�����
	LPCTSTR pszCmdLine=AfxGetApp()->m_lpCmdLine;
	if (pszCmdLine[0]!=0) PostMessage(WM_PROCESS_CMD_LINE,0,(LPARAM)pszCmdLine);

	//�������
	if (m_ServiceUnitsModule.GetInterface() == NULL)
	{
		//�������
		if (m_ServiceUnitsModule.CreateInstance() == false)
		{
			CTraceService::TraceString(TEXT("��Ϸ�����������ʧ��"), TraceLevel_Exception);			
		}

		//���ýӿ�
		if (m_ServiceUnitsModule->SetServiceUnitsSink(this) == false)
		{
			CTraceService::TraceString(TEXT("�󶨷������״̬�ӿ�ʧ��"), TraceLevel_Exception);			
		}
	}

	return TRUE;
}

//ȷ����Ϣ
VOID CGameServerDlg::OnOK()
{
	return;
}

//ȡ������
VOID CGameServerDlg::OnCancel()
{
	//�ر�ѯ��
	if (m_ServiceUnitsModule.GetInterface()!=NULL)
	{
		if (m_ServiceUnitsModule->GetServiceStatus()!=SERVICE_STATUS_STOP)
		{
			LPCTSTR pszQuestion=TEXT("��Ϸ���������������У���ȷʵҪ�رշ�������");
			if (AfxMessageBox(pszQuestion,MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION)!=IDYES) return;
		}

		//ֹͣ����
		if (!m_ServiceUnitsModule->ConcludeService())
			return;
	}

	__super::OnCancel();
}

//��Ϣ����
BOOL CGameServerDlg::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//����״̬
VOID CGameServerDlg::OnServiceUnitsStatus(WORD ServiceStatus)
{
	//״̬����
	switch (ServiceStatus)
	{
	case SERVICE_STATUS_STOP:	//ֹͣ״̬
		{
			//���±���
			UpdateServerTitle();

			//���ð�ť
			GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(TRUE);
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(TRUE);
			GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(TRUE);
			GetDlgItem(IDC_RUN_PARAMETER)->EnableWindow(FALSE);
			GetDlgItem(IDC_SERVICE_CONTROL)->EnableWindow(FALSE);

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("����ֹͣ�ɹ�"),TraceLevel_Normal);

			//�ر��ж�
			if (g_GlobalUnits.m_bFailureClose==true) PostMessage(WM_CLOSE);

			break;
		}
	case SERVICE_STATUS_CONFIG:	//����״̬
		{
			//���±���
			UpdateServerTitle();

			//���ð�ť
			GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_RUN_PARAMETER)->EnableWindow(FALSE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_SERVICE_CONTROL)->EnableWindow(FALSE);

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ڳ�ʼ�����..."),TraceLevel_Normal);

			break;
		}
	case SERVICE_STATUS_SERVICE:	//����״̬
		{
			//���±���
			UpdateServerTitle();

			//���ð�ť
			GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_RUN_PARAMETER)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_SERVICE_CONTROL)->EnableWindow(TRUE);

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���������ɹ�"),TraceLevel_Normal);

			break;
		}
	case SERVICE_STATUS_WILLSTOP://�����ر�
		{
			//���ð�ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);

			break;
		}
	}

	return;
}

//��������
VOID CGameServerDlg::OnBnClickedStartService()
{
	//����Ч��
	ASSERT(m_bOptionSuccess==true);
	if (m_bOptionSuccess==false) return;

	//�������
	if (m_ServiceUnitsModule.GetInterface()==NULL)
	{
		//�������
		if (m_ServiceUnitsModule.CreateInstance()==false)
		{
			CTraceService::TraceString(TEXT("��Ϸ�����������ʧ��"),TraceLevel_Exception);
			return;
		}

		//���ýӿ�
		if (m_ServiceUnitsModule->SetServiceUnitsSink(this)==false)
		{
			CTraceService::TraceString(TEXT("�󶨷������״̬�ӿ�ʧ��"),TraceLevel_Exception);
			return;
		}
	}

	//��������
	try
	{
		//����ģ��
		LPCTSTR pszServerDLLName=m_ModuleInitParameter.GameServiceAttrib.szServerDLLName;		
		m_ServiceUnitsModule->SetServiceCollocate(pszServerDLLName,m_ModuleInitParameter.GameServiceOption);

		//��������
		m_ServiceUnitsModule->StartService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//ֹͣ����
VOID CGameServerDlg::OnBnClickedConcludeService()
{
	//Ч��״̬
	ASSERT(m_ServiceUnitsModule.GetInterface()!=NULL);
	if (m_ServiceUnitsModule.GetInterface()==NULL) return;

	//ֹͣ����
	try
	{
		m_ServiceUnitsModule->ConcludeService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//�򿪷���
VOID CGameServerDlg::OnBnClickedOpenServer()
{
	//������Դ
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//���ô���
	CDlgServerList DlgServerList;
	DlgServerList.DoModal();

	//��������
	if (DlgServerList.m_bOptionSuccess==true)
	{
		//��������
		tagModuleInitParameter * pModuleInitParameter=&DlgServerList.m_ModuleInitParameter;

		//���ñ���
		m_bOptionSuccess=true;
		m_ModuleInitParameter.GameServiceAttrib=pModuleInitParameter->GameServiceAttrib;
		m_ModuleInitParameter.GameServiceOption=pModuleInitParameter->GameServiceOption;

		//״̬����
		OnLoadModuleParameter();
	}

	return;
}

//��������
VOID CGameServerDlg::OnBnClickedCreateServer()
{
	//������Դ
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//���÷���
	CDlgServerWizard DlgServerWizard;
	if (DlgServerWizard.DoModal()!=IDOK) return;

	//���ñ���
	m_bOptionSuccess=true;
	DlgServerWizard.GetGameServiceAttrib(m_ModuleInitParameter.GameServiceAttrib);
	DlgServerWizard.GetGameServiceOption(m_ModuleInitParameter.GameServiceOption);

	//״̬����
	OnLoadModuleParameter();

	return;	
}

//���÷���
VOID CGameServerDlg::OnBnClickedOptionServer()
{
	//��Ϸģ��
	CGameServiceManagerHelper GameServiceManager;
	GameServiceManager.SetModuleCreateInfo(m_ModuleInitParameter.GameServiceAttrib.szServerDLLName,GAME_SERVICE_CREATE_NAME);

	//����ģ��
	if (GameServiceManager.CreateInstance()==false)
	{
		AfxMessageBox(TEXT("������������ڻ��߼���ʧ�ܣ���������ʧ��"),MB_ICONERROR);
		return;
	}

	//������Դ
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//���÷���
	CDlgServerWizard DlgServerWizard;
	DlgServerWizard.SetGameServiceManager(GameServiceManager.GetInterface());
	DlgServerWizard.SetGameServiceOption(m_ModuleInitParameter.GameServiceOption);

	//���÷���
	if (DlgServerWizard.DoModal()!=IDOK) return;

	//��ȡ����
	tagGameServiceAttrib GameServiceAttrib;
	DlgServerWizard.GetGameServiceAttrib(GameServiceAttrib);

	//���ñ���
	m_bOptionSuccess=true;
	DlgServerWizard.GetGameServiceAttrib(m_ModuleInitParameter.GameServiceAttrib);
	DlgServerWizard.GetGameServiceOption(m_ModuleInitParameter.GameServiceOption);

	//״̬����
	OnLoadModuleParameter();

	return;
}

//�������
VOID CGameServerDlg::OnBnClickedDebugService()
{
	//�������
	if (m_ServiceUnitsModule.GetInterface() == NULL)
	{
		return;		
	}

	//���Է���
	try
	{
		//��������
		m_ServiceUnitsModule->DebugService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//���±���
VOID CGameServerDlg::UpdateServerTitle()
{
	//�������
	if (m_bOptionSuccess==true)
	{
		//������Ϣ
		LPCTSTR pszGameName=m_ModuleInitParameter.GameServiceAttrib.szGameName;
		LPCTSTR pszServerName=m_ModuleInitParameter.GameServiceOption.szServerName;

		//��ȡ״̬
		LPCTSTR pszStatus=TEXT("ֹͣ");
		WORD ServiceStatus=SERVICE_STATUS_STOP;
		if (m_ServiceUnitsModule.GetInterface()!=NULL) ServiceStatus=m_ServiceUnitsModule->GetServiceStatus();

		//����״̬
		switch (ServiceStatus)
		{
		case SERVICE_STATUS_CONFIG:		//����״̬
			{
				pszStatus=TEXT("��ʼ��");
				break; 
			}
		case SERVICE_STATUS_SERVICE:		//����״̬
			{ 
				pszStatus=TEXT("����");
				break;
			}
		}

		//�������
		TCHAR szServerTitle[128]=TEXT("");
		_sntprintf_s(szServerTitle,CountArray(szServerTitle),TEXT("[ %s - %s ] -- [ %s ]"),pszGameName,pszServerName,pszStatus);

		//���ñ���
		SetWindowText(szServerTitle);
	}
	else
	{
		SetWindowText(TEXT("��Ϸ������ -- [ ֹͣ ]"));
		return;
	}

	return;
}

//���óɹ�
VOID CGameServerDlg::OnLoadModuleParameter()
{
	//����Ч��
	ASSERT(m_bOptionSuccess==true);
	if (m_bOptionSuccess==false) return;

	//���±���
	UpdateServerTitle();

	//���ð�ť
	GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);
	GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(TRUE);

	//���ÿؼ�
	SetDlgItemText(IDC_GAME_NAME,m_ModuleInitParameter.GameServiceAttrib.szGameName);
	if(m_ModuleInitParameter.GameServiceOption.wServerPort!=0)
		SetDlgItemInt(IDC_SERVER_PORT,m_ModuleInitParameter.GameServiceOption.wServerPort);
	else
		SetDlgItemText(IDC_SERVER_PORT,TEXT("�Զ�����"));
	SetDlgItemText(IDC_SERVER_NAME,m_ModuleInitParameter.GameServiceOption.szServerName);

	//������ʾ
	TCHAR szString[256]=TEXT("");
	LPCTSTR pszServerName=m_ModuleInitParameter.GameServiceOption.szServerName;
	_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] ����������سɹ�"),pszServerName);

	//�����Ϣ
	CTraceService::TraceString(szString,TraceLevel_Normal);

	//�Զ�����
	if (g_GlobalUnits.m_bAutoStart==true) OnBnClickedStartService();

	return;
}

//�ر�ѯ��
BOOL CGameServerDlg::OnQueryEndSession()
{
	//��ʾ��Ϣ
	if ((m_ServiceUnitsModule.GetInterface()!=NULL)&&(m_ServiceUnitsModule->GetServiceStatus()!=SERVICE_STATUS_STOP))
	{
		CTraceService::TraceString(TEXT("�������������У�ϵͳҪ��ע���ػ�����ʧ��"),TraceLevel_Warning);
		return FALSE;
	}

	return TRUE;
}

//�����
LRESULT CGameServerDlg::OnMessageProcessCmdLine(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR lpszComLine=(LPCTSTR)(lParam);

	//��������
	if (lpszComLine[0]!=0)
	{
		//��������
		WORD wServerID = 0;

		//��ȡ����
		UINT nStringLength=0;
		CString strItemString;
		LPCTSTR lpszBeginString=lpszComLine;

		//���ط����ʶ
		LPCTSTR pszServerID=TEXT("/ServerID:");
		UINT nServerIDLength=lstrlen(pszServerID);

		//��ȡ����
		while (true)
		{
			//��������
			LPCTSTR lpszEndString=_tcschr(lpszBeginString,TEXT(' '));
			nStringLength=(lpszEndString==NULL)?lstrlen(lpszBeginString):(UINT)(lpszEndString-lpszBeginString);

			//���䴰��
			if ((nStringLength>=nServerIDLength)&&(memcmp(lpszBeginString,pszServerID,nServerIDLength*sizeof(TCHAR))==0))
			{
				//��ȡ����
				CopyMemory(strItemString.GetBufferSetLength(nStringLength-nServerIDLength),lpszBeginString+nServerIDLength,
					(nStringLength-nServerIDLength)*sizeof(TCHAR));
				strItemString.ReleaseBuffer();

				//��ȡ����
				wServerID=(WORD)(_tstoi(strItemString));

				break;
			}

			//���ñ���
			if (lpszEndString==NULL) break;
			lpszBeginString=(lpszEndString+1);
		}

		if( wServerID != 0 )
		{
			CDlgServerList DlgServerList;
			if( DlgServerList.LoadGameServer(wServerID) )
			{

				//���ñ���
				g_GlobalUnits.m_bAutoStart = true;

				//��������
				tagModuleInitParameter * pModuleInitParameter=&DlgServerList.m_ModuleInitParameter;

				//���ñ���
				m_bOptionSuccess=true;
				m_ModuleInitParameter.GameServiceAttrib=pModuleInitParameter->GameServiceAttrib;
				m_ModuleInitParameter.GameServiceOption=pModuleInitParameter->GameServiceOption;

				//״̬����
				OnLoadModuleParameter();
			}
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
