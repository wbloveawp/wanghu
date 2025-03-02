#ifndef MEMBER_PARSER_HEAD_FILE
#define MEMBER_PARSER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "ClientServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//索引说明
typedef CMap<BYTE,BYTE,CString,CString &>	CMapMemberDescribe;			//描述索引
typedef CMap<BYTE,BYTE,CString,CString &>	CMapMasterDescribe;			//描述索引

//会员解释器
class CMemberParser : public IMemberParser
{
	//变量定义
protected:
	CMapMemberDescribe				m_MapMemberDescribe;				//会员描述
	CMapMasterDescribe				m_MapMasterDescribe;				//管理员描述

	//函数定义
public:
	//构造函数
	CMemberParser();
	//析构函数
	virtual ~CMemberParser();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { delete this; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//配置接口
public:
	//解释数目
	virtual BYTE __cdecl GetMemberParserItemCount();
	//解释数目
	virtual BYTE __cdecl GetMasterParserItemCount();
	//设置解释
	virtual bool __cdecl SetMemberParser(tagMemberItem MemberItem[], WORD wItemCount);
	//设置解释
	virtual bool __cdecl SetMasterParser(tagMasterItem MasterItem[], WORD wItemCount);

	//功能接口
public:
	//获取描述
	virtual LPCTSTR __cdecl GetMemberDescribe(BYTE cbMemberOrder);
	//获取描述
	virtual LPCTSTR __cdecl GetMasterDescribe(BYTE cbMasterOrder);
	//获取解释
	virtual BYTE __cdecl GetMemberParser(tagMemberItem MemberItem[], WORD wItemCount);
	//获取解释
	virtual BYTE __cdecl GetMasterParser(tagMasterItem MasterItem[], WORD wItemCount);
};

//////////////////////////////////////////////////////////////////////////

#endif