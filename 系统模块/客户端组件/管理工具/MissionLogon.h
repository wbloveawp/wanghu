#ifndef MISSION_LOGON_HEAD_FILE
#define MISSION_LOGON_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgLogon.h"
#include "DlgStatus.h"
#include "MissionManager.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define MT_LOGON					0									//登录任务

//////////////////////////////////////////////////////////////////////////

//登录任务
class CMissionLogon : public CMissionItem, public IHttpServiceSink
{
	//友元定义
	friend class CDlgLogon;
	friend class CPlatformFrame;

	//变量定义
protected:
	BYTE							m_cbMissionType;					//任务标识
	CHAR							m_chBuffer[1024];					//数据缓冲

	//控件指针
protected:
	CDlgLogon *						m_pDlgLogon;						//登录窗口
	CDlgStatus *					m_pDlgStatus;						//状态窗口

	//组件变量
protected:
	CHttpServiceHelper				m_HttpServiceHelper;				//HTTP服务

	//验证变量
protected:
	MSXML2::IXMLDOMDocumentPtr		m_pXmlDoc;							//XML组件

	//静态变量
protected:
	static CMissionLogon *			m_pMissionLogon;					//对象指针

	//函数定义
public:
	//构造函数
	CMissionLogon();
	//析构函数
	virtual ~CMissionLogon();

	//HTTP服务回调
public:
	////请求缓冲
	//virtual DWORD __cdecl OnHttpRequestBuffer(DWORD dwRequestSize, LPVOID *ppvBuffer);
	////下载完成
	//virtual bool __cdecl OnHttpDataComplete(DWORD dwDataSize);
	////下载异常
	//virtual bool __cdecl OnHttpDataException(enDownLoadError DownLoadError);
	////连接关闭
	//virtual bool __cdecl OnHttpSocketClose(bool bCloseByServer);

	//重载函数
public:
	//连接事件
	virtual bool OnEventMissionLink(INT nErrorCode);
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//读取事件
	virtual bool OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize);

	//功能函数
public:
	//显示登录
	VOID ShowLogon();

	//流程函数
public:
	//执行登录
	bool PerformLogonMission(BYTE cbMissonType);

	//辅助函数
protected:
	//更新用户
	VOID UpdateUserInfo();
	//显示状态
	VOID ShowStatusInfo(LPCTSTR pszStatusInfo);
	//关闭状态
	VOID CloseStatusInfo();
	//恢复界面
	VOID RestoreShow();

	//网络事件
protected:
	//登录处理
	bool OnSocketMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//系统处理
	bool OnSocketMainSystem(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//玩家服务
	bool OnSocketMainService(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//房间列表
	bool OnSocketMainServerList(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//网络事件
protected:
	//登录成功
	bool OnSocketSubLogonSuccess(VOID * pData, WORD wDataSize);
	//登录失败
	bool OnSocketSubLogonFailure(VOID * pData, WORD wDataSize);
	//登录完成
	bool OnSocketSubLogonFinish(VOID * pData, WORD wDataSize);
	//升级提示
	bool OnSocketSubUpdateNotify(VOID * pData, WORD wDataSize);
	//动作消息
	bool OnSocketSubActionMessage(VOID * pData, WORD wDataSize);
	//安全提醒
	bool OnSocketSubSafePrompt(VOID * pData, WORD wDataSize);
	//激活帐号
	bool OnSocketSubActivateAccounts(VOID * pData, WORD wDataSize);
	//人数显示
	bool OnSocketSubShowOnLine(VOID * pData, WORD wDataSize);
	//后台升级
	bool OnSocketSubUpdateSilent(VOID * pData, WORD wDataSize);

	//静态函数
public:
	//获取对象
	static CMissionLogon * GetInstance() { return m_pMissionLogon; }
};

//////////////////////////////////////////////////////////////////////////

#endif