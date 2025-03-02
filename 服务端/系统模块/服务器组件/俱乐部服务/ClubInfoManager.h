#ifndef CLUB_INFO_MANAGER_HEAD_FILE
#define CLUB_INFO_MANAGER_HEAD_FILE

#pragma once

//�����ļ�
#include "ClubServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//����״̬
#define CLUB_STATE_OFFLINE					0							//����״̬
#define CLUB_STATE_LOADING					1							//����״̬
#define CLUB_STATE_ONLINE					2							//����״̬

//////////////////////////////////////////////////////////////////////////////////
//��������
struct tagClubTableItem
{
	tagBattleTableItem					TableItem;						//��������
	tagBattleTableParamEx				TableParam;						//���Ӳ���
};

//Լս�淨
struct tagClubBattleConfigEx : public tagClubBattleConfig
{
	WORD							wTableCount;						//��������	
};

//////////////////////////////////////////////////////////////////////////////////

//��˵��
class CClubInfoItem;
class CClubInfoManager;

//���鶨��
typedef CWHArray<tagSocketInfo> CSocketInfoArray;
typedef CWHArray<CClubInfoItem *> CClubInfoItemArray;
typedef CWHArray<tagClubTableItem*> CClubTableArray;
typedef CWHArray<tagClubMemberInfo *> CClubMemberArray;
typedef CWHArray<tagClubKindSetting*> CKindSettingArray;
typedef CWHArray<tagClubApplyMessage *> CApplyMessageArray;
typedef CWHArray<tagClubBattleConfigEx*> CBattleConfigArray;

//�ֿⶨ��
typedef CWHStockData<DWORD, CClubInfoItem> CStockClubID;

//�ֵ䶨��
typedef CMap<DWORD, DWORD, tagClubTableItem*, tagClubTableItem*> CMapTableID;
typedef CMap<DWORD, DWORD, tagClubMemberInfo*, tagClubMemberInfo*> CMapMemberID;
typedef CMap<DWORD, DWORD, tagClubApplyMessage*, tagClubApplyMessage*> CMapMessageID;

//////////////////////////////////////////////////////////////////////////////////
//��������
class CClubInfoItem
{
	//��Ԫ����
	friend class CClubInfoManager;

	//��������
protected:
	tagClubItem							m_ClubItem;							//��������

	//״̬����
protected:
	bool								m_bUnloadFlag;						//ж�ر�־
	WORD								m_wOnlineCount;						//��������
	BYTE								m_cbClubState;						//����״̬
	DWORD								m_dwActiveTime;						//��Ծʱ��
	
	//�������
protected:
	CMapTableID							m_MapTableID;						//�����ֵ�
	CMapMemberID						m_MapMemberID;						//��Ա�ֵ�
	CMapMessageID						m_MapMessageID;						//��Ϣ�ֵ�
	CSocketInfoArray					m_SocketInfoArray;					//��������	
	CKindSettingArray					m_KindSettingActive;				//��������
	CBattleConfigArray					m_BattleConfigActive;				//�淨����	

	//��̬�洢
protected:
	static CClubTableArray				m_ClubTableBuffer;					//��������
	static CClubMemberArray				m_ClubMemberBuffer;					//��Ա����
	static CKindSettingArray			m_KindSettingBuffer;				//��������
	static CBattleConfigArray			m_BattleConfigBuffer;				//�淨����
	static CApplyMessageArray			m_ApplyMessageBuffer;				//��Ϣ����

	//��������
protected:
	//���캯��
	CClubInfoItem();
	//��������
	virtual ~CClubInfoItem();

	//������Ϣ
public:
	//�����ʶ
	DWORD GetClubID() { return m_ClubItem.ClubProperty.dwClubID; }

	//״̬��Ϣ
public:
	//��ȡ״̬
	BYTE GetClubState() { return m_cbClubState; }
	//����״̬
	VOID SetClubState(BYTE cbClubState) { m_cbClubState = cbClubState; }

	//ж�ر�־
public:
	//��ȡ��־
	bool GetUnloadFlag() { return m_bUnloadFlag; }
	//����״̬
	VOID SetUnloadFlag(bool bUnloadFlag) { m_bUnloadFlag = bUnloadFlag; }

	//��Ծʱ��
public:
	//��ȡʱ��
	DWORD GetActiveTime() { return m_dwActiveTime; }
	//����ʱ��
	VOID SetActiveTime(DWORD dwActiveTime) { m_dwActiveTime = dwActiveTime; }

	//��������
public:
	//��������
	VOID IncreaseOnline() { m_wOnlineCount++; }
	//��������
	VOID DecreaseOnline() { if (m_wOnlineCount > 0) m_wOnlineCount--; }
	//��ȡ����
	WORD GetOnlineCount() { return m_wOnlineCount; }	
	//��������
	VOID SetOnlineCount(WORD wOnlineCount) { m_wOnlineCount = wOnlineCount; }

	//��������
public:
	//ö������
	tagSocketInfo* EnumEnterRequest(WORD wIndex);
	//��������
	VOID CacheEnterRequest(tagSocketInfo& SocketInfo);
	//�Ƴ�����
	VOID RemoveEnterRequest(tagSocketInfo& SocketInfo);	

	//��������
public:
	//��������
	tagClubItem * GetClubItem() { return &m_ClubItem; }	
	//�������
	tagClubRules * GetClubRules() { return &m_ClubItem.ClubRules; }
	//���繫��
	tagClubNotice * GetClubNotice() { return &m_ClubItem.ClubNotice; }
	//��������
	tagClubProperty * GetClubProperty() { return &m_ClubItem.ClubProperty; }	
	
	//�����Ա
public:	
	//�Ƴ���Ա
	VOID RemoveMember();
	//�Ƴ���Ա
	bool RemoveMember(DWORD dwMemberID);
	//��ӳ�Ա
	bool AppendMember(tagClubMemberInfo & ClubMember);
	//ö�ٳ�Ա
	tagClubMemberInfo * EnumMember(POSITION & Position);
	//���ҳ�Ա
	tagClubMemberInfo * SearchMember(DWORD dwMemberID);

	//Լս�淨
public:
	//�Ƴ��淨
	VOID RemoveConfig();
	//�Ƴ��淨
	bool RemoveConfig(DWORD dwConfigID);
	//����淨
	bool AppendConfig(tagClubBattleConfig& BattleConfig);
	//ö�ٳ�Ա
	tagClubBattleConfigEx* EnumConfig(INT nIndex);
	//���ҳ�Ա
	tagClubBattleConfigEx* SearchConfig(DWORD dwConfigID);

	//��������
public:
	//�Ƴ�����
	VOID RemoveKindSetting();
	//�Ƴ�����
	VOID RemoveKindSetting(WORD wKindID);
	//�������
	bool AppendKindSetting(tagClubKindSetting& KindSetting);
	//ö������
	tagClubKindSetting * EnumKindSetting(INT nIndex);
	//��������
	tagClubKindSetting * SearchKindSetting(WORD wKindID);

	//���ӹ���
public:
	//�Ƴ�����
	VOID RemoveClubTable();
	//�Ƴ�����
	VOID RemoveClubTable(DWORD dwBattleNum);
	//�������
	bool AppendClubTable(tagClubTableItem& TableItem);	
	//ö������
	tagClubTableItem* EnumClubTable(POSITION& Position);
	//��������
	tagClubTableItem* SearchClubTable(DWORD dwBattleNum);

	//������Ϣ
public:
	//�Ƴ���Ϣ
	VOID RemoveMessage();
	//�Ƴ���Ϣ
	bool RemoveMessage (DWORD dwMessageID);
	//�����Ϣ
	bool AppendMessage(tagClubApplyMessage & ApplyMessage);
	//ö����Ϣ
	tagClubApplyMessage * EnumMessage(POSITION & Position);
	//������Ϣ
	tagClubApplyMessage * SearchMessage(DWORD dwMessageID);

	//��������
protected:
	//���ö���
	void ResetClubItem();
	//��������
	INT SearchEnterRequest(tagSocketInfo& SocketInfo);

	//��̬����
protected:
	//��������
	static VOID CleanStaticData();

	//��̬����
private:
	//��������
	static tagClubTableItem * ActiveClubTable();
	//�Ƴ�����
	static VOID RemoveClubTable(tagClubTableItem * pClubTable);
	//������Ա
	static tagClubMemberInfo * ActiveClubMember();
	//�Ƴ���Ա
	static VOID RemoveClubMember(tagClubMemberInfo * pClubMember);
	//�����淨
	static tagClubBattleConfigEx * ActiveBattleConfig();
	//�Ƴ��淨
	static VOID RemoveBattleConfig(tagClubBattleConfigEx* pIMBattleConfig);
	//��������
	static tagClubKindSetting * ActiveKindSetting();
	//�Ƴ�����
	static VOID RemoveKindSetting (tagClubKindSetting* pClubKindSetting);
	//������Ϣ
	static tagClubApplyMessage * ActiveApplyMessage();
	//�Ƴ���Ϣ
	static VOID RemoveApplyMessage(tagClubApplyMessage * pIMApplyMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//���������
class CClubInfoManager
{
	//�û�����
protected:
	CStockClubID						m_StockClubID;					//����ֿ�

	//��������
public:
	//���캯��
	CClubInfoManager();
	//��������
	virtual ~CClubInfoManager();

	//���Һ���
public:	
	//���Ҳ���
	virtual CClubInfoItem * SearchClubItem(DWORD dwClubID);
	//ö�ٲ���
	virtual CClubInfoItem * EnumClubItem(CClubInfoItem* pClubInfoItem);

	//ͳ�ƺ���
public:
	//��������
	virtual DWORD GetClubItemCount() { return (DWORD)m_StockClubID.GetItemCount(); }

	//������
public:
	//�Ƴ�����
	virtual VOID RemoveClubItem();
	//�Ƴ�����
	virtual VOID RemoveClubItem(DWORD dwClubID);
	//�������
	virtual CClubInfoItem* ActiveClubItem(tagClubItem & ClubItem);
};

//////////////////////////////////////////////////////////////////////////////////
#endif