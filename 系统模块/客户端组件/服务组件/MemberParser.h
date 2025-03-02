#ifndef MEMBER_PARSER_HEAD_FILE
#define MEMBER_PARSER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "ClientServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//����˵��
typedef CMap<BYTE,BYTE,CString,CString &>	CMapMemberDescribe;			//��������
typedef CMap<BYTE,BYTE,CString,CString &>	CMapMasterDescribe;			//��������

//��Ա������
class CMemberParser : public IMemberParser
{
	//��������
protected:
	CMapMemberDescribe				m_MapMemberDescribe;				//��Ա����
	CMapMasterDescribe				m_MapMasterDescribe;				//����Ա����

	//��������
public:
	//���캯��
	CMemberParser();
	//��������
	virtual ~CMemberParser();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ýӿ�
public:
	//������Ŀ
	virtual BYTE __cdecl GetMemberParserItemCount();
	//������Ŀ
	virtual BYTE __cdecl GetMasterParserItemCount();
	//���ý���
	virtual bool __cdecl SetMemberParser(tagMemberItem MemberItem[], WORD wItemCount);
	//���ý���
	virtual bool __cdecl SetMasterParser(tagMasterItem MasterItem[], WORD wItemCount);

	//���ܽӿ�
public:
	//��ȡ����
	virtual LPCTSTR __cdecl GetMemberDescribe(BYTE cbMemberOrder);
	//��ȡ����
	virtual LPCTSTR __cdecl GetMasterDescribe(BYTE cbMasterOrder);
	//��ȡ����
	virtual BYTE __cdecl GetMemberParser(tagMemberItem MemberItem[], WORD wItemCount);
	//��ȡ����
	virtual BYTE __cdecl GetMasterParser(tagMasterItem MasterItem[], WORD wItemCount);
};

//////////////////////////////////////////////////////////////////////////

#endif