#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once


//////////////////////////////////////////////////////////////////////////

//配置参数
class CInitParameter
{
	//系统配置
public:
	WORD							m_wConnectTime;						//重连时间

	//配置信息
public:
	WORD							m_wCenterPort;						//中心端口
	tagAddressInfo					m_CenterAddress;					//中心地址

	//连接信息
public:
	tagDataBaseParameter			m_AccountsDBParameter;				//连接信息
	tagDataBaseParameter			m_PlatformDBParameter;				//连接信息	

	//连接信息
public:
	tagRedisConnOption				m_UserRConnOption;					//用户连接
	tagRedisConnOption				m_UserWConnOption;					//用户连接
	tagRedisConnOption				m_FriendConnOption;					//好友连接

	//函数定义
public:
	//构造函数
	CInitParameter();
	//析构函数
	virtual ~CInitParameter();

	//功能函数
public:
	//初始化
	VOID InitParameter();
	//加载配置
	VOID LoadInitParameter();
	//缓存配置
	VOID LoadRedisParameter();
};

//////////////////////////////////////////////////////////////////////////

#endif
