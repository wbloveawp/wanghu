#ifndef GLOBAL_INFO_MANAGER_HEAD_FILE
#define GLOBAL_INFO_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"

//////////////////////////////////////////////////////////////////////////

//��˵��
class CGlobalClubItem;

//�ֿⶨ��
typedef CWHStockData<WORD, CGlobalClubItem> CStockClubItem;

//////////////////////////////////////////////////////////////////////////

//��������
class CGlobalClubItem : public CGlobalServiceItem
{
	//��Ԫ����
	friend class CGlobalInfoManager;

	//��������
protected:
	//���캯��
	CGlobalClubItem();
	//��������
	virtual ~CGlobalClubItem();	
};

//////////////////////////////////////////////////////////////////////////

//ȫ����Ϣ
class CGlobalInfoManager : public CGlobalItemManager
{
	//��������
protected:
	CStockClubItem					m_StockClubItem;					//����ֿ�

	//��������
public:
	//���캯��
	CGlobalInfoManager();
	//��������
	virtual ~CGlobalInfoManager();

	//������
public:
	//��������
	VOID ResetData();

	//�������
public:
	//ɾ������
	bool DeleteClubItem(WORD wServiceID);
	//�������
	CGlobalClubItem * ActiveClubItem(DWORD dwSocketID, tagServiceItem & ServiceItem);

	//���Һ���
public:
	//Ѱ�Ҳ���
	CGlobalClubItem* SearchClubItem(WORD wServiceID);

	//ö�ٺ���
public:
	//ö������
	CGlobalClubItem* EnumClubItem(CGlobalClubItem* pGlobalClubItem);
};

//////////////////////////////////////////////////////////////////////////

#endif