#include "stdafx.h"
#include "ConfigCenter.h"

std::shared_ptr<CConfigCenter> CConfigCenter::m_Instance = nullptr;

CConfigCenter& CConfigCenter::GetInstance()
{
	if (m_Instance == nullptr)
	{
		m_Instance = std::make_shared<CConfigCenter>();
	}
	return *m_Instance;
}

CConfigCenter::CConfigCenter()
{
	InitConfig();
}

CConfigCenter::~CConfigCenter()
{

}

void CConfigCenter::InitConfig()
{
	m_TableConfigMap.clear();
}

bool CConfigCenter::LoadConfig(CString path, CString section)
{
	TableConfig tableConfig;

	TCHAR buf[500] = { 0 };
	int nSize = sizeof(buf) / sizeof(buf[0]);

	// 表字段总数
	tableConfig.nFiledCount = 0;

	// 是否第一列作为主ID，后面所有作为Option
	tableConfig.nIsFirstColMainKey = GetPrivateProfileInt(section, _T("IsFirstColMainKey"), 0, path);

	// 是否主表
	tableConfig.nIsMainTable = GetPrivateProfileInt(section, _T("IsMainTable"), 0, path);

	// 文件名
	memset(buf, 0, sizeof(buf));
	GetPrivateProfileString(section, _T("FileName"), _T("ResultData"), buf, nSize, path);
	tableConfig.strJsonName.Format(L"%s", buf);

	// 文件扩展名
	memset(buf, 0, sizeof(buf));
	GetPrivateProfileString(section, _T("FileExtension"), _T("json"), buf, nSize, path);
	tableConfig.strFileEx.Format(L"%s", buf);

	for (int i = 0; i < MAX_HEADER_COUNT; i++)
	{
		CString num;
		num.Format(L"%d", i + 1);
		memset(buf, 0, sizeof(buf));
		GetPrivateProfileString(section, num, _T("NULL"), buf, nSize, path);

		if (!_tcscmp(buf, _T("NULL")))
			break;

		tableConfig.nFiledCount++;

		TableHeader header;
		CString temp;
		int index = 0;
		header.nDecimalNum = 0;
		AfxExtractSubString(header.tableFiled, buf, index++, ',');
		AfxExtractSubString(header.jsonFiled, buf, index++, ',');
		AfxExtractSubString(header.typeName, buf, index++, ',');
		if (header.typeName == L"double")
		{
			AfxExtractSubString(temp, buf, index++, ',');
			header.nDecimalNum = _ttoi(temp);
		}
		else if (header.typeName == L"table")
		{
			AfxExtractSubString(temp, buf, index++, ',');
			header.nColCount = _ttoi(temp);

			for (int j = 0; j < header.nColCount; j++)
			{
				AfxExtractSubString(header.tableArrayFiled[j], buf, index++, ',');
			}

			for (int j = 0; j < header.nColCount; j++)
			{
				AfxExtractSubString(header.tableTypeArray[j], buf, index++, ',');

				if (header.tableTypeArray[j] == L"double")
				{
					AfxExtractSubString(temp, buf, index++, ',');
					header.nDecimalNumArray[j] = _ttoi(temp);
				}
			}

			AfxExtractSubString(temp, buf, index++, ',');
			header.nIsCancelBrackets = _ttoi(temp);
		}
		else if (header.typeName == L"special1")
		{
			AfxExtractSubString(temp, buf, index++, ',');
			header.nColCount = _ttoi(temp);

			for (int j = 0; j < header.nColCount + 1; j++)
			{
				AfxExtractSubString(header.tableArrayFiled[j], buf, index++, ',');
			}

			for (int j = 0; j < header.nColCount; j++)
			{
				AfxExtractSubString(header.tableTypeArray[j], buf, index++, ',');

				if (header.tableTypeArray[j] == L"double")
				{
					AfxExtractSubString(temp, buf, index++, ',');
					header.nDecimalNumArray[j] = _ttoi(temp);
				}
			}
		}
		
		if (header.tableFiled == L"" || header.jsonFiled == L"" || header.typeName == L"")
		{
			AfxMessageBox(L"表头配置错误，请检查配置！");
			return FALSE;
		}

		tableConfig.VecTableHeaderConfig.push_back(header);
	}

	m_TableConfigMap.insert(TableConfigMap::value_type(section, tableConfig));

	return TRUE;
}

int CConfigCenter::GetTableConfigBySection(CString section, TableConfig& config)
{
	TableConfigMap::iterator it = m_TableConfigMap.find(section);
	if (it != m_TableConfigMap.end())
	{
		config = it->second;
		return 0;
	}
	return -1;
}