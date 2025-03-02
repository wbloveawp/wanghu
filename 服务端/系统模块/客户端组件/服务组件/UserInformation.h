#ifndef USER_INFORMATION_HEAD_FILE
#define USER_INFORMATION_HEAD_FILE

#pragma once

#include "ClientServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//�ṹ����

//��˵��
typedef CWHArray<tagCompanionInfo *> CCompanionInfoArray;			//�û���ϵ

//////////////////////////////////////////////////////////////////////////

//�û���Ϣ
class CUserInformation : public IUserInformation
{
	//�ӿڱ���
protected:
	IUserCompanionSink *			m_pIUserCompanionSink;				//��ϵ�ص�

	//��ϵ��Ϣ
protected:
	CCompanionInfoArray				m_CompanionInfoActive;				//��������
	CCompanionInfoArray				m_CompanionInfoBuffer;				//�ͷ�����

	//��������
public:
	//���캯��
	CUserInformation();
	//��������
	virtual ~CUserInformation();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//������Ϣ
	virtual bool __cdecl LoadInformation();
	//���ýӿ�
	virtual bool __cdecl SetUserCompanionSink(IUnknownEx * pIUnknownEx);

	//��ϵ�ӿ�
public:
	//ö�ٹ�ϵ
	virtual tagCompanionInfo * __cdecl EmunCompanionInfo(WORD wIndex);
	//Ѱ�ҹ�ϵ
	virtual tagCompanionInfo * __cdecl SearchCompanionInfo(DWORD dwUserID);
	//�����ϵ
	virtual tagCompanionInfo * __cdecl InsertCompanionInfo(IClientUserItem * pIClientUserItem, BYTE cbCompanion);

	//�ڲ�����
private:
	//������ϵ
	tagCompanionInfo * CreateCompanionItem();
	//�ͷŹ�ϵ
	bool DeleteCompanionItem(tagCompanionInfo * pCompanionInfo);
};

//////////////////////////////////////////////////////////////////////////

#endif