#ifndef GLOBAL_SERVICE_HEAD_FILE
#define GLOBAL_SERVICE_HEAD_FILE

#pragma once

#include "PlatformDataHead.h"
#include "GlobalServer.h"

//////////////////////////////////////////////////////////////////////////

//全局服务器
class PLATFORM_DATA_CLASS CGlobalService
{
	//变量定义
protected:
	CString								m_strKeyName;					//配置文件

	//函数定义
public:
	//构造函数
	CGlobalService();
	//析构函数
	virtual ~CGlobalService();

	//全局信息
public:
	//子项数目
	WORD GetItemCount();

	//线路类型
public:
	//网络类型
	enNetworkKind GetNetworkKind();
	//网络类型
	VOID SetNetworkKind(enNetworkKind NetworkKind);

	//上次登录
public:
	//上次登录
	VOID SetLastServerName(LPCTSTR pszLastServer);
	//上次登录
	VOID GetLastServerName(TCHAR szLastServer[LEN_SERVER]);

	//子项信息
public:
	//子项名字
	VOID GetServerItemName(WORD wIndex, TCHAR szResult[LEN_SERVER]);
	//子项信息
	bool GetServerItemInfo(LPCTSTR pszServerName, enNetworkKind NetworkKind, tagServerItem & ServerItem);

	//写入函数
public:
	//写入子项
	VOID WriteServerItemInfo(LPCTSTR pszServerName, enNetworkKind NetworkKind, tagServerItem & ServerItem);

	//写入函数
protected:
	//读取数值
	VOID WriteFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nValue);
	//写入数据
	VOID WriteFileString(LPCTSTR pszKeyName, LPCTSTR pszItemName, LPCTSTR pszString);

	//读取函数
protected:
	//读取数值
	UINT ReadFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nDefault);
	//读取数据
	VOID ReadFileString(LPCTSTR pszKeyName, LPCTSTR pszItemName, TCHAR szResult[], WORD wMaxCount);
};

//////////////////////////////////////////////////////////////////////////

#endif