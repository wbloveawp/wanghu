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
	bool OnDBEventLoadGlobalConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//查询库存
	bool OnDBEventQueryStockItem(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//更新库存
	bool OnDBEventUpdateStockScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//更新状态
	bool OnDBEventUpdateStockState(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//更新彩池
	bool OnDBEventUpdateJackpotScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//调整库存
	bool OnDBEventAdjustStockScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//调整彩金
	bool OnDBEventAdjustJackpotScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	
	//辅助函数
protected:		
	//解析关联标识
	WORD ParseRelationID(LPCSTR szRelationIDList, WORD RelationStockID[], WORD wBufferCount);
	//解析抽水配置
	WORD ParseShrinkOption(LPCSTR szShrinkOption, tagShrinkItem ShrinkItem[], WORD wBufferCount);
	//解析彩金配置
	WORD ParseJackpotOption(LPCSTR szJackpotOption, tagJackpotItem JackpotItem[], WORD wBufferCount);
	//输出异常
	void TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException);
};

//////////////////////////////////////////////////////////////////////////

#endif