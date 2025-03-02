#ifndef MISSION_STOCK_HEAD_FILE
#define MISSION_STOCK_HEAD_FILE
#pragma once

#include "StdAfx.h"
#include "MissionManager.h"

//////////////////////////////////////////////////////////////////////////////////
//动作枚举
enum emStockAction
{
	SAction_NULL=0,
	SAction_QueryStockInfo,			//获取库存
	SAction_AppendStockInfo,		//添加库存
	SAction_ModifyStockInfo,		//修改库存
	SAction_DeleteStockInfo,		//删除库存
	SAction_AdjustStockScore,		//调整库存
	SAction_AdjustJackpotScore		//调整彩金
};

//////////////////////////////////////////////////////////////////////////////////
//接口定义
interface IStockOperateCenter
{
	//控件使能
	virtual VOID OnEventEnableControls(BOOL bEnabled)=NULL;
	//操作结果
	virtual VOID OnEventStockOperateResult(WORD wSubCommdID, WORD wStockCount, tagStockInfo * pStockInfo)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

//机器任务
class CMissionStock : public CWnd, public CMissionItem
{
	//存储变量
protected:
	WORD								m_wDataSize;							//数据大小
	BYTE								m_cbDataBuffer[SOCKET_PACKET];			//数据缓冲

	//变量定义
protected:
	emStockAction						m_StockAction;							//库存动作	

	//接口变量
protected:
	IStockOperateCenter *				m_pIStockOperateCenter;					//库存接口

	//静态变量
protected:
	static CMissionStock *				m_pMissionStock;						//对象指针

	//函数定义
public:
	//构造函数
	CMissionStock();
	//析构函数
	virtual ~CMissionStock();

	//重载函数
public:
	//连接事件
	virtual bool OnEventMissionLink(INT nErrorCode);
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//读取事件
	virtual bool OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize);

	//配置接口
public:
	//设置接口
	VOID SetStockOperateCenter(IStockOperateCenter* pIStockOperateCenter);

	//操作函数
public:
	//删除库存
	VOID DeleteStockInfo(WORD wStockID);
	//查询库存
	VOID QueryStockInfo(LPCTSTR pszKindIDList);
	//修改库存
	VOID ModifyStockInfo(tagStockInfo* pStockInfo);
	//添加库存
	VOID AppendStockInfo(tagStockInfo* pStockInfo);
	//调整库存
	VOID AdjustStockScore(WORD wStockID,SCORE lChangeScore);
	//调整彩金
	VOID AdjustJackpotScore(WORD wStockID,WORD wLevelID,SCORE lChangeScore);

	//静态函数
public:
	//获取对象
	static CMissionStock * GetInstance() { return m_pMissionStock; }

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif