#ifndef CLUB_USER_MANAGER_HEAD_FILE
#define CLUB_USER_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"

//////////////////////////////////////////////////////////////////////////////////
//��������
class CClubUserItem;
class CClubUserManager;

//////////////////////////////////////////////////////////////////////////////////
// 
//���Ͷ���
typedef CWHStockData<DWORD,CClubUserItem>  CStcokClubUser;

//////////////////////////////////////////////////////////////////////////////////
//�û���Ϣ
class CClubUserItem
{
	//��Ԫ����
	friend class CClubUserManager;	

	//�û���Ϣ
protected:
	tagClubUserInfo					m_UserInfo;							//�û���Ϣ

	//������Ϣ
protected:
	tagSocketInfo					m_SocketInfo;						//������Ϣ

	//��������
protected:
	//���캯��
	CClubUserItem();
	//��������
	virtual ~CClubUserItem();

	//�û�״̬
public:	
	//��ȡ״̬
	virtual BYTE GetUserStatus() { return m_UserInfo.cbUserStatus; }
	//�޸�״̬
	virtual VOID SetUserStatus(BYTE cbUserStatus) { m_UserInfo.cbUserStatus=cbUserStatus; }

	//������Ϣ
public:
	//�û��Ա�
	virtual BYTE GetGender() { return m_UserInfo.cbGender; }
	//�û���ʶ
	virtual DWORD GetUserID() { return m_UserInfo.dwUserID; }
	//��Ϸ��ʶ
	virtual DWORD GetGameID() { return m_UserInfo.dwGameID; }
	//�û��ǳ�
	virtual LPCTSTR GetNickName() { return m_UserInfo.szNickName; }
	//�û���Ϣ
	virtual tagClubUserInfo * GetUserInfo() { return &m_UserInfo; }

	//������Ϣ
public:
	//���Ʊ�ʶ
	virtual DWORD GetTokenID() { return m_SocketInfo.dwTokenID; }
	//���ر�ʶ
	virtual WORD GetGateID() { return m_SocketInfo.wGateID; }	
	//��ȡ��Ϣ
	virtual tagSocketInfo * GetSocketInfo() { return &m_SocketInfo; }
	//������Ϣ
	virtual VOID SetSocketInfo(tagSocketInfo & SocketInfo) { m_SocketInfo=SocketInfo; }

	//��������
protected:
	//��������
	virtual VOID ResetUserItem();
};

//////////////////////////////////////////////////////////////////////////////////

//�û�������
class CClubUserManager
{
	//�û�����
protected:
	CStcokClubUser					m_StockClubUser;					//�û��ֿ�

	//��������
public:
	//���캯��
	CClubUserManager();
	//��������
	virtual ~CClubUserManager();

	//���Һ���
public:
	//�����û�
	CClubUserItem * SearchUserItem(DWORD dwUserID);
	//ö���û�
	CClubUserItem* EnumUserItem(CClubUserItem* pClubUserItem);

	//ͳ�ƺ���
public:
	//��������
	DWORD GetUserItemCount() { return (DWORD)m_StockClubUser.GetItemCount(); }

	//������
public:
	//ɾ���û�
	VOID DeleteUserItem();
	//ɾ���û�
	bool DeleteUserItem(DWORD dwUserID);
	//�����û�
	CClubUserItem * InsertUserItem(tagClubUserInfo & UserInfo, tagSocketInfo &SocketInfo);	
};

//////////////////////////////////////////////////////////////////////////////////
#endif