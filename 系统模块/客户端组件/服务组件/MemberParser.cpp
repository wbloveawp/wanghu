#include "StdAfx.h"
#include "MemberParser.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CMemberParser::CMemberParser()
{
	//设置质数
	m_MapMemberDescribe.InitHashTable(23);
	m_MapMasterDescribe.InitHashTable(23);

	return;
}

//析构函数
CMemberParser::~CMemberParser()
{
}

//接口查询
VOID * __cdecl CMemberParser::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IMemberParser,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IMemberParser,Guid,dwQueryVer);
	return NULL;
}

//解释数目
BYTE __cdecl CMemberParser::GetMemberParserItemCount()
{
	return (BYTE)m_MapMemberDescribe.GetCount();
}

//解释数目
BYTE __cdecl CMemberParser::GetMasterParserItemCount()
{
	return (BYTE)m_MapMasterDescribe.GetCount();
}

//设置解释
bool __cdecl CMemberParser::SetMemberParser(tagMemberItem MemberItem[], WORD wItemCount)
{
	//设置解释
	for (WORD i=0;i<wItemCount;i++) 
	{
		CW2CT strMemberName(MemberItem[i].szMemberName);
		m_MapMemberDescribe[MemberItem[i].cbMemberOrder]=strMemberName;
	}

	return true;
}

//设置解释
bool __cdecl CMemberParser::SetMasterParser(tagMasterItem MasterItem[], WORD wItemCount)
{
	//设置解释
	for (WORD i=0;i<wItemCount;i++) 
	{
		CW2CT strMasterName(MasterItem[i].szMasterName);
		m_MapMasterDescribe[MasterItem[i].cbMasterOrder]=strMasterName;
	}

	return true;
}

//获取描述
LPCTSTR __cdecl CMemberParser::GetMemberDescribe(BYTE cbMemberOrder)
{
	CMapMemberDescribe::CPair * pPair=m_MapMemberDescribe.PLookup(cbMemberOrder);
	return (pPair!=NULL)?((LPCTSTR)pPair->value):TEXT("普通玩家");
}

//获取描述
LPCTSTR __cdecl CMemberParser::GetMasterDescribe(BYTE cbMasterOrder)
{
	ASSERT(cbMasterOrder>0);
	CMapMasterDescribe::CPair * pPair=m_MapMasterDescribe.PLookup(cbMasterOrder);
	return (pPair!=NULL)?((LPCTSTR)pPair->value):TEXT("游戏管理员");
}

//获取解释
BYTE __cdecl CMemberParser::GetMemberParser(tagMemberItem MemberItem[], WORD wItemCount)
{
	//变量定义
	BYTE cbCopyCount=0;
	CString strMemberName;
	POSITION Position=m_MapMemberDescribe.GetStartPosition();

	//设置变量
	while (Position!=NULL)
	{
		//完成判断
		if (cbCopyCount>=wItemCount) break;

		//获取子项
		BYTE cbMemberOrder=0;
		m_MapMemberDescribe.GetNextAssoc(Position,cbMemberOrder,strMemberName);

		//设置解释
		BYTE cbIndex=cbCopyCount++;
		CT2CW strMemberName(strMemberName);
		MemberItem[cbIndex].cbMemberOrder=cbMemberOrder;
		lstrcpynW(MemberItem[cbIndex].szMemberName,strMemberName,CountArray(MemberItem[cbIndex].szMemberName));
	}

	return cbCopyCount;
}

//获取解释
BYTE __cdecl CMemberParser::GetMasterParser(tagMasterItem MasterItem[], WORD wItemCount)
{
	//变量定义
	BYTE cbCopyCount=0;
	CString strMasterName;
	POSITION Position=m_MapMasterDescribe.GetStartPosition();

	//设置变量
	while (Position!=NULL)
	{
		//完成判断
		if (cbCopyCount>=wItemCount) break;

		//获取子项
		BYTE cbMasterOrder=0;
		m_MapMasterDescribe.GetNextAssoc(Position,cbMasterOrder,strMasterName);

		//设置解释
		BYTE cbIndex=cbCopyCount++;
		CT2CW wstrMasterName(strMasterName);
		MasterItem[cbIndex].cbMasterOrder=cbMasterOrder;
		lstrcpynW(MasterItem[cbIndex].szMasterName,wstrMasterName,CountArray(MasterItem[cbIndex].szMasterName));
	}

	return cbCopyCount;
}

//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(MemberParser);

//////////////////////////////////////////////////////////////////////////
