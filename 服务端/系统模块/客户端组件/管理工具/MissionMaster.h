#ifndef MISSION_MASTER_HEAD_FILE
#define MISSION_MASTER_HEAD_FILE
#pragma once

#include "StdAfx.h"
#include "MissionManager.h"

//////////////////////////////////////////////////////////////////////////////////
//动作枚举
//enum emRobotAction
//{
//	RAction_NULL=0,
//	RAction_QueryParameter,			//获取参数
//	RAction_AppendParameter,			//添加参数
//	RAction_ModifyCustom,			//修改参数
//	RAction_ModifyParameter,		//修改参数
//	RAction_DeleteParameter			//删除参数
//};

//////////////////////////////////////////////////////////////////////////////////
//接口定义
interface IManagerOperateCenter
{
	//控件使能
	virtual VOID OnEventEnableControls(BOOL bEnabled)=NULL;	
	//构造数据
	virtual WORD ConstructDataPacket(BYTE cbBuffer[], WORD wBufferSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

//管理任务
class CMissionMaster : public CWnd, public CMissionItem
{
	//存储变量
protected:
	WORD								m_wDataSize;							//数据大小
	BYTE								m_cbDataBuffer[SOCKET_PACKET];			//数据缓冲

	//接口变量
protected:
	IManagerOperateCenter *				m_pIManagerOperateCenter;				//机器接口

	//静态变量
protected:
	static CMissionMaster *				m_pMissionMaster;						//对象指针

	//函数定义
public:
	//构造函数
	CMissionMaster();
	//析构函数
	virtual ~CMissionMaster();

	//重载函数
public:
	//连接事件
	virtual bool OnEventMissionLink(LONG nErrorCode);
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//读取事件
	virtual bool OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize);

	//设置函数
public:
	//设置接口
	VOID SetManagerOperateCenter(IManagerOperateCenter * pIManagerOperateCenter);

	//功能函数
public:	
	//激活任务
	bool ActiveMission();	

	//静态函数
public:
	//获取对象
	static CMissionMaster * GetInstance() { return m_pMissionMaster; }

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif