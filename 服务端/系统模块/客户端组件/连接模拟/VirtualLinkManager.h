#ifndef VIRTUAL_LINK_MANAGER_HEAD_FILE
#define VIRTUAL_LINK_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "SimulateLinkHead.h"

//////////////////////////////////////////////////////////////////////////

//类说明
class CVirtualLinkItem;
class CVirtualLinkManager;

//////////////////////////////////////////////////////////////////////////
//回调接口
interface IVirtualLinkSink
{
	//关闭事件
	virtual bool OnEventVirtualShut() = NULL;
	//连接事件
	virtual bool OnEventVirtualLink(LONG lErrorCode) = NULL;	
	//读取事件
	virtual bool OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize) = NULL;
};

//回调接口
interface IVirtualManagerSink
{
	//连接通知
	virtual bool OnEventTCPSocketLink(INT nErrorCode)=NULL;
	//关闭通知
	virtual bool OnEventTCPSocketShut(BYTE cbShutReason)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//链接子项
class CVirtualLinkItem : public IVirtualLinkSink
{
	//友元定义
	friend class CVirtualLinkManager;

	//状态变量
protected:
	bool						m_bLinkStatus;							//链接状态

	//模块信息
protected:
	WORD						m_wServiceID;							//服务标识
	WORD						m_wServiceModule;						//服务模块	
	
	//变量定义
private:
	CVirtualLinkManager *		m_pVirtualLinkManager;					//连接管理

	//函数定义
public:
	//构造函数
	CVirtualLinkItem();
	//析构函数
	virtual ~CVirtualLinkItem();

	//信息函数
public:
	//连接状态
	bool GetLinkStatus() { return m_bLinkStatus; }
	//服务标识
	WORD GetServiceID() { return m_wServiceID; }
	//服务模块
	WORD GetServiceModule() { return m_wServiceModule; }	

	//配置函数
public:
	//服务模块
	VOID SetServiceModule(WORD wServiceModule) { m_wServiceModule=wServiceModule; }
	
	//发送函数
public:
	//发送数据
	bool SendVirtualData(WORD wMainCmdID, WORD wSubCmdID);
	//发送数据
	bool SendVirtualData(WORD wMainCmdID, WORD wSubCmdID, VOID* const pData, WORD wDataSize);

	//管理函数
public:
	//关闭链接
	VOID CloseVirtualLink();
	//创建链接
	bool CreateVirtualLink(WORD wServiceID, DWORD dwUserID, LPCTSTR pszLogonToken, BYTE cbExtendData[], WORD wDataSize);

	//功能函数
public:
	//链接管理
	CVirtualLinkManager * GetVirtualLinkManager() { return m_pVirtualLinkManager; }
};

//////////////////////////////////////////////////////////////////////////

//数组说明
typedef CWHArray<CVirtualLinkItem *> CVirtualLinkPtrArray;

//链接调遣
class CVirtualLinkManager : public CWnd, public ITCPSocketSink
{
	//友元定义
	friend class CVirtualLinkItem;

	//连接信息
protected:
	WORD							m_wServerPort;						//服务端口
	DWORD							m_dwServerAddr;						//服务地址

	//组件变量
protected:
	CTCPSocketHelper				m_TCPSocketModule;					//网络连接

	//链接子项
protected:
	CVirtualLinkPtrArray			m_VirtualLinkArray;					//链接子项

	//接口变量
protected:
	IVirtualManagerSink*			m_pIVirtualManagerSink;				//回调接口

	//函数定义
public:
	//构造函数
	CVirtualLinkManager();
	//析构函数
	virtual ~CVirtualLinkManager();

	//基础接口
public:
	//释放对象
	virtual VOID Release() {}
	//接口查询
	virtual VOID* QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//网络接口
protected:
	//连接事件
	virtual bool OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode);
	//关闭事件
	virtual bool OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason);
	//读取事件
	virtual bool OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID* pData, WORD wDataSize);

	//状态函数
public:
	//网络状态
	BYTE GetSocketStatus();	

	//管理函数
public:
	//加入链接
	bool InsertVirtualLink(CVirtualLinkItem * pVirtualLinkItem);
	//删除链接
	bool DeleteVirtualLink(CVirtualLinkItem * pVirtualLinkItem);
	//设置接口
	VOID SetVirtualManagerSink(IVirtualManagerSink * pIVirtualManagerSink);

	//功能函数
public:
	//中断连接
	bool PerformClose();
	//发起连接
	bool PerformConnect(LPCTSTR pszServerAddr, WORD wServerPort);

	//功能函数
protected:
	//发送函数
	bool SendSocketData(WORD wMainCmdID, WORD wSubCmdID);
	//发送函数
	bool SendSocketData(WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize);

	//消息映射
protected:
	//时间消息
	VOID OnTimer(UINT_PTR nIDEvent);

	//内部函数
private:
	//地址解释
	DWORD TranslateAddress(LPCTSTR szServerIP);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif