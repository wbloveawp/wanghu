#ifndef MODULE_LOGON_HEAD_FILE
#define MODULE_LOGON_HEAD_FILE
#pragma once

//引入文件
#include "SimulateLinkHead.h"
#include "VirtualLinkManager.h"

/////////////////////////////////////////////////////////////////////////////////////

//消息定义
#define WM_LOGON_LINK_SHUT			(WM_USER+1000)					//链接关闭
#define WM_LOGON_LINK_FAILURE		(WM_USER+1001)					//链接失败
#define WM_LOGON_LOGON_FINISH		(WM_USER+1002)					//登录完成
#define WM_LOGON_LOGON_FAILURE		(WM_USER+1003)					//登录失败

/////////////////////////////////////////////////////////////////////////////////////

//登录模块
class CModuleLogon : public IModuleItem, public CVirtualLinkItem
{
	//变量定义
protected:
	IModuleItemSink	*			m_pIModuleItemSink;					//回调接口

	//缓冲变量
protected:
	tagDataPacket				m_BufferPacket;						//缓冲数据

	//函数定义
public:
	//析构函数
	virtual ~CModuleLogon();
	//构造函数
	CModuleLogon(IModuleItemSink* pIModuleItemSink);

	//回调函数
public:
	//关闭事件
	virtual bool OnEventVirtualShut();
	//连接事件
	virtual bool OnEventVirtualLink(LONG lErrorCode);
	//读取事件
	virtual bool OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize);

	//信息函数
public:
	//时钟标识
	virtual DWORD GetTimerStart() { return IDI_MODULE_LOGON_START; }
	//时钟范围
	virtual DWORD GetTimerRange() { return IDI_MODULE_LOGON_RANGE; }

	//功能函数
public:
	//关闭时钟
	virtual bool KillModuleTimer(UINT uTimerID);
	//设置时钟
	virtual bool SetModuleTimer(UINT uTimerID, UINT nElapse);

	//时钟事件
public:
	//结束时钟
	virtual bool OnEventTimer(UINT uTimerID, WPARAM wBindParam);

	//网络事件
protected:
	//登录处理
	bool OnSocketMainLogon(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//列表处理
	bool OnSocketMainServerList(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//用户服务
	bool OnSocketMainUserService(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//网络事件
protected:
	//登录失败
	bool OnSocketSubLogonFailure(VOID* pData, WORD wDataSize);
	//登录成功
	bool OnSocketSubLogonSuccess(VOID* pData, WORD wDataSize);
	//登录完成
	bool OnSocketSubLogonFinish(VOID* pData, WORD wDataSize);
	//更新提示
	bool OnSocketSubUpdateNotify(VOID* pData, WORD wDataSize);

	//功能函数
public:
	//游客登录
	bool LogonByVisitor(LPCTSTR pszMachineID);
	//账号登录
	bool LogonByAccounts(LPCTSTR pszAccounts, LPCTSTR pszLogonPass);	

	//功能函数
public:
	//重置模块
	VOID ResetModule();
};

/////////////////////////////////////////////////////////////////////////////////////

#endif
