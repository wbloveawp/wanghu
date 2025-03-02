#ifndef GLOBAL_INFO_MANAGER_HEAD_FILE
#define GLOBAL_INFO_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"

/////////////////////////////////////////////////////////////////////////

//�ӿ�����
interface IServerUserItem;

//���Ʋ���
struct tagTokenParameter
{
	//��������
	DWORD							dwTokenID;							//���Ʊ�ʶ
	DWORD							dwClientIP;							//���ӵ�ַ	
	DWORD							dwActiveTime;						//����ʱ��

	//�汾��Ϣ
	BYTE							cbClientKind;						//�ն�����
	DWORD							dwPlazaVersion;						//�㳡�汾
	DWORD							dwFrameVersion;						//��ܰ汾
	DWORD							dwProcessVersion;					//��Ϸ�汾
	DWORD							dwPlazaStationID;					//�㳡վ��

	//�󶨲���
	DWORD							dwUserID;							//�û���ʶ
	IServerUserItem	*				pIServerUserItem;					//�û�ָ��
};

//////////////////////////////////////////////////////////////////////////

//��˵��
class CGlobalServiceItem;

//���鶨��
typedef CWHArray<DWORD>	CTokenIDArray;						
typedef CWHArray<tagTokenParameter*> CTokenParameterArray;
typedef CWHArray<CGlobalServiceItem *> CGlobalServiceItemArray;

//��������
typedef CMap<WORD,WORD,CGlobalServiceItem *,CGlobalServiceItem *> CMapServiceID;
typedef CMap<DWORD, DWORD, IServerUserItem *, IServerUserItem *> CMapServerUserItem;
typedef CMap<DWORD, DWORD, tagTokenParameter *, tagTokenParameter *> CMapTokenParamter;

//////////////////////////////////////////////////////////////////////////

//��������
class CGlobalServiceItem
{
	//��Ԫ����
	friend class CGlobalInfoManager;

	//��������
public:
	WORD							m_wBindIndex;						//������
	tagServiceItem					m_ServiceItem;						//������Ϣ
	CTokenIDArray					m_TokenIDArray;						//��������

	//�洢����
protected:
	CMapTokenParamter				m_MapTokenParamter;					//���Ʋ���	

	//��������
protected:
	CGlobalServiceItem *			m_pNextServiceItemPtr;				//����ָ��

	//��̬����
protected:
	static CTokenParameterArray		m_TokenParameterBuffer;				//���Ʋ���	

	//��������
protected:
	//���캯��
	CGlobalServiceItem();
	//��������
	virtual ~CGlobalServiceItem();

	//���ܺ���
public:
	//������
	WORD GetBindIndex() { return m_wBindIndex; }
	//�����ʶ
	WORD GetServiceID() { return m_ServiceItem.wServiceID; }	
	//�������
	WORD GetMaxConnect() { return m_ServiceItem.wMaxConnect; }
	//��������
	WORD GetTokenCount() { return m_MapTokenParamter.GetCount(); }

	//���ƹ���
public:	
	//ɾ������
	VOID DeleteToken(DWORD dwTokenID);
	//���Ҳ���
	tagTokenParameter* SearchToken(DWORD dwTokenID);
	//�������
	tagTokenParameter * AppendToken(DWORD dwTokenID);	
	//ö������
	tagTokenParameter * EnumToken(POSITION & Position);

	//������
protected:
	//��������
	tagTokenParameter* ActiveTokenParameter();
	//�Ƴ�����
	VOID RemoveTokenParameter(tagTokenParameter* pTokenParameter);

	//��������
protected:
	//��������
	VOID ResetData();	
};

//////////////////////////////////////////////////////////////////////////

//ȫ����Ϣ
class CGlobalInfoManager
{
	//��������
protected:
	CMapServiceID					m_MapServiceID[2];					//�����ʶ

	//�洢����
protected:
	CGlobalServiceItem *			m_pGlobalServiceItem;				//���ش洢

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

	//���ع���
public:
	//ɾ������
	bool DeleteServiceItem(WORD wServiceModule,WORD wAServiceID);
	//��������
	CGlobalServiceItem * ActiveServiceItem(WORD wBindIndex, tagServiceItem & ServiceItem);	

	//���Һ���
public:
	//Ѱ������
	CGlobalServiceItem * SearchServiceItem(WORD wServiceModule, WORD wServiceID);

	//ö�ٺ���
public:
	//ö������
	CGlobalServiceItem * EnumServiceItem(WORD wServiceModule,POSITION & Position);

	//��������
private:
	//��������
	CGlobalServiceItem * CreateGlobalServiceItem();

	//�ͷź���
private:
	//�ͷ�����
	bool FreeGlobalServiceItem(CGlobalServiceItem * pGlobalServiceItem);

	//��������
protected:
	//��������
	WORD GetServiceIndex(WORD wServiceModule);
};

//////////////////////////////////////////////////////////////////////////

#endif