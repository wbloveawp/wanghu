
// CJExcelToJsonDlg.cpp : 实现文件
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

// libxl 密钥
const wchar_t* XX = L"Halil Kural";
const wchar_t* YY = L"windows-2723210a07c4e90162b26966a8jcdboe";

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CExcelToJsonDlg 对话框



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


// CExcelToJsonDlg 消息处理程序

BOOL CExcelToJsonDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	InitData();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CExcelToJsonDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 当用户拖动最小化窗口时系统调用此函数取得光标
// 显示。
HCURSOR CExcelToJsonDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 初始化数据 
void CExcelToJsonDlg::InitData()
{
	// 数据初始化
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

	// 初始化配置
	CConfigCenter::GetInstance().InitConfig();

}

void CExcelToJsonDlg::CreateFolder(CString szFolder)
{
	if (!::PathIsDirectory(szFolder))
	{
		::CreateDirectory(szFolder, 0);
	}
}

// 生成excel
void CExcelToJsonDlg::OnBnClickedButtonToExcel()
{
	if (!IsFileExists(m_strSourceFile))
	{
		AfxMessageBox(L"请选择消息文件！");
		return;
	}
	// 读取文件
	ReadFile(m_strSourceFile);
	// 解析文件
	AnalysisFile();
	// 写入excel
	WriteMsgToExcel();
}

// 转成lua
void CExcelToJsonDlg::OnBnClickedButtonToLua()
{
	// 转成lua
	ExcelToLua();

	return;
}

// 选择文件
void CExcelToJsonDlg::OnBnClickedButtonChoose()
{
	CString	strFilter = L".h文件(*.h)|*.h|";
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

// 选择保存路径
void CExcelToJsonDlg::OnBnClickedButtonSave()
{
	// 获取路径
	BROWSEINFO   bInfo;
	ZeroMemory(&bInfo, sizeof(bInfo));
	bInfo.hwndOwner = m_hWnd;
	TCHAR tchPath[255];
	bInfo.lpszTitle = _T("所选择路径:   ");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST lpDlist;

	//用来保存返回信息的IDList，使用SHGetPathFromIDList函数转换为字符串
	lpDlist = SHBrowseForFolder(&bInfo);   //显示选择对话框
	if (lpDlist != NULL)
	{
		SHGetPathFromIDList(lpDlist, tchPath);//把项目标识列表转化成目录
		m_strSavePath.Format(L"%s", tchPath);
		SetDlgItemText(IDC_EDIT_SAVE, m_strSavePath);
	}
}

// 选择要转换的Excel文件
void CExcelToJsonDlg::OnBnClickedButtonChooseExcel()
{
	CString	strFilter = L"xlsx文件(*.xlsx)|*.xlsx|xls文件(*.xls)|*.xls|";
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

// 通过表名获取表序号
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

// 读取excel文件，注意如果excel文件正在被其它程序打开，则无法读取
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

	// 获取工作簿文件名
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

		// 表数量
		m_nSheetCount = book->sheetCount();

		// 保存工作簿表名
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
				// 读取表名
				CString msg;
				CString sheetName = sheet->name();

				CString configFilePath;
				CString sectionName;
				// 配置文件名
				CString configFileName = L"TableHeader.ini";
				fileName = tempFileName;

				configFilePath = strFileName;
				configFilePath.Replace(fileName, configFileName);

				// 配置文件段名
				fileName.Replace(L".xlsx", L"");
				fileName.Replace(L".xls", L"");
				sectionName = fileName + L"_" + sheetName;

				// 获取配置
				TableConfig config;
				VecTableHeader tableHeader = config.VecTableHeaderConfig;

				// 保存文件名
				CString jsonFileName = sheetName + L"." + config.strFileEx;
				m_strJsonName[sheetCount] = strFileName;
				m_strJsonName[sheetCount].Replace(tempFileName, jsonFileName);

				m_nHeaderCount[sheetCount] = sheet->lastCol() - sheet->firstCol();

				CString tempHeader[100];
				for (int j = 0; j < m_nHeaderCount[sheetCount]; j++)
				{
					//读取表头
					m_arHeader[sheetCount][j] = sheet->readStr(0, j);
					tempHeader[j] = m_arHeader[sheetCount][j];
				}

				// 读取数据 行数
				for (int i = 1; i < sheet->lastRow(); i++) {
					CJsonRow jsonRow;
					int nBlankCount = 0;
					// 列数
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

					// 一整行空白，则结束
					if (nBlankCount == m_nHeaderCount[sheetCount])
						break;

					m_lstData[sheetCount].AddTail(jsonRow);
				}

			} // 读取sheet结束

		} // 循环读取sheet结束

		// 遍历需要转换的表
		for (int sheetCount = 0; sheetCount < book->sheetCount(); sheetCount++)
		{
			Sheet* sheet = book->getSheet(sheetCount);
			if (sheet)
			{
				CString sheetName = sheet->name();
				CString sectionName;
				// 配置文件名
				fileName = tempFileName;

				// 配置文件段名
				fileName.Replace(L".xlsx", L"");
				fileName.Replace(L".xls", L"");
				sectionName = fileName + L"_" + sheetName;

				// 获取配置
				TableConfig config;

				// 保存文件
				if (config.nIsMainTable)
				{
					// 保存文件路径
					CString strOutFileName;

					//输出不同语言lua文件
					for (int i = 2; i < m_nHeaderCount[sheetCount]; i++)
					{
						//输出目录
						CString outFolder = strFileName;
						outFolder.Replace(tempFileName, L"");
						outFolder += L"LuaOutFiles";

						//创建目录
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

		AfxMessageBox(L"转换成功！");
		return TRUE;

	} // 加载工作簿结束
	else {
		CString sz(book->errorMessage());
		sz.Format(L"加载工作簿[%s]失败：%s", tempFileName, sz);
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
			if (strUnicode[i] <= 0x007f)      // 单字节编码  
			{
				offset += 1;
			}
			else if (strUnicode[i] >= 0x0080 && strUnicode[i] <= 0x07ff)   // 双字节编码  
			{
				offset += 2;
			}
			else if (strUnicode[i] >= 0x0800 && strUnicode[i] <= 0xffff)   // 三字节编码  
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
			if (strUnicode[i] <= 0x007f)      // 单字节编码  
			{
				strUTF8[offset++] = (char)(strUnicode[i] & 0x007f);
			}
			else if (strUnicode[i] >= 0x0080 && strUnicode[i] <= 0x07ff)   // 双字节编码  
			{
				strUTF8[offset++] = (char)(((strUnicode[i] & 0x07c0) >> 6) | 0x00c0);
				strUTF8[offset++] = (char)((strUnicode[i] & 0x003f) | 0x0080);
			}
			else if (strUnicode[i] >= 0x0800 && strUnicode[i] <= 0xffff)   // 三字节编码  
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

// char转CString
CString CExcelToJsonDlg::CharToCString(const char* str)
{
	// 计算char *数组大小，以字节为单位，一个汉字占两个字节
	int charLen = strlen(str);

	// 计算多字节字符的大小，按字符计算。
	int len = MultiByteToWideChar(CP_ACP, 0, str, charLen, NULL, 0);

	// 为宽字节字符数组申请空间，数组大小为按字节计算的多字节字符大小
	TCHAR* buf = new TCHAR[len + 1];

	// 多字节编码转换成宽字节编码
	MultiByteToWideChar(CP_ACP, 0, str, charLen, buf, len);

	//添加字符串结尾，注意不是len+1
	buf[len] = '\0';

	// 将TCHAR数组转换为CString
	CString pWideChar;
	pWideChar.Append(buf);

	// 删除缓冲区
	delete[]buf;

	return pWideChar;
}

// 将Unicode字符串转为为utf8字符串并写入文件中
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

// 保存lua文件
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
				// 过滤原消息
				if (j == 1) continue;

				// 消息ID
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

// 读取文件
void CExcelToJsonDlg::ReadFile(CString strFileName)
{
	FILE* fp;

	fopen_s(&fp, (LPCSTR)(_bstr_t)strFileName.GetBuffer(), "rb");
	if (fp == NULL)
		return;

	//获取文件长度
	fseek(fp, 0, SEEK_END);
	m_dwFileSize = ftell(fp);

	try
	{
		//将数据读入内存
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

// 解析数据
void CExcelToJsonDlg::AnalysisFile()
{
	// 初始化文档数据
	CString szFile(m_pData);
	// 解析到的上一条消息
	CString szLastMsg;
	CString szDefine = L"#define";
	CString szMIM;
	CString szLastMIM;
	// 消息段开始
	DWORD dwMsgCode = 0;
	// 当前消息号
	DWORD dwCurCode = 0;
	ResultMap mapResult;
	m_Result.clear();

	for (DWORD i = 0; i < szFile.GetLength(); i++)
	{
		if (szFile[i] == '#')
		{
			CString szTemp = szFile.Mid(i, 7);
			// 查找 #define
			if (szTemp == szDefine)
			{
				// 查找 #define MIM_DATABASE					1000
				if (i >= 22 && szFile[i - 20] == '/' && szFile[i - 21] == '/' && szFile[i - 22] == '/')
				{
					szLastMIM = szMIM;
					szMIM = "";
					i += 7;
					// 查找 MIM_DATABASE
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

					// 查找数字1000
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
							// 将指针往后移
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
				// 查找 #define MIM_DATABASE_0001				(MIM_DATABASE+1)
				else if ((szFile[i - 1] == '\n' && szFile[i - 2] == '/') || (szFile[i - 1] == '\n' && szFile[i - 2] == '\n' && szFile[i - 3] == '/') ||
					    ((szFile[i - 1] == '\n' && szFile[i - 2] == '\r' && szFile[i - 3] == '/') || (szFile[i - 1] == '\n' && szFile[i - 2] == '\r' && szFile[i - 3] == '\n' && szFile[i - 4] == '\r' && szFile[i - 5] == '/')))
				{
					i += 6;
					CString szCode;
					bool bStart = false;
					// 查找+后面的数字
					for (DWORD j = 1; j < 1000; j++)
					{
						if (szFile[i + j] == '+')
						{
							bStart = true;
							continue;
						}
						else if (szFile[i + j] == ')')
						{
							// 将指针往后移
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
			// 查找/* *" "* /
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
	// 插入最后一段消息
	m_Result.insert(make_pair(szMIM, mapResult));

	return;
}

// 判断文件是否存在
bool CExcelToJsonDlg::IsFileExists(CString& name) {
	ifstream f(name);
	return f.good();
}

// 写入excel
void CExcelToJsonDlg::WriteMsgToExcel()
{
	BOOL isXlsx = true;
	Book* book;
	if (isXlsx)
		book = xlCreateXMLBook();
	else
		book = xlCreateBook();

	// 设置密钥
	book->setKey(XX, YY);

	// excel文件
	CString szFile;
	if (m_strSavePath != L"")
		szFile = m_strSavePath + L"\\" + m_strExcelName;
	else 
		szFile = m_strExcelName;

	// 提取数据
	map<CString, DataMap> data;
	// 记录所有表
	map<CString, Sheet*> sheets;
	// 消息ID对应的行
	map<DWORD, int> dataRow;

	if (IsFileExists(szFile))
	{
		// 加载已存在的表
		if (book->load(szFile))
		{
			// 缓存表数据
			for (int sheetCount = 0; sheetCount < book->sheetCount(); sheetCount++)
			{
				Sheet* sheet = book->getSheet(sheetCount);

				if (sheet)
				{
					CString sheetName = sheet->name();
					DataMap res;

					// 记录表
					sheets.insert(make_pair(sheetName, sheet));

					// 读取数据 行数
					for (int i = 1; i < sheet->lastRow(); i++)
					{
						// 列数
						DWORD dwValue = sheet->readNum(i, 0);
						CString strValue = sheet->readStr(i, 1);
						res.insert(make_pair(dwValue, strValue));
						dataRow.insert(make_pair(dwValue, i));
					}

					data.insert(make_pair(sheetName, res));
				}
			}

			// 遍历主消息
			for (auto sh = m_MIMOrder.begin(); sh != m_MIMOrder.end(); sh++)
			{
				// 新建sheet
				CString sheetName;
				sheetName = sh->second;
				Sheet* sheet = NULL;

				// 如果表不存在
				auto da = data.find(sheetName);
				if (da == data.end())
				{
					sheet = book->addSheet(sheetName);
					WriteSheetData(book, sheet, sh->second);
				}
				// 存在表，对比ID是否存在
				else
				{
					sheet = sheets[sheetName];
					auto allrow = da->second;

					// 新建样式
					Format* newFormat = book->addFormat();
					newFormat->setAlignV(ALIGNV_CENTER);
					newFormat->setAlignH(ALIGNH_LEFT);

					auto it = m_Result[sh->second];
					// 遍历消息
					for (auto row = it.begin(); row != it.end(); row++)
					{
						// 新建样式
						Format* form = book->addFormat();
						form->setAlignV(ALIGNV_CENTER);
						form->setAlignH(ALIGNH_LEFT);
						form->setBorder(BORDERSTYLE_THIN);
						form->setBorderColor(COLOR_RED);

						// 插入消息
						CString szMsg;
						szMsg = row->second;

						// 根据ID查找是否存在
						auto curRow = allrow.find(row->first);
						if (curRow != allrow.end())
						{
							// 消息内容不一致，替换并清空翻译
							if (curRow->second != row->second)
							{
								sheet->writeStr(dataRow[row->first], 1, szMsg, newFormat);
								sheet->writeBlank(dataRow[row->first], 2, form);
								sheet->writeBlank(dataRow[row->first], 3, form);
							}
						}
						// 未找到 则插入
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
			sz.Format(L"加载工作簿失败：%s", sz);
			AfxMessageBox(sz);
			return;
		}
	}
	else
	{
		// 遍历主消息
		for (auto sh = m_MIMOrder.begin(); sh != m_MIMOrder.end(); sh++)
		{
			// 新建sheet
			CString sheetName;
			sheetName = sh->second;
			Sheet* sheet = book->addSheet(sheetName);

			WriteSheetData(book, sheet, sh->second);
		}
		book->save(szFile);
	}


	AfxMessageBox(L"生成成功！");
}

// 写入表数据
void CExcelToJsonDlg::WriteSheetData(Book* book, Sheet* sheet, CString msg)
{
	// 创建一个字体对象
	libxl::Font* font = book->addFont();
	font->setColor(COLOR_RED);
	font->setBold(true);

	Format* f = book->addFormat();
	f->setAlignV(ALIGNV_CENTER);
	f->setAlignH(ALIGNH_CENTER);
	f->setFont(font);
	f->setFillPattern(FILLPATTERN_SOLID);
	f->setPatternForegroundColor(COLOR_LIME);

	// 插入表头
	CString id("id");
	CString message("message");
	CString English("English");
	CString Portuguese("Portuguese");
	sheet->writeStr(0, 0, id, f);
	sheet->writeStr(0, 1, message, f);
	sheet->writeStr(0, 2, English, f);
	sheet->writeStr(0, 3, Portuguese, f);

	// 列宽
	sheet->setCol(1, 3, 90);
	// 首行高
	sheet->setRow(0, 25);

	// 新建样式
	Format* newFormat = book->addFormat();
	newFormat->setAlignV(ALIGNV_CENTER);
	newFormat->setAlignH(ALIGNH_LEFT);

	DWORD index = 1;
	auto it = m_Result[msg];
	// 遍历消息
	for (auto row = it.begin(); row != it.end(); row++)
	{
		// 插入消息
		CString szMsg;
		szMsg = row->second;
		sheet->writeNum(index, 0, row->first, newFormat);
		sheet->writeStr(index, 1, szMsg, newFormat);
		sheet->writeBlank(index, 2, newFormat);
		sheet->writeBlank(index, 3, newFormat);
		index++;
	}
}

// 将excel转成lua
void CExcelToJsonDlg::ExcelToLua()
{
	if (!IsFileExists(m_strExcelFile))
	{
		AfxMessageBox(L"请选择要转换的Excel文件！");
		return;
	}
	ReadExcel(m_strExcelFile);
}