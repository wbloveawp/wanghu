#pragma once
//游戏头文件
#include "Sqlite.h"
#include <string>
#include<unordered_map>
#include "..\..\..\..\第三方库\sqlite\sqlite3.h"

using std::string;

class CSqliteDataItemSink
{
private:
	sqlite3 * m_db;
	unordered_map<WORD, string>	m_GameList;

private:
	void Init();

public:
	CSqliteDataItemSink();
	virtual ~CSqliteDataItemSink();


public:

	std::string MbcsToUtf8(const char * pszMbcs);
    //字符串组合
    string OnSqliteString(string strName, TCHAR nNickName[128], BYTE cbOrder);
    //查找数据库、没有就新建
	virtual bool OnSqliteDatabaseStorage();

public:
	//保存数据
	virtual bool SaveUserControlInfo(tagNewUserControl & NewUserControl);
	//查询用户信息
	virtual bool QueryUserControlInfo(tagNewUserControl & NewUserControl);
	//删除数据
	virtual bool DeleteUserControlInfo(DWORD dwUserId,WORD wKindID);
	//保存数据
	virtual bool SaveSampleData(tagSampleData & SampleData);
	//查询样本信息
	virtual bool QuerySampleData(tagSampleData & SampleData);
	//删除数据
	virtual bool DeleteSampleData(WORD wKindID);
	
private:
	//更新返奖率
	virtual bool UpdateRebateRatio();

};
