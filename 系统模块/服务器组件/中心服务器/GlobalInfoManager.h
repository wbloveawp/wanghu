#ifndef GLOBAL_USER_MANAGER_HEAD_FILE
#define GLOBAL_USER_MANAGER_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

//��˵��
class CGlobalUserInfo;
class CGlobalLogonInfo;
class CGlobalAgentInfo;
class CGlobalServerInfo;
class CGlobalMonitorInfo;
class CGlobalServiceInfo;
class CGlobalServiceKind;
class CGlobalGameStock;

//���Ͷ���
typedef CWHArray<CGlobalServiceInfo*> CArrayServiceInfo;
typedef CWHStockData<WORD, CGlobalGameStock> CStockGameStock;

//��������
typedef CMap<DWORD, DWORD, tagRouteItem, tagRouteItem> CMapRouteItem;
typedef CMap<DWORD, DWORD, CGlobalUserInfo*, CGlobalUserInfo*> CMapUserID;
typedef CMap<WORD, WORD, CGlobalLogonInfo*, CGlobalLogonInfo*> CMapLogonID;
typedef CMap<WORD, WORD, CGlobalServerInfo*, CGlobalServerInfo*> CMapServerID;
typedef CMap<WORD, WORD, CGlobalServiceInfo*, CGlobalServiceInfo*> CMapServiceID;
typedef CMap<WORD, WORD, CGlobalServiceKind*, CGlobalServiceKind* > CMapServiceKind;

//////////////////////////////////////////////////////////////////////////

//������Ϣ
class CGlobalServiceInfo : public CGlobalServiceItem
{
	friend class CGlobalServiceKind;
	friend class CGlobalInfoManager;

	//���ӱ���
protected:
	CGlobalServiceInfo *			m_pNextServiceInfoPtr;			//������Ϣ

	//·�ɹ���
protected:
	CMapRouteItem					m_MapRouteItem;					//·��ӳ��

	//��������
public:
	//���캯��
	CGlobalServiceInfo();
	//��������
	virtual ~CGlobalServiceInfo();

	//·�ɹ���
public:
	//�Ƴ�·��
	VOID RemoveRouteItem(DWORD dwRouteID);
	//���·��
	VOID AppendRouteItem(tagRouteItem RouteItem[], WORD wItemCount);	

	//���Һ���
public:
	//����·��
	tagRouteItem* QueryRouteItem(DWORD dwRouteID);
	//ö��·��
	tagRouteItem* EnumRouteItem(POSITION &Position);

	//��������
public:
	//���ú���
	virtual VOID ResetData();
};


//////////////////////////////////////////////////////////////////////////
//��������
class CGlobalServiceKind
{
	//��Ԫ����
	friend class CGlobalInfoManager;

	//��������
public:
	WORD							m_wServiceKind;						//��������
	WORD							m_wServiceMapKind;					//ӳ�䷽ʽ
	CWHConHash						m_ServiceHash;						//����ӳ��
	CArrayServiceInfo				m_ArrayServiceInfo;					//��������

	//��������
protected:
	CGlobalServiceKind*				m_pNextServiceKindPtr;				//����ָ��

	//��������
protected:
	//���캯��
	CGlobalServiceKind();
	//��������
	virtual ~CGlobalServiceKind();

	//��Ϣ����
public:
	//��������
	WORD GetServiceKind() { return m_wServiceKind; }
	//��������
	int GetServiceCount() { return m_ServiceHash.GetNodeCount(); }

	//�������
public:
	//���ҷ���
	WORD LookupService(DWORD dwObjectID);
	//�������
	WORD AssignService(DWORD dwObjectID,WORD wServiceID=INVALID_WORD);
	//�Ƴ�����
	VOID RemoveService(CGlobalServiceInfo* pGlobalServiceInfo);
	//��ӷ���
	VOID AppendService(CGlobalServiceInfo* pGlobalServiceInfo);	

	//��������
protected:
	//��������
	VOID ResetData();
	//���ҷ���
	INT SearchService(WORD wServiceID);
};

//////////////////////////////////////////////////////////////////////////

//�㳡����
class CGlobalLogonInfo : public CGlobalServiceInfo
{
	friend class CGlobalInfoManager;

	//��������
public:
	CMapUserID						m_MapUserID;				//�û��ֿ�

	//��������
protected:
	//���캯��
	CGlobalLogonInfo();
	//��������
	virtual ~CGlobalLogonInfo();

	//��������
public:
	//���ú���
	virtual VOID ResetData();
};


//////////////////////////////////////////////////////////////////////////

//��Ϸ�ʳ�
class CGlobalGameStock
{
	friend class CGlobalMonitorInfo;

	//��������
public:
	WORD						m_wKindID;						//���ͱ�ʶ
	SCORE						m_lJackpotScore;				//�ʳزʽ�

	//��������
protected:
	//���캯��
	CGlobalGameStock();
	//��������
	virtual ~CGlobalGameStock();

	//��������
public:
	//���ú���
	virtual VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////

//�������
class CGlobalMonitorInfo : public CGlobalServiceInfo
{
	friend class CGlobalInfoManager;

	//�洢����
public:
	CStockGameStock					m_StockGameStock;				//���ֿ�

	//��������
protected:
	//���캯��
	CGlobalMonitorInfo();
	//��������
	virtual ~CGlobalMonitorInfo();

	//��������
public:
	//���ú���
	virtual VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////

//�û���Ϣ
class CGlobalUserInfo : public CGlobalUserItem
{
	friend class CGlobalInfoManager;

	//������Ϣ
public:
	CGlobalLogonInfo *				m_pGlobalLogonInfo;					//��¼����
	CGlobalServerInfo *				m_pGlobalServerInfo;				//��������	
	CGlobalServiceInfo *			m_pGlobalMessageInfo;				//��Ϣ����

	//��������
protected:
	CGlobalUserInfo*				m_pNextUserInfoPtr;					//����ָ��

	//��������
protected:
	//���캯��
	CGlobalUserInfo();
	//��������
	virtual ~CGlobalUserInfo();

	//��Ϣ����
public:
	//��������
	WORD BindServiceCount();

	//������Ϣ
public:	
	//��¼����
	CGlobalLogonInfo* GetBindLogonInfo() { return m_pGlobalLogonInfo; }
	//�������
	CGlobalServerInfo* GetBindServerInfo() { return m_pGlobalServerInfo; }
	//��Ϣ����
	CGlobalServiceInfo* GetBindMessageInfo() { return m_pGlobalMessageInfo; }

	//��¼��
public:
	//�󶨵�¼
	VOID BindLogonInfo(CGlobalLogonInfo* pGlobalLogonInfo);
	//�󶨵�¼
	VOID UnBindLogonInfo(CGlobalLogonInfo* pGlobalLogonInfo);	

	//�����
public:
	//�󶨷���
	VOID BindServerInfo(CGlobalServerInfo* pGlobalServerInfo);
	//��󷿼�
	VOID UnBindServerInfo(CGlobalServerInfo* pGlobalServerInfo);

	//��Ϣ��
public:
	//����Ϣ
	VOID BindMessageInfo(CGlobalServiceInfo* pGlobalMessageInfo);
	//�����Ϣ
	VOID UnBindMessageInfo(CGlobalServiceInfo* pGlobalMessageInfo);

	//��������
public:
	//��������
	virtual VOID ResetData();
};
//////////////////////////////////////////////////////////////////////////

//��������
class CGlobalServerInfo : public CGlobalServiceInfo
{
	friend class CGlobalInfoManager;

	//��������
public:
	tagGameServer					m_GameServer;						//��Ϸ����

	//��������
public:
	CMapUserID						m_MapUserID;						//�û��ֿ�

	//��������
protected:
	//���캯��
	CGlobalServerInfo();
	//��������
	virtual ~CGlobalServerInfo();

	//���ܺ���
public:
	//�û���Ŀ
	DWORD GetUserItemCount() { return (DWORD)m_MapUserID.GetCount(); }

	//���Һ���
public:
	//Ѱ���û�
	CGlobalUserInfo * SearchUserInfo(DWORD dwUserID);

	//��������
protected:
	//��������
	virtual VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////

//���Ͷ���
typedef CWHArray<tagPortPair> CPortPairArray;

//������Ŀ
class CGlobalAgentInfo : public CGlobalServiceInfo
{
	friend class CGlobalInfoManager;

	//��������
public:
	tagAgentItem					m_AgentItem;						//��������
	CPortPairArray					m_PortPairArray;					//�˿�����

	//��������
protected:
	CGlobalAgentInfo*				m_pNextAgentItemPtr;				//����ָ��

	//��������
protected:
	//���캯��
	CGlobalAgentInfo();
	//��������
	virtual ~CGlobalAgentInfo();

	//���ܺ���
public:
	//�˿�����
	WORD GetServicePortCount() { return (WORD)m_PortPairArray.GetCount(); }

	//���ܺ���
public:
	//ɾ���˿�
	bool RemoveServicePort(WORD wServicePort);
	//��������
	bool InsertServicePort(tagPortPair * pPortPair);

	//��������
public:
	//��������
	VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////

//ȫ����Ϣ
class CGlobalInfoManager
{
	//��������
protected:
	CMapUserID						m_MapUserID;						//�û���ʶ
	CMapServiceID					m_MapServiceID[SERVICE_COUNT];		//�����ʶ
	CMapServiceKind					m_MapServiceKind[SERVICE_COUNT];	//��������

	//�洢����
protected:
	CGlobalUserInfo*				m_pGlobalUserInfo;					//�û��洢
	CGlobalServiceKind*				m_pGlobalServiceKind;				//���ʹ洢
	CGlobalServiceInfo*				m_pGlobalServiceInfo[SERVICE_COUNT];//����洢

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
	//������Ŀ
	DWORD GetServiceItemCount(WORD wServiceModule);	
	//�û���Ŀ
	DWORD GetUserItemCount() { return (DWORD)m_MapUserID.GetCount(); }	

	//�û�����
public:
	//�����û�
	CGlobalUserInfo* ActiveUserInfo(tagGlobalUserInfo & GlobalUserInfo);
	//ɾ���û�
	bool DeleteUserInfo(DWORD dwUserID);

	//�������
public:
	//�������
	bool ActiveServiceInfo(DWORD dwSocketID, tagServiceItem & ServiceItem);
	//ɾ������
	bool DeleteServiceInfo(WORD wServiceModule, WORD wServiceID);	

	//�������
public:
	//�������
	bool ActiveAgentInfo(DWORD dwSocketID, tagServiceItem & ServiceItem, tagAgentItem & AgentItem);
	//ɾ������
	bool DeleteAgentInfo(WORD wAgentID);
	
	//�������
public:
	//�����
	bool ActiveServerInfo(DWORD dwSocketID, tagServiceItem & ServiceItem, tagGameServer & GameServer);
	//ɾ������
	bool DeleteServerInfo(WORD wServerID);	

	//��¼����
public:
	//ɾ����¼
	bool DeleteLogonInfo(WORD wServiceID);

	//���Һ���
public:
	//Ѱ���û�
	CGlobalUserInfo * SearchUserInfo(DWORD dwUserID);
	//Ѱ�ҷ���
	CGlobalServiceInfo* SearchServiceInfo(WORD wServiceModule, WORD wServiceID);

	//ö�ٺ���
public:
	//ö���û�
	CGlobalUserInfo * EnumUserInfo(POSITION& Postion);
	//ö�ٷ���
	CGlobalServiceInfo* EnumServiceInfo(WORD wServiceModule, POSITION& Position);

	//ӳ�亯��
public:
	//���ҷ���
	CGlobalServiceInfo* LookupService(WORD wServiceModule, WORD wServiceKind, DWORD dwObjectID);
	//�������
	CGlobalServiceInfo* AssignService(WORD wServiceModule, WORD wServiceKind, DWORD dwObjectID, WORD wServiceID=INVALID_WORD);

	//��������
private:
	//�����û�
	CGlobalUserInfo* CreateGlobalUserInfo();
	//��������
	CGlobalServiceKind* CreateGlobalServiceKind();
	//��������
	CGlobalServiceInfo* CreateGlobalServiceInfo(WORD wServiceModule);

	//�ͷź���
private:
	//�ͷ��û�
	bool FreeGlobalUserInfo(CGlobalUserInfo* pGlobalUserInfo);
	//�ͷ�����
	bool FreeGlobalServiceKind(CGlobalServiceKind* pGlobalServiceKind);
	//�ͷŷ���
	bool FreeGlobalServiceInfo(CGlobalServiceInfo* pGlobalServiceInfo);
};

//////////////////////////////////////////////////////////////////////////

#endif