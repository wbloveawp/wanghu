
// CJExcelToJsonDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ExcelToJson.h"
#include "ExcelToJsonDlg.h"
#include "afxdialogex.h"
#include "ConfigCenter.h"
#include <locale>
#include <afxpriv.h>
#include <regex>
#include <iostream>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// libxl ��Կ
const wchar_t* XX = L"Halil Kural";
const wchar_t* YY = L"windows-2723210a07c4e90162b26966a8jcdboe";

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CExcelToJsonDlg �Ի���



CExcelToJsonDlg::CExcelToJsonDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CExcelToJsonDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	for (int i = 0; i < MAX_SHEET_COUNT; i++)
		m_nHeaderCount[i] = 0;
}

void CExcelToJsonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CExcelToJsonDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CExcelToJsonDlg::OnBnClickedButtonToExcel)
	ON_BN_CLICKED(IDC_BUTTON_AUTO, &CExcelToJsonDlg::OnBnClickedButtonToLua)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE, &CExcelToJsonDlg::OnBnClickedButtonChoose)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CExcelToJsonDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_LUA, &CExcelToJsonDlg::OnBnClickedButtonChooseExcel)
END_MESSAGE_MAP()


// CExcelToJsonDlg ��Ϣ�������

BOOL CExcelToJsonDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	InitData();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CExcelToJsonDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CExcelToJsonDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
// ��ʾ��
HCURSOR CExcelToJsonDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ��ʼ������ 
void CExcelToJsonDlg::InitData()
{
	// ���ݳ�ʼ��
	m_strFileName = L"";
	m_nSheetCount = 0;
	m_strSavePath = L"";
	m_strSourceFile = L"Message.h";
	m_strExcelName = L"MsgTranslate.xlsx";
	m_strExcelFile = L"MsgTranslate.xlsx";

	for (int n = 0; n < MAX_SHEET_COUNT; n++)
	{
		m_strJsonName[n] = L"";
		m_strSheetName[n] = L"";
		for (int i = 0; i < 100; i++)
		{
			m_arHeader[n][i] = L"";
		}

		m_lstData[n].RemoveAll();
	}

	// ��ʼ������
	CConfigCenter::GetInstance().InitConfig();

}

void CExcelToJsonDlg::CreateFolder(CString szFolder)
{
	if (!::PathIsDirectory(szFolder))
	{
		::CreateDirectory(szFolder, 0);
	}
}

// ����excel
void CExcelToJsonDlg::OnBnClickedButtonToExcel()
{
	if (!IsFileExists(m_strSourceFile))
	{
		AfxMessageBox(L"��ѡ����Ϣ�ļ���");
		return;
	}
	// ��ȡ�ļ�
	ReadFile(m_strSourceFile);
	// �����ļ�
	AnalysisFile();
	// д��excel
	WriteMsgToExcel();
}

// ת��lua
void CExcelToJsonDlg::OnBnClickedButtonToLua()
{
	// ת��lua
	ExcelToLua();

	return;
}

// ѡ���ļ�
void CExcelToJsonDlg::OnBnClickedButtonChoose()
{
	CString	strFilter = L".h�ļ�(*.h)|*.h|";
	CFileDialog dlg(true, L"", L"", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter);

	TCHAR currentPath[MAX_PATH];
	GetCurrentDirectory(sizeof(currentPath), currentPath);
	dlg.m_ofn.lpstrInitialDir = currentPath;
	if (IDOK == dlg.DoModal())
	{
		m_strSourceFile = dlg.GetPathName();
		SetDlgItemText(IDC_EDIT_PATH, m_strSourceFile);
	}
}

// ѡ�񱣴�·��
void CExcelToJsonDlg::OnBnClickedButtonSave()
{
	// ��ȡ·��
	BROWSEINFO   bInfo;
	ZeroMemory(&bInfo, sizeof(bInfo));
	bInfo.hwndOwner = m_hWnd;
	TCHAR tchPath[255];
	bInfo.lpszTitle = _T("��ѡ��·��:   ");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST lpDlist;

	//�������淵����Ϣ��IDList��ʹ��SHGetPathFromIDList����ת��Ϊ�ַ���
	lpDlist = SHBrowseForFolder(&bInfo);   //��ʾѡ��Ի���
	if (lpDlist != NULL)
	{
		SHGetPathFromIDList(lpDlist, tchPath);//����Ŀ��ʶ�б�ת����Ŀ¼
		m_strSavePath.Format(L"%s", tchPath);
		SetDlgItemText(IDC_EDIT_SAVE, m_strSavePath);
	}
}

// ѡ��Ҫת����Excel�ļ�
void CExcelToJsonDlg::OnBnClickedButtonChooseExcel()
{
	CString	strFilter = L"xlsx�ļ�(*.xlsx)|*.xlsx|xls�ļ�(*.xls)|*.xls|";
	CFileDialog dlg(true, L"", L"", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter);

	TCHAR currentPath[MAX_PATH];
	GetCurrentDirectory(sizeof(currentPath), currentPath);
	dlg.m_ofn.lpstrInitialDir = currentPath;
	if (IDOK == dlg.DoModal())
	{
		m_strExcelFile = dlg.GetPathName();
		SetDlgItemText(IDC_EDIT_SAVE_LUA, m_strExcelFile);
	}
}

// ͨ��������ȡ�����
int CExcelToJsonDlg::GetSheetIndexBySheetName(CString sheetName)
{
	for (int i = 0; i < m_nSheetCount; i++)
	{
		if (m_strSheetName[i] == sheetName)
		{
			return i;
		}
	}
	return -1;
}

// ��ȡexcel�ļ���ע�����excel�ļ����ڱ���������򿪣����޷���ȡ
BOOL CExcelToJsonDlg::ReadExcel(CString strFileName)
{
	BOOL isXlsx = false;

	m_strFileName = strFileName;

	strFileName.MakeLower();
	if (strFileName.Find(L".xlsx") >= 0)
	{
		isXlsx = true;
	}

	Book* book;
	CString fileName;
	CString tempFileName;

	// ��ȡ�������ļ���
	int nPos = strFileName.ReverseFind('\\');
	fileName = strFileName.Right(strFileName.GetLength() - nPos - 1);
	tempFileName = fileName;

	if (isXlsx)
	{
		book = xlCreateXMLBook();
	}
	else
	{
		book = xlCreateBook();
	}
	book->setKey(XX, YY);

	if (book->load(strFileName)) {

		// ������
		m_nSheetCount = book->sheetCount();

		// ���湤��������
		for (int sheetCount = 0; sheetCount < book->sheetCount(); sheetCount++)
		{

			Sheet* sheet = book->getSheet(sheetCount);
			if (sheet)
			{
				m_strSheetName[sheetCount] = sheet->name();
			}
		}

		for (int sheetCount = 0; sheetCount < book->sheetCount(); sheetCount++)
		{

			Sheet* sheet = book->getSheet(sheetCount);
			if (sheet)
			{
				// ��ȡ����
				CString msg;
				CString sheetName = sheet->name();

				CString configFilePath;
				CString sectionName;
				// �����ļ���
				CString configFileName = L"TableHeader.ini";
				fileName = tempFileName;

				configFilePath = strFileName;
				configFilePath.Replace(fileName, configFileName);

				// �����ļ�����
				fileName.Replace(L".xlsx", L"");
				fileName.Replace(L".xls", L"");
				sectionName = fileName + L"_" + sheetName;

				// ��ȡ����
				TableConfig config;
				VecTableHeader tableHeader = config.VecTableHeaderConfig;

				// �����ļ���
				CString jsonFileName = sheetName + L"." + config.strFileEx;
				m_strJsonName[sheetCount] = strFileName;
				m_strJsonName[sheetCount].Replace(tempFileName, jsonFileName);

				m_nHeaderCount[sheetCount] = sheet->lastCol() - sheet->firstCol();

				CString tempHeader[100];
				for (int j = 0; j < m_nHeaderCount[sheetCount]; j++)
				{
					//��ȡ��ͷ
					m_arHeader[sheetCount][j] = sheet->readStr(0, j);
					tempHeader[j] = m_arHeader[sheetCount][j];
				}

				// ��ȡ���� ����
				for (int i = 1; i < sheet->lastRow(); i++) {
					CJsonRow jsonRow;
					int nBlankCount = 0;
					// ����
					for (int j = 0; j < m_nHeaderCount[sheetCount]; j++)
					{
						jsonRow.jsonNode[j].Name = tempHeader[j];

						CellType celltype = sheet->cellType(i, j);

						if (celltype == CELLTYPE_STRING)
						{
							jsonRow.jsonNode[j].strValue = sheet->readStr(i, j);
							jsonRow.jsonNode[j].strValue.Replace(L"\"", L"\\\"");
							jsonRow.jsonNode[j].strValue.Replace(L"\\N", L"\\\\N");
							jsonRow.jsonNode[j].strValue.Replace(L"\\0", L"");
							jsonRow.jsonNode[j].strValue.Replace(L"\n", L"");
							jsonRow.jsonNode[j].strValue.Replace(L"\r", L"");
						}
						else if (celltype == CELLTYPE_NUMBER)
						{
							jsonRow.jsonNode[j].nValue = sheet->readNum(i, j);
						}
						else if (celltype == CELLTYPE_BLANK)
						{
							nBlankCount++;
							jsonRow.jsonNode[j].strValue = L"null";

							jsonRow.jsonNode[j].nValue = 0;
							jsonRow.jsonNode[j].dValue = 0;
						}
						else
						{
							nBlankCount++;
							jsonRow.jsonNode[j].strValue = L"null";

							jsonRow.jsonNode[j].nValue = 0;
							jsonRow.jsonNode[j].dValue = 0;
						}
						jsonRow.jsonNode[j].ccellType = celltype;
						jsonRow.jsonNode[j].strType = "string";
						jsonRow.jsonNode[j].nDecimalNum = 0;
						jsonRow.jsonNode[j].nRowCount = 1;
						jsonRow.jsonNode[j].nColCount = 1;
						jsonRow.jsonNode[j].nIsCancelBrackets = 0;

					}

					// һ���пհף������
					if (nBlankCount == m_nHeaderCount[sheetCount])
						break;

					m_lstData[sheetCount].AddTail(jsonRow);
				}

			} // ��ȡsheet����

		} // ѭ����ȡsheet����

		// ������Ҫת���ı�
		for (int sheetCount = 0; sheetCount < book->sheetCount(); sheetCount++)
		{
			Sheet* sheet = book->getSheet(sheetCount);
			if (sheet)
			{
				CString sheetName = sheet->name();
				CString sectionName;
				// �����ļ���
				fileName = tempFileName;

				// �����ļ�����
				fileName.Replace(L".xlsx", L"");
				fileName.Replace(L".xls", L"");
				sectionName = fileName + L"_" + sheetName;

				// ��ȡ����
				TableConfig config;

				// �����ļ�
				if (config.nIsMainTable)
				{
					// �����ļ�·��
					CString strOutFileName;

					//�����ͬ����lua�ļ�
					for (int i = 2; i < m_nHeaderCount[sheetCount]; i++)
					{
						//���Ŀ¼
						CString outFolder = strFileName;
						outFolder.Replace(tempFileName, L"");
						outFolder += L"LuaOutFiles";

						//����Ŀ¼
						CreateFolder(outFolder);

						CString jsonFileName = m_arHeader[sheetCount][i] + L"_" + sheetName + L"." + config.strFileEx;
						strOutFileName = outFolder + L"\\" + jsonFileName;

						FILE* fp;

						if (fopen_s(&fp, (LPCSTR)(_bstr_t)strOutFileName.GetBuffer(), "w"))
						{
							return FALSE;
						}

						CString strText = L"";
						if (config.strFileEx == L"lua")
						{
							strText = SaveLua(m_lstData[sheetCount], m_strJsonName[sheetCount], m_nHeaderCount[sheetCount], m_arHeader[sheetCount], sheetName, m_arHeader[sheetCount][i]);
						}

						WriteUnicodeStringToUtf8File(fp, strText);
						fclose(fp);

						if (strText == L"null")
							return FALSE;
					}

					
				}
			}
		}

		AfxMessageBox(L"ת���ɹ���");
		return TRUE;

	} // ���ع���������
	else {
		CString sz(book->errorMessage());
		sz.Format(L"���ع�����[%s]ʧ�ܣ�%s", tempFileName, sz);
		AfxMessageBox(sz);
		return FALSE;
	}
}

int UnicodeToUTF_8(wchar_t* strUnicode, int strUnicodeLen, char* strUTF8, int strUTF8Len)
{
	if ((strUnicode == NULL) || (strUnicodeLen <= 0) || (strUTF8Len <= 0 && strUTF8Len != -1))
	{
		return -1;
	}

	int i, offset = 0;

	if (strUTF8Len == -1)
	{
		for (i = 0; i < strUnicodeLen; i++)
		{
			if (strUnicode[i] <= 0x007f)      // ���ֽڱ���  
			{
				offset += 1;
			}
			else if (strUnicode[i] >= 0x0080 && strUnicode[i] <= 0x07ff)   // ˫�ֽڱ���  
			{
				offset += 2;
			}
			else if (strUnicode[i] >= 0x0800 && strUnicode[i] <= 0xffff)   // ���ֽڱ���  
			{
				offset += 3;
			}
		}
		return offset + 1;
	}
	else
	{
		if (strUTF8 == NULL)
		{
			return -1;
		}

		for (i = 0; i < strUnicodeLen; i++)
		{
			if (strUnicode[i] <= 0x007f)      // ���ֽڱ���  
			{
				strUTF8[offset++] = (char)(strUnicode[i] & 0x007f);
			}
			else if (strUnicode[i] >= 0x0080 && strUnicode[i] <= 0x07ff)   // ˫�ֽڱ���  
			{
				strUTF8[offset++] = (char)(((strUnicode[i] & 0x07c0) >> 6) | 0x00c0);
				strUTF8[offset++] = (char)((strUnicode[i] & 0x003f) | 0x0080);
			}
			else if (strUnicode[i] >= 0x0800 && strUnicode[i] <= 0xffff)   // ���ֽڱ���  
			{
				strUTF8[offset++] = (char)(((strUnicode[i] & 0xf000) >> 12) | 0x00e0);
				strUTF8[offset++] = (char)(((strUnicode[i] & 0x0fc0) >> 6) | 0x0080);
				strUTF8[offset++] = (char)((strUnicode[i] & 0x003f) | 0x0080);
			}
		}
		strUTF8[offset] = '\0';
		return offset + 1;
	}
}

// charתCString
CString CExcelToJsonDlg::CharToCString(const char* str)
{
	// ����char *�����С�����ֽ�Ϊ��λ��һ������ռ�����ֽ�
	int charLen = strlen(str);

	// ������ֽ��ַ��Ĵ�С�����ַ����㡣
	int len = MultiByteToWideChar(CP_ACP, 0, str, charLen, NULL, 0);

	// Ϊ���ֽ��ַ���������ռ䣬�����СΪ���ֽڼ���Ķ��ֽ��ַ���С
	TCHAR* buf = new TCHAR[len + 1];

	// ���ֽڱ���ת���ɿ��ֽڱ���
	MultiByteToWideChar(CP_ACP, 0, str, charLen, buf, len);

	//����ַ�����β��ע�ⲻ��len+1
	buf[len] = '\0';

	// ��TCHAR����ת��ΪCString
	CString pWideChar;
	pWideChar.Append(buf);

	// ɾ��������
	delete[]buf;

	return pWideChar;
}

// ��Unicode�ַ���תΪΪutf8�ַ�����д���ļ���
void CExcelToJsonDlg::WriteUnicodeStringToUtf8File(FILE* fp, CString strText)
{
	int len = UnicodeToUTF_8(strText.GetBuffer(), strText.GetLength(), NULL, -1);

	if (len == 0)
	{
		return;
	}

	char* pUtf8Text = new char[len];

	UnicodeToUTF_8(strText.GetBuffer(), strText.GetLength(), pUtf8Text, len);

	fwrite(pUtf8Text, sizeof(char), len - 1, fp);
}

// ����lua�ļ�
CString CExcelToJsonDlg::SaveLua(JsonDataList& jsonDataList, CString szOutFileName, int nHeaderCount, CString strHeader[100], CString sheetName, CString szLanguage)
{

	CString strAll;
	int rowIndex = 0;

	strAll = L"local msg = {\n";

	POSITION pos = jsonDataList.GetHeadPosition();
	BOOL bFirstRow = true;
	int nRowCount = 0;
	while (pos)
	{
		CJsonRow jsonRow = jsonDataList.GetNext(pos);
		nRowCount++;

		{
			BOOL bFirstColumn = TRUE;
			CString strData;
			for (int j = 0; j < nHeaderCount; j++)
			{
				// ����ԭ��Ϣ
				if (j == 1) continue;

				// ��ϢID
				if (j == 0)
					if (jsonRow.jsonNode[j].ccellType == CELLTYPE_STRING)
						strData.Format(L"%s\n\t[%lld] = ", strData, atoi(CT2A(jsonRow.jsonNode[j].strValue.GetBuffer())));
					else if (jsonRow.jsonNode[j].ccellType == CELLTYPE_NUMBER)
						strData.Format(L"%s\n\t[%lld] = ", strData, jsonRow.jsonNode[j].nValue);
					else
						strData.Format(L"%s\n\t[%lld] = ", strData, jsonRow.jsonNode[j].nValue);
				else
				{
					if (jsonRow.jsonNode[j].Name != szLanguage)
						continue;

					if (jsonRow.jsonNode[j].ccellType == CELLTYPE_BLANK || jsonRow.jsonNode[j].ccellType == CELLTYPE_EMPTY)
						strData.Format(L"%s\"%s\", -- %s\n", strData, L"", jsonRow.jsonNode[j].Name);
					else
					{
						if (jsonRow.jsonNode[j].ccellType == CELLTYPE_NUMBER)
						{
							strData.Format(L"%s%lld, -- %s\n", strData, jsonRow.jsonNode[j].nValue, jsonRow.jsonNode[j].Name);
						}
						else if (jsonRow.jsonNode[j].ccellType == CELLTYPE_STRING)
						{
							strData.Format(L"%s\"%s\", -- %s\n", strData, jsonRow.jsonNode[j].strValue, jsonRow.jsonNode[j].Name);
						}
					}
				}
			}

			strAll += strData;
			strData = L"";
			//strAll += L"\t}";

			bFirstRow = FALSE;
		}

		rowIndex++;
	}

	strAll += L"\n}\n";
	strAll += L"return msg";

	return strAll;
}

// ��ȡ�ļ�
void CExcelToJsonDlg::ReadFile(CString strFileName)
{
	FILE* fp;

	fopen_s(&fp, (LPCSTR)(_bstr_t)strFileName.GetBuffer(), "rb");
	if (fp == NULL)
		return;

	//��ȡ�ļ�����
	fseek(fp, 0, SEEK_END);
	m_dwFileSize = ftell(fp);

	try
	{
		//�����ݶ����ڴ�
		m_pData = (char*)malloc(m_dwFileSize * sizeof(char));
	}
	catch (...)
	{
		fclose(fp);
		fp = NULL;

		//ASSERT(m_pData != NULL);
		return;
	}

	fseek(fp, 0, SEEK_SET);
	fread(m_pData, 1, m_dwFileSize, fp);

	fclose(fp);
	fp = NULL;
}

// ��������
void CExcelToJsonDlg::AnalysisFile()
{
	// ��ʼ���ĵ�����
	CString szFile(m_pData);
	// ����������һ����Ϣ
	CString szLastMsg;
	CString szDefine = L"#define";
	CString szMIM;
	CString szLastMIM;
	// ��Ϣ�ο�ʼ
	DWORD dwMsgCode = 0;
	// ��ǰ��Ϣ��
	DWORD dwCurCode = 0;
	ResultMap mapResult;
	m_Result.clear();

	for (DWORD i = 0; i < szFile.GetLength(); i++)
	{
		if (szFile[i] == '#')
		{
			CString szTemp = szFile.Mid(i, 7);
			// ���� #define
			if (szTemp == szDefine)
			{
				// ���� #define MIM_DATABASE					1000
				if (i >= 22 && szFile[i - 20] == '/' && szFile[i - 21] == '/' && szFile[i - 22] == '/')
				{
					szLastMIM = szMIM;
					szMIM = "";
					i += 7;
					// ���� MIM_DATABASE
					bool bStart = false;
					for (DWORD j = 1; j < 1000; j++)
					{
						char tmp = szFile[i + j];
						if ((tmp >= 'a' && tmp <= 'z') || (tmp >= 'A' && tmp <= 'Z'))
							bStart = true;

						if (tmp == '\t' || tmp == ' ')
						{
							i += j;
							break;
						}

						if (bStart)
						{
							szMIM.AppendChar(szFile[i + j]);
						}
					}

					// ��������1000
					CString szCode;
					bool bFind = false;
					for (DWORD j = 1; j < 1000; j++)
					{
						int tmp = (int)szFile[i + j];
						if (tmp >= 48 && tmp <= 57)
						{
							bFind = true;
							szCode.AppendChar(szFile[i + j]);
						}
						else if (bFind == true)
						{
							// ��ָ��������
							i += j;

							if (dwMsgCode != 0)
							{
								m_Result.insert(make_pair(szLastMIM, mapResult));
								mapResult.clear();
							}
							dwMsgCode = atol(CT2A(szCode.GetBuffer()));
							m_MIMOrder.insert(make_pair(dwMsgCode, szMIM));
							break;
						}
					}
				}
				// ���� #define MIM_DATABASE_0001				(MIM_DATABASE+1)
				else if ((szFile[i - 1] == '\n' && szFile[i - 2] == '/') || (szFile[i - 1] == '\n' && szFile[i - 2] == '\n' && szFile[i - 3] == '/') ||
					    ((szFile[i - 1] == '\n' && szFile[i - 2] == '\r' && szFile[i - 3] == '/') || (szFile[i - 1] == '\n' && szFile[i - 2] == '\r' && szFile[i - 3] == '\n' && szFile[i - 4] == '\r' && szFile[i - 5] == '/')))
				{
					i += 6;
					CString szCode;
					bool bStart = false;
					// ����+���������
					for (DWORD j = 1; j < 1000; j++)
					{
						if (szFile[i + j] == '+')
						{
							bStart = true;
							continue;
						}
						else if (szFile[i + j] == ')')
						{
							// ��ָ��������
							i += j;
							dwCurCode = dwMsgCode + atol(CT2A(szCode.GetBuffer()));
							mapResult.insert(make_pair(dwCurCode, szLastMsg));
							break;
						}
						if (bStart)
							szCode.AppendChar(szFile[i + j]);
					}
				}
			}
		}
		else if (szFile[i] == '*')
		{
			// ����/* *" "* /
			if ((szFile[i - 1] == '\n' && szFile[i - 2] == '*') || (szFile[i - 1] == '\n' && szFile[i - 2] == '\r' && szFile[i - 3] == '*'))
			{
				szLastMsg.Empty();
				bool bStart = false;
				for (DWORD j = 1; j < 1000; j++)
				{
					if (szFile[i + j] == '"')
					{
						if (!bStart)
						{
							bStart = true;
							continue;
						}
						else if (szFile[i + j + 1] == '\n' || (szFile[i + j + 1] == '\r' && szFile[i + j + 2] == '\n'))
						{
							i += j;
							break;
						}
					}
					if (bStart)
						szLastMsg.AppendChar(szFile[i + j]);
				}
			}
		}

	}
	// �������һ����Ϣ
	m_Result.insert(make_pair(szMIM, mapResult));

	return;
}

// �ж��ļ��Ƿ����
bool CExcelToJsonDlg::IsFileExists(CString& name) {
	ifstream f(name);
	return f.good();
}

// д��excel
void CExcelToJsonDlg::WriteMsgToExcel()
{
	BOOL isXlsx = true;
	Book* book;
	if (isXlsx)
		book = xlCreateXMLBook();
	else
		book = xlCreateBook();

	// ������Կ
	book->setKey(XX, YY);

	// excel�ļ�
	CString szFile;
	if (m_strSavePath != L"")
		szFile = m_strSavePath + L"\\" + m_strExcelName;
	else 
		szFile = m_strExcelName;

	// ��ȡ����
	map<CString, DataMap> data;
	// ��¼���б�
	map<CString, Sheet*> sheets;
	// ��ϢID��Ӧ����
	map<DWORD, int> dataRow;

	if (IsFileExists(szFile))
	{
		// �����Ѵ��ڵı�
		if (book->load(szFile))
		{
			// ���������
			for (int sheetCount = 0; sheetCount < book->sheetCount(); sheetCount++)
			{
				Sheet* sheet = book->getSheet(sheetCount);

				if (sheet)
				{
					CString sheetName = sheet->name();
					DataMap res;

					// ��¼��
					sheets.insert(make_pair(sheetName, sheet));

					// ��ȡ���� ����
					for (int i = 1; i < sheet->lastRow(); i++)
					{
						// ����
						DWORD dwValue = sheet->readNum(i, 0);
						CString strValue = sheet->readStr(i, 1);
						res.insert(make_pair(dwValue, strValue));
						dataRow.insert(make_pair(dwValue, i));
					}

					data.insert(make_pair(sheetName, res));
				}
			}

			// ��������Ϣ
			for (auto sh = m_MIMOrder.begin(); sh != m_MIMOrder.end(); sh++)
			{
				// �½�sheet
				CString sheetName;
				sheetName = sh->second;
				Sheet* sheet = NULL;

				// ���������
				auto da = data.find(sheetName);
				if (da == data.end())
				{
					sheet = book->addSheet(sheetName);
					WriteSheetData(book, sheet, sh->second);
				}
				// ���ڱ��Ա�ID�Ƿ����
				else
				{
					sheet = sheets[sheetName];
					auto allrow = da->second;

					// �½���ʽ
					Format* newFormat = book->addFormat();
					newFormat->setAlignV(ALIGNV_CENTER);
					newFormat->setAlignH(ALIGNH_LEFT);

					auto it = m_Result[sh->second];
					// ������Ϣ
					for (auto row = it.begin(); row != it.end(); row++)
					{
						// �½���ʽ
						Format* form = book->addFormat();
						form->setAlignV(ALIGNV_CENTER);
						form->setAlignH(ALIGNH_LEFT);
						form->setBorder(BORDERSTYLE_THIN);
						form->setBorderColor(COLOR_RED);

						// ������Ϣ
						CString szMsg;
						szMsg = row->second;

						// ����ID�����Ƿ����
						auto curRow = allrow.find(row->first);
						if (curRow != allrow.end())
						{
							// ��Ϣ���ݲ�һ�£��滻����շ���
							if (curRow->second != row->second)
							{
								sheet->writeStr(dataRow[row->first], 1, szMsg, newFormat);
								sheet->writeBlank(dataRow[row->first], 2, form);
								sheet->writeBlank(dataRow[row->first], 3, form);
							}
						}
						// δ�ҵ� �����
						else
						{
							int index = sheet->lastRow();
							sheet->writeNum(index, 0, row->first, newFormat);
							sheet->writeStr(index, 1, szMsg, newFormat);
							sheet->writeBlank(index, 2, form);
							sheet->writeBlank(index, 3, form);
						}
					}
				}
			}
			book->save(szFile);
		}
		else
		{
			CString sz(book->errorMessage());
			sz.Format(L"���ع�����ʧ�ܣ�%s", sz);
			AfxMessageBox(sz);
			return;
		}
	}
	else
	{
		// ��������Ϣ
		for (auto sh = m_MIMOrder.begin(); sh != m_MIMOrder.end(); sh++)
		{
			// �½�sheet
			CString sheetName;
			sheetName = sh->second;
			Sheet* sheet = book->addSheet(sheetName);

			WriteSheetData(book, sheet, sh->second);
		}
		book->save(szFile);
	}


	AfxMessageBox(L"���ɳɹ���");
}

// д�������
void CExcelToJsonDlg::WriteSheetData(Book* book, Sheet* sheet, CString msg)
{
	// ����һ���������
	libxl::Font* font = book->addFont();
	font->setColor(COLOR_RED);
	font->setBold(true);

	Format* f = book->addFormat();
	f->setAlignV(ALIGNV_CENTER);
	f->setAlignH(ALIGNH_CENTER);
	f->setFont(font);
	f->setFillPattern(FILLPATTERN_SOLID);
	f->setPatternForegroundColor(COLOR_LIME);

	// �����ͷ
	CString id("id");
	CString message("message");
	CString English("English");
	CString Portuguese("Portuguese");
	sheet->writeStr(0, 0, id, f);
	sheet->writeStr(0, 1, message, f);
	sheet->writeStr(0, 2, English, f);
	sheet->writeStr(0, 3, Portuguese, f);

	// �п�
	sheet->setCol(1, 3, 90);
	// ���и�
	sheet->setRow(0, 25);

	// �½���ʽ
	Format* newFormat = book->addFormat();
	newFormat->setAlignV(ALIGNV_CENTER);
	newFormat->setAlignH(ALIGNH_LEFT);

	DWORD index = 1;
	auto it = m_Result[msg];
	// ������Ϣ
	for (auto row = it.begin(); row != it.end(); row++)
	{
		// ������Ϣ
		CString szMsg;
		szMsg = row->second;
		sheet->writeNum(index, 0, row->first, newFormat);
		sheet->writeStr(index, 1, szMsg, newFormat);
		sheet->writeBlank(index, 2, newFormat);
		sheet->writeBlank(index, 3, newFormat);
		index++;
	}
}

// ��excelת��lua
void CExcelToJsonDlg::ExcelToLua()
{
	if (!IsFileExists(m_strExcelFile))
	{
		AfxMessageBox(L"��ѡ��Ҫת����Excel�ļ���");
		return;
	}
	ReadExcel(m_strExcelFile);
}