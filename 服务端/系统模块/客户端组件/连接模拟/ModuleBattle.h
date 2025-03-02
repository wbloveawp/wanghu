#ifndef MODULE_BATTLE_HEAD_FILE
#define MODULE_BATTLE_HEAD_FILE
#pragma once

//引入文件
#include "SimulateLinkHead.h"
#include "VirtualLinkManager.h"

/////////////////////////////////////////////////////////////////////////////////////

//消息定义
#define WM_BATTLE_LINK_SHUT			(WM_USER+1500)					//链接关闭
#define WM_BATTLE_LINK_FAILURE		(WM_USER+1501)					//链接失败
#define WM_BATTLE_LOGON_FINISH		(WM_USER+1502)					//登录完成
#define WM_BATTLE_LOGON_FAILURE		(WM_USER+1503)					//登录失败

/////////////////////////////////////////////////////////////////////////////////////
//类型定义
typedef CWHArray<tagBattleBaseOption>	CArrayBaseOption;

/////////////////////////////////////////////////////////////////////////////////////

//约战模块
class CModuleBattle : public IModuleItem, public CVirtualLinkItem
{
	//变量定义
protected:
	tagBattleTableItem			m_BattleTableItem;					//桌子子项
	CArrayBaseOption			m_ArrayBaseOption;					//基础配置

	//缓冲变量
protected:
	tagDataPacket				m_BufferPacket;						//缓冲数据

	//接口变量
protected:
	IModuleItemSink	*			m_pIModuleItemSink;					//回调接口		

	//函数定义
public:
	//析构函数
	virtual ~CModuleBattle();
	//构造函数
	CModuleBattle(IModuleItemSink* pIModuleItemSink);

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
	virtual DWORD GetTimerStart() { return IDI_MODULE_BATTLE_START; }
	//时钟范围
	virtual DWORD GetTimerRange() { return IDI_MODULE_BATTLE_RANGE; }

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
	bool OnSocketMainBattleService(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//网络事件
protected:
	//桌子子项
	bool OnSocketSubTableItem(VOID* pData, WORD wDataSize);
	//桌子列表
	bool OnSocketSubTableList(VOID* pData, WORD wDataSize);
	//桌子参数
	bool OnSocketSubTableParam(VOID* pData, WORD wDataSize);
	//桌子解散
	bool OnSocketSubTableDismiss(VOID* pData, WORD wDataSize);
	//操作成功
	bool OnSocketSubOperateSuccess(VOID* pData, WORD wDataSize);
	//操作失败
	bool OnSocketSubOperateFailure(VOID* pData, WORD wDataSize);
	//基础配置
	bool OnSocketSubBattleBaseOption(VOID* pData, WORD wDataSize);			
	//游戏配置
	bool OnSocketSubBattleGameOption(VOID* pData, WORD wDataSize);
	//配置完成
	bool OnSocketSubBattleOptionFinish(VOID* pData, WORD wDataSize);


	//测试函数
public:
	//创建桌子
	VOID CreateTable(tagBattleBaseOption* pBattleBaseOption);
	
	//功能函数
public:
	//重置模块
	VOID ResetModule();
	//进入服务
	bool EnterService();
};

/////////////////////////////////////////////////////////////////////////////////////

#endif
