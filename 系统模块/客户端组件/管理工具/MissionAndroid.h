#ifndef MISSION_ANDROID_HEAD_FILE
#define MISSION_ANDROID_HEAD_FILE
#pragma once

#include "StdAfx.h"
#include "MissionManager.h"

//////////////////////////////////////////////////////////////////////////////////
//����ö��
enum emAndroidAction
{
	AAction_NULL=0,
	AAction_GetParameter,			//��ȡ����
	AAction_AddParameter,			//��Ӳ���
	AAction_ModifyParameter,		//�޸Ĳ���
	AAction_DeleteParameter			//ɾ������
};

//////////////////////////////////////////////////////////////////////////////////
//�ӿڶ���
interface IAndroidOperateCenter
{
	//�ؼ�ʹ��
	virtual VOID OnEventEnableControls(BOOL bEnabled)=NULL;
	//��������
	virtual VOID OnEventAndroidParenter(WORD wSubCommdID,WORD wParameterCount, tagRobotParameter * pAndroidParameter)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

//��������
class CMissionAndroid : public CWnd, public CMissionItem
{
	//�洢����
protected:
	WORD								m_wDataSize;							//���ݴ�С
	BYTE								m_cbDataBuffer[SOCKET_PACKET];			//���ݻ���

	//��������
protected:
	emAndroidAction						m_AndroidAction;						//��������	

	//�ӿڱ���
protected:
	IAndroidOperateCenter *				m_pIAndroidOperateCenter;				//�����ӿ�

	//��̬����
protected:
	static CMissionAndroid *			m_pMissionAndroid;						//����ָ��

	//��������
public:
	//���캯��
	CMissionAndroid();
	//��������
	virtual ~CMissionAndroid();

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
	VOID SetAndroidOperateCenter(IAndroidOperateCenter * pIAndroidOperateCenter);
	//��ȡ����
	VOID GetAndroidParameter(WORD wServerID);
	//ɾ������
	VOID DeleteAndroidParameter(WORD wServerID, DWORD dwBatchID);
	//�޸Ĳ���
	VOID ModifyAndroidParameter(WORD wServerID, tagRobotParameter *  pAndroidParameter);
	//��Ӳ���
	VOID AddAndroidParameter(WORD wServerID, tagRobotParameter *  pAndroidParameter);

	//��̬����
public:
	//��ȡ����
	static CMissionAndroid * GetInstance() { return m_pMissionAndroid; }

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif