#include "StdAfx.h"
#include "SqliteData.h"

///////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
CSqliteDataItemSink::CSqliteDataItemSink():m_db(nullptr)
{
	Init();
}

CSqliteDataItemSink::~CSqliteDataItemSink()
{
	if (m_db != nullptr)
	{
		sqlite3_close(m_db);
		m_db = nullptr;
	}
}

void CSqliteDataItemSink::Init()
{
	m_GameList[525] = "88Fortunes";
	m_GameList[548] = "Vegas777";
	m_GameList[535] = "FruitSlot9";
	m_GameList[530] = "EgyptTreasure";
	m_GameList[546] = "HotBuffalo777";
	m_GameList[533] = "MaryFruit";
}

std::string CSqliteDataItemSink::MbcsToUtf8(const char * pszMbcs)
{
	std::string str;
	WCHAR * pwchar = 0;
	CHAR * pchar = 0;
	int len = 0;
	int codepage = AreFileApisANSI() ?
		CP_ACP : CP_OEMCP;
	len = MultiByteToWideChar(codepage, 0, pszMbcs, -1, NULL, 0);
	pwchar = new WCHAR[len];
	if (pwchar != 0)
	{
		len = MultiByteToWideChar(codepage, 0, pszMbcs, -1, pwchar, len);
		if (len != 0)
		{
			len = WideCharToMultiByte(CP_UTF8, 0, pwchar, -1, 0, 0, 0, 0);
			pchar = new CHAR[len];
			if (pchar != 0)
			{
				len = WideCharToMultiByte(CP_UTF8, 0, pwchar, -1, pchar, len, 0, 0);
				if (len != 0)
				{
					str = pchar;
				}
				delete[] pchar;
			}
		}
		delete[] pwchar;
	}
	return str;
}

//�������ݿ�·��
string CSqliteDataItemSink::OnSqliteString(string str,TCHAR nNickName[128],BYTE cbOrder)
{
	int iLen = WideCharToMultiByte(CP_ACP, 0, nNickName, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen * sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, nNickName, -1, chRtn, iLen, NULL, NULL);
	string sName = chRtn;
	delete[] chRtn;
	string strName;
	if (cbOrder == 0)
	{
		strName = str + sName;
	}
	else if (cbOrder == 1)
	{
		strName = str;
	}
	else
	{
		strName = sName;
	}
	
	return MbcsToUtf8(strName.c_str());

	return strName;
}
//�������ݿ⡢û�о��½�
bool CSqliteDataItemSink::OnSqliteDatabaseStorage()
{
	m_db = nullptr;
	int nRes = 0;
	//����·��
	TCHAR szWorkDir[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szWorkDir, CountArray(szWorkDir));

	//�ļ�Ŀ¼
	TCHAR szServiceDir[MAX_PATH] = TEXT("");
	_sntprintf_s(szServiceDir, CountArray(szServiceDir), TEXT("%s\\SqliteData"), szWorkDir);

	//����Ŀ¼
	if (!PathIsDirectory(szServiceDir)) CreateDirectory(szServiceDir, NULL);

	//�����ݿ�
	string strName = CT2A(szServiceDir);
	strName = OnSqliteString(strName+"\\", _T("UserControl.db"), 0);
	//string strName = OnSqliteString("./SqliteData/", _T("UserControl.db"), 0);
	nRes = sqlite3_open(strName.c_str(), &m_db);
	if (nRes != SQLITE_OK) {
		//���ݿ��ʧ��
		return true;
	}

	string UserControlTable("UserControlInfo");
	string strSql = "select count(*) from sqlite_master where type = 'table' and name = "+ UserControlTable +" ";
	int iCount = 0;
	nRes = sqlite3_exec(m_db, strSql.c_str(), CB_sqlite_count, &iCount, NULL);
	if (nRes != SQLITE_OK)
	{
		/* ���iCount = 0,���޴˱�, ��Ϊ1 ���� */
		//return false;
	}

	if (iCount == 0)
	{
		string Tmpsql = "CREATE TABLE UserControlInfo("  \
			"UserId			INT		   NOT NULL," \
			"GameId			INT		   NOT NULL," \
			"KindID			INT		   NOT NULL,"\
			"PosIndex		INT		   NOT NULL,"\
			"RebateRatio	INT		   NOT NULL,"
			"SampleData		INT		   NOT NULL,"\
			"SystemTime TIMESTAMP default (datetime('now', 'localtime')));";

		nRes = sqlite3_exec(m_db, Tmpsql.c_str(), CB_sqlite_count, 0, NULL);
		if (nRes != SQLITE_OK) {
			//�������ɹ�
			//return false;
		}
	}
	string SampleDataTable = ("SampleData");
	strSql = "select count(*) from sqlite_master where type = 'table' and name = " + SampleDataTable + "";
	iCount = 0;
	nRes = sqlite3_exec(m_db, strSql.c_str(), CB_sqlite_count, &iCount, NULL);
	if (nRes != SQLITE_OK)
	{
		/* ���iCount = 0,���޴˱�, ��Ϊ1 ���� */
		//return false;
	}
	if (iCount == 0)
	{
		string Tmpsql = "CREATE TABLE  SampleData 	("  \
			"SampleID			INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," \
			"KindID					INT		default(0)," \
			"GameName				TEXT,"
			"IsDisable			INT			default(0),"
			"RebateRate				INT			default(0),"
			"GameCount			INT		default(50),"
			"DataLen			INT		,"
			"ResultData			nvarchar(512)," \
			"SystemTime TIMESTAMP default (datetime('now', 'localtime')));";
		nRes = sqlite3_exec(m_db, Tmpsql.c_str(), CB_sqlite_count, 0, NULL);
		if (nRes != SQLITE_OK) {
			//�������ɹ�
			//return false;
		}
	}

	//�ر����ݿ�
	//sqlite3_close(db);

	return true;
}


//��������
bool CSqliteDataItemSink::SaveUserControlInfo(tagNewUserControl & NewUserControl)
{
	if (m_db == nullptr)
	{
		OnSqliteDatabaseStorage();
		if (m_db == nullptr)
		{
			ASSERT(FALSE);
			return false;
		}
	}
	//�����û���Ϣ�Ƿ����
	int index = 0;
	int nRes = 0;
	string strTmpSql = "select count(*)  from  UserControlInfo where UserId = " +to_string(NewUserControl.dwUserId) + " and KindID = " + to_string(NewUserControl.wKindID);

	int iCount = 0;
	nRes = sqlite3_exec(m_db, strTmpSql.c_str(), CB_sqlite_count, &iCount, NULL);

	//nRes = sqlite3_get_table(m_db, strTmpSql.c_str(), &dbResult, &nRow, &nColumn, NULL);
	if (nRes != 0)
	{
		ASSERT(FALSE);
		//return false;
	}
	if (iCount == 0)
	{
		//�޴�����Ϣ�������
		strTmpSql.clear();
		strTmpSql = strTmpSql = "insert into  UserControlInfo (UserId,GameId,KindID,PosIndex,RebateRatio,SampleData) values(" + to_string(NewUserControl.dwUserId) + "," +to_string(NewUserControl.dwGameId) +"," + to_string(NewUserControl.wKindID) + \
			"," + to_string(NewUserControl.cbIndex) + "," + to_string(NewUserControl.wRebateRatio) + "," + to_string(NewUserControl.wSelectSampleIndex) + ")";

		nRes = sqlite3_exec(m_db, strTmpSql.c_str(), CB_sqlite_count, &iCount, NULL);
		if (nRes != 0)
		{
			ASSERT(FALSE);
			//return false;
		}
	}
	do
	{
		//����
		strTmpSql.clear();
		strTmpSql = "update  UserControlInfo set PosIndex = " + to_string(NewUserControl.cbIndex) + " where UserId = " + to_string(NewUserControl.dwUserId);
		nRes = sqlite3_exec(m_db, strTmpSql.c_str(), CB_sqlite_count, &iCount, NULL);
		if (nRes != 0)
		{
			ASSERT(FALSE);
			return false;
		}
	} while (false);

	/*sqlite3_close(m_db);*/
	return true;
}

//��ѯ�û���Ϣ
bool CSqliteDataItemSink::QueryUserControlInfo(tagNewUserControl & NewUserControl)
{
	if (m_db == nullptr)
	{
		OnSqliteDatabaseStorage();
		if (m_db == nullptr)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	sqlite3_stmt * stmt;

	//�����û���Ϣ�Ƿ����
	char ** dbResult;
	int nRow, nColumn = 0;
	int index = 0;
	int nRes = 0;
	string strTmpSql = "select count(*)  from  UserControlInfo where UserId = " + to_string(NewUserControl.dwUserId) + " and KindID = " + to_string(NewUserControl.wKindID);

	int iCount = 0;
	nRes = sqlite3_exec(m_db, strTmpSql.c_str(), CB_sqlite_count, &iCount, NULL);

	//nRes = sqlite3_get_table(m_db, strTmpSql.c_str(), &dbResult, &nRow, &nColumn, NULL);
	if (nRes != 0)
	{
		ASSERT(FALSE);
		//return false;
	}
	if (iCount == 0)
	{
		//�򹹽��û�ʹ�õ�����
		strTmpSql.clear();
		strTmpSql = "select * from SampleData where KindID = " + to_string(NewUserControl.wKindID);
		nRes = sqlite3_exec(m_db, strTmpSql.c_str(), CB_sqlite_count, &iCount, NULL);
		if (nRes != 0)
		{
			ASSERT(FALSE);
			//return false;
		}
		nRes = sqlite3_get_table(m_db, strTmpSql.c_str(), &dbResult, &nRow, &nColumn, NULL);
		if (nRes != SQLITE_OK || nRow == 0)
		{
			//�ر����ݿ�
			sqlite3_free_table(dbResult);
			//sqlite3_close(m_db);
			return false;
		}

		tagSampleData SampleData;
		ZeroMemory(&SampleData, sizeof(SampleData));
		int nIndex = rand() % nRow;

		// ��ȡ����
		char sql[512] = {};
		sprintf(sql, "select SampleID,KindID,DataLen,ResultData from SampleData where  KindID = %d and IsDisable<>1 order by random() limit 1;", NewUserControl.wKindID);
		if (sqlite3_prepare(m_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
		{
			puts(sqlite3_errmsg(m_db));
		}

		string strResult;

		do {
			sqlite3_step(stmt);
			const char * len;
			SampleData.dwID = atoi((const char *)sqlite3_column_text(stmt, 0));
			SampleData.wKindID = atoi((const char *)sqlite3_column_text(stmt, 1));
			len = (const char *)sqlite3_column_text(stmt, 2);
			VOID * Info = (VOID *)sqlite3_column_text(stmt, 3);
			if (Info != NULL)
			{
				CopyMemory(SampleData.cbBuffer, Info, atoi(len));
			}
			strResult = string(SampleData.cbBuffer);
			break;

		} while (false);

		//NewUserControl.cbIndex = 0;
		NewUserControl.wSelectSampleIndex = SampleData.dwID;
		NewUserControl.wTimesCount = 0;
		string tmpStr;
		int nCount = 0;
		for (int i = 0; i < strResult.size(); i++)
		{
			tmpStr += strResult[i];
			if (strResult[i] == ',')
			{
				NewUserControl.wTimesList[NewUserControl.wTimesCount++] = atoi(tmpStr.c_str());
				tmpStr.clear();

			}
			if (strResult[i] == ':')
			{
				NewUserControl.wRebateRatio = atoi(tmpStr.c_str());
				tmpStr.clear();
			}
		}
		if (tmpStr.size() > 0)
		{
			NewUserControl.wTimesList[NewUserControl.wTimesCount++] = atoi(tmpStr.c_str());
			tmpStr.clear();
		}

		strTmpSql.clear();
		strTmpSql = "insert into  UserControlInfo (UserId,GameId,KindID,PosIndex,RebateRatio,SampleData) values(" + to_string(NewUserControl.dwUserId) + "," + to_string(NewUserControl.dwGameId) + "," + to_string(NewUserControl.wKindID) + \
			"," + to_string(NewUserControl.cbIndex) + "," + to_string(NewUserControl.wRebateRatio) + "," + to_string(NewUserControl.wSelectSampleIndex)   + ")";

		nRes = sqlite3_exec(m_db, strTmpSql.c_str(), CB_sqlite_count, &iCount, NULL);
		if (nRes != 0)
		{
			ASSERT(FALSE);
			//return false;
		}
		sqlite3_free_table(dbResult);
		sqlite3_finalize(stmt);
		//sqlite3_close(m_db);
		//m_db = nullptr;
	}
	else
	{
		//����
		strTmpSql.clear();
		strTmpSql = "select * from UserControlInfo where UserId = " + to_string(NewUserControl.dwUserId) + " and KindID = " + to_string(NewUserControl.wKindID);
		nRes = sqlite3_get_table(m_db, strTmpSql.c_str(), &dbResult, &nRow, &nColumn, NULL);
		if (nRes != SQLITE_OK || nRow == 0)
		{
			sqlite3_free_table(dbResult);
			//�ر����ݿ�
			//sqlite3_close(m_db);
			return false;
		}
		int nIndex = nColumn;
		NewUserControl.cbIndex = atoll(dbResult[nIndex + 3]);
		NewUserControl.wRebateRatio = atoll(dbResult[nIndex + 4]);
		NewUserControl.wSelectSampleIndex = atoll(dbResult[nIndex + 5]);
		NewUserControl.wTimesCount = 0;

		strTmpSql.clear();
		strTmpSql = "select * from SampleData where SampleID = " + to_string(NewUserControl.wSelectSampleIndex);
		nRes = sqlite3_exec(m_db, strTmpSql.c_str(), CB_sqlite_count, &iCount, NULL);

		sqlite3_free_table(dbResult);

		nRes = sqlite3_get_table(m_db, strTmpSql.c_str(), &dbResult, &nRow, &nColumn, NULL);

		if (nRes != SQLITE_OK || nRow == 0)
		{
			sqlite3_free_table(dbResult);
			//�ر����ݿ�
			//sqlite3_close(m_db);
			return false;
		}

		tagSampleData SampleData;
		ZeroMemory(&SampleData, sizeof(SampleData));
		string strResult;

		// ��ȡ����
		char sql[512] = {};
		sprintf(sql, "select SampleID,KindID,DataLen,ResultData from SampleData where  SampleID = %d;", NewUserControl.wSelectSampleIndex);
		if (sqlite3_prepare(m_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
		{
			puts(sqlite3_errmsg(m_db));
		}

		do {
			sqlite3_step(stmt);
			const char * len;
			SampleData.dwID = atoi((const char *)sqlite3_column_text(stmt, 0));
			SampleData.wKindID = atoi((const char *)sqlite3_column_text(stmt, 1));
			len = (const char *)sqlite3_column_text(stmt, 2);
			VOID * Info = (VOID *)sqlite3_column_text(stmt, 3);
			if (Info != NULL)
			{
				CopyMemory(SampleData.cbBuffer, Info, atoi(len));
			}
			strResult = string(SampleData.cbBuffer);
			break;

		} while (false);

		string tmpStr;
		int nCount = 0;
		for (int i = 0; i < strlen(SampleData.cbBuffer); i++)
		{
			tmpStr += SampleData.cbBuffer[i];
			if (SampleData.cbBuffer[i] == ',')
			{
				NewUserControl.wTimesList[NewUserControl.wTimesCount++] = atoll(tmpStr.c_str());
				tmpStr.clear();

			}
			if (SampleData.cbBuffer[i] == ':')
			{
				NewUserControl.wRebateRatio = atoll(tmpStr.c_str());
				tmpStr.clear();
			}
		}
		if (tmpStr.size() > 0)
		{
			NewUserControl.wTimesList[NewUserControl.wTimesCount++] = atoll(tmpStr.c_str());
			tmpStr.clear();
		}
		sqlite3_free_table(dbResult);
		sqlite3_finalize(stmt);
		//sqlite3_free(stmt);
	}
	
	//sqlite3_close(m_db);
	
	return true;
}



//���·�����
bool CSqliteDataItemSink::UpdateRebateRatio()
{
	if (m_db == nullptr)
	{
		OnSqliteDatabaseStorage();
		if (m_db == nullptr)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	sqlite3_stmt * stmt;

	// ��ȡ����
	char sql[512] = {};
	sprintf(sql, "select SampleID,KindID,DataLen,ResultData from SampleData ;");
	if (sqlite3_prepare(m_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
	{
		puts(sqlite3_errmsg(m_db));
	}

	string strResult;

	do {
		if (sqlite3_step(stmt) == SQLITE_DONE) break;
		const char * len;
		char cbBuffer[512] = {};
		DWORD dwID = atoi((const char *)sqlite3_column_text(stmt, 0));
		WORD wKindID = atoi((const char *)sqlite3_column_text(stmt, 1));
		len = (const char *)sqlite3_column_text(stmt, 2);
		VOID * Info = (VOID *)sqlite3_column_text(stmt, 3);
		if (Info != NULL)
		{
			CopyMemory(cbBuffer, Info, atoi(len));
		}
		strResult = string(cbBuffer);
		string tmpStr;
		WORD wRebateRatio = 0;
		for (int i = 0; i < strResult.size(); i++)
		{
			tmpStr += strResult[i];
			if (strResult[i] == ':')
			{
				wRebateRatio = atoi(tmpStr.c_str());
				tmpStr.clear();
				break;
			}
		}
		string strTmpSql;
		int iCount = 0;
		//����
		strTmpSql.clear();
		strTmpSql = "update  SampleData set RebateRate = " + to_string(wRebateRatio) + " where SampleID = " + to_string(dwID);
		int nRes = sqlite3_exec(m_db, strTmpSql.c_str(), CB_sqlite_count, &iCount, NULL);
		if (nRes != 0)
		{
			ASSERT(FALSE);
			return false;
		}

	} while (true);
	sqlite3_finalize(stmt);

	return true;
}

//��������
bool CSqliteDataItemSink::SaveSampleData(tagSampleData & SampleData)
{
	if (m_db == nullptr)
	{
		OnSqliteDatabaseStorage();
		if (m_db == nullptr)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	string tmpStr; 
	BYTE cbDisable = 0;
	WORD wTimesCount = 0;;
	WORD wRebateRatio = 0;
	for (int i = 0; ; i++)
	{
		if (SampleData.cbBuffer[i] == '\0') break;
		tmpStr += SampleData.cbBuffer[i];
		if (SampleData.cbBuffer[i] == ',')
		{
			wTimesCount++;
			tmpStr.clear();

		}
		if (SampleData.cbBuffer[i] == ':')
		{
			wRebateRatio = atoi(tmpStr.c_str());
			tmpStr.clear();
		}
	}
	if (tmpStr.size() > 0)
	{
		wTimesCount++;
		tmpStr.clear();
	}


	sqlite3_stmt * stmt;
	int nRes = 0;
	int iCount = 0;

	string  nDataSize(to_string(strlen(SampleData.cbBuffer)));
	string tmpId (to_string(SampleData.wKindID));
	char strSql[512] = {};
	sprintf(strSql, "insert into SampleData(KindID,GameName,IsDisable,RebateRate,GameCount,DataLen,ResultData) values(?,?,?,?,?,?,?);");
	nRes = sqlite3_prepare(m_db, strSql, strlen(strSql), &stmt, NULL);
	if (nRes != SQLITE_OK)
	{
		ASSERT(FALSE);
		puts(sqlite3_errmsg(m_db));
	}

	if (sqlite3_bind_text(stmt, 1, tmpId.c_str(), tmpId.size(), SQLITE_TRANSIENT) != SQLITE_OK)
	{
		ASSERT(FALSE);
		puts(sqlite3_errmsg(m_db));
	}
	if (sqlite3_bind_text(stmt, 2, m_GameList[SampleData.wKindID].c_str(), strlen(m_GameList[SampleData.wKindID].c_str()), SQLITE_TRANSIENT) != SQLITE_OK)
	{
		ASSERT(FALSE);
		puts(sqlite3_errmsg(m_db));
	}
	if (sqlite3_bind_text(stmt, 3, to_string(cbDisable).c_str(), to_string(cbDisable).size(), SQLITE_TRANSIENT) != SQLITE_OK)
	{
		ASSERT(FALSE);
		puts(sqlite3_errmsg(m_db));
	}
	if (sqlite3_bind_text(stmt, 4, to_string(wRebateRatio).c_str(), to_string(wRebateRatio).size(), SQLITE_TRANSIENT) != SQLITE_OK)
	{
		ASSERT(FALSE);
		puts(sqlite3_errmsg(m_db));
	}
	if (sqlite3_bind_text(stmt, 5, to_string(wTimesCount).c_str(), to_string(wTimesCount).size(), SQLITE_TRANSIENT) != SQLITE_OK)
	{
		ASSERT(FALSE);
		puts(sqlite3_errmsg(m_db));
	}
	if (sqlite3_bind_text(stmt, 6, nDataSize.c_str(), tmpId.size(), SQLITE_TRANSIENT) != SQLITE_OK)
	{
		ASSERT(FALSE);
		puts(sqlite3_errmsg(m_db));
	}
	if (sqlite3_bind_text(stmt, 7, SampleData.cbBuffer, strlen(SampleData.cbBuffer), SQLITE_TRANSIENT) != SQLITE_OK)
	{
		ASSERT(FALSE);
		puts(sqlite3_errmsg(m_db));
	}

	sqlite3_step(stmt);
	sqlite3_finalize(stmt);


	//�ر����ݿ�
	//sqlite3_close(m_db);
	return true;
}
//��ѯ�û���Ϣ
bool CSqliteDataItemSink::QuerySampleData(tagSampleData & SampleData)
{
	if (m_db == nullptr)
	{
		OnSqliteDatabaseStorage();
		if (m_db == nullptr)
		{
			ASSERT(FALSE);
			return false;
		}
	}


	sqlite3_stmt * stmt;

	// ��ȡ����
	char sql[512] = {};
	sprintf(sql, "select SampleID,KindID,DataLen,ResultData from SampleData where  KindID = %d;", SampleData.wKindID);
	if (sqlite3_prepare(m_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
		puts(sqlite3_errmsg(m_db));
	//һ��ֻ��ȡһ��
	
	sqlite3_step(stmt);
	string strResult;
	const char * len;
	SampleData.dwID = atoi((const char *)sqlite3_column_text(stmt, 0));
	SampleData.wKindID = atoi((const char *)sqlite3_column_text(stmt, 1));
	len = (const char *)sqlite3_column_text(stmt, 2);
	VOID * Info = (VOID *)sqlite3_column_text(stmt, 3);
	if (Info != NULL)
	{
		CopyMemory(SampleData.cbBuffer, Info, atoi(len));
	}
	strResult = string(SampleData.cbBuffer);
	sqlite3_finalize(stmt);

	//sqlite3_close(m_db);
	return true;
}


//ɾ������
bool CSqliteDataItemSink::DeleteUserControlInfo(DWORD dwUserId, WORD wKindID)
{
	return true;
	if (m_db == nullptr)
	{
		OnSqliteDatabaseStorage();
		if (m_db == nullptr)
		{
			ASSERT(FALSE);
			return false;
		}
	}
	////����KindID��UserID ɾ��
	int nRes = 0;
	string strTmpSql;

	if (wKindID != 65535)
	{
		strTmpSql = "delete from UserControlInfo where UserId = " + to_string(dwUserId) + " and KindID = " + to_string(wKindID);
	}
	else
	{
		strTmpSql = "delete from UserControlInfo where UserId = " + to_string(dwUserId);
	}
	int iCount = 0;
	nRes = sqlite3_exec(m_db, strTmpSql.c_str(), CB_sqlite_count, &iCount, NULL);

	if (nRes != 0)
	{
		ASSERT(FALSE);
		//return false;
	}
	//sqlite3_close(m_db);
	return true;
}

//ɾ������
bool CSqliteDataItemSink::DeleteSampleData(WORD wKindID)
{
	if (m_db == nullptr)
	{
		OnSqliteDatabaseStorage();
		if (m_db == nullptr)
		{
			ASSERT(FALSE);
			return false;
		}
	}
	//����KindIDɾ��
	int nRes = 0;
	string strTmpSql = "delete from SampleData where KindID = " + to_string(wKindID);

	int iCount = 0;
	nRes = sqlite3_exec(m_db, strTmpSql.c_str(), CB_sqlite_count, &iCount, NULL);

	if (nRes != 0)
	{
		ASSERT(FALSE);
		//return false;
	}
	//sqlite3_close(m_db);
	return true;
}