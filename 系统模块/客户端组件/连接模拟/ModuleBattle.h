#ifndef MODULE_BATTLE_HEAD_FILE
#define MODULE_BATTLE_HEAD_FILE
#pragma once

//�����ļ�
#include "SimulateLinkHead.h"
#include "VirtualLinkManager.h"

/////////////////////////////////////////////////////////////////////////////////////

//��Ϣ����
#define WM_BATTLE_LINK_SHUT			(WM_USER+1500)					//���ӹر�
#define WM_BATTLE_LINK_FAILURE		(WM_USER+1501)					//����ʧ��
#define WM_BATTLE_LOGON_FINISH		(WM_USER+1502)					//��¼���
#define WM_BATTLE_LOGON_FAILURE		(WM_USER+1503)					//��¼ʧ��

/////////////////////////////////////////////////////////////////////////////////////
//���Ͷ���
typedef CWHArray<tagBattleBaseOption>	CArrayBaseOption;

/////////////////////////////////////////////////////////////////////////////////////

//Լսģ��
class CModuleBattle : public IModuleItem, public CVirtualLinkItem
{
	//��������
protected:
	tagBattleTableItem			m_BattleTableItem;					//��������
	CArrayBaseOption			m_ArrayBaseOption;					//��������

	//�������
protected:
	tagDataPacket				m_BufferPacket;						//��������

	//�ӿڱ���
protected:
	IModuleItemSink	*			m_pIModuleItemSink;					//�ص��ӿ�		

	//��������
public:
	//��������
	virtual ~CModuleBattle();
	//���캯��
	CModuleBattle(IModuleItemSink* pIModuleItemSink);

	//�ص�����
public:
	//�ر��¼�
	virtual bool OnEventVirtualShut();
	//�����¼�
	virtual bool OnEventVirtualLink(LONG lErrorCode);
	//��ȡ�¼�
	virtual bool OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize);

	//��Ϣ����
public:
	//ʱ�ӱ�ʶ
	virtual DWORD GetTimerStart() { return IDI_MODULE_BATTLE_START; }
	//ʱ�ӷ�Χ
	virtual DWORD GetTimerRange() { return IDI_MODULE_BATTLE_RANGE; }

	//���ܺ���
public:
	//�ر�ʱ��
	virtual bool KillModuleTimer(UINT uTimerID);
	//����ʱ��
	virtual bool SetModuleTimer(UINT uTimerID, UINT nElapse);

	//ʱ���¼�
public:
	//����ʱ��
	virtual bool OnEventTimer(UINT uTimerID, WPARAM wBindParam);

	//�����¼�
protected:
	//��¼����
	bool OnSocketMainBattleService(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//�����¼�
protected:
	//��������
	bool OnSocketSubTableItem(VOID* pData, WORD wDataSize);
	//�����б�
	bool OnSocketSubTableList(VOID* pData, WORD wDataSize);
	//���Ӳ���
	bool OnSocketSubTableParam(VOID* pData, WORD wDataSize);
	//���ӽ�ɢ
	bool OnSocketSubTableDismiss(VOID* pData, WORD wDataSize);
	//�����ɹ�
	bool OnSocketSubOperateSuccess(VOID* pData, WORD wDataSize);
	//����ʧ��
	bool OnSocketSubOperateFailure(VOID* pData, WORD wDataSize);
	//��������
	bool OnSocketSubBattleBaseOption(VOID* pData, WORD wDataSize);			
	//��Ϸ����
	bool OnSocketSubBattleGameOption(VOID* pData, WORD wDataSize);
	//�������
	bool OnSocketSubBattleOptionFinish(VOID* pData, WORD wDataSize);


	//���Ժ���
public:
	//��������
	VOID CreateTable(tagBattleBaseOption* pBattleBaseOption);
	
	//���ܺ���
public:
	//����ģ��
	VOID ResetModule();
	//�������
	bool EnterService();
};

/////////////////////////////////////////////////////////////////////////////////////

#endif
