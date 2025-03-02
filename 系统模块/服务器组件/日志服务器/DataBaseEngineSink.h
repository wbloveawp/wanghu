#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "InitParameter.h"
#include "DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////

//数据库钩子
class CDataBaseEngineSink : public IDataBaseEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//组件变量
protected:
	CDataBaseAide					m_PlatformDBAide;					//平台数据库	
	CDataBaseHelper					m_PlatformDBModule;					//平台数据库

	//组件变量
protected:
	CInitParameter *				m_pInitParameter;					//配置参数
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//数据事件

	//函数定义
public:
	//构造函数
	CDataBaseEngineSink();
	//析构函数
	virtual ~CDataBaseEngineSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//系统事件
public:
	//启动事件
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnDataBaseEngineStop(IUnknownEx * pIUnknownEx);

	//内核事件
public:
	//时间事件
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//控制事件
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//请求事件
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

	//系统功能
protected:
	//加载配置
	bool OnRequestLoadLoggerOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

//	//约战玩法
//protected:
//	//添加玩法
//	bool OnRequestAppendConfig(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
//	//修改玩法
//	bool OnRequestModifyConfig(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
//	//删除玩法
//	bool OnRequestDeleteConfig(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	
	//辅助函数
protected:		
	//输出异常
	void TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException);
};

//////////////////////////////////////////////////////////////////////////

#endif