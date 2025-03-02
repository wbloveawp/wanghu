#ifndef VIRTUAL_LINK_MANAGER_HEAD_FILE
#define VIRTUAL_LINK_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
//#include "SimulateLinkHead.h"

//////////////////////////////////////////////////////////////////////////

//链接状态
#define LINK_STATUS_IDLE				0								//空闲状态
#define LINK_STATUS_WAIT				1								//等待状态
#define LINK_STATUS_LINKED				2								//链接状态

//////////////////////////////////////////////////////////////////////////

//类说明
class CVirtualLinkItem;
class CVirtualLinkManager;

//////////////////////////////////////////////////////////////////////////
//回调接口
interface IVirtualLinkSink
{
	//准备就绪
	virtual bool OnEventSocketReady() = NULL;
	//连接事件
	virtual bool OnEventVirtualLink(LONG lErrorCode) = NULL;
	//关闭事件
	virtual bool OnEventVirtualShut(BYTE cbShutReason) = NULL;			
	//读取事件
	virtual bool OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize) = NULL;
};

//回调接口
interface ISessionItemSink
{
	//连接事件
	virtual bool OnEventMissionLink(LONG lErrorCode) = NULL;
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason) = NULL;	
	//读取事件
	virtual bool OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize) = NULL;
};

//////////////////////////////////////////////////////////////////////////
// 
//任务子项
class CSessionItem : public ISessionItemSink
{
	//友元定义
	friend class CVirtualLinkItem;

	//变量定义
protected:
	bool						m_bActiveStatus;						//任务状态
	CVirtualLinkItem *			m_pVirtualLinkItem;						//虚拟链接

	//函数定义
public:
	//构造函数
	CSessionItem();
	//析构函数
	virtual ~CSessionItem();

	//功能函数
public:
	//任务状态
	bool GetActiveStatus() { return m_bActiveStatus; }
	//虚拟连接
	CVirtualLinkItem* GetVirtualLinkItem() { return m_pVirtualLinkItem; }
};

//////////////////////////////////////////////////////////////////////////

//数组说明
typedef CWHArray<CSessionItem*> CSessionItemPtrArray;

//链接子项
class CVirtualLinkItem : public IVirtualLinkSink
{
	//友元定义
	friend class CVirtualLinkManager;

	//状态变量
protected:
	BYTE						m_cbLinkStatus;							//链接状态	

	//模块信息
protected:
	WORD						m_wServiceID;							//服务标识
	WORD						m_wServiceModule;						//服务模块	

	//连接参数
protected:
	CMD_GG_C_LinkService		m_LinkService;							//连接参数

	//任务变量
protected:
	CSessionItemPtrArray		m_SessionItemArray;						//任务数组
	
	//变量定义
private:
	CVirtualLinkManager *		m_pVirtualLinkManager;					//连接管理

	//函数定义
public:
	//构造函数
	CVirtualLinkItem();
	//析构函数
	virtual ~CVirtualLinkItem();

	//回调接口
public:
	//准备就绪
	virtual bool OnEventSocketReady();
	//连接事件
	virtual bool OnEventVirtualLink(LONG lErrorCode);
	//关闭事件
	virtual bool OnEventVirtualShut(BYTE cbShutReason);	
	//读取事件
	virtual bool OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize);

	//信息函数
public:
	//服务标识
	WORD GetServiceID() { return m_wServiceID; }
	//连接状态
	BYTE GetLinkStatus() { return m_cbLinkStatus; }	
	//服务模块
	WORD GetServiceModule() { return m_wServiceModule; }	

	//配置函数
public:
	//服务模块
	VOID SetServiceModule(WORD wServiceModule) { m_wServiceModule=wServiceModule; }

	//任务管理
public:		
	//删除任务
	VOID DeleteSessionItem();
	//删除任务
	bool DeleteSessionItem(CSessionItem* pSessionItem);
	//加入任务
	bool InsertSessionItem(CSessionItem* pSessionItem);	
	
	//发送函数
public:
	//发送数据
	bool SendVirtualData(WORD wMainCmdID, WORD wSubCmdID);
	//发送数据
	bool SendVirtualData(WORD wMainCmdID, WORD wSubCmdID, VOID* const pData, WORD wDataSize);

	//链接管理
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
	CTCPSocketHelper				m_TCPSocketModule;					//网络连接

	//链接子项
protected:
	CVirtualLinkPtrArray			m_VirtualLinkArray;					//链接子项

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
	//设置地址
	VOID SetCurrentServer(LPCTSTR pszCurrentServer);
	//加入链接
	bool InsertVirtualLink(CVirtualLinkItem * pVirtualLinkItem);
	//删除链接
	bool DeleteVirtualLink(CVirtualLinkItem * pVirtualLinkItem);

	//功能函数
public:
	//关闭网络
	bool ConcludeVirtualLink();
	//激活链接
	bool ActiveVirtualLink(CVirtualLinkItem* pVirtualLinkItem);	

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
	//地址信息
	bool GetServerInfo();
	//切换连接
	bool SwitchServerItem();
	//地址解释
	DWORD TranslateAddress(LPCTSTR szServerIP);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif