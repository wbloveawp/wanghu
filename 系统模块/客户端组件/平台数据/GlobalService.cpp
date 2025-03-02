#include "StdAfx.h"
#include "GlobalService.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalService::CGlobalService()
{
	m_strKeyName = TEXT("ServiceSvrs");
}

//��������
CGlobalService::~CGlobalService()
{
}

//������Ŀ
WORD CGlobalService::GetItemCount()
{
	//��ȡ��Ŀ
	WORD wItemCount=ReadFileValue(TEXT("ServerInfo"),TEXT("ItemCount"),0);

	return wItemCount;
}

//��·����
enNetworkKind CGlobalService::GetNetworkKind()
{
	//��ȡ����
	UINT nNetworkKind=ReadFileValue(TEXT("GlobalInfo"),TEXT("NetworkKind"),NetworkKind_Default);

	//����ת��
	switch (nNetworkKind)
	{
	case NetworkKind_DianXin: { return NetworkKind_DianXin; }
	case NetworkKind_WangTong: { return NetworkKind_WangTong; }
	}

	return NetworkKind_Default;
}

//��·����
VOID CGlobalService::SetNetworkKind(enNetworkKind NetworkKind)
{
	//��������
	enNetworkKind NetworkKindTemp=NetworkKind_Default;

	//����ת��
	switch (NetworkKind)
	{
	case NetworkKind_DianXin: { NetworkKindTemp=NetworkKind_DianXin; break; }
	case NetworkKind_WangTong: { NetworkKindTemp=NetworkKind_WangTong; break; }
	}

	//д������
	WriteFileValue(TEXT("GlobalInfo"),TEXT("NetworkKind"),NetworkKindTemp);

	return;
}

//��������
VOID CGlobalService::GetServerItemName(WORD wIndex, TCHAR szResult[LEN_SERVER])
{
	//��������
	TCHAR szItemName[LEN_SERVER]=TEXT("");
	_sntprintf_s(szItemName,CountArray(szItemName),TEXT("ItemDescribe%d"),wIndex+1);

	//��ȡ����
	ReadFileString(TEXT("ServerInfo"),szItemName,szResult,LEN_SERVER);

	return;
}

//�ϴε�¼
VOID CGlobalService::SetLastServerName(LPCTSTR pszLastServer)
{
	//д������
	WriteFileString(TEXT("GlobalInfo"),TEXT("LastServerName"),pszLastServer);

	return;
}

//�ϴε�¼
VOID CGlobalService::GetLastServerName(TCHAR szResult[LEN_SERVER])
{
	//��ȡ����
	ReadFileString(TEXT("GlobalInfo"),TEXT("LastServerName"),szResult,LEN_SERVER);

	return;
}

//������Ϣ
bool CGlobalService::GetServerItemInfo(LPCTSTR pszServerName, enNetworkKind NetworkKind, tagServerItem & ServerItem)
{
	//Ч�����
	ASSERT(pszServerName!=NULL);
	if (pszServerName==NULL) return false;

	//��������
	TCHAR szServerName[LEN_MD5];
	CMD5Encrypt::EncryptData(pszServerName,szServerName);

	//��ȡ��Ϣ
	switch (NetworkKind)
	{
	case NetworkKind_DianXin:		//��������
		{
			ServerItem.wServerPort=ReadFileValue(szServerName,TEXT("PortDianXin"),PORT_LOGON);
			ServerItem.dwServerAddr=ReadFileValue(szServerName,TEXT("AddrDianXin"),INADDR_NONE);
			ReadFileString(szServerName,TEXT("HostDianXin"),ServerItem.szServerHost,CountArray(ServerItem.szServerHost));
			break;
		}
	case NetworkKind_WangTong:		//��ͨ����
		{
			ServerItem.wServerPort=ReadFileValue(szServerName,TEXT("PortWangTong"),PORT_LOGON);
			ServerItem.dwServerAddr=ReadFileValue(szServerName,TEXT("AddrWangTong"),INADDR_NONE);
			ReadFileString(szServerName,TEXT("HostWangTong"),ServerItem.szServerHost,CountArray(ServerItem.szServerHost));
			break;
		}
	}

	//��ȡĬ��
	if ((NetworkKind==NetworkKind_Default)||((ServerItem.dwServerAddr==INADDR_NONE)&&(ServerItem.szServerHost[0]==0)))
	{
		ServerItem.wServerPort=ReadFileValue(szServerName,TEXT("PortDefault"),PORT_LOGON);
		ServerItem.dwServerAddr=ReadFileValue(szServerName,TEXT("AddrDefault"),INADDR_NONE);
		ReadFileString(szServerName,TEXT("HostDefault"),ServerItem.szServerHost,CountArray(ServerItem.szServerHost));
	}

	//û�����������ȡĬ������
	if( ServerItem.szServerHost[0] == 0 )
	{
		ReadFileString(szServerName,TEXT("HostDefault"),ServerItem.szServerHost,CountArray(ServerItem.szServerHost));
	}

	//��ַװ��
	if (ServerItem.dwServerAddr==0L) ServerItem.dwServerAddr=INADDR_NONE;

	return ((ServerItem.dwServerAddr!=INADDR_NONE)||(ServerItem.szServerHost[0]!=0));
}

//д������
VOID CGlobalService::WriteServerItemInfo(LPCTSTR pszServerName, enNetworkKind NetworkKind, tagServerItem & ServerItem)
{
	//Ч�����
	ASSERT(pszServerName!=NULL);
	if (pszServerName==NULL) return;

	//��������
	TCHAR szServerName[LEN_MD5];
	CMD5Encrypt::EncryptData(pszServerName,szServerName);

	//��ȡ��Ϣ
	switch (NetworkKind)
	{
	case NetworkKind_DianXin:		//��������
		{
			WriteFileValue(szServerName,TEXT("AddrDianXin"),ServerItem.dwServerAddr);
			break;
		}
	case NetworkKind_WangTong:		//��ͨ����
		{
			WriteFileValue(szServerName,TEXT("AddrWangTong"),ServerItem.dwServerAddr);
			break;
		}
	case NetworkKind_Default:		//Ĭ������
		{
			WriteFileValue(szServerName,TEXT("AddrDefault"),ServerItem.dwServerAddr);
			break;
		}
	}

	return;
}

//д����ֵ
VOID CGlobalService::WriteFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nValue)
{
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//����·��
	TCHAR szServerInfoPath[MAX_PATH]=TEXT("");
	_sntprintf_s(szServerInfoPath,CountArray(szServerInfoPath),TEXT("%s\\%s.INI"),szDirectory,m_strKeyName.GetString());

	//��������
	TCHAR szString[16]=TEXT("");
	_sntprintf_s(szString,CountArray(szString),TEXT("%u"),nValue);

	//д������
	WritePrivateProfileString(pszKeyName,pszItemName,szString,szServerInfoPath);

	return;
}

//д������
VOID CGlobalService::WriteFileString(LPCTSTR pszKeyName, LPCTSTR pszItemName, LPCTSTR pszString)
{
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//����·��
	TCHAR szServerInfoPath[MAX_PATH]=TEXT("");
	_sntprintf_s(szServerInfoPath,CountArray(szServerInfoPath),TEXT("%s\\%s.INI"),szDirectory,m_strKeyName.GetString());

	//��������
	TCHAR szResultEncrypt[MAX_TARGET_LEN];
	ZeroMemory(szResultEncrypt,sizeof(szResultEncrypt));

	//д������
	CXOREncrypt::EncryptData(pszString,szResultEncrypt,CountArray(szResultEncrypt));
	WritePrivateProfileString(pszKeyName,pszItemName,szResultEncrypt,szServerInfoPath);

	return;
}

//��ȡ��ֵ
UINT CGlobalService::ReadFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nDefault)
{
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//����·��
	TCHAR szServerInfoPath[MAX_PATH]=TEXT("");
	_sntprintf_s(szServerInfoPath,CountArray(szServerInfoPath),TEXT("%s\\%s.INI"),szDirectory,m_strKeyName.GetString());

	//��ȡ����
	UINT nValue=GetPrivateProfileInt(pszKeyName,pszItemName,nDefault,szServerInfoPath);

	return nValue;
}

//��ȡ����
VOID CGlobalService::ReadFileString(LPCTSTR pszKeyName, LPCTSTR pszItemName, TCHAR szResult[], WORD wMaxCount)
{
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//����·��
	TCHAR szServerInfoPath[MAX_PATH]=TEXT("");
	_sntprintf_s(szServerInfoPath,CountArray(szServerInfoPath),TEXT("%s\\%s.INI"),szDirectory,m_strKeyName.GetString());

	//��ȡ����
	TCHAR szResultEncrypt[MAX_TARGET_LEN];
	GetPrivateProfileString(pszKeyName,pszItemName,TEXT(""),szResultEncrypt,CountArray(szResultEncrypt),szServerInfoPath);

	//��������
	szResult[0]=0;
	CXOREncrypt::CrevasseData(szResultEncrypt,szResult,wMaxCount);

	return;
}

//////////////////////////////////////////////////////////////////////////
