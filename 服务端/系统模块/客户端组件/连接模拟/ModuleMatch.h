#ifndef MODULE_MATCH_HEAD_FILE
#define MODULE_MATCH_HEAD_FILE
#pragma once

//引入文件
#include "SimulateLinkHead.h"
#include "VirtualLinkManager.h"

/////////////////////////////////////////////////////////////////////////////////////

//消息定义
#define WM_MATCH_LINK_SHUT			(WM_USER+1200)					//链接关闭
#define WM_MATCH_LINK_FAILURE		(WM_USER+1201)					//链接失败
#define WM_MATCH_LOGON_FINISH		(WM_USER+1202)					//登录完成
#define WM_MATCH_LOGON_FAILURE		(WM_USER+1203)					//登录失败


/////////////////////////////////////////////////////////////////////////////////////
// 
//分组详情
struct tagMatchGroupInfo
{
	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次	
	BYTE							cbMatchType;						//比赛类型
	WORD							wStartCount;						//开始人数
	WORD							wSignupCount;						//报名人数

	//奖券信息
	DWORD							dwMatchTicket;						//比赛券

	//扩展数据
	BYTE							cbGroupDetail[512];					//分组详情
};

/////////////////////////////////////////////////////////////////////////////////////

//比赛模块
class CModuleMatch : public IModuleItem, public CVirtualLinkItem
{
	//配置变量
protected:
	tagMatchGroupInfo			m_MatchGroupInfo;					//分组信息
	tagMatchViewOption			m_MatchViewOption;					//比赛配置	

	//缓冲变量
protected:
	tagDataPacket				m_BufferPacket;						//缓冲数据

	//接口变量
protected:
	IModuleItemSink	*			m_pIModuleItemSink;					//回调接口		

	//函数定义
public:
	//析构函数
	virtual ~CModuleMatch();
	//构造函数
	CModuleMatch(IModuleItemSink* pIModuleItemSink);

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
	virtual DWORD GetTimerStart() { return IDI_MODULE_MATCH_START; }
	//时钟范围
	virtual DWORD GetTimerRange() { return IDI_MODULE_MATCH_RANGE; }

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
	//系统处理
	bool OnSocketMainSystem(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//登录处理
	bool OnSocketMainMatchService(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//网络事件
protected:
	//进入完成
	bool OnSocketSubEnterFinish(VOID* pData, WORD wDataSize);
	//购买结果
	bool OnSocketSubBuyTickResult(VOID* pData, WORD wDataSize);
	//查询结果
	bool OnSocketSubQueryTickResult(VOID* pData, WORD wDataSize);
	//分组子项
	bool OnSocketSubMatchSessionItem(VOID* pData, WORD wDataSize);
	//分组列表
	bool OnSocketSubMatchSessionList(VOID* pData, WORD wDataSize);
	//分组详情
	bool OnSocketSubMatchSessionDetail(VOID* pData, WORD wDataSize);
	//分组状态
	bool OnSocketSubMatchSessionStatus(VOID* pData, WORD wDataSize);
	//配置列表
	bool OnSocketSubMatchOptionList(VOID* pData, WORD wDataSize);
	//分组状态
	bool OnSocketSubMatchOptionStatus(VOID* pData, WORD wDataSize);			

	//网络事件
protected:
	//系统消息
	bool OnSocketSubSystemMessage(VOID* pData, WORD wDataSize);
	//操作失败
	bool OnSocketSubOperateFailure(VOID* pData, WORD wDataSize);
	
	//功能函数
public:
	//重置模块
	VOID ResetModule();
	//进入服务
	bool EnterService();
};

/////////////////////////////////////////////////////////////////////////////////////

#endif
