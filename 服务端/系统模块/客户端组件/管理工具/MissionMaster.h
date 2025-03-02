#ifndef MISSION_MASTER_HEAD_FILE
#define MISSION_MASTER_HEAD_FILE
#pragma once

#include "StdAfx.h"
#include "MissionManager.h"

//////////////////////////////////////////////////////////////////////////////////
//����ö��
//enum emRobotAction
//{
//	RAction_NULL=0,
//	RAction_QueryParameter,			//��ȡ����
//	RAction_AppendParameter,			//��Ӳ���
//	RAction_ModifyCustom,			//�޸Ĳ���
//	RAction_ModifyParameter,		//�޸Ĳ���
//	RAction_DeleteParameter			//ɾ������
//};

//////////////////////////////////////////////////////////////////////////////////
//�ӿڶ���
interface IManagerOperateCenter
{
	//�ؼ�ʹ��
	virtual VOID OnEventEnableControls(BOOL bEnabled)=NULL;	
	//��������
	virtual WORD ConstructDataPacket(BYTE cbBuffer[], WORD wBufferSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

//��������
class CMissionMaster : public CWnd, public CMissionItem
{
	//�洢����
protected:
	WORD								m_wDataSize;							//���ݴ�С
	BYTE								m_cbDataBuffer[SOCKET_PACKET];			//���ݻ���

	//�ӿڱ���
protected:
	IManagerOperateCenter *				m_pIManagerOperateCenter;				//�����ӿ�

	//��̬����
protected:
	static CMissionMaster *				m_pMissionMaster;						//����ָ��

	//��������
public:
	//���캯��
	CMissionMaster();
	//��������
	virtual ~CMissionMaster();

	//���غ���
public:
	//�����¼�
	virtual bool OnEventMissionLink(LONG nErrorCode);
	//�ر��¼�
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize);

	//���ú���
public:
	//���ýӿ�
	VOID SetManagerOperateCenter(IManagerOperateCenter * pIManagerOperateCenter);

	//���ܺ���
public:	
	//��������
	bool ActiveMission();	

	//��̬����
public:
	//��ȡ����
	static CMissionMaster * GetInstance() { return m_pMissionMaster; }

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif