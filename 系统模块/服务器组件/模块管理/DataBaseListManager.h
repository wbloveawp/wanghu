#ifndef DATABASE_LIST_MANAGER_HEAD_FILE
#define DATABASE_LIST_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////

//���鶨��
typedef CWHArray<tagDataBaseList *> CDatabaseListArray;

//��������
typedef CMap<WORD,WORD,tagDataBaseList *,tagDataBaseList *> CDatabaseListMap;

//////////////////////////////////////////////////////////////////////////

//���ݿ��б���
class MODULE_MANAGER_CLASS CDatabaseListManager
{
	//��������
public:
	CDatabaseListMap				m_ListMap;					//���ݿ�����
	CDatabaseListArray				m_ListArray;				//���ݿ�����

	//��̬����
protected:
	static bool						m_bLoad;					//���ر�־
	static CDatabaseListManager		*m_spDBListMgr;				//���ݿ��б���
	
	//��������
public:
	//���캯��
	CDatabaseListManager();
	//��������
	virtual ~CDatabaseListManager();

	//������
public:
	//�������ݿ�
	bool Load();
	//��������
	bool Reset();
	//ɾ������
	bool Delete(WORD wDBListID);
	//��������
	bool Insert(tagDataBaseList * pItem);

	//��Ϣ����
public:
	//��ȡ��Ŀ
	INT_PTR GetCount();
	//��������
	tagDataBaseList * SearchItem(WORD wDBListID);
	//��������
	POSITION GetStartPosition();
	//��������
	tagDataBaseList * GetNext( POSITION &pos );

	//�ڲ�����
private:
	//��������
	tagDataBaseList * CreateItem();

	//��̬����
public:
	//��ȡʵ��
	static CDatabaseListManager * GetInstance() { return m_spDBListMgr; }
};

//////////////////////////////////////////////////////////////////////////

#endif