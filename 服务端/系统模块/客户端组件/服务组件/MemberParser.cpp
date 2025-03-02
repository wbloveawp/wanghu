#include "StdAfx.h"
#include "MemberParser.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CMemberParser::CMemberParser()
{
	//��������
	m_MapMemberDescribe.InitHashTable(23);
	m_MapMasterDescribe.InitHashTable(23);

	return;
}

//��������
CMemberParser::~CMemberParser()
{
}

//�ӿڲ�ѯ
VOID * __cdecl CMemberParser::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IMemberParser,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IMemberParser,Guid,dwQueryVer);
	return NULL;
}

//������Ŀ
BYTE __cdecl CMemberParser::GetMemberParserItemCount()
{
	return (BYTE)m_MapMemberDescribe.GetCount();
}

//������Ŀ
BYTE __cdecl CMemberParser::GetMasterParserItemCount()
{
	return (BYTE)m_MapMasterDescribe.GetCount();
}

//���ý���
bool __cdecl CMemberParser::SetMemberParser(tagMemberItem MemberItem[], WORD wItemCount)
{
	//���ý���
	for (WORD i=0;i<wItemCount;i++) 
	{
		CW2CT strMemberName(MemberItem[i].szMemberName);
		m_MapMemberDescribe[MemberItem[i].cbMemberOrder]=strMemberName;
	}

	return true;
}

//���ý���
bool __cdecl CMemberParser::SetMasterParser(tagMasterItem MasterItem[], WORD wItemCount)
{
	//���ý���
	for (WORD i=0;i<wItemCount;i++) 
	{
		CW2CT strMasterName(MasterItem[i].szMasterName);
		m_MapMasterDescribe[MasterItem[i].cbMasterOrder]=strMasterName;
	}

	return true;
}

//��ȡ����
LPCTSTR __cdecl CMemberParser::GetMemberDescribe(BYTE cbMemberOrder)
{
	CMapMemberDescribe::CPair * pPair=m_MapMemberDescribe.PLookup(cbMemberOrder);
	return (pPair!=NULL)?((LPCTSTR)pPair->value):TEXT("��ͨ���");
}

//��ȡ����
LPCTSTR __cdecl CMemberParser::GetMasterDescribe(BYTE cbMasterOrder)
{
	ASSERT(cbMasterOrder>0);
	CMapMasterDescribe::CPair * pPair=m_MapMasterDescribe.PLookup(cbMasterOrder);
	return (pPair!=NULL)?((LPCTSTR)pPair->value):TEXT("��Ϸ����Ա");
}

//��ȡ����
BYTE __cdecl CMemberParser::GetMemberParser(tagMemberItem MemberItem[], WORD wItemCount)
{
	//��������
	BYTE cbCopyCount=0;
	CString strMemberName;
	POSITION Position=m_MapMemberDescribe.GetStartPosition();

	//���ñ���
	while (Position!=NULL)
	{
		//����ж�
		if (cbCopyCount>=wItemCount) break;

		//��ȡ����
		BYTE cbMemberOrder=0;
		m_MapMemberDescribe.GetNextAssoc(Position,cbMemberOrder,strMemberName);

		//���ý���
		BYTE cbIndex=cbCopyCount++;
		CT2CW strMemberName(strMemberName);
		MemberItem[cbIndex].cbMemberOrder=cbMemberOrder;
		lstrcpynW(MemberItem[cbIndex].szMemberName,strMemberName,CountArray(MemberItem[cbIndex].szMemberName));
	}

	return cbCopyCount;
}

//��ȡ����
BYTE __cdecl CMemberParser::GetMasterParser(tagMasterItem MasterItem[], WORD wItemCount)
{
	//��������
	BYTE cbCopyCount=0;
	CString strMasterName;
	POSITION Position=m_MapMasterDescribe.GetStartPosition();

	//���ñ���
	while (Position!=NULL)
	{
		//����ж�
		if (cbCopyCount>=wItemCount) break;

		//��ȡ����
		BYTE cbMasterOrder=0;
		m_MapMasterDescribe.GetNextAssoc(Position,cbMasterOrder,strMasterName);

		//���ý���
		BYTE cbIndex=cbCopyCount++;
		CT2CW wstrMasterName(strMasterName);
		MasterItem[cbIndex].cbMasterOrder=cbMasterOrder;
		lstrcpynW(MasterItem[cbIndex].szMasterName,wstrMasterName,CountArray(MasterItem[cbIndex].szMasterName));
	}

	return cbCopyCount;
}

//////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(MemberParser);

//////////////////////////////////////////////////////////////////////////
