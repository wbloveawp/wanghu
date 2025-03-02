#ifndef GLOBAL_ITEM_MANAGER_HEAD_FILE
#define GLOBAL_ITEM_MANAGER_HEAD_FILE

#pragma once

#include "GlobalInfoItem.h"
#include "ServiceFrameHead.h"

//////////////////////////////////////////////////////////////////////////

//��Ϣ����
class SERVICE_FRAME_CLASS CGlobalItemManager
{
	//��������
protected:
	CStockUserItem					m_StockUserItem;					//�û�����
	CStockGateItem					m_StockGateItem;					//��������
	CStockServerItem				m_StockServerItem;					//��������

	//��������
public:
	//���캯��
	CGlobalItemManager();
	//��������
	virtual ~CGlobalItemManager();

	//������
public:
	//�û���Ŀ
	DWORD GetUserItemCount() { return (DWORD)m_StockUserItem.GetItemCount(); }
	//������Ŀ
	DWORD GetGateItemCount() { return (DWORD)m_StockGateItem.GetItemCount(); }
	//������Ŀ
	DWORD GetServerItemCount() { return (DWORD)m_StockServerItem.GetItemCount(); }

	//�û�����
public:
	//ɾ���û�
	bool DeleteUserItem(DWORD dwUserID);
	//�����û�
	CGlobalUserItem * ActiveUserItem(tagGlobalUserInfo & GlobalUserInfo);

	//���ع���
public:
	//ɾ������
	bool DeleteGateItem(WORD wGateID);
	//��������
	CGlobalGateItem * ActiveGateItem(DWORD dwSocketID, tagServiceItem & GateItem);	

	//�������
public:
	//ɾ������
	bool DeleteServerItem(WORD wServerID);
	//�����
	CGlobalServerItem* ActiveServerItem(DWORD dwSocketID, tagGameServer& GameServer);	

	//���Һ���
public:
	//Ѱ������
	CGlobalGateItem* SearchGateItem(WORD wGateID);
	//Ѱ���û�
	CGlobalUserItem * SearchUserItem(DWORD dwUserID);	
	//Ѱ�ҷ���
	CGlobalServerItem* SearchServerItem(WORD wServerID);

	//ö�ٺ���
public:
	//ö���û�
	CGlobalUserItem * EnumUserItem(CGlobalUserItem* pGlobalUserItem);
	//ö������
	CGlobalGateItem * EnumGateItem(CGlobalGateItem* pGlobalGateItem);
	//ö�ٷ���
	CGlobalServerItem * EnumServerItem(CGlobalServerItem* pGlobalServerItem);

	//��������
public:
	//��������
	virtual VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////

#endif