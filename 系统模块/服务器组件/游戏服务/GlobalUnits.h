#ifndef GLOBAL_UNITES_HEAD_FILE
#define GLOBAL_UNITES_HEAD_FILE

#pragma once

#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//ȫ�ֵ�Ԫ
class CGlobalUnits
{
	//�������
protected:
	bool							m_bAllowWisper;						//˽�ı�־
	bool							m_bAllowRoomChat;					//�����־
	bool							m_bAllowGameChat;					//�����־
	bool							m_bForbidPlayChat;					//ͬ����Ϸ����
	bool							m_bMemberCanChat;					//��Ա��������

	//�������
protected:
	bool							m_bAllowEnterRoom;					//�����־
	bool							m_bAllowEnterGame;					//�����־
	bool							m_bForbidQueryUser;					//��ѯ��־			
	bool							m_bCloseWhilePlayer;				//����ʱ�رշ���
	bool							m_bForbidLookon;					//��ֹ�Թ�
	BYTE							m_cbAllowLookOnChat;				//�����Թ�����0��ֹ����1�����Ա����2��������
	bool							m_bAllowRobotAttend;				//���������	
	bool							m_bAllowRobotSimulate;				//������ռ��
		
	//���ֿ���
protected:
	SCORE							m_lMaxJoinScore;					//��߽������

	//��̬����
protected:
	static CGlobalUnits *			m_pGlobalUnits;						//����ָ��


	//��������
public:
	//���캯��
	CGlobalUnits();
	//��������
	virtual ~CGlobalUnits();

	//�������
public:
	//˽��״̬
	bool IsAllowWisper() { return m_bAllowWisper; }
	//����״̬
	bool IsAllowRoomChat() { return m_bAllowRoomChat; }
	//����״̬
	bool IsAllowGameChat() { return m_bAllowGameChat; }
	//����״̬
	bool IsForbidPlayChat() { return m_bForbidPlayChat; }
	//��Ա����
	bool IsMemberCanChat() { return m_bMemberCanChat; }
	//˽��״̬
	VOID SetAllowWisper(bool bAllowWisper) { m_bAllowWisper=bAllowWisper; }
	//����״̬
	VOID SetAllowRoomChat(bool bAllowRoomChat) { m_bAllowRoomChat=bAllowRoomChat; }
	//����״̬
	VOID SetAllowGameChat(bool bAllowGameChat) { m_bAllowGameChat=bAllowGameChat; }
	//����״̬
	VOID SetForbidPlayChat(bool bForbidPlayChat) { m_bForbidPlayChat=bForbidPlayChat; }
	//��Ա����
	VOID SetMemberCanChat(bool bMemCanChat) { m_bMemberCanChat = bMemCanChat; }
	//�Թ�����
	BYTE GetAllowLookOnChat(){ return m_cbAllowLookOnChat;}

	//�������
public:
	//����״̬
	bool IsAllowEnterRoom() { return m_bAllowEnterRoom; }
	//����״̬
	bool IsAllowEnterGame() { return m_bAllowEnterGame; }
	//�Ƿ��ֹ��ѯ
	bool IsForbidQueryUser() { return m_bForbidQueryUser; }
	//�Ƿ����������
	bool IsAllowRobotAttend() { return m_bAllowRobotAttend; }
	//�Ƿ����������
	bool IsAllowRobotSimulate() { return m_bAllowRobotSimulate; }
	//�Ƿ��Զ��رշ���
	bool IsCloseWhileNoplayer() { return m_bCloseWhilePlayer; }
	//��ֹ�Թ�
	bool IsForbidLookon() { return m_bForbidLookon; }
	//��ȡ��߽������
	SCORE GetMaxJoinScore() { return m_lMaxJoinScore; }
	//����״̬
	VOID SetAllowEnterRoom(bool bAllowEnterRoom) { m_bAllowEnterRoom=bAllowEnterRoom; }
	//����״̬
	VOID SetAllowEnterGame(bool bAllowEnterGame) { m_bAllowEnterGame=bAllowEnterGame; }
	//������߽������
	VOID SetMaxJoinScore( SCORE lMaxJoinScore ) { m_lMaxJoinScore = lMaxJoinScore; }
	//���ò�ѯ
	VOID SetForbidQueryUser( bool bForbidQuery ) { m_bForbidQueryUser = bForbidQuery; }
	//���������
	VOID SetAllowRobotAttend(bool bAllowRobotAttend) { m_bAllowRobotAttend=bAllowRobotAttend; }	
	//������ռ��
	VOID SetAllowRobotSimulate(bool bAllowRobotSimulate) { m_bAllowRobotSimulate=bAllowRobotSimulate; }	
	//���÷����Զ��ر�
	VOID SetCloseWhileNoplayer(bool bCloseWhileNoplayer) { m_bCloseWhilePlayer = bCloseWhileNoplayer; }
	//��ֹ�Թ�
	VOID SetForbidLookon(bool bForbidLookon) { m_bForbidLookon = bForbidLookon; }
	//�����Թ�����
	VOID SetAllowLookOnChat(BYTE cbAllowLookOnChat) {m_cbAllowLookOnChat=cbAllowLookOnChat;}
};

//////////////////////////////////////////////////////////////////////////

//����˵��
extern CGlobalUnits					g_GlobalUnits;						//ȫ�ֵ�Ԫ

//////////////////////////////////////////////////////////////////////////

#endif