#include "StdAfx.h"
#include "GlobalServer.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalServer::CGlobalServer()
{
}

//析构函数
CGlobalServer::~CGlobalServer()
{
}

//子项数目
WORD CGlobalServer::GetItemCount()
{
	//读取数目
	WORD wItemCount=ReadFileValue(TEXT("ServerInfo"),TEXT("ItemCount"),0);

	return wItemCount;
}

//线路类型
enNetworkKind CGlobalServer::GetNetworkKind()
{
	//读取类型
	UINT nNetworkKind=ReadFileValue(TEXT("GlobalInfo"),TEXT("NetworkKind"),NetworkKind_Default);

	//类型转换
	switch (nNetworkKind)
	{
	case NetworkKind_DianXin: { return NetworkKind_DianXin; }
	case NetworkKind_WangTong: { return NetworkKind_WangTong; }
	}

	return NetworkKind_Default;
}

//线路类型
VOID CGlobalServer::SetNetworkKind(enNetworkKind NetworkKind)
{
	//变量定义
	enNetworkKind NetworkKindTemp=NetworkKind_Default;

	//类型转换
	switch (NetworkKind)
	{
	case NetworkKind_DianXin: { NetworkKindTemp=NetworkKind_DianXin; break; }
	case NetworkKind_WangTong: { NetworkKindTemp=NetworkKind_WangTong; break; }
	}

	//写入类型
	WriteFileValue(TEXT("GlobalInfo"),TEXT("NetworkKind"),NetworkKindTemp);

	return;
}

//子项名字
VOID CGlobalServer::GetServerItemName(WORD wIndex, TCHAR szResult[LEN_SERVER])
{
	//构造名字
	TCHAR szItemName[LEN_SERVER]=TEXT("");
	_sntprintf_s(szItemName,CountArray(szItemName),TEXT("ItemDescribe%d"),wIndex+1);

	//读取名字
	ReadFileString(TEXT("ServerInfo"),szItemName,szResult,LEN_SERVER);

	return;
}

//上次登录
VOID CGlobalServer::SetLastServerName(LPCTSTR pszLastServer)
{
	//写入名字
	//WriteFileString(TEXT("GlobalInfo"),TEXT("LastServerName"),pszLastServer);

	//变量定义
	CRegKeyAide RegOptionItem;

	//界面信息
	if (RegOptionItem.OpenRegKey(REG_GOBAL_OPTION,false)==true)
	{
		RegOptionItem.WriteString(TEXT("LastServer"),pszLastServer);
	}

	return;
}

//上次登录
VOID CGlobalServer::GetLastServerName(TCHAR szResult[LEN_SERVER])
{
	//读取名字
	//ReadFileString(TEXT("GlobalInfo"),TEXT("LastServerName"),szResult,LEN_SERVER);

	//变量定义
	CRegKeyAide RegOptionItem;

	//界面信息
	if (RegOptionItem.OpenRegKey(REG_GOBAL_OPTION,false)==true)
	{
		RegOptionItem.GetString(TEXT("LastServer"),szResult,LEN_SERVER);
	}

	return;
}

//子项信息
bool CGlobalServer::GetServerItemInfo(LPCTSTR pszServerName, enNetworkKind NetworkKind, tagServerItem & ServerItem)
{
	//效验参数
	ASSERT(pszServerName!=NULL);
	if (pszServerName==NULL) return false;

	//加密数据
	TCHAR szServerName[LEN_MD5];
	CMD5Encrypt::EncryptData(pszServerName,szServerName);

	//获取信息
	switch (NetworkKind)
	{
	case NetworkKind_DianXin:		//电信网络
		{
			ServerItem.wServerPort=ReadFileValue(szServerName,TEXT("PortDianXin"),PORT_LOGON);
			ServerItem.dwServerAddr=ReadFileValue(szServerName,TEXT("AddrDianXin"),INADDR_NONE);
			ReadFileString(szServerName,TEXT("HostDianXin"),ServerItem.szServerHost,CountArray(ServerItem.szServerHost));
			break;
		}
	case NetworkKind_WangTong:		//网通网络
		{
			ServerItem.wServerPort=ReadFileValue(szServerName,TEXT("PortWangTong"),PORT_LOGON);
			ServerItem.dwServerAddr=ReadFileValue(szServerName,TEXT("AddrWangTong"),INADDR_NONE);
			ReadFileString(szServerName,TEXT("HostWangTong"),ServerItem.szServerHost,CountArray(ServerItem.szServerHost));
			break;
		}
	}

	//读取默认
	if ((NetworkKind==NetworkKind_Default)||((ServerItem.dwServerAddr==INADDR_NONE)&&(ServerItem.szServerHost[0]==0)))
	{
		ServerItem.wServerPort=ReadFileValue(szServerName,TEXT("PortDefault"),PORT_LOGON);
		ServerItem.dwServerAddr=ReadFileValue(szServerName,TEXT("AddrDefault"),INADDR_NONE);
		ReadFileString(szServerName,TEXT("HostDefault"),ServerItem.szServerHost,CountArray(ServerItem.szServerHost));
	}

	//没有域名，则读取默认域名
	if( ServerItem.szServerHost[0] == 0 )
	{
		ReadFileString(szServerName,TEXT("HostDefault"),ServerItem.szServerHost,CountArray(ServerItem.szServerHost));
	}

	//地址装换
	if (ServerItem.dwServerAddr==0L) ServerItem.dwServerAddr=INADDR_NONE;

	return ((ServerItem.dwServerAddr!=INADDR_NONE)||(ServerItem.szServerHost[0]!=0));
}

//写入子项
VOID CGlobalServer::WriteServerItemInfo(LPCTSTR pszServerName, enNetworkKind NetworkKind, tagServerItem & ServerItem)
{
	//效验参数
	ASSERT(pszServerName!=NULL);
	if (pszServerName==NULL) return;

	//加密数据
	TCHAR szServerName[LEN_MD5];
	CMD5Encrypt::EncryptData(pszServerName,szServerName);

	//获取信息
	switch (NetworkKind)
	{
	case NetworkKind_DianXin:		//电信网络
		{
			WriteFileValue(szServerName,TEXT("AddrDianXin"),ServerItem.dwServerAddr);
			break;
		}
	case NetworkKind_WangTong:		//网通网络
		{
			WriteFileValue(szServerName,TEXT("AddrWangTong"),ServerItem.dwServerAddr);
			break;
		}
	case NetworkKind_Default:		//默认网络
		{
			WriteFileValue(szServerName,TEXT("AddrDefault"),ServerItem.dwServerAddr);
			break;
		}
	}

	return;
}

//写入数值
VOID CGlobalServer::WriteFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nValue)
{
	//获取目录
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//构造路径
	TCHAR szServerInfoPath[MAX_PATH]=TEXT("");
	_sntprintf_s(szServerInfoPath,CountArray(szServerInfoPath),TEXT("%s\\ServerInfo.INI"),szDirectory);

	//构造数据
	TCHAR szString[16]=TEXT("");
	_sntprintf_s(szString,CountArray(szString),TEXT("%u"),nValue);

	//写入数据
	WritePrivateProfileString(pszKeyName,pszItemName,szString,szServerInfoPath);

	return;
}

//写入数据
VOID CGlobalServer::WriteFileString(LPCTSTR pszKeyName, LPCTSTR pszItemName, LPCTSTR pszString)
{
	//获取目录
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//构造路径
	TCHAR szServerInfoPath[MAX_PATH]=TEXT("");
	_sntprintf_s(szServerInfoPath,CountArray(szServerInfoPath),TEXT("%s\\ServerInfo.INI"),szDirectory);

	//加密数据
	TCHAR szResultEncrypt[MAX_TARGET_LEN];
	ZeroMemory(szResultEncrypt,sizeof(szResultEncrypt));

	//写入数据
	CXOREncrypt::EncryptData(pszString,szResultEncrypt,CountArray(szResultEncrypt));
	WritePrivateProfileString(pszKeyName,pszItemName,szResultEncrypt,szServerInfoPath);

	return;
}

//读取数值
UINT CGlobalServer::ReadFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nDefault)
{
	//获取目录
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//构造路径
	TCHAR szServerInfoPath[MAX_PATH]=TEXT("");
	_sntprintf_s(szServerInfoPath,CountArray(szServerInfoPath),TEXT("%s\\ServerInfo.INI"),szDirectory);

	//读取数据
	UINT nValue=GetPrivateProfileInt(pszKeyName,pszItemName,nDefault,szServerInfoPath);

	return nValue;
}

//读取数据
VOID CGlobalServer::ReadFileString(LPCTSTR pszKeyName, LPCTSTR pszItemName, TCHAR szResult[], WORD wMaxCount)
{
	//获取目录
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//构造路径
	TCHAR szServerInfoPath[MAX_PATH]=TEXT("");
	_sntprintf_s(szServerInfoPath,CountArray(szServerInfoPath),TEXT("%s\\ServerInfo.INI"),szDirectory);

	//读取数据
	TCHAR szResultEncrypt[MAX_TARGET_LEN];
	GetPrivateProfileString(pszKeyName,pszItemName,TEXT(""),szResultEncrypt,CountArray(szResultEncrypt),szServerInfoPath);

	//解密数据
	szResult[0]=0;
	CXOREncrypt::CrevasseData(szResultEncrypt,szResult,wMaxCount);

	return;
}

//////////////////////////////////////////////////////////////////////////
