#ifndef GAME_GOODS_MANAGER_HEAD_FILE
#define GAME_GOODS_MANAGER_HEAD_FILE

#pragma once

#include "ConfigServiceHead.h"


//////////////////////////////////////////////////////////////////////////////////
//��������
class CGameGoodsItem;
class CGameGoodsManager;

//��������
class CGameGoodsMatchProps;

//////////////////////////////////////////////////////////////////////////////////

//���Ͷ���
typedef CWHAutoPtr<CGameGoodsItem> CGameGoodsItemPtr;

//���Ͷ���
typedef CWHArray<tagGoodsItem> CGoodsItemArray;
typedef CWHArray<tagGoodsInfo> CGoodsInfoArray;

//���Ͷ���
typedef CMap<WORD, WORD, CGameGoodsItemPtr, CGameGoodsItemPtr>	CGoodsItemMap;
typedef CMap<WORD, WORD, CGameGoodsItemPtr, CGameGoodsItemPtr>	CGoodsKindMap;

//////////////////////////////////////////////////////////////////////////////////

//��Ʒ����
class CONFIG_SERVICE_CLASS CGameGoodsItem : public CHandleBase
{
	friend class CGameGoodsManager;

	//��������
protected:
	WORD						m_wGoodsID;								//��Ʒ��ʶ
	WORD						m_wGoodsKind;							//��Ʒ����
	bool						m_bHasIndate;							//��Ч�ڱ�ʶ	
	WORD						m_wCanTradeType;						//��������
	WORD						m_wCanStorePlace;						//���λ��
	TCHAR						m_szGoodsName[LEN_GOODS_NAME];			//��Ʒ����		


	//ָ�����
protected:
	CGameGoodsItem*				m_LastGoodsItemPtr;						//��Ʒָ��
	CGameGoodsItem*				m_NextGoodsItemPtr;						//��Ʒָ��

	//��������
public:
	//���캯��
	CGameGoodsItem();
	//��������
	virtual ~CGameGoodsItem();

	//������Ϣ
public:
	//��Ʒ��ʶ
	WORD GetGoodsID() { return m_wGoodsID; }
	//��Ʒ����
	WORD GetGoodsKind() { return m_wGoodsKind; }
	//��Ч��
	bool GetHasIndate() { return m_bHasIndate; }
	//��Ʒ����
	LPCTSTR GetGoodsName() { return m_szGoodsName; }
	//��������
	WORD GetCanTradeType() { return m_wCanTradeType; }
	//���λ��
	WORD GetCanStorePlace() { return m_wCanStorePlace; }

	//������Ϣ
public:
	//��Ʒָ��	
	template <class TYPE> TYPE GetLastGoodsItemPtr();
	//��Ʒָ��	
	template <class TYPE> TYPE GetNextGoodsItemPtr();

	//���ܺ���
public:
	//������Ʒ
	bool ConstructGoods(tagGoodsInfo* GoodsInfo);

	//�̳к���
public:
	//��������
	virtual bool ParseGoodsOption(LPCTSTR pszGoodsOption);
};

//////////////////////////////////////////////////////////////////////////////////

//��Ʒ����
class CONFIG_SERVICE_CLASS CGameGoodsManager : public CSingleton<CGameGoodsManager>
{	
	//�洢����
protected:
	CGoodsItemMap				m_MapGoodsItem;						//��Ʒ����
	CGoodsKindMap				m_MapGoodsKind;						//��Ʒ����

	//��ʱ�洢
public:
	CGoodsInfoArray				m_ArrayGoodsInfo;					//��Ʒ��Ϣ

	//��������
public:
	//���캯��
	CGameGoodsManager();
	//��������
	virtual ~CGameGoodsManager();

	//������
public:
	//��������
	VOID ResetData();

	//��Ʒ����
public:
	//�Ƴ���Ʒ
	VOID RemoveGameGoods(WORD wGoodsID);
	//�����Ʒ
	WORD AppendGameGoods(tagGoodsInfo GoodsInfo[], WORD wGoodsCount);

	//��̬����
public:
	//������Ʒ
	static bool IsMoneyGoods(WORD wGoodsID);
	//������Ʒ
	static bool ParseFromJson(LPCTSTR pszGoodsItem, tagGoodsItem& GoodsItem);
	//������Ʒ
	static bool ParseFromJson(LPCTSTR pszGoodsJson, CGoodsItemArray & MoneyArray, CGoodsItemArray& GoodsArray);	
	//���л���Ʒ
	static bool SerializeToJson(tagGoodsItem & GoodsItem, TCHAR szBuffer[], WORD wBufferCount);
	//���л���Ʒ
	static bool SerializeToJson(tagGoodsItem GoodsItem[], WORD wItemCount, TCHAR szBuffer[], WORD wBufferCount);
	

	//ģ�庯��
public:	
	//������Ʒ
	template <class TYPE> TYPE SearchGoodsItem(WORD wGoodsID);
	//������Ʒ
	template <class TYPE> TYPE GetGoodsKindHead(WORD wGoodsKind);

	//��������
private:
	//������Ʒ
	CGameGoodsItemPtr CreateGameGoods(WORD wGoodsKind);
};

//////////////////////////////////////////////////////////////////////////////////

//��Ʒָ��	
template <class TYPE> TYPE CGameGoodsItem::GetLastGoodsItemPtr()
{ 
	return (typename TYPE::element_type*)m_LastGoodsItemPtr;
}

//������Ʒ
template <class TYPE> TYPE CGameGoodsItem::GetNextGoodsItemPtr()
{ 
	return (typename TYPE::element_type*)m_NextGoodsItemPtr;
}

//������Ʒ
template <class TYPE> TYPE CGameGoodsManager::SearchGoodsItem(WORD wGoodsID)
{
	//������Ʒ
	CGameGoodsItemPtr GameGoodsItemPtr;
	if (m_MapGoodsItem.Lookup(wGoodsID, GameGoodsItemPtr) == FALSE)
	{
		return NULL;
	}

	return (typename TYPE::element_type*)GameGoodsItemPtr.get();
}

//ö����Ʒ
template <class TYPE> TYPE CGameGoodsManager::GetGoodsKindHead(WORD wGoodsKind)
{
	//������Ʒ
	CGameGoodsItemPtr GameGoodsItemPtr;
	if (m_MapGoodsKind.Lookup(wGoodsKind, GameGoodsItemPtr) == FALSE)
	{
		return NULL;
	}

	return (typename TYPE::element_type*)GameGoodsItemPtr.get();
}

//////////////////////////////////////////////////////////////////////////////////

#define g_GameGoodsManager	(CGameGoodsManager::getInstance())

//////////////////////////////////////////////////////////////////////////////////

#endif