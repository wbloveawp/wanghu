#ifndef STOCK_DATA_MANAGER_HEAD_FILE
#define STOCK_DATA_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////////////

//��Ϣ����
typedef CWHArray<tagStockItem*> CStockItemArray;

//////////////////////////////////////////////////////////////////////////////////

//ȫ������
class CStockDataManager
{
	//������Ϣ
public:
	CStockItemArray					m_StockItemActive;					//�������
	CStockItemArray					m_StockItemBuffer;					//�������

	//��̬����
protected:
	static CStockDataManager*		m_pStockDataManager;				//����ָ��

	//��������
public:
	//���캯��
	CStockDataManager();
	//��������
	virtual ~CStockDataManager();

	//������
public:
	//��������
	VOID ResetGlobalData();
	//ɾ������
	VOID DeleteGlobalData();

	//�����Ϣ
public:
	//�����Ŀ
	WORD GetStockCount() { return (WORD)m_StockItemActive.GetCount(); }

	//�������
public:
	//ɾ�����
	VOID DeleteStockItem(WORD wStockID);
	//��ӿ��
	tagStockItem* AppendStockItem(tagStockItem& StockItem);	

	//�������
public:
	//ö�ٿ��
	tagStockItem * EnumStockItem(WORD wStockIndex);
	//Ѱ�ҿ��
	tagStockItem * SearchStockItem(WORD wStockID);
	//Ѱ�ҿ��
	tagStockItem * SearchStockItem(WORD wStockKind, WORD wKindID, WORD wStockSubtype);	

	//��������
private:
	//�������
	tagStockItem* ActiveStockItem();	

	//��̬����
public:
	//��ȡ����
	static CStockDataManager* GetInstance() { return m_pStockDataManager; }
};

//////////////////////////////////////////////////////////////////////////////////

#endif