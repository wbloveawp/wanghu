#ifndef MODULE_CLUBPLAZA_HEAD_FILE
#define MODULE_CLUBPLAZA_HEAD_FILE
#pragma once

//引入文件
#include "SimulateLinkHead.h"
#include "VirtualLinkManager.h"

/////////////////////////////////////////////////////////////////////////////////////

//消息定义
#define WM_CLUBPLAZA_LINK_SHUT			(WM_USER+1500)					//链接关闭
#define WM_CLUBPLAZA_LINK_FAILURE		(WM_USER+1501)					//链接失败
#define WM_CLUBPLAZA_LOGON_FINISH		(WM_USER+1502)					//登录完成
#define WM_CLUBPLAZA_LOGON_FAILURE		(WM_USER+1503)					//登录失败

/////////////////////////////////////////////////////////////////////////////////////
//类型定义
typedef CWHArray<tagClubApplyMessage>	CArrayApplyMessage;

/////////////////////////////////////////////////////////////////////////////////////

//茶社广场
class CModuleClubPlaza : public IModuleItem, public CVirtualLinkItem
{
	//标志变量
protected:
	bool						m_bTestFlag;						//测试标志

	//配置变量
protected:
	tagClubOption				m_ClubOption;						//茶社配置
	tagClubNameCard				m_ClubNameCard;						//茶社名片	
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
	virtual ~CModuleClubPlaza();
	//构造函数
	CModuleClubPlaza(IModuleItemSink* pIModuleItemSink);

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
	virtual DWORD GetTimerStart() { return IDI_MODULE_CLUBPLAZA_START; }
	//时钟范围
	virtual DWORD GetTimerRange() { return IDI_MODULE_CLUBPLAZA_RANGE; }

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
	bool OnSocketMainClubPlazaService(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//网络事件
protected:
	//茶社配置
	bool OnSocketSubClubOption(VOID* pData, WORD wDataSize);
	//茶社名片
	bool OnSocketSubClubNameCard(VOID* pData, WORD wDataSize);
	//茶社消息
	bool OnSocketSubClubMessage(VOID* pData, WORD wDataSize);
	//进入完成
	bool OnSocketSubLogonFinish(VOID* pData, WORD wDataSize);
	//进入成功
	bool OnSocketSubLogonSuccess(VOID* pData, WORD wDataSize);	
	//进入失败
	bool OnSocketSubLogonFailure(VOID* pData, WORD wDataSize);	
	//查询结果
	bool OnSocketSubSearchResult(VOID* pData, WORD wDataSize);
	//操作成功
	bool OnSocketSubOperateSuccess(VOID* pData, WORD wDataSize);
	//操作失败
	bool OnSocketSubOperateFailure(VOID* pData, WORD wDataSize);
	
	//功能函数
public:
	//重置模块
	VOID ResetModule();
	//进入服务
	bool EnterService();

	//测试函数
public:
	//创建茶社
	bool CreateClubItem();
	//查询茶社
	VOID SearchClubItem(DWORD dwClubID);
	//删除茶社
	VOID DeleteClubItem(DWORD dwClubID);
	//更新茶社
	VOID UpdateClubItem(DWORD dwClubID);
	//请求加入
	VOID RequestJoinClub(DWORD dwClubID);
	//删除消息
	VOID DeleteClubMessage(DWORD dwMessageID,BYTE cbMessageType);
	//删除成员
	VOID DeleteClubMember(DWORD dwClubID, DWORD dwMemberID, BYTE cbDeleteKind);
	//更新成员
	VOID UpdateClubMemberType(DWORD dwClubID, DWORD dwMemberID, BYTE cbMemberType);
	//更新成员
	VOID UpdateClubMemberStatus(DWORD dwClubID, DWORD dwMemberID, BYTE cbMemberStatus);
	//响应加入
	VOID RespondJoinClub(tagClubApplyMessage* pApplyMessage, BYTE cbApplyStatus);
};

/////////////////////////////////////////////////////////////////////////////////////

#endif
