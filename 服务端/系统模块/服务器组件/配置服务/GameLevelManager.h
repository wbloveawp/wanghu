#ifndef GAME_LEVEL_MANAGER_HEAD_FILE
#define GAME_LEVEL_MANAGER_HEAD_FILE

#pragma once

#include "ConfigServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//���Ͷ���
typedef CWHArray<tagGameLevelItem> CGameLevelArray;
typedef CWHArray<tagUserLevelItem> CUserLevelArray;
typedef CWHArray<tagRankLevelItem> CRankLevelArray;

//���Ͷ���
typedef CWHStockData<WORD, tagGameLevelItem> CGameLevelStock;
typedef CWHStockData<WORD, tagUserLevelItem> CUserLevelStock;
typedef CWHStockData<WORD, tagRankLevelItem> CRankLevelStock;

//////////////////////////////////////////////////////////////////////////////////

//�ȼ�����
class CONFIG_SERVICE_CLASS CGameLevelManager : public CSingleton<CGameLevelManager>
{	
	//�洢����
protected:
	CGameLevelStock				m_StockGameLevel;						//�ȼ��ֿ�
	CUserLevelStock				m_StockUserLevel;						//�ȼ��ֿ�
	CRankLevelStock				m_StockRankLevel;						//�ȼ��ֿ�

	//��ʱ�洢
public:
	CGameLevelArray				m_ArrayGameLevel;						//��Ϸ�ȼ�
	CUserLevelArray				m_ArrayUserLevel;						//�û��ȼ�
	CRankLevelArray				m_ArrayRankLevel;						//��λ�ȼ�

	//��������
public:
	//���캯��
	CGameLevelManager();
	//��������
	virtual ~CGameLevelManager();

	//������
public:
	//��������
	VOID ResetData();

	//��Ϣ����
public:
	//�ȼ�����
	WORD GetGameLevelCount() { return (WORD)m_StockGameLevel.GetItemCount(); }
	//�ȼ�����
	WORD GetUserLevelCount() { return (WORD)m_StockUserLevel.GetItemCount(); }
	//�ȼ�����
	WORD GetRankLevelCount() { return (WORD)m_StockRankLevel.GetItemCount(); }

	//���ܺ���
public:
	//����ȼ�
	WORD CalcUserOrder(LONG lMemberPoint);
	//����ȼ�
	WORD CalcGameOrder(LONG lExperience);
	//����ȼ�
	WORD CalcRankOrder(LONG lRankScore);

	//�ȼ�����
public:
	//��Ϸ�ȼ�
	bool AppendGameLevel(tagGameLevelItem GameLevelList[], WORD wLevelCount);	
	//�û��ȼ�
	bool AppendUserLevel(tagUserLevelItem UserLevelList[], WORD wLevelCount);	
	//��λ�ȼ�
	bool AppendRankLevel(tagRankLevelItem RankLevelList[], WORD wLevelCount);

	//���Һ���
public:
	//���ҵȼ�
	tagGameLevelItem* SearchGameLevel(WORD wLevelID);
	//���ҵȼ�
	tagUserLevelItem* SearchUserLevel(WORD wLevelID);
	//���ҵȼ�
	tagRankLevelItem* SearchRankLevel(WORD wLevelID);
};

//////////////////////////////////////////////////////////////////////////////////

#define g_GameLevelManager	(CGameLevelManager::getInstance())

//////////////////////////////////////////////////////////////////////////////////

#endif