#pragma once

#include "Stdafx.h"
#include "SimulateLinkHead.h"

//////////////////////////////////////////////////////////////////////////

//全局单元
class CGlobalUnits
{
	//状态变量
public:
	BYTE							m_cbTestMode;						//测试模式
	BYTE							m_cbConnectMode;					//连接模式	
	bool							m_bConnectStatus;					//连接状态

	//全局配置
public:
	WORD							m_wKindID;							//类型标识
	WORD							m_wServerType;						//房间类型
	WORD							m_wServerPort;						//服务端口
	WORD							m_wServerLevel;						//房间等级
	TCHAR							m_szServerAddr[32];					//服务地址	
	DWORD							m_dwProcessVersion;					//进程版本

	//函数定义
public:
	//构造函数
	CGlobalUnits();
	//析构函数
	virtual ~CGlobalUnits();
};

//////////////////////////////////////////////////////////////////////////

//全局单元
extern CGlobalUnits g_GlobalUnits;

//////////////////////////////////////////////////////////////////////////
