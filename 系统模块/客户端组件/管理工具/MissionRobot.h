#ifndef MISSION_ROBOT_HEAD_FILE
#define MISSION_ROBOT_HEAD_FILE
#pragma once

#include "StdAfx.h"
#include "MissionManager.h"

//////////////////////////////////////////////////////////////////////////////////
//����ö��
enum emRobotAction
{
	RAction_NULL=0,
	RAction_QueryParameter,			//��ȡ����
	RAction_AppendParameter,		//��Ӳ���
	RAction_ModifyParameter,		//�޸Ĳ���
	RAction_DeleteParameter			//ɾ������
};

//////////////////////////////////////////////////////////////////////////////////
//�ӿڶ���
interface IRobotOperateCenter
{
	//�ؼ�ʹ��
	virtual VOID OnEventEnableControls(BOOL bEnabled)=NULL;
	//��������
	virtual VOID OnEventRobotOperateResult(WORD wSubCommdID,WORD wParameterCount, tagRobotParameter * pRobotParameter)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

//��������
class CMissionRobot : public CWnd, public CMissionItem
{
	//�洢����
protected:
	WORD								m_wDataSize;							//���ݴ�С
	BYTE								m_cbDataBuffer[SOCKET_PACKET];			//���ݻ���

	//��������
protected:
	emRobotAction						m_RobotAction;							//��������	

	//�ӿڱ���
protected:
	IRobotOperateCenter *				m_pIRobotOperateCenter;					//�����ӿ�

	//��̬����
protected:
	static CMissionRobot *				m_pMissionRobot;						//����ָ��

	//��������
public:
	//���캯��
	CMissionRobot();
	//��������
	virtual ~CMissionRobot();

	//���غ���
public:
	//�����¼�
	virtual bool OnEventMissionLink(INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize);

	//���ܺ���
public:
	//���ýӿ�
	VOID SetRobotOperateCenter(IRobotOperateCenter * pIRobotOperateCenter);
	//��ȡ����
	VOID GetRobotParameter(WORD wServerID);
	//ɾ������
	VOID DeleteRobotParameter(WORD wServerID, DWORD dwBatchID);
	//�޸Ĳ���
	VOID ModifyRobotParameter(WORD wServerID, tagRobotParameter *  pRobotParameter);
	//��Ӳ���
	VOID AddRobotParameter(WORD wServerID, tagRobotParameter *  pRobotParameter);

	//��̬����
public:
	//��ȡ����
	static CMissionRobot * GetInstance() { return m_pMissionRobot; }

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif