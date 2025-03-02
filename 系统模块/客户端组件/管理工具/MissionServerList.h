#ifndef MISSION_SERVER_LIST_HEAD_FILE
#define MISSION_SERVER_LIST_HEAD_FILE

#pragma once

#include "StdAfx.h"
#include "MissionManager.h"

//////////////////////////////////////////////////////////////////////////

//����˵��
typedef CMap<WORD,WORD,WORD,WORD>	CGameKindIDMap;						//���ͱ�ʶ

//////////////////////////////////////////////////////////////////////////

//�б�����
class CMissionList : public CWnd, public CMissionItem
{
	//��������
protected:
	bool							m_bOnLineMission;					//��������
	CGameKindIDMap					m_GameKindIDMission;				//��Ϸ��ʶ
	CGameKindIDMap					m_GameKindIDPending;				//������ʶ

	//��̬����
protected:
	static CMissionList *		m_pMissionServerList;				//����ָ��

	//��������
public:
	//���캯��
	CMissionList();
	//��������
	virtual ~CMissionList();

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
	//��������
	VOID UpdateOnLineInfo();
	//��������
	VOID UpdateServerInfo(WORD wKindID);
	//���ط���
	VOID LoadServerInfo();

	//��̬����
public:
	//��ȡ����
	static CMissionList * GetInstance() { return m_pMissionServerList; }

	//��Ϣӳ��
protected:
	//ʱ����Ϣ
	VOID OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif