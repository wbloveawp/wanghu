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
	CString tableFiled;				// 表字段
	CString jsonFiled;				// 对应json字段
	CString typeName;				// 字段类型
	int		nDecimalNum;			// 小数位数
	int		nRowCount;				// 数组行数(未读取)
	int		nColCount;				// 数组列数
	int		nIsCancelBrackets;		// (table字段)是否取消中括号[]
	CString tableArrayFiled[10];	// 数组字段名
	CString tableTypeArray[10];		// 数组字段类型
	int		nDecimalNumArray[10];	// 小数位数
};

typedef std::vector<TableHeader> VecTableHeader;

struct TableConfig
{
	int		nFiledCount;			// 字段数
	int		nIsFirstColMainKey;		// 是否第一列作为主ID，后面所有作为Option
	int		nIsMainTable;			// 是否主表
	CString strJsonName;			// 生成的文件名
	CString strFileEx;				// 生成的文件扩展名
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

	// 加载配置
	bool LoadConfig(CString path, CString section);

	// 初始化配置
	void InitConfig();

	// 获取表配置
	int GetTableConfigBySection(CString section, TableConfig &config);

	static std::shared_ptr<CConfigCenter> m_Instance;

private:
	TableConfigMap m_TableConfigMap;		// 表配置映射

};

#endif