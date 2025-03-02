#ifndef GLOBAL_INFO_ITEM_HEAD_FILE
#define GLOBAL_INFO_ITEM_HEAD_FILE

#pragma once

#include "ServiceFrameHead.h"

//////////////////////////////////////////////////////////////////////////
// 
//��˵��
class CGlobalUserItem;
class CGlobalGateItem;
class CGlobalServerItem;
class CGlobalServiceItem;

//////////////////////////////////////////////////////////////////////////

//������Ϣ
struct tagUserSocketInfo
{
	DWORD							dwUserID;							//�û���ʶ	
	WORD							wGateID;							//���ر�ʶ
	DWORD							dwTokenID;							//���Ʊ�ʶ
};


//���Ʋ���
struct tagTokenParameter
{
	//��������
	DWORD							dwTokenID;							//���Ʊ�ʶ
	DWORD							dwClientIP;							//���ӵ�ַ	
	DWORD							dwActiveTime;						//����ʱ��	

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ	
	LPVOID							pBindUserItem;						//������

	//��չ����
	BYTE							cbExtendData[128];					//��չ����
};

//////////////////////////////////////////////////////////////////////////

//���Ͷ���
typedef CWHStockData<DWORD, tagTokenParameter> CStockTokenID;
typedef CWHStockData<WORD, CGlobalGateItem> CStockGateItem;
typedef CWHStockData<DWORD, CGlobalUserItem> CStockUserItem;
typedef CWHStockData<WORD, CGlobalServerItem> CStockServerItem;
typedef CWHStockData<WORD, CGlobalServiceItem> CStockServiceItem;

//////////////////////////////////////////////////////////////////////////

//�û�����
class SERVICE_FRAME_CLASS CGlobalUserItem
{
	//��������
public:	
	WORD							m_wGateID;							//���ر�ʶ
	DWORD							m_dwUserID;							//�û���ʶ		
	DWORD							m_dwTokenID;						//���Ʊ�ʶ	

	//�û�״̬
public:
	BYTE							m_cbIMUserStatus;					//�û�״̬
	BYTE							m_cbCBUserStatus;					//�û�״̬

	//�û�����
public:
	BYTE							m_cbGender;							//�û��Ա�
	DWORD							m_dwGameID;							//��Ϸ��ʶ
	TCHAR							m_szNickName[LEN_NICKNAME];			//��¼�ǳ�
	TCHAR							m_szUserHeadUrl[256];			//����ǳ�	kk jia

	//ͷ����Ϣ
public:
	WORD							m_wFaceID;							//ͷ���ʶ
	DWORD							m_dwCustomID;						//ͷ���ʶ

	//״̬��Ϣ
public:
	BYTE							m_cbShutUpStatus;					//����״̬
	DWORD							m_dwShutUpOverTime;					//���ʱ��

	//��������
protected:
	//���캯��
	CGlobalUserItem();
	//��������
	virtual ~CGlobalUserItem();

	//������Ϣ
public:
	//���ر�ʶ
	WORD GetGateID() { return m_wGateID; }
	//���Ʊ�ʶ
	DWORD GetTokenID() { return m_dwTokenID; }	

	//�û�״̬
public:
	//�û�״̬
	BYTE GetIMUserStatus() { return m_cbIMUserStatus; }
	//�û�״̬
	BYTE GetCBUserStatus() { return m_cbCBUserStatus; }

	//�û���Ϣ
public:
	//�û���ʶ
	DWORD GetUserID() { return m_dwUserID; }
	//��Ϸ��ʶ
	DWORD GetGameID() { return m_dwGameID; }
	//����Ա�
	BYTE GetGender() { return m_cbGender; }
	//�û��ʺ�
	LPCTSTR GetNickName() { return m_szNickName; }

	//ͷ����Ϣ
public:
	//ϵͳͷ��
	WORD GetFaceID() { return m_wFaceID; }
	//ͷ���ʶ
	DWORD GetCustomID() { return m_dwCustomID; }

	//��������
public:
	//��������
	virtual VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////
// 
//��������
class SERVICE_FRAME_CLASS CGlobalServiceItem
{
	//������Ϣ
public:
	DWORD							m_dwSocketID;						//�����ʶ
	tagServiceItem					m_ServiceItem;						//��������

	//״̬����
public:
	WORD							m_wOnlineCount;						//��������

	//��������
protected:
	//���캯��
	CGlobalServiceItem();
	//��������
	virtual ~CGlobalServiceItem();

	//���ܺ���
public:
	//������
	DWORD GetSocketID() { return m_dwSocketID; }
	//�����ʶ
	WORD GetServiceID() { return m_ServiceItem.wServiceID; }
	//��������
	WORD GetServiceModule() { return m_ServiceItem.wServiceModule; }

	//��������
public:
	//��������
	virtual VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////

//��������
class SERVICE_FRAME_CLASS CGlobalGateItem : public CGlobalServiceItem
{
	//��������
public:
	CDWordArray						m_TokenIDArray;						//��������	
	CStockTokenID					m_StockTokenID;						//���Ʋ���

	//��������
protected:
	//���캯��
	CGlobalGateItem();
	//��������
	virtual ~CGlobalGateItem();

	//���ܺ���
public:
	//��������
	WORD GetTokenCount() { return (WORD)m_StockTokenID.GetItemCount(); }

	//������
public:
	//�Ƴ�����
	VOID RemoveTokenID();
	//�Ƴ�����
	VOID RemoveTokenID(DWORD dwTokenID);
	//��������
	tagTokenParameter* ActiveTokenID(DWORD dwTokenID);

	//���Һ���
public:
	//��ѯ����
	tagTokenParameter* SearchTokenID(DWORD dwTokenID);

	//ö�ٺ���
public:
	//��ѯ����
	tagTokenParameter* EnumTokenID(tagTokenParameter* pTokenParameter);

	//���ܺ���
public:
	//��������
	VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////

//��������
class SERVICE_FRAME_CLASS CGlobalServerItem
{
	//��������
public:
	DWORD							m_dwSocketID;						//�����ʶ
	tagGameServer					m_GameServer;						//��Ϸ����

	//��������
protected:
	//���캯��
	CGlobalServerItem();
	//��������
	virtual ~CGlobalServerItem();

	//��Ϣ����
public:
	//�����ʶ
	DWORD GetSocketID() { return m_dwSocketID; }
	//�����ʶ
	WORD GetServerID() { return m_GameServer.wServerID; }	

	//��������
public:
	//��������
	virtual VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////

#endif