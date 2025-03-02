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
	CWininetHttp					m_WininetHttp;						//网页组件

	//组件变量
protected:
	CDataBaseAide					m_AccountsDBAide;					//账号数据库	
	CDataBaseHelper					m_AccountsDBModule;					//账号数据库

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

	//系统事件
public:
	//请求网页
	bool OnRequestRequestWebPage(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

	//邮件服务
protected:
	//查询邮件
	bool OnRequestUserQueryMail(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//修改邮件
	bool OnRequestUserModifyMail(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//插入邮件
	bool OnRequestUserInsertMail(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//删除附件
	bool OnRequestUserDeleteMail(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//领取附件
	bool OnRequestClaimMailAttach(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);	

	//辅助函数
protected:
	//更新财富
	bool UpdateUserWealth(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID);	
	//更新物品
	bool UpdateUserGoods(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID, tagGoodsItem GoodsItemList[], WORD wGoodsCount);

	//辅助函数
protected:		
	//输出异常
	void TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException);
};

//////////////////////////////////////////////////////////////////////////

#endif