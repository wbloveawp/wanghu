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
	
	//静态函数
protected:
	static CCriticalSection			m_DirectoryLocked;					//目录锁定

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

	//存储时间
public:
	//存储录像
	bool OnRequestStoreVideo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

	//静态函数
protected:
	//文件存在
	static BOOL IsFileExist(LPCTSTR pszFileName);
	//目录存在
	static BOOL IsDirectoryExist(LPCTSTR pszDirectory);
	//创建目录
	static BOOL CreateDirectory(LPCTSTR pszFileDirectory);		
	//创建文件
	static BOOL CreateFile(LPCTSTR pszFileName,BYTE cbFileData[],DWORD dwFileSize);	
};

//////////////////////////////////////////////////////////////////////////

#endif