#ifndef GLOBAL_INFO_MANAGER_HEAD_FILE
#define GLOBAL_INFO_MANAGER_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

//��˵��
class CGlobalServiceInfo;
class CGlobalServiceKind;
class CGlobalInfoManager;

//���Ͷ���
typedef CWHArray<CGlobalServiceInfo*> CArrayServiceInfo;
typedef CWHArray<CTCPSocketServiceHelper*> CSocketServiceArray;
typedef CMap<DWORD, DWORD, tagRouteItem, tagRouteItem> CMapRouteItem;

//���Ͷ���
typedef CWHStockData<WORD, CGlobalServiceInfo> CStockServiceInfo;
typedef CWHStockData<WORD, CGlobalServiceKind> CStockServiceKind;
typedef CWHStockData<DWORD, CGlobalServiceInfo> CStockServiceRoute;

//////////////////////////////////////////////////////////////////////////
//·������

//////////////////////////////////////////////////////////////////////////

//��������
class CGlobalServiceInfo : public CGlobalServiceItem
{
	friend class CGlobalServiceKind;
	friend class CGlobalInfoManager;

	//��������
public:
	ITCPSocketService *				m_pITCPSocketService;				//����ӿ�	

	//·�ɹ���
protected:
	CMapRouteItem					m_MapRouteItem;						//·��ӳ��

	//��չ����
protected:	
	WORD							m_wDataSize;						//���ݴ�С
	BYTE							m_cbExtendData[512];				//��չ����

	//��������
protected:
	//���캯��
	CGlobalServiceInfo();
	//��������
	virtual ~CGlobalServiceInfo();

	//��Ϣ����
public:
	//��չ����
	BYTE* GetExtendData() { return m_cbExtendData; }

	//·�ɹ���
public:
	//�Ƴ�·��
	VOID RemoveRouteItem();
	//�Ƴ�·��
	VOID RemoveRouteItem(DWORD dwRouteID);
	//���·��
	VOID AppendRouteItem(tagRouteItem RouteItem[],WORD wItemCount);	

	//��������
public:
	//��������
	virtual VOID ResetData();
};


//////////////////////////////////////////////////////////////////////////
//��������
class CGlobalServiceKind
{
	friend class CGlobalInfoManager;

	//��������
protected:
	WORD							m_wServiceKind;						//��������
	WORD							m_wServiceMapKind;					//ӳ�䷽ʽ
	CWHConHash						m_ServiceHash;						//��ϣӳ��
	CArrayServiceInfo				m_ArrayServiceInfo;					//�����б�	

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
	//�Ƴ�����
	VOID RemoveService(CGlobalServiceInfo* pGlobalServiceInfo);
	//��ӷ���
	VOID AppendService(CGlobalServiceInfo* pGlobalServiceInfo);	

	//��������
public:
	//��������
	VOID ResetData();
	//���ҷ���
	INT SearchService(WORD wServiceID);
};

//////////////////////////////////////////////////////////////////////////

//ȫ����Ϣ
class CGlobalInfoManager
{
	//���ú���
protected:
	IUnknownEx*						m_pIAttemperEngine;					//���Ƚӿ�
	tagServiceAttrib*				m_pServiceAttribute;				//��������

	//��������
protected:	
	CStockServiceInfo				m_StockServiceInfo[SERVICE_COUNT];	//�����ʶ	
	CStockServiceKind				m_StockServiceKind[SERVICE_COUNT];	//ɢ������	

	//�������
protected:
	CSocketServiceArray				m_SocketServiceActive;				//����洢
	CSocketServiceArray				m_SocketServiceBuffer;				//���񻺳�

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
	DWORD GetServiceCount(WORD wServiceIndex);

	//���ú���
public:
	//�ص��ӿ�
	VOID SetIAttemperEngine(IUnknownEx* pIAttemperEngine) { m_pIAttemperEngine = pIAttemperEngine; }	
	//��������
	VOID SetServiceAttribute(tagServiceAttrib* pServiceAttribute) { m_pServiceAttribute = pServiceAttribute; }

	//�������
public:
	//�������
	CGlobalServiceInfo * ActiveServiceInfo(WORD wServiceIndex, DWORD dwServiceID, tagServiceItem & ServiceItem);
	//ɾ������
	bool DeleteServiceItem(WORD wServiceIndex, WORD wServiceID);

	//���Һ���
public:
	//Ѱ�ҷ���
	CGlobalServiceInfo * SearchServiceInfo(WORD wServiceIndex,WORD wServiceID);
	//��������
	CGlobalServiceKind * SearchServiceKind(WORD wServiceIndex, WORD wServiceKind);
	//ӳ�����
	CGlobalServiceInfo * MapServiceInfo(WORD wServiceIndex, WORD wServiceKind, DWORD dwObjectID);

	//ö�ٺ���
public:
	//ö�ٷ���
	CGlobalServiceInfo * EnumServiceInfo(WORD wServiceIndex, CGlobalServiceInfo* pGlobalServiceItem);

	//������Ϣ
public:
	//������Ϣ
	ITCPSocketService * GetSocketService(WORD wServiceIndex,WORD wServiceID);

	//��������
private:
	//�������
	ITCPSocketService* ActiveSocketService(DWORD dwServiceID);
	//�Ƴ�����
	bool RemoveSocketService(ITCPSocketService* pITCPSocketService);
};

//////////////////////////////////////////////////////////////////////////

#endif