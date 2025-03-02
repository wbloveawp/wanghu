#ifndef SERVICE_PARAMETER_HEAD_FILE
#define SERVICE_PARAMETER_HEAD_FILE

#pragma once

#include "ServiceFrameHead.h"

//////////////////////////////////////////////////////////////////////////

//服务参数
class SERVICE_FRAME_CLASS CServiceParameter
{
	//函数定义
public:
	//构造函数
	CServiceParameter();
	//析构函数
	virtual ~CServiceParameter();

	//中心端口
public:
	//获取端口
	WORD GetCenterPort();
	//设置端口
	bool SetCenterPort(WORD wCenterPort);

	//服务器名
public:
	//服务器名
	bool GetServerName(TCHAR szServerName[LEN_SERVER]);
	//服务器名
	bool SetServerName(TCHAR szServerName[LEN_SERVER]);

	//服务地址
public:
	//服务地址
	bool GetServiceAddress(tagAddressInfo & AddressInfo);
	//服务地址
	bool SetServiceAddress(const tagAddressInfo & AddressInfo);

	//中心地址
public:
	//中心地址
	bool GetCenterAddress(tagAddressInfo & AddressInfo);
	//中心地址
	bool SetCenterAddress(const tagAddressInfo & AddressInfo);

	//查询函数
public:
	//消息数据库
	bool GetMessageDBInfo(tagDataBaseParameter& DataBaseParameter);
	//平台数据库
	bool GetPlatformDBInfo(tagDataBaseParameter & DataBaseParameter);
	//帐户数据库
	bool GetAccountsDBInfo(tagDataBaseParameter & DataBaseParameter);
	//金币数据库
	bool GetGameGoldDBInfo(tagDataBaseParameter & DataBaseParameter);
	//比赛数据库
	bool GetGameMatchDBInfo(tagDataBaseParameter& DataBaseParameter);
	//约战数据库
	bool GetGameBattleDBInfo(tagDataBaseParameter& DataBaseParameter);
	//排位数据库
	bool GetGameRankingDBInfo(tagDataBaseParameter& DataBaseParameter);

	//配置函数
public:
	//消息数据库
	bool SetMessageDBInfo(const tagDataBaseParameter& DataBaseParameter);
	//平台数据库
	bool SetPlatformDBInfo(const tagDataBaseParameter & DataBaseParameter);
	//帐户数据库
	bool SetAccountsDBInfo(const tagDataBaseParameter & DataBaseParameter);
	//金币数据库
	bool SetGameGoldDBInfo(const tagDataBaseParameter & DataBaseParameter);
	//比赛数据库
	bool SetGameMatchDBInfo(const tagDataBaseParameter& DataBaseParameter);
	//约战数据库
	bool SetGameBattleDBInfo(const tagDataBaseParameter& DataBaseParameter);
	//排位数据库
	bool SetGameRankingDBInfo(const tagDataBaseParameter& DataBaseParameter);

	//内部函数
private:
	//读取信息
	bool ReadDataBaseInfo(LPCTSTR pszSubName, tagDataBaseParameter & DataBaseParameter);
	//写入信息
	bool WriteDataBaseInfo(LPCTSTR pszSubName, const tagDataBaseParameter & DataBaseParameter);
	//写入配置
	bool WriteEncryptString(LPCTSTR pszAppName, LPCTSTR pszName, LPCTSTR pszString);
	//写入配置
	bool WriteNumber(LPCTSTR pszAppName, LPCTSTR pszName,int nNum);

	//读取配置
	bool ReadEncryptString(LPCTSTR pszAppName, LPCTSTR pszKeyName, LPTSTR pszString, UINT uMaxCount);
	//读取配置
	UINT ReadNumber(LPCTSTR pszAppName, LPCTSTR pszKeyName);
};

//////////////////////////////////////////////////////////////////////////

#endif