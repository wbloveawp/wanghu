#include "StdAfx.h"
#include "ServiceParameter.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define REG_ITEM_NAME				TEXT("Information")					//注册表项
#define PARA_FILE_NAME				TEXT("ServerInfo.ini")				//配置文件

//////////////////////////////////////////////////////////////////////////

//构造函数
CServiceParameter::CServiceParameter()
{	
}

//析构函数
CServiceParameter::~CServiceParameter()
{
}

//获取端口
WORD CServiceParameter::GetCenterPort()
{
	//读取数据
	WORD wCenterProt= ReadNumber(REG_ITEM_NAME, TEXT("CenterPort"));

	return wCenterProt;
}

//设置端口
bool CServiceParameter::SetCenterPort(WORD wCenterPort)
{
	//写入信息
	WriteNumber(REG_ITEM_NAME, TEXT("CenterPort"), wCenterPort);

	return true;
}

//服务器名
bool CServiceParameter::GetServerName(TCHAR szServerName[LEN_SERVER])
{
	//写入信息
	ReadEncryptString(REG_ITEM_NAME,TEXT("ServerName"),szServerName,LEN_SERVER);

	return true;
}

//服务器名
bool CServiceParameter::SetServerName(TCHAR szServerName[LEN_SERVER])
{
	//写入信息
	WriteEncryptString(REG_ITEM_NAME,TEXT("ServerName"), szServerName);

	return true;
}

//服务地址
bool CServiceParameter::GetServiceAddress(tagAddressInfo & AddressInfo)
{
	//设置变量
	AddressInfo.szServiceDomain[0]=0;
	AddressInfo.dwServiceAddr=INADDR_NONE;	

	//读取信息
	TCHAR szServiceAddr[32]=TEXT("\0");
	ReadEncryptString(REG_ITEM_NAME,TEXT("ServiceAddr"),szServiceAddr,32);

	//读取域名
	TCHAR szServiceDomain[LEN_DOMAIN]=TEXT("");
	ReadEncryptString(REG_ITEM_NAME,TEXT("ServiceDomain"),szServiceDomain,LEN_DOMAIN);

	//转换数据
#ifndef _UNICODE
	AddressInfo.dwServiceAddr=atol(szServiceAddr);	
#else
	AddressInfo.dwServiceAddr=_tcstoul(szServiceAddr,NULL,0);
#endif

	//拷贝域名
	StringCchCopy(AddressInfo.szServiceDomain,CountArray(AddressInfo.szServiceDomain), szServiceDomain);

	return true;
}

//服务地址
bool CServiceParameter::SetServiceAddress(const tagAddressInfo & AddressInfo)
{
	//转换数据
	CString strServiceAddr;
	strServiceAddr.Format(_T("%u"), AddressInfo.dwServiceAddr);

	//写入信息
	WriteEncryptString(REG_ITEM_NAME,TEXT("ServiceAddr"),strServiceAddr);

	//写入域名
	WriteEncryptString(REG_ITEM_NAME,TEXT("ServiceDomain"),CW2CT(AddressInfo.szServiceDomain));

	return true;
}

//中心地址
bool CServiceParameter::GetCenterAddress(tagAddressInfo & AddressInfo)
{
	//设置变量
	AddressInfo.dwServiceAddr=INADDR_NONE;
	
	//定义变量
	TCHAR strServiceAddr[32]=TEXT("\0");

	//读取信息
	ReadEncryptString(REG_ITEM_NAME,TEXT("CenterServerAddr"),strServiceAddr,32);

	//转换数据
	AddressInfo.dwServiceAddr= _tcstoul(strServiceAddr,NULL,0);

	return true;
}

//中心地址
bool CServiceParameter::SetCenterAddress(const tagAddressInfo & AddressInfo)
{
	//定义变量
	CString strServiceAddr;
	strServiceAddr.Format(TEXT("%u"), AddressInfo.dwServiceAddr);

	//写入信息
	WriteEncryptString(REG_ITEM_NAME,TEXT("CenterServerAddr"),strServiceAddr);

	return true;
}

//消息数据库
bool CServiceParameter::GetMessageDBInfo(tagDataBaseParameter& DataBaseParameter)
{
	//设置变量
	DataBaseParameter.wDataBasePort = 1433;
	DataBaseParameter.dwDataBaseAddr = 0L;
	DataBaseParameter.szDataBasePass[0] = 0;
	DataBaseParameter.szDataBaseUser[0] = 0;
	StringCchCopy(DataBaseParameter.szDataBaseName, CountArray(DataBaseParameter.szDataBaseName), szMessageDB);

	//读取信息
	bool bSuccess = ReadDataBaseInfo(szMessageDB, DataBaseParameter);

	return bSuccess;
}

//平台数据库
bool CServiceParameter::GetPlatformDBInfo(tagDataBaseParameter & DataBaseParameter)
{
	//设置变量
	DataBaseParameter.wDataBasePort=1433;
	DataBaseParameter.dwDataBaseAddr=0L;
	DataBaseParameter.szDataBasePass[0]=0;
	DataBaseParameter.szDataBaseUser[0]=0;
	StringCchCopy(DataBaseParameter.szDataBaseName,CountArray(DataBaseParameter.szDataBaseName), szPlatformDB);

	//读取信息
	bool bSuccess=ReadDataBaseInfo(szPlatformDB,DataBaseParameter);

	return bSuccess;
}

//帐户数据库
bool CServiceParameter::GetAccountsDBInfo(tagDataBaseParameter & DataBaseParameter)
{
	//设置变量
	DataBaseParameter.wDataBasePort=1433;
	DataBaseParameter.dwDataBaseAddr=0L;
	DataBaseParameter.szDataBasePass[0]=0;
	DataBaseParameter.szDataBaseUser[0]=0;
	StringCchCopy(DataBaseParameter.szDataBaseName,CountArray(DataBaseParameter.szDataBaseName), szAccountsDB);

	//读取信息
	bool bSuccess=ReadDataBaseInfo(szAccountsDB,DataBaseParameter);

	return bSuccess;
}

//金币数据库
bool CServiceParameter::GetGameGoldDBInfo(tagDataBaseParameter & DataBaseParameter)
{
	//设置变量
	DataBaseParameter.wDataBasePort=1433;
	DataBaseParameter.dwDataBaseAddr=0L;
	DataBaseParameter.szDataBasePass[0]=0;
	DataBaseParameter.szDataBaseUser[0]=0;
	StringCchCopy(DataBaseParameter.szDataBaseName,CountArray(DataBaseParameter.szDataBaseName), szGameGoldDB);

	//读取信息
	bool bSuccess=ReadDataBaseInfo(szGameGoldDB,DataBaseParameter);

	return bSuccess;
}


//比赛数据库
bool CServiceParameter::GetGameMatchDBInfo(tagDataBaseParameter& DataBaseParameter)
{
	//设置变量
	DataBaseParameter.wDataBasePort = 1433;
	DataBaseParameter.dwDataBaseAddr = 0L;
	DataBaseParameter.szDataBasePass[0] = 0;
	DataBaseParameter.szDataBaseUser[0] = 0;
	StringCchCopy(DataBaseParameter.szDataBaseName, CountArray(DataBaseParameter.szDataBaseName), szGameMatchDB);

	//读取信息
	bool bSuccess = ReadDataBaseInfo(szGameMatchDB, DataBaseParameter);

	return bSuccess;
}

//约战数据库
bool CServiceParameter::GetGameBattleDBInfo(tagDataBaseParameter& DataBaseParameter)
{
	//设置变量
	DataBaseParameter.wDataBasePort = 1433;
	DataBaseParameter.dwDataBaseAddr = 0L;
	DataBaseParameter.szDataBasePass[0] = 0;
	DataBaseParameter.szDataBaseUser[0] = 0;
	StringCchCopy(DataBaseParameter.szDataBaseName, CountArray(DataBaseParameter.szDataBaseName), szGameBattleDB);

	//读取信息
	bool bSuccess = ReadDataBaseInfo(szGameBattleDB, DataBaseParameter);

	return bSuccess;
}

//排位数据库
bool CServiceParameter::GetGameRankingDBInfo(tagDataBaseParameter& DataBaseParameter)
{
	//设置变量
	DataBaseParameter.wDataBasePort = 1433;
	DataBaseParameter.dwDataBaseAddr = 0L;
	DataBaseParameter.szDataBasePass[0] = 0;
	DataBaseParameter.szDataBaseUser[0] = 0;
	StringCchCopy(DataBaseParameter.szDataBaseName, CountArray(DataBaseParameter.szDataBaseName), szGameRankingDB);

	//读取信息
	bool bSuccess = ReadDataBaseInfo(szGameRankingDB, DataBaseParameter);

	return bSuccess;
}

//消息数据库
bool CServiceParameter::SetMessageDBInfo(const tagDataBaseParameter& DataBaseParameter)
{
	//写入信息
	bool bSuccess = WriteDataBaseInfo(szMessageDB, DataBaseParameter);

	return bSuccess;
}

//平台数据库
bool CServiceParameter::SetPlatformDBInfo(const tagDataBaseParameter & DataBaseParameter)
{
	//写入信息
	bool bSuccess=WriteDataBaseInfo(szPlatformDB,DataBaseParameter);

	return bSuccess;
}

//帐户数据库
bool CServiceParameter::SetAccountsDBInfo(const tagDataBaseParameter & DataBaseParameter)
{
	//写入信息
	bool bSuccess=WriteDataBaseInfo(szAccountsDB,DataBaseParameter);

	return bSuccess;
}

//金币数据库
bool CServiceParameter::SetGameGoldDBInfo(const tagDataBaseParameter & DataBaseParameter)
{
	//写入信息
	bool bSuccess=WriteDataBaseInfo(szGameGoldDB,DataBaseParameter);

	return bSuccess;
}

//比赛数据库
bool CServiceParameter::SetGameMatchDBInfo(const tagDataBaseParameter& DataBaseParameter)
{
	//写入信息
	bool bSuccess = WriteDataBaseInfo(szGameMatchDB, DataBaseParameter);

	return bSuccess;
}

//约战数据库
bool CServiceParameter::SetGameBattleDBInfo(const tagDataBaseParameter& DataBaseParameter)
{
	//写入信息
	bool bSuccess = WriteDataBaseInfo(szGameBattleDB, DataBaseParameter);

	return bSuccess;
}

//排位数据库
bool CServiceParameter::SetGameRankingDBInfo(const tagDataBaseParameter& DataBaseParameter)
{
	//写入信息
	bool bSuccess = WriteDataBaseInfo(szGameRankingDB, DataBaseParameter);

	return bSuccess;
}

//读取信息
bool CServiceParameter::ReadDataBaseInfo(LPCTSTR pszSubName, tagDataBaseParameter & DataBaseParameter)
{
	//读取信息
	TCHAR szDataBaseUser[33]=TEXT("");
	TCHAR szDataBasePass[33]=TEXT("");
	TCHAR szDataBaseName[33]=TEXT("");
	TCHAR szDataBaseAddr[33]=TEXT("");	

	ReadEncryptString(pszSubName,TEXT("DataBaseUser"),szDataBaseUser,CountArray(DataBaseParameter.szDataBaseUser));
	ReadEncryptString(pszSubName,TEXT("DataBasePass"),szDataBasePass,CountArray(DataBaseParameter.szDataBasePass));
	ReadEncryptString(pszSubName,TEXT("DataBaseName"),szDataBaseName,CountArray(DataBaseParameter.szDataBaseName));
	ReadEncryptString(pszSubName,TEXT("DataBaseAddr"),szDataBaseAddr,CountArray(szDataBaseAddr));

	//设置数据
 	StringCchCopy(DataBaseParameter.szDataBaseUser,CountArray(DataBaseParameter.szDataBaseUser), szDataBaseUser);
 	StringCchCopy(DataBaseParameter.szDataBasePass,CountArray(DataBaseParameter.szDataBasePass), szDataBasePass);
	if (szDataBaseName[0] != '\0')
	{
		StringCchCopy(DataBaseParameter.szDataBaseName, CountArray(DataBaseParameter.szDataBaseName), szDataBaseName);
	}		

	DataBaseParameter.wDataBasePort=ReadNumber(pszSubName,TEXT("DataBasePort"));
	DataBaseParameter.dwDataBaseAddr= _tcstoul(szDataBaseAddr,NULL,0);	

	return true;
}

//写入信息
bool CServiceParameter::WriteDataBaseInfo(LPCTSTR pszSubName, const tagDataBaseParameter & DataBaseParameter)
{
	//转换数据
	CString strDataBaseAddr;
	strDataBaseAddr.Format(TEXT("%u"),DataBaseParameter.dwDataBaseAddr);	

	//写入信息
	WriteNumber(pszSubName,TEXT("DataBasePort"),DataBaseParameter.wDataBasePort);
	WriteEncryptString(pszSubName,TEXT("DataBaseAddr"),strDataBaseAddr);
	WriteEncryptString(pszSubName,TEXT("DataBaseUser"),CW2CT(DataBaseParameter.szDataBaseUser));
	WriteEncryptString(pszSubName,TEXT("DataBasePass"),CW2CT(DataBaseParameter.szDataBasePass));
	WriteEncryptString(pszSubName,TEXT("DataBaseName"),CW2CT(DataBaseParameter.szDataBaseName));
  
	return true;
}

//写入配置
bool CServiceParameter::WriteEncryptString(LPCTSTR pszAppName,LPCTSTR pszName, LPCTSTR pszString)
{
	//获取路径
	TCHAR szPath[MAX_PATH]=TEXT("");
	TCHAR szFileName[MAX_PATH]=TEXT("");
	//GetCurrentDirectory(MAX_PATH, szPath);
	CWHService::GetWorkDirectory(szPath,CountArray(szPath));
	_sntprintf_s(szFileName,MAX_PATH, TEXT("%s\\%s"), szPath, PARA_FILE_NAME);

	//加密数据
	TCHAR szRegString[MAX_TARGET_LEN]=TEXT("");
	CXOREncrypt::EncryptData(pszString,szRegString,CountArray(szRegString));

	//写入数据
	WritePrivateProfileString(pszAppName,pszName, szRegString, szFileName);
	return true;
}

//写入配置
bool CServiceParameter::WriteNumber(LPCTSTR pszAppName, LPCTSTR pszName,int nNum)
{
	//获取路径
	TCHAR szPath[MAX_PATH]=TEXT("");
	TCHAR szFileName[MAX_PATH]=TEXT("");
	//GetCurrentDirectory(MAX_PATH, szPath);
	CWHService::GetWorkDirectory(szPath,CountArray(szPath));
	_sntprintf_s(szFileName,MAX_PATH, TEXT("%s\\%s"), szPath, PARA_FILE_NAME);

	TCHAR  szValBuf[16]; 
	_sntprintf_s(szValBuf,sizeof(szValBuf),TEXT("%i"), nNum);

	//写入数据
	WritePrivateProfileString(pszAppName,pszName, szValBuf, szFileName); 

	return true;
}

//读取配置
bool CServiceParameter::ReadEncryptString(LPCTSTR pszAppName, LPCTSTR pszKeyName, LPTSTR pszString, UINT uMaxCount)
{
	//获取路径
	TCHAR szPath[MAX_PATH]=TEXT("");
	TCHAR szFileName[MAX_PATH]=TEXT("");
	//GetCurrentDirectory(MAX_PATH, szPath);
	CWHService::GetWorkDirectory(szPath,CountArray(szPath));
	_sntprintf_s(szFileName,MAX_PATH, TEXT("%s\\%s"), szPath, PARA_FILE_NAME);

	//读取数据
	TCHAR szRegString[MAX_TARGET_LEN]=TEXT("");
	GetPrivateProfileString(pszAppName ,pszKeyName,TEXT( ""), szRegString, MAX_TARGET_LEN-1,szFileName);

	//解密数据
	CXOREncrypt::CrevasseData(szRegString,pszString,uMaxCount);
	
	return true;
}

//读取配置
UINT CServiceParameter::ReadNumber(LPCTSTR pszAppName, LPCTSTR pszKeyName)
{
	//获取路径
	TCHAR szPath[MAX_PATH]=TEXT("");
	TCHAR szFileName[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szPath,CountArray(szPath));
	_sntprintf_s(szFileName,MAX_PATH, TEXT("%s\\%s"), szPath, PARA_FILE_NAME);

	//读取数据
	return GetPrivateProfileInt(pszAppName ,pszKeyName, 0,szFileName);
}
//////////////////////////////////////////////////////////////////////////
