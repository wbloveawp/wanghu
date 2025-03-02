#include "StdAfx.h"
#include "SkinRichEditCtrl.h"

//////////////////////////////////////////////////////////////////////////////////

//��Ϣ��ɫ
#define COLOR_INFO					RGB(64,0,0)							//��Ϣ��ɫ
#define COLOR_BACK_GROUND			RGB(230,230,230)					//������ɫ

//�˵�����
#define IDM_CLEAR_ALL				(WM_USER+201)						//ɾ����Ϣ
#define IDM_SELECT_ALL				(WM_USER+202)						//ȫ��ѡ��
#define IDM_COPY_STRING				(WM_USER+203)						//�����ַ�
#define IDM_SAVE_STRING				(WM_USER+205)						//������Ϣ
#define IDM_DELETE_STRING			(WM_USER+204)						//ɾ���ַ�

//��������
#define IDT_TRACE_STRING			1									//׷���¼�

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkinRichEditCtrl, CRichEditCtrl)

	//ϵͳ��Ϣ
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()

	//�˵���Ϣ
	ON_COMMAND(IDM_CLEAR_ALL, OnClearAll)
	ON_COMMAND(IDM_SELECT_ALL, OnSelectAll)
	ON_COMMAND(IDM_COPY_STRING, OnCopyString)
	ON_COMMAND(IDM_SAVE_STRING, OnSaveString)
	ON_COMMAND(IDM_DELETE_STRING, OnDeleteString)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CSkinRichEditCtrl::CSkinRichEditCtrl()
{
	//���ñ���
	m_lMaxLineCount = 1024;
	m_lReserveLineCount = 512;

	//�߳���Ϣ
	m_dwMainThreadID = 0L;
	m_hMainThreadHandle = NULL;

	return;
}

//��������
CSkinRichEditCtrl::~CSkinRichEditCtrl()
{
}

//�󶨺���
VOID CSkinRichEditCtrl::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//���ÿؼ�
	InitializeService();

	return;
}

//������Ϣ
bool CSkinRichEditCtrl::LoadMessage(LPCTSTR pszFileName)
{
	//Ч�����
	ASSERT(pszFileName != NULL);
	if (pszFileName == NULL) return false;

	//���ļ�
	CFile File;
	if (File.Open(pszFileName, CFile::modeRead, NULL) == FALSE) return false;

	//��������
	EDITSTREAM EditStream;
	EditStream.pfnCallback = LoadCallBack;
	EditStream.dwCookie = (DWORD)(LONGLONG)(&File);

	//��ȡ�ļ�
	StreamIn(SF_RTF, EditStream);

	return true;
}

//������Ϣ
bool CSkinRichEditCtrl::SaveMessage(LPCTSTR pszFileName)
{
	//��������
	CString strFileName = pszFileName;

	//���ļ�
	if (strFileName.IsEmpty() == true)
	{
		CFileDialog FileDlg(FALSE, TEXT("RTF"), 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, TEXT("��Ϣ�ļ�(*.RTF)|*.RTF||"), this);
		if (FileDlg.DoModal() == IDOK) strFileName = FileDlg.GetPathName();
		else return false;
	}

	//���ļ�
	CFile File;
	if (File.Open(strFileName, CFile::modeWrite | CFile::modeCreate, NULL) == FALSE) return false;

	//��������
	EDITSTREAM EditStream;
	EditStream.pfnCallback = SaveCallBack;
	EditStream.dwCookie = (DWORD)(LONGLONG)(&File);

	//д���ļ�
	StreamOut(SF_RTF, EditStream);

	return true;
}

//���ò���
bool CSkinRichEditCtrl::SetParameter(LONG lMaxLineCount, LONG lReserveLineCount)
{
	//Ч�����
	ASSERT((m_lMaxLineCount == 0) || (m_lMaxLineCount > m_lReserveLineCount));
	if ((m_lMaxLineCount != 0) && (m_lMaxLineCount <= m_lReserveLineCount)) return false;

	//���ñ���
	m_lMaxLineCount = lMaxLineCount;
	m_lReserveLineCount = lReserveLineCount;

	return true;
}

//���÷���
VOID CSkinRichEditCtrl::InitializeService()
{
	//��ն���
	m_DataQueue.RemoveData(false);

	//�߳̾��
	m_hMainThreadHandle = GetCurrentThread();
	m_dwMainThreadID = GetThreadId(m_hMainThreadHandle);

	//���ÿؼ�
	SetBackgroundColor(FALSE, COLOR_BACK_GROUND);

	return;
}

//�����ִ�
VOID CSkinRichEditCtrl::InsertString(LPCTSTR pszString, CHARFORMAT2 & CharFormat)
{
	//������Ϣ
	CHARRANGE CharRange;
	GetSel(CharRange);

	//��������
	LONG lTextLength = GetTextLengthEx(GTL_NUMCHARS);

	//������Ϣ
	SetSel(lTextLength, lTextLength);
	SetSelectionCharFormat(CharFormat);
	ReplaceSel(pszString);

	//�����ַ�
	SendMessage(WM_VSCROLL, SB_BOTTOM, 0);

	return;
}

//���ػص�
DWORD CALLBACK CSkinRichEditCtrl::LoadCallBack(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb)
{
	//��ȡ�ļ�
	*pcb = ((CFile *)(ULONGLONG)(dwCookie))->Read(pbBuff, cb);

	return 0;
}

//����ص�
DWORD CALLBACK CSkinRichEditCtrl::SaveCallBack(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb)
{
	//д���ļ�
	((CFile *)(ULONGLONG)(dwCookie))->Write(pbBuff, cb);

	return 0;
}

//ɾ����Ϣ
VOID CSkinRichEditCtrl::OnClearAll()
{
	//ɾ����Ϣ
	SetSel(0L, -1L);
	ReplaceSel(NULL);

	return;
}

//ȫ��ѡ��
VOID CSkinRichEditCtrl::OnSelectAll()
{
	//ȫ��ѡ��
	SetSel(0L, -1L);

	return;
}

//�����ַ�
VOID CSkinRichEditCtrl::OnCopyString()
{
	//�����ַ�
	Copy();

	return;
}

//������Ϣ
VOID CSkinRichEditCtrl::OnSaveString()
{
	//������Ϣ
	SaveMessage(NULL);

	return;
}

//ɾ���ַ�
VOID CSkinRichEditCtrl::OnDeleteString()
{
	//ɾ���ַ�
	ReplaceSel(TEXT(""));

	return;
}

//������Ϣ
INT CSkinRichEditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1) return -1;

	//���ÿؼ�
	InitializeService();

	return 0;
}

//�Ҽ���Ϣ
VOID CSkinRichEditCtrl::OnRButtonDown(UINT nFlags, CPoint Point)
{
	//���ý���
	SetFocus();

	//����˵�
	CMenu FunctionMenu;
	FunctionMenu.CreatePopupMenu();

	//�����˵�
	FunctionMenu.AppendMenu(MF_STRING, IDM_COPY_STRING, TEXT("����(&C)     Ctrl+C"));
	FunctionMenu.AppendMenu(MF_STRING, IDM_SELECT_ALL, TEXT("ȫѡ(&A)     Ctrl+A"));
	FunctionMenu.AppendMenu(MF_STRING, IDM_DELETE_STRING, TEXT("ɾ��(&D)"));
	FunctionMenu.AppendMenu(MF_STRING, IDM_CLEAR_ALL, TEXT("�����Ϣ"));
	FunctionMenu.AppendMenu(MF_SEPARATOR);
	FunctionMenu.AppendMenu(MF_STRING, IDM_SAVE_STRING, TEXT("������Ϣ..."));

	//��ֹ�˵�
	UINT nEnableMask = ((GetSelectionType() != SEL_EMPTY) ? 0 : MF_GRAYED);
	FunctionMenu.EnableMenuItem(IDM_COPY_STRING, MF_BYCOMMAND | nEnableMask);
	FunctionMenu.EnableMenuItem(IDM_DELETE_STRING, MF_BYCOMMAND | nEnableMask);

	//��ʾ�˵�
	ClientToScreen(&Point);
	FunctionMenu.TrackPopupMenu(TPM_RIGHTBUTTON, Point.x, Point.y, this);

	return;
}

//////////////////////////////////////////////////////////////////////////////////
