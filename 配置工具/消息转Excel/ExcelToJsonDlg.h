
// CJExcelToJsonDlg.h : 头文件
//

#pragma once

#define MAX_SHEET_COUNT 50
#define MAX_HEADER_COUNT 100

#include <vector>
#include <map>
#include "include/libxl.h"
#pragma comment (lib, "lib/libxl.lib")

using namespace std;
using namespace libxl;


class CJsonNode
{
public:
	CString		Name;
	CString		strValue;
	double		dValue;
	long long   nValue;
	int			nDecimalNum;			// 小数位数
	int			nRowCount;				// 数组行数(未读取)
	int			nColCount;				// 数组列数
	int			nIsCancelBrackets;		// (table字段)是否取消中括号[]
	CellType	ccellType;
	CString		strType;
	CString		tableArrayFiled[10];	// 数组字段名
	CString		tableTypeArray[10];		// 数组字段类型
	int			nDecimalNumArray[10];	// 小数位数
};

class CJsonRow
{
public:
	CJsonNode jsonNode[MAX_HEADER_COUNT];
};

typedef CList<CJsonRow, CJsonRow&> JsonDataList;
typedef map<DWORD, CString> ResultMap;
typedef map<DWORD, CString> DataMap;

// CExcelToJsonDlg 对话框
class CExcelToJsonDlg : public CDialogEx
{
// 构造
public:
	CExcelToJsonDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CJEXCELTOJSON_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	// 生成Excel
	afx_msg void OnBnClickedButtonToExcel();
	// 转成Lua
	afx_msg void OnBnClickedButtonToLua();
	// 选择文件
	afx_msg void OnBnClickedButtonChoose();
	// 选择保存路径
	afx_msg void OnBnClickedButtonSave();
	// 选择要转换的Excel文件
	afx_msg void OnBnClickedButtonChooseExcel();

	// 初始化数据 
	void InitData();

public:
	CString							m_strFileName;										// 当前打开的文件名
	CString							m_strJsonName[MAX_SHEET_COUNT];						// 要保存的json文件名
	CString							m_strSheetName[MAX_SHEET_COUNT];					// 工作簿所有表名
	CString							m_arHeader[MAX_SHEET_COUNT][100];					// 列头
	JsonDataList					m_lstData[MAX_SHEET_COUNT];							// 数据
	int								m_nHeaderCount[MAX_SHEET_COUNT];					// 列数
	int								m_nSheetCount;										// 工作簿表个数
	CString							m_strExcelName;										// 工作簿名称
	char*							m_pData;											// 文件数据
	DWORD							m_dwFileSize;										// 文件大小
	map<CString, ResultMap>			m_Result;											// 解析结果
	map<DWORD, CString>				m_MIMOrder;											// 消息索引

	CString							m_strSavePath;										// 保存路径
	CString							m_strSourceFile;									// 文件路径
	CString							m_strExcelFile;										// 文件路径

	//新建目录
	void CreateFolder(CString szFolder);

	// 读取excel文件
	BOOL ReadExcel(CString strFileName);

	// 表名获取表序号
	int GetSheetIndexBySheetName(CString sheetName);

	// 保存lua文件
	CString SaveLua(JsonDataList &jsonDataList, CString szOutFileName, int nHeaderCount, CString strHeader[100], CString sheetName, CString szLanguage);

	// 写入文件
	void WriteUnicodeStringToUtf8File(FILE *fp, CString strText);

	// char转CString
	CString CharToCString(const char* str);

	// 判断文件是否存在
	bool IsFileExists(CString& name);

	// 读取文件
	void ReadFile(CString strFileName);

	// 解析数据
	void AnalysisFile();

	// 将消息写入excel
	void WriteMsgToExcel();

	// 写入表数据
	void WriteSheetData(Book * book, Sheet * sheet, CString msg);

	// 将excel转成lua
	void ExcelToLua();
};
