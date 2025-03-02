
// CJExcelToJsonDlg.h : ͷ�ļ�
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
	int			nDecimalNum;			// С��λ��
	int			nRowCount;				// ��������(δ��ȡ)
	int			nColCount;				// ��������
	int			nIsCancelBrackets;		// (table�ֶ�)�Ƿ�ȡ��������[]
	CellType	ccellType;
	CString		strType;
	CString		tableArrayFiled[10];	// �����ֶ���
	CString		tableTypeArray[10];		// �����ֶ�����
	int			nDecimalNumArray[10];	// С��λ��
};

class CJsonRow
{
public:
	CJsonNode jsonNode[MAX_HEADER_COUNT];
};

typedef CList<CJsonRow, CJsonRow&> JsonDataList;
typedef map<DWORD, CString> ResultMap;
typedef map<DWORD, CString> DataMap;

// CExcelToJsonDlg �Ի���
class CExcelToJsonDlg : public CDialogEx
{
// ����
public:
	CExcelToJsonDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CJEXCELTOJSON_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	// ����Excel
	afx_msg void OnBnClickedButtonToExcel();
	// ת��Lua
	afx_msg void OnBnClickedButtonToLua();
	// ѡ���ļ�
	afx_msg void OnBnClickedButtonChoose();
	// ѡ�񱣴�·��
	afx_msg void OnBnClickedButtonSave();
	// ѡ��Ҫת����Excel�ļ�
	afx_msg void OnBnClickedButtonChooseExcel();

	// ��ʼ������ 
	void InitData();

public:
	CString							m_strFileName;										// ��ǰ�򿪵��ļ���
	CString							m_strJsonName[MAX_SHEET_COUNT];						// Ҫ�����json�ļ���
	CString							m_strSheetName[MAX_SHEET_COUNT];					// ���������б���
	CString							m_arHeader[MAX_SHEET_COUNT][100];					// ��ͷ
	JsonDataList					m_lstData[MAX_SHEET_COUNT];							// ����
	int								m_nHeaderCount[MAX_SHEET_COUNT];					// ����
	int								m_nSheetCount;										// �����������
	CString							m_strExcelName;										// ����������
	char*							m_pData;											// �ļ�����
	DWORD							m_dwFileSize;										// �ļ���С
	map<CString, ResultMap>			m_Result;											// �������
	map<DWORD, CString>				m_MIMOrder;											// ��Ϣ����

	CString							m_strSavePath;										// ����·��
	CString							m_strSourceFile;									// �ļ�·��
	CString							m_strExcelFile;										// �ļ�·��

	//�½�Ŀ¼
	void CreateFolder(CString szFolder);

	// ��ȡexcel�ļ�
	BOOL ReadExcel(CString strFileName);

	// ������ȡ�����
	int GetSheetIndexBySheetName(CString sheetName);

	// ����lua�ļ�
	CString SaveLua(JsonDataList &jsonDataList, CString szOutFileName, int nHeaderCount, CString strHeader[100], CString sheetName, CString szLanguage);

	// д���ļ�
	void WriteUnicodeStringToUtf8File(FILE *fp, CString strText);

	// charתCString
	CString CharToCString(const char* str);

	// �ж��ļ��Ƿ����
	bool IsFileExists(CString& name);

	// ��ȡ�ļ�
	void ReadFile(CString strFileName);

	// ��������
	void AnalysisFile();

	// ����Ϣд��excel
	void WriteMsgToExcel();

	// д�������
	void WriteSheetData(Book * book, Sheet * sheet, CString msg);

	// ��excelת��lua
	void ExcelToLua();
};
