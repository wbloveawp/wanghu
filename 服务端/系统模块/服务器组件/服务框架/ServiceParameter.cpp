#include "StdAfx.h"
#include "ServiceParameter.h"

//////////////////////////////////////////////////////////////////////////
//�궨��

#define REG_ITEM_NAME				TEXT("Information")					//ע�����
#define PARA_FILE_NAME				TEXT("ServerInfo.ini")				//�����ļ�

//////////////////////////////////////////////////////////////////////////

//���캯��
CServiceParameter::CServiceParameter()
{	
}

//��������
CServiceParameter::~CServiceParameter()
{
}

//��ȡ�˿�
WORD CServiceParameter::GetCenterPort()
{
	//��ȡ����
	WORD wCenterProt= ReadNumber(REG_ITEM_NAME, TEXT("CenterPort"));

	return wCenterProt;
}

//���ö˿�
bool CServiceParameter::SetCenterPort(WORD wCenterPort)
{
	//д����Ϣ
	WriteNumber(REG_ITEM_NAME, TEXT("CenterPort"), wCenterPort);

	return true;
}

//��������
bool CServiceParameter::GetServerName(TCHAR szServerName[LEN_SERVER])
{
	//д����Ϣ
	ReadEncryptString(REG_ITEM_NAME,TEXT("ServerName"),szServerName,LEN_SERVER);

	return true;
}

//��������
bool CServiceParameter::SetServerName(TCHAR szServerName[LEN_SERVER])
{
	//д����Ϣ
	WriteEncryptString(REG_ITEM_NAME,TEXT("ServerName"), szServerName);

	return true;
}

//�����ַ
bool CServiceParameter::GetServiceAddress(tagAddressInfo & AddressInfo)
{
	//���ñ���
	AddressInfo.szServiceDomain[0]=0;
	AddressInfo.dwServiceAddr=INADDR_NONE;	

	//��ȡ��Ϣ
	TCHAR szServiceAddr[32]=TEXT("\0");
	ReadEncryptString(REG_ITEM_NAME,TEXT("ServiceAddr"),szServiceAddr,32);

	//��ȡ����
	TCHAR szServiceDomain[LEN_DOMAIN]=TEXT("");
	ReadEncryptString(REG_ITEM_NAME,TEXT("ServiceDomain"),szServiceDomain,LEN_DOMAIN);

	//ת������
#ifndef _UNICODE
	AddressInfo.dwServiceAddr=atol(szServiceAddr);	
#else
	AddressInfo.dwServiceAddr=_tcstoul(szServiceAddr,NULL,0);
#endif

	//��������
	StringCchCopy(AddressInfo.szServiceDomain,CountArray(AddressInfo.szServiceDomain), szServiceDomain);

	return true;
}

//�����ַ
bool CServiceParameter::SetServiceAddress(const tagAddressInfo & AddressInfo)
{
	//ת������
	CString strServiceAddr;
	strServiceAddr.Format(_T("%u"), AddressInfo.dwServiceAddr);

	//д����Ϣ
	WriteEncryptString(REG_ITEM_NAME,TEXT("ServiceAddr"),strServiceAddr);

	//д������
	WriteEncryptString(REG_ITEM_NAME,TEXT("ServiceDomain"),CW2CT(AddressInfo.szServiceDomain));

	return true;
}

//���ĵ�ַ
bool CServiceParameter::GetCenterAddress(tagAddressInfo & AddressInfo)
{
	//���ñ���
	AddressInfo.dwServiceAddr=INADDR_NONE;
	
	//�������
	TCHAR strServiceAddr[32]=TEXT("\0");

	//��ȡ��Ϣ
	ReadEncryptString(REG_ITEM_NAME,TEXT("CenterServerAddr"),strServiceAddr,32);

	//ת������
	AddressInfo.dwServiceAddr= _tcstoul(strServiceAddr,NULL,0);

	return true;
}

//���ĵ�ַ
bool CServiceParameter::SetCenterAddress(const tagAddressInfo & AddressInfo)
{
	//�������
	CString strServiceAddr;
	strServiceAddr.Format(TEXT("%u"), AddressInfo.dwServiceAddr);

	//д����Ϣ
	WriteEncryptString(REG_ITEM_NAME,TEXT("CenterServerAddr"),strServiceAddr);

	return true;
}

//��Ϣ���ݿ�
bool CServiceParameter::GetMessageDBInfo(tagDataBaseParameter& DataBaseParameter)
{
	//���ñ���
	DataBaseParameter.wDataBasePort = 1433;
	DataBaseParameter.dwDataBaseAddr = 0L;
	DataBaseParameter.szDataBasePass[0] = 0;
	DataBaseParameter.szDataBaseUser[0] = 0;
	StringCchCopy(DataBaseParameter.szDataBaseName, CountArray(DataBaseParameter.szDataBaseName), szMessageDB);

	//��ȡ��Ϣ
	bool bSuccess = ReadDataBaseInfo(szMessageDB, DataBaseParameter);

	return bSuccess;
}

//ƽ̨���ݿ�
bool CServiceParameter::GetPlatformDBInfo(tagDataBaseParameter & DataBaseParameter)
{
	//���ñ���
	DataBaseParameter.wDataBasePort=1433;
	DataBaseParameter.dwDataBaseAddr=0L;
	DataBaseParameter.szDataBasePass[0]=0;
	DataBaseParameter.szDataBaseUser[0]=0;
	StringCchCopy(DataBaseParameter.szDataBaseName,CountArray(DataBaseParameter.szDataBaseName), szPlatformDB);

	//��ȡ��Ϣ
	bool bSuccess=ReadDataBaseInfo(szPlatformDB,DataBaseParameter);

	return bSuccess;
}

//�ʻ����ݿ�
bool CServiceParameter::GetAccountsDBInfo(tagDataBaseParameter & DataBaseParameter)
{
	//���ñ���
	DataBaseParameter.wDataBasePort=1433;
	DataBaseParameter.dwDataBaseAddr=0L;
	DataBaseParameter.szDataBasePass[0]=0;
	DataBaseParameter.szDataBaseUser[0]=0;
	StringCchCopy(DataBaseParameter.szDataBaseName,CountArray(DataBaseParameter.szDataBaseName), szAccountsDB);

	//��ȡ��Ϣ
	bool bSuccess=ReadDataBaseInfo(szAccountsDB,DataBaseParameter);

	return bSuccess;
}

//������ݿ�
bool CServiceParameter::GetGameGoldDBInfo(tagDataBaseParameter & DataBaseParameter)
{
	//���ñ���
	DataBaseParameter.wDataBasePort=1433;
	DataBaseParameter.dwDataBaseAddr=0L;
	DataBaseParameter.szDataBasePass[0]=0;
	DataBaseParameter.szDataBaseUser[0]=0;
	StringCchCopy(DataBaseParameter.szDataBaseName,CountArray(DataBaseParameter.szDataBaseName), szGameGoldDB);

	//��ȡ��Ϣ
	bool bSuccess=ReadDataBaseInfo(szGameGoldDB,DataBaseParameter);

	return bSuccess;
}


//�������ݿ�
bool CServiceParameter::GetGameMatchDBInfo(tagDataBaseParameter& DataBaseParameter)
{
	//���ñ���
	DataBaseParameter.wDataBasePort = 1433;
	DataBaseParameter.dwDataBaseAddr = 0L;
	DataBaseParameter.szDataBasePass[0] = 0;
	DataBaseParameter.szDataBaseUser[0] = 0;
	StringCchCopy(DataBaseParameter.szDataBaseName, CountArray(DataBaseParameter.szDataBaseName), szGameMatchDB);

	//��ȡ��Ϣ
	bool bSuccess = ReadDataBaseInfo(szGameMatchDB, DataBaseParameter);

	return bSuccess;
}

//Լս���ݿ�
bool CServiceParameter::GetGameBattleDBInfo(tagDataBaseParameter& DataBaseParameter)
{
	//���ñ���
	DataBaseParameter.wDataBasePort = 1433;
	DataBaseParameter.dwDataBaseAddr = 0L;
	DataBaseParameter.szDataBasePass[0] = 0;
	DataBaseParameter.szDataBaseUser[0] = 0;
	StringCchCopy(DataBaseParameter.szDataBaseName, CountArray(DataBaseParameter.szDataBaseName), szGameBattleDB);

	//��ȡ��Ϣ
	bool bSuccess = ReadDataBaseInfo(szGameBattleDB, DataBaseParameter);

	return bSuccess;
}

//��λ���ݿ�
bool CServiceParameter::GetGameRankingDBInfo(tagDataBaseParameter& DataBaseParameter)
{
	//���ñ���
	DataBaseParameter.wDataBasePort = 1433;
	DataBaseParameter.dwDataBaseAddr = 0L;
	DataBaseParameter.szDataBasePass[0] = 0;
	DataBaseParameter.szDataBaseUser[0] = 0;
	StringCchCopy(DataBaseParameter.szDataBaseName, CountArray(DataBaseParameter.szDataBaseName), szGameRankingDB);

	//��ȡ��Ϣ
	bool bSuccess = ReadDataBaseInfo(szGameRankingDB, DataBaseParameter);

	return bSuccess;
}

//��Ϣ���ݿ�
bool CServiceParameter::SetMessageDBInfo(const tagDataBaseParameter& DataBaseParameter)
{
	//д����Ϣ
	bool bSuccess = WriteDataBaseInfo(szMessageDB, DataBaseParameter);

	return bSuccess;
}

//ƽ̨���ݿ�
bool CServiceParameter::SetPlatformDBInfo(const tagDataBaseParameter & DataBaseParameter)
{
	//д����Ϣ
	bool bSuccess=WriteDataBaseInfo(szPlatformDB,DataBaseParameter);

	return bSuccess;
}

//�ʻ����ݿ�
bool CServiceParameter::SetAccountsDBInfo(const tagDataBaseParameter & DataBaseParameter)
{
	//д����Ϣ
	bool bSuccess=WriteDataBaseInfo(szAccountsDB,DataBaseParameter);

	return bSuccess;
}

//������ݿ�
bool CServiceParameter::SetGameGoldDBInfo(const tagDataBaseParameter & DataBaseParameter)
{
	//д����Ϣ
	bool bSuccess=WriteDataBaseInfo(szGameGoldDB,DataBaseParameter);

	return bSuccess;
}

//�������ݿ�
bool CServiceParameter::SetGameMatchDBInfo(const tagDataBaseParameter& DataBaseParameter)
{
	//д����Ϣ
	bool bSuccess = WriteDataBaseInfo(szGameMatchDB, DataBaseParameter);

	return bSuccess;
}

//Լս���ݿ�
bool CServiceParameter::SetGameBattleDBInfo(const tagDataBaseParameter& DataBaseParameter)
{
	//д����Ϣ
	bool bSuccess = WriteDataBaseInfo(szGameBattleDB, DataBaseParameter);

	return bSuccess;
}

//��λ���ݿ�
bool CServiceParameter::SetGameRankingDBInfo(const tagDataBaseParameter& DataBaseParameter)
{
	//д����Ϣ
	bool bSuccess = WriteDataBaseInfo(szGameRankingDB, DataBaseParameter);

	return bSuccess;
}

//��ȡ��Ϣ
bool CServiceParameter::ReadDataBaseInfo(LPCTSTR pszSubName, tagDataBaseParameter & DataBaseParameter)
{
	//��ȡ��Ϣ
	TCHAR szDataBaseUser[33]=TEXT("");
	TCHAR szDataBasePass[33]=TEXT("");
	TCHAR szDataBaseName[33]=TEXT("");
	TCHAR szDataBaseAddr[33]=TEXT("");	

	ReadEncryptString(pszSubName,TEXT("DataBaseUser"),szDataBaseUser,CountArray(DataBaseParameter.szDataBaseUser));
	ReadEncryptString(pszSubName,TEXT("DataBasePass"),szDataBasePass,CountArray(DataBaseParameter.szDataBasePass));
	ReadEncryptString(pszSubName,TEXT("DataBaseName"),szDataBaseName,CountArray(DataBaseParameter.szDataBaseName));
	ReadEncryptString(pszSubName,TEXT("DataBaseAddr"),szDataBaseAddr,CountArray(szDataBaseAddr));

	//��������
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

//д����Ϣ
bool CServiceParameter::WriteDataBaseInfo(LPCTSTR pszSubName, const tagDataBaseParameter & DataBaseParameter)
{
	//ת������
	CString strDataBaseAddr;
	strDataBaseAddr.Format(TEXT("%u"),DataBaseParameter.dwDataBaseAddr);	

	//д����Ϣ
	WriteNumber(pszSubName,TEXT("DataBasePort"),DataBaseParameter.wDataBasePort);
	WriteEncryptString(pszSubName,TEXT("DataBaseAddr"),strDataBaseAddr);
	WriteEncryptString(pszSubName,TEXT("DataBaseUser"),CW2CT(DataBaseParameter.szDataBaseUser));
	WriteEncryptString(pszSubName,TEXT("DataBasePass"),CW2CT(DataBaseParameter.szDataBasePass));
	WriteEncryptString(pszSubName,TEXT("DataBaseName"),CW2CT(DataBaseParameter.szDataBaseName));
  
	return true;
}

//д������
bool CServiceParameter::WriteEncryptString(LPCTSTR pszAppName,LPCTSTR pszName, LPCTSTR pszString)
{
	//��ȡ·��
	TCHAR szPath[MAX_PATH]=TEXT("");
	TCHAR szFileName[MAX_PATH]=TEXT("");
	//GetCurrentDirectory(MAX_PATH, szPath);
	CWHService::GetWorkDirectory(szPath,CountArray(szPath));
	_sntprintf_s(szFileName,MAX_PATH, TEXT("%s\\%s"), szPath, PARA_FILE_NAME);

	//��������
	TCHAR szRegString[MAX_TARGET_LEN]=TEXT("");
	CXOREncrypt::EncryptData(pszString,szRegString,CountArray(szRegString));

	//д������
	WritePrivateProfileString(pszAppName,pszName, szRegString, szFileName);
	return true;
}

//д������
bool CServiceParameter::WriteNumber(LPCTSTR pszAppName, LPCTSTR pszName,int nNum)
{
	//��ȡ·��
	TCHAR szPath[MAX_PATH]=TEXT("");
	TCHAR szFileName[MAX_PATH]=TEXT("");
	//GetCurrentDirectory(MAX_PATH, szPath);
	CWHService::GetWorkDirectory(szPath,CountArray(szPath));
	_sntprintf_s(szFileName,MAX_PATH, TEXT("%s\\%s"), szPath, PARA_FILE_NAME);

	TCHAR  szValBuf[16]; 
	_sntprintf_s(szValBuf,sizeof(szValBuf),TEXT("%i"), nNum);

	//д������
	WritePrivateProfileString(pszAppName,pszName, szValBuf, szFileName); 

	return true;
}

//��ȡ����
bool CServiceParameter::ReadEncryptString(LPCTSTR pszAppName, LPCTSTR pszKeyName, LPTSTR pszString, UINT uMaxCount)
{
	//��ȡ·��
	TCHAR szPath[MAX_PATH]=TEXT("");
	TCHAR szFileName[MAX_PATH]=TEXT("");
	//GetCurrentDirectory(MAX_PATH, szPath);
	CWHService::GetWorkDirectory(szPath,CountArray(szPath));
	_sntprintf_s(szFileName,MAX_PATH, TEXT("%s\\%s"), szPath, PARA_FILE_NAME);

	//��ȡ����
	TCHAR szRegString[MAX_TARGET_LEN]=TEXT("");
	GetPrivateProfileString(pszAppName ,pszKeyName,TEXT( ""), szRegString, MAX_TARGET_LEN-1,szFileName);

	//��������
	CXOREncrypt::CrevasseData(szRegString,pszString,uMaxCount);
	
	return true;
}

//��ȡ����
UINT CServiceParameter::ReadNumber(LPCTSTR pszAppName, LPCTSTR pszKeyName)
{
	//��ȡ·��
	TCHAR szPath[MAX_PATH]=TEXT("");
	TCHAR szFileName[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szPath,CountArray(szPath));
	_sntprintf_s(szFileName,MAX_PATH, TEXT("%s\\%s"), szPath, PARA_FILE_NAME);

	//��ȡ����
	return GetPrivateProfileInt(pszAppName ,pszKeyName, 0,szFileName);
}
//////////////////////////////////////////////////////////////////////////
