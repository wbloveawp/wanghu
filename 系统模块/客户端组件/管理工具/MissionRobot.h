#ifndef MISSION_ROBOT_HEAD_FILE
#define MISSION_ROBOT_HEAD_FILE
#pragma once

#include "StdAfx.h"
#include "MissionManager.h"

//////////////////////////////////////////////////////////////////////////////////
//动作枚举
enum emRobotAction
{
	RAction_NULL=0,
	RAction_QueryParameter,			//获取参数
	RAction_AppendParameter,		//添加参数
	RAction_ModifyParameter,		//修改参数
	RAction_DeleteParameter			//删除参数
};

//////////////////////////////////////////////////////////////////////////////////
//接口定义
interface IRobotOperateCenter
{
	//控件使能
	virtual VOID OnEventEnableControls(BOOL bEnabled)=NULL;
	//机器参数
	virtual VOID OnEventRobotOperateResult(WORD wSubCommdID,WORD wParameterCount, tagRobotParameter * pRobotParameter)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

//机器任务
class CMissionRobot : public CWnd, public CMissionItem
{
	//存储变量
protected:
	WORD								m_wDataSize;							//数据大小
	BYTE								m_cbDataBuffer[SOCKET_PACKET];			//数据缓冲

	//变量定义
protected:
	emRobotAction						m_RobotAction;							//机器动作	

	//接口变量
protected:
	IRobotOperateCenter *				m_pIRobotOperateCenter;					//机器接口

	//静态变量
protected:
	static CMissionRobot *				m_pMissionRobot;						//对象指针

	//函数定义
public:
	//构造函数
	CMissionRobot();
	//析构函数
	virtual ~CMissionRobot();

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
	//设置接口
	VOID SetRobotOperateCenter(IRobotOperateCenter * pIRobotOperateCenter);
	//获取参数
	VOID GetRobotParameter(WORD wServerID);
	//删除参数
	VOID DeleteRobotParameter(WORD wServerID, DWORD dwBatchID);
	//修改参数
	VOID ModifyRobotParameter(WORD wServerID, tagRobotParameter *  pRobotParameter);
	//添加参数
	VOID AddRobotParameter(WORD wServerID, tagRobotParameter *  pRobotParameter);

	//静态函数
public:
	//获取对象
	static CMissionRobot * GetInstance() { return m_pMissionRobot; }

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif