#ifndef MODULE_CLUB_HEAD_FILE
#define MODULE_CLUB_HEAD_FILE
#pragma once

//引入文件
#include "SimulateLinkHead.h"
#include "VirtualLinkManager.h"

/////////////////////////////////////////////////////////////////////////////////////

//消息定义
#define WM_CLUB_LINK_SHUT			(WM_USER+1400)					//链接关闭
#define WM_CLUB_LINK_FAILURE		(WM_USER+1401)					//链接失败
#define WM_CLUB_LOGON_FINISH		(WM_USER+1402)					//登录完成
#define WM_CLUB_LOGON_FAILURE		(WM_USER+1403)					//登录失败

/////////////////////////////////////////////////////////////////////////////////////
//类型定义
typedef CWHArray<tagClubApplyMessage>	CArrayApplyMessage;

/////////////////////////////////////////////////////////////////////////////////////

//茶社模块
class CModuleClub : public IModuleItem, public CVirtualLinkItem
{
	//变量定义
protected:
	tagClubItem					m_ClubItem;							//茶社子项
	CArrayApplyMessage			m_ArrayApplyMessage;				//申请消息

	//缓冲变量
protected:
	tagDataPacket				m_BufferPacket;						//缓冲数据

	//接口变量
protected:
	IModuleItemSink	*			m_pIModuleItemSink;					//回调接口		

	//函数定义
public:
	//析构函数
	virtual ~CModuleClub();
	//构造函数
	CModuleClub(IModuleItemSink* pIModuleItemSink);

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
	virtual DWORD GetTimerStart() { return IDI_MODULE_CLUB_START; }
	//时钟范围
	virtual DWORD GetTimerRange() { return IDI_MODULE_CLUB_RANGE; }

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
	bool OnSocketMainClubService(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//网络事件
protected:
	//茶社子项
	bool OnSocketSubClubItem(VOID* pData, WORD wDataSize);
	//茶社成员
	bool OnSocketSubClubMember(VOID* pData, WORD wDataSize);
	//茶社消息
	bool OnSocketSubClubMessage(VOID* pData, WORD wDataSize);
	//进入成功
	bool OnSocketSubEnterSuccess(VOID* pData, WORD wDataSize);
	//进入失败
	bool OnSocketSubEnterFailure(VOID* pData, WORD wDataSize);			
	
	//功能函数
public:
	//重置模块
	VOID ResetModule();
	//进入服务
	bool EnterService(DWORD dwClubID);
};

/////////////////////////////////////////////////////////////////////////////////////

#endif
