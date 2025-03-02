#pragma once

#ifndef _CONFIG_CENTER__
#define _CONFIG_CENTER__ 

#include <iostream>
#include <atlstr.h>
#include <vector>
#include <map>

#define MAX_SHEET_COUNT 50
#define MAX_HEADER_COUNT 100

struct TableHeader
{
	CString tableFiled;				// ���ֶ�
	CString jsonFiled;				// ��Ӧjson�ֶ�
	CString typeName;				// �ֶ�����
	int		nDecimalNum;			// С��λ��
	int		nRowCount;				// ��������(δ��ȡ)
	int		nColCount;				// ��������
	int		nIsCancelBrackets;		// (table�ֶ�)�Ƿ�ȡ��������[]
	CString tableArrayFiled[10];	// �����ֶ���
	CString tableTypeArray[10];		// �����ֶ�����
	int		nDecimalNumArray[10];	// С��λ��
};

typedef std::vector<TableHeader> VecTableHeader;

struct TableConfig
{
	int		nFiledCount;			// �ֶ���
	int		nIsFirstColMainKey;		// �Ƿ��һ����Ϊ��ID������������ΪOption
	int		nIsMainTable;			// �Ƿ�����
	CString strJsonName;			// ���ɵ��ļ���
	CString strFileEx;				// ���ɵ��ļ���չ��
	VecTableHeader VecTableHeaderConfig;

	TableConfig()
	{
		nFiledCount = 0;
		nIsFirstColMainKey = 1;
		nIsMainTable = 1;
		strJsonName = L"ResultData";
		strFileEx = L"lua";
		VecTableHeaderConfig.clear();
	}
};

typedef std::map<CString, TableConfig> TableConfigMap;

class CConfigCenter
{
public:
	CConfigCenter();
	~CConfigCenter();

	static CConfigCenter& GetInstance();

	// ��������
	bool LoadConfig(CString path, CString section);

	// ��ʼ������
	void InitConfig();

	// ��ȡ������
	int GetTableConfigBySection(CString section, TableConfig &config);

	static std::shared_ptr<CConfigCenter> m_Instance;

private:
	TableConfigMap m_TableConfigMap;		// ������ӳ��

};

#endif