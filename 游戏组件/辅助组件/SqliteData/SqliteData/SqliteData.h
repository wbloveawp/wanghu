#pragma once
//��Ϸͷ�ļ�
#include "Sqlite.h"
#include <string>
#include<unordered_map>
#include "..\..\..\..\��������\sqlite\sqlite3.h"

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
    //�ַ������
    string OnSqliteString(string strName, TCHAR nNickName[128], BYTE cbOrder);
    //�������ݿ⡢û�о��½�
	virtual bool OnSqliteDatabaseStorage();

public:
	//��������
	virtual bool SaveUserControlInfo(tagNewUserControl & NewUserControl);
	//��ѯ�û���Ϣ
	virtual bool QueryUserControlInfo(tagNewUserControl & NewUserControl);
	//ɾ������
	virtual bool DeleteUserControlInfo(DWORD dwUserId,WORD wKindID);
	//��������
	virtual bool SaveSampleData(tagSampleData & SampleData);
	//��ѯ������Ϣ
	virtual bool QuerySampleData(tagSampleData & SampleData);
	//ɾ������
	virtual bool DeleteSampleData(WORD wKindID);
	
private:
	//���·�����
	virtual bool UpdateRebateRatio();

};
