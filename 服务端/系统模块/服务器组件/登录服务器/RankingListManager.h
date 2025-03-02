#ifndef RANKING_LIST_MANAGER_HEAD
#define RANKING_LIST_MANAGER_HEAD
#pragma once

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////////////////////
//��������
class CRankingListItem;
class CRankingListManager;

//////////////////////////////////////////////////////////////////////////////////////////
//�ṹ����
struct tagListUserItem
{
	DWORD							dwUserID;								//�û���ʶ
	WORD							wRankID;								//������ʶ
};

//////////////////////////////////////////////////////////////////////////////////////////
//���Ͷ���
typedef CWHStockData<DWORD, tagListUserItem>	CStockListUserItem;
typedef CWHStockData<BYTE, CRankingListItem>	CStockRankingListItem;

//////////////////////////////////////////////////////////////////////////////////////////

//������
class CRankingListItem
{
	friend class CRankingListManager;

	//��������
protected:
	BYTE							m_cbListType;							//������
	TCHAR							m_szRankingList[8192];					//������
	CStockListUserItem				m_StockListUserItem;					//�û��ֿ�

	//��������
public:
	//���캯��
	CRankingListItem();
	//��������
	virtual~CRankingListItem();

	//��Ϣ����
public:
	//������
	LPCTSTR GetRankingList() { return m_szRankingList; }

	//������
public:
	//�����
	bool ConstructRankingList(BYTE cbListType, LPCTSTR pszRankingList);

	//���ܺ���
public:	
	//�����û�
	tagListUserItem* SearchListUser(DWORD dwUserID);

	//��������
private:
	//������
	bool ParseRankingList();
};

//////////////////////////////////////////////////////////////////////////////////////////

//�񵥹���
class CRankingListManager
{
	//�������
protected:
	CStockRankingListItem			m_StockRankingList;						//�񵥲ֿ�

	//��������
public:
	//���캯��
	CRankingListManager();
	//��������
	virtual~CRankingListManager();

	//�񵥹���
public:
	//ɾ����
	VOID DeleteRankingList(BYTE cbListType);
	//��Ӱ�
	bool AppendRankingList(BYTE cbListType, LPCTSTR pszRankingList);	

	//���ܺ���
public:
	//���Ұ�
	CRankingListItem* SearchRankingList(BYTE cbListType);

	//��������
public:
	//��������
	VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////////////////////

#endif

