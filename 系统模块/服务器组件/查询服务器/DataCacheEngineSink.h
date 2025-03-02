#ifndef DATACACHE_ENGINE_SINK_HEAD_FILE
#define DATACACHE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "map"
#include "redis++.h"
#include "InitParameter.h"
#include "DataBasePacket.h"

using namespace redis;
using namespace lhchat;

//////////////////////////////////////////////////////////////////////////

//数据库钩子
class CDataCacheEngineSink : public IDataBaseEngineSink
{
	//友元定义
	friend class CServiceUnits;			

	//组件变量
protected:
	CDataBaseAide					m_AccountsDBAide;					//消息数据库	
	CDataBaseHelper					m_AccountsDBModule;					//消息数据库

	//缓冲对象
protected:
	Redis *							m_pUserWClient;						//缓冲对象
	Redis *							m_pUserRClient;						//缓冲对象
	Redis *							m_pFriendClient;					//缓冲对象

	//脚本变量
protected:
	string							m_ScriptFriendList;					//好友列表

	//组件变量
protected:
	CInitParameter *				m_pInitParameter;					//配置参数
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//数据事件

	//函数定义
public:
	//构造函数
	CDataCacheEngineSink();
	//析构函数
	virtual ~CDataCacheEngineSink();

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

	//好友命令
public:
	//同步消息
	bool OnRequestSyncUserInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//查询消息
	bool OnRequestQueryUserInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//查询黑名单
	bool OnRequestQueryBlackList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//查询好友列表
	bool OnRequestQueryFriendList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//查询好友信息
	bool OnRequestQueryFriendInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//查询好友扩展
	bool OnRequestQueryFriendExtras(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//请求命令
public:
	//请求令牌
	bool OnRequestBuildAccessToken(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//辅助函数
protected:
	//查询权限
	WORD QueryMasterOrder(UINT32 iUserID);
	//查询标识
	bool QueryUserID(vector<UINT32>& vGameID, vector<UINT32>& vUserID);
	//查询信息
	bool QueryUserInfo(vector<UINT32>& vUserID,list<tagIMUserPublicInfo>& lUserPublicInfo);
	//缓冲用户
	bool CacheUserInfo(list<tagIMUserPublicInfo>& lIMUserPublicInfo, list<string>& lUserPublicInfo);	

	//辅助函数
protected:		
	//输出异常
	void TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException);
};

//////////////////////////////////////////////////////////////////////////

#endif