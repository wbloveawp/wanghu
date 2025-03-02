#include "StdAfx.h"
#include "DlgDownLoad.h"


//////////////////////////////////////////////////////////////////////////////////

//λ�ö���
#define CX_BORAD					0									//�߿���
#define CY_BORAD					0									//�߿�߶�

//��Ŀ����
#define MIN_LINE					3									//������Ŀ
#define MAX_LINE					6									//������Ŀ
#define FLASH_ITEM_TIMES			8									//��˸����

//ʱ���ʶ
#define IDI_FLASH_ITEM				100									//���̼��
#define IDI_UPDATE_STATUS			101									//����״̬

//ʱ���ʶ
#define IDI_CHECK_PROCESS			200									//���̼��

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDownLoadSink, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgDownLoad, CSkinDialog)
	
	//ϵͳ��Ϣ
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_WINDOWPOSCHANGED()

	//��ť��Ϣ
	ON_BN_CLICKED(IDC_LAST, OnBnClickedLast)
	ON_BN_CLICKED(IDC_NEXT, OnBnClickedNext)
	ON_BN_CLICKED(IDC_CANCEL_ONE, OnBnClickedCancelOne)
	ON_BN_CLICKED(IDC_RETRY_AGAIN, OnBnClickedRetryAgain)

	//�Զ���Ϣ
	ON_MESSAGE(WM_ITEM_STATUS_UPDATE,OnItemStatusUpdateMessage)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDownLoadSink::CDownLoadSink()
{
	//�����ٶ�
	m_dwDownSpeed=0L;
	m_dwLastCalcSize=0L;
	m_dwLastCalcTime=0L;

	//�ļ���Ϣ
	m_dwDownLoadSize=0L;
	m_dwTotalFileSize=0L;

	//״̬����
	m_cbDownLoadStatus=DOWN_LOAD_IDLE;

	//��������
	m_wKindID=0;
	m_wServerID=0;
	ZeroMemory(m_szClientName,sizeof(m_szClientName));
	m_dwPlazaStationID=0;
	m_dwUserStationID=0;
	m_bSilent=false;
	m_uDownloaded=0;

	//�ں���Ϣ
	m_pDlgDownLoad=NULL;
	ZeroMemory(&m_ProcessInfoMation,sizeof(m_ProcessInfoMation));

	return;
}

//��������
CDownLoadSink::~CDownLoadSink()
{
}

//�ӿڲ�ѯ
VOID * CDownLoadSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDownLoadSink,Guid,dwQueryVer);
//	QUERYINTERFACE(IDownLoadMissionSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDownLoadSink,Guid,dwQueryVer);
	return NULL;
}

//�ر�����
bool CDownLoadSink::CloseDownLoad()
{
	//�رմ���
	DestroyWindow();

	//�ر�����
	m_DownLoad.CloseDownLoad();

	//����״̬
	m_cbDownLoadStatus=DOWN_LOAD_ERROR;

	//�ر��ļ�
	if (m_DownFile.m_hFile!=INVALID_HANDLE_VALUE) m_DownFile.Close();

	//����״̬
	m_pDlgDownLoad->SendMessage(WM_ITEM_STATUS_UPDATE,(WPARAM)this,0);

	return true;
}

//ִ������
bool CDownLoadSink::PerformDownLoad(LPCTSTR pszClientName, WORD wKindID, WORD wServerID,DWORD dwPlazaStationID,DWORD dwUserStationID,bool bSilent)
{
	//Ч��״̬
	ASSERT((m_cbDownLoadStatus==DOWN_LOAD_IDLE)||(m_cbDownLoadStatus==DOWN_LOAD_ERROR));
	if ((m_cbDownLoadStatus!=DOWN_LOAD_IDLE)&&(m_cbDownLoadStatus!=DOWN_LOAD_ERROR)) return false;

	//�����ٶ�
	m_dwDownSpeed=0L;
	m_dwLastCalcSize=0L;
	m_dwLastCalcTime=GetTickCount();

	//���ñ���
	m_wKindID=wKindID;
	m_wServerID=wServerID;
	m_dwPlazaStationID=dwPlazaStationID;
	m_dwUserStationID=dwUserStationID;
	m_cbDownLoadStatus=DOWN_LOAD_LINK;
	lstrcpyn(m_szClientName,pszClientName,CountArray(m_szClientName));
	m_bSilent=bSilent;

	//�����ַ
	TCHAR szDownLoadUrl[MAX_PATH]=TEXT("");
	LPCTSTR pszDownLoadUrl=TEXT("http://service.abcdefgh.com/DownLoad.aspx");

	//�����ַ
#ifdef _UNICODE
	// add by humorly 2018/8/3
	// replace func
	_snwprintf_s(szDownLoadUrl, CountArray(szDownLoadUrl),
		TEXT("%s?KindID=%ld&PlazaStationID=%ld&UserStationID=%ld&Encode=Unicode"),pszDownLoadUrl,wKindID,dwPlazaStationID,dwUserStationID);
#else
	// add by humorly 2018/8/3
	// replace func
	_snwprintf_s(szDownLoadUrl, CountArray(szDownLoadUrl),
		TEXT("%s?KindID=%ld&PlazaStationID=%ld&UserStationID=%ld&Encode=Ansi"),pszDownLoadUrl,wKindID,dwPlazaStationID,dwUserStationID);
#endif

	//����״̬
	m_pDlgDownLoad->SendMessage(WM_ITEM_STATUS_UPDATE,(WPARAM)this,0);

	//ִ������
	m_DownLoad.PerformDownLoad(szDownLoadUrl,QUERY_ME_INTERFACE(IUnknownEx));

	return true;
}

//�����쳣
bool CDownLoadSink::OnDownLoadError(enDownLoadError DownLoadError)
{
	//���ñ���
	m_cbDownLoadStatus=DOWN_LOAD_ERROR;

	//����״̬
	m_pDlgDownLoad->SendMessage(WM_ITEM_STATUS_UPDATE,(WPARAM)this,0);

	return true;
}

//����״̬
bool CDownLoadSink::OnDownLoadStatus(enDownLoadStatus DownLoadStatus) 
{
	//����״̬
	switch (DownLoadStatus)
	{
	case DownLoadStatus_Service:		//����״̬
		{
			//����״̬
			m_cbDownLoadStatus=DOWN_LOAD_READ;

			//����״̬
			m_pDlgDownLoad->SendMessage(WM_ITEM_STATUS_UPDATE,(WPARAM)this,0);

			break;
		}
	case DownLoadStatus_Conclude:		//���״̬
		{
			//����״̬
			m_cbDownLoadStatus=DOWN_LOAD_SETUP;

			if(m_bSilent)
			{
				m_cbDownLoadStatus=DOWN_LOAD_FINISH;
				//�������
				TCHAR szPlatformFile[MAX_PATH]=TEXT("\0");			
				TCHAR szDirectory[MAX_PATH]=TEXT("");
				TCHAR szDownLoad[32]=TEXT("");
				TCHAR szFileMD5[LEN_MD5]=TEXT("");

				//��ȡĿ¼
				CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

				try
				{
					//����Ŀ¼
					// add by humorly 2018/8/3
					// replace func
					_snwprintf_s(szPlatformFile, MAX_PATH, TEXT("%s\\Platform.ini"), szDirectory);
					// add by humorly 2018/8/3
					// replace func
					_snwprintf_s(szDownLoad, CountArray(szDownLoad), TEXT("%d"), DOWN_LOAD_FINISH);
					WritePrivateProfileString(TEXT("UpdateInfo"),TEXT("DownLoadStatus"),szDownLoad,szPlatformFile);
					WritePrivateProfileString(TEXT("UpdateInfo"), TEXT("FileName"), m_DownFile.GetFileName(),szPlatformFile);

					//��ȡMD5
					CString strFileName(m_DownFile.GetFilePath());
					m_DownFile.Close();
					CMD5Encrypt::GetFileMD5(strFileName,szFileMD5);
					WritePrivateProfileString(TEXT("UpdateInfo"), TEXT("FileMD5"), szFileMD5, szPlatformFile);
				}
				catch(...)
				{
					return true;
				}
				return true;
			}

			//��װ�ļ�
			if (PerformInstall()==false)
			{
				//�ر�����
				CloseDownLoad();

				return false;
			}

			//����״̬
			m_pDlgDownLoad->SendMessage(WM_ITEM_STATUS_UPDATE,(WPARAM)this,0);

			break;
		}
	}

	return true;
}

//��������
bool CDownLoadSink::OnDataStream(const VOID * pcbMailData, WORD wStreamSize)
{
	//д���ļ�
	m_dwDownLoadSize+=wStreamSize;
	m_DownFile.Write(pcbMailData,wStreamSize);

	//�����ٶ�
	if (GetTickCount()>=(m_dwLastCalcTime+2000L))
	{
		//�����ٶ�
		m_dwDownSpeed=(m_dwDownLoadSize-m_dwLastCalcSize)/(GetTickCount()-m_dwLastCalcTime);

		//���ñ���
		m_dwLastCalcTime=GetTickCount();
		m_dwLastCalcSize=m_dwDownLoadSize;
	}
	
	return true;
}

//������Ϣ
bool CDownLoadSink::OnDataInformation(DWORD dwTotalFileSize, LPCTSTR pszEntityTag, LPCTSTR pszLocation)
{
	//���ñ���
	m_dwTotalFileSize+=dwTotalFileSize;

	//�����ļ�
	if (CreateDownFile(pszLocation)==false)
	{
		//�ر�����
		CloseDownLoad();

		return false;
	}

	return true;
}

//ִ�а�װ
bool CDownLoadSink::PerformInstall()
{
	//�����ļ�
	TCHAR szTargetName[MAX_PATH]=TEXT("");
	CString strSourceName=m_DownFile.GetFilePath();

	//��������
	INT nTailCount=lstrlen(TEXT(".WHD"));
	INT nSourceLength=lstrlen(strSourceName);

	//��������
	szTargetName[nSourceLength-nTailCount]=0;
	CopyMemory(szTargetName,(LPCTSTR)strSourceName,sizeof(TCHAR)*(nSourceLength-nTailCount));

	//�ر��ļ�
	m_DownFile.Close();

	//ɾ���ļ�
	DeleteFile(szTargetName);

	//�ƶ��ļ�
	if (MoveFileWithProgress(strSourceName,szTargetName,NULL,NULL,MOVEFILE_WRITE_THROUGH|MOVEFILE_REPLACE_EXISTING)==FALSE)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	TCHAR szCommandLine[MAX_PATH];
	CString strTargetName(szTargetName);
	strTargetName.MakeLower();
	if(strTargetName.Find(TEXT("gameplaza.exe"))==-1 && m_wKindID!=0)
		// add by humorly 2018/8/3
		// replace func
		_snwprintf_s(szCommandLine, CountArray(szCommandLine), TEXT("%s /VERYSILENT"), szTargetName);
	else
		// add by humorly 2018/8/3
		// replace func
		_snwprintf_s(szCommandLine, CountArray(szCommandLine), TEXT("%s"), szTargetName);

	//��������
	STARTUPINFO StartInfo;
	ZeroMemory(&StartInfo,sizeof(StartInfo));

	//��������
	StartInfo.cb=sizeof(StartInfo);
	StartInfo.wShowWindow=SW_SHOWDEFAULT;
	if (CreateProcess(NULL,szCommandLine,NULL,NULL,FALSE,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&StartInfo,&m_ProcessInfoMation)==FALSE)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if (m_hWnd==NULL)
	{
		CRect rcCreate(0,0,0,0);
		Create(NULL,NULL,WS_CHILD,rcCreate,m_pDlgDownLoad,100);
	}

	//����ʱ��
	SetTimer(IDI_CHECK_PROCESS,100,NULL);

	return true;
}

//�����ļ�
bool CDownLoadSink::CreateDownFile(LPCTSTR pszLocation)
{
	//Ч��״̬
	ASSERT(m_DownFile.m_hFile==INVALID_HANDLE_VALUE);
	if (m_DownFile.m_hFile!=INVALID_HANDLE_VALUE) return false;

	//����Ŀ¼
	TCHAR szWorkDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szWorkDirectory,CountArray(szWorkDirectory));

	//Ŀ��Ŀ¼
	TCHAR szDownLoadPath[MAX_PATH]=TEXT("");
	// add by humorly 2018/8/3
	// replace func
	_snwprintf_s(szDownLoadPath, CountArray(szDownLoadPath), TEXT("%s\\DownLoad"), szWorkDirectory);

	//�ļ�����
	DWORD dwIndex=lstrlen(pszLocation);
	while ((dwIndex>0)&&(pszLocation[dwIndex]!=TEXT('/'))) dwIndex--;

	//�ļ�·��
	TCHAR szFilePath[MAX_PATH]=TEXT("");
	LPCTSTR pszFileName=&pszLocation[dwIndex+1];
	// add by humorly 2018/8/3
	// replace func
	_snwprintf_s(szFilePath, CountArray(szFilePath), TEXT("%s\\%s.WHD"), szDownLoadPath, pszFileName);

	//����Ŀ¼
	CreateDirectory(szDownLoadPath,NULL);

	//�����ļ�
	if (m_DownFile.Open(szFilePath,CFile::modeCreate|CFile::modeWrite)==FALSE) return false;

	return true;
}

//ʱ����Ϣ
VOID CDownLoadSink::OnTimer(UINT_PTR nIDEvent)
{
	//���̼��
	if (nIDEvent==IDI_CHECK_PROCESS)
	{
		//״̬Ч��
		if (m_ProcessInfoMation.hProcess==NULL)
		{
			//�������
			ASSERT(FALSE);

			//�ر�����
			CloseDownLoad();

			return;
		}

		//���̼��
		if (WaitForSingleObject(m_ProcessInfoMation.hProcess,0L)==WAIT_OBJECT_0)
		{
			//�رմ���
			DestroyWindow();

			//����״̬
			m_cbDownLoadStatus=DOWN_LOAD_FINISH;

			//����״̬
			m_pDlgDownLoad->SendMessage(WM_ITEM_STATUS_UPDATE,(WPARAM)this,0);
		}

		return;
	}

	__super::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgDownLoad::CDlgDownLoad() : CSkinDialog(IDD_DLG_DOWNLOAD)
{
	//״̬����
	m_bCreate=false;
	m_wCurrentPage=0;
	m_wViewPageCount=0;
	m_wViewLineCount=0;

	//����״̬
	m_wFocusItem=INVALID_WORD;
	m_wHoverItem=INVALID_WORD;

	//��˸����
	m_wFlashItem=INVALID_WORD;
	m_wFlashCount=INVALID_WORD;

	m_pPlazaResource=NULL;

	//��ȡ��С
	CBitImage ImageDownLoadItem;
	ImageDownLoadItem.LoadFromResource(GetModuleHandle(UPDATE_SERVICE_DLL_NAME),IDB_DOWN_LOAD_ITEM);
	m_SizeItemImage.SetSize(ImageDownLoadItem.GetWidth(),ImageDownLoadItem.GetHeight()/5L);

	//��ȡ��С
	CPngImageWarpper ImageDownLoadScale;
	ImageDownLoadScale.LoadImage(GetModuleHandle(UPDATE_SERVICE_DLL_NAME),TEXT("DOWN_LOAD_SCALE"));
	m_SizeScaleImage.SetSize(ImageDownLoadScale.GetWidth(),ImageDownLoadScale.GetHeight()/2);

	return;
}

//��������
CDlgDownLoad::~CDlgDownLoad()
{
}

//�ؼ���
VOID CDlgDownLoad::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LAST, m_btLast);
	DDX_Control(pDX, IDC_NEXT, m_btNext);
	DDX_Control(pDX, IDCANCEL, m_btCancelAll);
	DDX_Control(pDX, IDC_CANCEL_ONE, m_btCancelOne);
	DDX_Control(pDX, IDC_RETRY_AGAIN, m_btRetryAgain);
	DDX_Control(pDX, IDC_DOWNLOAD_LINK, m_HyperDownLoad);
}

//��Ϣ����
BOOL CDlgDownLoad::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message==WM_KEYDOWN)&&((pMsg->wParam==VK_ESCAPE)||(pMsg->wParam==VK_RETURN)))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//��������
BOOL CDlgDownLoad::OnInitDialog()
{
	__super::OnInitDialog();

	//״̬����
	m_bCreate=true;
	m_wCurrentPage=0;
	m_wViewPageCount=0;
	m_wViewLineCount=0;

	//����״̬
	m_wFocusItem=INVALID_WORD;
	m_wHoverItem=INVALID_WORD;

	//��˸����
	m_wFlashItem=INVALID_WORD;
	m_wFlashCount=INVALID_WORD;

	//����ʱ��
	SetTimer(IDI_UPDATE_STATUS,200,NULL);

	//���ð�ť
	if(m_pPlazaResource!=NULL)
	{
		tagDownLoadImage image;
		m_pPlazaResource->GetDownloadImage(image);
		m_btLast.SetButtonImage(image.uBtLoadLast,m_hInstance,false,false);
		m_btNext.SetButtonImage(image.uBtLoadNext,m_hInstance,false,false);
	}

	//��������
	TCHAR szHyperKinkUrl[128]=TEXT("");
	CGlobalStation * pGlobalStation=CGlobalStation::GetInstance();	
	// add by humorly 2018/8/3
	// replace func
	_snwprintf_s(szHyperKinkUrl, CountArray(szHyperKinkUrl), TEXT("%s/GameDownload.aspx"), pGlobalStation->GetStationHomeLink());

	//��������
	m_HyperDownLoad.SetHyperLinkUrl(szHyperKinkUrl);

	//����ͼ��
	SetWindowText(TEXT("��Ϸ����"));
	SetIcon(LoadIcon(GetModuleHandle(UPDATE_SERVICE_DLL_NAME),MAKEINTRESOURCE(IDI_DOWNLOAD)),FALSE);
	EnableButton(WS_MINIMIZEBOX);

	return TRUE;
}

//ȡ����Ϣ
VOID CDlgDownLoad::OnCancel()
{
	//�ر���ʾ
	CInformation Information(this);
	INT nResult=Information.ShowMessageBox(TEXT("��ȷ��Ҫȡ��ȫ���ĳ������ظ�����"),MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2);

	//�رմ���
	if (nResult==IDYES) DestroyWindow();

	return;
}

//ת��һҳ
VOID CDlgDownLoad::OnBnClickedLast()
{
	//Ч��״̬
	ASSERT(m_wCurrentPage>0);
	if (m_wCurrentPage==0) return;

	//���ñ���
	m_wCurrentPage--;

	//��˸����
	m_wFlashCount=0;
	m_wFlashItem=INVALID_WORD;

	//״̬����
	m_wFocusItem=INVALID_WORD;
	m_wHoverItem=INVALID_WORD;

	//���ư�ť
	m_btCancelOne.EnableWindow(FALSE);
	m_btRetryAgain.EnableWindow(FALSE);

	//ҳ�����
	m_btLast.EnableWindow((m_wCurrentPage>0)?TRUE:FALSE);
	m_btNext.EnableWindow(((m_wCurrentPage+1)<m_wViewPageCount)?TRUE:FALSE);

	//���½���
	RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);

	return;
}

//ת��һҳ
VOID CDlgDownLoad::OnBnClickedNext()
{
	//Ч��״̬
	ASSERT((m_wCurrentPage+1)<m_wViewPageCount);
	if ((m_wCurrentPage+1)>=m_wViewPageCount) return;

	//���ñ���
	m_wCurrentPage++;

	//��˸����
	m_wFlashCount=0;
	m_wFlashItem=INVALID_WORD;

	//״̬����
	m_wFocusItem=INVALID_WORD;
	m_wHoverItem=INVALID_WORD;

	//���ư�ť
	m_btCancelOne.EnableWindow(FALSE);
	m_btRetryAgain.EnableWindow(FALSE);

	//ҳ�����
	m_btLast.EnableWindow((m_wCurrentPage>0)?TRUE:FALSE);
	m_btNext.EnableWindow(((m_wCurrentPage+1)<m_wViewPageCount)?TRUE:FALSE);

	//���½���
	RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);

	return;
}

//ȡ������
VOID CDlgDownLoad::OnBnClickedCancelOne()
{
	//Ч��״̬
	ASSERT(m_wFocusItem!=INVALID_WORD);
	ASSERT(m_DownLoadSinkArray.GetCount()>m_wFocusItem);

	//��ȡ����
	CDownLoadSink * pDownLoadSink=m_DownLoadSinkArray[m_wFocusItem];

	//�ر�����
	pDownLoadSink->CloseDownLoad();

	//ɾ������
	SafeDelete(pDownLoadSink);
	m_DownLoadSinkArray.RemoveAt(m_wFocusItem);

	//�ر��ж�
	if (m_DownLoadSinkArray.GetCount()>0L)
	{
		//��������
		RectifyControl();

		//���½���
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
	}
	else
	{
		//�رմ���
		DestroyWindow();
	}

	return;
}

//�ٴ�����
VOID CDlgDownLoad::OnBnClickedRetryAgain()
{
	//Ч��״̬
	ASSERT(m_wFocusItem!=INVALID_WORD);
	ASSERT(m_DownLoadSinkArray.GetCount()>m_wFocusItem);

	//��ȡ����
	CDownLoadSink * pDownLoadSink=m_DownLoadSinkArray[m_wFocusItem];
	pDownLoadSink->PerformDownLoad(pDownLoadSink->m_szClientName,pDownLoadSink->m_wKindID,pDownLoadSink->m_wServerID,
		pDownLoadSink->m_dwPlazaStationID,pDownLoadSink->m_dwUserStationID);

	return;
}

//������Ϣ
VOID CDlgDownLoad::OnDestroy()
{
	__super::OnDestroy();

	//״̬����
	m_bCreate=false;
	m_wCurrentPage=0;
	m_wViewPageCount=0;
	m_wViewLineCount=0;

	//����״̬
	m_wFocusItem=INVALID_WORD;
	m_wHoverItem=INVALID_WORD;

	//��˸����
	m_wFlashItem=INVALID_WORD;
	m_wFlashCount=INVALID_WORD;

	//�ر�����
	for (INT_PTR i=0;i<m_DownLoadSinkArray.GetCount();i++)
	{
		//��ȡ����
		ASSERT(m_DownLoadSinkArray[i]!=NULL);
		CDownLoadSink * pDownLoadSink=m_DownLoadSinkArray[i];

		//ɾ������
		SafeDelete(pDownLoadSink);
	}

	//ɾ������
	m_DownLoadSinkArray.RemoveAll();

	return;
}

//ʱ����Ϣ
VOID CDlgDownLoad::OnTimer(UINT_PTR nIDEvent)
{
	//����״̬
	switch (nIDEvent)
	{
	case IDI_FLASH_ITEM:		//��˸����
		{
			//���ñ���
			m_wFlashCount++;

			//��ֹ�ж�
			if (m_wFlashCount>=FLASH_ITEM_TIMES)
			{
				//ɾ��ʱ��
				KillTimer(IDI_FLASH_ITEM);

				//���ñ���
				m_wFlashItem=INVALID_WORD;
				m_wFlashCount=INVALID_WORD;
			}

			//���½���
			RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);

			return;
		}
	case IDI_UPDATE_STATUS:		//����״̬
		{
			//���½���
			RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);

			return;
		}
	}

	__super::OnTimer(nIDEvent);
}

//�����Ϣ
VOID CDlgDownLoad::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//����ж�
	if (m_wFocusItem!=m_wHoverItem)
	{
		//���ñ���
		m_wFocusItem=m_wHoverItem;

		//���½���
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);

		//���ư�ť
		m_btCancelOne.EnableWindow((VerdictCancelOne(m_wFocusItem)==true)?TRUE:FALSE);
		m_btRetryAgain.EnableWindow((VerdictRetryAgain(m_wFocusItem)==true)?TRUE:FALSE);
	}

	return;
}

//�����Ϣ
VOID CDlgDownLoad::OnLButtonDblClk(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDblClk(nFlags,Point);

	//�����ж�
	if ((m_wHoverItem!=INVALID_WORD)&&(VerdictRetryAgain(m_wHoverItem)==true))
	{
		OnBnClickedRetryAgain();
	}

	return;
}

//λ�øı�
VOID CDlgDownLoad::OnWindowPosChanged(WINDOWPOS * lpwndpos)
{
	__super::OnWindowPosChanged(lpwndpos);

	//�ƶ���ť
	if (m_bCreate==true)
	{
		//��ȡλ��
		CRect rcClient;
		GetClientRect(&rcClient);

		//ǰ��ҳ��
		CRect rcLast;
		m_btLast.GetWindowRect(&rcLast);

		//���λ��
		INT nBBorder=m_SkinAttribute.m_nCaptionHeigth;
		INT nRBorder=m_SkinAttribute.m_nXBorder;

		//�ƶ���ť
		INT nYControlPos=rcClient.Height()-rcLast.Height()-20;
		m_btLast.SetWindowPos(NULL,10,nYControlPos,0,0,SWP_NOSIZE|SWP_NOZORDER);
		m_btNext.SetWindowPos(NULL,15+rcLast.Width(),nYControlPos,0,0,SWP_NOSIZE|SWP_NOZORDER);

		//���ӿؼ�
		CRect rcNext(0,0,0,0);
		m_btNext.GetWindowRect(rcNext);
		m_HyperDownLoad.SetWindowPos(NULL,15+rcLast.Width()+rcNext.Width()+6,nYControlPos+4,0,0,SWP_NOSIZE|SWP_NOZORDER);

		//��ť����
		CRect rcButton;
		m_btCancelAll.GetWindowRect(&rcButton);

		//�ƶ���ť
		INT nYButtonPos=rcClient.Height()-rcButton.Height()-19;
		m_btCancelAll.SetWindowPos(NULL,rcClient.Width()-rcButton.Width()-8-nRBorder,nYButtonPos,0,0,SWP_NOSIZE|SWP_NOZORDER);
		m_btCancelOne.SetWindowPos(NULL,rcClient.Width()-rcButton.Width()*2-18-nRBorder,nYButtonPos,0,0,SWP_NOSIZE|SWP_NOZORDER);
		m_btRetryAgain.SetWindowPos(NULL,rcClient.Width()-rcButton.Width()*3-28-nRBorder,nYButtonPos,0,0,SWP_NOSIZE|SWP_NOZORDER);
	}

	return;
}

//�����Ϣ
BOOL CDlgDownLoad::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//��ȡ���
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//����ѡ��
	WORD wHoverItem=m_wHoverItem;
	m_wHoverItem=SwitchItemIndex(MousePoint);

	//���½���
	if (m_wHoverItem!=wHoverItem)
	{
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
	}

	//���ù��
	if (m_wHoverItem!=INVALID_WORD)
	{
		SetCursor(LoadCursor(GetModuleHandle(UPDATE_SERVICE_DLL_NAME),MAKEINTRESOURCE(IDC_HAND_CUR)));
		return true;
	}

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//�滭��Ϣ
VOID CDlgDownLoad::OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeigth)
{
	//���û���
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(0,0,0));

	//������Դ
	CPngImageWarpper ImageDownLoadScale;
	ImageDownLoadScale.LoadImage(GetModuleHandle(UPDATE_SERVICE_DLL_NAME),TEXT("DOWN_LOAD_SCALE"));

	//������Դ
	CImage ImageDownLoadItem;
	ImageDownLoadItem.LoadFromResource(GetModuleHandle(UPDATE_SERVICE_DLL_NAME),IDB_DOWN_LOAD_ITEM);


	//���λ��
	INT nTBorder=m_SkinAttribute.m_nCaptionHeigth;
	INT nLBorder=m_SkinAttribute.m_nYBorder-4;

	//�滭����
	ImageDownLoadItem.BitBlt(pDC->m_hDC,CX_BORAD+nLBorder,CY_BORAD+nTBorder,m_SizeItemImage.cx,m_SizeItemImage.cy,0,0);

	int nHideCount=0;
	//�滭����
	for (INT i=0;i<m_wViewLineCount;i++)
	{
		//����λ��
		INT nYImagePos=0;
		INT nXScreenPos=CX_BORAD+nLBorder;
		INT nYScreenPos=CY_BORAD+nTBorder+(i+1-nHideCount)*m_SizeItemImage.cy;

		//��������
		WORD wDrawIndex=m_wCurrentPage*m_wViewLineCount+i;

		//��˸�ж�
		if (m_wFlashItem!=wDrawIndex)
		{
			if (m_wFocusItem==wDrawIndex) nYImagePos=4*m_SizeItemImage.cy;
			else if (m_wHoverItem==wDrawIndex) nYImagePos=3*m_SizeItemImage.cy;
			else nYImagePos=((i%2)!=0)?m_SizeItemImage.cy:m_SizeItemImage.cy*2;
		}
		else
		{
			if ((i%2)==0) nYImagePos=4*m_SizeItemImage.cy;
			if ((i%2)==1) nYImagePos=((i%2)!=0)?m_SizeItemImage.cy:m_SizeItemImage.cy*2;
		}

		//�滭����
		ImageDownLoadItem.BitBlt(pDC->m_hDC,nXScreenPos,nYScreenPos,m_SizeItemImage.cx,m_SizeItemImage.cy,0,nYImagePos);

		//�滭����
		if (wDrawIndex<m_DownLoadSinkArray.GetCount())
		{
			//��ȡ����
			ASSERT(m_DownLoadSinkArray[wDrawIndex]!=NULL);
			CDownLoadSink * pDownLoadSink=m_DownLoadSinkArray[wDrawIndex];
			if(pDownLoadSink->m_bSilent)
			{
				nHideCount++;
				continue;
			}

			//��������
			UINT uDrawFormat=DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS;

			//��������
			CRect rcClientName;
			rcClientName.SetRect(nXScreenPos,nYScreenPos,nXScreenPos+84,nYScreenPos+30);
			pDC->DrawText(pDownLoadSink->m_szClientName,lstrlen(pDownLoadSink->m_szClientName),&rcClientName,uDrawFormat);

			//�ļ���С
			if ((pDownLoadSink->m_cbDownLoadStatus==DOWN_LOAD_READ)||(pDownLoadSink->m_cbDownLoadStatus==DOWN_LOAD_SETUP))
			{
				//�����ַ�
				TCHAR szTotalFileSize[64]=TEXT("");
				DWORD dwTotalFileSize=pDownLoadSink->m_dwTotalFileSize;
				// add by humorly 2018/8/3
				// replace func
				_snwprintf_s(szTotalFileSize, CountArray(szTotalFileSize), TEXT("%ld KB"), dwTotalFileSize / 1000L);

				//�ļ���С
				CRect rcTotalSize;
				rcTotalSize.SetRect(nXScreenPos+84,nYScreenPos,nXScreenPos+164,nYScreenPos+30);
				pDC->DrawText(szTotalFileSize,lstrlen(szTotalFileSize),&rcTotalSize,uDrawFormat);
			}

			//�����ٶ�
			if ((pDownLoadSink->m_cbDownLoadStatus==DOWN_LOAD_READ)||(pDownLoadSink->m_cbDownLoadStatus==DOWN_LOAD_SETUP))
			{
				//�����ַ�
				TCHAR szDownSpeed[64]=TEXT("");
				// add by humorly 2018/8/3
				// replace func
				_snwprintf_s(szDownSpeed, CountArray(szDownSpeed), TEXT("%ld KB/S"), pDownLoadSink->m_dwDownSpeed);

				//�ļ���С
				CRect rcDownSpeed;
				rcDownSpeed.SetRect(nXScreenPos+398,nYScreenPos,nXScreenPos+483,nYScreenPos+30);
				pDC->DrawText(szDownSpeed,lstrlen(szDownSpeed),&rcDownSpeed,uDrawFormat);
			}

			//���ؽ���
			if (pDownLoadSink->m_cbDownLoadStatus!=DOWN_LOAD_IDLE)
			{
				//�������
				DWORD dwDownScale=0L;
				if (pDownLoadSink->m_dwTotalFileSize>0L)
				{
					dwDownScale=pDownLoadSink->m_dwDownLoadSize*100L/pDownLoadSink->m_dwTotalFileSize;
				}

				//�����ַ�
				TCHAR szDownScale[64]=TEXT("");
				LPCTSTR pszStatusString=GetStatusString(pDownLoadSink->m_cbDownLoadStatus);
				// add by humorly 2018/8/3
				// replace func
				_snwprintf_s(szDownScale, CountArray(szDownScale), TEXT("%ld%% %s"), dwDownScale, pszStatusString);

				//���ؽ���
				pDC->TextOut(nXScreenPos+315,nYScreenPos+8,szDownScale,lstrlen(szDownScale));

				//�滭����
				INT nXDrawArea=m_SizeScaleImage.cx*dwDownScale/100L;
				ImageDownLoadScale.DrawImage(pDC,nXScreenPos+168,nYScreenPos+8,m_SizeScaleImage.cx,m_SizeScaleImage.cy,0,0);
				ImageDownLoadScale.DrawImage(pDC,nXScreenPos+168,nYScreenPos+8,nXDrawArea,m_SizeScaleImage.cy,0,m_SizeScaleImage.cy);
			}
		}
	}

	//�滭����
	ImageDownLoadItem.BitBlt(pDC->m_hDC,CX_BORAD+nLBorder,CY_BORAD+nTBorder+(m_wViewLineCount+1)*m_SizeItemImage.cy,m_SizeItemImage.cx,1,0,0);

	return;
}

//������Ϸ
bool CDlgDownLoad::DownLoadClient(LPCTSTR pszClientName, WORD wKindID, WORD wServerID,DWORD dwPlazaStationID,DWORD dwUserStationID,bool bSilent)
{
	//������Ϸ
	for (INT_PTR i=0;i<m_DownLoadSinkArray.GetCount();i++)
	{
		//��ȡ����
		ASSERT(m_DownLoadSinkArray[i]!=NULL);
		CDownLoadSink * pDownLoadSink=m_DownLoadSinkArray[i];

		//�����ж�
		if (pDownLoadSink->m_wKindID==wKindID)
		{
			//��ʾ����
			if(bSilent)
				ShowWindow(SW_HIDE);
			else
				ShowWindow(SW_SHOW);
			SetForegroundWindow();

			//��������
			FlashDownLoadItem((WORD)(i));

			return true;
		}
	}

	//��������
	CDownLoadSink * pDownLoadSink=NULL;
	try
	{
		//��������
		pDownLoadSink=new CDownLoadSink;

		//�����ж�
		if (pDownLoadSink==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}
	catch (...)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	m_DownLoadSinkArray.Add(pDownLoadSink);

	//��������
	pDownLoadSink->m_pDlgDownLoad=this;
	pDownLoadSink->PerformDownLoad(pszClientName,wKindID,wServerID,dwPlazaStationID,dwUserStationID,bSilent);

	//��������
	RectifyControl();

	//��ʾ����
	if(bSilent)
		ShowWindow(SW_HIDE);
	else
		ShowWindow(SW_SHOW);
	SetForegroundWindow();

	//���½���
	RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);

	return true;
}

//�����ؼ�
VOID CDlgDownLoad::RectifyControl()
{
	//ҳ������
	INT nItemCount=(INT)m_DownLoadSinkArray.GetCount();
	m_wViewLineCount=__min(__max(MIN_LINE,nItemCount),MAX_LINE);
	m_wViewPageCount=(nItemCount+m_wViewLineCount-1)/m_wViewLineCount;

	//ҳ�����
	if (m_wCurrentPage>=m_wViewPageCount) m_wCurrentPage=(m_wViewPageCount-1);

	//���㷶Χ
	WORD wMinIndex=m_wCurrentPage*m_wViewLineCount;
	WORD wMaxtIndex=__min((m_wCurrentPage+1)*m_wViewLineCount-1,(nItemCount-1));

	//����״̬
	m_wHoverItem=((m_wHoverItem<wMinIndex)||(m_wHoverItem>wMaxtIndex))?INVALID_WORD:m_wHoverItem;
	m_wFocusItem=(m_wFocusItem!=INVALID_WORD)?__max(__min(m_wFocusItem,wMaxtIndex),wMinIndex):INVALID_WORD;

	//ҳ�����
	m_btLast.EnableWindow((m_wCurrentPage>0)?TRUE:FALSE);
	m_btNext.EnableWindow(((m_wCurrentPage+1)<m_wViewPageCount)?TRUE:FALSE);

	//���ư�ť
	m_btCancelOne.EnableWindow((VerdictCancelOne(m_wFocusItem)==true)?TRUE:FALSE);
	m_btRetryAgain.EnableWindow((VerdictRetryAgain(m_wFocusItem)==true)?TRUE:FALSE);

	//���λ��
	INT nTBorder=m_SkinAttribute.m_nCaptionHeigth;
	INT nBBorder=10;//m_SkinAttribute.m_SkinImageBM.GetHeight();
	INT nLBorder=10;//m_SkinAttribute.m_SkinImageML.GetWidth();
	INT nRBorder=10;//=m_SkinAttribute.m_SkinImageMR.GetWidth();

	//�ͻ�����
	CRect rcWindow;
	rcWindow.SetRect(0,0,m_SizeItemImage.cx+CX_BORAD*2+nLBorder+nRBorder,m_SizeItemImage.cy*(m_wViewLineCount+1)+nTBorder+nBBorder+44);

	//�ƶ�����
	CalcWindowRect(&rcWindow,CWnd::adjustBorder);
	SetWindowPos(NULL,0,0,rcWindow.Width(),rcWindow.Height(),SWP_NOZORDER|SWP_NOMOVE|SWP_NOCOPYBITS);

	return;
}

//��˸����
VOID CDlgDownLoad::FlashDownLoadItem(WORD wFlashItem)
{
	//Ч��״̬
	ASSERT((wFlashItem==INVALID_WORD)||(wFlashItem<m_DownLoadSinkArray.GetCount()));
	if ((wFlashItem!=INVALID_WORD)&&(wFlashItem>=m_DownLoadSinkArray.GetCount())) return;

	//���ñ���
	m_wFlashCount=0;
	m_wFlashItem=wFlashItem;

	//����ʱ��
	SetTimer(IDI_FLASH_ITEM,150,NULL);

	return;
}

//ת������
WORD CDlgDownLoad::SwitchItemIndex(CPoint MousePoint)
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	INT_PTR nItemCount=m_DownLoadSinkArray.GetCount();
	INT_PTR nHideCount=m_wCurrentPage*m_wViewLineCount;
	INT_PTR nCurrentCount=__min(nItemCount-nHideCount,MAX_LINE);

	//���λ��
	INT nTBorder=m_SkinAttribute.m_nCaptionHeigth;
	INT nBBorder=10;//m_SkinAttribute.m_SkinImageBM.GetHeight();
	INT nLBorder=10;//m_SkinAttribute.m_SkinImageML.GetWidth();
	INT nRBorder=10;//m_SkinAttribute.m_SkinImageMR.GetWidth();

	//�����ж�
	if (MousePoint.x<=(CX_BORAD+nLBorder)) return INVALID_WORD;
	if (MousePoint.x>=(rcClient.Width()-CX_BORAD-nRBorder)) return INVALID_WORD;

	//�����ж�
	if (MousePoint.y<=(CY_BORAD+nTBorder+m_SizeItemImage.cy)) return INVALID_WORD;
	if (MousePoint.y>=(CY_BORAD+nTBorder+m_SizeItemImage.cy*(nCurrentCount+1))) return INVALID_WORD;

	//�������
	return (WORD)((MousePoint.y-CY_BORAD-nTBorder-m_SizeItemImage.cy)/m_SizeItemImage.cy+nHideCount);
}

//ȡ���ж�
bool CDlgDownLoad::VerdictCancelOne(WORD wFocuxIndex)
{
	//��ȡ����
	if (wFocuxIndex!=INVALID_WORD)
	{
		//��ȡ����
		ASSERT(m_DownLoadSinkArray[m_wFocusItem]!=NULL);
		CDownLoadSink * pDownLoadSink=m_DownLoadSinkArray[m_wFocusItem];

		//ȡ���ж�
		BYTE cbDownLoadStatus=pDownLoadSink->m_cbDownLoadStatus;
		if ((cbDownLoadStatus!=DOWN_LOAD_SETUP)&&(cbDownLoadStatus!=DOWN_LOAD_SETUP)) return true;
	}

	return false;
}

//�����ж�
bool CDlgDownLoad::VerdictRetryAgain(WORD wFocuxIndex)
{
	//��ȡ����
	if (wFocuxIndex!=INVALID_WORD)
	{
		//��ȡ����
		ASSERT(m_DownLoadSinkArray[m_wFocusItem]!=NULL);
		CDownLoadSink * pDownLoadSink=m_DownLoadSinkArray[m_wFocusItem];

		//ȡ���ж�
		if (pDownLoadSink->m_cbDownLoadStatus==DOWN_LOAD_ERROR) return true;
	}

	return false;
}

//��ȡ״̬
LPCTSTR CDlgDownLoad::GetStatusString(BYTE cbDownLoadStatus)
{
	switch (cbDownLoadStatus)
	{
	case DOWN_LOAD_IDLE:
	case DOWN_LOAD_LINK: { return TEXT("��������"); }
	case DOWN_LOAD_READ: { return TEXT("��������"); }
	case DOWN_LOAD_SETUP: { return TEXT("���ڰ�װ"); }
	case DOWN_LOAD_ERROR: { return TEXT("����ʧ��"); }
	case DOWN_LOAD_FINISH: { return TEXT("��װ�ɹ�"); }
	}
	
	return NULL;
}

//״̬��Ϣ
LRESULT CDlgDownLoad::OnItemStatusUpdateMessage(WPARAM wParam, LPARAM lParam)
{
	//��ȡ����
	ASSERT((CDownLoadSink *)wParam!=NULL);
	CDownLoadSink * pDownLoadSink=(CDownLoadSink *)wParam;

	//����ж�
	if (pDownLoadSink->m_cbDownLoadStatus==DOWN_LOAD_FINISH)
	{
		//��������
		WORD wKindID=pDownLoadSink->m_wKindID;
		WORD wServerID=pDownLoadSink->m_wServerID;

		//ɾ������
		for (INT_PTR i=0;i<m_DownLoadSinkArray.GetCount();i++)
		{
			if (m_DownLoadSinkArray[i]==pDownLoadSink)
			{
				m_DownLoadSinkArray.RemoveAt(i);
				break;
			}
		}

		//ɾ������
		SafeDelete(pDownLoadSink);

		//�ر��ж�
		if (m_DownLoadSinkArray.GetCount()>0L)
		{
			//��������
			RectifyControl();
		
			//���½���
			RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
		}
		else
		{
			//�رմ���
			DestroyWindow();
		}

		//����֪ͨ
		CWnd * pWndGamePlaza=AfxGetMainWnd();
		if (pWndGamePlaza!=NULL) pWndGamePlaza->SendMessage(WM_SETUP_FINISH,wKindID,wServerID);
	}
	else
	{
		//���ư�ť
		if ((m_wFocusItem!=INVALID_WORD)&&(m_DownLoadSinkArray[m_wFocusItem]==pDownLoadSink))
		{
			m_btCancelOne.EnableWindow((VerdictCancelOne(m_wFocusItem)==true)?TRUE:FALSE);
			m_btRetryAgain.EnableWindow((VerdictRetryAgain(m_wFocusItem)==true)?TRUE:FALSE);
		}
	}

	//���½���
	if (m_hWnd!=NULL) RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);

	return 0L;
}

//////////////////////////////////////////////////////////////////////////////////
