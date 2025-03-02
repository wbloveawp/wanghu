#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//��������
class CRobotUserItemSink : public IRobotUserItemSink
{
	//��Ϸ����
protected:
	WORD							m_wBankerUser;						//ׯ���û�
	LONGLONG						m_lMinAddScore;						//��С��ע
	LONGLONG						m_lTurnMaxScore;					//�����ע
	LONGLONG						m_lTurnMinScore;					//��С��ע
	LONGLONG						m_lUserInvest;						//�û��±�
	LONGLONG						m_lUserScore;						//�û�����
	LONGLONG						m_lCellScore;						//��Ԫ��ע
	LONGLONG						m_lTotalAddBet;						//����ע
	BYTE							m_cbUserStatus[GAME_PLAYER];		//�û�״̬
	LONGLONG						m_lMaxBoboMultiple;					//��������
	bool							m_bGiveUp;							//����

	//�˿˱���
protected:
	BYTE							m_cbHandCardCount[GAME_PLAYER];			//��������
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//�����˿�

	//�ؼ�����
protected:
	CGameLogic						m_GameLogic;						//��Ϸ�߼�
	IRobotUserItem *				m_pITestUserItem;					//�û��ӿ�

	//��������
public:
	//���캯��
	CRobotUserItemSink();
	//��������
	virtual ~CRobotUserItemSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ƽӿ�
public:
	//��ʼ�ӿ�
	virtual bool InitUserItemSink(IUnknownEx * pIUnknownEx);
	//���ýӿ�
	virtual bool ResetUserItemSink();

	//��Ϸ�¼�
public:
	//ʱ����Ϣ
	virtual bool __cdecl OnEventTimer(UINT nTimerID);
	//��Ϸ��Ϣ
	virtual bool __cdecl OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	virtual bool __cdecl OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//������Ϣ
	virtual bool __cdecl OnEventSendGameScene(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//�û��¼�
public:
	//�û�����
	virtual VOID __cdecl OnEventUserEnter(IRobotUserItem * pITestUserItem, bool bLookonUser);
	//�û��뿪
	virtual VOID __cdecl OnEventUserLeave(IRobotUserItem * pITestUserItem, bool bLookonUser);
	//�û�����
	virtual VOID __cdecl OnEventUserScore(IRobotUserItem * pITestUserItem, bool bLookonUser);
	//�û�״̬
	virtual VOID __cdecl OnEventUserStatus(IRobotUserItem * pITestUserItem, bool bLookonUser);
	//�û���λ
	virtual VOID __cdecl OnEventUserSegment(IRobotUserItem * pITestUserItem, bool bLookonUser);

	//��Ϣ����
protected:
	//��Ϸ��ʼ
	bool OnSubGameStart(VOID * pData, WORD wDataSize);
	//�û�����
	bool OnUserInvest(VOID * pData, WORD wDataSize);
	//�û���ע
	bool OnAddScore(VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnSubSendCard(VOID * pData, WORD wDataSize);


//��������
protected:
	//����д��־
	VOID WriteControlLog(string & str);
};

//////////////////////////////////////////////////////////////////////////

#endif