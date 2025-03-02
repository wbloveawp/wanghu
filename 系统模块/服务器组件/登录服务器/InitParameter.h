#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

//配置参数
class CInitParameter
{
	//组件时间
public:
	WORD							m_wConnectTime;						//重连时间
	WORD							m_wCollectTime;						//统计时间
	WORD							m_wLoadListTime;					//列表时间
	WORD							m_wReLoadListTime;					//列表时间

	//客户时间
public:
	WORD							m_wIntermitTime;					//中断时间
	WORD							m_wOnLineCountTime;					//人数时间

	//配置信息
public:
	TCHAR							m_szServerName[LEN_SERVER];			//服务器名

	//连接信息
public:
	tagRedisConnOption				m_UserConnOption;					//用户连接	
	tagRedisConnOption				m_TokenConnOption;					//令牌连接	
	tagRedisConnOption				m_FriendConnOption;					//好友连接
	tagRedisConnOption				m_MasterConnOption;					//管理连接

	//连接信息
public:
	WORD							m_wCenterPort;						//中心端口
	tagAddressInfo					m_CenterAddress;					//中心地址
	tagAddressInfo					m_ServiceAddress;					//服务地址	
	tagDataBaseParameter			m_PlatformDBParameter;				//连接信息
	tagDataBaseParameter			m_AccountsDBParameter;				//连接信息

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
