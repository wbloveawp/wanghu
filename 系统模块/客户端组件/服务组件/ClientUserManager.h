#ifndef CLIENT_USER_MANAGER_HEAD_FILE
#define CLIENT_USER_MANAGER_HEAD_FILE

#pragma once

#include "ClientServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//�û���Ϣ
class CClientUserItem : public IClientUserItem
{
	//��Ԫ����
	friend class CGameUserManager;
	friend class CPlazaUserManager;

	//���Ա���
protected:
	tagClientUserInfo				m_UserInfo;							//�û���Ϣ
	tagCustomFaceInfo				m_CustomFaceInfo;					//�Զ�ͷ��

	//��չ����
protected:
	BYTE							m_cbCompanion;						//�û���ϵ
	WCHAR							m_szUserNote[LEN_NOTE];				//�û���ע

	//��������
public:
	//���캯��
	CClientUserItem();
	//��������
	virtual ~CClientUserItem();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���Խӿ�
public:
	//��Ҫ�ȼ�
	virtual LONG __cdecl GetImportOrder();
	//�û���Ϣ
	virtual tagClientUserInfo* __cdecl GetUserInfo() { return &m_UserInfo; }
	//�Զ�ͷ��
	virtual tagCustomFaceInfo * GetCustomFaceInfo() { return &m_CustomFaceInfo; }

	//ͷ����Ϣ
public:
	//ͷ������
	virtual WORD __cdecl GetFaceID(){ return m_UserInfo.wFaceID; }
	//�Զ�����
	virtual DWORD __cdecl GetCustomID() { return m_UserInfo.dwCustomID; }
		
	//������Ϣ
public:
	//������ʾ
	virtual bool __cdecl IsRobot() { return m_UserInfo.bIsRobot; }
	//�û�����
	virtual BYTE __cdecl GetAge() { return m_UserInfo.cbAge; }
	//�û��Ա�
	virtual BYTE __cdecl GetGender() { return m_UserInfo.cbGender; }
	//�û���ʶ
	virtual DWORD __cdecl GetUserID() { return m_UserInfo.dwUserID; }
	//��Ϸ��ʶ
	virtual DWORD __cdecl GetGameID() { return m_UserInfo.dwGameID; }
	//���ű�ʶ
	virtual DWORD __cdecl GetGroupID() { return 0; }
	//�û�����
	virtual LPCWSTR __cdecl GetAccounts() { return m_UserInfo.szAccounts; }
	//����ǩ��
	virtual LPCWSTR __cdecl GetUnderWrite() { return m_UserInfo.szUnderWrite; }

	//�ȼ���Ϣ
public:
	//��Ա�ȼ�
	virtual BYTE __cdecl GetMemberOrder() { return m_UserInfo.dwMemberPoint; }
	//����ȼ�
	virtual BYTE __cdecl GetMasterOrder() { return m_UserInfo.cbMasterOrder; }

	//������Ϣ
public:
	//������Ϣ
	virtual LPCWSTR __cdecl GetUserArea() { return m_UserInfo.szArea; }
	//������Ϣ
	virtual LPCWSTR __cdecl GetUserCity() { return m_UserInfo.szCity; }
	//ʡ����Ϣ
	virtual LPCWSTR __cdecl GetUserProvince() { return m_UserInfo.szProvince; }

	//�û�״̬
public:
	//�û�����
	virtual WORD __cdecl GetTableID() { return m_UserInfo.wTableID; }
	//�û�����
	virtual WORD __cdecl GetChairID() { return m_UserInfo.wChairID; }
	//�û�״̬
	virtual BYTE __cdecl GetUserStatus() { return m_UserInfo.cbUserStatus; }

	//����ȼ�
public:
	//������ֵ
	virtual LONG __cdecl GetUserExperience();
	//��ȡ�ȼ�
	virtual WORD __cdecl GetExperienceLevel();
	//�ȼ���Ϣ
	virtual WORD __cdecl GetExperienceIndex(BYTE cbOrderIndex[], WORD wMaxCount);

	//������Ϣ
public:
	//������ֵ
	virtual SCORE __cdecl GetUserScore() { return m_UserInfo.lScore; }
	//�����Ϣ
	virtual SCORE __cdecl GetUserGold() { return m_UserInfo.lGold; }
	//������Ϣ
	virtual SCORE __cdecl GetInsureGold() { return m_UserInfo.lInsureGold; }

	//��Ϸ��Ϣ
public:
	//ʤ������
	virtual LONG __cdecl GetUserWinCount() { return m_UserInfo.lWinCount; }
	//ʧ������
	virtual LONG __cdecl GetUserLostCount() { return m_UserInfo.lLostCount; }
	//�;�����
	virtual LONG __cdecl GetUserDrawCount() { return m_UserInfo.lDrawCount; }
	//��������
	virtual LONG __cdecl GetUserFleeCount() { return m_UserInfo.lFleeCount; }
	//��Ϸ����
	virtual LONG __cdecl GetUserPlayCount() { return m_UserInfo.lWinCount+m_UserInfo.lLostCount+m_UserInfo.lDrawCount+m_UserInfo.lFleeCount; }

	//������Ϣ
public:
	//�û�ʤ��
	virtual float __cdecl GetUserWinRate();
	//�û�����
	virtual float __cdecl GetUserLostRate();
	//�û�����
	virtual float __cdecl GetUserDrawRate();
	//�û�����
	virtual float __cdecl GetUserFleeRate();

	//�û���ϵ
public:
	//��ȡ��ϵ
	virtual BYTE __cdecl GetUserCompanion() { return m_cbCompanion; }
	//���ù�ϵ
	virtual VOID __cdecl SetUserCompanion(BYTE cbCompanion) { m_cbCompanion=cbCompanion; }

	//�û���ע
public:
	//��ȡ��ע
	virtual LPCWSTR __cdecl GetUserNoteInfo() { return m_szUserNote; }
	//���ñ�ע
	virtual VOID __cdecl SetUserNoteInfo(LPCWSTR pszUserNote);
};

//////////////////////////////////////////////////////////////////////////

//�û�����
typedef CWHArray<CClientUserItem *> CClientUserItemArray;

//////////////////////////////////////////////////////////////////////////

//�û�����
class CPlazaUserManager : public IPlazaUserManager
{
	//��������
protected:
	CClientUserItemArray			m_UserItemActive;					//�����
	static CClientUserItemArray		m_UserItemStorage;					//�洢����

	//����ӿ�
protected:
	IUserManagerSink *				m_pIUserManagerSink;				//֪ͨ�ӿ�
	IUserInformation *				m_pIUserInformation;				//�û���Ϣ

	//��������
public:
	//���캯��
	CPlazaUserManager();
	//��������
	virtual ~CPlazaUserManager();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ýӿ�
public:
	//���ýӿ�
	virtual bool __cdecl SetUserInformation(IUnknownEx * pIUnknownEx);
	//���ýӿ�
	virtual bool __cdecl SetUserManagerSink(IUnknownEx * pIUnknownEx);

	//����ӿ�
public:
	//�����û�
	virtual IClientUserItem * __cdecl ActiveUserItem(const tagClientUserInfo& UserInfo, tagCustomFaceInfo & CustomFaceInfo);
	//ɾ���û�
	virtual bool __cdecl DeleteUserItem(IClientUserItem * pIClientUserItem);

	//���½ӿ�
public:
	//���»���
	virtual bool __cdecl UpdateUserItemScore(IClientUserItem * pIClientUserItem, tagClientUserScore * const pUserScore);
	//����״̬
	virtual bool __cdecl UpdateUserItemStatus(IClientUserItem * pIClientUserItem, tagUserStatus * const pUserStatus);
	//���¶�λ
	//virtual bool __cdecl UpdateUserItemSegment(IClientUserItem * pIClientUserItem, tagUserSegment * const pUserSegment);
	//����ͷ��
	virtual bool __cdecl UpdateUserCustomFace(IClientUserItem * pIClientUserItem, DWORD dwCustomID, tagCustomFaceInfo & CustomFaceInfo);

	//���ҽӿ�
public:
	//ö���û�
	virtual IClientUserItem * __cdecl EnumUserItem(WORD wEnumIndex);
	//�����û�
	virtual IClientUserItem * __cdecl SearchUserByUserID(DWORD dwUserID);
	//�����û�
	virtual IClientUserItem * __cdecl SearchUserByGameID(DWORD dwGameID);
	//�����û�
	virtual IClientUserItem * __cdecl SearchUserByAccounts(LPCTSTR pszAccounts);
};

//////////////////////////////////////////////////////////////////////////

//�û�����
class CGameUserManager : public IGameUserManager
{
	//�û�����
protected:
	CClientUserItemArray			m_UserItemLookon;					//�Թ�����
	CClientUserItemArray			m_UserItemStorage;					//�洢����

	//��������
protected:
	CClientUserItem *				m_pTableUserItem[MAX_CHAIR];		//��Ϸ�û�

	//����ӿ�
protected:
	IUserManagerSink *				m_pIUserManagerSink;				//֪ͨ�ӿ�

	//��������
public:
	//���캯��
	CGameUserManager();
	//��������
	virtual ~CGameUserManager();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ýӿ�
public:
	//���ýӿ�
	virtual bool __cdecl SetUserManagerSink(IUnknownEx * pIUnknownEx);
	//��ȡ�ӿ�
	virtual VOID * __cdecl GetUserManagerSink(REFGUID Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//�����û�
	virtual IClientUserItem * __cdecl ActiveUserItem(const tagClientUserInfo& UserInfo, tagCustomFaceInfo & CustomFaceInfo, BYTE cbCompanion, LPCWSTR pszUserNote);
	//ɾ���û�
	virtual bool __cdecl DeleteUserItem(IClientUserItem * pIClientUserItem);
	//�����û�
	virtual bool __cdecl DeleteAllUserItem();

	//���½ӿ�
public:
	//���»���
	virtual bool __cdecl UpdateUserItemScore(IClientUserItem * pIClientUserItem, tagClientUserScore* const pUserScore);
	//����״̬
	virtual bool __cdecl UpdateUserItemStatus(IClientUserItem * pIClientUserItem, tagUserStatus * const pUserStatus);
	//���¶�λ
	//virtual bool __cdecl UpdateUserItemSegment(IClientUserItem * pIClientUserItem, tagUserSegment * const pUserSegment);
	//����ͷ��
	virtual bool __cdecl UpdateUserCustomFace(IClientUserItem * pIClientUserItem, DWORD dwCustomID, tagCustomFaceInfo & CustomFaceInfo);

	//��ѯ�ӿ�
public:
	//��Ϸ�û�
	virtual IClientUserItem * __cdecl GetTableUserItem(WORD wChariID);
	//�Թ��û�
	virtual IClientUserItem * __cdecl EnumLookonUserItem(WORD wEnumIndex);

	//���ҽӿ�
public:
	//�����û�
	virtual IClientUserItem * __cdecl SearchUserByUserID(DWORD dwUserID);
	//�����û�
	virtual IClientUserItem * __cdecl SearchUserByGameID(DWORD dwGameID);
	//�����û�
	virtual IClientUserItem * __cdecl SearchUserByAccounts(LPCTSTR pszAccounts);
};

//////////////////////////////////////////////////////////////////////////

#endif