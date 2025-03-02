#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once


//////////////////////////////////////////////////////////////////////////

//配置参数
class CInitParameter
{
	//系统配置
public:
	WORD							m_wMaxConnect;						//最大连接
	WORD							m_wServicePort;						//服务端口
	WORD							m_wConnectTime;						//重连时间
	WORD							m_wCollectTime;						//统计时间
	WORD							m_wUploadTime;						//上传时间
	WORD							m_wUpdateTime;						//更新时间

	//配置信息
public:
	TCHAR							m_szUploadDomain[128];				//上传域名	

	//连接信息
public:
	tagDataBaseParameter			m_AccountsDBParameter;				//连接信息
	tagDataBaseParameter			m_PlatformDBParameter;				//连接信息

	//连接信息
public:
	tagRedisConnOption				m_RedisConnOption;					//连接选项

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
