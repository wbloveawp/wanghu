#ifndef GLOBAL_USER_MANAGER_HEAD_FILE
#define GLOBAL_USER_MANAGER_HEAD_FILE

#pragma once

#include "ClientServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//�ṹ����

//������Ϣ
struct tagExtraItem
{
	tagDataDescribe					DataDescribe;						//��Ϣͷ
	BYTE							*pBuffer;							//��Ϣ��
};

//��Ϣ����
typedef CWHArray<tagExtraItem *>	CExtraItemPtrArray;				//��Ϣ����

//////////////////////////////////////////////////////////////////////////

//�û���Ϣ
class CGlobalUserItem : public IGlobalUserItem
{
	//��Ԫ����
	friend class CGlobalUserManager;

	//���Ա���
protected:
	tagGlobalUserInfo				m_GlobalUserInfo;					//�û���Ϣ

	//������Ϣ
protected:
	CExtraItemPtrArray				m_ExtraItemPtrArry;					//��Ϣ����
	static CExtraItemPtrArray		m_ExtraItemStorge;					//��Ϣ���

	//��������
protected:
	//���캯��
	CGlobalUserItem();
public:
	//��������
	virtual ~CGlobalUserItem();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//������Ϣ
public:
	//�û���ʶ
	virtual DWORD __cdecl GetUserID() { return m_GlobalUserInfo.dwUserID; }
	//��Ϸ��ʶ
	virtual DWORD __cdecl GetGameID() { return m_GlobalUserInfo.dwGameID; }
	//�û�����
	virtual LPCWSTR __cdecl GetAccounts() { return m_GlobalUserInfo.szNickName; }

	//������Ϣ
public:
	//��������
	virtual VOID * __cdecl GetExtraInfo(WORD &wDataSize,WORD wDataType);

	//���ú���
public:
	//�����Ϣ
	virtual bool __cdecl AddExtraInfo(VOID * pData, WORD wDataSize, WORD wDataType);
	//ɾ����Ϣ
	virtual bool __cdecl RemoveExtraInfo(WORD wDataType);
	//�����û�
	virtual bool __cdecl ResetUserItem();
};

//////////////////////////////////////////////////////////////////////////

//�û�����
typedef CWHArray<CGlobalUserItem *> CGlobalUserItemArray;

//�ص�����
typedef CWHArray<IGlobalUserManagerSink *> CGlobalUserManagerSinkArray;

//////////////////////////////////////////////////////////////////////////

//�û�����
class CGlobalUserManager : public IGlobalUserManager
{
	//��������
protected:
	CGlobalUserItemArray			m_UserItemActive;					//�����
	static CGlobalUserItemArray		m_UserItemStorage;					//�洢����

	//����ӿ�
protected:
	CGlobalUserManagerSinkArray		m_UserManagerSinkArry;				//֪ͨ�ӿ�

	//��������
public:
	//���캯��
	CGlobalUserManager();
	//��������
	virtual ~CGlobalUserManager();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	
	//���ýӿ�
public:
	//���ýӿ�
	virtual bool __cdecl AddUserManagerSink(IUnknownEx * pIUnknownEx);
	//ɾ���ӿ�
	virtual bool __cdecl RemoveUserManagerSink(IUnknownEx *pIUnkownEx);
	//ɾ���ӿ�
	virtual bool __cdecl RemoveAllUserManagerSink();

	//����ӿ�
public:
	//�����û�
	virtual IGlobalUserItem * __cdecl ActiveUserItem(const tagGlobalUserInfo & GlobalUserInfo);
	//ɾ���û�
	virtual bool __cdecl DeleteUserItem(IGlobalUserItem * pIGlobalUserItem);

	//���½ӿ�
public:
	//���»�����Ϣ
	virtual bool __cdecl UpdateUserItem(IGlobalUserItem * pIGlobalUserItem, tagGlobalUserInfo * const pGlobalUserInfo);
	//��Ӹ�����Ϣ
	virtual bool __cdecl AddUserExtraInfo(IGlobalUserItem * pIGlobalUserItem, VOID * pData, WORD wDataSize, WORD wDataType);
	//ɾ��������Ϣ
	virtual bool __cdecl RemoveUserExtraInfo(IGlobalUserItem * pIGlobalUserItem, WORD wDataType);

	//���ҽӿ�
public:
	//ö���û�
	virtual IGlobalUserItem * __cdecl EnumUserItem(WORD wEnumIndex);
	//�����û�
	virtual IGlobalUserItem * __cdecl SearchUserByUserID(DWORD dwUserID);
	//�����û�
	virtual IGlobalUserItem * __cdecl SearchUserByGameID(DWORD dwGameID);
	//�����û�
	virtual IGlobalUserItem * __cdecl SearchUserByAccounts(LPCTSTR pszAccounts);
};

//////////////////////////////////////////////////////////////////////////

#endif