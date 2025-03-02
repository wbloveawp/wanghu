#ifndef MISSION_MANAGER_HEAD_FILE
#define MISSION_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//类说明
class CMissionManager;

//////////////////////////////////////////////////////////////////////////

//任务回调
interface IMissionSocketSink
{
	//连接事件
	virtual bool OnEventMissionLink(INT nErrorCode)=NULL;
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason)=NULL;
	//读取事件
	virtual bool OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//任务连接
class CMissionSocket : public ITCPSocketSink
{
	//连接信息
protected:
	WORD							m_wCurrentPort;						//当前端口
	DWORD							m_dwCurrentServer;					//当前地址

	//组件变量
protected:
	CTCPSocketHelper				m_TCPSocketModule;					//网络连接

	//接口变量
protected:
	IMissionSocketSink *			m_pIMissionSocketSink;				//回调接口

	//函数定义
public:
	//构造函数
	CMissionSocket();
	//析构函数
	virtual ~CMissionSocket();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { return; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//网络接口
protected:
	//连接事件
	virtual bool __cdecl OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode);
	//关闭事件
	virtual bool __cdecl OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason);
	//读取事件
	virtual bool __cdecl OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID * pData, WORD wDataSize);

	//状态函数
public:
	//网络状态
	BYTE GetSocketStatus();

	//配置函数
public:
	//设置接口
	VOID SetMissionSocketSink(IMissionSocketSink * pIMissionSocketSink) { m_pIMissionSocketSink=pIMissionSocketSink; }
	//设置连接信息
	VOID SetConnectInfo( DWORD dwServerIP, WORD wPort );

	//功能函数
public:
	//中断连接
	bool PerformClose();
	//发起连接
	bool PerformConnect();
	//发送函数
	bool PerformSendData(WORD wMainCmdID, WORD wSubCmdID);
	//发送函数
	bool PerformSendData(WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////

//任务子项
class CMissionItem : public IMissionSocketSink
{
	//友元定义
	friend class CMissionManager;

	//变量定义
private:
	bool						m_bActiveStatus;						//任务状态
	CMissionManager *			m_pMissionManager;						//任务管理

	//函数定义
public:
	//构造函数
	CMissionItem();
	//析构函数
	virtual ~CMissionItem();

	//功能函数
public:
	//任务状态
	bool GetActiveStatus() { return m_bActiveStatus; }
	//任务管理
	CMissionManager * GetMissionManager() { return m_pMissionManager; }
};

//////////////////////////////////////////////////////////////////////////

//数组说明
typedef CWHArray<CMissionItem *> CMissionItemPtrArray;

//任务调遣
class CMissionManager : public CWnd, public IMissionSocketSink, public IRedirectServiceSink
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
	CMissionItemPtrArray			m_MissionItemArray;					//任务子项

	//函数定义
public:
	//构造函数
	CMissionManager();
	//析构函数
	virtual ~CMissionManager();

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
	//下载完成
	//virtual bool __cdecl OnRedirectComplete(LPCTSTR pszServerIP, int nParam);
	////下载异常
	//virtual bool __cdecl OnRedirectException(enDownLoadError DownLoadError);

	//管理函数
public:
	//设置地址
	VOID SetCurrentServer(LPCTSTR pszCurrentServer);
	//加入任务
	bool InsertMissionItem(CMissionItem * pMissionItem);
	//删除任务
	bool DeleteMissionItem(CMissionItem * pMissionItem);

	//功能函数
public:
	//激活任务
	bool AvtiveMissionItem(CMissionItem * pMissionItem, bool bContinue);
	//终止任务
	bool ConcludeMissionItem(CMissionItem * pMissionItem);

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
	//创建虚拟链接
	void CreateVirtualLink();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif