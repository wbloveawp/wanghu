#ifndef SERVICE_MANAGER_HEAD_FILE
#define SERVICE_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

class CServiceManager;

//任务回调
interface IServiceSocketSink
{
	//连接函数
public:
	//连接事件
	virtual bool OnEventMissionLink(INT nErrorCode)=NULL;
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason)=NULL;
	//读取事件
	virtual bool OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize)=NULL;

	//设置函数
public:
	//设置管理类
	virtual VOID SetServiceManager( CServiceManager *pServiceManager )=NULL;
	//设置活动状态
	virtual VOID SetActiveStatus(bool bActiveStatus)=NULL;

	//状态函数
public:
	//获取管理类
	virtual CServiceManager *GetServiceManager()=NULL;
	//是否活动状态
	virtual bool IsActiveStatus()=NULL;
};

//////////////////////////////////////////////////////////////////////////

//数组说明
typedef CWHArray<IServiceSocketSink *> CServiceItemPtrArray;

//任务调遣
class CServiceManager : public CWnd, public IMissionSocketSink, public IRedirectServiceSink
{
	//状态变量
protected:
	bool							m_bSwitchDns;						//DNS 转换
	bool							m_bTryDefault;						//尝试默认
	bool							m_bTryRedirect;						//尝试定向
	WORD							m_wSwitchIndex;						//切换索引

	//连接信息
protected:
	WORD							m_wCurrentPort;						//当前端口
	DWORD							m_dwCurrentServer;					//当前地址
	TCHAR							m_szCurrentServer[LEN_SERVER];		//当前地址

	//组件变量
protected:
	CMissionSocket					m_MissionSocket;					//任务连接
	CRedirectServiceHelper			m_RedirectServiceHelper;			//定向服务

	//任务子项
protected:
	CServiceItemPtrArray			m_MissionItemArray;					//任务子项

	//静态变量
public:
	static CServiceManager			*m_pServiceManager;					//服务组件

	//函数定义
public:
	//构造函数
	CServiceManager();
	//析构函数
	virtual ~CServiceManager();
	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { return; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//任务接口
protected:
	//连接事件
	virtual bool OnEventMissionLink(INT nErrorCode);
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//读取事件
	virtual bool OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize);

	//定向接口
protected:
	////下载完成
	//virtual bool __cdecl OnRedirectComplete(LPCTSTR pszServerIP, int nParam);
	////下载异常
	//virtual bool __cdecl OnRedirectException(enDownLoadError DownLoadError);

	//管理函数
public:
	//设置地址
	VOID SetCurrentServer(LPCTSTR pszCurrentServer);
	//加入任务
	bool InsertMissionItem(IServiceSocketSink * pMissionItem);
	//删除任务
	bool DeleteMissionItem(IServiceSocketSink * pMissionItem);

	//功能函数
public:
	//激活任务
	bool AvtiveMissionItem(IServiceSocketSink * pMissionItem, bool bContinue);
	//终止任务
	bool ConcludeMissionItem(IServiceSocketSink * pMissionItem);

	//功能函数
public:
	//发送函数
	bool SendData(WORD wMainCmdID, WORD wSubCmdID);
	//发送函数
	bool SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize);

	//消息映射
protected:
	//时间消息
	VOID OnTimer(UINT_PTR nIDEvent);

	//内部函数
private:
	//地址信息
	bool GetServerInfo();
	//切换连接
	bool SwitchServerItem();
	//重新定向
	bool RedirectionServer();

	//静态函数
public:
	static CServiceManager *GetServiceManager() { return m_pServiceManager; }

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif